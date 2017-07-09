//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 01.01.05.0399
// ----------------------------------------------------------------------------
// FITS.cpp - Released 2017-07-09T18:07:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
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

#include "FITS.h"

#ifdef __PCL_WINDOWS
# include <pcl/AutoLock.h>
#endif
#include <pcl/Console.h>
#include <pcl/ErrorHandler.h>
#include <pcl/ICCProfile.h>
#include <pcl/MetaModule.h>
#include <pcl/StdStatus.h>
#include <pcl/Version.h>

// Tell fitsio.h that we do support 64-bit integers.
#define HAVE_LONGLONG   1

#include <cfitsio/fitsio.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * ### N.B.: The Windows implementation of CFITSIO has caused thread safety
 * problems. For sanity, we prevent concurrent accesses to CFITSIO on Windows.
 */
#ifdef __PCL_WINDOWS
static Mutex s_mutex;
# define CFITSIO_LOCK volatile AutoLock lock( s_mutex );
#else
# define CFITSIO_LOCK
#endif

// ----------------------------------------------------------------------------

String FITS::Error::Message() const
{
   String message = ErrorMessage();

   String filePath = FilePath();
   if ( !filePath.IsEmpty() )
   {
      message += ": ";
      message += filePath;
   }

   IsoString cfitsioMsgStack;
   char cfitsioMsg[ 96 ]; // 80 should suffice, according to cfitsio's doc.
   {
      CFITSIO_LOCK
      for ( int i = 1; ::fits_read_errmsg( cfitsioMsg ) != 0; ++i )
      {
         cfitsioMsgStack += IsoString().Format( "\n%02d : ", i );
         cfitsioMsgStack += cfitsioMsg;
      }
   }
   if ( !cfitsioMsgStack.IsEmpty() )
      message += "\nCFITSIO error message stack: " + cfitsioMsgStack;

   return message;
}

// ----------------------------------------------------------------------------

struct FITSFileData
{
   void* fits         = nullptr; // CFITSIO's ::fitsfile*
   mutable int status = 0;       // CFITSIO's persistent error code

   FITSFileData() = default;

   ~FITSFileData()
   {
      if ( fits != nullptr )
      {
         CFITSIO_LOCK
         ::fits_close_file( (::fitsfile*)fits, &status );
         fits = nullptr;
      }
   }
};

// ----------------------------------------------------------------------------

struct FITSHDUData
{
   int                        bitpix           = 0;           // file sample type
   int                        equivBitpix      = 0;           // equivalent sample type
   int                        naxis            = 0;           // image dimensions: 0=empty, 2=grayscale, 3=RGBA or 3=gray+alpha
   long                       naxes[3]         = { 0, 0, 0 }; // width, height, number of channels
   AbstractImage::color_space colorSpace       = ColorSpace::Unknown; // working safe copy
   int                        hduNumber        = -1;          // index of this HDU in the FITS file (zero-based)
   double                     zeroOffset       = 0;           // BZERO
   double                     scaleRange       = 1;           // BSCALE
   bool                       signedIsPhysical = false;       // signed integer values store physical pixel data
   IsoString                  iccExtName;                     // name of ICC profile extension
   IsoString                  thumbExtName;                   // name of thumbnail image extension
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FITS::FITS()
{
}

FITS::~FITS() noexcept( false )
{
   CloseStream();
}

bool FITS::IsOpenStream() const
{
   return m_fileData && m_fileData->fits != nullptr;
}

void FITS::CloseStream() // ### N.B.: Derived must call base
{
   m_fileData.Destroy();
}

// ----------------------------------------------------------------------------

#define fits_handle  ((::fitsfile*)m_fileData->fits)
#define fitsStatus   (m_fileData->status)

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct FITSReaderData
{
   Array<FITSHDUData>      hdus;
   Array<FITSKeywordArray> keywords;
   Array<FITSImageOptions> fitsOptions;
};

// ----------------------------------------------------------------------------

#define hdus        m_data->hdus
#define keywords    m_data->keywords
#define fitsOptions m_data->fitsOptions

// ----------------------------------------------------------------------------

class FITSReaderPrivate
{
public:

   template <class P> inline
   static void ReadImage( GenericImage<P>& image, FITSReader& reader )
   {
      if ( !reader.IsOpen() )
         throw FITS::InvalidReadOperation( String() );

      if ( reader.m_index >= int( reader.hdus.Length() ) )
         throw FITS::InvalidReadOperation( reader.m_path );

      const FITSHDUData& hdu = reader.hdus[reader.m_index];

      if ( hdu.naxis == 0 ) // empty image ?
      {
         image.FreeData();
         return;
      }

      if ( !reader.Info().IsValid() )
         throw FITS::InvalidImage( reader.m_path );

      if ( reader.Options().lowerRange >= reader.Options().upperRange )
         throw FITS::InvalidNormalizationRange( reader.m_path );

      FITSFileData* m_fileData = reader.m_fileData;

      try
      {
         // Restore CFITSIO's current HDU to the current image HDU.
         // N.B.: CFITSIO expects HDU numbers relative to 1.
         {
            CFITSIO_LOCK
            ::fits_movabs_hdu( fits_handle, hdu.hduNumber+1, 0, &fitsStatus );
         }
         if ( fitsStatus != 0 )
            throw FITS::UnableToAccessCurrentHDU( reader.m_path );

         // Allocate pixel data.
         // Don't trust info fields since they are publicly accessible.
         image.AllocateData( hdu.naxes[0], hdu.naxes[1],
                             (hdu.naxis == 2) ? 1 : hdu.naxes[2],
                             typename GenericImage<P>::color_space( hdu.colorSpace ) );

         /*
          * Read pixels
          */
         StatusMonitor monitor;
         StandardStatus status;
         if ( reader.FITSOptions().verbosity > 0 )
         {
            monitor.SetCallback( &status );
            monitor.Initialize( String().Format(
                  "Reading FITS image: %d-bit %s, %d channel(s), %dx%d pixels",
                  reader.Options().bitsPerSample,
                  reader.Options().ieeefpSampleFormat ? "floating point" : "integers",
                        image.NumberOfChannels(), image.Width(), image.Height() ),
                  image.NumberOfSamples() );
         }

         // To support 32-bit integer samples and to provide for arbitrary integer
         // format output, we'll ask FITSIO to store 64-bit floating point pixel
         // values in a temporary row buffer.
         F64Vector buffer( image.Width() );

         // A rescaling operation is required for integer sample values if the
         // source data type (as provided by FITSIO, i.e taking BZERO into account)
         // doesn't match the target image's sample type.
         double k = 1; // calm compilers
         bool rescalingRequired = hdu.zeroOffset != P::MinSampleValue() || hdu.scaleRange != P::MaxSampleValue();
         if ( rescalingRequired )
            k = (double( P::MaxSampleValue() ) - P::MinSampleValue())/(hdu.scaleRange - hdu.zeroOffset);

         // Coordinate selectors.
         // The primary image HDU is assumed to be FITSIO's current HDU.
         // N.B.: CFITSIO expects one-based indexes.
         long fpixel[ 3 ];
         fpixel[0] = 1;
         fpixel[2] = 0;

         // For each channel
         for ( int c = 0; c < image.NumberOfChannels(); ++c )
         {
            ++fpixel[2]; // next channel

            fpixel[1] = 0;

            // For each row
            for ( int i = 0; i < image.Height(); ++i )
            {
               ++fpixel[1]; // next row

               // Read a row of pixels.
               {
                  CFITSIO_LOCK
                  ::fits_read_pix( fits_handle, TDOUBLE, fpixel, image.Width(), 0, buffer.Begin(), 0, &fitsStatus );
               }
               if ( fitsStatus != 0 )
                  throw FITS::FileReadError( reader.m_path );

               // When reading floating point images, replace NaN and infinity IEEE 754 entities
               // with zeros. For sanity, we perform this cleaning task for all image types, as
               // we are forcing CFITSIO to read pixels to a floating point buffer.
               for ( int j = 0; j < image.Width(); ++j )
                  if ( !IsFinite( buffer[j] ) )
                     buffer[j] = 0;

               // Optional truncation of negative pixel values for signed integer FITS files.
               // This is just for compatibility with writers that store raw (physical) pixel
               // data as signed integers.
               if ( hdu.signedIsPhysical )
                  for ( int j = 0; j < image.Width(); ++j )
                     if ( buffer[j] < 0 )
                        buffer[j] = 0;

               // Transfer pixels to the image.
               // Mirror pixel rows vertically if we are loading with bottom-up orientation.
               typename P::sample* f = image.ScanLine( reader.FITSOptions().bottomUp ? image.Height()-i-1 : i, c );
               if ( rescalingRequired )
                  for ( int j = 0; j < image.Width(); ++j )
                     f[j] = P::FloatToSample( k*(buffer[j] - hdu.zeroOffset) + P::MinSampleValue() );
               else
                  for ( int j = 0; j < image.Width(); ++j )
                     f[j] = typename P::sample( buffer[j] );

               if ( reader.FITSOptions().verbosity > 0 )
                  monitor += image.Width();
            }
         }
      }
      catch ( ... )
      {
         image.FreeData();
         reader.Close();
         throw;
      }
   }

