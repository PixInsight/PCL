//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// pcl/Bitmap.cpp - Released 2015/10/08 11:24:19 UTC
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

#include <pcl/AutoLock.h>
#include <pcl/Bitmap.h>
#include <pcl/ImageVariant.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

Bitmap::Bitmap() :
   UIObject( (*API->Bitmap->CreateEmptyBitmap)( ModuleHandle() ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateEmptyBitmap" );
}

// ----------------------------------------------------------------------------

Bitmap::Bitmap( int w, int h, pixel_format fmt ) :
   UIObject( (*API->Bitmap->CreateBitmap)( ModuleHandle(), w, h, 0 ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateBitmap" );
   if ( fmt != BitmapFormat::ARGB32 )
      (*API->Bitmap->SetBitmapFormat)( handle, fmt );
}

// ----------------------------------------------------------------------------

Bitmap::Bitmap( const char** xpm ) :
   UIObject( (*API->Bitmap->CreateBitmapXPM)( ModuleHandle(), xpm ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateBitmapXPM" );
}

// ----------------------------------------------------------------------------

Bitmap::Bitmap( const String& filePath ) :
   UIObject( (*API->Bitmap->CreateBitmapFromFile)( ModuleHandle(), filePath.c_str() ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateBitmapFromFile" );
}

// ----------------------------------------------------------------------------

Bitmap::Bitmap( const char* filePath ) :
   UIObject( (*API->Bitmap->CreateBitmapFromFile8)( ModuleHandle(), filePath ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateBitmapFromFile8" );
}

// ----------------------------------------------------------------------------

Bitmap::Bitmap( const IsoString& filePath ) :
   UIObject( (*API->Bitmap->CreateBitmapFromFile8)( ModuleHandle(), filePath.c_str() ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateBitmapFromFile8" );
}

// ----------------------------------------------------------------------------

Bitmap::Bitmap( const void* data, size_type size, const char* format, uint32 flags ) :
   UIObject( (*API->Bitmap->CreateBitmapFromData)( ModuleHandle(), data, size, format, flags ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateBitmapFromData" );
}

// ----------------------------------------------------------------------------

Bitmap::Bitmap( const Bitmap& bmp, const pcl::Rect& r ) :
   UIObject( (*API->Bitmap->CloneBitmapRect)( ModuleHandle(), bmp.handle, r.x0, r.y0, r.x1, r.y1 ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CloneBitmapRect" );
}

// ----------------------------------------------------------------------------

Bitmap::Bitmap( const Bitmap& bmp, int x0, int y0, int x1, int y1 ) :
   UIObject( (*API->Bitmap->CloneBitmapRect)( ModuleHandle(), bmp.handle, x0, y0, x1, y1 ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CloneBitmapRect" );
}

// ----------------------------------------------------------------------------

Bitmap& Bitmap::Null()
{
   static Bitmap* nullBitmap = nullptr;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullBitmap == nullptr )
      nullBitmap = new Bitmap( reinterpret_cast<void*>( 0 ) );
   return *nullBitmap;
}

// ----------------------------------------------------------------------------

Bitmap::pixel_format Bitmap::PixelFormat() const
{
   return pixel_format( (*API->Bitmap->GetBitmapFormat)( handle ) );
}

// ----------------------------------------------------------------------------

void Bitmap::SetPixelFormat( pixel_format fmt )
{
   (*API->Bitmap->SetBitmapFormat)( handle, fmt );
}

// ----------------------------------------------------------------------------

// Implemented in Render.cpp
extern bool Render( Bitmap& bitmap, int x, int y, int zoom, int channel,
                    const pcl::ImageVariant& image,  bool transparency,
                    const pcl::ImageVariant* mask, int maskMode, bool maskInverted,
                    const uint8** LUT, bool fast, bool (*callback)() );

Bitmap Bitmap::Render( const ImageVariant& image, int zoom, display_channel displayChannel, bool transparency,
                       const ImageVariant* mask, mask_mode maskMode, bool maskInverted,
                       const uint8** LUT, bool fast, bool (*callback)() )
{
   if ( !image || image->IsEmptySelection() )
      return Null();

   zoom = pcl::Range( zoom, -32, +100 );
   if ( zoom == 0 )
      zoom = 1;

   if ( mask != 0 &&
         (!*mask ||
            mask->Width() != image->Width() ||
            mask->Height() != image->Height() ||
            mask->IsColor() && !image->IsColor()) )
   {
      mask = 0;
   }

   pcl::Rect r = image->SelectedRectangle();
   int w = r.Width();
   int h = r.Height();

   if ( zoom > 0 )
      w *= zoom, h *= zoom;
   else
      w /= -zoom, h /= -zoom;

   Bitmap bmp( pcl::Max( 1, w ), pcl::Max( 1, h ) );

   if ( !pcl::Render( bmp, 0, 0, zoom, displayChannel, image, transparency,
                      mask, maskMode, maskInverted, LUT, fast, callback ) )
      return Null();

   return bmp;
}

// ----------------------------------------------------------------------------

void Bitmap::GetDimensions( int& w, int& h ) const
{
   if ( handle == 0 )
   {
      w = h = 0;
      return;
   }

   if ( (*API->Bitmap->GetBitmapDimensions)( handle, &w, &h ) == api_false )
      throw APIFunctionError( "GetBitmapDimensions" );
}

// ----------------------------------------------------------------------------

bool Bitmap::IsEmpty() const
{
   return handle == 0 || (*API->Bitmap->IsEmptyBitmap)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

RGBA Bitmap::Pixel( int x, int y ) const
{
   return (*API->Bitmap->GetBitmapPixel)( handle, x, y );
}

// ----------------------------------------------------------------------------

void Bitmap::SetPixel( int x, int y, RGBA v )
{
   EnsureUnique();
   (*API->Bitmap->SetBitmapPixel)( handle, x, y, v );
}

// ----------------------------------------------------------------------------

const RGBA* Bitmap::ScanLine( int i ) const
{
   return (*API->Bitmap->GetBitmapScanLine)( handle, i );
}

// ----------------------------------------------------------------------------

RGBA* Bitmap::ScanLine( int i )
{
   EnsureUnique();
   return (*API->Bitmap->GetBitmapScanLine)( handle, i );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::Mirrored() const
{
   return Bitmap( (*API->Bitmap->MirroredBitmap)( handle, api_true, api_true ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::MirroredHorizontally() const
{
   return Bitmap( (*API->Bitmap->MirroredBitmap)( handle, api_true, api_false ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::MirroredVertically() const
{
   return Bitmap( (*API->Bitmap->MirroredBitmap)( handle, api_false, api_true ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::Scaled( double sx, double sy, bool precise ) const
{
   int w, h; GetDimensions( w, h );
   w = pcl::RoundI( sx*w ); h = pcl::RoundI( sy*h );
   return Bitmap( (*API->Bitmap->ScaledBitmap)( handle, w, h, precise ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::Scaled( double sxy, bool precise ) const
{
   int w, h; GetDimensions( w, h );
   w = pcl::RoundI( sxy*w ); h = pcl::RoundI( sxy*h );
   return Bitmap( (*API->Bitmap->ScaledBitmap)( handle, w, h, precise ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::ScaledToWidth( int w, bool precise ) const
{
   int dum, h; GetDimensions( dum, h );
   return Bitmap( (*API->Bitmap->ScaledBitmap)( handle, w, h, precise ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::ScaledToHeight( int h, bool precise ) const
{
   int w, dum; GetDimensions( w, dum );
   return Bitmap( (*API->Bitmap->ScaledBitmap)( handle, w, h, precise ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::ScaledToSize( int w, int h, bool precise ) const
{
   return Bitmap( (*API->Bitmap->ScaledBitmap)( handle, w, h, precise ) );
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::Rotated( double angleRadians, bool precise ) const
{
   return Bitmap( (*API->Bitmap->RotatedBitmap)( handle, angleRadians, precise ) );
}

// ----------------------------------------------------------------------------

bool Bitmap::Load( const String& fileName )
{
   if ( !IsUnique() )
      SetHandle( (*API->Bitmap->CreateEmptyBitmap)( ModuleHandle() ) );
   return (*API->Bitmap->LoadBitmap)( handle, fileName.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

bool Bitmap::Save( const String& fileName, int quality ) const
{
   return (*API->Bitmap->SaveBitmap)( handle, fileName.c_str(), quality ) != api_false;
}

// ----------------------------------------------------------------------------

bool Bitmap::Load( const void* data, size_type size, const char* format, uint32 flags )
{
   return (*API->Bitmap->LoadBitmapData)( handle, data, size, format, flags ) != api_false;
}

// ----------------------------------------------------------------------------

Bitmap Bitmap::Subimage( int x0, int y0, int x1, int y1 ) const
{
   return Bitmap( (*API->Bitmap->CloneBitmapRect)( ModuleHandle(), handle, x0, y0, x1, y1 ) );
}

// ----------------------------------------------------------------------------

void Bitmap::Copy( const pcl::Point& p, const Bitmap& src, const pcl::Rect& r )
{
   EnsureUnique();
   (*API->Bitmap->CopyBitmap)( handle, p.x, p.y, src.handle, r.x0, r.y0, r.x1, r.y1 );
}

// ----------------------------------------------------------------------------

void Bitmap::Fill( const pcl::Rect& r, RGBA fillWith )
{
   EnsureUnique();
   (*API->Bitmap->FillBitmap)( handle, r.x0, r.y0, r.x1, r.y1, fillWith );
}

// ----------------------------------------------------------------------------

void Bitmap::Or( const pcl::Rect& r, RGBA orWith )
{
   EnsureUnique();
   (*API->Bitmap->OrBitmap)( handle, r.x0, r.y0, r.x1, r.y1, orWith );
}

// ----------------------------------------------------------------------------

void Bitmap::Or( const pcl::Point& p, const Bitmap& src, const pcl::Rect& r )
{
   EnsureUnique();
   (*API->Bitmap->OrBitmaps)( handle, p.x, p.y, src.handle, r.x0, r.y0, r.x1, r.y1 );
}

// ----------------------------------------------------------------------------

void Bitmap::And( const pcl::Rect& r, RGBA andWith )
{
   EnsureUnique();
   (*API->Bitmap->AndBitmap)( handle, r.x0, r.y0, r.x1, r.y1, andWith );
}

// ----------------------------------------------------------------------------

void Bitmap::And( const pcl::Point& p, const Bitmap& src, const pcl::Rect& r )
{
   EnsureUnique();
   (*API->Bitmap->AndBitmaps)( handle, p.x, p.y, src.handle, r.x0, r.y0, r.x1, r.y1 );
}

// ----------------------------------------------------------------------------

void Bitmap::Xor( const pcl::Rect& r, RGBA xorWith )
{
   EnsureUnique();
   (*API->Bitmap->XorBitmap)( handle, r.x0, r.y0, r.x1, r.y1, xorWith );
}

// ----------------------------------------------------------------------------

void Bitmap::Xor( const pcl::Point& p, const Bitmap& src, const pcl::Rect& r )
{
   EnsureUnique();
   (*API->Bitmap->XorBitmaps)( handle, p.x, p.y, src.handle, r.x0, r.y0, r.x1, r.y1 );
}

// ----------------------------------------------------------------------------

void Bitmap::XorRect( const pcl::Rect& r, RGBA xorWith )
{
   EnsureUnique();
   (*API->Bitmap->XorBitmapRect)( handle, r.x0, r.y0, r.x1, r.y1, xorWith );
}

// ----------------------------------------------------------------------------

void Bitmap::ReplaceColor( const pcl::Rect& r, RGBA replaceThis, RGBA replaceWith )
{
   EnsureUnique();
   (*API->Bitmap->ReplaceBitmapColor)( handle, r.x0, r.y0, r.x1, r.y1, replaceThis, replaceWith );
}

// ----------------------------------------------------------------------------

void Bitmap::SetAlpha( const pcl::Rect& r, uint8 alpha )
{
   EnsureUnique();
   (*API->Bitmap->SetBitmapAlpha)( handle, r.x0, r.y0, r.x1, r.y1, alpha );
}

// ----------------------------------------------------------------------------

void* Bitmap::CloneHandle() const
{
   return (*API->Bitmap->CloneBitmap)( ModuleHandle(), handle );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Bitmap.cpp - Released 2015/10/08 11:24:19 UTC
