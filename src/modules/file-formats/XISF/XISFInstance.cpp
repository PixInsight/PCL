//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard XISF File Format Module Version 01.00.09.0125
// ----------------------------------------------------------------------------
// XISFInstance.cpp - Released 2017-04-14T23:07:03Z
// ----------------------------------------------------------------------------
// This file is part of the standard XISF PixInsight module.
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

#include "XISFInstance.h"
#include "XISFFormat.h"
#include "XISFOptionsDialog.h"

#include <pcl/ErrorHandler.h>
#include <pcl/MetaModule.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * XISF hints
 *
 *    auto-metadata                 r
 *    no-auto-metadata              r
 *    block-alignment <n>            w
 *    no-block-alignment             w
 *    max-inline-block-size <n>      w
 *    cfa-type <cfa>                 w
 *    no-cfa                         w
 *    compression-codec <codec>      w
 *    compress-data                  w
 *    no-compress-data               w
 *    compression-level <n>          w
 *    checksums <method>             w
 *    no-checksums                   w
 *    embedded-data                 rw
 *    no-embedded-data              rw
 *    fits-keywords                 rw
 *    no-fits-keywords              rw
 *    image-ids                      w
 *    import-fits-keywords          r
 *    no-import-fits-keywords       r
 *    lower-bound <n>                w
 *    upper-bound <n>                w
 *    normalize                     r
 *    no-normalize                  r
 *    properties                    rw
 *    no-properties                 rw
 *    resolution <n>                 w
 *    resolution-unit <u>            w
 *    no-resolution                  w
 *    verbosity <n>                 rw
 *    warnings                      r
 *    no-warnings                   r
 *    warnings-are-errors           r
 *    no-warnings-are-errors        r
 */
class XISFStreamHints
{
public:

   typedef XISF::block_compression  block_compression;

   typedef XISF::block_checksum     block_checksum;

   template <typename T>
   class HintValue
   {
   public:

      typedef T value_type;

      HintValue( value_type value = T() ) : m_value( value ), m_changed( false )
      {
      }

      T Value() const
      {
         return m_value;
      }

      operator T() const
      {
         return Value();
      }

      HintValue& operator =( value_type value )
      {
         m_value = value;
         m_changed = true;
         return *this;
      }

      bool HasChanged() const
      {
         return m_changed;
      }

   private:

      value_type m_value;
      bool       m_changed : 1;
   };

   HintValue<bool>              properties;
   HintValue<bool>              fitsKeywords;
   HintValue<bool>              importFITSKeywords;
   HintValue<bool>              autoMetadata;
   HintValue<block_compression> compressionCodec;
   HintValue<int>               compressionLevel;
   HintValue<block_checksum>    checksumAlgorithm;
   HintValue<unsigned>          blockAlignmentSize;
   HintValue<unsigned>          maxInlineBlockSize;
   HintValue<bool>              embeddedData;
   HintValue<IsoString>         cfa;
   HintValue<bool>              normalize;
   HintValue<double>            resolution;
   HintValue<IsoString>         resolutionUnit;
   HintValue<int>               verbosity;
   HintValue<bool>              noWarnings;
   HintValue<bool>              warningsAreErrors;
   IsoStringList                imageIds;
   int                          imageIdIndex;
   HintValue<double>            outputLowerBound;
   HintValue<double>            outputUpperBound;

