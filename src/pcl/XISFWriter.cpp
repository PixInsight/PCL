//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/XISFWriter.cpp - Released 2017-04-14T23:04:51Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/Compression.h>
#include <pcl/Cryptography.h>
#include <pcl/MetaModule.h>
#include <pcl/Version.h>
#include <pcl/XISF.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Generic data block in an XISF output stream.
 */
struct XISFOutputBlock
{
   typedef Compression::subblock_list  subblock_list;

   typedef XISF::block_checksum        block_checksum;

   typedef XISF::block_compression     block_compression;

   IsoString         attachmentPos;
   block_compression compressionCodec  = XISFCompression::None;
   int               itemSize          = 1;
   subblock_list     subblocks;
   ByteArray         data;
   block_checksum    checksumAlgorithm = XISFChecksum::None;
   ByteArray         checksum;

   XISFOutputBlock() :
      attachmentPos( UniqueAttachmentToken() )
   {
   }

   XISFOutputBlock( const ByteArray& src ) :
      attachmentPos( UniqueAttachmentToken() ),
      data( src )
   {
   }

   XISFOutputBlock( const void* src, size_type size ) :
      attachmentPos( UniqueAttachmentToken() ),
      data( reinterpret_cast<const uint8*>( src ),
            reinterpret_cast<const uint8*>( src ) + size )
   {
   }

   XISFOutputBlock( const XISFOutputBlock& ) = default;

   XISFOutputBlock& operator =( const XISFOutputBlock& ) = default;

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

   size_type BlockSize() const
   {
      if ( HasData() )
         return data.Length();

      if ( HasCompressedData() )
      {
         size_type compressedSize = 0;
         for ( const Compression::Subblock& subblock : subblocks )
            compressedSize += subblock.compressedData.Length();
         return compressedSize;
      }

      return 0;
   }

   String EncodedData() const
   {
      if ( HasData() )
         return IsoString::ToBase64( data );

      if ( HasCompressedData() )
      {
         ByteArray compressedData;
         for ( const Compression::Subblock& subblock : subblocks )
            compressedData.Append( subblock.compressedData );
         return IsoString::ToBase64( compressedData );
      }

      return String();
   }

   String CompressionAttributeValue() const
   {
      // compression="<codec>:<uncompressed-size>:<item-size>"
      String value;
      if ( IsCompressed() )
      {
         size_type uncompressedSize = 0;
         for ( const Compression::Subblock& subblock : subblocks )
            uncompressedSize += subblock.uncompressedSize;
         value = String( XISF::CompressionCodecId( compressionCodec ) ) + ':' + String( uncompressedSize );
         if ( itemSize > 1 && XISF::CompressionNeedsItemSize( compressionCodec ) )
            value += ':' + String( itemSize );
      }
      return value;
   }

   String SubblocksAttributeValue() const
   {
      // subblocks="<cs0>,<us0>:<cs1>,<us1>:...:<csN-1>,<usN-1>"
      String value;
      if ( subblocks.Length() > 1 )
         for ( subblock_list::const_iterator i = subblocks.Begin(); ; )
         {
            value.AppendFormat( "%llu,%llu", i->compressedData.Length(), i->uncompressedSize );
            if ( ++i == subblocks.End() )
               break;
            value += ':';
         }
      return value;
   }

   void CompressData( block_compression method, int bytesPerItem, int level )
   {
      if ( HasData() )
      {
         if ( bytesPerItem < 2 )
            method = XISF::CompressionCodecNoShuffle( method );

         AutoPointer<Compression> compressor( XISF::NewCompression( method, bytesPerItem ) );
         compressor->SetCompressionLevel( level );

         subblocks = compressor->Compress( data );
         if ( subblocks.IsEmpty() )
         {
            compressionCodec = XISFCompression::None;
            itemSize = 1;
         }
         else
         {
            compressionCodec = method;
            itemSize = bytesPerItem;
            data.Clear();
         }
      }
   }

   void ComputeChecksum( block_checksum method )
   {
      AutoPointer<CryptographicHash> hash( XISF::NewCryptographicHash( checksumAlgorithm = method ) );

      if ( HasData() )
      {
         checksum = hash->Hash( data );
      }
      else if ( HasCompressedData() )
      {
         hash->Initialize();
         for ( const Compression::Subblock& subblock : subblocks )
            hash->Update( subblock.compressedData );
         checksum = hash->Finalize();
      }
   }

   String ChecksumAttributeValue() const
   {
      // checksum="<algorithm>:<digest>"
      String value;
      if ( HasChecksum() )
         value = String( XISF::ChecksumAlgorithmId( checksumAlgorithm ) ) + ':' + IsoString::ToHex( checksum );
      return value;
   }

   void WriteData( File& file ) const
   {
      if ( HasData() )
         file.Write( reinterpret_cast<const void*>( data.Begin() ), fsize_type( data.Length() ) );
      else if ( HasCompressedData() )
         for ( const Compression::Subblock& subblock : subblocks )
            file.Write( reinterpret_cast<const void*>( subblock.compressedData.Begin() ), fsize_type( subblock.compressedData.Length() ) );
   }

   void Clear()
   {
      (void)operator =( XISFOutputBlock() );
   }

private:

   /*
    * Create a unique token for second-pass generation of an attachment
    * attribute:
    *
    *    location="attachment:<position>:<size>"
    */
   static IsoString UniqueAttachmentToken()
   {
      return "attachment:" + IsoString::Random( 16 );
   }
};

typedef Array<XISFOutputBlock> XISFOutputBlockArray;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Property data block in an XISF output stream.
 */
struct XISFOutputProperty
{
   IsoString id;
   Variant   value;

   XISFOutputProperty() = default;
   XISFOutputProperty( const XISFOutputProperty& ) = default;

   XISFOutputProperty( const IsoString& a_id, const Variant& a_value = Variant() ) :
   id( a_id ), value( a_value )
   {
   }

   bool operator ==( const XISFOutputProperty& x ) const
   {
      return id == x.id;
   }

   bool operator <( const XISFOutputProperty& x ) const
   {
      return id < x.id;
   }
};

typedef SortedArray<XISFOutputProperty> XISFOutputPropertyArray;

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * Internal XISF output stream implementation.
 */
class XISFWriterEngine
{
public:

   XISFWriterEngine() = default;
   virtual ~XISFWriterEngine() = default;

   /*
    * Set format-specific options.
    */
   void SetOptions( const XISFOptions& options )
   {
      m_xisfOptions = options;
   }

   /*
    * Set hints string for OutputHints metadata property generation.
    */
   void SetHints( const IsoString& hints )
   {
      m_hints = hints;
   }

   void SetLogHandler( XISFLogHandler* handler )
   {
      m_logHandler = handler;
   }

   void SetCreatorApplication( const String& appName )
   {
      m_creatorApplication = appName;
   }

   void SetCreatorModule( const String& modName )
   {
      m_creatorModule = modName;
   }

