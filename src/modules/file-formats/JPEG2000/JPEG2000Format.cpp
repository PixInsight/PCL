//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard JPEG2000 File Format Module Version 01.00.02.0289
// ----------------------------------------------------------------------------
// JPEG2000Format.cpp - Released 2017-04-14T23:07:03Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG2000 PixInsight module.
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

#include "JPEG2000Format.h"
#include "JPEG2000PreferencesDialog.h"

#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

JPCFormat::JPCFormat() : MetaFileFormat()
{
}

IsoString JPCFormat::Name() const
{
   return "JPC";
}

StringList JPCFormat::FileExtensions() const
{
   return StringList() << ".jpc" << ".j2c";
}

IsoStringList JPCFormat::MimeTypes() const
{
   return IsoStringList();
}

uint32 JPCFormat::Version() const
{
   return 0x102;
}

String JPCFormat::Description() const
{
   return "JPEG-2000 Code Stream File Format.";
}

String JPCFormat::Implementation() const
{
   return

   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>This implementation of the JPEG2000 format is based on the JasPer "
   "Software Library (version 1.900.1), written by Michael Adams.</p>"

   "<p>JasPer Software Library:<br/>"
   "Copyright (c) 1999-2000 Image Power, Inc.<br/>"
   "Copyright (c) 1999-2000 The University of British Columbia<br/>"
   "Copyright (c) 2001-2007 Michael David Adams</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2017, Pleiades Astrophoto</p>"

   "<p style=\"white-space:pre;\">"
"\n-------------------------------------------------------------------------------"
"\nJPEG2000 Format Hints         Description"
"\n============================  ================================================="
"\nlossy                   ( w)  Write a lossy JPEG-2000 image."
"\n-------------------------------------------------------------------------------"
"\nlossless                ( w)  Write a lossless JPEG-2000 image."
"\n-------------------------------------------------------------------------------"
"\ncompression-rate n      ( w)  n is a compression ratio in the range [0.01,0.99]"
"\n                              expressed as compressed_size/original_size."
"\n-------------------------------------------------------------------------------"
"\nsigned                  ( w)  Write signed integers."
"\n-------------------------------------------------------------------------------"
"\nunsigned                ( w)  Write unsigned integers."
"\n-------------------------------------------------------------------------------"
"\ntiled                   ( w)  Generate a tiled JPEG-2000 image."
"\n-------------------------------------------------------------------------------"
"\nuntiled                 ( w)  Do not generate a tiled JPEG-2000 image."
"\n-------------------------------------------------------------------------------"
"\ntile-width n            ( w)  n is the tile width in pixels, in [16,8192]."
"\n-------------------------------------------------------------------------------"
"\ntile-height n           ( w)  n is the tile height in pixels, in [16,8192]."
"\n-------------------------------------------------------------------------------"
"\nlayers n                ( w)  n is the number of decompression layers, in the"
"\n                              range [1,10]. n > 1 means progressive JPEG-2000."
"\n-------------------------------------------------------------------------------"
"\nlrcp                    ( w)  Use the Layer-Resolution-Component-Position"
"\n                              progression order."
"\n-------------------------------------------------------------------------------"
"\nrlcp                    ( w)  Use the Resolution-Layer-Component-Position"
"\n                              progression order."
"\n-------------------------------------------------------------------------------"
"\nrpcl                    ( w)  Use the Resolution-Position-Component-Layer"
"\n                              progression order."
"\n-------------------------------------------------------------------------------"
"\npcrl                    ( w)  Use the Position-Component-Resolution-Layer"
"\n                              progression order."
"\n-------------------------------------------------------------------------------"
"\ncprl                    ( w)  Use the Component-Position-Resolution-Layer"
"\n                              progression order."
"\n-------------------------------------------------------------------------------"
"\n"
   "</p>"
   "</html>";
}

String JPCFormat::IconImageFile() const
{
   return ":/file-format/jpc-format-icon.png";
}

bool JPCFormat::CanEditPreferences() const
{
   return true;
}

bool JPCFormat::UsesFormatSpecificData() const
{
   return true;
}

bool JPCFormat::ValidateFormatSpecificData( const void* data ) const
{
   return JPEG2000FormatOptions::FromGenericDataBlock( data ) != nullptr;
}

void JPCFormat::DisposeFormatSpecificData( void* data ) const
{
   JPEG2000FormatOptions* o = JPEG2000FormatOptions::FromGenericDataBlock( data );
   if ( o != nullptr )
      delete o;
}

FileFormatImplementation* JPCFormat::Create() const
{
   return new JPCInstance( this );
}

bool JPCFormat::EditPreferences() const
{
   JPEG2000ImageOptions options = DefaultOptions();
   JP2Format::EmbeddingOverrides dummy;

   JPEG2000PreferencesDialog dlg( dummy, options, true );

   if ( dlg.Execute() == StdDialogCode::Ok )
   {
      options = dlg.jp2Options;

      Settings::Write( "JPCLossyCompression", options.lossyCompression );
      Settings::Write( "JPCCompressionRate",  options.compressionRate );
      Settings::Write( "JPCSignedSample",     options.signedSample );
      Settings::Write( "JPCResolutionData",   options.resolutionData );
      Settings::Write( "JPCTiledImage",       options.tiledImage );
      Settings::Write( "JPCTileWidth",        options.tileWidth );
      Settings::Write( "JPCTileHeight",       options.tileHeight );
      Settings::Write( "JPCNumberOfLayers",   options.numberOfLayers );
      Settings::Write( "JPCProgressionOrder", options.progressionOrder );

      return true;
   }

   return false;
}

