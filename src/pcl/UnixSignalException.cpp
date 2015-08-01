//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// pcl/UnixSignalException.cpp - Released 2015/07/30 17:15:31 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#if defined( __PCL_LINUX ) || defined( __PCL_FREEBSD ) || defined( __PCL_MACOSX )

#include <pcl/UnixSignalException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static void CriticalSignalHandler( int sig_num )
{
   sigset_t x;
   sigemptyset( &x );
   sigaddset( &x, SIGSEGV );
   sigaddset( &x, SIGBUS );
   sigaddset( &x, SIGFPE );
   //sigprocmask( SIG_UNBLOCK, &x, NULL );
   pthread_sigmask( SIG_UNBLOCK, &x, NULL );

   switch ( sig_num )
   {
   case SIGSEGV:
      throw EUnixSegmentationViolation();
   case SIGBUS:
      throw EUnixBusError();
   case SIGFPE:
      throw EUnixFloatingPointException();
   default:
      throw UnixSignalException( sig_num );
   }
}

void UnixSignalException::Initialize()
{
   struct sigaction criticalAction;
   criticalAction.sa_flags = 0;
   criticalAction.sa_handler = CriticalSignalHandler;
   sigemptyset( &criticalAction.sa_mask );
   sigaction( SIGSEGV, &criticalAction, 0 );
   sigaction( SIGBUS, &criticalAction, 0 );
   sigaction( SIGFPE, &criticalAction, 0 );
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_LINUX || __PCL_FREEBSD || __PCL_MACOSX

// ----------------------------------------------------------------------------
// EOF pcl/UnixSignalException.cpp - Released 2015/07/30 17:15:31 UTC
