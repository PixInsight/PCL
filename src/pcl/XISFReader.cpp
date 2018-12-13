//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/XISFReader.cpp - Released 2018-12-12T09:24:30Z
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

#include <pcl/Compression.h>
#include <pcl/Cryptography.h>
#include <pcl/EndianConversions.h>
#include <pcl/XISF.h>

/*
 * Macro for handling recoverable errors during the XML header parsing phase.
 */
#define RECOVERABLE_ERROR_HANDLER()                                           \
   catch ( ... )                                                              \
   {                                                                          \
      try                                                                     \
      {                                                                       \
         throw;                                                               \
      }                                                                       \
      catch ( pcl::CaughtException& )                                        \
      {                                                                       \
      }                                                                       \
      catch ( pcl::Exception& x )                                             \
      {                                                                       \
         LogLn( x.Message(), XISFMessageType::RecoverableError );             \
      }                                                                       \
      catch ( ... )                                                           \
      {                                                                       \
         LogLn( "Unknown exception", XISFMessageType::RecoverableError );     \
      }                                                                       \
   }

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Generic data block in an XISF input stream.
 */
struct XISFInputDataBlock
{
   struct SubblockDimensions
   {
      fsize_type compressedSize   = 0; // size in bytes of the compressed subblock.
      fsize_type uncompressedSize = 0; // size in bytes of the uncompressed subblock.
   };

   /*
    * A list of compressed subblock dimensions.
    */
   typedef Array<SubblockDimensions>   subblock_info;

   /*
    * Compressed subblocks.
    */
   typedef Compression::subblock_list  subblock_list;

   /*
    * Block checksum algorithm.
    */
   typedef XISF::block_checksum        block_checksum;

   /*
    * Block compression codec.
    */
   typedef XISF::block_compression     block_compression;

   /*
    * Block byte order.
    */
   typedef XISF::block_endianness      block_endianness;

   /*
    * For blocks stored as attachments, the position and size members provide
    * direct file coordinates.
    *
    * For inline and embedded blocks, we have position=0 and the data already
    * acquired in the data array. The size member is redundant in this case.
    *
    * For compressed blocks stored as attachments, the subblockDimensions
    * member provides compressed and uncompressed subblock sizes in bytes.
    *
    * For inline and embedded compressed blocks, the data member already has
    * the compressed data loaded.
    */
   fpos_type         position           = 0; // absolute file position in bytes
   fsize_type        size               = 0; // file block size in bytes
   block_compression compressionCodec   = XISFCompression::None; // compression codec
   unsigned          compressedItemSize = 1; // size in bytes of a data item, for byte shuffling
   subblock_info     subblockInfo;           // compressed subblock dimensions
   subblock_list     subblocks;              // compressed data
   ByteArray         data;                   // uncompressed data
   ByteArray         checksum;               // cryptographic hash digest
   block_checksum    checksumAlgorithm  = XISFChecksum::None; // hashing algorithm
   mutable bool      checksumVerified   = false;
   unsigned          itemSize           = 1; // size in bytes of a data item, for endian conversion
   block_endianness  byteOrder          = XISFByteOrder::LittleEndian; // XISF little-endian default
   mutable bool      byteOrderApplied   = IsLittleEndianMachine();

   XISFInputDataBlock() = default;
   XISFInputDataBlock( const XISFInputDataBlock& ) = default;
   XISFInputDataBlock& operator =( const XISFInputDataBlock& ) = default;

   bool IsValid() const
   {
      return position > 0 || HasData() || HasCompressedData();
   }

   bool IsAttachment() const
   {
      return position > 0;
   }

   bool IsCompressed() const
   {
      return compressionCodec != XISFCompression::None;
   }

   bool HasData() const
   {
      return !data.IsEmpty();
   }

   bool HasCompressedData() const
   {
      return !subblocks.IsEmpty();
   }

   bool HasChecksum() const
   {
      return !checksum.IsEmpty();
   }

   bool IsEmpty() const
   {
      return size <= 0 && !HasData() && !HasCompressedData();
   }

   size_type DataSize() const
   {
      if ( IsEmpty() )
         return 0;

      if ( HasData() )
         return data.Length();

      if ( IsCompressed() )
      {
         size_type uncompressedSize = 0;
         for ( const SubblockDimensions& info : subblockInfo )
            uncompressedSize += info.uncompressedSize;
         return uncompressedSize;
      }

      return size;
   }

   void GetData( File& file, void* dst, size_type dstSize, size_type offset = 0 )
   {
      if ( IsEmpty() )
         throw Error( String( "XISFInputDataBlock::GetData(): " ) + "Internal error: Invalid function call." );

      VerifyChecksum( file );

      if ( IsCompressed() )
         Uncompress( file );

      if ( HasData() )
      {
         if ( offset + dstSize > data.Length() )
            throw Error( String( "XISFInputDataBlock::GetData(): " ) + "Internal error: Invalid destination array size." );
         ApplyByteOrder();
         ::memcpy( dst, data.At( offset ), dstSize );
      }
      else
      {
         if ( offset + dstSize > size_type( size ) )
            throw Error( String( "XISFInputDataBlock::GetData(): " ) + "Internal error: Invalid destination array size." );
         file.SetPosition( position + offset );
         file.Read( dst, dstSize );
         ApplyByteOrder( dst, dstSize );
      }
   }

   void LoadData( File& file )
   {
      VerifyChecksum( file );

      if ( !HasData() )
         if ( IsCompressed() )
            Uncompress( file );
         else
         {
            data = ByteArray( size );
            file.SetPosition( position );
            file.Read( data.Begin(), size );
         }

      ApplyByteOrder();
   }

   void UnloadData()
   {
      if ( IsCompressed() )
      {
         data.Clear();
         if ( IsAttachment() )
            subblocks.Clear();
      }
      else
      {
         if ( IsAttachment() )
            data.Clear();
      }

      byteOrderApplied = IsLittleEndianMachine() == (byteOrder == XISFByteOrder::LittleEndian);
   }

   void SetCompressedData( const ByteArray& compressedData )
   {
      if ( IsAttachment() || !IsCompressed() )
         throw Error( String( "XISFInputDataBlock::SetCompressedData(): " ) + "Internal error: Invalid function call." );

      size_type offset = 0;
      subblocks.Clear();
      for ( const SubblockDimensions& info : subblockInfo )
      {
         if ( offset + info.compressedSize > compressedData.Length() )
            throw Error( "Invalid or corrupted compressed block data." );
         Compression::Subblock subblock;
         subblock.compressedData = ByteArray( compressedData.At( offset ), compressedData.At( offset + info.compressedSize ) );
         subblock.uncompressedSize = info.uncompressedSize;
         subblocks << subblock;
         offset += info.compressedSize;
      }

      File f;
      VerifyChecksum( f );
   }

   void LoadCompressedData( File& file )
   {
      if ( IsEmpty() || !IsCompressed() )
         throw Error( String( "XISFInputDataBlock::LoadCompressedData(): " ) + "Internal error: Invalid function call." );

      VerifyChecksum( file );

      if ( !HasCompressedData() )
      {
         file.SetPosition( position );
         for ( const SubblockDimensions& info : subblockInfo )
         {
            Compression::Subblock subblock;
            subblock.compressedData = ByteArray( size_type( info.compressedSize ) );
            subblock.uncompressedSize = info.uncompressedSize;
            file.Read( subblock.compressedData.Begin(), info.compressedSize );
            subblocks << subblock;
         }

         if ( subblocks.IsEmpty() )
            throw Error( String( "XISFInputDataBlock::LoadCompressedData(): " ) + "Internal error: Invalid or corrupted compressed subblock data." );
      }
   }

   void Uncompress( File& file )
   {
      if ( IsEmpty() || !IsCompressed() )
         throw Error( String( "XISFInputDataBlock::Uncompress(): " ) + "Internal error: Invalid function call." );

      if ( !HasData() )
      {
         AutoPointer<Compression> compressor( XISF::NewCompression( compressionCodec, compressedItemSize ) );
         LoadCompressedData( file );
         data = compressor->Uncompress( subblocks );
         subblocks.Clear();
      }

      ApplyByteOrder();
   }

   void VerifyChecksum( File& file ) const
   {
      if ( HasChecksum() )
         if ( !checksumVerified )
         {
            ByteArray theChecksum;
            AutoPointer<CryptographicHash> hash( XISF::NewCryptographicHash( checksumAlgorithm ) );

            if ( IsAttachment() )
            {
               hash->Initialize();
               const size_type chunkSize = 4096;
               size_type numberOfChunks = size / chunkSize;
               size_type lastChunkSize = size % chunkSize;
               ByteArray chunk( chunkSize );
               file.SetPosition( position );
               for ( size_type i = 0; i <= numberOfChunks; ++i )
               {
                  size_type thisChunkSize = (i < numberOfChunks) ? chunkSize : lastChunkSize;
                  if ( thisChunkSize > 0 )
                  {
                     file.Read( reinterpret_cast<void*>( chunk.Begin() ), thisChunkSize );
                     hash->Update( chunk.Begin(), thisChunkSize );
                  }
               }
               theChecksum = hash->Finalize();
            }
            else
            {
               if ( HasCompressedData() )
               {
                  hash->Initialize();
                  for ( const Compression::Subblock& subblock : subblocks )
                     hash->Update( subblock.compressedData );
                  theChecksum = hash->Finalize();
               }
               else if ( HasData() )
               {
                  theChecksum = hash->Hash( data );
               }
               else
               {
                  throw Error( String( "XISFInputDataBlock::VerifyChecksum(): " ) + "Internal error: Invalid function call." );
               }
            }

            checksumVerified = true;

            if ( theChecksum != checksum )
               throw Error( "Block " + hash->AlgorithmName() + " checksum mismatch: "
                            "Expected " + IsoString::ToHex( checksum ) +
                               ", got " + IsoString::ToHex( theChecksum ) );
         }
   }

   void ApplyByteOrder( void* dst, size_type dstSize )
   {
      if ( !byteOrderApplied )
      {
         if ( HasChecksum() )
            if ( !checksumVerified )
               throw Error( String( "XISFInputDataBlock::ApplyByteOrder(): " ) + "Internal error: Invalid function call." );

         switch ( itemSize )
         {
         case 1:
            break;
         case 2:
            for ( uint16* p = reinterpret_cast<uint16*>( dst ), * q = p + dstSize/2; p < q; ++p )
               *p = BigToLittleEndian( *p );
            break;
         case 4:
            for ( uint32* p = reinterpret_cast<uint32*>( dst ), * q = p + dstSize/4; p < q; ++p )
               *p = BigToLittleEndian( *p );
            break;
         case 8:
            for ( uint64* p = reinterpret_cast<uint64*>( dst ), * q = p + dstSize/8; p < q; ++p )
               *p = BigToLittleEndian( *p );
            break;
         default: // ?!
            break;
         }

         byteOrderApplied = true;
      }
   }

   void ApplyByteOrder()
   {
      if ( !HasData() )
         throw Error( String( "XISFInputDataBlock::ApplyByteOrder(): " ) + "Internal error: Invalid function call." );

      ApplyByteOrder( data.Begin(), data.Length() );
   }
};

// ----------------------------------------------------------------------------

/*
 * Image data block in an XISF input stream.
 */
struct XISFInputImageBlock : public XISFInputDataBlock
{
   ImageInfo info;           // geometry and color space parameters
   bool      normal = false; // normal storage model?

   XISFInputImageBlock() = default;
   XISFInputImageBlock( const XISFInputImageBlock& ) = default;
   XISFInputImageBlock& operator =( const XISFInputImageBlock& ) = default;
};

// ----------------------------------------------------------------------------

/*
 * Property data block in an XISF input stream.
 */
struct XISFInputPropertyBlock : public XISFInputDataBlock
{
   typedef XISF::property_type   property_type;

           IsoString     id;         // unique property identifier
           property_type type;       // property data type
           Array<int>    dimensions; // dimensions of a vector or matrix property
   mutable Variant       value;      // property value

   XISFInputPropertyBlock() = default;
   XISFInputPropertyBlock( const XISFInputPropertyBlock& ) = default;
   XISFInputPropertyBlock& operator =( const XISFInputPropertyBlock& ) = default;

   XISFInputPropertyBlock( const IsoString& a_id ) : id( a_id )
   {
   }

   bool HasValue() const
   {
      return value.IsValid();
   }