JPEG2000ImageOptions JPCFormat::DefaultOptions()
{
   JPEG2000ImageOptions options;

   Settings::Read( "JPCLossyCompression", options.lossyCompression );
   Settings::Read( "JPCCompressionRate",  options.compressionRate );
   Settings::Read( "JPCSignedSample",     options.signedSample );
   Settings::Read( "JPCResolutionData",   options.resolutionData );
   Settings::Read( "JPCTiledImage",       options.tiledImage );
   Settings::Read( "JPCTileWidth",        options.tileWidth );
   Settings::Read( "JPCTileHeight",       options.tileHeight );
   Settings::Read( "JPCNumberOfLayers",   options.numberOfLayers );
   Settings::Read( "JPCProgressionOrder", options.progressionOrder );

   return options;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

JP2Format::JP2Format() : JPCFormat()
{
}

IsoString JP2Format::Name() const
{
   return "JP2";
}

StringList JP2Format::FileExtensions() const
{
   return StringList() << ".jp2" << ".jpx" << ".jpk" << ".j2k";
}

IsoStringList JP2Format::MimeTypes() const
{
   // RFC3745
   return IsoStringList() << "image/jp2" << "image/jpeg2000" << "image/jpeg2000-image" << "image/x-jpeg2000-image";
}

String JP2Format::Description() const
{
   return "JPEG-2000 JP2 File Format.";
}

String JP2Format::IconImageFile() const
{
   return ":/file-format/jp2-format-icon.png";
}

bool JP2Format::CanStoreICCProfiles() const
{
   return true;
}

bool JP2Format::CanStoreResolution() const
{
   return true;
}

FileFormatImplementation* JP2Format::Create() const
{
   return new JP2Instance( this );
}

bool JP2Format::EditPreferences() const
{
   JPEG2000ImageOptions options = DefaultOptions();
   EmbeddingOverrides overrides = DefaultEmbeddingOverrides();

   JPEG2000PreferencesDialog dlg( overrides, options, false );

   if ( dlg.Execute() == StdDialogCode::Ok )
   {
      options = dlg.jp2Options;
      overrides = dlg.overrides;

      Settings::Write( "JP2LossyCompression", options.lossyCompression );
      Settings::Write( "JP2CompressionRate",  options.compressionRate );
      Settings::Write( "JP2SignedSample",     options.signedSample );
      Settings::Write( "JP2ResolutionData",   options.resolutionData );
      Settings::Write( "JP2TiledImage",       options.tiledImage );
      Settings::Write( "JP2TileWidth",        options.tileWidth );
      Settings::Write( "JP2TileHeight",       options.tileHeight );
      Settings::Write( "JP2NumberOfLayers",   options.numberOfLayers );
      Settings::Write( "JP2ProgressionOrder", options.progressionOrder );

      Settings::Write( "JP2OverrideICCProfileEmbedding", overrides.overrideICCProfileEmbedding );
      Settings::Write( "JP2EmbedICCProfiles",            overrides.embedICCProfiles );

      return true;
   }

   return false;
}

JPEG2000ImageOptions JP2Format::DefaultOptions()
{
   JPEG2000ImageOptions options;

   Settings::Read( "JP2LossyCompression", options.lossyCompression );
   Settings::Read( "JP2CompressionRate",  options.compressionRate );
   Settings::Read( "JP2SignedSample",     options.signedSample );
   Settings::Read( "JP2ResolutionData",   options.resolutionData );
   Settings::Read( "JP2TiledImage",       options.tiledImage );
   Settings::Read( "JP2TileWidth",        options.tileWidth );
   Settings::Read( "JP2TileHeight",       options.tileHeight );
   Settings::Read( "JP2NumberOfLayers",   options.numberOfLayers );
   Settings::Read( "JP2ProgressionOrder", options.progressionOrder );

   return options;
}

JP2Format::EmbeddingOverrides JP2Format::DefaultEmbeddingOverrides()
{
   JP2Format::EmbeddingOverrides overrides;

   Settings::Read( "JP2OverrideICCProfileEmbedding", overrides.overrideICCProfileEmbedding );
   Settings::Read( "JP2EmbedICCProfiles",            overrides.embedICCProfiles );

   return overrides;
}

// ----------------------------------------------------------------------------

#define JP2K_SIGNATURE  0x4A50324Bu  // JP2K

JPEG2000FormatOptions::JPEG2000FormatOptions( bool isCodeStream ) :
   signature( JP2K_SIGNATURE ),
   options( isCodeStream ? JPCFormat::DefaultOptions() : JP2Format::DefaultOptions() )
{
}

JPEG2000FormatOptions::JPEG2000FormatOptions( const JPEG2000FormatOptions& x ) :
   signature( JP2K_SIGNATURE ), options( x.options )
{
}

JPEG2000FormatOptions* JPEG2000FormatOptions::FromGenericDataBlock( const void* data )
{
   if ( data == nullptr )
      return nullptr;
   const JPEG2000FormatOptions* o = reinterpret_cast<const JPEG2000FormatOptions*>( data );
   if ( o->signature != JP2K_SIGNATURE )
      return nullptr;
   return const_cast<JPEG2000FormatOptions*>( o );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF JPEG2000Format.cpp - Released 2017-04-14T23:07:03Z
