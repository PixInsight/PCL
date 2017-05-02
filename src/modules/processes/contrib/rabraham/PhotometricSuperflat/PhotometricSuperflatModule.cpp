// ****************************************************************************
// PixInsight Class Library - PCL 02.00.09.0651
// Standard PhotometricSuperflat Process Module Version 01.00.02.0116
// ****************************************************************************
// PhotometricSuperflatModule.cpp - Released 2013/12/13 00:03:11 UTC
// ****************************************************************************
// This file is part of the standard PhotometricSuperflat PixInsight module.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#define MODULE_VERSION_MAJOR     01
#define MODULE_VERSION_MINOR     00
#define MODULE_VERSION_REVISION  02
#define MODULE_VERSION_BUILD     0116
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2013
#define MODULE_RELEASE_MONTH     12
#define MODULE_RELEASE_DAY       13

#include "PhotometricSuperflatModule.h"
#include "PhotometricSuperflatProcess.h"
#include "PhotometricSuperflatInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

PhotometricSuperflatModule::PhotometricSuperflatModule() : MetaModule()
{
}

const char* PhotometricSuperflatModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

IsoString PhotometricSuperflatModule::Name() const
{
   return "PhotometricSuperflat";
}

String PhotometricSuperflatModule::Description() const
{
   return "PixInsight PhotometricSuperflat Process Module"; // Replace with your own description
}

String PhotometricSuperflatModule::Company() const
{
   return "Your company comes here";
}

String PhotometricSuperflatModule::Author() const
{
   return "Your name comes here";
}

String PhotometricSuperflatModule::Copyright() const
{
   return "Copyright (c) the year you wrote this, your name";
}

String PhotometricSuperflatModule::TradeMarks() const
{
   return "Your trade marks come here";
}

String PhotometricSuperflatModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "PhotometricSuperflat-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "PhotometricSuperflat-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "PhotometricSuperflat-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "PhotometricSuperflat-pxm.dll";
#endif
}

void PhotometricSuperflatModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // pcl

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::PhotometricSuperflatModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::PhotometricSuperflatProcess;
      new pcl::PhotometricSuperflatInterface;
   }

   return 0;
}

// ****************************************************************************
// EOF PhotometricSuperflatModule.cpp - Released 2013/12/13 00:03:11 UTC