   bool operator ==( const XISFInputPropertyBlock& x ) const
   {
      return id == x.id;
   }

   bool operator <( const XISFInputPropertyBlock& x ) const
   {
      return id < x.id;
   }
};

typedef SortedArray<XISFInputPropertyBlock> XISFInputPropertyBlockArray;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Internal XISF input stream implementation.
 */
class XISFReaderEngine
{
public:

   XISFReaderEngine() = default;

   virtual ~XISFReaderEngine() noexcept( false )
   {
   }

   XISFReaderEngine( const XISFReaderEngine& ) = delete;
   XISFReaderEngine& operator =( const XISFReaderEngine& ) = delete;

   /*
    * Set format-specific options.
    */
   void SetOptions( const XISFOptions& options )
   {
      m_xisfOptions = options;
   }

   /*
    * Set hints string for InputHints metadata property generation.
    */
   void SetHints( const IsoString& hints )
   {
      m_hints = hints;
   }

   void SetLogHandler( XISFLogHandler* handler )
   {
      m_logHandler = handler;
   }

   bool IsOpen() const
   {
      return !m_path.IsEmpty();
   }

   /*
    * Open an XISF file for read-only access. This member function opens a
    * local file and parses the whole XML header.
    */
   void Open( const String& path )
   {
      Reset();

#ifdef __PCL_WINDOWS
      m_path = File::WindowsPathToUnix( path );
#else
      m_path = path;
#endif

      try
      {
         if ( m_logHandler != nullptr )
            m_logHandler->Init( m_path, false/*writing*/ );

         m_file.OpenForReading( m_path );

         XMLDocument xml;
         {

            XISFFileSignature signature;
            m_file.Read( signature );
            signature.Validate();

            m_headerLength = signature.headerLength;
            m_fileSize = m_file.Size();
            m_minBlockPos = m_headerLength + sizeof( XISFFileSignature );

            IsoString header;
            header.SetLength( m_headerLength );
            m_file.Read( reinterpret_cast<void*>( header.Begin() ), m_headerLength );

            xml.SetParserOption( XMLParserOption::IgnoreComments );
            xml.SetParserOption( XMLParserOption::IgnoreUnknownElements );
            xml.Parse( header.UTF8ToUTF16() );
         }

         if ( xml.RootElement()->Name() != "xisf" || xml.RootElement()->AttributeValue( "version" ) != "1.0" )
            HeaderError( *xml.RootElement(), "Not an XISF version 1.0 file." );

         for ( const XMLNode& node : *xml.RootElement() )
         {
            if ( !node.IsElement() )
            {
               HeaderWarning( node, "Ignoring unexpected XML root child node of " + XMLNodeType::AsString( node.NodeType() ) + " type." );
               continue;
            }

            const XMLElement& element = static_cast<const XMLElement&>( node );

            if ( element.Name() == "Image" )
            {
               try
               {
                  ImageData data;

                  // geometry="<width>:<height>:<channel-count>"
                  GetImageGeometry( data.image, element );

                  // sampleFormat="<sample-format>"
                  String s = element.AttributeValue( "sampleFormat" );
                  if ( s.IsEmpty() )
                     HeaderError( element, "Missing Image sampleFormat attribute." );
                  {
                     int bitsPerSample;
                     bool floatSample, complexSample;
                     if ( !XISF::GetSampleFormatFromId( bitsPerSample, floatSample, complexSample, s ) )
                        HeaderError( element, "Invalid/unknown image sample format '" + s + "'" );
                     if ( bitsPerSample == 64 && !floatSample )
                        HeaderError( element, "64-bit integer images are not supported by this XISF implementation." );
                     data.options.bitsPerSample = bitsPerSample;
                     data.options.ieeefpSampleFormat = floatSample;
                     data.options.complexSample = complexSample;
                     data.image.itemSize = bitsPerSample >> 3;
                  }

                  data.options.lowerRange = 0;
                  if ( data.options.ieeefpSampleFormat )
                     data.options.upperRange = 1;
                  else
                     data.options.upperRange = (uint64( 1 ) << data.options.bitsPerSample) - 1; // assume bitsPerSample <= 32

                  // bounds="<lower>:<upper>"
                  s = element.AttributeValue( "bounds" );
                  if ( s.IsEmpty() )
                  {
                     if ( data.options.ieeefpSampleFormat && !data.options.complexSample )
                        HeaderError( element, "Missing bounds Image attribute, which is mandatory for a floating point real image." );
                  }
                  else
                  {
                     if ( data.options.complexSample )
                        HeaderWarning( element, "Ignoring bounds Image attribute, which is unsupported for complex images." );

                     StringList tokens;
                     s.Break( tokens, ':', true/*trim*/ );
                     if ( tokens.Length() < 2 )
                        HeaderError( element, "Malformed bounds Image attribute: '" + s + "'" );
                     data.options.lowerRange = tokens[0].ToDouble();
                     data.options.upperRange = tokens[1].ToDouble();

                     if ( !IsFinite( data.options.lowerRange ) || !IsFinite( data.options.upperRange ) )
                        HeaderWarning( element, "Non-numeric bounds Image attribute value(s)." );

                     if ( tokens.Length() > 2 )
                        HeaderWarning( element, "Ignoring excess image bounds data: '" + s + "'" );

                     if ( !data.options.ieeefpSampleFormat )
                     {
                        if ( data.options.lowerRange < 0 || data.options.lowerRange >= (uint64( 1 ) << data.options.bitsPerSample) )
                           HeaderError( element, "Invalid " + String( data.options.bitsPerSample ) + "-bit integer lower bound: " + s );
                        if ( data.options.upperRange < 0 || data.options.upperRange >= (uint64( 1 ) << data.options.bitsPerSample) )
                           HeaderError( element, "Invalid " + String( data.options.bitsPerSample ) + "-bit integer upper bound: " + s );
                     }

                     if ( data.options.upperRange < data.options.lowerRange )
                     {
                        Swap( data.options.lowerRange, data.options.upperRange );
                        HeaderWarning( element, "Swapping unordered lower and upper bound attribute values (good try! :)" );
                     }

                     if ( 1 == 1 + data.options.upperRange - data.options.lowerRange )
                        HeaderWarning( element, "Empty or infinitesimal pixel sample range." );
                  }

                  // colorSpace="<s>"
                  s = element.AttributeValue( "colorSpace" );
                  if ( s.IsEmpty() )
                  {
                     data.image.info.colorSpace = ColorSpace::Gray;
                     HeaderWarning( element, "Missing colorSpace Image attribute: Assuming the grayscale color space." );
                  }
                  else
                  {
                     data.image.info.colorSpace = XISF::ColorSpaceFromId( s );
                     if ( data.image.info.colorSpace == ColorSpace::Unknown )
                        HeaderError( element, "Invalid/unknown image color space '" + s + "'" );
                  }

                  data.image.info.supported = data.image.info.IsValid();
                  if ( !data.image.info.supported )
                     HeaderError( element, "Invalid/unsupported image parameters." );

                  // offset="<sample-value>"
                  s = element.AttributeValue( "offset" );
                  if ( !s.IsEmpty() )
                  {
                     // ### TODO
                  }

                  // pixelStorage="<storage-model>"
                  s = element.AttributeValue( "pixelStorage" ).CaseFolded();
                  if ( !s.IsEmpty() )
                  {
                     if ( s == "normal" )
                        data.image.normal = true;
                     else if ( s != "planar" )
                        HeaderError( element, "Unknown/unsupported pixel storage model \'" + s + "\'" );
                  }

                  // imageType="<type-spec>"
                  s = element.AttributeValue( "imageType" );
                  if ( !s.IsEmpty() )
                  {
                     // ### TODO
                  }

                  // id="<image-id>"
                  data.id = element.AttributeValue( "id" );
                  if ( !data.id.IsEmpty() )
                     if ( data.id.IsValidIdentifier() )
                     {
                        XISFInputPropertyBlock property;
                        property.id = XISF::InternalPropertyId( "ImageIdentifier" );
                        property.type = VariantType::IsoString;
                        property.value = data.id;
                        AddPropertyBlock( data.properties, property, element );
                     }
                     else
                     {
                        HeaderWarning( element, "Ignoring invalid image identifier '" + data.id + "'" );
                        data.id.Clear();
                     }

                  // uuid="<uuid>"
                  s = element.AttributeValue( "uuid" );
                  if ( !s.IsEmpty() )
                  {
                     // ### TODO
                  }

                  // orientation="<rotation>"
                  s = element.AttributeValue( "orientation" );
                  if ( !s.IsEmpty() )
                  {
                     // ### TODO
                  }

                  if ( m_xisfOptions.verbosity > 0 )
                     LogLn( "Loading image" + (data.id.IsEmpty() ? String() : String( " '" + data.id + '\'' )) +
                        String().Format( ": w=%d h=%d n=%d ",
                                          data.image.info.width, data.image.info.height, data.image.info.numberOfChannels ) +
                        XISF::ColorSpaceId( data.image.info.colorSpace ) + ' ' +
                        XISF::SampleFormatId( data.options.bitsPerSample, data.options.ieeefpSampleFormat, data.options.complexSample ) );

                  /*
                   * Image elements can only have attachment or embedded
                   * storage.
                   */
                  GetBlock( data.image, element );

                  /*
                   * Child Image elements.
                   */
                  for ( const XMLNode& childNode : element )
                  {
                     if ( !childNode.IsElement() )
                     {
                        HeaderWarning( childNode, "Ignoring unexpected Image child node of " + XMLNodeType::AsString( childNode.NodeType() ) + " type." );
                        continue;
                     }

                     const XMLElement& childElement = static_cast<const XMLElement&>( childNode );

                     if ( childElement.Name() == "Property" )
                     {
                        /*
                         * A property of the current image.
                         */
                        if ( !m_xisfOptions.ignoreProperties )
                        {
                           try
                           {
                              GetProperty( data.properties, childElement, false/*isInternal*/ );
                           }
                           RECOVERABLE_ERROR_HANDLER()
                        }
                     }
                     else if ( childElement.Name() == "FITSKeyword" )
                     {
                        /*
                         * FITS header keywords.
                         */
                        if ( !m_xisfOptions.ignoreFITSKeywords )
                        {
                           try
                           {
                              GetFITSKeyword( data.properties, data.keywords, childElement );
                           }
                           RECOVERABLE_ERROR_HANDLER()
                        }
                     }
                     else if ( childElement.Name() == "RGBWorkingSpace" )
                     {
                        /*
                         * RGB working space parameters.
                         */
                        if ( !m_xisfOptions.ignoreEmbeddedData )
                        {
                           try
                           {
                              GetRGBWS( data.rgbws, childElement );
                           }
                           RECOVERABLE_ERROR_HANDLER()
                        }
                     }
                     else if ( childElement.Name() == "DisplayFunction" )
                     {
                        /*
                         * Display function parameters.
                         */
                        if ( !m_xisfOptions.ignoreEmbeddedData )
                        {
                           try
                           {
                              GetDisplayFunction( data.df, childElement );
                           }
                           RECOVERABLE_ERROR_HANDLER()
                        }
                     }
                     else if ( childElement.Name() == "ColorFilterArray" )
                     {
                        /*
                         * CFA parameters.
                         */
                        if ( !m_xisfOptions.ignoreEmbeddedData )
                        {
                           try
                           {
                              GetColorFilterArray( data.cfa, childElement );
                           }
                           RECOVERABLE_ERROR_HANDLER()
                        }
                     }
                     else if ( childElement.Name() == "Resolution" )
                     {
                        /*
                         * Image resolution data.
                         */
                        if ( !m_xisfOptions.ignoreEmbeddedData )
                        {
                           try
                           {
                              GetImageResolution( data.options, childElement );
                           }
                           RECOVERABLE_ERROR_HANDLER()
                        }
                     }
                     else if ( childElement.Name() == "ICCProfile" )
                     {
                        /*
                         * Embedded ICC profile.
                         */
                        if ( !m_xisfOptions.ignoreEmbeddedData )
                        {
                           try
                           {
                              if ( data.iccProfile.IsValid() )
                                 HeaderWarning( childElement, "Redefining ICCProfile Image child element - previously defined ICC profile will be ignored." );

                              GetICCProfile( data.iccProfile, childElement );
                           }
                           RECOVERABLE_ERROR_HANDLER()
                        }
                     }
                     else if ( childElement.Name() == "Thumbnail" )
                     {
                        /*
                         * Embedded image thumbnail.
                         */
                        if ( !m_xisfOptions.ignoreEmbeddedData )
                        {
                           try
                           {
                              if ( data.thumbnail.IsValid() )
                                 HeaderWarning( childElement, "Redefining Thumbnail Image child element - previously defined thumbnail will be ignored." );

                              GetThumbnail( data.thumbnail, childElement );
                           }
                           RECOVERABLE_ERROR_HANDLER()
                        }
                     }
                     else if ( childElement.Name() == "Data" )
                     {
                        /*
                         * Image data (location="embedded").
                         */
                        if ( data.image.IsEmpty() )
                           GetBlockEmbeddedData( data.image, childElement );
                        else
                           HeaderWarning( childElement, "Ignoring unexpected Image embedded data." );
                     }
                     else
                     {
                        HeaderWarning( childElement, "Skipping unknown \'" + childElement.Name() + "\' Image child element." );
                     }
                  }

                  if ( m_xisfOptions.verbosity > 0 )
                     if ( !data.properties.IsEmpty() )
                        LogLn( String( data.properties.Length() ) + " image " +
                                          ((data.properties.Length() > 1) ? "properties" : "property") );

                  m_images.Append( data );

               } // try
               RECOVERABLE_ERROR_HANDLER()
            }
            else if ( element.Name() == "Property" )
            {
               /*
                * A property associated with the XISF unit.
                */
               if ( !m_xisfOptions.ignoreProperties )
               {
                  try
                  {
                     GetProperty( m_properties, element, false/*isInternal*/ );
                  }
                  RECOVERABLE_ERROR_HANDLER()
               }
            }
            else if ( element.Name() == "Metadata" )
            {
               try
               {
                  /*
                   * The Metadata element contains a sequence of property child
                   * elements. All of these properties are internal, i.e. they
                   * are in the XISF namespace.
                   */
                  for ( const XMLNode& childNode : element )
                  {
                     if ( !childNode.IsElement() )
                     {
                        HeaderWarning( childNode, "Ignoring unexpected Metadata child node of " + XMLNodeType::AsString( childNode.NodeType() ) + " type." );
                        continue;
                     }

                     const XMLElement& childElement = static_cast<const XMLElement&>( childNode );

                     if ( childElement.Name() == "Property" )
                     {
                        try
                        {
                           GetProperty( m_properties, childElement, true/*isInternal*/ );
                        }
                        RECOVERABLE_ERROR_HANDLER()
                     }
                     else
                     {
                        HeaderWarning( childElement, "Skipping unexpected \'" + childElement.Name() + "\' Metadata child element." );
                     }
                  }
               }
               RECOVERABLE_ERROR_HANDLER()
            }
            else
            {
               HeaderWarning( element, "Skipping unknown \'" + element.Name() + "\' element." );
            }
         }

         /*
          * Optional automatic generation of internal properties.
          */
         if ( m_xisfOptions.autoMetadata )
         {
            XISFInputPropertyBlock property;
            property.id = XISF::InternalPropertyId( "ResourceURL" );
            property.type = VariantType::String;
            property.value = File::FileURI( m_path );
            AddPropertyBlock( m_properties, property );

            property.id = XISF::InternalPropertyId( "LoadTime" );
            property.type = VariantType::TimePoint;
            property.value = TimePoint::Now();
            AddPropertyBlock( m_properties, property );

            if ( !m_hints.IsEmpty() )
            {
               property.id = XISF::InternalPropertyId( "InputHints" );
               property.type = VariantType::IsoString;
               property.value = m_hints;
               AddPropertyBlock( m_properties, property );
            }
         }
      }
      catch ( ... )
      {
         Reset();
         throw;
      }
   }

