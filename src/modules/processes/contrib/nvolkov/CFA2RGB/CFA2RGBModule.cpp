//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard CFA2RGB Process Module Version 01.01.01.0010
// ----------------------------------------------------------------------------
// CFA2RGBModule.cpp - Released 2016/02/03 00:00:00 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard CFA2RGB PixInsight module.
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
// ****************************************************************************

#define MODULE_VERSION_MAJOR     01
#define MODULE_VERSION_MINOR     01
#define MODULE_VERSION_REVISION  01
#define MODULE_VERSION_BUILD     0010
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2016
#define MODULE_RELEASE_MONTH     2
#define MODULE_RELEASE_DAY       3

#include "CFA2RGBModule.h"
#include "CFA2RGBProcess.h"
#include "CFA2RGBInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CFA2RGBModule::CFA2RGBModule() : MetaModule()
{
}

const char* CFA2RGBModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

IsoString CFA2RGBModule::Name() const
{
   return "CFA2RGB";
}

String CFA2RGBModule::Description() const
{
   return "CFA2RGB Module";
}

String CFA2RGBModule::Company() const
{
   return "";
}

String CFA2RGBModule::Author() const
{
   return "Nikolay Volkov";
}

String CFA2RGBModule::Copyright() const
{
   return "Copyright (c) 2012-2016 Nikolay Volkov";
}

String CFA2RGBModule::TradeMarks() const
{
   return "PixInsight";
}

String CFA2RGBModule::OriginalFileName() const
{
#ifdef __PCL_FREEBSD
   return "CFA2RGB-pxm.so";
#endif
#ifdef __PCL_LINUX
   return "CFA2RGB-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "CFA2RGB-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "CFA2RGB-pxm.dll";
#endif
}

void CFA2RGBModule::GetReleaseDate( int& year, int& month, int& day ) const
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
   /*
    * When the PixInsight application installs this module, we just have to
    * instantiate the meta objects describing it.
    */

   new pcl::CFA2RGBModule;

   /*
    * The mode argument tells us what kind of installation is being requested
    * by the PixInsight application. Incomplete installation requests only need
    * module descriptions.
    */
   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::CFA2RGBProcess;
      new pcl::CFA2RGBInterface;
   }

   /*
    * Return zero to signal successful installation
    */
   return 0;
}

// ****************************************************************************
// EOF CFA2RGBModule.cpp - Released 2016/02/03 00:00:00 UTC
