//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
//
// This file is part of the JPL planetary ephemeris test utility.
//
// Copyright (c) 2016 Pleiades Astrophoto S.L.
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

/*
 * A command line utility to test the pcl::JPLEphemeris class.
 *
 * Usage: jpltest <header-file> <test-file> <data-file>[ <data-file> ...]
 *
 * The program is useful to verify the integrity of JPL ephemeris calculation
 * routines implemented in PCL.
 *
 * Written by Juan Conejero, PTeam.
 * Copyright (c) 2016, Pleiades Astrophoto S.L.
 */

#include <pcl/JPLEphemeris.h>

#include <wordexp.h>
#include <iostream>

using namespace pcl;

// ----------------------------------------------------------------------------

int main( int argc, const char* argv[] )
{
   Exception::DisableGUIOutput();
   Exception::EnableConsoleOutput();

   try
   {
      StringList cmdLineArgs;
      for ( int i = 1; i < argc; ++i )
      {
         wordexp_t p;
         if ( wordexp( argv[i], &p, 0 ) != 0 )
            throw Error( "Invalid argument: '" + String( argv[i] ) + '\'' );
         for ( unsigned i = 0; i < p.we_wordc; ++i )
            cmdLineArgs.Add( String::UTF8ToUTF16( p.we_wordv[i] ) );
         wordfree( &p );
      }

      std::cout << String().ToNewLineSeparated( cmdLineArgs ) << '\n' << '\n';

      ArgumentList arguments = ExtractArguments( cmdLineArgs,
                                                 ArgumentItemMode::AsFiles,
                                                 ArgumentOption::AllowWildcards );
      StringList files;

      for ( ArgumentList::const_iterator i = arguments.Begin(); i != arguments.End(); ++i )
      {
         if ( i->IsItemList() )
            for ( StringList::const_iterator j = i->Items().Begin(); j != i->Items().End(); ++j )
               files << *j;
         else
            throw Error( "Unknown argument: " + i->Token() );
         /*
         else if ( i->IsNumeric() )
            throw Error( "Unknown numeric argument: " + i->Token() );
         else if ( i->IsString() )
            throw Error( "Unknown string argument: " + i->Token() );
         else if ( i->IsSwitch() )
            throw Error( "Unknown switch argument: " + i->Token() );
         else if ( i->IsLiteral() )
            throw Error( "Unknown argument: " + i->Token() );
         */
      }

      if ( files.Length() < 3 )
      {
         std::cout << "Usage: jpltest <header-file> <test-file> <data-file>[ <data-file> ...]\n";
         return 1;
      }

      JPLEphemeris ephem( files[0] );
      std::cout << ephem.Summary() << '\n';

      StringList coefficientFiles( files.At( 2 ), files.End() );
      coefficientFiles.Sort();
      for ( auto coefficientFile : coefficientFiles )
         ephem.AddData( coefficientFile );

      ephem.Test( files[1] );

      return 0;
   }
   ERROR_HANDLER
   return -1;
}

// ----------------------------------------------------------------------------
// EOF pcl/jpltest.cpp - Released 2016/06/24 23:10:29 UTC