   template <class T, class P> inline
   static void ReadSamples( T* f, P*, int startRow, int rowCount, int c, FITSReader& reader )
   {
      if ( !reader.IsOpen() )
         throw FITS::InvalidReadOperation( String() );

      if ( reader.m_index >= int( reader.hdus.Length() ) )
         throw FITS::InvalidReadOperation( reader.m_path );

      const FITSHDUData& hdu = reader.hdus[reader.m_index];

      if ( hdu.naxis == 0 ||
             startRow < 0 || startRow+rowCount > hdu.naxes[1] ||
                    c < 0 || c >= ((hdu.naxis == 2) ? 1 : hdu.naxes[2]) )
         throw FITS::ReadCoordinatesOutOfRange( reader.m_path );

      if ( reader.Options().lowerRange >= reader.Options().upperRange )
         throw FITS::InvalidNormalizationRange( reader.m_path );

      if ( reader.FITSOptions().bottomUp )
         startRow = hdu.naxes[1] - startRow - rowCount;

      FITSFileData* m_fileData = reader.m_fileData;

      try
      {
         // Restore CFITSIO's current HDU to the current image HDU.
         // N.B.: CFITSIO expects HDU numbers relative to 1.
         {
            CFITSIO_LOCK
            ::fits_movabs_hdu( fits_handle, hdu.hduNumber+1, 0, &fitsStatus );
         }
         if ( fitsStatus != 0 )
            throw FITS::UnableToAccessCurrentHDU( reader.m_path );

         // Number of samples to read.
         long N = hdu.naxes[0]*rowCount;

         // To support 32-bit integer samples and to provide for arbitrary integer
         // format output, we'll ask FITSIO to store 64-bit floating point pixel
         // values in a temporary row buffer.
         F64Vector buffer( N );

         // A rescaling operation is required for integer sample values if the
         // source data type (as provided by FITSIO, i.e taking BZERO into account)
         // doesn't match the target image's sample type.
         double k = 1; // calm compilers
         bool rescalingRequired = hdu.zeroOffset != P::MinSampleValue() || hdu.scaleRange != P::MaxSampleValue();
         if ( rescalingRequired )
            k = (double( P::MaxSampleValue() ) - P::MinSampleValue())/(hdu.scaleRange - hdu.zeroOffset);

         // Coordinate selectors.
         // N.B.: CFITSIO expects one-based indexes.
         long fpixel[ 3 ];
         fpixel[0] = 1;
         fpixel[1] = startRow + 1;
         fpixel[2] = c + 1;

         // Read a strip of FITS rows.
         {
            CFITSIO_LOCK
            ::fits_read_pix( fits_handle, TDOUBLE, fpixel, N, 0, buffer.Begin(), 0, &fitsStatus );
         }
         if ( fitsStatus != 0 )
            throw FITS::FileReadError( reader.m_path );

         // When reading floating point images, replace NaN and infinity IEEE 754 entities
         // with zeros. For sanity, we perform this cleaning task for all image types, as
         // we are forcing CFITSIO to read pixels to a floating point buffer.
         for ( int i = 0; i < N; ++i )
            if ( !IsFinite( buffer[i] ) )
               buffer[i] = 0;

         // Optional truncation of negative pixel values for signed integer FITS files.
         // This is just for compatibility with writers that store raw (physical) pixel
         // data as signed integers.
         if ( hdu.signedIsPhysical )
            for ( int i = 0; i < N; ++i )
               if ( buffer[i] < 0 )
                  buffer[i] = 0;

         // Transfer pixels from the buffer to the image.
         if ( rescalingRequired )
            for ( int i = 0; i < N; ++i )
               f[i] = P::FloatToSample( k*(buffer[i] - hdu.zeroOffset) + P::MinSampleValue() );
         else
            for ( int i = 0; i < N; ++i )
               f[i] = typename P::sample( buffer[i] );

         // Mirror pixel rows vertically if we are loading with bottom-up orientation.
         if ( reader.FITSOptions().bottomUp )
            for ( int t = 0, b = rowCount; --b > t; ++t )
               for ( int i = t*hdu.naxes[0], j = b*hdu.naxes[0], i1 = i + hdu.naxes[0]; i != i1; ++i, ++j )
                  Swap( f[i], f[j] );
      }
      catch ( ... )
      {
         reader.Close();
         throw;
      }
   }
};

// ----------------------------------------------------------------------------

FITSReader::FITSReader()
{
}

FITSReader::~FITSReader() noexcept( false )
{
   Close();
}

bool FITSReader::IsOpen() const
{
   return FITS::IsOpenStream();
}

void FITSReader::Close()
{
   m_data.Destroy();
   m_index = 0;
   FITS::CloseStream();
}

void FITSReader::Open( const String& filePath )
{
   if ( IsOpen() )
      throw FITS::InvalidReadOperation( m_path );

   if ( filePath.IsEmpty() )
      throw FITS::InvalidFilePath( filePath );

   try
   {
      if ( !m_fileData )
         m_fileData = new FITSFileData;

      if ( !m_data )
         m_data = new FITSReaderData;

#ifdef __PCL_WINDOWS
      m_path = File::WindowsPathToUnix( filePath );
#else
      m_path = filePath;
#endif

      // Open the FITS image file stream for reading.

      IsoString path8 =
#ifdef __PCL_WINDOWS
         File::UnixPathToWindows( m_path ).ToMBS();
#else
         m_path.ToUTF8();
#endif
      fitsStatus = 0;
      {
         CFITSIO_LOCK
         ::fits_open_diskfile( (::fitsfile**)&m_fileData->fits, path8.c_str(), READONLY, &fitsStatus );
      }
      if ( fitsStatus != 0 )
         throw FITS::UnableToOpenFile( m_path );

      // Explore all image HDUs

      int numberOfReadableImages = 0;
      int numberOfHDUs;
      {
         CFITSIO_LOCK
         ::fits_get_num_hdus( fits_handle, &numberOfHDUs, &fitsStatus );
      }
      if ( fitsStatus != 0 )
         throw FITS::FileReadError( m_path );
      if ( numberOfHDUs == 0 )
         throw FITS::UnableToAccessPrimaryHDU( m_path );

      for ( int i = 0; i < numberOfHDUs; ++i )
      {
         int hduType;
         {
            CFITSIO_LOCK
            ::fits_movabs_hdu( fits_handle, i+1, &hduType, &fitsStatus );
         }

         // Skip non-image HDUs
         if ( hduType != IMAGE_HDU )
            continue;

         FITSHDUData hdu;
         FITSKeywordArray keys;
         FITSImageOptions fits;
         ImageDescription image;

         hdu.hduNumber = i;

         {
            CFITSIO_LOCK

            // Get the file sample type.
            ::fits_get_img_type( fits_handle, &hdu.bitpix, &fitsStatus );

            // Get the equivalent image sample type (e.g., BZERO/BSCALE trick for unsigned integers).
            ::fits_get_img_equivtype( fits_handle, &hdu.equivBitpix, &fitsStatus );
         }

         // 64-bit integer FITS files are not supported in this implementation.
         if ( hdu.bitpix == LONGLONG_IMG || hdu.equivBitpix == LONGLONG_IMG )
         {
            if ( FITSOptions().verbosity > 0 )
               Console().WarningLn( String().Format( "<end><cbr>** Skipping unsupported 64-bit integer image in HDU #%d", i+1 ) );
            continue;
         }

         // Get the number of image dimensions.
         // Must be either 0 (empty), 2 (grayscale), or 3 (RGB or gray+alpha)
         {
            CFITSIO_LOCK
            ::fits_get_img_dim( fits_handle, &hdu.naxis, &fitsStatus );
         }
         if ( hdu.naxis > 0 && hdu.naxis != 2 && hdu.naxis != 3 )
         {
            // NAXIS=1 for data extensions, such as ICC profile extensions or data properties.
            if ( hdu.naxis != 1 )
               if ( FITSOptions().verbosity > 0 )
                  Console().WarningLn( String().Format( "<end><cbr>** Skipping image with unsupported dimensions (%d) in HDU #%d", hdu.naxis, i+1 ) );
            continue;
         }

         if ( hdu.naxis > 0 )
         {
            // Get the size of the image for each dimension.
            {
               CFITSIO_LOCK
               ::fits_get_img_size( fits_handle, 3, hdu.naxes, &fitsStatus );
            }

            // Guess a default value for the color space of this image, based on
            // the number of dimensions. Optional keywords may modify this later.
            hdu.colorSpace = (hdu.naxis == 2 || hdu.naxes[2] < 3) ? ColorSpace::Gray : ColorSpace::RGB;

            // Update geometry data
            image.info.width = hdu.naxes[0];
            image.info.height = hdu.naxes[1];
            image.info.numberOfChannels = (hdu.naxis == 2) ? 1 : hdu.naxes[2];

            // If we are here, then the image is supported.
            image.info.supported = true;
         }

         // Set default values for the zero offset and scale range parameters.

         switch ( hdu.equivBitpix )
         {
         case BYTE_IMG:       // 8-bit unsigned integers
            image.options.bitsPerSample = 8;
            image.options.ieeefpSampleFormat = false;
            image.options.signedIntegers = false;
            fits.unsignedIntegers = true;
            hdu.zeroOffset = 0;
            hdu.scaleRange = uint8_max;
            break;

         case SBYTE_IMG:      // 8-bit signed integers
            image.options.bitsPerSample = 8;
            image.options.ieeefpSampleFormat = false;
            image.options.signedIntegers = true;
            fits.unsignedIntegers = false;
            hdu.zeroOffset = int8_min;
            hdu.scaleRange = int8_max;
            break;

         default: // ??
         case SHORT_IMG:      // 16-bit signed integers
            image.options.bitsPerSample = 16;
            image.options.ieeefpSampleFormat = false;
            image.options.signedIntegers = true;
            fits.unsignedIntegers = false;
            hdu.zeroOffset = int16_min;
            hdu.scaleRange = int16_max;
            break;

         case USHORT_IMG:     // 16-bit unsigned integers
            image.options.bitsPerSample = 16;
            image.options.ieeefpSampleFormat = false;
            image.options.signedIntegers = false;
            fits.unsignedIntegers = true;
            hdu.zeroOffset = 0;
            hdu.scaleRange = uint16_max;
            break;

         case LONG_IMG:       // 32-bit signed integers
            image.options.bitsPerSample = 32;
            image.options.ieeefpSampleFormat = false;
            image.options.signedIntegers = true;
            fits.unsignedIntegers = false;
            hdu.zeroOffset = int32_min;
            hdu.scaleRange = int32_max;
            break;

         case ULONG_IMG:      // 32-bit unsigned integers
            image.options.bitsPerSample = 32;
            image.options.ieeefpSampleFormat = false;
            image.options.signedIntegers = false;
            fits.unsignedIntegers = true;
            hdu.zeroOffset = 0;
            hdu.scaleRange = uint32_max;
            break;

         // Integer 64-bit samples not supported.
         // We already have thrown an exception if bitPix == 64 and not ieeefp
         /*
         case LONGLONG_IMG:   // 64-bit signed integers
            image.options.bitsPerSample = 64;
            image.options.ieeefpSampleFormat = false;
            image.options.signedIntegers = true;
            fits.unsignedIntegers = false;
            hdu.zeroOffset = int64_min;
            hdu.scaleRange = int64_max;
            hdu.signedIsPhysical = false;
            break;
         */

         case FLOAT_IMG:      // 32-bit IEEE floating point
            image.options.bitsPerSample = 32;
            image.options.ieeefpSampleFormat = true;
            image.options.signedIntegers = false;
            fits.unsignedIntegers = false;
            hdu.zeroOffset = 0;
            hdu.scaleRange = 1;
            break;

         case DOUBLE_IMG:     // 64-bit IEEE floating point
            image.options.bitsPerSample = 64;
            image.options.ieeefpSampleFormat = true;
            image.options.signedIntegers = false;
            fits.unsignedIntegers = false;
            hdu.zeroOffset = 0;
            hdu.scaleRange = 1;
            break;
         }

         // Complex FITS not supported in this implementation.
         image.options.complexSample = false;

         // Default BZERO and BSCALE values
         fits.zeroOffset = 0;
         fits.scaleRange = 1;

         // Reset these fields in case we don't find meaningful keywords.
         image.options.metricResolution = false;
         image.options.xResolution = image.options.yResolution = 72;

         // When reading floating-point FITS files, turn off CFITSIO's automatic
         // data scaling. Optional normalization of floating-point samples will
         // be done on raw pixel values directly.
         if ( hdu.naxis > 0 && image.options.ieeefpSampleFormat )
         {
            CFITSIO_LOCK
            ::fits_set_bscale( fits_handle, 1, 0, &fitsStatus );
         }

         // Read HDU keywords

         // Learn the length of the keywords list.
         int keysexist;
         {
            CFITSIO_LOCK
            ::fits_get_hdrspace( fits_handle, &keysexist, 0, &fitsStatus );
         }
         if ( fitsStatus != 0 )
            throw FITS::KeywordReadError( m_path );

         // Prepare flags to control presence of two essential keywords.
         int seenBSCALE = 0, seenBZERO = 0;
         bool nonImageExtension = false;

         // Iterate through the list of existing keywords.

         for ( int j = 1; j <= keysexist; ++j ) // CFITSIO wants 1-offset indexes
         {
            char cname[ 81 ], cvalue[ 81 ], ccomment[ 81 ];

            // Read this keyword's elements.
            {
               CFITSIO_LOCK
               ::fits_read_keyn( fits_handle, j, cname, cvalue, ccomment, &fitsStatus );
            }
            if ( fitsStatus != 0 )
               throw FITS::KeywordReadError( m_path );

            // Create a FITSHeaderKeyword instance.
            FITSHeaderKeyword k( cname, cvalue, ccomment );

            // Check out critical reserved keywords.
            // This includes a few of our private PCL keywords.

            if ( !k.name.CompareIC( "BSCALE" ) )
            {
               if ( seenBSCALE++ == 0 ) // ignore multiple instances
                  fits.scaleRange = ::atof( k.value.c_str() );
            }
            else if ( !k.name.CompareIC( "BZERO" ) )
            {
               if ( seenBZERO++ == 0 ) // ignore multiple instances
                  fits.zeroOffset = ::atof( k.value.c_str() );
            }
            else if ( !k.name.CompareIC( "EXTNAME" ) )
            {
               image.id = k.StripValueDelimiters();
            }
            else if ( !k.name.CompareIC( "EXPTIME" ) || !k.name.CompareIC( "EXPOSURE" ) )
            {
               image.options.exposure = ::atof( k.value.c_str() );
            }
            else if ( !k.name.CompareIC( "COLORSPC" ) )
            {
               // *** PCL private keyword: Color Space
               IsoString value = k.StripValueDelimiters();
               if ( !value.CompareIC( "Grayscale" ) || !value.CompareIC( "Gray" ) )
                  hdu.colorSpace = ColorSpace::Gray;
               else if ( !value.CompareIC( "RGB" ) || !value.CompareIC( "RGBColor" ) )
                  hdu.colorSpace = ColorSpace::RGB;
               /* else if ... <-- More color spaces come here */
               else
               {
                  // Warning! We have not generated this keyword!
                  // Invalid/unsupported key value: assuming the grayscale space.
                  hdu.colorSpace = ColorSpace::Gray;
               }
            }
            else if ( !k.name.CompareIC( "RESOLUTN" ) )
            {
               // *** PCL private keyword: Resolution in both axes.
               image.options.xResolution = image.options.yResolution = ::atof( k.value.c_str() );
            }
            else if ( !k.name.CompareIC( "XRESOLTN" ) )
            {
               // *** PCL private keyword: Horizontal resolution.
               image.options.xResolution = ::atof( k.value.c_str() );
            }
            else if ( !k.name.CompareIC( "YRESOLTN" ) )
            {
               // *** PCL private keyword: Vertical resolution.
               image.options.yResolution = ::atof( k.value.c_str() );
            }
            else if ( !k.name.CompareIC( "RESOUNIT" ) )
            {
               // *** PCL private keyword: Resolution units.
               IsoString value = k.StripValueDelimiters();
               image.options.metricResolution =
                  !value.CompareIC( "cm" ) ||
                  !value.CompareIC( "centimeter" ) ||
                  !value.CompareIC( "centimeters" );
            }
            else if ( !k.name.CompareIC( "ICCPROFL" ) )
            {
               // *** PCL private keyword: ICC profile extension.
               IsoString extName = k.StripValueDelimiters();
               if ( extName.IsValidIdentifier() )
                  hdu.iccExtName = extName;
            }
            else if ( !k.name.CompareIC( "THUMBIMG" ) )
            {
               // *** PCL private keyword: Thumbnail image extension.
               IsoString extName = k.StripValueDelimiters();
               if ( extName.IsValidIdentifier() )
                  hdu.thumbExtName = extName;
            }
            else if ( !k.name.CompareIC( "XTENSION" ) )
            {
               IsoString extType = k.StripValueDelimiters();
               if ( extType.CompareIC( "IMAGE" ) )
               {
                  // This is an ASCII or binary table extension. CFITSIO is
                  // simulating an image extension, so this is almost certainly
                  // a compressed image. We do not support these.
                  nonImageExtension = true;
                  break;
               }
            }

            // Add this keyword to the list
            keys.Add( k );
         }

         // Not an image; probably a compressed FITS
         if ( nonImageExtension )
            continue;

         // Increment the count of nonempty images
         if ( hdu.naxis > 0 )
            ++numberOfReadableImages;

         image.info.colorSpace = hdu.colorSpace;

         hdus.Add( hdu );
         keywords.Add( keys );
         fitsOptions.Add( fits );
         m_images.Add( image );
      }

      if ( hdus.IsEmpty() )
         throw FITS::NoImageHDU( m_path );

      if ( numberOfReadableImages == 0 )
         throw FITS::NoReadableImage( m_path );
   }
   catch ( ... )
   {
      Close();
      throw;
   }
}

// ----------------------------------------------------------------------------

const FITSImageOptions& FITSReader::FITSOptions() const
{
   if ( !IsOpen() )
      throw FITS::InvalidReadOperation( String() );

   if ( m_index >= int( fitsOptions.Length() ) )
      throw FITS::InvalidReadOperation( m_path );

   return fitsOptions[m_index];
}

void FITSReader::SetFITSOptions( const FITSImageOptions& newOptions )
{
   if ( !IsOpen() )
      throw FITS::InvalidReadOperation( String() );

   if ( m_index >= int( fitsOptions.Length() ) )
      throw FITS::InvalidReadOperation( m_path );

   FITSImageOptions& fits = fitsOptions[m_index];
   FITSHDUData& hdu = hdus[m_index];

   fits = newOptions;

   if ( hdu.signedIsPhysical != newOptions.signedIntegersArePhysical )
   {
      hdu.signedIsPhysical = newOptions.signedIntegersArePhysical;

      if ( hdu.signedIsPhysical )
      {
         hdu.zeroOffset = 0;
         switch ( hdu.equivBitpix )
         {
         case SBYTE_IMG:      // 8-bit signed integers
            hdu.scaleRange = uint8_max;
            break;
         case SHORT_IMG:      // 16-bit signed integers
            hdu.scaleRange = uint16_max;
            break;
         case LONG_IMG:       // 32-bit signed integers
            hdu.scaleRange = uint32_max;
            break;
         }
      }
      else
      {
         switch ( hdu.equivBitpix )
         {
         case SBYTE_IMG:      // 8-bit signed integers
            hdu.zeroOffset = int8_min;
            hdu.scaleRange = int8_max;
            break;
         case SHORT_IMG:      // 16-bit signed integers
            hdu.zeroOffset = int16_min;
            hdu.scaleRange = int16_max;
            break;
         case LONG_IMG:       // 32-bit signed integers
            hdu.zeroOffset = int32_min;
            hdu.scaleRange = int32_max;
            break;
         }
      }
   }
}

// ----------------------------------------------------------------------------

ICCProfile FITSReader::ReadICCProfile()
{
   if ( !IsOpen() )
      throw FITS::InvalidReadOperation( String() );
   if ( m_index >= int( hdus.Length() ) )
      throw FITS::InvalidReadOperation( m_path );

   ByteArray iccData;
   if ( !hdus[m_index].iccExtName.IsEmpty() )
      if ( ReadBLOB( iccData, hdus[m_index].iccExtName ) )
         return ICCProfile( iccData );

   if ( ReadBLOB( iccData, "ICCProfile" ) ||
        ReadBLOB( iccData, "ICC_PROFILE" ) ||
        ReadBLOB( iccData, "ICCPROFILE" ) )
      return ICCProfile( iccData );

   return ICCProfile();
}

// ----------------------------------------------------------------------------

UInt8Image FITSReader::ReadThumbnail()
{
   static const char* extNames[] =
   {
      "Thumbnail",
      "ThumbnailImage",
      "THUMBNAIL",
      "THUMBNAILIMAGE",
      nullptr
   };

   if ( !IsOpen() )
      throw FITS::InvalidReadOperation( String() );

   if ( m_index >= int( hdus.Length() ) )
      throw FITS::InvalidReadOperation( m_path );

   UInt8Image image;

   bool ok = false;

   if ( !hdus[m_index].thumbExtName.IsEmpty() )
   {
      {
         CFITSIO_LOCK
         ::fits_movnam_hdu( fits_handle, IMAGE_HDU, hdus[m_index].thumbExtName.Begin(), 0, &fitsStatus );
      }
      if ( fitsStatus == 0 ) // HDU found ?
         ok = true;
   }

   for ( const char** x = extNames; !ok; )
   {
      IsoString hduName( *x ); // make a temporary copy because fits_movnam_hdu takes a char* instead of const char* (?!)
      {
         CFITSIO_LOCK
         ::fits_movnam_hdu( fits_handle, IMAGE_HDU, hduName.Begin(), 0, &fitsStatus );
      }
      if ( fitsStatus == 0 ) // HDU found ?
         ok = true;
      else
      {
         fitsStatus = 0;  // this is not an actual error condition
         if ( *++x == nullptr ) // no more extension names to look for ?
            return UInt8Image();
      }
   }

   try
   {
      // Get number of thumbnail image dimensions.
      // Must be 2 (x,y = grayscale) or 3 (x,y,c = RGB)
      int naxis;
      {
         CFITSIO_LOCK
         ::fits_get_img_dim( fits_handle, &naxis, &fitsStatus );
      }
      if ( naxis != 2 && naxis != 3 )
         throw FITS::UnsupportedThumbnailImageDimensions( m_path );

      // Get the size of the image for each dimension.
      // Both dimensions must be within predefined limits.
      long naxes[ 3 ];
      {
         CFITSIO_LOCK
         ::fits_get_img_size( fits_handle, 3, naxes, &fitsStatus );
      }
      if ( naxes[0] < MinThumbnailSize || naxes[1] < MinThumbnailSize ||
           naxes[0] > MaxThumbnailSize || naxes[1] > MaxThumbnailSize ||
           naxis == 3 && naxes[2] != 3 )
      {
         throw FITS::InvalidThumbnailImage( m_path );
      }

      // Get thumbnail image sample type.
      // Thumbnails must be unsigned 8-bit images.
      int bitpix;
      {
         CFITSIO_LOCK
         ::fits_get_img_type( fits_handle, &bitpix, &fitsStatus );
      }
      if ( bitpix != BYTE_IMG )
         throw FITS::UnsupportedThumbnailSampleFormat( m_path );

      // Allocate thumbnail pixel data.
      image.AllocateData( naxes[0], naxes[1],
                          (naxis == 2) ? 1 : 3,
                          (naxis == 2) ? ColorSpace::Gray : ColorSpace::RGB );

      // Coordinate selectors. N.B.: CFITSIO expects one-based indexes.
      long fpixel[ 3 ];
      fpixel[0] = 1;
      fpixel[1] = 1;
      fpixel[2] = 0;

      // Read image channels
      for ( int c = 0; c < image.NumberOfChannels(); ++c )
      {
         ++fpixel[2]; // next channel
         {
            CFITSIO_LOCK
            ::fits_read_pix( fits_handle, TBYTE, fpixel,
                             long( image.NumberOfPixels() ), 0, image[c], 0, &fitsStatus );
         }
         if ( fitsStatus != 0 )
            throw FITS::FileReadError( m_path );
      }

      // Restore CFITSIO's current HDU to the current image HDU.
      // N.B.: CFITSIO expects HDU numbers relative to 1.
      {
         CFITSIO_LOCK
         ::fits_movabs_hdu( fits_handle, hdus[m_index].hduNumber+1, 0, &fitsStatus );
      }
      if ( fitsStatus != 0 )
         throw FITS::UnableToAccessCurrentHDU( m_path );

      return image;
   }
   catch ( UnableToAccessCurrentHDU& )
   {
      throw;
   }
   catch ( ... )
   {
      int dummy;
      {
         CFITSIO_LOCK
         ::fits_movabs_hdu( fits_handle, hdus[m_index].hduNumber+1, 0, &dummy );
      }
      throw;
   }
}

// ----------------------------------------------------------------------------

FITSKeywordArray FITSReader::ReadFITSKeywords()
{
   if ( !IsOpen() )
      throw FITS::InvalidReadOperation( String() );
   if ( m_index >= int( hdus.Length() ) )
      throw FITS::InvalidReadOperation( m_path );
   return keywords[m_index];
}

// ----------------------------------------------------------------------------

bool FITSReader::ReadBLOB( ByteArray& extData, const IsoString& extName )
{
   if ( !IsOpen() )
      throw FITS::InvalidReadOperation( String() );
   if ( m_index >= int( hdus.Length() ) )
      throw FITS::InvalidReadOperation( m_path );

   extData.Clear();

   // Select an existing extension HDU.
   {
      CFITSIO_LOCK
      ::fits_movnam_hdu( fits_handle, IMAGE_HDU, const_cast<char*>( extName.c_str() ), 0, &fitsStatus );
   }

   if ( fitsStatus != 0 ) // HDU not found ?
   {
      fitsStatus = 0; // this is not an error condition
      return false;
   }

   try
   {
      // Get HDU's defining parameters.
      int bitpix;
      int naxis;
      long naxes[ 3 ];
      {
         CFITSIO_LOCK
         ::fits_get_img_param( fits_handle, 1, &bitpix, &naxis, naxes, &fitsStatus );
      }

      // Supported extension HDUs are simple, nonempty image extensions.
      if ( bitpix != BYTE_IMG || naxis != 1 || *naxes == 0 )
         throw FITS::InvalidExtensionHDU( String( "Extension \'" + extName + "\' in " ) + m_path );

      // Make room for the data block.
      extData = ByteArray( *naxes );

      // Read data block.
      long fpixel = 1;
      {
         CFITSIO_LOCK
         ::fits_read_pix( fits_handle, TBYTE, &fpixel, *naxes, 0, extData.Begin(), 0, &fitsStatus );
      }
      if ( fitsStatus != 0 )
         throw FITS::FileReadError( m_path );

      // Restore CFITSIO's current HDU to the current image HDU.
      // N.B.: CFITSIO expects HDU numbers relative to 1.
      {
         CFITSIO_LOCK
         ::fits_movabs_hdu( fits_handle, hdus[m_index].hduNumber+1, 0, &fitsStatus );
      }
      if ( fitsStatus != 0 )
         throw FITS::UnableToAccessCurrentHDU( m_path );

      return true;
   }
   catch ( ... )
   {
      extData.Clear();

      try
      {
         throw;
      }
      catch ( UnableToAccessCurrentHDU& )
      {
         throw;
      }
      catch ( ... )
      {
         int dummy;
         {
            CFITSIO_LOCK
            ::fits_movabs_hdu( fits_handle, hdus[m_index].hduNumber+1, 0, &dummy );
         }
         throw;
      }
   }
}

// ----------------------------------------------------------------------------

static bool IsReservedExtensionName( const IsoString& extName )
{
   return !extName.Compare( "ICCProfile" ) || !extName.Compare( "Thumbnail" );
}

IsoStringList FITSReader::DataExtensionNames() const
{
   int numberOfHDUs;
   {
      CFITSIO_LOCK
      ::fits_get_num_hdus( fits_handle, &numberOfHDUs, &fitsStatus );
   }
   if ( fitsStatus != 0 )
      throw FITS::FileReadError( m_path );
   if ( numberOfHDUs == 0 )
      throw FITS::UnableToAccessPrimaryHDU( m_path );

   IsoStringList extNames;
   for ( int i = 0; i < numberOfHDUs; ++i )
   {
      int hduType;
      {
         CFITSIO_LOCK
         ::fits_movabs_hdu( fits_handle, i+1, &hduType, &fitsStatus );
      }

      // Skip non-image HDUs
      if ( hduType != IMAGE_HDU )
         continue;

      // Get the number of image dimensions. NAXIS=1 for data extensions.
      int naxis;
      {
         CFITSIO_LOCK
         ::fits_get_img_dim( fits_handle, &naxis, &fitsStatus );
      }
      if ( naxis == 1 )
      {
         char cvalue[ 81 ], ccomment[ 81 ];
         {
            CFITSIO_LOCK
            ::fits_read_keyword( fits_handle, "EXTNAME", cvalue, ccomment, &fitsStatus );
         }
         if ( fitsStatus == 0 ) // keyword found
         {
            IsoString extName = FITSHeaderKeyword( "EXTNAME", cvalue ).StripValueDelimiters();
            if ( !extName.IsEmpty() ) // anonymous extensions are ignored
               if ( !IsReservedExtensionName( extName ) ) // reserved properties are not listed
                  if ( !extNames.Contains( extName ) ) // duplicate extensions are not allowed (only the 1st one is listed)
                     extNames.Append( extName );
         }
         else
            fitsStatus = 0; // don't propagate a false error condition
      }
   }

   return extNames;
}

// ----------------------------------------------------------------------------

/*
 * Optional automatic normalization of floating point sample values.
 *
 * Normalization is a rescaling operation that takes place only if the existing
 * range of sample values exceeds the specified input range:
 *    [Options().lowerRange, Options().upperRange]
 */
#define NORMALIZE( I )                                                           \
   if ( Options().readNormalized )                                               \
   {                                                                             \
      StatusMonitor status = image.Status();                                     \
      image.SetStatusCallback( nullptr );                                        \
                                                                                 \
      /* Replace NaNs and infinities with zeros. */                              \
      image.ResetSelections();                                                   \
      for ( int c = 0; c < image.NumberOfChannels(); ++c )                       \
         for ( I::sample_iterator i( image, c ); i; ++i )                        \
            if ( !IsFinite( *i ) )                                               \
               *i = 0;                                                           \
                                                                                 \
      /* Get extreme pixel sample values. */                                     \
      I::sample mn, mx;                                                          \
      image.GetExtremePixelValues( mn, mx );                                     \
                                                                                 \
      /* If necessary, rescale to the user-specified range. */                   \
      double lowerRange = Options().lowerRange;                                  \
      double upperRange = Options().upperRange;                                  \
      if ( mn < lowerRange || mx > upperRange )                                  \
      {                                                                          \
         double iDelta = mx - mn;                                                \
         double oDelta = upperRange - lowerRange;                                \
         for ( int c = 0; c < image.NumberOfChannels(); ++c )                    \
            if ( oDelta != 1 || lowerRange != 0 )                                \
               for ( I::sample_iterator i( image, c ); i; ++i )                  \
                  *i = ((*i - mn)/iDelta)*oDelta + lowerRange;                   \
            else                                                                 \
               for ( I::sample_iterator i( image, c ); i; ++i )                  \
                  *i = (*i - mn)/iDelta;                                         \
      }                                                                          \
                                                                                 \
      image.Status() = status;                                                   \
   }

void FITSReader::ReadImage( FImage& image )
{
   FITSReaderPrivate::ReadImage( image, *this );
   NORMALIZE( FImage )
}

void FITSReader::ReadImage( DImage& image )
{
   FITSReaderPrivate::ReadImage( image, *this );
   NORMALIZE( DImage )
}

void FITSReader::ReadImage( UInt8Image& image )
{
   FITSReaderPrivate::ReadImage( image, *this );
}

void FITSReader::ReadImage( UInt16Image& image )
{
   FITSReaderPrivate::ReadImage( image, *this );
}

void FITSReader::ReadImage( UInt32Image& image )
{
   FITSReaderPrivate::ReadImage( image, *this );
}

#undef NORMALIZE

// ----------------------------------------------------------------------------

void FITSReader::ReadSamples( FImage::sample* f, int startRow, int rowCount, int c )
{
   FITSReaderPrivate::ReadSamples( f, (FImage::pixel_traits*)0, startRow, rowCount, c, *this );
}

void FITSReader::ReadSamples( DImage::sample* f, int startRow, int rowCount, int c )
{
   FITSReaderPrivate::ReadSamples( f, (DImage::pixel_traits*)0, startRow, rowCount, c, *this );
}

void FITSReader::ReadSamples( UInt8Image::sample* f, int startRow, int rowCount, int c )
{
   FITSReaderPrivate::ReadSamples( f, (UInt8Image::pixel_traits*)0, startRow, rowCount, c, *this );
}

void FITSReader::ReadSamples( UInt16Image::sample* f, int startRow, int rowCount, int c )
{
   FITSReaderPrivate::ReadSamples( f, (UInt16Image::pixel_traits*)0, startRow, rowCount, c, *this );
}

void FITSReader::ReadSamples( UInt32Image::sample* f, int startRow, int rowCount, int c )
{
   FITSReaderPrivate::ReadSamples( f, (UInt32Image::pixel_traits*)0, startRow, rowCount, c, *this );
}

// ----------------------------------------------------------------------------

#undef hdus
#undef keywords
#undef fitsOptions

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

struct FITSExtensionData
{
   ByteArray data;
   IsoString name;

