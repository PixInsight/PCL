//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/Position.cpp - Released 2019-01-21T12:06:21Z
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

#include <pcl/AkimaInterpolation.h>
#include <pcl/Position.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * These expressions are odr-used, so a definition is necessary:
 *
 * https://en.cppreference.com/w/cpp/language/static
 * https://en.cppreference.com/w/cpp/language/definition#ODR-use
 *
 * Fortunately, this oddity will be deprecated by C++17.
 */
constexpr double Position::au_km;
constexpr double Position::c_km_s;
constexpr double Position::c_km_day;
constexpr double Position::c_au_day;
constexpr double Position::earth_omega;

// ----------------------------------------------------------------------------

static double TDB_TT( const TimePoint& t )
{
   // Approximate formula for TDB-TT, result in seconds.
   // ESAsA Eq. 7.101
   double T1 = t.CenturiesSinceJ2000();
   double g = Rad( 357.53 + 3.599905e+4*T1 );
   double L_LJ = Rad( 246.11 + 3.296447e+4*T1 );
   return 0.001657*Sin( g ) + 0.000022*Sin( L_LJ );
}

Position::Position( TimePoint t, const IsoString& timescale )
{
   if ( !t.IsValid() )
      throw Error( "Invalid or uninitialized time point specified." );

   if ( timescale.IsEmpty() )
      throw Error( "No time scale has been specified." );

   /*
    * Access to fundamental ephemerides.
    */
   const EphemerisFile& E = EphemerisFile::FundamentalEphemerides();

   /*
    * Dynamic Barycentric Time (TDB)
    * Terrestrial Time (TT)
    * Universal Time (UT1)
    */
   if ( timescale == "TDB" || timescale == "Teph" )
   {
      // TDB
      m_t = t;

      // Compute TT from TDB.
      if ( E.IsObjectAvailable( "TT_TDB" ) )
      {
         // If TT-TDB at the geocenter is available from fundamental
         // ephemerides, prefer it to analytical approximations.
         m_TT_TDB = new EphemerisFile::Handle( E, "TT_TDB" );
         m_tt = m_t + m_TT_TDB->StateVector( m_t )[0]/86400;
      }
      else
      {
         // Otherwise, use approximate formulae.
         m_tt = m_t - TDB_TT( m_t )/86400;
      }

      m_ut1 = m_tt - m_tt.DeltaT()/86400;
   }
   else
   {
      // Get time in the Terrestrial Time (TT) time scale.
      if ( timescale == "TT" )
      {
         m_tt = t;
         m_ut1 = m_tt - m_tt.DeltaT()/86400;
      }
      else if ( timescale == "UTC" )
      {
         m_tt = t + (t.DeltaAT() + 32.184)/86400;
         m_ut1 = m_tt - m_tt.DeltaT()/86400;
      }
      else if ( timescale == "UT1" || timescale == "UT" )
      {
         m_ut1 = t;
         m_tt = m_ut1 + m_ut1.DeltaT()/86400;
      }
      else if ( timescale == "TAI" )
      {
         m_tt = t + 32.184/86400;
         m_ut1 = m_tt - m_tt.DeltaT()/86400;
      }
      else
         throw Error( "Unknown or unsupported time scale '" + timescale + "'." );

      // Compute TDB from TT.
      if ( E.IsObjectAvailable( "TT_TDB" ) )
      {
         // Strictly, we should get the value of TDB iteratively here, because
         // we are using TT as the independent variable instead of Teph=TDB.
         // The difference should be negligible, though.
         m_TT_TDB = new EphemerisFile::Handle( E, "TT_TDB" );
         m_t = t - m_TT_TDB->StateVector( m_tt )[0]/86400;
      }
      else
      {
         // Otherwise, use TDB-TT approximation.
         m_t = m_tt + TDB_TT( m_tt )/86400;
      }
   }

   m_TT = m_tt.CenturiesSinceJ2000();

   /*
    * Ephemerides for Earth and Sun.
    */
   m_HE = new EphemerisFile::Handle( E, "Ea", "SSB" );
   m_HS = new EphemerisFile::Handle( E, "Sn", "SSB" );

   /*
    * Barycentric position and velocity of the Earth.
    */
   m_HE->ComputeState( m_Eb, m_Edb, m_t );

   /*
    * Barycentric position of the Sun.
    */
   m_HS->ComputeState( m_Sb, m_t );

   /*
    * Heliocentric position of the Earth.
    */
   m_Eh = m_Eb - m_Sb;
   m_E = m_Eh.L2Norm();
}

// ----------------------------------------------------------------------------

