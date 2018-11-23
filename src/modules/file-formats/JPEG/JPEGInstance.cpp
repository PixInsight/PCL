//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard JPEG File Format Module Version 01.00.04.0358
// ----------------------------------------------------------------------------
// JPEGInstance.cpp - Released 2018-11-23T16:14:51Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG PixInsight module.
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

#include "JPEGInstance.h"
#include "JPEGFormat.h"
#include "JPEGOptionsDialog.h"

#include <pcl/StdStatus.h>
#include <pcl/ErrorHandler.h>

namespace pcl
{

// ----------------------------------------------------------------------------

JPEGInstance::JPEGInstance( const JPEGFormat* f ) :
   FileFormatImplementation( f )
{
}

// ----------------------------------------------------------------------------

JPEGInstance::~JPEGInstance() noexcept( false )
{
   Close();
}

// ----------------------------------------------------------------------------

template <class S>
static void CheckOpenStream( const S& stream, const String& memberFunc )
{
   if ( !stream || !stream->IsOpen() )
      throw Error( "JPEGInstance::" + memberFunc + "(): Illegal request on a closed stream." );
}

// ----------------------------------------------------------------------------

ImageDescriptionArray JPEGInstance::Open( const String& filePath, const IsoString& hints )
{
   Close();

   try
   {
      Exception::EnableConsoleOutput();

      JPEGImageOptions jpegOptions = JPEGFormat::DefaultOptions();
      IsoStringList theHints;
      hints.Break( theHints, ' ', true/*trim*/ );
      theHints.Remove( IsoString() );
      for ( IsoStringList::const_iterator i = theHints.Begin(); i < theHints.End(); ++i )
         if ( *i == "verbosity" )
         {
            if ( ++i == theHints.End() )
               break;
            int n;
            if ( i->TryToInt( n ) )
               jpegOptions.verbosity = Range( n, 0, 3 );
         }

      m_reader = new JPEGReader;
      m_reader->SetJPEGOptions( jpegOptions );
      m_reader->Open( filePath );
      return ImageDescriptionArray() << ImageDescription( m_reader->Info(), m_reader->Options() );
   }
   catch ( ... )
   {
      Close();
      throw;
   }
}

// ----------------------------------------------------------------------------

bool JPEGInstance::IsOpen() const
{
   return m_reader && m_reader->IsOpen() || m_writer && m_writer->IsOpen();
}

// ----------------------------------------------------------------------------

String JPEGInstance::FilePath() const
{
   if ( m_reader )
      return m_reader->Path();
   if ( m_writer )
      return m_writer->Path();
   return String();
}

// ----------------------------------------------------------------------------

void JPEGInstance::Close()
{
   m_reader.Destroy();
   m_writer.Destroy();
   m_queriedOptions = false;
   m_readCount = 0;
}

// ----------------------------------------------------------------------------

void* JPEGInstance::FormatSpecificData() const
{
   if ( !IsOpen() )
      return nullptr;

   JPEGFormat::FormatOptions* data = new JPEGFormat::FormatOptions;
   if ( m_reader )
      data->options = m_reader->JPEGOptions();
   else if ( m_writer )
      data->options = m_writer->JPEGOptions();
   return data;
}

// ----------------------------------------------------------------------------

String JPEGInstance::ImageFormatInfo() const
{
   JPEGImageOptions options;
   if ( m_reader )
      options = m_reader->JPEGOptions();
   else if ( m_writer )
      options = m_writer->JPEGOptions();
   else
      return String();

   return String().Format( "quality=%d coding=%s progressive=%s JFIF-version=%d.%d",
                           int( options.quality ),
                           options.arithmeticCoding ? "arithmetic" : "Huffman",
                           options.progressive ? "yes" : "no",
                           int( options.JFIFMajorVersion ), int( options.JFIFMinorVersion ) );
}

// ----------------------------------------------------------------------------

ICCProfile JPEGInstance::ReadICCProfile()
{
   CheckOpenStream( m_reader, "ReadICCProfile" );
   ICCProfile icc = m_reader->ReadICCProfile();
   if ( icc.IsProfile() )
      if ( m_reader->JPEGOptions().verbosity > 0 )
         Console().WriteLn( "<end><cbr>ICC profile extracted: \'" + icc.Description() + "\', " + String( icc.ProfileSize() ) + " bytes." );
   return icc;
}

// ----------------------------------------------------------------------------

template <class P>
static void ReadJPEGImage( GenericImage<P>& image, JPEGReader* reader, int& readCount )
{
   CheckOpenStream( reader, "ReadImage" );

   /*
    * The readCount thing is a trick to allow reading the same JPEG image
    * multiple times from the same format instance. That does not work with
    * jpeglib. An ugly trick, but, heck, it works.
    */
   if ( readCount++ )
   {
      AutoPointer<JPEGReader> newReader( new JPEGReader );
      newReader->Open( reader->Path() );
      newReader->ReadImage( image );
   }
   else
      reader->ReadImage( image );
}

void JPEGInstance::ReadImage( Image& img )
{
   ReadJPEGImage( img, m_reader, m_readCount );
}

void JPEGInstance::ReadImage( DImage& img )
{
   ReadJPEGImage( img, m_reader, m_readCount );
}

void JPEGInstance::ReadImage( UInt8Image& img )
{
   ReadJPEGImage( img, m_reader, m_readCount );
}

void JPEGInstance::ReadImage( UInt16Image& img )
{
   ReadJPEGImage( img, m_reader, m_readCount );
}

void JPEGInstance::ReadImage( UInt32Image& img )
{
   ReadJPEGImage( img, m_reader, m_readCount );
}

// ----------------------------------------------------------------------------

bool JPEGInstance::QueryOptions( Array<ImageOptions>& imageOptions, Array<void*>& formatOptions )
{
   m_queriedOptions = true;

   /*
    * Format-independent options
    */
   ImageOptions options;
   if ( !imageOptions.IsEmpty() )
      options = *imageOptions;

   /*
    * Format-specific options
    */
   JPEGFormat::FormatOptions* jpeg = nullptr;

   if ( !formatOptions.IsEmpty() )
   {
      JPEGFormat::FormatOptions* o = JPEGFormat::FormatOptions::FromGenericDataBlock( *formatOptions );
      if ( o != nullptr )
         jpeg = o;
   }

   bool reusedFormatOptions = jpeg != nullptr;
   if ( !reusedFormatOptions )
      jpeg = new JPEGFormat::FormatOptions;

   /*
    * Override embedding options, if requested.
    */
   JPEGFormat::EmbeddingOverrides overrides = JPEGFormat::DefaultEmbeddingOverrides();

   if ( overrides.overrideICCProfileEmbedding )
      options.embedICCProfile = overrides.embedICCProfiles;

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

void JPEGInstance::Create( const String& filePath, int numberOfImages, const IsoString& hints )
{
   Close();

   Exception::EnableConsoleOutput();

   m_writer = new JPEGWriter;
   m_writer->Create( filePath );

   JPEGImageOptions jpegOptions = JPEGFormat::DefaultOptions();

   IsoStringList theHints;
   hints.Break( theHints, ' ', true/*trim*/ );
   theHints.Remove( IsoString() );
   for ( IsoStringList::const_iterator i = theHints.Begin(); i != theHints.End(); ++i )
   {
      if ( *i == "quality" )
      {
         if ( ++i == theHints.End() )
            break;
         int q = jpegOptions.quality;
         if ( i->TryToInt( q ) )
            jpegOptions.quality = Range( q, 0, 100 );
      }
      else if ( *i == "optimized" )
         jpegOptions.optimizedCoding = true;
      else if ( *i == "no-optimized" )
         jpegOptions.optimizedCoding = false;
      else if ( *i == "arithmetic" )
         jpegOptions.arithmeticCoding = true;
      else if ( *i == "huffman" )
         jpegOptions.arithmeticCoding = false;
      else if ( *i == "progressive" )
         jpegOptions.progressive = true;
      else if ( *i == "no-progressive" )
         jpegOptions.progressive = false;
      else if ( *i == "verbosity" )
      {
         if ( ++i == theHints.End() )
            break;
         int n;
         if ( i->TryToInt( n ) )
            jpegOptions.verbosity = Range( n, 0, 3 );
      }
   }

   m_writer->SetJPEGOptions( jpegOptions );
}

// ----------------------------------------------------------------------------

void JPEGInstance::SetOptions( const ImageOptions& newOptions )
{
   CheckOpenStream( m_writer, "SetOptions" );
   ImageOptions options = newOptions;
   if ( !m_queriedOptions )
   {
      JPEGFormat::EmbeddingOverrides overrides = JPEGFormat::DefaultEmbeddingOverrides();
      if ( overrides.overrideICCProfileEmbedding )
         options.embedICCProfile = overrides.embedICCProfiles;
   }
   m_writer->SetOptions( options );
}

// ----------------------------------------------------------------------------

void JPEGInstance::SetFormatSpecificData( const void* data )
{
   CheckOpenStream( m_writer, "SetFormatSpecificData" );
   const JPEGFormat::FormatOptions* o = JPEGFormat::FormatOptions::FromGenericDataBlock( data );
   if ( o != nullptr )
      m_writer->SetJPEGOptions( o->options );
}

// ----------------------------------------------------------------------------

void JPEGInstance::WriteICCProfile( const ICCProfile& icc )
{
   CheckOpenStream( m_writer, "WriteICCProfile" );
   if ( icc.IsProfile() )
   {
      m_writer->WriteICCProfile( icc );
      if ( m_writer->JPEGOptions().verbosity > 0 )
         Console().WriteLn( "<end><cbr>ICC profile embedded: \'" + icc.Description() + "\', " + String( icc.ProfileSize() ) + " bytes." );
   }
}

// ----------------------------------------------------------------------------

template <class P>
static void WriteJPEGImage( const GenericImage<P>& image, JPEGWriter* writer )
{
   CheckOpenStream( writer, "WriteImage" );
   image.SelectNominalChannels(); // JPEG doesn't support alpha channels
   writer->WriteImage( image );
}

void JPEGInstance::WriteImage( const Image& img )
{
   WriteJPEGImage( img, m_writer );
}

void JPEGInstance::WriteImage( const DImage& img )
{
   WriteJPEGImage( img, m_writer );
}

void JPEGInstance::WriteImage( const UInt8Image& img )
{
   WriteJPEGImage( img, m_writer );
}

void JPEGInstance::WriteImage( const UInt16Image& img )
{
   WriteJPEGImage( img, m_writer );
}

void JPEGInstance::WriteImage( const UInt32Image& img )
{
   WriteJPEGImage( img, m_writer );
}

bool JPEGInstance::WasLossyWrite() const
{
   return true; // in case you didn't know :)
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF JPEGInstance.cpp - Released 2018-11-23T16:14:51Z
