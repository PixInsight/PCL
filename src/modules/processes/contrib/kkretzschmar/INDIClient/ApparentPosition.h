//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.15.0205
// ----------------------------------------------------------------------------
// ApparentPosition.h - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2017 Klaus Kretzschmar
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

#ifndef __ApparentPosition_h
#define __ApparentPosition_h

#include <pcl/Matrix.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Apparent positions with corrections for:
 *
 * - Proper motions
 * - Stellar aberration
 * - Precession
 * - Nutation
 * - Frame bias
 *
 * Implementation of the IAU 2006 Precession/Nutation model.
 *
 * We neglect (for now) the effects of parallax / radial velocity.
 *
 * Adapted from:
 *
 *    IAU−SOFA SOFTWARE LIBRARIES
 *    Copyright (C) 2016
 *    Standards Of Fundamental Astronomy Board
 *    of the International Astronomical Union.
 *    http://www.iausofa.org/
 *
 * Written by Juan Conejero (PTeam)
 */
class ApparentPosition
{
public:

   ApparentPosition( double jd1, double jd2 = 0 );

   /*
    * ra       Right Ascension, radians
    *
    * dec      Declination, radians
    *
    * muRA     Proper motion in R.A. = mu_alpha*cos(delta), mas/year
    *
    * muDec    Proper motion in Dec., mas/year
    *
    * inverse  if true, apply the inverse transformation to get mean position,
    *          ignoring proper motions. If false, compute apparent position.
    */
   void Apply( double& ra, double& dec, double muRA = 0, double muDec = 0, bool inverse = false ) const;

   void ApplyInverse( double& ra, double& dec ) const
   {
      Apply( ra, dec, 0, 0, true/*inverse*/ );
   }

   /*
    * Approximate DeltaT in days for the specified year. Adapted from:
    * http://eclipse.gsfc.nasa.gov/SEcat5/deltatpoly.html
    */
   static double DeltaT( int year, int month );

private:

   // Time in Julian years since J2000.0
   double t;
   // Time in Julian centuries since J2000.0
   double T;
   // Bias+precession angles
   double gamb, phib, psib, epsa;
   // Nutation angles
   double dpsi, deps;
   // Precession * nutation * frame bias matrix
   Matrix r, rinv;
   // Barycentric and Heliocentric position and velocity of Earth (in AU and
   // AU/day, resp.)
   Vector pb, vb, ph, vh;

   void Time( double jd1, double jd2 );
   void Precession();
   void Nutation();
   void EarthPosVel();
   void Aberration( Vector&, bool inverse ) const;

   /*
    * Assignment matrix-vector product.
    */
   static void MatrixVectorMulAssign( const Matrix& r, Vector& p )
   {
      double x = p[0];
      double y = p[1];
      double z = p[2];
      p[0] = x*r[0][0] + y*r[0][1] + z*r[0][2];
      p[1] = x*r[1][0] + y*r[1][1] + z*r[1][2];
      p[2] = x*r[2][0] + y*r[2][1] + z*r[2][2];
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ApparentPosition_h

// ----------------------------------------------------------------------------
// EOF ApparentPosition.h - Released 2017-07-09T18:07:33Z
