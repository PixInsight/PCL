//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard CosmeticCorrection Process Module Version 01.02.05.0201
// ----------------------------------------------------------------------------
// CosmeticCorrectionModule.cpp - Released 2017-10-02T10:08:47Z
// ----------------------------------------------------------------------------
// This file is part of the standard CosmeticCorrection PixInsight module.
//
// Copyright (c) 2011-2017 Nikolay Volkov
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L.
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
#define MODULE_VERSION_REVISION  05
#define MODULE_VERSION_BUILD     0201
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2017
#define MODULE_RELEASE_MONTH     10
#define MODULE_RELEASE_DAY       2

#include "CosmeticCorrectionModule.h"
#include "CosmeticCorrectionProcess.h"
#include "CosmeticCorrectionInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CosmeticCorrectionModule::CosmeticCorrectionModule() : MetaModule()
{
}

const char* CosmeticCorrectionModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

IsoString CosmeticCorrectionModule::Name() const
{
   return "CosmeticCorrection";
}

String CosmeticCorrectionModule::Description() const
{
   return "CosmeticCorrection module";
}

String CosmeticCorrectionModule::Company() const
{
   return "";
}

String CosmeticCorrectionModule::Author() const
{
   return "Nikolay Volkov";
}

String CosmeticCorrectionModule::Copyright() const
{
   return "Copyright (c) 2011-2017 Nikolay Volkov";
}

String CosmeticCorrectionModule::TradeMarks() const
{
   return "PixInsight";
}

String CosmeticCorrectionModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "CosmeticCorrection-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "CosmeticCorrection-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "CosmeticCorrection-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "CosmeticCorrection-pxm.dll";
#endif
}

void CosmeticCorrectionModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

#define READABLE_VERSION_NUMBERS_1( a, b, c, d )  #a + '.' + #b + '.' + #c + '.' + #d
#define READABLE_VERSION_NUMBERS( a, b, c, d ) READABLE_VERSION_NUMBERS_1( a, b, c, d )

String CosmeticCorrectionModule::ReadableVersion()
{
   return String( "CosmeticCorrection module " ) +
            READABLE_VERSION_NUMBERS( MODULE_VERSION_MAJOR,
                                      MODULE_VERSION_MINOR,
                                      MODULE_VERSION_REVISION,
                                      MODULE_VERSION_BUILD );
}

// ----------------------------------------------------------------------------

} // pcl

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::CosmeticCorrectionModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::CosmeticCorrectionProcess;
      new pcl::CosmeticCorrectionInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF CosmeticCorrectionModule.cpp - Released 2017-10-02T10:08:47Z
