//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/Position.h - Released 2018-11-01T11:06:36Z
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

#ifndef __PCL_Position_h
#define __PCL_Position_h

/// \file pcl/Position.h

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/EphemerisFile.h>
#include <pcl/Matrix.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \struct StarPosition
 * \brief Positional data of a star.
 *
 * This structure provides the data necessary for reduction of star positions
 * with the following member functions:
 *
 * Position::Geometric( const StarPosition& ) \n
 * Position::Astrometric( const StarPosition& ) \n
 * Position::Proper( const StarPosition& ) \n
 * Position::Apparent( const StarPosition& ) \n
 * Position::Intermediate( const StarPosition& )
 *
 * \ingroup solar_system_ephemerides
 */
struct PCL_CLASS StarPosition
{
   double    alpha = 0;    //!< ICRS right ascension in hours.
   double    delta = 0;    //!< ICRS declination in degrees.
   double    muAlpha = 0;  //!< Proper motion in right ascension, mas/year * cos( delta ).
   double    muDelta = 0;  //!< Proper motion in declination, in mas/year.
   double    p = 0;        //!< Parallax in arcseconds.
   double    v = 0;        //!< Radial velocity in km/s, positive away from Earth.
   TimePoint t0 = TimePoint::J2000(); //!< Epoch of coordinates.

   /*!
    * Default constructor. See data members for default values.
    */
   StarPosition() = default;

   /*!
    * Copy constructor.
    */
   StarPosition( const StarPosition& ) = default;

   /*!
    * Copy assignment operator.
    */
   StarPosition& operator =( const StarPosition& ) = default;

   /*!
    * Memberwise constructor.
    *
    * \param ra               Right ascension equatorial coordinate in hours.
    *
    * \param dec              Declination equatorial coordinate in degrees.
    *
    * \param properMotionRA   Proper motion in right ascension, in mas/year,
    *                         measured on a great circle, that is, the
    *                         specified value must be the proper motion in R.A.
    *                         multiplied by the cosine of the declination. Zero
    *                         by default.
    *
    * \param properMotionDec  Proper motion in declination, in mas/year. Zero
    *                         by default.
    *
    * \param parallax         Parallax in arcseconds. Zero by default.
    *
    * \param radialVelocity   Radial velocity in km/s, positive away from the
    *                         Earth. Zero by default.
    *
    * \param epoch            Epoch of catalog coordinates. J2000.0 by default.
    *
    * Positions and proper motions must be referred to ICRS/J2000.0.
    *
    * If out-of-range coordinates are specified, their values will be
    * constrained to their proper ranges: right ascension to [0h,24h) and
    * declination to [-90&deg;,+90&deg;].
    */
   StarPosition( double ra, double dec,
                 double properMotionRA = 0, double properMotionDec = 0,
                 double parallax = 0,
                 double radialVelocity = 0,
                 TimePoint epoch = TimePoint::J2000() ) :
      alpha( ((ra = Mod( ra, 24.0 )) < 0) ? ra + 24 : ra ),
      delta( Range( dec, -90.0, +90.0 ) ),
      muAlpha( properMotionRA ),
      muDelta( properMotionDec ),
      p( parallax ),
      v( radialVelocity ),
      t0( epoch )
   {
   }
};

// ----------------------------------------------------------------------------

/*!
 * \struct ObserverPosition
 * \brief Geodetic coordinates of a terrestrial observer.
 *
 * This structure provides the data necessary to calculate topocentric places
 * of solar system bodies and stars.
 *
 * \ingroup solar_system_ephemerides
 */
struct PCL_CLASS ObserverPosition
{
   double lambda = 0;             //!< Geodetic longitude in degrees.
   double phi = 0;                //!< Geodetic latitude in degrees.
   double h = 0;                  //!< Geodetic height in meters.
   double a = ea_eq_radius_IAU2009;   //!< Equatorial radius of the reference spheroid in meters.
   double f = ea_flattening_IERS2010; //!< Flattening of the reference spheroid.
   Vector r0 = Vector( 0, 0, 0 ); //!< Geocentric rectangular coordinates of the center of the regional spheroid, in meters.
   bool   cioBased = false;       //!< Whether to compute CIO-based or equinox-based topocentric coordinates.

   /*!
    * Equatorial radius of Earth in meters, IAU 2009/2012.
    */
   constexpr static double ea_eq_radius_IAU2009 = 6378136.6;

   /*!
    * Flattening of Earth, IERS 2010.
    */
   constexpr static double ea_flattening_IERS2010 = 1/298.25642;

   /*!
    * Default constructor.
    *
    * Constructs an %ObserverPosition object for a fictitious observer at the
    * geocenter. See data members for specific default values.
    */
   ObserverPosition() = default;

   /*!
    * Copy constructor.
    */
   ObserverPosition( const ObserverPosition& ) = default;

   /*!
    * Move constructor.
    */
   ObserverPosition( ObserverPosition&& ) = default;

   /*!
    * Copy assignment operator.
    */
   ObserverPosition& operator =( const ObserverPosition& ) = default;

   /*!
    * Move assignment operator.
    */
   ObserverPosition& operator =( ObserverPosition&& ) = default;

