//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.01.00.0274
// ----------------------------------------------------------------------------
// IntegerResampleInstance.cpp - Released 2015/10/08 11:24:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "IntegerResampleInstance.h"
#include "IntegerResampleParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

IntegerResampleInstance::IntegerResampleInstance( const MetaProcess* P ) :
ProcessImplementation( P ),
p_zoomFactor( int32( TheZoomFactorParameter->DefaultValue() ) ),
p_downsampleMode( TheIntegerDownsamplingModeParameter->Default ),
p_resolution( TheXResolutionIntegerResampleParameter->DefaultValue(), TheYResolutionIntegerResampleParameter->DefaultValue() ),
p_metric( TheMetricResolutionIntegerResampleParameter->DefaultValue() ),
p_forceResolution( TheForceResolutionIntegerResampleParameter->DefaultValue() )
{
}

IntegerResampleInstance::IntegerResampleInstance( const IntegerResampleInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void IntegerResampleInstance::Assign( const ProcessImplementation& p )
{
   const IntegerResampleInstance* x = dynamic_cast<const IntegerResampleInstance*>( &p );
   if ( x != 0 )
   {
      p_zoomFactor      = x->p_zoomFactor;
      p_downsampleMode  = x->p_downsampleMode;
      p_resolution      = x->p_resolution;
      p_metric          = x->p_metric;
      p_forceResolution = x->p_forceResolution;
   }
}

// ----------------------------------------------------------------------------

bool IntegerResampleInstance::CanExecuteOn( const View& v, pcl::String& whyNot ) const
{
   if ( v.IsPreview() )
   {
      whyNot = "IntegerResample cannot be executed on previews.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

bool IntegerResampleInstance::BeforeExecution( View& view )
{
   ImageWindow w = view.Window();

   if ( w.HasPreviews() || w.HasMaskReferences() || !w.Mask().IsNull() )
      if ( MessageBox( view.Id() + ":\n"
                       "Existing previews and mask references will be destroyed.\n"
                       "Some of these side effects could be irreversible. Proceed?",
                       "IntegerResample", // caption
                       StdIcon::Warning,
                       StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes )
      {
         return false;
      }

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
      return false;  // should not happen!

   AutoViewLock lock( view );

   ImageWindow window = view.Window();
   ImageVariant image = view.Image();

   if ( !image.IsComplexSample() )
      if ( p_zoomFactor == 1 || p_zoomFactor == 0 || p_zoomFactor == -1 )
         Console().WriteLn( "<end><cbr>&lt;* Identity *&gt;" );
      else
      {
         IntegerResample I( p_zoomFactor, static_cast<IntegerResample::downsample_mode>( p_downsampleMode ) );

         /*
          * On 32-bit systems, make sure the resulting image requires less than 4 GB.
          */
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
               throw Error( "IntegerResample: Invalid operation: Target image dimensions would exceed four gigabytes" );
         }

         window.RemoveMaskReferences();
         window.RemoveMask();
         window.DeletePreviews();

         Console().EnableAbort();

         StandardStatus status;
         image.SetStatusCallback( &status );

         I >> image;
      }

   if ( p_forceResolution )
   {
      Console().WriteLn( String().Format( "Setting resolution: h:%.3lf, image:%.3lf, px/%s",
                                          p_resolution.x, p_resolution.y, p_metric ? "cm" : "inch" ) );
      window.SetResolution( p_resolution.x, p_resolution.y, p_metric );
   }

   return true;
}

// ----------------------------------------------------------------------------

void* IntegerResampleInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheZoomFactorParameter )
      return &p_zoomFactor;
   if ( p == TheIntegerDownsamplingModeParameter )
      return &p_downsampleMode;
   if ( p == TheXResolutionIntegerResampleParameter )
      return &p_resolution.x;
   if ( p == TheYResolutionIntegerResampleParameter )
      return &p_resolution.y;
   if ( p == TheMetricResolutionIntegerResampleParameter )
      return &p_metric;
   if ( p == TheForceResolutionIntegerResampleParameter )
      return &p_forceResolution;
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF IntegerResampleInstance.cpp - Released 2015/10/08 11:24:39 UTC