   /*
    * Start a new XISF file.
    */
   void Open( const String& path )
   {
      Reset();

#ifdef __PCL_WINDOWS
      m_path = File::WindowsPathToUnix( path );
#else
      m_path = path;
#endif

      if ( m_logHandler != nullptr )
         m_logHandler->Init( m_path, true/*writing*/ );

      StartDocument();
   }

   /*
    * Flush and close the current output stream.
    */
   void Close()
   {
      if ( !IsOpen() )
         return;

      /*
       * Complete a possible sequential/random access image block, in case the
       * latest image has been written via CreateImage() / WriteSamples() /
       * CloseImage() instead of a single call to WriteImage().
       */
      CloseImage();

      /*
       * Complete the XML document and serialize it.
       */
      EndDocument();

      /*
       * Replace block position attributes. This is an iterative algorithm
       * resilient to variations in attribute value lengths.
       */
      for ( size_type n = 0; ; )
      {
         fpos_type pos = AlignedPosition( sizeof( XISFFileSignature ) + m_text.Length() );
         for ( XISFOutputBlock& block : m_blocks )
         {
            IsoString attachmentPos = IsoString().Format( "attachment:%llu", pos );
            m_text.ReplaceString( block.attachmentPos, attachmentPos );
            pos = AlignedPosition( pos + block.BlockSize() );
            block.attachmentPos = attachmentPos;
         }
         if ( m_text.Length() == n )
            break;
         n = m_text.Length();
      }

      /*
       * Generate the monolithic XISF unit.
       */

      // Prepare sufficient unused space, if necessary for alignment.
      ByteArray zero( size_type( m_xisfOptions.blockAlignmentSize ), uint8( 0 ) );

      // Create the output file.
      File file = File::CreateFileForWriting( m_path );

      // 1. XISF signature.
      file.Write( XISFFileSignature( uint32( m_text.Length() ) ) );

      // 2. XISF header.
      file.Write( reinterpret_cast<const void*>( m_text.Begin() ), m_text.Length() );

      // 3. Attached XISF blocks.
      for ( const XISFOutputBlock& block : m_blocks )
      {
         fpos_type pos = file.Position();
         fsize_type padding = AlignedPosition( pos ) - pos;
         if ( padding )
            file.Write( zero.Begin(), padding );
         block.WriteData( file );
      }

      // Close the output file.
      file.Close();

      // Liberate us.
      Reset();

      // Tell them.
      if ( m_logHandler != nullptr )
         m_logHandler->Close();
   }

   bool IsOpen() const
   {
      return m_root;
   }

   String FilePath() const
   {
      return m_path;
   }

   /*
    * format-independent options set for the current image.
    */
   const pcl::ImageOptions& ImageOptions() const
   {
      return m_options;
   }

   /*
    * Set format-independent image parameters and options.
    */
   void SetImageOptions( const pcl::ImageOptions& options )
   {
      m_options = options;
   }

   /*
    * Set the identifier of the current image in this output stream.
    */
   void SetImageId( const IsoString& id )
   {
      m_id = id;
   }

   /*
    * Set the RGB working space of the current image in this output stream.
    */
   void WriteRGBWorkingSpace( const RGBColorSystem& rgbws )
   {
      m_rgbws = rgbws;
   }

   /*
    * Set the display function of the current image in this output stream.
    */
   void WriteDisplayFunction( const DisplayFunction& df )
   {
      m_df = df;
   }

   /*
    * Set the color filter array (CFA) of the current image in this output
    * stream.
    */
   void WriteColorFilterArray( const ColorFilterArray& cfa )
   {
      m_cfa = cfa;
   }

   /*
    * Embed a list of FITS header keywords in the current image of this output
    * stream.
    */
   void WriteFITSKeywords( const FITSKeywordArray& keywords )
   {
      m_keywords = keywords;
   }

   /*
    * The embedded FITS keywords. This is necessary because the PixInsight core
    * application has an option to reload all FITS keywords after writing a new
    * image.
    */
   const FITSKeywordArray& FITSKeywords() const
   {
      return m_lastKeywords;
   }

   /*
    * Embed an ICC profile structure for the current image in this output
    * stream.
    */
   void WriteICCProfile( const ICCProfile& iccProfile )
   {
      m_iccProfile = iccProfile;
   }

   /*
    * Embed a thumbnail image for the current image in this output stream.
    */
   void WriteThumbnail( const UInt8Image& thumbnail )
   {
      if ( thumbnail.Width() <= XISF::MaxThumbnailSize && thumbnail.Height() <= XISF::MaxThumbnailSize )
         m_thumbnail = thumbnail;
   }

   /*
    * Create a new property associated with the current image, or change its
    * value if the property already exists in this output stream.
    */
   void WriteImageProperty( const IsoString& id, const Variant& value )
   {
      if ( !XISF::IsValidPropertyId( id ) )
         throw Error( String( "XISFWriterEngine::WriteImageProperty(): " ) + "Invalid XISF property identifier '" + id + "'" );

      if ( value.IsTimePoint() )
         if ( !value.ToTimePoint().IsValid() )
            throw Error( String( "XISFWriterEngine::WriteImageProperty(): " ) + "Invalid TimePoint value for property '" + id + "'" );

      if ( !XISF::IsInternalPropertyId( id ) )
      {
         XISFOutputPropertyArray::const_iterator i = m_imageProperties.Search( id );
         if ( i == m_imageProperties.End() )
            m_imageProperties << XISFOutputProperty( id, value );
         else
         {
            m_imageProperties.MutableIterator( i )->value = value;
            if ( m_xisfOptions.verbosity > 1 )
               LogLn( "Redefining image property '" + id + "'", XISFMessageType::Warning );
         }
         if ( m_xisfOptions.verbosity > 1 )
            LogLn( "Property '" + id + "' (" + XISF::PropertyTypeId( value.Type() ) + ") embedded in image." );
      }
      else
      {
         if ( m_xisfOptions.verbosity > 0 )
            LogLn( "Ignoring attempt to associate a reserved XISF property '" + id + "' with an image.", XISFMessageType::Warning );
      }
   }

