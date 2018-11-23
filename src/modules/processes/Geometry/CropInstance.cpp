//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.02.0394
// ----------------------------------------------------------------------------
// CropInstance.cpp - Released 2018-11-23T18:45:58Z
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

#include "CropInstance.h"
#include "CropParameters.h"
#include "GeometryModule.h"

#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

CropInstance::CropInstance( const MetaProcess* P ) :
   ProcessImplementation( P ),
   p_margins( 0.0 ),
   p_mode( CRMode::Default ),
   p_resolution( TheCRXResolutionParameter->DefaultValue(), TheCRYResolutionParameter->DefaultValue() ),
   p_metric( TheCRMetricResolutionParameter->DefaultValue() ),
   p_forceResolution( TheCRForceResolutionParameter->DefaultValue() ),
   p_fillColor( TheCRFillRedParameter->DefaultValue(),
                TheCRFillGreenParameter->DefaultValue(),
                TheCRFillBlueParameter->DefaultValue(),
                TheCRFillAlphaParameter->DefaultValue() ),
   p_noGUIMessages( TheCRNoGUIMessagesParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

CropInstance::CropInstance( const CropInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void CropInstance::Assign( const ProcessImplementation& p )
{
   const CropInstance* x = dynamic_cast<const CropInstance*>( &p );
   if ( x != nullptr )
   {
      p_margins         = x->p_margins;
      p_mode            = x->p_mode;
      p_resolution      = x->p_resolution;
      p_metric          = x->p_metric;
      p_forceResolution = x->p_forceResolution;
      p_fillColor       = x->p_fillColor;
      p_noGUIMessages   = x->p_noGUIMessages;
   }
}

// ----------------------------------------------------------------------------

bool CropInstance::IsMaskable( const View&, const ImageWindow& ) const
{
   return false;
}

// ----------------------------------------------------------------------------

UndoFlags CropInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData
        | UndoFlag::Keywords
        | UndoFlag::AstrometricSolution
        | (p_forceResolution ? UndoFlag::Resolution : 0);
}

// ----------------------------------------------------------------------------

bool CropInstance::CanExecuteOn( const View& v, String& whyNot ) const
{
   if ( v.IsPreview() )
   {
      whyNot = "Crop cannot be executed on previews.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

bool CropInstance::BeforeExecution( View& view )
{
   if ( p_margins != 0.0 )
      return WarnOnAstrometryMetadataOrPreviewsOrMask( view.Window(), Meta()->Id(), p_noGUIMessages );
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
      return false;

   AutoViewLock lock( view );

   Console console;

   ImageWindow window = view.Window();

   if ( p_margins != 0.0 )
   {
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

      // On 32-bit systems, make sure the resulting image requires less than 4 GiB.
      if ( sizeof( void* ) == sizeof( uint32 ) )
      {
         uint64 sz = uint64( width )*uint64( height )*image.NumberOfChannels()*image.BytesPerSample();
         if ( sz > uint64( uint32_max-256 ) )
            throw Error( "Crop: Invalid operation: The resulting image would require more than 4 GiB" );
      }

      DeleteAstrometryMetadataAndPreviewsAndMask( window );

      console.EnableAbort();

      StandardStatus status;
      image.SetStatusCallback( &status );

      C >> image;
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

void* CropInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheCRLeftMarginParameter )
      return &p_margins.x0;
   if ( p == TheCRTopMarginParameter )
      return &p_margins.y0;
   if ( p == TheCRRightMarginParameter )
      return &p_margins.x1;
   if ( p == TheCRBottomMarginParameter )
      return &p_margins.y1;
   if ( p == TheCRModeParameter )
      return &p_mode;
   if ( p == TheCRXResolutionParameter )
      return &p_resolution.x;
   if ( p == TheCRYResolutionParameter )
      return &p_resolution.y;
   if ( p == TheCRMetricResolutionParameter )
      return &p_metric;
   if ( p == TheCRForceResolutionParameter )
      return &p_forceResolution;
   if ( p == TheCRFillRedParameter )
      return p_fillColor.At( 0 );
   if ( p == TheCRFillGreenParameter )
      return p_fillColor.At( 1 );
   if ( p == TheCRFillBlueParameter )
      return p_fillColor.At( 2 );
   if ( p == TheCRFillAlphaParameter )
      return p_fillColor.At( 3 );
   if ( p == TheCRNoGUIMessagesParameter )
      return & p_noGUIMessages;
   return nullptr;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CropInstance.cpp - Released 2018-11-23T18:45:58Z
