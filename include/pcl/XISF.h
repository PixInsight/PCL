//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/XISF.h - Released 2018-11-01T11:06:36Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#ifndef __PCL_XISF_h
#define __PCL_XISF_h

/// \file pcl/XISF.h

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/ColorFilterArray.h>
#include <pcl/DisplayFunction.h>
#include <pcl/FITSHeaderKeyword.h>
#include <pcl/ICCProfile.h>
#include <pcl/Image.h>
#include <pcl/ImageInfo.h>
#include <pcl/ImageOptions.h>
#include <pcl/Property.h>
#include <pcl/XML.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup xisf_support XISF Support Classes
 *
 * This group of classes can be used to implement %XISF support in PixInsight
 * modules as well as in external applications. They don't require a running
 * PixInsight core application and don't have dependencies outside the standard
 * PCL distribution on all supported platforms.
 *
 * All publicly declared classes in this group are thread-safe: they can be
 * instantiated and their member functions can be called from different threads
 * running concurrently.
 *
 * For introductory usage examples, see the utility command-line applications
 * included in the PCL distribution. For an advanced example, see the source
 * code of the %XISF format support PixInsight module.
 *
 * The latest %XISF specification document is available at:
 *
 * http://pixinsight.com/doc/docs/XISF-1.0-spec/XISF-1.0-spec.html
 *
 * For general information on %XISF, including the latest news on the format
 * and its development:
 *
 * http://pixinsight.com/xisf/
 */

// ----------------------------------------------------------------------------

class XISFReaderEngine;
class XISFWriterEngine;

class PCL_CLASS Compression;
class PCL_CLASS CryptographicHash;

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::XISFChecksum
 * \brief %XISF block checksum algorithms
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>XISFChecksum::Unknown</td> <td>Unknown or unsupported checksum algorithm.</td></tr>
 * <tr><td>XISFChecksum::None</td>    <td>No checksums.</td></tr>
 * <tr><td>XISFChecksum::SHA1</td>    <td>SHA-1 checksums.</td></tr>
 * <tr><td>XISFChecksum::SHA256</td>  <td>SHA-256 checksums.</td></tr>
 * <tr><td>XISFChecksum::SHA512</td>  <td>SHA-512 checksums.</td></tr>
 * </table>
 *
 * \ingroup xisf_support
 */
namespace XISFChecksum
{
   enum value_type
   {
      Unknown = -1,
      None = 0,
      SHA1,
      SHA256,
      SHA512,
      NumberOfSupportedAlgorithms
   };
}

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::XISFCompression
 * \brief %XISF block compression codecs
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>XISFCompression::Unknown</td>  <td>Unknown or unsupported block compression algorithm.</td></tr>
 * <tr><td>XISFCompression::None</td>     <td>No block compression.</td></tr>
 * <tr><td>XISFCompression::Zlib</td>     <td>Zlib compression.</td></tr>
 * <tr><td>XISFCompression::LZ4</td>      <td>LZ4 compression.</td></tr>
 * <tr><td>XISFCompression::LZ4HC</td>    <td>LZ4-HC compression.</td></tr>
 * <tr><td>XISFCompression::Zlib_Sh</td>  <td>Zlib compression with byte shuffling.</td></tr>
 * <tr><td>XISFCompression::LZ4_Sh</td>   <td>LZ4 compression with byte shuffling.</td></tr>
 * <tr><td>XISFCompression::LZ4HC_Sh</td> <td>Lz4-HC compression with byte shuffling.</td></tr>
 * </table>
 *
 * \ingroup xisf_support
 */
namespace XISFCompression
{
   enum value_type
   {
      Unknown = -1,
      None = 0,
      Zlib,
      LZ4,
      LZ4HC,
      Zlib_Sh,
      LZ4_Sh,
      LZ4HC_Sh,
      NumberOfSupportedCodecs
   };
}

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::XISFByteOrder
 * \brief %XISF block byte order
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>XISFByteOrder::LittleEndian</td> <td>Little-endian byte order.</td></tr>
 * <tr><td>XISFByteOrder::BigEndian</td>    <td>Big-endian byte order.</td></tr>
 * </table>
 *
 * \ingroup xisf_support
 */
namespace XISFByteOrder
{
   enum value_type
   {
      LittleEndian,
      BigEndian
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class XISF
 * \brief Utility functions, data and constants for %XISF support
 * \ingroup xisf_support
 */
class PCL_CLASS XISF
{
public:

   /*!
    * Represents a supported %XISF property type.
    */
   typedef VariantType::value_type     property_type;

   /*!
    * Represents a supported color space.
    */
   typedef ColorSpace::value_type      color_space;

   /*!
    * Represents a supported block checksum algorithm.
    */
   typedef XISFChecksum::value_type    block_checksum;

   /*!
    * Represents a supported block compression codec.
    */
   typedef XISFCompression::value_type block_compression;

   /*!
    * Represents a block byte order (endianness).
    */
   typedef XISFByteOrder::value_type   block_endianness;

   /*!
    * Default constructor. This constructor is disabled because %XISF is not an
    * instantiable class.
    */
   XISF() = delete;

   /*!
    * Copy constructor. This constructor is disabled because %XISF is not an
    * instantiable class.
    */
   XISF( const XISF& ) = delete;

   /*!
    * Copy assignment. This operator is disabled because %XISF is not an
    * instantiable class.
    */
   XISF& operator =( const XISF& ) = delete;

   /*!
    * Destructor. This destructor is disabled because %XISF is not an
    * instantiable class.
    */
   ~XISF() = delete;

   /*
    * ### N.B.: As of version 1.8.5.1309: Clang on macOS does not know how to
    * compile the following constexpr static members, giving buggy 'Undefined
    * symbols for architecture x86_64' errors on some modules...
    */
#ifndef __clang__

