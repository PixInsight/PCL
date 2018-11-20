//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
//
// This file is part of the ChebyshevFit test utility.
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
 * A command line utility to test the pcl::ChebyshevFit template class.
 *
 * Usage: chbtest (no arguments for now)
 *
 * Written by Juan Conejero, PTeam.
 * Copyright (c) 2016, Pleiades Astrophoto S.L.
 */

#include <pcl/ChebyshevFit.h>

//#include <wordexp.h>
#include <iostream>

using namespace pcl;

// ----------------------------------------------------------------------------

/*
 * The test function
 */
static double f( double x )
{
   return 2*x*x + 3*x*x*x - x*Sin( x ) + Cos( 2*x );
}

/*
 * The derivative of f(x)
 */
static double fd( double x )
{
   return x*(4 + 9*x) - x*Cos( x ) - Sin( x ) - 2*Sin( 2*x );
}

/*
 * The indefinite integral of f(x)
 */
static double fi( double x )
{
   return 1.0/12 * x*x*x*(8 + 9*x) - Sin( x ) + Cos( x )*(x + Sin( x ));
}

int main( int argc, const char* argv[] )
{
   Exception::DisableGUIOutput();
   Exception::EnableConsoleOutput();

   try
   {
      // Fitting interval.
      const double x1 = -10.0;
      const double x2 = +10.0;

      // Number of coefficients.
      const int n = 50;

      // Error bound.
      const double eps = 1.0e-08;

      // Construct Chebyshev expansions for the function, its derivative and
      // its indefinite integral.
      ScalarChebyshevFit T( f, x1, x2, n );
      ScalarChebyshevFit Td = T.Derivative();
      ScalarChebyshevFit Ti = T.Integral();

      std::cout << IsoString().Format( "n = %d  e = %.3g\n", T.Length(), T.TruncationError() );

      // Truncate the expansions to a given error bound.
      T.Truncate( eps );
      Td.Truncate( eps );
      Ti.Truncate( eps );
      std::cout << IsoString().Format( "Tn  = %d  Te  = %.3g\n", T.TruncatedLength(), T.TruncationError() );
      std::cout << IsoString().Format( "Tdn = %d  Tde = %.3g\n", Td.TruncatedLength(), Td.TruncationError() );
      std::cout << IsoString().Format( "Tin = %d  Tie = %.3g\n", Ti.TruncatedLength(), Ti.TruncationError() );

      // Set the arbitrary constant of integration to vanish the indefinite
      // integral at x = x1.
      double C = fi( x1 );
      std::cout << IsoString().Format( "C = %+.15e\n", C );

      // Tabulate values for the function, its derivative and integral, with
      // approximation errors.
      for ( int i = 0, numTests = 20; i <= numTests; ++i )
      {
         // Evaluation point.
         double x   = x1 + i*(x2 - x1)/numTests;

         // Function and its analytic derivative and integral.
         double y   = f( x );
         double yd  = fd( x );
         double yi  = fi( x ) - C;

         // Approximated function, derivative and integral.
         double yT  = T.Evaluate( x );
         double yTd = Td.Evaluate( x );
         double yTi = Ti.Evaluate( x );

//          1         2         3
// 1234567890123456789012345678901234
//      +7.000 +1.122537831027176e+03
         std::cout << IsoString().Format( "%+11.3f %+.15e %+.15e %+.15e\n"
                                                "%+34.15e %+.15e %+.15e\n"
                                                 "%22.3e %22.3e %22.3e\n",
                                          x, y, yd, yi,
                                          yT, yTd, yTi,
                                          Abs( yT - y ), Abs( yTd - yd ), Abs( yTi - yi ) );
      }

      return 0;
   }
   ERROR_HANDLER
   return -1;
}

// ----------------------------------------------------------------------------
// EOF pcl/chbtest.cpp - Released 2016/06/30 12:08:36 UTC
