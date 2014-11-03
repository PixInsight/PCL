// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard JPEG File Format Module Version 01.00.01.0226
// ****************************************************************************
// JPEG.h - Released 2014/10/29 07:34:49 UTC
// ****************************************************************************
// This file is part of the standard JPEG PixInsight module.
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

#ifndef __PCL_JPEG_h
#define __PCL_JPEG_h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_ImageStream_h
#include <pcl/ImageStream.h>
#endif

#ifndef __PCL_ICCProfile_h
#include <pcl/ICCProfile.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

struct JPEGFileData; // internal

// ----------------------------------------------------------------------------

/*
   Standard JPEG quality values
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
   JPEG-specific image file options.
*/
class JPEGImageOptions
{
public:

   uint8    quality           : 8; // Quality: 1 to 100, 0 = unknown/default quality
   bool     optimizedCoding   : 1; // Optimized coding, baseline otherwise
   bool     arithmeticCoding  : 1; // Arithmetic coding, Huffman otherwise
   bool     progressive       : 1; // Compress for progressive download
   int      __rsv1__          : 5; // reserved for future extension --must be zero
   bool     JFIFMarker        : 1; // Include JFIF marker
   uint8    JFIFMajorVersion  : 4; // JFIF main version number
   uint8    JFIFMinorVersion  : 4; // JFIF second version number
   int      __rsv2__          : 7; // reserved for future extension --must be zero

   JPEGImageOptions()
   {
      Reset();
   }

   JPEGImageOptions( const JPEGImageOptions& x )
   {
      (void)operator =( x );
   }

   JPEGImageOptions& operator =( const JPEGImageOptions& x )
   {
      quality          = x.quality;
      optimizedCoding  = x.optimizedCoding;
      arithmeticCoding = x.arithmeticCoding;
      progressive      = x.progressive;
      JFIFMarker       = x.JFIFMarker;
      JFIFMajorVersion = x.JFIFMajorVersion;
      JFIFMinorVersion = x.JFIFMinorVersion;
      return *this;
   }

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
      __rsv2__         = 0;
   }
};

// ----------------------------------------------------------------------------

/*
   Base class for the JPEGReader and JPEGWriter classes.
*/
class JPEG
{
public:

   /*
      JPEG-specific exception class
   */
   class Exception : public ImageStream::Exception
   {
   public:

      Exception( const String& fn ) : ImageStream::Exception( fn )
      {
      }

      Exception( const JPEG::Exception& x ) : ImageStream::Exception( x )
      {
      }

      virtual String ExceptionClass() const
      {
         return "PCL JPEG Format Support";
      }

      virtual String ErrorMessage() const = 0;
   };

#define PCL_DECLARE_JPEG_EXCEPTION( className, errorMessage )                 \
   class className : public JPEG::Exception                                   \
   {                                                                          \
   public:                                                                    \
      className( const String& fn ) : JPEG::Exception( fn )                   \
      {                                                                       \
      }                                                                       \
      className( const className& x ) : JPEG::Exception( x )                  \
      {                                                                       \
      }                                                                       \
      virtual String ErrorMessage() const                                     \
      {                                                                       \
         return errorMessage;                                                 \
      }                                                                       \
   }

   PCL_DECLARE_JPEG_EXCEPTION( InvalidFilePath,
      "Internal error: Invalid file path" );
   PCL_DECLARE_JPEG_EXCEPTION( WriterNotInitialized,
      "Internal error: JPEGWriter instance not initialized" );
   PCL_DECLARE_JPEG_EXCEPTION( UnableToOpenFile,
      "Unable to open JPEG file" );
   PCL_DECLARE_JPEG_EXCEPTION( UnableToCreateFile,
      "Unable to create JPEG file" );
   PCL_DECLARE_JPEG_EXCEPTION( InvalidNormalizationRange,
      "Internal error: Invalid JPEG normalization range" );
   PCL_DECLARE_JPEG_EXCEPTION( InvalidReadOperation,
      "Internal error: Invalid JPEG read operation" );
   PCL_DECLARE_JPEG_EXCEPTION( InvalidWriteOperation,
      "Internal error: Invalid JPEG write operation" );
   PCL_DECLARE_JPEG_EXCEPTION( InvalidPixelSelection,
      "Internal error: Invalid pixel selection in JPEG write operation" );
   PCL_DECLARE_JPEG_EXCEPTION( InvalidChannelSelection,
      "Internal error: Invalid channel selection in JPEG write operation" );

