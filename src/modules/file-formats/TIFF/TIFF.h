// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard TIFF File Format Module Version 01.00.05.0227
// ****************************************************************************
// TIFF.h - Released 2014/10/29 07:34:49 UTC
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

#ifndef __PCL_TIFF_h
#define __PCL_TIFF_h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_String_h
#include <pcl/String.h>
#endif

#ifndef __PCL_ImageStream_h
#include <pcl/ImageStream.h>
#endif

#ifndef __PCL_ICCProfile_h
#include <pcl/ICCProfile.h>
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
   Supported TIFF compression algorithms
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
   TIFF-specific image file options
*/
class TIFFImageOptions
{
public:

   typedef TIFFCompression::value_type compression_algorithm;

   compression_algorithm compression         :  8; // Compression algorithm
   bool                  planar              :  1; // Planar organization; chunky otherwise
   bool                  associatedAlpha     :  1; // Associated alpha channel
   bool                  premultipliedAlpha  :  1; // RGB/K premultiplied by alpha
   int                   __rsv__             : 21; // Reserved for future extension --must be zero

   String software;         // Software description
   String imageDescription; // Image description
   String copyright;        // Copyright notice

   TIFFImageOptions()
   {
      Reset();
   }

   TIFFImageOptions( const TIFFImageOptions& x )
   {
      (void)operator =( x );
   }

   TIFFImageOptions& operator =( const TIFFImageOptions& x )
   {
      compression        = x.compression;
      planar             = x.planar;
      associatedAlpha    = x.associatedAlpha;
      premultipliedAlpha = x.premultipliedAlpha;
      software           = x.software;
      imageDescription   = x.imageDescription;
      copyright          = x.copyright;
      return *this;
   }

   void Reset()
   {
      compression        = TIFFCompression::None;
      planar             = false;
      associatedAlpha    = true;
      premultipliedAlpha = false;
      __rsv__            = 0;
      software = PixInsightVersion::AsString() + " / " + Version::AsString();
      imageDescription.Clear();
      copyright.Clear();
   }
};

// ----------------------------------------------------------------------------

/*
   Base class for the TIFFReader and TIFFWriter classes.
*/
class TIFF
{
public:

   /*
      TIFF-specific exception class
   */
   class Exception : public ImageStream::Exception
   {
   public:

      Exception( const String& fn ) : ImageStream::Exception( fn )
      {
      }

      Exception( const TIFF::Exception& x ) : ImageStream::Exception( x )
      {
      }

      virtual String ExceptionClass() const
      {
         return "PCL TIFF Format Support";
      }

      virtual String ErrorMessage() const = 0;
   };

#define PCL_DECLARE_TIFF_EXCEPTION( className, errorMessage )                 \
   class className : public TIFF::Exception                                   \
   {                                                                          \
   public:                                                                    \
      className( const String& fn ) : TIFF::Exception( fn )                   \
      {                                                                       \
      }                                                                       \
      className( const className& x ) : TIFF::Exception( x )                  \
      {                                                                       \
      }                                                                       \
      virtual String ErrorMessage() const                                     \
      {                                                                       \
         return errorMessage;                                                 \
      }                                                                       \
   }

   PCL_DECLARE_TIFF_EXCEPTION( InvalidFilePath,
      "Internal error: Invalid file path" );
   PCL_DECLARE_TIFF_EXCEPTION( WriterNotInitialized,
      "Internal error: TIFFWriter instance not initialized" );
   PCL_DECLARE_TIFF_EXCEPTION( UnableToOpenFile,
      "Unable to open TIFF file" );
   PCL_DECLARE_TIFF_EXCEPTION( UnableToCreateFile,
      "Unable to create TIFF file" );
   PCL_DECLARE_TIFF_EXCEPTION( FileReadError,
      "Error reading TIFF file" );
   PCL_DECLARE_TIFF_EXCEPTION( FileWriteError,
      "Error writing TIFF file" );
   PCL_DECLARE_TIFF_EXCEPTION( UnsupportedColorSpace,
      "Unsupported TIFF color space -- must be either grayscale or RGB color" );
   PCL_DECLARE_TIFF_EXCEPTION( UnsupportedBitsPerSample,
      "Unsupported TIFF bits per sample -- must be 8/16/32/64 bits" );
   PCL_DECLARE_TIFF_EXCEPTION( UnsupportedSampleFormat,
      "Unsupported TIFF sample format -- must be 8/16/32-bit integers or 32/64-bit IEEE floating point" );
   PCL_DECLARE_TIFF_EXCEPTION( InvalidNormalizationRange,
      "Internal error: Invalid TIFF normalization range" );
   PCL_DECLARE_TIFF_EXCEPTION( InvalidReadOperation,
      "Internal error: Invalid TIFF read operation" );
   PCL_DECLARE_TIFF_EXCEPTION( InvalidWriteOperation,
      "Internal error: Invalid TIFF write operation" );

   TIFF() : tiffOptions(), fileData( 0 )
   {
   }

   virtual ~TIFF()
   {
      __Close();
   }

   const TIFFImageOptions& TIFFOptions() const
   {
      return tiffOptions;
   }

   TIFFImageOptions& TIFFOptions()
   {
      return tiffOptions;
   }

   static bool IsStrictMode();
   static void SetStrictMode( bool enable = true );

   static bool AreWarningsEnabled();
   static void EnableWarnings( bool enable = true );

protected:

   TIFFImageOptions tiffOptions;
   TIFFFileData*    fileData;

   void __Close(); // ### derived must call base

private:

   // Disable copy
   TIFF( const TIFF& ) { PCL_CHECK( 0 ) }
   void operator =( const TIFF& ) { PCL_CHECK( 0 ) }
};

// ----------------------------------------------------------------------------

/*
   TIFF image file reader
*/
class TIFFReader : public ImageReader, public TIFF
{
public:

   TIFFReader() : ImageReader(), TIFF()
   {
      // Make room for an image description
      images.Add( ImageDescription() );
   }

   virtual ~TIFFReader()
   {
      // TIFF does the job.
   }

   virtual bool IsOpen() const;

   virtual void Close();

   virtual void Open( const String& filePath );

   virtual void SetIndex( int i )
   {
      if ( i != 0 )
         throw NotImplemented( *this, "Read multiple images from a TIFF file" );
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
   TIFFReader( const TIFFReader& ) { PCL_CHECK( 0 ) }
   void operator =( const TIFFReader& ) { PCL_CHECK( 0 ) }
};

// ----------------------------------------------------------------------------

/*
   TIFF image file writer.
*/
class TIFFWriter : public ImageWriter, public TIFF
{
public:

   TIFFWriter() : ImageWriter(), TIFF(), icc()
   {
   }

   virtual ~TIFFWriter()
   {
      // TIFF does the job.
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

   /*
      Returns the strip size for TIFF image file transference in bytes.
   */
   static size_type StripSize();

   /*
      Sets the strip size in bytes for TIFF image file transference.

      The default strip size is 4 kilobytes. Too low or high sizes can affect
      performance adversely.
   */
   static void SetStripSize( size_type sz );

protected:

   // Embedded data.
   ICCProfile icc;
   // ### TODO: XML metadata

private:

   // Image streams are unique
   TIFFWriter( const TIFFWriter& ) { PCL_CHECK( 0 ) }
   void operator =( const TIFFWriter& ) { PCL_CHECK( 0 ) }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_TIFF_h

// ****************************************************************************
// EOF TIFF.h - Released 2014/10/29 07:34:49 UTC
