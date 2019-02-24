//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard TIFF File Format Module Version 01.00.07.0369
// ----------------------------------------------------------------------------
// TIFFInstance.cpp - Released 2019-01-21T12:06:31Z
// ----------------------------------------------------------------------------
// This file is part of the standard TIFF PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "TIFFInstance.h"
#include "TIFFFormat.h"
#include "TIFFOptionsDialog.h"
#include "TIFFRangeOptionsDialog.h"

#include <pcl/StdStatus.h>
#include <pcl/Version.h>
#include <pcl/ErrorHandler.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class TIFFReadHints
{
public:

   double                            lowerRange;
   double                            upperRange;
   TIFFFormat::out_of_range_fix_mode outOfRangeFixMode;
   int                               verbosity;


   TIFFReadHints( const IsoString& hints )
   {
      TIFFFormat::OutOfRangePolicyOptions options = TIFFFormat::DefaultOutOfRangePolicyOptions();
      lowerRange = options.lowerRange;
      upperRange = options.upperRange;
      outOfRangeFixMode = options.outOfRangeFixMode;
      verbosity = 1;

      IsoStringList theHints;
      hints.Break( theHints, ' ', true/*trim*/ );
      theHints.Remove( IsoString() );
      for ( IsoStringList::const_iterator i = theHints.Begin(); i < theHints.End(); ++i )
      {
         if ( *i == "lower-range" )
         {
            if ( ++i == theHints.End() )
               break;
            i->TryToDouble( lowerRange );
         }
         else if ( *i == "upper-range" )
         {
            if ( ++i == theHints.End() )
               break;
            i->TryToDouble( upperRange );
         }
         else if ( *i == "rescale" || *i == "rescale-out-of-range" )
            outOfRangeFixMode = TIFFFormat::OutOfRangeFix_Rescale;
         else if ( *i == "truncate" || *i == "truncate-out-of-range" )
            outOfRangeFixMode = TIFFFormat::OutOfRangeFix_Truncate;
         else if ( *i == "ignore-out-of-range" )
            outOfRangeFixMode = TIFFFormat::OutOfRangeFix_Ignore;
         else if ( *i == "verbosity" )
         {
            if ( ++i == theHints.End() )
               break;
            int n;
            if ( i->TryToInt( n ) )
               verbosity = Range( n, 0, 3 );
         }
      }
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

TIFFInstance::TIFFInstance( const TIFFFormat* f ) :
   FileFormatImplementation( f )
{
}

// ----------------------------------------------------------------------------

TIFFInstance::~TIFFInstance() noexcept( false )
{
   Close();
}

// ----------------------------------------------------------------------------

template <class S>
static void CheckOpenStream( const S& stream, const String& memberFunc )
{
   if ( !stream || !stream->IsOpen() )
      throw Error( "TIFFInstance::" + memberFunc + "(): Illegal request on a closed stream." );
}

// ----------------------------------------------------------------------------

ImageDescriptionArray TIFFInstance::Open( const String& filePath, const IsoString& hints )
{
   Close();

   try
   {
      Exception::EnableConsoleOutput();

      TIFFImageOptions tiffOptions = TIFFFormat::DefaultOptions();
      if ( !hints.IsEmpty() )
      {
         m_readHints = new TIFFReadHints( hints );
         tiffOptions.verbosity = m_readHints->verbosity;
      }

      m_reader = new TIFFReader;
      m_reader->SetTIFFOptions( tiffOptions );
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

bool TIFFInstance::IsOpen() const
{
   return m_reader && m_reader->IsOpen() || m_writer && m_writer->IsOpen();
}

// ----------------------------------------------------------------------------

String TIFFInstance::FilePath() const
{
   if ( m_reader )
      return m_reader->Path();
   if ( m_writer )
      return m_writer->Path();
   return String();
}

// ----------------------------------------------------------------------------

void TIFFInstance::Close()
{
   m_reader.Destroy();
   m_writer.Destroy();
   m_readHints.Destroy();
   m_queriedOptions = false;
}

// ----------------------------------------------------------------------------

void* TIFFInstance::FormatSpecificData() const
{
   if ( !IsOpen() )
      return nullptr;

   TIFFFormat::FormatOptions* data = new TIFFFormat::FormatOptions;
   if ( m_reader )
      data->options = m_reader->TIFFOptions();
   else if ( m_writer )
      data->options = m_writer->TIFFOptions();
   return data;
}

// ----------------------------------------------------------------------------

String TIFFInstance::ImageFormatInfo() const
{
   TIFFImageOptions options;
   if ( m_reader )
      options = m_reader->TIFFOptions();
   else if ( m_writer )
      options = m_writer->TIFFOptions();
   else
      return String();

   const char* cmp;
   switch ( options.compression )
   {
   default:
   case TIFFCompression::Unknown: cmp = "unknown"; break;
   case TIFFCompression::None:    cmp = "none"; break;
   case TIFFCompression::ZIP:     cmp = "ZIP"; break;
   case TIFFCompression::LZW:     cmp = "LZW"; break;
   }

   String info = String().Format(
      "compression=%s planar=%s associated-alpha=%s premultiplied-alpha=%s",
      cmp,
      options.planar ? "planar" : "chunky",
      options.associatedAlpha ? "yes" : "no",
      options.premultipliedAlpha ? "yes" : "no" );

   info += " software=\"" + options.software + '\"';
   info += " description=\"" + options.imageDescription + '\"';
   info += " copyright=\"" + options.copyright + '\"';

   return info;
}

// ----------------------------------------------------------------------------

ICCProfile TIFFInstance::ReadICCProfile()
{
   CheckOpenStream( m_reader, "ReadICCProfile" );
   ICCProfile icc = m_reader->ReadICCProfile();
   if ( icc.IsProfile() )
      if ( m_reader->TIFFOptions().verbosity > 0 )
         Console().WriteLn( "<end><cbr>ICC profile extracted: \'" + icc.Description() + "\', " + String( icc.ProfileSize() ) + " bytes." );
   return icc;
}

// ----------------------------------------------------------------------------

template <class P>
static bool ApplyOutOfRangePolicy( GenericImage<P>& image, const TIFFReadHints* readHints )
{
   /*
    * Replace NaNs and infinities with zeros.
    */
   image.ResetSelections();
   for ( int c = 0; c < image.NumberOfChannels(); ++c )
      for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i )
         if ( !IsFinite( *i ) )
            *i = 0;

   /*
    * Get extreme pixel sample values.
    */
   typename P::sample mn, mx;
   {
      StatusMonitor status = image.Status();
      image.SetStatusCallback( 0 );
      image.GetExtremePixelValues( mn, mx );
      image.Status() = status;
   }

   TIFFFormat::OutOfRangePolicyOptions options = TIFFFormat::DefaultOutOfRangePolicyOptions();
   if ( readHints != nullptr )
   {
      options.lowerRange = readHints->lowerRange;
      options.upperRange = readHints->upperRange;
      options.outOfRangeFixMode = readHints->outOfRangeFixMode;
   }
   else
   {
      switch ( options.outOfRangePolicy )
      {
      case TIFFFormat::OutOfRangePolicy_SetRange:
         break;
      case TIFFFormat::OutOfRangePolicy_AskIfOut:
         if ( mn >= options.lowerRange && mx <= options.upperRange )
            break;
         // else fall-through
      case TIFFFormat::OutOfRangePolicy_AskAlways:
         {
            TIFFRangeOptionsDialog dlg( options, image.BitsPerSample(), mn, mx );

            for ( ;; )
            {
               if ( dlg.Execute() != StdDialogCode::Ok )
                  return false;

               options = dlg.outOfRange;

               if ( options.upperRange < options.lowerRange )
                  Swap( options.lowerRange, options.upperRange );

               double delta = options.upperRange - options.lowerRange;
               if ( 1 + delta == 1 )
               {
                  try
                  {
                     throw Error( "Empty input range!" );
                  }

                  ERROR_HANDLER
                  continue;
               }

               break;
            }
         }
         break;
      }
   }

   bool verbose = readHints == nullptr || readHints->verbosity > 0;

   if ( options.outOfRangeFixMode != TIFFFormat::OutOfRangeFix_Ignore )
   {
      double delta = options.upperRange - options.lowerRange;

      StatusMonitor monitor;
      StandardStatus status;
      if ( verbose )
         monitor.SetCallback( &status );

      if ( mn < options.lowerRange || mx > options.upperRange )
      {
         if ( verbose )
            monitor.Initialize( String().Format( "%s sample values: [%.15g,%.15g] -> [%.15g,%.15g]",
                  (options.outOfRangeFixMode == TIFFFormat::OutOfRangeFix_Truncate) ? "Truncating" : "Rescaling",
                  mn, mx,
                  options.lowerRange, options.upperRange ), image.NumberOfSamples() );

         double idelta = mx - mn;
         for ( int c = 0; c < image.NumberOfChannels(); ++c )
            switch ( options.outOfRangeFixMode )
            {
            default: // ?!
            case TIFFFormat::OutOfRangeFix_Truncate:
               for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i )
               {
                  if ( *i < options.lowerRange )
                     *i = options.lowerRange;
                  else if ( *i > options.upperRange )
                     *i = options.upperRange;
                  if ( verbose )
                     ++monitor;
               }
               break;
            case TIFFFormat::OutOfRangeFix_Rescale:
               if ( delta != 1 || options.lowerRange != 0 )
                  for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i )
                  {
                     *i = ((*i - mn)/idelta)*delta + options.lowerRange;
                     if ( verbose )
                        ++monitor;
                  }
               else
                  for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i )
                  {
                     *i = Range( (*i - mn)/idelta, 0.0, 1.0 );
                     if ( verbose )
                        ++monitor;
                  }
               break;
            }
      }

      if ( options.lowerRange != 0 || options.upperRange != 1 )
      {
         if ( verbose )
            monitor.Initialize( "Normalizing sample values", image.NumberOfSamples() );

         for ( int c = 0; c < image.NumberOfChannels(); ++c )
            if ( delta != 1 )
            {
               if ( options.lowerRange != 0 )
                  for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i )
                  {
                     *i = Range( (*i - options.lowerRange)/delta, 0.0, 1.0 );
                     if ( verbose )
                        ++monitor;
                  }
               else
                  for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i )
                  {
                     *i = Range( *i/delta, 0.0, 1.0 );
                     if ( verbose )
                        ++monitor;
                  }
            }
            else
            {
               for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i )
               {
                  *i = Range( *i - options.lowerRange, 0.0, 1.0 );
                  if ( verbose )
                     ++monitor;
               }
            }
      }
   }
   else
   {
      if ( mn < options.lowerRange || mx > options.upperRange )
         if ( verbose )
            Console().WarningLn( String().Format( "<end><cbr>** Warning: TIFF: Out-of-range floating point pixel sample values "
                                                  "were not fixed because of permissive policy. Data range is [%.15g,%.15g]", mn, mx ) );
   }

   return true;
}

