//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0402
// ----------------------------------------------------------------------------
// PSF.cpp - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "PSF.h"

#include <cminpack/cminpack.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PSFFit::PSFFit( const ImageVariant& image, const DPoint& pos, const DRect& rect, Function function, bool circular )
{
   psf.status = NotFitted;

   if ( !image )
      return;

   /*
    * Pixel sample matrix
    */
   Rect r( TruncI( rect.x0 ),   TruncI( rect.y0 ),
           TruncI( rect.x1 )+1, TruncI( rect.y1 )+1 );
   image.Clip( r );
   S = Matrix::FromImage( image, r );

   /*
    * Center of the sampling region
    */
   DPoint r0 = DRect( r ).Center();

   int h = S.Rows();
   int w = S.Cols();

   /*
    * Setup working parameters
    */
   P = Vector( 8 );
   P[0] = (S.RowVector( 0 ).Median() +
           S.RowVector( h-1 ).Median() +
           S.ColVector( 0 ).Median() +
           S.ColVector( w-1 ).Median())/4;         // B
   P[1] = *MaxItem( S.Begin(), S.End() ) - P[0];   // A
   P[2] = pos.x - r0.x;                            // x0
   P[3] = pos.y - r0.y;                            // y0

   if ( circular )
   {
      P[4] = 0.15*rect.Width();                    // sx
   }
   else
   {
      P[4] = P[5] = 0.15*rect.Width();             // sx, sy
      P[6] = 0;                                    // theta
   }

   /*
    * CMINPACK L-M - Finite differences
    */

   // Number of data points
   int m = S.NumberOfElements();

   // Number of fitted parameters
   int n = (function == Moffat) ? 6 : 5;
   if ( !circular )
      n += 2; // sy, theta

   // Fitting function
   cminpack_callback fitFunc;
   if ( circular )
      switch ( function )
      {
      default:
      case Gaussian:
         fitFunc = FitCircularGaussian;
         break;
      case Moffat:
         fitFunc = FitCircularMoffat;
         break;
      case MoffatA:
         fitFunc = FitCircularMoffatWithFixedBeta;
         break;
      case Moffat8:
         fitFunc = FitCircularMoffatWithFixedBeta;
         break;
      case Moffat6:
         fitFunc = FitCircularMoffatWithFixedBeta;
         break;
      case Moffat4:
         fitFunc = FitCircularMoffatWithFixedBeta;
         break;
      case Moffat25:
         fitFunc = FitCircularMoffatWithFixedBeta;
         break;
      case Moffat15:
         fitFunc = FitCircularMoffatWithFixedBeta;
         break;
      case Lorentzian:
         fitFunc = FitCircularMoffatWithFixedBeta;
         break;
      }
   else
      switch ( function )
      {
      default:
      case Gaussian:
         fitFunc = FitGaussian;
         break;
      case Moffat:
         fitFunc = FitMoffat;
         break;
      case MoffatA:
         fitFunc = FitMoffatWithFixedBeta;
         break;
      case Moffat8:
         fitFunc = FitMoffatWithFixedBeta;
         break;
      case Moffat6:
         fitFunc = FitMoffatWithFixedBeta;
         break;
      case Moffat4:
         fitFunc = FitMoffatWithFixedBeta;
         break;
      case Moffat25:
         fitFunc = FitMoffatWithFixedBeta;
         break;
      case Moffat15:
         fitFunc = FitMoffatWithFixedBeta;
         break;
      case Lorentzian:
         fitFunc = FitMoffatWithFixedBeta;
         break;
      }

   int ibeta = circular ? 5 : 7;
   switch ( function )
   {
   default:
   case Gaussian:
      break;
   case Moffat:
      beta0 = P[ibeta] = 3;
      break;
   case MoffatA:
      P[ibeta] = 10;
      break;
   case Moffat8:
      P[ibeta] = 8;
      break;
   case Moffat6:
      P[ibeta] = 6;
      break;
   case Moffat4:
      P[ibeta] = 4;
      break;
   case Moffat25:
      P[ibeta] = 2.5;
      break;
   case Moffat15:
      P[ibeta] = 1.5;
      break;
   case Lorentzian:
      P[ibeta] = 1;
      break;
   }

   Vector fvec( m );
   IVector iwa( n );
   Vector wa( m*n + 5*n + m );

   int info = lmdif1( fitFunc,
                      this,
                      m,
                      n,
                      P.Begin(),
                      fvec.Begin(),
                      1.0e-08,
                      iwa.Begin(),
                      wa.Begin(),
                      wa.Length() );

   psf.status = CminpackInfoToStatus( info );

   if ( psf )
      if ( function == Moffat )
         if ( P[ibeta] > 9.99 )
            psf.status = NoConvergence;

   psf.function = function;
   psf.circular = circular;

   if ( psf )
   {
      // Force sx > 0
      P[4] = Abs( P[4] );

      if ( !circular )
      {
         // Force 0 < sy < sx
         P[5] = Abs( P[5] );
         if ( P[4] < P[5] )
            Swap( P[4], P[5] );
      }

      if ( psf.FWHM( function, P[4], P[ibeta] ) > rect.Width() )
         psf.status = NoConvergence;
   }

   if ( psf )
   {
      psf.B = Max( 0.0, P[0] );
      psf.A = P[1];
      psf.c0.x = P[2] + r0.x;
      psf.c0.y = P[3] + r0.y;

      if ( circular )
      {
         /*
          * Circular PSF (prescribed)
          */
         psf.sx = psf.sy = P[4];
         psf.theta = 0;
         psf.mad = AbsoluteDeviation( function, true )/m;
      }
      else
      {
         psf.sx = P[4];
         psf.sy = P[5];

         if ( Abs( psf.sx - psf.sy ) < 0.01 )
         {
            /*
             * Circular PSF (incidental)
             */
            psf.theta = 0;
            psf.mad = AbsoluteDeviation( function, false )/m;
         }
         else
         {
            /*
             * Elliptical PSF
             *
             * After LM minimization the rotation angle cannot be determined
             * without uncertainty from the fitted parameters. We check the
             * four possibilities and select the angle that causes the minimum
             * absolute difference with the sampled matrix.
             */

            // Constrain theta to the first quadrant
            psf.theta = P[6];
            psf.theta = ArcTan( Sin( psf.theta ), Cos( psf.theta ) );
            if ( psf.theta < 0 )
               psf.theta += Pi();
            if ( psf.theta > Pi()/2 )
               psf.theta -= Pi()/2;

            // There are four choices that we must check
            Vector a( 4 );
            a[0] = psf.theta;
            a[1] = Pi()/2 - psf.theta;
            a[2] = Pi()/2 + psf.theta;
            a[3] = Pi() - psf.theta;

            // Generate the four models and compute absolute differences
            P[6] = a[0];
            double d = AbsoluteDeviation( function, false );
            int imin = 0;
            for ( int i = 1; i < 4; ++i )
            {
               P[6] = a[i];
               double di = AbsoluteDeviation( function, false, d );
               if ( di < d )
               {
                  imin = i;
                  d = di;
               }
            }
            // Select the orientation angle that minimizes absolute deviation
            psf.theta = Deg( a[imin] );
            psf.mad = d/m;
         }
      }

      // Moffat beta parameter
      psf.beta = (function == Gaussian) ? 0.0 : P[ibeta];

      // Normalize mean absolute deviation with respect to amplitude
      psf.mad /= psf.A;
   }
   else
   {
      psf.B =
      psf.A =
      psf.c0.x = psf.c0.y =
      psf.sx = psf.sy =
      psf.theta =
      psf.beta =
      psf.mad = 0;
   }
}

