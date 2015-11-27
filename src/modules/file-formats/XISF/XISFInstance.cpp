//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard XISF File Format Module Version 01.00.04.0094
// ----------------------------------------------------------------------------
// XISFInstance.cpp - Released 2015/11/26 15:59:58 UTC
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

#include "XISFInstance.h"
#include "XISFFormat.h"
#include "XISFOptionsDialog.h"

#include <pcl/AutoPointer.h>
#include <pcl/ErrorHandler.h>
#include <pcl/StdStatus.h>

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

   HintValue<bool>      properties;
   HintValue<bool>      fitsKeywords;
   HintValue<bool>      importFITSKeywords;
   HintValue<bool>      autoMetadata;
   HintValue<int>       compressionMethod;
   HintValue<int>       compressionLevel;
   HintValue<int>       checksumMethod;
   HintValue<unsigned>  blockAlignmentSize;
   HintValue<unsigned>  maxInlineBlockSize;
   HintValue<bool>      embeddedData;
   HintValue<IsoString> cfa;
   HintValue<bool>      normalize;
   HintValue<double>    resolution;
   HintValue<IsoString> resolutionUnit;
   HintValue<int>       verbosity;
   HintValue<bool>      noWarnings;
   HintValue<bool>      warningsAreErrors;
   IsoStringList        imageIds;
   int                  imageIdIndex;
   HintValue<double>    outputLowerBound;
   HintValue<double>    outputUpperBound;

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
            int n = XISFEngineBase::CompressionMethodFromId( *i );
            if ( n != XISF_COMPRESSION_UNKNOWN )
               compressionMethod = n;
         }
         else if ( *i == "compression-level" )
         {
            if ( ++i == theHints.End() )
               break;
            int n;
            if ( i->TryToInt( n ) )
               if ( n >= 0 ) // 0=default
                  compressionLevel = Range( n, XISF_COMPRESSION_LEVEL_DEFAULT, XISF_COMPRESSION_LEVEL_MAX );
         }
         else if ( *i == "compress-data" ) // (deprecated) = compression-codec zlib
            compressionMethod = XISF_COMPRESSION_ZLIB;
         else if ( *i == "no-compression" ||
                   *i == "no-compress-data" ) // (deprecated) = no-compression
            compressionMethod = XISF_COMPRESSION_NONE;
         else if ( *i == "checksums" )
         {
            if ( ++i == theHints.End() )
               break;
            int n = XISFEngineBase::ChecksumMethodFromId( *i );
            if ( n != XISF_CHECKSUM_UNKNOWN )
               checksumMethod = n;
         }
         else if ( *i == "no-checksums" )
            checksumMethod = XISF_CHECKSUM_NONE;
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
               verbosity = Max( 0, n );
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

      if ( compressionMethod.HasChanged() )
         hints << ((compressionMethod.Value() != XISF_COMPRESSION_NONE) ?
                   "compression-codec " + IsoString( XISFEngineBase::CompressionMethodId( compressionMethod.Value() ) ) :
                   "no-compression");

      if ( compressionLevel.HasChanged() && compressionMethod.Value() != XISF_COMPRESSION_NONE )
         hints << "compression-level " + IsoString( compressionLevel.Value() );

      if ( checksumMethod.HasChanged() )
         hints << ((checksumMethod.Value() != XISF_CHECKSUM_NONE) ?
                   "checksums " + IsoString( XISFEngineBase::ChecksumMethodId( checksumMethod.Value() ) ) :
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
      if ( cfa.HasChanged() )
         options.cfaType = XISFEngineBase::CFATypeFromId( cfa );
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
      if ( compressionMethod.HasChanged() )
         options.compressionMethod = compressionMethod;
      if ( compressionLevel.HasChanged() )
         options.compressionLevel = compressionLevel;
      if ( checksumMethod.HasChanged() )
         options.checksumMethod = checksumMethod;
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

XISFInstance::XISFInstance( const XISFFormat* F ) :
   FileFormatImplementation( F ),
   m_reader( nullptr ), m_writer( nullptr ),
   m_readHints( nullptr ), m_writeHints( nullptr ),
   m_queriedOptions( false )
{
}

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
      }

      m_reader->SetOptions( xisfOptions );
      m_reader->Open( filePath );

      ImageDescriptionArray images;
      for ( int i = 0; i < int( m_reader->NumberOfImages() ); ++i )
      {
         m_reader->SelectImage( i );
         images.Append( ImageDescription( m_reader->ImageInfo(), m_reader->ImageOptions(), m_reader->ImageId() ) );
      }
      m_reader->SelectImage( 0 );

      Console console;
      StringList warnings = m_reader->Warnings();
      for ( StringList::const_iterator i = warnings.Begin(); i != warnings.End(); ++i )
         console.WarningLn( "** Warning: " + *i );

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
   return m_reader != nullptr && m_reader->IsOpen() || m_writer != nullptr && m_writer->IsOpen();
}

