//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// pcl/UnixSignalException.cpp - Released 2018-11-23T16:14:31Z
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

#if defined( __PCL_LINUX ) || defined( __PCL_FREEBSD ) || defined( __PCL_MACOSX )

#include <pcl/StringList.h>
#include <pcl/UnixSignalException.h>
#include <pcl/Version.h>

#include <cxxabi.h>
#include <execinfo.h>
#include <stdio.h>

#define STACK_DEPTH 60

#define STACK_TRACE_TITLE     \
   "\n\n" + IsoString( PixInsightVersion::AsString() ) + " - Critical Signal Backtrace\n"

#define STACK_TRACE_HEADER    \
   "Signal : %d\n"            \
   "Module : %s\n"            \
   "========================================================================================================================\n"

#define STACK_TRACE_BOTTOM    \
   "========================================================================================================================\n"

namespace pcl
{

// ----------------------------------------------------------------------------

static IsoString GetDemangledFunctionName( const char* symbol, IsoString& addrStr )
{
   IsoString symbolStr( symbol );
   addrStr.Clear();

   // Get mangled function name. Example:
   //    /opt/PixInsight/bin/lib/libQtGui.so.4(_ZN7QWidget5eventEP6QEvent+0x411) [0x7fa271347811]
   StringList tokens;
   symbolStr.Break( tokens, '(' , true/*trim*/ );
   if ( tokens.Length() != 2 )
      return symbolStr;

   // Take second token and split again.
   StringList tokens2;
   tokens[1].Break( tokens2, '+' , true/*trim*/ );
   if ( tokens2.Length() != 2 )
      return symbolStr;

   // If there is no function name, do not set the addr string.
   if ( !tokens2[0].IsEmpty() )
   {
      addrStr = tokens2[1];
      addrStr.DeleteChar( '(' );
      addrStr.DeleteChar( ')' );
   }

   // The first token of tokens2 contains the mangled string. Demangle it.
   size_t funcnameSize = 256;
   char funcname[ funcnameSize ];
   int status;
   IsoString token( tokens2[0] );
   const char* demangledFuncname = abi::__cxa_demangle( token.c_str(), funcname, &funcnameSize, &status );
   return (status == 0) ? IsoString( demangledFuncname ) : symbolStr;
}

static void CriticalSignalHandler( int sig_num )
{
   sigset_t x;
   sigemptyset( &x );
   sigaddset( &x, SIGSEGV );
   sigaddset( &x, SIGBUS );
   sigaddset( &x, SIGFPE );
   sigaddset( &x, SIGILL );
   sigaddset( &x, SIGPIPE );
   //sigprocmask( SIG_UNBLOCK, &x, NULL );
   pthread_sigmask( SIG_UNBLOCK, &x, NULL );

   void* addrList[ STACK_DEPTH ];
   size_t addrLen = backtrace( addrList, sizeof( addrList )/sizeof( void* ) );
   char** symbolList = backtrace_symbols( addrList, addrLen );
   IsoString details;
   if ( symbolList != NULL )
   {
      if ( symbolList[0] != NULL )
      {
         details = STACK_TRACE_TITLE + IsoString().Format( STACK_TRACE_HEADER, sig_num, symbolList[0] );
         for ( size_t i = 1; i < addrLen; ++i )
         {
            if ( symbolList[i] != NULL )
            {
               IsoString addrOffsetStr;
               IsoString demangledFuncname = GetDemangledFunctionName( symbolList[i], addrOffsetStr );
               details.AppendFormat( "%d: ", addrLen-i );
               details += demangledFuncname;
               if ( !addrOffsetStr.IsEmpty() )
               {
                  details += "(+";
                  details += addrOffsetStr;
                  details += ')';
               }
               details += '\n';
            }
         }
         details += STACK_TRACE_BOTTOM;

         fprintf( stderr, "%s", details.c_str() );
      }

      // symbolList must be freed
      free( symbolList );
   }

   switch ( sig_num )
   {
   case SIGSEGV:
      throw EUnixSegmentationViolation( details );
   case SIGBUS:
      throw EUnixBusError( details );
   case SIGFPE:
      throw EUnixFloatingPointException( details );
   case SIGILL:
      throw EUnixIllegalInstructionException( details );
   case SIGPIPE:
      throw EUnixIBrokenPipeException( details );
   default:
      throw UnixSignalException( sig_num, details );
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
   sigaction( SIGILL, &criticalAction, 0 );
   sigaction( SIGPIPE, &criticalAction, 0 );
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_LINUX || __PCL_FREEBSD || __PCL_MACOSX

// ----------------------------------------------------------------------------
// EOF pcl/UnixSignalException.cpp - Released 2018-11-23T16:14:31Z
