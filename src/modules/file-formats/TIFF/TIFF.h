//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard TIFF File Format Module Version 01.00.07.0359
// ----------------------------------------------------------------------------
// TIFF.h - Released 2018-11-23T16:14:51Z
// ----------------------------------------------------------------------------
// This file is part of the standard TIFF PixInsight module.
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

#ifndef __PCL_TIFF_h
#define __PCL_TIFF_h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_AutoPointer_h
#include <pcl/AutoPointer.h>
#endif

#ifndef __PCL_File_h
#include <pcl/File.h>
#endif

#ifndef __PCL_ICCProfile_h
#include <pcl/ICCProfile.h>
#endif

#ifndef __PCL_Image_h
#include <pcl/Image.h>
#endif

#ifndef __PCL_ImageDescription_h
#include <pcl/ImageDescription.h>
#endif

#ifndef __PCL_Version_h
#include <pcl/Version.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

struct TIFFFileData;

// ----------------------------------------------------------------------------

/*
 * Supported TIFF compression algorithms
 */
namespace TIFFCompression
{
   enum value_type
   {
      Unknown  = 0x80,  // Unknown or unsupported TIFF compression
      None     = 0x00,  // No compression
      ZIP      = 0x01,  // ZLIB (deflate) compression: Open, efficient, recommended.
      LZW      = 0x02   // LZW: Less efficient than ZIP, had patent issues, discouraged.
   };
}

// ----------------------------------------------------------------------------

/*
 * TIFF-specific image file options
 */
class TIFFImageOptions
{
public:

   typedef TIFFCompression::value_type compression_codec;

   compression_codec compression         :  8; // Compression algorithm
   bool              planar              :  1; // Planar organization; chunky otherwise
   bool              associatedAlpha     :  1; // Associated alpha channel
   bool              premultipliedAlpha  :  1; // RGB/K premultiplied by alpha
   uint8             verbosity           :  3; // Verbosity level: 0 = quiet, > 0 = write console state messages.
   int               __rsv__             : 18; // Reserved for future extension --must be zero
   uint16            stripSize;                // The strip size in bytes for TIFF image file I/O

   String software;         // Software description
   String imageDescription; // Image description
   String copyright;        // Copyright notice

   TIFFImageOptions()
   {
      Reset();
   }

   TIFFImageOptions( const TIFFImageOptions& ) = default;

   TIFFImageOptions& operator =( const TIFFImageOptions& ) = default;

   void Reset()
   {
      compression        = TIFFCompression::None;
      planar             = false;
      associatedAlpha    = true;
      premultipliedAlpha = false;
      verbosity          = 1;
      __rsv__            = 0;
      stripSize          = 4096;
      software = PixInsightVersion::AsString() + " / " + Version::AsString();
      imageDescription.Clear();
      copyright.Clear();
   }
};

// ----------------------------------------------------------------------------

/*
 * Base class for the TIFFReader and TIFFWriter classes.
 */
class TIFF
{
public:

   /*
    * TIFF-specific exception class
    */
   class Error : public File::Error
   {
   public:

      Error( const String& filePath, const String& message ) :
         File::Error( filePath, message )
      {
      }

      Error( const TIFF::Error& x ) = default;

      virtual String ExceptionClass() const
      {
         return "PCL TIFF Format Support";
      }
   };

#define PCL_DECLARE_TIFF_ERROR( className, errorMessage )   \
   class className : public TIFF::Error                     \
   {                                                        \
   public:                                                  \
      className( const String& filePath ) :                 \
         TIFF::Error( filePath, errorMessage )              \
      {                                                     \
      }                                                     \
      className( const className& x ) = default;            \
   };