void Position::SetObserver( const ObserverPosition& O )
{
   m_U = m_ub = m_u1 = m_u2 = m_u3e = m_u3i = Vector();
   m_tau = 0;
   m_isMoon = m_isSun = m_isStar = false;

   m_observer = new ObserverPosition( O );

   if ( m_observer->cioBased )
      InitCIOBasedParameters();
   else
      InitEquinoxBasedParameters();

   /*
    * Geocentric position and velocity of the observer.
    */
   double slst, clst;
   SinCos( Norm2Pi( (m_observer->cioBased ? m_ERA : m_GAST) + Rad( m_observer->lambda ) ), slst, clst );
   double sphi, cphi;
   SinCos( Rad( m_observer->phi ), sphi, cphi );
   double f2 = 1 - m_observer->f;
   f2 *= f2;
   double C = 1/Sqrt( cphi*cphi + f2*sphi*sphi );
   double S = f2*C;
   double aC_hcphi = (m_observer->a*C + m_observer->h)*cphi;
   double aS_h = m_observer->a*S + m_observer->h;
   // guard us against ill-formed structures
   Vector r0 = m_observer->r0;
   if ( r0.Length() < 3 )
      r0 = Vector( 0.0, 0.0, 0.0 );
   // Position vector in km
   Vector g( r0[0] + aC_hcphi*clst,
             r0[1] + aC_hcphi*slst,
             r0[2] + aS_h*sphi );
   g /= 1000;
   // Velocity vector in km/day
   Vector gd( -earth_omega * g[1],
               earth_omega * g[0],
               0.0 );
   gd *= 86400;

   if ( m_usePolarMotion )
   {
      Vector p = CIP_ITRS();
      double s1 = AsRad( -0.000047*m_TT ); // TIO locator
      // ESAsA Eq. 7.77
      Matrix Winv = Matrix::UnitMatrix( 3 );
      Winv.RotateX( p[1] );
      Winv.RotateY( p[0] );
      Winv.RotateZ( -s1 );
      // ESAsA Eq. 7.138, Eq. 7.140
      g = Winv*g;
      gd = Winv*gd;
   }

   /*
    * Geocentric position and velocity of the observer in the ICRS.
    */
   m_G = m_Minv*g;   // km
   m_Gd = m_Minv*gd; // km/day

   /*
    * Heliocentric position of the observer.
    */
   m_Oh = m_Eh + m_G/au_km; // au
   m_O = m_Oh.L2Norm();     // au
}

// ----------------------------------------------------------------------------

void Position::SetGeocentric()
{
   if ( m_observer )
   {
      m_observer.Destroy();

      m_U0 = m_U = m_ub = m_u1 = m_u2 = m_u3e = m_u3i = m_G = m_Gd = m_Oh = Vector();
      m_tau = 0;
      m_isMoon = m_isSun = m_isStar = false;
   }
}

// ----------------------------------------------------------------------------

void Position::InitEquinoxBasedParameters()
{
   if ( m_M.IsEmpty() )
   {
      /*
       * Precession+bias angles, IAU 2006 precession model.
       * Fukushima-Williams parameterization.
       */
      m_gamb = AsRad( Poly( m_TT, { -0.052928, 10.556378, 0.4932044, -0.00031238, -0.000002788, 0.0000000260 } ) );
      m_phib = AsRad( Poly( m_TT, { 84381.412819, -46.811016, 0.0511268, 0.00053289, -0.000000440, -0.0000000176 } ) );
      m_psib = AsRad( Poly( m_TT, { -0.041775, 5038.481484, 1.5584175, -0.00018522, -0.000026452, -0.0000000148 } ) );

      /*
       * Mean obliquity of the ecliptic, IAU 2006 precession model.
       */
      m_epsa = AsRad( Poly( m_TT, { 84381.406, -46.836769, -0.0001831, 0.00200340, -0.000000576, -0.0000000434 } ) );

      /*
       * Nutation angles, IAU 2006/2000A_R nutation model.
       */
      if ( !m_HN.IsValid() )
         m_HN = new EphemerisFile::Handle( EphemerisFile::NutationModel(), "IAUNut" );
      Vector n = m_HN->StateVector( m_tt );
      m_dpsi = n[0];
      m_deps = n[1];

      /*
       * Combined bias-precession-nutation matrix.
       */
      m_M = Matrix::UnitMatrix( 3 );
      m_M.RotateZ(  m_gamb );
      m_M.RotateX(  m_phib );
      m_M.RotateZ( -(m_psib + m_dpsi) );
      m_M.RotateX( -(m_epsa + m_deps) );
      m_Minv = m_M.Inverse();

      /*
       * Position of the Celestial Intermediate Pole (CIP).
       */
      m_X = m_M[2][0];
      m_Y = m_M[2][1];

      /*
       * Celestial Intermediate Origin (CIO) locator.
       */
      m_s = CIOLocator( m_TT, m_X, m_Y );

      /*
       * Equation of the origins.
       * Wallace, P. & Capitaine, N., 2006, Astron.Astrophys. 459, 981, Eq. 16.
       */
      double aX = m_X/(1 + m_M[2][2]);
      double xs = 1 - aX*m_X;
      double ys = -aX * m_Y;
      double zs = -m_X;
      double p = m_M[0][0]*xs + m_M[0][1]*ys + m_M[0][2]*zs;
      double q = m_M[1][0]*xs + m_M[1][1]*ys + m_M[1][2]*zs;
      m_EO = m_s;
      if ( p != 0 || q != 0 )
         m_EO -= ArcTan( q, p );

      /*
       * Earth rotation angle (ERA) for the current UT1.
       * IERS Technical Note No. 32, 2003, Section 5.4.4, Eq. 14.
       */
      double t = m_ut1.DaysSinceJ2000();
      m_ERA = Norm2Pi( TwoPi()*(Frac( t ) + 0.7790572732640 + 0.00273781191135448*t) );

      /*
       * Greenwich apparent sidereal time, IAU 2006.
       */
      m_GAST = Norm2Pi( m_ERA - m_EO );
   }
}

// ----------------------------------------------------------------------------

