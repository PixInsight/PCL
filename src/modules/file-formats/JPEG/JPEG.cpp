//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard JPEG File Format Module Version 01.00.03.0295
// ----------------------------------------------------------------------------
// JPEG.cpp - Released 2016/02/21 20:22:34 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

// stdio.h must be included before jpeglib.h for things to work properly.
#include <stdio.h>

// Including jpeglib.h requires some hacking, especially on Windows.

#ifdef FAR
#  undef FAR
#endif

extern "C" {

// Ensure that boolean is properly defined on Windows
#if defined( __PCL_WINDOWS ) && defined( __RPCNDR_H__ ) && !defined( boolean )
typedef unsigned char boolean;
#define HAVE_BOOLEAN
#endif

// Simulate that we are on X11 to leave INT16 and INT32 as defined by Windows headers
#define XMD_H
#include <jpeglib.h>

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
 * We want to throw a C++ exception when an error occurs within the IJG
 * library. This is done in routine JPEGErrorExit() below.
 */
struct JPEGErrorManager
{
   // ### Do not change the order of members in this structure.
   //     A pointer to this structure must be castable to ::jpeg_error_mgr*.
   ::jpeg_error_mgr error;
   String           path;

   JPEGErrorManager( const String& _path ) : path( _path )
   {
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
   FILE*  handle       = nullptr; // file stream
   void*  compressor   = nullptr; // IJG's compression struct
   void*  decompressor = nullptr; // IJG's decompression strct
   void*  errorManager = nullptr; // custom IJG error mgr

   double lowerRange;   // safe copies of critical parameters
   double upperRange;

   JPEGFileData() = default;

   ~JPEGFileData()
   {
      if ( decompressor != nullptr )
      {
         ::jpeg_destroy_decompress( j_decompress_ptr( decompressor ) );
         delete j_decompress_ptr( decompressor ), decompressor = nullptr;
      }

      if ( handle != nullptr )
         ::fclose( handle ), handle = nullptr;

      if ( compressor != nullptr )
      {
         ::jpeg_destroy_compress( j_compress_ptr( compressor ) );
         delete j_compress_ptr( compressor ), compressor = nullptr;
      }

      if ( errorManager != nullptr )
         delete (JPEGErrorManager*)errorManager, errorManager = nullptr;
   }
};

// ----------------------------------------------------------------------------

void JPEG::CloseStream()
{
   // Close the JPEG file stream and clean up IJG software data.
   if ( fileData != nullptr )
      delete fileData, fileData = nullptr;
}

// ----------------------------------------------------------------------------

#define jpeg_decompressor  ((j_decompress_ptr)fileData->decompressor)
#define jpeg_compressor    ((j_compress_ptr)fileData->compressor)

// ----------------------------------------------------------------------------

bool JPEGReader::IsOpen() const
{
   return fileData != nullptr && fileData->handle != nullptr;
}

void JPEGReader::Close()
{
   JPEG::CloseStream();
}

#define info      ((*images).info)
#define options   ((*images).options)

void JPEGReader::Open( const String& _path )
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
      fileData = new JPEGFileData;
#ifdef __PCL_WINDOWS
      path = File::WindowsPathToUnix( _path );
#else
      path = _path;
#endif

      // Keep critical parameters secured in private data.
      fileData->lowerRange = options.lowerRange;
      fileData->upperRange = options.upperRange;

      IsoString path8 =
#ifdef __PCL_WINDOWS
         File::UnixPathToWindows( path ).ToMBS();
#else
         path.ToUTF8();
#endif
      fileData->handle = ::fopen( path8.c_str(), "rb" );

      if ( fileData->handle == 0 )
         throw UnableToOpenFile( path );

      // Allocate and initialize a JPEG decompression object.

      // This struct contains the JPEG decompression parameters and pointers to
      // working space, which is allocated as needed by the IJG library.
      fileData->decompressor = new ::jpeg_decompress_struct;

      // Set up normal JPEG error routines, passing a pointer to our custom
      // error handler structure.
      jpeg_decompressor->err = ::jpeg_std_error( (::jpeg_error_mgr*)
                  (fileData->errorManager = new JPEGErrorManager( path )) );

      // Override error_exit. JPEG library errors will be handled by throwing
      // exceptions from the JPEGErrorExit() routine.
      ((JPEGErrorManager*)fileData->errorManager)->error.error_exit = JPEGErrorExit;

      // Initialize the JPEG decompression object.
      ::jpeg_create_decompress( jpeg_decompressor );

      // Specify data source (e.g., a file).
      ::jpeg_stdio_src( jpeg_decompressor, fileData->handle );

      // Read file parameters with ::jpeg_read_header().

      ::jpeg_save_markers( jpeg_decompressor, JPEG_COM,     0xFFFF );
      ::jpeg_save_markers( jpeg_decompressor, JPEG_APP0+ 0, 0xFFFF );
      ::jpeg_save_markers( jpeg_decompressor, JPEG_APP0+ 1, 0xFFFF );
      ::jpeg_save_markers( jpeg_decompressor, JPEG_APP0+ 2, 0xFFFF );
      ::jpeg_save_markers( jpeg_decompressor, JPEG_APP0+ 3, 0xFFFF );
      ::jpeg_save_markers( jpeg_decompressor, JPEG_APP0+ 4, 0xFFFF );
      ::jpeg_save_markers( jpeg_decompressor, JPEG_APP0+ 5, 0xFFFF );
      ::jpeg_save_markers( jpeg_decompressor, JPEG_APP0+ 6, 0xFFFF );
      ::jpeg_save_markers( jpeg_decompressor, JPEG_APP0+ 7, 0xFFFF );
      ::jpeg_save_markers( jpeg_decompressor, JPEG_APP0+ 8, 0xFFFF );
      ::jpeg_save_markers( jpeg_decompressor, JPEG_APP0+ 9, 0xFFFF );
      ::jpeg_save_markers( jpeg_decompressor, JPEG_APP0+10, 0xFFFF );
      ::jpeg_save_markers( jpeg_decompressor, JPEG_APP0+11, 0xFFFF );
      ::jpeg_save_markers( jpeg_decompressor, JPEG_APP0+12, 0xFFFF );
      ::jpeg_save_markers( jpeg_decompressor, JPEG_APP0+13, 0xFFFF );
      ::jpeg_save_markers( jpeg_decompressor, JPEG_APP0+14, 0xFFFF );
      ::jpeg_save_markers( jpeg_decompressor, JPEG_APP0+15, 0xFFFF );

      ::jpeg_read_header( jpeg_decompressor, boolean( true ) );

      // Deal with the JFIF marker and associated resolution data.

      if ( (jpegOptions.JFIFMarker = jpeg_decompressor->saw_JFIF_marker) != false )
      {
         jpegOptions.JFIFMajorVersion = jpeg_decompressor->JFIF_major_version;
         jpegOptions.JFIFMinorVersion = jpeg_decompressor->JFIF_minor_version;
         options.metricResolution = jpeg_decompressor->density_unit == 2;
         options.xResolution = jpeg_decompressor->X_density;
         options.yResolution = jpeg_decompressor->Y_density;
      }
      else
      {
         // Set default JFIF version and resolution data if a JFIF marker is
         // not present in this JPEG file.
         jpegOptions.JFIFMajorVersion = 1;
         jpegOptions.JFIFMinorVersion = 2;
         options.metricResolution = false;
         options.xResolution = options.yResolution = 72;
      }

      // Set sample format options.

      options.bitsPerSample = 8;
      options.signedIntegers = options.complexSample = options.ieeefpSampleFormat = false;

      // Extract JPEG quality from dedicated JPEG marker

      jpegOptions.quality = JPEGQuality::Unknown;

      if ( jpeg_decompressor->marker_list != nullptr )
      {
         const jpeg_marker_struct* m = jpeg_decompressor->marker_list;

         do
         {
            if ( !STRNCASECMP( (const char*)m->data, "JPEGQuality", 11 ) )
            {
               // Skip marker header + null char
               jpegOptions.quality = Range( int( *(m->data + 12) ), 1, 100 );
               break;
            }
            else if ( !STRNCASECMP( (const char*)m->data, "JPEG Quality", 12 ) ||
                      !STRNCASECMP( (const char*)m->data, "JPEG_Quality", 12 ) )
            {
               // Skip marker header + null char
               jpegOptions.quality = Range( int( *(m->data + 13) ), 1, 100 );
               break;
            }
         }
         while ( (m = m->next) != nullptr );
      }

      jpegOptions.progressive = jpeg_decompressor->progressive_mode;
      jpegOptions.arithmeticCoding = jpeg_decompressor->arith_code;

      // Fill in ImageInfo and JPEGFileData fields.

      ::jpeg_calc_output_dimensions( jpeg_decompressor );

      info.width = jpeg_decompressor->output_width;
      info.height = jpeg_decompressor->output_height;
      info.numberOfChannels = jpeg_decompressor->out_color_components;
      info.colorSpace = (jpeg_decompressor->out_color_space == JCS_GRAYSCALE) ?
                                             ColorSpace::Gray : ColorSpace::RGB;
      info.supported = true;
   }
   catch ( ... )
   {
      Close();
      throw;
   }
}

