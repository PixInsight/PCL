//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard TIFF File Format Module Version 01.00.07.0369
// ----------------------------------------------------------------------------
// TIFFFormat.cpp - Released 2019-01-21T12:06:31Z
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

#include "TIFFFormat.h"
#include "TIFFInstance.h"
#include "TIFFPreferencesDialog.h"

#include <pcl/Settings.h>
#include <pcl/ErrorHandler.h>

namespace pcl
{

// ----------------------------------------------------------------------------

TIFFFormat::TIFFFormat() : MetaFileFormat()
{
}

IsoString TIFFFormat::Name() const
{
   return "TIFF";
}

StringList TIFFFormat::FileExtensions() const
{
   return StringList() << ".tif" << ".tiff";
}

IsoStringList TIFFFormat::MimeTypes() const
{
   // RFC3302
   return IsoStringList() << "image/tiff" << "image/tif" << "application/tiff" << "application/tif";
}

uint32 TIFFFormat::Version() const
{
   return 0x103;
}

String TIFFFormat::Description() const
{
   return "Tag Image File Format (TIFF), 6.0 Specification.";
}

String TIFFFormat::Implementation() const
{
   return

   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>This implementation of the TIFF format is based on LibTIFF Software, "
   "written by Sam Leffler at Silicon Graphics Inc.</p>"

   "<p>LibTIFF Software (Version 4.0.7, released 2016 November 19):<br/>"
   "Copyright (c) 1988-1997 Sam Leffler<br/>"
   "Copyright (c) 1991-1997 Silicon Graphics, Inc.</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2018, Pleiades Astrophoto</p>"

   "<p style=\"white-space:pre;\">"
"\n-------------------------------------------------------------------------------"
"\nTIFF Format Hints             Description"
"\n============================  ================================================="
"\nlower-range n           (r )  n is the lower bound of the input range for"
"\n                              floating point pixel data (default = 0)."
"\n-------------------------------------------------------------------------------"
"\nupper-range n           (r )  n is the upper bound of the input range for"
"\n                              floating point pixel data (default = 1)."
"\n-------------------------------------------------------------------------------"
"\nrescale-out-of-range    (r )  Normalize out-of-range floating point pixel data."
"\n-------------------------------------------------------------------------------"
"\ntruncate-out-of-range   (r )  Truncate out-of-range floating point pixel data."
"\n                              Warning &emdash; out-of-range pixel values will"
"\n                              be set to fixed values, so data will be lost."
"\n-------------------------------------------------------------------------------"
"\nignore-out-of-range     (r )  Ignore out-of-range floating point pixel data."
"\n                              Warning &emdash; out-of-range pixel values will"
"\n                              not be fixed. Use at your own risk."
"\n-------------------------------------------------------------------------------"
"\nverbosity n             (rw)  n is a verbosity level in the range [0,3] to"
"\n                              control the amount of generated messages"
"\n                              (default = 1)."
"\n-------------------------------------------------------------------------------"
"\nzip-compression         ( w)  Use ZIP compression."
"\n-------------------------------------------------------------------------------"
"\nlzw-compression         ( w)  use LZW compression."
"\n-------------------------------------------------------------------------------"
"\nno-compression          ( w)  Do not compress images."
"\n-------------------------------------------------------------------------------"
"\nplanar                  ( w)  Write planar TIFF images (separate RGBA color"
"\n                              planes)."
"\n-------------------------------------------------------------------------------"
"\nchunky                  ( w)  Write chunky TIFF images (mixed RGBA color"
"\n                              components)."
"\n-------------------------------------------------------------------------------"
"\nno-planar               ( w)  Same as chunky."
"\n-------------------------------------------------------------------------------"
"\nassociated-alpha        ( w)  Associate alpha channels with images. The alpha"
"\n                              channel should be interpreted as image opacity."
"\n-------------------------------------------------------------------------------"
"\nno-associated-alpha     ( w)  Do not associate alpha channels with images."
"\n-------------------------------------------------------------------------------"
"\npremultiplied-alpha     ( w)  Write RGB/K components premultiplied by the"
"\n                              active alpha channel."
"\n-------------------------------------------------------------------------------"
"\nno-premultiplied-alpha  ( w)  Do not premultiply RGB/K components by alpha."
"\n-------------------------------------------------------------------------------"
"\n"
   "</p>"
   "</html>";
}

String TIFFFormat::IconImageFile() const
{
   return ":/file-format/tiff-format-icon.png";
}

bool TIFFFormat::CanStore32Bit() const
{
   return true;
}

bool TIFFFormat::CanStoreFloat() const
{
   return true;
}

bool TIFFFormat::CanStoreDouble() const
{
   return true;
}

bool TIFFFormat::CanStoreResolution() const
{
   return true;
}

bool TIFFFormat::CanStoreICCProfiles() const
{
   return true;
}

bool TIFFFormat::SupportsCompression() const
{
   return true;
}

bool TIFFFormat::CanEditPreferences() const
{
   return true;
}

bool TIFFFormat::UsesFormatSpecificData() const
{
   return true; // use format-specific data to manage some special TIFF flags and data
}

bool TIFFFormat::ValidateFormatSpecificData( const void* data ) const
{
   return FormatOptions::FromGenericDataBlock( data ) != nullptr;
}

void TIFFFormat::DisposeFormatSpecificData( void* data ) const
{
   if ( FormatOptions* o = FormatOptions::FromGenericDataBlock( data ) )
      delete o;
}

FileFormatImplementation* TIFFFormat::Create() const
{
   return new TIFFInstance( this );
}

bool TIFFFormat::EditPreferences() const
{
   OutOfRangePolicyOptions outOfRange = DefaultOutOfRangePolicyOptions();
   TIFFImageOptions options = DefaultOptions();
   EmbeddingOverrides overrides = DefaultEmbeddingOverrides();

   TIFFPreferencesDialog dlg( outOfRange, overrides, options );

   while ( dlg.Execute() == StdDialogCode::Ok )
   {
      outOfRange = dlg.outOfRange;

      if ( outOfRange.upperRange < outOfRange.lowerRange )
         Swap( outOfRange.lowerRange, outOfRange.upperRange );

      double delta = outOfRange.upperRange - outOfRange.lowerRange;
      if ( 1 + delta == 1 )
      {
         try
         {
            throw Error( "Empty input range!" );
         }
         ERROR_HANDLER
         continue;
      }

      options = dlg.tiffOptions;
      overrides = dlg.overrides;

      Settings::Write ( "TIFFLowerRange",                  outOfRange.lowerRange );
      Settings::Write ( "TIFFUpperRange",                  outOfRange.upperRange );
      Settings::WriteI( "TIFFOutOfRangePolicy",            outOfRange.outOfRangePolicy );
      Settings::WriteI( "TIFFOutOfRangeFixMode",           outOfRange.outOfRangeFixMode );

      Settings::WriteU( "TIFFCompression",                 options.compression );
      Settings::Write ( "TIFFPlanar",                      options.planar );
      Settings::Write ( "TIFFAssociatedAlpha",             options.associatedAlpha );
      Settings::Write ( "TIFFPremultipliedAlpha",          options.premultipliedAlpha );
      Settings::Write ( "TIFFSoftware",                    options.software );
      Settings::Write ( "TIFFImageDescription",            options.imageDescription );
      Settings::Write ( "TIFFCopyright",                   options.copyright );

      Settings::Write ( "TIFFOverrideICCProfileEmbedding", overrides.overrideICCProfileEmbedding );
      Settings::Write ( "TIFFEmbedICCProfiles",            overrides.embedICCProfiles );

      return true;
   }

   return false;
}

TIFFImageOptions TIFFFormat::DefaultOptions()
{
   TIFFImageOptions options;
   unsigned u; bool b;

   u = options.compression;
   Settings::Read( "TIFFCompression", u );
   options.compression = TIFFImageOptions::compression_codec( u );

   b = options.planar;
   Settings::Read( "TIFFPlanar", b );
   options.planar = b;

   b = options.associatedAlpha;
   Settings::Read( "TIFFAssociatedAlpha", b );
   options.associatedAlpha = b;

   b = options.premultipliedAlpha;
   Settings::Read( "TIFFPremultipliedAlpha", b );
   options.premultipliedAlpha = b;

   Settings::Read( "TIFFSoftware", options.software );
   Settings::Read( "TIFFImageDescription", options.imageDescription );
   Settings::Read( "TIFFCopyright", options.copyright );

   return options;
}

TIFFFormat::OutOfRangePolicyOptions TIFFFormat::DefaultOutOfRangePolicyOptions()
{
   OutOfRangePolicyOptions options;

   Settings::Read ( "TIFFLowerRange",        options.lowerRange );
   Settings::Read ( "TIFFUpperRange",        options.upperRange );
   Settings::ReadI( "TIFFOutOfRangePolicy",  options.outOfRangePolicy );
   Settings::ReadI( "TIFFOutOfRangeFixMode", options.outOfRangeFixMode );

   return options;
}

TIFFFormat::EmbeddingOverrides TIFFFormat::DefaultEmbeddingOverrides()
{
   EmbeddingOverrides overrides;

   Settings::Read( "TIFFOverrideICCProfileEmbedding", overrides.overrideICCProfileEmbedding );
   Settings::Read( "TIFFEmbedICCProfiles",            overrides.embedICCProfiles );

   return overrides;
}

// ----------------------------------------------------------------------------

#define TIFF_SIGNATURE  0x54494646u

TIFFFormat::FormatOptions::FormatOptions() :
   signature( TIFF_SIGNATURE ), options( TIFFFormat::DefaultOptions() )
{
}

TIFFFormat::FormatOptions::FormatOptions( const TIFFFormat::FormatOptions& x ) :
   signature( TIFF_SIGNATURE ), options( x.options )
{
}

TIFFFormat::FormatOptions* TIFFFormat::FormatOptions::FromGenericDataBlock( const void* data )
{
   if ( data == nullptr )
      return nullptr;
   const TIFFFormat::FormatOptions* o = reinterpret_cast<const TIFFFormat::FormatOptions*>( data );
   if ( o->signature != TIFF_SIGNATURE )
      return nullptr;
   return const_cast<TIFFFormat::FormatOptions*>( o );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF TIFFFormat.cpp - Released 2019-01-21T12:06:31Z
