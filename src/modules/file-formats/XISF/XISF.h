//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard XISF File Format Module Version 01.00.05.0101
// ----------------------------------------------------------------------------
// XISF.h - Released 2015/12/18 08:55:16 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard XISF PixInsight module.
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

#ifndef __PCL_XISF_h
#define __PCL_XISF_h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_FileFormatImplementation_h
#include <pcl/FileFormatImplementation.h>
#endif

#ifndef __PCL_AutoPointer_h
#include <pcl/AutoPointer.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

class XISFReaderEngine;
class XISFWriterEngine;

class PCL_CLASS Compression;
class PCL_CLASS CryptographicHash;

// ----------------------------------------------------------------------------

/*
 * Default block size in bytes for optional alignment of XISF data structures.
 */
#define XISF_BLOCK_ALIGN_SIZE          4096

/*
 * Maximum size in bytes of an inline data block.
 */
#define XISF_BLOCK_INLINE_MAX          3072 // 3072*4/3 = 4096 (base64)

/*
 * Maximum allowed width or height of an XISF image thumbnail in pixels.
 */
#define XISF_THUMBNAIL_MAX             1024

/*
 * Block checksum algorithms.
 */
#define XISF_CHECKSUM_NONE             0
#define XISF_CHECKSUM_SHA1             1
#define XISF_CHECKSUM_SHA256           2
#define XISF_CHECKSUM_SHA512           3
#define XISF_CHECKSUM_UNKNOWN          0x0F

/*
 * Default block checksum algorithm.
 */
#define XISF_CHECKSUM_DEFAULT          XISF_CHECKSUM_NONE

/*
 * Block compression algorithms.
 */
#define XISF_COMPRESSION_NONE          0
#define XISF_COMPRESSION_ZLIB          1
#define XISF_COMPRESSION_LZ4           2
#define XISF_COMPRESSION_LZ4HC         3
#define XISF_COMPRESSION_ZLIB_SH       4
#define XISF_COMPRESSION_LZ4_SH        5
#define XISF_COMPRESSION_LZ4HC_SH      6
#define XISF_COMPRESSION_UNKNOWN       0x0F

/*
 * Default compression algorithm.
 */
#define XISF_COMPRESSION_DEFAULT       XISF_COMPRESSION_NONE

/*
 * Default compression level. This means that the specific compression level
 * used will be chosen as a good compromise for the current compression method.
 */
#define XISF_COMPRESSION_LEVEL_DEFAULT 0

/*
 * Maximum codec-independent compression level.
 */
#define XISF_COMPRESSION_LEVEL_MAX     100

/*
 * The default verbosity level (0=quiet, 1=normal, 2=quite, >2=very)
 */
#define XISF_VERBOSITY_DEFAULT         1

/*
 * The default output range for floating point pixel samples.
 */
#define XISF_OUT_LOWER_BOUND_DEFAULT   0
#define XISF_OUT_UPPER_BOUND_DEFAULT   1

/*
 * Whether to serialize FITS keywords by default.
 */
#define XISF_STORE_FITS_DEFAULT        true

/*
 * Whether to ignore existing FITS keywords by default.
 */
#define XISF_IGNORE_FITS_DEFAULT       false

/*
 * Whether to import FITS keywords as XISF properties by default.
 */
#define XISF_IMPORT_FITS_DEFAULT       false

/*
 * Whether to ignore embedded data by default.
 */
#define XISF_IGNORE_EMBEDDED_DEFAULT   false

/*
 * Whether to ignore embedded properties by default.
 */
#define XISF_IGNORE_PROPERTIES_DEFAULT false

/*
 * Whether to generate XISF standard properties by default.
 */
#define XISF_AUTO_METADATA_DEFAULT     true

/*
 * Whether to silent warning conditions by default.
 */
#define XISF_NO_WARNINGS_DEFAULT       false

/*
 * Whether to treat warnings as unrecoverable errors by default.
 */
#define XISF_WARNINGS_ERRORS_DEFAULT   false

/*
 * XISF-specific file options.
 * Items marked with * are only accessible through format hints.
 */
class XISFOptions
{
public:

