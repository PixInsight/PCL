//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard XISF File Format Module Version 01.00.09.0135
// ----------------------------------------------------------------------------
// XISFFormat.cpp - Released 2017-05-02T09:42:51Z
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

#include "XISFFormat.h"
#include "XISFInstance.h"
#include "XISFPreferencesDialog.h"

#include <pcl/ErrorHandler.h>
#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

XISFFormat::XISFFormat() : MetaFileFormat()
{
}

IsoString XISFFormat::Name() const
{
   return "XISF";
}

StringList XISFFormat::FileExtensions() const
{
   return StringList() << ".xisf";
}

IsoStringList XISFFormat::MimeTypes() const
{
   return IsoStringList();
}

uint32 XISFFormat::Version() const
{
   return 0x100;
}

String XISFFormat::Description() const
{
   return

   "<html>"
   "<p>XISF - Extensible Image Serialization Format Version 1.0</p>"

   "<p>This implementation supports a large subset of the XISF Version 1.0 "
   "Specification DRAFT 9.4 for monolithic XISF units.</p>"

   "<p>For the latest XISF specification document, please visit:</p>"

   "<p>http://pixinsight.com/doc/docs/XISF-1.0-spec/XISF-1.0-spec.html</p>"
   "</html>";
}

String XISFFormat::Implementation() const
{
   return

   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2017, Pleiades Astrophoto</p>"

   "<p style=\"white-space:pre;\">"
"\n-------------------------------------------------------------------------------"
"\nXISF Format Hints             Description"
"\n============================  ================================================="
"\nnormalize               (r )  Normalize floating point real pixel data to the"
"\n                              [0,1] range. Normalize integer data to [0,2^n-1]."
"\n-------------------------------------------------------------------------------"
"\nno-normalize            (r )  Do not normalize pixel data; load it exactly as"
"\n                              has been stored in the XISF unit, including wrong"
"\n                              out-of-range values and non-numeric entities."
"\n-------------------------------------------------------------------------------"
"\nimport-fits-keywords    (r )  Import FITS header keywords as image properties."
"\n-------------------------------------------------------------------------------"
"\nno-import-fits-keywords (r )  Do not import FITS header keywords as image"
"\n                              properties."
"\n-------------------------------------------------------------------------------"
"\nauto-metadata           (r )  Automatically generate a number of metadata"
"\n                              properties to describe the XISF unit."
"\n-------------------------------------------------------------------------------"
"\nno-auto-metadata        (r )  Do not generate metadata automatically."
"\n-------------------------------------------------------------------------------"
"\nembedded-data           (rw)  Read/write embedded image data."
"\n-------------------------------------------------------------------------------"
"\nno-embedded-data        (rw)  Do not read/write embedded image data."
"\n-------------------------------------------------------------------------------"
"\nproperties              (rw)  Read/write image and unit properties."
"\n-------------------------------------------------------------------------------"
"\nno-properties           (rw)  Do not read/write image and unit properties."
"\n-------------------------------------------------------------------------------"
"\nfits-keywords           (rw)  Read/write FITS header keywords, for legacy"
"\n                              format compatibility."
"\n-------------------------------------------------------------------------------"
"\nno-fits-keywords        (rw)  Do not read/write FITS header keywords."
"\n-------------------------------------------------------------------------------"
"\nwarnings                (rw)  Generate warning messages."
"\n-------------------------------------------------------------------------------"
"\nno-warnings             (rw)  Do not generate warning messages."
"\n-------------------------------------------------------------------------------"
"\nwarnings-are-errors     (rw)  Treat warning messages as errors."
"\n-------------------------------------------------------------------------------"
"\nno-warnings-are-errors  (rw)  Do not treat warning messages as errors"
"\n-------------------------------------------------------------------------------"
"\nverbosity n             (rw)  n is a verbosity level in the range [0,3] to"
"\n                              control the amount of generated messages"
"\n                              (default = 1)."
"\n-------------------------------------------------------------------------------"
"\ncompression-codec id    ( w)  id is the identifier of a compression codec, one"
"\n                              of: zlib, zlib+sh, lz4, lz4+sh, lz4hc, lz4hc+sh."
"\n-------------------------------------------------------------------------------"
"\ncompression-level n     ( w)  n is an abstract compression level in the range"
"\n                              [0,100]. Higher levels compress more, lower"
"\n                              levels compress faster."
"\n-------------------------------------------------------------------------------"
"\nno-compression          ( w)  Do not generate compressed data blocks."
"\n-------------------------------------------------------------------------------"
"\nchecksums id            ( w)  id is a block checksum algorithm, one of: sha1,"
"\n                              sha256, sha512."
"\n-------------------------------------------------------------------------------"
"\nno-checksums            ( w)  Do not generate block checksums."
"\n-------------------------------------------------------------------------------"
"\nblock-alignment n       ( w)  Align data blocks to multiples of n bytes, n in"
"\n                              the range [0,65536]."
"\n-------------------------------------------------------------------------------"
"\nno-block-alignment      ( w)  Write unaligned data blocks."
"\n-------------------------------------------------------------------------------"
"\nmax-inline-block-size n ( w)  n is the maximum length of a block in bytes to be"
"\n                              generated as an inline or embedded block."
"\n-------------------------------------------------------------------------------"
"\ncfa-type id             ( w)  id is the identifier of a color filter array"
"\n                              (CFA) that will be associated with the generated"
"\n                              images (* obsolete *)"
"\n-------------------------------------------------------------------------------"
"\nno-cfa                  ( w)  Do not generate color filter array (CFA) image"
"\n                              properties (* obsolete *)"
"\n-------------------------------------------------------------------------------"
"\nresolution n            ( w)  n is an image resolution in pixels per resolution"
"\n                              unit (inches or centimeters)"
"\n-------------------------------------------------------------------------------"
"\nno-resolution           ( w)  Do not generate image resolution metadata."
"\n-------------------------------------------------------------------------------"
"\nresolution-unit id      ( w)  id is a resolution unit, one of: inch, cm."
"\n-------------------------------------------------------------------------------"
"\nimage-ids ids           ( w)  ids is a comma-separated list of image"
"\n                              identifiers, which will be associated with the"
"\n                              corresponding images serialized in the XISF unit."
"\n-------------------------------------------------------------------------------"
"\nlower-bound n           ( w)  n is the lower bound of the output range for"
"\n                              generated pixel sample data."
"\n-------------------------------------------------------------------------------"
"\nupper-bound n           ( w)  n is the upper bound of the output range for"
"\n                              generated pixel sample data."
"\n-------------------------------------------------------------------------------"
"\n"
   "</p>"
   "</html>";
}