// ----------------------------------------------------------------------------

bool JPEGReader::Extract( ICCProfile& icc )
{
   icc.Clear();

   if ( !IsOpen() )
      return false;

   if ( jpeg_decompressor->marker_list == nullptr )
      return true;

   ByteArray iccData;
   uint32 iccSize = 0;
   uint32 iccChunkOffset = 0;

   const ::jpeg_marker_struct* m = jpeg_decompressor->marker_list;

   do
   {
      if ( !STRNCASECMP( (const char*)m->data, "ICC_PROFILE", 11 ) )
      {
         //
         // ICC_PROFILE 0 i N ddddddddddddd...ddd
         // -----------
         //   iCC id
         //
         // 0 = nul character
         // i = index number of this chunk
         // N = total number of chunks
         // d = data bytes in this chunk
         //
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
               return true;

            iccData = ByteArray( size_type( iccSize ) );
         }

         uint32 iccChunkSize = m->data_length - 14;

         // Guard us against insane ICC profile data.
         if ( iccChunkOffset+iccChunkSize > iccSize )
            return true;

         ::memcpy( iccData.At( iccChunkOffset ), iccChunkData, iccChunkSize );

         iccChunkOffset += iccChunkSize;

         // More chunks ?
         if ( m->data[12] >= m->data[13] )
            break;
      }
   }
   while ( (m = m->next) != nullptr );

   icc.Set( iccData );
   return true;
}

