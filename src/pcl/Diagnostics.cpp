//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0837
// ----------------------------------------------------------------------------
// pcl/Diagnostics.cpp - Released 2017-06-09T08:12:54Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <stdlib.h> // for exit()

#include <iostream>

#include <pcl/Diagnostics.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static bool s_useConsole = false;

// ----------------------------------------------------------------------------

bool PCL_FUNC IsConsoleDiagnostics()
{
   return s_useConsole;
}

// ----------------------------------------------------------------------------

void PCL_FUNC SetConsoleDiagnostics( bool b )
{
   s_useConsole = b;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

extern "C"
void __PCLAssertFail(
   int __type, const char* __cond, const char* __file, int __line )
{
   pcl::IsoString msg;

   msg << "Diagnostics Level " << pcl::IsoString( __PCL_DIAGNOSTICS_LEVEL ) << ": ";

   const char* typeMsg;

   switch ( __type )
   {
   case __PCL_PRECONDITION :
      typeMsg = "Precondition violated"; break;
   case __PCL_CHECK :
      typeMsg = "Check failed"; break;
   default :
      typeMsg = "Assertion not satisfied"; break;
   }

   msg << typeMsg << ":\n"
       << __cond
       << "\nFile: " << __file << ", Line: " << pcl::IsoString( __line )
       << "\nProgram terminated.";

#define caption   "PCL Diagnostics Event: "

   if ( pcl::s_useConsole )
   {
      std::cerr << '\n' << caption << msg << '\n';
      exit( 1 );
   }
   else
      throw pcl::FatalError( caption + msg );

#undef caption
}

// ----------------------------------------------------------------------------
// EOF pcl/Diagnostics.cpp - Released 2017-06-09T08:12:54Z