   /*!
    * Default block size in bytes for optional alignment of %XISF data
    * structures.
    */
   constexpr static fsize_type DefaultBlockAlignSize = 4096;

   /*!
    * Default maximum size in bytes of an inline %XISF block.
    */
   constexpr static fsize_type DefaultMaxBlockInlineSize = 3072; // 3072*4/3 = 4096 (base64)

   /*!
    * Maximum allowed width or height of an %XISF image thumbnail in pixels.
    */
   constexpr static int MaxThumbnailSize = 1024;

   /*!
    * Default block checksum algorithm.
    */
   constexpr static block_checksum DefaultChecksum = XISFChecksum::None;

   /*!
    * Default block compression codec.
    */
   constexpr static block_compression DefaultCompression = XISFCompression::None;

   /*!
    * Default compression level. This is zero by default, which means that the
    * specific compression level used will be chosen as a good compromise for
    * the selected compression codec.
    */
   constexpr static int DefaultCompressionLevel = 0;

   /*!
    * Maximum codec-independent compression level.
    */
   constexpr static int MaxCompressionLevel = 100;

   /*!
    * The default verbosity level: 0=quiet, 1=normal, 2=quite, >2=very.
    */
   constexpr static int DefaultVerbosity = 1;

   /*!
    * The default lower bound of the output floating point pixel sample range.
    */
   constexpr static double DefaultOutputLowerBound = 0.0;

   /*!
    * The default upper bound of the output floating point pixel sample range.
    */
   constexpr static double DefaultOutputUpperBound = 1.0;

   /*!
    * Whether to serialize FITS keywords by default. Has to be true because
    * some components of our standard tool set still depend heavily on FITS
    * keywords. Should be false, and it will be, eventually.
    */
   constexpr static bool DefaultStoreFITSKeywords = true;

   /*!
    * Whether to ignore existing FITS keywords by default. Has to be false
    * because some components of our standard tool set still depend heavily on
    * FITS keywords. Should be true, and it will be, eventually.
    */
   constexpr static bool DefaultIgnoreFITSKeywords = false;

   /*!
    * Whether to import FITS keywords as %XISF properties by default. Can be
    * useful for digestions of data stored in legacy formats, but is a bad idea
    * on a regular basis.
    */
   constexpr static bool DefaultImportFITSKeywords = false;

   /*!
    * Whether to ignore all embedded data by default.
    */
   constexpr static bool DefaultIgnoreEmbeddedData = false;

   /*!
    * Whether to ignore embedded image properties by default.
    */
   constexpr static bool DefaultIgnoreProperties = false;

   /*!
    * Whether to generate %XISF standard metadata properties by default.
    */
   constexpr static bool DefaultAutoMetadata = true;

   /*!
    * Whether to silent warning conditions by default.
    */
   constexpr static bool DefaultNoWarnings = false;

   /*!
    * Whether to treat warnings as unrecoverable errors by default.
    */
   constexpr static bool DefaultWarningsAreErrors = false;

   /*!
    * The namespace prefix of all %XISF reserved properties.
    */
   constexpr static const char* InternalNamespacePrefix = "XISF:";

#else

   static const fsize_type DefaultBlockAlignSize;
   static const fsize_type DefaultMaxBlockInlineSize;
   static const int MaxThumbnailSize;
   static const block_checksum DefaultChecksum;
   static const block_compression DefaultCompression;
   static const int DefaultCompressionLevel;
   static const int MaxCompressionLevel;
   static const int DefaultVerbosity;
   static const double DefaultOutputLowerBound;
   static const double DefaultOutputUpperBound;
   static const bool DefaultStoreFITSKeywords;
   static const bool DefaultIgnoreFITSKeywords;
   static const bool DefaultImportFITSKeywords;
   static const bool DefaultIgnoreEmbeddedData;
   static const bool DefaultIgnoreProperties;
   static const bool DefaultAutoMetadata;
   static const bool DefaultNoWarnings;
   static const bool DefaultWarningsAreErrors;
   static const char* InternalNamespacePrefix;

#endif // !__clang__

   /*!
    * Returns the identifier of a pixel sample data type. Used as %XML element
    * attribute values in %XISF headers.
    *
    * %XISF 1.0 supports seven pixel sample formats:
    *
    * \li 32-bit IEEE 754 floating point real (float)
    * \li 64-bit IEEE 754 floating point real (double)
    * \li 32-bit IEEE 754 floating point complex (fcomplex)
    * \li 64-bit IEEE 754 floating point complex (dcomplex)
    * \li 8-bit unsigned integer real (uint8)
    * \li 16-bit unsigned integer real (uint16)
    * \li 32-bit unsigned integer real (uint32)
    */
   static const char* SampleFormatId( int bitsPerSample, bool floatSample, bool complexSample );

   /*!
    * Returns the identifier of the pixel sample data type corresponding to the
    * specified \a image.
    */
   template <class P>
   static const char* SampleFormatId( const GenericImage<P>& image )
   {
      return SampleFormatId( P::BitsPerSample(), P::IsFloatSample(), P::IsComplexSample() );
   }

   /*!
    * Provides the parameters (bit size, complex/float/integer format) of a
    * pixel sample format, given its identifier. Used for deserialization from
    * %XML file headers.
    */
   static bool GetSampleFormatFromId( int& bitsPerSample, bool& floatSample, bool& complexSample, const String& id );

   /*!
    * Returns the identifier of a color space. Used as %XML element attribute
    * values in %XISF headers.
    *
    * XISF 1.0 supports three color spaces:
    *
    * \li Grayscale
    * \li RGB
    * \li CIE L*a*b*
    *
    * For more information on color spaces, see RGBColorSystem.
    */
   static const char* ColorSpaceId( color_space colorSpace );