// ----------------------------------------------------------------------------

/*
bool JPEGReader::Extract( IPTCPhotoInfo& iptc )
{
   iptc.Clear();

   if ( !IsOpen() )
      return false;

   if ( jpeg_decompressor->marker_list == nullptr )
      return true;

   const ::jpeg_marker_struct* m = jpeg_decompressor->marker_list;
   do
   {
      if ( (!STRNCASECMP( (const char*)m->data, "IPTCPhotoInfo", 13 ) ||
            !STRNCASECMP( (const char*)m->data, "IPTC PhotoInfo", 14 ) ||
            !STRNCASECMP( (const char*)m->data, "IPTC_PhotoInfo", 14 ) ||
            !STRNCASECMP( (const char*)m->data, "IPTC Photo Info", 15 ) ||
            !STRNCASECMP( (const char*)m->data, "IPTC_Photo_Info", 15 )) )
      {
         // Find beginning of IPTC data block
         const char* iptcData = (const char*)::memchr( (const char*)m->data, 0x1c, m->data_length );
         if ( iptcData != nullptr )
            iptc.GetInfo( iptcData, m->data_length - (iptcData - (const char*)m->data) );
         break;
      }
   }
   while ( (m = m->next) != nullptr );

   return true;
}
*/
// ----------------------------------------------------------------------------