static void ReadTIFFImage3( Image& image, TIFFReader* reader, const TIFFReadHints* readHints )
{
   reader->ReadImage( image );
   if ( reader->Options().ieeefpSampleFormat )
      if ( !ApplyOutOfRangePolicy( image, readHints ) )
         throw ProcessAborted();
}

static void ReadTIFFImage3( DImage& image, TIFFReader* reader, const TIFFReadHints* readHints )
{
   reader->ReadImage( image );
   if ( reader->Options().ieeefpSampleFormat )
      if ( !ApplyOutOfRangePolicy( image, readHints ) )
         throw ProcessAborted();
}

template <class P>
static void ReadTIFFImage2( GenericImage<P>& image, TIFFReader* reader, const TIFFReadHints* readHints )
{
   if ( reader->Options().ieeefpSampleFormat )
   {
      switch ( reader->Options().bitsPerSample )
      {
      case 32:
         {
            Image tmp;
            ReadTIFFImage3( tmp, reader, readHints );
            image.Assign( tmp );
         }
         break;
      default: // !?
      case 64:
         {
            DImage tmp;
            ReadTIFFImage3( tmp, reader, readHints );
            image.Assign( tmp );
         }
         break;
      }
   }
   else
      reader->ReadImage( image );
}

static void ReadTIFFImage2( Image& image, TIFFReader* reader, const TIFFReadHints* readHints )
{
   ReadTIFFImage3( image, reader, readHints );
}