void Position::InitCIOBasedParameters()
{
   if ( m_C.IsEmpty() )
   {
      InitEquinoxBasedParameters();

      /*
       * Celestial-to-intermediate reduction matrix.
       */
      double r2 = m_X*m_X + m_Y*m_Y;
      double e = (r2 > 0) ? ArcTan( m_Y, m_X ) : 0.0;
      double d = ArcTan( Sqrt( r2/(1 - r2) ) );
      m_C = Matrix::UnitMatrix( 3 );
      m_C.RotateZ( e );
      m_C.RotateY( d );
      m_C.RotateZ( -(e + m_s) );
      m_Cinv = m_C.Inverse();
   }
}

// ----------------------------------------------------------------------------

static AkimaInterpolation<double> s_xp;
static AkimaInterpolation<double> s_yp;
static TimePoint                  s_cipITRSStart;
static TimePoint                  s_cipITRSEnd;
static AtomicInt                  s_cipITRSInitialized;
static Mutex                      s_cipITRSMutex;

Vector Position::CIP_ITRS() const
{
   if ( !s_cipITRSInitialized )
   {
      /*
       * Load the CIP_ITRS plain text database, thread-safe.
       */
      volatile AutoLock lock( s_cipITRSMutex );
      if ( s_cipITRSInitialized.Load() == 0 )
      {
         Array<double> T, XP, YP;
         IsoStringList lines = File::ReadLines( EphemerisFile::CIP_ITRSDataFilePath() );
         for ( IsoString line : lines )
         {
            line.Trim();
            if ( line.IsEmpty() || line.StartsWith( '/' ) ) // empty line or comment
               continue;
            IsoStringList tokens;
            line.Break( tokens, ' ', true/*trim*/ );
            tokens.Remove( IsoString() );
            if ( tokens.Length() < 3 ) // invalid format, ignore
               continue;
            TimePoint t( tokens[0].ToDouble() + 2400000.5 );
            double xp = tokens[1].ToDouble();
            double yp = tokens[2].ToDouble();

            // Make sure that dates are valid and lines are sorted in ascending
            // date order.
            if ( t.IsValid() )
            {
               if ( T.IsEmpty() )
                  s_cipITRSStart = t;
               else if ( t <= s_cipITRSEnd )
                  continue;

               T << t.YearsSinceJ2000();
               XP << xp;
               YP << yp;
               s_cipITRSEnd = t;
            }
         }

         // Check for reasonable minimum data.
         if ( (s_cipITRSEnd - s_cipITRSStart) < 365 )
            throw Error( "The CIP_ITRS data covers an insufficient time span." );
         if ( int( T.Length() ) < TruncInt( (s_cipITRSEnd - s_cipITRSStart)/365.25 )*8 )
            throw Error( "Insufficient CIP_ITRS data points." );

         s_xp.Initialize( Vector( T.Begin(), T.Length() ), Vector( XP.Begin(), XP.Length() ) );
         s_yp.Initialize( Vector( T.Begin(), T.Length() ), Vector( YP.Begin(), YP.Length() ) );
         s_cipITRSInitialized.Store( 1 );
      }
   }

   Vector p( 2 );

   /*
    * Return an interpolated value when possible.
    */
   if ( m_tt >= s_cipITRSStart && m_tt <= s_cipITRSEnd )
   {
      double t = m_tt.YearsSinceJ2000();
      p[0] = AsRad( s_xp( t ) );
      p[1] = AsRad( s_yp( t ) );
   }
   else
      p[0] = p[1] = 0;

   return p;
}

// ----------------------------------------------------------------------------

bool Position::Validate( const void* o )
{
   if ( o != m_object )
   {
      m_U0 = m_U = m_ub = m_u1 = m_u2 = m_u3e = m_u3i = Vector();
      m_tau = 0;
      m_isMoon = m_isSun = m_isStar = false;
      m_object = o;
      return false;
   }
   return true;
}

// ----------------------------------------------------------------------------

Vector Position::Deflection()
{
   /*
    * Relativistic light deflection.
    */
   double U = m_U.L2Norm();
   if ( !m_isStar && (m_isMoon || m_isSun || U <= m_E) )
   {
      // Do not apply light deflection to objects that are closer from Earth
      // than the Sun at the time of observation.
      m_u1 = m_U;
   }
   else
   {
      double d = m_observer ? m_O : m_E;
      Vector u = m_U.Unit();
      Vector e = (m_observer ? m_Oh : m_Eh)/d;
      double eu = e*u;
      if ( (1 - eu) < 1.0e-6 )
      {
         // Do not apply light deflection to objects observed too close to the
         // Sun's center. Here we apply a limit of about 5 arcminutes from the
         // center of the solar disk.
         m_u1 = m_U;
      }
      else
      {
         // Barycentric position of the Sun at t-tau.
         Vector Sb = m_HS->StateVector( m_t-m_tau );
         // Heliocentric position at t-tau.
         Vector q = (m_ub - Sb).Unit();
         // ESAsA Eq. 7.64
         double g1 = 2*9.8706e-9/d;
         double g2 = 1 + q*e;
         // ESAsA Eq. 7.63
         m_u1 = U*(u + g1*(((u*q)*e - eu*q)/g2));
      }
   }

   return m_u1;
}

// ----------------------------------------------------------------------------