   /*!
    * Memberwise constructor.
    *
    * \param longitude        Geodetic longitude in degrees.
    *
    * \param latitude         Geodetic latitude in degrees.
    *
    * \param height           Geodetic height in meters. Zero by default.
    *
    * \param equatorialRadius Equatorial radius of the reference spheroid in
    *                         meters. The default value is the IAU 2009/2012
    *                         equatorial radius of Earth: 6,378,136.6 meters.
    *
    * \param flattening       Flattening of the reference spheroid. The default
    *                         value is the IERS 2010 flattening of Earth:
    *                         1/298.25642.
    *
    * \param regionalCenter   Geocentric rectangular coordinates of the center
    *                         of the regional spheroid, in meters. A zero 3-D
    *                         vector (that is, the geocenter) by default.
    *
    * \param useCIO           Whether to compute CIO-based or equinox-based
    *                         topocentric coordinates. False (equinox-based) by
    *                         default.
    *
    * Typically, the values used should be WGS84 coordinates (for example, as
    * distributed by GPS) or ITRF coordinates (both systems close together at
    * the level of a few centimeters).
    *
    * Geographic longitudes grow eastward, so longitudes are positive to the
    * east and negative to the west of the central meridian. For ease of
    * calculation, this constructor converts longitudes to the
    * [0&deg;,360&deg;) range.
    *
    * If out-of-range coordinates are specified, their values will be
    * constrained to their proper ranges: longitude to [0&deg;,360&deg;), and
    * latitude to [-90&deg;,+90&deg;].
    */
   ObserverPosition( double longitude, double latitude,
                     double height = 0,
                     double equatorialRadius = ea_eq_radius_IAU2009,
                     double flattening = ea_flattening_IERS2010,
                     const Vector& regionalCenter = Vector( 0, 0, 0 ),
                     bool useCIO = false ) :
      lambda( ((longitude = Mod( longitude, 360.0 )) < 0) ? longitude + 360 : longitude ),
      phi( Range( latitude, -90.0, +90.0 ) ),
      h( Max( 0.0, height ) ),
      a( Max( 0.0, equatorialRadius ) ),
      f( Max( 0.0, flattening ) ),
      r0( regionalCenter ),
      cioBased( useCIO )
   {
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class Position
 * \brief Reduction of planetary and stellar positions.
 *
 * This class implements algorithms for reduction of positions of solar system
 * bodies and stars. It allows for calculation of geometric, astrometric,
 * proper, apparent and intermediate places, including geocentric and
 * topocentric coordinates.
 *
 * The implemented vector astrometry and ephemeris calculation algorithms are
 * rigorous and compliant with current IAU and IERS resolutions. Both
 * equinox-based and CIO-based paradigms have been implemented for calculation
 * of positions that depend on Earth's rotation. The apparent and intermediate
 * places include the following corrections:
 *
 * \li Light-travel time for solar system bodies.
 *
 * \li Space motion for stars, including parallax, radial velocity and proper
 * motions, with corrections for the relativistic Doppler effect that takes
 * into account the change in light-travel time for stars.
 *
 * \li Relativistic deflection of light due to solar gravitation.
 *
 * \li Aberration of light, relativistic model.
 *
 * \li %Frame bias, precession and nutation. (equinox-based and CIO-based).
 *
 * \li Accurate topocentric places with polar motion corrections.
 *
 * Vector components are expressed in astronomical units (au) for stars and all
 * solar system bodies except the Moon, for which positions are given in
 * kilometers.
 *
 * As of writing this documentation (October 2018), the IAU 2006/2000A
 * precession-nutation theory is implemented (adjusted model with corrections
 * to nutation angles, IAU 2006/2000A<sub>R</sub>). The standard fundamental
 * ephemerides are JPL's DE438/LE438.
 *
 * Most of the reference publications and material used are cited in source
 * code comments and the documentation. The main authoritative resource is:
 *
 * Urban, Sean E., Kenneth Seidelmann, P., ed. (2013), <em>The Explanatory
 * Supplement to the Astronomical Almanac</em> 3rd Edition.
 *
 * \ingroup solar_system_ephemerides
 */
class PCL_CLASS Position
{
public:

   /*!
    * Constructs a %Position object initialized for the specified time of
    * calculation \a t in the specified \a timescale.
    *
    * The supported time scales are:
    *
    * <table border="1" cellpadding="4" cellspacing="0">
    * <tr><td>\b TT</td>   <td>Terrestrial Time. This is the default
    *                      timescale.</td></tr>
    *
    * <tr><td>\b TDB</td>  <td>Barycentric Dynamical Time.</td></tr>
    *
    * <tr><td>\b Teph</td> <td>Ephemeris time, as defined by JPL DE/LE
    *                      numerical integrations. For all purposes, this is
    *                      equivalent to TDB.</td></tr>
    *
    * <tr><td>\b UTC</td>  <td>Coordinated Universal Time.</td></tr>
    *
    * <tr><td>\b TAI</td>  <td>Atomic International Time.</td></tr>
    *
    * <tr><td>\b UT1</td>  <td>Universal Time.</td></tr>
    *
    * <tr><td>\b UT</td>   <td>Universal Time (same as UT1).</td></tr>
    * </table>
    *
    * \a timescale string values are considered case-sensitive.
    *
    * All necessary timescale conversions to compute ephemerides and reduction
    * of positions are performed automatically.
    */
   Position( TimePoint t, const IsoString& timescale = "TT" );

   /*!
    * Copy constructor.
    */
   Position( const Position& ) = default;

   /*!
    * Move constructor.
    */
   Position( Position&& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   Position& operator =( const Position& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   Position& operator =( Position&& ) = default;

   /*!
    * Computes the true position of a solar system body.
    *
    * The components of the returned vector are the geocentric or topocentric
    * rectangular equatorial coordinates for the calculation instant defined by
    * this object in the TT timescale, <em>without accounting for light-travel
    * time,</em> for the body defined by the specified ephemeris handle \a H.
    *
    * This function calls Geometric( EphemerisFile::Handle& ) internally to
    * compute, if necessary, the geometric position with correction for light
    * time, that is, no separate calculation routine is implemented for true
    * positions. The returned vector is only useful to compute the true
    * geocentric or topocentric distance, and for verification purposes.
    */
   Vector True( EphemerisFile::Handle& H )
   {
      (void)Geometric( H );
      return m_U0;
   }

   /*!
    * Computes the geometric position of a star.
    *
    * This function has been implemented for completeness. It is a synonym for
    * Geometric( const StarPosition& ). There are no known 'true' positions of
    * stars, since their light-travel time is implicitly included in the space
    * motion equations.
    */
   Vector True( const StarPosition& S )
   {
      (void)Geometric( S );
      return m_U0;
   }

   /*!
    * Computes the true distance of a solar system body.
    *
    * The true distance is the actual distance from the body to the observer,
    * geocentric or topocentric, at the instant of calculation. This excludes
    * the light-travel time correction.
    */
   double TrueDistance( EphemerisFile::Handle& H )
   {
      return True( H ).L2Norm();
   }

   /*!
    * Computes the true distance of a star.
    *
    * The returned value is just the norm of the geometric position vector,
    * that is:
    *
    * \code Geometric( S ).L2Norm() \endcode
    *
    * This should be an actual distance in au only for stars with known
    * parallaxes. For stars where the parallax is unknown or undefined, this
    * value is meaningless because in such cases position vectors are unit
    * vectors, whose components are also known as <em>direction cosines.</em>
    */
   double TrueDistance( const StarPosition& S )
   {
      return True( S ).L2Norm();
   }

   /*!
    * Computes the geometric position of a solar system body.
    *
    * The components of the returned vector are the geocentric or topocentric
    * rectangular equatorial coordinates for the instant of calculation defined
    * by this object in the TT timescale, accounting for light-travel time, for
    * the body defined by the specified ephemeris handle \a H.
    *
    * The implemented reduction algorithm includes just the correction for
    * light-travel time, but no corrections for light deflection, annual
    * aberration, nutation, or precession. The position so calculated allows to
    * plot the specified body directly on an existing sky chart referred to
    * GCRS/J2000.0. Note however, that for generation of new graphical
    * representations for a given date using star catalog data, astrometric or
    * proper places should be used instead.
    */
   Vector Geometric( EphemerisFile::Handle& H );

   /*!
    * Computes the geometric position of a star.
    *
    * The components of the returned vector are the geocentric or topocentric
    * rectangular equatorial coordinates for the instant of calculation defined
    * by this object in the TT timescale, for the positional star data defined
    * by the specified object \a S.
    *
    * The implemented reduction algorithm includes just the corrections for
    * space motion: parallax, radial velocity and proper motions, when the
    * corresponding data items are nonzero in the specified object \a S. The
    * space motion vector includes terms to account for the relativistic
    * Doppler effect.
    */
   Vector Geometric( const StarPosition& S );

   /*!
    * Computes the astrometric place of a solar system body.
    *
    * The returned vector is the astrometric place of the object defined by the
    * specified ephemeris handle \a H in geocentric or topocentric rectangular
    * equatorial coordinates, calculated for the instant defined by this object
    * in the TT timescale.
    *
    * The implemented reduction algorithm includes the following corrections:
    *
    * \li Light-travel time.
    *
    * \li Relativistic deflection of light due to solar gravitation (except
    * for the Sun, the Moon, and any object closer from Earth than the Sun at
    * the time of observation).
    *
    * An astrometric place does not include annual aberration, nutation and
    * precession corrections. Hence it is referred to an 'hybrid' reference
    * system, but similar to GCRS J2000.0.
    */
   Vector Astrometric( EphemerisFile::Handle& H );

   /*!
    * Computes the astrometric place of a star.
    *
    * The returned vector is the astrometric place calculated for the
    * positional star data defined by the specified object \a S. Its components
    * are geocentric or topocentric rectangular equatorial coordinates,
    * calculated for the instant defined by this object in the TT timescale.
    *
    * The implemented reduction algorithm includes the following corrections:
    *
    * \li Space motion, including parallax, radial velocity and proper motions,
    * with corrections for the relativistic Doppler effect.
    *
    * \li Relativistic deflection of light due to solar gravitation.
    *
    * An astrometric place does not include annual aberration, nutation and
    * precession corrections. Hence it is referred to an 'hybrid' reference
    * system, but similar to GCRS J2000.0.
    */
   Vector Astrometric( const StarPosition& S );

   /*!
    * Computes the proper place of a solar system body.
    *
    * The returned vector is the proper place of the object defined by the
    * specified ephemeris handle \a H in geocentric or topocentric rectangular
    * equatorial coordinates, calculated for the instant defined by this object
    * in the TT timescale.
    *
    * The implemented reduction algorithm includes the following corrections:
    *
    * \li Light-travel time.
    *
    * \li Relativistic deflection of light due to solar gravitation (except
    * for the Sun, the Moon, and any object closer from Earth than the Sun at
    * the time of observation.
    *
    * \li Aberration of light, including relativistic terms.
    *
    * A proper place does not include nutation and precession corrections,
    * hence it is referred to the reference coordinate system: GCRS J2000.0.
    */
   Vector Proper( EphemerisFile::Handle& H );

   /*!
    * Computes the proper place of a star.
    *
    * The returned vector is the proper place calculated for the positional
    * star data defined by the specified object \a S. Its components are
    * geocentric or topocentric rectangular equatorial coordinates, calculated
    * for the instant defined by this object in the TT timescale.
    *
    * The implemented reduction algorithm includes the following corrections:
    *
    * \li Space motion, including parallax, radial velocity and proper motions,
    * with corrections for the relativistic Doppler effect.
    *
    * \li Relativistic deflection of light due to solar gravitation.
    *
    * \li Aberration of light, including relativistic terms.
    *
    * A proper place does not include nutation and precession corrections,
    * hence it is referred to the reference coordinate system: GCRS J2000.0.
    */
   Vector Proper( const StarPosition& S );

   /*!
    * Computes the apparent place of a solar system body.
    *
    * The returned vector is the apparent place of the object defined by the
    * specified ephemeris handle \a H in geocentric or topocentric rectangular
    * equatorial coordinates, calculated for the instant defined by this object
    * in the TT timescale.
    *
    * The implemented reduction algorithm includes the following corrections:
    *
    * \li Light-travel time.
    *
    * \li Relativistic deflection of light due to solar gravitation (except
    * for the Sun, the Moon, and any object closer from Earth than the Sun at
    * the time of observation.
    *
    * \li Aberration of light, including relativistic terms.
    *
    * \li %Frame bias, precession and nutation. The origin of right ascension is
    * the true equinox of date.
    *
    * \note The declination coordinate is identical in both equinox-based
    * (apparent) and CIO-based (intermediate) positions. Only the origin of
    * right ascensions differs between both systems.
    *
    * \warning If a valid observer location has been defined by calling the
    * SetObserver() member function, and the specified ObserverPosition
    * structure requires CIO-based transformations (see the
    * ObserverPosition::cioBased member), this function will throw an Error
    * exception.
    */
   Vector Apparent( EphemerisFile::Handle& H );

   /*!
    * Computes the apparent place of a star.
    *
    * The returned vector is the apparent place calculated for the positional
    * star data defined by the specified object \a S. Its components are
    * geocentric or topocentric rectangular equatorial coordinates, calculated
    * for the instant defined by this object in the TT timescale.
    *
    * The implemented reduction algorithm includes the following corrections:
    *
    * \li Space motion, including parallax, radial velocity and proper motions,
    * with corrections for the relativistic Doppler effect.
    *
    * \li Relativistic deflection of light due to solar gravitation.
    *
    * \li Aberration of light, including relativistic terms.
    *
    * \li %Frame bias, precession and nutation. The origin of right ascension is
    * the true equinox of date.
    *
    * \note The declination coordinate is identical in both equinox-based
    * (apparent) and CIO-based (intermediate) positions. Only the origin of
    * right ascensions differs between both systems.
    *
    * \warning If a valid observer location has been defined by calling the
    * SetObserver() member function, and the specified ObserverPosition
    * structure requires CIO-based transformations (see the
    * ObserverPosition::cioBased member), this function will throw an Error
    * exception.
    */
   Vector Apparent( const StarPosition& S );

   /*!
    * Computes the intermediate place of a solar system body.
    *
    * The returned vector is the intermediate place of the object defined by
    * the specified ephemeris handle \a H in geocentric or topocentric
    * rectangular equatorial coordinates, calculated for the instant defined by
    * this object in the TT timescale.
    *
    * The implemented reduction algorithm includes the following corrections:
    *
    * \li Light-travel time.
    *
    * \li Relativistic deflection of light due to solar gravitation (except
    * for the Sun, the Moon, and any object closer from Earth than the Sun at
    * the time of observation.
    *
    * \li Aberration of light, including relativistic terms.
    *
    * \li %Frame bias, precession and nutation. The origin of right ascension is
    * the Celestial Intermediate Origin (CIO), following the IAU
    * recommendations since January 2003.
    *
    * \note The declination coordinate is identical in both equinox-based
    * (apparent) and CIO-based (intermediate) positions. Only the origin of
    * right ascensions differs between both systems.
    *
    * \warning If a valid observer location has been defined by calling the
    * SetObserver() member function, and the specified ObserverPosition
    * structure requires equinox-based transformations (see the
    * ObserverPosition::cioBased member), this function will throw an Error
    * exception.
    */
   Vector Intermediate( EphemerisFile::Handle& H );

   /*!
    * Computes the intermediate place of a star.
    *
    * The returned vector is the intermediate place calculated for the
    * positional star data defined by the specified object \a S. Its components
    * are geocentric or topocentric rectangular equatorial coordinates,
    * calculated for the instant defined by this object in the TT timescale.
    *
    * The implemented reduction algorithm includes the following corrections:
    *
    * \li Space motion, including parallax, radial velocity and proper motions,
    * with corrections for the relativistic Doppler effect.
    *
    * \li Relativistic deflection of light due to solar gravitation.
    *
    * \li Aberration of light, including relativistic terms.
    *
    * \li %Frame bias, precession and nutation. The origin of right ascension is
    * the Celestial Intermediate Origin (CIO), following the IAU
    * recommendations since January 2003.
    *
    * \note The declination coordinate is identical in both equinox-based
    * (apparent) and CIO-based (intermediate) positions. Only the origin of
    * right ascensions differs between both systems.
    *
    * \warning If a valid observer location has been defined by calling the
    * SetObserver() member function, and the specified ObserverPosition
    * structure requires equinox-based transformations (see the
    * ObserverPosition::cioBased member), this function will throw an Error
    * exception.
    */
   Vector Intermediate( const StarPosition& S );

   /*!
    * Defines the geodetic positional and reference data necessary for
    * calculation of topocentric positions of solar system objects and stars.
    *
    * By default, an instance of the %Position class calculates geocentric
    * coordinates. After calling this member function, subsequently calculated
    * geometric, proper, astrometric, apparent and intermediate places will be
    * topocentric, that is, will be referred to the location of the observer
    * with respect to the center of the Earth.
    *
    * By calling this member funcion, all previously computed positional data
    * will be erased with the exception of fundamental ephemerides and existing
    * bias-precession-nutation matrices, which can always be preserved.
    *
    * The ObserverPosition::cioBased data member of the specified \a observer
    * structure selects the celestial system and paradigm to be used in the
    * calculation of positions that depend on Earth's rotation. If the cioBased
    * member is true, equinox-based places cannot be calculated, and any of the
    * Apparent() member functions will throw an exception if called.
    * Conversely, if cioBased is false, CIO-based places cannot be calculated
    * and no call to an Intermediate() member function will be allowed.
    *
    * If polar motion corrections are enabled, the position of the CIP with
    * respect to the ITRS is interpolated from the global CIP_ITRS database, if
    * it provides data for the current time of calculation. In such case, polar
    * motion is taken into account in the calculation of the observer's
    * geocentric position and velocity. For the geocentric velocity a standard
    * constant value for the rotation rate of the Earth is used; the velocity
    * component due to precession and nutation is not taken into account since
    * its effect is negligible. See the IsPolarMotionEnabled() and CIP_ITRS()
    * member functions for more information and additional references.
    */
   void SetObserver( const ObserverPosition& observer );

   /*!
    * Removes the current observer, if a valid one has been defined by a
    * previous call to SetObserver(). In such case, all previously computed
    * positional data will be invalidated, with the exception of fundamental
    * ephemerides and existing bias-precession-nutation matrices.
    *
    * If no observer has been defined for this object, calling this member
    * function has no effect.
    *
    * After calling this member function, all subsequently calculated
    * geometric, proper, astrometric, apparent and intermediate places will be
    * geocentric.
    */
   void SetGeocentric();

   /*!
    * Returns the current observer. If no observer has been defined for this
    * object, returns a default-constructed structure for a fictitious observer
    * at the geocenter.
    */
   ObserverPosition Observer() const
   {
      if ( m_observer )
         return *m_observer;
      return ObserverPosition();
   }

   /*!
    * Returns true iff a valid observer location has been defined for this
    * object in a previous call to SetObserver().
    */
   bool IsTopocentric() const
   {
      return m_observer;
   }

   /*!
    * Returns true iff topocentric places take into account polar motion
    * corrections to compute the geocentric position and velocity of the
    * observer. This involves calculation of CIP coordinates with respect to
    * the ITRS, as well as access to a database of CIP/ITRS positions. See the
    * CIP_ITRS() member function for more details.
    *
    * Polar motion introduces changes at the mas level for calculation of
    * topocentric coordinates of the Moon. For the rest of objects, the effect
    * of polar motion corrections is completely negligible. For topocentric
    * positions of the Moon, polar motion can be necessary to achieve the
    * highest accuracy, but in such case one may also have to take into account
    * a regional geoid referred to the Earth's reference ellipsoid. See the
    * ObserverPosition structure.
    */
   bool IsPolarMotionEnabled() const
   {
      return m_usePolarMotion;
   }

   /*!
    * Enables polar motion corrections for calculation of topocentric places.
    * See the CIP_ITRS() and IsPolarMotionEnabled() functions for more details.
    */
   void EnablePolarMotion( bool enable = true )
   {
      m_usePolarMotion = enable;
   }

   /*!
    * Disables polar motion corrections for calculation of topocentric places.
    * See the CIP_ITRS() and IsPolarMotionEnabled() functions for more details.
    */
   void DisablePolarMotion( bool disable = true )
   {
      EnablePolarMotion( !disable );
   }

   /*!
    * Returns the time of calculation for this object in the Barycentric
    * Dynamical Time (TDB) timescale.
    *
    * The times of calculation in different timescales are calculated by the
    * class constructor.
    */
   TimePoint TDB() const
   {
      return m_t;
   }

   /*!
    * Returns the time of calculation for this object in the ephemeris
    * timescale defined by the JPL DE/LE numerical integration. For all
    * purposes this is equivalent to TDB.
    *
    * The times of calculation in different timescales are calculated by the
    * class constructor.
    */
   TimePoint Teph() const
   {
      return m_t;
   }

   /*!
    * Returns the time of calculation for this object in the Terrestrial Time
    * (TT) timescale.
    *
    * The times of calculation in different timescales are calculated by the
    * class constructor.
    */
   TimePoint TT() const
   {
      return m_tt;
   }

   /*!
    * Returns the time of calculation for this object in the Universal Time
    * (UT1) timescale.
    *
    * The times of calculation in different timescales are calculated by the
    * class constructor.
    */
   TimePoint UT1() const
   {
      return m_ut1;
   }

   /*!
    * Returns the ICRS barycentric position of the Earth (barycentric
    * rectangular equatorial coordinates), computed for the TDB time of
    * calculation by the class constructor. The components of the
    * returned vector are expressed in au.
    */
   Vector BarycentricPositionOfEarth() const
   {
      return m_Eb;
   }

   /*!
    * Returns the ICRS barycentric velocity of the Earth (barycentric
    * rectangular equatorial coordinates), computed for the TDB time of
    * calculation by the class constructor. The components of the
    * returned vector are expressed in au/day.
    */
   Vector BarycentricVelocityOfEarth() const
   {
      return m_Edb;
   }

   /*!
    * Returns the ICRS barycentric position of the Sun (barycentric rectangular
    * equatorial coordinates), computed for the TDB time of calculation by the
    * class constructor. The components of the returned vector are in au.
    */
   Vector BarycentricPositionOfSun() const
   {
      return m_Sb;
   }

   /*!
    * Returns the ICRS heliocentric position of the Earth (heliocentric
    * rectangular equatorial coordinates), computed for the TDB time of
    * calculation by the class constructor. The components of the
    * returned vector are expressed in au.
    */
   Vector HeliocentricPositionOfEarth() const
   {
      return m_Eh;
   }

   /*!
    * Calculates all parameters and data structures necessary for equinox-based
    * reduction of positions.
    *
    * This member function calculates the following structures:
    *
    * \li Precession+bias angles, IAU 2006 precession model, Fukushima-Williams
    * parameterization. See ESAsA sections 6.6.2.2 and 7.2.5.1.
    *
    * \li Mean obliquity of the ecliptic, IAU 2006 precession model. See ESAsA
    * section 7.2.5.1.
    *
    * \li Nutation angles, IAU 2006/2000A_R nutation model. See ESAsA section
    * 6.6.1.
    *
    * \li Combined bias-precession-nutation matrix, equinox-based. See ESAsA
    * sections 6.7 and 7.2.5.1.
    *
    * \li Position of the Celestial Intermediate Pole (CIP). ESAsA section 6.7.
    *
    * \li Celestial Intermediate Origin (CIO) locator. ESAsA section 6.7.
    *
    * \li Equation of the origins (EO). See Wallace, P. & Capitaine, N., 2006,
    * Astron.Astrophys. 459, 981, and ESAsA section 6.4.
    *
    * \li Earth rotation angle (ERA) for the UT1 time of calculation. See IERS
    * Technical Note No. 32, 2003, Section 5.4.4.
    *
    * \li Greenwich Apparent Sidereal Time (GAST), IAU 2006. ESAsA 6.8.5.
    *
    * Since all of these items depend exclusively on time, they are computed
    * only once in the first call to this function, and subsequent calls will
    * have no effect.
    *
    * Normally, you don't have to call this function directly because it is
    * invoked automatically when necessary by the different position reduction
    * routines.
    */
   void InitEquinoxBasedParameters();

   /*!
    * Calculates all parameters and data structures necessary for CIO-based
    * reduction of positions.
    *
    * This member function starts by calling InitEquinoxBasedParameters(), so
    * it implicitly calculates all equinox-based parameters. Then it calculates
    * the CIO-based combined bias-precession-nutation matrix. See ESAsA
    * sections 6.7 and 7.2.5.2.
    *
    * Since all of these items depend exclusively on time, they are computed
    * only once in the first call to this function, and subsequent calls will
    * have no effect.
    *
    * Normally, you don't have to call this function directly because it is
    * invoked automatically when necessary by the different position reduction
    * routines.
    */
   void InitCIOBasedParameters();

   /*!
    * Returns the equinox-based combined bias-precession-nutation matrix.
    *
    * This member function calls InitEquinoxBasedParameters() to ensure that
    * all data required for equinox-based reduction of positions is available.
    */
   Matrix EquinoxBiasPrecessionNutationMatrix()
   {
      InitEquinoxBasedParameters();
      return m_M;
   }

   /*!
    * Returns the CIO-based combined bias-precession-nutation matrix.
    *
    * This member function calls InitCIOBasedParameters() to ensure that
    * all data required for equinox-based and CIO-based reduction of positions
    * is available.
    */
   Matrix CIOBiasPrecessionNutationMatrix()
   {
      InitCIOBasedParameters();
      return m_C;
   }

   /*!
    * Coordinates of the Celestial Intermediate Pole (CIP) in the GCRS, in
    * radians.
    *
    * This member function calls InitEquinoxBasedParameters() to ensure that
    * all data required for equinox-based reduction of positions is available.
    */
   Vector CIP()
   {
      InitEquinoxBasedParameters();
      return Vector( m_X, m_Y );
   }

   /*!
    * Coordinates of the Celestial Intermediate Pole (CIP) in the ITRS.
    *
    * If possible, this member function returns an interpolated value from the
    * global CIP_ITRS database, which will be loaded and parsed upon the first
    * call to this function as a thread-safe procedure. See
    * EphemerisFile::CIP_ITRSDataFilePath() and
    * EphemerisFile::OverrideCIP_ITRSDataFilePath() for more information. See
    * also IsPolarMotionEnabled() for some practical considerations.
    *
    * Otherwise, if the time of calculation for this object falls outside the
    * CIP_ITRS database time span, this function will return a two-dimensional
    * vector with zero components.
    *
    * The components of the returned 2-D vector are the coordinates
    * x<sub>p</sub>, y<sub>p</sub> of the CIP with respect to the ITRS,
    * expressed in radians.
    */
   Vector CIP_ITRS() const;

   /*!
    * The Celestial Intermediate Origin (CIO) locator, in radians.
    *
    * This member function calls InitEquinoxBasedParameters() to ensure that
    * all data required for equinox-based reduction of positions is available.
    */
   double CIO()
   {
      InitEquinoxBasedParameters();
      return m_s;
   }

   /*!
    * Equation of the origins, expressed in radians.
    *
    * This member function calls InitEquinoxBasedParameters() to ensure that
    * all data required for equinox-based reduction of positions is available.
    */
   double EO()
   {
      InitEquinoxBasedParameters();
      return m_EO;
   }

   /*!
    * Earth rotation angle, expressed in radians.
    *
    * This member function calls InitEquinoxBasedParameters() to ensure that
    * all data required for equinox-based reduction of positions is available.
    */
   double ERA()
   {
      InitEquinoxBasedParameters();
      return m_ERA;
   }

   /*!
    * Greenwich apparent sidereal time, expressed in radians.
    *
    * This member function calls InitEquinoxBasedParameters() to ensure that
    * all data required for equinox-based reduction of positions is available.
    */
   double GAST()
   {
      InitEquinoxBasedParameters();
      return m_GAST;
   }

   /*!
    * Mean obliquity of the ecliptic, in radians.
    */
   double EpsA() const
   {
      // Mean obliquity of the ecliptic, IAU 2006 precession model.
      if ( m_M.IsEmpty() )
         return AsRad( Poly( m_TT, { 84381.406, -46.836769, -0.0001831, 0.00200340, -0.000000576, -0.0000000434 } ) );
      return m_epsa;
   }

   /*!
    * Returns the nutation angles in radians as a point p, where \e p.x is the
    * nutation in longitude and \e p.y is the nutation in obliquity.
    *
    * This member function calls InitEquinoxBasedParameters() to ensure that
    * all data required for equinox-based reduction of positions is available.
    */
   DPoint NutationAngles()
   {
      InitEquinoxBasedParameters();
      return DPoint( m_dpsi, m_deps );
   }

   /*!
    * Conversion from rectangular equatorial to rectangular ecliptic
    * coordinates.
    *
    * \param q    Rectangular equatorial coordinates.
    *
    * \param se   Sine of the obliquity of the ecliptic.
    *
    * \param ce   Cosine of the obliquity of the ecliptic.
    *
    * Returns a vector whose components are the rectangular ecliptic
    * coordinates corresponding to the specified equatorial position \a q at
    * the epoch where the specified obliquity has been calculated.
    */
   static Vector EquatorialToEcliptic( const Vector& q, double se, double ce )
   {
      // Rx(eps)*q
      return Vector( q[0], q[1]*ce + q[2]*se, q[2]*ce - q[1]*se );
   }

   /*!
    * Conversion from rectangular equatorial to rectangular ecliptic
    * coordinates.
    *
    * \param q    Rectangular equatorial coordinates.
    *
    * \param eps  Oliquity of the ecliptic in radians.
    *
    * Returns a vector whose components are the rectangular ecliptic
    * coordinates corresponding to the specified equatorial position \a q at
    * the epoch where the specified obliquity has been calculated.
    */
   static Vector EquatorialToEcliptic( const Vector& q, double eps )
   {
      double se, ce; SinCos( eps, se, ce );
      return EquatorialToEcliptic( q, se, ce );
   }

   /*!
    * Conversion from spherical equatorial to spherical ecliptic coordinates.
    *
    * \param q    Spherical equatorial coordinates in radians, where \a q.x is
    *             the right ascension and \a q.y is the declination.
    *
    * \param se   Sine of the obliquity of the ecliptic.
    *
    * \param ce   Cosine of the obliquity of the ecliptic.
    *
    * Returns the ecliptic coordinates in radians as a point \e p, where \e p.x
    * is the longitude in the range [0,2pi) and \e p.y is the latitude in
    * [-pi/2,+pi/2].
    */
   static DPoint EquatorialToEcliptic( const DPoint& q, double se, double ce )
   {
      DPoint e;
      EquatorialToEcliptic( Vector::FromSpherical( q.x, q.y ), se, ce ).ToSpherical2Pi( e.x, e.y );
      return e;
   }

   /*!
    * Conversion from spherical equatorial to spherical ecliptic coordinates.
    *
    * \param q    Spherical equatorial coordinates in radians, where \a q.x is
    *             the right ascension and \a q.y is the declination.
    *
    * \param eps  Oliquity of the ecliptic in radians.
    *
    * Returns the ecliptic coordinates in radians as a point \e p, where \e p.x
    * is the longitude in the range [0,2pi) and \e p.y is the latitude in
    * [-pi/2,+pi/2].
    */
   static DPoint EquatorialToEcliptic( const DPoint& q, double eps )
   {
      double se, ce; SinCos( eps, se, ce );
      return EquatorialToEcliptic( q, se, ce );
   }

   /*!
    * Conversion from ICRS rectangular equatorial to rectangular galactic
    * coordinates.
    *
    * \param q    Rectangular equatorial coordinates in the ICRS.
    *
    * Returns a vector whose components are the calculated rectangular galactic
    * coordinates.
    *
    * In this routine we adopt the proposed ICRS coordinates of the
    * galactic pole in:
    *
    * Jia-Cheng Liu, Zi Zhu, and Hong Zhang, <em>Reconsidering the galactic
    * coordinate system</em>, Astronomy & Astrophysics manuscript no. AA2010,
    * October 26, 2018.
    *
    * The applied conventional definitions are as follows. The ICRS equatorial
    * coordinates of the zero point of galactic coordinates are:
    *
    * &alpha; = 17<sup>h</sup>45<sup>m</sup>40<sup>s</sup>.0400
    * &delta; = &ndash;29&deg;00'28".138
    *
    * The equatorial coordinates of the galactic pole, coherent with the ICRS,
    * are:
    *
    * &alpha;<sup>p</sup> = 12<sup>h</sup>51<sup>m</sup>36<sup>s</sup>.7151981
    * &delta;<sup>p</sup> = +27&deg;06'11".193172
    *
    * Note that these definitions are not consistent with the conventional
    * values currently accepted by the IAU. The current (as of October 2018)
    * galactic coordinate system was defined by the IAU in 1959 in the FK4
    * B1950.0 reference system.
    */
   static Vector ICRSEquatorialToGalactic( const Vector& q )
   {
      return Matrix( +0.494055821648, -0.054657353964, -0.445679169947,
                     -0.872844082054, -0.484928636070, +0.746511167077,
                     -0.867710446378, -0.198779490637, +0.455593344276 )*q;
   }

   /*!
    * Conversion from ICRS spherical equatorial to spherical galactic
    * coordinates.
    *
    * \param q    Spherical ICRS equatorial coordinates in radians, where
    *             \a q.x is the right ascension and \a q.y is the declination.
    *
    * Returns the galactic coordinates in radians as a point \e p, where \e p.x
    * is the longitude in the range [0,2pi) and \e p.y is the latitude in
    * [-pi/2,+pi/2].
    *
    * See the documentation for ICRSEquatorialToGalactic( const Vector& ) for
    * critical information on the adopted galactic coordinate system.
    */
   static DPoint ICRSEquatorialToGalactic( const DPoint& q )
   {
      DPoint g;
      ICRSEquatorialToGalactic( Vector::FromSpherical( q.x, q.y ) ).ToSpherical2Pi( g.x, g.y );
      return g;
   }

private:

   // TDB
   TimePoint   m_t;
   // TT
   TimePoint   m_tt;
   // UT1
   TimePoint   m_ut1;
   // TT in Julian centuries since J2000.0.
   double      m_TT;
   // Barycentric ICRS position and velocity of the Earth.
   Vector      m_Eb, m_Edb;
   // Geocentric ICRS position and velocity of the observer, in km and km/day.
   Vector      m_G, m_Gd;
   // Barycentric ICRS position of the Sun.
   Vector      m_Sb;
   // Heliocentric position of the Earth.
   Vector      m_Eh;
   // Distance Earth-Sun.
   double      m_E;
   // Heliocentric position of the observer (au).
   Vector      m_Oh;
   // Distance observer-Sun (au).
   double      m_O;
   // Bias+precession angles.
   double      m_gamb, m_phib, m_psib, m_epsa;
   // Nutation angles.
   double      m_dpsi, m_deps;
   // Position of the Celestial Intermediate Pole (CIP) in GCRS.
   double      m_X, m_Y;
   // CIO locator.
   double      m_s;
   // Combined bias-precession-nutation matrix (NPB).
   Matrix      m_M, m_Minv;
   // Combined bias-precession-nutation matrix (NPB_CIO).
   Matrix      m_C, m_Cinv;
   // Equation of the origins.
   double      m_EO;
   // Earth rotation angle.
   double      m_ERA;
   // Greenwich apparent sidereal time.
   double      m_GAST;
   // Light-travel time in days.
   double      m_tau = 0;
   // Barycentric position.
   Vector      m_ub;
   // True geocentric position.
   Vector      m_U0;
   // Geocentric position.
   Vector      m_U;
   // Astrometric place.
   Vector      m_u1;
   // Proper place.
   Vector      m_u2;
   // Apparent place.
   Vector      m_u3e;
   // Intermediate place.
   Vector      m_u3i;

   // Handles for calculation of fundamental ephemerides and nutation angles.
   AutoPointerCloner<EphemerisFile::Handle> m_TT_TDB, m_HE, m_HS, m_HN;

   // Current observer for calculation of topocentric coordinates.
   AutoPointerCloner<ObserverPosition> m_observer;

   // Opaque pointer to the object whose positions are being calculated.
   const void* m_object = nullptr;

   // Special case flags.
   bool m_isMoon = false, m_isSun = false, m_isStar = false;

   // Whether to account for polar motion (CIP->ITRS) in calculation of
   // topocentric positions.
   bool m_usePolarMotion = true;

   bool Validate( const void* );

   Vector Deflection();
   Vector Aberration();

   static double CIOLocator( double T, double X, double Y );

   /*
    * Astronomical constants, IAU 2009/2012 and IERS 2003/2010.
    */
   constexpr static double au_km    = 149597870.7;          // astronomical unit (km)
   constexpr static double c_km_s   = 299792.458;           // speed of light (km/s)
   constexpr static double c_km_day = c_km_s*86400;         // speed of light (km/day)
   constexpr static double c_au_day = (c_km_s/au_km)*86400; // speed of light (au/day)
   constexpr static double earth_omega = 7.292115e-5;       // angular velocity of Earth in radians/s
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Position_h

// ----------------------------------------------------------------------------
// EOF pcl/Position.h - Released 2018-11-01T11:06:36Z
