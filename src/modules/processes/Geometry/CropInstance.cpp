//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.01.00.0266
// ----------------------------------------------------------------------------
// CropInstance.cpp - Released 2015/07/31 11:49:48 UTC
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

#include "CropInstance.h"
#include "CropParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

CropInstance::CropInstance( const MetaProcess* P ) :
ProcessImplementation( P ),
p_margins( 0.0 ),
p_mode( CroppingMode::Default ),
p_resolution( TheXResolutionCropParameter->DefaultValue(), TheYResolutionCropParameter->DefaultValue() ),
p_metric( TheMetricResolutionCropParameter->DefaultValue() ),
p_forceResolution( TheForceResolutionCropParameter->DefaultValue() ),
p_fillColor( 4 )
{
   p_fillColor[0] = TheFillRedCropParameter->DefaultValue();
   p_fillColor[1] = TheFillGreenCropParameter->DefaultValue();
   p_fillColor[2] = TheFillBlueCropParameter->DefaultValue();
   p_fillColor[3] = TheFillAlphaCropParameter->DefaultValue();
}

CropInstance::CropInstance( const CropInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void CropInstance::Assign( const ProcessImplementation& p )
{
   const CropInstance* x = dynamic_cast<const CropInstance*>( &p );
   if ( x != 0 )
   {
      p_margins         = x->p_margins;
      p_mode            = x->p_mode;
      p_resolution      = x->p_resolution;
      p_metric          = x->p_metric;
      p_forceResolution = x->p_forceResolution;
      p_fillColor       = x->p_fillColor;
   }
}

// ----------------------------------------------------------------------------

bool CropInstance::CanExecuteOn( const View& v, String& whyNot ) const
{
   if ( v.IsPreview() )
   {
      whyNot = "Crop cannot be executed on previews.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

bool CropInstance::BeforeExecution( View& view )
{
   ImageWindow w = view.Window();

   if ( w.HasPreviews() || w.HasMaskReferences() || !w.Mask().IsNull() )
      if ( MessageBox( view.Id() + ":\n"
                       "Existing previews and mask references will be destroyed.\n"
                       "Some of these side effects could be irreversible. Proceed?",
                       "Crop", // caption
                       StdIcon::Warning,
                       StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes )
      {
         return false;
      }

   return true;
}

// ----------------------------------------------------------------------------

void CropInstance::GetNewSizes( int& w, int& h ) const
{
   Crop C( p_margins );
   C.SetMode( Crop::crop_mode( p_mode ) );
   C.SetResolution( p_resolution.x, p_resolution.y );
   C.SetMetricResolution( p_metric );
   C.GetNewSizes( w, h );
}

// ----------------------------------------------------------------------------

bool CropInstance::ExecuteOn( View& view )
{
   if ( !view.IsMainView() )
      return false;  // should not happen!

   if ( p_margins == 0.0 )
   {
      Console().WriteLn( "<end><cbr>&lt;* Identity *&gt;" );
      return true;
   }

   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   Crop C( p_margins );
   C.SetMode( static_cast<Crop::crop_mode>( p_mode ) );
   C.SetResolution( p_resolution.x, p_resolution.y );
   C.SetMetricResolution( p_metric );
   C.SetFillValues( p_fillColor );

   // Dimensions of target image
   int w0 = image.Width();
   int h0 = image.Height();

   // Dimensions of transformed image
   int width = w0, height = h0;
   C.GetNewSizes( width, height );

   if ( width < 1 || height < 1 )
      throw Error( "Crop: Invalid operation: Null target image dimensions" );

   /*
    * On 32-bit systems, make sure the resulting image requires less than 4 GB.
    */
   if ( sizeof( void* ) == sizeof( uint32 ) )
   {
      uint64 sz = uint64( width )*uint64( height )*image.NumberOfChannels()*image.BytesPerSample();
      if ( sz > uint64( uint32_max-256 ) )
         throw Error( "Crop: Invalid operation: Target image dimensions would exceed four gigabytes" );
   }

   ImageWindow window = view.Window();

   window.RemoveMaskReferences();
   window.RemoveMask();
   window.DeletePreviews();

   Console().EnableAbort();

   StandardStatus status;
   image.SetStatusCallback( &status );

   C >> image;

   if ( p_forceResolution )
   {
      Console().WriteLn( String().Format( "Setting resolution: h:%.3lf, image:%.3lf, px/%s",
                                          p_resolution.x, p_resolution.y, p_metric ? "cm" : "inch" ) );
      window.SetResolution( p_resolution.x, p_resolution.y, p_metric );
   }

   return true;
}

// ----------------------------------------------------------------------------

void* CropInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheLeftMarginParameter )
      return &p_margins.x0;
   if ( p == TheTopMarginParameter )
      return &p_margins.y0;
   if ( p == TheRightMarginParameter )
      return &p_margins.x1;
   if ( p == TheBottomMarginParameter )
      return &p_margins.y1;
   if ( p == TheCroppingModeParameter )
      return &p_mode;
   if ( p == TheXResolutionCropParameter )
      return &p_resolution.x;
   if ( p == TheYResolutionCropParameter )
      return &p_resolution.y;
   if ( p == TheMetricResolutionCropParameter )
      return &p_metric;
   if ( p == TheForceResolutionCropParameter )
      return &p_forceResolution;
   if ( p == TheFillRedCropParameter )
      return p_fillColor.At( 0 );
   if ( p == TheFillGreenCropParameter )
      return p_fillColor.At( 1 );
   if ( p == TheFillBlueCropParameter )
      return p_fillColor.At( 2 );
   if ( p == TheFillAlphaCropParameter )
      return p_fillColor.At( 3 );
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CropInstance.cpp - Released 2015/07/31 11:49:48 UTC