String XISFFormat::IconImageFile() const
{
   return ":/file-format/xisf-format-icon.png";
}

bool XISFFormat::CanReadIncrementally() const
{
   return true;
}

bool XISFFormat::CanWriteIncrementally() const
{
   return true;
}

bool XISFFormat::CanStore32Bit() const
{
   return true;
}

bool XISFFormat::CanStore64Bit() const
{
   return false;
}

bool XISFFormat::CanStoreFloat() const
{
   return true;
}

bool XISFFormat::CanStoreDouble() const
{
   return true;
}

bool XISFFormat::CanStoreComplex() const
{
   return true;
}

bool XISFFormat::CanStoreDComplex() const
{
   return true;
}

bool XISFFormat::CanStoreResolution() const
{
   return true;
}

bool XISFFormat::CanStoreKeywords() const
{
   return true;
}

bool XISFFormat::CanStoreICCProfiles() const
{
   return true;
}

bool XISFFormat::CanStoreThumbnails() const
{
   return true;
}

bool XISFFormat::CanStoreProperties() const
{
   return true;
}

bool XISFFormat::CanStoreImageProperties() const
{
   return true;
}

bool XISFFormat::CanStoreRGBWS() const
{
   return true;
}

bool XISFFormat::CanStoreDisplayFunctions() const
{
   return true;
}