   /*
    * Close the input stream and reset all internal data structures.
    */
   void Close()
   {
      Reset();
      if ( m_logHandler != nullptr )
         m_logHandler->Close();
   }

   /*
    * The file path of the current input stream.
    */
   String FilePath() const
   {
      return m_path;
   }

   /*
    * The number of images stored in this XISF file.
    */
   int NumberOfImages() const
   {
      return int( m_images.Length() );
   }

   /*
    * Select the current image for input operations. index must be in the range
    * [0,NumberOfImages()-1].
    */
   void SelectImage( int index )
   {
      ValidateImageAccess( index );
      m_currentImage = index;
   }

   /*
    * Zero-based index of the current image.
    */
   int SelectedImageIndex() const
   {
      return m_currentImage;
   }

   /*
    * Geometry and color space parameters of the current image.
    */
   const pcl::ImageInfo& ImageInfo() const
   {
      ValidateImageAccess( m_currentImage );
      return m_images[m_currentImage].image.info;
   }

   /*
    * Format-independent options and parameters of the current image.
    */
   const pcl::ImageOptions& ImageOptions() const
   {
      ValidateImageAccess( m_currentImage );
      return m_images[m_currentImage].options;
   }

   /*
    * Sets format-independent options for the current image. Only options that
    * can change the reading behavior are taken into account.
    */
   void SetImageOptions( const pcl::ImageOptions& options )
   {
      ValidateImageAccess( m_currentImage );
      m_images[m_currentImage].options.readNormalized = options.readNormalized;
   }

   /*
    * Identifier (optional) of the current image.
    */
   IsoString ImageId() const
   {
      ValidateImageAccess( m_currentImage );
      return m_images[m_currentImage].id;
   }

   /*
    * The set of FITS header keywords associated with the current image.
    */
   FITSKeywordArray FITSKeywords() const
   {
      ValidateImageAccess( m_currentImage );
      FITSKeywordArray keywords = m_images[m_currentImage].keywords;
      if ( m_xisfOptions.verbosity > 0 )
         if ( !keywords.IsEmpty() )
            LogLn( String( keywords.Length() ) + " FITS keyword(s) extracted." );
      return keywords;
   }

   /*
    * The RGB working space (RGBWS) associated with the current image.
    */
   pcl::RGBColorSystem RGBWorkingSpace() const
   {
      ValidateImageAccess( m_currentImage );
      return m_images[m_currentImage].rgbws;
   }

   /*
    * The display function (aka screen transfer function, or STF) associated
    * with the current image.
    */
   pcl::DisplayFunction DisplayFunction() const
   {
      ValidateImageAccess( m_currentImage );
      return m_images[m_currentImage].df;
   }

   /*
    * The CFA pattern of the current image.
    */
   pcl::ColorFilterArray ColorFilterArray() const
   {
      ValidateImageAccess( m_currentImage );
      return m_images[m_currentImage].cfa;
   }

   /*
    * The ICC profile structure associated with the current image.
    */
   ICCProfile ReadICCProfile()
   {
      ValidateImageAccess( m_currentImage );
      XISFInputDataBlock& block = m_images[m_currentImage].iccProfile;
      if ( !block.IsValid() )
         return ICCProfile();

      ByteArray data( block.DataSize() );
      GetBlockData( block, data.Begin(), data.Size() );
      block.UnloadData();
      ICCProfile icc( data );
      if ( m_xisfOptions.verbosity > 0 )
         if ( icc.IsProfile() )
            LogLn( "ICC profile extracted: \'" + icc.Description() + "\', " + String( icc.ProfileSize() ) + " bytes." );
      return icc;
   }

   /*
    * The thumbnail image associated with the current image.
    */
   UInt8Image ReadThumbnail()
   {
      ValidateImageAccess( m_currentImage );
      UInt8Image thumbnail;
      XISFInputImageBlock& block = m_images[m_currentImage].thumbnail;
      if ( block.IsValid() )
         if ( block.itemSize == 1 )
            ReadThumbnail( thumbnail, block );
         else
         {
            UInt16Image thumbnail16;
            ReadThumbnail( thumbnail16, block );
            thumbnail = thumbnail16;
         }
      return thumbnail;
   }

   template <class P>
   void ReadThumbnail( GenericImage<P>& thumbnail, XISFInputImageBlock& block )
   {
      thumbnail.AllocateData( block.info.width, block.info.height, block.info.numberOfChannels, ColorSpace::value_type( block.info.colorSpace ) );
      if ( block.DataSize() != thumbnail.ImageSize() )
         throw Error( String( "XISFReaderEngine::ReadThumbnail(): " ) + "Internal error: Inconsistent block size." );

      GetBlockData( block, thumbnail );
      block.UnloadData();

      if ( m_xisfOptions.verbosity > 1 )
         if ( !thumbnail.IsEmpty() )
            LogLn( "Image thumbnail extracted " +
               String().Format( "(w=%d h=%d n=%d)", thumbnail.Width(), thumbnail.Height(), thumbnail.NumberOfChannels() ) );
   }

   /*
    * Returns a descriptive list of the properties associated with the current
    * image.
    */
   PropertyDescriptionArray ImageProperties() const
   {
      ValidateImageAccess( m_currentImage );
      return MakePropertyDescriptions( m_images[m_currentImage].properties );
   }

   /*
    * Read the value of a property of the current image.
    */
   Variant ReadImageProperty( const IsoString& id )
   {
      ValidateImageAccess( m_currentImage );
      return LoadPropertyValue( m_images[m_currentImage].properties, id );
   }

   /*
    * Returns a descriptive list of the properties associated with this XISF
    * unit.
    */
   PropertyDescriptionArray Properties() const
   {
      return MakePropertyDescriptions( m_properties );
   }

   /*
    * Read the value of a property associated with this XISF unit.
    */
   Variant ReadProperty( const IsoString& id )
   {
      return LoadPropertyValue( m_properties, id );
   }

   /*
    * Read the current image and store it (possibly involving a pixel sample
    * data type conversion) in the specified ImageVariant.
    */
   void ReadImage( ImageVariant& image )
   {
      if ( image )
         if ( image.IsFloatSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: ReadImage( static_cast<FImage&>( *image ) ); break;
            case 64: ReadImage( static_cast<DImage&>( *image ) ); break;
            }
         else if ( image.IsComplexSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: ReadImage( static_cast<ComplexImage&>( *image ) ); break;
            case 64: ReadImage( static_cast<DComplexImage&>( *image ) ); break;
            }
         else
            switch ( image.BitsPerSample() )
            {
            case  8: ReadImage( static_cast<UInt8Image&>( *image ) ); break;
            case 16: ReadImage( static_cast<UInt16Image&>( *image ) ); break;
            case 32: ReadImage( static_cast<UInt32Image&>( *image ) ); break;
            }
   }

   /*
    * Read the current image and store it (possibly involving a pixel sample
    * data type conversion) in the specified GenericImage<>.
    */
   template <class P>
   void ReadImage( GenericImage<P>& image )
   {
      ValidateImageAccess( m_currentImage );
      const pcl::ImageOptions& options = m_images[m_currentImage].options;
      if (  options.bitsPerSample != P::BitsPerSample() ||
           (options.ieeefpSampleFormat && !options.complexSample) != P::IsFloatSample() ||
            options.complexSample != P::IsComplexSample() )
      {
         ImageVariant tmp;
         tmp.CreateImage( options.ieeefpSampleFormat, options.complexSample, options.bitsPerSample );
         ReadImage( tmp );
         ImageVariant( &image ).CopyImage( tmp );
         return;
      }

      XISFInputImageBlock& block = m_images[m_currentImage].image;
      if ( !block.IsValid() )
         throw Error( String( "XISFReaderEngine::ReadImage(): " ) + "Internal error: invalid image block." );

      image.AllocateData( block.info.width, block.info.height, block.info.numberOfChannels, ColorSpace::value_type( block.info.colorSpace ) );
      if ( block.DataSize() != image.ImageSize() )
         throw Error( String( "XISFReaderEngine::ReadImage(): " ) + "Internal error: Inconsistent block size." );

      GetBlockData( block, image );
      block.UnloadData();

      if ( options.readNormalized )
         NormalizeImage( image, options );
   }

