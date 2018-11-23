//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.02.0394
// ----------------------------------------------------------------------------
// IntegerResampleInstance.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "GeometryModule.h"
#include "IntegerResampleInstance.h"
#include "IntegerResampleParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

IntegerResampleInstance::IntegerResampleInstance( const MetaProcess* P ) :
   ProcessImplementation( P ),
   p_zoomFactor( int32( TheIRZoomFactorParameter->DefaultValue() ) ),
   p_downsampleMode( TheIRDownsamplingModeParameter->Default ),
   p_resolution( TheIRXResolutionParameter->DefaultValue(), TheIRYResolutionParameter->DefaultValue() ),
   p_metric( TheIRMetricResolutionParameter->DefaultValue() ),
   p_forceResolution( TheIRForceResolutionParameter->DefaultValue() ),
   p_noGUIMessages( TheIRNoGUIMessagesParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

IntegerResampleInstance::IntegerResampleInstance( const IntegerResampleInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void IntegerResampleInstance::Assign( const ProcessImplementation& p )
{
   const IntegerResampleInstance* x = dynamic_cast<const IntegerResampleInstance*>( &p );
   if ( x != nullptr )
   {
      p_zoomFactor      = x->p_zoomFactor;
      p_downsampleMode  = x->p_downsampleMode;
      p_resolution      = x->p_resolution;
      p_metric          = x->p_metric;
      p_forceResolution = x->p_forceResolution;
      p_noGUIMessages   = x->p_noGUIMessages;
   }
}

// ----------------------------------------------------------------------------

bool IntegerResampleInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

UndoFlags IntegerResampleInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData
        | UndoFlag::Keywords
        | UndoFlag::AstrometricSolution
        | (p_forceResolution ? UndoFlag::Resolution : 0);
}

// ----------------------------------------------------------------------------

bool IntegerResampleInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.IsPreview() )
   {
      whyNot = "IntegerResample cannot be executed on previews.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool IntegerResampleInstance::BeforeExecution( View& view )
{
   if ( p_zoomFactor > 1 || p_zoomFactor < -1 )
      return WarnOnAstrometryMetadataOrPreviewsOrMask( view.Window(), Meta()->Id(), p_noGUIMessages );
   return true;
}

// ----------------------------------------------------------------------------

void IntegerResampleInstance::GetNewSizes( int& width, int& height ) const
{
   IntegerResample R( p_zoomFactor );
   R.GetNewSizes( width, height );
}

// ----------------------------------------------------------------------------

bool IntegerResampleInstance::ExecuteOn( View& view )
{
   if ( !view.IsMainView() )
      return false;

   AutoViewLock lock( view );

   Console console;

   ImageWindow window = view.Window();
   ImageVariant image = view.Image();

   if ( !image.IsComplexSample() )
      if ( p_zoomFactor > 1 || p_zoomFactor < -1 )
      {
         IntegerResample I( p_zoomFactor, static_cast<IntegerResample::downsample_mode>( p_downsampleMode ) );

         // On 32-bit systems, make sure the resulting image requires less than 4 GB.
         if ( sizeof( void* ) == sizeof( uint32 ) )
         {
            // Dimensions of target image
            int w0 = image.Width();
            int h0 = image.Height();

            // Dimensions of transformed image
            int width = w0, height = h0;
            I.GetNewSizes( width, height );
            width = Max( 1, width );
            height = Max( 1, height );

            uint64 sz = uint64( width )*uint64( height )*image.NumberOfChannels()*image.BytesPerSample();
            if ( sz > uint64( uint32_max-256 ) )
               throw Error( "IntegerResample: Invalid operation: The resulting image would require more than 4 GiB" );
         }

         DeleteAstrometryMetadataAndPreviewsAndMask( window );

         console.EnableAbort();

         StandardStatus status;
         image.SetStatusCallback( &status );

         I >> image;
      }
      else
      {
         console.WriteLn( "<end><cbr>&lt;* identity *&gt;" );
      }

   if ( p_forceResolution )
   {
      console.WriteLn( String().Format( "<end><cbr>Setting resolution: h:%.3f, v:%.3f, u:px/%s",
                                        p_resolution.x, p_resolution.y, p_metric ? "cm" : "inch" ) );
      window.SetResolution( p_resolution.x, p_resolution.y, p_metric );
   }

   return true;
}

// ----------------------------------------------------------------------------

void* IntegerResampleInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheIRZoomFactorParameter )
      return &p_zoomFactor;
   if ( p == TheIRDownsamplingModeParameter )
      return &p_downsampleMode;
   if ( p == TheIRXResolutionParameter )
      return &p_resolution.x;
   if ( p == TheIRYResolutionParameter )
      return &p_resolution.y;
   if ( p == TheIRMetricResolutionParameter )
      return &p_metric;
   if ( p == TheIRForceResolutionParameter )
      return &p_forceResolution;
   if ( p == TheIRNoGUIMessagesParameter )
      return &p_noGUIMessages;
   return nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF IntegerResampleInstance.cpp - Released 2018-11-23T18:45:58Z
