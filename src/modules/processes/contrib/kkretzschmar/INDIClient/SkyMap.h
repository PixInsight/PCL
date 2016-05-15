//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.07.0141
// ----------------------------------------------------------------------------
// SkyMap.h - Released 2016/04/28 15:13:36 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2016 Klaus Kretzschmar
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

#ifndef __SkyMap_h
#define __SkyMap_h

#include "pcl/Array.h"
#include "pcl/String.h"
#include "pcl/Math.h"
#include "pcl/Graphics.h"

namespace pcl
{

class CoordUtils
{
public:

   struct HMS
   {
      double hour;
      double minute;
      double second;
   };

   static HMS convertToHMS( double coord );
   static HMS parse( String coordStr );
   static double convertFromHMS( const HMS& coord );
   static double convertDegFromHMS( const HMS& coord );
   static double convertRadFromHMS( const HMS& coord );
};

class StereoProjection
{
public:

   struct Rectangular
   {
      double x = 0;
      double y = 0;

      Rectangular() = default;
      Rectangular( const Rectangular& ) = default;

      Rectangular( double _x, double _y ) : x( _x ), y( _y )
      {
      }
   };

   struct Polar
   {
      double r = 0;
      double phi = 0;

      Polar() = default;
      Polar( const Polar& ) = default;

      Polar( double _r, double _phi ) : r( _r ), phi( _phi )
      {
      }

      Rectangular ToRectangular() const
      {
         return Rectangular( r*Sin( -phi ), r*Cos( -phi ) );
      }
   };

   struct Spherical
   {
      double phi = 0;
      double theta = 0;

      Spherical() = default;
      Spherical( const Spherical& ) = default;

      Spherical( double _phi, double _theta ) : phi( _phi ), theta( _theta )
      {
      }

      Polar Projected() const
      {
         return Polar( Sin( theta )/(1 - Cos( theta )), phi );
      }

      Polar Projected( double scale ) const
      {
         return Polar( scale*Sin( theta )/(1 - Cos( theta )), phi );
      }
   };

   Rectangular PolarToRectangular( Polar& p ) const
   {
      return Rectangular( p.r*Sin( -p.phi ), p.r*Cos( -p.phi ) );
   }
};

class SkyMap
{
public:

   struct object
   {
      IsoString mainId;   // main identifier
      double    ra;       // right ascension
      double    dec;      // declination
      double    v;        // visual apparent luminosity
      IsoString spType;   // spectral type
   };

   typedef Array<object> ObjectListType;

   struct filter
   {
      double dec_upperLimit; // upper dec limit of visible object
      double dec_lowerLimit; // lower dec limit of visible object
      double v_upperLimit;   // upper limit of visual apparent luminosity
      //IsoString spType;      // spectral type
   };

   struct geoCoord
   {
      double geo_lat;      // geographical latitude
      double geo_long;     // geographical longitude
   };

   typedef double coord_h; // spherical coordinate in hours, e.g. 21.23412
   typedef double coord_d; // spherical coordinate in degrees, e.g. 342.34678

   SkyMap( const filter& f, const geoCoord& g ) : m_filter( f ), m_geoCoord( g )
   {
   }

   virtual ~SkyMap()
   {
   }

   // get object list of current sky map
   ObjectListType* getObjectList()
   {
      return &m_objectList;
   }

   // get object list of current sky map
   ObjectListType* getFoVObjectList()
   {
      return &m_fovObjectList;
   }

   void addObject( const object& o )
   {
      m_objectList.Add( o );
   }

   void addObjectToFoV( const object& o )
   {
      m_fovObjectList.Add( o );
   }

   void clearFoVObjectList()
   {
      m_fovObjectList.Clear();
   }

   IsoString getASDLQueryString();
   IsoString getASDLFoVQueryString( coord_d ra_center, coord_d dec_center, double width, double height, double limitStarMag );

   static double getObjectAltitude( coord_d dec, coord_d hourAngle, coord_d geo_lat );
   static double getObjectAzimut( coord_d dec, coord_d hourAngle, coord_d geo_lat );
   void plotStars( coord_d lst, coord_d geo_lat, int x, int y, double scale, Graphics& g, double limitStarMag );
   void plotFoVStars( coord_d lst, coord_d geo_lat, int x, int y, double scale, Graphics& g, double limitStarMag );

   static RGBA getStarColor( String spType );

private:

   ObjectListType m_objectList;
   ObjectListType m_fovObjectList;
   filter         m_filter;
   geoCoord       m_geoCoord;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SkyMap_h

// ----------------------------------------------------------------------------
// EOF SkyMap.h - Released 2016/04/28 15:13:36 UTC