static void ReadTIFFImage2( DImage& image, TIFFReader* reader, const TIFFReadHints* readHints )
{
   ReadTIFFImage3( image, reader, readHints );
}

template <class P>
static void ReadTIFFImage1( GenericImage<P>& image, TIFFReader* reader, const TIFFReadHints* readHints )
{
   CheckOpenStream( reader, "ReadImage" );
   // Disable automatic normalization of floating point FITS images
   ImageOptions options = reader->Options();
   options.readNormalized = !options.ieeefpSampleFormat;
   reader->SetOptions( options );
   ReadTIFFImage2( image, reader, readHints );
}

void TIFFInstance::ReadImage( Image& image )
{
   ReadTIFFImage1( image, m_reader, m_readHints );
}

void TIFFInstance::ReadImage( DImage& image )
{
   ReadTIFFImage1( image, m_reader, m_readHints );
}

void TIFFInstance::ReadImage( UInt8Image& image )
{
   ReadTIFFImage1( image, m_reader, m_readHints );
}

void TIFFInstance::ReadImage( UInt16Image& image )
{
   ReadTIFFImage1( image, m_reader, m_readHints );
}

void TIFFInstance::ReadImage( UInt32Image& image )
{
   ReadTIFFImage1( image, m_reader, m_readHints );
}

