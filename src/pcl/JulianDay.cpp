//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// pcl/JulianDay.cpp - Released 2016/02/21 20:22:19 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/Math.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Julian Day Number Conversion Algorithms
 *
 * Based on:
 * Meeus, Jean (1991), Astronomical Algorithms, Willmann-Bell, Inc., ch. 7.
 *
 * We have modified the original Meeus' algorithms to support negative julian
 * day numbers.
 */

void PCL_FUNC ComplexTimeToJD( int& jdi, double& jdf, int year, int month, int day, double dayf )
{
   if ( month <= 2 )
   {
      --year;
      month += 12;
   }

   jdi = TruncInt( Floor( 365.25*(year + 4716) ) ) + TruncInt( 30.6001*(month + 1) ) + day - 1524;
   jdf = dayf - 0.5;

   if ( jdi > 0 && jdf < 0  )
   {
      jdf += 1;
      --jdi;
   }
   else if ( jdi < 0 && jdf > 0 )
   {
      jdf -= 1;
      ++jdi;
   }

   if ( jdi > 2299160 || jdi == 2299160 && jdf >= 0.5 )
   {
      int a = TruncInt( 0.01*year );
      jdi += 2 - a + (a >> 2);
   }
}

void PCL_FUNC JDToComplexTime( int& year, int& month, int& day, double& dayf, int jdi, double jdf )
{
   dayf = jdf + 0.5;
   if ( dayf >= 1 )
   {
      dayf -= 1;
      ++jdi;
   }
   else if ( dayf < 0 )
   {
      dayf += 1;
      --jdi;
   }

   if ( jdi > 2299160 )
   {
      int a = TruncInt( (jdi - 1867216.25)/36524.25 );
      jdi += 1 + a - (a >> 2);
   }

   int B = jdi + 1524;
   int C = TruncInt( (B - 122.1)/365.25 );

   if ( jdi < -1401 )  // -1400.5 = -4716 feb 29
      --C;

   int D = TruncInt( Floor( 365.25*C ) );
   int E = TruncInt( (B - D)/30.6001 );

   day = B - D - TruncInt( 30.6001*E );
   month = E - ((E < 14) ? 1 : 13);
   year = C - ((month > 2) ? 4716 : 4715);
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/JulianDay.cpp - Released 2016/02/21 20:22:19 UTC
