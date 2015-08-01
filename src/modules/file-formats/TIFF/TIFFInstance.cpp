//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// Standard TIFF File Format Module Version 01.00.06.0248
// ----------------------------------------------------------------------------
// TIFFInstance.cpp - Released 2015/07/31 11:49:40 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard TIFF PixInsight module.
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
   bool                              strictMode;
   bool                              showWarnings;

   TIFFReadHints( const IsoString& hints )
   {
      TIFFFormat::OutOfRangePolicyOptions options = TIFFFormat::DefaultOutOfRangePolicyOptions();
      lowerRange = options.lowerRange;
      upperRange = options.upperRange;
      outOfRangeFixMode = options.outOfRangeFixMode;

      strictMode = false;
      showWarnings = false;

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
         else if ( *i == "strict" )
            strictMode = true;
         else if ( *i == "permissive" )
            strictMode = false;
         else if ( *i == "show-warnings" )
            showWarnings = true;
         else if ( *i == "no-show-warnings" )
            showWarnings = false;
      }
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

TIFFInstance::TIFFInstance( const TIFFFormat* f ) :
FileFormatImplementation( f ), reader( 0 ), writer( 0 ), readHints( 0 ),
queriedOptions( false ), embeddedICCProfile( 0 )
{
}

TIFFInstance::~TIFFInstance()
{
   Close();
}

// ----------------------------------------------------------------------------

ImageDescriptionArray TIFFInstance::Open( const String& filePath, const IsoString& hints )
{
   Close();

   try
   {
      if ( !hints.IsEmpty() )
      {
         readHints = new TIFFReadHints( hints );
         TIFF::SetStrictMode( readHints->strictMode );
         TIFF::EnableWarnings( readHints->showWarnings );
      }
      else
      {
         TIFF::SetStrictMode( false );
         TIFF::EnableWarnings( false );
      }

      Exception::EnableConsoleOutput();

      reader = new TIFFReader;
      reader->Open( filePath );

      ImageDescriptionArray a;
      a.Add( ImageDescription( reader->Info(), reader->Options() ) );
      return a;
   }

   catch ( ... )
   {
      if ( reader != 0 )
         delete reader, reader = 0;
      if ( readHints != 0 )
         delete readHints, readHints = 0;
      throw;
   }
}

// ----------------------------------------------------------------------------

bool TIFFInstance::IsOpen() const
{
   return reader != 0 && reader->IsOpen() || writer != 0 && writer->IsOpen();
}

// ----------------------------------------------------------------------------

String TIFFInstance::FilePath() const
{
   if ( reader != 0 )
      return reader->Path();
   if ( writer != 0 )
      return writer->Path();
   return String();
}

// ----------------------------------------------------------------------------

void TIFFInstance::Close()
{
   if ( reader != 0 )
      delete reader, reader = 0;
   if ( writer != 0 )
      delete writer, writer = 0;
   if ( readHints != 0 )
      delete readHints, readHints = 0;
   if ( embeddedICCProfile != 0 )
      delete embeddedICCProfile, embeddedICCProfile = 0;
}

// ----------------------------------------------------------------------------

void* TIFFInstance::FormatSpecificData() const
{
   if ( !IsOpen() )
      return 0;

   TIFFFormat::FormatOptions* data = new TIFFFormat::FormatOptions;
   if ( reader != 0 )
      data->options = reader->TIFFOptions();
   else if ( writer != 0 )
      data->options = writer->TIFFOptions();
   return data;
}

// ----------------------------------------------------------------------------

