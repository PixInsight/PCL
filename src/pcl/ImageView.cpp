//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// pcl/ImageView.cpp - Released 2015/10/08 11:24:19 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/ErrorHandler.h>
#include <pcl/ICCProfile.h>
#include <pcl/ImageView.h>
#include <pcl/RGBColorSystem.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ImageView::ImageView( Control& parent, int width, int height, int numberOfChannels,
                      int bitsPerSample, bool floatSample, bool color ) :
   ScrollBox( nullptr )
{
   TransferHandle( (*API->ImageView->CreateImageView)( ModuleHandle(), this, parent.handle, 0/*flags*/,
                                                       width, height, numberOfChannels,
                                                       bitsPerSample, floatSample, color ) );
   if ( IsNull() )
      throw APIFunctionError( "CreateImageView" );

   m_viewport.TransferHandle( (*API->ImageView->CreateImageViewViewport)( handle, &m_viewport ) );
   if ( m_viewport.IsNull() )
      throw APIFunctionError( "CreateImageViewViewport" );
}

ImageView::ImageView( void* h ) :
   ScrollBox( nullptr )
{
   TransferHandle( h );
   if ( !IsNull() )
   {
      m_viewport.TransferHandle( (*API->ImageView->CreateImageViewViewport)( handle, &m_viewport ) );
      if ( m_viewport.IsNull() )
         throw APIFunctionError( "CreateImageViewViewport" );
   }
}

ImageView::ImageView( void* h, void* hV ) :
   ScrollBox( h, hV )
{
}

// ----------------------------------------------------------------------------

ImageVariant ImageView::Image() const
{
   image_handle hImg = (*API->ImageView->GetImageViewImage)( handle );
   if ( hImg != 0 )
   {
      uint32 bitsPerSample;
      api_bool isFloat;
      if ( !(*API->SharedImage->GetImageFormat)( hImg, &bitsPerSample, &isFloat ) )
         throw APIFunctionError( "GetImageFormat" );

      /*
       * Use a private ImageVariant constructor that forces image ownership by
       * ImageVariant. This constructor is selected with the second dummy int
       * argument (set to zero below).
       */
      if ( isFloat )
         switch ( bitsPerSample )
         {
         case 32 : return ImageVariant( new pcl::Image( hImg ), 0 );
         case 64 : return ImageVariant( new pcl::DImage( hImg ), 0 );
         }
      else
         switch ( bitsPerSample )
         {
         case  8 : return ImageVariant( new pcl::UInt8Image( hImg ), 0 );
         case 16 : return ImageVariant( new pcl::UInt16Image( hImg ), 0 );
         case 32 : return ImageVariant( new pcl::UInt32Image( hImg ), 0 );
         }
   }

   return ImageVariant();
}

// ----------------------------------------------------------------------------