// ----------------------------------------------------------------------------

bool TIFFInstance::QueryOptions( Array<ImageOptions>& imageOptions, Array<void*>& formatOptions )
{
   m_queriedOptions = true;

   // Format-independent options
   ImageOptions options;
   if ( !imageOptions.IsEmpty() )
      options = *imageOptions;

   // Format-specific options
   TIFFFormat::FormatOptions* tiff = nullptr;
   if ( !formatOptions.IsEmpty() )
   {
      TIFFFormat::FormatOptions* o = TIFFFormat::FormatOptions::FromGenericDataBlock( *formatOptions );
      if ( o != nullptr )
         tiff = o;
   }

   bool reusedFormatOptions = tiff != nullptr;
   if ( !reusedFormatOptions )
      tiff = new TIFFFormat::FormatOptions;

   // Override embedding options, if requested.
   TIFFFormat::EmbeddingOverrides overrides = TIFFFormat::DefaultEmbeddingOverrides();
   if ( overrides.overrideICCProfileEmbedding )
      options.embedICCProfile = overrides.embedICCProfiles;

   TIFFOptionsDialog dlg( options, tiff->options );
   if ( dlg.Execute() == StdDialogCode::Ok )
   {
      tiff->options = dlg.tiffOptions;

      if ( imageOptions.IsEmpty() )
         imageOptions.Add( dlg.options );
      else
         *imageOptions = dlg.options;

      if ( formatOptions.IsEmpty() )
         formatOptions.Add( (void*)tiff );
      else
         *formatOptions = (void*)tiff;

      return true;
   }

   if ( !reusedFormatOptions )
      delete tiff;

   return false;
}

// ----------------------------------------------------------------------------