   bool   storeFITSKeywords   : 1;  // write FITS header keywords from/to XISF files
   bool   ignoreFITSKeywords  : 1;  // do not load FITS keywords
   bool   importFITSKeywords  : 1;  // import FITS keywords as XISF properties
   bool   ignoreEmbeddedData  : 1;  // do not load embedded data
   bool   ignoreProperties    : 1;  // do not load XISF properties
   bool   autoMetadata        : 1;  // * automatically generate a number of internal XISF properties
   bool   noWarnings          : 1;  // * suppress all warning messages
   bool   warningsAreErrors   : 1;  // * treat warnings as errors
   uint8  checksumMethod      : 4;  // algorithm for block checksum calculations
   uint8  compressionMethod   : 4;  // algorithm for compression of data blocks
   uint8  compressionLevel    : 7;  // codec-independent compression level: 0=auto, 1=fast, 100=max-compression
   uint8  verbosity           : 3;  // * 0 = quiet, > 0 = write console state messages
   uint16 blockAlignmentSize;       // block alignment size in bytes (0 = 1 = unaligned)
   uint16 maxInlineBlockSize;       // maximum size in bytes of an inline/embedded data block
   double outputLowerBound;         // * lower bound for output floating point samples (default=0)
   double outputUpperBound;         // * upper bound for output floating point samples (default=1)

   XISFOptions()
   {
      Reset();
   }

   XISFOptions( const XISFOptions& ) = default;

   XISFOptions& operator =( const XISFOptions& ) = default;