// ----------------------------------------------------------------------------
// Elliptical PSF Functions
// ----------------------------------------------------------------------------

#define F      reinterpret_cast<const PSFFit*>( p )
#define B      a[0]
#define A      a[1]
#define x0     a[2]
#define y0     a[3]
#define sx     a[4]
#define sy     a[5]
#define theta  a[6]
#define beta   a[7]

/*
 * Elliptical Gaussian PSF.
 */
int PSFFit::FitGaussian( void* p, int m, int n, const double* a, double* fvec, int iflag )
{
   /*
    *   f(x,y) : B + A*Exp(-(a*(x - x0)^2 + 2*b*(x - x0)*(y - y0) + c*(y - y0)^2))
    */

   if ( B < 0 || A < 0 )
   {
      for ( int i = 0; i < m; ++i )
         *fvec++ = DBL_MAX;
      return 0;
   }

   double st, ct;
   SinCos( theta, st, ct );
   double sct  = st*ct;
   double st2  = st*st;
   double ct2  = ct*ct;
   double ksx2 = 2*sx*sx;
   double ksy2 = 2*sy*sy;
   double p1   = ct2/ksx2 + st2/ksy2;
   double p2   = sct/ksy2 - sct/ksx2;
   double p3   = st2/ksx2 + ct2/ksy2;
   int h = F->S.Rows();
   int w = F->S.Cols();
   double w2x0 = (w >> 1) + x0;
   double h2y0 = (h >> 1) + y0;
   const double* s = F->S.Begin();
   for ( int y = 0; y < h; ++y )
   {
      double dy = y - h2y0;
      double twop2dy = 2*p2*dy;
      double p3dy2 = p3*dy*dy;
      for ( int x = 0; x < w; ++x )
      {
         double dx = x - w2x0;
         *fvec++ = Abs( *s++ - B - A*Exp( -(p1*dx*dx + twop2dy*dx + p3dy2) ) );
      }
   }
   return 0;
}

