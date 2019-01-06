//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/Diagnostics.h - Released 2018-12-12T09:24:21Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#ifndef __PCL_Diagnostics_h
#define __PCL_Diagnostics_h

#include <pcl/Defs.h>

namespace pcl
{

// ----------------------------------------------------------------------------
// Definitions and macros for implementation of diagnostics code.
// ----------------------------------------------------------------------------
// ###### THIS HEADER IS FOR INTERNAL USE EXCLUSIVELY - PLEASE IGNORE IT ######
// ----------------------------------------------------------------------------

bool PCL_FUNC IsConsoleDiagnostics();

void PCL_FUNC SetConsoleDiagnostics( bool set = true );

inline void SetGUIDiagnostics( bool set = true )
{
   SetConsoleDiagnostics( !set );
}

} // pcl

extern "C" void PCL_FUNC __PCLAssertFail( int, const char*, const char*, int );

#define __PCL_ASSERTION     0
#define __PCL_PRECONDITION  1
#define __PCL_CHECK         2

// ----------------------------------------------------------------------------
// The value of __PCL_DIAGNOSTICS_LEVEL controls generation of diagnostics
// code, as follows:
//    0 = no diagnostics code is generated (the default value)
//    1 = CHECK code only
//    2 = both CHECK and PRECONDITION code
// ----------------------------------------------------------------------------

#ifndef __PCL_DIAGNOSTICS_LEVEL
#define __PCL_DIAGNOSTICS_LEVEL 0
#endif

// ----------------------------------------------------------------------------
// Precondition diagnostics macro.
// This macro must be used when the arguments of a function are required to
// satisfy a given condition p. The default behavior, under diagnostics mode,
// is to terminate program execution if p doesn't hold.
// ----------------------------------------------------------------------------

#if     __PCL_DIAGNOSTICS_LEVEL > 1
#define PCL_PRECONDITION( __p ) \
   ((__p) ? (void)0 : \
   (void)__PCLAssertFail( __PCL_PRECONDITION, #__p, __FILE__, __LINE__ ));
#else
#define PCL_PRECONDITION( __p )
#endif

// ----------------------------------------------------------------------------
// Check diagnostics macro.
// This macro should be used at any point where execution cannot continue if a
// given condition p is not satisfied, except when a precondition applies. The
// default behavior is to terminate program execution if p doesn't hold.
// ----------------------------------------------------------------------------

#if     __PCL_DIAGNOSTICS_LEVEL > 0
#define PCL_CHECK( __p ) \
   ((__p) ? (void)0 : \
   (void)__PCLAssertFail( __PCL_CHECK, #__p, __FILE__, __LINE__ ));
#else
#define PCL_CHECK( __p )
#endif

// ----------------------------------------------------------------------------

#endif  // __PCL_Diagnostics_h

// ----------------------------------------------------------------------------
// EOF pcl/Diagnostics.h - Released 2018-12-12T09:24:21Z