   /*
    * Create a new property associated with the XISF unit, or change its value
    * if the property already exists in this output stream.
    */
   void WriteProperty( const IsoString& id, const Variant& value )
   {
      if ( !XISF::IsValidPropertyId( id ) )
         throw Error( String( "XISFWriterEngine::WriteProperty(): " ) + "Invalid XISF property identifier '" + id + "'" );

      if ( XISF::IsInternalPropertyId( id ) )
      {
         static const char* autoGeneratedPropertyIds[] = {
            "CreationTime", "CreatorApplication", "CreatorModule", "CreatorOS",
            "BlockAlignmentSize", "MaxInlineBlockSize", "CompressionCodecs",
            "CompressionLevel", "ChecksumAlgorithms", "OutputHints" };
         IsoString id1 = id.Substring( 5 );
         for ( size_type i = 0; i < ItemsInArray( autoGeneratedPropertyIds ); ++i )
            if ( id1 == autoGeneratedPropertyIds[i] )
            {
               if ( m_xisfOptions.verbosity > 0 )
                  LogLn( "Ignoring attempt to define auto-generated reserved property '" + id + "'", XISFMessageType::Warning );
               return;
            }
      }

      if ( value.IsTimePoint() )
         if ( !value.ToTimePoint().IsValid() )
            throw Error( String( "XISFWriterEngine::WriteProperty(): " ) + "Invalid TimePoint value for property '" + id + "'" );

      XISFOutputPropertyArray::const_iterator i = m_properties.Search( id );
      if ( i == m_properties.End() )
         m_properties << XISFOutputProperty( id, value );
      else
      {
         m_properties.MutableIterator( i )->value = value;
         if ( m_xisfOptions.verbosity > 1 )
            LogLn( "Redefining property '" + id + "'", XISFMessageType::Warning );
      }
      if ( m_xisfOptions.verbosity > 1 )
         LogLn( "Property '" + id + "' (" + XISF::PropertyTypeId( value.Type() ) + ") generated." );
   }

   /*
    * Generate a new Image element to store the specified image.
    */
   void WriteImage( const ImageVariant& image )
   {
      if ( image )
         if ( image.IsComplexSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: WriteImage( static_cast<const ComplexImage&>( *image ) ); break;
            case 64: WriteImage( static_cast<const DComplexImage&>( *image ) ); break;
            }
         else if ( image.IsFloatSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: WriteImage( static_cast<const FImage&>( *image ) ); break;
            case 64: WriteImage( static_cast<const DImage&>( *image ) ); break;
            }
         else
            switch ( image.BitsPerSample() )
            {
            case  8: WriteImage( static_cast<const UInt8Image&>( *image ) ); break;
            case 16: WriteImage( static_cast<const UInt16Image&>( *image ) ); break;
            case 32: WriteImage( static_cast<const UInt32Image&>( *image ) ); break;
            }
   }

   /*
    * Generate a new Image element to store the specified image.
    */
   template <class P>
   void WriteImage( const GenericImage<P>& image )
   {
      if ( !IsOpen() )
         return;

      if (  m_options.bitsPerSample != P::BitsPerSample() ||
           (m_options.ieeefpSampleFormat && !m_options.complexSample) != P::IsFloatSample() ||
            m_options.complexSample != P::IsComplexSample() )
      {
         ImageVariant tmp;
         tmp.CreateImage( m_options.ieeefpSampleFormat,
                          m_options.complexSample,
                          m_options.bitsPerSample ).CopyImage( image );
         WriteImage( tmp );
         return;
      }

      m_randomData.Clear();

      if ( m_xisfOptions.verbosity > 0 )
         LogLn( "Writing image" + (m_id.IsEmpty() ? String() : String( " '" + m_id + '\'' )) +
            String().Format( ": w=%d h=%d n=%d ",
                              image.Width(), image.Height(), image.NumberOfChannels() ) +
            XISF::ColorSpaceId( image.ColorSpace() ) + ' ' +
            XISF::SampleFormatId( P::BitsPerSample(), P::IsFloatSample(), P::IsComplexSample() ) );

      m_info = ImageInfo( image );

      // Partial channel selections will be serialized as grayscale images.
      if ( image.FirstSelectedChannel() != 0 || image.LastSelectedChannel() < 2 )
         m_info.colorSpace = ColorSpace::Gray;

      m_rgbws = image.RGBWorkingSpace();

      /*
       * To support embedded storage (for very small images) and child
       * elements, serialization of an image consists of the following steps:
       *
       * 1. Open the Image element and prepare some internal structures
       *    (geometry, color space, etc.)
       *
       * 2. Call WriteImageAttributes() to generate all required attributes.
       *
       * 3. Call NewBlock() to generate the location attribute, and possibly
       *    emit embedded pixel data.
       *
       * 4. Call WriteImageElements() to generate all child Image elements.
       *
       * 5. Call ResetImage() to destroy all data structures associated with
       *    the image, except the associated block, if the image has attached
       *    storage, which has been generated in step 3, and will be written
       *    upon stream completion in Close().
       */
      XMLElement* element = NewElement( m_root, "Image" );
      WriteImageAttributes( element );
      NewBlock( element, image );
      WriteImageElements( element );
      ResetImage();
   }

   /*
    * Create a new Image element with the specified image parameters, and
    * prepare for sequential/random write access.
    */
   void CreateImage( const pcl::ImageInfo& info )
   {
      if ( !IsOpen() )
         return;

      CloseImage();

      m_info = info;
      m_randomData = ByteArray( size_type( m_info.width )
                                         * m_info.height
                                         * m_info.numberOfChannels
                                         * (m_options.bitsPerSample >> 3), uint8( 0 ) );
   }

   /*
    * Sequential/random write of pixel samples.
    */
   template <typename T, class P>
   void WriteSamples( const T* buffer, int startRow, int rowCount, int channel, P* )
   {
      if ( !m_randomData.IsEmpty() ) // CreateImage() should have been called before this
         if ( rowCount > 0 )
            if ( m_options.complexSample )
               switch ( m_options.bitsPerSample )
               {
               case 32: WriteSamples( buffer, startRow, rowCount, channel, (P*)0, (ComplexPixelTraits*)0 ); break;
               case 64: WriteSamples( buffer, startRow, rowCount, channel, (P*)0, (DComplexPixelTraits*)0 ); break;
               }
            else if ( m_options.ieeefpSampleFormat )
               switch ( m_options.bitsPerSample )
               {
               case 32: WriteSamples( buffer, startRow, rowCount, channel, (P*)0, (FloatPixelTraits*)0 ); break;
               case 64: WriteSamples( buffer, startRow, rowCount, channel, (P*)0, (DoublePixelTraits*)0 ); break;
               }
            else
               switch ( m_options.bitsPerSample )
               {
               case  8: WriteSamples( buffer, startRow, rowCount, channel, (P*)0, (UInt8PixelTraits*)0 ); break;
               case 16: WriteSamples( buffer, startRow, rowCount, channel, (P*)0, (UInt16PixelTraits*)0 ); break;
               case 32: WriteSamples( buffer, startRow, rowCount, channel, (P*)0, (UInt32PixelTraits*)0 ); break;
               }
   }

   /*
    * Complete the current data block for incremental/random image output. If
    * no random access data have been allocated, this function has no effect.
    */
   void CloseImage()
   {
      if ( !m_randomData.IsEmpty() ) // CreateImage() should have been called before this
      {
         if ( m_xisfOptions.verbosity > 0 )
            LogLn( "Writing image" + (m_id.IsEmpty() ? String() : String( " '" + m_id + '\'' )) +
               String().Format( ": w=%d h=%d n=%d ",
                                 m_info.width, m_info.height, m_info.numberOfChannels ) +
               XISF::ColorSpaceId( m_info.colorSpace ) + ' ' +
               XISF::SampleFormatId( m_options.bitsPerSample, m_options.ieeefpSampleFormat, m_options.complexSample ) );

         XMLElement* element = NewElement( m_root, "Image" );
         WriteImageAttributes( element );
         NewBlock( element, m_randomData, m_options.bitsPerSample >> 3, false/*canInline*/ );
         WriteImageElements( element );
         ResetImage();
      }
   }

private:

   /*
    * Stream data.
    */
   XISFOptions             m_xisfOptions;     // format-specific options
   IsoString               m_hints;           // format hints (for metadata generation only)
   XISFLogHandler*         m_logHandler = nullptr;
   String                  m_creatorApplication;
   String                  m_creatorModule;
   XISFOutputPropertyArray m_properties;      // XISF unit properties
   AutoPointer<XMLElement> m_root;            // the XML root element
   String                  m_path;            // path to the current output file
   IsoString               m_text;            // the XML header, UTF-8
   XISFOutputBlockArray    m_blocks;          // attached blocks

   /*
    * Image data.
    */
   pcl::ImageOptions       m_options;         // format-independent options
   pcl::ImageInfo          m_info;            // geometry and color space data
   IsoString               m_id;              // optional image identifier
   RGBColorSystem          m_rgbws;           // RGB working space
   DisplayFunction         m_df;              // display function
   ColorFilterArray        m_cfa;             // CFA pattern
   FITSKeywordArray        m_keywords;        // compatibility FITS header keywords
   FITSKeywordArray        m_lastKeywords;    // embedded FITS keywords, to support reloading
   ICCProfile              m_iccProfile;      // ICC profile
   UInt8Image              m_thumbnail;       // thumbnail image
   XISFOutputPropertyArray m_imageProperties; // image properties
   ByteArray               m_randomData;      // sequential/random access image data

   /*
    * Reset the state of the engine and destroy all internal data structures.
    */
   void Reset()
   {
      m_root.Destroy();
      m_path.Clear();
      m_text.Clear();
      m_blocks.Clear();
      m_lastKeywords.Clear();
      ResetImage();
   }

   /*
    * Destroy all internal data structures associated with the current image.
    */
   void ResetImage()
   {
      m_info.Reset();
      m_id.Clear();
      m_rgbws = RGBColorSystem::sRGB;
      m_df.Reset();
      m_cfa.Clear();
      m_keywords.Clear();
      m_iccProfile.Clear();
      m_thumbnail.FreeData();
      m_imageProperties.Clear();
      m_randomData.Clear();
   }

   /*
    * Start a new XML document.
    */
   void StartDocument()
   {
      m_root = new XMLElement( "xisf", XMLAttributeList()
         << XMLAttribute( "version", "1.0" )
         << XMLAttribute( "xmlns", "http://www.pixinsight.com/xisf" )
         << XMLAttribute( "xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance" )
         << XMLAttribute( "xsi:schemaLocation", "http://www.pixinsight.com/xisf http://pixinsight.com/xisf/xisf-1.0.xsd" ) );
   }

   /*
    * Terminate an XML document.
    */
   void EndDocument()
   {
      /*
       * XISF metadata properties.
       */
      XMLElement* metadata = NewElement( m_root, "Metadata" );
      WriteProperty( metadata, "XISF:CreationTime", String::CurrentUTCISO8601DateTime( ISO8601ConversionOptions( 3/*timeItems*/, 0/*precision*/, true/*timeZone*/, true/*zuluTime*/ ) ) );

      if ( Module != nullptr && Module->IsInstalled() )
      {
         // We are running in an installed PixInsight module.
         if ( !m_creatorApplication.IsEmpty() )
            LogLn( "Ignoring user-specified XISF:CreatorApplication metadata property value in running module.", XISFMessageType::Warning );
         if ( !m_creatorModule.IsEmpty() )
            LogLn( "Ignoring user-specified XISF:CreatorModule metadata property value in running module.", XISFMessageType::Warning );

         WriteProperty( metadata, "XISF:CreatorApplication", PixInsightVersion::AsString( false/*withCodename*/ ) );
         WriteProperty( metadata, "XISF:CreatorModule", Module->ReadableVersion() );
      }
      else
      {
         // We are running in an independent application.
         if ( m_creatorApplication.IsEmpty() )
         {
            // XISF:CreatorApplication is mandatory as per XISF 1.0 D9.3
            LogLn( "Mandatory XISF:CreatorApplication metadata property value undefined - default 'unknown' tag will be written (which is ugly!).", XISFMessageType::Warning );
            WriteProperty( metadata, "XISF:CreatorApplication", "(* unknown *)" );
         }
         else
            WriteProperty( metadata, "XISF:CreatorApplication", m_creatorApplication );

         if ( !m_creatorModule.IsEmpty() )
            WriteProperty( metadata, "XISF:CreatorModule", m_creatorModule );
      }

      WriteProperty( metadata, "XISF:CreatorOS",
#ifdef __PCL_FREEBSD
                           "FreeBSD"
#endif
#ifdef __PCL_LINUX
                           "Linux"
#endif
#ifdef __PCL_MACOSX
                           "macOS"
#endif
#ifdef __PCL_WINDOWS
                           "Windows"
#endif
                           );

      if ( m_xisfOptions.compressionCodec == XISFCompression::None )
      {
         WriteProperty( metadata, "XISF:BlockAlignmentSize", m_xisfOptions.blockAlignmentSize );
         WriteProperty( metadata, "XISF:MaxInlineBlockSize", m_xisfOptions.maxInlineBlockSize );
      }
      else
      {
         WriteProperty( metadata, "XISF:CompressionCodecs", XISF::CompressionCodecId( m_xisfOptions.compressionCodec ) );
         WriteProperty( metadata, "XISF:CompressionLevel", int( m_xisfOptions.compressionLevel ) );
      }

      if ( m_xisfOptions.checksumAlgorithm != XISFChecksum::None )
         WriteProperty( metadata, "XISF:ChecksumAlgorithms", XISF::ChecksumAlgorithmId( m_xisfOptions.checksumAlgorithm ) );

      if ( !m_hints.IsEmpty() )
         WriteProperty( metadata, "XISF:OutputHints", m_hints );

      for ( const XISFOutputProperty& property : m_properties )
         WriteProperty( XISF::IsInternalPropertyId( property.id ) ? metadata : m_root.Pointer(), property );

      XMLDocument xml;
      xml.SetXML( "1.0", "UTF-8" );
      xml << new XMLComment( "\nExtensible Image Serialization Format - XISF version 1.0"
                             "\nCreated with PixInsight software - http://pixinsight.com/"
                             "\n" );
      xml.SetRootElement( m_root.Release() );
      m_text = xml.Serialize();
   }

   /*
    * Start a new XML element.
    */
   XMLElement* NewElement( XMLElement* parent, const String& id )
   {
      XMLElement* element = new XMLElement( id );
      *parent << element;
      return element;
   }

