// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard JPEG2000 File Format Module Version 01.00.01.0212
// ****************************************************************************
// JPEG2000Instance.cpp - Released 2014/10/29 07:34:49 UTC
// ****************************************************************************
// This file is part of the standard JPEG2000 PixInsight module.
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

#include "JPEG2000Instance.h"
#include "JPEG2000Format.h"
#include "JPEG2000OptionsDialog.h"

#include <pcl/ICCProfile.h>
#include <pcl/StdStatus.h>
#include <pcl/ErrorHandler.h>
#include <pcl/File.h>

#define JP2KERROR( msg ) throw Error( String( msg ) + ": " + path );

namespace pcl
{

// ----------------------------------------------------------------------------

static bool __JasPerInitialized = false;

static void __InitJasPer()
{
   if ( !__JasPerInitialized )
   {
      if ( jas_init() != 0 )
         throw Error( "JasPer Library: Initialization error." );

      __JasPerInitialized = true;
   }
}

static void __CleanupJasPer()
{
   if ( __JasPerInitialized )
   {
      jas_cleanup();
      __JasPerInitialized = false;
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

JPCInstance::JPCInstance( const JPCFormat* f ) :
FileFormatImplementation( f ),
path(), options(), jp2Options( JPCFormat::DefaultOptions() ),
jp2Stream( 0 ), jp2Image( 0 ), jp2CMProfile( 0 ),
queriedOptions( false )
{
}

JPCInstance::~JPCInstance()
{
   Close();
}

ImageDescriptionArray JPCInstance::Open( const String& filePath, const IsoString& /*hints*/ )
{
   Close();

   Exception::EnableConsoleOutput();

   __InitJasPer();

   try
   {
      path = filePath;

      IsoString path8 =
#ifdef __PCL_WINDOWS
         File::UnixPathToWindows( path ).ToMBS();
#else
         path.ToUTF8();
#endif
      jp2Stream = jas_stream_fopen( path8.c_str(), "rb" );
      if ( jp2Stream == 0 )
         JP2KERROR( "Unable to open JPEG2000 file" );

      int fmtid = jas_image_getfmt( jp2Stream );
      if ( fmtid < 0 )
         JP2KERROR( "JasPer: Unsupported image format" );

      Console().Write( String().Format( "<end><cbr>Decoding JPEG2000 %s format...",
                                                  IsCodeStream() ? "code stream" : "JP2" ) );
      Console().Flush();

      jp2Image = jas_image_decode( jp2Stream, fmtid, 0 );

      Console().WriteLn( " done." );

      if ( jp2Image == 0 )
         JP2KERROR( "JasPer: Unable to decode image" );

      ImageInfo info;

      info.width            = jas_image_cmptwidth( jp2Image, 0 );
      info.height           = jas_image_cmptheight( jp2Image, 0 );
      info.numberOfChannels = jas_image_numcmpts( jp2Image );
      info.colorSpace       = ColorSpace::Unknown;

      switch ( jas_clrspc_fam( jas_image_clrspc( jp2Image ) ) )
      {
         case JAS_CLRSPC_FAM_RGB:
            // Fix some JasPer's problems to recognize grayscale images.
            info.colorSpace = (info.numberOfChannels >= 3) ? ColorSpace::RGB : ColorSpace::Gray;
            break;

         case JAS_CLRSPC_FAM_GRAY:
            info.colorSpace = ColorSpace::Gray;
            break;

         case JAS_CLRSPC_FAM_YCBCR:
            JP2KERROR( "YCbCr JPEG2000 images are not supported by this implementation" );

         default:
            JP2KERROR( "Unknown color space in JPEG2000 image" );
      }

      // Verify component geometry

      for ( int c = 0; c < info.numberOfChannels; ++c )
         if ( jas_image_cmptwidth( jp2Image, c ) != info.width ||
              jas_image_cmptheight( jp2Image, c ) != info.height ||
              jas_image_cmpttlx( jp2Image, c ) != 0 ||
              jas_image_cmpttly( jp2Image, c ) != 0 ||
              jas_image_cmpthstep( jp2Image, c ) != 1 ||
              jas_image_cmptvstep( jp2Image, c ) != 1 )
         {
            JP2KERROR( "Nonuniform JPEG2000 image geometry not supported" );
         }

      info.supported = true;

      options.bitsPerSample = jas_image_cmptprec( jp2Image, 0 );

      options.embedICCProfile = jas_image_cmprof( jp2Image ) != 0;

      options.metricResolution = jp2Image->rescm_;
      if ( jp2Image->hdispres_ != 0 )
         options.xResolution = jp2Image->hdispres_;
      if ( jp2Image->vdispres_ != 0 )
         options.yResolution = jp2Image->vdispres_;

      options.embedMetadata = false; // ### TODO: Implement JP2 metadata support

      options.ieeefpSampleFormat = options.complexSample = options.embedThumbnail = false;

      ImageDescriptionArray a;
      a.Add( ImageDescription( info, options ) );
      return a;
   }

   catch ( ... )
   {
      Close();
      throw;
   }
}

bool JPCInstance::IsOpen() const
{
   return jp2Stream != 0 && jp2Image != 0;
}

String JPCInstance::FilePath() const
{
   return path;
}

void JPCInstance::Close()
{
   if ( jp2Stream != 0 )
      jas_stream_close( jp2Stream ), jp2Stream = 0;

   if ( jp2Image != 0 )
   {
      jas_image_setcmprof( jp2Image, 0 ); // don't destroy embedded profile now
      jas_image_destroy( jp2Image ), jp2Image = 0;
   }

   if ( jp2CMProfile != 0 )
      jas_cmprof_destroy( jp2CMProfile ), jp2CMProfile = 0;

   __CleanupJasPer();

   path.Clear();

   options.Reset();

   jp2Options = JPCFormat::DefaultOptions();
}

void* JPCInstance::FormatSpecificData() const
{
   JPEG2000FormatOptions* data = new JPEG2000FormatOptions( IsCodeStream() );
   data->options = jp2Options;
   return data;
}

String JPCInstance::ImageProperties() const
{
   return String().Format(
      "lossy=%s compression-rate=%.2f signed-samples=%s "
      "tiled=%s tile-width=%d tile-height=%d progressive-layer-count=%d progression-order=%d",
      jp2Options.lossyCompression ? "yes" : "no",
      jp2Options.compressionRate,
      jp2Options.signedSample ? "signed" : "unsigned",
      jp2Options.tiledImage ? "yes" : "no",
      jp2Options.tileWidth,
      jp2Options.tileHeight,
      jp2Options.numberOfLayers,
      jp2Options.progressionOrder );
}

template <class P>
static void __ReadImage( GenericImage<P>& img, jas_stream_t* jp2Stream, jas_image_t* jp2Image )
{
   int width = jas_image_cmptwidth( jp2Image, 0 );
   int height = jas_image_cmptheight( jp2Image, 0 );
   int numberOfChannels = jas_image_numcmpts( jp2Image );

   jas_matrix_t* pixels = 0;

   try
   {
      pixels = jas_matrix_create( 1, width );
      if ( pixels == 0 )
         throw Error( "Memory allocation error reading JPEG2000 image" );

      // Allocate pixel data
      img.AllocateData( width, height, numberOfChannels,
                        (jas_clrspc_fam( jas_image_clrspc( jp2Image ) ) == JAS_CLRSPC_FAM_GRAY) ?
                              ColorSpace::Gray : ColorSpace::RGB );

      for ( int c = 0; c < numberOfChannels; ++c )
      {
         int n = jas_image_cmptprec( jp2Image, c );
         bool s = jas_image_cmptsgnd( jp2Image, c ) != 0;

         for ( int y = 0; y < height; ++y )
         {
            jas_image_readcmpt( jp2Image, c, 0, y, width, 1, pixels );

            typename P::sample* f = img.ScanLine( y, c );

            if ( n == 8 )
            {
               if ( s )
                  for ( int x = 0; x < width; ++x )
                     *f++ = P::ToSample( int8( jas_matrix_get( pixels, 0, x ) ) );
               else
                  for ( int x = 0; x < width; ++x )
                     *f++ = P::ToSample( uint8( jas_matrix_get( pixels, 0, x ) ) );
            }
            else
            {
               if ( s )
                  for ( int x = 0; x < width; ++x )
                     *f++ = P::ToSample( int16( jas_matrix_get( pixels, 0, x ) ) );
               else
                  for ( int x = 0; x < width; ++x )
                     *f++ = P::ToSample( uint16( jas_matrix_get( pixels, 0, x ) ) );
            }
         }
      }

      jas_matrix_destroy( pixels ), pixels = 0;
   }

   catch ( ... )
   {
      if ( pixels != 0 )
         jas_matrix_destroy( pixels );
      throw;
   }
}

void JPCInstance::ReadImage( Image& img )
{
   __ReadImage( img, jp2Stream, jp2Image );
}

void JPCInstance::ReadImage( DImage& img )
{
   __ReadImage( img, jp2Stream, jp2Image );
}

void JPCInstance::ReadImage( UInt8Image& img )
{
   __ReadImage( img, jp2Stream, jp2Image );
}

void JPCInstance::ReadImage( UInt16Image& img )
{
   __ReadImage( img, jp2Stream, jp2Image );
}

void JPCInstance::ReadImage( UInt32Image& img )
{
   __ReadImage( img, jp2Stream, jp2Image );
}

// ----------------------------------------------------------------------------

bool JPCInstance::QueryOptions( Array<ImageOptions>& imageOptions, Array<void*>& formatOptions )
{
   queriedOptions = true;

   // Format-independent options

   ImageOptions options;
   if ( !imageOptions.IsEmpty() )
      options = *imageOptions;

   // Format-specific options

   JPEG2000FormatOptions* jpc = 0;

   if ( !formatOptions.IsEmpty() )
   {
      JPEG2000FormatOptions* o = JPEG2000FormatOptions::FromGenericDataBlock( *formatOptions );
      if ( o != 0 )
         jpc = o;
   }

   bool reusedFormatOptions = jpc != 0;
   if ( !reusedFormatOptions )
      jpc = new JPEG2000FormatOptions( IsCodeStream() );

   if ( !IsCodeStream() )
   {
      // Override embedding options, if requested.

      JP2Format::EmbeddingOverrides overrides = JP2Format::DefaultEmbeddingOverrides();

      if ( overrides.overrideICCProfileEmbedding )
         options.embedICCProfile = overrides.embedICCProfiles;

      if ( overrides.overrideMetadataEmbedding )
         options.embedMetadata = overrides.embedMetadata;

      /*
      if ( overrides.overrideThumbnailEmbedding )
         options.embedThumbnail = overrides.embedThumbnails;
      */
   }

   JPEG2000OptionsDialog dlg( options, jp2Options, IsCodeStream() );

   if ( dlg.Execute() == StdDialogCode::Ok )
   {
      jpc->options = dlg.jp2Options;

      if ( imageOptions.IsEmpty() )
         imageOptions.Add( dlg.options );
      else
         *imageOptions = dlg.options;

      if ( formatOptions.IsEmpty() )
         formatOptions.Add( (void*)jpc );
      else
         *formatOptions = (void*)jpc;

      return true;
   }

   if ( !reusedFormatOptions )
      delete jpc;

   return false;
}

void JPCInstance::Create( const String& filePath, int /*numberOfImages*/, const IsoString& hints )
{
   Close();

   Exception::EnableConsoleOutput();

   path = filePath;

   IsoStringList theHints;
   hints.Break( theHints, ' ', true/*trim*/ );
   theHints.Remove( IsoString() );
   for ( IsoStringList::const_iterator i = theHints.Begin(); i < theHints.End(); ++i )
   {
      if ( *i == "lossy" )
         jp2Options.lossyCompression = true;
      else if ( *i == "lossless" )
         jp2Options.lossyCompression = false;
      else if ( *i == "compression-rate" )
      {
         if ( ++i == theHints.End() )
            break;
         if ( i->TryToFloat( jp2Options.compressionRate ) )
            jp2Options.compressionRate = Range( jp2Options.compressionRate, 0.01F, 0.99F );
      }
      else if ( *i == "signed" )
         jp2Options.signedSample = true;
      else if ( *i == "unsigned" )
         jp2Options.signedSample = false;
      else if ( *i == "tiled" )
         jp2Options.tiledImage = true;
      else if ( *i == "untiled" )
         jp2Options.tiledImage = true;
      else if ( *i == "tile-width" )
      {
         if ( ++i == theHints.End() )
            break;
         if ( i->TryToInt( jp2Options.tileWidth ) )
            jp2Options.tileWidth = Range( jp2Options.tileWidth, 16, 8192 );
      }
      else if ( *i == "tile-height" )
      {
         if ( ++i == theHints.End() )
            break;
         if ( i->TryToInt( jp2Options.tileHeight ) )
            jp2Options.tileHeight = Range( jp2Options.tileHeight, 16, 8192 );
      }
      else if ( *i == "layers" )
      {
         if ( ++i == theHints.End() )
            break;
         if ( i->TryToInt( jp2Options.numberOfLayers ) )
            jp2Options.numberOfLayers = Range( jp2Options.numberOfLayers, 1, 10 );
      }
      else if ( *i == "lrcp" )
         jp2Options.progressionOrder = JPEG2000ProgressionOrder::LRCP;
      else if ( *i == "rlcp" )
         jp2Options.progressionOrder = JPEG2000ProgressionOrder::RLCP;
      else if ( *i == "rpcl" )
         jp2Options.progressionOrder = JPEG2000ProgressionOrder::RPCL;
      else if ( *i == "pcrl" )
         jp2Options.progressionOrder = JPEG2000ProgressionOrder::PCRL;
      else if ( *i == "cprl" )
         jp2Options.progressionOrder = JPEG2000ProgressionOrder::CPRL;
   }
}

void JPCInstance::SetOptions( const ImageOptions& imageOptions )
{
   if ( path.IsEmpty() )
      throw Error( "JPEG-2000 format: Attempt to set image options before creating a file" );

   options = imageOptions;

   // Override embedding options, if requested and not already asked them.

   if ( !IsCodeStream() && !queriedOptions )
   {
      JP2Format::EmbeddingOverrides overrides = JP2Format::DefaultEmbeddingOverrides();

      if ( overrides.overrideICCProfileEmbedding )
         options.embedICCProfile = overrides.embedICCProfiles;

      if ( overrides.overrideMetadataEmbedding )
         options.embedMetadata = overrides.embedMetadata;

      /*
      if ( overrides.overrideThumbnailEmbedding )
         options.embedThumbnail = overrides.embedThumbnails;
      */
   }
}

void JPCInstance::SetFormatSpecificData( const void* data )
{
   if ( path.IsEmpty() )
      throw Error( "JPEG-2000 format: Attempt to set format-specific options before creating a file" );

   const JPEG2000FormatOptions* o = JPEG2000FormatOptions::FromGenericDataBlock( data );
   if ( o != 0 )
      jp2Options = o->options;
}

void JPCInstance::__CreateImage( const ImageInfo& info )
{
   if ( path.IsEmpty() )
      throw Error( "JPEG-2000 format: Attempt to write an image before creating a file" );

   __InitJasPer();

   if ( !info.IsValid() )
      JP2KERROR( "Invalid image parameters in JPEG2000 file creation" );

   if ( info.colorSpace != ColorSpace::RGB && info.colorSpace != ColorSpace::Gray )
      JP2KERROR( "Unsupported color space in JPEG2000 file creation" );

   if ( options.bitsPerSample != 8 )
      if ( jp2Options.lossyCompression || options.bitsPerSample < 8 )
         options.bitsPerSample = 8;
      else if ( options.bitsPerSample != 16 )
         options.bitsPerSample = 16;

   if ( jp2CMProfile == 0 )
      options.embedICCProfile = false;

   options.ieeefpSampleFormat = options.complexSample = options.embedThumbnail = false;

   IsoString path8 =
#ifdef __PCL_WINDOWS
      File::UnixPathToWindows( path ).ToMBS();
#else
      path.ToUTF8();
#endif
   jp2Stream = jas_stream_fopen( path8.c_str(), "wb" );
   if ( jp2Stream == 0 )
      JP2KERROR( "Unable to create JPEG2000 file" );

   jp2Image = jas_image_create0();
   if ( jp2Image == 0 )
      JP2KERROR( "Unable to create JPEG2000 image" );

   for ( int c = 0; c < info.numberOfChannels; ++c )
   {
      jas_image_cmptparm_t p;
      ::memset( &p, 0, sizeof( jas_image_cmptparm_t ) );
      p.tlx = p.tly = 0;               // top-left corner position
      p.hstep = p.vstep = 1;           // coordinate grid step sizes
      p.width = info.width;            // width in pixels
      p.height = info.height;          // height in pixels
      p.prec = options.bitsPerSample;  // bit depth: 8 or 16 bits
      p.sgnd = jp2Options.signedSample;   // signed or unsigned samples

      if ( jas_image_addcmpt( jp2Image, c, &p ) != 0 )
         JP2KERROR( "Unable to create JPEG2000 image component" );
   }

   if ( info.colorSpace == ColorSpace::Gray )
   {
      jas_image_setclrspc( jp2Image, options.embedICCProfile ? JAS_CLRSPC_GENGRAY : JAS_CLRSPC_SGRAY );

      jas_image_setcmpttype( jp2Image, 0, JAS_IMAGE_CT_COLOR( JAS_CLRSPC_CHANIND_GRAY_Y ) );

      if ( info.numberOfChannels > 1 )
         jas_image_setcmpttype( jp2Image, 1, JAS_IMAGE_CT_COLOR( JAS_IMAGE_CT_OPACITY ) );
   }
   else
   {
      jas_image_setclrspc( jp2Image, options.embedICCProfile ? JAS_CLRSPC_GENRGB : JAS_CLRSPC_SRGB );

      jas_image_setcmpttype( jp2Image, 0, JAS_IMAGE_CT_COLOR( JAS_CLRSPC_CHANIND_RGB_R ) );
      jas_image_setcmpttype( jp2Image, 1, JAS_IMAGE_CT_COLOR( JAS_CLRSPC_CHANIND_RGB_G ) );
      jas_image_setcmpttype( jp2Image, 2, JAS_IMAGE_CT_COLOR( JAS_CLRSPC_CHANIND_RGB_B ) );

      if ( info.numberOfChannels > 3 )
         jas_image_setcmpttype( jp2Image, 3, JAS_IMAGE_CT_COLOR( JAS_IMAGE_CT_OPACITY ) );
   }

   if ( options.embedICCProfile )
      jas_image_setcmprof( jp2Image, jp2CMProfile );

   if ( jp2Options.resolutionData )
   {
      jp2Image->rescm_ = options.metricResolution;
      jp2Image->hdispres_ = RoundI( options.xResolution );
      jp2Image->vdispres_ = RoundI( options.yResolution );
   }
}

template <class P>
static void __WriteImage( const GenericImage<P>& img,
                          const ImageInfo& info, const ImageOptions& options,
                          jas_stream_t* jp2Stream, jas_image_t* jp2Image, int jp2Format,
                          const JPEG2000ImageOptions& jp2Options )
{
   jas_matrix_t* pixels = 0;

   try
   {
      pixels = jas_matrix_create( 1, img.Width() );
      if ( pixels == 0 )
         throw Error( "Memory allocation error writing JPEG2000 image." );

      for ( int c = 0; c < img.NumberOfChannels(); ++c )
      {
         for ( int y = 0; y < img.Height(); ++y )
         {
            const typename P::sample* f = img.ScanLine( y, c );

            if ( options.bitsPerSample == 8 )
            {
               if ( jp2Options.signedSample )
               {
                  int8 v;
                  for ( int x = 0; x < img.Width(); ++x )
                  {
                     P::FromSample( v, *f++ );
                     jas_matrix_set( pixels, 0, x, v );
                  }
               }
               else
               {
                  uint8 v;
                  for ( int x = 0; x < img.Width(); ++x )
                  {
                     P::FromSample( v, *f++ );
                     jas_matrix_set( pixels, 0, x, v );
                  }
               }
            }
            else
            {
               if ( jp2Options.signedSample )
               {
                  int16 v;
                  for ( int x = 0; x < img.Width(); ++x )
                  {
                     P::FromSample( v, *f++ );
                     jas_matrix_set( pixels, 0, x, v );
                  }
               }
               else
               {
                  uint16 v;
                  for ( int x = 0; x < img.Width(); ++x )
                  {
                     P::FromSample( v, *f++ );
                     jas_matrix_set( pixels, 0, x, v );
                  }
               }
            }

            jas_image_writecmpt( jp2Image, c, 0, y, img.Width(), 1, pixels );
         }
      }

      IsoString jp2OptionsStr;

      jp2OptionsStr.AppendFormat( "mode=%s", jp2Options.lossyCompression ? "real" : "int" );

      if ( jp2Options.lossyCompression )
         jp2OptionsStr.AppendFormat( " rate=%g", jp2Options.compressionRate );

      if ( jp2Options.tiledImage )
      {
         jp2OptionsStr.AppendFormat( " tilewidth=%d", Range( jp2Options.tileWidth, 8, img.Width() ) );
         jp2OptionsStr.AppendFormat( " tileheight=%d", Range( jp2Options.tileHeight, 8, img.Height() ) );
      }

      if ( jp2Options.numberOfLayers > 1 )
      {
         jp2OptionsStr.Append( " ilyrrates=" );

         float dr = (jp2Options.lossyCompression ? jp2Options.compressionRate : 1.0F)/jp2Options.numberOfLayers;

         for ( int l = 1; ; )
         {
            jp2OptionsStr.AppendFormat( "%g", l*dr );

            if ( ++l == jp2Options.numberOfLayers )
               break;

            jp2OptionsStr.Append( ',' );
         }

         jp2OptionsStr.Append( " prg=" );

         switch ( jp2Options.progressionOrder )
         {
         default:
         case JPEG2000ProgressionOrder::LRCP: jp2OptionsStr.Append( "lrcp" ); break;
         case JPEG2000ProgressionOrder::RLCP: jp2OptionsStr.Append( "rlcp" ); break;
         case JPEG2000ProgressionOrder::RPCL: jp2OptionsStr.Append( "rpcl" ); break;
         case JPEG2000ProgressionOrder::PCRL: jp2OptionsStr.Append( "pcrl" ); break;
         case JPEG2000ProgressionOrder::CPRL: jp2OptionsStr.Append( "cprl" ); break;
         }
      }

      if ( jas_image_encode( jp2Image, jp2Stream, jp2Format, jp2OptionsStr.c_str() ) < 0 )
         throw Error( "Unable to encode JPEG2000 image." );

      jas_matrix_destroy( pixels ), pixels = 0;
   }

   catch ( ... )
   {
      if ( pixels != 0 )
         jas_matrix_destroy( pixels );
      throw;
   }
}

#define IMPLEMENT_WRITE_IMAGE()                                                                    \
   try                                                                                             \
   {                                                                                               \
      Console().Write( String().Format( "<end><cbr>Encoding JPEG2000 %s format...",      \
                                                   IsCodeStream() ? "code stream" : "JP2" ) );     \
      Console().Flush();                                                                 \
                                                                                                   \
      ImageInfo info( img );                                                                       \
      __CreateImage( info );                                                                       \
      __WriteImage( img, info, options, jp2Stream, jp2Image,                                       \
                    jas_image_fmtfromname( (char*)(IsCodeStream() ? ".jpc" : ".jp2") ),            \
                    jp2Options );                                                                  \
                                                                                                   \
      Console().WriteLn( " done." );                                                     \
   }                                                                                               \
                                                                                                   \
   catch ( ... )                                                                                   \
   {                                                                                               \
      Close();                                                                                     \
      throw;                                                                                       \
   }

void JPCInstance::WriteImage( const Image& img )
{
   IMPLEMENT_WRITE_IMAGE()
}

void JPCInstance::WriteImage( const DImage& img )
{
   IMPLEMENT_WRITE_IMAGE()
}

void JPCInstance::WriteImage( const UInt8Image& img )
{
   IMPLEMENT_WRITE_IMAGE()
}

void JPCInstance::WriteImage( const UInt16Image& img )
{
   IMPLEMENT_WRITE_IMAGE()
}

void JPCInstance::WriteImage( const UInt32Image& img )
{
   IMPLEMENT_WRITE_IMAGE()
}

bool JPCInstance::WasLossyWrite() const
{
   return jp2Options.lossyCompression;
}

bool JPCInstance::IsCodeStream() const
{
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

JP2Instance::JP2Instance( const JP2Format* f ) : JPCInstance( f )
{
   jp2Options.Assign( JP2Format::DefaultOptions() );
}

JP2Instance::~JP2Instance()
{
}

void JP2Instance::Extract( ICCProfile& icc )
{
   if ( path.IsEmpty() || !__JasPerInitialized )
      throw Error( "JPEG-2000 format: Attempt to extract an ICC profile before opening a file" );

   icc.Clear();

   if ( jas_image_cmprof( jp2Image ) != 0 )
   {
      jas_stream_t* iccStream = 0;
      try
      {
         // Create a growable memory stream for output
         iccStream = jas_stream_memopen( 0, 0 );

         if ( iccStream == 0 )
            JP2KERROR( "Extracting ICC profile from JPEG2000 image: Unable to create JasPer stream" );

         if ( jas_iccprof_save( jas_image_cmprof( jp2Image )->iccprof, iccStream ) < 0 )
            JP2KERROR( "Extracting ICC profile from JPEG2000 image: Error saving profile to JasPer stream" );

         long iccSize = jas_stream_tell( iccStream );

         if ( iccSize <= 0 )
            JP2KERROR( "Extracting ICC profile from JPEG2000 image: Invalid JasPer stream position" );

         ByteArray iccData( iccSize );

         jas_stream_rewind( iccStream );

         if ( jas_stream_read( iccStream, iccData.Begin(), iccSize ) != iccSize )
            JP2KERROR( "Extracting ICC profile from JPEG2000 image: Error reading JasPer stream" );

         jas_stream_close( iccStream );
         iccStream = 0;

         icc.Set( iccData );
         if ( icc.IsProfile() )
            Console().WriteLn( "<end><cbr>ICC profile extracted: \'" + icc.Description() + "\', " + String( icc.ProfileSize() ) + " bytes" );
      }
      catch ( ... )
      {
         if ( iccStream != 0 )
            jas_stream_close( iccStream );
         Close();
         throw;
      }
   }
}

void JP2Instance::Embed( const ICCProfile& icc )
{
   if ( path.IsEmpty() )
      throw Error( "JPEG-2000 format: Attempt to embed an ICC profile before creating a file" );

   if ( jp2CMProfile != 0 )
      jas_cmprof_destroy( jp2CMProfile ), jp2CMProfile = 0;

   if ( icc.IsProfile() )
   {
      jas_iccprof_t* jp2ICCProfile = 0;

      try
      {
         ByteArray iccData = icc.ProfileData();
         iccData.SetUnique();

         jp2ICCProfile = jas_iccprof_createfrombuf( iccData.Begin(), int( icc.ProfileSize() ) );
         if ( jp2ICCProfile == 0 )
            JP2KERROR( "Unable to generate embedded ICC profile in JPEG2000 image" );

         jp2CMProfile = jas_cmprof_createfromiccprof( jp2ICCProfile );
         if ( jp2CMProfile == 0 )
            JP2KERROR( "Unable to embed ICC profile in JPEG2000 image" );

         jas_iccprof_destroy( jp2ICCProfile ), jp2ICCProfile = 0;

         Console().WriteLn( "<end><cbr>ICC profile embedded: \'" + icc.Description() + "\', " + String( icc.ProfileSize() ) + " bytes" );
      }
      catch ( ... )
      {
         if ( jp2ICCProfile != 0 )
            jas_iccprof_destroy( jp2ICCProfile );
         Close();
         throw;
      }
   }
}

bool JP2Instance::IsCodeStream() const
{
   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF JPEG2000Instance.cpp - Released 2014/10/29 07:34:49 UTC