// ----------------------------------------------------------------------------

String XISFInstance::FilePath() const
{
   if ( m_reader != nullptr )
      return m_reader->FilePath();
   if ( m_writer != nullptr )
      return m_writer->FilePath();
   return String();
}

// ----------------------------------------------------------------------------

void XISFInstance::Close()
{
   if ( m_reader != nullptr )
      delete m_reader, m_reader = nullptr;
   if ( m_writer != nullptr )
      delete m_writer, m_writer = nullptr;
   if ( m_readHints != nullptr )
      delete m_readHints, m_readHints = nullptr;
   if ( m_writeHints != nullptr )
      delete m_writeHints, m_writeHints = nullptr;
}

// ----------------------------------------------------------------------------

void XISFInstance::SelectImage( int index )
{
   if ( m_reader == nullptr || !m_reader->IsOpen() )
      throw Error( "XISF: Attempt to select an image before opening a file." );

   if ( int( m_reader->NumberOfImages() ) == 0 && index != 0 ||
        int( m_reader->NumberOfImages() ) <= index )
      throw Error( "XISF: Attempt to select a nonexistent image." );

   m_reader->SelectImage( index );
}

int XISFInstance::SelectedImageIndex() const
{
   if ( m_reader == nullptr || !m_reader->IsOpen() )
      throw Error( "XISF: Attempt to query the selected image before opening a file." );
   return m_reader->SelectedImageIndex();
}

// ----------------------------------------------------------------------------

void* XISFInstance::FormatSpecificData() const
{
   if ( !IsOpen() )
      return nullptr;

   XISFFormat::FormatOptions* data = new XISFFormat::FormatOptions;
   if ( m_reader != nullptr )
      data->options = m_reader->Options();
   else if ( m_writer != nullptr )
      data->options = m_writer->Options();
   return data;
}

// ----------------------------------------------------------------------------

void XISFInstance::Extract( FITSKeywordArray& keywords )
{
   if ( !IsOpen() )
      throw Error( "XISF: Attempt to extract keywords without opening or creating a file." );
   keywords.Clear();
   if ( m_reader != nullptr )
      m_reader->Extract( keywords );
   else
      keywords = m_writer->EmbeddedKeywords();
}

void XISFInstance::Extract( ICCProfile& icc )
{
   if ( !IsOpen() )
      throw Error( "XISF: Attempt to extract an ICC profile without opening or creating a file." );
   icc.Clear();
   if ( m_reader != nullptr )
      m_reader->Extract( icc );
   else
      icc = m_writer->EmbeddedICCProfile();
}

void XISFInstance::Extract( pcl::UInt8Image& thumbnail )
{
   if ( !IsOpen() )
      throw Error( "XISF: Attempt to extract a thumbnail image without opening or creating a file." );
   thumbnail.FreeData();
   if ( m_reader != nullptr )
      m_reader->Extract( thumbnail );
   else
      thumbnail = m_writer->EmbeddedThumbnail();
}

void XISFInstance::Extract( void*& data, size_type& length )
{
   // ### TODO
   data = nullptr;
   length = 0;
}

// ----------------------------------------------------------------------------