   /*
    * Write the compression and subblocks attributes for the specified XML
    * element. If the block is not compressed, this function does nothing.
    */
   void WriteBlockCompressionAttributes( XMLElement* element, const XISFOutputBlock& block )
   {
      String compressionAttributeValue = block.CompressionAttributeValue();
      String subblocksAttributeValue = block.SubblocksAttributeValue();
      if ( !compressionAttributeValue.IsEmpty() )
         element->SetAttribute( "compression", compressionAttributeValue );
      if ( !subblocksAttributeValue.IsEmpty() )
         element->SetAttribute( "subblocks", subblocksAttributeValue );
   }

   /*
    * Write the checksum attribute for the specified XML element. If the block
    * has not computed a valid checksum, this function does nothing.
    */
   void WriteBlockChecksumAttributes( XMLElement* element, const XISFOutputBlock& block )
   {
      String checksumAttributeValue = block.ChecksumAttributeValue();
      if ( !checksumAttributeValue.IsEmpty() )
         element->SetAttribute( "checksum", checksumAttributeValue );
   }

   /*
    * Generate a new output data block.
    *
    * For compressed data, write the required attribute:
    *
    *    compression="<alg>:<usize>:<isize>",
    *
    * where <alg> is the compression algorithm, <usize> is the uncompressed
    * block size, and <isize> is the optional item size in bytes (default=1).
    *
    * For inline and embedded data, write the required attribute:
    *
    *    location="inline:<encoding>"
    *
    * or
    *
    *    location="embedded"
    *
    * and emit base64-encoded data.
    *
    * For attached data, write the attribute:
    *
    *    location="attachment:<pos>:<size>"
    *
    * and generate an offline block structure to be written upon stream
    * completion.
    */
   void NewBlock( XMLElement* element, const ByteArray& blockData, int itemSize = 1, bool canInline = true )
   {
      XISFOutputBlock block( blockData );

      if ( m_xisfOptions.compressionCodec != XISFCompression::None )
         CompressBlock( block, itemSize );

      if ( m_xisfOptions.checksumAlgorithm != XISFChecksum::None )
         block.ComputeChecksum( m_xisfOptions.checksumAlgorithm );

      size_type blockSize = block.BlockSize();

      if ( blockSize <= m_xisfOptions.maxInlineBlockSize )
      {
         /*
          * Generate inline or embedded data.
          */
         if ( canInline )
         {
            WriteBlockCompressionAttributes( element, block );
            WriteBlockChecksumAttributes( element, block );
            element->SetAttribute( "location", "inline:base64" );
            *element << new XMLText( block.EncodedData(), false/*preserveSpaces*/ );
         }
         else
         {
            element->SetAttribute( "location", "embedded" );
            XMLElement* dataElement = NewElement( element, "Data" );
            WriteBlockCompressionAttributes( dataElement, block );
            WriteBlockChecksumAttributes( dataElement, block );
            dataElement->SetAttribute( "encoding", "base64" );
            *dataElement << new XMLText( block.EncodedData(), false/*preserveSpaces*/ );
         }
      }
      else
      {
         /*
          * Prepare a new block for attachment.
          */
         WriteBlockCompressionAttributes( element, block );
         WriteBlockChecksumAttributes( element, block );
         element->SetAttribute( "location", block.attachmentPos + ':' + String( blockSize ) );
         m_blocks << block;
      }
   }

   /*
    * Generate a new output data block.
    */
   void NewBlock( XMLElement* element, const void* blockData, size_type blockSize, int itemSize = 1, bool canInline = true )
   {
      NewBlock( element, ByteArray( reinterpret_cast<const uint8*>( blockData ),
                                    reinterpret_cast<const uint8*>( blockData ) + blockSize ), itemSize, canInline );
   }

   /*
    * Generate a new image block.
    *
    * Image blocks can only be stored as attachments or as embedded data.
    * Inline image blocks are not feasible because the Image element has child
    * elements.
    */
   template <class P>
   void NewBlock( XMLElement* element, const GenericImage<P>& image )
   {
      ByteArray blockData;
      if ( image.IsFullSelection() )
      {
         for ( int i = image.FirstSelectedChannel(); i <= image.LastSelectedChannel(); ++i )
            blockData.Append( reinterpret_cast<const uint8*>( image[i] ),
                              reinterpret_cast<const uint8*>( image[i] ) + image.ChannelSize() );
      }
      else
      {
         Rect r = image.SelectedRectangle();
         size_type bytesPerRow = r.Width() * image.BytesPerSample();
         for ( int i = image.FirstSelectedChannel(); i <= image.LastSelectedChannel(); ++i )
         {
            const typename P::sample* p = image.PixelAddress( r.LeftTop(), i );
            for ( int j = r.y0; j < r.y1; ++j, p += image.Width() )
               blockData.Append( reinterpret_cast<const uint8*>( p ),
                                 reinterpret_cast<const uint8*>( p ) + bytesPerRow );
         }
      }

      NewBlock( element, blockData, P::BytesPerSample(), false/*canInline*/ );
   }

   /*
    * Compress an output data block.
    */
   void CompressBlock( XISFOutputBlock& block, int itemSize )
   {
      size_type uncompressedSize = block.data.Length();
      int compressionLevel = XISF::CompressionLevelForMethod( m_xisfOptions.compressionCodec, m_xisfOptions.compressionLevel );
      if ( m_xisfOptions.verbosity > 0 )
         Log( "<end><cbr>Compressing block (" +
               String( XISF::CompressionCodecId( m_xisfOptions.compressionCodec ) ) +
               String().Format( ":%d): ", compressionLevel ) +
               File::SizeAsString( uncompressedSize ) + " -> " );

      block.CompressData( m_xisfOptions.compressionCodec, itemSize, compressionLevel );

      if ( m_xisfOptions.verbosity > 0 )
      {
         size_type compressedSize = block.BlockSize();
         LogLn( File::SizeAsString( compressedSize ) +
            String().Format( " (%.2f%%)", 100*double( uncompressedSize - compressedSize )/uncompressedSize ) );
      }
   }

   /*
    * Generate XML output for Image element attributes.
    */
   void WriteImageAttributes( XMLElement* element )
   {
      if ( !m_id.IsEmpty() )
         element->SetAttribute( "id", m_id );

      element->SetAttribute( "geometry", String().Format( "%d:%d:%d", m_info.width, m_info.height, m_info.numberOfChannels ) );
      element->SetAttribute( "sampleFormat", XISF::SampleFormatId( m_options.bitsPerSample, m_options.ieeefpSampleFormat, m_options.complexSample ) );

      if ( m_options.ieeefpSampleFormat )
         if ( !m_options.complexSample )
         {
            if ( m_xisfOptions.outputUpperBound < m_xisfOptions.outputLowerBound )
               Swap( m_xisfOptions.outputLowerBound, m_xisfOptions.outputUpperBound );
            element->SetAttribute( "bounds", String().Format( "%.16lg:%.16lg", m_xisfOptions.outputLowerBound, m_xisfOptions.outputUpperBound ) );
         }

      element->SetAttribute( "colorSpace", XISF::ColorSpaceId( m_info.colorSpace ) );
   }

