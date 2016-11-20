//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.00.0322
// ----------------------------------------------------------------------------
// ResampleInstance.cpp - Released 2016/11/17 18:14:58 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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
#include "ResampleInstance.h"
#include "ResampleParameters.h"

#include <pcl/AutoPointer.h>
#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/IntegerResample.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ResampleInstance::ResampleInstance( const MetaProcess* P ) :
   ProcessImplementation( P ),
   p_size( TheRSXSizeParameter->DefaultValue(), TheRSYSizeParameter->DefaultValue() ),
   p_mode( TheRSModeParameter->Default ),
   p_absMode( TheRSAbsoluteModeParameter->Default ),
   p_resolution( TheRSXResolutionParameter->DefaultValue(), TheRSYResolutionParameter->DefaultValue() ),
   p_metric( TheRSMetricResolutionParameter->DefaultValue() ),
   p_forceResolution( TheRSForceResolutionParameter->DefaultValue() ),
   p_interpolation( TheRSInterpolationAlgorithmParameter->Default ),
   p_clampingThreshold( TheRSClampingThresholdParameter->DefaultValue() ),
   p_smoothness( TheRSSmoothnessParameter->DefaultValue() )
{
}

ResampleInstance::ResampleInstance( const ResampleInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

void ResampleInstance::Assign( const ProcessImplementation& p )
{
   const ResampleInstance* x = dynamic_cast<const ResampleInstance*>( &p );
   if ( x != nullptr )
   {
      p_size = x->p_size;
      p_mode = x->p_mode;
      p_absMode = x->p_absMode;
      p_resolution = x->p_resolution;
      p_metric = x->p_metric;
      p_forceResolution = x->p_forceResolution;
      p_interpolation = x->p_interpolation;
      p_clampingThreshold = x->p_clampingThreshold;
      p_smoothness = x->p_smoothness;
   }
}

bool ResampleInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

UndoFlags ResampleInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData | UndoFlag::Keywords | (p_forceResolution ? UndoFlag::Resolution : 0);
}

bool ResampleInstance::CanExecuteOn( const View& v, String& whyNot ) const
{
   if ( v.IsPreview() )
   {
      whyNot = "Resample cannot be executed on previews.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool ResampleInstance::BeforeExecution( View& view )
{
   return WarnOnAstrometryMetadataOrPreviewsOrMask( view.Window(), Meta()->Id() );
}

void ResampleInstance::GetNewSizes( int& width, int& height ) const
{
   NearestNeighborPixelInterpolation I;
   Resample S( I, p_size.x, p_size.y );
   S.SetMode( Resample::resize_mode( p_mode ) );
   S.SetAbsMode( Resample::abs_resize_mode( p_absMode ) );
   S.SetResolution( p_resolution.x, p_resolution.y );
   S.SetMetricResolution( p_metric );
   S.GetNewSizes( width, height );
}

bool ResampleInstance::ExecuteOn( View& view )
{
   if ( !view.IsMainView() )
      return false;

   AutoViewLock lock( view );

   ImageWindow window = view.Window();
   ImageVariant image = view.Image();

   if ( !image.IsComplexSample() )
   {
      // Dimensions of the target image
      int w0 = image.Width();
      int h0 = image.Height();

      // Dimensions of the transformed image
      int width = w0, height = h0;
      GetNewSizes( width, height );
      width = Max( 1, width );
      height = Max( 1, height );

      if ( width == w0 && height == h0 )
      {
         Console().WriteLn( "<end><cbr>&lt;* Identity *&gt;" );
         return true;
      }

      /*
       * On 32-bit systems, make sure the resulting image requires less than 4 GB.
       */
      if ( sizeof( void* ) == sizeof( uint32 ) )
      {
         uint64 sz = uint64( width )*uint64( height )*image.NumberOfChannels()*image.BytesPerSample();
         if ( sz > uint64( uint32_max-256 ) )
            throw Error( "Resample: Invalid operation: Target image dimensions would exceed four gigabytes" );
      }

      AutoPointer<PixelInterpolation> interpolation( NewInterpolation(
         p_interpolation, width, height, w0, h0, false, p_clampingThreshold, p_smoothness, image ) );

      DeleteAstrometryMetadataAndPreviewsAndMask( window );

      Console().EnableAbort();

      StandardStatus status;
      image.SetStatusCallback( &status );

      Resample S( *interpolation, p_size.x, p_size.y );
      S.SetSizes( width, height );
      S.SetMode( ResizeMode::AbsolutePixels );
      S.SetAbsMode( AbsoluteResizeMode::ForceWidthAndHeight );
      S >> image;
   }

   if ( p_forceResolution )
   {
      Console().WriteLn( String().Format( "Setting resolution: h:%.3lf, v:%.3lf, u:px/%s",
                                          p_resolution.x, p_resolution.y, p_metric ? "cm" : "inch" ) );
      window.SetResolution( p_resolution.x, p_resolution.y, p_metric );
   }

   return true;
}

// ----------------------------------------------------------------------------

void* ResampleInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheRSXSizeParameter )
      return &p_size.x;
   if ( p == TheRSYSizeParameter )
      return &p_size.y;
   if ( p == TheRSModeParameter )
      return &p_mode;
   if ( p == TheRSAbsoluteModeParameter )
      return &p_absMode;
   if ( p == TheRSXResolutionParameter )
      return &p_resolution.x;
   if ( p == TheRSYResolutionParameter )
      return &p_resolution.y;
   if ( p == TheRSMetricResolutionParameter )
      return &p_metric;
   if ( p == TheRSForceResolutionParameter )
      return &p_forceResolution;
   if ( p == TheRSInterpolationAlgorithmParameter )
      return &p_interpolation;
   if ( p == TheRSClampingThresholdParameter )
      return &p_clampingThreshold;
   if ( p == TheRSSmoothnessParameter )
      return &p_smoothness;
   return nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ResampleInstance.cpp - Released 2016/11/17 18:14:58 UTC