String TIFFInstance::ImageProperties() const
{
   if ( !IsOpen() )
      return String();

   TIFFImageOptions options;
   if ( reader != 0 )
      options = reader->TIFFOptions();
   else if ( writer != 0 )
      options = writer->TIFFOptions();

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

void TIFFInstance::Extract( ICCProfile& icc )
{
   if ( !IsOpen() )
      throw Error( "TIFF format: Attempt to extract an ICC profile without opening or creating a file" );

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
static bool __ApplyOutOfRangePolicy( GenericImage<P>& image, const TIFFReadHints* readHints )
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
   if ( readHints )
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

   if ( options.outOfRangeFixMode != TIFFFormat::OutOfRangeFix_Ignore )
   {
      double delta = options.upperRange - options.lowerRange;

      if ( mn < options.lowerRange || mx > options.upperRange )
      {
         image.Status().Initialize( String().Format( "%s sample values: [%.15g,%.15g] -> [%.15g,%.15g]",
                        (options.outOfRangeFixMode == TIFFFormat::OutOfRangeFix_Truncate) ? "Truncating" : "Rescaling",
                        mn, mx,
                        options.lowerRange, options.upperRange ), image.NumberOfSamples() );

         double idelta = mx - mn;
         for ( int c = 0; c < image.NumberOfChannels(); ++c )
            switch ( options.outOfRangeFixMode )
            {
            default: // ?!
            case TIFFFormat::OutOfRangeFix_Truncate:
               for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i, ++image.Status() )
                  if ( *i < options.lowerRange )
                     *i = options.lowerRange;
                  else if ( *i > options.upperRange )
                     *i = options.upperRange;
               break;
            case TIFFFormat::OutOfRangeFix_Rescale:
               if ( delta != 1 || options.lowerRange != 0 )
                  for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i, ++image.Status() )
                     *i = ((*i - mn)/idelta)*delta + options.lowerRange;
               else
                  for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i, ++image.Status() )
                     *i = Range( (*i - mn)/idelta, 0.0, 1.0 );
               break;
            }
      }

      if ( options.lowerRange != 0 || options.upperRange != 1 )
      {
         image.Status().Initialize( "Normalizing sample values", image.NumberOfSamples() );

         for ( int c = 0; c < image.NumberOfChannels(); ++c )
            if ( delta != 1 )
            {
               if ( options.lowerRange != 0 )
                  for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i, ++image.Status() )
                     *i = Range( (*i - options.lowerRange)/delta, 0.0, 1.0 );
               else
                  for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i, ++image.Status() )
                     *i = Range( *i/delta, 0.0, 1.0 );
            }
            else
            {
               for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i, ++image.Status() )
                  *i = Range( *i - options.lowerRange, 0.0, 1.0 );
            }
      }
   }
   else
   {
      if ( mn < options.lowerRange || mx > options.upperRange )
         Console().WarningLn( String().Format( "<end><cbr>** Warning: TIFF: Out-of-range floating point pixel sample values "
                                               "were not fixed because of permissive policy. Data range is [%.15g,%.15g]", mn, mx ) );
   }

   return true;
}

static void __ReadImage3( Image& image, TIFFReader* reader, const TIFFReadHints* readHints )
{
   reader->ReadImage( image );
   if ( reader->Options().ieeefpSampleFormat )
      if ( !__ApplyOutOfRangePolicy( image, readHints ) )
         throw ProcessAborted();
}

static void __ReadImage3( DImage& image, TIFFReader* reader, const TIFFReadHints* readHints )
{
   reader->ReadImage( image );
   if ( reader->Options().ieeefpSampleFormat )
      if ( !__ApplyOutOfRangePolicy( image, readHints ) )
         throw ProcessAborted();
}

template <class P>
static void __ReadImage2( GenericImage<P>& image, TIFFReader* reader, const TIFFReadHints* readHints )
{
   if ( reader->Options().ieeefpSampleFormat )
   {
      switch ( reader->Options().bitsPerSample )
      {
      case 32:
         {
            Image tmp;
            __ReadImage3( tmp, reader, readHints );
            image.Assign( tmp );
         }
         break;
      default: // !?
      case 64:
         {
            DImage tmp;
            __ReadImage3( tmp, reader, readHints );
            image.Assign( tmp );
         }
         break;
      }
   }
   else
      reader->ReadImage( image );
}

static void __ReadImage2( Image& image, TIFFReader* reader, const TIFFReadHints* readHints )
{
   __ReadImage3( image, reader, readHints );
}

static void __ReadImage2( DImage& image, TIFFReader* reader, const TIFFReadHints* readHints )
{
   __ReadImage3( image, reader, readHints );
}

template <class P>
static void __ReadImage1( GenericImage<P>& image, TIFFReader* reader, const TIFFReadHints* readHints )
{
   if ( reader == 0 || !reader->IsOpen() )
      throw Error( "TIFF format: Attempt to read an image before opening a file" );

   // Disable automatic normalization of floating point TIFF images
   reader->Options().readNormalized = !reader->Options().ieeefpSampleFormat;

   StandardStatus status;
   image.SetStatusCallback( &status );

   __ReadImage2( image, reader, readHints );
}

void TIFFInstance::ReadImage( Image& image )
{
   __ReadImage1( image, reader, readHints );
}

void TIFFInstance::ReadImage( DImage& image )
{
   __ReadImage1( image, reader, readHints );
}

void TIFFInstance::ReadImage( UInt8Image& image )
{
   __ReadImage1( image, reader, readHints );
}

void TIFFInstance::ReadImage( UInt16Image& image )
{
   __ReadImage1( image, reader, readHints );
}

void TIFFInstance::ReadImage( UInt32Image& image )
{
   __ReadImage1( image, reader, readHints );
}