   /*!
    * Returns the identifier of the color space corresponding to the specified
    * \a image.
    */
   static const char* ColorSpaceId( const AbstractImage& image )
   {
      return ColorSpaceId( image.ColorSpace() );
   }

   /*!
    * Returns a color space, given its identifier. Used for deserialization
    * from %XML file headers.
    */
   static color_space ColorSpaceFromId( const String& id );

   /*!
    * Returns the identifier of a property data \a type. Used as %XML element
    * attribute values in %XISF headers.
    *
    * %XISF can store image properties in a variety of scalar, vector and
    * matrix types. See the code below and pcl/Variant.h for details.
    */
   static const char* PropertyTypeId( property_type type );

   /*!
    * Get a property data type, given its identifier. Used for deserialization
    * from %XML file headers.
    */
   static property_type PropertyTypeFromId( const String& id );

   /*!
    * Returns the identifier of a supported compression \a codec. Used as %XML
    * element attribute values in %XISF headers.
    */
   static const char* CompressionCodecId( block_compression codec );

   /*!
    * Returns a compression codec, given its identifier. Used for
    * deserialization from %XML file headers.
    */
   static block_compression CompressionCodecFromId( const String& id );

   /*!
    * Returns a pointer to a dynamically allocated Compression object. The
    * returned object implements the specified compression \a codec.
    * \a itemSize is the length in bytes of a data element, for byte shufflig.
    */
   static Compression* NewCompression( block_compression codec, size_type itemSize = 1 );

   /*!
    * Returns the codec-specific compression level to be used for the specified
    * compression \a codec and abstract compression \a level.
    */
   static int CompressionLevelForMethod( block_compression codec, int level );

   /*!
    * Returns true iff the specified compression \a codec uses byte shuffling
    * to preprocess uncompressed data.
    */
   static bool CompressionUsesByteShuffle( block_compression codec );

   /*!
    * Given a compression \a codec, returns the equivalent codec without byte
    * shuffling.
    */
   static block_compression CompressionCodecNoShuffle( block_compression codec );

   /*!
    * Returns true iff a compression \a codec needs to know the size of each
    * element in a compressed block (for example, for byte shuffling).
    */
   static bool CompressionNeedsItemSize( block_compression codec );

   /*!
    * Returns the identifier of a supported checksum \a algorithm. Used as %XML
    * element attribute values in %XISF headers.
    */
   static const char* ChecksumAlgorithmId( block_checksum algorithm );

   /*!
    * Returns a checksum algorithm, given its identifier. Used for
    * deserialization from %XML file headers.
    */
   static block_checksum ChecksumAlgorithmFromId( const String& id );

   /*!
    * Returns the length in bytes of a cryptographic digest computed with the
    * specified \a algorithm.
    */
   static size_type ChecksumLength( block_checksum algorithm );

   /*!
    * Returns a pointer to a dynamically allocated CryptographicHash object.
    * The returned object implements the specified hashing \a algorithm.
    */
   static CryptographicHash* NewCryptographicHash( block_checksum algorithm );

   /*!
    * Returns true iff the specified string \a id is a valid XISF property
    * identifier.
    *
    * A valid XISF property identifier is a sequence:
    *
    * <tt>t1[:t2[:...:tn]]</tt>
    *
    * where each \a ti satisfies the following conditions:
    *
    * \li It is not an empty string.
    *
    * \li Its first character is either an alphabetic character or an
    * underscore character.
    *
    * \li Its second and successive characters, if they exist, are all of them
    * either alphabetic characters, decimal digits, or underscores.
    */
   static bool IsValidPropertyId( const IsoString& id )
   {
      return Property::IsValidIdentifier( id );
   }

   static bool IsValidPropertyId( const IsoString::ustring_base& id )
   {
      return IsValidPropertyId( IsoString( id ) );
   }

   /*!
    * Returns true iff the specified string \a id is the identifier of a
    * reserved %XISF property.
    *
    * Property identifiers starting with the XISF: namespace prefix are
    * reserved by the %XISF format and cannot be defined by external client
    * applications.
    */
   static bool IsInternalPropertyId( const IsoString& id )
   {
      return id.StartsWith( InternalNamespacePrefix );
   }

   static bool IsInternalPropertyId( const IsoString::ustring_base& id )
   {
      return IsInternalPropertyId( IsoString( id ) );
   }

   /*!
    * Returns a property identifier 'internalized' with the XISF: prefix.
    */
   static IsoString InternalPropertyId( const IsoString& id )
   {
      if ( !IsInternalPropertyId( id ) )
         return InternalNamespacePrefix + id;
      return id;
   }

   static IsoString InternalPropertyId( const IsoString::ustring_base& id )
   {
      return InternalPropertyId( IsoString( id ) );
   }

   /*!
    * Ensures that the internal pixel traits lookup tables have been properly
    * allocated and initialized. This is a thread-safe function used internally
    * by the XISFReader and XISFWriter classes to accelerate conversions among
    * all supported pixel sample data types. This allows external applications
    * to perform image I/O operations without a running PixInsight core
    * application.
    *
    * \note This is an internal routine. You normally should not need to call
    * it, unless you are hacking the current %XISF implementation.
    */
   static void EnsurePTLUTInitialized();
};

// ----------------------------------------------------------------------------

/*!
 * \class XISFOptions
 * \brief %XISF-specific file options
 *
 * This structure stores a collection of settings and options that control the
 * way %XISF units are loaded and generated by this implementation.
 *
 * %XISFOptions, along with ImageOptions, allow client modules and applications
 * to manipulate properties and images serialized in %XISF units with a high
 * degree of flexibility, tailoring them to the needs of each application.
 *
 * \ingroup xisf_support
 */
class PCL_CLASS XISFOptions
{
public:

   bool                    storeFITSKeywords  : 1;  //!< Include FITS header keywords in output %XISF files.
   bool                    ignoreFITSKeywords : 1;  //!< Do not load FITS keywords from input %XISF files.
   bool                    importFITSKeywords : 1;  //!< Import FITS keywords as %XISF properties.
   bool                    ignoreEmbeddedData : 1;  //!< Do not load existing embedded data (such as ICC profiles for example).
   bool                    ignoreProperties   : 1;  //!< Do not load existing %XISF properties.
   bool                    autoMetadata       : 1;  //!< Automatically generate a number of reserved %XISF properties.
   bool                    noWarnings         : 1;  //!< Suppress all warning and diagnostics messages.
   bool                    warningsAreErrors  : 1;  //!< Treat warnings as fatal errors.
   XISF::block_checksum    checksumAlgorithm  : 4;  //!< The algorithm used for block checksum calculations.
   XISF::block_compression compressionCodec   : 4;  //!< The codec used for compression of %XISF blocks.
   uint8                   compressionLevel   : 7;  //!< Codec-independent compression level: 0 = auto, 1 = fast, 100 = maximum compression.
   uint8                   verbosity          : 3;  //!< Verbosity level: 0 = quiet, > 0 = write console state messages.
   uint16                  blockAlignmentSize;      //!< Block alignment size in bytes (0 = 1 = unaligned).
   uint16                  maxInlineBlockSize;      //!< Maximum size in bytes of an inline/embedded block.
   double                  outputLowerBound;        //!< Lower bound for output floating point pixel samples (=0.0 by default).
   double                  outputUpperBound;        //!< Upper bound for output floating point pixel samples (=1.0 by default).

   /*!
    * Constructs a default set of %XISF format-specific options.
    */
   XISFOptions()
   {
      Reset();
   }

   /*!
    * Copy constructor.
    */
   XISFOptions( const XISFOptions& ) = default;

   /*!
    * Copy-assignment operator. Returns a reference to this object.
    */
   XISFOptions& operator =( const XISFOptions& ) = default;

