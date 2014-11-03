// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard TIFF File Format Module Version 01.00.05.0227
// ****************************************************************************
// TIFF.cpp - Released 2014/10/29 07:34:49 UTC
// ****************************************************************************
// This file is part of the standard TIFF PixInsight module.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#include "TIFF.h"

#include <pcl/ErrorHandler.h>
#include <pcl/MessageBox.h>
#include <pcl/Console.h>

#include <libtiff/tiffio.h>

namespace pcl
{

// ----------------------------------------------------------------------------

// Default TIFF strip size in bytes.
static const size_type defaultStripSize = 4096;

// Maximum size of a TIFF strip in bytes.
// The default strip size is 4 KB. Too low or high sizes can affect performance.
static size_type stripSize = defaultStripSize;

// Whether to abort file open operations on libtiff errors.
static bool strictMode = false;

// Whether to show libtiff's warnings.
static bool showWarnings = false;

// ----------------------------------------------------------------------------

/*
 * TIFF errors are necessarily fatal only in strict mode.
 * In permissive mode, we only show error messages on the console.
 */
static void __TIFFError( const char* module, const char* fmt, va_list args )
{
   String msg; (void)msg.VFormat( fmt, args );
   if ( strictMode )
      throw Error( String( "TIFF Error: " ) + module + ": " + msg );
   Console().CriticalLn( "<end><cbr>*** Error: TIFF: " + msg );
}

/*
 * TIFF warnings are shown (console only) when warning messages are enabled.
 */
static void __TIFFWarning( const char* module, const char* fmt, va_list args )
{
   if ( showWarnings )
   {
      String msg; (void)msg.VFormat( fmt, args );
      Console().WarningLn( "<end><cbr>** Warning: TIFF: " + msg );
   }
}

// ----------------------------------------------------------------------------

struct TIFFFileData
{
   //void*    handle; // libtiff's ::TIFF*
   ::TIFF*  handle;
   uint32   width;
   uint32   length;
   uint16   bitsPerSample;
   uint16   sampleFormat;
   uint16   samplesPerPixel;
   uint16   photometric;
   uint16   planarConfig;

   double   lowerRange; // safe copies of critical parameters
   double   upperRange;

   TIFFFileData() : handle( 0 )
   {
      static bool firstTime = true;

      if ( firstTime )
      {
         ::TIFFSetErrorHandler( __TIFFError );
         ::TIFFSetWarningHandler( __TIFFWarning );
         firstTime = false;
      }
   }

