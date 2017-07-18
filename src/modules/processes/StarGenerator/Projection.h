//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard StarGenerator Process Module Version 01.01.00.0289
// ----------------------------------------------------------------------------
// Projection.h - Released 2017-07-18T16:14:19Z
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

#ifndef __Projection_h
#define __Projection_h

#include "ProjectionSystem.h"

#include <pcl/Rectangle.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class Projection
{
public:

   Projection() : system( 0 )
   {
   }

   virtual ~Projection()
   {
      if ( system != 0 )
         delete system, system = 0;
   }

   const ProjectionSystem& System() const
   {
      return *system;
   }

   bool IsSouth() const
   {
      return clat < 0;
   }

   bool IsNorth() const
   {
      return !IsSouth();
   }

   double CenterLongitude() const
   {
      return clon;
   }

   double CenterLatitude() const
   {
      return clat;
   }

   double CenterX() const
   {
      return cx;
   }

   double CenterY() const
   {
      return cy;
   }

   double FieldOfView() const
   {
      return 2*radius;
   }

   double Width() const
   {
      return width;
   }

   double Height() const
   {
      return height;
   }

   double Scale() const
   {
      return scale;
   }

   double EastLongitude() const
   {
      return elon;
   }

   double WestLongitude() const
   {
      return wlon;
   }

   double NorthLatitude() const
   {
      return nlat;
   }

   double SouthLatitude() const
   {
      return slat;
   }

   virtual void Initialize( double lon, double lat, double fov, double w, double h, bool conformal );

   void SphericalToRectangular( double& x, double& y, double lon, double lat ) const
   {
      system->SphericalToRectangular( x, y, lon, lat );
      x = scale*(x - cx);
      y = scale*(y - cy);
   }

   void RectangularToSpherical( double& lon, double& lat, double x, double y ) const
   {
      system->RectangularToSpherical( lon, lat, x/scale + cx, y/scale + cy );
   }

   double AngularDistanceToPlane( double d ) const
   {
      double d2 = Abs( d )/2;
      double x, y1, y2;
      if ( IsSouth() )
      {
         SphericalToRectangular( x, y2, clon, clat + d/2 );

         if ( Deg( clat - d2 ) >= -90 )
            SphericalToRectangular( x, y1, clon, clat - d/2 );
         else
            y1 = cy;
      }
      else
      {
         SphericalToRectangular( x, y1, clon, clat - d/2 );

         if ( Deg( clat + d2 ) <= 90 )
            SphericalToRectangular( x, y2, clon, clat + d/2 );
         else
            y2 = cy;
      }

      return Abs( y2 - y1 );
   }

   DRect MapBounds() const
   {
      return DRect( -width/2, -height/2, width/2, height/2 );
   }

   bool IntersectsSector( int i, int j ) const
   {
      DRect r;
      SphericalToRectangular( r.x0, r.y0, Rad( double( j ) ), Rad( double( i ) ) );
      SphericalToRectangular( r.x1, r.y1, Rad( double( j+1 ) - 1e-10 ), Rad( double( i+1 ) ) );
      r.Order();
      r.x0 -= 1e-07; r.y0 -= 1e-07;
      r.x1 += 1e-07; r.y1 += 1e-07;
      return MapBounds().Intersects( r );
   }

protected:

   ProjectionSystem* system;
   double            clon, clat; // map center, spherical coordinates
   double            cx, cy;     // map center, rectangular coordinates
   double            radius;     // half FOV
   double            width;      // map width in map units
   double            height;     // map height in map units
   double            scale;      // map scale
   double            elon, wlon; // longitude range, elon=east(left), wlon=west(right)
   double            slat, nlat; // latitude range, slat=south(bottom), nlat=north(top)

   static double RoundedDegrees( double x )
   {
      return Round( Deg( x ), 7 ); // rounded to about 0".01
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __Projection_h

// ----------------------------------------------------------------------------
// EOF Projection.h - Released 2017-07-18T16:14:19Z
