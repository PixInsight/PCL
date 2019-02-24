//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard JPEG File Format Module Version 01.00.04.0368
// ----------------------------------------------------------------------------
// JPEG.cpp - Released 2019-01-21T12:06:31Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "JPEG.h"

#include <pcl/EndianConversions.h>
#include <pcl/ErrorHandler.h>
#include <pcl/StdStatus.h>

// N.B.: Using jpeglib.h on Windows requires some extra hacking.

#ifdef FAR
#  undef FAR
#endif

extern "C" {

// Ensure that boolean is properly defined on Windows
#if defined( __PCL_WINDOWS ) && defined( __RPCNDR_H__ ) && !defined( boolean )
typedef unsigned char boolean;
#define HAVE_BOOLEAN 1
#endif

// Pretend that we are on X11 to leave INT16 and INT32 as they are defined by
// standard Windows headers.
#define XMD_H

#include <jpeg/jpeglib.h>

} // extern "C"

#ifdef __PCL_WINDOWS
#  define STRNCASECMP   ::strnicmp
#else
#  define STRNCASECMP   ::strncasecmp
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Custom JPEG error manager.
 *
 * We want to throw a C++ exception when an error occurs within the IJG
 * library. This is done in routine JPEGErrorExit() below.
 */
struct JPEGErrorManager
{
   /*
    * ### N.B.: Do not change the order of members in this structure. A pointer
    * to this structure must be castable to ::jpeg_error_mgr*.
    */
   ::jpeg_error_mgr error;
   String           path;

   JPEGErrorManager( const String& filePath ) : path( filePath )
   {
   }

   ::jpeg_error_mgr* ToJPEGLibStdError()
   {
      return ::jpeg_std_error( (::jpeg_error_mgr*)this );
   }
};

static void JPEGErrorExit( j_common_ptr cinfo )
{
   const jpeg_error_mgr* e = (const jpeg_error_mgr*)cinfo->err;
   const JPEGErrorManager* p = (const JPEGErrorManager*)e;
   char msg[ JMSG_LENGTH_MAX+16 ];
   e->format_message( cinfo, msg );
   throw JPEG::Error( p->path, String( msg ) );
}

// ----------------------------------------------------------------------------

struct JPEGFileData
{
   ByteArray                           data;
   AutoPointer<jpeg_compress_struct>   compressor;   // IJG's compression struct
   AutoPointer<jpeg_decompress_struct> decompressor; // IJG's decompression strct
   AutoPointer<JPEGErrorManager>       errorManager; // custom IJG error mgr

   double lowerRange;   // safe copies of critical parameters
   double upperRange;

   JPEGFileData() = default;

   ~JPEGFileData()
   {
      if ( decompressor )
      {
         ::jpeg_destroy_decompress( decompressor );
         decompressor.Destroy();
      }
      if ( compressor )
      {
         ::jpeg_destroy_compress( compressor );
         compressor.Destroy();
      }
      errorManager.Destroy();
   }
};

// ----------------------------------------------------------------------------

JPEG::JPEG()
{
}

// ----------------------------------------------------------------------------

JPEG::~JPEG() noexcept( false )
{
   CloseStream();
}

// ----------------------------------------------------------------------------

