//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.01.0377
// ----------------------------------------------------------------------------
// GeometryModule.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#define MODULE_VERSION_MAJOR     01
#define MODULE_VERSION_MINOR     02
#define MODULE_VERSION_REVISION  01
#define MODULE_VERSION_BUILD     0377
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2017
#define MODULE_RELEASE_MONTH     8
#define MODULE_RELEASE_DAY       1

#include <pcl/Console.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/View.h>

#include "ChannelMatchInterface.h"
#include "ChannelMatchProcess.h"
#include "CropInterface.h"
#include "CropProcess.h"
#include "DynamicCropInterface.h"
#include "DynamicCropProcess.h"
#include "FastRotationActions.h"
#include "FastRotationInterface.h"
#include "FastRotationProcess.h"
#include "GeometryModule.h"
#include "IntegerResampleInterface.h"
#include "IntegerResampleProcess.h"
#include "ResampleInterface.h"
#include "ResampleProcess.h"
#include "RotationInterface.h"
#include "RotationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

GeometryModule::GeometryModule() : MetaModule()
{
}

// ----------------------------------------------------------------------------

const char* GeometryModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString GeometryModule::Name() const
{
   return "Geometry";
}

// ----------------------------------------------------------------------------

String GeometryModule::Description() const
{
   return "PixInsight Standard Geometry Process Module";
}

// ----------------------------------------------------------------------------

String GeometryModule::Company() const
{
   return "Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String GeometryModule::Author() const
{
   return "Juan Conejero, PTeam";
}

// ----------------------------------------------------------------------------

String GeometryModule::Copyright() const
{
   return "Copyright (c) 2005-2017, Pleiades Astrophoto";
}

// ----------------------------------------------------------------------------

String GeometryModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String GeometryModule::OriginalFileName() const
{
#ifdef __PCL_LINUX
   return "Geometry-pxm.so";
#endif
#ifdef __PCL_FREEBSD
   return "Geometry-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Geometry-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Geometry-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void GeometryModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

void GeometryModule::OnLoad()
{
   new Rotate180Action;
   new Rotate90CWAction;
   new Rotate90CCWAction;
   new HorizontalMirrorAction;
   new VerticalMirrorAction;
}

// ----------------------------------------------------------------------------

static SortedIsoStringList s_astrometryKeywords;

bool WarnOnAstrometryMetadataOrPreviewsOrMask( const ImageWindow& window, const IsoString& processId, bool noGUIMessages )
{
   if ( window.HasPreviews() || window.HasMaskReferences() || !window.Mask().IsNull() )
   {
      if ( !noGUIMessages )
         if ( MessageBox( "<p>" + window.MainView().Id() + "</p>"
                          "<p>Existing previews and mask references will be deleted.</p>"
                          "<p><b>Some of these side effects could be irreversible. Proceed?</b></p>",
                          processId,
                          StdIcon::Warning,
                          StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes )
         {
            return false;
         }

      Console().WarningLn( "<end><cbr><br>** Warning: " + processId + ": Existing previews and/or mask references will be deleted." );
   }

   if ( s_astrometryKeywords.IsEmpty() )
      s_astrometryKeywords << "CTYPE1"
                           << "CTYPE2"
                           << "CRPIX1"
                           << "CRPIX2"
                           << "CRVAL1"
                           << "CRVAL2"
                           << "CD1_1"
                           << "CD1_2"
                           << "CD2_1"
                           << "CD2_2"
                           << "CDELT1"
                           << "CDELT2"
                           << "CROTA1"
                           << "CROTA2"
                           << "REFSPLINE"; // a custom keyword set by the ImageSolver script

   FITSKeywordArray keywords;
   window.GetKeywords( keywords );
   for ( auto k : keywords )
      if ( s_astrometryKeywords.Contains( k.name ) )
      {
         if ( !noGUIMessages )
            if ( MessageBox( "<p>" + window.MainView().Id() + "</p>"
                             "<p>The image contains an astrometric solution that will be deleted by the geometric transformation.</p>"
                             "<p><b>This side effect could be irreversible. Proceed?</b></p>",
                             processId,
                             StdIcon::Warning,
                             StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes )
            {
               return false;
            }

         Console().WarningLn( "<end><cbr><br>** Warning: " + processId + ": Existing astrometric solution will be deleted." );
         break;
      }

   return true;
}

// ----------------------------------------------------------------------------

void DeleteAstrometryMetadataAndPreviewsAndMask( ImageWindow& window )
{
   window.RemoveMaskReferences();
   window.RemoveMask();
   DeleteAstrometryMetadataAndPreviews( window );
}

// ----------------------------------------------------------------------------

void DeleteAstrometryMetadataAndPreviews( ImageWindow& window )
{
   window.DeletePreviews();

   FITSKeywordArray keywords, newKeywords;
   window.GetKeywords( keywords );
   for ( auto k : keywords )
      if ( !s_astrometryKeywords.Contains( k.name ) )
         newKeywords << k;
   if ( newKeywords.Length() < keywords.Length() )
      window.SetKeywords( newKeywords );

   // Delete a custom property generated by the ImageSolver script.
   if ( window.MainView().HasProperty( "Transformation_ImageToProjection" ) )
      window.MainView().DeleteProperty( "Transformation_ImageToProjection" );
}

// ----------------------------------------------------------------------------

} // pcl

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::GeometryModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::DynamicCropProcess;
      new pcl::DynamicCropInterface;
      new pcl::FastRotationProcess;
      new pcl::FastRotationInterface;
      new pcl::ChannelMatchProcess;
      new pcl::ChannelMatchInterface;
      new pcl::ResampleProcess;
      new pcl::ResampleInterface;
      new pcl::IntegerResampleProcess;
      new pcl::IntegerResampleInterface;
      new pcl::RotationProcess;
      new pcl::RotationInterface;
      new pcl::CropProcess;
      new pcl::CropInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF GeometryModule.cpp - Released 2017-08-01T14:26:58Z
