//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/UnixSignalException.cpp - Released 2019-01-21T12:06:21Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#define STACK_DEPTH 256

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

// ----------------------------------------------------------------------------

static void CriticalSignalHandler( int signalNumber )
{
   static const char* signalNames[] =
   {
      "",
      "SIGHUP",  //  1
      "SIGINT",  //  2
      "SIGQUIT", //  3
      "SIGILL",  //  4
      "SIGTRAP", //  5
      "SIGABRT", //  6
      "SIGBUS",  //  7
      "SIGFPE",  //  8
      "SIGKILL", //  9
      "SIGUSR1", // 10
      "SIGSEGV", // 11
      "SIGUSR2", // 12
      "SIGPIPE", // 13
      "SIGALRM", // 14
      "SIGTERM"  // 15
   };

   sigset_t x;
   sigemptyset( &x );
   sigaddset( &x, SIGSEGV );
   sigaddset( &x, SIGBUS );
   sigaddset( &x, SIGFPE );
   sigaddset( &x, SIGILL );
   sigaddset( &x, SIGPIPE );
   //sigprocmask( SIG_UNBLOCK, &x, nullptr );
   pthread_sigmask( SIG_UNBLOCK, &x, nullptr );

   void* addrList[ STACK_DEPTH ];
   size_t addrLen = backtrace( addrList, sizeof( addrList )/sizeof( void* ) );
   char** symbolList = backtrace_symbols( addrList, addrLen );
   IsoString details;
   if ( symbolList != nullptr )
      if ( symbolList[0] != nullptr )
      {
         details = IsoString( PixInsightVersion::AsString() ) + " - Critical Signal Backtrace\n";
         details.AppendFormat( "Received signal %d (%s)\n"
                               "Module: %s\n",
                               signalNumber,
                               (signalNumber > 0 && signalNumber < 16) ? signalNames[signalNumber] : "?",
                               symbolList[0] );
         details << IsoString( '=', 80 ) << '\n';
         for ( size_t i = 1; i < addrLen; ++i )
            if ( symbolList[i] != nullptr )
            {
               IsoString addrOffsetStr;
               IsoString demangledFuncname = GetDemangledFunctionName( symbolList[i], addrOffsetStr );
               details.AppendFormat( "%3u: ", addrLen-i );
               details += demangledFuncname;
               if ( !addrOffsetStr.IsEmpty() )
               {
                  details += "(+";
                  details += addrOffsetStr;
                  details += ')';
               }
               details += '\n';
            }
         details << IsoString( '=', 80 ) << '\n';

         fprintf( stderr, "\n\n%s", details.c_str() );

         /*
          * The symbol list array is allocated by backtrace_symbols using
          * malloc() and should be released by the caller using free(). There
          * is no need to free the individual strings in the array.
          *
          * See references:
          *    https://linux.die.net/man/3/backtrace_symbols
          *    https://developer.apple.com/library/archive/documentation/
          *                System/Conceptual/ManPages_iPhoneOS/man3/
          *                backtrace_symbols.3.html
          */
         free( symbolList );
      }

   switch ( signalNumber )
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
      throw UnixSignalException( signalNumber, details );
   }
}

// ----------------------------------------------------------------------------

void UnixSignalException::Show() const
{
   bool wasConsole = IsConsoleOutputEnabled();
   bool wasGUI = IsGUIOutputEnabled();
   EnableConsoleOutput();
   DisableGUIOutput();

   Exception::Show();

   EnableConsoleOutput( wasConsole );
   EnableGUIOutput( wasGUI );
}

// ----------------------------------------------------------------------------

void UnixSignalException::Initialize()
{
   struct sigaction criticalAction;
   criticalAction.sa_flags = 0;
   criticalAction.sa_handler = CriticalSignalHandler;
   sigemptyset( &criticalAction.sa_mask );
   sigaction( SIGSEGV, &criticalAction, nullptr );
   sigaction( SIGBUS, &criticalAction, nullptr );
   sigaction( SIGFPE, &criticalAction, nullptr );
   sigaction( SIGILL, &criticalAction, nullptr );
   sigaction( SIGPIPE, &criticalAction, nullptr );
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_LINUX || __PCL_FREEBSD || __PCL_MACOSX

// ----------------------------------------------------------------------------
// EOF pcl/UnixSignalException.cpp - Released 2019-01-21T12:06:21Z
