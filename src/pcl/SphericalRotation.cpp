//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/SphericalRotation.cpp - Released 2018-11-01T11:06:52Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/Exception.h>
#include <pcl/SphericalRotation.h>

/*
 * Based on original work contributed by Andrés del Pozo.
 */

namespace pcl
{

// ----------------------------------------------------------------------------

SphericalRotation::SphericalRotation( double lng0, double lat0, double phi0, double theta0, double phip,
                                      const Optional<double>& latpole )
{
   double lngp;
   double latp = latpole.OrElse( 90 );
   int latpreq = 0;

   if ( theta0 == 90 )
   {
      // Fiducial point at the native pole.
      lngp = lng0;
      latp = lat0;
   }
   else
   {
      // Fiducial point away from the native pole.
      double slat0, clat0;
      SinCos( Rad( lat0 ), slat0, clat0 );
      double sthe0, cthe0;
      SinCos( Rad( theta0 ), sthe0, cthe0 );

      double sphip, cphip;
      double u, v;
      if ( phip == phi0 )
      {
         sphip = 0;
         cphip = 1;
         u = theta0;
         v = 90 - lat0;
      }
      else
      {
         SinCos( Rad( phip - phi0 ), sphip, cphip );

         double x = cthe0 * cphip;
         double y = sthe0;
         double z = Sqrt( x*x + y*y );
         if ( z == 0 )
         {
            if ( slat0 != 0 )
               throw Error( "Invalid WCS coordinates: nlat0 == 0 is required for |phip - phi0| == 90 and theta0 == 0" );

            // latp determined solely by LATPOLEa in this case.
            latpreq = 2;
            if ( latp > 90 )
               latp = 90;
            else if ( latp < -90 )
               latp = -90;
         }
         else
         {
            double slz = slat0/z;
            if ( Abs( slz ) > 1 )
            {
               if ( (Abs( slz ) - 1) > Tolerance )
                  throw Error( "Invalid WCS coordinates: |lat0| <= ArcSin( z ) is required for these values of phip, phi0, and theta0" );
               slz = (slz > 0) ? 1 : -1;
            }

            u = Deg( ArcTan( y, x ) );
            v = Deg( ArcCos( slz ) );
         }
      }

      if ( latpreq == 0 )
      {
         double latp1 = u + v;
         if ( latp1 > 180 )
            latp1 -= 360;
         else if ( latp1 < -180 )
            latp1 += 360;

         double latp2 = u - v;
         if ( latp2 > 180 )
            latp2 -= 360;
         else if ( latp2 < -180 )
            latp2 += 360;

         if ( Abs( latp1 ) < 90 + Tolerance )
            if ( Abs( latp2 ) < 90 + Tolerance )
               latpreq = 1; // there are two valid solutions for latp

         latp = (Abs( latp - latp1 ) < Abs( latp - latp2 )) ?
                  ((Abs( latp1 ) < 90 + Tolerance) ? latp1 : latp2) :
                  ((Abs( latp2 ) < 90 + Tolerance) ? latp2 : latp1);

         // Account for rounding errors.
         if ( Abs( latp ) > 90 + Tolerance )
            latp = Range( latp, -90.0, +90.0 );
      }

      double z = Cos( Rad( latp ) )*clat0;
      if ( Abs( z ) < Tolerance )
      {
         if ( Abs( clat0 ) < Tolerance ) // Celestial pole at the fiducial point
            lngp = lng0;
         else if ( latp > 0 )            // Celestial North pole at the native pole
            lngp = lng0 + phip - phi0 - 180;
         else                            // Celestial South pole at the native pole
            lngp = lng0 - phip + phi0;
      }
      else
      {
         double x = (sthe0 - Sin( Rad( latp ) )*slat0)/z;
         double y = sphip*cthe0/clat0;
         if ( x == 0 )
            if ( y == 0 ) // sanity check (shouldn't be possible).
               throw Error( "Internal error: SphericalRotation::SphericalRotation(): Invalid WCS coordinates" );
         lngp = lng0 - Deg( ArcTan( y, x ) );
      }

      // Make celestial longitude of the native pole the same sign as at the fiducial point.
      if ( lng0 >= 0 )
      {
         if ( lngp < 0 )
            lngp += 360;
         else if ( lngp > 360 )
            lngp -= 360;
      }
      else
      {
         if ( lngp > 0 )
            lngp -= 360;
         else if ( lngp < -360 )
            lngp += 360;
      }
   }

   m_latpole = latp;
   m_alphaP = lngp;
   m_deltaP = 90 - latp;
   m_phiP = phip;
   SinCos( Rad( m_deltaP ), m_sindeltaP, m_cosdeltaP );
}

// ----------------------------------------------------------------------------

inline static double Adjust360( double x, double min, double max )
{
   while ( x >= max )
      x -= 360;
   while ( x < min )
      x += 360;
   return x;
}

// ----------------------------------------------------------------------------

DPoint SphericalRotation::NativeToCelestial( const DPoint& np ) const
{
   DPoint cp;
   if ( m_sindeltaP == 0 )
   {
      if ( m_deltaP == 0 )
      {
         double dlng = Adjust360( m_alphaP + 180 - m_phiP, 0, 360 );
         cp.x = np.x + dlng;
         cp.y = np.y;
      }
      else
      {
         double dlng = Adjust360( m_alphaP + m_phiP, 0, 360 );
         cp.x = dlng - np.x;
         cp.y = -np.y;
      }
   }
   else
   {
      cp.x = np.x - m_phiP;

      double sinthe, costhe;
      SinCos( Rad( np.y ), sinthe, costhe );
      double costhe3 = costhe * m_cosdeltaP;

      double dphi = cp.x;
      double sinphi, cosphi;
      SinCos( Rad( dphi ), sinphi, cosphi );

      // Compute the celestial longitude.
      double x = sinthe*m_sindeltaP - costhe3*cosphi;
      if ( Abs( x ) < Tolerance ) // rearrange formula to reduce roundoff errors
         x = -Cos( Rad( np.y + m_deltaP ) ) + costhe3*(1 - cosphi);

      double y = -costhe * sinphi;
      double dlng = (Abs( x ) > Tolerance || Abs( y ) > Tolerance) ?
                        Deg( ArcTan( y, x ) ) :
                        ((m_deltaP < 90) ? dphi + 180 : -dphi); // change of the origin of longitude
      cp.x = m_alphaP + dlng;

      // Compute the celestial latitude.
      if ( Mod( dphi, 180.0 ) == 0 )
      {
         cp.y = np.y + cosphi*m_deltaP;
         if ( cp.y > 90 )
            cp.y = 180 - cp.y;
         if ( cp.y < -90 )
            cp.y = -180 - cp.y;
      }
      else
      {
         double z = sinthe*m_cosdeltaP + costhe*m_sindeltaP*cosphi;
         if ( Abs( z ) > 0.99 )
         {
            // Use an alternative formula for greater accuracy.
            cp.y = Deg( ArcCos( Sqrt( x*x + y*y ) ) );
            if ( cp.y*z < 0 )
               cp.y = -cp.y;
         }
         else
            cp.y = Deg( ArcSin( z ) );
      }
   }

   // Normalize the celestial longitude.
   if ( m_alphaP >= 0 )
   {
      if ( cp.x < 0 )
         cp.x += 360;
   }
   else
   {
      if ( cp.x > 0 )
         cp.x -= 360;
   }
   cp.x = Adjust360( cp.x, -360, 360 );

   return cp;
}

// ----------------------------------------------------------------------------

DPoint SphericalRotation::CelestialToNative( const DPoint& cp ) const
{
   // Check for a simple change in origin of longitude.
   DPoint np;
   double dphi;
   if ( m_sindeltaP == 0 )
   {
      if ( m_deltaP == 0 )
      {
         dphi = Adjust360( m_phiP - 180 - m_alphaP, 0, 360 );
         np.x = Adjust360( cp.x + dphi, -180, 180 );
         np.y = cp.y;
      }
      else
      {
         dphi = Adjust360( m_phiP + m_alphaP, 0, 360 );
         np.x = Adjust360( dphi - cp.x, -180, 180 );
         np.y = -cp.y;
      }
   }
   else
   {
      np.x = cp.x - m_alphaP;

      double sinlat, coslat;
      SinCos( Rad( cp.y ), sinlat, coslat );
      double coslat3 = coslat * m_cosdeltaP;

      double dlng = np.x;
      double sinlng, coslng;
      SinCos( Rad( dlng ), sinlng, coslng );

      // Compute the native longitude.
      double x = sinlat*m_sindeltaP - coslat3*coslng;
      if ( Abs( x ) < Tolerance ) // rearrange formula to reduce roundoff errors
         x = -Cos( Rad( cp.y + m_deltaP ) ) + coslat3*(1 - coslng);

      double y = -coslat * sinlng;
      dphi = (x != 0 || y != 0) ?
                     Deg( ArcTan( y, x ) ) :
                     ((m_deltaP < 90) ? dlng - 180 : -dlng);

      np.x = Adjust360( m_phiP + dphi, -180, 180 );

      // Compute the native latitude.
      if ( Mod( dlng, 180.0 ) == 0 )
      {
         np.y = cp.y + coslng * m_deltaP;
         if ( np.y > 90 )
            np.y = 180 - np.y;
         if ( np.y < -90 )
            np.y = -180 - np.y;
      }
      else
      {
         double z = sinlat*m_cosdeltaP + coslat*m_sindeltaP*coslng;
         if ( Abs( z ) > 0.99 )
         {
            // Use an alternative formula for greater accuracy.
            np.y = Deg( ArcCos( Sqrt( x*x + y*y ) ) );
            if ( np.y*z < 0 )
               np.y = -np.y;
         }
         else
            np.y = Deg( ArcSin( z ) );
      }
   }

   return np;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/SphericalRotation.cpp - Released 2018-11-01T11:06:52Z