void JPEG::CloseStream()
{
   // Close the JPEG file stream and clean up IJG software data.
   m_fileData.Destroy();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

JPEGReader::~JPEGReader() noexcept( false )
{
}

// ----------------------------------------------------------------------------

bool JPEGReader::IsOpen() const
{
   return m_fileData && !m_fileData->data.IsEmpty();
}

// ----------------------------------------------------------------------------

void JPEGReader::Close()
{
   JPEG::CloseStream();
}

// ----------------------------------------------------------------------------

void JPEGReader::Open( const String& _path )
{
   if ( IsOpen() )
      throw JPEG::InvalidReadOperation( m_path );

   if ( _path.IsEmpty() )
      throw JPEG::InvalidFilePath( _path );

   if ( m_image.options.lowerRange >= m_image.options.upperRange )
      throw JPEG::InvalidNormalizationRange( _path );

   m_image.info.Reset();

   try
   {
      m_fileData = new JPEGFileData;

      // Keep critical parameters secured in private data.
      m_fileData->lowerRange = m_image.options.lowerRange;
      m_fileData->upperRange = m_image.options.upperRange;

#ifdef __PCL_WINDOWS
      m_path = File::WindowsPathToUnix( _path );
#else
      m_path = _path;
#endif

      m_fileData->data = File::ReadFile( m_path );

      /*
       * Allocate and initialize a JPEG decompression object.
       */

      // This struct contains the JPEG decompression parameters and pointers to
      // working space, which is allocated as needed by the IJG library.
      m_fileData->decompressor = new ::jpeg_decompress_struct;

      // Set up normal JPEG error routines, passing a pointer to our custom
      // error handler structure.
      m_fileData->errorManager = new JPEGErrorManager( m_path );
      m_fileData->decompressor->err = m_fileData->errorManager->ToJPEGLibStdError();

      // Override error_exit. JPEG library errors will be handled by throwing
      // exceptions from the JPEGErrorExit() routine.
      m_fileData->errorManager->error.error_exit = JPEGErrorExit;

      // Initialize the JPEG decompression object.
      ::jpeg_create_decompress( m_fileData->decompressor );

      // Specify memory data source.
      ::jpeg_mem_src( m_fileData->decompressor, m_fileData->data.Begin(), m_fileData->data.Length() );

      /*
       * Read file parameters.
       */
      ::jpeg_save_markers( m_fileData->decompressor, JPEG_COM,     0xFFFF );
      ::jpeg_save_markers( m_fileData->decompressor, JPEG_APP0+ 0, 0xFFFF );
      ::jpeg_save_markers( m_fileData->decompressor, JPEG_APP0+ 1, 0xFFFF );
      ::jpeg_save_markers( m_fileData->decompressor, JPEG_APP0+ 2, 0xFFFF );
      ::jpeg_save_markers( m_fileData->decompressor, JPEG_APP0+ 3, 0xFFFF );
      ::jpeg_save_markers( m_fileData->decompressor, JPEG_APP0+ 4, 0xFFFF );
      ::jpeg_save_markers( m_fileData->decompressor, JPEG_APP0+ 5, 0xFFFF );
      ::jpeg_save_markers( m_fileData->decompressor, JPEG_APP0+ 6, 0xFFFF );
      ::jpeg_save_markers( m_fileData->decompressor, JPEG_APP0+ 7, 0xFFFF );
      ::jpeg_save_markers( m_fileData->decompressor, JPEG_APP0+ 8, 0xFFFF );
      ::jpeg_save_markers( m_fileData->decompressor, JPEG_APP0+ 9, 0xFFFF );
      ::jpeg_save_markers( m_fileData->decompressor, JPEG_APP0+10, 0xFFFF );
      ::jpeg_save_markers( m_fileData->decompressor, JPEG_APP0+11, 0xFFFF );
      ::jpeg_save_markers( m_fileData->decompressor, JPEG_APP0+12, 0xFFFF );
      ::jpeg_save_markers( m_fileData->decompressor, JPEG_APP0+13, 0xFFFF );
      ::jpeg_save_markers( m_fileData->decompressor, JPEG_APP0+14, 0xFFFF );
      ::jpeg_save_markers( m_fileData->decompressor, JPEG_APP0+15, 0xFFFF );

      ::jpeg_read_header( m_fileData->decompressor, TRUE/*require_image*/ );

      /*
       * Deal with the JFIF marker and associated image resolution data.
       */
      if ( (m_jpegOptions.JFIFMarker = m_fileData->decompressor->saw_JFIF_marker) != false )
      {
         m_jpegOptions.JFIFMajorVersion = m_fileData->decompressor->JFIF_major_version;
         m_jpegOptions.JFIFMinorVersion = m_fileData->decompressor->JFIF_minor_version;
         m_image.options.metricResolution = m_fileData->decompressor->density_unit == 2;
         m_image.options.xResolution = m_fileData->decompressor->X_density;
         m_image.options.yResolution = m_fileData->decompressor->Y_density;
      }
      else
      {
         // Set default JFIF version and resolution data if a JFIF marker is
         // not present in this JPEG file.
         m_jpegOptions.JFIFMajorVersion = 1;
         m_jpegOptions.JFIFMinorVersion = 2;
         m_image.options.metricResolution = false;
         m_image.options.xResolution = m_image.options.yResolution = 72;
      }

      /*
       * Set sample format parameters.
       */
      m_image.options.bitsPerSample = 8;
      m_image.options.signedIntegers = m_image.options.complexSample = m_image.options.ieeefpSampleFormat = false;

      /*
       * Extract JPEG quality and other format-specific parameters.
       */
      m_jpegOptions.quality = JPEGQuality::Unknown;
      if ( m_fileData->decompressor->marker_list != nullptr )
      {
         const jpeg_marker_struct* m = m_fileData->decompressor->marker_list;

         do
         {
            if ( !STRNCASECMP( (const char*)m->data, "JPEGQuality", 11 ) )
            {
               // Skip marker header + null char
               m_jpegOptions.quality = Range( int( *(m->data + 12) ), 1, 100 );
               break;
            }
            else if ( !STRNCASECMP( (const char*)m->data, "JPEG Quality", 12 ) ||
                      !STRNCASECMP( (const char*)m->data, "JPEG_Quality", 12 ) )
            {
               // Skip marker header + null char
               m_jpegOptions.quality = Range( int( *(m->data + 13) ), 1, 100 );
               break;
            }
         }
         while ( (m = m->next) != nullptr );
      }

      m_jpegOptions.progressive = m_fileData->decompressor->progressive_mode;
      m_jpegOptions.arithmeticCoding = m_fileData->decompressor->arith_code;

      /*
       * Fill in ImageInfo and JPEGFileData fields.
       */
      ::jpeg_calc_output_dimensions( m_fileData->decompressor );

      m_image.info.width = m_fileData->decompressor->output_width;
      m_image.info.height = m_fileData->decompressor->output_height;
      m_image.info.numberOfChannels = m_fileData->decompressor->out_color_components;
      m_image.info.colorSpace = (m_fileData->decompressor->out_color_space == JCS_GRAYSCALE) ?
                                             ColorSpace::Gray : ColorSpace::RGB;
      m_image.info.supported = true;
   }
   catch ( ... )
   {
      Close();
      throw;
   }
}

// ----------------------------------------------------------------------------

ICCProfile JPEGReader::ReadICCProfile()
{
   if ( !IsOpen() )
      throw JPEG::InvalidReadOperation( String() );

   if ( m_fileData->decompressor->marker_list == nullptr )
      return ICCProfile();

   ByteArray iccData;
   uint32 iccSize = 0;
   uint32 iccChunkOffset = 0;

   const ::jpeg_marker_struct* m = m_fileData->decompressor->marker_list;

   do
   {
      if ( !STRNCASECMP( (const char*)m->data, "ICC_PROFILE", 11 ) )
      {
         /*
          * ICC_PROFILE 0 i N ddddddddddddd...ddd
          * -----------
          *   iCC id
          *
          * 0 = nul character
          * i = index number of this chunk
          * N = total number of chunks
          * d = data bytes in this chunk
          */
         void* iccChunkData = m->data + 14;

         // Allocate ICC data block when we see the first chunk.
         if ( iccChunkOffset == 0 )
         {
            // iccChunkData points now to an ICC profile header.
            // The first field in an ICC profile header is the uint32
            // profile length in bytes. ICC profiles are encoded using
            // big endian byte order.
            iccSize = BigToLittleEndian( *(const uint32*)iccChunkData );
            if ( iccSize == 0 )
               return ICCProfile();

            iccData = ByteArray( size_type( iccSize ) );
         }

         uint32 iccChunkSize = m->data_length - 14;

         // Guard us against insane ICC profile data.
         if ( iccChunkOffset+iccChunkSize > iccSize )
            return ICCProfile();

         ::memcpy( iccData.At( iccChunkOffset ), iccChunkData, iccChunkSize );

         iccChunkOffset += iccChunkSize;

         // More chunks ?
         if ( m->data[12] >= m->data[13] )
            break;
      }
   }
   while ( (m = m->next) != nullptr );

   return ICCProfile( iccData );
}

// ----------------------------------------------------------------------------

template <class P>
static void ReadJPEGImage( GenericImage<P>& image, JPEGReader& reader, JPEGFileData* fileData )
{
   if ( !reader.IsOpen() )
      throw JPEG::InvalidReadOperation( String() );

   try
   {
      /*
       * Set parameters for decompression.
       * Most parameters have already been established by Open().
       * We just ensure that we'll get either a grayscale or a RGB color image.
       */
      if ( fileData->decompressor->out_color_space != JCS_GRAYSCALE )
         fileData->decompressor->out_color_space = JCS_RGB;

      /*
       * Start decompression.
       */
      ::jpeg_start_decompress( fileData->decompressor );

      /*
       * Allocate pixel data.
       */
      image.AllocateData( fileData->decompressor->output_width,
                          fileData->decompressor->output_height,
                          fileData->decompressor->output_components,
                          (fileData->decompressor->out_color_space == JCS_GRAYSCALE) ?
                                                      ColorSpace::Gray : ColorSpace::RGB );

      /*
       * Read pixels row by row.
       */

      StatusMonitor monitor;
      StandardStatus status;
      if ( reader.JPEGOptions().verbosity > 0 )
      {
         monitor.SetCallback( &status );
         monitor.Initialize( String().Format(
               "Decompressing JPEG: %d channel(s), %dx%d pixels",
               image.NumberOfChannels(), image.Width(), image.Height() ), image.NumberOfSamples() );
      }

      // JSAMPLEs per row in output buffer.
      int rowStride = image.Width() * image.NumberOfChannels();

      // Make a one-row-high sample array.
      Array<JSAMPLE> buffer( rowStride );

      // JPEG organization is chunky; PCL images are planar.
      Array<typename P::sample*> samples( image.NumberOfChannels() );

      while ( fileData->decompressor->output_scanline < fileData->decompressor->output_height )
      {
         {
            JSAMPLE* p = buffer.Begin();
            ::jpeg_read_scanlines( fileData->decompressor, &p, 1 );
         }

         const JSAMPLE* b = buffer.Begin();

         for ( int c = 0; c < image.NumberOfChannels(); ++c )
            samples[c] = image.ScanLine( fileData->decompressor->output_scanline-1, c );

         for ( int i = 0; i < image.Width(); ++i )
            for ( int c = 0; c < image.NumberOfChannels(); ++c, ++b )
               *samples[c]++ = P::IsFloatSample() ? typename P::sample( *b ) : P::ToSample( *b );

         if ( reader.JPEGOptions().verbosity > 0 )
            monitor += rowStride;
      }

      /*
       * Finish decompression.
       */
      ::jpeg_finish_decompress( fileData->decompressor );

      // ### TODO --> At this point we might check whether any corrupt-data
      // warnings occurred (test whether jerr.pub.num_warnings is nonzero).
   }
   catch ( ... )
   {
      reader.Close();
      image.FreeData();
      throw;
   }
}

#define NORMALIZE( I )                                                           \
   if ( m_image.options.readNormalized )                                         \
   {                                                                             \
      if ( m_fileData->lowerRange != 0 || m_fileData->upperRange != 255 )        \
      {                                                                          \
         double oDelta = m_fileData->upperRange - m_fileData->lowerRange;        \
                                                                                 \
         for ( int c = 0; c < image.NumberOfChannels(); ++c )                    \
         {                                                                       \
            I::sample* f = image.PixelData( c );                                 \
            const I::sample* f1 = f + image.NumberOfPixels();                    \
                                                                                 \
            if ( oDelta != 1 || m_fileData->lowerRange != 0 )                    \
               for ( ; f < f1; ++f )                                             \
                  *f = Range( (*f/255)*oDelta + m_fileData->lowerRange,          \
                              m_fileData->lowerRange, m_fileData->upperRange );  \
            else                                                                 \
               for ( ; f < f1; ++f )                                             \
                  *f /= 255;                                                     \
         }                                                                       \
      }                                                                          \
   }

void JPEGReader::ReadImage( FImage& image )
{
   ReadJPEGImage( image, *this, m_fileData );
   NORMALIZE( FImage )
}

void JPEGReader::ReadImage( DImage& image )
{
   ReadJPEGImage( image, *this, m_fileData );
   NORMALIZE( DImage )
}

void JPEGReader::ReadImage( UInt8Image& image )
{
   ReadJPEGImage( image, *this, m_fileData );
}

void JPEGReader::ReadImage( UInt16Image& image )
{
   ReadJPEGImage( image, *this, m_fileData );
}

void JPEGReader::ReadImage( UInt32Image& image )
{
   ReadJPEGImage( image, *this, m_fileData );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

JPEGWriter::~JPEGWriter() noexcept( false )
{
}

// ----------------------------------------------------------------------------

bool JPEGWriter::IsOpen() const
{
   return m_fileData && !m_path.IsEmpty();
}

// ----------------------------------------------------------------------------

void JPEGWriter::Close()
{
   JPEG::CloseStream();
}

// ----------------------------------------------------------------------------

void JPEGWriter::Create( const String& filePath )
{
   if ( IsOpen() )
      throw JPEG::InvalidWriteOperation( m_path );

   if ( filePath.IsEmpty() )
      throw JPEG::InvalidFilePath( filePath );

   Reset();

   if ( !m_fileData )
      m_fileData = new JPEGFileData;

#ifdef __PCL_WINDOWS
      m_path = File::WindowsPathToUnix( filePath );
#else
      m_path = filePath;
#endif
}

// ----------------------------------------------------------------------------

void JPEGWriter::WriteICCProfile( const ICCProfile& icc )
{
   m_iccProfile = icc;
}

// ----------------------------------------------------------------------------

template <class P> inline
static void WriteJPEGImage( const GenericImage<P>& image, JPEGWriter& writer,
                            const ICCProfile& icc, JPEGFileData* fileData )
{
   if ( !writer.IsOpen() )
      throw JPEG::WriterNotInitialized( String() );

   if ( writer.Options().lowerRange >= writer.Options().upperRange )
      throw JPEG::InvalidNormalizationRange( writer.Path() );

   if ( image.IsEmptySelection() )
      throw JPEG::InvalidPixelSelection( writer.Path() );

   // Retrieve information on selected subimage.
   Rect r = image.SelectedRectangle();
   int width = r.Width();
   int height = r.Height();
   int channels = image.NumberOfSelectedChannels();

   // JPEG doesn't support alpha channels, just plain grayscale and RGB images.
   if ( channels != 1 && channels != 3 )
      throw JPEG::InvalidChannelSelection( writer.Path() );

   unsigned char qualityMarker[] =
   { 'J', 'P', 'E', 'G', 'Q', 'u', 'a', 'l', 'i', 't', 'y', 0, 0 };

   unsigned char* outputBuffer = nullptr;
   unsigned long outputSize = 0;

   try
   {
      // Make safe copies of critical parameters.
      fileData->lowerRange = writer.Options().lowerRange;
      fileData->upperRange = writer.Options().upperRange;

      StatusMonitor monitor;
      StandardStatus status;
      if ( writer.JPEGOptions().verbosity > 0 )
      {
         monitor.SetCallback( &status );
         monitor.Initialize( String().Format(
               "Compressing JPEG: %d channel(s), %dx%d pixels",
               channels, width, height ), image.NumberOfSelectedSamples() );
      }

      /*
       * Allocate and initialize a JPEG compressor instance.
       */

      // This struct contains the JPEG compression parameters and pointers to
      // working space, which is allocated as needed by the IJG library.
      fileData->compressor = new ::jpeg_compress_struct;

      // Set up normal JPEG error routines, passing a pointer to our custom
      // error handler structure.
      fileData->errorManager = new JPEGErrorManager( writer.Path() );
      fileData->compressor->err = fileData->errorManager->ToJPEGLibStdError();

      // Override error_exit. JPEG library errors will be handled by throwing
      // exceptions from the JPEGErrorExit() routine.
      fileData->errorManager->error.error_exit = JPEGErrorExit;

      // Initialize the JPEG compression object.
      ::jpeg_create_compress( fileData->compressor );

      /*
       * Set parameters for compression.
       */

      fileData->compressor->image_width = width;
      fileData->compressor->image_height = height;
      fileData->compressor->input_components = channels;
      fileData->compressor->in_color_space = (channels == 1) ? JCS_GRAYSCALE : JCS_RGB;

      // Set default compression parameters.
      ::jpeg_set_defaults( fileData->compressor );

      // Set any non-default parameters.

      if ( writer.JPEGOptions().quality == JPEGQuality::Unknown )
         const_cast<JPEGImageOptions&>( writer.JPEGOptions() ).quality = JPEGQuality::Default;

      // Set up compression according to specified JPEG quality.
      // We limit to baseline-JPEG values (TRUE arg in jpeg_set_quality).
      ::jpeg_set_quality( fileData->compressor, writer.JPEGOptions().quality, TRUE );

      fileData->compressor->optimize_coding = boolean( writer.JPEGOptions().optimizedCoding );
      fileData->compressor->arith_code = boolean( writer.JPEGOptions().arithmeticCoding );
      fileData->compressor->dct_method = JDCT_FLOAT;
      fileData->compressor->write_JFIF_header = boolean( writer.JPEGOptions().JFIFMarker );
      fileData->compressor->JFIF_major_version = writer.JPEGOptions().JFIFMajorVersion;
      fileData->compressor->JFIF_minor_version = writer.JPEGOptions().JFIFMinorVersion;
      fileData->compressor->density_unit = writer.Options().metricResolution ? 2 : 1;
      fileData->compressor->X_density = RoundI( writer.Options().xResolution );
      fileData->compressor->Y_density = RoundI( writer.Options().yResolution );

      if ( writer.JPEGOptions().progressive )
         ::jpeg_simple_progression( fileData->compressor );

      // Specify memory data destination.

//       fileData->compressor->dest->init_destination = JPEGCompressionData::InitDestination;
//       fileData->compressor->dest->empty_output_buffer = JPEGCompressionData::EmptyOutputBuffer;
//       fileData->compressor->dest->term_destination = JPEGCompressionData::TermDestination;

      ::jpeg_mem_dest( fileData->compressor, &outputBuffer, &outputSize );

      // Start compressor.
      ::jpeg_start_compress( fileData->compressor, TRUE );

      /*
       * Output ICC Profile data.
       * We use APP2 markers, as stated on ICC.1:2001-12.
       */
      if ( writer.Options().embedICCProfile && icc.IsProfile() )
      {
         // Get ICC profile size in bytes from the ICC profile header
         uint32 byteCount = uint32( icc.ProfileSize() );

         if ( byteCount != 0 )
         {
            // The ICC profile must be writen as a sequence of chunks if its
            // size is larger than (roughly) 64K.

            // Number of whole chunks.
            int chunkCount = byteCount/0xFF00;

            // Remainder chunk size in bytes.
            int lastChunk = byteCount - chunkCount*0xFF00;

            // chunkCount includes a possible remainder from now on.
            if ( lastChunk != 0 )
               ++chunkCount;

            // Allocate a working buffer. 64K bytes to hold chunk data, ICC
            // marker id, etc.
            ByteArray iccData( 0xFFFF );

            // ICC JFIF marker identifier, as recommended by ICC.1:2001-12.
            // Note that this is a null-terminated string, thus the total
            // length is 12 bytes.
            ::memcpy( iccData.Begin(), "ICC_PROFILE", 12 );

            // Byte #12, next to id's null terminator, is a 1-based chunk index.
            // Byte #13 is the total count of chunks (including remainder).
            iccData[13] = uint8( chunkCount );

            // Write sequence of markers

            for ( int i = 1, offset = 0; i <= chunkCount; ++i )
            {
               // Size in bytes of this chunk.
               int size = (i == chunkCount) ? lastChunk : 0xFF00;

               // Copy ICC profile data for this chunk, preserve ICC id, etc.
               ::memcpy( iccData.At( 14 ), icc.ProfileData().At( offset ), size );

               // Keep track of this chunk's index number, one-based.
               iccData[12] = uint8( i );

               // Output an APP2 marker for this chunk.
               ::jpeg_write_marker( fileData->compressor, JPEG_APP0+2, (const JOCTET *)iccData.Begin(), size+14 );

               // Prepare for next chunk.
               offset += size;
            }
         }
      }

      /*
       * Output a JPEGQuality marker.
       * We use an APP15 marker for quality.
       */
      qualityMarker[12] = writer.JPEGOptions().quality;
      ::jpeg_write_marker( fileData->compressor, JPEG_APP0+15, qualityMarker, 13 );

      /*
       * Write pixels row by row.
       */

      // JSAMPLEs per row in image_buffer.
      int rowStride = width * channels;

      // Make a one-row-high sample array.
      Array<JSAMPLE> buffer( rowStride );

      // JPEG organization is chunky; PCL images are planar.
      Array<const typename P::sample*> samples( channels );

      // Either rescaling or truncating to the [0,1] range is mandatory when
      // writing floating-point images to JPEG files.
      bool rescale = P::IsFloatSample() && (fileData->upperRange != 1 || fileData->lowerRange != 0);
      double k = 1.0;
      if ( rescale )
         k /= fileData->upperRange - fileData->lowerRange;

      while ( fileData->compressor->next_scanline < fileData->compressor->image_height )
      {
         JSAMPLE* b = buffer.Begin();

         for ( int c = 0; c < channels; ++c )
            samples[c] = image.PixelAddress( r.x0,
                                           r.y0 + fileData->compressor->next_scanline,
                                           image.FirstSelectedChannel() + c );

         for ( int i = 0; i < width; ++i )
            for ( int c = 0; c < channels; ++c, ++b )
            {
               typename P::sample f = *samples[c]++;
               if ( P::IsFloatSample() )
               {
                  f = typename P::sample( Range( double( f ), fileData->lowerRange, fileData->upperRange ) );
                  if ( rescale )
                     f = typename P::sample( k*(f - fileData->lowerRange) );
               }
               P::FromSample( *b, f );
            }

         b = buffer.Begin();
         ::jpeg_write_scanlines( fileData->compressor, &b, 1 );

         if ( writer.JPEGOptions().verbosity > 0 )
            monitor += rowStride;
      }

      /*
       * Finish JPEG compression.
       */
      ::jpeg_finish_compress( fileData->compressor );

      /*
       * Write the output file.
       */
      if ( outputBuffer != nullptr )
         if ( outputSize > 0 )
            File::WriteFile( writer.Path(), outputBuffer, outputSize );

      /*
       * Release the JPEG compression object.
       */
      ::jpeg_destroy_compress( fileData->compressor );
      fileData->compressor.Destroy();

      /*
       * Release the memory buffer.
       */
      if ( outputBuffer != nullptr )
         ::free( outputBuffer );
   }
   catch ( ... )
   {
      if ( outputBuffer != nullptr )
         ::free( outputBuffer );
      throw;
   }
}

void JPEGWriter::WriteImage( const FImage& image )
{
   WriteJPEGImage( image, *this, m_iccProfile, m_fileData );
}

void JPEGWriter::WriteImage( const DImage& image )
{
   WriteJPEGImage( image, *this, m_iccProfile, m_fileData );
}

void JPEGWriter::WriteImage( const UInt8Image& image )
{
   WriteJPEGImage( image, *this, m_iccProfile, m_fileData );
}

void JPEGWriter::WriteImage( const UInt16Image& image )
{
   WriteJPEGImage( image, *this, m_iccProfile, m_fileData );
}

void JPEGWriter::WriteImage( const UInt32Image& image )
{
   WriteJPEGImage( image, *this, m_iccProfile, m_fileData );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF JPEG.cpp - Released 2019-01-21T12:06:31Z
