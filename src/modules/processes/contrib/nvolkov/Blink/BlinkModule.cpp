//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Blink Process Module Version 01.02.02.0290
// ----------------------------------------------------------------------------
// BlinkModule.cpp - Released 2018-11-23T18:45:59Z
// ----------------------------------------------------------------------------
// This file is part of the standard Blink PixInsight module.
//
// Copyright (c) 2011-2018 Nikolay Volkov
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L.
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
#define MODULE_VERSION_REVISION  02
#define MODULE_VERSION_BUILD     0290
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2018
#define MODULE_RELEASE_MONTH     11
#define MODULE_RELEASE_DAY       23

#include "BlinkModule.h"
#include "BlinkProcess.h"
#include "BlinkInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

BlinkModule::BlinkModule() : MetaModule()
{
}

// ----------------------------------------------------------------------------

const char* BlinkModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

// ----------------------------------------------------------------------------

IsoString BlinkModule::Name() const
{
   return "Blink";
}

// ----------------------------------------------------------------------------

String BlinkModule::Description() const
{
   return "Blink module by Nikolay Volkov - Animates a set of images like a movie.";
}

// ----------------------------------------------------------------------------

String BlinkModule::Company() const
{
   return "";
}

// ----------------------------------------------------------------------------

String BlinkModule::Author() const
{
   return "Nikolay Volkov";
}

// ----------------------------------------------------------------------------

String BlinkModule::Copyright() const
{
   return "Copyright (c) 2011-2018 Nikolay Volkov";
}

// ----------------------------------------------------------------------------

String BlinkModule::TradeMarks() const
{
   return "PixInsight";
}

// ----------------------------------------------------------------------------

String BlinkModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "Blink-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "Blink-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Blink-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Blink-pxm.dll";
#endif
}

// ----------------------------------------------------------------------------

void BlinkModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // pcl

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::BlinkModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::BlinkProcess;
      new pcl::BlinkInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF BlinkModule.cpp - Released 2018-11-23T18:45:59Z
