//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard TIFF File Format Module Version 01.00.07.0335
// ----------------------------------------------------------------------------
// TIFF.cpp - Released 2017-07-09T18:07:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard TIFF PixInsight module.
//
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "TIFF.h"

#include <pcl/Atomic.h>
#include <pcl/AutoLock.h>
#include <pcl/Console.h>
#include <pcl/ErrorHandler.h>
#include <pcl/MessageBox.h>

#include <libtiff/tiffio.h>

#define  PCL_TIFF_STRICT_MODE    1
#define  PCL_TIFF_SHOW_WARNINGS  1

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * TIFF errors are necessarily fatal only in strict mode.
 * In permissive mode, we only show error messages on the console.
 */
static void TIFFErrorHandler( const char* module, const char* fmt, va_list args )
{
   String msg; (void)msg.VFormat( fmt, args );
#ifdef PCL_TIFF_STRICT_MODE
   throw Error( String( "TIFF Error: " ) + module + ": " + msg );
#else
   Console().CriticalLn( "<end><cbr>*** Error: TIFF: " + msg );
#endif
}

/*
 * TIFF warnings are shown (console only) when warning messages are enabled.
 */
static void TIFFWarningHandler( const char* module, const char* fmt, va_list args )
{
#ifdef PCL_TIFF_SHOW_WARNINGS
   String msg; (void)msg.VFormat( fmt, args );
   Console().WarningLn( "<end><cbr>** Warning: TIFF: " + msg );
#endif
}

// ----------------------------------------------------------------------------

struct TIFFFileData
{
   ::TIFF*  handle = nullptr;
   uint32   width;
   uint32   length;
   uint16   bitsPerSample;
   uint16   sampleFormat;
   uint16   samplesPerPixel;
   uint16   photometric;
   uint16   planarConfig;

   double   lowerRange; // safe copies of critical parameters
   double   upperRange;

   TIFFFileData()
   {
      static Mutex     mutex;
      static AtomicInt initialized;

      if ( initialized.FetchAndAdd( 0 ) == 0 )
      {
         volatile AutoLock lock( mutex );
         if ( !initialized )
         {
            ::TIFFSetErrorHandler( TIFFErrorHandler );
            ::TIFFSetWarningHandler( TIFFWarningHandler );
            (void)initialized.FetchAndAdd( 1 );
         }
      }
   }

   ~TIFFFileData()
   {
      if ( handle != nullptr )
         ::TIFFClose( (::TIFF*)handle ), handle = nullptr;
   }
};

// ----------------------------------------------------------------------------

TIFF::TIFF()
{
}

// ----------------------------------------------------------------------------

TIFF::~TIFF() noexcept( false )
{
   CloseStream();
}

// ----------------------------------------------------------------------------

void TIFF::CloseStream() // ### derived must call base
{
   // Close TIFF file stream and clean up libtiff data.
   m_fileData.Destroy();
}

// ----------------------------------------------------------------------------

TIFFReader::TIFFReader()
{
}

// ----------------------------------------------------------------------------

TIFFReader::~TIFFReader() noexcept( false )
{
}

// ----------------------------------------------------------------------------

bool TIFFReader::IsOpen() const
{
   return m_fileData && m_fileData->handle != nullptr;
}

// ----------------------------------------------------------------------------

void TIFFReader::Close()
{
   TIFF::CloseStream();
}

// ----------------------------------------------------------------------------

