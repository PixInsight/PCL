//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 01.02.07.0386
// ----------------------------------------------------------------------------
// GlobalModule.cpp - Released 2018-11-01T11:07:20Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
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
#define MODULE_VERSION_MINOR     02
#define MODULE_VERSION_REVISION  07
#define MODULE_VERSION_BUILD     0386
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2018
#define MODULE_RELEASE_MONTH     11
#define MODULE_RELEASE_DAY       1

#include "ColorManagementSetupAction.h"
#include "ColorManagementSetupInterface.h"
#include "ColorManagementSetupProcess.h"
#include "GlobalModule.h"
#include "PreferencesAction.h"
#include "PreferencesInterface.h"
#include "PreferencesProcess.h"
#include "ReadoutOptionsAction.h"
#include "ReadoutOptionsInterface.h"
#include "ReadoutOptionsProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

GlobalModule::GlobalModule() : MetaModule()
{
}

const char* GlobalModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

IsoString GlobalModule::Name() const
{
   return "Global";
}

String GlobalModule::Description() const
{
   return "PixInsight Standard Global Process Module";
}

String GlobalModule::Company() const
{
   return "Pleiades Astrophoto";
}

String GlobalModule::Author() const
{
   return "Juan Conejero, PTeam";
}

String GlobalModule::Copyright() const
{
   return "Copyright (c) 2005-2018, Pleiades Astrophoto";
}

String GlobalModule::TradeMarks() const
{
   return "PixInsight";
}

String GlobalModule::OriginalFileName() const
{
#ifdef __PCL_LINUX
   return "Global-pxm.so";
#endif
#ifdef __PCL_FREEBSD
   return "Global-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Global-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Global-pxm.dll";
#endif
}

void GlobalModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

void GlobalModule::OnLoad()
{
   new PreferencesAction;
   new ReadoutOptionsAction;
   new ColorManagementSetupAction;
}

// ----------------------------------------------------------------------------

} // pcl

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::GlobalModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::PreferencesProcess;
      new pcl::PreferencesInterface;
      new pcl::ReadoutOptionsProcess;
      new pcl::ReadoutOptionsInterface;
      new pcl::ColorManagementSetupProcess;
      new pcl::ColorManagementSetupInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF GlobalModule.cpp - Released 2018-11-01T11:07:20Z
