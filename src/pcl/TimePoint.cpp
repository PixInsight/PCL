//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/TimePoint.cpp - Released 2017-08-01T14:23:38Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/Defs.h>

#ifdef __PCL_WINDOWS
#  include <windows.h>
#else
#  include <sys/time.h>
#endif

#include <time.h>

#include <pcl/AutoLock.h>
#include <pcl/File.h>
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
// EOF pcl/TimePoint.cpp - Released 2017-08-01T14:23:38Z
