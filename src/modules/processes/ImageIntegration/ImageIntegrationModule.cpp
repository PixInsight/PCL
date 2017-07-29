//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.11.00.0344
// ----------------------------------------------------------------------------
// ImageIntegrationModule.cpp - Released 2016/11/13 17:30:54 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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
#define MODULE_VERSION_MINOR     11
#define MODULE_VERSION_REVISION  00
#define MODULE_VERSION_BUILD     0344
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2016
#define MODULE_RELEASE_MONTH     11
#define MODULE_RELEASE_DAY       13

#include "DrizzleIntegrationInstance.h"
#include "DrizzleIntegrationInterface.h"
#include "DrizzleIntegrationProcess.h"
#include "HDRCompositionInstance.h"
#include "HDRCompositionInterface.h"
#include "HDRCompositionProcess.h"
#include "ImageIntegrationInstance.h"
#include "ImageIntegrationInterface.h"
#include "ImageIntegrationModule.h"
#include "ImageIntegrationProcess.h"
#include "IntegrationCache.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ImageIntegrationModule::ImageIntegrationModule() : MetaModule()
{
}

const char* ImageIntegrationModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

IsoString ImageIntegrationModule::Name() const
{
   return "ImageIntegration";
}

String ImageIntegrationModule::Description() const
{
   return "PixInsight Standard ImageIntegration Process Module";
}

String ImageIntegrationModule::Company() const
{
   return "Pleiades Astrophoto";
}

String ImageIntegrationModule::Author() const
{
   return "Juan Conejero, PTeam";
}

String ImageIntegrationModule::Copyright() const
{
   return "Copyright (c) 2009-2015, Pleiades Astrophoto";
}

String ImageIntegrationModule::TradeMarks() const
{
   return "PixInsight";
}

String ImageIntegrationModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "ImageIntegration-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "ImageIntegration-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "ImageIntegration-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "ImageIntegration-pxm.dll";
#endif
}

void ImageIntegrationModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

void ImageIntegrationModule::OnLoad()
{
   // ### File caches are created and loaded the first time an instance
   //     is executed, or the first time a preferences dialog is launched.
   //     Loading them here would add unnecessary workload during startup.

   /*
   if ( TheIntegrationCache == 0 )
      new IntegrationCache;
   */
}

void ImageIntegrationModule::OnUnload()
{
   if ( TheIntegrationCache != 0 )
      TheIntegrationCache->Save();
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

   new pcl::ImageIntegrationModule;

   // The mode argument tells us what kind of installation is being requested
   // by the PixInsight application. Incomplete installation requests only need
   // module descriptions.

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::DrizzleIntegrationProcess;
      new pcl::DrizzleIntegrationInterface;
      new pcl::HDRCompositionProcess;
      new pcl::HDRCompositionInterface;
      new pcl::ImageIntegrationProcess;
      new pcl::ImageIntegrationInterface;
   }

   // Return zero to signal successful installation
   return 0;
}

// ----------------------------------------------------------------------------
// EOF ImageIntegrationModule.cpp - Released 2016/11/13 17:30:54 UTC