   /*
    * Read a block of contiguous pixel sample rows from the current image in
    * this input stream.
    */
   template <typename T, class P>
   void ReadSamples( T* buffer, int startRow, int rowCount, int channel, P* )
   {
      ValidateImageAccess( m_currentImage );
      XISFInputImageBlock& block = m_images[m_currentImage].image;
      if ( !block.IsValid() )
         throw Error( String( "XISFReaderEngine::ReadSamples(): " ) + "Internal error: invalid image block." );

      const pcl::ImageOptions& options = m_images[m_currentImage].options;
      if ( options.complexSample )
         switch ( options.bitsPerSample )
         {
         case 32: ReadSamples( buffer, startRow, rowCount, channel, (P*)0, (ComplexPixelTraits*)0 ); break;
         case 64: ReadSamples( buffer, startRow, rowCount, channel, (P*)0, (DComplexPixelTraits*)0 ); break;
         }
      else if ( options.ieeefpSampleFormat )
         switch ( options.bitsPerSample )
         {
         case 32: ReadSamples( buffer, startRow, rowCount, channel, (P*)0, (FloatPixelTraits*)0 ); break;
         case 64: ReadSamples( buffer, startRow, rowCount, channel, (P*)0, (DoublePixelTraits*)0 ); break;
         }
      else
         switch ( options.bitsPerSample )
         {
         case  8: ReadSamples( buffer, startRow, rowCount, channel, (P*)0, (UInt8PixelTraits*)0 ); break;
         case 16: ReadSamples( buffer, startRow, rowCount, channel, (P*)0, (UInt16PixelTraits*)0 ); break;
         case 32: ReadSamples( buffer, startRow, rowCount, channel, (P*)0, (UInt32PixelTraits*)0 ); break;
         }
   }

private:

   struct ImageData
   {
      pcl::ImageOptions           options;    // sample format, etc.
      IsoString                   id;         // optional image identifier
      pcl::RGBColorSystem         rgbws;      // RGB working space
      pcl::DisplayFunction        df;         // display function
      pcl::ColorFilterArray       cfa;        // CFA pattern
      pcl::FITSKeywordArray       keywords;   // legacy FITS header keywords
      XISFInputImageBlock         image;      // image data block
      XISFInputDataBlock          iccProfile; // ICC profile data block
      XISFInputImageBlock         thumbnail;  // image thumbnail data block
      XISFInputPropertyBlockArray properties; // image properties
   };

   typedef Array<ImageData> ImageDataArray;

   /*
    * Stream data
    */
   XISFOptions             m_xisfOptions;    // format-specific options
   IsoString               m_hints;          // format hints (for metadata generation only)
   mutable XISFLogHandler* m_logHandler = nullptr;
   String                  m_path;           // path to the input file
   File                    m_file;           // the input file
   fsize_type              m_fileSize;       // size in bytes of the input file
   fsize_type              m_headerLength;   // length in bytes of the XML file header
   fsize_type              m_minBlockPos;    // minimum valid absolute block position in bytes

   /*
    * Image data
    */
   ImageDataArray          m_images;         // fetched data of all readable images
   int                     m_currentImage;   // index of the currently selected image

   XISFInputPropertyBlockArray
                           m_properties;     // unit properties

   int                     m_historyKeywordCount;  // conversion of multiple HISTORY
   int                     m_commentKeywordCount;  //   and COMMENT FITS keywords

   /*
    * Get the position, size and compression parameters of a block from the
    * current XML element.
    */
   void GetBlock( XISFInputDataBlock& block, const XMLElement& element, bool getEmbeddedData = false )
   {
      block.position = 0;
      block.size = 0;
      block.data.Clear();

      String s( element.AttributeValue( "location" ) );
      if ( s.IsEmpty() )
         HeaderError( element, "Missing block location attribute." );

      StringList tokens;
      s.Break( tokens, ':', true/*trim*/ );

      if ( tokens[0] == "attachment" )
      {
         //  location="attachment:<position>:<size>"
         if ( tokens.Length() != 3 )
            HeaderError( element, "Malformed attachment location attribute: '" + s + "'" );
         block.position = tokens[1].ToUInt64();
         if ( block.position < m_minBlockPos || block.position >= m_fileSize )
            HeaderError( element, "Invalid block position: " + s );
         block.size = tokens[2].ToUInt64();
         if ( block.size == 0 || block.size + block.position > m_fileSize )
            HeaderError( element, "Invalid block size: " + s );

         GetBlockCompression( block, element );
         GetBlockChecksum( block, element );
         GetBlockByteOrder( block, element );
      }
      else if ( tokens[0] == "inline" )
      {
         // location="inline:<encoding>"
         if ( tokens.Length() != 2 )
            HeaderError( element, "Malformed inline location attribute: '" + s + "'" );

         GetBlockEncodedData( block, element, tokens[1] );
      }
      else if ( tokens[0] == "embedded" )
      {
         // location="embedded"
         if ( tokens.Length() != 1 )
            HeaderError( element, "Malformed embedded location attribute: '" + s + "'" );

         if ( getEmbeddedData )
            for ( const XMLNode& childNode : element )
            {
               if ( !childNode.IsElement() )
               {
                  HeaderWarning( childNode, "Ignoring unexpected " + element.Name() + " element child node of " + XMLNodeType::AsString( childNode.NodeType() ) + " type." );
                  continue;
               }

               const XMLElement& childElement = dynamic_cast<const XMLElement&>( childNode );
               if ( childElement.Name() == "Data" )
               {
                  // location="embedded"
                  if ( !block.IsEmpty() )
                     HeaderWarning( element, "Redefining " + element.Name() + " embedded data - previously defined data will be ignored." );

                  GetBlockEmbeddedData( block, childElement );
               }
               else
               {
                  HeaderWarning( childElement, "Skipping unknown \'" + childElement.Name() + "\' " + element.Name() + " child element." );
               }
            }
      }
      else if ( tokens[0] == "url" )
      {
         // location="url(<URL>)"
         // location="url(<URL>):<index-id>"
         HeaderError( element, "URL block locations are forbidden in a monolithic XISF file: '" + s + "'" );
      }
      else if ( tokens[0] == "path" )
      {
         // location="path(<abs-file-path>)"
         // location="path(<abs-file-path>):<index-id>"
         HeaderError( element, "Path block locations are forbidden in a monolithic XISF file: '" + s + "'" );
      }
      else
      {
         throw Error( "Invalid or unknown block location: '" + s + "'" );
      }
   }

   void GetBlockEncodedData( XISFInputDataBlock& block, const XMLElement& element, const String& encoding )
   {
      GetBlockCompression( block, element );
      GetBlockChecksum( block, element );
      GetBlockByteOrder( block, element );

      ByteArray data;
      IsoString encodedData( element.Text().Trimmed() ); // white space is irrelevant in encoded data
      if ( encoding == "base64" )
         data = encodedData.FromBase64();
      else if ( encoding == "hex" )
         data = encodedData.FromHex();
      else
         HeaderError( element, "Invalid/unsupported data encoding '" + encoding + "'" );

      block.size = data.Length();

      if ( block.IsCompressed() )
      {
         if ( block.subblockInfo[0].compressedSize == 0 )
            block.subblockInfo[0].compressedSize = data.Length();
         block.SetCompressedData( data );
      }
      else
      {
         block.data = data;
         block.VerifyChecksum( m_file );
         block.ApplyByteOrder();
      }
   }

   void GetBlockEmbeddedData( XISFInputDataBlock& block, const XMLElement& element )
   {
      String encoding = element.AttributeValue( "encoding" ).CaseFolded(); // encodings are case-insensitive
      if ( encoding.IsEmpty() )
      {
         encoding = "base64";
         HeaderWarning( element, String( "Missing data encoding attribute; assuming base64 encoding by default." ) );
      }

      GetBlockEncodedData( block, element, encoding );
   }

   /*
    * Get block compression parameters: compression algorithm and subblock
    * descriptors.
    */
   void GetBlockCompression( XISFInputDataBlock& block, const XMLElement& element )
   {
      block.compressionCodec = XISFCompression::None;
      block.compressedItemSize = 1;
      block.subblockInfo.Clear();
      block.subblocks.Clear();

      // compression="<codec>:<uncompressed-size>"
      // compression="<codec>:<uncompressed-size>:<item-size>"
      String value = element.AttributeValue( "compression" );
      if ( !value.IsEmpty() )
      {
         StringList tokens;
         value.Break( tokens, ':', true/*trim*/ );

         if ( tokens.Length() < 2 || tokens.Length() > 3 ) // all supported codecs use one or two parameters
            HeaderError( element, "Malformed block compression attribute: '" + value + "'" );

         block.compressionCodec = XISF::CompressionCodecFromId( tokens[0] );
         if ( block.compressionCodec == XISFCompression::None )
            HeaderError( element, "Missing data compression algorithm: " + value );
         if ( block.compressionCodec == XISFCompression::Unknown )
            HeaderError( element, "Unknown/unsupported data compression algorithm '" + tokens[0] + "'" );

         size_type uncompressedSize = tokens[1].ToUInt64();
         if ( uncompressedSize == 0 )
            HeaderError( element, "Invalid uncompressed block size: " + value );

         if ( tokens.Length() > 2 )
         {
            block.compressedItemSize = tokens[2].ToUInt();
            if ( block.compressedItemSize < 1 || block.compressedItemSize > 16 )
               HeaderError( element, "Invalid uncompressed item size: " + value );
         }

         // subblocks="<cs0>,<us0>:<cs1>,<us1>:...:<csN-1>,<usN-1>"
         value = element.AttributeValue( "subblocks" );
         if ( !value.IsEmpty() )
         {
            tokens.Clear();
            value.Break( tokens, ':', true/*trim*/ );
            for ( const String& token : tokens )
            {
               StringList items;
               token.Break( items, ',', true/*trim*/ );

               if ( items.Length() != 2 )
                  HeaderError( element, "Malformed compression subblocks attribute: '" + value + "'" );

               XISFInputDataBlock::SubblockDimensions d;
               d.compressedSize = items[0].ToInt64();
               d.uncompressedSize = items[1].ToInt64();
               if ( d.compressedSize <= 0 || d.uncompressedSize <= 0 )
                  HeaderError( element, "Invalid compression subblock parameters: '" + token + "'" );
               block.subblockInfo << d;
            }
         }
         else
         {
            XISFInputDataBlock::SubblockDimensions d;
            d.compressedSize = block.size;
            d.uncompressedSize = uncompressedSize;
            block.subblockInfo << d;
         }
      }
   }

   /*
    * Get block checksum parameters: hashing algorithm and digest value.
    */
   void GetBlockChecksum( XISFInputDataBlock& block, const XMLElement& element )
   {
      block.checksum.Clear();
      block.checksumAlgorithm = XISFChecksum::None;
      block.checksumVerified = false;

      // checksum="<algorithm>:<digest>"
      String value = element.AttributeValue( "checksum" );
      if ( !value.IsEmpty() )
      {
         StringList tokens;
         value.Break( tokens, ':', true/*trim*/ );

         if ( tokens.Length() != 2 )
            HeaderError( element, "Malformed block checksum attribute: '" + value + "'" );

         block.checksumAlgorithm = XISF::ChecksumAlgorithmFromId( tokens[0] );
         if ( block.checksumAlgorithm == XISFChecksum::None )
            HeaderError( element, "Missing checksum algorithm: " + value );
         if ( block.checksumAlgorithm == XISFChecksum::Unknown )
            HeaderError( element, "Unknown/unsupported checksum algorithm '" + tokens[0] + "'" );

         block.checksum = IsoString( tokens[1] ).FromHex();
         if ( block.checksum.Length() != XISF::ChecksumLength( block.checksumAlgorithm ) )
            HeaderError( element, "Invalid checksum length: '" + tokens[1] + "'" );
      }
   }

   /*
    * Get block endianness.
    */
   void GetBlockByteOrder( XISFInputDataBlock& block, const XMLElement& element )
   {
      block.byteOrder = XISFByteOrder::LittleEndian;
      block.byteOrderApplied = IsLittleEndianMachine();

      // byteOrder="<endianness>"
      String value = element.AttributeValue( "byteOrder" );
      if ( !value.IsEmpty() )
      {
         if ( value == "big" )
         {
            block.byteOrder = XISFByteOrder::BigEndian;
            block.byteOrderApplied = IsBigEndianMachine();
         }
         else if ( value != "little" )
            HeaderError( element, "Invalid block byte order specification '" + value + "'" );

         if ( block.itemSize == 1 )
            HeaderWarning( element, "The byteOrder attribute should not be specified for unstructured byte sequences." );
      }
   }