/*
 * Elliptical Moffat PSF, variable beta exponent.
 */
int PSFFit::FitMoffat( void* p, int m, int n, const double* a, double* fvec, int iflag )
{
   /*
    *   f(x,y) : B + A/(1 + a*(x - x0)^2 + 2*b*(x - x0)*(y - y0) + c*(y - y0)^2)^beta
    */

//std::cout << String().Format( "b = %.2f\n", beta ) << std::flush;

   if ( B < 0 || A < 0 || beta < 0 || beta > 10 || Abs( beta - F->beta0 )/F->beta0 > 0.05 )
   {
      for ( int i = 0; i < m; ++i )
         *fvec++ = DBL_MAX;
      return 0;
   }

   F->beta0 = beta;

   double st, ct;
   SinCos( theta, st, ct );
   double sct = st*ct;
   double st2 = st*st;
   double ct2 = ct*ct;
   double sx2 = sx*sx;
   double sy2 = sy*sy;
   double p1  = ct2/sx2 + st2/sy2;
   double p2  = sct/sy2 - sct/sx2;
   double p3  = st2/sx2 + ct2/sy2;
   int h = F->S.Rows();
   int w = F->S.Cols();
   double w2x0 = (w >> 1) + x0;
   double h2y0 = (h >> 1) + y0;
   const double* s = F->S.Begin();
   for ( int y = 0; y < h; ++y )
   {
      double dy = y - h2y0;
      double twop2dy = 2*p2*dy;
      double p3dy2 = p3*dy*dy;
      for ( int x = 0; x < w; ++x )
      {
         double dx = x - w2x0;
         *fvec++ = Abs( *s++ - B - A/Pow( 1 + p1*dx*dx + twop2dy*dx + p3dy2, beta ) );
      }
   }
   return 0;
}

/*
 * Elliptical Moffat PSF, fixed beta exponent.
 */
#undef beta
#define beta   F->P[7]
int PSFFit::FitMoffatWithFixedBeta( void* p, int m, int n, const double* a, double* fvec, int iflag )
{
   /*
    *   f(x,y) : B + A/(1 + a*(x - x0)^2 + 2*b*(x - x0)*(y - y0) + c*(y - y0)^2)^beta
    */

   if ( B < 0 || A < 0 )
   {
      for ( int i = 0; i < m; ++i )
         *fvec++ = DBL_MAX;
      return 0;
   }

   double st, ct;
   SinCos( theta, st, ct );
   double sct = st*ct;
   double st2 = st*st;
   double ct2 = ct*ct;
   double sx2 = sx*sx;
   double sy2 = sy*sy;
   double p1  = ct2/sx2 + st2/sy2;
   double p2  = sct/sy2 - sct/sx2;
   double p3  = st2/sx2 + ct2/sy2;
   int h = F->S.Rows();
   int w = F->S.Cols();
   double w2x0 = (w >> 1) + x0;
   double h2y0 = (h >> 1) + y0;
   const double* s = F->S.Begin();
   for ( int y = 0; y < h; ++y )
   {
      double dy = y - h2y0;
      double twop2dy = 2*p2*dy;
      double p3dy2 = p3*dy*dy;
      for ( int x = 0; x < w; ++x )
      {
         double dx = x - w2x0;
         *fvec++ = Abs( *s++ - B - A/Pow( 1 + p1*dx*dx + twop2dy*dx + p3dy2, beta ) );
      }
   }
   return 0;
}

#undef F
#undef B
#undef A
#undef x0
#undef y0
#undef sx
#undef sy
#undef theta
#undef beta

// ----------------------------------------------------------------------------
// Circular PSF Functions
// ----------------------------------------------------------------------------