void TIFFReader::Open( const String& path )
{
   if ( IsOpen() )
      throw InvalidReadOperation( m_path );

   if ( path.IsEmpty() )
      throw InvalidFilePath( path );

   if ( m_image.options.lowerRange >= m_image.options.upperRange )
      throw InvalidNormalizationRange( path );

   m_image.info.Reset();

   try
   {
      m_fileData = new TIFFFileData;
#ifdef __PCL_WINDOWS
      m_path = File::WindowsPathToUnix( path );
#else
      m_path = path;
#endif

      // Keep critical parameters secured in private data.
      m_fileData->lowerRange = m_image.options.lowerRange;
      m_fileData->upperRange = m_image.options.upperRange;

      //
      // Open the TIFF image file stream for reading.
      //
      IsoString path8 =
#ifdef __PCL_WINDOWS
         File::UnixPathToWindows( m_path ).ToMBS();
#else
         m_path.ToUTF8();
#endif
      if ( (m_fileData->handle = ::TIFFOpen( path8.c_str(), "r" )) == nullptr )
         throw UnableToOpenFile( m_path );

      //
      // Retrieve TIFF image information --------------------------------------
      //

      // Mandatory fields.
      ::TIFFGetField( m_fileData->handle, TIFFTAG_IMAGEWIDTH, &m_fileData->width );
      ::TIFFGetField( m_fileData->handle, TIFFTAG_IMAGELENGTH, &m_fileData->length );
      ::TIFFGetField( m_fileData->handle, TIFFTAG_BITSPERSAMPLE, &m_fileData->bitsPerSample );
      ::TIFFGetField( m_fileData->handle, TIFFTAG_SAMPLESPERPIXEL, &m_fileData->samplesPerPixel );
      ::TIFFGetField( m_fileData->handle, TIFFTAG_PLANARCONFIG, &m_fileData->planarConfig );

      // Update ImageInfo.
      m_image.info.width = m_fileData->width;
      m_image.info.height = m_fileData->length;
      m_image.info.numberOfChannels = m_fileData->samplesPerPixel;

      // Photometric mode = color space of the target Image object.
      ::TIFFGetField( m_fileData->handle, TIFFTAG_PHOTOMETRIC, &m_fileData->photometric );

      // We only support the grayscale and RGB color photometric modes.
      switch ( m_fileData->photometric )
      {
      case PHOTOMETRIC_MINISBLACK:
      case PHOTOMETRIC_MINISWHITE:
         m_image.info.colorSpace = ColorSpace::Gray;
         break;

      case PHOTOMETRIC_RGB:
         m_image.info.colorSpace = ColorSpace::RGB;
         break;

      default:
         m_image.info.colorSpace = ColorSpace::Unknown;
         throw UnsupportedColorSpace( m_path );
      }

      // The SampleFormat field is optional. If not present, we assume the
      // sample format is unsigned 16-bit integer. This is in conformance with
      // baseline TIFF 6.0.
      if ( !::TIFFGetField( m_fileData->handle, TIFFTAG_SAMPLEFORMAT, &m_fileData->sampleFormat ) )
         m_fileData->sampleFormat = SAMPLEFORMAT_UINT; // assume unsigned integers

      // Supported pixel depths: 8, 16, 32 and 64 bits.
      if ( m_fileData->bitsPerSample != 8 &&
           m_fileData->bitsPerSample != 16 &&
           m_fileData->bitsPerSample != 32 &&
           m_fileData->bitsPerSample != 64 )
      {
         throw UnsupportedBitsPerSample( m_path );
      }

      // Supported sample formats:
      //    8, 16 and 32 bits unsigned integers -> sample format = 1
      //    8, 16 and 32 bits signed integers -> sample format = 2
      //    32 and 64 bits IEEE floating point -> sample format = 3
      //    void (will be treated as unsigned integer) -> sample format = 4
      if ( m_fileData->sampleFormat != SAMPLEFORMAT_UINT &&
           m_fileData->sampleFormat != SAMPLEFORMAT_INT &&
           m_fileData->sampleFormat != SAMPLEFORMAT_IEEEFP &&
           m_fileData->sampleFormat != SAMPLEFORMAT_VOID )
      {
         throw UnsupportedSampleFormat( m_path );
      }

      // Treat unknown sample format as unsigned integer.
      if ( m_fileData->sampleFormat == SAMPLEFORMAT_VOID )
         m_fileData->sampleFormat = SAMPLEFORMAT_UINT;

      // Reject bizarre things, such as 64-bit integers, or impossible reals.
      if ( m_fileData->bitsPerSample == 64 && m_fileData->sampleFormat != SAMPLEFORMAT_IEEEFP ||
           m_fileData->bitsPerSample < 32 && m_fileData->sampleFormat == SAMPLEFORMAT_IEEEFP )
      {
         throw UnsupportedSampleFormat( m_path );
      }

      // If reached this point, then hopefully this TIFF image is valid for us.
      m_image.info.supported = true;

      //
      // Retrieve optional TIFF information -----------------------------------
      //

      uint16 tiffUInt16;
      uint32 tiffUInt32;
      float  tiffFloat;
      char*  tiffStr;

      // Sample format.
      m_image.options.bitsPerSample = m_fileData->bitsPerSample;
      m_image.options.ieeefpSampleFormat = m_fileData->sampleFormat == SAMPLEFORMAT_IEEEFP;
      m_image.options.signedIntegers = m_image.options.complexSample = false;

      // Compression.

      if ( ::TIFFGetField( m_fileData->handle, TIFFTAG_COMPRESSION, &tiffUInt16 ) )
      {
         switch ( tiffUInt16 )
         {
         case COMPRESSION_NONE:
            m_tiffOptions.compression = TIFFCompression::None;
            break;
         case COMPRESSION_DEFLATE:
            m_tiffOptions.compression = TIFFCompression::ZIP;
            break;
         case COMPRESSION_LZW:
            m_tiffOptions.compression = TIFFCompression::LZW;
            break;
         default:
            m_tiffOptions.compression = TIFFCompression::Unknown;
            break;
         }
      }
      else
         m_tiffOptions.compression = TIFFCompression::None;

      // Planar configuration.
      m_tiffOptions.planar = ::TIFFGetField( m_fileData->handle, TIFFTAG_PLANARCONFIG, &tiffUInt16 ) &&
                           tiffUInt16 == PLANARCONFIG_SEPARATE;

      // Resolution units: metric (centimeters) or English (inches).
      m_image.options.metricResolution = ::TIFFGetField( m_fileData->handle, TIFFTAG_RESOLUTIONUNIT, &tiffUInt16 ) &&
                           tiffUInt16 == RESUNIT_CENTIMETER;

      // TIFF specifies a separate resolution for each axis.
      // Resolution values are stored as float values in the TIFF file, so we
      // need a temporary float variable.

      if ( ::TIFFGetField( m_fileData->handle, TIFFTAG_XRESOLUTION, &tiffFloat ) )
         m_image.options.xResolution = tiffFloat;
      else
         m_image.options.xResolution = 0;

      if ( ::TIFFGetField( m_fileData->handle, TIFFTAG_YRESOLUTION, &tiffFloat ) )
         m_image.options.yResolution = tiffFloat;
      else
         m_image.options.yResolution = 0;

      // Some programs out there don't write correct resolution data.
      if ( m_image.options.xResolution <= 0 || m_image.options.yResolution <= 0 )
         m_image.options.xResolution = m_image.options.yResolution = m_image.options.metricResolution ? 72/2.54 : 72;

      // Extra samples information

      m_tiffOptions.premultipliedAlpha = false;

      if ( m_image.info.numberOfChannels > ((m_image.info.colorSpace == ColorSpace::RGB) ? 3 : 1) )
      {
         uint16* sampleinfo;
         uint16 extrasamples;
         if ( ::TIFFGetField( m_fileData->handle, TIFFTAG_EXTRASAMPLES, &extrasamples, &sampleinfo ) )
            m_tiffOptions.premultipliedAlpha = *sampleinfo == EXTRASAMPLE_ASSOCALPHA;
      }

      // Optional information literals.

      if ( ::TIFFGetField( m_fileData->handle, TIFFTAG_SOFTWARE, &tiffStr ) )
         m_tiffOptions.software = tiffStr;
      else
         m_tiffOptions.software.Clear();

      if ( ::TIFFGetField( m_fileData->handle, TIFFTAG_IMAGEDESCRIPTION, &tiffStr ) )
         m_tiffOptions.imageDescription = tiffStr;
      else
         m_tiffOptions.imageDescription.Clear();

      if ( ::TIFFGetField( m_fileData->handle, TIFFTAG_COPYRIGHT, &tiffStr ) )
         m_tiffOptions.copyright = tiffStr;
      else
         m_tiffOptions.copyright.Clear();

      // Additional m_image.options from EXIF data (libtiff >= 3.8.0)

      if ( ::TIFFGetField( m_fileData->handle, TIFFTAG_EXIFIFD, &tiffUInt32 ) )
      {
         tdir_t dir = ::TIFFCurrentDirectory( m_fileData->handle );

         if ( ::TIFFReadEXIFDirectory( m_fileData->handle, tiffUInt32 ) )
         {
            if ( ::TIFFGetField( m_fileData->handle, EXIFTAG_ISOSPEEDRATINGS, &tiffUInt32, &tiffStr ) )
               m_image.options.isoSpeed = int( *((uint16*)tiffStr) );

            if ( ::TIFFGetField( m_fileData->handle, EXIFTAG_EXPOSURETIME, &tiffFloat ) )
               m_image.options.exposure = tiffFloat;

            if ( ::TIFFGetField( m_fileData->handle, EXIFTAG_APERTUREVALUE, &tiffFloat ) )
               m_image.options.aperture = Pow2( tiffFloat/2 );

            if ( ::TIFFGetField( m_fileData->handle, EXIFTAG_FOCALLENGTH, &tiffFloat ) )
               m_image.options.focalLength = tiffFloat; // mm

            if ( ::TIFFGetField( m_fileData->handle, EXIFTAG_CFAPATTERN, &tiffUInt32, &tiffStr ) )
               if ( tiffUInt32 == 8 && *((uint32*)tiffStr) == 0x00020002u ) // support RGB CFAs only
                  switch ( *((uint32*)(tiffStr + 4)) )
                  {
                  case 0x00010102u : m_image.options.cfaType = CFAType::BGGR; break;
                  case 0x01020001u : m_image.options.cfaType = CFAType::GRBG; break;
                  case 0x01000201u : m_image.options.cfaType = CFAType::GBRG; break;
                  case 0x02010100u : m_image.options.cfaType = CFAType::RGGB; break;
                  default          : m_image.options.cfaType = CFAType::None; break;
                  }
         }

        ::TIFFSetDirectory( m_fileData->handle, dir );
      }
   }
   catch ( ... )
   {
      Close();
      throw;
   }
}

