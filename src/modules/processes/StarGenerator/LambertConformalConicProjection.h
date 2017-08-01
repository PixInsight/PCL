//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard StarGenerator Process Module Version 01.01.00.0297
// ----------------------------------------------------------------------------
// LambertConformalConicProjection.h - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard StarGenerator PixInsight module.
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

#ifndef __LambertConformalConicProjection_h
#define __LambertConformalConicProjection_h

#include "ProjectionSystem.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class LambertConformalConicProjection : public ProjectionSystem
{
public:

   LambertConformalConicProjection( double lon, double lat, double lat1, double lat2 ) :
   ProjectionSystem( lon, lat ), sp1( lat1 ), sp2( lat2 )
   {
      if ( 1 + (sp1 + sp2) == 1 )
         throw Error( "LambertConformalConicProjection: standard parallels cannot define a cylinder." );

      double c1 = Cos( sp1 );
      double c2 = Cos( sp2 );
      double t0 = TL2( lat0 );
      double t1 = TL2( sp1 );
      double t2 = TL2( sp2 );

      n = Ln( c1/c2 )/Ln( t2/t1 );
      ni = 1/n;
      sn = Sign( n );
      F = c1 * Pow( t1, n ) / n;
      rho0 = F * Pow( t0, -n );
   }

   virtual ~LambertConformalConicProjection()
   {
   }

   virtual bool IsConicProjection() const
   {
      return true;
   }

   virtual bool IsConformalProjection() const
   {
      return true;
   }

   virtual String Name() const
   {
      return "Lambert Conformal Conic Projection";
   }

   virtual void SphericalToRectangular( double& x, double& y, double lon, double lat ) const
   {
      double ndl = n * InPiRange( lon - lon0 );
      double rho = F * Pow( TL2( lat ), -n );
      x = rho*Sin( ndl );
      y = rho0 - rho*Cos( ndl );
   }

   virtual void RectangularToSpherical( double& lon, double& lat, double x, double y ) const
   {
      double ry = rho0 - y;
      double rho = sn * Sqrt( x*x + ry*ry );
      lon = In2PiRange( lon0 + ArcTan( x, ry )/n );
      lat = 2*ArcTan( Pow( F/rho, ni ) ) - Const<double>::pi2();
   }

   double FirstStandardParallel() const
   {
      return sp1;
   }

   double SecondStandardParallel() const
   {
      return sp2;
   }

private:

   double sp1, sp2;  // standard parallels

   double n, ni;
   int sn;
   double F, rho0;

   static double TL2( double lat )
   {
      return Tan( Const<double>::pi4() + lat/2 );
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __LambertConformalConicProjection_h

// ----------------------------------------------------------------------------
// EOF LambertConformalConicProjection.h - Released 2017-08-01T14:26:58Z