Vector Position::Aberration()
{
   /*
    * Aberration of light.
    */
   if ( m_isMoon )
   {
      // For topocentric coordinates of the Moon, use classical aberration to
      // account for the geocentric velocity of the observer.
      m_u2 = m_u1;
      if ( m_observer )
         m_u2 += m_tau*m_Gd;  // ESAsA Eq. 7.55
   }
   else
   {
      // For other solar system objects and stars, apply stellar aberration
      // with relativistic terms.
      Vector u = m_u1.Unit();
      // Barycentric velocity of the Earth in units of c.
      Vector V = m_Edb/c_au_day;
      // Barycentric velocity of the observer in units of c.
      if ( m_observer )
         V += m_Gd/c_km_day;
      // Sqrt( 1 - |V|^2 ): Reciprocal of Lorenz factor
      double bm1 = Sqrt( 1 - V.SumOfSquares() );
      double f1 = u*V;
      double f2 = 1 + f1/(1 + bm1);
      m_u2 = (bm1*m_u1 + f2*m_u1.L2Norm()*V)/(1 + f1);
   }

   return m_u2;
}

// ----------------------------------------------------------------------------

Vector Position::Geometric( EphemerisFile::Handle& H )
{
   if ( Validate( &H ) )
      if ( !m_U.IsEmpty() )
         return m_U;

   /*
    * Special cases.
    */
   m_isMoon = H.ObjectId() == "Mn";
   m_isSun = !m_isMoon && H.ObjectId() == "Sn";

   /*
    * Ensure valid coordinate origins.
    */
   if ( H.OriginId() != (m_isMoon ? "Ea" : "SSB") )
      throw Error( "Invalid coordinate origin '" + H.OriginId() + "\' for reduction of solar system positions." );

   /*
    * Geocentric position, accounting for light-travel time.
    */
   m_tau = 0;
   int it = 0;
   for ( double d0 = 0; ; ++it )
   {
      // Position at t-tau
      m_U = H.StateVector( m_t-m_tau );

      if ( m_isMoon )
      {
         if ( m_observer )
            m_U -= m_G; // topocentric position at t-tau
         // True position.
         if ( it == 0 )
            m_U0 = m_U;
         // Geometric distance in km.
         double d = m_U.L2Norm();
         // Light-travel time in days.
         m_tau = d/c_km_day;
         // Iterate until convergence to 1 cm.
         if ( Abs( d - d0 ) < 1.0e-5 )
            break;
         d0 = d;
      }
      else
      {
         // Barycentric position at t-tau
         m_ub = m_U;
         // Geocentric position at t-tau.
         m_U -= m_Eb;
         if ( m_observer )
            m_U -= m_G/au_km; // topocentric position at t-tau
         // True position.
         if ( it == 0 )
            m_U0 = m_U;
         // Geometric distance in au.
         double d = m_U.L2Norm();
         // Light-travel time in days.
         m_tau = d/c_au_day;
         // Iterate until convergence to about 15 m.
         if ( Abs( d - d0 ) < 1.0e-10 )
            break;
         d0 = d;
      }
   }

   return m_U;
}

Vector Position::Geometric( const StarPosition& S )
{
   if ( Validate( &S ) )
      if ( !m_U.IsEmpty() )
         return m_U;

   m_isStar = true;

   double sa, ca;
   SinCos( Rad( S.alpha*15 ), sa, ca );
   double sd, cd;
   SinCos( Rad( S.delta ), sd, cd );

   // Barycentric direction at the catalog epoch.
   Vector u( cd*ca, cd*sa, sd );

   // Parallax
   double p = AsRad( S.p );

   // Relativistic Doppler effect.
   // ESAsA Eq. 7.26
   double f = 1/(1 - S.v/c_km_s);

   // Unit conversion factors.
   // - from mas/year to radians/day
   static constexpr double s = Pi()/180/365.25/3600000;
   // - from km/s to ua/day
   static constexpr double k = 86400/au_km;

   // Space motion vector in radians/day.
   Vector udot = (Matrix( -sa, -sd*ca, cd*ca,
                           ca, -sd*sa, cd*sa,
                          0.0,  cd,    sd    ) * Vector( f*s*S.muAlpha, f*s*S.muDelta, f*k*S.v*p ));

   // Barycentric position vector at time t.
   // ESAsA Eq. 7.127
   m_ub = u + (m_t - S.t0)*udot;

   // Geocentric position vector at time t.
   // ESAsA Eq. 7.128
   m_U = m_ub - p*m_Eb;
   if ( m_observer )
      m_U -= p*(m_G/au_km); // topocentric position

   // We cannot know the 'true' direction of a star.
   m_tau = 0;
   m_U0 = m_U;

   return m_U;
}

// ----------------------------------------------------------------------------

Vector Position::Astrometric( EphemerisFile::Handle& H )
{
   (void)Geometric( H );

   if ( !m_u1.IsEmpty() )
      return m_u1;

   return Deflection();
}

Vector Position::Astrometric( const StarPosition& S )
{
   (void)Geometric( S );

   if ( !m_u1.IsEmpty() )
      return m_u1;

   return Deflection();
}

// ----------------------------------------------------------------------------

Vector Position::Proper( EphemerisFile::Handle& H )
{
   (void)Astrometric( H );

   if ( !m_u2.IsEmpty() )
      return m_u2;

   return Aberration();
}

Vector Position::Proper( const StarPosition& S )
{
   (void)Astrometric( S );

   if ( !m_u2.IsEmpty() )
      return m_u2;

   return Aberration();
}