   FITSExtensionData() = default;

   FITSExtensionData( const FITSExtensionData& x ) = default;

   FITSExtensionData( const ByteArray& d, const IsoString& n ) : data( d ), name( n )
   {
   }

   bool IsValid() const
   {
      return !data.IsEmpty() && !name.IsEmpty();
   }

   bool operator ==( const FITSExtensionData& x ) const
   {
      return name == x.name;
   }

   bool operator <( const FITSExtensionData& x ) const
   {
      return name < x.name;
   }
};

// ----------------------------------------------------------------------------

struct FITSWriterData
{
   AutoPointer<FITSHDUData> hdu; // current image HDU
   FITSImageOptions         fitsOptions;
   FITSKeywordArray         keywords;
   ICCProfile               iccProfile;
   UInt8Image               thumbnail;
   Array<FITSExtensionData> extensions;
};

// ----------------------------------------------------------------------------

#define hdu               m_data->hdu
#define fitsOptions       m_data->fitsOptions
#define keywords          m_data->keywords
#define iccProfile        m_data->iccProfile
#define iccProfileExtName m_data->iccProfileExtName
#define thumbnail         m_data->thumbnail
#define thumbnailExtName  m_data->thumbnailExtName
#define extensions        m_data->extensions

// ----------------------------------------------------------------------------

class FITSWriterPrivate
{
public:

