//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 01.01.05.0433
// ----------------------------------------------------------------------------
// FITSFormat.cpp - Released 2019-01-21T12:06:31Z
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
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

#include "FITSFormat.h"
#include "FITSInstance.h"
#include "FITSPreferencesDialog.h"

#include <pcl/Settings.h>
#include <pcl/ErrorHandler.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FITSFormat::FITSFormat() : MetaFileFormat()
{
}

IsoString FITSFormat::Name() const
{
   return "FITS";
}

StringList FITSFormat::FileExtensions() const
{
   return StringList() << ".fit" << ".fits" << ".fts";
}

IsoStringList FITSFormat::MimeTypes() const
{
   // RFC4047
   return IsoStringList() << "image/fits" << "application/fits";
}

uint32 FITSFormat::Version() const
{
   return 0x102;
}

String FITSFormat::Description() const
{
   return

   "<html>"
   "<p>Flexible Image Transport System (FITS).<br/>"
   "The FITS format is defined in <i>Astronomy and Astrophysics</i>, "
   "volume 376, page 359; bibcode: 2001A&A...376..359H</p>"
   "</html>";
}

String FITSFormat::Implementation() const
{
   return

   "<html>"
   "<p>PixInsight Standard File Format Support Modules.</p>"

   "<p>This implementation of the FITS format is based on the CFITSIO library "
   "of NASA/HEASARC (CFITSIO version 3.45 - released May 2018).</p>"

   "<p>The FITSIO software was written by William Pence at the High Energy "
   "Astrophysic Science Archive Research Center (HEASARC) at the NASA "
   "Goddard Space Flight Center.</p>"

   "<p>FITSIO:<br/>"
   "Copyright U.S. Government as represented by the Administrator of the "
   "National Aeronautics and Space Administration.</p>"

   "<p>PixInsight Class Library (PCL):<br/>"
   "Copyright (c) 2003-2018, Pleiades Astrophoto</p>"

   "<p style=\"white-space:pre;\">"
"\n-------------------------------------------------------------------------------"
"\nFITS Format Hints             Description"
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
"\nsigned-is-physical      (r )  Signed integer images store physical pixel data"
"\n                              in the range [0,+2^(n-1)-1], where n is bit"
"\n                              depth."
"\n-------------------------------------------------------------------------------"
"\nsigned-is-logical       (r )  Signed integer images store pixel data with the"
"\n                              full range [-2^(n-1),+2^(n-1)-1], where n is bit"
"\n                              depth."
"\n-------------------------------------------------------------------------------"
"\nbottom-up               (rw)  Follow the lower left origin, bottom to top"
"\n                              convention of image coordinates."
"\n-------------------------------------------------------------------------------"
"\nup-bottom               (rw)  Follow the upper left origin, top to bottom"
"\n                              convention of image coordinates."
"\n-------------------------------------------------------------------------------"
"\nproperties              (rw)  Read/write image properties stored as BLOBs in"
"\n                              FITS image extensions."
"\n-------------------------------------------------------------------------------"
"\nno-properties           (rw)  Do not read/write BLOB image properties."
"\n-------------------------------------------------------------------------------"
"\nicc-profile             ( w)  Write ICC profiles stored as special FITS image"
"\n                              extensions."
"\n-------------------------------------------------------------------------------"
"\nno-icc-profile          ( w)  Do not write ICC profile image extensions."
"\n-------------------------------------------------------------------------------"
"\nthumbnail               ( w)  Write thumbnail image extensions."
"\n-------------------------------------------------------------------------------"
"\nno-thumbnail            ( w)  Do not write thumbnail image extensions."
"\n-------------------------------------------------------------------------------"
"\nverbosity n             (rw)  n is a verbosity level in the range [0,3] to"
"\n                              control the amount of generated messages"
"\n                              (default = 1)."
"\n-------------------------------------------------------------------------------"
"\nunsigned                ( w)  Write unsigned integer pixel data in the range"
"\n                              [0,2^n-1], where n is bit depth."
"\n-------------------------------------------------------------------------------"
"\nsigned                  ( w)  Write signed integer pixel data in the range"
"\n                              [-2^(n-1),+2^(n-1)-1], where n is bit depth."
"\n-------------------------------------------------------------------------------"
"\ncleanup-headers         ( w)  Automatically fix characters outside the"
"\n                              printable range of ASCII (from 0x20 to 0x7E) in"
"\n                              generated FITS header keywords."
"\n-------------------------------------------------------------------------------"
"\nno-cleanup-headers      ( w)  Do not fix non-printable ASCII characters in FITS"
"\n                              header keywords."
"\n-------------------------------------------------------------------------------"
"\n"
   "</p>"
   "</html>";
}