   class IJGLibraryError : public JPEG::Exception
   {
   public:

      IJGLibraryError( const String& _path, const String& _msg ) :
      JPEG::Exception( _path ), errorMessage( _msg )
      {
      }

      IJGLibraryError( const IJGLibraryError& x ) :
      JPEG::Exception( x ), errorMessage( x.errorMessage )
      {
      }

      virtual String ErrorMessage() const
      {
         return errorMessage;
      }

      String errorMessage;
   };

   JPEG() : jpegOptions(), fileData( 0 )
   {
   }

   virtual ~JPEG()
   {
      __Close();
   }

   const JPEGImageOptions& JPEGOptions() const
   {
      return jpegOptions;
   }

   JPEGImageOptions& JPEGOptions()
   {
      return jpegOptions;
   }

protected:

   JPEGImageOptions jpegOptions;
   JPEGFileData*    fileData;

   void __Close(); // ### derived must call base

private:

   // Disable copy
   JPEG( const JPEG& ) { PCL_CHECK( 0 ) }
   void operator =( const JPEG& ) { PCL_CHECK( 0 ) }
};

// ----------------------------------------------------------------------------

/*
   JPEG image file reader
*/
class JPEGReader : public ImageReader, public JPEG
{
public:

   JPEGReader() : ImageReader(), JPEG()
   {
      // Make room for an image description
      images.Add( ImageDescription() );
   }

   virtual ~JPEGReader()
   {
      // JPEG does the job
   }

   virtual bool IsOpen() const;

   virtual void Close();

   virtual void Open( const String& filePath );

   virtual void SetIndex( int i )
   {
      if ( i != 0 )
         throw NotImplemented( *this, "Read multiple images from a JPEG file" );
   }

   virtual bool Extract( ICCProfile& icc );

   virtual void ReadImage( FImage& );
   virtual void ReadImage( DImage& );
   virtual void ReadImage( UInt8Image& );
   virtual void ReadImage( UInt16Image& );
   virtual void ReadImage( UInt32Image& );

   virtual void ReadImage( ImageVariant& v )
   {
      ImageReader::ReadImage( v );
   }

private:

   // Image streams are unique
   JPEGReader( const JPEGReader& ) { PCL_CHECK( 0 ) }
   void operator =( const JPEGReader& ) { PCL_CHECK( 0 ) }
};

// ----------------------------------------------------------------------------

/*
   JPEG image file writer
*/
class JPEGWriter : public ImageWriter, public JPEG
{
public:

   JPEGWriter() : ImageWriter(), JPEG(), icc()
   {
   }

   virtual ~JPEGWriter()
   {
      // JPEG does the job
   }

   virtual bool IsOpen() const;

   virtual void Close();

   virtual void Create( const String& filePath, int count );

   virtual void Create( const String& filePath )
   {
      Create( filePath, 1 );
   }

   virtual void Embed( const ICCProfile& _icc )
   {
      icc = _icc;
   }

   const ICCProfile* EmbeddedICCProfile() const
   {
      return &icc;
   }

   virtual void WriteImage( const FImage& );
   virtual void WriteImage( const DImage& );
   virtual void WriteImage( const UInt8Image& );
   virtual void WriteImage( const UInt16Image& );
   virtual void WriteImage( const UInt32Image& );

   virtual void WriteImage( const ImageVariant& v )
   {
      ImageWriter::WriteImage( v );
   }

   void Reset()
   {
      Close();
      icc.Clear();
   }

protected:

   // Embedded data
   ICCProfile icc;
   // ### TODO: XML metadata

private:

   // Image streams are unique
   JPEGWriter( const JPEGWriter& ) { PCL_CHECK( 0 ) }
   void operator =( const JPEGWriter& ) { PCL_CHECK( 0 ) }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_JPEG_h

// ****************************************************************************
// EOF JPEG.h - Released 2014/10/29 07:34:49 UTC