   void GetBlockData( XISFInputDataBlock& block, void* dst, size_type dstSize, size_type offset = 0u )
   {
      bool verbose = m_xisfOptions.verbosity > 0 && block.IsCompressed() && !block.HasData();
      if ( verbose )
         Log( "Uncompressing block (" +
               String( XISF::CompressionCodecId( block.compressionCodec ) ) + "): " +
               File::SizeAsString( block.size ) + " -> " );

      block.GetData( m_file, dst, dstSize, offset );

      if ( verbose )
         LogLn( File::SizeAsString( block.data.Size() ) +
               String().Format( " (%.2f%%)", 100*double( block.data.Size() - block.size )/block.data.Size() ) );
   }

   template <class P>
   void GetBlockData( XISFInputImageBlock& block, GenericImage<P>& image )
   {
      if ( block.normal && image.NumberOfChannels() > 1 )
      {
         // Normal storage model.
         AutoPointer<typename P::sample> data = new typename P::sample[ image.NumberOfSamples() ];
         typename P::sample* p = data.Ptr();
         GetBlockData( block, p, image.ImageSize() );
         for ( typename GenericImage<P>::pixel_iterator i( image ); i; ++i )
            for ( int j = 0; j < image.NumberOfChannels(); ++j )
               i[j] = *p++;
      }
      else
      {
         // Planar storage model.
         for ( int i = 0; i < image.NumberOfChannels(); ++i )
            GetBlockData( block, image[i], image.ChannelSize(), size_type( i )*image.ChannelSize() );
      }
   }

   /*
    * Add a new property block to the specified property list.
    */
   void AddPropertyBlock( XISFInputPropertyBlockArray& properties, const XISFInputPropertyBlock& property, const XMLElement& element )
   {
      XISFInputPropertyBlockArray::const_iterator i = properties.Search( property );
      if ( i == properties.End() )
         properties.Add( property );
      else
      {
         *properties.MutableIterator( i ) = property;
         HeaderWarning( element, "Redefining property '" + property.id + "'" );
      }
   }

   void AddPropertyBlock( XISFInputPropertyBlockArray& properties, const XISFInputPropertyBlock& property )
   {
      AddPropertyBlock( properties, property, XMLElement() );
   }

   /*
    * XISF 1.0 - Section 8.3.2
    */
   static unsigned long ParseUInt( const String& value )
   {
      if ( value.StartsWith( '0' ) )
         if ( value.Length() > 2 )
            switch ( value[1] )
            {
            case 'x':
            case 'X':
               return value.ToUInt( 0 ); // automatic base detection for 0x prefix
            case 'b':
            case 'B':
               return value.Substring( 2 ).ToUInt( 2 ); // XISF binary prefix 0b
            case 'o':
            case 'O':
               return value.Substring( 2 ).ToUInt( 8 ); // XISF octal prefix 0o
            default:
               break;
            }

      return value.ToUInt( 10 ); // XISF default decimal representation
   }
   static unsigned long long ParseUInt64( const String& value )
   {
      if ( value.StartsWith( '0' ) )
         if ( value.Length() > 2 )
            switch ( value[1] )
            {
            case 'x':
            case 'X':
               return value.ToUInt64( 0 ); // automatic base detection for 0x prefix
            case 'b':
            case 'B':
               return value.Substring( 2 ).ToUInt64( 2 ); // XISF binary prefix 0b
            case 'o':
            case 'O':
               return value.Substring( 2 ).ToUInt64( 8 ); // XISF octal prefix 0o
            default:
               break;
            }

      return value.ToUInt64( 10 ); // XISF default decimal representation
   }

   /*
    * Deserialize an XISF property.
    */
   void GetProperty( XISFInputPropertyBlockArray& properties, const XMLElement& element, bool isInternal )
   {
      XISFInputPropertyBlock property;

      property.id = element.AttributeValue( "id" );
      if ( property.id.IsEmpty() )
         HeaderError( element, "Missing property id attribute." );
      if ( !XISF::IsValidPropertyId( property.id ) )
         HeaderError( element, "Invalid XISF property identifier '" + property.id + "'" );
      if ( XISF::IsInternalPropertyId( property.id ) != isInternal )
         if ( isInternal )
            HeaderError( element, "Expected a reserved property in the 'XISF:' namespace: '" + property.id + "'" );
         else
            HeaderError( element, "Use of the 'XISF:' reserved namespace is forbidden here: '" + property.id + "'" );

      String type = element.AttributeValue( "type" );
      if ( type.IsEmpty() )
         HeaderError( element, "Missing property type attribute." );
      property.type = XISF::PropertyTypeFromId( type );
      if ( property.type == VariantType::Invalid )
         HeaderError( element, "Invalid/unsupported property type '" + type + "'" );

      if ( Variant::IsScalarType( property.type ) )
      {
         String value = element.AttributeValue( "value" );
         if ( value.IsEmpty() )
            HeaderError( element, "Missing value attribute for scalar property '" + property.id + "'" );
         switch ( property.type )
         {
         case VariantType::Boolean:
            property.value = Variant( value.ToBool() );
            break;
         case VariantType::Int8:
            property.value = Variant( int8( value.ToInt() ) );
            break;
         case VariantType::Int16:
            property.value = Variant( int16( value.ToInt() ) );
            break;
         case VariantType::Int32:
            property.value = Variant( int32( value.ToInt() ) );
            break;
         case VariantType::Int64:
            property.value = Variant( int64( value.ToInt64() ) );
            break;
         case VariantType::UInt8:
            property.value = Variant( uint8( ParseUInt( value ) ) );
            break;
         case VariantType::UInt16:
            property.value = Variant( uint16( ParseUInt( value ) ) );
            break;
         case VariantType::UInt32:
            property.value = Variant( uint32( ParseUInt( value ) ) );
            break;
         case VariantType::UInt64:
            property.value = Variant( uint64( ParseUInt64( value ) ) );
            break;
         case VariantType::Float32:
            property.value = Variant( value.ToFloat() );
            break;
         case VariantType::Float64:
            property.value = Variant( value.ToDouble() );
            break;
         default:
            HeaderError( element, "Internal error: Invalid scalar property type '" + type + "'" );
         }
      }
      else
      {
         if ( Variant::IsStringType( property.type ) )
         {
            if ( element.HasAttribute( "location" ) )
            {
               // An embedded or attached UTF-8 string
               property.type = VariantType::IsoString;
               GetBlock( property, element, true/*getEmbeddedData*/ );
            }
            else
            {
               // An inline string already converted from UTF-8 to UTF-16
               property.type = VariantType::String;
               property.value = Variant( element.Text() );
            }
         }
         else if ( Variant::IsVectorType( property.type ) )
         {
            String length = element.AttributeValue( "length" );
            if ( length.IsEmpty() )
               HeaderError( element, "Missing vector length attribute." );
            int n = length.ToInt();
            if ( n < 0 )
               HeaderError( element, "Invalid vector length: " + length );
            property.dimensions.Append( n );

            property.itemSize = Variant::BytesPerBlockElementForType( property.type );

            GetBlock( property, element, true/*getEmbeddedData*/ );
         }
         else if ( Variant::IsMatrixType( property.type ) )
         {
            String rows = element.AttributeValue( "rows" );
            if ( rows.IsEmpty() )
               HeaderError( element, "Missing matrix rows attribute." );
            int n = rows.ToInt();
            if ( n <= 0 )
               HeaderError( element, "Invalid matrix dimension: " + rows );
            property.dimensions.Append( n );

            String columns = element.AttributeValue( "columns" );
            if ( columns.IsEmpty() )
               HeaderError( element, "Missing matrix columns attribute." );
            n = columns.ToInt();
            if ( n <= 0 )
               HeaderError( element, "Invalid matrix dimension: " + columns );
            property.dimensions.Append( n );

            property.itemSize = Variant::BytesPerBlockElementForType( property.type );

            GetBlock( property, element, true/*getEmbeddedData*/ );
         }
         else if ( Variant::IsComplexType( property.type ) )
         {
            String value = element.AttributeValue( "value" );
            if ( value.IsEmpty() )
               HeaderError( element, "Missing value attribute for complex property '" + property.id + "'" );
            if ( !value.StartsWith( '(' ) || !value.EndsWith( ')' ) )
               HeaderError( element, "Invalid complex literal: '" + value + "'" );
            value.Delete( value.Length()-1 ); // remove closing ')'
            StringList c;
            value.Break( c, ',', true/*trim*/, 1 ); // skip opening '(', white space allowed
            if ( c.Length() != 2 )
               HeaderError( element, "Malformed complex literal: '" + value + "'" );
            switch ( property.type )
            {
            case VariantType::Complex32:
               property.value = Variant( Complex32( c[0].ToFloat(), c[1].ToFloat() ) );
               break;
            case VariantType::Complex64:
               property.value = Variant( Complex64( c[0].ToDouble(), c[1].ToDouble() ) );
               break;
            default:
               HeaderError( element, "Internal error: Invalid complex property type '" + type + "'" );
            }
         }
         else if ( property.type == VariantType::TimePoint )
         {
            String value = element.AttributeValue( "value" );
            if ( value.IsEmpty() )
               HeaderError( element, "Missing value attribute for TimePoint property '" + property.id + "'" );
            property.value = Variant( TimePoint( value ) );
         }
         else
            HeaderError( element, "Internal error: Invalid property type '" + type + "'" );
      }

      AddPropertyBlock( properties, property, element );
   }

   void GetFITSKeyword( XISFInputPropertyBlockArray& properties, FITSKeywordArray& keywords, const XMLElement& element )
   {
      String name = element.AttributeValue( "name" );
      String value = element.AttributeValue( "value" );
      String comment = element.AttributeValue( "comment" );
      FITSHeaderKeyword keyword( name, value, comment );
      keyword.FixValueDelimiters();
      keywords.Append( keyword );
      if ( m_xisfOptions.importFITSKeywords )
         GetPropertyFromFITSKeyword( properties, keyword, element );
   }

   void GetPropertyFromFITSKeyword( XISFInputPropertyBlockArray& properties, const FITSHeaderKeyword& keyword, const XMLElement& element )
   {
      IsoString kname = keyword.name.Trimmed().Uppercase(); // just in case!
      if ( kname.IsEmpty() )
         return;

      /*
       * Do not import irrelevant or mechanic FITS keywords.
       * This includes some of our reserved keywords, present and legacy ones.
       */
      if ( kname          == "SIMPLE"    ||
           kname          == "BITPIX"    ||
           kname.StartsWith( "NAXIS" )   ||
           kname          == "EXTEND"    ||
           kname          == "NEXTEND"   ||
           kname          == "BSCALE"    ||
           kname          == "BZERO"     ||
           kname          == "PROGRAM"   ||
           kname          == "CREATOR"   ||
           kname          == "CONFIGUR"  ||
           kname          == "XTENSION"  ||
           kname          == "PCOUNT"    ||
           kname          == "GCOUNT"    ||
           kname          == "FILENAME"  ||
           kname          == "FILETYPE"  ||
           kname          == "ROOTNAME"  ||
           kname          == "HDUNAME"   ||
           kname          == "EXTNAME"   ||
           kname.StartsWith( "TLMIN" )   ||
           kname.StartsWith( "TLMAX" )   ||
           kname.StartsWith( "TDMIN" )   ||
           kname.StartsWith( "TDMAX" )   ||
           kname.StartsWith( "TDBIN" )   ||
           kname          == "PINSIGHT"  ||
           kname          == "COLORSPC"  ||
           kname          == "ALPHACHN"  ||
           kname          == "RESOLUTN"  ||
           kname          == "XRESOLTN"  ||
           kname          == "YRESOLTN"  ||
           kname          == "RESOUNIT"  ||
           kname          == "ICCPROFL"  ||
           kname          == "THUMBIMG"  ||
           kname          == "XMPDATA" ) return;

      XISFInputPropertyBlock property;

      property.id = "FITS:";
      if ( !IsoCharTraits::IsStartingSymbolDigit( kname[0] ) )
         property.id << '_';
      for ( char c : kname )
         property.id << (IsoCharTraits::IsSymbolDigit( c ) ? c : '_');

      /*
       * HISTORY and COMMENT keywords are special in that they don't have a
       * value. The whole contents is in their comments.
       */
      if ( kname == "HISTORY" )
      {
         property.id.AppendFormat( "%03d", ++m_historyKeywordCount );
         property.type = VariantType::IsoString;
         property.value = keyword.comment;
      }
      else if ( kname == "COMMENT" )
      {
         property.id.AppendFormat( "%03d", ++m_commentKeywordCount );
         property.type = VariantType::IsoString;
         property.value = keyword.comment;
      }
      else
      {
         if ( keyword.IsNull() ) // empty value? we don't want these!
            return;

         if ( keyword.IsString() )
         {
            property.type = VariantType::IsoString;
            property.value = keyword.StripValueDelimiters();
         }
         else if ( keyword.IsBoolean() )
         {
            property.type = VariantType::Boolean;
            property.value = keyword.value == 'T';
         }
         else if ( keyword.IsNumeric() )
         {
            double value;
            if ( keyword.GetNumericValue( value ) )
            {
               property.type = VariantType::Double;
               property.value = value;
            }
            else  // !? else assume a string 'thing', probably wrong...
            {
               property.type = VariantType::IsoString;
               property.value = keyword.value;
            }
         }
         else  // !? else assume a string 'value', without quotes...
         {
            property.type = VariantType::IsoString;
            property.value = keyword.value;
         }
      }

      AddPropertyBlock( properties, property, element );
   }

