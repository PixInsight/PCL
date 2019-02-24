//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 01.00.06.0191
// ----------------------------------------------------------------------------
// MergeCFAInstance.cpp - Released 2019-01-21T12:06:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2018 Nikolay Volkov
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L.
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
#include <pcl/AutoPointer.h>
#include <pcl/Console.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl {

// ----------------------------------------------------------------------------

MergeCFAInstance::MergeCFAInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   p_viewId( 4 ),
   o_outputViewId()
{
}

MergeCFAInstance::MergeCFAInstance( const MergeCFAInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

void MergeCFAInstance::Assign( const ProcessImplementation& p )
{
   const MergeCFAInstance* x = dynamic_cast<const MergeCFAInstance*>( &p );
   if ( x != nullptr )
   {
      p_viewId = x->p_viewId;
      o_outputViewId = x->o_outputViewId;
   }
}

bool MergeCFAInstance::CanExecuteOn( const View&, pcl::String& whyNot ) const
{
   whyNot = "MergeCFA can only be executed in the global context.";
   return false;
}

bool MergeCFAInstance::CanExecuteGlobal( String& whyNot ) const
{
   return true;
}

View MergeCFAInstance::GetView( int n )
{
   const String id = p_viewId[n];
   if ( id.IsEmpty() )
      throw Error( "MergeCFA: Source image #" + String( n ) + " not set." );

   ImageWindow w = ImageWindow::WindowById( id );
   if ( w.IsNull() )
      throw Error( "MergeCFA: Source image not found: " + id );

   ImageVariant image = w.MainView().Image();
   if ( n == 0 )
   {
      m_width = image.Width();
      m_height = image.Height();
      m_isColor = image.IsColor();
      m_isFloatSample = image.IsFloatSample();
      m_bitsPerSample = image.BitsPerSample();
      m_numberOfChannels = image.NumberOfChannels();
   }
   else
   {
      const String str = "MergeCFA: Incompatible source image ";
      if ( image.Width() != m_width || image.Height() != m_height || image.NumberOfChannels() != m_numberOfChannels )
         throw Error( str + "geometry: " + id );
      if ( image.BitsPerSample() != m_bitsPerSample || image.IsFloatSample() != m_isFloatSample )
         throw Error( str + "sample type : " + id );
      if ( image.IsColor() != m_isColor )
         throw Error( str + "color space: " + id );
   }
   return w.MainView();
}

template <class P>
static void MergeCFAImage( GenericImage<P>& outputImage,
                           const GenericImage<P>& inputImage0,
                           const GenericImage<P>& inputImage1,
                           const GenericImage<P>& inputImage2,
                           const GenericImage<P>& inputImage3 )
{
   for ( int c = 0; c < outputImage.NumberOfChannels(); ++c )
      for ( int sY = 0, tY = 0; tY < outputImage.Height(); tY += 2, ++sY )
      {
         for ( int sX = 0, tX = 0; tX < outputImage.Width(); tX += 2, ++sX )
         {
            outputImage( tX,   tY,   c ) = inputImage0( sX, sY, c );
            outputImage( tX,   tY+1, c ) = inputImage1( sX, sY, c );
            outputImage( tX+1, tY,   c ) = inputImage2( sX, sY, c );
            outputImage( tX+1, tY+1, c ) = inputImage3( sX, sY, c );
         }
         outputImage.Status() += outputImage.Width()/2;
      }
}

bool MergeCFAInstance::ExecuteGlobal()
{
   o_outputViewId.Clear();

   Array<View> sourceView;
   for ( int i = 0; i < 4; ++i )
      sourceView << GetView( i );

   ImageWindow outputWindow( m_width*2, m_height*2, m_numberOfChannels, m_bitsPerSample, m_isFloatSample, m_isColor, true );
   if ( outputWindow.IsNull() )
      throw Error( "MergeCFA: Unable to create target image." );
   View outputView = outputWindow.MainView();

   try
   {
      volatile AutoViewLock outputLock( outputView );
      Array<AutoPointer<AutoViewWriteLock> > sourceViewLock;
      sourceViewLock << new AutoViewWriteLock( sourceView[0] );
      for ( int i = 1; i < 4; ++i )
         if ( sourceView[i].CanWrite() ) // allow the same view selected for several input channels
            sourceViewLock << new AutoViewWriteLock( sourceView[i] );

      ImageVariant outputImage = outputView.Image();
      Array<ImageVariant> inputImage;
      for ( int i = 0; i < 4; ++i )
         inputImage << sourceView[i].Image();

      StandardStatus status;
      outputImage.SetStatusCallback( &status );
      outputImage.Status().Initialize( "Merging CFA components", outputImage.NumberOfSamples()/4 );

      Console().EnableAbort();

#define MERGE_CFA_IMAGE( I )                                \
   MergeCFAImage( static_cast<I&>( *outputImage ),          \
                  static_cast<const I&>( *inputImage[0] ),  \
                  static_cast<const I&>( *inputImage[1] ),  \
                  static_cast<const I&>( *inputImage[2] ),  \
                  static_cast<const I&>( *inputImage[3] ) )

      if ( outputImage.IsFloatSample() )
         switch ( outputImage.BitsPerSample() )
         {
         case 32: MERGE_CFA_IMAGE( Image ); break;
         case 64: MERGE_CFA_IMAGE( DImage ); break;
         }
      else
         switch ( outputImage.BitsPerSample() )
         {
         case  8: MERGE_CFA_IMAGE( UInt8Image ); break;
         case 16: MERGE_CFA_IMAGE( UInt16Image ); break;
         case 32: MERGE_CFA_IMAGE( UInt32Image ); break;
         }

#undef MERGE_CFA_IMAGE

      Console().DisableAbort();

      o_outputViewId = outputView.Id();

      outputWindow.Show();
      return true;
   }
   catch ( ... )
   {
      outputWindow.Close();
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
   if ( p == TheMergeCFAOutputViewIdParameter )
      return o_outputViewId.Begin();
   return nullptr;
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
   else if ( p == TheMergeCFAOutputViewIdParameter )
      s = &o_outputViewId;
   else
      return false;

   s->Clear();
   if ( length > 0 )
      s->SetLength( length );
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
   if ( p == TheMergeCFAOutputViewIdParameter )
      return o_outputViewId.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF MergeCFAInstance.cpp - Released 2019-01-21T12:06:42Z