// ----------------------------------------------------------------------------

Vector Position::Apparent( EphemerisFile::Handle& H )
{
   if ( m_observer )
      if ( m_observer->cioBased )
         throw Error( "Invalid call for CIO-based topocentric observer." );

   (void)Proper( H );

   if ( !m_u3e.IsEmpty() )
      return m_u3e;

   /*
    * Apparent position = proper place + Frame bias + precession + nutation
    * (equinox-based).
    */
   InitEquinoxBasedParameters();
   m_u3e = m_M * m_u2;

   return m_u3e;
}

Vector Position::Apparent( const StarPosition& S )
{
   if ( m_observer )
      if ( m_observer->cioBased )
         throw Error( "Invalid call for CIO-based topocentric observer." );

   (void)Proper( S );

   if ( !m_u3e.IsEmpty() )
      return m_u3e;

   /*
    * Apparent position = proper place + Frame bias + precession + nutation
    * (equinox-based).
    */
   InitEquinoxBasedParameters();
   m_u3e = (m_M * m_u2).Unit();

   return m_u3e;
}

// ----------------------------------------------------------------------------

Vector Position::Intermediate( EphemerisFile::Handle& H )
{
   if ( m_observer )
      if ( !m_observer->cioBased )
         throw Error( "Invalid call for equinox-based topocentric observer." );

   (void)Proper( H );

   if ( !m_u3i.IsEmpty() )
      return m_u3i;

   /*
    * Intermediate position = proper place + Frame bias + precession + nutation
    * (CIO-based).
    */
   InitCIOBasedParameters();
   m_u3i = m_C * m_u2;

   return m_u3i;
}

Vector Position::Intermediate( const StarPosition& S )
{
   if ( m_observer )
      if ( !m_observer->cioBased )
         throw Error( "Invalid call for equinox-based topocentric observer." );

   (void)Proper( S );

   if ( !m_u3i.IsEmpty() )
      return m_u3i;

   /*
    * Intermediate position = proper place + Frame bias + precession + nutation
    * (CIO-based).
    */
   InitCIOBasedParameters();
   m_u3i = (m_C * m_u2).Unit();

   return m_u3i;
}

// ----------------------------------------------------------------------------