   void GetImageGeometry( XISFInputImageBlock& image, const XMLElement& element )
   {
      String geometry = element.AttributeValue( "geometry" );
      if ( geometry.IsEmpty() )
         HeaderError( element, "Missing image geometry attribute." );

      // geometry="<width>:<height>:<channel-count>"
      StringList tokens;
      geometry.Break( tokens, ':', true/*trim*/ );
      if ( tokens.Length() < 2 )
         HeaderError( element, "Insufficient image geometry parameters: '" + geometry + "'" );
      if ( tokens.Length() < 3 )
         HeaderError( element, "One-dimensional images are not supported by this XISF implementation: '" + geometry + "'" );
      if ( tokens.Length() > 3 )
         HeaderError( element, "This XISF implementation only supports two-dimensional images: '" + geometry + "'" );
      image.info.width = tokens[0].ToInt();
      image.info.height = tokens[1].ToInt();
      image.info.numberOfChannels = tokens[2].ToInt();

      if ( image.info.width < 1 )
         HeaderError( element, "Invalid image width: " + String( image.info.width ) );
      if ( image.info.height < 1 )
         HeaderError( element, "Invalid image height: " + String( image.info.height ) );
      if ( image.info.numberOfChannels < 1 )
         HeaderError( element, "Invalid number of image channels: " + String( image.info.numberOfChannels ) );
   }

   void GetRGBWS( RGBColorSystem& rgbws, const XMLElement& element )
   {
      float gamma, x[ 3 ], y[ 3 ], Y[ 3 ];
      bool sRGB;
      String sgamma = element.AttributeValue( "gamma" );
      String sxr    = element.AttributeValue( "xr" );
      String sxg    = element.AttributeValue( "xg" );
      String sxb    = element.AttributeValue( "xb" );
      String syr    = element.AttributeValue( "yr" );
      String syg    = element.AttributeValue( "yg" );
      String syb    = element.AttributeValue( "yb" );
      String sYr    = element.AttributeValue( "Yr" );
      String sYg    = element.AttributeValue( "Yg" );
      String sYb    = element.AttributeValue( "Yb" );

      if ( sgamma.IsEmpty() ||
              sxr.IsEmpty() || sxg.IsEmpty() || sxb.IsEmpty() ||
              syr.IsEmpty() || syg.IsEmpty() || syb.IsEmpty() ||
              sYr.IsEmpty() || sYg.IsEmpty() || sYb.IsEmpty() )
         HeaderError( element, "Missing required RGBWS parameter(s)." );

      if ( sgamma.CaseFolded() == "srgb" )
      {
         gamma = 2.2F;
         sRGB = true;
      }
      else
      {
         gamma = sgamma.ToFloat();
         sRGB = false;
      }
      x[0] = sxr.ToFloat();
      x[1] = sxg.ToFloat();
      x[2] = sxb.ToFloat();
      y[0] = syr.ToFloat();
      y[1] = syg.ToFloat();
      y[2] = syb.ToFloat();
      Y[0] = sYr.ToFloat();
      Y[1] = sYg.ToFloat();
      Y[2] = sYb.ToFloat();

      rgbws = RGBColorSystem( gamma, sRGB, x, y, Y );
   }

   void ParseDisplayFunctionParameters( DVector& v, const char* attributeName, const XMLElement& element, double vmin, double vmax )
   {
      String value = element.AttributeValue( attributeName );
      if ( value.IsEmpty() )
         HeaderError( element, "Missing DisplayFunction '" + String( attributeName ) + "' attribute." );
      StringList items;
      value.Break( items, ':', true/*trim*/ );
      if ( items.Length() != 4 )
         HeaderError( element, "Malformed DisplayFunction '" + String( attributeName ) + "' attribute." );
      v = DVector( 4 );
      for ( int i = 0; i < 4; ++i )
      {
         v[i] = items[i].ToDouble();
         if ( v[i] < vmin || v[i] > vmax )
            HeaderError( element, "Invalid DisplayFunction '" + String( attributeName ) + "' attribute component(value)." );
      }
   }

   void GetDisplayFunction( pcl::DisplayFunction& df, const XMLElement& element )
   {
      DVector m, s, h, l, r;
      ParseDisplayFunctionParameters( m, "m", element, 0, 1 );
      ParseDisplayFunctionParameters( s, "s", element, 0, 1 );
      ParseDisplayFunctionParameters( h, "h", element, 0, 1 );
      ParseDisplayFunctionParameters( l, "l", element, int_min, 0 );
      ParseDisplayFunctionParameters( r, "r", element, 1, int_max );
      df = pcl::DisplayFunction( m, s, h, l, r );
   }

   void GetColorFilterArray( pcl::ColorFilterArray& cfa, const XMLElement& element )
   {
      IsoString pattern( element.AttributeValue( "pattern" ) );
      if ( pattern.IsEmpty() )
         HeaderError( element, "Missing pattern ColorFilterArray attribute." );

      String value = element.AttributeValue( "width" );
      if ( value.IsEmpty() )
         HeaderError( element, "Missing width ColorFilterArray attribute." );
      int width = value.ToInt();

      value = element.AttributeValue( "height" );
      if ( value.IsEmpty() )
         HeaderError( element, "Missing height ColorFilterArray attribute." );
      int height = value.ToInt();

      cfa = pcl::ColorFilterArray( pattern, width, height, element.AttributeValue( "name" ) );
   }

   void GetImageResolution( pcl::ImageOptions& options, const XMLElement& element )
   {
      String value = element.AttributeValue( "horizontal" );
      if ( value.IsEmpty() )
         HeaderError( element, "Missing horizontal Resolution attribute." );
      options.xResolution = value.ToDouble();
      if ( options.xResolution <= 0 || !IsFinite( options.xResolution ) )
         HeaderError( element, "Invalid horizontal resolution: " + value );

      value = element.AttributeValue( "vertical" );
      if ( value.IsEmpty() )
         HeaderError( element, "Missing vertical Resolution attribute." );
      options.yResolution = value.ToDouble();
      if ( options.yResolution <= 0 || !IsFinite( options.yResolution ) )
         HeaderError( element, "Invalid vertical resolution: " + value );

      value = element.AttributeValue( "unit" ).CaseFolded(); // resolution unit specifiers are case-insensitive
      if ( !value.IsEmpty() )
         if ( value == "cm" || value == "metric" )
            options.metricResolution = true;
         else if ( value == "inch" || value == "english" )
            options.metricResolution = false;
         else
            HeaderError( element, "Unknown/unsupported resolution unit '" + value + "'" );
   }

   void GetICCProfile( XISFInputDataBlock& iccProfile, const XMLElement& element )
   {
      if ( element.HasAttribute( "byteOrder" ) )
         HeaderError( element, "The byteOrder attribute is forbidden for ICCProfile core elements." );
      GetBlock( iccProfile, element, true/*getEmbeddedData*/ );
      if ( iccProfile.IsEmpty() )
         HeaderError( element, "Missing ICC profile data." );
   }

   void GetThumbnail( XISFInputImageBlock& thumbnail, const XMLElement& element )
   {
      GetImageGeometry( thumbnail, element );

      thumbnail.itemSize = 1;
      String value = element.AttributeValue( "sampleFormat" ).CaseFolded();
      if ( !value.IsEmpty() )
         if ( value != "uint8" )
         {
            if ( value != "uint16" )
               HeaderError( element, "Invalid/unsupported Thumbnail sampleFormat attribute: Must be either UInt8 or UInt16." );
            thumbnail.itemSize = 2;
         }

      value = element.AttributeValue( "colorSpace" );
      if ( value.IsEmpty() )
         thumbnail.info.colorSpace = ColorSpace::Gray;
      else
      {
         thumbnail.info.colorSpace = XISF::ColorSpaceFromId( value );
         if ( thumbnail.info.colorSpace == ColorSpace::Unknown )
            HeaderError( element, "Invalid/unknown thumbnail color space '" + value + "'" );
         if ( thumbnail.info.colorSpace != ColorSpace::RGB && thumbnail.info.colorSpace != ColorSpace::Gray )
            HeaderError( element, "Unsupported thumbnail color space '" + value + "'. Must be either RGB or grayscale." );
      }

      value = element.AttributeValue( "pixelStorage" ).CaseFolded();
      if ( !value.IsEmpty() )
      {
         if ( value == "normal" )
            thumbnail.normal = true;
         else if ( value != "planar" )
            HeaderError( element, "Unknown/unsupported pixel storage model \'" + value + "\'" );
      }

      GetBlock( thumbnail, element, true/*getEmbeddedData*/ );

      if ( thumbnail.info.width > XISF::MaxThumbnailSize || thumbnail.info.height > XISF::MaxThumbnailSize )
      {
         HeaderWarning( element, String().Format( "Ignoring too big image thumbnail (%dx%d)", thumbnail.info.width, thumbnail.info.height ) );
         thumbnail.data.Clear();
         thumbnail.position = thumbnail.size = 0;
         thumbnail.itemSize = 1;
         thumbnail.info = ImageInfo();
      }
   }

   /*!
    * Generate a descriptive list for a set of XISF properties.
    */
   PropertyDescriptionArray MakePropertyDescriptions( const XISFInputPropertyBlockArray& properties ) const
   {
      PropertyDescriptionArray descriptions;
      for ( const XISFInputPropertyBlock& property : properties )
         descriptions << PropertyDescription( property.id, Variant::data_type( property.type ) );
      return descriptions;
   }

