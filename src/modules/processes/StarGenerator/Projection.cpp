//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard StarGenerator Process Module Version 01.01.00.0322
// ----------------------------------------------------------------------------
// Projection.cpp - Released 2019-01-21T12:06:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard StarGenerator PixInsight module.
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

#include "Projection.h"
#include "StereographicProjection.h"
#include "LambertConformalConicProjection.h"
#include "MercatorCylindricalProjection.h"
#include "GnomonicProjection.h"

namespace pcl
{

// ----------------------------------------------------------------------------

void Projection::Initialize( double lon, double lat, double fov, double w, double h, bool conformal )
{
   if ( system != 0 )
      delete system, system = 0;

   clon = In2PiRange( lon );
   clat = lat;

   radius = fov/2;

   bool isEquatorial = Abs( Deg( clat ) ) <= 20;
   bool isPolar = Abs( Deg( clat ) ) >= 70;

   if ( conformal )
   {
      do
      {
         if ( isEquatorial )
            system = new MercatorCylindricalProjection( clon );
         else if ( isPolar )
            system = new StereographicProjection( clon, clat );
         else
         {
            double sp1 = clat - radius/2;
            double sp2 = clat + radius/2;
            if ( RoundedDegrees( sp1 ) < -80 || RoundedDegrees( sp2 ) > +80 )
               isPolar = true;
            else
               system = new LambertConformalConicProjection( clon, clat, sp1, sp2 );
         }
      }
      while ( system == 0 );
   }
   else
   {
      system = new GnomonicProjection( clon, clat );
   }

   system->SphericalToRectangular( cx, cy, clon, clat );

   width = w;
   height = h;

   double x, y;
   system->SphericalToRectangular( x, y, clon, IsSouth() ? clat - radius : clat + radius );
   scale = height/2/Abs( y - cy );

   if ( IsSouth() )
   {
      bool poleVisible = false;
      if ( !isEquatorial )
      {
         SphericalToRectangular( x, y, clon, -Const<double>::pi2() );
         poleVisible = y >= -height/2;
      }

      if ( !poleVisible )
      {
         RectangularToSpherical( lon, slat, 0, -height/2 );
         if ( RoundedDegrees( slat ) == -90 )
            poleVisible = true;
      }

      RectangularToSpherical( lon, nlat, -width/2, +height/2 );

      if ( poleVisible )
      {
         slat = -Const<double>::pi2();
         elon = 0;
         wlon = Const<double>::_2pi() - 1e-10;
      }
      else
      {
         RectangularToSpherical( elon, lat, -width/2, -height/2 );
         RectangularToSpherical( wlon, lat, +width/2, -height/2 );
         elon = In2PiRange( elon );
         wlon = In2PiRange( wlon );
      }
   }
   else
   {
      bool poleVisible = false;
      if ( !isEquatorial )
      {
         SphericalToRectangular( x, y, clon, Const<double>::pi2() );
         poleVisible = y <= height/2;
      }

      if ( !poleVisible )
      {
         RectangularToSpherical( lon, nlat, 0, +height/2 );
         if ( RoundedDegrees( nlat ) == +90 )
            poleVisible = true;
      }

      RectangularToSpherical( lon, slat, -width/2, -height/2 );

      if ( poleVisible )
      {
         nlat = Const<double>::pi2();
         elon = 0;
         wlon = Const<double>::_2pi() - 1e-10;
      }
      else
      {
         RectangularToSpherical( elon, lat, -width/2, +height/2 );
         RectangularToSpherical( wlon, lat, +width/2, +height/2 );
         elon = In2PiRange( elon );
         wlon = In2PiRange( wlon );
      }
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF Projection.cpp - Released 2019-01-21T12:06:42Z