   XISFStreamHints( const IsoString& hints ) : imageIdIndex( 0 )
   {
      IsoStringList theHints;
      hints.Break( theHints, ' ', true/*trim*/ );
      theHints.Remove( IsoString() );
      for ( IsoStringList::const_iterator i = theHints.Begin(); i < theHints.End(); ++i )
      {
         if ( *i == "no-properties" )
            properties = false;
         else if ( *i == "properties" )
            properties = true;
         else if ( *i == "no-fits-keywords" )
            fitsKeywords = false;
         else if ( *i == "fits-keywords" )
            fitsKeywords = true;
         else if ( *i == "import-fits-keywords" )
            importFITSKeywords = true;
         else if ( *i == "no-import-fits-keywords" )
            importFITSKeywords = false;
         else if ( *i == "auto-metadata" )
            autoMetadata = true;
         else if ( *i == "no-auto-metadata" )
            autoMetadata = false;
         else if ( *i == "compression-codec" )
         {
            if ( ++i == theHints.End() )
               break;
            block_compression n = XISF::CompressionCodecFromId( *i );
            if ( n != XISFCompression::Unknown )
               compressionCodec = n;
         }
         else if ( *i == "compression-level" )
         {
            if ( ++i == theHints.End() )
               break;
            int n;
            if ( i->TryToInt( n ) )
               if ( n >= 0 ) // 0=default
                  compressionLevel = Range( n, XISF::DefaultCompressionLevel, /*XISF::MaxCompressionLevel*/100 );
         }
         else if ( *i == "compress-data" ) // (deprecated) = compression-codec zlib
            compressionCodec = XISFCompression::Zlib;
         else if ( *i == "no-compression" ||
                   *i == "no-compress-data" ) // (deprecated) = no-compression
            compressionCodec = XISFCompression::None;
         else if ( *i == "checksums" )
         {
            if ( ++i == theHints.End() )
               break;
            block_checksum n = XISF::ChecksumAlgorithmFromId( *i );
            if ( n != XISFChecksum::Unknown )
               checksumAlgorithm = n;
         }
         else if ( *i == "no-checksums" )
            checksumAlgorithm = XISFChecksum::None;
         else if ( *i == "block-alignment" )
         {
            if ( ++i == theHints.End() )
               break;
            unsigned n;
            if ( i->TryToUInt( n ) )
               if ( n > 0 && n <= uint16_max )
                  blockAlignmentSize = uint16( n );
         }
         else if ( *i == "no-block-alignment" )
            blockAlignmentSize = uint16( 1 );
         else if ( *i == "max-inline-block-size" )
         {
            if ( ++i == theHints.End() )
               break;
            unsigned n;
            if ( i->TryToUInt( n ) )
               if ( n > 0 && n <= uint16_max )
                  maxInlineBlockSize = uint16( n );
         }
         else if ( *i == "no-embedded-data" )
            embeddedData = false;
         else if ( *i == "embedded-data" )
            embeddedData = true;
         else if ( *i == "cfa-type" )
         {
            if ( ++i == theHints.End() )
               break;
            cfa = *i;
         }
         else if ( *i == "no-cfa" )
            cfa = IsoString();
         else if ( *i == "verbosity" )
         {
            if ( ++i == theHints.End() )
               break;
            int n;
            if ( i->TryToInt( n ) )
               verbosity = Range( n, 0, 3 );
         }
         else if ( *i == "normalize" )
            normalize = true;
         else if ( *i == "no-normalize" )
            normalize = false;
         else if ( *i == "resolution" )
         {
            if ( ++i == theHints.End() )
               break;
            double r;
            if ( i->TryToDouble( r ) )
               if ( r >= 0 )
                  resolution = r;
         }
         else if ( *i == "no-resolution" )
            resolution = 0;
         else if ( *i == "resolution-unit" )
         {
            if ( ++i == theHints.End() )
               break;
            resolutionUnit = *i;
         }
         else if ( *i == "image-ids" )
         {
            if ( ++i == theHints.End() )
               break;
            i->Break( imageIds, ',', true/*trim*/ );
         }
         else if ( *i == "lower-bound" )
         {
            if ( ++i == theHints.End() )
               break;
            double l;
            if ( i->TryToDouble( l ) )
               outputLowerBound = l;
         }
         else if ( *i == "upper-bound" )
         {
            if ( ++i == theHints.End() )
               break;
            double u;
            if ( i->TryToDouble( u ) )
               outputUpperBound = u;
         }
         else if ( *i == "no-warnings" )
            noWarnings = true;
         else if ( *i == "warnings" )
            noWarnings = false;
         else if ( *i == "warnings-are-errors" )
            warningsAreErrors = true;
         else if ( *i == "no-warnings-are-errors" )
            warningsAreErrors = false;
      }
   }

   XISFStreamHints() = delete;
   XISFStreamHints( const XISFStreamHints& ) = delete;