   /*!
    * Retrieve the value of an XISF property in a given set.
    */
   Variant LoadPropertyValue( XISFInputPropertyBlockArray& properties, const IsoString& id )
   {
      XISFInputPropertyBlockArray::const_iterator p = properties.Search( id );
      if ( p == properties.End() )
         return Variant();

      XISFInputPropertyBlockArray::iterator i = properties.MutableIterator( p );

      if ( !i->HasValue() )
      {
         if ( Variant::IsScalarType( i->type ) || Variant::IsComplexType( i->type ) )
            throw Error( String( "XISFReaderEngine::LoadPropertyValue(): " ) + "Internal error: Invalid or corrupted property '" + i->id + "'" );

         if ( i->type == VariantType::IsoString ) // UTF-8 string
         {
            i->value = Variant( IsoString( '\0', i->DataSize() ) );
         }
         else if ( Variant::IsVectorType( i->type ) )
         {
            if ( i->dimensions.Length() != 1 || i->dimensions[0] < 0 )
               throw Error( String( "XISFReaderEngine::LoadPropertyValue(): " ) + "Internal error: Invalid vector length for property '" + i->id + "'" );

            int length = i->dimensions[0];
            switch ( i->type )
            {
            case VariantType::I8Vector:
               i->value = Variant( I8Vector( length ) );
               break;
            case VariantType::UI8Vector:
               i->value = Variant( UI8Vector( length ) );
               break;
            case VariantType::I16Vector:
               i->value = Variant( I16Vector( length ) );
               break;
            case VariantType::UI16Vector:
               i->value = Variant( UI16Vector( length ) );
               break;
            case VariantType::I32Vector:
               i->value = Variant( I32Vector( length ) );
               break;
            case VariantType::UI32Vector:
               i->value = Variant( UI32Vector( length ) );
               break;
            case VariantType::I64Vector:
               i->value = Variant( I64Vector( length ) );
               break;
            case VariantType::UI64Vector:
               i->value = Variant( UI64Vector( length ) );
               break;
            case VariantType::F32Vector:
               i->value = Variant( F32Vector( length ) );
               break;
            case VariantType::F64Vector:
               i->value = Variant( F64Vector( length ) );
               break;
            case VariantType::C32Vector:
               i->value = Variant( C32Vector( length ) );
               break;
            case VariantType::C64Vector:
               i->value = Variant( C64Vector( length ) );
               break;
            default:
               throw Error( String( "XISFReaderEngine::LoadPropertyValue(): " ) + "Internal error: Invalid vector data type for property '" + i->id + "'" );
            }
         }
         else if ( Variant::IsMatrixType( i->type ) )
         {
            if ( i->dimensions.Length() != 2 || i->dimensions[0] < 0 || i->dimensions[1] < 0 )
               throw Error( String( "XISFReaderEngine::LoadPropertyValue(): " ) + "Internal error: Invalid matrix dimensions for property '" + i->id + "'" );

            int rows = i->dimensions[0];
            int cols = i->dimensions[1];
            switch ( i->type )
            {
            case VariantType::I8Matrix:
               i->value = Variant( I8Matrix( rows, cols ) );
               break;
            case VariantType::UI8Matrix:
               i->value = Variant( UI8Matrix( rows, cols ) );
               break;
            case VariantType::I16Matrix:
               i->value = Variant( I16Matrix( rows, cols ) );
               break;
            case VariantType::UI16Matrix:
               i->value = Variant( UI16Matrix( rows, cols ) );
               break;
            case VariantType::I32Matrix:
               i->value = Variant( I32Matrix( rows, cols ) );
               break;
            case VariantType::UI32Matrix:
               i->value = Variant( UI32Matrix( rows, cols ) );
               break;
            case VariantType::I64Matrix:
               i->value = Variant( I64Matrix( rows, cols ) );
               break;
            case VariantType::UI64Matrix:
               i->value = Variant( UI64Matrix( rows, cols ) );
               break;
            case VariantType::F32Matrix:
               i->value = Variant( F32Matrix( rows, cols ) );
               break;
            case VariantType::F64Matrix:
               i->value = Variant( F64Matrix( rows, cols ) );
               break;
            case VariantType::C32Matrix:
               i->value = Variant( C32Matrix( rows, cols ) );
               break;
            case VariantType::C64Matrix:
               i->value = Variant( C64Matrix( rows, cols ) );
               break;
            default:
               throw Error( String( "XISFReaderEngine::LoadPropertyValue(): " ) + "Internal error: Invalid matrix data type for property '" + i->id + "'" );
            }
         }
         else
            throw Error( String( "XISFReaderEngine::LoadPropertyValue(): " ) + "Internal error: Invalid data type for property '" + i->id + "'" );

         if ( i->DataSize() != i->value.BlockSize() )
            throw Error( String( "XISFReaderEngine::LoadPropertyValue(): " ) + "Internal error: Inconsistent block size for property '" + i->id + "'" );

         if ( i->value.BlockSize() > 0 )
         {
            GetBlockData( *i, const_cast<void*>( i->value.InternalBlockAddress() ), i->value.BlockSize() );
            i->UnloadData();
         }

         if ( i->type == VariantType::IsoString )
         {
            i->type = VariantType::String; // UTF-8 string converted to UTF-16
            i->value = Variant( String( i->value.ToIsoString().UTF8ToUTF16() ) );
         }
      }

      if ( m_xisfOptions.verbosity > 1 )
         LogLn( "Property '" + i->id + "' (" + XISF::PropertyTypeId( i->type ) + ") extracted." );

      return i->value;
   }

   /*
    * Sequential/random read of a block of contiguous rows of pixel samples in
    * the native sample data type of the current image.
    */
   template <class P>
   void ReadSamples( typename P::sample* buffer, int startRow, int rowCount, int channel, P*, P* )
   {
      XISFInputImageBlock& block = m_images[m_currentImage].image;

      size_type count = BlockSampleCount( rowCount );

      GetBlockData( block, buffer, count*sizeof( *buffer ), BlockSampleOffset( startRow, channel ) );

      if ( m_images[m_currentImage].options.readNormalized )
         NormalizeSamples( buffer, count, m_images[m_currentImage].options );
   }

   /*
    * Sequential/random read of a block of contiguous rows of pixel samples in
    * a foreign sample data type.
    */
   template <class O, class P>
   void ReadSamples( typename O::sample* buffer, int startRow, int rowCount, int channel, O*, P* )
   {
      XISFInputImageBlock& block = m_images[m_currentImage].image;

      Array<typename P::sample> data( BlockSampleCount( rowCount ) );

      GetBlockData( block, data.Begin(), data.Size(), BlockSampleOffset( startRow, channel ) );

      if ( m_images[m_currentImage].options.readNormalized )
         NormalizeSamples( data.Begin(), data.Length(), m_images[m_currentImage].options );

      XISF::EnsurePTLUTInitialized();
      for ( const typename P::sample& sample : data )
         P::FromSample( *buffer++, sample );
   }

   /*
    * Normalization of floating point and integer real pixel samples.
    */

#define NORMALIZE_FLOAT_IMAGE( I )                                                  \
      /*                                                                            \
       * Replace NaNs and infinities with minimum values.                           \
       * Truncate out-of-range sample values.                                       \
       */                                                                           \
      for ( int c = 0; c < image.NumberOfChannels(); ++c )                          \
         for ( I::sample_iterator i( image, c ); i; ++i )                           \
            if ( !IsFinite( *i ) || *i < options.lowerRange )                       \
               *i = options.lowerRange;                                             \
            else if ( *i > options.upperRange )                                     \
               *i = options.upperRange;                                             \
                                                                                    \
      /*                                                                            \
       * Normalize to [0,1]                                                         \
       */                                                                           \
      if ( options.lowerRange != 0 || options.upperRange != 1 )                     \
      {                                                                             \
         double range = options.upperRange - options.lowerRange;                    \
         if ( 1 + range != 1 )                                                      \
         {                                                                          \
            for ( int c = 0; c < image.NumberOfChannels(); ++c )                    \
               for ( I::sample_iterator i( image, c ); i; ++i )                     \
                  *i = (*i - options.lowerRange)/range;                             \
         }                                                                          \
         else if ( options.lowerRange < 0 || options.lowerRange > 1 )               \
         {                                                                          \
            double f = Range( options.lowerRange, 0.0, 1.0 );                       \
            for ( int c = 0; c < image.NumberOfChannels(); ++c )                    \
               for ( I::sample_iterator i( image, c ); i; ++i )                     \
                  *i = f;                                                           \
         }                                                                          \
      }

#define NORMALIZE_INTEGER_IMAGE( I )                                                \
      /*                                                                            \
       * Truncate out-of-range sample values                                        \
       */                                                                           \
      I::sample r0 = I::pixel_traits::FloatToSample( Ceil( options.lowerRange ) );  \
      I::sample r1 = I::pixel_traits::FloatToSample( Floor( options.upperRange ) ); \
      for ( int c = 0; c < image.NumberOfChannels(); ++c )                          \
         for ( I::sample_iterator i( image, c ); i; ++i )                           \
            if ( *i < r0 )                                                          \
               *i = r0;                                                             \
            else if ( *i > r1 )                                                     \
               *i = r1;                                                             \
                                                                                    \
      /*                                                                            \
       * Normalize to [0,I::pixel_traits::MaxSampleValue()]                         \
       */                                                                           \
      if ( options.lowerRange > 0 ||                                                \
           options.upperRange < I::pixel_traits::MaxSampleValue() )                 \
      {                                                                             \
         double range = options.upperRange - options.lowerRange;                    \
         if ( 1 + range != 1 )                                                      \
         {                                                                          \
            double scale = I::pixel_traits::MaxSampleValue()/range;                 \
            for ( int c = 0; c < image.NumberOfChannels(); ++c )                    \
               for ( I::sample_iterator i( image, c ); i; ++i )                     \
                  *i = (*i - options.lowerRange)*scale;                             \
         }                                                                          \
      }

   void NormalizeImage( Image& image, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_FLOAT_IMAGE( Image )
   }

   void NormalizeImage( DImage& image, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_FLOAT_IMAGE( DImage )
   }

   void NormalizeImage( ComplexImage& image, const pcl::ImageOptions& options ) const
   {
      // Representable ranges are undefined for complex images as per XISF 1.0
   }

   void NormalizeImage( DComplexImage& image, const pcl::ImageOptions& options ) const
   {
      // Representable ranges are undefined for complex images as per XISF 1.0
   }

   void NormalizeImage( UInt8Image& image, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_INTEGER_IMAGE( UInt8Image )
   }

   void NormalizeImage( UInt16Image& image, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_INTEGER_IMAGE( UInt16Image )
   }

   void NormalizeImage( UInt32Image& image, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_INTEGER_IMAGE( UInt32Image )
   }

#define NORMALIZE_FLOAT_SAMPLES( P )                                                \
      /*                                                                            \
       * Replace NaNs and infinities with minimum values.                           \
       * Truncate out-of-range sample values                                        \
       */                                                                           \
      for ( P::sample* i = buffer, * j = i + count; i < j; ++i )                    \
         if ( !IsFinite( *i ) || *i < options.lowerRange )                          \
            *i = options.lowerRange;                                                \
         else if ( *i > options.upperRange )                                        \
            *i = options.upperRange;                                                \
                                                                                    \
      /*                                                                            \
       * Normalize to [0,1]                                                         \
       */                                                                           \
      if ( options.lowerRange != 0 || options.upperRange != 1 )                     \
      {                                                                             \
         double range = options.upperRange - options.lowerRange;                    \
         if ( 1 + range != 1 )                                                      \
         {                                                                          \
            for ( P::sample* i = buffer, * j = i + count; i < j; ++i )              \
               *i = (*i - options.lowerRange)/range;                                \
         }                                                                          \
         else if ( options.lowerRange < 0 || options.lowerRange > 1 )               \
         {                                                                          \
            double f = Range( options.lowerRange, 0.0, 1.0 );                       \
            for ( P::sample* i = buffer, * j = i + count; i < j; ++i )              \
               *i = f;                                                              \
         }                                                                          \
      }

#define NORMALIZE_INTEGER_SAMPLES( P )                                              \
      /*                                                                            \
       * Truncate out-of-range sample values                                        \
       */                                                                           \
      P::sample r0 = P::FloatToSample( Ceil( options.lowerRange ) );                \
      P::sample r1 = P::FloatToSample( Floor( options.upperRange ) );               \
      for ( P::sample* i = buffer, * j = i + count; i < j; ++i )                    \
         if ( *i < r0 )                                                             \
            *i = r0;                                                                \
         else if ( *i > r1 )                                                        \
            *i = r1;                                                                \
                                                                                    \
      /*                                                                            \
       * Normalize to [0,P::MaxSampleValue()]                                       \
       */                                                                           \
      if ( options.lowerRange > 0 || options.upperRange < P::MaxSampleValue() )     \
      {                                                                             \
         double range = options.upperRange - options.lowerRange;                    \
         if ( 1 + range != 1 )                                                      \
         {                                                                          \
            double scale = P::MaxSampleValue()/range;                               \
            for ( P::sample* i = buffer, * j = i + count; i < j; ++i )              \
               *i = (*i - options.lowerRange)*scale;                                \
         }                                                                          \
      }

   void NormalizeSamples( FloatPixelTraits::sample* buffer, size_type count, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_FLOAT_SAMPLES( FloatPixelTraits )
   }

   void NormalizeSamples( DoublePixelTraits::sample* buffer, size_type count, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_FLOAT_SAMPLES( DoublePixelTraits )
   }

   void NormalizeSamples( ComplexPixelTraits::sample* buffer, size_type count, const pcl::ImageOptions& options ) const
   {
      // Representable ranges are undefined for complex images as per XISF 1.0
   }

   void NormalizeSamples( DComplexPixelTraits::sample* buffer, size_type count, const pcl::ImageOptions& options ) const
   {
      // Representable ranges are undefined for complex images as per XISF 1.0
   }

   void NormalizeSamples( UInt8PixelTraits::sample* buffer, size_type count, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_INTEGER_SAMPLES( UInt8PixelTraits )
   }

   void NormalizeSamples( UInt16PixelTraits::sample* buffer, size_type count, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_INTEGER_SAMPLES( UInt16PixelTraits )
   }

   void NormalizeSamples( UInt32PixelTraits::sample* buffer, size_type count, const pcl::ImageOptions& options ) const
   {
      NORMALIZE_INTEGER_SAMPLES( UInt32PixelTraits )
   }