String FITSFormat::Status() const
{
   return

   "<p>The FITS format has been deprecated in PixInsight. It is maintained for "
   "compatibility with existing data and legacy applications, but no active "
   "development will be done to implement new features in FITS.</p>"

   "<p><b>Consider using the XISF format.</b> XISF is the open-source, native "
   "file format of the PixInsight platform. It is fully backwards-compatible "
   "with FITS metadata and provides many advanced features that cannot be "
   "implemented in FITS. XISF is subject to intense development as an initiative "
   "to create a free, modern, efficient and interoperable format for storage, "
   "management and interchange of digital images and associated data. For "
   "up-to-date information on XISF, please visit:</p>"

   "<p>http://pixinsight.com/xisf/</p>";
}

String FITSFormat::IconImageFile() const
{
   return ":/file-format/fits-format-icon.png";
}

bool FITSFormat::CanReadIncrementally() const
{
   return true;
}

bool FITSFormat::CanWriteIncrementally() const
{
   return true;
}

bool FITSFormat::CanStore32Bit() const
{
   return true;
}

bool FITSFormat::CanStore64Bit() const
{
   return false;
}

bool FITSFormat::CanStoreFloat() const
{
   return true;
}

bool FITSFormat::CanStoreDouble() const
{
   return true;
}

bool FITSFormat::CanStoreResolution() const
{
   return true;
}

bool FITSFormat::CanStoreKeywords() const
{
   return true;
}

bool FITSFormat::CanStoreICCProfiles() const
{
   return true;
}

bool FITSFormat::CanStoreThumbnails() const
{
   return true;
}

bool FITSFormat::CanStoreImageProperties() const
{
   return true; // BLOBs only
}

bool FITSFormat::SupportsMultipleImages() const
{
   return true;
}

bool FITSFormat::CanEditPreferences() const
{
   return true;
}

bool FITSFormat::UsesFormatSpecificData() const
{
   return true; // use format-specific data to manage some special FITS options
}

bool FITSFormat::IsDeprecated() const
{
   return true; // formally deprecated since core build 1187
}

bool FITSFormat::ValidateFormatSpecificData( const void* data ) const
{
   return FormatOptions::FromGenericDataBlock( data ) != nullptr;
}

void FITSFormat::DisposeFormatSpecificData( void* data ) const
{
   if ( FormatOptions* o = FormatOptions::FromGenericDataBlock( data ) )
      delete o;
}

FileFormatImplementation* FITSFormat::Create() const
{
   return new FITSInstance( this );
}

bool FITSFormat::EditPreferences() const
{
   OutOfRangePolicyOptions outOfRange = DefaultOutOfRangePolicyOptions();
   EmbeddingOverrides overrides = DefaultEmbeddingOverrides();
   FITSImageOptions options = DefaultOptions();

   FITSPreferencesDialog dlg( outOfRange, overrides, options );

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

      overrides = dlg.overrides;
      options = dlg.fitsOptions;

      Settings::Write ( "FITSLowerRange",                        outOfRange.lowerRange );
      Settings::Write ( "FITSUpperRange",                        outOfRange.upperRange );
      Settings::WriteI( "FITSOutOfRangePolicy",                  outOfRange.outOfRangePolicy );
      Settings::WriteI( "FITSOutOfRangeFixMode",                 outOfRange.outOfRangeFixMode );

      Settings::Write ( "FITSOverrideICCProfileEmbedding",       overrides.overrideICCProfileEmbedding );
      Settings::Write ( "FITSEmbedICCProfiles",                  overrides.embedICCProfiles );
      Settings::Write ( "FITSOverrideThumbnailEmbedding",        overrides.overrideThumbnailEmbedding );
      Settings::Write ( "FITSEmbedThumbnails",                   overrides.embedThumbnails );
      Settings::Write ( "FITSOverridePropertyEmbedding",         overrides.overridePropertyEmbedding );
      Settings::Write ( "FITSEmbedProperties",                   overrides.embedProperties );

      Settings::Write ( "FITSBottomUp",                          options.bottomUp );
      Settings::Write ( "FITSUnsignedIntegers",                  options.unsignedIntegers );
      Settings::Write ( "FITSWriteFixedFloatKeywords",           options.writeFixedFloatKeywords );
      Settings::Write ( "FITSWriteScalingKeywordsForSignedData", options.writeScalingKeywordsForSignedData );
      Settings::Write ( "FITSSignedIntegersArePhysical",         options.signedIntegersArePhysical );
      Settings::Write ( "FITSCleanupHeaders",                    options.cleanupHeaders );

      return true;
   }

   return false;
}

