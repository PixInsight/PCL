//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 01.01.03.0276
// ----------------------------------------------------------------------------
// ConvolutionModule.cpp - Released 2018-12-12T09:25:24Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
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

#define MODULE_VERSION_MAJOR     01
#define MODULE_VERSION_MINOR     01
#define MODULE_VERSION_REVISION  03
#define MODULE_VERSION_BUILD     0276
#define MODULE_VERSION_LANGUAGE  eng

#define MODULE_RELEASE_YEAR      2018
#define MODULE_RELEASE_MONTH     12
#define MODULE_RELEASE_DAY       12

#include "ConvolutionModule.h"
#include "ConvolutionProcess.h"
#include "ConvolutionInterface.h"
#include "UnsharpMaskProcess.h"
#include "UnsharpMaskInterface.h"
#include "LarsonSekaninaProcess.h"
#include "LarsonSekaninaInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ConvolutionModule::ConvolutionModule() : MetaModule()
{
}

const char* ConvolutionModule::Version() const
{
   return PCL_MODULE_VERSION( MODULE_VERSION_MAJOR,
                              MODULE_VERSION_MINOR,
                              MODULE_VERSION_REVISION,
                              MODULE_VERSION_BUILD,
                              MODULE_VERSION_LANGUAGE );
}

IsoString ConvolutionModule::Name() const
{
   return "Convolution";
}

String ConvolutionModule::Description() const
{
   return "PixInsight Convolution Process Module"; // Replace with your own description
}

String ConvolutionModule::Company() const
{
   return "Pleiades Astrophoto S.L.";
}

String ConvolutionModule::Author() const
{
   return "Convolution: Javier Sanchis Mu&ntilde;oz / Juan Conejero\n"
          "UnsharpMask: Carlos Milovic F. / Juan Conejero\n"
          "LarsonSekanina: Carlos Milovic F. / Juan Conejero";
}

String ConvolutionModule::Copyright() const
{
   return "Copyright (c) 2006-2012 Pleiades Astrophoto S.L.";
}

String ConvolutionModule::TradeMarks() const
{
   return "Pixinsight";
}

String ConvolutionModule::OriginalFileName() const
{
#ifdef __PCL_LINUX
   return "Convolution-pxm.so";
#endif
#ifdef __PCL_FREEBSD
   return "Convolution-pxm.so";
#endif
#ifdef __PCL_MACOSX
   return "Convolution-pxm.dylib";
#endif
#ifdef __PCL_WINDOWS
   return "Convolution-pxm.dll";
#endif
}

void ConvolutionModule::GetReleaseDate( int& year, int& month, int& day ) const
{
   year  = MODULE_RELEASE_YEAR;
   month = MODULE_RELEASE_MONTH;
   day   = MODULE_RELEASE_DAY;
}

// ----------------------------------------------------------------------------

} // pcl

PCL_MODULE_EXPORT int InstallPixInsightModule( int mode )
{
   new pcl::ConvolutionModule;

   if ( mode == pcl::InstallMode::FullInstall )
   {
      new pcl::ConvolutionProcess;
      new pcl::ConvolutionInterface;
      new pcl::UnsharpMaskProcess;
      new pcl::UnsharpMaskInterface;
      new pcl::LarsonSekaninaProcess;
      new pcl::LarsonSekaninaInterface;
   }

   return 0;
}

// ----------------------------------------------------------------------------
// EOF ConvolutionModule.cpp - Released 2018-12-12T09:25:24Z