   ~TIFFFileData()
   {
      if ( handle != 0 )
         ::TIFFClose( (::TIFF*)handle ), handle = 0;
   }
};

// ----------------------------------------------------------------------------

void TIFF::__Close() // ### derived must call base
{
   // Close TIFF file stream and clean up libtiff data.
   if ( fileData != 0 )
      delete fileData, fileData = 0;
}

// ----------------------------------------------------------------------------

#define info                  ((*images).info)
#define options               ((*images).options)

#define tif                   (fileData->handle)
#define tiffWidth             (fileData->width)
#define tiffLength            (fileData->length)
#define tiffBitsPerSample     (fileData->bitsPerSample)
#define tiffSampleFormat      (fileData->sampleFormat)
#define tiffSamplesPerPixel   (fileData->samplesPerPixel)
#define tiffPhotometric       (fileData->photometric)
#define tiffPlanarConfig      (fileData->planarConfig)

// ----------------------------------------------------------------------------

bool TIFFReader::IsOpen() const
{
   return fileData != 0 && fileData->handle != 0;
}

void TIFFReader::Close()
{
   TIFF::__Close();
}

void TIFFReader::Open( const String& _path )
{
   if ( IsOpen() )
      throw InvalidReadOperation( path );

   if ( _path.IsEmpty() )
      throw InvalidFilePath( _path );

   if ( options.lowerRange >= options.upperRange )
      throw InvalidNormalizationRange( _path );

   info.Reset();

   try
   {
      fileData = new TIFFFileData;
#ifdef __PCL_WINDOWS
      path = File::WindowsPathToUnix( _path );
#else
      path = _path;
#endif

      // Keep critical parameters secured in private data.
      fileData->lowerRange = options.lowerRange;
      fileData->upperRange = options.upperRange;

      //
      // Open the TIFF image file stream for reading.
      //
      IsoString path8 =
#ifdef __PCL_WINDOWS
         File::UnixPathToWindows( path ).ToMBS();
#else
         path.ToUTF8();
#endif
      if ( (tif = ::TIFFOpen( path8.c_str(), "r" )) == 0 )
         throw UnableToOpenFile( path );

      //
      // Retrieve TIFF image information --------------------------------------
      //

      // Mandatory fields.
      ::TIFFGetField( tif, TIFFTAG_IMAGEWIDTH, &tiffWidth );
      ::TIFFGetField( tif, TIFFTAG_IMAGELENGTH, &tiffLength );
      ::TIFFGetField( tif, TIFFTAG_BITSPERSAMPLE, &tiffBitsPerSample );
      ::TIFFGetField( tif, TIFFTAG_SAMPLESPERPIXEL, &tiffSamplesPerPixel );
      ::TIFFGetField( tif, TIFFTAG_PLANARCONFIG, &tiffPlanarConfig );

      // Update ImageInfo.
      info.width = tiffWidth;
      info.height = tiffLength;
      info.numberOfChannels = tiffSamplesPerPixel;

      // Photometric mode = color space of the target Image object.
      ::TIFFGetField( tif, TIFFTAG_PHOTOMETRIC, &tiffPhotometric );

      // We only support the grayscale and RGB color photometric modes.
      switch ( tiffPhotometric )
      {
      case PHOTOMETRIC_MINISBLACK:
      case PHOTOMETRIC_MINISWHITE:
         info.colorSpace = ColorSpace::Gray;
         break;

      case PHOTOMETRIC_RGB:
         info.colorSpace = ColorSpace::RGB;
         break;

      default:
         info.colorSpace = ColorSpace::Unknown;
         throw UnsupportedColorSpace( path );
      }

      // The SampleFormat field is optional. If not present, we assume the
      // sample format is unsigned 16-bit integer. This is in conformance with
      // baseline TIFF 6.0.
      if ( !::TIFFGetField( tif, TIFFTAG_SAMPLEFORMAT, &tiffSampleFormat ) )
         tiffSampleFormat = SAMPLEFORMAT_UINT; // assume unsigned integers

      // Supported pixel depths: 8, 16, 32 and 64 bits.
      if ( tiffBitsPerSample != 8 &&
           tiffBitsPerSample != 16 &&
           tiffBitsPerSample != 32 &&
           tiffBitsPerSample != 64 )
      {
         throw UnsupportedBitsPerSample( path );
      }

      // Supported sample formats:
      //    8, 16 and 32 bits unsigned integers -> sample format = 1
      //    8, 16 and 32 bits signed integers -> sample format = 2
      //    32 and 64 bits IEEE floating point -> sample format = 3
      //    void (will be treated as unsigned integer) -> sample format = 4
      if ( tiffSampleFormat != SAMPLEFORMAT_UINT &&
           tiffSampleFormat != SAMPLEFORMAT_INT &&
           tiffSampleFormat != SAMPLEFORMAT_IEEEFP &&
           tiffSampleFormat != SAMPLEFORMAT_VOID )
      {
         throw UnsupportedSampleFormat( path );
      }

      // Treat unknown sample format as unsigned integer.
      if ( tiffSampleFormat == SAMPLEFORMAT_VOID )
         tiffSampleFormat = SAMPLEFORMAT_UINT;

      // Reject bizarre things, such as 64-bit integers, or impossible reals.
      if ( tiffBitsPerSample == 64 && tiffSampleFormat != SAMPLEFORMAT_IEEEFP ||
           tiffBitsPerSample < 32 && tiffSampleFormat == SAMPLEFORMAT_IEEEFP )
      {
         throw UnsupportedSampleFormat( path );
      }

      // If reached this point, then hopefully this TIFF image is valid for us.
      info.supported = true;

      //
      // Retrieve optional TIFF information -----------------------------------
      //

      uint16 tiffUInt16;
      uint32 tiffUInt32;
      float  tiffFloat;
      char*  tiffStr;

      // Sample format.
      options.bitsPerSample = tiffBitsPerSample;
      options.ieeefpSampleFormat = tiffSampleFormat == SAMPLEFORMAT_IEEEFP;
      options.signedIntegers = options.complexSample = false;

      // Compression.

      if ( ::TIFFGetField( tif, TIFFTAG_COMPRESSION, &tiffUInt16 ) )
      {
         switch ( tiffUInt16 )
         {
         case COMPRESSION_NONE:
            tiffOptions.compression = TIFFCompression::None;
            break;
         case COMPRESSION_DEFLATE:
            tiffOptions.compression = TIFFCompression::ZIP;
            break;
         case COMPRESSION_LZW:
            tiffOptions.compression = TIFFCompression::LZW;
            break;
         default:
            tiffOptions.compression = TIFFCompression::Unknown;
            break;
         }
      }
      else
         tiffOptions.compression = TIFFCompression::None;

      // Planar configuration.
      tiffOptions.planar = ::TIFFGetField( tif, TIFFTAG_PLANARCONFIG, &tiffUInt16 ) &&
                           tiffUInt16 == PLANARCONFIG_SEPARATE;

      // Resolution units: metric (centimeters) or English (inches).
      options.metricResolution = ::TIFFGetField( tif, TIFFTAG_RESOLUTIONUNIT, &tiffUInt16 ) &&
                           tiffUInt16 == RESUNIT_CENTIMETER;

      // TIFF specifies a separate resolution for each axis.
      // Resolution values are stored as float values in the TIFF file, so we
      // need a temporary float variable.

      if ( ::TIFFGetField( tif, TIFFTAG_XRESOLUTION, &tiffFloat ) )
         options.xResolution = tiffFloat;
      else
         options.xResolution = 0;

      if ( ::TIFFGetField( tif, TIFFTAG_YRESOLUTION, &tiffFloat ) )
         options.yResolution = tiffFloat;
      else
         options.yResolution = 0;

      // Some programs out there don't write correct resolution data.
      if ( options.xResolution <= 0 || options.yResolution <= 0 )
         options.xResolution = options.yResolution = options.metricResolution ? 72/2.54 : 72;

      // Extra samples information

      tiffOptions.premultipliedAlpha = false;

      if ( info.numberOfChannels > ((info.colorSpace == ColorSpace::RGB) ? 3 : 1) )
      {
         uint16* sampleinfo;
         uint16 extrasamples;
         if ( ::TIFFGetField( tif, TIFFTAG_EXTRASAMPLES, &extrasamples, &sampleinfo ) )
            tiffOptions.premultipliedAlpha = *sampleinfo == EXTRASAMPLE_ASSOCALPHA;
      }

      // Optional information literals.

      if ( ::TIFFGetField( tif, TIFFTAG_SOFTWARE, &tiffStr ) )
         tiffOptions.software = tiffStr;
      else
         tiffOptions.software.Clear();

      if ( ::TIFFGetField( tif, TIFFTAG_IMAGEDESCRIPTION, &tiffStr ) )
         tiffOptions.imageDescription = tiffStr;
      else
         tiffOptions.imageDescription.Clear();

      if ( ::TIFFGetField( tif, TIFFTAG_COPYRIGHT, &tiffStr ) )
         tiffOptions.copyright = tiffStr;
      else
         tiffOptions.copyright.Clear();

      // Additional options from EXIF data (libtiff >= 3.8.0)

      if ( ::TIFFGetField( tif, TIFFTAG_EXIFIFD, &tiffUInt32 ) )
      {
         tdir_t dir = ::TIFFCurrentDirectory( tif );

         if ( ::TIFFReadEXIFDirectory( tif, tiffUInt32 ) )
         {
            if ( ::TIFFGetField( tif, EXIFTAG_ISOSPEEDRATINGS, &tiffUInt32, &tiffStr ) )
               options.isoSpeed = int( *((uint16*)tiffStr) );

            if ( ::TIFFGetField( tif, EXIFTAG_EXPOSURETIME, &tiffFloat ) )
               options.exposure = tiffFloat;

            if ( ::TIFFGetField( tif, EXIFTAG_APERTUREVALUE, &tiffFloat ) )
               options.aperture = Pow2( tiffFloat/2 );

            if ( ::TIFFGetField( tif, EXIFTAG_FOCALLENGTH, &tiffFloat ) )
               options.focalLength = tiffFloat; // mm

            if ( ::TIFFGetField( tif, EXIFTAG_CFAPATTERN, &tiffUInt32, &tiffStr ) )
               if ( tiffUInt32 == 8 && *((uint32*)tiffStr) == 0x00020002u ) // support RGB CFAs only
                  switch ( *((uint32*)(tiffStr + 4)) )
                  {
                  case 0x00010102u : options.cfaType = CFAType::BGGR; break;
                  case 0x01020001u : options.cfaType = CFAType::GRBG; break;
                  case 0x01000201u : options.cfaType = CFAType::GBRG; break;
                  case 0x02010100u : options.cfaType = CFAType::RGGB; break;
                  default          : options.cfaType = CFAType::None; break;
                  }
         }

        ::TIFFSetDirectory( tif, dir );
      }
   }

   catch ( ... )
   {
      Close();
      throw;
   }
}

// ----------------------------------------------------------------------------

bool TIFFReader::Extract( ICCProfile& icc )
{
   icc.Clear();

   if ( IsOpen() )
   {
      uint32 byteCount;
      void* iccData;

      if ( ::TIFFGetField( tif, TIFFTAG_ICCPROFILE, &byteCount, &iccData ) )
      {
         try
         {
            icc.Set( iccData );
            return true;
         }
         ERROR_HANDLER
      }
   }

   return false;
}

// ----------------------------------------------------------------------------

/*
bool TIFFReader::Extract( IPTCPhotoInfo& iptc )
{
   iptc.Clear();

   if ( IsOpen() )
   {
      uint16 uint32Count;
      void* iptcData;

      // The length of an IPTC PhotoInfo data block is given as a count of
      // 32-bit integers by the libtiff library.
      if ( ::TIFFGetField( tif, TIFFTAG_RICHTIFFIPTC, &uint32Count, &iptcData ) )
      {
         iptc.GetInfo( iptcData, uint32Count*4 );
         return true;
      }
   }

   return false;
}
*/

// ----------------------------------------------------------------------------

template <class P> inline
static void __ReadImage( GenericImage<P>& image, TIFFReader& reader, TIFFFileData* fileData )
{
   if ( !reader.IsOpen() )
      throw TIFF::InvalidReadOperation( String() );

   try
   {
      //
      // Read pixel data
      //

      // Allocate space.
      // Don't trust info fields since they are publicly accessible.
      image.AllocateData( tiffWidth, tiffLength, tiffSamplesPerPixel,
         (tiffPhotometric == PHOTOMETRIC_RGB) ? ColorSpace::RGB : ColorSpace::Gray );

      // Determine if we are dealing with a planar image. This is from our
      // algorithmic POV, which includes when we have a single channel to read.
      bool isPlanar = tiffPlanarConfig == PLANARCONFIG_SEPARATE ||
                      image.NumberOfChannels() == 1;

      // Are we dealing with signed integer samples?
      bool isSignedIntSample = tiffSampleFormat == SAMPLEFORMAT_INT;

      // Are we dealing with floating-point samples?
      bool isFloatSample = tiffSampleFormat == SAMPLEFORMAT_IEEEFP;

      // Begin reading pixels.

      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( String().Format(
                  "Reading TIFF: %d-bit %s, %d channel(s), %dx%d pixels, %s",
                  tiffBitsPerSample,
                  isFloatSample ? "floating point" : "integers",
                  tiffSamplesPerPixel,
                  tiffWidth, tiffLength,
                  (tiffPlanarConfig == PLANARCONFIG_SEPARATE) ? "planar" : "chunky" ),
         image.NumberOfSamples() );

      // If we are reading a floating-point TIFF file into an integer image, we
      // use the declared normalization range to rescale floating-point sample
      // values to [0,1] before expanding them to the native integer range.
      double k = 1.0;
      if ( isFloatSample && !P::IsFloatSample() )
         k /= fileData->lowerRange - fileData->upperRange;

      if ( isPlanar ) // separate planes or grayscale
      {
         if ( P::IsFloatSample() == isFloatSample && P::BitsPerSample() == tiffBitsPerSample )
         {
            // File sample type same as image sample type.
            // Planar organization.

            for ( int c = 0; c < image.NumberOfChannels(); ++c )
               for ( int i = 0; i < image.Height(); ++i, image.Status() += image.Width() )
                  if ( ::TIFFReadScanline( tif, image.ScanLine( i, c ), i, c ) < 0 )
                     throw TIFF::FileReadError( reader.Path() );
         }
         else
         {
            // File sample type and image sample type are different.
            // Planar organization.

            size_type tiffLineSize = image.Width() * (tiffBitsPerSample >> 3);
            ByteArray buffer( tiffLineSize );

            for ( int c = 0; c < image.NumberOfChannels(); ++c )
               for ( int i = 0; i < image.Height(); ++i, image.Status() += image.Width() )
               {
                  if ( ::TIFFReadScanline( tif, buffer.Begin(), i, c ) < 0 )
                     throw TIFF::FileReadError( reader.Path() );

                  typename P::sample* v = image.ScanLine( i, c );

                  // We use separate loops to reduce the overload required to
                  // perform signed/unsigned decisions.
                  // Signed integers are very rare in TIFF files.

                  if ( P::IsFloatSample() )
                  {
                     // For floating-point output samples, we perform optional
                     // normalization as a separate step after reading the
                     // entire image.

                     if ( isSignedIntSample )
                     {
                        // Signed integers.

                        union { const uint8* u8;
                                const int8*  i8;
                                const int16* i16;
                                const int32* i32; } b;
                        b.u8 = buffer.Begin();

                        for ( int j = 0; j < image.Width(); ++j )
                           switch ( tiffBitsPerSample )
                           {
                           case 8:
                              *v++ = typename P::sample( *b.i8++ );
                              break;
                           case 16:
                              *v++ = typename P::sample( *b.i16++ );
                              break;
                           case 32:
                              *v++ = typename P::sample( *b.i32++ );
                              break;
                           }
                     }
                     else
                     {
                        // Unsigned integers and floating point.

                        union { const uint8*  u8;
                                const uint16* u16;
                                const uint32* u32;
                                const float*  f32;
                                const double* f64; } b;
                        b.u8 = buffer.Begin();

                        for ( int j = 0; j < image.Width(); ++j )
                           switch ( tiffBitsPerSample )
                           {
                           case 8:
                              *v++ = typename P::sample( *b.u8++ );
                              break;
                           case 16:
                              *v++ = typename P::sample( *b.u16++ );
                              break;
                           case 32:
                              if ( isFloatSample )
                                 *v++ = typename P::sample( *b.f32++ );
                              else
                                 *v++ = typename P::sample( *b.u32++ );
                              break;
                           case 64:
                              *v++ = typename P::sample( *b.f64++ );
                              break;
                           }
                     }
                  }
                  else
                  {
                     if ( isSignedIntSample )
                     {
                        // Signed integers.

                        union { const uint8* u8;
                                const int8*  i8;
                                const int16* i16;
                                const int32* i32; } b;
                        b.u8 = buffer.Begin();

                        for ( int j = 0; j < image.Width(); ++j )
                           switch ( tiffBitsPerSample )
                           {
                           case 8:
                              *v++ = P::ToSample( *b.i8++ );
                              break;
                           case 16:
                              *v++ = P::ToSample( *b.i16++ );
                              break;
                           case 32:
                              *v++ = P::ToSample( *b.i32++ );
                              break;
                           }
                     }
                     else
                     {
                        // Unsigned integers and floating point.

                        union { const uint8*  u8;
                                const uint16* u16;
                                const uint32* u32;
                                const float*  f32;
                                const double* f64; } b;
                        b.u8 = buffer.Begin();

                        for ( int j = 0; j < image.Width(); ++j )
                           switch ( tiffBitsPerSample )
                           {
                           case 8:
                              *v++ = P::ToSample( *b.u8++ );
                              break;
                           case 16:
                              *v++ = P::ToSample( *b.u16++ );
                              break;
                           case 32:
                              if ( isFloatSample )
                                 *v++ = P::ToSample( k*(*b.f32++ - fileData->lowerRange) );
                              else
                                 *v++ = P::ToSample( *b.u32++ );
                              break;
                           case 64:
                              *v++ = P::ToSample( k*(*b.f64++ - fileData->lowerRange) );
                              break;
                           }
                     }
                  }
               }
         }
      }
      else
      {
         // Chunky TIFF image.

         // A chunky TIFF image is divided into a set of strips. Each strip
         // contains a fixed number of pixel rows, except for the last strip,
         // which may contain a smaller amount.

         unsigned tiffNumberOfStrips = ::TIFFNumberOfStrips( tif );

         uint32 tiffRowsPerStrip;
         if ( tiffNumberOfStrips > 1 )
            ::TIFFGetField( tif, TIFFTAG_ROWSPERSTRIP, &tiffRowsPerStrip );
         else
            tiffRowsPerStrip = uint32( image.Height() );

         ByteArray buffer( ::TIFFStripSize( tif ) );

         Array<typename P::sample*> v( image.NumberOfChannels() );

         // Main loop over strips.
         for ( unsigned s = 0, y = 0; s < tiffNumberOfStrips; ++s )
         {
            if ( ::TIFFReadEncodedStrip( tif, s, buffer.Begin(), -1 ) < 0 )
               throw TIFF::FileReadError( reader.Path() );

            union { const uint8*  u8;
                    const int8*   i8;
                    const uint16* u16;
                    const int16*  i16;
                    const uint32* u32;
                    const int32*  i32;
                    const float*  f32;
                    const double* f64; } b;
            b.u8 = buffer.Begin();

            // Loop over rows within this strip.
            for ( uint32 r = 0;
                  r < tiffRowsPerStrip && y < unsigned( image.Height() );
                  ++r, ++y, image.Status() += image.Width()*image.NumberOfChannels() )
            {
               for ( int c = 0; c < image.NumberOfChannels(); ++c )
                  v[c] = image.ScanLine( y, c );

               if ( P::IsFloatSample() == isFloatSample && P::BitsPerSample() == tiffBitsPerSample )
               {
                  // File sample type same as image sample type.
                  // Chunky organization.

                  for ( int x = 0; x < image.Width(); ++x )
                     for ( int c = 0; c < image.NumberOfChannels(); ++c )
                     {
                        *v[c]++ = *reinterpret_cast<const typename P::sample *>( b.u8 );
                        b.u8 += sizeof( typename P::sample );
                     }
               }
               else
               {
                  // File sample type and image sample type are different.

                  if ( P::IsFloatSample() )
                  {
                     if ( isSignedIntSample )
                     {
                        // File samples are signed integers.

                        for ( int x = 0; x < image.Width(); ++x )
                           for ( int c = 0; c < image.NumberOfChannels(); ++c )
                              switch ( tiffBitsPerSample )
                              {
                              case 8:
                                 *v[c]++ = typename P::sample( *b.i8++ );
                                 break;
                              case 16:
                                 *v[c]++ = typename P::sample( *b.i16++ );
                                 break;
                              case 32:
                                 *v[c]++ = typename P::sample( *b.i32++ );
                                 break;
                              }
                     }
                     else
                     {
                        // Unsigned integers and floating point.

                        for ( int x = 0; x < image.Width(); ++x )
                           for ( int c = 0; c < image.NumberOfChannels(); ++c )
                              switch ( tiffBitsPerSample )
                              {
                              case 8 :
                                 *v[c]++ = typename P::sample( *b.u8++ );
                                 break;
                              case 16 :
                                 *v[c]++ = typename P::sample( *b.u16++ );
                                 break;
                              case 32 :
                                 if ( isFloatSample )
                                    *v[c]++ = typename P::sample( *b.f32++ );
                                 else
                                    *v[c]++ = typename P::sample( *b.u32++ );
                                 break;
                              case 64:
                                 *v[c]++ = typename P::sample( *b.f64++ );
                                 break;
                              }
                     }
                  }
                  else
                  {
                     if ( isSignedIntSample )
                     {
                        // File samples are signed integers.

                        for ( int x = 0; x < image.Width(); ++x )
                           for ( int c = 0; c < image.NumberOfChannels(); ++c )
                              switch ( tiffBitsPerSample )
                              {
                              case 8:
                                 *v[c]++ = P::ToSample( *b.i8++ );
                                 break;
                              case 16:
                                 *v[c]++ = P::ToSample( *b.i16++ );
                                 break;
                              case 32:
                                 *v[c]++ = P::ToSample( *b.i32++ );
                                 break;
                              }
                     }
                     else
                     {
                        // Unsigned integers and floating point.

                        for ( int x = 0; x < image.Width(); ++x )
                           for ( int c = 0; c < image.NumberOfChannels(); ++c )
                              switch ( tiffBitsPerSample )
                              {
                              case 8 :
                                 *v[c]++ = P::ToSample( *b.u8++ );
                                 break;
                              case 16 :
                                 *v[c]++ = P::ToSample( *b.u16++ );
                                 break;
                              case 32 :
                                 if ( isFloatSample )
                                    *v[c]++ = P::ToSample( k*(*b.f32++ - fileData->lowerRange) );
                                 else
                                    *v[c]++ = P::ToSample( *b.u32++ );
                                 break;
                              case 64:
                                 *v[c]++ = P::ToSample( k*(*b.f64++ - fileData->lowerRange) );
                                 break;
                              }
                     }
                  }
               }
            }
         }
      }
   }
   catch ( ... )
   {
      reader.Close();
      image.FreeData();
      throw;
   }
}

#define NORMALIZE( I )                                                                                \
   if ( options.readNormalized )                                                                      \
   {                                                                                                  \
      StatusMonitor status = image.Status();                                                          \
      image.SetStatusCallback( 0 );                                                                   \
                                                                                                      \
      /* Replace NaNs and infinities with zeros.                                    */                \
      image.ResetSelections();                                                                        \
      for ( int c = 0; c < image.NumberOfChannels(); ++c )                                            \
         for ( I::sample_iterator i( image, c ); i; ++i )                                             \
            if ( !IsFinite( *i ) )                                                                    \
               *i = 0;                                                                                \
                                                                                                      \
      double zeroOffset, scaleRange;                                                                  \
                                                                                                      \
      if ( tiffSampleFormat == SAMPLEFORMAT_IEEEFP )                                                  \
      {                                                                                               \
         /* For floating-point TIFFs we have no standard way to know the actual     */                \
         /* data range in advance. If existing pixel values fit into the            */                \
         /* normalization range, then we assume they are already normalized         */                \
         /* and just do nothing. If this is not the case, we have to rescale.       */                \
                                                                                                      \
         /* Obtain extreme pixel values.                                            */                \
         I::sample mn, mx;                                                                            \
         image.GetExtremePixelValues( mn, mx );                                                       \
         zeroOffset = Min( double( fileData->lowerRange ), double( mn ) );                            \
         scaleRange = Max( double( fileData->upperRange ), double( mx ) );                            \
      }                                                                                               \
      else                                                                                            \
      {                                                                                               \
         /* For integer samples we always know scaling parameters in advance.       */                \
                                                                                                      \
         if ( tiffSampleFormat == SAMPLEFORMAT_INT )  /* signed integers ?          */                \
         {                                                                                            \
            zeroOffset = BitMin( tiffBitsPerSample );                                                 \
            scaleRange = BitMax( tiffBitsPerSample );                                                 \
         }                                                                                            \
         else                                                                                         \
         {                                                                                            \
            zeroOffset = 0;                                                                           \
            scaleRange = UBitMax( tiffBitsPerSample );                                                \
         }                                                                                            \
      }                                                                                               \
                                                                                                      \
      /* If necessary, rescale to the normalized range.                             */                \
                                                                                                      \
      if ( zeroOffset != fileData->lowerRange || scaleRange != fileData->upperRange )                 \
      {                                                                                               \
         double iDelta = scaleRange - zeroOffset;                                                     \
         double oDelta = fileData->upperRange - fileData->lowerRange;                                 \
         for ( int c = 0; c < image.NumberOfChannels(); ++c )                                         \
            if ( oDelta != 1 || fileData->lowerRange != 0 )                                           \
            {                                                                                         \
               double rDelta = oDelta/iDelta;                                                         \
               for ( I::sample_iterator i( image, c ); i; ++i )                                       \
                  *i = I::sample( (*i - zeroOffset)*rDelta + fileData->lowerRange );                  \
            }                                                                                         \
            else                                                                                      \
            {                                                                                         \
               for ( I::sample_iterator i( image, c ); i; ++i )                                       \
                  *i = I::sample( (*i - zeroOffset)/iDelta );                                         \
            }                                                                                         \
      }                                                                                               \
                                                                                                      \
      image.Status() = status;                                                                        \
   }

void TIFFReader::ReadImage( FImage& image )
{
   __ReadImage( image, *this, fileData );
   NORMALIZE( FImage )
}

void TIFFReader::ReadImage( DImage& image )
{
   __ReadImage( image, *this, fileData );
   NORMALIZE( DImage )
}

void TIFFReader::ReadImage( UInt8Image& image )
{
   __ReadImage( image, *this, fileData );
}

void TIFFReader::ReadImage( UInt16Image& image )
{
   __ReadImage( image, *this, fileData );
}

void TIFFReader::ReadImage( UInt32Image& image )
{
   __ReadImage( image, *this, fileData );
}

#undef NORMALIZE

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#undef info
#undef options

// ----------------------------------------------------------------------------

bool TIFFWriter::IsOpen() const
{
   return fileData != 0 && !path.IsEmpty();
}

void TIFFWriter::Close()
{
   TIFF::__Close();
}

void TIFFWriter::Create( const String& filePath, int count )
{
   if ( IsOpen() )
      throw InvalidWriteOperation( path );

   if ( filePath.IsEmpty() )
      throw InvalidFilePath( filePath );

   if ( count <= 0 )
      throw NotImplemented( *this, "Write empty TIFF files" );

   if ( count > 1 )
      throw NotImplemented( *this, "Write multiple images to a single TIFF file" );

   Reset();

   if ( fileData == 0 )
      fileData = new TIFFFileData;

   path = File::WindowsPathToUnix( filePath );
}

// ----------------------------------------------------------------------------

template <class P> inline
static void __WriteImage( const GenericImage<P>& image, TIFFWriter& writer,
                          const ICCProfile& icc, TIFFFileData* fileData )
{
   if ( !writer.IsOpen() )
      throw TIFF::WriterNotInitialized( String() );

   if ( writer.Options().lowerRange >= writer.Options().upperRange )
      throw TIFF::InvalidNormalizationRange( writer.Path() );

   if ( image.IsEmptySelection() )
      throw TIFF::InvalidWriteOperation( writer.Path() );

   // Make safe copies of critical parameters.
   fileData->lowerRange = writer.Options().lowerRange;
   fileData->upperRange = writer.Options().upperRange;

   //
   // Ensure coherence of optional parameters ---------------------------------
   //

   // Supported sample sizes are 8, 16, 32 and 64 bits:
   //
   // 8-bit and 16-bit samples must be unsigned integers.
   // 32-bit samples can be unsigned integers or IEEE floating-point.
   // 64-bit samples can only be IEEE floating-point samples.

   switch ( writer.Options().bitsPerSample )
   {
   default:
      writer.Options().bitsPerSample = 16;
   case 8:
   case 16:
      writer.Options().ieeefpSampleFormat = false;
   case 32:
      break;
   case 64:
      writer.Options().ieeefpSampleFormat = true;
      break;
   }

   tiffBitsPerSample = writer.Options().bitsPerSample;

   // TIFF sample format.
   // We support signed integers for input, but not for output.

   tiffSampleFormat = writer.Options().ieeefpSampleFormat ?
                                    SAMPLEFORMAT_IEEEFP : SAMPLEFORMAT_UINT;

   // TIFF color space.
   // For RGB images, we'll write a grayscale TIFF image (possibly with
   // additional alpha channels) if the three RGB nominal channels are not
   // selected.

   tiffPhotometric = (image.ColorSpace() == ColorSpace::Gray ||
                          image.NumberOfSelectedChannels() < 3 ||
                          image.FirstSelectedChannel() != 0) ?
                                    PHOTOMETRIC_MINISBLACK : PHOTOMETRIC_RGB;

   // Size of a file sample in bytes.
   int tiffSampleSize = writer.Options().bitsPerSample >> 3;

   // Ensure coherent planar organization.
   // A grayscale image is functionally planar, but grayscale images should
   // never be denoted as planar by the TIFFTAG_PLANARCONFIG tag.

   bool isPlanar = image.NumberOfSelectedChannels() == 1 || writer.TIFFOptions().planar;

   bool tiffPlanar = image.NumberOfSelectedChannels() > 1 && writer.TIFFOptions().planar;

   tiffPlanarConfig = tiffPlanar ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;

   // Only ZIP and LZW compressions are supported for output.
   // Our current policy is to ignore unsupported compressions.

   int tiffCompression;

   switch ( writer.TIFFOptions().compression )
   {
   default:
   case TIFFCompression::None:
      tiffCompression = COMPRESSION_NONE;
      break;
   case TIFFCompression::ZIP:
      tiffCompression = COMPRESSION_DEFLATE;
      break;
   case TIFFCompression::LZW:
      tiffCompression = COMPRESSION_LZW;
      break;
   }

   //
   // TIFF image file generation ----------------------------------------------
   //

   try
   {
      Rect r = image.SelectedRectangle();
      int width = r.Width();
      int height = r.Height();

      tiffWidth = width;
      tiffLength = height;
      tiffSamplesPerPixel = image.NumberOfSelectedChannels();

      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( String().Format(
                  "Writing TIFF: %d-bit %s, %d channel(s), %dx%d pixels, %s",
                  writer.Options().bitsPerSample,
                  writer.Options().ieeefpSampleFormat ? "floating point" : "integers",
                  tiffSamplesPerPixel,
                  width, height, tiffPlanar ? "planar" : "chunky" ),
            image.NumberOfSelectedSamples() );

      //
      // Create the TIFF file stream.
      //
      IsoString path8 =
#ifdef __PCL_WINDOWS
         File::UnixPathToWindows( writer.Path() ).ToMBS();
#else
         writer.Path().ToUTF8();
#endif
      tif = ::TIFFOpen( path8.c_str(), "w" );
      if ( tif == 0 )
         throw TIFF::UnableToCreateFile( writer.Path() );

      //
      // Mandatory TIFF tags.
      //

      // Change this when porting PCL to Mac OSX, or, better, define a global
      // macro in Defs.h :)
      ::TIFFSetField( tif, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB );

      // We always generate top->bottom, left->right oriented images.
      ::TIFFSetField( tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT );

      // We write only RGB and grayscale TIFF images.
      // N.B.: This field has already been corrected as a function of the
      // image's currently selected channels.
      ::TIFFSetField( tif, TIFFTAG_PHOTOMETRIC, tiffPhotometric );

      // We do support both planar and chunky organizations.
      // N.B.: This field has already been corrected to avoid marking grayscale
      // image files as planar.
      ::TIFFSetField( tif, TIFFTAG_PLANARCONFIG, tiffPlanarConfig );

      // Image dimensions in pixels.
      ::TIFFSetField( tif, TIFFTAG_IMAGEWIDTH, width );
      ::TIFFSetField( tif, TIFFTAG_IMAGELENGTH, height );

      // Number of bits per sample.
      // N.B.: This field has already been checked to ensure a correct value.
      ::TIFFSetField( tif, TIFFTAG_BITSPERSAMPLE, writer.Options().bitsPerSample );

      // Sample format: unsigned integers or IEEE floating-point.
      // N.B.: This value has been deduced from corrected options.bitsPerSample.
      ::TIFFSetField( tif, TIFFTAG_SAMPLEFORMAT, tiffSampleFormat );

      // Number of samples per pixel.
      ::TIFFSetField( tif, TIFFTAG_SAMPLESPERPIXEL, image.NumberOfSelectedChannels() );

      // Flag true if RGB components must be premultiplied by alpha.
      bool hasAlpha = image.FirstSelectedChannel() == 0 &&
                      image.NumberOfSelectedChannels() > image.NumberOfNominalChannels();

      if ( hasAlpha )
      {
         // Extra samples, including an alpha channel
         int na = image.NumberOfSelectedChannels() - image.NumberOfNominalChannels();
         Array<uint16> extra( (size_type)na );
         extra[0] = writer.TIFFOptions().associatedAlpha ? EXTRASAMPLE_ASSOCALPHA : EXTRASAMPLE_UNASSALPHA;
         for ( int i = 1; i < na; ++i )
            extra[i] = EXTRASAMPLE_UNSPECIFIED;
         ::TIFFSetField( tif, TIFFTAG_EXTRASAMPLES, na, extra.Begin() );
      }
      else if ( image.FirstSelectedChannel() > 0 && image.NumberOfSelectedChannels() > 1 )
      {
         // Extra samples, all of them unspecified
         int na = image.NumberOfSelectedChannels() - 1;
         Array<uint16> extra( (size_type)na );
         for ( int i = 0; i < na; ++i )
            extra[i] = EXTRASAMPLE_UNSPECIFIED;
         ::TIFFSetField( tif, TIFFTAG_EXTRASAMPLES, na, extra.Begin() );
      }

      // Number of pixel rows per TIFF strip. The stripSize static variable
      // controls the strip size in bytes.
      ::TIFFSetField( tif, TIFFTAG_ROWSPERSTRIP,
            Max( size_type( 1 ), writer.StripSize()/(width*tiffSampleSize) ) );

      /*
      // No, MinSampleValue and MaxSampleValue are not intended to represent
      // the available dynamic range - sigh!
      if ( writer.Options().ieeefpSampleFormat )
      {
         typename P::sample mn, mx;
         image.GetExtremePixelValues( mn, mx );
         ::TIFFSetField( tif, TIFFTAG_SMINSAMPLEVALUE, double( mn ) );
         ::TIFFSetField( tif, TIFFTAG_SMAXSAMPLEVALUE, double( mx ) );
      }
      */

      //
      // Optional TIFF features/tags.
      //

      if ( tiffCompression != COMPRESSION_NONE )
      {
         // Supported compression schemes:
         //    COMPRESSION_DEFLATE  : ZIP compression, free, recommended
         //    COMPRESSION_LZW      : LZW compression, had patent issues
         //
         // N.B.: This field has already been corrected.

         ::TIFFSetField( tif, TIFFTAG_COMPRESSION, tiffCompression );

         // For 8 and 16 bit samples, use LZW and ZIP encoded data with
         // horizontal differencing. Unfortunately this is not supported for
         // 32-bit samples.
         if ( tiffBitsPerSample < 32 )
            ::TIFFSetField( tif, TIFFTAG_PREDICTOR, 2 );
      }

      // Metric or English resolution units. Default is English (inches).
      ::TIFFSetField( tif, TIFFTAG_RESOLUTIONUNIT,
         writer.Options().metricResolution ? RESUNIT_CENTIMETER : RESUNIT_INCH );

      // Horizontal resolution. <= zero means no specified resolution.
      if ( writer.Options().xResolution > 0 )
         ::TIFFSetField( tif, TIFFTAG_XRESOLUTION, float( writer.Options().xResolution ) );

      // Vertical resolution. <= zero means no specified resolution.
      if ( writer.Options().yResolution > 0 )
         ::TIFFSetField( tif, TIFFTAG_YRESOLUTION, float( writer.Options().yResolution ) );

      // Software description and version information.
      // We are not forcing our own signature here... this might be our good
      // action of the day :)
      if ( !writer.TIFFOptions().software.IsEmpty() )
      {
         IsoString s = writer.TIFFOptions().software.ToUTF8();
         ::TIFFSetField( tif, TIFFTAG_SOFTWARE, s.c_str() );
      }

      // Place a description of this image here.
      if ( !writer.TIFFOptions().imageDescription.IsEmpty() )
      {
         IsoString s = writer.TIFFOptions().imageDescription.ToUTF8();
         ::TIFFSetField( tif, TIFFTAG_IMAGEDESCRIPTION, s.c_str() );
      }

      // Copyright info for this image comes here.
      if ( !writer.TIFFOptions().copyright.IsEmpty() )
      {
         IsoString s = writer.TIFFOptions().copyright.ToUTF8();
         ::TIFFSetField( tif, TIFFTAG_COPYRIGHT, s.c_str() );
      }

      // ICC Profile.

      if ( writer.Options().embedICCProfile && icc.IsProfile() )
      {
         // Retrieve the length in bytes of the ICC profile structure.
         uint32 byteCount = uint32( icc.ProfileSize() );

         // Don't try to write an empty ICC profile.
         if ( byteCount != 0 )
            ::TIFFSetField( tif, TIFFTAG_ICCPROFILE, byteCount, icc.ProfileData().Begin() );
      }

      /*
      if ( ... && writer.Options().embedMetadata )
      {
         // ### TODO: XML metadata support.
      }
      */

      //
      // Write pixel data.
      //

      int nc = image.NumberOfSelectedChannels();
      int c0 = image.FirstSelectedChannel();
      int ca = (hasAlpha && writer.TIFFOptions().premultipliedAlpha) ? image.NumberOfNominalChannels() : -1;

      if ( P::IsFloatSample() == writer.Options().ieeefpSampleFormat &&
           P::BitsPerSample() == tiffBitsPerSample )
      {
         // File sample type same as image sample type.

         if ( isPlanar )
         {
            // Planar organization.

            // We need an intermediate buffer for two reasons. First, if
            // compression is selected, libtiff's compression is destructive.
            // Second, if we are writing floating-point samples we have to
            // ensure that they are within the specified normalized range.

            Array<typename P::sample> buffer( width );

            for ( int c = 0; c < nc; ++c )
               for ( int y = 0; y < height; ++y, image.Status() += width )
               {
                  const typename P::sample* v = image.PixelAddress( r.x0, r.y0+y, c+c0 );

                  if ( P::IsFloatSample() )
                  {
                     typename P::sample* b = buffer.Begin();
                     for ( int x = 0; x < width; ++x )
                        *b++ = typename P::sample( Range( double( *v++ ),
                                             double( fileData->lowerRange ),
                                             double( fileData->upperRange ) ) );
                  }
                  else
                     P::Copy( buffer.Begin(), v, width );

                  if ( ca >= 0 && c < ca ) // premultiplied alpha channel
                  {
                     const typename P::sample* a = image.PixelAddress( r.x0, r.y0+y, ca );
                           typename P::sample* b = buffer.Begin();

                     if ( P::IsFloatSample() )
                        for ( int x = 0; x < width; ++x, ++a )
                           *b++ *= *a; // ### warning: alpha values must be in the proper range
                     else
                        for ( int x = 0; x < width; ++x, ++a )
                        {
                           double fa; P::FromSample( fa, *a );
                           double fb; P::FromSample( fb, *b );
                           *b++ = P::ToSample( fb*fa );
                        }
                  }

                  if ( ::TIFFWriteScanline( tif, buffer.Begin(), y, c ) < 0 )
                     throw TIFF::FileWriteError( writer.Path() );
               }
         }
         else
         {
            // Chunky organization.

            size_type N = nc*width;
            Array<typename P::sample> buffer( N );
            Array<const typename P::sample*> v( nc );

            for ( int y = 0; y < height; ++y, image.Status() += N )
            {
               typename P::sample* b = buffer.Begin();

               for ( int c = 0; c < nc; ++c )
                  v[c] = image.PixelAddress( r.x0, r.y0+y, c+c0 );

               for ( int x = 0; x < width; ++x )
                  for ( int c = 0; c < nc; ++c )
                     if ( P::IsFloatSample() )
                        *b++ = typename P::sample( Range( double( *v[c]++ ),
                                             double( fileData->lowerRange ),
                                             double( fileData->upperRange ) ) );
                     else
                        *b++ = *v[c]++;

               if ( ca >= 0 )
               {
                  const typename P::sample* a = image.PixelAddress( r.x0, r.y0+y, ca );
                                            b = buffer.Begin();

                  if ( P::IsFloatSample() )
                     for ( int x = 0; x < width; ++x, ++a )
                     {
                        for ( int c = 0; c < ca; ++c )
                           *b++ *= *a;
                        b += nc - ca;
                     }
                  else
                     for ( int x = 0; x < width; ++x, ++a )
                     {
                        double fa; P::FromSample( fa, *a );
                        for ( int c = 0; c < ca; ++c )
                        {
                           double fb; P::FromSample( fb, *b );
                           *b++ = P::ToSample( fb*fa );
                        }
                        b += nc - ca;
                     }
               }

               if ( ::TIFFWriteScanline( tif, buffer.Begin(), y, 0 ) < 0 )
                  throw TIFF::FileWriteError( writer.Path() );
            }
         }
      }
      else
      {
         // File sample type and image sample type are different.

         // Either rescaling or truncating to the [0,1] range is required if we
         // are writing a floating-point image to an integer TIFF file.
         bool rescale = P::IsFloatSample() && !writer.Options().ieeefpSampleFormat &&
                     (fileData->upperRange != 1 || fileData->lowerRange != 0);
         double k = 1.0;
         if ( rescale )
            k /= fileData->upperRange - fileData->lowerRange;

         if ( isPlanar )
         {
            // File sample type and image sample type are different.
            // Planar organization.

            ByteArray buffer( width*tiffSampleSize );

            for ( int c = 0; c < nc; ++c )
               for ( int y = 0; y < height; ++y, image.Status() += width )
               {
                  union { uint8*  u8;
                          uint16* u16;
                          uint32* u32;
                          float*  f32;
                          double* f64; } b;
                  b.u8 = buffer.Begin();

                  const typename P::sample* v = image.PixelAddress( r.x0, r.y0+y, c+c0 );
                  const typename P::sample* a = (ca >= 0 && c < ca) ? image.PixelAddress( r.x0, r.y0+y, ca ) : 0;

                  for ( int x = 0; x < width; ++x )
                  {
                     typename P::sample f = *v++;

                     if ( P::IsFloatSample() )
                     {
                        f = typename P::sample( Range( double( f ),
                                             double( fileData->lowerRange ),
                                             double( fileData->upperRange ) ) );

                        if ( a != 0 )
                           f *= *a++;

                        if ( rescale )
                           f = typename P::sample( k*(f - fileData->lowerRange) );
                     }
                     else if ( a != 0 )
                     {
                        double fa; P::FromSample( fa, *a++ );
                        double fb; P::FromSample( fb, f );
                        f = P::ToSample( fb*fa );
                     }

                     switch ( writer.Options().bitsPerSample )
                     {
                     case  8:
                        P::FromSample( *b.u8++, f );
                        break;
                     case 16:
                        P::FromSample( *b.u16++, f );
                        break;
                     case 32:
                        if ( writer.Options().ieeefpSampleFormat )
                           P::FromSample( *b.f32++, f );
                        else
                           P::FromSample( *b.u32++, f );
                        break;
                     case 64: // 64-bit integers not allowed.
                        P::FromSample( *b.f64++, f );
                        break;
                     }
                  }

                  if ( ::TIFFWriteScanline( tif, buffer.Begin(), y, c ) < 0 )
                     throw TIFF::FileWriteError( writer.Path() );
               }
         }
         else
         {
            // File sample type and image sample type are different.
            // Chunky organization.

            size_type N = nc*width;
            ByteArray buffer( N*tiffSampleSize );
            Array<const typename P::sample*> v( nc );

            for ( int y = 0; y < height; ++y, image.Status() += N )
            {
               union { uint8*  u8;
                       uint16* u16;
                       uint32* u32;
                       float*  f32;
                       double* f64; } b;
               b.u8 = buffer.Begin();

               for ( int c = 0; c < nc; ++c )
                  v[c] = image.PixelAddress( r.x0, r.y0+y, c+c0 );

               const typename P::sample* a = (ca >= 0) ? image.PixelAddress( r.x0, r.y0+y, ca ) : 0;

               for ( int x = 0; x < width; ++x, ++a )
                  for ( int c = 0; c < nc; ++c )
                  {
                     typename P::sample f = *v[c]++;

                     if ( P::IsFloatSample() )
                     {
                        f = typename P::sample( Range( double( f ),
                                             double( fileData->lowerRange ),
                                             double( fileData->upperRange ) ) );

                        if ( ca >= 0 && c < ca )
                           f *= *a;

                        if ( rescale )
                           f = typename P::sample( k*(f - fileData->lowerRange) );
                     }
                     else if ( ca >= 0 && c < ca )
                     {
                        double fa; P::FromSample( fa, *a );
                        double fb; P::FromSample( fb, f );
                        f = P::ToSample( fb*fa );
                     }

                     switch ( writer.Options().bitsPerSample )
                     {
                     case  8:
                        P::FromSample( *b.u8++, f );
                        break;
                     case 16:
                        P::FromSample( *b.u16++, f );
                        break;
                     case 32:
                        if ( writer.Options().ieeefpSampleFormat )
                           P::FromSample( *b.f32++, f );
                        else
                           P::FromSample( *b.u32++, f );
                        break;
                     case 64: // 64-bit integers not allowed.
                        P::FromSample( *b.f64++, f );
                        break;
                     }
                  }

               if ( ::TIFFWriteScanline( tif, buffer.Begin(), y, 0 ) < 0 )
                  throw TIFF::FileWriteError( writer.Path() );
            }
         }
      }

      ::TIFFClose( tif ), tif = 0;
   }
   catch ( ... )
   {
      if ( tif != 0 )
         ::TIFFClose( tif ), tif = 0;
      throw;
   }
}

void TIFFWriter::WriteImage( const FImage& image )
{
   __WriteImage( image, *this, icc, fileData );
}

void TIFFWriter::WriteImage( const DImage& image )
{
   __WriteImage( image, *this, icc, fileData );
}

void TIFFWriter::WriteImage( const UInt8Image& image )
{
   __WriteImage( image, *this, icc, fileData );
}

void TIFFWriter::WriteImage( const UInt16Image& image )
{
   __WriteImage( image, *this, icc, fileData );
}

void TIFFWriter::WriteImage( const UInt32Image& image )
{
   __WriteImage( image, *this, icc, fileData );
}

// ----------------------------------------------------------------------------

size_type TIFFWriter::StripSize()
{
   return stripSize;
}

void TIFFWriter::SetStripSize( size_type sz )
{
   // Keep it within reasonable limits.
   stripSize = Range( sz, size_type( 1024 ), size_type( 65536 ) );
}

// ----------------------------------------------------------------------------

bool TIFF::IsStrictMode()
{
   return strictMode;
}

void TIFF::SetStrictMode( bool enable )
{
   strictMode = enable;
}

bool TIFF::AreWarningsEnabled()
{
   return showWarnings;
}

void TIFF::EnableWarnings( bool enable )
{
   showWarnings = enable;
}

// ----------------------------------------------------------------------------

}  // pcl

// ****************************************************************************
// EOF TIFF.cpp - Released 2014/10/29 07:34:49 UTC
