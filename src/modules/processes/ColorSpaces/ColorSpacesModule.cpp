//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0368
// ----------------------------------------------------------------------------
// ColorSpacesModule.cpp - Released 2018-12-12T09:25:24Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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
#define MODULE_VERSION_MINOR     01
#define MODULE_VERSION_REVISION  00
#define MODULE_VERSION_BUILD     0368
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2018
#define MODULE_RELEASE_MONTH     12
#define MODULE_RELEASE_DAY       12

#include "ColorSpacesModule.h"
#include "GrayscaleRGBProcesses.h"
#include "GrayscaleRGBActions.h"
#include "ChannelExtractionProcess.h"
#include "ChannelExtractionInterface.h"
#include "ChannelCombinationProcess.h"
#include "ChannelCombinationInterface.h"
#include "RGBWorkingSpaceProcess.h"
#include "RGBWorkingSpaceInterface.h"
#include "LRGBCombinationProcess.h"
#include "LRGBCombinationInterface.h"
#include "ExtractCIELAction.h"
#include "SplitRGBChannelsAction.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ColorSpacesModule::ColorSpacesModule() : MetaModule()
{
}

const char* ColorSpacesModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

IsoString ColorSpacesModule::Name() const
{
   return "ColorSpaces";
}

String ColorSpacesModule::Description() const
{
   return "PixInsight Standard ColorSpaces Module";
}

String ColorSpacesModule::Company() const
{
   return "Pleiades Astrophoto";
}

String ColorSpacesModule::Author() const
{
   return "Juan Conejero, PTeam";
}

String ColorSpacesModule::Copyright() const
{
   return "Copyright (c) 2005-2018, Pleiades Astrophoto";
}

String ColorSpacesModule::TradeMarks() const
{
   return "PixInsight";
}

String ColorSpacesModule::OriginalFileName() const
{
#ifdef __PCL_LINUX
   return "ColorSpaces-pxm.so";
#endif
#ifdef __PCL_FREEBSD
   return "ColorSpaces-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "ColorSpaces-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "ColorSpaces-pxm.dll";
#endif
}

void ColorSpacesModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

void ColorSpacesModule::OnLoad()
{
   new ConvertToGrayscaleAction;
   new ConvertToRGBColorAction;
   new ExtractCIELAction;
   new SplitRGBChannelsAction;
   new ExtractRedChannelAction;
   new ExtractGreenChannelAction;
   new ExtractBlueChannelAction;
}

// ----------------------------------------------------------------------------

} // pcl

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::ColorSpacesModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::ConvertToGrayscaleProcess;

      new pcl::ConvertToRGBColorProcess;

      new pcl::ChannelExtractionProcess;
      new pcl::ChannelExtractionInterface;

      new pcl::ChannelCombinationProcess;
      new pcl::ChannelCombinationInterface;

      new pcl::RGBWorkingSpaceProcess;
      new pcl::RGBWorkingSpaceInterface;

      new pcl::LRGBCombinationProcess;
      new pcl::LRGBCombinationInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF ColorSpacesModule.cpp - Released 2018-12-12T09:25:24Z