   /*
    * Generate XML output for Image child elements.
    */
   void WriteImageElements( XMLElement* element )
   {
      if ( m_xisfOptions.storeFITSKeywords )
         if ( !m_keywords.IsEmpty() )
         {
            m_lastKeywords = m_keywords;
            for ( FITSHeaderKeyword& keyword : m_lastKeywords )
            {
               XMLElement* keywordElement = NewElement( element, "FITSKeyword" );
               keyword.Trim();
               keyword.FixValueDelimiters();
               keywordElement->SetAttribute( "name", keyword.name );
               keywordElement->SetAttribute( "value", keyword.value );
               keywordElement->SetAttribute( "comment", keyword.comment );
            }
            if ( m_xisfOptions.verbosity > 0 )
               LogLn( String( m_lastKeywords.Length() ) + " FITS keyword(s) embedded." );
         }

      if ( m_options.embedColorFilterArray )
         if ( !m_cfa.IsEmpty() )
         {
            XMLElement* cfaElement = NewElement( element, "ColorFilterArray" );
            cfaElement->SetAttribute( "pattern", m_cfa.Pattern() );
            cfaElement->SetAttribute( "width", String( m_cfa.Width() ) );
            cfaElement->SetAttribute( "height", String( m_cfa.Height() ) );
            if ( !m_cfa.Name().IsEmpty() )
               cfaElement->SetAttribute( "name", m_cfa.Name() );
            if ( m_xisfOptions.verbosity > 0 )
               LogLn( "CFA parameters embedded: pattern='" + m_cfa.Pattern()
                  + "' width=" + String( m_cfa.Width() )
                  + " height=" + String( m_cfa.Height() )
                  + (m_cfa.Name().IsEmpty() ? String() : " name='" + m_cfa.Name() + '\'') );
         }

      if ( m_options.embedRGBWS )
         if ( m_rgbws != RGBColorSystem::sRGB )
         {
            XMLElement* rgbwsElement = NewElement( element, "RGBWorkingSpace" );
            rgbwsElement->SetAttribute( "gamma", m_rgbws.IsSRGB() ? String( "sRGB" ) : String( m_rgbws.Gamma() ) );
            rgbwsElement->SetAttribute( "xr", String( m_rgbws.ChromaticityXCoordinates()[0] ) );
            rgbwsElement->SetAttribute( "xg", String( m_rgbws.ChromaticityXCoordinates()[1] ) );
            rgbwsElement->SetAttribute( "xb", String( m_rgbws.ChromaticityXCoordinates()[2] ) );
            rgbwsElement->SetAttribute( "yr", String( m_rgbws.ChromaticityYCoordinates()[0] ) );
            rgbwsElement->SetAttribute( "yg", String( m_rgbws.ChromaticityYCoordinates()[1] ) );
            rgbwsElement->SetAttribute( "yb", String( m_rgbws.ChromaticityYCoordinates()[2] ) );
            rgbwsElement->SetAttribute( "Yr", String( m_rgbws.LuminanceCoefficients()[0] ) );
            rgbwsElement->SetAttribute( "Yg", String( m_rgbws.LuminanceCoefficients()[1] ) );
            rgbwsElement->SetAttribute( "Yb", String( m_rgbws.LuminanceCoefficients()[2] ) );
            if ( m_xisfOptions.verbosity > 0 )
               LogLn( "RGBWS parameters embedded." );
         }

      if ( m_options.embedDisplayFunction )
         if ( !m_df.IsIdentityTransformation() )
         {
            DVector m, s, h, l, r;
            m_df.GetDisplayFunctionParameters( m, s, h, l, r );
            XMLElement* dfElement = NewElement( element, "DisplayFunction" );
            dfElement->SetAttribute( "m", String().Format( "%.16lg:%.16lg:%.16lg:%.16lg", m[0], m[1], m[2], m[3] ) );
            dfElement->SetAttribute( "s", String().Format( "%.16lg:%.16lg:%.16lg:%.16lg", s[0], s[1], s[2], s[3] ) );
            dfElement->SetAttribute( "h", String().Format( "%.16lg:%.16lg:%.16lg:%.16lg", h[0], h[1], h[2], h[3] ) );
            dfElement->SetAttribute( "l", String().Format( "%.16lg:%.16lg:%.16lg:%.16lg", l[0], l[1], l[2], l[3] ) );
            dfElement->SetAttribute( "r", String().Format( "%.16lg:%.16lg:%.16lg:%.16lg", r[0], r[1], r[2], r[3] ) );
            if ( m_xisfOptions.verbosity > 0 )
               LogLn( "Display function parameters embedded." );
         }

      if ( m_options.xResolution > 0 )
         if ( m_options.yResolution > 0 )
         {
            XMLElement* resElement = NewElement( element, "Resolution" );
            resElement->SetAttribute( "horizontal", String( m_options.xResolution ) );
            resElement->SetAttribute( "vertical", String( m_options.yResolution ) );
            resElement->SetAttribute( "unit", m_options.metricResolution ? "cm" : "inch" );
            if ( m_xisfOptions.verbosity > 1 )
               LogLn( "Image resolution parameters embedded." );
         }

      if ( m_options.embedICCProfile )
         if ( m_iccProfile.IsProfile() )
         {
            NewBlock( NewElement( element, "ICCProfile" ), m_iccProfile.ProfileData() );
            if ( m_xisfOptions.verbosity > 0 )
               LogLn( "ICC profile embedded: \'" + m_iccProfile.Description() + "\', " + String( m_iccProfile.ProfileSize() ) + " bytes." );
         }

      if ( m_options.embedProperties )
         if ( !m_imageProperties.IsEmpty() )
         {
            for ( const XISFOutputProperty& property : m_imageProperties )
               WriteProperty( element, property );
            if ( m_xisfOptions.verbosity > 0 )
               LogLn( String( m_imageProperties.Length() ) + " image " +
                                 ((m_imageProperties.Length() > 1) ? "properties" : "property") + " embedded." );
         }

      if ( m_options.embedThumbnail )
         if ( !m_thumbnail.IsEmpty() )
         {
            m_thumbnail.ResetSelections();
            XMLElement* tnElement = NewElement( element, "Thumbnail" );
            tnElement->SetAttribute( "geometry", String().Format( "%d:%d:%d", m_thumbnail.Width(), m_thumbnail.Height(), m_thumbnail.NumberOfChannels() ) );
            tnElement->SetAttribute( "sampleFormat", "UInt8" );
            tnElement->SetAttribute( "colorSpace", m_thumbnail.IsColor() ? "RGB" : "Gray" );
            NewBlock( tnElement, m_thumbnail );
            if ( m_xisfOptions.verbosity > 1 )
               LogLn( "Image thumbnail embedded " +
                  String().Format( "(w=%d h=%d n=%d)", m_thumbnail.Width(), m_thumbnail.Height(), m_thumbnail.NumberOfChannels() ) );
         }
   }

