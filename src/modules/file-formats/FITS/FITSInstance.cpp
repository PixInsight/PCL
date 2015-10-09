//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 01.01.02.0314
// ----------------------------------------------------------------------------
// FITSInstance.cpp - Released 2015/10/08 11:24:33 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
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

#include "FITSInstance.h"
#include "FITSFormat.h"
#include "FITSOptionsDialog.h"
#include "FITSRangeOptionsDialog.h"

#include <pcl/StdStatus.h>
#include <pcl/ErrorHandler.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class FITSReadHints
{
public:

   double                            lowerRange;
   double                            upperRange;
   FITSFormat::out_of_range_fix_mode outOfRangeFixMode;
   bool                              bottomUp                  : 1;
   bool                              signedIntegersArePhysical : 1;
   bool                              noProperties              : 1;
   int                               verbosity;

   FITSReadHints( const IsoString& hints )
   {
      FITSFormat::OutOfRangePolicyOptions options = FITSFormat::DefaultOutOfRangePolicyOptions();
      lowerRange = options.lowerRange;
      upperRange = options.upperRange;
      outOfRangeFixMode = options.outOfRangeFixMode;

      FITSImageOptions fitsOptions = FITSFormat::DefaultOptions();
      bottomUp = fitsOptions.bottomUp;
      signedIntegersArePhysical = fitsOptions.signedIntegersArePhysical;
      noProperties = false;
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
            outOfRangeFixMode = FITSFormat::OutOfRangeFix_Rescale;
         else if ( *i == "truncate" || *i == "truncate-out-of-range" )
            outOfRangeFixMode = FITSFormat::OutOfRangeFix_Truncate;
         else if ( *i == "ignore-out-of-range" )
            outOfRangeFixMode = FITSFormat::OutOfRangeFix_Ignore;
         else if ( *i == "bottom-up" )
            bottomUp = true;
         else if ( *i == "up-bottom" )
            bottomUp = false;
         else if ( *i == "signed-is-physical" )
            signedIntegersArePhysical = true;
         else if ( *i == "signed-is-logical" )
            signedIntegersArePhysical = false;
         else if ( *i == "no-properties" )
            noProperties = true;
         else if ( *i == "properties" )
            noProperties = false;
         else if ( *i == "verbosity" )
         {
            if ( ++i == theHints.End() )
               break;
            int n;
            if ( i->TryToInt( n ) )
               verbosity = Max( 0, n );
         }
      }
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FITSInstance::FITSInstance( const FITSFormat* f ) :
   FileFormatImplementation( f ),
   reader( 0 ),
   writer( 0 ),
   readHints( 0 ),
   embeddedICCProfile( 0 ),
   embeddedThumbnail( 0 ),
   queriedOptions( false )
{
}

FITSInstance::~FITSInstance()
{
   Close();
}

// ----------------------------------------------------------------------------

ImageDescriptionArray FITSInstance::Open( const String& filePath, const IsoString& hints )
{
   Close();

   try
   {
      Exception::EnableConsoleOutput();

      reader = new FITSReader;
      reader->Open( filePath );

      FITSImageOptions defaultOptions = FITSFormat::DefaultOptions();

      if ( !hints.IsEmpty() )
      {
         readHints = new FITSReadHints( hints );
         defaultOptions.bottomUp = readHints->bottomUp;
         defaultOptions.signedIntegersArePhysical = readHints->signedIntegersArePhysical;
      }

      ImageDescriptionArray a;

      for ( int i = 0; i < int( reader->NumberOfImages() ); ++i )
      {
         reader->SetIndex( i );
         reader->SetFITSOptions( defaultOptions );

         // Some alien FITS files use invalid image identifiers - fix them here
         IsoString id = reader->Id();
         if ( !id.IsEmpty() )
         {
            for ( IsoString::iterator i = id.Begin(); i != id.End(); ++i )
               if ( (*i < 'A' || *i > 'Z') && (*i < 'a' || *i > 'z') && (*i < '0' || *i > '9') && *i != '_' )
                  *i = '_';
            if ( *id >= '0' && *id <= '9' )
               id.Prepend( '_' );
         }

         a.Add( ImageDescription( reader->Info(), reader->Options(), id ) );
      }

      reader->SetIndex( 0 );

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

bool FITSInstance::IsOpen() const
{
   return reader != 0 && reader->IsOpen() || writer != 0 && writer->IsOpen();
}

// ----------------------------------------------------------------------------

String FITSInstance::FilePath() const
{
   if ( reader != 0 )
      return reader->Path();
   if ( writer != 0 )
      return writer->Path();
   return String();
}

// ----------------------------------------------------------------------------

void FITSInstance::Close()
{
   if ( reader != 0 )
      delete reader, reader = 0;
   if ( writer != 0 )
      delete writer, writer = 0;
   if ( readHints != 0 )
      delete readHints, readHints = 0;
   if ( embeddedICCProfile != 0 )
      delete embeddedICCProfile, embeddedICCProfile = 0;
   if ( embeddedThumbnail != 0 )
      delete embeddedThumbnail, embeddedThumbnail = 0;
}

// ----------------------------------------------------------------------------

void FITSInstance::SelectImage( int index )
{
   if ( reader == 0 || !reader->IsOpen() )
      throw Error( "FITS format: Attempt to select an image before opening a file" );

   if ( int( reader->NumberOfImages() ) == 0 && index != 0 ||
        int( reader->NumberOfImages() ) <= index )
      throw Error( "FITS format: Attempt to select a nonexistent image" );

   reader->SetIndex( index );
}

int FITSInstance::SelectedImageIndex() const
{
   if ( reader == 0 || !reader->IsOpen() )
      throw Error( "FITS format: Attempt to query the selected image before opening a file" );

   return reader->Index();
}

// ----------------------------------------------------------------------------

void* FITSInstance::FormatSpecificData() const
{
   if ( !IsOpen() )
      return 0;

   FITSFormat::FormatOptions* data = new FITSFormat::FormatOptions;
   if ( reader != 0 )
      data->options = reader->FITSOptions();
   else if ( writer != 0 )
      data->options = writer->FITSOptions();
   return data;
}

// ----------------------------------------------------------------------------

void FITSInstance::Extract( FITSKeywordArray& keywords )
{
   if ( !IsOpen() )
      throw Error( "FITS format: Attempt to extract keywords without opening or creating a file" );

   if ( reader != 0 )
   {
      reader->Extract( keywords );
      if ( !readHints || readHints->verbosity > 0 )
         Console().WriteLn( "<end><cbr>" + String( keywords.Length() ) + " FITS keywords extracted" );
   }
   else
      keywords = writer->EmbeddedKeywords();
}

void FITSInstance::Extract( ICCProfile& icc )
{
   if ( !IsOpen() )
      throw Error( "FITS format: Attempt to extract an ICC profile without opening or creating a file" );

   if ( reader != 0 )
   {
      reader->Extract( icc );
      if ( icc.IsProfile() )
         if ( !readHints || readHints->verbosity > 0 )
            Console().WriteLn( "<end><cbr>ICC profile extracted: \'" + icc.Description() + "\', " + String( icc.ProfileSize() ) + " bytes" );
   }
   else if ( writer->EmbeddedICCProfile() != 0 )
      icc = *writer->EmbeddedICCProfile();
}

void FITSInstance::Extract( pcl::UInt8Image& thumbnail )
{
   if ( !IsOpen() )
      throw Error( "FITS format: Attempt to extract a thumbnail image without opening or creating a file" );

   if ( reader != 0 )
      reader->Extract( thumbnail );
   else if ( writer->EmbeddedThumbnail() != 0 )
      thumbnail = *writer->EmbeddedThumbnail();
}

// ----------------------------------------------------------------------------

ImagePropertyDescriptionArray FITSInstance::Properties()
{
   if ( reader == 0 )
      throw Error( "FITS format: Attempt to get property information without opening an existing file" );

   ImagePropertyDescriptionArray properties;
   if ( !readHints || !readHints->noProperties )
   {
      IsoStringList dataExtensions = reader->DataExtensionNames();
      for ( IsoStringList::const_iterator i = dataExtensions.Begin(); i != dataExtensions.End(); ++i )
      {
         /*
          * ### FIXME: Temporary workaround to prevent XISF internal properties
          *            embedded in FITS files.
          */
         if ( !i->StartsWith( "XISF:" ) )
            properties.Append( ImagePropertyDescription( *i, VariantType::ByteArray ) );
      }
   }
   return properties;
}

Variant FITSInstance::ReadProperty( const IsoString& property )
{
   if ( reader == 0 )
      throw Error( "FITS format: Attempt to read a data property without opening an existing file" );

   if ( !readHints || !readHints->noProperties )
   {
      ByteArray data;
      if ( reader->Extract( data, property ) )
      {
         if ( !readHints || readHints->verbosity > 0 )
            Console().WriteLn( "<end><cbr>Property extracted: \'" + property + "\', " + String( data.Length() ) + " bytes" );
         return data;
      }
   }
   return Variant();
}

void FITSInstance::WriteProperty( const IsoString& property, const Variant& value )
{
   if ( writer == 0 )
      throw Error( "FITS format: Attempt to write a data property without creating a new file" );

   /*
    * ### FIXME: Temporary workaround to prevent XISF internal properties
    *            embedded in FITS files.
    */
   if ( !property.StartsWith( "XISF:" ) )
   {
      ByteArray data = value.ToByteArray();
      writer->Embed( data, property );
      if ( !readHints || readHints->verbosity > 0 )
         Console().WriteLn( "<end><cbr>Property embedded: \'" + property + "\', " + String( data.Length() ) + " bytes" );
   }
}

// ----------------------------------------------------------------------------

template <class P>
static bool ApplyOutOfRangePolicy( GenericImage<P>& image, const FITSReadHints* readHints )
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

   /*
    * Apply out-of-range policy.
    */
   FITSFormat::OutOfRangePolicyOptions options = FITSFormat::DefaultOutOfRangePolicyOptions();
   if ( readHints )
   {
      options.lowerRange = readHints->lowerRange;
      options.upperRange = readHints->upperRange;
      options.outOfRangeFixMode = readHints->outOfRangeFixMode;
   }
   else // if there are readHints, never ask user.
   {
      switch ( options.outOfRangePolicy )
      {
      case FITSFormat::OutOfRangePolicy_SetRange:
         break;
      case FITSFormat::OutOfRangePolicy_AskIfOut:
         if ( mn >= options.lowerRange && mx <= options.upperRange )
            break;
         // else pass-through
      case FITSFormat::OutOfRangePolicy_AskAlways:
         {
            FITSRangeOptionsDialog dlg( options, image.BitsPerSample(), mn, mx );

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

   if ( options.outOfRangeFixMode != FITSFormat::OutOfRangeFix_Ignore )
   {
      double delta = options.upperRange - options.lowerRange;

      if ( mn < options.lowerRange || mx > options.upperRange )
      {
         image.Status().Initialize( String().Format( "%s sample values: [%.15g,%.15g] -> [%.15g,%.15g]",
                        (options.outOfRangeFixMode == FITSFormat::OutOfRangeFix_Truncate) ? "Truncating" : "Rescaling",
                        mn, mx,
                        options.lowerRange, options.upperRange ), image.NumberOfSamples() );

         double idelta = mx - mn;
         for ( int c = 0; c < image.NumberOfChannels(); ++c )
            switch ( options.outOfRangeFixMode )
            {
            default: // ?!
            case FITSFormat::OutOfRangeFix_Truncate:
               for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i, ++image.Status() )
                  if ( *i < options.lowerRange )
                     *i = options.lowerRange;
                  else if ( *i > options.upperRange )
                     *i = options.upperRange;
               break;
            case FITSFormat::OutOfRangeFix_Rescale:
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
         if ( !readHints || readHints->verbosity > 0 )
            Console().WarningLn( String().Format( "<end><cbr>** Warning: FITS: Out-of-range floating point pixel sample values "
                                                  "were not fixed because of permissive policy. Data range is [%.15g,%.15g]", mn, mx ) );
   }

   return true;
}

static void ReadFITSImage3( Image& image, FITSReader* reader, const FITSReadHints* readHints )
{
   reader->ReadImage( image );
   if ( reader->Options().ieeefpSampleFormat )
      if ( !ApplyOutOfRangePolicy( image, readHints ) )
         throw ProcessAborted();
}

static void ReadFITSImage3( DImage& image, FITSReader* reader, const FITSReadHints* readHints )
{
   reader->ReadImage( image );
   if ( reader->Options().ieeefpSampleFormat )
      if ( !ApplyOutOfRangePolicy( image, readHints ) )
         throw ProcessAborted();
}

template <class P>
static void ReadFITSImage2( GenericImage<P>& image, FITSReader* reader, const FITSReadHints* readHints )
{
   if ( reader->Options().ieeefpSampleFormat )
   {
      switch ( reader->Options().bitsPerSample )
      {
      case 32:
         {
            Image tmp;
            ReadFITSImage3( tmp, reader, readHints );
            image.Assign( tmp );
         }
         break;
      default: // !?
      case 64:
         {
            DImage tmp;
            ReadFITSImage3( tmp, reader, readHints );
            image.Assign( tmp );
         }
         break;
      }
   }
   else
      reader->ReadImage( image );
}

static void ReadFITSImage2( Image& image, FITSReader* reader, const FITSReadHints* readHints )
{
   ReadFITSImage3( image, reader, readHints );
}

static void ReadFITSImage2( DImage& image, FITSReader* reader, const FITSReadHints* readHints )
{
   ReadFITSImage3( image, reader, readHints );
}

template <class P>
static void ReadFITSImage1( GenericImage<P>& image, FITSReader* reader, const FITSReadHints* readHints )
{
   if ( reader == 0 || !reader->IsOpen() )
      throw Error( "FITS format: Attempt to read an image before opening a file" );

   // Disable automatic normalization of floating point FITS images
   reader->Options().readNormalized = !reader->Options().ieeefpSampleFormat;

   StandardStatus status;
   image.SetStatusCallback( &status );

   ReadFITSImage2( image, reader, readHints );
}

void FITSInstance::ReadImage( Image& image )
{
   ReadFITSImage1( image, reader, readHints );
}

void FITSInstance::ReadImage( DImage& image )
{
   ReadFITSImage1( image, reader, readHints );
}

void FITSInstance::ReadImage( UInt8Image& image )
{
   ReadFITSImage1( image, reader, readHints );
}

void FITSInstance::ReadImage( UInt16Image& image )
{
   ReadFITSImage1( image, reader, readHints );
}

void FITSInstance::ReadImage( UInt32Image& image )
{
   ReadFITSImage1( image, reader, readHints );
}

// ----------------------------------------------------------------------------

template <class T>
void ReadFITSPixels( T* buffer, int startRow, int rowCount, int channel, FITSReader* reader )
{
   if ( reader == 0 || !reader->IsOpen() )
      throw Error( "FITS format: Attempt to perform an incremental read operation before opening a file" );

   reader->Read( buffer, startRow, rowCount, channel );
}

void FITSInstance::Read( pcl::Image::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadFITSPixels( buffer, startRow, rowCount, channel, reader );
}

void FITSInstance::Read( pcl::DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadFITSPixels( buffer, startRow, rowCount, channel, reader );
}

void FITSInstance::Read( UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadFITSPixels( buffer, startRow, rowCount, channel, reader );
}

void FITSInstance::Read( UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadFITSPixels( buffer, startRow, rowCount, channel, reader );
}

void FITSInstance::Read( UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
{
   ReadFITSPixels( buffer, startRow, rowCount, channel, reader );
}

// ----------------------------------------------------------------------------

bool FITSInstance::QueryOptions( Array<ImageOptions>& imageOptions, Array<void*>& formatOptions )
{
   queriedOptions = true;

   // Format-independent options

   ImageOptions options;
   if ( !imageOptions.IsEmpty() )
      options = *imageOptions;

   // Format-specific options

   FITSFormat::FormatOptions* fits = 0;
   if ( !formatOptions.IsEmpty() )
   {
      FITSFormat::FormatOptions* o = FITSFormat::FormatOptions::FromGenericDataBlock( *formatOptions );
      if ( o != 0 )
         fits = o;
   }

   bool reusedFormatOptions = fits != 0;
   if ( !reusedFormatOptions )
      fits = new FITSFormat::FormatOptions;

   // Override embedding options, if requested.

   FITSFormat::EmbeddingOverrides overrides = FITSFormat::DefaultEmbeddingOverrides();

   if ( overrides.overrideICCProfileEmbedding )
      options.embedICCProfile = overrides.embedICCProfiles;

   if ( overrides.overrideThumbnailEmbedding )
      options.embedThumbnail = overrides.embedThumbnails;

   if ( overrides.overridePropertyEmbedding )
      options.embedProperties = overrides.embedProperties;

   FITSOptionsDialog dlg( options, fits->options );

   if ( dlg.Execute() == StdDialogCode::Ok )
   {
      fits->options = dlg.fitsOptions;

      if ( imageOptions.IsEmpty() )
         imageOptions.Add( dlg.options );
      else
         *imageOptions = dlg.options;

      if ( formatOptions.IsEmpty() )
         formatOptions.Add( (void*)fits );
      else
         *formatOptions = (void*)fits;

      return true;
   }

   if ( !reusedFormatOptions )
      delete fits;

   return false;
}

// ----------------------------------------------------------------------------

void FITSInstance::Create( const String& filePath, int /*numberOfImages*/, const IsoString& hints )
{
   Close();

   Exception::EnableConsoleOutput();

   writer = new FITSWriter;
   writer->Create( filePath );

   FITSImageOptions options = FITSFormat::DefaultOptions();

   IsoStringList theHints;
   hints.Break( theHints, ' ', true/*trim*/ );
   theHints.Remove( IsoString() );
   for ( IsoStringList::const_iterator i = theHints.Begin(); i < theHints.End(); ++i )
   {
      if ( *i == "unsigned" )
         options.unsignedIntegers = true;
      else if ( *i == "signed" )
         options.unsignedIntegers = false;
      else if ( *i == "bottom-up" )
         options.bottomUp = true;
      else if ( *i == "up-bottom" )
         options.bottomUp = false;
      else if ( *i == "cleanup-headers" )
         options.cleanupHeaders = true;
      else if ( *i == "no-cleanup-headers" )
         options.cleanupHeaders = false;
      else if ( *i == "no-properties" )
         writer->Options().embedProperties = false;
      else if ( *i == "properties" )
         writer->Options().embedProperties = true;
   }

   writer->SetFITSOptions( options );
}

// ----------------------------------------------------------------------------

void FITSInstance::SetId( const IsoString& id )
{
   if ( writer == 0 || !writer->IsOpen() )
      throw Error( "FITS format: Attempt to set an image identifier before creating a file" );

   writer->SetId( id );
}

void FITSInstance::SetOptions( const ImageOptions& options )
{
   if ( writer == 0 || !writer->IsOpen() )
      throw Error( "FITS format: Attempt to set image options before creating a file" );

   writer->Options() = options;

   if ( !queriedOptions )
   {
      FITSFormat::EmbeddingOverrides overrides = FITSFormat::DefaultEmbeddingOverrides();

      if ( overrides.overrideICCProfileEmbedding )
         writer->Options().embedICCProfile = overrides.embedICCProfiles;

      if ( overrides.overrideThumbnailEmbedding )
         writer->Options().embedThumbnail = overrides.embedThumbnails;
   }
}

// ----------------------------------------------------------------------------

void FITSInstance::SetFormatSpecificData( const void* data )
{
   if ( writer == 0 || !writer->IsOpen() )
      throw Error( "FITS format: Attempt to set format-specific options before creating a file" );
/*
 * ### The FITS writer has no per-instance configurable format data.
 *
   const FITSFormat::FormatOptions* o = FITSFormat::FormatOptions::FromGenericDataBlock( data );
   if ( o != 0 )
      writer->SetFITSOptions( o->options );
*/
}

// ----------------------------------------------------------------------------

void FITSInstance::Embed( const FITSKeywordArray& keywords )
{
   if ( writer == 0 || !writer->IsOpen() )
      throw Error( "FITS format: Attempt to embed FITS header keywords before creating a file" );

   writer->Embed( keywords );
   if ( !readHints || readHints->verbosity > 0 )
      Console().WriteLn( "<end><cbr>" + String( keywords.Length() ) + " FITS keywords embedded" );
}

void FITSInstance::Embed( const ICCProfile& icc )
{
   if ( writer == 0 || !writer->IsOpen() )
      throw Error( "FITS format: Attempt to embed an ICC profile before creating a file" );

   if ( icc.IsProfile() )
   {
      writer->Embed( *(embeddedICCProfile = new ICCProfile( icc )) );
      if ( !readHints || readHints->verbosity > 0 )
         Console().WriteLn( "<end><cbr>ICC profile embedded: \'" + icc.Description() + "\', " + String( icc.ProfileSize() ) + " bytes" );
   }
}

void FITSInstance::Embed( const pcl::UInt8Image& thumbnail )
{
   if ( writer == 0 || !writer->IsOpen() )
      throw Error( "FITS format: Attempt to embed a thumbnail image before creating a file" );

   if ( !thumbnail.IsEmpty() )
      writer->Embed( *(embeddedThumbnail = new UInt8Image( thumbnail )) );
}

// ----------------------------------------------------------------------------

template <class P>
static void WriteFITSImage( const GenericImage<P>& image, FITSWriter* writer )
{
   if ( writer == 0 || !writer->IsOpen() )
      throw Error( "FITS format: Attempt to write an image before creating a file" );

   StandardStatus status;
   image.SetStatusCallback( &status );
   writer->WriteImage( image );
}

void FITSInstance::WriteImage( const Image& image )
{
   WriteFITSImage( image, writer );
}

void FITSInstance::WriteImage( const DImage& image )
{
   WriteFITSImage( image, writer );
}

void FITSInstance::WriteImage( const UInt8Image& image )
{
   WriteFITSImage( image, writer );
}

void FITSInstance::WriteImage( const UInt16Image& image )
{
   WriteFITSImage( image, writer );
}

void FITSInstance::WriteImage( const UInt32Image& image )
{
   WriteFITSImage( image, writer );
}

void FITSInstance::CreateImage( const ImageInfo& info )
{
   if ( writer == 0 || !writer->IsOpen() )
      throw Error( "FITS format: Attempt to create an image before creating a file" );

   writer->CreateImage( info );
}

template <class T>
void WriteFITSPixels( const T* buffer, int startRow, int rowCount, int channel, FITSWriter* writer )
{
   if ( writer == 0 || !writer->IsOpen() )
      throw Error( "FITS format: Attempt to perform an incremental write operation before creating a file" );

   writer->Write( buffer, startRow, rowCount, channel );
}

void FITSInstance::Write( const pcl::Image::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteFITSPixels( buffer, startRow, rowCount, channel, writer );
}

void FITSInstance::Write( const pcl::DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteFITSPixels( buffer, startRow, rowCount, channel, writer );
}

void FITSInstance::Write( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteFITSPixels( buffer, startRow, rowCount, channel, writer );
}

void FITSInstance::Write( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteFITSPixels( buffer, startRow, rowCount, channel, writer );
}

void FITSInstance::Write( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
{
   WriteFITSPixels( buffer, startRow, rowCount, channel, writer );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FITSInstance.cpp - Released 2015/10/08 11:24:33 UTC
