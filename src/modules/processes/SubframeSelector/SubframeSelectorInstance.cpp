//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.00.02.0261
// ----------------------------------------------------------------------------
// SubframeSelectorInstance.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "SubframeSelectorInstance.h"
#include "SubframeSelectorParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SubframeSelectorInstance::SubframeSelectorInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   targetFrames(),
   p_one( TheSubframeSelectorParameterOneParameter->DefaultValue() ),
   p_two( int32( TheSubframeSelectorParameterTwoParameter->DefaultValue() ) ),
   p_three( TheSubframeSelectorParameterThreeParameter->DefaultValue() ),
   p_four( SubframeSelectorParameterFour::Default ),
   p_five( TheSubframeSelectorParameterFiveParameter->DefaultValue() )
{
}

SubframeSelectorInstance::SubframeSelectorInstance( const SubframeSelectorInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

void SubframeSelectorInstance::Assign( const ProcessImplementation& p )
{
   const SubframeSelectorInstance* x = dynamic_cast<const SubframeSelectorInstance*>( &p );
   if ( x != nullptr )
   {
      targetFrames   = x->targetFrames;
      p_one          = x->p_one;
      p_two          = x->p_two;
      p_three        = x->p_three;
      p_four         = x->p_four;
      p_five         = x->p_five;
   }
}

bool SubframeSelectorInstance::CanExecuteOn( const View& view, String& whyNot ) const
{
   if ( targetFrames.IsEmpty() )
   {
      whyNot = "No target frames have been specified.";
      return false;
   }
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "SubframeSelector cannot be executed on complex images.";
      return false;
   }
   return true;
}

class SubframeSelectorEngine
{
public:

   template <class P>
   static void Apply( GenericImage<P>& image, const SubframeSelectorInstance& instance )
   {
      /*
       * Your magic comes here...
       */
      Console().WriteLn( "<end><cbr>Ah, did I mention that I do just <em>nothing</em> at all? ;D" );
   }
};

bool SubframeSelectorInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();
   if ( image.IsComplexSample() )
      return false;

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console().EnableAbort();

   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: SubframeSelectorEngine::Apply( static_cast<Image&>( *image ), *this ); break;
      case 64: SubframeSelectorEngine::Apply( static_cast<DImage&>( *image ), *this ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: SubframeSelectorEngine::Apply( static_cast<UInt8Image&>( *image ), *this ); break;
      case 16: SubframeSelectorEngine::Apply( static_cast<UInt16Image&>( *image ), *this ); break;
      case 32: SubframeSelectorEngine::Apply( static_cast<UInt32Image&>( *image ), *this ); break;
      }

   return true;
}

void* SubframeSelectorInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSSTargetFrameEnabledParameter )
      return &targetFrames[tableRow].enabled;
   if ( p == TheSSTargetFramePathParameter )
      return targetFrames[tableRow].path.Begin();
   if ( p == TheSubframeSelectorParameterOneParameter )
      return &p_one;
   if ( p == TheSubframeSelectorParameterTwoParameter )
      return &p_two;
   if ( p == TheSubframeSelectorParameterThreeParameter )
      return &p_three;
   if ( p == TheSubframeSelectorParameterFourParameter )
      return &p_four;
   if ( p == TheSubframeSelectorParameterFiveParameter )
      return p_five.Begin();

   return nullptr;
}

bool SubframeSelectorInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheSSTargetFramesParameter )
   {
      targetFrames.Clear();
      if ( sizeOrLength > 0 )
         targetFrames.Add( ImageItem(), sizeOrLength );
   }
   else if ( p == TheSSTargetFramePathParameter )
   {
      targetFrames[tableRow].path.Clear();
      if ( sizeOrLength > 0 )
         targetFrames[tableRow].path.SetLength( sizeOrLength );
   }
   if ( p == TheSubframeSelectorParameterFiveParameter )
   {
      p_five.Clear();
      if ( sizeOrLength > 0 )
         p_five.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type SubframeSelectorInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheSSTargetFramesParameter )
      return targetFrames.Length();
   if ( p == TheSSTargetFramePathParameter )
      return targetFrames[tableRow].path.Length();
   if ( p == TheSubframeSelectorParameterFiveParameter )
      return p_five.Length();

   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInstance.cpp - Released 2017-08-01T14:26:58Z