   /*
    * Generate XML output for a Property element.
    */
   void WriteProperty( XMLElement* element, const String& id, const Variant& value )
   {
      XMLElement* propertyElement = NewElement( element, "Property" );
      propertyElement->SetAttribute( "id", id );
      propertyElement->SetAttribute( "type", XISF::PropertyTypeId( value.Type() ) );

      if ( value.IsScalar() || value.Type() == VariantType::TimePoint )
      {
         propertyElement->SetAttribute( "value", value.ToString() );
      }
      else if ( value.Type() == VariantType::IsoString )
      {
         if ( m_xisfOptions.compressionCodec == XISFCompression::None || value.BlockSize() <= 80 )
         {
            if ( value.BlockSize() > 0 )
               *propertyElement << new XMLText( String::UTF8ToUTF16( reinterpret_cast<const char*>( value.InternalBlockAddress() ) ) );
         }
         else
            NewBlock( propertyElement, value.InternalBlockAddress(), value.BlockSize() );
      }
      else if ( value.Type() == VariantType::String )
      {
         if ( m_xisfOptions.compressionCodec == XISFCompression::None || value.BlockSize() <= 160 )
         {
            if ( value.BlockSize() > 0 )
               *propertyElement << new XMLText( reinterpret_cast<const char16_type*>( value.InternalBlockAddress() ) );
         }
         else
         {
            IsoString utf8 = value.ToString().ToUTF8();
            NewBlock( propertyElement, utf8.Begin(), utf8.Length() );
         }
      }
      else if ( value.IsVector() )
      {
         propertyElement->SetAttribute( "length", String( value.VectorLength() ) );
         NewBlock( propertyElement, value.InternalBlockAddress(), value.BlockSize(), value.BytesPerBlockElement() );
      }
      else if ( value.IsMatrix() )
      {
         Rect d = value.MatrixDimensions();
         propertyElement->SetAttribute( "rows", String( d.Height() ) );
         propertyElement->SetAttribute( "columns", String( d.Width() ) );
         NewBlock( propertyElement, value.InternalBlockAddress(), value.BlockSize(), value.BytesPerBlockElement() );
      }
      else
      {
         throw Error( "XISFWriterEngine::WriteProperty(): Internal error: Invalid property data type '" + String( value.Type() ) + '\'' );
      }
   }

   void WriteProperty( XMLElement* element, const XISFOutputProperty& property )
   {
      WriteProperty( element, property.id, property.value );
   }

   /*
    * Write samples from source data in a foreign sample data type.
    */
   template <class I, class P>
   void WriteSamples( const typename I::sample* buffer, int startRow, int rowCount, int channel, I*, P* )
   {
      typename P::sample* p = reinterpret_cast<typename P::sample*>( m_randomData.At( BlockSampleOffset( startRow, channel ) ) );
      size_type n = BlockSampleCount( rowCount );
      XISF::EnsurePTLUTInitialized();
      for ( size_type i = 0; i < n; ++i, ++p, ++buffer )
         *p = P::ToSample( *buffer );
   }

   /*
    * Write samples from source in the native pixel sample data type.
    */
   template <class P>
   void WriteSamples( const typename P::sample* buffer, int startRow, int rowCount, int channel, P*, P* )
   {
      ::memcpy( m_randomData.At( BlockSampleOffset( startRow, channel ) ), buffer, BlockSampleSize( rowCount ) );
   }

   /*
    * Byte offset of the first pixel sample in a given row of a given channel
    * of the current image.
    */
   size_type BlockSampleOffset( size_type row, size_type channel ) const
   {
      return size_type( m_options.bitsPerSample >> 3 ) * (size_type( channel ) * BlockSampleCount( m_info.height ) + BlockSampleCount( row ));
   }

   /*
    * Number of samples in a block of rowCount contiguous pixel sample rows of
    * the current image.
    */
   size_type BlockSampleCount( size_type rowCount ) const
   {
      return rowCount * size_type( m_info.width );
   }

   /*
    * Size in bytes of a block of rowCount contiguous pixel sample rows of the
    * current image.
    */
   size_type BlockSampleSize( size_type rowCount ) const
   {
      return size_type( m_options.bitsPerSample >> 3 ) * BlockSampleCount( rowCount );
   }

   /*
    * Given an absolute file position upos in bytes, compute the next aligned
    * position >= upos.
    */
   fpos_type AlignedPosition( fpos_type upos ) const
   {
      if ( m_xisfOptions.compressionCodec != XISFCompression::None || m_xisfOptions.blockAlignmentSize < 2 )
         return upos;
      fpos_type apos = m_xisfOptions.blockAlignmentSize * (upos/m_xisfOptions.blockAlignmentSize);
      if ( apos < upos )
         apos += m_xisfOptions.blockAlignmentSize;
      return apos;
   }