#define F      reinterpret_cast<const PSFFit*>( p )
#define B      a[0]
#define A      a[1]
#define x0     a[2]
#define y0     a[3]
#define sx     a[4]
#define beta   a[5]

/*
 * Circular Gaussian PSF.
 */
int PSFFit::FitCircularGaussian( void* p, int m, int n, const double* a, double* fvec, int iflag )
{
   /*
    *   f(x,y) : B + A*Exp( -((x - x0)^2 + (y - y0)^2)/(2*sigma^2) )
    */

   if ( B < 0 || A < 0 )
   {
      for ( int i = 0; i < m; ++i )
         *fvec++ = DBL_MAX;
      return 0;
   }

   double twosx2 = 2*sx*sx;
   int h = F->S.Rows();
   int w = F->S.Cols();
   double w2x0 = (w >> 1) + x0;
   double h2y0 = (h >> 1) + y0;
   const double* s = F->S.Begin();
   for ( int y = 0; y < h; ++y )
   {
      double dy = y - h2y0;
      double dy2 = dy*dy;
      for ( int x = 0; x < w; ++x )
      {
         double dx = x - w2x0;
         *fvec++ = Abs( *s++ - B - A*Exp( -(dx*dx + dy2)/twosx2 ) );
      }
   }
   return 0;
}

/*
 * Circular Moffat PSF, variable beta exponent.
 */
int PSFFit::FitCircularMoffat( void* p, int m, int n, const double* a, double* fvec, int iflag )
{
   /*
    *   f(x,y) : B + A/(1 + ((x - x0)^2 + (y - y0)^2)/sigma^2)^beta
    */

   if ( B < 0 || A < 0 || beta < 0 || beta > 10 || Abs( beta - F->beta0 )/F->beta0 > 0.05 )
   {
      for ( int i = 0; i < m; ++i )
         *fvec++ = DBL_MAX;
      return 0;
   }

   F->beta0 = beta;

   double sx2 = sx*sx;
   int h = F->S.Rows();
   int w = F->S.Cols();
   double w2x0 = (w >> 1) + x0;
   double h2y0 = (h >> 1) + y0;
   const double* s = F->S.Begin();
   for ( int y = 0; y < h; ++y )
   {
      double dy = y - h2y0;
      double dy2 = dy*dy;
      for ( int x = 0; x < w; ++x )
      {
         double dx = x - w2x0;
         *fvec++ = Abs( *s++ - B - A/Pow( 1 + (dx*dx + dy2)/sx2, beta ) );
      }
   }
   return 0;
}

/*
 * Circular Moffat PSF, fixed beta exponent.
 */
#undef beta
#define beta   F->P[5]
int PSFFit::FitCircularMoffatWithFixedBeta( void* p, int m, int n, const double* a, double* fvec, int iflag )
{
   /*
    *   f(x,y) : B + A/(1 + ((x - x0)^2 + (y - y0)^2)/sigma^2)^beta
    */

   if ( B < 0 || A < 0 )
   {
      for ( int i = 0; i < m; ++i )
         *fvec++ = DBL_MAX;
      return 0;
   }

   double sx2 = sx*sx;
   int h = F->S.Rows();
   int w = F->S.Cols();
   double w2x0 = (w >> 1) + x0;
   double h2y0 = (h >> 1) + y0;
   const double* s = F->S.Begin();
   for ( int y = 0; y < h; ++y )
   {
      double dy = y - h2y0;
      double dy2 = dy*dy;
      for ( int x = 0; x < w; ++x )
      {
         double dx = x - w2x0;
         *fvec++ = Abs( *s++ - B - A/Pow( 1 + (dx*dx + dy2)/sx2, beta ) );
      }
   }
   return 0;
}

#undef F
#undef B
#undef A
#undef x0
#undef y0
#undef sx
#undef beta

/*
 * Mean absolute deviation
 */
