//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/TimePoint.cpp - Released 2018-12-12T09:24:30Z
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

#include <pcl/Defs.h>

#ifdef __PCL_WINDOWS
#  include <windows.h>
#else
#  include <sys/time.h>
#endif

#include <time.h>

#include <pcl/AkimaInterpolation.h>
#include <pcl/AutoLock.h>
#include <pcl/EphemerisFile.h>
#include <pcl/TimePoint.h>

// ----------------------------------------------------------------------------

#ifdef __PCL_WINDOWS

/*
 * Implement gettimeofday() on Windows.
 */
static int gettimeofday( timeval* tv, void* )
{
   if ( tv != nullptr )
   {
      FILETIME ft;
      ::GetSystemTimeAsFileTime( &ft );

      unsigned __int64 t = ft.dwHighDateTime;
      t <<= 32;
      t |= ft.dwLowDateTime;

      // A FILETIME is the number of 100-nanosecond intervals since 1601-01-01.
      // Convert it to the UNIX epoch 1970-01-01.
      t -= 116444736000000000Ui64; // offset from the UNIX epoch in 0.1 us
      t /= 10;                     // convert to microseconds
      tv->tv_sec = (long)(t / 1000000UL);
      tv->tv_usec = (long)(t % 1000000UL);
   }

   return 0;
}

#endif   // __PCL_WINDOWS

// ----------------------------------------------------------------------------