double Position::CIOLocator( double T, double X, double Y )
{
   /*
    * Poisson terms
    * Source: http://cdsarc.u-strasbg.fr/viz-bin/qcat?J/A+A/459/981
    */
   struct Term
   {
      double s, c; // coefficients of the sine and cosine of the argument, in uas.
      int k[ 8 ];  // coefficients of lunisolar and planetary arguments.
   };

   static const Term t0[] =
   {
      { -2640.73,   0.39, { 0,  0,  0,  0,  1,  0,  0,  0 } },
      {   -63.53,   0.02, { 0,  0,  0,  0,  2,  0,  0,  0 } },
      {   -11.75,  -0.01, { 0,  0,  2, -2,  3,  0,  0,  0 } },
      {   -11.21,  -0.01, { 0,  0,  2, -2,  1,  0,  0,  0 } },
      {     4.57,   0.00, { 0,  0,  2, -2,  2,  0,  0,  0 } },
      {    -2.02,   0.00, { 0,  0,  2,  0,  3,  0,  0,  0 } },
      {    -1.98,   0.00, { 0,  0,  2,  0,  1,  0,  0,  0 } },
      {     1.72,   0.00, { 0,  0,  0,  0,  3,  0,  0,  0 } },
      {     1.41,   0.01, { 0,  1,  0,  0,  1,  0,  0,  0 } },
      {     1.26,   0.01, { 0,  1,  0,  0, -1,  0,  0,  0 } },
      {     0.63,   0.00, { 1,  0,  0,  0, -1,  0,  0,  0 } },
      {     0.63,   0.00, { 1,  0,  0,  0,  1,  0,  0,  0 } },
      {    -0.46,   0.00, { 0,  1,  2, -2,  3,  0,  0,  0 } },
      {    -0.45,   0.00, { 0,  1,  2, -2,  1,  0,  0,  0 } },
      {    -0.36,   0.00, { 0,  0,  4, -4,  4,  0,  0,  0 } },
      {     0.24,   0.12, { 0,  0,  1, -1,  1, -8, 12,  0 } },
      {    -0.32,   0.00, { 0,  0,  2,  0,  0,  0,  0,  0 } },
      {    -0.28,   0.00, { 0,  0,  2,  0,  2,  0,  0,  0 } },
      {    -0.27,   0.00, { 1,  0,  2,  0,  3,  0,  0,  0 } },
      {    -0.26,   0.00, { 1,  0,  2,  0,  1,  0,  0,  0 } },
      {     0.21,   0.00, { 0,  0,  2, -2,  0,  0,  0,  0 } },
      {    -0.19,   0.00, { 0,  1, -2,  2, -3,  0,  0,  0 } },
      {    -0.18,   0.00, { 0,  1, -2,  2, -1,  0,  0,  0 } },
      {     0.10,  -0.05, { 0,  0,  0,  0,  0,  8,-13, -1 } },
      {    -0.15,   0.00, { 0,  0,  0,  2,  0,  0,  0,  0 } },
      {     0.14,   0.00, { 2,  0, -2,  0, -1,  0,  0,  0 } },
      {     0.14,   0.00, { 0,  1,  2, -2,  2,  0,  0,  0 } },
      {    -0.14,   0.00, { 1,  0,  0, -2,  1,  0,  0,  0 } },
      {    -0.14,   0.00, { 1,  0,  0, -2, -1,  0,  0,  0 } },
      {    -0.13,   0.00, { 0,  0,  4, -2,  4,  0,  0,  0 } },
      {     0.11,   0.00, { 0,  0,  2, -2,  4,  0,  0,  0 } },
      {    -0.11,   0.00, { 1,  0, -2,  0, -3,  0,  0,  0 } },
      {    -0.11,   0.00, { 1,  0, -2,  0, -1,  0,  0,  0 } }
   };
   static const Term t1[] =
   {
      {    -0.07,   3.57, { 0,  0,  0,  0,  2,  0,  0,  0 } },
      {     1.73,  -0.03, { 0,  0,  0,  0,  1,  0,  0,  0 } },
      {     0.00,   0.48, { 0,  0,  2, -2,  3,  0,  0,  0 } }
   };
   static const Term t2[] =
   {
      {   743.52,  -0.17, { 0,  0,  0,  0,  1,  0,  0,  0 } },
      {    56.91,   0.06, { 0,  0,  2, -2,  2,  0,  0,  0 } },
      {     9.84,  -0.01, { 0,  0,  2,  0,  2,  0,  0,  0 } },
      {    -8.85,   0.01, { 0,  0,  0,  0,  2,  0,  0,  0 } },
      {    -6.38,  -0.05, { 0,  1,  0,  0,  0,  0,  0,  0 } },
      {    -3.07,   0.00, { 1,  0,  0,  0,  0,  0,  0,  0 } },
      {     2.23,   0.00, { 0,  1,  2, -2,  2,  0,  0,  0 } },
      {     1.67,   0.00, { 0,  0,  2,  0,  1,  0,  0,  0 } },
      {     1.30,   0.00, { 1,  0,  2,  0,  2,  0,  0,  0 } },
      {     0.93,   0.00, { 0,  1, -2,  2, -2,  0,  0,  0 } },
      {     0.68,   0.00, { 1,  0,  0, -2,  0,  0,  0,  0 } },
      {    -0.55,   0.00, { 0,  0,  2, -2,  1,  0,  0,  0 } },
      {     0.53,   0.00, { 1,  0, -2,  0, -2,  0,  0,  0 } },
      {    -0.27,   0.00, { 0,  0,  0,  2,  0,  0,  0,  0 } },
      {    -0.27,   0.00, { 1,  0,  0,  0,  1,  0,  0,  0 } },
      {    -0.26,   0.00, { 1,  0, -2, -2, -2,  0,  0,  0 } },
      {    -0.25,   0.00, { 1,  0,  0,  0, -1,  0,  0,  0 } },
      {     0.22,   0.00, { 1,  0,  2,  0,  1,  0,  0,  0 } },
      {    -0.21,   0.00, { 2,  0,  0, -2,  0,  0,  0,  0 } },
      {     0.20,   0.00, { 2,  0, -2,  0, -1,  0,  0,  0 } },
      {     0.17,   0.00, { 0,  0,  2,  2,  2,  0,  0,  0 } },
      {     0.13,   0.00, { 2,  0,  2,  0,  2,  0,  0,  0 } },
      {    -0.13,   0.00, { 2,  0,  0,  0,  0,  0,  0,  0 } },
      {    -0.12,   0.00, { 1,  0,  2, -2,  2,  0,  0,  0 } },
      {    -0.11,   0.00, { 0,  0,  2,  0,  0,  0,  0,  0 } }
   };
   static const Term t3[] =
   {
      {     0.30, -23.42, { 0,  0,  0,  0,  1,  0,  0,  0 } },
      {    -0.03,  -1.46, { 0,  0,  2, -2,  2,  0,  0,  0 } },
      {    -0.01,  -0.25, { 0,  0,  2,  0,  2,  0,  0,  0 } },
      {     0.00,   0.23, { 0,  0,  0,  0,  2,  0,  0,  0 } }
   };
   static const Term t4[] =
   {
      {    -0.26,  -0.01, { 0,  0,  0,  0,  1,  0,  0,  0 } }
   };

   static const Term* t[ 6 ] = { t0, t1, t2, t3, t4, nullptr };
   static const int   n[ 6 ] = { ItemsInArray( t0 ), ItemsInArray( t1 ),
                                 ItemsInArray( t2 ), ItemsInArray( t3 ),
                                 ItemsInArray( t4 ), 0 };

   /*
    * Non-periodic terms, values in arcseconds.
    * Source: ESAsA Eq. 6.53
    * N.B: There is a known error in the third edition (2013) of ESAsA:
    *    On page 229, Eq. 6.53, the last line of Equation 6.53: the factor
    *    -0.7257411 should be -0.07257411.
    */
   static const double s0[ 6 ] = { 9.4e-5, 3.80865e-3, -1.2268e-4, -7.257411e-2, 2.798e-5, 1.562e-5 };

   /*
    * Lunisolar and planetary arguments.
    * Source: SOFA release 2018-01-30
    */
   const double f[ 8 ] =
   {
      // Mean anomaly of the Moon
      Mod2Pi( AsRad( Poly( T, { 485868.249036, 1717915923.2178, 31.8792, 0.051635, -0.00024470 } ) ) ),

      // Mean anomaly of the Sun
      Mod2Pi( AsRad( Poly ( T, { 1287104.793048, 129596581.0481, -0.5532, 0.000136, -0.00001149 } ) ) ),

      // Mean longitude of the Moon minus that of the ascending node
      Mod2Pi( AsRad( Poly( T, { 335779.526232, 1739527262.8478, -12.7512, -0.001037, 0.00000417 } ) ) ),

      // Mean elongation from the Moon to the Sun
      Mod2Pi( AsRad( Poly( T, { 1072260.703692, 1602961601.2090, -6.3706, 0.006593, -0.00003169 } ) ) ),

      // Mean longitude of the ascending node of the Moon
      Mod2Pi( AsRad( Poly( T, { 450160.398036, -6962890.5431, 7.4722, 0.007702, -0.00005939 } ) ) ),

      // Mean longitude of Venus
      Mod2Pi( 3.176146697 + 1021.3285546211 * T ),

      // Mean longitude of the Earth
      Mod2Pi( 1.753470314 + 628.3075849991 * T ),

      // Accumulated general precession in longitude
      (0.024381750 + 0.00000538691 * T) * T
   };

   double s[ 6 ];
   for ( int i = 0; i < 6; ++i )
   {
      double u = 0;
      for ( int j = n[i]; --j >= 0; )
      {
         double a = 0;
         for ( int k = 0; k < 8; ++k )
         {
            int q = t[i][j].k[k];
            if ( q != 0 )
               a += q*f[k];
         }
         u += t[i][j].s * Sin( a ) + t[i][j].c * Cos( a );
      }
      s[i] = s0[i] + u*1.0e-6;
   }
   return AsRad( Poly( T, s, 5 ) ) - X*Y/2;
}

