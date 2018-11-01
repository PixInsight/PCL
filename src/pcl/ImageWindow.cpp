//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/ImageWindow.cpp - Released 2018-11-01T11:06:52Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/AutoLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/RGBColorSystem.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class InternalWindowEnumerator
{
public:

   static api_bool api_func Callback( window_handle hW, void* ptrToArray )
   {
      reinterpret_cast<Array<ImageWindow>*>( ptrToArray )->Add( ImageWindow( hW ) );
      return api_true;
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ImageWindow::ImageWindow( int width, int height, int numberOfChannels,
                          int bitsPerSample, bool floatSample, bool color,
                          bool initialProcessing, const IsoString& id ) :
   UIObject( (*API->ImageWindow->CreateImageWindow)(
                        width, height, numberOfChannels, bitsPerSample, floatSample, color,
                        initialProcessing, id.c_str() ) )
{
   if ( IsNull() )
      throw APIFunctionError( "CreateImageWindow" );
}

// ----------------------------------------------------------------------------

ImageWindow& ImageWindow::Null()
{
   static ImageWindow* nullImageWindow = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullImageWindow == nullptr )
      nullImageWindow = new ImageWindow( nullptr );
   return *nullImageWindow;
}

// ----------------------------------------------------------------------------

Array<ImageWindow> ImageWindow::Open( const String& url,
               const IsoString& id, const IsoString& formatHints, bool asACopy, bool allowMessages )
{
   Array<ImageWindow> a;
   if ( (*API->ImageWindow->LoadImageWindows)( url.c_str(), id.c_str(), formatHints.c_str(),
                                 api_bool( asACopy ), api_bool( allowMessages ),
                                 InternalWindowEnumerator::Callback, &a ) == api_false )
      throw APIFunctionError( "LoadImageWindows" );
   return a;
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsNew() const
{
   return (*API->ImageWindow->GetImageWindowNewFlag)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsACopy() const
{
   return (*API->ImageWindow->GetImageWindowCopyFlag)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

String ImageWindow::FilePath() const
{
   size_type len = 0;
   (*API->ImageWindow->GetImageWindowFilePath)( handle, 0, &len );

   String path;
   if ( len > 0 )
   {
      path.SetLength( len );
      if ( (*API->ImageWindow->GetImageWindowFilePath)( handle, path.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetImageWindowFilePath" );
      path.ResizeToNullTerminated();
   }
   return path;
}

// ----------------------------------------------------------------------------

String ImageWindow::FileURL() const
{
   size_type len = 0;
   (*API->ImageWindow->GetImageWindowFileURL)( handle, 0, &len );

   String url;
   if ( len > 0 )
   {
      url.SetLength( len );
      if ( (*API->ImageWindow->GetImageWindowFileURL)( handle, url.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetImageWindowFileURL" );
      url.ResizeToNullTerminated();
   }
   return url;
}

// ----------------------------------------------------------------------------

ImageOptions ImageWindow::FileInfo() const
{
   api_image_file_info a;
   (*API->ImageWindow->GetImageWindowFileInfo)( handle, &a );

   ImageOptions o;
   o.bitsPerSample         = a.bitsPerSample;
   o.ieeefpSampleFormat    = a.ieeefpSampleFormat;
   o.complexSample         = a.complexSample;
   o.signedIntegers        = a.signedIntegers;
   o.metricResolution      = a.metricResolution;
   o.embedICCProfile       = a.embedICCProfile;
   o.embedThumbnail        = a.embedThumbnail;
   o.embedProperties       = a.embedProperties;
   o.embedRGBWS            = a.embedRGBWS;
   o.embedDisplayFunction  = a.embedDisplayFunction;
   o.embedColorFilterArray = a.embedColorFilterArray;
   o.embedPreviewRects     = a.embedPreviewRects;
   o.xResolution           = a.xResolution;
   o.yResolution           = a.yResolution;
   o.isoSpeed              = a.isoSpeed;
   o.exposure              = a.exposure;
   o.aperture              = a.aperture;
   o.focalLength           = a.focalLength;
   o.cfaType               = a.cfaType;
   return o;
}

// ----------------------------------------------------------------------------

size_type ImageWindow::ModifyCount() const
{
   return (*API->ImageWindow->GetImageWindowModifyCount)( handle );
}

// ----------------------------------------------------------------------------

bool ImageWindow::Close()
{
   return (*API->ImageWindow->CloseImageWindow)( handle, api_false/*force*/ ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::ForceClose()
{
   if ( (*API->ImageWindow->CloseImageWindow)( handle, api_true/*force*/ ) == api_false )
      throw APIFunctionError( "CloseImageWindow" );
}

// ----------------------------------------------------------------------------

View ImageWindow::MainView() const
{
   return View( (*API->ImageWindow->GetImageWindowMainView)( handle ) );
}

// ----------------------------------------------------------------------------

View ImageWindow::CurrentView() const
{
   return View( (*API->ImageWindow->GetImageWindowCurrentView)( handle ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::SelectView( View& v )
{
   (*API->ImageWindow->SetImageWindowCurrentView)( handle, v.handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::PurgeProperties()
{
   (*API->ImageWindow->PurgeImageWindowProperties)( handle );
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsValidView( const View& v ) const
{
   return (*API->ImageWindow->ValidateImageWindowView)( handle, v.handle ) != api_false;
}

// ----------------------------------------------------------------------------

int ImageWindow::NumberOfPreviews() const
{
   return (*API->ImageWindow->GetPreviewCount)( handle );
}

// ----------------------------------------------------------------------------

class InternalPreviewEnumerator
{
public:

   static api_bool api_func Callback( view_handle hV, void* ptrToArray )
   {
      reinterpret_cast<Array<View>*>( ptrToArray )->Add( View( hV ) );
      return api_true;
   }
};

Array<View> ImageWindow::Previews() const
{
   Array<View> a;
   (*API->ImageWindow->EnumeratePreviews)( handle, InternalPreviewEnumerator::Callback, &a );
   return a;
}

// ----------------------------------------------------------------------------

View ImageWindow::PreviewById( const IsoString& previewId ) const
{
   return View( (*API->ImageWindow->GetPreviewById)( handle, previewId.c_str() ) );
}

// ----------------------------------------------------------------------------

View ImageWindow::SelectedPreview() const
{
   return View( (*API->ImageWindow->GetSelectedPreview)( handle ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::SelectPreview( View& preview )
{
   (*API->ImageWindow->SelectPreview)( handle, preview.handle );
}

// ----------------------------------------------------------------------------

View ImageWindow::CreatePreview( int x0, int y0, int x1, int y1, const IsoString& previewId )
{
   return View( (*API->ImageWindow->CreatePreview)( handle, x0, y0, x1, y1, previewId.c_str() ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::ModifyPreview( const IsoString& previewId, int x0, int y0, int x1, int y1, const IsoString& newId )
{
   (*API->ImageWindow->ModifyPreview)( handle, previewId.c_str(), x0, y0, x1, y1, newId.c_str() );
}

// ----------------------------------------------------------------------------

pcl::Rect ImageWindow::PreviewRect( const IsoString& previewId ) const
{
   pcl::Rect r( -1, -1, -1, -1 );
   (*API->ImageWindow->GetPreviewRect)( handle, previewId.c_str(), &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

void ImageWindow::DeletePreview( const IsoString& previewId )
{
   (*API->ImageWindow->DeletePreview)( handle, previewId.c_str() );
}

// ----------------------------------------------------------------------------

void ImageWindow::DeletePreviews()
{
   (*API->ImageWindow->DeletePreviews)( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::GetSampleFormat( int& bitsPerSample, bool& floatSample ) const
{
   uint32 bits;
   api_bool flt;
   (*API->ImageWindow->GetImageWindowSampleFormat)( handle, &bits, &flt );
   bitsPerSample = int( bits );
   floatSample = flt != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::SetSampleFormat( int bitsPerSample, bool floatSample )
{
   (*API->ImageWindow->SetImageWindowSampleFormat)( handle, bitsPerSample, api_bool( floatSample ) );
}

// ----------------------------------------------------------------------------

ImageWindow ImageWindow::Mask() const
{
   return ImageWindow( (*API->ImageWindow->GetImageWindowMask)( handle, 0 ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetMask( ImageWindow& mask, bool inverted )
{
   (*API->ImageWindow->SetImageWindowMask)( handle, mask.handle, inverted );
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsMaskInverted() const
{
   api_bool inverted;
   (void)(*API->ImageWindow->GetImageWindowMask)( handle, &inverted );
   return inverted != api_false;
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsMaskEnabled() const
{
   return (*API->ImageWindow->GetImageWindowMaskEnabled)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::EnableMask( bool enable )
{
   (*API->ImageWindow->SetImageWindowMaskEnabled)( handle, enable );
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsMaskVisible() const
{
   return (*API->ImageWindow->GetImageWindowMaskVisible)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::ShowMask( bool show )
{
   (*API->ImageWindow->SetImageWindowMaskVisible)( handle, show );
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsMaskCompatible( const ImageWindow& mask )
{
   return (*API->ImageWindow->ValidateImageWindowMask)( handle, mask.handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool ImageWindow::HasMaskReferences() const
{
   return (*API->ImageWindow->GetMaskReferenceCount)( handle ) > 0;
}

// ----------------------------------------------------------------------------

void ImageWindow::RemoveMaskReferences()
{
   (*API->ImageWindow->RemoveImageWindowMaskReferences)( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::UpdateMaskReferences()
{
   (*API->ImageWindow->UpdateImageWindowMaskReferences)( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::GetRGBWS( RGBColorSystem& rgbws ) const
{
   api_RGBWS a;
   (*API->ImageWindow->GetImageWindowRGBWS)( handle, &a );
   rgbws = RGBColorSystem( a.gamma, a.isSRGBGamma != api_false, a.x, a.y, a.Y );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetRGBWS( const RGBColorSystem& rgbws )
{
   api_RGBWS a;
   a.gamma = rgbws.Gamma();
   a.isSRGBGamma = rgbws.IsSRGB();
   memcpy( a.x, *rgbws.ChromaticityXCoordinates(), sizeof( a.x ) );
   memcpy( a.y, *rgbws.ChromaticityYCoordinates(), sizeof( a.y ) );
   memcpy( a.Y, *rgbws.LuminanceCoefficients(), sizeof( a.Y ) );
   (*API->ImageWindow->SetImageWindowRGBWS)( handle, &a );
}

// ----------------------------------------------------------------------------

bool ImageWindow::UsingGlobalRGBWS() const
{
   return (*API->ImageWindow->GetImageWindowGlobalRGBWS)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::UseGlobalRGBWS()
{
   (*API->ImageWindow->SetImageWindowGlobalRGBWS)( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::GetGlobalRGBWS( RGBColorSystem& rgbws )
{
   api_RGBWS a;
   (*API->ImageWindow->GetGlobalRGBWS)( &a );
   rgbws = RGBColorSystem( a.gamma, a.isSRGBGamma != api_false, a.x, a.y, a.Y );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetGlobalRGBWS( const RGBColorSystem& rgbws )
{
   api_RGBWS a;
   a.gamma = rgbws.Gamma();
   a.isSRGBGamma = rgbws.IsSRGB();
   memcpy( a.x, *rgbws.ChromaticityXCoordinates(), sizeof( a.x ) );
   memcpy( a.y, *rgbws.ChromaticityYCoordinates(), sizeof( a.y ) );
   memcpy( a.Y, *rgbws.LuminanceCoefficients(), sizeof( a.Y ) );
   (*API->ImageWindow->SetGlobalRGBWS)( &a );
}

// ----------------------------------------------------------------------------

FITSKeywordArray ImageWindow::Keywords() const
{
   FITSKeywordArray keywords;
   int n = (*API->ImageWindow->GetImageWindowKeywordCount)( handle );
   if ( n > 0 )
      for ( int i = 0; i < n; ++i )
      {
         IsoString name, value, comment;
         name.Reserve( 256 );
         value.Reserve( 256 );
         comment.Reserve( 256 );
         // N.B. The passed maximum lengths *do not* include an ending null character.
         (*API->ImageWindow->GetImageWindowKeyword)( handle, i,
                        name.Begin(), 255, value.Begin(), 255, comment.Begin(), 255 );
         name.ResizeToNullTerminated();
         value.ResizeToNullTerminated();
         comment.ResizeToNullTerminated();
         keywords << FITSHeaderKeyword( name.c_str(), value.c_str(), comment.c_str() );
      }
   return keywords;
}

// ----------------------------------------------------------------------------

void ImageWindow::SetKeywords( const FITSKeywordArray& keywords )
{
   (*API->ImageWindow->ResetImageWindowKeywords)( handle );
   for ( const FITSHeaderKeyword& k : keywords )
      (*API->ImageWindow->AddImageWindowKeyword)( handle, k.name.c_str(), k.value.c_str(), k.comment.c_str() );
}

// ----------------------------------------------------------------------------

void ImageWindow::ResetKeywords()
{
   (*API->ImageWindow->ResetImageWindowKeywords)( handle );
}

// ----------------------------------------------------------------------------

bool ImageWindow::HasAstrometricSolution() const
{
   return (*API->ImageWindow->GetImageWindowHasAstrometricSolution)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool ImageWindow::RegenerateAstrometricSolution( bool allowGUIMessages, bool notify )
{
   return (*API->ImageWindow->RegenerateImageWindowAstrometricSolution)( handle,
                                          api_bool( allowGUIMessages ), api_bool( notify ) ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::ClearAstrometricSolution( bool notify )
{
   (*API->ImageWindow->ClearImageWindowAstrometricSolution)( handle, api_bool( notify ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::UpdateAstrometryMetadata( bool notify )
{
   (*API->ImageWindow->UpdateImageWindowAstrometryMetadata)( handle, api_bool( notify ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::GetResolution( double& xRes, double& yRes, bool& metric ) const
{
   api_bool m;
   (*API->ImageWindow->GetImageWindowResolution)( handle, &xRes, &yRes, &m );
   metric = m != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::SetResolution( double xRes, double yRes, bool metric )
{
   (*API->ImageWindow->SetImageWindowResolution)( handle, xRes, yRes, metric );
}

// ----------------------------------------------------------------------------

void ImageWindow::GetDefaultResolution( double& xRes, double& yRes, bool& metric )
{
   api_bool m;
   (*API->ImageWindow->GetDefaultResolution)( &xRes, &yRes, &m );
   metric = m != api_false;
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsDefaultICCProfileEmbeddingEnabledForRGBImages()
{
   api_bool rgb = api_false;
   (*API->ImageWindow->GetDefaultICCProfileEmbedding)( &rgb, 0 );
   return rgb != api_false;
}

bool ImageWindow::IsDefaultICCProfileEmbeddingEnabledForGrayscaleImages()
{
   api_bool gray = api_false;
   (*API->ImageWindow->GetDefaultICCProfileEmbedding)( 0, &gray );
   return gray != api_false;
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsDefaultThumbnailEmbeddingEnabled()
{
   return (*API->ImageWindow->GetDefaultThumbnailEmbedding)() != api_false;
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsDefaultPropertiesEmbeddingEnabled()
{
   return (*API->ImageWindow->GetDefaultPropertiesEmbedding)() != api_false;
}

// ----------------------------------------------------------------------------

StringList ImageWindow::SwapDirectories()
{
   StringList directories;

   for ( int i = 0; ; ++i )
   {
      size_type len = 0;
      (*API->ImageWindow->GetSwapDirectory)( i, 0, &len );
      if ( len == 0 )
         break;

      String path;
      path.SetLength( len );
      if ( (*API->ImageWindow->GetSwapDirectory)( i, path.Begin(), &len ) == api_false )
         throw APIFunctionError( "GetSwapDirectory" );
      path.ResizeToNullTerminated();

      directories.Add( path );
   }

   return directories;
}

// ----------------------------------------------------------------------------

bool ImageWindow::SetSwapDirectories( const StringList& directories )
{
   Array<const char16_type*> strings;
   for ( const String& dir : directories )
      strings.Add( dir.c_str() );
   return (*API->ImageWindow->SetSwapDirectories)( strings.Begin(), int32( strings.Length() ) ) != api_false;
}

// ----------------------------------------------------------------------------

int ImageWindow::CursorTolerance()
{
   return (*API->ImageWindow->GetCursorTolerance)();
}

// ----------------------------------------------------------------------------

ImageWindow::gui_mode ImageWindow::CurrentMode()
{
   return gui_mode( (*API->ImageWindow->GetImageWindowMode)() );
}

// ----------------------------------------------------------------------------

void ImageWindow::SelectMode( ImageWindow::gui_mode mode )
{
   (*API->ImageWindow->SetImageWindowMode)( mode );
}

// ----------------------------------------------------------------------------

ImageWindow::display_channel ImageWindow::CurrentChannel() const
{
   return display_channel( (*API->ImageWindow->GetImageWindowDisplayChannel)( handle ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::SelectChannel( ImageWindow::display_channel chn )
{
   (*API->ImageWindow->SetImageWindowDisplayChannel)( handle, chn );
}

// ----------------------------------------------------------------------------

ImageWindow::mask_mode ImageWindow::MaskMode() const
{
   return mask_mode( (*API->ImageWindow->GetImageWindowMaskMode)( handle ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetMaskMode( ImageWindow::mask_mode mode )
{
   (*API->ImageWindow->SetImageWindowMaskMode)( handle, mode );
}

// ----------------------------------------------------------------------------

ImageWindow::background_brush ImageWindow::GetBackgroundBrush( uint32& fgColor, uint32& bgColor )
{
   return background_brush( (*API->ImageWindow->GetTransparencyBackgroundBrush)( &fgColor, &bgColor ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetBackgroundBrush( ImageWindow::background_brush brush, uint32 fgColor, uint32 bgColor )
{
   (*API->ImageWindow->SetTransparencyBackgroundBrush)( brush, fgColor, bgColor );
}

// ----------------------------------------------------------------------------

ImageWindow::transparency_mode ImageWindow::TransparencyMode() const
{
   return transparency_mode( (*API->ImageWindow->GetImageWindowTransparencyMode)( handle, 0 ) );
}

// ----------------------------------------------------------------------------

RGBA ImageWindow::TransparencyColor() const
{
   uint32 color;
   (void)(*API->ImageWindow->GetImageWindowTransparencyMode)( handle, &color );
   return color;
}

// ----------------------------------------------------------------------------

void ImageWindow::SetTransparencyMode( ImageWindow::transparency_mode mode, RGBA color )
{
   (*API->ImageWindow->SetImageWindowTransparencyMode)( handle, mode, color );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetViewport( double cx, double cy, int zoom )
{
   (*API->ImageWindow->SetImageWindowViewport)( handle, cx, cy, zoom );
}

// ----------------------------------------------------------------------------

void ImageWindow::FitWindow()
{
   (*API->ImageWindow->FitImageWindow)( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::ZoomToFit( bool allowMagnification )
{
   (*API->ImageWindow->ZoomImageWindowToFit)( handle, allowMagnification );
}

// ----------------------------------------------------------------------------

int ImageWindow::ZoomFactor() const
{
   return (*API->ImageWindow->GetImageWindowZoomFactor)( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetZoomFactor( int z )
{
   (*API->ImageWindow->SetImageWindowZoomFactor)( handle, z );
}

// ----------------------------------------------------------------------------

void ImageWindow::GetViewportSize( int& width, int& height ) const
{
   (*API->ImageWindow->GetImageWindowViewportSize)( handle, &width, &height );
}

// ----------------------------------------------------------------------------

Point ImageWindow::ViewportPosition() const
{
   Point p;
   (*API->ImageWindow->GetImageWindowViewportPosition)( handle, &p.x, &p.y );
   return p;
}

// ----------------------------------------------------------------------------

void ImageWindow::SetViewportPosition( int x, int y )
{
   (*API->ImageWindow->SetImageWindowViewportPosition)( handle, x, y );
}

// ----------------------------------------------------------------------------

Rect ImageWindow::VisibleViewportRect() const
{
   Rect r;
   (*API->ImageWindow->GetImageWindowVisibleViewportRect)( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsVisible() const
{
   return (*API->ImageWindow->GetImageWindowVisible)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::Show()
{
   (*API->ImageWindow->SetImageWindowVisible)( handle, api_true );
}

// ----------------------------------------------------------------------------

void ImageWindow::Hide()
{
   (*API->ImageWindow->SetImageWindowVisible)( handle, api_false );
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsIconic() const
{
   return (*API->ImageWindow->GetImageWindowIconic)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::Iconize()
{
   (*API->ImageWindow->SetImageWindowIconic)( handle, api_true );
}

// ----------------------------------------------------------------------------

void ImageWindow::Deiconize()
{
   (*API->ImageWindow->SetImageWindowIconic)( handle, api_false );
}

// ----------------------------------------------------------------------------

void ImageWindow::BringToFront()
{
   (*API->ImageWindow->BringImageWindowToFront)( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::SendToBack()
{
   (*API->ImageWindow->SendImageWindowToBack)( handle );
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsDynamicSessionActive()
{
   return (*API->ImageWindow->GetActiveDynamicInterface)() != 0;
}

// ----------------------------------------------------------------------------

ProcessInterface* ImageWindow::ActiveDynamicInterface()
{
   return reinterpret_cast<ProcessInterface*>( (*API->ImageWindow->GetActiveDynamicInterface)() );
}

// ----------------------------------------------------------------------------

bool ImageWindow::TerminateDynamicSession( bool closeInterface )
{
   return (*API->ImageWindow->TerminateDynamicSession)( closeInterface ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::SetDynamicCursor( const char** xpm, int hx, int hy )
{
   (*API->ImageWindow->SetDynamicCursorXPM)( handle, xpm, hx, hy );
}

// ----------------------------------------------------------------------------

void ImageWindow::SetDynamicCursor( const Bitmap& bmp, int hx, int hy )
{
   (*API->ImageWindow->SetDynamicCursor)( handle, bmp.handle, hx, hy );
}

// ----------------------------------------------------------------------------

Bitmap ImageWindow::DynamicCursorBitmap() const
{
   return Bitmap( (*API->ImageWindow->GetDynamicCursorBitmap)( handle ) );
}

// ----------------------------------------------------------------------------

Point ImageWindow::DynamicCursorHotSpot() const
{
   Point p( 0 );
   (*API->ImageWindow->GetDynamicCursorHotSpot)( handle, &p.x, &p.y );
   return p;
}

// ----------------------------------------------------------------------------

double ImageWindow::DisplayPixelRatio() const
{
   double r;
   if ( (*API->ImageWindow->GetImageWindowDisplayPixelRatio)( handle, &r ) == api_false )
      throw APIFunctionError( "GetImageWindowDisplayPixelRatio" );
   return r;
}

// ----------------------------------------------------------------------------

void ImageWindow::ViewportToImage( int& x, int& y ) const
{
   (*API->ImageWindow->ViewportToImage)( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageWindow::ViewportToImage( double& x, double& y ) const
{
   (*API->ImageWindow->ViewportToImageD)( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageWindow::ViewportToImage( Point* p, size_type n ) const
{
   (*API->ImageWindow->ViewportToImageArray)( handle, reinterpret_cast<int32*>( p ), n );
}

// ----------------------------------------------------------------------------

void ImageWindow::ViewportToImage( DPoint* p, size_type n ) const
{
   (*API->ImageWindow->ViewportToImageArrayD)( handle, reinterpret_cast<double*>( p ), n );
}

// ----------------------------------------------------------------------------

void ImageWindow::ViewportScalarToImage( int* d, size_type n ) const
{
   (*API->ImageWindow->ViewportScalarToImageArray)( handle, d, n );
}

// ----------------------------------------------------------------------------

void ImageWindow::ViewportScalarToImage( double* d, size_type n ) const
{
   (*API->ImageWindow->ViewportScalarToImageArrayD)( handle, d, n );
}

// ----------------------------------------------------------------------------

double ImageWindow::ViewportScalarToImage( double d ) const
{
   (*API->ImageWindow->ViewportScalarToImageD)( handle, &d );
   return d;
}

// ----------------------------------------------------------------------------

void ImageWindow::ImageToViewport( int& x, int& y ) const
{
   (*API->ImageWindow->ImageToViewport)( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageWindow::ImageToViewport( double& x, double& y ) const
{
   (*API->ImageWindow->ImageToViewportD)( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageWindow::ImageToViewport( Point* p, size_type n ) const
{
   (*API->ImageWindow->ImageToViewportArray)( handle, reinterpret_cast<int32*>( p ), n );
}

// ----------------------------------------------------------------------------

void ImageWindow::ImageToViewport( DPoint* p, size_type n ) const
{
   (*API->ImageWindow->ImageToViewportArrayD)( handle, reinterpret_cast<double*>( p ), n );
}

// ----------------------------------------------------------------------------

void ImageWindow::ImageScalarToViewport( int* d , size_type n ) const
{
   (*API->ImageWindow->ImageScalarToViewportArray)( handle, d, n );
}

// ----------------------------------------------------------------------------

void ImageWindow::ImageScalarToViewport( double* d , size_type n ) const
{
   (*API->ImageWindow->ImageScalarToViewportArrayD)( handle, d, n );
}

// ----------------------------------------------------------------------------

int ImageWindow::ImageScalarToViewport( int d ) const
{
   (*API->ImageWindow->ImageScalarToViewport)( handle, &d );
   return d;
}

// ----------------------------------------------------------------------------

double ImageWindow::ImageScalarToViewport( double d ) const
{
   (*API->ImageWindow->ImageScalarToViewportD)( handle, &d );
   return d;
}

// ----------------------------------------------------------------------------

void ImageWindow::ViewportToGlobal( int& x, int& y ) const
{
   (*API->ImageWindow->ViewportToGlobal)( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageWindow::GlobalToViewport( int& x, int& y ) const
{
   (*API->ImageWindow->GlobalToViewport)( handle, &x, &y );
}

// ----------------------------------------------------------------------------

void ImageWindow::Regenerate()
{
   (*API->ImageWindow->RegenerateImageWindowViewport)( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::RegenerateViewportRect( int x0, int y0, int x1, int y1 )
{
   (*API->ImageWindow->RegenerateViewportRect)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void ImageWindow::RegenerateImageRect( double x0, double y0, double x1, double y1 )
{
   (*API->ImageWindow->RegenerateImageRect)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void ImageWindow::UpdateViewport()
{
   (*API->ImageWindow->UpdateImageWindowViewport)( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::UpdateViewportRect( int x0, int y0, int x1, int y1 )
{
   (*API->ImageWindow->UpdateViewportRect)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

void ImageWindow::UpdateImageRect( double x0, double y0, double x1, double y1 )
{
   (*API->ImageWindow->UpdateImageRect)( handle, x0, y0, x1, y1 );
}

// ----------------------------------------------------------------------------

bool ImageWindow::HasPendingUpdates() const
{
   return (*API->ImageWindow->GetViewportUpdateRect)( handle, 0, 0, 0, 0 ) != api_false;
}

// ----------------------------------------------------------------------------

void ImageWindow::CommitPendingUpdates()
{
   (*API->ImageWindow->CommitViewportUpdates)( handle );
}

// ----------------------------------------------------------------------------

Rect ImageWindow::ViewportUpdateRect() const
{
   Rect r;
   (*API->ImageWindow->GetViewportUpdateRect)( handle, &r.x0, &r.y0, &r.x1, &r.y1 );
   return r;
}

// ----------------------------------------------------------------------------

Bitmap ImageWindow::ViewportBitmap( int x0, int y0, int x1, int y1, uint32 flags ) const
{
   return Bitmap( (*API->ImageWindow->GetViewportBitmap)( ModuleHandle(), handle, x0, y0, x1, y1, 0 ) );
}

// ----------------------------------------------------------------------------

void ImageWindow::BeginSelection( int x, int y, uint32 flags )
{
   (*API->ImageWindow->BeginViewportSelection)( handle, x, y, flags );
}

// ----------------------------------------------------------------------------

void ImageWindow::ModifySelection( int x, int y, uint32 flags )
{
   (*API->ImageWindow->ModifyViewportSelection)( handle, x, y, flags );
}

// ----------------------------------------------------------------------------

void ImageWindow::UpdateSelection()
{
   (*API->ImageWindow->UpdateViewportSelection)( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::CancelSelection()
{
   (*API->ImageWindow->CancelViewportSelection)( handle );
}

// ----------------------------------------------------------------------------

void ImageWindow::EndSelection()
{
   (*API->ImageWindow->EndViewportSelection)( handle );
}

// ----------------------------------------------------------------------------

pcl::Rect ImageWindow::SelectionRect( uint32* flags ) const
{
   pcl::Rect r;
   (*API->ImageWindow->GetViewportSelection)( handle, &r.x0, &r.y0, &r.x1, &r.y1, flags );
   return r;
}

// ----------------------------------------------------------------------------

bool ImageWindow::IsSelection() const
{
   return (*API->ImageWindow->GetViewportSelection)( handle, 0, 0, 0, 0, 0 ) != api_false;
}

// ----------------------------------------------------------------------------

ImageWindow ImageWindow::WindowById( const IsoString& id )
{
   return ImageWindow( (*API->ImageWindow->GetImageWindowById)( id.c_str() ) );
}

// ----------------------------------------------------------------------------

ImageWindow ImageWindow::WindowByFilePath( const String& filePath )
{
   return ImageWindow( (*API->ImageWindow->GetImageWindowByFilePath)( filePath.c_str() ) );
}

// ----------------------------------------------------------------------------

ImageWindow ImageWindow::ActiveWindow()
{
   return ImageWindow( (*API->ImageWindow->GetActiveImageWindow)() );
}

// ----------------------------------------------------------------------------

Array<ImageWindow> ImageWindow::AllWindows( bool includeIconicWindows )
{
   Array<ImageWindow> a;
   (*API->ImageWindow->EnumerateImageWindows)(
               InternalWindowEnumerator::Callback, &a, includeIconicWindows );
   return a;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ImageWindow.cpp - Released 2018-11-01T11:06:52Z
