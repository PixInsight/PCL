//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0430
// ----------------------------------------------------------------------------
// IntensityTransformationsModule.cpp - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#define MODULE_VERSION_MAJOR     01
#define MODULE_VERSION_MINOR     07
#define MODULE_VERSION_REVISION  01
#define MODULE_VERSION_BUILD     0430
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2019
#define MODULE_RELEASE_MONTH     1
#define MODULE_RELEASE_DAY       21

#include "AdaptiveStretchCurveGraphInterface.h"
#include "AdaptiveStretchInterface.h"
#include "AdaptiveStretchProcess.h"
#include "AutoHistogramInterface.h"
#include "AutoHistogramProcess.h"
#include "BinarizeAction.h"
#include "BinarizeInterface.h"
#include "BinarizeProcess.h"
#include "ColorSaturationInterface.h"
#include "ColorSaturationProcess.h"
#include "CurvesTransformationInterface.h"
#include "CurvesTransformationProcess.h"
#include "ExponentialTransformationInterface.h"
#include "ExponentialTransformationProcess.h"
#include "HistogramTransformationInterface.h"
#include "HistogramTransformationProcess.h"
#include "IntensityTransformationsModule.h"
#include "InvertAction.h"
#include "InvertProcess.h"
#include "MaskedStretchInterface.h"
#include "MaskedStretchProcess.h"
#include "RescaleActions.h"
#include "RescaleInterface.h"
#include "RescaleProcess.h"
#include "STFAutoStretchAction.h"
#include "ScreenTransferFunctionInterface.h"
#include "ScreenTransferFunctionProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

IntensityTransformationsModule::IntensityTransformationsModule() : MetaModule()
{
}

const char* IntensityTransformationsModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

IsoString IntensityTransformationsModule::Name() const
{
   return "IntensityTransformations";
}

String IntensityTransformationsModule::Description() const
{
   return "PixInsight Standard IntensityTransformations Process Module";
}

String IntensityTransformationsModule::Company() const
{
   return "Pleiades Astrophoto";
}

String IntensityTransformationsModule::Author() const
{
   return "Juan Conejero, PTeam / Carlos Milovic F., PTeam";
}

String IntensityTransformationsModule::Copyright() const
{
   return "Copyright (c) 2005-2018, Pleiades Astrophoto";
}

String IntensityTransformationsModule::TradeMarks() const
{
   return "PixInsight";
}

String IntensityTransformationsModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "IntensityTransformations-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "IntensityTransformations-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "IntensityTransformations-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "IntensityTransformations-pxm.dll";
#endif
}

void IntensityTransformationsModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

void IntensityTransformationsModule::OnLoad()
{
   new InvertAction;
   new BinarizeAction;
   new RescaleRGBAction;
   new RescaleRGBIndividualAction;
   new RescaleCIELAction;
   new RescaleCIEYAction;
   new STFAutoStretchAction;
   new STFAutoStretchBoostAction;
   new STFAutoStretchToolBarAction;
   new STFAutoStretchToolBarBoostAction;
   //new AutoHistogramAction;
}

// ----------------------------------------------------------------------------

} // pcl

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::IntensityTransformationsModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::AdaptiveStretchProcess;
      new pcl::AdaptiveStretchInterface;
      new pcl::AdaptiveStretchCurveGraphInterface;
      new pcl::AutoHistogramProcess;
      new pcl::AutoHistogramInterface;
      new pcl::BinarizeProcess;
      new pcl::BinarizeInterface;
      new pcl::ColorSaturationProcess;
      new pcl::ColorSaturationInterface;
      new pcl::CurvesTransformationProcess;
      new pcl::CurvesTransformationInterface;
      new pcl::ExponentialTransformationProcess;
      new pcl::ExponentialTransformationInterface;
      new pcl::HistogramTransformationProcess;
      new pcl::HistogramTransformationInterface;
      new pcl::InvertProcess;
      new pcl::MaskedStretchProcess;
      new pcl::MaskedStretchInterface;
      new pcl::RescaleProcess;
      new pcl::RescaleInterface;
      new pcl::ScreenTransferFunctionProcess;
      new pcl::ScreenTransferFunctionInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF IntensityTransformationsModule.cpp - Released 2019-01-21T12:06:41Z