   /*
    * The absolute file position in bytes of the first pixel sample in the
    * specified row of a channel of the current image.
    */
   size_type BlockSampleOffset( size_type startRow, size_type channel = 0 ) const
   {
      const pcl::ImageOptions& options = m_images[m_currentImage].options;
      const XISFInputImageBlock& block = m_images[m_currentImage].image;
      return size_type( options.bitsPerSample >> 3 ) * (channel * BlockSampleCount( block.info.height ) + BlockSampleCount( startRow ));
   }

   /*
    * Total number of samples in a block of contiguous pixel rows in the
    * current image.
    */
   size_type BlockSampleCount( size_type rowCount ) const
   {
      const XISFInputImageBlock& block = m_images[m_currentImage].image;
      return rowCount * size_type( block.info.width );
   }

   /*
    * Size in bytes of a block of contiguous rows of pixel samples in the
    * current image.
    */
   size_type BlockSampleSize( size_type rowCount ) const
   {
      const pcl::ImageOptions& options = m_images[m_currentImage].options;
      return size_type( options.bitsPerSample >> 3 ) * BlockSampleCount( rowCount );
   }

   /*
    * Reset the state of the engine and destroy all internal data structures.
    * This includes closing the input file if it has been opened.
    */
   void Reset()
   {
      m_path.Clear();
      m_file.Close();
      m_fileSize = 0;
      m_headerLength = 0;
      m_minBlockPos = 0;
      m_images.Clear();
      m_currentImage = 0;
      m_properties.Clear();
      m_historyKeywordCount = 0;
      m_commentKeywordCount = 0;
   }

   /*
    * Log a new warning message during the XML header deserialization phase.
    */
   void HeaderWarning( const XMLNode& node, const String& text )
   {
      if ( m_xisfOptions.noWarnings )
         return;
      String warning = text + node.Location().ToString();
      if ( m_xisfOptions.warningsAreErrors )
         throw Error( warning );
      LogLn( warning, XISFMessageType::Warning );
   }

   /*
    * Throw an Error exception during the XML header deserialization phase.
    */
   void HeaderError( const XMLNode& node, const String& text )
   {
      throw pcl::Error( text + node.Location().ToString() );
   }

   /*
    * Ensure that the input file has been opened and contains at least one
    * image before trying to read an image.
    */
   void ValidateImageAccess( int index ) const
   {
      if ( index < 0 || index >= NumberOfImages() )
         throw Error( "XISFReaderEngine: " + String().Format( "Attempt to access nonexistent image at index %d.", index ) );
   }

   /*
    * Send log messages to the log handler, if we have one.
    */
   void Log( const String& text, XISFLogHandler::message_type type = XISFMessageType::Informative ) const
   {
      if ( m_logHandler != nullptr )
         m_logHandler->Log( text, type );
   }
   void LogLn( const String& text, XISFLogHandler::message_type type = XISFMessageType::Informative ) const
   {
      if ( m_logHandler != nullptr )
         m_logHandler->Log( text + '\n', type );
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * XISFReader Implementation
 */

XISFReader::XISFReader()
{
   // N.B.: This cannot be inlined in the header file, since XISFReaderEngine
   // is unknown to the public.
}

// ----------------------------------------------------------------------------

XISFReader::~XISFReader() noexcept( false )
{
   Close();
   // N.B.: The above comment applies.
}

// ----------------------------------------------------------------------------

void XISFReader::SetOptions( const pcl::XISFOptions& options )
{
   m_options = options;
   if ( IsOpen() )
      m_engine->SetOptions( m_options );
}

// ----------------------------------------------------------------------------

void XISFReader::SetHints( const IsoString& hints )
{
   m_hints = hints.Trimmed();
   if ( IsOpen() )
      m_engine->SetHints( m_hints );
}

// ----------------------------------------------------------------------------

void XISFReader::SetLogHandler( XISFLogHandler* handler )
{
   m_logHandler = handler;
   if ( IsOpen() )
      m_engine->SetLogHandler( m_logHandler );
}

// ----------------------------------------------------------------------------

bool XISFReader::IsOpen() const
{
   return m_engine;
}

// ----------------------------------------------------------------------------

void XISFReader::Open( const String& path )
{
   CheckClosedStream( "Open" );
   m_engine = new XISFReaderEngine;
   m_engine->SetOptions( m_options );
   m_engine->SetHints( m_hints );
   m_engine->SetLogHandler( m_logHandler );
   m_engine->Open( path );
}

// ----------------------------------------------------------------------------

void XISFReader::Close()
{
   if ( IsOpen() )
   {
      m_engine->Close();
      m_engine.Destroy();
   }
}

// ----------------------------------------------------------------------------

String XISFReader::FilePath() const
{
   CheckOpenStream( "FilePath" );
   return m_engine->FilePath();
}

// ----------------------------------------------------------------------------

int XISFReader::NumberOfImages() const
{
   CheckOpenStream( "NumberOfImages" );
   return m_engine->NumberOfImages();
}

// ----------------------------------------------------------------------------

void XISFReader::SelectImage( int index )
{
   CheckOpenStream( "SelectImage" );
   m_engine->SelectImage( index );
}

// ----------------------------------------------------------------------------

int XISFReader::SelectedImageIndex() const
{
   return IsOpen() ? m_engine->SelectedImageIndex() : -1;
}

// ----------------------------------------------------------------------------

pcl::ImageInfo XISFReader::ImageInfo() const
{
   CheckOpenStream( "ImageInfo" );
   return m_engine->ImageInfo();
}

// ----------------------------------------------------------------------------

pcl::ImageOptions XISFReader::ImageOptions() const
{
   CheckOpenStream( "ImageOptions" );
   return m_engine->ImageOptions();
}

// ----------------------------------------------------------------------------

void XISFReader::SetImageOptions( const pcl::ImageOptions& options )
{
   CheckOpenStream( "SetImageOptions" );
   m_engine->SetImageOptions( options );
}

// ----------------------------------------------------------------------------

IsoString XISFReader::ImageId() const
{
   CheckOpenStream( "ImageId" );
   return m_engine->ImageId();
}

// ----------------------------------------------------------------------------

FITSKeywordArray XISFReader::ReadFITSKeywords()
{
   CheckOpenStream( "FITSKeywords" );
   return m_engine->FITSKeywords();
}

// ----------------------------------------------------------------------------

ICCProfile XISFReader::ReadICCProfile()
{
   CheckOpenStream( "ReadICCProfile" );
   return m_engine->ReadICCProfile();
}

// ----------------------------------------------------------------------------

UInt8Image XISFReader::ReadThumbnail()
{
   CheckOpenStream( "ReadThumbnail" );
   return m_engine->ReadThumbnail();
}

// ----------------------------------------------------------------------------

RGBColorSystem XISFReader::ReadRGBWorkingSpace()
{
   CheckOpenStream( "ReadRGBWorkingSpace" );
   return m_engine->RGBWorkingSpace();
}

// ----------------------------------------------------------------------------

DisplayFunction XISFReader::ReadDisplayFunction()
{
   CheckOpenStream( "ReadDisplayFunction" );
   return m_engine->DisplayFunction();
}

// ----------------------------------------------------------------------------

ColorFilterArray XISFReader::ReadColorFilterArray()
{
   CheckOpenStream( "ReadColorFilterArray" );
   return m_engine->ColorFilterArray();
}

// ----------------------------------------------------------------------------

PropertyDescriptionArray XISFReader::ImageProperties() const
{
   CheckOpenStream( "ImageProperties" );
   return m_engine->ImageProperties();
}

Variant XISFReader::ReadImageProperty( const IsoString& identifier )
{
   CheckOpenStream( "ReadImageProperty" );
   return m_engine->ReadImageProperty( identifier );
}

PropertyArray XISFReader::ReadImageProperties()
{
   PropertyDescriptionArray descriptions = ImageProperties();
   PropertyArray properties;
   for ( const PropertyDescription& description : descriptions )
      properties << Property( description.id, m_engine->ReadImageProperty( description.id ) );
   return properties;
}

// ----------------------------------------------------------------------------

PropertyDescriptionArray XISFReader::Properties() const
{
   CheckOpenStream( "Properties" );
   return m_engine->Properties();
}

Variant XISFReader::ReadProperty( const IsoString& identifier )
{
   CheckOpenStream( "ReadProperty" );
   return m_engine->ReadProperty( identifier );
}

PropertyArray XISFReader::ReadProperties()
{
   PropertyDescriptionArray descriptions = Properties();
   PropertyArray properties;
   for ( const PropertyDescription& description : descriptions )
      properties << Property( description.id, m_engine->ReadProperty( description.id ) );
   return properties;
}

// ----------------------------------------------------------------------------

void XISFReader::ReadImage( FImage& image )
{
   CheckOpenStream( "ReadImage" );
   m_engine->ReadImage( image );
}

void XISFReader::ReadImage( DImage& image )
{
   CheckOpenStream( "ReadImage" );
   m_engine->ReadImage( image );
}

void XISFReader::ReadImage( ComplexImage& image )
{
   CheckOpenStream( "ReadImage" );
   m_engine->ReadImage( image );
}

void XISFReader::ReadImage( DComplexImage& image )
{
   CheckOpenStream( "ReadImage" );
   m_engine->ReadImage( image );
}

void XISFReader::ReadImage( UInt8Image& image )
{
   CheckOpenStream( "ReadImage" );
   m_engine->ReadImage( image );
}

void XISFReader::ReadImage( UInt16Image& image )
{
   CheckOpenStream( "ReadImage" );
   m_engine->ReadImage( image );
}

void XISFReader::ReadImage( UInt32Image& image )
{
   CheckOpenStream( "ReadImage" );
   m_engine->ReadImage( image );
}

// ----------------------------------------------------------------------------

void XISFReader::ReadSamples( FImage::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "ReadSamples" );
   m_engine->ReadSamples( buffer, startRow, rowCount, channel, (FloatPixelTraits*)0 );
}

void XISFReader::ReadSamples( DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "ReadSamples" );
   m_engine->ReadSamples( buffer, startRow, rowCount, channel, (DoublePixelTraits*)0 );
}

void XISFReader::ReadSamples( ComplexImage::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "ReadSamples" );
   m_engine->ReadSamples( buffer, startRow, rowCount, channel, (ComplexPixelTraits*)0 );
}

void XISFReader::ReadSamples( DComplexImage::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "ReadSamples" );
   m_engine->ReadSamples( buffer, startRow, rowCount, channel, (DComplexPixelTraits*)0 );
}

void XISFReader::ReadSamples( UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "ReadSamples" );
   m_engine->ReadSamples( buffer, startRow, rowCount, channel, (UInt8PixelTraits*)0 );
}

void XISFReader::ReadSamples( UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "ReadSamples" );
   m_engine->ReadSamples( buffer, startRow, rowCount, channel, (UInt16PixelTraits*)0 );
}

void XISFReader::ReadSamples( UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "ReadSamples" );
   m_engine->ReadSamples( buffer, startRow, rowCount, channel, (UInt32PixelTraits*)0 );
}

// ----------------------------------------------------------------------------

void XISFReader::CheckOpenStream( const char* memberFunction ) const
{
   if ( !IsOpen() )
      throw Error( "Invalid call to XISFReader::" + String( memberFunction ) + "() on a closed stream." );
}

void XISFReader::CheckClosedStream( const char* memberFunction ) const
{
   if ( IsOpen() )
      throw Error( "Invalid call to XISFReader::" + String( memberFunction ) + "() on an open stream." );
}

// ----------------------------------------------------------------------------

XMLDocument* XISFReader::ExtractHeader( const String& path, XMLParserOptions options )
{
   File file = File::OpenFileForReading( path );

   XISFFileSignature signature;
   file.Read( signature );
   signature.Validate();

   IsoString header;
   header.SetLength( signature.headerLength );
   file.Read( reinterpret_cast<void*>( header.Begin() ), signature.headerLength );
   file.Close();

   AutoPointer<XMLDocument> xml = new XMLDocument;
   xml->SetParserOptions( options );
   xml->Parse( header.UTF8ToUTF16() );
   return xml.Release();
}

// ----------------------------------------------------------------------------

} //pcl

// ----------------------------------------------------------------------------
// EOF pcl/XISFReader.cpp - Released 2018-12-12T09:24:30Z
