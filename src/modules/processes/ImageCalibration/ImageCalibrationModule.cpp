//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.03.05.0272
// ----------------------------------------------------------------------------
// ImageCalibrationModule.cpp - Released 2016/02/21 20:22:43 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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
#define MODULE_VERSION_MINOR     03
#define MODULE_VERSION_REVISION  05
#define MODULE_VERSION_BUILD     0272
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2016
#define MODULE_RELEASE_MONTH     2
#define MODULE_RELEASE_DAY       21

#include "ImageCalibrationModule.h"
#include "ImageCalibrationProcess.h"
#include "ImageCalibrationInterface.h"
#include "DefectMapProcess.h"
#include "DefectMapInterface.h"
#include "SuperbiasProcess.h"
#include "SuperbiasInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ImageCalibrationModule::ImageCalibrationModule() : MetaModule()
{
}

const char* ImageCalibrationModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

IsoString ImageCalibrationModule::Name() const
{
   return "ImageCalibration";
}

String ImageCalibrationModule::Description() const
{
   return "PixInsight Standard ImageCalibration Process Module";
}

String ImageCalibrationModule::Company() const
{
   return "Pleiades Astrophoto";
}

String ImageCalibrationModule::Author() const
{
   return "ImageCalibration/Superbias: Juan Conejero, PTeam / DefectMap/Superbias: Carlos Milovic F., PTeam";
}

String ImageCalibrationModule::Copyright() const
{
   return "Copyright (c) 2009-2015, Pleiades Astrophoto";
}

String ImageCalibrationModule::TradeMarks() const
{
   return "PixInsight";
}

String ImageCalibrationModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "ImageCalibration-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "ImageCalibration-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "ImageCalibration-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "ImageCalibration-pxm.dll";
#endif
}

void ImageCalibrationModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// PCL_MODULE_EXPORT int InstallPixInsightModule( int )
//
// Module installation routine.
// If defined, the PixInsight application calls this routine just after
// loading the module shared library.
// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT
int InstallPixInsightModule( int mode )
{
   // When the PixInsight application installs this module, we just have to
   // instantiate the meta objects describing it.

   new pcl::ImageCalibrationModule;

   // The mode argument tells us what kind of installation is being requested
   // by the PixInsight application. Incomplete installation requests only need
   // module descriptions.

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::ImageCalibrationProcess;
      new pcl::ImageCalibrationInterface;
      new pcl::DefectMapProcess;
      new pcl::DefectMapInterface;
      new pcl::SuperbiasProcess;
      new pcl::SuperbiasInterface;
   }

   // Return zero to signal successful installation
   return 0;
}

// ----------------------------------------------------------------------------
// EOF ImageCalibrationModule.cpp - Released 2016/02/21 20:22:43 UTC
