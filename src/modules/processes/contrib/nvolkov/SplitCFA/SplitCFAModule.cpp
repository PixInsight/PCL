//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard SplitCFA Process Module Version 01.00.06.0191
// ----------------------------------------------------------------------------
// SplitCFAModule.cpp - Released 2019-01-21T12:06:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard SplitCFA PixInsight module.
//
// Copyright (c) 2013-2018 Nikolay Volkov
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
#define MODULE_VERSION_MINOR     00
#define MODULE_VERSION_REVISION  06
#define MODULE_VERSION_BUILD     0191
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2019
#define MODULE_RELEASE_MONTH     1
#define MODULE_RELEASE_DAY       21

#include "MergeCFAInterface.h"
#include "MergeCFAProcess.h"
#include "SplitCFAInterface.h"
#include "SplitCFAModule.h"
#include "SplitCFAProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SplitCFAModule::SplitCFAModule() : MetaModule()
{
}

const char* SplitCFAModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

IsoString SplitCFAModule::Name() const
{
   return "SplitCFA";
}

String SplitCFAModule::Description() const
{
   return "SplitCFA module";
}

String SplitCFAModule::Company() const
{
   return String();
}

String SplitCFAModule::Author() const
{
   return "Nikolay Volkov";
}

String SplitCFAModule::Copyright() const
{
   return "Copyright (c) 2013-2018 Nikolay Volkov";
}

String SplitCFAModule::TradeMarks() const
{
   return "PixInsight";
}

String SplitCFAModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "SplitCFA-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "SplitCFA-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "SplitCFA-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "SplitCFA-pxm.dll";
#endif
}

void SplitCFAModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

#define READABLE_VERSION_NUMBERS_1( a, b, c, d )  #a + '.' + #b + '.' + #c + '.' + #d
#define READABLE_VERSION_NUMBERS( a, b, c, d ) READABLE_VERSION_NUMBERS_1( a, b, c, d )

String SplitCFAModule::ReadableVersion()
{
   return String( "SplitCFA module " ) +
            READABLE_VERSION_NUMBERS( MODULE_VERSION_MAJOR,
                                      MODULE_VERSION_MINOR,
                                      MODULE_VERSION_REVISION,
                                      MODULE_VERSION_BUILD );
}

// ----------------------------------------------------------------------------

} // pcl

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::SplitCFAModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::SplitCFAProcess;
      new pcl::SplitCFAInterface;
      new pcl::MergeCFAProcess;
      new pcl::MergeCFAInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF SplitCFAModule.cpp - Released 2019-01-21T12:06:42Z
