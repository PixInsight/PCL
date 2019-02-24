//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/JPLEphemeris.h - Released 2019-01-21T12:06:07Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#ifndef __PCL_JPLEphemeris_h
#define __PCL_JPLEphemeris_h

/// \file pcl/JPLEphemeris.h

#include <pcl/Defs.h>

#include <pcl/Arguments.h>
#include <pcl/EphemerisFile.h>
#include <pcl/ErrorHandler.h>
#include <pcl/KeyValue.h>
#include <pcl/MultiVector.h>
#include <pcl/StringList.h>
#include <pcl/TimePoint.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup solar_system_ephemerides Solar System Ephemerides
 */

/*!
 * \namespace pcl::JPLEphemerisItem
 * \brief     JPL planetary ephemeris items
 *
 * Defines symbolic constants for native and derived JPL planetary ephemeris
 * items. All items defined by DE438t/LE438t are currently supported.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>JPLEphemerisItem::Unknown</td>               <td>Unknown ephemeris item.</td></tr>
 * <tr><td>JPLEphemerisItem::Mercury</td>               <td></td></tr>
 * <tr><td>JPLEphemerisItem::Venus</td>                 <td></td></tr>
 * <tr><td>JPLEphemerisItem::EarthMoonBarycenter</td>   <td></td></tr>
 * <tr><td>JPLEphemerisItem::Mars</td>                  <td></td></tr>
 * <tr><td>JPLEphemerisItem::Jupiter</td>               <td></td></tr>
 * <tr><td>JPLEphemerisItem::Saturn</td>                <td></td></tr>
 * <tr><td>JPLEphemerisItem::Uranus</td>                <td></td></tr>
 * <tr><td>JPLEphemerisItem::Neptune</td>               <td></td></tr>
 * <tr><td>JPLEphemerisItem::Pluto</td>                 <td></td></tr>
 * <tr><td>JPLEphemerisItem::Moon</td>                  <td>Geocentric Moon.</td></tr>
 * <tr><td>JPLEphemerisItem::Sun</td>                   <td></td></tr>
 * <tr><td>JPLEphemerisItem::Nutations</td>             <td>Nutation angles dpsi and deps.</td></tr>
 * <tr><td>JPLEphemerisItem::LunarLibration</td>        <td>Lunar libration angles phi, theta and psi.</td></tr>
 * <tr><td>JPLEphemerisItem::LunarMantleVelocity</td>   <td>Lunar mantle velocity components omega_x, omega_y and omega_z.</td></tr>
 * <tr><td>JPLEphemerisItem::TT_TDB</td>                <td>Difference TT-TDB at the geocenter in seconds</td></tr>
 * <tr><td>JPLEphemerisItem::NumberOfNativeItems</td>   <td>Number of items directly available in JPL DE/LE ASCII files.</td></tr>
 * <tr><td>JPLEphemerisItem::NumberOfRequiredItems</td> <td>Number of required items in a valid JPL DE/LE ASCII file.</td></tr>
 * <tr><td>JPLEphemerisItem::Earth</td>                 <td>Geocenter relative to the solar system barycenter, synthesized from Earth-Moon barycenter and geocentric Moon.</td></tr>
 * <tr><td>JPLEphemerisItem::SSBMoon</td>               <td>Moon relative to the solar system barycenter, synthesized from Earth-Moon barycenter and geocentric Moon.</td></tr>
 * </table>
 *
 * \ingroup solar_system_ephemerides
 */
namespace JPLEphemerisItem
{
   enum value_type
   {
      Unknown = -1,

      // Native JPL DE/LE items, as of 438t
      Mercury = 0,
      Venus,
      EarthMoonBarycenter,
      Mars,
      Jupiter,
      Saturn,
      Uranus,
      Neptune,
      Pluto,
      Moon,
      Sun,
      Nutations,
      LunarLibration,
      LunarMantleVelocity,
      TT_TDB,

      NumberOfNativeItems,
      NumberOfRequiredItems = Sun + 1,