   template <class P> inline
   static void WriteImage( const GenericImage<P>& image, FITSWriter& writer )
   {
      FITSFileData* m_fileData = writer.m_fileData;

      try
      {
         ImageInfo info( image );

         writer.CreateImage( info );

         StatusMonitor monitor;
         StandardStatus status;
         if ( writer.FITSOptions().verbosity > 0 )
         {
            monitor.SetCallback( &status );
            monitor.Initialize( String().Format(
                  "Writing FITS image: %d-bit %s, %d channel(s), %dx%d pixels",
                  writer.m_options.bitsPerSample,
                  writer.m_options.ieeefpSampleFormat ? "floating point" :
                        (writer.FITSOptions().unsignedIntegers ? "unsigned integers" : "signed integers"),
                  info.numberOfChannels,
                  info.width, info.height ),
                  info.NumberOfSamples() );
         }

         if ( fitsStatus != 0 )
            throw FITS::FileWriteError( writer.m_path );

         // Allocate a one-row buffer
         F64Vector buffer( info.width );

         // Get range of source pixel values.
         //
         // For integer samples, use the native range from PixelTraits.
         //
         // For floating-point source samples, the actual range of pixel values
         // has been declared in optional image parameters.

         double k0 = P::IsFloatSample() ? writer.m_options.lowerRange : double( P::MinSampleValue() );
         double k1 = P::IsFloatSample() ? writer.m_options.upperRange : double( P::MaxSampleValue() );

         // Rescaling is necessary to write integer samples if the source and
         // output sample types don't match.
         double k = 1; // calm compilers
         bool rescale = k0 != writer.hdu->zeroOffset || k1 != writer.hdu->scaleRange;
         if ( rescale )
            k = (writer.hdu->scaleRange - writer.hdu->zeroOffset)/(k1 - k0);

         // Rescaled integer samples must be rounded to the nearest integers.
         bool round = writer.hdu->bitpix != FLOAT_IMG && writer.hdu->bitpix != DOUBLE_IMG;

         Rect r = image.SelectedRectangle();

         long fpixel[ 3 ];
         fpixel[0] = 1;

         // Main loop over selected source image channels

         for ( int c = image.FirstSelectedChannel(); c <= image.LastSelectedChannel(); ++c )
         {
            fpixel[2] = c - image.FirstSelectedChannel() + 1;

            // Loop over selected rows

            for ( int i = 0; i < info.height; ++i )
            {
               fpixel[1] = const_cast<const FITSWriter&>( writer ).FITSOptions().bottomUp ? info.height - i : i + 1;

               // Address of the starting pixel in this row
               const typename P::sample* f = image.PixelAddress( r.x0, r.y0+i, c );

               // Copy a row of source pixels

               for ( int j = 0; j < info.width; ++j )
               {
                  double g = double( *f++ );

                  //if ( P::IsFloatSample() )
                  //   g = Range( g, k0, k1 );

                  // Rescale if necessary to adapt the range of input sample
                  // values to the output range.
                  if ( rescale )
                  {
                     g = k*(g - k0) + writer.hdu->zeroOffset;
                     if ( round )
                        g = Round( g );
                  }

                  buffer[j] = g;
               }

               // Do write them
               {
                  CFITSIO_LOCK
                  ::fits_write_pix( fits_handle, TDOUBLE, fpixel, info.width, buffer.Begin(), &fitsStatus );
               }
               if ( fitsStatus != 0 )
                  throw FITS::FileWriteError( writer.m_path );

               if ( writer.FITSOptions().verbosity > 0 )
                  monitor += info.width;
            }
         }

         writer.CloseImage();
      }
      catch ( ... )
      {
         fitsStatus = 1; // in case this is a non-CFITSIO error
         writer.Close();
         throw;
      }
   }