   /*
    * Send log messages to the log handler, if we have one.
    */
   void Log( const String& text, XISFLogHandler::message_type type = XISFMessageType::Informative )
   {
      if ( m_logHandler != nullptr )
         m_logHandler->Log( text, type );
   }
   void LogLn( const String& text, XISFLogHandler::message_type type = XISFMessageType::Informative )
   {
      if ( m_logHandler != nullptr )
         m_logHandler->Log( text + '\n', type );
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * XISFWriter Implementation
 */

XISFWriter::XISFWriter()
{
   // N.B.: This cannot be inlined in the header file, since XISFWriterEngine
   // is unknown to the public.
}

// ----------------------------------------------------------------------------

XISFWriter::~XISFWriter()
{
   Close();
   // N.B.: The above comment applies.
}

// ----------------------------------------------------------------------------

void XISFWriter::SetOptions( const pcl::XISFOptions& options )
{
   m_options = options;
   if ( IsOpen() )
      m_engine->SetOptions( m_options );
}

// ----------------------------------------------------------------------------

void XISFWriter::SetHints( const IsoString& hints )
{
   m_hints = hints.Trimmed();
   if ( IsOpen() )
      m_engine->SetHints( m_hints );
}

// ----------------------------------------------------------------------------

void XISFWriter::SetLogHandler( XISFLogHandler* handler )
{
   m_logHandler = handler;
   if ( IsOpen() )
      m_engine->SetLogHandler( m_logHandler );
}

// ----------------------------------------------------------------------------

void XISFWriter::SetCreatorApplication( const String& appName )
{
   m_creatorApplication = appName.Trimmed();
   if ( IsOpen() )
      m_engine->SetCreatorApplication( m_creatorApplication );
}

// ----------------------------------------------------------------------------

void XISFWriter::SetCreatorModule( const String& modName )
{
   m_creatorModule = modName.Trimmed();
   if ( IsOpen() )
      m_engine->SetCreatorModule( m_creatorModule );
}

// ----------------------------------------------------------------------------

bool XISFWriter::IsOpen() const
{
   return m_engine;
}

// ----------------------------------------------------------------------------

void XISFWriter::Create( const String& path, int count )
{
   CheckClosedStream( "Create" );
   m_engine = new XISFWriterEngine;
   m_engine->SetOptions( m_options );
   m_engine->SetHints( m_hints );
   m_engine->SetLogHandler( m_logHandler );
   m_engine->Open( path );
}

// ----------------------------------------------------------------------------

void XISFWriter::Close()
{
   if ( IsOpen() )
   {
      m_engine->Close();
      m_engine.Destroy();
   }
}

// ----------------------------------------------------------------------------

String XISFWriter::FilePath() const
{
   CheckOpenStream( "FilePath" );
   return m_engine->FilePath();
}

// ----------------------------------------------------------------------------

void XISFWriter::SetImageOptions( const ImageOptions& options )
{
   CheckOpenStream( "SetImageOptions" );
   m_engine->SetImageOptions( options );
}

// ----------------------------------------------------------------------------

void XISFWriter::SetImageId( const IsoString& id )
{
   CheckOpenStream( "SetImageId" );
   m_engine->SetImageId( id );
}

// ----------------------------------------------------------------------------

void XISFWriter::WriteFITSKeywords( const FITSKeywordArray& keywords )
{
   CheckOpenStream( "WriteFITSKeywords" );
   m_engine->WriteFITSKeywords( keywords );
}

// ----------------------------------------------------------------------------

const FITSKeywordArray& XISFWriter::FITSKeywords() const
{
   CheckOpenStream( "WriteFITSKeywords" );
   return m_engine->FITSKeywords();
}

// ----------------------------------------------------------------------------

void XISFWriter::WriteICCProfile( const ICCProfile& profile )
{
   CheckOpenStream( "WriteICCProfile" );
   m_engine->WriteICCProfile( profile );
}

// ----------------------------------------------------------------------------

void XISFWriter::WriteThumbnail( const UInt8Image& thumbnail )
{
   CheckOpenStream( "WriteThumbnail" );
   m_engine->WriteThumbnail( thumbnail );
}

// ----------------------------------------------------------------------------

void XISFWriter::WriteRGBWorkingSpace( const RGBColorSystem& rgbws )
{
   CheckOpenStream( "WriteRGBWorkingSpace" );
   m_engine->WriteRGBWorkingSpace( rgbws );
}

// ----------------------------------------------------------------------------

void XISFWriter::WriteDisplayFunction( const DisplayFunction& df )
{
   CheckOpenStream( "WriteDisplayFunction" );
   m_engine->WriteDisplayFunction( df );
}

// ----------------------------------------------------------------------------

void XISFWriter::WriteColorFilterArray( const ColorFilterArray& cfa )
{
   CheckOpenStream( "WriteColorFilterArray" );
   m_engine->WriteColorFilterArray( cfa );
}

// ----------------------------------------------------------------------------

void XISFWriter::WriteImageProperty( const IsoString& identifier, const Variant& value )
{
   CheckOpenStream( "WriteImageProperty" );
   m_engine->WriteImageProperty( identifier, value );
}

// ----------------------------------------------------------------------------

void XISFWriter::WriteProperty( const IsoString& identifier, const Variant& value )
{
   CheckOpenStream( "WriteProperty" );
   m_engine->WriteProperty( identifier, value );
}

// ----------------------------------------------------------------------------

void XISFWriter::WriteImage( const Image& image )
{
   CheckOpenStream( "WriteImage" );
   m_engine->WriteImage( image );
}

void XISFWriter::WriteImage( const DImage& image )
{
   CheckOpenStream( "WriteImage" );
   m_engine->WriteImage( image );
}

void XISFWriter::WriteImage( const ComplexImage& image )
{
   CheckOpenStream( "WriteImage" );
   m_engine->WriteImage( image );
}

void XISFWriter::WriteImage( const DComplexImage& image )
{
   CheckOpenStream( "WriteImage" );
   m_engine->WriteImage( image );
}

void XISFWriter::WriteImage( const UInt8Image& image )
{
   CheckOpenStream( "WriteImage" );
   m_engine->WriteImage( image );
}

void XISFWriter::WriteImage( const UInt16Image& image )
{
   CheckOpenStream( "WriteImage" );
   m_engine->WriteImage( image );
}

void XISFWriter::WriteImage( const UInt32Image& image )
{
   CheckOpenStream( "WriteImage" );
   m_engine->WriteImage( image );
}

// ----------------------------------------------------------------------------

void XISFWriter::CreateImage( const ImageInfo& info )
{
   CheckOpenStream( "CreateImage" );
   m_engine->CreateImage( info );
}

// ----------------------------------------------------------------------------

void XISFWriter::WriteSamples( const FImage::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "WriteSamples" );
   m_engine->WriteSamples( buffer, startRow, rowCount, channel, (FloatPixelTraits*)0 );
}

void XISFWriter::WriteSamples( const DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "WriteSamples" );
   m_engine->WriteSamples( buffer, startRow, rowCount, channel, (DoublePixelTraits*)0 );
}

void XISFWriter::WriteSamples( const ComplexImage::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "WriteSamples" );
   m_engine->WriteSamples( buffer, startRow, rowCount, channel, (ComplexPixelTraits*)0 );
}

void XISFWriter::WriteSamples( const DComplexImage::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "WriteSamples" );
   m_engine->WriteSamples( buffer, startRow, rowCount, channel, (DComplexPixelTraits*)0 );
}

void XISFWriter::WriteSamples( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "WriteSamples" );
   m_engine->WriteSamples( buffer, startRow, rowCount, channel, (UInt8PixelTraits*)0 );
}

void XISFWriter::WriteSamples( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "WriteSamples" );
   m_engine->WriteSamples( buffer, startRow, rowCount, channel, (UInt16PixelTraits*)0 );
}

void XISFWriter::WriteSamples( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
{
   CheckOpenStream( "WriteSamples" );
   m_engine->WriteSamples( buffer, startRow, rowCount, channel, (UInt32PixelTraits*)0 );
}

// ----------------------------------------------------------------------------

void XISFWriter::CloseImage()
{
   CheckOpenStream( "CloseImage" );
   m_engine->CloseImage();
}

// ----------------------------------------------------------------------------

void XISFWriter::CheckOpenStream( const char* memberFunction ) const
{
   if ( !IsOpen() )
      throw Error( "Invalid call to XISFWriter::" + String( memberFunction ) + "() on a closed stream." );
}

void XISFWriter::CheckClosedStream( const char* memberFunction ) const
{
   if ( IsOpen() )
      throw Error( "Invalid call to XISFWriter::" + String( memberFunction ) + "() on an open stream." );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/XISFWriter.cpp - Released 2017-04-14T23:04:51Z
