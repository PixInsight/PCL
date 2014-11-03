// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/Parser.cpp - Released 2014/10/29 07:34:21 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/Parser.h>
#include <pcl/Exception.h>
#include <pcl/Math.h>

#include <stdlib.h>
#include <errno.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static double Parse( const String& literal,
                     double min, double max, double tolerance, bool isReal )
{
   String t = literal;
   t.Trim();
   const char16_type* s = t.c_str();

   int digCount = 0;
   int decPoint = 0;
   int exponent = 0;
   const char16_type* exponentPtr = 0;

#define PARSE_ERROR( msg )  throw ParseError( msg, t, c-s )

   for ( const char16_type* c = s; *c != 0; ++c )
      switch ( *c )
      {
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            ++digCount;
            break;

         case '.':
            if ( !isReal )
               PARSE_ERROR( "Decimal point not allowed in integer numeral" );
            if ( exponent )
               PARSE_ERROR( "Misplaced decimal point" );
            if ( decPoint )
               PARSE_ERROR( "Too many decimal points" );
            ++decPoint;
            break;

         case '+':
         case '-':
            if ( c != s && (!exponent || c != exponentPtr+1) )
               PARSE_ERROR( "Misplaced sign" );
            break;

         case 'e':
         case 'E':
         case 'd':
         case 'D':
         case 'f':
         case 'F':
            if ( !isReal )
               PARSE_ERROR( "Exponent not allowed in integer numeral" );
            if ( digCount == 0 )
               PARSE_ERROR( "Misplaced exponent separator" );
            if ( exponent )
               PARSE_ERROR( "Too many exponent separators" );
            exponentPtr = c;
            ++exponent;
            digCount = 0;
            break;

         default:
            if ( CharTraits::IsSpace( *c ) )
               PARSE_ERROR( "Misplaced whitespace" );
            PARSE_ERROR( "Invalid character(s) in numeric literal" );
      }

#undef PARSE_ERROR

   if ( digCount == 0 )
      throw ParseError( "Digit(s) expected" );

   IsoString s8( t );
   char* endptr = 0;

   errno = 0;

   double x = ::strtod( s8.c_str(), &endptr );

   if ( errno == ERANGE )
      throw ParseError( "Numeric value out of representable range", t );

   if ( errno != 0 || (endptr != 0 && *endptr != '\0') )
      throw ParseError( "Internal parser error", t,
                        (endptr != 0 && *endptr != '\0') ? endptr - s8.c_str() : -1 );

   if ( min <= max )
   {
      if ( x < min )
      {
         if ( min - x > tolerance )
            goto outOfRange;
         x = min;
      }

      if ( x > max )
      {
         if ( x - max > tolerance )
            goto outOfRange;
         x = max;
      }
   }

   return x;

outOfRange:

   int prec = isReal ? Range( RoundI( -Log( tolerance ) ), 0, 15 ) : 0;

   throw ParseError( String().Format( "Numeric value out of range: %.*f\n"
                                      "Valid range is: [%.*f,%.*f]",
                                      prec, x,
                                      prec, min, prec, max ) );
}


// ----------------------------------------------------------------------------

double PCL_FUNC ParseReal( const String& s, double min, double max, double tolerance )
{
   return Parse( s, min, max, tolerance, true );
}

double PCL_FUNC ParseInteger( const String& s, double min, double max )
{
   return Parse( s, min, max, 1.0e-07, false );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/Parser.cpp - Released 2014/10/29 07:34:21 UTC
