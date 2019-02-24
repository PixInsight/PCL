//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 01.00.00.0359
// ----------------------------------------------------------------------------
// ColorManagementModule.cpp - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
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
#define MODULE_VERSION_MINOR     00
#define MODULE_VERSION_REVISION  00
#define MODULE_VERSION_BUILD     0359
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2019
#define MODULE_RELEASE_MONTH     1
#define MODULE_RELEASE_DAY       21

#include "AssignICCProfileInterface.h"
#include "AssignICCProfileProcess.h"
#include "ColorManagementModule.h"
#include "ICCProfileTransformationInterface.h"
#include "ICCProfileTransformationProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ColorManagementModule::ColorManagementModule() : MetaModule()
{
}

const char* ColorManagementModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

IsoString ColorManagementModule::Name() const
{
   return "ColorManagement";
}

String ColorManagementModule::Description() const
{
   return "PixInsight Standard ColorManagement Process Module";
}

String ColorManagementModule::Company() const
{
   return "Pleiades Astrophoto";
}

String ColorManagementModule::Author() const
{
   return "Juan Conejero, PTeam";
}

String ColorManagementModule::Copyright() const
{
   return "Copyright (c) 2005-2018, Pleiades Astrophoto";
}

String ColorManagementModule::TradeMarks() const
{
   return "PixInsight";
}

String ColorManagementModule::OriginalFileName() const
{
#ifdef __PCL_LINUX
   return "ColorManagement-pxm.so";
#endif
#ifdef __PCL_FREEBSD
   return "ColorManagement-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "ColorManagement-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "ColorManagement-pxm.dll";
#endif
}

void ColorManagementModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // pcl

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::ColorManagementModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::AssignICCProfileProcess;
      new pcl::AssignICCProfileInterface;
      new pcl::ICCProfileTransformationProcess;
      new pcl::ICCProfileTransformationInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF ColorManagementModule.cpp - Released 2019-01-21T12:06:41Z