template <class P>
static void ReadXISFImage( GenericImage<P>& image, XISFReader* reader, const XISFStreamHints* hints )
{
   if ( reader == nullptr || !reader->IsOpen() )
      throw Error( "XISF: Attempt to read an image before opening a file." );

   if ( hints )
   {
      ImageOptions imageOptions;
      hints->ApplyReadHints( imageOptions );
      reader->SetImageOptions( imageOptions );
   }

   StandardStatus status;
   image.SetStatusCallback( &status );
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
   if ( reader == nullptr || !reader->IsOpen() )
      throw Error( "XISF: Attempt to perform an incremental read operation before opening a file." );

   if ( hints )
   {
      ImageOptions imageOptions;
      hints->ApplyReadHints( imageOptions );
      reader->SetImageOptions( imageOptions );
   }

   reader->ReadSamples( buffer, startRow, rowCount, channel );
}

void XISFInstance::Read( pcl::Image::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadXISFSamples( buffer, startRow, rowCount, channel, m_reader, m_readHints );
}

void XISFInstance::Read( pcl::DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadXISFSamples( buffer, startRow, rowCount, channel, m_reader, m_readHints );
}

void XISFInstance::Read( UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadXISFSamples( buffer, startRow, rowCount, channel, m_reader, m_readHints );
}

void XISFInstance::Read( UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadXISFSamples( buffer, startRow, rowCount, channel, m_reader, m_readHints );
}

void XISFInstance::Read( UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
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

   if ( !hints.IsEmpty() )
   {
      m_writeHints = new XISFStreamHints( hints );
      m_writeHints->ApplyReadHints( xisfOptions );
      m_writeHints->ApplyReadHints( imageOptions );
      m_writer->SetHints( m_writeHints->ToHintsString() );
   }
   else
      m_writer->SetHints( IsoString() );

   m_writer->SetOptions( xisfOptions );
   m_writer->Create( filePath, numberOfImages );
   m_writer->SetImageOptions( imageOptions );
}

// ----------------------------------------------------------------------------

void XISFInstance::SetId( const IsoString& id )
{
   if ( m_writer == nullptr || !m_writer->IsOpen() )
      throw Error( "XISF: Attempt to set an image identifier before creating a file." );
   m_writer->SetImageId( id );
}

void XISFInstance::SetOptions( const ImageOptions& options )
{
   if ( m_writer == nullptr || !m_writer->IsOpen() )
      throw Error( "XISF: Attempt to set image options before creating a file." );

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
   }

   if ( m_writeHints )
      m_writeHints->ApplyWriteHints( imageOptions );

   m_writer->SetImageOptions( imageOptions );
}