      // Special items synthesized from EMB and Moon
      Earth = 100,  // Geocenter with respect to solar system barycenter
      SSBMoon = 101 // Moon with respect to solar system barycenter
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class JPLEphemeris
 * \brief JPL planetary ephemeris
 *
 * This class implements JPL DE/LE planetary ephemerides computed from original
 * files in ASCII format. On the PixInsight/PCL platform, this is a utility
 * class used to generate truncated binary ephemeris files in XEPH format by
 * reinterpolation with Chebyshev polynomials from the original ephemeris data.
 *
 * Source JPL ephemeris ASCII files, documentation, and original test and
 * utility programs and examples, are available at:
 *
 * ftp://ssd.jpl.nasa.gov/pub/eph/planets/
 *
 * \ingroup solar_system_ephemerides
 *
 * \sa EphemerisFile
 */
class JPLEphemeris
{
public:

   /*!
    * Represents an ephemeris item. See the JPLEphemerisItem namespace for a
    * list of the available items.
    */
   typedef JPLEphemerisItem::value_type   item_index;

   /*!
    * Represents a numerical integration ephemeris constant defined by its
    * name and value.
    */
   typedef EphemerisConstant              constant;

   /*!
    * Represents a list of integration ephemeris constants.
    */
   typedef EphemerisConstantList          constant_list;

   /*!
    * Constructs a &JPLEphemeris instance initialized from the specified DE/LE
    * ephemeris header file in ASCII format.
    *
    * In the event of errors or invalid header data, this constructor throws
    * the appropriate Error exceptions.
    */
   JPLEphemeris( const String& filePath );

   /*!
    * Adds new ephemeris Chebyshev coefficients from the specified JPL DE/LE
    * ephemeris data file in ASCII format.
    *
    * If this is not the first call to this member function for this object,
    * the specified data file must cover a time span contiguous to the existing
    * coefficients, that is, you must call this function successively with a
    * list of consecutive ephemeris data files. For example, for DE/LE 432, you
    * can call this function in sequence for the following file names:
    *
    * ascp01550.432, ascp01650.432, ascp01750.432, ..., ascp02550.432
    *
    * to cover the entire ephemeris time span from JDE 2287184.5 to 2688976.5.
    * You can also call this function to load a smaller time span; for example,
    * calling it with the two file names:
    *
    * ascp02050.432, ascp02150.432
    *
    * will cover a limited range from JDE 2469776.5 to 2542864.5.
    */
   void AddData( const String& filePath );

#ifndef __PCL_NO_JPL_EPHEMERIS_TEST_ROUTINES

   /*!
    * Verifies validity of the calculated ephemerides for a standard JPL DE/LE
    * test file in ASCII format. Returns true iff the test was performed and it
    * was successful.
    *
    * A test is successful if the computed coordinates and components do not
    * differ from the values provided by the test file by more than 1e-13 in
    * absolute value. If this member function returns true, you can be
    * confident that the ephemerides being calculated with this object are
    * correct.
    *
    * The specified file must be a standard "testpo" JPL ephemeris file
    * corresponding to the DE/LE ephemeris loaded by this object. For example,
    * for DE/LE 432, the required test file is "testpo.432".
    *
    * The test is only performed within the time span covered by this object,
    * and unavailable target and center items are ignored. See the AddData()
    * member function for more information.
    *
    * If the \a verbose argument is \c true, the function will output
    * diagnostics information to stdout.
    */
   bool Test( const String& filePath, bool verbose = true ) const;

#endif   // !__PCL_NO_JPL_EPHEMERIS_TEST_ROUTINES

   /*!
    * Returns the DE (Development Ephemeris) number for the data loaded by this
    * object. The returned number is the value of the "DENUM" integration
    * constant.
    */
   int DENumber() const
   {
      return TruncInt( ConstantValue( "DENUM" ) );
   }

   /*!
    * Returns the LE (Lunar Ephemeris) number for the data loaded by this
    * object. The returned number is the value of the "LENUM" integration
    * constant.
    */
   int LENumber() const
   {
      return TruncInt( ConstantValue( "LENUM" ) );
   }