// ----------------------------------------------------------------------------

ICCProfile TIFFReader::ReadICCProfile()
{
   if ( !IsOpen() )
      throw TIFF::InvalidReadOperation( String() );

   uint32 byteCount;
   void* iccData;
   if ( ::TIFFGetField( m_fileData->handle, TIFFTAG_ICCPROFILE, &byteCount, &iccData ) )
      return ICCProfile( iccData );
   return ICCProfile();
}

// ----------------------------------------------------------------------------

template <class P>
static void ReadTIFFImage( GenericImage<P>& image, TIFFReader& reader, TIFFFileData* fileData )
{
   if ( !reader.IsOpen() )
      throw TIFF::InvalidReadOperation( String() );

   try
   {
      //
      // Read pixel data
      //

      // Allocate space.
      // Don't trust m_image.info fields since they are publicly accessible.
      image.AllocateData( fileData->width, fileData->length, fileData->samplesPerPixel,
         (fileData->photometric == PHOTOMETRIC_RGB) ? ColorSpace::RGB : ColorSpace::Gray );

      // Determine if we are dealing with a planar image. This is from our
      // algorithmic POV, which includes when we have a single channel to read.
      bool isPlanar = fileData->planarConfig == PLANARCONFIG_SEPARATE ||
                      image.NumberOfChannels() == 1;

      // Are we dealing with signed integer samples?
      bool isSignedIntSample = fileData->sampleFormat == SAMPLEFORMAT_INT;

      // Are we dealing with floating-point samples?
      bool isFloatSample = fileData->sampleFormat == SAMPLEFORMAT_IEEEFP;

      // Begin reading pixels.

      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( String().Format(
                  "Reading TIFF: %d-bit %s, %d channel(s), %dx%d pixels, %s",
                  fileData->bitsPerSample,
                  isFloatSample ? "floating point" : "integers",
                  fileData->samplesPerPixel,
                  fileData->width, fileData->length,
                  (fileData->planarConfig == PLANARCONFIG_SEPARATE) ? "planar" : "chunky" ),
         image.NumberOfSamples() );

      // If we are reading a floating-point TIFF file into an integer image, we
      // use the declared normalization range to rescale floating-point sample
      // values to [0,1] before expanding them to the native integer range.
      double k = 1.0;
      if ( isFloatSample && !P::IsFloatSample() )
         k /= fileData->lowerRange - fileData->upperRange;

      if ( isPlanar ) // separate planes or grayscale
      {
         if ( P::IsFloatSample() == isFloatSample && P::BitsPerSample() == fileData->bitsPerSample )
         {
            // File sample type same as image sample type.
            // Planar organization.

            for ( int c = 0; c < image.NumberOfChannels(); ++c )
               for ( int i = 0; i < image.Height(); ++i, image.Status() += image.Width() )
                  if ( ::TIFFReadScanline( fileData->handle, image.ScanLine( i, c ), i, c ) < 0 )
                     throw TIFF::FileReadError( reader.Path() );
         }
         else
         {
            // File sample type and image sample type are different.
            // Planar organization.

            size_type tiffLineSize = image.Width() * (fileData->bitsPerSample >> 3);
            ByteArray buffer( tiffLineSize );

            for ( int c = 0; c < image.NumberOfChannels(); ++c )
               for ( int i = 0; i < image.Height(); ++i, image.Status() += image.Width() )
               {
                  if ( ::TIFFReadScanline( fileData->handle, buffer.Begin(), i, c ) < 0 )
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
                           switch ( fileData->bitsPerSample )
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
                           switch ( fileData->bitsPerSample )
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
                           switch ( fileData->bitsPerSample )
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
                           switch ( fileData->bitsPerSample )
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

         unsigned tiffNumberOfStrips = ::TIFFNumberOfStrips( fileData->handle );

         uint32 tiffRowsPerStrip;
         if ( tiffNumberOfStrips > 1 )
            ::TIFFGetField( fileData->handle, TIFFTAG_ROWSPERSTRIP, &tiffRowsPerStrip );
         else
            tiffRowsPerStrip = uint32( image.Height() );

         ByteArray buffer( ::TIFFStripSize( fileData->handle ) );

         Array<typename P::sample*> v( image.NumberOfChannels() );

         // Main loop over strips.
         for ( unsigned s = 0, y = 0; s < tiffNumberOfStrips; ++s )
         {
            if ( ::TIFFReadEncodedStrip( fileData->handle, s, buffer.Begin(), -1 ) < 0 )
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

               if ( P::IsFloatSample() == isFloatSample && P::BitsPerSample() == fileData->bitsPerSample )
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
                              switch ( fileData->bitsPerSample )
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
                              switch ( fileData->bitsPerSample )
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
                              switch ( fileData->bitsPerSample )
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
                              switch ( fileData->bitsPerSample )
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
   if ( m_image.options.readNormalized )                                                              \
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
      if ( m_fileData->sampleFormat == SAMPLEFORMAT_IEEEFP )                                          \
      {                                                                                               \
         /* For floating-point TIFFs we have no standard way to know the actual     */                \
         /* data range in advance. If existing pixel values fit into the            */                \
         /* normalization range, then we assume they are already normalized         */                \
         /* and just do nothing. If this is not the case, we have to rescale.       */                \
                                                                                                      \
         /* Obtain extreme pixel values.                                            */                \
         I::sample mn, mx;                                                                            \
         image.GetExtremePixelValues( mn, mx );                                                       \
         zeroOffset = Min( double( m_fileData->lowerRange ), double( mn ) );                          \
         scaleRange = Max( double( m_fileData->upperRange ), double( mx ) );                          \
      }                                                                                               \
      else                                                                                            \
      {                                                                                               \
         /* For integer samples we always know scaling parameters in advance.       */                \
                                                                                                      \
         if ( m_fileData->sampleFormat == SAMPLEFORMAT_INT )  /* signed integers ?          */        \
         {                                                                                            \
            zeroOffset = BitMin( m_fileData->bitsPerSample );                                         \
            scaleRange = BitMax( m_fileData->bitsPerSample );                                         \
         }                                                                                            \
         else                                                                                         \
         {                                                                                            \
            zeroOffset = 0;                                                                           \
            scaleRange = UBitMax( m_fileData->bitsPerSample );                                        \
         }                                                                                            \
      }                                                                                               \
                                                                                                      \
      /* If necessary, rescale to the normalized range.                             */                \
                                                                                                      \
      if ( zeroOffset != m_fileData->lowerRange || scaleRange != m_fileData->upperRange )             \
      {                                                                                               \
         double iDelta = scaleRange - zeroOffset;                                                     \
         double oDelta = m_fileData->upperRange - m_fileData->lowerRange;                             \
         for ( int c = 0; c < image.NumberOfChannels(); ++c )                                         \
            if ( oDelta != 1 || m_fileData->lowerRange != 0 )                                         \
            {                                                                                         \
               double rDelta = oDelta/iDelta;                                                         \
               for ( I::sample_iterator i( image, c ); i; ++i )                                       \
                  *i = I::sample( (*i - zeroOffset)*rDelta + m_fileData->lowerRange );                \
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
   ReadTIFFImage( image, *this, m_fileData );
   NORMALIZE( FImage )
}

void TIFFReader::ReadImage( DImage& image )
{
   ReadTIFFImage( image, *this, m_fileData );
   NORMALIZE( DImage )
}

void TIFFReader::ReadImage( UInt8Image& image )
{
   ReadTIFFImage( image, *this, m_fileData );
}

void TIFFReader::ReadImage( UInt16Image& image )
{
   ReadTIFFImage( image, *this, m_fileData );
}

void TIFFReader::ReadImage( UInt32Image& image )
{
   ReadTIFFImage( image, *this, m_fileData );
}

#undef NORMALIZE

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

TIFFWriter::TIFFWriter()
{
}

// ----------------------------------------------------------------------------

TIFFWriter::~TIFFWriter() noexcept( false )
{
}

// ----------------------------------------------------------------------------

bool TIFFWriter::IsOpen() const
{
   return m_fileData && !m_path.IsEmpty();
}

// ----------------------------------------------------------------------------

void TIFFWriter::Close()
{
   TIFF::CloseStream();
}

// ----------------------------------------------------------------------------

void TIFFWriter::Create( const String& filePath )
{
   if ( IsOpen() )
      throw InvalidWriteOperation( m_path );

   if ( filePath.IsEmpty() )
      throw InvalidFilePath( filePath );

   Reset();

   if ( !m_fileData )
      m_fileData = new TIFFFileData;

   m_path = File::WindowsPathToUnix( filePath );
}

// ----------------------------------------------------------------------------

void TIFFWriter::WriteICCProfile( const ICCProfile& icc )
{
   if ( !IsOpen() )
      throw TIFF::WriterNotInitialized( String() );

   m_iccProfile = icc;
}

// ----------------------------------------------------------------------------

template <class P>
static void WriteTIFFImage( const GenericImage<P>& image, TIFFWriter& writer,
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
   {
      ImageOptions options = writer.Options();
      switch ( options.bitsPerSample )
      {
      default:
         options.bitsPerSample = 16;
      case 8:
      case 16:
         options.ieeefpSampleFormat = false;
      case 32:
         break;
      case 64:
         options.ieeefpSampleFormat = true;
         break;
      }
      writer.SetOptions( options );
   }

   fileData->bitsPerSample = writer.Options().bitsPerSample;

   // TIFF sample format.
   // We support signed integers for input, but not for output.

   fileData->sampleFormat = writer.Options().ieeefpSampleFormat ?
                                    SAMPLEFORMAT_IEEEFP : SAMPLEFORMAT_UINT;

   // TIFF color space.
   // For RGB images, we'll write a grayscale TIFF image (possibly with
   // additional alpha channels) if the three RGB nominal channels are not
   // selected.

   fileData->photometric = (image.ColorSpace() == ColorSpace::Gray ||
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

   fileData->planarConfig = tiffPlanar ? PLANARCONFIG_SEPARATE : PLANARCONFIG_CONTIG;

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

      fileData->width = width;
      fileData->length = height;
      fileData->samplesPerPixel = image.NumberOfSelectedChannels();

      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( String().Format(
                  "Writing TIFF: %d-bit %s, %d channel(s), %dx%d pixels, %s",
                  writer.Options().bitsPerSample,
                  writer.Options().ieeefpSampleFormat ? "floating point" : "integers",
                  fileData->samplesPerPixel,
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
      if ( (fileData->handle = ::TIFFOpen( path8.c_str(), "w" )) == nullptr )
         throw TIFF::UnableToCreateFile( writer.Path() );

      //
      // Mandatory TIFF tags.
      //

      // Change this when porting PCL to Mac OSX, or, better, define a global
      // macro in Defs.h :)
      ::TIFFSetField( fileData->handle, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB );

      // We always generate top->bottom, left->right oriented images.
      ::TIFFSetField( fileData->handle, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT );

      // We write only RGB and grayscale TIFF images.
      // N.B.: This field has already been corrected as a function of the
      // image's currently selected channels.
      ::TIFFSetField( fileData->handle, TIFFTAG_PHOTOMETRIC, fileData->photometric );

      // We do support both planar and chunky organizations.
      // N.B.: This field has already been corrected to avoid marking grayscale
      // image files as planar.
      ::TIFFSetField( fileData->handle, TIFFTAG_PLANARCONFIG, fileData->planarConfig );

      // Image dimensions in pixels.
      ::TIFFSetField( fileData->handle, TIFFTAG_IMAGEWIDTH, width );
      ::TIFFSetField( fileData->handle, TIFFTAG_IMAGELENGTH, height );

      // Number of bits per sample.
      // N.B.: This field has already been checked to ensure a correct value.
      ::TIFFSetField( fileData->handle, TIFFTAG_BITSPERSAMPLE, writer.Options().bitsPerSample );

      // Sample format: unsigned integers or IEEE floating-point.
      // N.B.: This value has been deduced from corrected options.bitsPerSample.
      ::TIFFSetField( fileData->handle, TIFFTAG_SAMPLEFORMAT, fileData->sampleFormat );

      // Number of samples per pixel.
      ::TIFFSetField( fileData->handle, TIFFTAG_SAMPLESPERPIXEL, image.NumberOfSelectedChannels() );

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
         ::TIFFSetField( fileData->handle, TIFFTAG_EXTRASAMPLES, na, extra.Begin() );
      }
      else if ( image.FirstSelectedChannel() > 0 && image.NumberOfSelectedChannels() > 1 )
      {
         // Extra samples, all of them unspecified
         int na = image.NumberOfSelectedChannels() - 1;
         Array<uint16> extra( (size_type)na );
         for ( int i = 0; i < na; ++i )
            extra[i] = EXTRASAMPLE_UNSPECIFIED;
         ::TIFFSetField( fileData->handle, TIFFTAG_EXTRASAMPLES, na, extra.Begin() );
      }

      // Number of pixel rows per TIFF strip. The stripSize static variable
      // controls the strip size in bytes.
      ::TIFFSetField( fileData->handle, TIFFTAG_ROWSPERSTRIP,
            Max( 1, writer.TIFFOptions().stripSize/(width*tiffSampleSize) ) );

      /*
      // No, MinSampleValue and MaxSampleValue are not intended to represent
      // the available dynamic range - sigh!
      if ( writer.Options().ieeefpSampleFormat )
      {
         typename P::sample mn, mx;
         image.GetExtremePixelValues( mn, mx );
         ::TIFFSetField( fileData->handle, TIFFTAG_SMINSAMPLEVALUE, double( mn ) );
         ::TIFFSetField( fileData->handle, TIFFTAG_SMAXSAMPLEVALUE, double( mx ) );
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

         ::TIFFSetField( fileData->handle, TIFFTAG_COMPRESSION, tiffCompression );

         // For 8 and 16 bit samples, use LZW and ZIP encoded data with
         // horizontal differencing. Unfortunately this is not supported for
         // 32-bit samples.
         if ( fileData->bitsPerSample < 32 )
            ::TIFFSetField( fileData->handle, TIFFTAG_PREDICTOR, 2 );
      }

      // Metric or English resolution units. Default is English (inches).
      ::TIFFSetField( fileData->handle, TIFFTAG_RESOLUTIONUNIT,
         writer.Options().metricResolution ? RESUNIT_CENTIMETER : RESUNIT_INCH );

      // Horizontal resolution. <= zero means no specified resolution.
      if ( writer.Options().xResolution > 0 )
         ::TIFFSetField( fileData->handle, TIFFTAG_XRESOLUTION, float( writer.Options().xResolution ) );

      // Vertical resolution. <= zero means no specified resolution.
      if ( writer.Options().yResolution > 0 )
         ::TIFFSetField( fileData->handle, TIFFTAG_YRESOLUTION, float( writer.Options().yResolution ) );

      // Software description and version information.
      // We are not forcing our own signature here... this might be our good
      // action of the day :)
      if ( !writer.TIFFOptions().software.IsEmpty() )
      {
         IsoString s = writer.TIFFOptions().software.ToUTF8();
         ::TIFFSetField( fileData->handle, TIFFTAG_SOFTWARE, s.c_str() );
      }

      // Place a description of this image here.
      if ( !writer.TIFFOptions().imageDescription.IsEmpty() )
      {
         IsoString s = writer.TIFFOptions().imageDescription.ToUTF8();
         ::TIFFSetField( fileData->handle, TIFFTAG_IMAGEDESCRIPTION, s.c_str() );
      }

      // Copyright info for this image comes here.
      if ( !writer.TIFFOptions().copyright.IsEmpty() )
      {
         IsoString s = writer.TIFFOptions().copyright.ToUTF8();
         ::TIFFSetField( fileData->handle, TIFFTAG_COPYRIGHT, s.c_str() );
      }

      // ICC Profile.

      if ( writer.Options().embedICCProfile && icc.IsProfile() )
      {
         // Retrieve the length in bytes of the ICC profile structure.
         uint32 byteCount = uint32( icc.ProfileSize() );

         // Don't try to write an empty ICC profile.
         if ( byteCount != 0 )
            ::TIFFSetField( fileData->handle, TIFFTAG_ICCPROFILE, byteCount, icc.ProfileData().Begin() );
      }

      //
      // Write pixel data.
      //

      int nc = image.NumberOfSelectedChannels();
      int c0 = image.FirstSelectedChannel();
      int ca = (hasAlpha && writer.TIFFOptions().premultipliedAlpha) ? image.NumberOfNominalChannels() : -1;

      if ( P::IsFloatSample() == writer.Options().ieeefpSampleFormat &&
           P::BitsPerSample() == fileData->bitsPerSample )
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

                  if ( ::TIFFWriteScanline( fileData->handle, buffer.Begin(), y, c ) < 0 )
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

               if ( ::TIFFWriteScanline( fileData->handle, buffer.Begin(), y, 0 ) < 0 )
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

                  if ( ::TIFFWriteScanline( fileData->handle, buffer.Begin(), y, c ) < 0 )
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

               if ( ::TIFFWriteScanline( fileData->handle, buffer.Begin(), y, 0 ) < 0 )
                  throw TIFF::FileWriteError( writer.Path() );
            }
         }
      }

      ::TIFFClose( fileData->handle ), fileData->handle = nullptr;
   }
   catch ( ... )
   {
      if ( fileData->handle != nullptr )
         ::TIFFClose( fileData->handle ), fileData->handle = nullptr;
      throw;
   }
}

void TIFFWriter::WriteImage( const FImage& image )
{
   WriteTIFFImage( image, *this, m_iccProfile, m_fileData );
}

void TIFFWriter::WriteImage( const DImage& image )
{
   WriteTIFFImage( image, *this, m_iccProfile, m_fileData );
}

void TIFFWriter::WriteImage( const UInt8Image& image )
{
   WriteTIFFImage( image, *this, m_iccProfile, m_fileData );
}

void TIFFWriter::WriteImage( const UInt16Image& image )
{
   WriteTIFFImage( image, *this, m_iccProfile, m_fileData );
}

void TIFFWriter::WriteImage( const UInt32Image& image )
{
   WriteTIFFImage( image, *this, m_iccProfile, m_fileData );
}

// ----------------------------------------------------------------------------

}  // pcl

// ----------------------------------------------------------------------------
// EOF TIFF.cpp - Released 2017-07-09T18:07:25Z
