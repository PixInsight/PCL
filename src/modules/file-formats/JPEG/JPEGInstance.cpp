// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard JPEG File Format Module Version 01.00.01.0226
// ****************************************************************************
// JPEGInstance.cpp - Released 2014/10/29 07:34:49 UTC
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

#include "JPEGInstance.h"
#include "JPEGFormat.h"
#include "JPEGOptionsDialog.h"

#include <pcl/StdStatus.h>
#include <pcl/ErrorHandler.h>

namespace pcl
{

// ----------------------------------------------------------------------------

JPEGInstance::JPEGInstance( const JPEGFormat* f ) :
FileFormatImplementation( f ), reader( 0 ), writer( 0 ), queriedOptions( false ), embeddedICCProfile( 0 )
{
}

JPEGInstance::~JPEGInstance()
{
   Close();
}

// ----------------------------------------------------------------------------

ImageDescriptionArray JPEGInstance::Open( const String& filePath, const IsoString& /*hints*/ )
{
   Close();

   try
   {
      Exception::EnableConsoleOutput();

      reader = new JPEGReader;
      reader->Open( filePath );

      ImageDescriptionArray a;
      a.Add( ImageDescription( reader->Info(), reader->Options() ) );
      return a;
   }
   catch ( ... )
   {
      if ( reader != 0 )
         delete reader, reader = 0;
      throw;
   }
}

// ----------------------------------------------------------------------------

bool JPEGInstance::IsOpen() const
{
   return reader != 0 && reader->IsOpen() || writer != 0 && writer->IsOpen();
}

// ----------------------------------------------------------------------------

String JPEGInstance::FilePath() const
{
   if ( reader != 0 )
      return reader->Path();
   if ( writer != 0 )
      return writer->Path();
   return String();
}

// ----------------------------------------------------------------------------

void JPEGInstance::Close()
{
   if ( reader != 0 )
      delete reader, reader = 0;
   if ( writer != 0 )
      delete writer, writer = 0;
   if ( embeddedICCProfile != 0 )
      delete embeddedICCProfile, embeddedICCProfile = 0;
}

// ----------------------------------------------------------------------------

void* JPEGInstance::FormatSpecificData() const
{
   if ( !IsOpen() )
      return 0;

   JPEGFormat::FormatOptions* data = new JPEGFormat::FormatOptions;
   if ( reader != 0 )
      data->options = reader->JPEGOptions();
   else if ( writer != 0 )
      data->options = writer->JPEGOptions();
   return data;
}

// ----------------------------------------------------------------------------

String JPEGInstance::ImageProperties() const
{
   if ( !IsOpen() )
      return String();

   JPEGImageOptions options;
   if ( reader != 0 )
      options = reader->JPEGOptions();
   else if ( writer != 0 )
      options = writer->JPEGOptions();

   return String().Format( "quality=%d coding=%s progressive=%s JFIF-version=%d.%d",
                           int( options.quality ),
                           options.arithmeticCoding ? "arithmetic" : "Huffman",
                           options.progressive ? "yes" : "no",
                           int( options.JFIFMajorVersion ), int( options.JFIFMinorVersion ) );
}

// ----------------------------------------------------------------------------

void JPEGInstance::Extract( ICCProfile& icc )
{
   if ( !IsOpen() )
      throw Error( "JPEG format: Attempt to extract an ICC profile without opening or creating a file" );

   if ( reader != 0 )
   {
      reader->Extract( icc );
      if ( icc.IsProfile() )
         Console().WriteLn( "<end><cbr>ICC profile extracted: \'" + icc.Description() + "\', " + String( icc.ProfileSize() ) + " bytes" );
   }
   else if ( writer->EmbeddedICCProfile() != 0 )
      icc = *writer->EmbeddedICCProfile();
}

// ----------------------------------------------------------------------------

template <class P>
static void ReadJPEGImage( GenericImage<P>& image, JPEGReader* reader )
{
   if ( reader == 0 || !reader->IsOpen() )
      throw Error( "JPEG format: Attempt to read an image before opening a file" );

   StandardStatus status;
   image.SetStatusCallback( &status );
   reader->ReadImage( image );
}

void JPEGInstance::ReadImage( Image& img )
{
   ReadJPEGImage( img, reader );
}

void JPEGInstance::ReadImage( DImage& img )
{
   ReadJPEGImage( img, reader );
}

void JPEGInstance::ReadImage( UInt8Image& img )
{
   ReadJPEGImage( img, reader );
}

void JPEGInstance::ReadImage( UInt16Image& img )
{
   ReadJPEGImage( img, reader );
}

void JPEGInstance::ReadImage( UInt32Image& img )
{
   ReadJPEGImage( img, reader );
}

// ----------------------------------------------------------------------------

bool JPEGInstance::QueryOptions( Array<ImageOptions>& imageOptions, Array<void*>& formatOptions )
{
   queriedOptions = true;

   /*
    * Format-independent options
    */
   ImageOptions options;
   if ( !imageOptions.IsEmpty() )
      options = *imageOptions;

   /*
    * Format-specific options
    */
   JPEGFormat::FormatOptions* jpeg = 0;

   if ( !formatOptions.IsEmpty() )
   {
      JPEGFormat::FormatOptions* o = JPEGFormat::FormatOptions::FromGenericDataBlock( *formatOptions );
      if ( o != 0 )
         jpeg = o;
   }

   bool reusedFormatOptions = jpeg != 0;
   if ( !reusedFormatOptions )
      jpeg = new JPEGFormat::FormatOptions;

   /*
    * Override embedding options, if requested.
    */
   JPEGFormat::EmbeddingOverrides overrides = JPEGFormat::DefaultEmbeddingOverrides();

   if ( overrides.overrideICCProfileEmbedding )
      options.embedICCProfile = overrides.embedICCProfiles;

   if ( overrides.overrideMetadataEmbedding )
      options.embedMetadata = overrides.embedMetadata;

   if ( overrides.overrideThumbnailEmbedding )
      options.embedThumbnail = overrides.embedThumbnails;

   JPEGOptionsDialog dlg( options, jpeg->options );

   if ( dlg.Execute() == StdDialogCode::Ok )
   {
      jpeg->options = dlg.jpegOptions;

      if ( imageOptions.IsEmpty() )
         imageOptions.Add( dlg.options );
      else
         *imageOptions = dlg.options;

      if ( formatOptions.IsEmpty() )
         formatOptions.Add( (void*)jpeg );
      else
         *formatOptions = (void*)jpeg;

      return true;
   }

   if ( !reusedFormatOptions )
      delete jpeg;

   return false;
}

// ----------------------------------------------------------------------------

void JPEGInstance::Create( const String& filePath, int /*numberOfImages*/, const IsoString& hints )
{
   Close();

   Exception::EnableConsoleOutput();

   writer = new JPEGWriter;
   writer->Create( filePath );

   JPEGImageOptions options = JPEGFormat::DefaultOptions();

   IsoStringList theHints;
   hints.Break( theHints, ' ', true/*trim*/ );
   theHints.Remove( IsoString() );
   for ( IsoStringList::const_iterator i = theHints.Begin(); i < theHints.End(); ++i )
   {
      if ( *i == "quality" )
      {
         if ( ++i == theHints.End() )
            break;
         int q = options.quality;
         if ( i->TryToInt( q ) )
            options.quality = Range( q, 0, 100 );
      }
      else if ( *i == "optimized" )
         options.optimizedCoding = true;
      else if ( *i == "no-optimized" )
         options.optimizedCoding = false;
      else if ( *i == "arithmetic" )
         options.arithmeticCoding = true;
      else if ( *i == "huffman" )
         options.arithmeticCoding = false;
      else if ( *i == "progressive" )
         options.progressive = true;
      else if ( *i == "no-progressive" )
         options.progressive = false;
   }

   writer->JPEGOptions() = options;
}

// ----------------------------------------------------------------------------

void JPEGInstance::SetOptions( const ImageOptions& options )
{
   if ( writer == 0 || !writer->IsOpen() )
      throw Error( "JPEG format: Attempt to set image options before creating a file" );

   writer->Options() = options;

   if ( !queriedOptions )
   {
      JPEGFormat::EmbeddingOverrides overrides = JPEGFormat::DefaultEmbeddingOverrides();

      if ( overrides.overrideICCProfileEmbedding )
         writer->Options().embedICCProfile = overrides.embedICCProfiles;

      if ( overrides.overrideMetadataEmbedding )
         writer->Options().embedMetadata = overrides.embedMetadata;

      if ( overrides.overrideThumbnailEmbedding )
         writer->Options().embedThumbnail = overrides.embedThumbnails;
   }
}

// ----------------------------------------------------------------------------

void JPEGInstance::SetFormatSpecificData( const void* data )
{
   if ( writer == 0 || !writer->IsOpen() )
      throw Error( "JPEG format: Attempt to set format-specific options before creating a file" );

   const JPEGFormat::FormatOptions* o = JPEGFormat::FormatOptions::FromGenericDataBlock( data );
   if ( o != 0 )
      writer->JPEGOptions() = o->options;
}

// ----------------------------------------------------------------------------

void JPEGInstance::Embed( const ICCProfile& icc )
{
   if ( writer == 0 || !writer->IsOpen() )
      throw Error( "JPEG format: Attempt to embed an ICC profile before creating a file" );

   if ( icc.IsProfile() )
   {
      writer->Embed( *(embeddedICCProfile = new ICCProfile( icc )) );
      Console().WriteLn( "<end><cbr>ICC profile embedded: \'" + icc.Description() + "\', " + String( icc.ProfileSize() ) + " bytes" );
   }
}

// ----------------------------------------------------------------------------

template <class P>
static void WriteJPEGImage( const GenericImage<P>& image, JPEGWriter* writer )
{
   if ( writer == 0 || !writer->IsOpen() )
      throw Error( "JPEG format: Attempt to write an image before creating a file" );

   StandardStatus status;
   image.SetStatusCallback( &status );
   image.SelectNominalChannels(); // JPEG doesn't support alpha channels
   writer->WriteImage( image );
}

void JPEGInstance::WriteImage( const Image& img )
{
   WriteJPEGImage( img, writer );
}

void JPEGInstance::WriteImage( const DImage& img )
{
   WriteJPEGImage( img, writer );
}

void JPEGInstance::WriteImage( const UInt8Image& img )
{
   WriteJPEGImage( img, writer );
}

void JPEGInstance::WriteImage( const UInt16Image& img )
{
   WriteJPEGImage( img, writer );
}

void JPEGInstance::WriteImage( const UInt32Image& img )
{
   WriteJPEGImage( img, writer );
}

bool JPEGInstance::WasLossyWrite() const
{
   return true; // in case you didn't know :)
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF JPEGInstance.cpp - Released 2014/10/29 07:34:49 UTC