bool XISFFormat::CanStoreColorFilterArrays() const
{
   return true;
}

bool XISFFormat::SupportsCompression() const
{
   return true;
}

bool XISFFormat::SupportsMultipleImages() const
{
   return true;
}

bool XISFFormat::SupportsViewProperties() const
{
   return true;
}

bool XISFFormat::CanEditPreferences() const
{
   return true;
}

bool XISFFormat::UsesFormatSpecificData() const
{
   return true;
}

bool XISFFormat::ValidateFormatSpecificData( const void* data ) const
{
   return FormatOptions::FromGenericDataBlock( data ) != nullptr;
}

void XISFFormat::DisposeFormatSpecificData( void* data ) const
{
   if ( FormatOptions* o = FormatOptions::FromGenericDataBlock( data ) )
      delete o;
}

FileFormatImplementation* XISFFormat::Create() const
{
   return new XISFInstance( this );
}

bool XISFFormat::EditPreferences() const
{
   EmbeddingOverrides overrides = DefaultEmbeddingOverrides();
   XISFOptions options = DefaultOptions();

   XISFPreferencesDialog dlg( overrides, options );

   if ( dlg.Execute() == StdDialogCode::Ok )
   {
      overrides = dlg.overrides;
      Settings::Write( "XISFOverridePropertyEmbedding",        overrides.overridePropertyEmbedding );
      Settings::Write( "XISFEmbedProperties",                  overrides.embedProperties );
      Settings::Write( "XISFOverrideICCProfileEmbedding",      overrides.overrideICCProfileEmbedding );
      Settings::Write( "XISFEmbedICCProfiles",                 overrides.embedICCProfiles );
      Settings::Write( "XISFOverrideDisplayFunctionEmbedding", overrides.overrideDisplayFunctionEmbedding );
      Settings::Write( "XISFEmbedDisplayFunctions",            overrides.embedDisplayFunctions );
      Settings::Write( "XISFOverrideRGBWorkingSpaceEmbedding", overrides.overrideRGBWorkingSpaceEmbedding );
      Settings::Write( "XISFEmbedRGBWorkingSpaces",            overrides.embedRGBWorkingSpaces );
      Settings::Write( "XISFOverrideThumbnailEmbedding",       overrides.overrideThumbnailEmbedding );
      Settings::Write( "XISFEmbedThumbnails",                  overrides.embedThumbnails );
      Settings::Write( "XISFOverridePreviewRectsEmbedding",    overrides.overridePreviewRectsEmbedding );
      Settings::Write( "XISFEmbedPreviewRects",                overrides.embedPreviewRects );

      options = dlg.options;
      Settings::Write( "XISFStoreFITSKeywords",  options.storeFITSKeywords );
      Settings::Write( "XISFIgnoreFITSKeywords", options.ignoreFITSKeywords );
      Settings::Write( "XISFImportFITSKeywords", options.importFITSKeywords );
      Settings::Write( "XISFIgnoreEmbeddedData", options.ignoreEmbeddedData );
      Settings::Write( "XISFIgnoreProperties",   options.ignoreProperties );
      Settings::Write( "XISFAutoMetadata",       options.autoMetadata );
      Settings::Write( "XISFCompressionCodec",   options.compressionCodec );
      Settings::Write( "XISFCompressionLevel",   options.compressionLevel );
      Settings::Write( "XISFChecksums",          options.checksumAlgorithm );
      Settings::Write( "XISFBlockAlignmentSize", options.blockAlignmentSize );
      Settings::Write( "XISFMaxInlineBlockSize", options.maxInlineBlockSize );

      return true;
   }

   return false;
}

