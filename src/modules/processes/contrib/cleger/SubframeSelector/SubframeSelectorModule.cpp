//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.04.01.0005
// ----------------------------------------------------------------------------
// SubframeSelectorModule.cpp - Released 2018-11-01T11:07:21Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2018 Cameron Leger
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

#include "SubframeSelectorInterface.h"
#include "SubframeSelectorModule.h"
#include "SubframeSelectorProcess.h"

#define MODULE_VERSION_MAJOR     01
#define MODULE_VERSION_MINOR     04
#define MODULE_VERSION_REVISION  01
#define MODULE_VERSION_BUILD     0005
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2018
#define MODULE_RELEASE_MONTH     11
#define MODULE_RELEASE_DAY       1

namespace pcl
{

// ----------------------------------------------------------------------------

const char* SubframeSelectorModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString SubframeSelectorModule::Name() const
{
   return "SubframeSelector";
}

// ----------------------------------------------------------------------------

String SubframeSelectorModule::Description() const
{
   return
   "Facilitates subframe evaluation, selection and weighting based on several subframe \
   quality related measurements, including estimates of star profile <i>full width at \
   half maximum</i> (FWHM), star profile <i>eccentricity</i> and subframe \
   <i>signal to noise ratio weight</i>. Approved/rejected subframes may be copied/moved \
   to output directories for postprocessing. Subframe weights may be recorded in the \
   FITS header of the copies.";
}

// ----------------------------------------------------------------------------

String SubframeSelectorModule::Company() const
{
   return IsoString(); // N/A
}

// ----------------------------------------------------------------------------

String SubframeSelectorModule::Author() const
{
   return "Cameron Leger";
}

// ----------------------------------------------------------------------------

String SubframeSelectorModule::Copyright() const
{
   return "Copyright (c) 2017-2018, Cameron Leger";
}

// ----------------------------------------------------------------------------

String SubframeSelectorModule::TradeMarks() const
{
   return IsoString(); // N/A
}

// ----------------------------------------------------------------------------

String SubframeSelectorModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "SubframeSelector-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "SubframeSelector-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "SubframeSelector-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "SubframeSelector-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void SubframeSelectorModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

void SubframeSelectorModule::OnUnload()
{
   if ( TheSubframeSelectorCache != nullptr )
      TheSubframeSelectorCache->Save();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

/*
 * PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
 *
 * Module installation routine.
 *
 * If this routine is defined as a public symbol in a module, the PixInsight
 * core application will call it just after loading and initializing the module
 * shared object or dynamic-link library.
 *
 * The mode argument specifies the kind of installation being performed by the
 * core application. See the pcl::InstallMode namespace for more information.
 */
PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   /*
    * When the PixInsight application installs this module, we just have to
    * instantiate the meta objects describing it.
    */
   new pcl::SubframeSelectorModule;

   /*
    * The mode argument tells us which kind of installation is being requested
    * by the PixInsight application. Incomplete installation requests only need
    * module descriptions.
    */
   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::SubframeSelectorProcess;
      new pcl::SubframeSelectorInterface;
   }

   /*
    * Return zero to signal successful installation
    */
   return 0;
}

// ----------------------------------------------------------------------------
// EOF SubframeSelectorModule.cpp - Released 2018-11-01T11:07:21Z
