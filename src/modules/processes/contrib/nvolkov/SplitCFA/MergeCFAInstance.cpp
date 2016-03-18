//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 01.00.05.0104
// ----------------------------------------------------------------------------
// MergeCFAInstance.cpp - Released 2016/02/21 20:22:43 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2015 Nikolay Volkov
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
//
// Redistribution and use in both source and binary forms, with or without
// modification, is permitted provided that the following conditions are met:
//
// 1. All redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. All redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the names "PixInsight" and "Pleiades Astrophoto", nor the names
//    of their contributors, may be used to endorse or promote products derived
//    from this software without specific prior written permission. For written
//    permission, please contact info@pixinsight.com.
//
// 4. All products derived from this software, in any form whatsoever, must
//    reproduce the following acknowledgment in the end-user documentation
//    and/or other materials provided with the product:
//
//    "This product is based on software from the PixInsight project, developed
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
//
//    Alternatively, if that is where third-party acknowledgments normally
//    appear, this acknowledgment must be reproduced in the product itself.
//
// THIS SOFTWARE IS PROVIDED BY PLEIADES ASTROPHOTO AND ITS CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PLEIADES ASTROPHOTO OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, BUSINESS
// INTERRUPTION; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; AND LOSS OF USE,
// DATA OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// ----------------------------------------------------------------------------

#include "MergeCFAInstance.h"
#include "MergeCFAParameters.h"
#include <pcl/AutoViewLock.h>
#include <pcl/View.h>
//#include <pcl/StdStatus.h>
#include <pcl/Console.h>