   IsoString ToHintsString() const
   {
      IsoStringList hints;

      if ( properties.HasChanged() )
         hints << (properties.Value() ? "properties" : "no-properties");

      if ( fitsKeywords.HasChanged() )
         hints << (fitsKeywords.Value() ? "fits-keywords" : "no-fits-keywords");

      if ( importFITSKeywords.HasChanged() )
         hints << (importFITSKeywords.Value() ? "import-fits-keywords" : "no-import-fits-keywords");

      if ( autoMetadata.HasChanged() )
         hints << (autoMetadata.Value() ? "auto-metadata" : "no-auto-metadata");

      if ( compressionCodec.HasChanged() )
         hints << ((compressionCodec.Value() != XISFCompression::None) ?
                   "compression-codec " + IsoString( XISF::CompressionCodecId( compressionCodec.Value() ) ) :
                   "no-compression");

      if ( compressionLevel.HasChanged() && compressionCodec.Value() != XISFCompression::None )
         hints << "compression-level " + IsoString( compressionLevel.Value() );

      if ( checksumAlgorithm.HasChanged() )
         hints << ((checksumAlgorithm.Value() != XISFChecksum::None) ?
                   "checksums " + IsoString( XISF::ChecksumAlgorithmId( checksumAlgorithm.Value() ) ) :
                   "no-checksums");

      if ( blockAlignmentSize.HasChanged() )
         hints << ((blockAlignmentSize.Value() > 1) ?
                   "block-alignment " + IsoString( blockAlignmentSize.Value() ) :
                   "no-block-alignment");

      if ( maxInlineBlockSize.HasChanged() )
         hints << "max-inline-block-size " + IsoString( maxInlineBlockSize.Value() );

      if ( embeddedData.HasChanged() )
         hints << (embeddedData.Value() ? "embedded-data" : "no-embedded-data");

      if ( cfa.HasChanged() )
         hints << (cfa.Value().IsEmpty() ? "no-cfa" : "cfa " + cfa.Value());

      if ( normalize.HasChanged() )
         hints << (normalize.Value() ? "normalize" : "no-normalize");

      if ( resolution.HasChanged() )
         hints << ((resolution.Value() > 0) ?
                   "resolution " + IsoString( resolution.Value() ) :
                   "no-resolution");

      if ( resolutionUnit.HasChanged() )
         hints << "resolution-unit " + resolutionUnit.Value();

      if ( verbosity.HasChanged() )
         hints << "verbosity " + IsoString( verbosity.Value() );

      if ( !imageIds.IsEmpty() )
         hints << "image-ids " + IsoString().ToCommaSeparated( imageIds );

      if ( outputLowerBound.HasChanged() )
         hints << "lower-bound " + IsoString( outputLowerBound.Value() );

      if ( outputUpperBound.HasChanged() )
         hints << "upper-bound " + IsoString( outputUpperBound.Value() );

      if ( noWarnings.HasChanged() )
         hints << (noWarnings.Value() ? "no-warnings" : "warnings");

      if ( warningsAreErrors.HasChanged() )
         hints << (warningsAreErrors.Value() ? "warnings-are-errors" : "no-warnings-are-errors");

      return IsoString().ToSpaceSeparated( hints );
   }

   void ApplyReadHints( ImageOptions& options ) const
   {
      if ( normalize.HasChanged() )
         options.readNormalized = normalize;
   }

   void ApplyReadHints( XISFOptions& options ) const
   {
      if ( fitsKeywords.HasChanged() )
         options.ignoreFITSKeywords = !fitsKeywords;
      if ( importFITSKeywords.HasChanged() )
         options.importFITSKeywords = importFITSKeywords;
      if ( autoMetadata.HasChanged() )
         options.autoMetadata = autoMetadata;
      if ( embeddedData.HasChanged() )
         options.ignoreEmbeddedData = embeddedData;
      if ( properties.HasChanged() )
         options.ignoreProperties = properties;
      if ( verbosity.HasChanged() )
         options.verbosity = verbosity;
      if ( noWarnings.HasChanged() )
         options.noWarnings = noWarnings;
      if ( warningsAreErrors.HasChanged() )
         options.warningsAreErrors = warningsAreErrors;
   }

   void ApplyWriteHints( ImageOptions& options ) const
   {
      if ( properties.HasChanged() )
         options.embedProperties = properties;
//       if ( cfa.HasChanged() )
//          options.cfaType = XISF::CFATypeFromId( cfa );
      if ( resolution.HasChanged() )
         options.xResolution = options.yResolution = resolution;
      if ( resolutionUnit.HasChanged() )
         if ( resolutionUnit.Value().CompareIC( "cm" ) == 0 )
            options.metricResolution = true;
         else if ( resolutionUnit.Value().CompareIC( "inch" ) == 0 )
            options.metricResolution = false;
   }