   void Reset()
   {
      storeFITSKeywords  = XISF_STORE_FITS_DEFAULT;
      ignoreFITSKeywords = XISF_IGNORE_FITS_DEFAULT;
      importFITSKeywords = XISF_IMPORT_FITS_DEFAULT;
      ignoreEmbeddedData = XISF_IGNORE_EMBEDDED_DEFAULT;
      ignoreProperties   = XISF_IGNORE_PROPERTIES_DEFAULT;
      autoMetadata       = XISF_AUTO_METADATA_DEFAULT;
      noWarnings         = XISF_NO_WARNINGS_DEFAULT;
      warningsAreErrors  = XISF_WARNINGS_ERRORS_DEFAULT;
      checksumMethod     = XISF_CHECKSUM_DEFAULT;
      compressionMethod  = XISF_COMPRESSION_DEFAULT;
      compressionLevel   = XISF_COMPRESSION_LEVEL_DEFAULT;
      verbosity          = XISF_VERBOSITY_DEFAULT;
      blockAlignmentSize = XISF_BLOCK_ALIGN_SIZE;
      maxInlineBlockSize = XISF_BLOCK_INLINE_MAX;
      outputLowerBound   = XISF_OUT_LOWER_BOUND_DEFAULT;
      outputUpperBound   = XISF_OUT_UPPER_BOUND_DEFAULT;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class XISFReader
 * \brief XISF input stream
 *
 * \sa XISFWriter
 */
class PCL_CLASS XISFReader
{
public:

   /*!
    * Constructs an %XISFReader object. The stream is created in a default
    * closed state.
    */
   XISFReader();

   /*!
    * Destroys an %XISFReader object. If the stream is open, the destructor
    * calls the Close() member function before destroying the object.
    */
   virtual ~XISFReader();

   /*!
    * Set format-specific options.
    */
   void SetOptions( const pcl::XISFOptions& );

   /*!
    * Returns a reference to the current set of format-specific options.
    */
   const pcl::XISFOptions& Options() const
   {
      return m_options;
   }

   /*!
    * Tell the reader which format hints has been specified, for inclusion as a
    * metadata property.
    */
   void SetHints( const IsoString& );

   /*!
    * Returns true if this stream is currently open for file read operations.
    * The stream is open only after a successful call to Open().
    */
   bool IsOpen() const;

   /*!
    * Opens an image file for reading at the specified \a filePath.
    */
   void Open( const String& filePath );

   /*!
    * Returns a list of warning messages generated during the XML header
    * deserialization phase.
    */
   StringList Warnings() const;

   /*!
    * If this stream is open, closes the disk file and clears all internal data
    * structures. If this stream is closed calling this member function has no
    * effect.
    */
   void Close();

   /*!
    * Returns the full path of the file being accessed through this %XISFReader
    * object, or an empty string if no file has been opened.
    */
   String FilePath() const;

   /*!
    * Returns the number of images available in this input stream.
    */
   int NumberOfImages() const;

   /*!
    * Sets the current image \a index in this input stream. \a index must be
    * in the range [0,NumberOfImages()-1].
    */
   void SelectImage( int index );

   /*!
    * Returns the index of the currently selected image, or -1 if no file has
    * been opened.
    */
   int SelectedImageIndex() const;

   /*!
    * Returns geometry and color space parameters for the current image in this
    * input stream.
    */
   const pcl::ImageInfo& ImageInfo() const;

   /*!
    * Returns format-independent options corresponding to the current image in
    * this input stream.
    */
   const pcl::ImageOptions& ImageOptions() const;

   /*!
    * Sets a new set of format-independent options for current image in this
    * input stream.
    *
    * Only options that modify the reading behavior of the stream will be taken
    * into account.
    */
   void SetImageOptions( const pcl::ImageOptions& options );

   /*!
    * Returns the identifier of the current image in this input stream.
    *
    * If no identifier is available, this function returns an empty string.
    */
   IsoString ImageId() const;

   /*!
    * Extracts a list of FITS header keywords from the current image in this
    * input stream. Returns true if the extraction operation was successful.
    */
   bool Extract( FITSKeywordArray& keywords );

   /*!
    * Extracts an ICC profile from the current image in this input stream.
    * Returns true if the extraction operation was successful.
    */
   bool Extract( ICCProfile& iccProfile );

   /*!
    * Extracts a thumbnail image from the current image in this input stream.
    * Returns true if the extraction operation was successful.
    */
   bool Extract( UInt8Image& thumbnail );

   /*!
    * Extracts an image \a property with the specified \a identifier. Returns
    * true if the extraction operation was successful.
    */
   bool Extract( Variant& property, const IsoString& identifier );

   /*
    * Returns a list of property identifiers in this XISF file.
    */
   ImagePropertyDescriptionArray Properties() const;

   /*!
    * Extracts RGB working space parameters for the current image in this input
    * stream. Returns true if the extraction operation was successful.
    */
   bool Extract( RGBColorSystem& rgbws );

   /*!
    * Extracts display function (aka STF) parameters for the current image in
    * this input stream. Returns true if the extraction operation was
    * successful.
    */
   bool Extract( DisplayFunction& df );

   /*!
    * Extracts a color filter array (CFA) description for the current image in
    * this input stream. Returns true if the extraction operation was
    * successful.
    */
   bool Extract( ColorFilterArray& cfa );

   /*!
    * Reads a 32-bit floating point image.
    */
   void ReadImage( FImage& image );

   /*!
    * Reads a 64-bit floating point image.
    */
   void ReadImage( DImage& image );

   /*!
    * Reads a 32-bit floating point complex image.
    */
   void ReadImage( ComplexImage& image );

   /*!
    * Reads a 64-bit floating point complex image.
    */
   void ReadImage( DComplexImage& image );

   /*!
    * Reads an 8-bit unsigned integer image.
    */
   void ReadImage( UInt8Image& image );

   /*!
    * Reads a 16-bit unsigned integer image.
    */
   void ReadImage( UInt16Image& image );

   /*!
    * Reads a 32-bit unsigned integer image.
    */
   void ReadImage( UInt32Image& image );

   /*!
    * Incremental/random access read of 32-bit floating point pixel samples.
    *
    * \param[out] buffer      Address of the destination pixel sample buffer.
    * \param      startRow    First pixel row to read.
    * \param      rowCount    Number of pixel rows to read.
    * \param      channel     Channel index to read.
    */
   void ReadSamples( FImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random access read of 64-bit floating point pixel samples.
    *
    * This is an overloaded member function for the DImage type; see
    * ReadSamples( Image::sample*, int, int, int ) for a full description.
    */
   void ReadSamples( DImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random access read of 32-bit complex pixel samples.
    *
    * This is an overloaded member function for the ComplexImage type; see
    * ReadSamples( Image::sample*, int, int, int ) for a full description.
    */
   void ReadSamples( ComplexImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random access read of 64-bit complex pixel samples.
    *
    * This is an overloaded member function for the DComplexImage type; see
    * ReadSamples( Image::sample*, int, int, int ) for a full description.
    */
   void ReadSamples( DComplexImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random access read of 8-bit unsigned integer pixel samples.
    *
    * This is an overloaded member function for the UInt8Image type; see
    * ReadSamples( Image::sample*, int, int, int ) for a full description.
    */
   void ReadSamples( UInt8Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random access read of 16-bit unsigned integer pixel samples.
    *
    * This is an overloaded member function for the UInt16Image type; see
    * ReadSamples( Image::sample*, int, int, int ) for a full description.
    */
   void ReadSamples( UInt16Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random access read of 32-bit unsigned integer pixel samples.
    *
    * This is an overloaded member function for the UInt32Image type; see
    * ReadSamples( Image::sample*, int, int, int ) for a full description.
    */
   void ReadSamples( UInt32Image::sample* buffer, int startRow, int rowCount, int channel );

private:

   AutoPointer<XISFReaderEngine> m_engine;
   XISFOptions                   m_options;
   IsoString                     m_hints;

   void CheckOpenStream( const char* ) const;
   void CheckClosedStream( const char* ) const;

   // XISF streams are unique objects
   XISFReader( const XISFReader& ) = delete;
   void operator =( const XISFReader& ) = delete;
};

// ----------------------------------------------------------------------------

/*!
 * \class XISFWriter
 * \brief XISF output stream
 *
 * \sa XISFReader
 */
class PCL_CLASS XISFWriter
{
public:

   /*!
    * Constructs an %XISFWriter object. The stream is created in a default
    * closed state.
    */
   XISFWriter();

   /*!
    * Destroys an %XISFWriter object. If the stream is open, the destructor
    * calls the Close() member function before destroying the object.
    */
   virtual ~XISFWriter();

   /*
    * Set format-specific options.
    */
   void SetOptions( const pcl::XISFOptions& );

   /*!
    * Returns a reference to the current set of format-specific options.
    */
   const pcl::XISFOptions& Options() const
   {
      return m_options;
   }

   /*!
    * Tell the writer which format hints has been specified, for inclusion as a
    * metadata property.
    */
   void SetHints( const IsoString& );

   /*!
    * Returns true if this stream is currently open for file write operations.
    * The stream is open only after a successful call to Create().
    */
   bool IsOpen() const;

   /*!
    * Creates a new file for writing at the specified \a path, and prepares to
    * write \a count images and data embedding operations.
    */
   void Create( const String& path, int count );

   /*!
    * If this stream is open, flushes all pending file write operations, closes
    * the disk file, and clears all internal data structures. If this stream is
    * closed calling this member function has no effect.
    */
   void Close();

   /*!
    * Returns the full path of the output file being accessed through this
    * %XISFWriter object, or an empty string if no file has been opened.
    */
   String FilePath() const;

   /*!
    * Returns format-independent options corresponding to the current image in
    * this input stream.
    */
   const pcl::ImageOptions& ImageOptions() const;

   /*!
    * Sets a new set of format-independent options for the next image written
    * by this output stream.
    *
    * Only options that modify the writing behavior of the stream will be taken
    * into account.
    */
   void SetImageOptions( const pcl::ImageOptions& options );

   /*!
    * Sets the identifier of the current image (that is, of the next image that
    * will be written) in this input stream.
    */
   void SetImageId( const IsoString& id );

   /*!
    * Embeds a set of %FITS header keywords in the current image of this image
    * writer.
    */
   void Embed( const FITSKeywordArray& keywords );

   /*!
    * Returns a copy of the list of %FITS header keywords that has been
    * embedded in the current image, or an empy list if the stream is closed or
    * no keywords have been embedded.
    */
   FITSKeywordArray EmbeddedKeywords() const;

   /*!
    * Embeds an ICC profile in the current image of this image writer.
    */
   void Embed( const ICCProfile& iccProfile );

   /*!
    * Returns a copy of the ICC profile that has been embedded in the current
    * image, or an empty ICCProfile structure if the stream is closed or no ICC
    * profile has been embedded.
    */
   ICCProfile EmbeddedICCProfile() const;

   /*!
    * Embeds an 8-bit thumbnail image in the current image of this image
    * writer.
    */
   void Embed( const UInt8Image& thumbnail );

   /*!
    * Returns a copy of the thumbnail image that has been embedded in the
    * current image, or an empty image if the stream is closed or no thumbnail
    * has been embedded.
    */
   UInt8Image EmbeddedThumbnail() const;

   /*!
    * Embeds an image property with the specified \a value and \a identifier.
    */
   void Embed( const Variant& value, const IsoString& identifier );

   /*!
    * Embeds RGB working space parameters for the current image of this image
    * writer.
    */
   void Embed( const RGBColorSystem& rgbws );

   /*!
    * Embeds display function (aka STF) parameters for the current image of
    * this image writer.
    */
   void Embed( const DisplayFunction& df );

   /*!
    * Embeds a color filter array (CFA) description for the current image of
    * this image writer.
    */
   void Embed( const ColorFilterArray& cfa );

   /*!
    * Writes a 32-bit floating point image.
    */
   void WriteImage( const Image& image );

   /*!
    * Writes a 64-bit floating point image.
    */
   void WriteImage( const DImage& image );

   /*!
    * Writes a 32-bit floating point complex image.
    */
   void WriteImage( const ComplexImage& image );

   /*!
    * Writes a 64-bit floating point complex image.
    */
   void WriteImage( const DComplexImage& image );

   /*!
    * Writes an 8-bit unsigned integer image.
    */
   void WriteImage( const UInt8Image& image );

   /*!
    * Writes a 16-bit unsigned integer image.
    */
   void WriteImage( const UInt16Image& image );

   /*!
    * Writes a 32-bit unsigned integer image.
    */
   void WriteImage( const UInt32Image& image );

   /*!
    * Creates a new image with the specified geometry and color space. The data
    * type and other image parameters are defined by current format-independent
    * options (see the Options() member function).
    *
    * This member function can be reimplemented by derived classes supporting
    * incremental write operations.
    */
   void CreateImage( const pcl::ImageInfo& info );

   /*!
    * Incremental/random access write of 32-bit floating point pixel samples.
    *
    * \param buffer     Address of the source pixel sample buffer.
    * \param startRow   First pixel row to write.
    * \param rowCount   Number of pixel rows to write.
    * \param channel    Channel index to write.
    */
   void WriteSamples( const FImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random access write of 64-bit floating point pixel samples.
    *
    * This is an overloaded member function for the DImage type; see
    * WriteSamples( const Image::sample*, int, int, int ) for a full
    * description.
    */
   void WriteSamples( const DImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random access write of 32-bit complex pixel samples.
    *
    * This is an overloaded member function for the ComplexImage type; see
    * WriteSamples( const Image::sample*, int, int, int ) for a full
    * description.
    */
   void WriteSamples( const ComplexImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random access write of 64-bit complex pixel samples.
    *
    * This is an overloaded member function for the DComplexImage type; see
    * WriteSamples( const Image::sample*, int, int, int ) for a full
    * description.
    */
   void WriteSamples( const DComplexImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random access write of 8-bit unsigned integer pixel samples.
    *
    * This is an overloaded member function for the UInt8Image type; see
    * WriteSamples( const Image::sample*, int, int, int ) for a full
    * description.
    */
   void WriteSamples( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random access write of 16-bit unsigned integer pixel samples.
    *
    * This is an overloaded member function for the UInt16Image type; see
    * WriteSamples( const Image::sample*, int, int, int ) for a full
    * description.
    */
   void WriteSamples( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random access write of 32-bit unsigned integer pixel samples.
    *
    * This is an overloaded member function for the UInt32Image type; see
    * WriteSamples( const Image::sample*, int, int, int ) for a full
    * description.
    */
   void WriteSamples( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel );

private:

   AutoPointer<XISFWriterEngine> m_engine;
   XISFOptions                   m_options;
   IsoString                     m_hints;

   void CheckOpenStream( const char* ) const;
   void CheckClosedStream( const char* ) const;

   // XISF streams are unique objects
   XISFWriter( const XISFWriter& ) = delete;
   void operator =( const XISFWriter& ) = delete;
};

// ----------------------------------------------------------------------------

/*
 * Base class of XISF stream engines.
 */
class XISFEngineBase
{
public:

   XISFEngineBase() = default;
   virtual ~XISFEngineBase() = default;

   /*
    * Identifier of a pixel sample data type. Used as XML element attribute
    * values in XISF headers.
    *
    * XISF 1.0 supports seven pixel sample formats:
    *
    * - 32-bit IEEE 754 floating point real (float)
    * - 64-bit IEEE 754 floating point real (double)
    * - 32-bit IEEE 754 floating point complex (fcomplex)
    * - 64-bit IEEE 754 floating point complex (dcomplex)
    * - 8-bit unsigned integer real (uint8)
    * - 16-bit unsigned integer real (uint16)
    * - 32-bit unsigned integer real (uint32)
    */
   static const char* SampleFormatId( int bitsPerSample, bool floatSample, bool complexSample );

   template <class P>
   static const char* SampleFormatId( const GenericImage<P>& )
   {
      return SampleFormatId( P::BitsPerSample(), P::IsFloatSample(), P::IsComplexSample() );
   }

   /*
    * Get the parameters (bit size, complex/float/integer format) of a pixel
    * sample format given its identifier. Used for deserialization from XML
    * file headers.
    */
   static bool GetSampleFormatFromId( int& bitsPerSample, bool& floatSample, bool& complexSample, const IsoString& formatId );

   /*
    * Identifier of a color space. Used as XML element attribute values in XISF
    * headers.
    *
    * XISF 1.0 supports three color spaces:
    *
    * - Grayscale
    * - RGB
    * - CIE L*a*b*
    *
    * For more information on color spaces, see pcl/RGBColorSystem.h
    */
   static const char* ColorSpaceId( int colorSpace );

   static const char* ColorSpaceId( const AbstractImage& image )
   {
      return ColorSpaceId( image.ColorSpace() );
   }

   /*
    * Get a color space given its identifier. Used for deserialization from XML
    * file headers.
    */
   static int ColorSpaceFromId( const IsoString& spaceId );

   /*
    * Identifier of a Color Filter Array (CFA) type. Used as XML element
    * attribute values in XISF headers.
    */
   static const char* CFATypeId( int cfa );

   /*
    * Get a CFA type given its identifier. Used for deserialization from XML
    * file headers.
    */
   static int CFATypeFromId( const IsoString& cfaId );

   /*
    * Identifier of a property data type. Used as XML element attribute values
    * in XISF headers.
    *
    * Currently XISF can store image properties in a variety of scalar, vector
    * and matrix types. See the code below and pcl/Variant.h for details.
    */
   static const char* PropertyTypeId( int type );

   /*
    * Get a property data type given its identifier. Used for deserialization
    * from XML file headers.
    */
   static int PropertyTypeFromId( const IsoString& typeId );

   /*
    * Identifier of a supported compression algorithm.  Used as XML element
    * attribute values in XISF headers.
    */
   static const char* CompressionMethodId( int method );

   /*
    * Get a compression algorithm given its identifier. Used for
    * deserialization from XML file headers.
    */
   static int CompressionMethodFromId( const IsoString& methodId );

   /*
    * Returns a pointer to a dynamically allocated Compression object. The
    * returned object implements the specified compression method. itemSize is
    * the length in bytes of a data element for byte shufflig.
    */
   static Compression* NewCompression( int method, int itemSize = 1 );

   /*
    * Actual compression level to be used for the specified compression method
    * and user-defined compression level.
    */
   static int CompressionLevelForMethod( int method, int level );

   /*
    * Whether a compression algorithm uses byte shuffling to preprocess
    * uncompressed data.
    */
   static bool CompressionUsesByteShuffle( int method );

   /*
    * Given a compression algorithm, returns the equivalent without byte
    * shuffling.
    */
   static int CompressionMethodNoShuffle( int method );

   /*
    * Whether a compression algorithm needs to know the size of each element in
    * a compressed block (for example, for byte shuffling).
    */
   static bool CompressionNeedsItemSize( int method );

   /*
    * Identifier of a supported checksum algorithm.  Used as XML element
    * attribute values in XISF headers.
    */
   static const char* ChecksumMethodId( int method );

   /*
    * Get a checksum algorithm given its identifier. Used for deserialization
    * from XML file headers.
    */
   static int ChecksumMethodFromId( const IsoString& methodId );

   /*
    * Returns the length in bytes of a cryptographic digest computed with the
    * specified algorithm.
    */
   static size_type ChecksumLength( int method );

   /*
    * Returns a pointer to a dynamically allocated CryptographicHash object.
    * The returned object implements the specified hashing algorithm.
    */
   static CryptographicHash* NewCryptographicHash( int method );

   /*
    * Property identifiers starting with the XISF: namespace prefix are
    * reserved by the XISF engine and cannot be defined by external client
    * applications.
    */
   static bool IsInternalPropertyId( const IsoString& id );

   /*
    * Returns a property identifier 'internalized' with the XISF: prefix.
    */
   static IsoString InternalPropertyId( const IsoString& id );

   XISFEngineBase( const XISFEngineBase& ) = delete;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_XISF_h

// ----------------------------------------------------------------------------
// EOF XISF.h - Released 2015/12/18 08:55:16 UTC