void TIFFInstance::Create( const String& filePath, int /*numberOfImages*/, const IsoString& hints )
{
   Close();

   Exception::EnableConsoleOutput();

   m_writer = new TIFFWriter;
   m_writer->Create( filePath );

   TIFFImageOptions tiffOptions = TIFFFormat::DefaultOptions();

   IsoStringList theHints;
   hints.Break( theHints, ' ', true/*trim*/ );
   theHints.Remove( IsoString() );
   for ( IsoStringList::const_iterator i = theHints.Begin(); i < theHints.End(); ++i )
   {
      if ( *i == "zip-compression" )
         tiffOptions.compression = TIFFCompression::ZIP;
      else if ( *i == "lzw-compression" )
         tiffOptions.compression = TIFFCompression::LZW;
      else if ( *i == "uncompressed" || *i == "no-compression" )
         tiffOptions.compression = TIFFCompression::None;
      else if ( *i == "planar" )
         tiffOptions.planar = true;
      else if ( *i == "chunky" || *i == "no-planar" )
         tiffOptions.planar = false;
      else if ( *i == "associated-alpha" )
         tiffOptions.associatedAlpha = true;
      else if ( *i == "no-associated-alpha" )
         tiffOptions.associatedAlpha = false;
      else if ( *i == "premultiplied-alpha" )
         tiffOptions.premultipliedAlpha = true;
      else if ( *i == "no-premultiplied-alpha" )
         tiffOptions.premultipliedAlpha = false;
      else if ( *i == "verbosity" )
      {
         if ( ++i == theHints.End() )
            break;
         int n;
         if ( i->TryToInt( n ) )
            tiffOptions.verbosity = Range( n, 0, 3 );
      }
   }

   m_writer->SetTIFFOptions( tiffOptions );
}

// ----------------------------------------------------------------------------

void TIFFInstance::SetOptions( const ImageOptions& newOptions )
{
   CheckOpenStream( m_writer, "SetOptions" );
   ImageOptions options = newOptions;
   if ( !m_queriedOptions )
   {
      TIFFFormat::EmbeddingOverrides overrides = TIFFFormat::DefaultEmbeddingOverrides();
      if ( overrides.overrideICCProfileEmbedding )
         options.embedICCProfile = overrides.embedICCProfiles;
   }
   m_writer->SetOptions( options );
}

// ----------------------------------------------------------------------------

void TIFFInstance::SetFormatSpecificData( const void* data )
{
   CheckOpenStream( m_writer, "SetFormatSpecificData" );
   const TIFFFormat::FormatOptions* o = TIFFFormat::FormatOptions::FromGenericDataBlock( data );
   if ( o != nullptr )
      m_writer->SetTIFFOptions( o->options );
}

// ----------------------------------------------------------------------------

void TIFFInstance::WriteICCProfile( const ICCProfile& icc )
{
   CheckOpenStream( m_writer, "WriteICCProfile" );
   if ( icc.IsProfile() )
   {
      m_writer->WriteICCProfile( icc );
      if ( m_writer->TIFFOptions().verbosity > 0 )
         Console().WriteLn( "<end><cbr>ICC profile embedded: \'" + icc.Description() + "\', " + String( icc.ProfileSize() ) + " bytes." );
   }
}

// ----------------------------------------------------------------------------

template <class P>
static void WriteTIFFImage( const GenericImage<P>& image, TIFFWriter* writer )
{
   CheckOpenStream( writer, "WriteImage" );
   writer->WriteImage( image );
}

void TIFFInstance::WriteImage( const Image& image )
{
   WriteTIFFImage( image, m_writer );
}

void TIFFInstance::WriteImage( const DImage& image )
{
   WriteTIFFImage( image, m_writer );
}

void TIFFInstance::WriteImage( const UInt8Image& image )
{
   WriteTIFFImage( image, m_writer );
}

void TIFFInstance::WriteImage( const UInt16Image& image )
{
   WriteTIFFImage( image, m_writer );
}

void TIFFInstance::WriteImage( const UInt32Image& image )
{
   WriteTIFFImage( image, m_writer );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF TIFFInstance.cpp - Released 2019-01-21T12:06:31Z
