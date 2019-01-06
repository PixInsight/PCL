//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard GREYCstoration Process Module Version 01.00.02.0341
// ----------------------------------------------------------------------------
// GREYCstorationModule.cpp - Released 2018-12-12T09:25:24Z
// ----------------------------------------------------------------------------
// This file is part of the standard GREYCstoration PixInsight module.
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

/******************************************************************************
 * CImg Library and GREYCstoration Algorithm:
 *   Copyright David Tschumperlé - http://www.greyc.ensicaen.fr/~dtschump/
 *
 * See:
 *   http://cimg.sourceforge.net
 *   http://www.greyc.ensicaen.fr/~dtschump/greycstoration/
 *
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and or redistribute the software under the terms of the CeCILL-C
 * license as circulated by CEA, CNRS and INRIA at the following URL:
 * "http://www.cecill.info".
 *****************************************************************************/

#define MODULE_VERSION_MAJOR     01
#define MODULE_VERSION_MINOR     00
#define MODULE_VERSION_REVISION  02
#define MODULE_VERSION_BUILD     0341
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2018
#define MODULE_RELEASE_MONTH     12
#define MODULE_RELEASE_DAY       12

#include "GREYCstorationModule.h"
#include "GREYCstorationProcess.h"
#include "GREYCstorationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

GREYCstorationModule::GREYCstorationModule() : MetaModule()
{
}

const char* GREYCstorationModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

IsoString GREYCstorationModule::Name() const
{
   return "GREYCstoration";
}

String GREYCstorationModule::Description() const
{
   return "PixInsight GREYCstoration Process Module";
}

String GREYCstorationModule::Company() const
{
   return "Pleiades Astrophoto";
}

String GREYCstorationModule::Author() const
{
   return "CImg: David Tschumperl&eacute; / PCL module: Juan Conejero, PTeam";
}

String GREYCstorationModule::Copyright() const
{
   return "CImg: Copyright (c) David Tschumperl&eacute; / PCL: Copyright (c) 2003-2010, Pleiades Astrophoto";
}

String GREYCstorationModule::TradeMarks() const
{
   return "GREYCstoration, CImg, PixInsight";
}

String GREYCstorationModule::OriginalFileName() const
{
#ifdef __PCL_LINUX
   return "GREYCstoration-pxm.so";
#endif
#ifdef __PCL_FREEBSD
   return "GREYCstoration-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "GREYCstoration-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "GREYCstoration-pxm.dll";
#endif
}

void GREYCstorationModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // pcl

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::GREYCstorationModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::GREYCstorationProcess;
      new pcl::GREYCstorationInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF GREYCstorationModule.cpp - Released 2018-12-12T09:25:24Z