   PCL_DECLARE_TIFF_ERROR( InvalidFilePath,
                           "Internal error: Invalid file path" )
   PCL_DECLARE_TIFF_ERROR( WriterNotInitialized,
                           "Internal error: TIFFWriter instance not initialized" )
   PCL_DECLARE_TIFF_ERROR( UnableToOpenFile,
                           "Unable to open TIFF file" )
   PCL_DECLARE_TIFF_ERROR( UnableToCreateFile,
                           "Unable to create TIFF file" )
   PCL_DECLARE_TIFF_ERROR( FileReadError,
                           "Error reading TIFF file" )
   PCL_DECLARE_TIFF_ERROR( FileWriteError,
                           "Error writing TIFF file" )
   PCL_DECLARE_TIFF_ERROR( UnsupportedColorSpace,
                           "Unsupported TIFF color space: Must be either grayscale or RGB color" )
   PCL_DECLARE_TIFF_ERROR( UnsupportedBitsPerSample,
                           "Unsupported TIFF bits per sample: Must be 8|16|32|64 bits" )
   PCL_DECLARE_TIFF_ERROR( UnsupportedSampleFormat,
                           "Unsupported TIFF sample format: Must be 8|16|32-bit integers or 32|64-bit IEEE floating point" )
   PCL_DECLARE_TIFF_ERROR( InvalidNormalizationRange,
                           "Internal error: Invalid TIFF normalization range" )
   PCL_DECLARE_TIFF_ERROR( InvalidReadOperation,
                           "Internal error: Invalid TIFF read operation" )
   PCL_DECLARE_TIFF_ERROR( InvalidWriteOperation,
                           "Internal error: Invalid TIFF write operation" )

   TIFF();

   TIFF( const TIFF& ) = delete;
   TIFF& operator =( const TIFF& ) = delete;

   virtual ~TIFF() noexcept( false );

   const TIFFImageOptions& TIFFOptions() const
   {
      return m_tiffOptions;
   }

   void SetTIFFOptions( const TIFFImageOptions& options )
   {
      m_tiffOptions = options;
   }

   String Path() const
   {
      return m_path;
   }

protected:

   String                    m_path;
   TIFFImageOptions          m_tiffOptions;
   AutoPointer<TIFFFileData> m_fileData;

   void CloseStream(); // ### derived must call base
};

// ----------------------------------------------------------------------------

/*
 * TIFF image file reader
 */
class TIFFReader : public TIFF
{
public:

   TIFFReader();

   TIFFReader( const TIFFReader& ) = delete;
   TIFFReader& operator =( const TIFFReader& ) = delete;

   virtual ~TIFFReader() noexcept( false );

   bool IsOpen() const;
   void Close();
   void Open( const String& filePath );

   const ImageInfo& Info() const
   {
      return m_image.info;
   }

   const ImageOptions& Options() const
   {
      return m_image.options;
   }

   void SetOptions( const ImageOptions& options )
   {
      m_image.options = options;
   }

   ICCProfile ReadICCProfile();

   void ReadImage( FImage& );
   void ReadImage( DImage& );
   void ReadImage( UInt8Image& );
   void ReadImage( UInt16Image& );
   void ReadImage( UInt32Image& );

private:

   ImageDescription m_image;
};

// ----------------------------------------------------------------------------

/*
 * TIFF image file writer.
 */
class TIFFWriter : public TIFF
{
public:

   TIFFWriter();

   TIFFWriter( const TIFFWriter& ) = delete;
   TIFFWriter& operator =( const TIFFWriter& ) = delete;

   virtual ~TIFFWriter() noexcept( false );

   bool IsOpen() const;
   void Close();
   void Create( const String& filePath );

   const ImageOptions& Options() const
   {
      return m_options;
   }

   void SetOptions( const ImageOptions& options )
   {
      m_options = options;
   }

   void WriteICCProfile( const ICCProfile& icc );

   void WriteImage( const FImage& );
   void WriteImage( const DImage& );
   void WriteImage( const UInt8Image& );
   void WriteImage( const UInt16Image& );
   void WriteImage( const UInt32Image& );

   void Reset()
   {
      Close();
      m_iccProfile.Clear();
   }

private:

   ImageOptions m_options;
   ICCProfile   m_iccProfile;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_TIFF_h

// ----------------------------------------------------------------------------
// EOF TIFF.h - Released 2018-11-23T16:14:51Z