template <class P>
static void ReadJPEGImage( GenericImage<P>& img, JPEGReader& reader, JPEGFileData* fileData )
{
   if ( !reader.IsOpen() )
      throw JPEG::InvalidReadOperation( String() );

   JSAMPLE* buffer = nullptr;        // one-row sample array for scanline reading
   typename P::sample** v = nullptr; // pointers to destination scan lines

   try
   {
      // Set parameters for decompression.
      // Most parameters have already been established by Open().
      // We just ensure that we'll get either a grayscale or a RGB color image.
      if ( jpeg_decompressor->out_color_space != JCS_GRAYSCALE )
         jpeg_decompressor->out_color_space = JCS_RGB;

      // Start decompressor.
      ::jpeg_start_decompress( jpeg_decompressor );

      // Allocate pixel data.
      img.AllocateData( jpeg_decompressor->output_width,
                        jpeg_decompressor->output_height,
                        jpeg_decompressor->output_components,
                  (jpeg_decompressor->out_color_space == JCS_GRAYSCALE) ?
                        ColorSpace::Gray : ColorSpace::RGB );

      // Initialize status callback.
      if ( img.Status().IsInitializationEnabled() )
         img.Status().Initialize( String().Format(
                  "Decompressing JPEG: %d channel(s), %dx%d pixels",
                  img.NumberOfChannels(), img.Width(), img.Height() ), img.NumberOfSamples() );

      //
      // Read pixels row by row.
      //

      // JSAMPLEs per row in output buffer.
      int row_stride = img.Width() * img.NumberOfChannels();

      // Make a one-row-high sample array.
      buffer = new JSAMPLE[ row_stride ];

      // JPEG organization is chunky; PCL images are planar.
      v = new typename P::sample*[ img.NumberOfChannels() ];

      while ( jpeg_decompressor->output_scanline < jpeg_decompressor->output_height )
      {
         ::jpeg_read_scanlines( jpeg_decompressor, &buffer, 1 );

         const JSAMPLE* b = buffer;

         for ( int c = 0; c < img.NumberOfChannels(); ++c )
            v[c] = img.ScanLine( jpeg_decompressor->output_scanline-1, c );

         for ( int i = 0; i < img.Width(); ++i )
            for ( int c = 0; c < img.NumberOfChannels(); ++c, ++b )
               *v[c]++ = P::IsFloatSample() ? typename P::sample( *b ) : P::ToSample( *b );

         img.Status() += img.Width()*img.NumberOfChannels();
      }

      // Clean up temporary structures.
      delete [] v, v = nullptr;
      delete [] buffer, buffer = nullptr;

      // Finish decompression.
      ::jpeg_finish_decompress( jpeg_decompressor );

      // ### TODO --> At this point we might check whether any corrupt-data
      // warnings occurred (test whether jerr.pub.num_warnings is nonzero).
   }
   catch ( ... )
   {
      reader.Close();

      if ( buffer != nullptr )
         delete [] buffer;
      if ( v != nullptr )
         delete [] v;

      img.FreeData();

      throw;
   }
}

#define NORMALIZE( I )                                                        \
   if ( options.readNormalized )                                              \
   {                                                                          \
      if ( fileData->lowerRange != 0 || fileData->upperRange != 255 )         \
      {                                                                       \
         double oDelta = fileData->upperRange - fileData->lowerRange;         \
                                                                              \
         for ( int c = 0; c < img.NumberOfChannels(); ++c )                   \
         {                                                                    \
            I::sample* f = img.PixelData( c );                                \
            const I::sample* f1 = f + img.NumberOfPixels();                   \
                                                                              \
            if ( oDelta != 1 || fileData->lowerRange != 0 )                   \
               for ( ; f < f1; ++f )                                          \
                  *f = Range( (*f/255)*oDelta + fileData->lowerRange,         \
                              fileData->lowerRange, fileData->upperRange );   \
            else                                                              \
               for ( ; f < f1; ++f )                                          \
                  *f /= 255;                                                  \
         }                                                                    \
      }                                                                       \
   }

void JPEGReader::ReadImage( FImage& img )
{
   ReadJPEGImage( img, *this, fileData );
   NORMALIZE( FImage )
}

void JPEGReader::ReadImage( DImage& img )
{
   ReadJPEGImage( img, *this, fileData );
   NORMALIZE( DImage )
}

void JPEGReader::ReadImage( UInt8Image& img )
{
   ReadJPEGImage( img, *this, fileData );
}

void JPEGReader::ReadImage( UInt16Image& img )
{
   ReadJPEGImage( img, *this, fileData );
}