   /*!
    * Returns a Julian date corresponding to the first date covered by the JPL
    * ephemeris for which this object has been initialized.
    *
    * The value returned by this function is for informative purposes only.
    * Note that the ephemeris time span does not have to be the same as the
    * effective time span covered by the data loaded in this object. See the
    * AddData() member function for more information on ephemeris data.
    */
   double EphemerisStartJD() const
   {
      return m_ephStartJD;
   }

   /*!
    * Returns a Julian date corresponding to the last date covered by the JPL
    * ephemeris for which this object has been initialized.
    *
    * The value returned by this function is for informative purposes only.
    * Note that the ephemeris time span does not have to be the same as the
    * effective time span covered by the data loaded in this object. See the
    * AddData() member function for more information on ephemeris data.
    */
   double EphemerisEndJD() const
   {
      return m_ephEndJD;
   }

   /*!
    * Returns a Julian date corresponding to the first date covered by the
    * ephemeris data loaded in this object.
    *
    * The value returned by this function is the smallest time point for which
    * this object can compute ephemerides. The effective ephemeris time span is
    * defined by the data loaded in successive calls to AddData().
    */
   double StartJD() const
   {
      return m_startJD;
   }

   /*!
    * Returns a Julian date corresponding to the last date covered by the
    * ephemeris data loaded in this object.
    *
    * The value returned by this function is the largest time point for which
    * this object can compute ephemerides. The effective ephemeris time span is
    * defined by the data loaded in successive calls to AddData().
    */
   double EndJD() const
   {
      return m_endJD;
   }

   /*!
    * Returns a reference to the list of numerical integration constants used
    * by this DE/LE ephemeris. The integration constants are loaded from an
    * ASCII header file by the class constructor.
    *
    * The returned list is sorted by constant name in ascending order.
    */
   const constant_list& Constants() const
   {
      return m_constants;
   }

   /*!
    * Returns the value of an integration constant given by its \a name.
    *
    * If no integration constant is available with the specified \a name
    * (case-insensitive), this function throws an Error exception.
    */
   double ConstantValue( const IsoString& name ) const
   {
      constant_list::const_iterator i =
      BinarySearch( m_constants.Begin(), m_constants.End(), EphemerisConstant( name ) );
      if ( i == m_constants.End() )
         throw Error( "Undefined integration constant '" + name + '\'' );
      return i->value;
   }

   /*!
    * Returns true if the specified ephemeris item is available in the data
    * loaded by this object. See the JPLEphemerisItem namespace for information
    * on ephemeris items.
    */
   bool IsItemAvailable( int i ) const
   {
      if ( i == JPLEphemerisItem::Earth || i == JPLEphemerisItem::SSBMoon )
         return IsItemAvailable( JPLEphemerisItem::EarthMoonBarycenter ) && IsItemAvailable( JPLEphemerisItem::Moon );
      return i >= 0 && i < JPLEphemerisItem::NumberOfNativeItems && m_blockIndex[i].subblocks > 0;
   }

   /*!
    * Returns the number of components calculated for the specified ephemeris
    * item:
    *
    * 3 for the position and velocity of all major solar system bodies: Sun,
    * Mercury to Pluto, and Moon.
    *
    * 2 for nutation angles.
    *
    * 3 for lunar libration angles and lunar mantle velocity.
    *
    * 1 for the difference TT-TDB.
    */
   static int ComponentsForItem( int i )
   {
      return (i == JPLEphemerisItem::Nutations) ? 2 : (i == JPLEphemerisItem::TT_TDB ? 1 : 3);
   }