// ----------------------------------------------------------------------------

bool Position::CanComputeApparentVisualMagnitude( const EphemerisFile::Handle& H ) const
{
   // Minor planets with known absolute magnitudes and phase coefficients.
   if ( H.H().IsDefined() )
      if ( H.G().IsDefined() )
         return true;

   if ( H.OriginId() == "SSB" )
   {
      // The main solar system bodies except Sun, Earth and Moon.
      switch ( H.ObjectId().Hash32() )
      {
      case 0x77ee4333: // Me
      case 0x15a0ab4d: // Me0
      case 0x37880029: // Ve
      case 0x8d916cbb: // Ve0
      case 0xf2fd2887: // Ma
      case 0xc26c7504: // Ma0
      case 0x199a744d: // Ju
      case 0xf6229e2d: // Ju0
      case 0xdca2ceaf: // Sa
      case 0xd207c79b: // Sa0
      case 0x482a514e: // Ur
      case 0x2b057643: // Ur0
      case 0xe6b3dd6d: // Ne
      case 0x21f8c294: // Ne0
      case 0x909f2663: // Pl
      case 0x9dc1f5e4: // Pl0
         return true;
      default:
         break;
      }

      return false;
   }

   if ( H.OriginId() == "Ju0" || H.OriginId() == "Ju" )
   {
      // The four Galilean satellites of Jupiter.
      int id;
      if ( H.ObjectId().TryToInt( id ) )
         return id >= 1 && id <= 4;
   }

   return false;
}