// ----------------------------------------------------------------------------

bool TIFFInstance::QueryOptions( Array<ImageOptions>& imageOptions, Array<void*>& formatOptions )
{
   queriedOptions = true;

   // Format-independent options

   ImageOptions options;
   if ( !imageOptions.IsEmpty() )
      options = *imageOptions;

   // Format-specific options

   TIFFFormat::FormatOptions* tiff = 0;

   if ( !formatOptions.IsEmpty() )
   {
      TIFFFormat::FormatOptions* o = TIFFFormat::FormatOptions::FromGenericDataBlock( *formatOptions );
      if ( o != 0 )
         tiff = o;
   }

   bool reusedFormatOptions = tiff != 0;
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

   TIFF::SetStrictMode( true );
   TIFF::EnableWarnings( true );

   Exception::EnableConsoleOutput();

   writer = new TIFFWriter;
   writer->Create( filePath );

   TIFFImageOptions options = TIFFFormat::DefaultOptions();

   IsoStringList theHints;
   hints.Break( theHints, ' ', true/*trim*/ );
   theHints.Remove( IsoString() );
   for ( IsoStringList::const_iterator i = theHints.Begin(); i < theHints.End(); ++i )
   {
      if ( *i == "zip-compression" )
         options.compression = TIFFCompression::ZIP;
      else if ( *i == "lzw-compression" )
         options.compression = TIFFCompression::LZW;
      else if ( *i == "uncompressed" || *i == "no-compression" )
         options.compression = TIFFCompression::None;
      else if ( *i == "planar" )
         options.planar = true;
      else if ( *i == "chunky" || *i == "no-planar" )
         options.planar = false;
      else if ( *i == "associated-alpha" )
         options.associatedAlpha = true;
      else if ( *i == "no-associated-alpha" )
         options.associatedAlpha = false;
      else if ( *i == "premultiplied-alpha" )
         options.premultipliedAlpha = true;
      else if ( *i == "no-premultiplied-alpha" )
         options.premultipliedAlpha = false;
   }

   writer->TIFFOptions() = options;
}

// ----------------------------------------------------------------------------

void TIFFInstance::SetOptions( const ImageOptions& options )
{
   if ( writer == 0 || !writer->IsOpen() )
      throw Error( "TIFF format: Attempt to set image options before creating a file" );

   writer->Options() = options;

   if ( !queriedOptions )
   {
      TIFFFormat::EmbeddingOverrides overrides = TIFFFormat::DefaultEmbeddingOverrides();

      if ( overrides.overrideICCProfileEmbedding )
         writer->Options().embedICCProfile = overrides.embedICCProfiles;
   }
}

// ----------------------------------------------------------------------------

void TIFFInstance::SetFormatSpecificData( const void* data )
{
   if ( writer == 0 || !writer->IsOpen() )
      throw Error( "TIFF format: Attempt to set format-specific options before creating a file" );

   const TIFFFormat::FormatOptions* o = TIFFFormat::FormatOptions::FromGenericDataBlock( data );
   if ( o != 0 )
      writer->TIFFOptions() = o->options;
}

// ----------------------------------------------------------------------------

void TIFFInstance::Embed( const ICCProfile& icc )
{
   if ( writer == 0 || !writer->IsOpen() )
      throw Error( "TIFF format: Attempt to embed an ICC profile before creating a file" );

   if ( icc.IsProfile() )
   {
      writer->Embed( *(embeddedICCProfile = new ICCProfile( icc )) );
      Console().WriteLn( "<end><cbr>ICC profile embedded: \'" + icc.Description() + "\', " + String( icc.ProfileSize() ) + " bytes" );
   }
}

// ----------------------------------------------------------------------------

template <class P>
static void __WriteImage( const GenericImage<P>& image, TIFFWriter* writer )
{
   if ( writer == 0 || !writer->IsOpen() )
      throw Error( "TIFF format: Attempt to write an image before creating a file" );

   StandardStatus status;
   image.SetStatusCallback( &status );
   writer->WriteImage( image );
}

void TIFFInstance::WriteImage( const Image& image )
{
   __WriteImage( image, writer );
}

void TIFFInstance::WriteImage( const DImage& image )
{
   __WriteImage( image, writer );
}

void TIFFInstance::WriteImage( const UInt8Image& image )
{
   __WriteImage( image, writer );
}

void TIFFInstance::WriteImage( const UInt16Image& image )
{
   __WriteImage( image, writer );
}

void TIFFInstance::WriteImage( const UInt32Image& image )
{
   __WriteImage( image, writer );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF TIFFInstance.cpp - Released 2015/07/31 11:49:40 UTC