void XISFInstance::SetFormatSpecificData( const void* data )
{
   if ( m_writer == nullptr || !m_writer->IsOpen() )
      throw Error( "XISF: Attempt to set image options before creating a file." );

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

void XISFInstance::Embed( const FITSKeywordArray& keywords )
{
   if ( m_writer == nullptr || !m_writer->IsOpen() )
      throw Error( "XISF: Attempt to embed XISF header keywords before creating a file." );
   m_writer->Embed( keywords );
}

void XISFInstance::Embed( const ICCProfile& icc )
{
   if ( m_writer == nullptr || !m_writer->IsOpen() )
      throw Error( "XISF: Attempt to embed an ICC profile before creating a file." );
   m_writer->Embed( icc );
}

void XISFInstance::Embed( const pcl::UInt8Image& thumbnail )
{
   if ( m_writer == nullptr || !m_writer->IsOpen() )
      throw Error( "XISF: Attempt to embed a thumbnail image before creating a file." );
   m_writer->Embed( thumbnail );
}

// ----------------------------------------------------------------------------

template <class P>
static void WriteXISFImage( const GenericImage<P>& image, XISFWriter* writer, XISFStreamHints* hints )
{
   if ( writer == nullptr || !writer->IsOpen() )
      throw Error( "XISF: Attempt to write an image before creating a file." );

   if ( hints )
      if ( hints->imageIdIndex < int( hints->imageIds.Length() ) )
         writer->SetImageId( hints->imageIds[hints->imageIdIndex++] );

   StandardStatus status;
   image.SetStatusCallback( &status );
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

void XISFInstance::CreateImage( const ImageInfo& info )
{
   if ( m_writer == nullptr || !m_writer->IsOpen() )
      throw Error( "XISF: Attempt to create an image before creating a file." );

   if ( m_writeHints )
      if ( m_writeHints->imageIdIndex < int( m_writeHints->imageIds.Length() ) )
         m_writer->SetImageId( m_writeHints->imageIds[m_writeHints->imageIdIndex++] );

  m_writer->CreateImage( info );
}

template <class T>
void WriteXISFSamples( const T* buffer, int startRow, int rowCount, int channel, XISFWriter* writer )
{
   if ( writer == nullptr || !writer->IsOpen() )
      throw Error( "XISF: Attempt to perform an incremental write operation before creating a file." );
   writer->WriteSamples( buffer, startRow, rowCount, channel );
}

void XISFInstance::Write( const pcl::Image::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteXISFSamples( buffer, startRow, rowCount, channel, m_writer );
}

void XISFInstance::Write( const pcl::DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteXISFSamples( buffer, startRow, rowCount, channel, m_writer );
}

void XISFInstance::Write( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteXISFSamples( buffer, startRow, rowCount, channel, m_writer );
}

void XISFInstance::Write( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteXISFSamples( buffer, startRow, rowCount, channel, m_writer );
}

void XISFInstance::Write( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteXISFSamples( buffer, startRow, rowCount, channel, m_writer );
}

// ----------------------------------------------------------------------------

ImagePropertyDescriptionArray XISFInstance::Properties()
{
   if ( m_reader == nullptr )
      throw Error( "XISF: Attempt to get property information without opening an existing file." );
   return m_reader->Properties();
}

Variant XISFInstance::ReadProperty( const IsoString& id )
{
   if ( m_reader == nullptr )
      throw Error( "XISF: Attempt to read a data property without opening an existing file." );
   Variant property;
   m_reader->Extract( property, id );
   return property;
}

void XISFInstance::WriteProperty( const IsoString& id, const Variant& value )
{
   if ( m_writer == nullptr )
      throw Error( "XISF: Attempt to write a data property without creating a new file." );
   m_writer->Embed( value, id );
}

// ----------------------------------------------------------------------------

RGBColorSystem XISFInstance::ReadRGBWS()
{
   if ( m_reader == nullptr )
      throw Error( "XISF: Attempt to read RGB working space parameters without opening an existing file." );
   RGBColorSystem rgbws;
   m_reader->Extract( rgbws );
   return rgbws;
}

void XISFInstance::WriteRGBWS( const RGBColorSystem& rgbws )
{
   if ( m_writer == nullptr )
      throw Error( "XISF: Attempt to write RGB working space parameters without creating a new file." );
   m_writer->Embed( rgbws );
}

// ----------------------------------------------------------------------------

DisplayFunction XISFInstance::ReadDisplayFunction()
{
   if ( m_reader == nullptr )
      throw Error( "XISF: Attempt to read display function parameters without opening an existing file." );
   DisplayFunction df;
   m_reader->Extract( df );
   return df;
}

void XISFInstance::WriteDisplayFunction( const DisplayFunction& df )
{
   if ( m_writer == nullptr )
      throw Error( "XISF: Attempt to write display function parameters without creating a new file." );
   m_writer->Embed( df );
}

// ----------------------------------------------------------------------------

ColorFilterArray XISFInstance::ReadColorFilterArray()
{
   if ( m_reader == nullptr )
      throw Error( "XISF: Attempt to read color filter array parameters without opening an existing file." );
   ColorFilterArray cfa;
   m_reader->Extract( cfa );
   return cfa;
}

void XISFInstance::WriteColorFilterArray( const ColorFilterArray& cfa )
{
   if ( m_writer == nullptr )
      throw Error( "XISF: Attempt to write color filter array parameters without creating a new file." );
   m_writer->Embed( cfa );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF XISFInstance.cpp - Released 2015/11/26 15:59:58 UTC