   /*!
    * Resets this object to a default set of format-specific options.
    */
   void Reset()
   {
      storeFITSKeywords  = XISF::DefaultStoreFITSKeywords;
      ignoreFITSKeywords = XISF::DefaultIgnoreFITSKeywords;
      importFITSKeywords = XISF::DefaultImportFITSKeywords;
      ignoreEmbeddedData = XISF::DefaultIgnoreEmbeddedData;
      ignoreProperties   = XISF::DefaultIgnoreProperties;
      autoMetadata       = XISF::DefaultAutoMetadata;
      noWarnings         = XISF::DefaultNoWarnings;
      warningsAreErrors  = XISF::DefaultWarningsAreErrors;
      checksumAlgorithm  = XISF::DefaultChecksum;
      compressionCodec   = XISF::DefaultCompression;
      compressionLevel   = XISF::DefaultCompressionLevel;
      verbosity          = XISF::DefaultVerbosity;
      blockAlignmentSize = XISF::DefaultBlockAlignSize;
      maxInlineBlockSize = XISF::DefaultMaxBlockInlineSize;
      outputLowerBound   = XISF::DefaultOutputLowerBound;
      outputUpperBound   = XISF::DefaultOutputUpperBound;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \struct XISFFileSignature
 * \brief %XISF monolithic file signature
 *
 * All %XISF version 1.0 monolithic files begin with the following sequence:
 *
 * XISF0100&lt;header-length&gt;&lt;reserved&gt;
 *
 * where 'XISF0100' is the 'magic marker' identifying the format, and
 * &lt;header-length&gt; is the size in bytes of the XML file header encoded as
 * a 32-bit unsigned integer with little-endian byte order. &lt;reserved&gt; is
 * a 32-bit integer reserved for future use; it must be zero. After the file
 * signature sequence comes the %XML header and all attached blocks.
 *
 * \ingroup xisf_support
 */
struct PCL_CLASS XISFFileSignature
{
   uint8  magic[ 8 ]   = { 'X', 'I', 'S', 'F', '0', '1', '0', '0' };
   uint32 headerLength = 0;  // length in bytes of the XML file header
   uint32 reserved     = 0;  // reserved - must be zero

   /*!
    * Default constructor. Yields an invalid %XISF signature that cannot be
    * used without explicit initialization.
    */
   XISFFileSignature() = default;

   /*!
    * Copy constructor.
    */
   XISFFileSignature( const XISFFileSignature& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   XISFFileSignature& operator =( const XISFFileSignature& ) = default;

   /*!
    * Constructs a monolithic %XISF file signature initialized for the
    * specified header \a length.
    */
   XISFFileSignature( uint32 length ) : headerLength( length )
   {
   }

   /*!
    * Validates this monolithic %XISF file signature. Throws an Error exception
    * if it is not valid.
    */
   void Validate() const;
};

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::XISFMessageType
 * \brief %XISF log message types
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>XISFMessageType::Informative</td>      <td>A regular informative message.</td></tr>
 * <tr><td>XISFMessageType::Note</td>             <td>A note or remark message.</td></tr>
 * <tr><td>XISFMessageType::Warning</td>          <td>A warning message.</td></tr>
 * <tr><td>XISFMessageType::RecoverableError</td> <td>A recoverable error message.</td></tr>
 * </table>
 *
 * \ingroup xisf_support
 * \sa XISFLogHandler
 */
namespace XISFMessageType
{
   enum value_type
   {
      Informative,
      Note,
      Warning,
      RecoverableError
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class XISFLogHandler
 * \brief %XISF stream message logger
 *
 * %XISFLogHandler is a simple handler object which logs messages generated
 * by the XISFReader and XISFWriter classes during %XISF file transactions.
 * These messages can be informative, warnings, and recoverable error messages.
 *
 * Note that unrecoverable errors are not logged and hence not sent to this
 * class (or derived); they are always thrown as Exception instances, which the
 * caller must catch and manage appropriately.
 *
 * \ingroup xisf_support
 * \sa XISFReader, XISFWriter
 */
class PCL_CLASS XISFLogHandler
{
public:

   /*!
    * Represents a log message type. Supported values are enumerated in the
    * XISFMessageType namespace.
    */
   typedef XISFMessageType::value_type    message_type;

   /*!
    * Default constructor.
    */
   XISFLogHandler() = default;

   /*!
    * Virtual destructor.
    */
   virtual ~XISFLogHandler()
   {
   }

   /*!
    * Initializes this log handler object. This function will be called by
    * internal XISFReader and XISFWriter engines at the beginning of an %XISF
    * file transaction.
    *
    * \param filePath   Full path to the %XISF file about to be loaded or
    *                   generated.
    *
    * \param writing    Will be true at the beginning of a file generation
    *                   process; false at the beginning of a file loading
    *                   process.
    *
    * The default implementation does nothing. This virtual function can be
    * reimplemented in a derived class requiring special initialization at the
    * beginning of a file loading or generation process.
    */
   virtual void Init( const String& filePath, bool writing )
   {
   }

   /*!
    * Handles a log message. This function will be called by internal
    * XISFReader and XISFWriter engines at different points during %XISF file
    * transactions.
    *
    * \param text       The log text message.
    *
    * \param type       The type of this log message. See the XISFMessageType
    *                   namespace for possible values.
    *
    * The default implementation does nothing. This virtual function should be
    * reimplemented in a derived class. Typically, the received \a text
    * messages are sent directly to the platform console; for example, the
    * standard %XISF format support module does just that. Other options are
    * writing messages to text files, or sending them to other processes.
    */
   virtual void Log( const String& text, message_type type )
   {
   }

   /*!
    * Closes this log handler object. This function will be called by internal
    * XISFReader and XISFWriter engines at the end of an %XISF file
    * transaction.
    *
    * The default implementation does nothing. This virtual function can be
    * reimplemented in a derived class requiring special cleanup or
    * initialization at the end of a file loading or generation process.
    */
   virtual void Close()
   {
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class XISFReader
 * \brief %XISF input file stream
 *
 * %XISFReader allows you to read properties, images and metadata serialized in
 * monolithic %XISF units stored as local files. This class can be used without
 * a running PixInsight core application to implement %XISF support in external
 * applications.
 *
 * \ingroup xisf_support
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
    * calls the Close() member function before destroying this object.
    */
   virtual ~XISFReader() noexcept( false );

   /*
    * Copy constructor. This constructor is disabled because %XISF streams are
    * unique objects.
    */
   XISFReader( const XISFReader& ) = delete;

   /*
    * Copy assignment. This operator is disabled because %XISF streams are
    * unique objects.
    */
   XISFReader& operator =( const XISFReader& ) = delete;

   /*!
    * Define a set of format-specific \a options.
    */
   void SetOptions( const pcl::XISFOptions& options );

   /*!
    * Returns the current set of format-specific options.
    */
   pcl::XISFOptions Options() const
   {
      return m_options;
   }

   /*!
    * Tell the reader which format hints have been specified, for inclusion as
    * a reserved metadata property.
    */
   void SetHints( const IsoString& );

   /*!
    * Associates a log \a handler with this %XISF input stream.
    *
    * The specified \a handler object must be allocated dynamically by the
    * caller. It will be owned by this %XISFReader instance, which will destroy
    * and deallocate it when appropriate. To force destruction of the
    * associated log handler (if any), call this function with a null pointer
    * as argument.
    */
   void SetLogHandler( XISFLogHandler* handler );

   /*!
    * Returns true iff this stream is currently open for file read operations.
    * The stream is open only after a successful call to Open().
    */
   bool IsOpen() const;

   /*!
    * Opens an existing file for reading at the specified file \a path.
    */
   void Open( const String& path );

   /*!
    * If this stream is open, closes the disk file and clears all internal data
    * structures. If this stream is closed, calling this member function has no
    * effect.
    */
   void Close();

   /*!
    * Returns the full path of the file being accessed through this %XISFReader
    * object, or an empty string if no file has been opened.
    */
   String FilePath() const;

   /*!
    * Returns the number of images available in this %XISF input stream.
    */
   int NumberOfImages() const;

   /*!
    * Sets the current image \a index in this input stream. \a index must be
    * in the range [0,NumberOfImages()-1].
    */
   void SelectImage( int index );

   /*!
    * Returns the index of the currently selected image, or -1 if either no
    * file has been opened, or if the file does not contain any images.
    */
   int SelectedImageIndex() const;

   /*!
    * Returns geometry and color space parameters for the current image in this
    * input stream.
    */
   pcl::ImageInfo ImageInfo() const;

   /*!
    * Returns format-independent options corresponding to the current image in
    * this input stream.
    */
   pcl::ImageOptions ImageOptions() const;

   /*!
    * Defines a new set of format-independent options for the current image in
    * this input stream.
    *
    * Only options that modify the reading behavior of the stream will be taken
    * into account; the rest will be ignored, irrespective of their values.
    */
   void SetImageOptions( const pcl::ImageOptions& options );

   /*!
    * Returns the identifier of the current image in this input stream.
    *
    * If no identifier is available for the current image, if no file has been
    * opened, or if the current file contains no images, this function returns
    * an empty string.
    */
   IsoString ImageId() const;

   /*!
    * Extracts a list of FITS header keywords from the current image in this
    * input stream, and returns the extracted keywords as a dynamic array.
    *
    * If no FITS keywords are available for the current image, if no file has
    * been opened, or if the current file contains no images, this function
    * returns an empty array.
    */
   FITSKeywordArray ReadFITSKeywords();

   /*!
    * Extracts an ICC profile from the current image in this input stream, and
    * returns the extracted ICC profile structure.
    *
    * If no ICC profile is available for the current image, if no file has been
    * opened, or if the current file contains no images, this function returns
    * an empty ICCProfile structure.
    */
   ICCProfile ReadICCProfile();

   /*!
    * Extracts a thumbnail image from the current image in this input stream,
    * and returns the extracted thumbnail as an 8-bit unsigned integer image.
    *
    * If no thumbnail is available for the current image, if no file has been
    * opened, or if the current file contains no images, this function returns
    * an empty image.
    */
   UInt8Image ReadThumbnail();

   /*!
    * Extracts RGB working space parameters from the current image in this
    * input stream, and returns them as a RGBColorSystem object.
    *
    * If no RGB working space has been defined for the current image, if no
    * file has been opened, or if the current file contains no images, this
    * function returns a duplicate of the sRGB color space, since this is the
    * default %XISF color space.
    */
   RGBColorSystem ReadRGBWorkingSpace();

   /*!
    * Extracts display function parameters from the current image in this input
    * stream, and returns them as a pcl::DisplayFunction object.
    *
    * If no display function has been defined for the current image, if no file
    * has been opened, or if the current file contains no images, this function
    * returns an identity display function.
    */
   DisplayFunction ReadDisplayFunction();

   /*!
    * Extracts a color filter array (CFA) description from the current image in
    * this input stream, and returns it as a pcl::ColorFilterArray object.
    *
    * If no CFA has been defined for the current image, if no file has been
    * opened, or if the current file contains no images, this function returns
    * an invalid ColorFilterArray instance.
    */
   ColorFilterArray ReadColorFilterArray();

   /*!
    * Returns a list of property identifiers and data types describing the set
    * of properties associated with the current image in this %XISF unit. The
    * returned list can be empty if no image is available, or if the current
    * image has no properties.
    */
   PropertyDescriptionArray ImageProperties() const;

   /*!
    * Extracts a \a property from the current image with the specified
    * \a identifier, and returns its value as a Variant object.
    *
    * If no property with the specified \a identifier is available for the
    * current image, if no file has been opened, or if the current file
    * contains no images, this function returns an invalid %Variant object.
    */
   Variant ReadImageProperty( const IsoString& identifier );

   /*!
    * Returns an array with all properties extracted from the current image.
    *
    * If no property is available for the current image, if no file has been
    * opened, or if the current file contains no images, this function returns
    * an empty array.
    */
   PropertyArray ReadImageProperties();

   /*!
    * Returns a list of property identifiers and data types describing the set
    * of properties associated with the %XISF unit. The returned list can be
    * empty if no file has been opened, or if the %XISF unit has no properties.
    */
   PropertyDescriptionArray Properties() const;

   /*!
    * Extracts a \a property associated with the %XISF unit with the specified
    * \a identifier, and returns its value as a Variant object.
    *
    * If no property with the specified \a identifier is available for this
    * %XISF unit, or if no file has been opened, this function returns an
    * invalid %Variant object.
    */
   Variant ReadProperty( const IsoString& identifier );

   /*!
    * Returns an array with all properties associated with the %XISF unit.
    *
    * If no property is available for the %XISF unit, or if no file has been
    * opened, this function returns an empty array.
    */
   PropertyArray ReadProperties();

   /*!
    * Reads a 32-bit floating point image from this input stream.
    */
   void ReadImage( FImage& image );

   /*!
    * Reads a 64-bit floating point image from this input stream.
    */
   void ReadImage( DImage& image );

   /*!
    * Reads a 32-bit floating point complex image from this input stream.
    */
   void ReadImage( ComplexImage& image );

   /*!
    * Reads a 64-bit floating point complex image from this input stream.
    */
   void ReadImage( DComplexImage& image );

   /*!
    * Reads an 8-bit unsigned integer image from this input stream.
    */
   void ReadImage( UInt8Image& image );

   /*!
    * Reads a 16-bit unsigned integer image from this input stream.
    */
   void ReadImage( UInt16Image& image );

   /*!
    * Reads a 32-bit unsigned integer image from this input stream.
    */
   void ReadImage( UInt32Image& image );

   /*!
    * Incremental random access read of 32-bit floating point pixel samples.
    *
    * \param[out] buffer      Address of the destination pixel sample buffer.
    * \param      startRow    First pixel row to read.
    * \param      rowCount    Number of pixel rows to read.
    * \param      channel     Channel index to read.
    */
   void ReadSamples( FImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental random access read of 64-bit floating point pixel samples.
    *
    * This is an overloaded member function for the DImage type; see
    * ReadSamples( Image::sample*, int, int, int ) for a full description.
    */
   void ReadSamples( DImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental random access read of 32-bit complex pixel samples.
    *
    * This is an overloaded member function for the ComplexImage type; see
    * ReadSamples( Image::sample*, int, int, int ) for a full description.
    */
   void ReadSamples( ComplexImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental random access read of 64-bit complex pixel samples.
    *
    * This is an overloaded member function for the DComplexImage type; see
    * ReadSamples( Image::sample*, int, int, int ) for a full description.
    */
   void ReadSamples( DComplexImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental random access read of 8-bit unsigned integer pixel samples.
    *
    * This is an overloaded member function for the UInt8Image type; see
    * ReadSamples( Image::sample*, int, int, int ) for a full description.
    */
   void ReadSamples( UInt8Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental random access read of 16-bit unsigned integer pixel samples.
    *
    * This is an overloaded member function for the UInt16Image type; see
    * ReadSamples( Image::sample*, int, int, int ) for a full description.
    */
   void ReadSamples( UInt16Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental random access read of 32-bit unsigned integer pixel samples.
    *
    * This is an overloaded member function for the UInt32Image type; see
    * ReadSamples( Image::sample*, int, int, int ) for a full description.
    */
   void ReadSamples( UInt32Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Extract and parse the %XML header document of an existing monolithic
    * %XISF file at the specified \a path, and return it as a dynamically
    * allocated instance of XMLDocument.
    *
    * Ownership of the returned object is transferred to the caller, who is
    * responsible for destroying it when appropriate.
    *
    * This function will throw an Error exception if either the file is not a
    * monolithic %XISF unit, or the header is not a well-formed %XML document.
    * The header is not verified for validity or correctness as per the %XISF
    * format specification; only well-formedness of the XML document is
    * checked.
    */
   static XMLDocument* ExtractHeader( const String& path, XMLParserOptions options = XMLParserOptions() );

private:

   AutoPointer<XISFReaderEngine> m_engine;
   AutoPointer<XISFLogHandler>   m_logHandler;
   XISFOptions                   m_options;
   IsoString                     m_hints;

   void CheckOpenStream( const char* ) const;
   void CheckClosedStream( const char* ) const;
};

// ----------------------------------------------------------------------------

/*!
 * \class XISFWriter
 * \brief %XISF output file stream
 *
 * %XISFWriter allows you to write properties, images and metadata serialized
 * in monolithic %XISF units stored as local files. This class can be used
 * without a running PixInsight core application to implement %XISF support in
 * external applications.
 *
 * \ingroup xisf_support
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
   virtual ~XISFWriter() noexcept( false );

   /*
    * Copy constructor. This constructor is disabled because %XISF streams are
    * unique objects.
    */
   XISFWriter( const XISFWriter& ) = delete;

   /*
    * Copy assignment. This operator is disabled because %XISF streams are
    * unique objects.
    */
   XISFWriter& operator =( const XISFWriter& ) = delete;

   /*!
    * Define a set of format-specific \a options.
    */
   void SetOptions( const pcl::XISFOptions& options );

   /*!
    * Returns the current set of format-specific options.
    */
   pcl::XISFOptions Options() const
   {
      return m_options;
   }

   /*!
    * Tell the writer which format hints have been specified, for inclusion as
    * a reserved metadata property.
    */
   void SetHints( const IsoString& );

   /*!
    * Associates a log \a handler with this %XISF output stream.
    *
    * The specified \a handler object must be allocated dynamically by the
    * caller. It will be owned by this %XISFReader instance, which will destroy
    * and deallocate it when appropriate. To force destruction of the
    * associated log handler (if any), call this function with a null pointer
    * as argument.
    */
   void SetLogHandler( XISFLogHandler* handler );

   /*!
    * Sets the value of the %XISF:CreatorApplication reserved metadata property
    * to be included in newly created %XISF units.
    *
    * The %XISF:CreatorApplication property is mandatory. If this
    * implementation is being used in an installed PixInsight module (such as
    * the standard %XISF format support module for example), this property will
    * be generated automatically by retrieving version information from the
    * running PixInsight core application, and the value set with this function
    * will be ignored. If this implementation is used in an external
    * application, calling this function with the appropriate application name
    * (including version information) is \e mandatory. See the %XISF
    * specification for more information on reserved metadata properties.
    */
   void SetCreatorApplication( const String& appName );

   /*!
    * Sets the value of the %XISF:CreatorModule reserved metadata property to
    * be included in newly created %XISF units.
    *
    * The %XISF:CreatorModule property is optional. If this implementation is
    * being used in an installed PixInsight module (such as the standard %XISF
    * format support module for example), this property will be generated
    * automatically by retrieving the name and version of the running module,
    * and the value set with this function will be ignored. If this
    * implementation is being used in an external application, the value set by
    * calling this function will be used. If no CreatorModule is defined, the
    * corresponding metadata property will not be generated. See the %XISF
    * specification for more information on reserved metadata properties.
    */
   void SetCreatorModule( const String& modName );

   /*!
    * Returns true iff this stream is currently open for file write operations.
    * The stream is open only after a successful call to Create().
    */
   bool IsOpen() const;

   /*!
    * Creates a new file for writing at the specified \a path, and prepares to
    * write \a count images and their embedded data.
    */
   void Create( const String& path, int count );

   /*!
    * If this stream is open, flushes all pending file write operations, closes
    * the disk file, and clears all internal data structures. If this stream is
    * closed, calling this member function has no effect.
    */
   void Close();

   /*!
    * Returns the full path of the output file being accessed through this
    * %XISFWriter object, or an empty string if no file has been created.
    */
   String FilePath() const;

   /*!
    * Defines a new set of format-independent options for the next image
    * written by this output stream.
    *
    * Only options that modify the writing behavior of the stream will be taken
    * into account; the rest will be ignored, irrespective of their values.
    */
   void SetImageOptions( const ImageOptions& options );

   /*!
    * Sets the identifier of the current image (that is, of the next image that
    * will be written) in this input stream.
    */
   void SetImageId( const IsoString& id );

   /*!
    * Embeds a set of %FITS header \a keywords in the current image of this
    * output stream.
    */
   void WriteFITSKeywords( const FITSKeywordArray& keywords );

   /*!
    * Returns the list of %FITS header keywords embedded in the current image.
    * This is necessary because the PixInsight core application has an option
    * to reload the list of keywords actually embedded after writing a new
    * image. Client applications without this requirement can safely ignore
    * this member function.
    */
   const FITSKeywordArray& FITSKeywords() const;

   /*!
    * Embeds an ICC \a profile in the current image of this output stream.
    */
   void WriteICCProfile( const ICCProfile& profile );

   /*!
    * Embeds an 8-bit \a thumbnail image in the current image of this output
    * stream.
    */
   void WriteThumbnail( const UInt8Image& thumbnail );

   /*!
    * Defines RGB working space parameters for the current image of this output
    * stream.
    */
   void WriteRGBWorkingSpace( const RGBColorSystem& rgbws );

   /*!
    * Defines display function parameters for the current image of this output
    * stream.
    */
   void WriteDisplayFunction( const DisplayFunction& df );

   /*!
    * Embeds a color filter array (CFA) description for the current image of
    * this output stream.
    */
   void WriteColorFilterArray( const ColorFilterArray& cfa );

   /*!
    * Associates a property with the specified \a identifier and \a value with
    * the current image in this output stream.
    */
   void WriteImageProperty( const IsoString& identifier, const Variant& value );

   /*!
    * Associates a set of \a properties with the current image in this output
    * stream.
    */
   void WriteImageProperties( const PropertyArray& properties );

   /*!
    * Removes a property with the specified \a identifier from the list of
    * properties associated with the current image. If no property with the
    * specified \a identifier has been defined for the current image, this
    * member function takes no action.
    */
   void RemoveImageProperty( const IsoString& identifier );

   /*!
    * Associates a property with the specified \a identifier and \a value with
    * the %XISF unit being generated by this output stream.
    */
   void WriteProperty( const IsoString& identifier, const Variant& value );

   /*!
    * Associates a set of \a properties with the %XISF unit being generated by
    * this output stream.
    */
   void WriteProperties( const PropertyArray& properties );

   /*!
    * Removes a property with the specified \a identifier from the list of
    * properties associated with the %XISF unit. If no property with the
    * specified \a identifier has been defined previously, this member function
    * takes no action.
    */
   void RemoveProperty( const IsoString& identifier );

   /*!
    * Writes a 32-bit floating point image to this output stream.
    */
   void WriteImage( const Image& image );

   /*!
    * Writes a 64-bit floating point image to this output stream.
    */
   void WriteImage( const DImage& image );

   /*!
    * Writes a 32-bit floating point complex image to this output stream.
    */
   void WriteImage( const ComplexImage& image );

   /*!
    * Writes a 64-bit floating point complex image to this output stream.
    */
   void WriteImage( const DComplexImage& image );

   /*!
    * Writes an 8-bit unsigned integer image to this output stream.
    */
   void WriteImage( const UInt8Image& image );

   /*!
    * Writes a 16-bit unsigned integer image to this output stream.
    */
   void WriteImage( const UInt16Image& image );

   /*!
    * Writes a 32-bit unsigned integer image to this output stream.
    */
   void WriteImage( const UInt32Image& image );

   /*!
    * Writes the specified \a image to this output stream.
    */
   void WriteImage( const ImageVariant& image );

   /*!
    * Creates a new image with the specified geometry and color space, and
    * prepare for sequential/random write access.
    *
    * The data type and other image parameters are defined by the current set
    * of format-independent options (see SetImageOptions()).
    */
   void CreateImage( const ImageInfo& info );

   /*!
    * Incremental/random write of 32-bit floating point pixel samples.
    *
    * \param buffer     Address of the source pixel sample buffer.
    * \param startRow   First pixel row to write.
    * \param rowCount   Number of pixel rows to write.
    * \param channel    Channel index to write.
    */
   void WriteSamples( const FImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random write of 64-bit floating point pixel samples.
    *
    * This is an overloaded member function for the DImage type; see
    * WriteSamples( const Image::sample*, int, int, int ) for a full
    * description.
    */
   void WriteSamples( const DImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random write of 32-bit complex pixel samples.
    *
    * This is an overloaded member function for the ComplexImage type; see
    * WriteSamples( const Image::sample*, int, int, int ) for a full
    * description.
    */
   void WriteSamples( const ComplexImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random write of 64-bit complex pixel samples.
    *
    * This is an overloaded member function for the DComplexImage type; see
    * WriteSamples( const Image::sample*, int, int, int ) for a full
    * description.
    */
   void WriteSamples( const DComplexImage::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random write of 8-bit unsigned integer pixel samples.
    *
    * This is an overloaded member function for the UInt8Image type; see
    * WriteSamples( const Image::sample*, int, int, int ) for a full
    * description.
    */
   void WriteSamples( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random write of 16-bit unsigned integer pixel samples.
    *
    * This is an overloaded member function for the UInt16Image type; see
    * WriteSamples( const Image::sample*, int, int, int ) for a full
    * description.
    */
   void WriteSamples( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Incremental/random write of 32-bit unsigned integer pixel samples.
    *
    * This is an overloaded member function for the UInt32Image type; see
    * WriteSamples( const Image::sample*, int, int, int ) for a full
    * description.
    */
   void WriteSamples( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel );

   /*!
    * Complete the image created by a previous call to CreateImage() and
    * generate it in the output stream.
    */
   void CloseImage();

private:

   AutoPointer<XISFWriterEngine> m_engine;
   AutoPointer<XISFLogHandler>   m_logHandler;
   XISFOptions                   m_options;
   IsoString                     m_hints;
   String                        m_creatorApplication;
   String                        m_creatorModule;

   void CheckOpenStream( const char* ) const;
   void CheckClosedStream( const char* ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_XISF_h

// ----------------------------------------------------------------------------
// EOF pcl/XISF.h - Released 2018-11-01T11:06:36Z
