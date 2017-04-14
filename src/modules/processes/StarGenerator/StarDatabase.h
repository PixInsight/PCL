//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard StarGenerator Process Module Version 01.01.00.0256
// ----------------------------------------------------------------------------
// StarDatabase.h - Released 2017-04-14T23:07:12Z
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

#ifndef __StarDatabase_h
#define __StarDatabase_h

#include <pcl/ByteArray.h>
#include <pcl/File.h>
#include <pcl/Math.h>
#include <pcl/Matrix.h>
#include <pcl/TimePoint.h>

//
// PPMX extreme values
//
#define MA_MIN   -0.1227267
#define MA_MAX   +0.1879620
#define MD_MIN   -0.1615297
#define MD_MAX   +0.2868889
#define MAG_MIN  -1.50
#define MAG_MAX +17.19

namespace pcl
{

// ----------------------------------------------------------------------------

class Star
{
public:

   double ra;  // Right ascension, degrees, epoch = equinox = J2000
   double dec; // Declination, degrees, epoch = equinox = J2000
   double ma;  // Proper motion in ra * cos( dec ), degrees/100yr
   double md;  // Proper motion in dec, degrees/100yr
   float  mag; // Catalog magnitude

   Star( const ByteArray& b, int i, int j, int k )
   {
      k *= 14;
      ra  = j       + 1e-07*(b[k   ] | (b[k+ 1] << 8) | (b[k+ 2] << 16));
      dec = i       + 1e-07*(b[k+ 3] | (b[k+ 4] << 8) | (b[k+ 5] << 16)) - 90;
      ma  = MA_MIN  + 1e-07*(b[k+ 6] | (b[k+ 7] << 8) | (b[k+ 8] << 16));
      md  = MD_MIN  + 1e-07*(b[k+ 9] | (b[k+10] << 8) | (b[k+11] << 16));
      mag = MAG_MIN + 1e-02*(b[k+12] | (b[k+13] << 8) );
   }

   Star( const Star& s ) : ra( s.ra ), dec( s.dec ), ma( s.ma ), md( s.md ), mag( s.mag )
   {
   }

   void SetEpoch( double t )
   {
      double T = TimePoint( t ).CenturiesSinceJ2000();

      double cd = Cos( Rad( dec ) );
      if ( cd > 1e-10 )
      {
         ra += T*ma/cd;
         if ( ra < 0 )
            ra += 360;
         else if ( ra >= 360 )
            ra -= 360;
      }

      dec += T*md;
      if ( dec < -90 )
         dec = -180 - dec;
      else if ( dec > +90 )
         dec = 180 - dec;
   }
};

class StarDatabase
{
public:

   typedef Array<Star>  star_list;

   StarDatabase( const String& fileName = String(), TimePoint ep = TimePoint::J2000() ) :
      epoch( ep.JD() )
   {
      if ( !fileName.IsEmpty() )
         Open( fileName );
   }

   virtual ~StarDatabase()
   {
      Close();
   }

   bool IsOpen() const
   {
      return file.IsOpen();
   }

   String FileName() const
   {
      return file.FileName();
   }

   double Epoch() const
   {
      return epoch;
   }

   void SetEpoch( double jd )
   {
      epoch = jd;
   }

   void Open( const String& fileName )
   {
      Close();

      file.OpenForReading( fileName );

      if ( index.IsEmpty() )
         index = index_matrix( 180, 360 );

      for ( int i = 0; i < 180; ++i )
         for ( int j = 0; j < 360; ++j )
         {
            uint16 n; file.Read( n );
            index[i][j].count = n;
            uint32 p; file.Read( p );
            index[i][j].position = p;
         }
   }

   void Close()
   {
      file.Close();
   }

   star_list ReadSectors( int i, int j1, int j2, float mmax = int_max )
   {
      star_list stars;

      if ( IsOpen() )
      {
         i += 90;
         if ( i >= 0 && i < 180 )
         {
            for ( int j = j1; j <= j2; ++j )
               if ( j >= 0 && j < 360 )
               {
                  const IndexNode& n = index[i][j];

                  if ( n.count > 0 )
                  {
                     file.SetPosition( n.position );

                     ByteArray b( n.count*14 );
                     file.Read( b.Begin(), n.count*14 );

                     for ( int k = 0; k < n.count; ++k )
                     {
                        Star s( b, i, j, k );
                        if ( s.mag <= mmax )
                           stars.Add( s );
                     }
                  }
               }

            if ( epoch != TimePoint::J2000() )
               for ( star_list::iterator i = stars.Begin(); i != stars.End(); ++i )
                  i->SetEpoch( epoch );
         }
      }

      return stars;
   }

private:

   struct IndexNode
   {
      int       count;
      fpos_type position;
   };

   typedef GenericMatrix<IndexNode> index_matrix;

   index_matrix index;
   File         file;
   double       epoch;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __StarDatabase_h

// ----------------------------------------------------------------------------
// EOF StarDatabase.h - Released 2017-04-14T23:07:12Z
