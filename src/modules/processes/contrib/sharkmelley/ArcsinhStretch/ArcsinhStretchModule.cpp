//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard ArcsinhStretch Process Module Version 01.00.00.0132
// ----------------------------------------------------------------------------
// ArcsinhStretchModule.cpp - Released 2018-11-23T18:45:59Z
// ----------------------------------------------------------------------------
// This file is part of the standard ArcsinhStretch PixInsight module.
//
// Copyright (c) 2017-2018 Mark Shelley
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
#define MODULE_VERSION_BUILD     0132
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2018
#define MODULE_RELEASE_MONTH     11
#define MODULE_RELEASE_DAY       23

#include "ArcsinhStretchModule.h"
#include "ArcsinhStretchProcess.h"
#include "ArcsinhStretchInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ArcsinhStretchModule::ArcsinhStretchModule() : MetaModule()
{
}

// ----------------------------------------------------------------------------

const char* ArcsinhStretchModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString ArcsinhStretchModule::Name() const
{
   return "ArcsinhStretch";
}

// ----------------------------------------------------------------------------

String ArcsinhStretchModule::Description() const
{
   return "ArcsinhStretch module. Apply hyperbolic arcsine stretch.";
}

// ----------------------------------------------------------------------------

String ArcsinhStretchModule::Company() const
{
   return String();
}

// ----------------------------------------------------------------------------

String ArcsinhStretchModule::Author() const
{
   return "Mark Shelley";
}

// ----------------------------------------------------------------------------

String ArcsinhStretchModule::Copyright() const
{
   return "Copyright (c) 2017 Mark Shelley";
}

// ----------------------------------------------------------------------------

String ArcsinhStretchModule::TradeMarks() const
{
   return String();
}

// ----------------------------------------------------------------------------

String ArcsinhStretchModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "ArcsinhStretch-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "ArcsinhStretch-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "ArcsinhStretch-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "ArcsinhStretch-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void ArcsinhStretchModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // pcl

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::ArcsinhStretchModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::ArcsinhStretchProcess;
      new pcl::ArcsinhStretchInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF ArcsinhStretchModule.cpp - Released 2018-11-23T18:45:59Z