bool ImageView::IsColorImage() const
{
   return (*API->ImageView->IsImageViewColorImage)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageView::GetImageGeometry( int& width, int& height, int& numberOfChannels ) const
{
   if ( (*API->ImageView->GetImageViewImageGeometry)( handle, &width, &height, &numberOfChannels ) == api_false )
      throw APIFunctionError( "GetImageViewImageGeometry" );
}

// ----------------------------------------------------------------------------

void ImageView::GetSampleFormat( int& bitsPerSample, bool& floatSample ) const
{
   api_bool b = floatSample;
   if ( (*API->ImageView->GetImageViewSampleFormat)( handle, &bitsPerSample, &b ) == api_false )
      throw APIFunctionError( "GetImageViewSampleFormat" );
   floatSample = b != api_false;
}

// ----------------------------------------------------------------------------

void ImageView::SetSampleFormat( int bitsPerSample, bool floatSample )
{
   (*API->ImageView->SetImageViewSampleFormat)( handle, bitsPerSample, floatSample );
}

// ----------------------------------------------------------------------------

void ImageView::GetRGBWS( RGBColorSystem& rgbws ) const
{
   api_RGBWS a;
   (*API->ImageView->GetImageViewRGBWS)( handle, &a );
   rgbws = RGBColorSystem( a.gamma, a.isSRGBGamma != api_false, a.x, a.y, a.Y );
}

// ----------------------------------------------------------------------------

void ImageView::SetRGBWS( const RGBColorSystem& rgbws )
{
   api_RGBWS a;
   a.gamma = rgbws.Gamma();
   a.isSRGBGamma = rgbws.IsSRGB();
   memcpy( a.x, *rgbws.ChromaticityXCoordinates(), sizeof( a.x ) );
   memcpy( a.y, *rgbws.ChromaticityYCoordinates(), sizeof( a.y ) );
   memcpy( a.Y, *rgbws.LuminanceCoefficients(), sizeof( a.Y ) );
   (*API->ImageView->SetImageViewRGBWS)( handle, &a );
}

// ----------------------------------------------------------------------------

bool ImageView::IsColorManagementEnabled() const
{
   api_bool enableCM = api_false;
   (*API->ImageView->GetImageViewCMEnabled)( handle, &enableCM, 0, 0 );
   return enableCM != api_false;
}

// ----------------------------------------------------------------------------

void ImageView::EnableColorManagement( bool enable )
{
   api_bool proofing, gamutCheck;
   (*API->ImageView->GetImageViewCMEnabled)( handle, 0, &proofing, &gamutCheck );
   (*API->ImageView->SetImageViewCMEnabled)( handle, enable, proofing, gamutCheck );
}

// ----------------------------------------------------------------------------

bool ImageView::IsProofingEnabled() const
{
   api_bool proofing = api_false;
   (*API->ImageView->GetImageViewCMEnabled)( handle, 0, &proofing, 0 );
   return proofing != api_false;
}

// ----------------------------------------------------------------------------

void ImageView::EnableProofing( bool enable )
{
   api_bool enabled, gamutCheck;
   (*API->ImageView->GetImageViewCMEnabled)( handle, &enabled, 0, &gamutCheck );
   (*API->ImageView->SetImageViewCMEnabled)( handle, enabled, enable, gamutCheck );
}

// ----------------------------------------------------------------------------

bool ImageView::IsGamutCheckEnabled() const
{
   api_bool gamutCheck = api_false;
   (*API->ImageView->GetImageViewCMEnabled)( handle, 0, 0, &gamutCheck );
   return gamutCheck != api_false;
}

// ----------------------------------------------------------------------------

void ImageView::EnableGamutCheck( bool enable )
{
   api_bool enabled, proofing;
   (*API->ImageView->GetImageViewCMEnabled)( handle, &enabled, &proofing, 0 );
   (*API->ImageView->SetImageViewCMEnabled)( handle, enabled, proofing || enable, enable );
}

// ----------------------------------------------------------------------------

void ImageView::SetColorManagementFeatures( bool cmEnabled, bool proofing, bool gamutCheck )
{
   (*API->ImageView->SetImageViewCMEnabled)( handle, cmEnabled, proofing, gamutCheck );
}

// ----------------------------------------------------------------------------

bool ImageView::GetICCProfile( ICCProfile& profile ) const
{
   profile.Clear();

   size_type profileSize = (*API->ImageView->GetImageViewICCProfileLength)( handle );
   if ( profileSize == 0 )
      return false;

   try
   {
      ByteArray profileData( profileSize );
      (*API->ImageView->GetImageViewICCProfile)( handle, profileData.Begin() );
      profile.Set( profileData );
      return true;
   }
   ERROR_HANDLER

   return false;
}

// ----------------------------------------------------------------------------

void ImageView::SetICCProfile( const ICCProfile& profile )
{
   if ( profile.IsProfile() )
      (*API->ImageView->SetImageViewICCProfile)( handle, profile.ProfileData().Begin() );
}

// ----------------------------------------------------------------------------

void ImageView::SetICCProfile( const String& filePath )
{
   (*API->ImageView->LoadImageViewICCProfile)( handle, filePath.c_str() );
}

// ----------------------------------------------------------------------------

void ImageView::DeleteICCProfile()
{
   (*API->ImageView->DeleteImageViewICCProfile)( handle );
}

// ----------------------------------------------------------------------------

ImageView::gui_mode ImageView::CurrentMode() const
{
   return gui_mode( (*API->ImageView->GetImageViewMode)( handle ) );
}

// ----------------------------------------------------------------------------

void ImageView::SelectMode( gui_mode mode )
{
   (*API->ImageView->SetImageViewMode)( handle, mode );
}

// ----------------------------------------------------------------------------

ImageView::display_channel ImageView::CurrentChannel() const
{
   return display_channel( (*API->ImageView->GetImageViewDisplayChannel)( handle ) );
}

// ----------------------------------------------------------------------------

void ImageView::SelectChannel( display_channel channel )
{
   (*API->ImageView->SetImageViewDisplayChannel)( handle, channel );
}

// ----------------------------------------------------------------------------

ImageView::transparency_mode ImageView::TransparencyMode() const
{
   return transparency_mode( (*API->ImageView->GetImageViewTransparencyMode)( handle, 0 ) );
}

// ----------------------------------------------------------------------------

RGBA ImageView::TransparencyColor() const
{
   RGBA color;
   (*API->ImageView->GetImageViewTransparencyMode)( handle, &color );
   return color;
}

// ----------------------------------------------------------------------------

void ImageView::SetTransparencyMode( transparency_mode mode, RGBA color )
{
   (*API->ImageView->SetImageViewTransparencyMode)( handle, mode, color );
}

// ----------------------------------------------------------------------------

void ImageView::SetViewport( double cx, double cy, int zoom )
{
   (*API->ImageView->SetImageViewViewport)( handle, cx, cy, zoom );
}

// ----------------------------------------------------------------------------

int ImageView::ZoomFactor() const
{
   return (*API->ImageView->GetImageViewZoomFactor)( handle );
}

// ----------------------------------------------------------------------------

void ImageView::SetZoomFactor( int z )
{
   (*API->ImageView->SetImageViewZoomFactor)( handle, z );
}

// ----------------------------------------------------------------------------

void ImageView::GetViewportSize( int& width, int& height ) const
{
   (*API->ImageView->GetImageViewViewportSize)( handle, &width, &height );
}

// ----------------------------------------------------------------------------

Point ImageView::ViewportPosition() const
{
   Point p;
   (*API->ImageView->GetImageViewViewportPosition)( handle, &p.x, &p.y );
   return p;
}

// ----------------------------------------------------------------------------

void ImageView::SetViewportPosition( int x, int y )
{
   (*API->ImageView->SetImageViewViewportPosition)( handle, x, y );
}

// ----------------------------------------------------------------------------

Rect ImageView::VisibleViewportRect() const
{
   Rect r;
   (*API->ImageView->GetImageViewVisibleViewportRect)( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

void ImageView::ViewportToImage( int& x, int& y ) const
{
   (*API->ImageView->ViewportToImage)( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageView::ViewportToImage( double& x, double& y ) const
{
   (*API->ImageView->ViewportToImageD)( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageView::ViewportToImage( Point* p, size_type n ) const
{
   (*API->ImageView->ViewportToImageArray)( handle, reinterpret_cast<int32*>( p ), n );
}

// ----------------------------------------------------------------------------

void ImageView::ViewportToImage( DPoint* p, size_type n ) const
{
   (*API->ImageView->ViewportToImageArrayD)( handle, reinterpret_cast<double*>( p ), n );
}

// ----------------------------------------------------------------------------

void ImageView::ViewportScalarToImage( int* d, size_type n ) const
{
   (*API->ImageView->ViewportScalarToImageArray)( handle, d, n );
}

// ----------------------------------------------------------------------------

void ImageView::ViewportScalarToImage( double* d, size_type n ) const
{
   (*API->ImageView->ViewportScalarToImageArrayD)( handle, d, n );
}

// ----------------------------------------------------------------------------

double ImageView::ViewportScalarToImage( double d ) const
{
   (*API->ImageView->ViewportScalarToImageD)( handle, &d );
   return d;
}

// ----------------------------------------------------------------------------

void ImageView::ImageToViewport( int& x, int& y ) const
{
   (*API->ImageView->ImageToViewport)( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageView::ImageToViewport( double& x, double& y ) const
{
   (*API->ImageView->ImageToViewportD)( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageView::ImageToViewport( Point* p, size_type n ) const
{
   (*API->ImageView->ImageToViewportArray)( handle, reinterpret_cast<int32*>( p ), n );
}

// ----------------------------------------------------------------------------

void ImageView::ImageToViewport( DPoint* p, size_type n ) const
{
   (*API->ImageView->ImageToViewportArrayD)( handle, reinterpret_cast<double*>( p ), n );
}

// ----------------------------------------------------------------------------

void ImageView::ImageScalarToViewport( int* d, size_type n ) const
{
   (*API->ImageView->ImageScalarToViewportArray)( handle, d, n );
}

// ----------------------------------------------------------------------------

void ImageView::ImageScalarToViewport( double* d, size_type n ) const
{
   (*API->ImageView->ImageScalarToViewportArrayD)( handle, d, n );
}

// ----------------------------------------------------------------------------

int ImageView::ImageScalarToViewport( int d ) const
{
   (*API->ImageView->ImageScalarToViewport)( handle, &d );
   return d;
}

// ----------------------------------------------------------------------------

double ImageView::ImageScalarToViewport( double d ) const
{
   (*API->ImageView->ImageScalarToViewportD)( handle, &d );
   return d;
}

// ----------------------------------------------------------------------------

void ImageView::ViewportToGlobal( int& x, int& y ) const
{
   (*API->ImageView->ViewportToGlobal)( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageView::GlobalToViewport( int& x, int& y ) const
{
   (*API->ImageView->GlobalToViewport)( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageView::Regenerate()
{
   (*API->ImageView->RegenerateImageViewViewport)( handle );
}

// ----------------------------------------------------------------------------

void ImageView::RegenerateViewportRect( int x0, int y0, int x1, int y1 )
{
   (*API->ImageView->RegenerateViewportRect)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void ImageView::RegenerateImageRect( double x0, double y0, double x1, double y1 )
{
   (*API->ImageView->RegenerateImageRect)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void ImageView::UpdateViewport()
{
   (*API->ImageView->UpdateImageViewViewport)( handle );
}

// ----------------------------------------------------------------------------

void ImageView::UpdateViewportRect( int x0, int y0, int x1, int y1 )
{
   (*API->ImageView->UpdateViewportRect)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void ImageView::UpdateImageRect( double x0, double y0, double x1, double y1 )
{
   (*API->ImageView->UpdateImageRect)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

bool ImageView::HasPendingUpdates() const
{
   return (*API->ImageView->GetViewportUpdateRect)( handle, 0, 0, 0, 0 ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageView::CommitPendingUpdates()
{
   (*API->ImageView->CommitViewportUpdates)( handle );
}

// ----------------------------------------------------------------------------

Rect ImageView::ViewportUpdateRect() const
{
   Rect r;
   (*API->ImageView->GetViewportUpdateRect)( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

Bitmap ImageView::ViewportBitmap( int x0, int y0, int x1, int y1, uint32 flags ) const
{
   return Bitmap( (*API->ImageView->GetViewportBitmap)( ModuleHandle(), handle, x0, y0, x1, y1, flags ) );
}

// ----------------------------------------------------------------------------

void ImageView::BeginSelection( int x, int y, uint32 flags )
{
   (*API->ImageView->BeginViewportSelection)( handle, x, y, flags );
}

// ----------------------------------------------------------------------------

void ImageView::ModifySelection( int x, int y, uint32 flags )
{
   (*API->ImageView->ModifyViewportSelection)( handle, x, y, flags );
}

// ----------------------------------------------------------------------------

void ImageView::UpdateSelection()
{
   (*API->ImageView->UpdateViewportSelection)( handle );
}

// ----------------------------------------------------------------------------

void ImageView::CancelSelection()
{
   (*API->ImageView->CancelViewportSelection)( handle );
}

// ----------------------------------------------------------------------------

void ImageView::EndSelection()
{
   (*API->ImageView->EndViewportSelection)( handle );
}

// ----------------------------------------------------------------------------

Rect ImageView::SelectionRect( uint32* flags ) const
{
   Rect r;
   (*API->ImageView->GetViewportSelection)( handle, &r.x0, &r.y0, &r.x1, &r.y1, flags );
   return r;
}

// ----------------------------------------------------------------------------

bool ImageView::IsSelection() const
{
   return (*API->ImageView->GetViewportSelection)( handle, 0, 0, 0, 0, 0 ) != api_false;
}

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<ImageView*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))
#define handlers  sender->m_handlers

class ImageViewEventDispatcher
{
public:

   static void ViewportScrolled( control_handle hSender, control_handle hReceiver, int32 dx, int32 dy )
   {
      if ( handlers->onScrollViewport != nullptr )
         (receiver->*handlers->onScrollViewport)( *sender, dx, dy );
   }
}; // ImageViewEventDispatcher

#undef sender
#undef receiver
#undef handlers

// ----------------------------------------------------------------------------

#define INIT_EVENT_HANDLERS()    \
   __PCL_NO_ALIAS_HANDLERS;      \
   if ( m_handlers.IsNull() )    \
      m_handlers = new EventHandlers

void ImageView::OnScrollViewport( scroll_event_handler f, Control& receiver )
{
   INIT_EVENT_HANDLERS();
   if ( (*API->ImageView->SetImageViewScrollEventRoutine)( handle, &receiver,
                  (f != nullptr) ? ImageViewEventDispatcher::ViewportScrolled : 0 ) == api_false )
      throw APIFunctionError( "SetImageViewScrollEventRoutine" );
   m_handlers->onScrollViewport = f;
}

#undef INIT_EVENT_HANDLERS

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ImageView.cpp - Released 2015/10/08 11:24:19 UTC