namespace pcl {

// ----------------------------------------------------------------------------

MergeCFAInstance::MergeCFAInstance(const MetaProcess* m) :
ProcessImplementation(m),
p_viewId()
{
   for (int i = 0; i<4; ++i)
      p_viewId.Add(String());
}

MergeCFAInstance::MergeCFAInstance(const MergeCFAInstance& x) :
ProcessImplementation(x)
{
   Assign(x);
}

void MergeCFAInstance::Assign(const ProcessImplementation& p)
{
   const MergeCFAInstance* x = dynamic_cast<const MergeCFAInstance*> (&p);
   if (x != 0)
   {
      p_viewId = x->p_viewId;
   }
}

bool MergeCFAInstance::CanExecuteOn(const View&, pcl::String& whyNot) const
{
   whyNot = "Merge CFA can only be executed in the global context.";
   return false;
}

template <class P>
static void __Combine( GenericImage<P>& t, GenericImage<P>& s0, GenericImage<P>& s1, GenericImage<P>& s2, GenericImage<P>& s3)
{
   for ( int c = 0; c < t.NumberOfChannels(); ++c )
   {
      for ( int sY = 0, tY = 0; tY < t.Height(); tY+=2, ++sY )
      {
         for ( int sX = 0, tX = 0; tX < t.Width(); tX+=2, ++sX )
         {
            t( tX,   tY,   c ) = s0( sX, sY, c);
            t( tX,   tY+1, c ) = s1( sX, sY, c);
            t( tX+1, tY,   c ) = s2( sX, sY, c);
            t( tX+1, tY+1, c ) = s3( sX, sY, c);
         }
      }
   }
}

bool MergeCFAInstance::CanExecuteGlobal(String& whyNot) const
{
   whyNot.Clear();
   return true;
}
View MergeCFAInstance::GetView(int n)
{
   const String id = p_viewId[n];
   if ( id.IsEmpty() )
      throw Error( "MergeCFA: Source image #" +String(n)+ " not set.");

   ImageWindow w = ImageWindow::WindowById( id );

   if ( w.IsNull() )
      throw Error( "MergeCFA: Source image not found: " + id );

   ImageVariant i = w.MainView().Image();

   if ( n == 0 )
   {
      m_width = i.Width();
      m_height = i.Height();
      m_isColor = i.IsColor();
      m_isFloatSample = i.IsFloatSample();
      m_bitsPerSample = i.BitsPerSample();
      m_numberOfChannels = i.NumberOfChannels();
   }
   else
   {
      const String str = "MergeCFA: Incompatible source image ";
      if ( i.Width() != m_width || i.Height() != m_height )
         throw Error( str + "dimensions: " + id );
      if ( i.BitsPerSample() != m_bitsPerSample )
         throw Error( str + "bitsPerSample: " + id );
      if ( i.IsFloatSample() != m_isFloatSample )
         throw Error( str + "FloatSample: " + id );
      if ( i.IsColor() != m_isColor  )
         throw Error( str + "Color spaces: " + id );
      if ( i.NumberOfChannels() != m_numberOfChannels  )
         throw Error( str + "number of channels: " + id );
   }
   return w.MainView();
}

bool MergeCFAInstance::ExecuteGlobal()
{
   View s0View = GetView(0);
   View s1View = GetView(1);
   View s2View = GetView(2);
   View s3View = GetView(3);

   ImageWindow w( m_width*2, m_height*2, m_numberOfChannels, m_bitsPerSample, m_isFloatSample, m_isColor, true );

   if ( w.IsNull() )
      throw Error( "MergeCFA: Unable to create target image." );

   View mainView = w.MainView();

   try
   {
      mainView.Lock();
      s0View.Lock();
      s1View.Lock();
      s2View.Lock();
      s3View.Lock();

      Console().EnableAbort();

      ImageVariant v = mainView.Image();
      ImageVariant s0 = s0View.Image();
      ImageVariant s1 = s1View.Image();
      ImageVariant s2 = s2View.Image();
      ImageVariant s3 = s3View.Image();

      if ( v.IsFloatSample() )
         switch ( v.BitsPerSample() )
         {
         case 32 : __Combine( static_cast<Image&>(*v), static_cast<Image&>(*s0), static_cast<Image&>(*s1), static_cast<Image&>(*s2), static_cast<Image&>(*s3) ); break;
         case 64 : __Combine( static_cast<DImage&>( *v ), static_cast<DImage&>(*s0), static_cast<DImage&>(*s1), static_cast<DImage&>(*s2), static_cast<DImage&>(*s3) ); break;
         }
      else
         switch ( v.BitsPerSample() )
         {
         case  8 : __Combine( static_cast<UInt8Image&>( *v ), static_cast<UInt8Image&>(*s0), static_cast<UInt8Image&>(*s1), static_cast<UInt8Image&>(*s2), static_cast<UInt8Image&>(*s3) ); break;
         case 16 : __Combine( static_cast<UInt16Image&>( *v ), static_cast<UInt16Image&>(*s0), static_cast<UInt16Image&>(*s1), static_cast<UInt16Image&>(*s2), static_cast<UInt16Image&>(*s3) ); break;
         case 32 : __Combine( static_cast<UInt32Image&>( *v ), static_cast<UInt32Image&>(*s0), static_cast<UInt32Image&>(*s1), static_cast<UInt32Image&>(*s2), static_cast<UInt32Image&>(*s3) ); break;
         }

      mainView.Unlock();
      s0View.Unlock();
      s1View.Unlock();
      s2View.Unlock();
      s3View.Unlock();

      w.Show();

      return true;
   }

   catch ( ... )
   {
      s0View.Unlock();
      s1View.Unlock();
      s2View.Unlock();
      s3View.Unlock();
      mainView.Unlock();
      w.Close();
      throw;
   }
}

void* MergeCFAInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheMergeCFASourceImage0Parameter )
      return p_viewId[0].Begin();
   if ( p == TheMergeCFASourceImage1Parameter )
      return p_viewId[1].Begin();
   if ( p == TheMergeCFASourceImage2Parameter )
      return p_viewId[2].Begin();
   if ( p == TheMergeCFASourceImage3Parameter )
      return p_viewId[3].Begin();
   return 0;
}

bool MergeCFAInstance::AllocateParameter( size_type length, const MetaParameter* p, size_type /*tableRow*/ )
{
   StringList::iterator s;
   if ( p == TheMergeCFASourceImage0Parameter )
      s = p_viewId.At( 0 );
   else if ( p == TheMergeCFASourceImage1Parameter )
      s = p_viewId.At( 1 );
   else if ( p == TheMergeCFASourceImage2Parameter )
      s = p_viewId.At( 2 );
   else if ( p == TheMergeCFASourceImage3Parameter )
      s = p_viewId.At( 3 );
   else
      return false;

   s->Clear();
   if ( length > 0 )
      s->Reserve( length );
   return true;
}

size_type MergeCFAInstance::ParameterLength( const MetaParameter* p, size_type /*tableRow*/ ) const
{
   if ( p == TheMergeCFASourceImage0Parameter )
      return p_viewId[0].Length();
   if ( p == TheMergeCFASourceImage1Parameter )
      return p_viewId[1].Length();
   if ( p == TheMergeCFASourceImage2Parameter )
      return p_viewId[2].Length();
   if ( p == TheMergeCFASourceImage3Parameter )
      return p_viewId[3].Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF MergeCFAInstance.cpp - Released 2016/02/21 20:22:43 UTC