namespace pcl
{

// ----------------------------------------------------------------------------

TimePoint::TimePoint( time_t time )
{
   tm ut;
   {
      static Mutex mutex;
      volatile AutoLock lock( mutex );
      ut = *::gmtime( &time );
   }
   ComplexTimeToJD( m_jdi, m_jdf,
      ut.tm_year+1900, ut.tm_mon+1, ut.tm_mday, (ut.tm_hour + (ut.tm_min + ut.tm_sec/60.0)/60)/24 );
}

// ----------------------------------------------------------------------------

TimePoint::TimePoint( const FileTime& fileTime )
{
   ComplexTimeToJD( m_jdi, m_jdf, fileTime.year, fileTime.month, fileTime.day,
                    (fileTime.hour + (fileTime.minute + (fileTime.second + 0.001*fileTime.milliseconds)/60)/60)/24 );
}

// ----------------------------------------------------------------------------

static AkimaInterpolation<double> s_deltaT;
static TimePoint                  s_deltaTStart;
static TimePoint                  s_deltaTEnd;
static AtomicInt                  s_deltaTInitialized;
static Mutex                      s_deltaTMutex;

double TimePoint::DeltaT() const
{
   if ( !s_deltaTInitialized )
   {
      /*
       * Load the Delta T plain text database, thread-safe.
       */
      volatile AutoLock lock( s_deltaTMutex );
      if ( s_deltaTInitialized.Load() == 0 )
      {
         Array<double> T, D;
         IsoStringList lines = File::ReadLines( EphemerisFile::DeltaTDataFilePath() );
         for ( IsoString line : lines )
         {
            line.Trim();
            if ( line.IsEmpty() || line.StartsWith( '/' ) ) // empty line or comment
               continue;
            IsoStringList tokens;
            line.Break( tokens, ' ', true/*trim*/ );
            tokens.Remove( IsoString() );
            TimePoint t;
            double d;
            switch ( tokens.Length() )
            {
            case 2:
               // YY.yyy DeltaT
               {
                  double y = tokens[0].ToDouble();
                  t = TimePoint( TruncInt( y ), 1, 1.0 ) + Frac( y )*365.25;
                  d = tokens[1].ToDouble();
               }
               break;
            case 4:
               // YY MM DD.ddd DeltaT
               t = TimePoint( tokens[0].ToInt(), tokens[1].ToInt(), tokens[2].ToDouble() );
               d = tokens[3].ToDouble();
               break;
            default:
               // Invalid format, ignore.
               continue;
            }

            // Make sure that dates are valid and lines are sorted in ascending
            // date order.
            if ( t.IsValid() )
            {
               if ( T.IsEmpty() )
                  s_deltaTStart = t;
               else if ( t <= s_deltaTEnd )
                  continue;

               T << t.YearsSinceJ2000();
               D << d;
               s_deltaTEnd = t;
            }
         }

         // Check for reasonable minimum data.
         if ( T.Length() < 10 )
            throw Error( "Insufficient DeltaT data items." );
         if ( (s_deltaTEnd - s_deltaTStart) < 365 )
            throw Error( "The DeltaT data covers an insufficient time span." );

         s_deltaT.Initialize( Vector( T.Begin(), T.Length() ), Vector( D.Begin(), D.Length() ) );
         s_deltaTInitialized.Store( 1 );
      }
   }

   /*
    * Return an interpolated value when possible.
    */
   if ( *this >= s_deltaTStart )
      if ( *this <= s_deltaTEnd )
         return s_deltaT( YearsSinceJ2000() );

   /*
    * Use Espenak/Meeus polynomials otherwise.
    */
   int year, month, dum1;
   double dum2;
   GetComplexTime( year, month, dum1, dum2 );
   double y = year + (month - 0.5)/12;

   if ( year > 2005 )
   {
      if ( year <= 2050 )
         return Poly( y - 2000, { 62.92, 0.32217, 0.005589 } );

      double u = (y - 1820)/100;

      if ( year <= 2150 )
         return -20 + 32*u*u - 0.5628*(2150 - y);

      return -20 + 32*u*u;
   }

   if ( year < -500 )
      return Poly( (y - 1820)/100, { -20.0, 1.0, 32.0 } );

   if ( year <= +500 )
      return Poly( y/100, { 10583.6, -1014.41, 33.78311, -5.952053, -0.1798452, 0.022174192, 0.0090316521 } );

   if ( year <= 1600 )
      return Poly( (y - 1000)/100, { 1574.2, -556.01, 71.23472, 0.319781, -0.8503463, -0.005050998, 0.0083572073 } );

   if ( year <= 1700 )
      return Poly( y - 1600, { 120.0, -0.9808, -0.01532, 1.0/7129 } );

   if ( year <= 1800 )
      return Poly( y - 1700, { 8.83, 0.1603, -0.0059285, 0.00013336, -1.0/1174000 } );

   if ( year <= 1860 )
      return Poly( y - 1800, { 13.72, -0.332447, 0.0068612, 0.0041116, -0.00037436, 0.0000121272, -0.0000001699, 0.000000000875 } );

   if ( year <= 1900 )
      return Poly( y - 1860, { 7.62, 0.5737, -0.251754, 0.01680668, -0.0004473624, 1.0/233174 } );

   if ( year <= 1920 )
      return Poly( y - 1900, { -2.79, 1.494119, -0.0598939, 0.0061966, -0.000197 } );

   if ( year <= 1941 )
      return Poly( y - 1920, { 21.20, 0.84493, -0.076100, 0.0020936 } );

   if ( year <= 1961 )
      return Poly( y - 1950, { 29.07, 0.407, -1.0/233, 1.0/2547 } );

   if ( year <= 1986 )
      return Poly( y - 1975, { 45.45, 1.067, -1.0/260, -1.0/718 } );

   //if ( year <= 2005 )
   return Poly( y - 2000,  { 63.86, 0.3345, -0.060374, 0.0017275, 0.000651814, 0.00002373599 } );
}

// ----------------------------------------------------------------------------

struct DeltaATItem
{
   int    jdi;
   double DeltaAT;
   double D1;
   double D2;
};

static Array<DeltaATItem> s_deltaAT;
static TimePoint          s_deltaATStart;
static TimePoint          s_deltaATEnd;
static AtomicInt          s_deltaATInitialized;
static Mutex              s_deltaATMutex;

double TimePoint::DeltaAT() const
{
   if ( !s_deltaATInitialized )
   {
      /*
       * Load the Delta AT plain text database, thread-safe.
       */
      volatile AutoLock lock( s_deltaATMutex );
      if ( s_deltaATInitialized.Load() == 0 )
      {
         IsoStringList lines = File::ReadLines( EphemerisFile::DeltaATDataFilePath() );
         for ( IsoString line : lines )
         {
            line.Trim();
            if ( line.IsEmpty() || line.StartsWith( '/' ) ) // empty line or comment
               continue;
            IsoStringList tokens;
            line.Break( tokens, ' ', true/*trim*/ );
            tokens.Remove( IsoString() );
            if ( tokens.Length() < 2 ) // malformed
               continue;
            TimePoint t( tokens[0].ToDouble() );
            double d = tokens[1].ToDouble();
            double d1 = 0, d2 = 0;
            if ( tokens.Length() > 2 )
            {
               if ( tokens.Length() != 4 ) // malformed
                  continue;
               d1 = tokens[2].ToDouble();
               d2 = tokens[3].ToDouble();
            }

            // Make sure that dates are valid and lines are sorted in ascending
            // date order.
            if ( t.IsValid() )
            {
               if ( s_deltaAT.IsEmpty() )
                  s_deltaATStart = t;
               else if ( t.JDI() <= s_deltaATEnd.JDI() )
                  continue;

               s_deltaAT << DeltaATItem{ t.JDI(), d, d1, d2 };
               s_deltaATEnd = t;
            }
         }

         // UTC does not exist before 1960.0
         if ( s_deltaATStart < TimePoint( 1960, 1, 1.0 ) )
            throw Error( "Invalid DeltaAT time span." );

         // Check for reasonable data.
         if ( s_deltaAT.Length() < 40 )
            throw Error( "Insufficient DeltaAT data items." );
         if ( (s_deltaATEnd - s_deltaATStart) < 40*365.25 )
            throw Error( "The DeltaAT data covers an insufficient time span." );

         s_deltaATInitialized.Store( 1 );
      }
   }

   if ( m_jdi < 2436934 ) // 1960
      return 0; // pre-UTC!

   for ( int i = int( s_deltaAT.Length() ); --i >= 0; )
      if ( m_jdi >= s_deltaAT[i].jdi )
      {
         if ( m_jdi >= 2441317 ) // 1972
            return s_deltaAT[i].DeltaAT;
         return s_deltaAT[i].DeltaAT + (m_jdi + m_jdf - 2400000.5 - s_deltaAT[i].D1)*s_deltaAT[i].D2;
      }

   return 0;
}

// ----------------------------------------------------------------------------

double TimePoint::SystemOffsetFromUTC() const
{
   time_t t = time_t( SecondsSinceUNIXEpoch() );
   tm ut, lt;
   {
      static Mutex mutex;
      volatile AutoLock lock( mutex );
      ut = *::gmtime( &t );
      lt = *::localtime( &t );
   }

   double udh = ut.tm_hour + (ut.tm_min + ut.tm_sec/60.0)/60;
   double ldh = lt.tm_hour + (lt.tm_min + lt.tm_sec/60.0)/60;
   if ( ut.tm_year == lt.tm_year )
      if ( ut.tm_mon == lt.tm_mon )
         if ( ut.tm_mday == lt.tm_mday )
            return ldh - udh;

   return Round( 24 * (ComplexTimeToJD( lt.tm_year+1900, lt.tm_mon+1, lt.tm_mday, ldh/24 )
                     - ComplexTimeToJD( ut.tm_year+1900, ut.tm_mon+1, ut.tm_mday, udh/24 )), 6 );
}

// ----------------------------------------------------------------------------

TimePoint TimePoint::Now()
{
   timeval tv;
   ::gettimeofday( &tv, nullptr );
   // 1970-01-01 = JD 2440587.5
   return TimePoint( 2440587, 0.5 + (tv.tv_sec + 1.0e-06*tv.tv_usec)/86400 );
}

// ----------------------------------------------------------------------------

static const char* s_months[] = { "January", "February", "March", "April",
                                  "May", "June", "July", "August",
                                  "September", "October", "November", "December" };

template <class S>
static S Format( const TimePoint& t, const S& format )
{
   if ( !t.IsValid() )
      return S();

   int year, month, day, hour, minute;
   double seconds;
   t.GetComplexTime( year, month, day, hour, minute, seconds );

   S result;
   for ( typename S::const_iterator i = format.Begin(); i < format.End(); ++i )
      if ( *i == '%' )
      {
         if ( ++i == format.End() )
            break;

         switch ( *i )
         {
         case 'Y':
            result.AppendFormat( "%d", year );
            break;
         case 'M':
            result.AppendFormat( "%02d", month );
            break;
         case 'n':
            result.AppendFormat( "%d", month );
            break;
         case 'N':
            result.Append( s_months[month] );
            break;
         case 'D':
            result.AppendFormat( "%02d", day );
            break;
         case 'd':
            result.AppendFormat( "%d", day );
            break;
         case 'h':
            result.AppendFormat( "%02d", hour );
            break;
         case 'm':
            result.AppendFormat( "%02d", minute );
            break;
         case 's':
            {
               typename S::const_iterator j = i;
               if ( ++j < format.End() && S::char_traits::IsDigit( *j ) )
               {
                  int n = *j - '0';
                  if ( n == 0 )
                  {
                     int si = RoundInt( seconds );
                     if ( si < 60 )
                        result.AppendFormat( "%02d", si );
                     else
                        result.Append( "59" );
                  }
                  else
                  {
                     int si = TruncInt( seconds );
                     int e = TruncInt( Pow10I<double>()( n ) );
                     double f = Frac( seconds ) * e;
                     int sf = RoundInt( f );
                     if ( sf == e )
                        sf = TruncInt( f );
                     result.AppendFormat( "%02d.%0*d", si, n, sf );
                  }
                  i = j;
               }
               else
                  result.AppendFormat( "%02d", TruncInt( seconds ) );
            }
            break;
         }
      }
      else if ( *i == '\\' )
      {
         if ( ++i < format.End() )
            result.Append( *i );
      }
      else
         result.Append( *i );

   return result;
}

IsoString TimePoint::ToIsoString( const IsoString& format ) const
{
   return Format( *this, format );
}

String TimePoint::ToString( const String& format ) const
{
   return Format( *this, format );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/TimePoint.cpp - Released 2018-12-12T09:24:30Z