double PSFFit::AbsoluteDeviation( Function function, bool circular, double bestSoFar ) const
{
   double adev = 0;
   bool haveBest = bestSoFar > 0;

#define B      P[0]
#define A      P[1]
#define x0     P[2]
#define y0     P[3]
#define sx     P[4]

   int h = S.Rows();
   int w = S.Cols();
   double w2x0 = (w >> 1) + x0;
   double h2y0 = (h >> 1) + y0;
   const double* s = S.Begin();

   if ( circular )
   {
#define beta   P[5]
      switch ( function )
      {
      case Gaussian:
         {
            double twosx2 = 2*sx*sx;
            for ( int y = 0; y < h; ++y )
            {
               double dy = y - h2y0;
               double dy2 = dy*dy;
               for ( int x = 0; x < w; ++x )
               {
                  double dx = x - w2x0;
                  adev += Abs( *s++ - B - A*Exp( -(dx*dx + dy2)/twosx2 ) );
                  if ( haveBest && adev >= bestSoFar )
                     return adev;
               }
            }
         }
         break;

      case Moffat:
      case MoffatA:
      case Moffat8:
      case Moffat6:
      case Moffat4:
      case Moffat25:
      case Moffat15:
      case Lorentzian:
         {
            double sx2 = sx*sx;
            for ( int y = 0; y < h; ++y )
            {
               double dy = y - h2y0;
               double dy2 = dy*dy;
               for ( int x = 0; x < w; ++x )
               {
                  double dx = x - w2x0;
                  adev += Abs( *s++ - B - A/Pow( 1 + (dx*dx + dy2)/sx2, beta ) );
               }
            }
         }
         break;
      }
#undef beta
   }
   else
   {
#define sy     P[5]
#define theta  P[6]
#define beta   P[7]
      switch ( function )
      {
      case Gaussian:
         {
            double st, ct;
            SinCos( theta, st, ct );
            double sct    = st*ct;
            double st2    = st*st;
            double ct2    = ct*ct;
            double twosx2 = 2*sx*sx;
            double twosy2 = 2*sy*sy;
            double p1     = ct2/twosx2 + st2/twosy2;
            double p2     = sct/twosy2 - sct/twosx2;
            double p3     = st2/twosx2 + ct2/twosy2;
            for ( int y = 0; y < h; ++y )
            {
               double dy      = y - h2y0;
               double twop2dy = 2*p2*dy;
               double p3dy2   = p3*dy*dy;
               for ( int x = 0; x < w; ++x )
               {
                  double dx = x - w2x0;
                  adev += Abs( *s++ - B - A*Exp( -(p1*dx*dx + twop2dy*dx + p3dy2) ) );
                  if ( haveBest && adev >= bestSoFar )
                     return adev;
               }
            }
         }
         break;

      case Moffat:
      case MoffatA:
      case Moffat8:
      case Moffat6:
      case Moffat4:
      case Moffat25:
      case Moffat15:
      case Lorentzian:
         {
            double st, ct;
            SinCos( theta, st, ct );
            double sct = st*ct;
            double st2 = st*st;
            double ct2 = ct*ct;
            double sx2 = sx*sx;
            double sy2 = sy*sy;
            double p1  = ct2/sx2 + st2/sy2;
            double p2  = sct/sy2 - sct/sx2;
            double p3  = st2/sx2 + ct2/sy2;
            for ( int y = 0; y < h; ++y )
            {
               double dy      = y - h2y0;
               double twop2dy = 2*p2*dy;
               double p3dy2   = p3*dy*dy;
               for ( int x = 0; x < w; ++x )
               {
                  double dx = x - w2x0;
                  adev += Abs( *s++ - B - A/Pow( 1 + p1*dx*dx + twop2dy*dx + p3dy2, beta ) );
                  if ( haveBest && adev >= bestSoFar )
                     return adev;
               }
            }
         }
         break;
      }
#undef sy
#undef theta
#undef beta
   }

#undef B
#undef A
#undef x0
#undef y0
#undef sx

   return adev;
}

/*
 * Translate CMINPACK information codes into PSF error codes.
 */
PSFFit::Status PSFFit::CminpackInfoToStatus( int info )
{
   switch ( info )
   {
   case 0:  // improper input parameters
      return BadParameters;
   case 1:  // The relative error in the sum of squares is at most tol
   case 2:  // The relative error between x and the solution is at most tol
   case 3:  // Conditions 1 and 2 both hold
      return FittedOk;
   case 4:  // Function vector (fvec) is orthogonal to the columns of the Jacobian to machine precision
      return NoSolution;
   case 5:  // Number of calls to fcn with iflag = 1 has reached 100*(n+1)
      return NoConvergence;
   case 6:  // tol is too small. No further reduction in the sum of squares is possible
   case 7:  // tol is too small. No further improvement in the approximate solution x is possible
      return InaccurateSolution;
   default:
      return UnknownError;
   }
}

// ----------------------------------------------------------------------------

void PSFData::ToImage( Image& img ) const
{



}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PSF.cpp - Released 2017-08-01T14:26:58Z