FITSImageOptions FITSFormat::DefaultOptions()
{
   FITSImageOptions options;
   bool b;

   b = options.bottomUp;
   Settings::Read( "FITSBottomUp", b );
   options.bottomUp = b;

   b = options.unsignedIntegers;
   Settings::Read( "FITSUnsignedIntegers", b );
   options.unsignedIntegers = b;

   b = options.writeFixedFloatKeywords;
   Settings::Read( "FITSWriteFixedFloatKeywords", b );
   options.writeFixedFloatKeywords = b;

   b = options.writeScalingKeywordsForSignedData;
   Settings::Read( "FITSWriteScalingKeywordsForSignedData", b );
   options.writeScalingKeywordsForSignedData = b;

   b = options.signedIntegersArePhysical;
   Settings::Read( "FITSSignedIntegersArePhysical", b );
   options.signedIntegersArePhysical = b;

   b = options.cleanupHeaders;
   Settings::Read( "FITSCleanupHeaders", b );
   options.cleanupHeaders = b;

   return options;
}

FITSFormat::OutOfRangePolicyOptions FITSFormat::DefaultOutOfRangePolicyOptions()
{
   OutOfRangePolicyOptions options;

   Settings::Read ( "FITSLowerRange",        options.lowerRange );
   Settings::Read ( "FITSUpperRange",        options.upperRange );
   Settings::ReadI( "FITSOutOfRangePolicy",  options.outOfRangePolicy );
   Settings::ReadI( "FITSOutOfRangeFixMode", options.outOfRangeFixMode );

   return options;
}

FITSFormat::EmbeddingOverrides FITSFormat::DefaultEmbeddingOverrides()
{
   EmbeddingOverrides overrides;

   Settings::Read( "FITSOverrideICCProfileEmbedding", overrides.overrideICCProfileEmbedding );
   Settings::Read( "FITSEmbedICCProfiles",            overrides.embedICCProfiles );
   Settings::Read( "FITSOverrideThumbnailEmbedding",  overrides.overrideThumbnailEmbedding );
   Settings::Read( "FITSEmbedThumbnails",             overrides.embedThumbnails );
   Settings::Read( "FITSOverridePropertyEmbedding",   overrides.overridePropertyEmbedding );
   Settings::Read( "FITSEmbedProperties",             overrides.embedProperties );

   return overrides;
}

// ----------------------------------------------------------------------------

#define FITS_SIGNATURE  0x46495453u // 'FITS'

FITSFormat::FormatOptions::FormatOptions() :
   signature( FITS_SIGNATURE ),
   options( FITSFormat::DefaultOptions() )
{
}

FITSFormat::FormatOptions::FormatOptions( const FITSFormat::FormatOptions& x ) :
   signature( FITS_SIGNATURE ),
   options( x.options )
{
}

FITSFormat::FormatOptions* FITSFormat::FormatOptions::FromGenericDataBlock( const void* data )
{
   if ( data == nullptr )
      return nullptr;
   const FITSFormat::FormatOptions* o = reinterpret_cast<const FITSFormat::FormatOptions*>( data );
   if ( o->signature != FITS_SIGNATURE )
      return nullptr;
   return const_cast<FITSFormat::FormatOptions*>( o );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FITSFormat.cpp - Released 2019-01-21T12:06:31Z