   /*!
    * Computes a state vector.
    *
    * \param[out] r     Reference to a vector where the components of the
    *                   computed state will be stored.
    *
    * \param jd1,jd2    The requested time point in the TDB time scale, equal
    *                   to jd1+jd2.
    *
    * \param i          Index of the requested ephemeris item.
    *
    * Rectangular position coordinates are provided in au, except for the
    * geocentric Moon, whose position is provided in km.
    *
    * Angles are provided in radians.
    *
    * TT-TDB differences are provided in seconds.
    */
   void ComputeState( Vector& r, double jd1, double jd2, item_index i ) const
   {
      if ( i == JPLEphemerisItem::Earth )
      {
         Vector rm( 3 );
         Interpolate( r.Begin(), nullptr, jd1, jd2, JPLEphemerisItem::EarthMoonBarycenter );
         Interpolate( rm.Begin(), nullptr, jd1, jd2, JPLEphemerisItem::Moon );
         rm *= m_emb2Earth;
         r -= rm;
      }
      else if ( i == JPLEphemerisItem::SSBMoon )
      {
         Vector re( 3 );
         Interpolate( re.Begin(), nullptr, jd1, jd2, JPLEphemerisItem::EarthMoonBarycenter );
         Interpolate( r.Begin(), nullptr, jd1, jd2, JPLEphemerisItem::Moon );
         r += re - m_emb2Earth*r;
      }
      else
         Interpolate( r.Begin(), nullptr, jd1, jd2, i );

      if ( i != JPLEphemerisItem::Moon )
         if ( i != JPLEphemerisItem::LunarLibration )
            if ( i != JPLEphemerisItem::Nutations )
               r *= m_km2au;
   }

   /*!
    * Computes a state vector and its first derivative.
    *
    * \param[out] r     Reference to a vector where the components of the
    *                   computed state will be stored.
    *
    * \param[out] v     Reference to a vector where the components of the
    *                   computed first derivative will be stored.
    *
    * \param jd1,jd2    The requested time point in the TDB time scale, equal
    *                   to jd1+jd2.
    *
    * \param i          Index of the requested ephemeris item.
    *
    * Rectangular position and velocity coordinates are provided in au and
    * au/day, respectively, except for the geocentric Moon, whose position and
    * velocity are provided in km and km/day, respectively.
    *
    * Angles and their variations are provided in radians and radians/day.
    *
    * TT-TDB differences and their variations are provided in seconds and
    * seconds/day.
    */
   void ComputeState( Vector& r, Vector& v, double jd1, double jd2, item_index i ) const
   {
      if ( i == JPLEphemerisItem::Earth )
      {
         Vector rm( 3 ), vm( 3 );
         Interpolate( r.Begin(), v.Begin(), jd1, jd2, JPLEphemerisItem::EarthMoonBarycenter );
         Interpolate( rm.Begin(), vm.Begin(), jd1, jd2, JPLEphemerisItem::Moon );
         rm *= m_emb2Earth;
         vm *= m_emb2Earth;
         r -= rm;
         v -= vm;
      }
      else if ( i == JPLEphemerisItem::SSBMoon )
      {
         Vector re( 3 ), ve( 3 );
         Interpolate( re.Begin(), ve.Begin(), jd1, jd2, JPLEphemerisItem::EarthMoonBarycenter );
         Interpolate( r.Begin(), v.Begin(), jd1, jd2, JPLEphemerisItem::Moon );
         r += re - m_emb2Earth*r;
         v += ve - m_emb2Earth*v;
      }
      else
         Interpolate( r.Begin(), v.Begin(), jd1, jd2, i );

      if ( i != JPLEphemerisItem::Moon )
         if ( i != JPLEphemerisItem::LunarLibration )
            if ( i != JPLEphemerisItem::Nutations )
            {
               r *= m_km2au;
               v *= m_km2au;
            }
   }

   /*!
    * Computes a state vector for the specified time coordinate \a t.
    *
    * Calling this member function is equivalent to:
    *
    * \code ComputeState( r, t.JDI(), t.JDF(), i ); \endcode
    */
   void ComputeState( Vector& r, TimePoint t, item_index i ) const
   {
      ComputeState( r, t.JDI(), t.JDF(), i );
   }