Optional<double> Position::ApparentVisualMagnitude( EphemerisFile::Handle& H )
{
   /*
    * Phase angle, or the angle between the observer-body and Sun-body vectors.
    */
   Vector r = True( H ); // observer-body vector
   Vector r0 = (m_Oh ? m_Oh : m_Eh) + r; // Sun-body vector
   double d = r.L2Norm() * r0.L2Norm();
   double i = ArcCos( (r * r0)/d );

   Optional<double> V;

   /*
    * For objects with available H and G values in ephemeris data, apply the
    * standard apparent magnitude algorithm for minor planets.
    * See ESAsA Section 10.4.3.
    */
   if ( H.H().IsDefined() )
   {
      if ( H.G().IsDefined() )
         if ( 0 <= i && Deg( i ) <= 120 )
         {
            // ESAsA Eq. 10.40
            double t2 = ArcTan( i/2 );
            double phi1 = Exp( -3.33*Pow( t2, 0.63 ) );
            double phi2 = Exp( -1.87*Pow( t2, 1.22 ) );
            // ESAsA Eq. 10.38
            V = H.H()() -2.5*Log( (1 - H.G()())*phi1 + H.G()()*phi2 );
         }
   }
   else
   {
      i = Deg( i );

      /*
       * For Mercury, Venus, Mars, Jupiter, Saturn and Neptune, apply equations
       * from:
       *
       * https://arxiv.org/pdf/1808.01973.pdf
       * Anthony Mallama, James L. Hilton, Computing Apparent Planetary
       * Magnitudes for The Astronomical Almanac, revised 2018 June 21.
       *
       * For Saturn, we compute the apparent magnitude taking into account
       * light reflected by the rings.
       *
       * For Uranus, Pluto and the Galilean satellites of Jupiter, apply
       * coefficients taken from ESAsA Table 10.6. See also Errata in the ESAsA
       * (3rd edition, 1st printing), last update of 24 April 2018.
       */
      if ( H.OriginId() == "SSB" )
      {
         switch ( H.ObjectId().Hash32() )
         {
         case 0x77ee4333: // Me
         case 0x15a0ab4d: // Me0
            if ( 2 <= i && i <= 170 )
               V = Poly( i, {-0.613, +6.3280e-02, -1.6336e-03, +3.3644e-05, -3.4265e-07, +1.6893e-09, -3.0334e-12} );
            break;
         case 0x37880029: // Ve
         case 0x8d916cbb: // Ve0
            if ( 0 < i && i <= 163.7 )
               V = Poly( i, {-4.384, -1.044e-03, +3.687e-04, -2.814e-06, +8.938e-09} );
            else if ( 163.7 < i && i < 179 )
               V = Poly( i, {236.05828, -2.81914, +8.39034e-03} );
            break;
         case 0xf2fd2887: // Ma
         case 0xc26c7504: // Ma0
            if ( i <= 50 )
               V = -1.601 + 2.267e-02*i - 1.302e-04*i*i;
            else
               V = -0.367 - 0.02573*i + 0.0003445*i*i;
            break;
         case 0x199a744d: // Ju
         case 0xf6229e2d: // Ju0
            if ( i <= 12 )
               V = -9.395 - 3.7e-04*i + 6.16e-04*i*i;
            else
               V = -9.428 - 2.5*Log( Poly( i/180, {1.0, -1.507, -0.363, -0.062, +2.809, -1.876} ) );
            break;
         case 0xdca2ceaf: // Sa
         case 0xd207c79b: // Sa0
            {
               if ( i < 6.5 )
               {
                  // The following procedure to compute saturnicentric
                  // latitudes of the Earth and the Sun has been adapted from:
                  // Jean Meeus (1991), Astronomical Algorithms, Chapter 44.
                  double ea = EpsA();
                  Vector R0 = EquatorialToEcliptic( m_U + (m_Oh ? m_Oh : m_Eh), ea );
                  double d0 = R0.L2Norm();
                  double N  = Rad( 113.6655 + 0.8771*m_TT );
                  // Heliocentric longitude and latitude of Saturn, corrected for
                  // the Sun's aberration as seen from Saturn.
                  double ls, bs; R0.ToSpherical( ls, bs );
                  ls -= Rad( 0.01759 )/d0;
                  bs -= Rad( 0.000764 )*Cos( ls - N )/d0;
                  double ir = Rad(  28.075216 - 0.012998*m_TT + 0.000004*m_TT*m_TT );
                  double si, ci; SinCos( ir, si, ci );
                  double Or = Rad( 169.508470 + 1.394681*m_TT + 0.000412*m_TT*m_TT );
                  // Geocentric longitude and latitude of Saturn.
                  double le, be; EquatorialToEcliptic( m_U, ea ).ToSpherical( le, be );
                  // Saturnicentric latitude of the Earth.
                  double BE = ArcSin( si*Cos( be )*Sin( le - Or ) - ci*Sin( be ) );
                  // Saturnicentric latitude of the Sun.
                  double BS = ArcSin( si*Cos( bs )*Sin( ls - Or ) - ci*Sin( bs ) );
                  // Effective inclination of the rings plane.
                  double B  = ((BE < 0) == (BS < 0)) ? Sqrt( BE*BS ) : 0.0;
                  if ( Deg( B ) < 27 )
                  {
                     double sB = Sin( B );
                     V = -8.914 - 1.825*sB + 0.026*i - 0.378*sB*Exp( -2.25*i );
                  }
                  else
                     V = -8.95 - 3.7e-4*i + 6.16e-4*i*i;
               }
               else
                  V = Poly( i, {-8.94, 2.446e-4, +2.672e-4, -1.506e-6, +4.767e-9} );
            }
            break;
         case 0x482a514e: // Ur
         case 0x2b057643: // Ur0
            V = -7.19 + 0.002*i;
            break;
         case 0xe6b3dd6d: // Ne
         case 0x21f8c294: // Ne0
            {
               int y, m, d; double f;
               m_tt.GetComplexTime( y, m, d, f );
               double year = y + m/12.0 + (d + f)/365.25;
               if ( year > 2000 )
               {
                  if ( i <= 1.9 )
                     V = -7.00;
                  else
                     V = -7.00 + 7.944e-3*i + 9.617e-5*i*i;
               }
               else if ( year < 1980 )
                  V = -6.89;
               else
                  V = -6.89 - 0.0054*(year - 1980);
            }
            break;
         case 0x909f2663: // Pl
         case 0x9dc1f5e4: // Pl0
            V = -1.01; //+ 0.041*i;
            break;
         }
      }
      else if ( H.OriginId() == "Ju0" || H.OriginId() == "Ju" )
      {
         int id;
         if ( H.ObjectId().TryToInt( id ) )
            switch ( id )
            {
            case 1: // Io
               V = -1.68 + 0.46*i -0.0010*i*i;
               break;
            case 2: // Europa
               V = -1.41 + 0.0312*i - 0.00125*i*i;
               break;
            case 3: // Ganymede
               V = -2.09 + 0.323*i - 0.00066*i*i;
               break;
            case 4: // Callisto
               V = -1.05 +0.078*i - 0.00274*i*i;
               break;
            }
      }
   }

   // Observed visual magnitude, ESAsA Eq. 10.4, Eq. 10.41.
   if ( V.IsDefined() )
      return V() + 5*Log( d );

   // Unable to compute.
   return Optional<double>();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Position.cpp - Released 2019-01-21T12:06:21Z