XISFOptions XISFFormat::DefaultOptions()
{
   XISFOptions options;
   bool   b;
   uint8  u8;
   uint16 u16;

   b = options.storeFITSKeywords;
   Settings::Read( "XISFStoreFITSKeywords", b );
   options.storeFITSKeywords = b;

   b = options.ignoreFITSKeywords;
   Settings::Read( "XISFIgnoreFITSKeywords", b );
   options.ignoreFITSKeywords = b;

   b = options.importFITSKeywords;
   Settings::Read( "XISFImportFITSKeywords", b );
   options.importFITSKeywords = b;

   b = options.ignoreEmbeddedData;
   Settings::Read( "XISFIgnoreEmbeddedData", b );
   options.ignoreEmbeddedData = b;

   b = options.ignoreProperties;
   Settings::Read( "XISFIgnoreProperties", b );
   options.ignoreProperties = b;

   b = options.autoMetadata;
   Settings::Read( "XISFAutoMetadata", b );
   options.autoMetadata = b;

   u8 = options.compressionCodec;
   Settings::ReadU( "XISFCompressionCodec", u8 );
   options.compressionCodec = XISF::block_compression( u8 );

   u8 = options.compressionLevel;
   Settings::ReadU( "XISFCompressionLevel", u8 );
   options.compressionLevel = u8;

   u8 = options.checksumAlgorithm;
   Settings::ReadU( "XISFChecksums", u8 );
   options.checksumAlgorithm = XISF::block_checksum( u8 );

   u16 = options.blockAlignmentSize;
   Settings::ReadU( "XISFBlockAlignmentSize", u16 );
   options.blockAlignmentSize = u16;

   u16 = options.maxInlineBlockSize;
   Settings::ReadU( "XISFMaxInlineBlockSize", u16 );
   options.maxInlineBlockSize = u16;

   return options;
}

XISFFormat::EmbeddingOverrides XISFFormat::DefaultEmbeddingOverrides()
{
   EmbeddingOverrides overrides;

   Settings::Read( "XISFOverridePropertyEmbedding",        overrides.overridePropertyEmbedding );
   Settings::Read( "XISFEmbedProperties",                  overrides.embedProperties );
   Settings::Read( "XISFOverrideICCProfileEmbedding",      overrides.overrideICCProfileEmbedding );
   Settings::Read( "XISFEmbedICCProfiles",                 overrides.embedICCProfiles );
   Settings::Read( "XISFOverrideDisplayFunctionEmbedding", overrides.overrideDisplayFunctionEmbedding );
   Settings::Read( "XISFEmbedDisplayFunctions",            overrides.embedDisplayFunctions );
   Settings::Read( "XISFOverrideRGBWorkingSpaceEmbedding", overrides.overrideRGBWorkingSpaceEmbedding );
   Settings::Read( "XISFEmbedRGBWorkingSpaces",            overrides.embedRGBWorkingSpaces );
   Settings::Read( "XISFOverrideThumbnailEmbedding",       overrides.overrideThumbnailEmbedding );
   Settings::Read( "XISFEmbedThumbnails",                  overrides.embedThumbnails );
   Settings::Read( "XISFOverridePreviewRectsEmbedding",    overrides.overridePreviewRectsEmbedding );
   Settings::Read( "XISFEmbedPreviewRects",                overrides.embedPreviewRects );

   return overrides;
}

// ----------------------------------------------------------------------------

#define XISF_SIGNATURE  0x58495346u // 'XISF'

XISFFormat::FormatOptions::FormatOptions() :
   options( XISFFormat::DefaultOptions() ), signature( XISF_SIGNATURE )
{
}

XISFFormat::FormatOptions* XISFFormat::FormatOptions::FromGenericDataBlock( const void* data )
{
   if ( data == nullptr )
      return nullptr;
   const XISFFormat::FormatOptions* o = reinterpret_cast<const XISFFormat::FormatOptions*>( data );
   if ( o->signature != XISF_SIGNATURE )
      return nullptr;
   return const_cast<XISFFormat::FormatOptions*>( o );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF XISFFormat.cpp - Released 2017-05-02T09:42:51Z