   template <class T, class P> inline
   static void WriteSamples( const T* f, P*, int startRow, int rowCount, int c, FITSWriter& writer )
   {
      if ( !writer.IsOpen() || !writer.hdu )
         throw FITS::InvalidWriteOperation( writer.m_path );

      // Ensure arguments in the valid range.
      if ( writer.hdu->naxis == 0 ||
                     startRow < 0 || startRow+rowCount > writer.hdu->naxes[1] ||
                            c < 0 || c >= writer.hdu->naxes[2] )
         throw FITS::WriteCoordinatesOutOfRange( writer.m_path );

      if ( writer.FITSOptions().bottomUp )
         startRow = writer.hdu->naxes[1] - startRow - rowCount;

      FITSFileData* m_fileData = writer.m_fileData;

      try
      {
         // Coordinate selectors.
         // N.B.: CFITSIO expects one-based indexes.
         long fpixel[ 3 ];
         fpixel[0] = 1;
         fpixel[1] = startRow + 1;
         fpixel[2] = c + 1;

         // This is the number of pixels to write.
         long N = rowCount * writer.hdu->naxes[0];

         // Allocate space for them.
         F64Vector buffer( N );

         // Get range of source pixel values.
         //
         // For integer samples, use the native range from PixelTraits.
         //
         // For floating-point source samples, the actual range of pixel values
         // has been declared in optional image parameters.

         double k0 = P::IsFloatSample() ? writer.m_options.lowerRange : double( P::MinSampleValue() );
         double k1 = P::IsFloatSample() ? writer.m_options.upperRange : double( P::MaxSampleValue() );

         // Rescaling is necessary to write integer samples if the source and
         // output sample types don't match.
         double k = 1; // calm compilers
         bool rescale = k0 != writer.hdu->zeroOffset || k1 != writer.hdu->scaleRange;
         if ( rescale )
            k = (writer.hdu->scaleRange - writer.hdu->zeroOffset)/(k1 - k0);

         // Rescaled integer samples must be rounded to the nearest integers.
         bool round = writer.hdu->bitpix != FLOAT_IMG && writer.hdu->bitpix != DOUBLE_IMG;

         // Copy a row of source pixels

         for ( int j = 0; j < N; ++j )
         {
            double g = double( *f++ );

            //if ( P::IsFloatSample() )
            //   g = Range( g, k0, k1 );

            // Rescale as necessary to adapt the range of input sample
            // values to the output range.
            if ( rescale )
            {
               g = k*(g - k0) + writer.hdu->zeroOffset;
               if ( round )
                  g = Round( g );
            }

            buffer[j] = g;
         }

         // Write strip of FITS rows
         {
            CFITSIO_LOCK
            ::fits_write_pix( fits_handle, TDOUBLE, fpixel, N, buffer.Begin(), &fitsStatus );
         }
         if ( fitsStatus != 0 )
            throw FITS::FileWriteError( writer.m_path );
      }
      catch ( ... )
      {
         fitsStatus = 1; // in case this is a non-CFITSIO error
         writer.Close();
         throw;
      }
   }
};

// ----------------------------------------------------------------------------

FITSWriter::FITSWriter()
{
}

FITSWriter::~FITSWriter() noexcept( false )
{
   Close();
}

bool FITSWriter::IsOpen() const
{
   return FITS::IsOpenStream();
}

void FITSWriter::Create( const String& filePath, int count )
{
   if ( IsOpen() )
      throw FITS::InvalidWriteOperation( m_path );

   if ( filePath.IsEmpty() )
      throw FITS::InvalidFilePath( filePath );

   if ( count <= 0 )
      throw pcl::NotImplemented( *this, "Write empty FITS files" );

   Reset();

   String backupPath;

   try
   {
      if ( !m_fileData )
         m_fileData = new FITSFileData;
      if ( !m_data )
         m_data = new FITSWriterData;

#ifdef __PCL_WINDOWS
      m_path = File::WindowsPathToUnix( filePath );
#else
      m_path = filePath;
#endif

      /*
       * NB: fits_create_diskfile() will fail if the specified file already
       * exists. Instead of deleting an existing file before attempting to
       * create a new one (which is irresponsible to say the least), we'll
       * backup it until a new file has been successfully created.
       */
      if ( File::Exists( m_path ) )
      {
         backupPath = m_path + '~';
         if ( File::Exists( backupPath ) )
            backupPath = File::UniqueFileName( File::ExtractDrive( m_path ) + File::ExtractDirectory( m_path ) );
         File::Move( m_path, backupPath );
      }

      /*
       * Create the FITS image file stream for writing.
       */
      IsoString path8 =
#ifdef __PCL_WINDOWS
         File::UnixPathToWindows( m_path ).ToMBS();
#else
         m_path.ToUTF8();
#endif
      fitsStatus = 0;
      {
         CFITSIO_LOCK
         ::fits_create_diskfile( (::fitsfile**)&m_fileData->fits, path8.c_str(), &fitsStatus );
      }
      if ( fitsStatus != 0 )
         throw FITS::UnableToCreateFile( m_path );

      if ( count > 1 )
      {
         /*
          * If we are writing more than one image in this FITS file, create an
          * empty primary HDU (naxis = 0). In such cases, all actual images
          * will be written as image extensions.
          */
         long naxes[ 3 ] = { 0, 0, 0 };
         {
            CFITSIO_LOCK
            ::fits_create_img( fits_handle, 16, 0, naxes, &fitsStatus );
         }
         if ( fitsStatus != 0 )
            throw FITS::UnableToCreatePrimaryHDU( m_path );

         /*
          * Software information keywords.
          */
         {
            CFITSIO_LOCK

            IsoString version( PixInsightVersion::AsString() );
            ::fits_write_key_str( fits_handle, "PROGRAM",
                                  const_cast<char*>( version.c_str() ),
                                  "Software that created this file",
                                  &fitsStatus );
            version = "PixInsight Class Library: " + Version::AsString();
            ::fits_write_comment( fits_handle,
                                  const_cast<char*>( version.c_str() ),
                                  &fitsStatus );
            version = Module->ReadableVersion();
            ::fits_write_comment( fits_handle,
                                  const_cast<char*>( version.c_str() ),
                                  &fitsStatus );
         }
         if ( fitsStatus != 0 )
            throw FITS::KeywordWriteError( m_path );
      }

      if ( !backupPath.IsEmpty() && File::Exists( backupPath ) )
      {
         try
         {
            File::Remove( backupPath );
         }
         catch ( ... )
         {
            // Don't propagate an exception if we can't delete a backup file.
         }
      }
   }
   catch ( ... )
   {
      if ( !backupPath.IsEmpty() && File::Exists( backupPath ) )
      {
         if ( File::Exists( m_path ) )
            File::Remove( m_path );
         File::Move( backupPath, m_path );
      }

      fitsStatus = 1; // in case this is a non-CFITSIO error
      Close();
      throw;
   }
}

// ----------------------------------------------------------------------------

const FITSImageOptions& FITSWriter::FITSOptions() const
{
   return fitsOptions;
}

void FITSWriter::SetFITSOptions( const FITSImageOptions& newOptions )
{
   if ( hdu )
      throw FITS::InvalidWriteOperation( m_path );
   fitsOptions = newOptions;
}

// ----------------------------------------------------------------------------

void FITSWriter::WriteICCProfile( const ICCProfile& profile )
{
   if ( hdu )
      throw FITS::InvalidWriteOperation( m_path );
   iccProfile = profile;
}

void FITSWriter::WriteThumbnail( const UInt8Image& image )
{
   if ( hdu )
      throw FITS::InvalidWriteOperation( m_path );
   thumbnail = image;
}

void FITSWriter::WriteFITSKeywords( const FITSKeywordArray& kwds )
{
   if ( hdu )
      throw FITS::InvalidWriteOperation( m_path );
   keywords = kwds;
}

const FITSKeywordArray& FITSWriter::FITSKeywords() const
{
   return keywords;
}

void FITSWriter::WriteBLOB( const ByteArray& extData, const IsoString& extName )
{
   if ( hdu )
      throw FITS::InvalidWriteOperation( m_path );
   extensions.Add( FITSExtensionData( extData, extName ) );
}

// ----------------------------------------------------------------------------

void FITSWriter::WriteImage( const Image& image )
{
   FITSWriterPrivate::WriteImage( image, *this );
}

void FITSWriter::WriteImage( const DImage& image )
{
   FITSWriterPrivate::WriteImage( image, *this );
}

void FITSWriter::WriteImage( const UInt8Image& image )
{
   FITSWriterPrivate::WriteImage( image, *this );
}

void FITSWriter::WriteImage( const UInt16Image& image )
{
   FITSWriterPrivate::WriteImage( image, *this );
}

void FITSWriter::WriteImage( const UInt32Image& image )
{
   FITSWriterPrivate::WriteImage( image, *this );
}

// ----------------------------------------------------------------------------

void FITSWriter::WriteSamples( const FImage::sample* f, int startRow, int rowCount, int c )
{
   FITSWriterPrivate::WriteSamples( f, (FImage::pixel_traits*)0, startRow, rowCount, c, *this );
}

void FITSWriter::WriteSamples( const DImage::sample* f, int startRow, int rowCount, int c )
{
   FITSWriterPrivate::WriteSamples( f, (DImage::pixel_traits*)0, startRow, rowCount, c, *this );
}

void FITSWriter::WriteSamples( const UInt8Image::sample* f, int startRow, int rowCount, int c )
{
   FITSWriterPrivate::WriteSamples( f, (UInt8Image::pixel_traits*)0, startRow, rowCount, c, *this );
}

void FITSWriter::WriteSamples( const UInt16Image::sample* f, int startRow, int rowCount, int c )
{
   FITSWriterPrivate::WriteSamples( f, (UInt16Image::pixel_traits*)0, startRow, rowCount, c, *this );
}

void FITSWriter::WriteSamples( const UInt32Image::sample* f, int startRow, int rowCount, int c )
{
   FITSWriterPrivate::WriteSamples( f, (UInt32Image::pixel_traits*)0, startRow, rowCount, c, *this );
}

// ----------------------------------------------------------------------------

static void CleanupKeywordField( IsoString& k )
{
   if ( k.IsEmpty() )
      return;

   for ( size_type i = 0; ; ++i )
   {
      int c = (int)(unsigned char)k[i];

      if ( c == 0 )
         break;

      if ( c <= 0x7E )
      {
         if ( c < 0x20 )
            k[i] = '_';
         continue;
      }

      switch ( c )
      {
      case 0xA0: k[i] = ' '; break;
      case 0xA1: k[i] = '!'; break;
      case 0xA2: k[i] = 'c'; break;
      case 0xA3: k[i] = 'L'; break;
      case 0xA4: k[i] = 'o'; break;
      case 0xA5: k[i] = 'Y'; break;
      case 0xA6: k[i] = '|'; break;
      case 0xA7: k[i] = 'S'; break;
      case 0xA8: k[i] = ':'; break;

      case 0xA9: // copyright sign
         k[i] = '(';
         k.Insert( ++i, 'C' );
         k.Insert( ++i, ')' );
         break;

      case 0xAA: k[i] = 'a'; break;
      case 0xAB: k[i] = '<'; break;
      case 0xAC: k[i] = '-'; break;
      case 0xAD: k[i] = '-'; break;

      case 0xAE: // registered sign
         k[i] = '(';
         k.Insert( ++i, 'R' );
         k.Insert( ++i, ')' );
         break;

      case 0xAF: k[i] = '-'; break;

      case 0xB0: k[i] = 'd'; break;

      case 0xB1: // +-
         k[i] = '+';
         k.Insert( ++i, '-' );
         break;

      case 0xB2: k[i] = '2'; break;
      case 0xB3: k[i] = '3'; break;
      case 0xB4: k[i] = '\''; break;
      case 0xB5: k[i] = 'u'; break;
      case 0xB6: k[i] = 'P'; break;
      case 0xB7: k[i] = '.'; break;
      case 0xB8: k[i] = '.'; break;
      case 0xB9: k[i] = '1'; break;
      case 0xBA: k[i] = '0'; break;
      case 0xBB: k[i] = '>'; break;

      case 0xBC: // 1/4
         k[i] = '1';
         k.Insert( ++i, '/' );
         k.Insert( ++i, '4' );
         break;

      case 0xBD: // 1/2
         k[i] = '1';
         k.Insert( ++i, '/' );
         k.Insert( ++i, '2' );
         break;

      case 0xBE: // 3/4
         k[i] = '3';
         k.Insert( ++i, '/' );
         k.Insert( ++i, '4' );
         break;

      case 0xBF: k[i] = '?'; break;

      case 0xC0:
      case 0xC1:
      case 0xC2:
      case 0xC3:
      case 0xC4:
      case 0xC5: k[i] = 'A'; break;

      case 0xC6: // capital AE
         k[i] = 'A';
         k.Insert( ++i, 'E' );
         break;

      case 0xC7: k[i] = 'C'; break;

      case 0xC8:
      case 0xC9:
      case 0xCA:
      case 0xCB: k[i] = 'E'; break;

      case 0xCC:
      case 0xCD:
      case 0xCE:
      case 0xCF: k[i] = 'I'; break;

      case 0xD0: k[i] = 'D'; break;

      case 0xD1: k[i] = 'N'; break;

      case 0xD2:
      case 0xD3:
      case 0xD4:
      case 0xD5:
      case 0xD6:
      case 0xD8: k[i] = 'O'; break;

      case 0xD7: k[i] = 'x'; break;

      case 0xD9:
      case 0xDA:
      case 0xDB:
      case 0xDC: k[i] = 'U'; break;

      case 0xDD: k[i] = 'Y'; break;

      case 0xDE: // capital Thorn
         k[i] = 'T';
         k.Insert( ++i, 'H' );
         break;

      case 0xDF: k[i] = 's'; break;

      case 0xE0:
      case 0xE1:
      case 0xE2:
      case 0xE3:
      case 0xE4:
      case 0xE5: k[i] = 'a'; break;

      case 0xE6: // small ae
         k[i] = 'a';
         k.Insert( ++i, 'e' );
         break;

      case 0xE7: k[i] = 'c'; break;

      case 0xE8:
      case 0xE9:
      case 0xEA:
      case 0xEB: k[i] = 'e'; break;

      case 0xEC:
      case 0xED:
      case 0xEE:
      case 0xEF: k[i] = 'i'; break;

      case 0xF0: k[i] = 'd'; break;

      case 0xF1: k[i] = 'n'; break;

      case 0xF2:
      case 0xF3:
      case 0xF4:
      case 0xF5:
      case 0xF6:
      case 0xF8: k[i] = 'o'; break;

      case 0xF7: k[i] = '/'; break;

      case 0xF9:
      case 0xFA:
      case 0xFB:
      case 0xFC: k[i] = 'u'; break;

      case 0xFD: k[i] = 'y'; break;

      case 0xFE: // small Thorn
         k[i] = 't';
         k.Insert( ++i, 'h' );
         break;

      case 0xFF: k[i] = 'y'; break;

      default:   k[i] = '_'; break; // *i > 0x7E && *i < 0xA0
      }
   }
}

void FITSWriter::CreateImage( const ImageInfo& info )
{
   if ( !IsOpen() )
      throw FITS::InvalidWriteOperation( String() );

   CloseImage();

   if ( m_options.complexSample )
      throw FITS::UnsupportedComplexFITS( m_path );

   if ( m_options.bitsPerSample == 64 && !m_options.ieeefpSampleFormat )
      throw FITS::Unsupported64BitFITS( m_path );

   if ( m_options.lowerRange >= m_options.upperRange )
      throw FITS::InvalidNormalizationRange( m_path );

   try
   {
      /*
       * Create the image FITS HDU
       */

      hdu = new FITSHDUData;

      // Zero-based position of this image HDU in this FITS file
      {
         CFITSIO_LOCK
         ::fits_get_num_hdus( fits_handle, &hdu->hduNumber, &fitsStatus );
      }
      if ( fitsStatus != 0 )
         throw FITS::FileReadError( m_path );

      switch ( m_options.bitsPerSample )
      {
      case 8:
         /*
          * Signed 8-bit images are not supported for writing
          * Note that CFITSIO rejects BITPIX=SBYTE_IMG
          */
         /*
         if ( fitsOptions.unsignedIntegers )
         */
         {
            hdu->bitpix = BYTE_IMG;
            hdu->zeroOffset = 0;
            hdu->scaleRange = uint8_max;
         }
         /*
         else
         {
            hdu->bitpix = SBYTE_IMG;
            hdu->zeroOffset = int8_min;
            hdu->scaleRange = int8_max;
         }
         */
         break;

      default:
      case 16:
         if ( fitsOptions.unsignedIntegers )
         {
            hdu->bitpix = USHORT_IMG;
            hdu->zeroOffset = 0;
            hdu->scaleRange = uint16_max;
         }
         else
         {
            hdu->bitpix = SHORT_IMG;
            hdu->zeroOffset = int16_min;
            hdu->scaleRange = int16_max;
         }
         break;

      case 32:
         if ( m_options.ieeefpSampleFormat )
         {
            hdu->bitpix = FLOAT_IMG;
            hdu->zeroOffset = m_options.lowerRange;
            hdu->scaleRange = m_options.upperRange;
         }
         else
         {
            if ( fitsOptions.unsignedIntegers )
            {
               hdu->bitpix = ULONG_IMG;
               hdu->zeroOffset = 0;
               hdu->scaleRange = uint32_max;
            }
            else
            {
               hdu->bitpix = LONG_IMG;
               hdu->zeroOffset = int32_min;
               hdu->scaleRange = int32_max;
            }
         }
         break;

      case 64:
         /*
          * Integer 64-bit samples are not supported.
          * Conditions already enforced by initial checks.
          */
         /*
         if ( m_options.ieeefpSampleFormat )
         {
         */
            hdu->bitpix = DOUBLE_IMG;
            hdu->zeroOffset = m_options.lowerRange;
            hdu->scaleRange = m_options.upperRange;
         /*
         }
         else
         {

            // Unsigned 64-bit integers not supported.
            hdu->bitpix = LONGLONG_IMG;
            hdu->zeroOffset = int64_min;
            hdu->scaleRange = int64_max;
         }
         */
         break;
      }

      if ( info.IsValid() )
      {
         hdu->naxis = (info.numberOfChannels == 1) ? 2 : 3;
         hdu->naxes[0] = info.width;
         hdu->naxes[1] = info.height;
         hdu->naxes[2] = info.numberOfChannels;
         hdu->colorSpace = info.colorSpace;
      }
      else
      {
         hdu->naxis = 0;
         hdu->naxes[0] = hdu->naxes[1] = hdu->naxes[2] = 0;
      }

      {
         CFITSIO_LOCK
         ::fits_create_img( fits_handle, hdu->bitpix, hdu->naxis, hdu->naxes, &fitsStatus );
      }
      if ( fitsStatus != 0 )
         throw FITS::UnableToCreateImageHDU( m_path );

      /*
       * Software information keywords.
       */
      {
         CFITSIO_LOCK

         IsoString version( PixInsightVersion::AsString() );
         ::fits_write_key_str( fits_handle, "PROGRAM",
                               const_cast<char*>( version.c_str() ),
                               "Software that created this HDU",
                               &fitsStatus );
         version = "PixInsight Class Library: " + Version::AsString();
         ::fits_write_comment( fits_handle,
                               const_cast<char*>( version.c_str() ),
                               &fitsStatus );
         version = Module->ReadableVersion();
         ::fits_write_comment( fits_handle,
                               const_cast<char*>( version.c_str() ),
                               &fitsStatus );
      }

      /*
       * Name of this image, if this is the primary HDU.
       * Name of this image extension, if this is not the primary HDU.
       */
      bool hduNameWritten = false;
      bool extNameWritten = false;
      if ( !m_id.IsEmpty() )
         if ( hdu->hduNumber == 0 )
         {
            CFITSIO_LOCK

            ::fits_write_key_str( fits_handle, "HDUNAME",
                                  const_cast<char*>( m_id.c_str() ),
                                  "Identifier of primary image HDU",
                                  &fitsStatus );
            hduNameWritten = true;
         }
         else
         {
            CFITSIO_LOCK

            ::fits_write_key_str( fits_handle, "EXTNAME",
                                  const_cast<char*>( m_id.c_str() ),
                                  "Identifier of image extension HDU",
                                  &fitsStatus );
            extNameWritten = true;
         }

      /*
       * Optional BZERO and BSCALE keywords for signed data (added 2009 Feb 28)
       * Some FITS readers out there need this or they get fool...
       */
      if ( !m_options.ieeefpSampleFormat )
         if ( !fitsOptions.unsignedIntegers )
            if ( fitsOptions.writeScalingKeywordsForSignedData )
            {
               CFITSIO_LOCK

               ::fits_write_key_flt( fits_handle, "BZERO",
                                     0.0, -6,
                                     "PCL: Default signed data offset",
                                     &fitsStatus );
               ::fits_write_key_flt( fits_handle, "BSCALE",
                                     1.0, -6,
                                     "PCL: Default data scaling ratio",
                                     &fitsStatus );
            }

      if ( info.IsValid() )
      {
         CFITSIO_LOCK

         /*
          * The COLORSPC keyword identifies the color space for this image
          */
         ::fits_write_key_str( fits_handle, "COLORSPC",
                               const_cast<char*>( (info.colorSpace == ColorSpace::RGB) ? "RGB" : "Grayscale" ),
                               "PCL: Color space",
                               &fitsStatus );

         /*
          * Signal presence of alpha channels
          */
         if ( info.numberOfChannels > ((info.colorSpace == ColorSpace::RGB) ? 3 : 1) )
            ::fits_write_key_flt( fits_handle, "ALPHACHN",
                                  info.numberOfChannels - ((info.colorSpace == ColorSpace::RGB) ? 3 : 1), -6,
                                  "PCL: Image includes alpha channels",
                                  &fitsStatus );

         /*
          * Image resolution keywords
          */
         if ( m_options.xResolution == m_options.yResolution )
            ::fits_write_key_flt( fits_handle, "RESOLUTN",
                                  m_options.xResolution, -6,
                                  "PCL: Resolution in pixels per resolution unit",
                                  &fitsStatus );
         else
         {
            ::fits_write_key_flt( fits_handle, "XRESOLTN",
                                  m_options.xResolution, -6,
                                  "PCL: Resolution in pixels: X-axis",
                                  &fitsStatus );
            ::fits_write_key_flt( fits_handle, "YRESOLTN",
                                  m_options.yResolution, -6,
                                  "PCL: Resolution in pixels: Y-axis",
                                  &fitsStatus );
         }
         // Resolution unit
         ::fits_write_key_str( fits_handle, "RESOUNIT",
                               const_cast<char*>( m_options.metricResolution ? "cm" : "inch" ),
                               "PCL: Resolution unit",
                               &fitsStatus );

         /*
          * Signal presence of an ICC Profile extension, if ICC profile
          * embedding has been requested, the profile has been specified, and
          * it is not empty.
          */
         if ( m_options.embedICCProfile )
            if ( iccProfile.IsProfile() )
            {
               if ( hdu->hduNumber > 0 )
                  hdu->iccExtName.Format( "ICCProfile%02d", hdu->hduNumber );
               else
                  hdu->iccExtName = "ICCProfile";

               ::fits_write_key_str( fits_handle, "ICCPROFL",
                                     const_cast<char*>( hdu->iccExtName.c_str() ),
                                     "PCL: File includes ICC color profile extension",
                                     &fitsStatus );
            }

         /*
          * Signal presence of a thumbnail image extension, if thumbnail
          * embedding has been requested and a valid thumbnail image has been
          * specified.
          */
         if ( m_options.embedThumbnail )
            if ( !thumbnail.IsEmpty() )
            {
               if ( hdu->hduNumber > 0 )
                  hdu->thumbExtName.Format( "Thumbnail%02d", hdu->hduNumber );
               else
                  hdu->thumbExtName = "Thumbnail";

               ::fits_write_key_str( fits_handle, "THUMBIMG",
                                     const_cast<char*>( hdu->thumbExtName.c_str() ),
                                     "PCL: File includes thumbnail image extension",
                                     &fitsStatus );
            }
      }

      if ( fitsStatus != 0 )
         throw FITS::KeywordWriteError( m_path );

      /*
       * Write the list of user keywords, if it has been specified.
       */
      for ( FITSKeywordArray::const_iterator i = keywords.Begin(); i != keywords.End(); ++i )
      {
         FITSHeaderKeyword k = *i;

         // Take it as uppercase to simplify subsequent comparisons
         IsoString kname = k.name.Trimmed().Uppercase();

         /*
          * Do not duplicate mandatory and PCL FITS keywords, and do not
          * propagate some keywords that don't make sense.
          */
         if ( !kname.IsEmpty() &&
               kname          != "SIMPLE"    &&
               kname          != "BITPIX"    &&
              !kname.StartsWith( "NAXIS" )   && // All NAXISxxx keywords must be caught here
               kname          != "EXTEND"    &&
               kname          != "NEXTEND"   &&
               kname          != "BSCALE"    &&
               kname          != "BZERO"     &&
               kname          != "PROGRAM"   &&
               kname          != "CREATOR"   &&
               kname          != "CONFIGUR"  &&
               kname          != "XTENSION"  &&
               kname          != "PCOUNT"    &&
               kname          != "GCOUNT"    &&
               kname          != "FILENAME"  &&
               kname          != "FILETYPE"  &&
               kname          != "ROOTNAME"  &&
    (!hduNameWritten || kname != "HDUNAME")  && // image ids have precedence over explicit HDUNAME keywords
    (!extNameWritten || kname != "EXTNAME")  && // image ids have precedence over explicit EXTNAME keywords
              !kname.StartsWith( "TLMIN" )   &&
              !kname.StartsWith( "TLMAX" )   &&
              !kname.StartsWith( "TDMIN" )   &&
              !kname.StartsWith( "TDMAX" )   &&
              !kname.StartsWith( "TDBIN" )   &&
               kname          != "PINSIGHT"  && // obsolete
               kname          != "COLORSPC"  &&
               kname          != "ALPHACHN"  &&
               kname          != "RESOLUTN"  &&
               kname          != "XRESOLTN"  &&
               kname          != "YRESOLTN"  &&
               kname          != "RESOUNIT"  &&
               kname          != "ICCPROFL"  &&
               kname          != "THUMBIMG"  &&
               kname          != "XMPDATA" )
         {
            /*
             * Avoid exact duplicates of already existing keywords.
             * This is also useful to avoid duplicating COMMENT keywords
             * generated by CFITSIO.
             */

            // Try to find a keyword with the same name as this one.
            char cvalue[ 81 ], ccomment[ 81 ];
            {
               CFITSIO_LOCK
               ::fits_read_keyword( fits_handle, kname.c_str(), cvalue, ccomment, &fitsStatus );
            }
            if ( fitsStatus == 0 ) // keyword found, don't include if identical
               if ( k == FITSHeaderKeyword( k.name.c_str(), cvalue, ccomment ) )
                  continue;

            fitsStatus = 0; // don't propagate a false error condition

            /*
             * Cleanup value and comment keyword fields, if requested.
             */
            if ( fitsOptions.cleanupHeaders )
            {
               CleanupKeywordField( k.value );
               CleanupKeywordField( k.comment );
            }

            /*
             * Write a keyword.
             */
            {
               CFITSIO_LOCK

               if ( kname == "COMMENT" || kname == "HISTORY" )
               {
                  // COMMENT and HISTORY keywords are special cases.

                  IsoString comment = k.value;

                  // COMMENT and HISTORY keywords cannot have a value string.
                  // If present, move the value to the comment field.

                  if ( !k.value.IsEmpty() )
                     comment += "// "; // use a C++ comment to indicate this

                  comment += k.comment;

                  if ( kname == "COMMENT" )
                     ::fits_write_comment( fits_handle, comment.c_str(), &fitsStatus );
                  else
                     ::fits_write_history( fits_handle, comment.c_str(), &fitsStatus );
               }
               else if ( k.IsNull() )
               {
                  // Write a "null" keyword without a value field.
                  // This case has to be handled in a special way with CFITSIO.
                  ::fits_write_key_null( fits_handle, k.name.c_str(), k.comment.c_str(), &fitsStatus );
               }
               else if ( k.IsString() )
               {
                  // Write a string keyword.
                  // We identify this by a prepending delimiter.
                  IsoString kvalue( k.StripValueDelimiters() );
                  ::fits_write_key_str( fits_handle, k.name.c_str(), kvalue.c_str(), k.comment.c_str(), &fitsStatus );
               }
               else if ( k.IsBoolean() )
               {
                  // Write a logical keyword.
                  // Logical keyword values are T and F, no delimiters.
                  ::fits_write_key_log( fits_handle, k.name.c_str(), k.value[0] == 'T', k.comment.c_str(), &fitsStatus );
               }
               else
               {
                  // Should be a numeric keyword.
                  double kvalue;
                  if ( k.value.TryToDouble( kvalue ) )  // valid numeral?
                  {
                     // Write a valid floating-point numeric keyword
                     ::fits_write_key_dbl( fits_handle, k.name.c_str(), kvalue,
                                           fitsOptions.writeFixedFloatKeywords ? +15 : -15,
                                           k.comment.c_str(),
                                           &fitsStatus );
                  }
                  else
                  {
                     // Assume this keyword is a string.
                     ::fits_write_key_str( fits_handle, k.name.c_str(), k.value.c_str(), k.comment.c_str(), &fitsStatus );
                  }
               }
            }

            if ( fitsStatus != 0 )
               throw FITS::KeywordWriteError( m_path );
         }
      }

      // Turn off CFITSIO's automatic data scaling for floating-point samples.
      if ( m_options.ieeefpSampleFormat )
      {
         CFITSIO_LOCK
         ::fits_set_bscale( fits_handle, 1, 0, &fitsStatus );
      }
   }
   catch ( ... )
   {
      fitsStatus = 1; // in case this is a non-CFITSIO error
      Close();
      throw;
   }
}

// ----------------------------------------------------------------------------

void FITSWriter::CloseImage()
{
   if ( !IsOpen() || !m_data )
      throw FITS::InvalidCloseImageOperation( m_path );

   // hdu != nullptr if we have an image currently open
   if ( !hdu )
      return;

   try
   {
      /*
       * Reload the keywords list with actual file keywords, if requested.
       */
      if ( fitsOptions.reloadKeywords )
      {
         // Learn the actual number of existing FITS keywords
         int count;
         {
            CFITSIO_LOCK
            ::fits_get_hdrspace( fits_handle, &count, 0, &fitsStatus );
         }
         if ( fitsStatus != 0 )
            throw FITS::KeywordReadError( m_path );

         // Reload keywords
         keywords.Clear();
         for ( int i = 1; i <= count; ++i )
         {
            char cname[ 81 ], cvalue[ 81 ], ccomment[ 81 ];
            {
               CFITSIO_LOCK
               ::fits_read_keyn( fits_handle, i, cname, cvalue, ccomment, &fitsStatus );
            }
            if ( fitsStatus != 0 )
               throw FITS::KeywordReadError( m_path );

            keywords << FITSHeaderKeyword( cname, cvalue, ccomment );
         }
      }

      /*
       * Embed thumbnail image if requested and specified.
       */
      if ( m_options.embedThumbnail && !thumbnail.IsEmpty() )
      {
         long naxes[ 3 ];
         naxes[0] = thumbnail.Width();
         naxes[1] = thumbnail.Height();
         naxes[2] = thumbnail.NumberOfChannels();

         // Verify validity of thumbnail image
         if ( naxes[0] < MinThumbnailSize || naxes[1] < MinThumbnailSize ||
              naxes[0] > MaxThumbnailSize || naxes[1] > MaxThumbnailSize ||
              thumbnail.ColorSpace() != ColorSpace::Gray && thumbnail.ColorSpace() != ColorSpace::RGB ||
              naxes[2] != (thumbnail.IsColor() ? 3 : 1) )
         {
            throw FITS::InvalidThumbnailImage( m_path );
         }

         // Create the thumbnail image extension HDU
         {
            CFITSIO_LOCK
            ::fits_create_img( fits_handle, BYTE_IMG, (naxes[2] == 1) ? 2 : 3, naxes, &fitsStatus );
         }
         if ( fitsStatus != 0 )
            throw FITS::UnableToCreateThumbnailHDU( m_path );

         // Emit the extension name keyword.

         IsoString ivers( Version::AsString() );
         {
            CFITSIO_LOCK
            ::fits_write_key_str( fits_handle, "EXTNAME",
                                  const_cast<char*>( hdu->thumbExtName.c_str() ),
                                  const_cast<char*>( ivers.c_str() ),
                                  &fitsStatus );
         }
         if ( fitsStatus != 0 )
            throw FITS::FileWriteError( m_path );

         // Coordinate selectors. N.B.: CFITSIO expects one-based indexes.
         long fpixel[ 3 ];
         fpixel[0] = 1;
         fpixel[1] = 1;
         fpixel[2] = 0;

         // Write thumbnail image channels.
         for ( int c = 0; c < thumbnail.NumberOfChannels(); ++c )
         {
            ++fpixel[2]; // next channel
            {
               CFITSIO_LOCK
               ::fits_write_pix( fits_handle, TBYTE, fpixel,
                                 long( thumbnail.NumberOfPixels() ),
                                 const_cast<uint8*>( thumbnail[c] ), &fitsStatus );
            }
            if ( fitsStatus != 0 )
               throw FITS::FileWriteError( m_path );
         }
      }

      /*
       * Embed an ICC color profile if requested and specified.
       */
      if ( m_options.embedICCProfile && iccProfile.IsProfile() )
         WriteExtensionHDU( FITSExtensionData( iccProfile.ProfileData(), hdu->iccExtName ) );

      hdu.Destroy();
   }
   catch ( ... )
   {
      fitsStatus = 1; // in case this is a non-CFITSIO error
      Close();
      throw;
   }
}

// ----------------------------------------------------------------------------

void FITSWriter::Close()
{
   if ( m_data )
   {
      if ( fitsStatus == 0 )
      {
         try
         {
            CloseImage();

            // Embed all user-defined arbitrary extensions
            for ( Array<FITSExtensionData>::const_iterator i = extensions.Begin(); i != extensions.End(); ++i )
               WriteExtensionHDU( *i );
         }
         catch ( ... )
         {
            m_data.Destroy();
            FITS::CloseStream();
            throw;
         }
      }

      m_data.Destroy();
   }

   FITS::CloseStream();
}

// ----------------------------------------------------------------------------

void FITSWriter::WriteExtensionHDU( const FITSExtensionData& ext )
{
   // Ignore invalid extensions
   if ( !ext.IsValid() )
      return;

   {
      CFITSIO_LOCK

      // Create extension HDU.
      long size = long( ext.data.Length() );
      ::fits_create_img( fits_handle, BYTE_IMG, 1, &size, &fitsStatus );

      // Emit the extension name keyword.
      IsoString ivers( Version::AsString() );
      ::fits_write_key_str( fits_handle, "EXTNAME",
                            const_cast<char*>( ext.name.c_str() ),
                            const_cast<char*>( ivers.c_str() ),
                            &fitsStatus );

      // Write extension data block.
      long fpixel = 1;
      ::fits_write_pix( fits_handle, TBYTE, &fpixel, size, const_cast<uint8*>( ext.data.Begin() ), &fitsStatus );
   }
   if ( fitsStatus != 0 )
      throw FITS::FileWriteError( m_path );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FITS.cpp - Released 2017-07-09T18:07:25Z
