//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard BMP File Format Module Version 01.00.03.0266
// ----------------------------------------------------------------------------
// BMPInstance.cpp - Released 2015/11/26 15:59:58 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard BMP PixInsight module.
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

#include "BMPInstance.h"
#include "BMPFormat.h"

#include <pcl/Bitmap.h>

namespace pcl
{

// ----------------------------------------------------------------------------

BitmapInstance::BitmapInstance( const MetaFileFormat* F ) : FileFormatImplementation( F ), bitmap( 0 ), path(), info()
{
}

BitmapInstance::~BitmapInstance()
{
   Close();
}

static void __GetBitmapInfo( int& width, int& height, bool& grayscale, bool& alpha, const Bitmap* bmp )
{
   bmp->GetDimensions( width, height );
   grayscale = true;
   alpha = false;

   for ( int i = 0; i < height; ++i )
   {
      const RGBA* p = bmp->ScanLine( i );
      const RGBA* p1 = p + width;
      for ( ;; )
      {
         RGBA rgba = *p;

         if ( !alpha && (rgba & 0xff000000) != 0xff000000 )
         {
            alpha = true;
            if ( !grayscale )
               return;
         }

         if ( grayscale )
         {
            uint8 red = Red( rgba );
            uint8 green = Green( rgba );
            uint8 blue = Blue( rgba );

            if ( red != green || red != blue || green != blue )
            {
               grayscale = false;
               if ( alpha )
                  return;
            }
         }

         if ( ++p == p1 )
            break;
      }
   }
}

ImageDescriptionArray BitmapInstance::Open( const String& filePath, const IsoString& /*hints*/ )
{
   Close();

   bitmap = new Bitmap( path = filePath );

   if ( bitmap->IsNull() || bitmap->IsEmpty() )
   {
      Close();
      throw Error( "Error reading bitmap" );
   }

   bool grayscale, alpha;
   __GetBitmapInfo( info.width, info.height, grayscale, alpha, bitmap );

   info.numberOfChannels = (grayscale ? 1 : 3) + (alpha ? 1 : 0);
   info.colorSpace = grayscale ? ColorSpace::Gray : ColorSpace::RGB;
   info.supported = true;

   ImageDescription d;

   d.info = info;

   d.options.bitsPerSample = 8;
   d.options.embedICCProfile= false;
   d.options.embedThumbnail = false;

   ImageDescriptionArray a;
   a.Add( d );
   return a;
}

bool BitmapInstance::IsOpen() const
{
   return bitmap != 0;
}

String BitmapInstance::FilePath() const
{
   return path;
}

void BitmapInstance::Create( const String& filePath, int /*numberOfImages*/, const IsoString& /*hints*/ )
{
   Close();

   path = filePath;

   bitmap = new Bitmap; // this is just to "seem open"; we'll create a true bitmap upon image write
   if ( bitmap->IsNull() )
   {
      Close();
      throw Error( "Unable to create bitmap" );
   }
}

void BitmapInstance::Close()
{
   if ( bitmap != 0 )
      delete bitmap, bitmap = 0;
}

// ----------------------------------------------------------------------------

template <class P>
static void ReadImageImp( GenericImage<P>& image, const Bitmap* bmp, const ImageInfo& info )
{
   if ( bmp == 0 || bmp->IsNull() )
      throw Error( "Attempt to read bitmap before opening a file" );

   image.AllocateData( info.width, info.height, info.numberOfChannels, ColorSpace::value_type( info.colorSpace ) );
   int n = image.NumberOfNominalChannels();

   for ( int y = 0; y < image.Height(); ++y )
   {
      const RGBA* b = bmp->ScanLine( y );

      typename P::sample* p[ 4 ];
      for ( int c = 0; c < image.NumberOfChannels(); ++c )
         p[c] = image.ScanLine( y, c );

      for ( int j = 0; j < image.Width(); ++j, ++b )
      {
         for ( int c = 0, s = 16; c < n; ++c, s -= 8 )
            UInt8PixelTraits::FromSample( *p[c]++, uint8( *b >> s ) );
         if ( image.HasAlphaChannels() )
            UInt8PixelTraits::FromSample( *p[n]++, uint8( *b >> 24 ) );
      }
   }
}

void BitmapInstance::ReadImage( Image& img )
{
   ReadImageImp( img, bitmap, info );
}

void BitmapInstance::ReadImage( DImage& img )
{
   ReadImageImp( img, bitmap, info );
}

void BitmapInstance::ReadImage( UInt8Image& img )
{
   ReadImageImp( img, bitmap, info );
}

void BitmapInstance::ReadImage( UInt16Image& img )
{
   ReadImageImp( img, bitmap, info );
}

void BitmapInstance::ReadImage( UInt32Image& img )
{
   ReadImageImp( img, bitmap, info );
}

// ----------------------------------------------------------------------------

template <class P>
static void WriteImageImp( const String& path, Bitmap*& bmp, const GenericImage<P>& img )
{
   if ( path.IsEmpty() || bmp == 0 )
      throw Error( "Attempt to write bitmap before creating a file" );

   if ( bmp != 0 )
      delete bmp, bmp = 0;
   ImageVariant v( &const_cast<GenericImage<P>&>( img ) );
   bmp = new Bitmap( Bitmap::Render( v ) );
   bmp->Save( path );
}

void BitmapInstance::WriteImage( const Image& img )
{
   WriteImageImp( path, bitmap, img );
}

void BitmapInstance::WriteImage( const DImage& img )
{
   WriteImageImp( path, bitmap, img );
}

void BitmapInstance::WriteImage( const UInt8Image& img )
{
   WriteImageImp( path, bitmap, img );
}

void BitmapInstance::WriteImage( const UInt16Image& img )
{
   WriteImageImp( path, bitmap, img );
}

void BitmapInstance::WriteImage( const UInt32Image& img )
{
   WriteImageImp( path, bitmap, img );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF BMPInstance.cpp - Released 2015/11/26 15:59:58 UTC