   void ApplyWriteHints( XISFOptions& options ) const
   {
      if ( fitsKeywords.HasChanged() )
         options.storeFITSKeywords = fitsKeywords;
      if ( compressionCodec.HasChanged() )
         options.compressionCodec = compressionCodec;
      if ( compressionLevel.HasChanged() )
         options.compressionLevel = compressionLevel;
      if ( checksumAlgorithm.HasChanged() )
         options.checksumAlgorithm = checksumAlgorithm;
      if ( blockAlignmentSize.HasChanged() )
         options.blockAlignmentSize = blockAlignmentSize;
      if ( maxInlineBlockSize.HasChanged() )
         options.maxInlineBlockSize = maxInlineBlockSize;
      if ( verbosity.HasChanged() )
         options.verbosity = verbosity;
      if ( outputLowerBound.HasChanged() )
         options.outputLowerBound = outputLowerBound;
      if ( outputUpperBound.HasChanged() )
         options.outputUpperBound = outputUpperBound;
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void XISFInstance::LogHandler::Init( const String& filePath, bool writing )
{
   m_console.Write( "<end><cbr>" );
}

void XISFInstance::LogHandler::Log( const String& text, XISFLogHandler::message_type type )
{
   m_console.Write( "<end>" );
   switch ( type )
   {
   default:
   case XISFMessageType::Informative:
      m_console.Write( text );
      break;
   case XISFMessageType::Note:
      m_console.Note( "* " + text );
      break;
   case XISFMessageType::Warning:
      m_console.Warning( "** " + text );
      break;
   case XISFMessageType::RecoverableError:
      m_console.Critical( "*** " + text );
      break;
   }
   Module->ProcessEvents();
}

void XISFInstance::LogHandler::Close()
{
   m_console.Write( "<end><cbr>" );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

template <class S>
static void CheckOpenStream( const S& stream, const String& memberFunc )
{
   if ( !stream || !stream->IsOpen() )
      throw Error( "XISFInstance::" + memberFunc + "(): Illegal request on a closed stream." );
}

// ----------------------------------------------------------------------------

XISFInstance::XISFInstance( const XISFFormat* F ) :
   FileFormatImplementation( F ),
   m_queriedOptions( false )
{
}

// ----------------------------------------------------------------------------

XISFInstance::~XISFInstance()
{
   Close();
}

// ----------------------------------------------------------------------------

ImageDescriptionArray XISFInstance::Open( const String& filePath, const IsoString& hints )
{
   Close();

   try
   {
      Exception::EnableConsoleOutput();

      m_reader = new XISFReader;

      XISFOptions xisfOptions = XISFFormat::DefaultOptions();

      if ( !hints.IsEmpty() )
      {
         m_readHints = new XISFStreamHints( hints );
         m_readHints->ApplyReadHints( xisfOptions );
         m_reader->SetHints( m_readHints->ToHintsString() );
      }

      m_reader->SetOptions( xisfOptions );
      m_reader->SetLogHandler( new LogHandler );
      m_reader->Open( filePath );

      if ( m_reader->NumberOfImages() < 1 )
         throw Error( "The XSIF file contains no readable images." );

      ImageDescriptionArray images;
      for ( int i = 0; i < int( m_reader->NumberOfImages() ); ++i )
      {
         m_reader->SelectImage( i );
         images.Append( ImageDescription( m_reader->ImageInfo(), m_reader->ImageOptions(), m_reader->ImageId() ) );
      }
      m_reader->SelectImage( 0 );

      return images;
   }
   catch ( ... )
   {
      Close();
      throw;
   }
}

// ----------------------------------------------------------------------------

bool XISFInstance::IsOpen() const
{
   return m_reader && m_reader->IsOpen() || m_writer && m_writer->IsOpen();
}

// ----------------------------------------------------------------------------

String XISFInstance::FilePath() const
{
   if ( m_reader )
      return m_reader->FilePath();
   if ( m_writer != nullptr )
      return m_writer->FilePath();
   return String();
}

// ----------------------------------------------------------------------------

void XISFInstance::Close()
{
   m_reader.Destroy();
   m_writer.Destroy();
   m_readHints.Destroy();
   m_writeHints.Destroy();
}

// ----------------------------------------------------------------------------

void XISFInstance::SelectImage( int index )
{
   CheckOpenStream( m_reader, "SelectImage" );
   if ( index < 0 || index >= m_reader->NumberOfImages() )
      throw Error( String( "XISFInstance::SelectImage(): " ) + "Attempt to select a nonexistent image." );
   m_reader->SelectImage( index );
}

// ----------------------------------------------------------------------------

int XISFInstance::SelectedImageIndex() const
{
   CheckOpenStream( m_reader, "SelectedImageIndex" );
   return m_reader->SelectedImageIndex();
}

// ----------------------------------------------------------------------------

void* XISFInstance::FormatSpecificData() const
{
   if ( !IsOpen() )
      return nullptr;

   XISFFormat::FormatOptions* data = new XISFFormat::FormatOptions;
   if ( m_reader )
      data->options = m_reader->Options();
   else if ( m_writer )
      data->options = m_writer->Options();
   return data;
}

// ----------------------------------------------------------------------------

ICCProfile XISFInstance::ReadICCProfile()
{
   CheckOpenStream( m_reader, "ReadICCProfile" );
   return m_reader->ReadICCProfile();
}

// ----------------------------------------------------------------------------

RGBColorSystem XISFInstance::ReadRGBWorkingSpace()
{
   CheckOpenStream( m_reader, "ReadRGBWorkingSpace" );
   return m_reader->ReadRGBWorkingSpace();
}

// ----------------------------------------------------------------------------

DisplayFunction XISFInstance::ReadDisplayFunction()
{
   CheckOpenStream( m_reader, "ReadDisplayFunction" );
   return m_reader->ReadDisplayFunction();
}

// ----------------------------------------------------------------------------

ColorFilterArray XISFInstance::ReadColorFilterArray()
{
   CheckOpenStream( m_reader, "ReadColorFilterArray" );
   return m_reader->ReadColorFilterArray();
}

// ----------------------------------------------------------------------------

UInt8Image XISFInstance::ReadThumbnail()
{
   CheckOpenStream( m_reader, "ReadThumbnail" );
   return m_reader->ReadThumbnail();
}

// ----------------------------------------------------------------------------

FITSKeywordArray XISFInstance::ReadFITSKeywords()
{
   /*!
    * N.B.: The PixInsight core application has an option to reload the list of
    * keywords actually embedded after writing a new image. Hence the
    * 'peculiar' implementation below.
    */
   if ( m_reader )
   {
      if ( m_reader->IsOpen() )
         return m_reader->ReadFITSKeywords();
   }
   if ( m_writer )
   {
      if ( m_writer->IsOpen() )
         return m_writer->FITSKeywords();
   }
   CheckOpenStream( m_reader, "ReadFITSKeywords" );
   return FITSKeywordArray();
}

// ----------------------------------------------------------------------------

PropertyDescriptionArray XISFInstance::Properties()
{
   CheckOpenStream( m_reader, "Properties" );
   return m_reader->Properties();
}

// ----------------------------------------------------------------------------

Variant XISFInstance::ReadProperty( const IsoString& id )
{
   CheckOpenStream( m_reader, "ReadProperty" );
   return m_reader->ReadProperty( id );
}

// ----------------------------------------------------------------------------

PropertyDescriptionArray XISFInstance::ImageProperties()
{
   CheckOpenStream( m_reader, "ImageProperties" );
   return m_reader->ImageProperties();
}

// ----------------------------------------------------------------------------

Variant XISFInstance::ReadImageProperty( const IsoString& id )
{
   CheckOpenStream( m_reader, "ReadImageProperty" );
   return m_reader->ReadImageProperty( id );
}

// ----------------------------------------------------------------------------

template <class P>
static void ReadXISFImage( GenericImage<P>& image, XISFReader* reader, const XISFStreamHints* hints )
{
   CheckOpenStream( reader, "ReadImage" );
   if ( hints != nullptr )
   {
      ImageOptions imageOptions;
      hints->ApplyReadHints( imageOptions );
      reader->SetImageOptions( imageOptions );
   }
   reader->ReadImage( image );
}

void XISFInstance::ReadImage( Image& image )
{
   ReadXISFImage( image, m_reader, m_readHints );
}

void XISFInstance::ReadImage( DImage& image )
{
   ReadXISFImage( image, m_reader, m_readHints );
}

void XISFInstance::ReadImage( UInt8Image& image )
{
   ReadXISFImage( image, m_reader, m_readHints );
}

void XISFInstance::ReadImage( UInt16Image& image )
{
   ReadXISFImage( image, m_reader, m_readHints );
}

void XISFInstance::ReadImage( UInt32Image& image )
{
   ReadXISFImage( image, m_reader, m_readHints );
}

// ----------------------------------------------------------------------------

template <class T>
void ReadXISFSamples( T* buffer, int startRow, int rowCount, int channel, XISFReader* reader, const XISFStreamHints* hints )
{
   CheckOpenStream( reader, "ReadSamples" );

   if ( hints )
   {
      ImageOptions imageOptions;
      hints->ApplyReadHints( imageOptions );
      reader->SetImageOptions( imageOptions );
   }

   reader->ReadSamples( buffer, startRow, rowCount, channel );
}

void XISFInstance::ReadSamples( pcl::Image::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadXISFSamples( buffer, startRow, rowCount, channel, m_reader, m_readHints );
}

void XISFInstance::ReadSamples( pcl::DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadXISFSamples( buffer, startRow, rowCount, channel, m_reader, m_readHints );
}

void XISFInstance::ReadSamples( UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadXISFSamples( buffer, startRow, rowCount, channel, m_reader, m_readHints );
}

void XISFInstance::ReadSamples( UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadXISFSamples( buffer, startRow, rowCount, channel, m_reader, m_readHints );
}

void XISFInstance::ReadSamples( UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadXISFSamples( buffer, startRow, rowCount, channel, m_reader, m_readHints );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool XISFInstance::QueryOptions( Array<ImageOptions>& imageOptions, Array<void*>& formatOptions )
{
   m_queriedOptions = true;

   // Format-independent options

   ImageOptions options;
   if ( !imageOptions.IsEmpty() )
      options = *imageOptions;

   // Format-specific options

   AutoPointer<XISFFormat::FormatOptions> xisfFormatOptions;
   if ( !formatOptions.IsEmpty() )
      xisfFormatOptions = XISFFormat::FormatOptions::FromGenericDataBlock( *formatOptions );

   if ( xisfFormatOptions.IsNull() )
      xisfFormatOptions = new XISFFormat::FormatOptions;
   else
      xisfFormatOptions.DisableAutoDelete();

   // Override embedding options, if requested.

   XISFFormat::EmbeddingOverrides overrides = XISFFormat::DefaultEmbeddingOverrides();

   if ( overrides.overridePropertyEmbedding )
      options.embedProperties = overrides.embedProperties;

   if ( overrides.overrideICCProfileEmbedding )
      options.embedICCProfile = overrides.embedICCProfiles;

   if ( overrides.overrideDisplayFunctionEmbedding )
      options.embedDisplayFunction = overrides.embedDisplayFunctions;

   if ( overrides.overrideRGBWorkingSpaceEmbedding )
      options.embedRGBWS = overrides.embedRGBWorkingSpaces;

   if ( overrides.overrideThumbnailEmbedding )
      options.embedThumbnail = overrides.embedThumbnails;

   if ( overrides.overridePreviewRectsEmbedding )
      options.embedPreviewRects = overrides.embedPreviewRects;

   // Execute the XISF Options dialog

   XISFOptionsDialog dlg( options, xisfFormatOptions->options );
   if ( dlg.Execute() != StdDialogCode::Ok )
      return false;

   if ( imageOptions.IsEmpty() )
      imageOptions.Append( dlg.imageOptions );
   else
      *imageOptions = dlg.imageOptions;

   xisfFormatOptions->options = dlg.options;
   if ( formatOptions.IsEmpty() )
      formatOptions.Add( reinterpret_cast<void*>( xisfFormatOptions.Pointer() ) );
   else
      *formatOptions = reinterpret_cast<void*>( xisfFormatOptions.Pointer() );

   xisfFormatOptions.Release();

   return true;
}

// ----------------------------------------------------------------------------

void XISFInstance::Create( const String& filePath, int numberOfImages, const IsoString& hints )
{
   Close();

   Exception::EnableConsoleOutput();

   m_writer = new XISFWriter;
   m_writer->SetLogHandler( new LogHandler );

   XISFOptions xisfOptions = XISFFormat::DefaultOptions();
   ImageOptions imageOptions;

   XISFFormat::EmbeddingOverrides overrides = XISFFormat::DefaultEmbeddingOverrides();
   if ( overrides.overridePropertyEmbedding )
      imageOptions.embedProperties = overrides.embedProperties;
   if ( overrides.overrideICCProfileEmbedding )
      imageOptions.embedICCProfile = overrides.embedICCProfiles;
   if ( overrides.overrideDisplayFunctionEmbedding )
      imageOptions.embedDisplayFunction = overrides.embedDisplayFunctions;
   if ( overrides.overrideRGBWorkingSpaceEmbedding )
      imageOptions.embedRGBWS = overrides.embedRGBWorkingSpaces;
   if ( overrides.overrideThumbnailEmbedding )
      imageOptions.embedThumbnail = overrides.embedThumbnails;
   if ( overrides.overridePreviewRectsEmbedding )
      imageOptions.embedPreviewRects = overrides.embedPreviewRects;

   if ( !hints.IsEmpty() )
   {
      m_writeHints = new XISFStreamHints( hints );
      m_writeHints->ApplyWriteHints( xisfOptions );
      m_writeHints->ApplyWriteHints( imageOptions );
      m_writer->SetHints( m_writeHints->ToHintsString() );
   }

   m_writer->SetOptions( xisfOptions );
   m_writer->Create( filePath, numberOfImages );
   m_writer->SetImageOptions( imageOptions );
}

// ----------------------------------------------------------------------------

void XISFInstance::SetId( const IsoString& id )
{
   CheckOpenStream( m_writer, "SetId" );
   m_writer->SetImageId( id );
}

// ----------------------------------------------------------------------------

void XISFInstance::SetOptions( const ImageOptions& options )
{
   CheckOpenStream( m_writer, "SetOptions" );

   ImageOptions imageOptions = options;

   if ( !m_queriedOptions )
   {
      XISFFormat::EmbeddingOverrides overrides = XISFFormat::DefaultEmbeddingOverrides();
      if ( overrides.overridePropertyEmbedding )
         imageOptions.embedProperties = overrides.embedProperties;
      if ( overrides.overrideICCProfileEmbedding )
         imageOptions.embedICCProfile = overrides.embedICCProfiles;
      if ( overrides.overrideDisplayFunctionEmbedding )
         imageOptions.embedDisplayFunction = overrides.embedDisplayFunctions;
      if ( overrides.overrideRGBWorkingSpaceEmbedding )
         imageOptions.embedRGBWS = overrides.embedRGBWorkingSpaces;
      if ( overrides.overrideThumbnailEmbedding )
         imageOptions.embedThumbnail = overrides.embedThumbnails;
      if ( overrides.overridePreviewRectsEmbedding )
         imageOptions.embedPreviewRects = overrides.embedPreviewRects;
   }

   if ( m_writeHints )
      m_writeHints->ApplyWriteHints( imageOptions );

   m_writer->SetImageOptions( imageOptions );
}

// ----------------------------------------------------------------------------

void XISFInstance::SetFormatSpecificData( const void* data )
{
   CheckOpenStream( m_writer, "SetFormatSpecificData" );

   const XISFFormat::FormatOptions* formatOptions = XISFFormat::FormatOptions::FromGenericDataBlock( data );
   if ( formatOptions != nullptr )
   {
      XISFOptions xisfOptions = formatOptions->options;
      if ( m_writeHints )
         m_writeHints->ApplyWriteHints( xisfOptions );
      m_writer->SetOptions( xisfOptions );
   }
}

// ----------------------------------------------------------------------------

void XISFInstance::WriteICCProfile( const ICCProfile& icc )
{
   CheckOpenStream( m_writer, "WriteICCProfile" );
   m_writer->WriteICCProfile( icc );
}

// ----------------------------------------------------------------------------

void XISFInstance::WriteRGBWorkingSpace( const RGBColorSystem& rgbws )
{
   CheckOpenStream( m_writer, "WriteRGBWorkingSpace" );
   m_writer->WriteRGBWorkingSpace( rgbws );
}

// ----------------------------------------------------------------------------

void XISFInstance::WriteDisplayFunction( const DisplayFunction& df )
{
   CheckOpenStream( m_writer, "WriteDisplayFunction" );
   m_writer->WriteDisplayFunction( df );
}

// ----------------------------------------------------------------------------

void XISFInstance::WriteColorFilterArray( const ColorFilterArray& cfa )
{
   CheckOpenStream( m_writer, "WriteColorFilterArray" );
   m_writer->WriteColorFilterArray( cfa );
}

// ----------------------------------------------------------------------------

void XISFInstance::WriteThumbnail( const pcl::UInt8Image& thumbnail )
{
   CheckOpenStream( m_writer, "WriteThumbnail" );
   m_writer->WriteThumbnail( thumbnail );
}

// ----------------------------------------------------------------------------

void XISFInstance::WriteFITSKeywords( const FITSKeywordArray& keywords )
{
   CheckOpenStream( m_writer, "WriteFITSKeywords" );
   m_writer->WriteFITSKeywords( keywords );
}

// ----------------------------------------------------------------------------

void XISFInstance::WriteProperty( const IsoString& id, const Variant& value )
{
   CheckOpenStream( m_writer, "WriteProperty" );
   m_writer->WriteProperty( id, value );
}

// ----------------------------------------------------------------------------

void XISFInstance::WriteImageProperty( const IsoString& id, const Variant& value )
{
   CheckOpenStream( m_writer, "WriteImageProperty" );
   m_writer->WriteImageProperty( id, value );
}

// ----------------------------------------------------------------------------

template <class P>
static void WriteXISFImage( const GenericImage<P>& image, XISFWriter* writer, XISFStreamHints* hints )
{
   CheckOpenStream( writer, "WriteImage" );
   if ( hints )
      if ( hints->imageIdIndex < int( hints->imageIds.Length() ) )
         writer->SetImageId( hints->imageIds[hints->imageIdIndex++] );
   writer->WriteImage( image );
}

void XISFInstance::WriteImage( const Image& image )
{
   WriteXISFImage( image, m_writer, m_writeHints );
}

void XISFInstance::WriteImage( const DImage& image )
{
   WriteXISFImage( image, m_writer, m_writeHints );
}

void XISFInstance::WriteImage( const UInt8Image& image )
{
   WriteXISFImage( image, m_writer, m_writeHints );
}

void XISFInstance::WriteImage( const UInt16Image& image )
{
   WriteXISFImage( image, m_writer, m_writeHints );
}

void XISFInstance::WriteImage( const UInt32Image& image )
{
   WriteXISFImage( image, m_writer, m_writeHints );
}

// ----------------------------------------------------------------------------

void XISFInstance::CreateImage( const ImageInfo& info )
{
   CheckOpenStream( m_writer, "CreateImage" );
   if ( m_writeHints )
      if ( m_writeHints->imageIdIndex < int( m_writeHints->imageIds.Length() ) )
         m_writer->SetImageId( m_writeHints->imageIds[m_writeHints->imageIdIndex++] );
  m_writer->CreateImage( info );
}

// ----------------------------------------------------------------------------

template <class T>
void WriteXISFSamples( const T* buffer, int startRow, int rowCount, int channel, XISFWriter* writer )
{
   CheckOpenStream( writer, "WriteSamples" );
   writer->WriteSamples( buffer, startRow, rowCount, channel );
}

void XISFInstance::WriteSamples( const pcl::Image::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteXISFSamples( buffer, startRow, rowCount, channel, m_writer );
}

void XISFInstance::WriteSamples( const pcl::DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteXISFSamples( buffer, startRow, rowCount, channel, m_writer );
}

void XISFInstance::WriteSamples( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteXISFSamples( buffer, startRow, rowCount, channel, m_writer );
}

void XISFInstance::WriteSamples( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteXISFSamples( buffer, startRow, rowCount, channel, m_writer );
}

void XISFInstance::WriteSamples( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteXISFSamples( buffer, startRow, rowCount, channel, m_writer );
}

// ----------------------------------------------------------------------------

void XISFInstance::CloseImage()
{
   CheckOpenStream( m_writer, "CloseImage" );
  m_writer->CloseImage();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF XISFInstance.cpp - Released 2017-04-14T23:07:03Z