   /*!
    * Computes a state vector and its first derivative for the specified time
    * coordinate \a t.
    *
    * Calling this member function is equivalent to:
    *
    * \code ComputeState( r, v, t.JDI(), t.JDF(), i ); \endcode
    */
   void ComputeState( Vector& r, Vector& v, TimePoint t, item_index i ) const
   {
      ComputeState( r, v, t.JDI(), t.JDF(), i );
   }

   /*!
    * Generates a summary of the ephemeris loaded by this object, giving
    * information on general ephemeris properties and a list of integration
    * constants.
    *
    * The generated information is organized as a sequence of text lines
    * terminated with UNIX end-of-line characters.
    */
   IsoString Summary() const;

private:

   struct block_index_item
   {
      int offset = 0;            // offset of this ephemeris item in a block
      int coefficients = 0;      // number of coefficients per subblock
      int subblocks = 0;         // number of subblocks
   };

   typedef Array<block_index_item>  block_index;

   typedef MultiVector              block_list;

   double        m_ephStartJD;   // ephemeris start time
   double        m_ephEndJD;     // ephemeris end time
   int           m_blockDelta;   // block time span in days
   constant_list m_constants;    // integration constants
   block_index   m_blockIndex;   // coefficients block index
   block_list    m_blocks;       // coefficients blocks
   double        m_startJD;      // start time of the first coefficients block
   double        m_endJD;        // end time of the last coefficients block
   double        m_km2au;        // astronomical unit in kilometers
   double        m_emb2Earth;    // scale factor for translation Earth-Moon barycenter -> Earth

   /*!
    * \internal
    * Chebyshev polynomial evaluation. Special function adapted to work
    * directly with JPL DE/LE ephemeris data.
    */
   void Interpolate( double* r, double* v, double jd1, double jd2, item_index n ) const
   {
      // block index
      int ix = TruncInt( ((jd1 - m_startJD) + jd2)/m_blockDelta );
      if ( ix < 0 || ix > int( m_blocks.Length() ) )
         throw Error( String().Format( "Time point out of range: %.15g", jd1+jd2 ) );
      if ( ix == int( m_blocks.Length() ) )
         --ix;

      // block coefficients
      const double* block = m_blocks[ix].Begin();

      // block index item
      const block_index_item& index = m_blockIndex[n];

      // block starting time
      double jd0 = *block;

      // number of components
      int nv = ComponentsForItem( n );

      // number of coefficients per subblock
      int nk = index.coefficients;

      // subblock time span in days
      int dx = m_blockDelta/index.subblocks;

      // subblock index
      ix = ((jd1 - jd0) + jd2)/dx;
      if ( ix == index.subblocks )
         --ix;

      // subblock coefficients
      const double* k = block + index.offset + ix*nv*nk;

      // compute state variables
      Vector pc( nk );
      pc[0] = 1;
      pc[1] = (2*((jd1 - (jd0 + ix*dx)) + jd2) - dx)/dx;
      double y2 = 2*pc[1];
      for ( int i = 2; i < nk; ++i )
         pc[i] = y2*pc[i-1] - pc[i-2];
      for ( int i = 0; i < nv; ++i )
      {
         r[i] = 0;
         for ( int j = 0; j < nk; ++j )
            r[i] += pc[j] * *k++;
      }

      // if requested, compute first derivatives.
      if ( v != nullptr )
      {
         double vf = 2.0*index.subblocks/m_blockDelta;
         Vector vc( nk );
         vc[0] = 0;
         vc[1] = 1;
         vc[2] = 2*y2;
         for ( int i = 3; i < nk; ++i )
            vc[i] = y2*vc[i-1] + 2*pc[i-1] - vc[i-2];
         for ( int i = nv; --i >= 0; )
         {
            v[i] = 0;
            for ( int j = nk; --j >= 0; )
               v[i] += vc[j] * *--k;
            v[i] *= vf;
         }
      }
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_JPLEphemeris_h

// ----------------------------------------------------------------------------
// EOF pcl/JPLEphemeris.h - Released 2019-01-21T12:06:07Z
