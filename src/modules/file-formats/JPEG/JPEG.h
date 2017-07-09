//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard JPEG File Format Module Version 01.00.04.0334
// ----------------------------------------------------------------------------
// JPEG.h - Released 2017-07-09T18:07:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG PixInsight module.
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

#ifndef __PCL_JPEG_h
#define __PCL_JPEG_h

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

namespace pcl
{

// ----------------------------------------------------------------------------

struct JPEGFileData; // internal

// ----------------------------------------------------------------------------

/*
 * Standard JPEG quality values
 */
namespace JPEGQuality
{
   enum value_type
   {
      Minimum  =  10,
      VeryLow  =  30,
      Low      =  50,
      Mean     =  70,
      High     =  90,
      VeryHigh =  95,
      Maximum  = 100,

      Unknown  =   0,
      Default  =  80
   };
}

// ----------------------------------------------------------------------------

/*
 * JPEG-specific image file options.
 */
class JPEGImageOptions
{
public:

   uint8    quality           : 8; // Quality: 1 to 100, 0 = unknown/default quality
   bool     optimizedCoding   : 1; // Optimized coding, baseline otherwise
   bool     arithmeticCoding  : 1; // Arithmetic coding, Huffman otherwise
   bool     progressive       : 1; // Compress for progressive download
   uint32   __rsv1__          : 5; // reserved for future extension --must be zero
   bool     JFIFMarker        : 1; // Include JFIF marker
   uint8    JFIFMajorVersion  : 4; // JFIF main version number
   uint8    JFIFMinorVersion  : 4; // JFIF second version number
   uint8    verbosity         : 3; // verbosity level: 0 = quiet, > 0 = write console state messages.
   uint32   __rsv2__          : 4; // reserved for future extension --must be zero

   JPEGImageOptions()
   {
      Reset();
   }

   JPEGImageOptions( const JPEGImageOptions& ) = default;

   JPEGImageOptions& operator =( const JPEGImageOptions& ) = default;

   void Reset()
   {
      quality          = JPEGQuality::Default;
      optimizedCoding  = true;
      arithmeticCoding = false;
      progressive      = false;
      __rsv1__         = 0;
      JFIFMarker       = true;
      JFIFMajorVersion = 1;
      JFIFMinorVersion = 2;
      verbosity        = 1;
      __rsv2__         = 0;
   }
};

// ----------------------------------------------------------------------------

/*
 * Base class for the JPEGReader and JPEGWriter classes.
 */
class JPEG
{
public:

   /*
    * JPEG-specific exception class
    */
   class Error : public File::Error
   {
   public:

      Error( const String& filePath, const String& message ) :
         File::Error( filePath, message )
      {
      }

      Error( const JPEG::Error& x ) = default;

      virtual String ExceptionClass() const
      {
         return "PCL JPEG Format Support";
      }
   };

#define PCL_DECLARE_JPEG_ERROR( className, errorMessage )   \
   class className : public JPEG::Error                     \
   {                                                        \
   public:                                                  \
      className( const String& filePath ) :                 \
         JPEG::Error( filePath, errorMessage )              \
      {                                                     \
      }                                                     \
      className( const className& x ) = default;            \
   };

   PCL_DECLARE_JPEG_ERROR( InvalidFilePath,
                           "Internal error: Invalid file path" )
   PCL_DECLARE_JPEG_ERROR( WriterNotInitialized,
                           "Internal error: JPEGWriter instance not initialized" )
   PCL_DECLARE_JPEG_ERROR( UnableToOpenFile,
                           "Unable to open JPEG file" )
   PCL_DECLARE_JPEG_ERROR( UnableToCreateFile,
                           "Unable to create JPEG file" )
   PCL_DECLARE_JPEG_ERROR( InvalidNormalizationRange,
                           "Internal error: Invalid JPEG normalization range" )
   PCL_DECLARE_JPEG_ERROR( InvalidReadOperation,
                           "Internal error: Invalid JPEG read operation" )
   PCL_DECLARE_JPEG_ERROR( InvalidWriteOperation,
                           "Internal error: Invalid JPEG write operation" )
   PCL_DECLARE_JPEG_ERROR( InvalidPixelSelection,
                           "Internal error: Invalid pixel selection in JPEG write operation" )
   PCL_DECLARE_JPEG_ERROR( InvalidChannelSelection,
                           "Internal error: Invalid channel selection in JPEG write operation" )

   JPEG();

   JPEG( const JPEG& ) = delete;
   JPEG& operator =( const JPEG& ) = delete;

   virtual ~JPEG() noexcept( false );

   const JPEGImageOptions& JPEGOptions() const
   {
      return m_jpegOptions;
   }

   void SetJPEGOptions( const JPEGImageOptions& options )
   {
      m_jpegOptions = options;
   }

   String Path() const
   {
      return m_path;
   }

protected:

   String                    m_path;
   JPEGImageOptions          m_jpegOptions;
   AutoPointer<JPEGFileData> m_fileData;

   void CloseStream(); // ### derived must call base
};

// ----------------------------------------------------------------------------

/*
 * JPEG image file reader
 */
class JPEGReader : public JPEG
{
public:

   JPEGReader() = default;

   JPEGReader( const JPEGReader& ) = delete;
   JPEGReader& operator =( const JPEGReader& ) = delete;

   virtual ~JPEGReader() noexcept( false );

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
 * JPEG image file writer
 */
class JPEGWriter : public JPEG
{
public:

   JPEGWriter() = default;

   JPEGWriter( const JPEGWriter& ) = delete;
   JPEGWriter& operator =( const JPEGWriter& ) = delete;

   virtual ~JPEGWriter() noexcept( false );

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

protected:

   ImageOptions m_options;
   ICCProfile   m_iccProfile;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_JPEG_h

// ----------------------------------------------------------------------------
// EOF JPEG.h - Released 2017-07-09T18:07:25Z