void JPEGReader::ReadImage( UInt32Image& img )
{
   ReadJPEGImage( img, *this, fileData );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#undef info
#undef options

bool JPEGWriter::IsOpen() const
{
   return fileData != nullptr && !path.IsEmpty();
}

void JPEGWriter::Close()
{
   JPEG::CloseStream();
}

void JPEGWriter::Create( const String& filePath, int count )
{
   if ( IsOpen() )
      throw InvalidWriteOperation( path );

   if ( filePath.IsEmpty() )
      throw InvalidFilePath( filePath );

   if ( count <= 0 )
      throw NotImplemented( *this, "Write empty JPEG files" );

   if ( count > 1 )
      throw NotImplemented( *this, "Write multiple images to a single JPEG file" );

   Reset();

   if ( fileData == nullptr )
      fileData = new JPEGFileData;

#ifdef __PCL_WINDOWS
      path = File::WindowsPathToUnix( filePath );
#else
      path = filePath;
#endif
}

// ----------------------------------------------------------------------------

template <class P> inline
static void WriteJPEGImage( const GenericImage<P>& img, JPEGWriter& writer,
                            const ICCProfile& icc, JPEGFileData* fileData )
{
   if ( !writer.IsOpen() )
      throw JPEG::WriterNotInitialized( String() );

   if ( writer.Options().lowerRange >= writer.Options().upperRange )
      throw JPEG::InvalidNormalizationRange( writer.Path() );

   if ( img.IsEmptySelection() )
      throw JPEG::InvalidPixelSelection( writer.Path() );

   // Retrieve information on selected subimage.
   Rect r = img.SelectedRectangle();
   int width = r.Width();
   int height = r.Height();
   int channels = img.NumberOfSelectedChannels();

   // JPEG doesn't support alpha channels, just plain grayscale and RGB images.
   if ( channels != 1 && channels != 3 )
      throw JPEG::InvalidChannelSelection( writer.Path() );

   unsigned char qualityMarker[] =
   { 'J', 'P', 'E', 'G', 'Q', 'u', 'a', 'l', 'i', 't', 'y', 0, 0 };

   try
   {
      // Make safe copies of critical parameters.
      fileData->lowerRange = writer.Options().lowerRange;
      fileData->upperRange = writer.Options().upperRange;

      // Initialize status callback.
      if ( img.Status().IsInitializationEnabled() )
         img.Status().Initialize( String().Format(
                  "Compressing JPEG: %d channel(s), %dx%d pixels",
                  channels, width, height ), img.NumberOfSelectedSamples() );

      //
      // Allocate and initialize a JPEG compressor instance.
      //

      // This struct contains the JPEG compression parameters and pointers to
      // working space, which is allocated as needed by the IJG library.
      fileData->compressor = new ::jpeg_compress_struct;

      // Set up normal JPEG error routines, passing a pointer to our custom
      // error handler structure.
      jpeg_compressor->err = ::jpeg_std_error(
         (::jpeg_error_mgr*)(fileData->errorManager = new JPEGErrorManager( writer.Path() )) );

      // Override error_exit. JPEG library errors will be handled by throwing
      // exceptions from the JPEGErrorExit() routine.
      ((JPEGErrorManager*)fileData->errorManager)->error.error_exit = JPEGErrorExit;

      // Initialize the JPEG compression object.
      ::jpeg_create_compress( jpeg_compressor );

      //
      // Set parameters for compression.
      //

      jpeg_compressor->image_width = width;
      jpeg_compressor->image_height = height;
      jpeg_compressor->input_components = channels;
      jpeg_compressor->in_color_space = (channels == 1) ? JCS_GRAYSCALE : JCS_RGB;

      // Set default compression parameters.
      ::jpeg_set_defaults( jpeg_compressor );

      // Set any non-default parameters.

      if ( writer.JPEGOptions().quality == JPEGQuality::Unknown )
         const_cast<JPEGImageOptions&>( writer.JPEGOptions() ).quality = JPEGQuality::Default;

      // Set up compression according to specified JPEG quality.
      // We limit to baseline-JPEG values (TRUE arg in jpeg_set_quality).
      ::jpeg_set_quality( jpeg_compressor, writer.JPEGOptions().quality, TRUE );

      jpeg_compressor->optimize_coding = boolean( writer.JPEGOptions().optimizedCoding );
      jpeg_compressor->arith_code = boolean( writer.JPEGOptions().arithmeticCoding );
      jpeg_compressor->dct_method = JDCT_FLOAT;
      jpeg_compressor->write_JFIF_header = boolean( writer.JPEGOptions().JFIFMarker );
      jpeg_compressor->JFIF_major_version = writer.JPEGOptions().JFIFMajorVersion;
      jpeg_compressor->JFIF_minor_version = writer.JPEGOptions().JFIFMinorVersion;
      jpeg_compressor->density_unit = writer.Options().metricResolution ? 2 : 1;
      jpeg_compressor->X_density = RoundI( writer.Options().xResolution );
      jpeg_compressor->Y_density = RoundI( writer.Options().yResolution );

      if ( writer.JPEGOptions().progressive )
         ::jpeg_simple_progression( jpeg_compressor );

      IsoString path8 =
#ifdef __PCL_WINDOWS
         File::UnixPathToWindows( writer.Path() ).ToMBS();
#else
         writer.Path().ToUTF8();
#endif
      fileData->handle = ::fopen( path8.c_str(), "wb" );

      if ( fileData->handle == 0 )
         throw JPEG::UnableToCreateFile( writer.Path() );

      // Specify data destination (e.g., a file).
      ::jpeg_stdio_dest( jpeg_compressor, fileData->handle );

      // Start compressor.
      ::jpeg_start_compress( jpeg_compressor, TRUE );

      // Output ICC Profile data.
      // We use APP2 markers, as stated on ICC.1:2001-12.

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
               ::jpeg_write_marker( jpeg_compressor, JPEG_APP0+2, (const JOCTET *)iccData.Begin(), size+14 );

               // Prepare for next chunk.
               offset += size;
            }
         }
      }

      /*
      // Output IPTC PhotoInfo data.
      // We use an APP13 marker, as recommended by IPTC.

      if ( iptc != nullptr )
      {
         size_type byteCount = iptc->MakeInfo( iptcPureData );

         if ( byteCount != 0 )
         {
            iptcData = new char[ byteCount+14 ];
            ::strcpy( (char*)iptcData, "IPTCPhotoInfo" );
            ::memcpy( ((char*)iptcData)+14, iptcPureData, byteCount );
            delete (uint8*)iptcPureData, iptcPureData = nullptr;

            ::jpeg_write_marker( jpeg_compressor, JPEG_APP0+13,
                  (const JOCTET *)iptcData, unsigned( byteCount+14 ) );

            delete (uint8*)iptcData, iptcData = nullptr;
         }
      }
      */

      // Output JPEGQuality marker.
      // We use an APP15 marker for quality.

      qualityMarker[12] = writer.JPEGOptions().quality;
      ::jpeg_write_marker( jpeg_compressor, JPEG_APP0+15, qualityMarker, 13 );

      //
      // Write pixels row by row.
      //

      // JSAMPLEs per row in image_buffer.
      int row_stride = width * channels;

      // Make a one-row-high sample array.
      Array<JSAMPLE> buffer( row_stride );

      // JPEG organization is chunky; PCL images are planar.
      Array<const typename P::sample*> v( channels );

      // Either rescaling or truncating to the [0,1] range is mandatory when
      // writing floating-point images to JPEG files.
      bool rescale = P::IsFloatSample() && (fileData->upperRange != 1 || fileData->lowerRange != 0);
      double k = 1.0;
      if ( rescale )
         k /= fileData->upperRange - fileData->lowerRange;

      while ( jpeg_compressor->next_scanline < jpeg_compressor->image_height )
      {
         JSAMPLE* b = buffer.Begin();

         for ( int c = 0; c < channels; ++c )
            v[c] = img.PixelAddress( r.x0,
                                     r.y0+jpeg_compressor->next_scanline,
                                     img.FirstSelectedChannel()+c );

         for ( int i = 0; i < width; ++i )
            for ( int c = 0; c < channels; ++c, ++b )
            {
               typename P::sample f = *v[c]++;

               if ( P::IsFloatSample() )
               {
                  f = typename P::sample( Range( double( f ), fileData->lowerRange, fileData->upperRange ) );
                  if ( rescale )
                     f = typename P::sample( k*(f - fileData->lowerRange) );
               }

               P::FromSample( *b, f );
            }

         b = buffer.Begin();
         ::jpeg_write_scanlines( jpeg_compressor, &b, 1 );

         img.Status() += width * channels;
         ++img.Status();
      }

      // Finish JPEG compression.
      ::jpeg_finish_compress( jpeg_compressor );

      // Close the output file.
      ::fclose( fileData->handle ), fileData->handle = nullptr;

      // Release the JPEG compression object.
      ::jpeg_destroy_compress( jpeg_compressor );
      delete jpeg_compressor, fileData->compressor = nullptr;
   }
   catch ( ... )
   {
      if ( fileData->handle != nullptr )
         ::fclose( fileData->handle ), fileData->handle = nullptr;
      throw;
   }
}

void JPEGWriter::WriteImage( const FImage& img )
{
   WriteJPEGImage( img, *this, icc, fileData );
}

void JPEGWriter::WriteImage( const DImage& img )
{
   WriteJPEGImage( img, *this, icc, fileData );
}

void JPEGWriter::WriteImage( const UInt8Image& img )
{
   WriteJPEGImage( img, *this, icc, fileData );
}

void JPEGWriter::WriteImage( const UInt16Image& img )
{
   WriteJPEGImage( img, *this, icc, fileData );
}

void JPEGWriter::WriteImage( const UInt32Image& img )
{
   WriteJPEGImage( img, *this, icc, fileData );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF JPEG.cpp - Released 2016/02/21 20:22:34 UTC
