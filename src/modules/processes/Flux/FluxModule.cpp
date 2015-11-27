//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 01.00.00.0123
// ----------------------------------------------------------------------------
// FluxModule.cpp - Released 2015/11/26 16:00:12 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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
#define MODULE_VERSION_BUILD     0123
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2015
#define MODULE_RELEASE_MONTH     11
#define MODULE_RELEASE_DAY       26

#include "FluxModule.h"
#include "B3EProcess.h"
#include "B3EInterface.h"
#include "FluxCalibrationProcess.h"
#include "FluxCalibrationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FluxModule::FluxModule() : MetaModule()
{
}

const char* FluxModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

IsoString FluxModule::Name() const
{
   return "Flux";
}

String FluxModule::Description() const
{
   return "PixInsight Flux Process Module"; // Replace with your own description
}

String FluxModule::Company() const
{
   return "Pleiades Astrophoto";
}

String FluxModule::Author() const
{
   return "PTeam";
}

String FluxModule::Copyright() const
{
   return "Copyright (c) 2012 Pleiades Astrophoto";
}

String FluxModule::TradeMarks() const
{
   return "PixInsight";
}

String FluxModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "Flux-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "Flux-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Flux-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Flux-pxm.dll";
#endif
}

void FluxModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
//
// Module installation routine.
//
// If this routine is defined as a public symbol in a module, the PixInsight
// core application calls it just after loading and initialization of the
// module shared object.
//
// The mode argument specifies the kind of installation being performed by the
// core application. See the pcl::InstallMode namespace for more information.
// ----------------------------------------------------------------------------

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   // When the PixInsight application installs this module, we just have to
   // instantiate the meta objects describing it.

   new pcl::FluxModule;

   // The mode argument tells us what kind of installation is being requested
   // by the PixInsight application. Incomplete installation requests only need
   // module descriptions.

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::B3EProcess;
      new pcl::B3EInterface;
      new pcl::FluxCalibrationProcess;
      new pcl::FluxCalibrationInterface;
   }

   // Return zero to signal successful installation
   return 0;
}

// ----------------------------------------------------------------------------
// EOF FluxModule.cpp - Released 2015/11/26 16:00:12 UTC
