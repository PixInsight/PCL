//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0837
// ----------------------------------------------------------------------------
// pcl/TimePoint.h - Released 2017-06-09T08:12:42Z
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

#ifndef __PCL_TimePoint_h
#define __PCL_TimePoint_h

/// \file pcl/TimePoint.h

#include <pcl/Defs.h>

#include <pcl/Math.h>
#include <pcl/String.h>

/*
 * N.B.: Remove conflicting names defined by 3rd-party packages used by some
 * standard modules.
 */
#ifdef J2000
# undef J2000
#endif
#ifdef B1950
# undef B1950
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup time_representation Time Representation and Utilities
 */

// ----------------------------------------------------------------------------

struct FileTime;

/*!
 * \class TimePoint
 * \brief A time point in an arbitrary time scale
 *
 * %TimePoint represents an instant in any time scale. The internal
 * representation of time consists of the separate integer and fractional
 * components of a Julian day number. This guarantees a resolution better than
 * one nanosecond and fast time arithmetic operations.
 *
 * \ingroup time_representation
 * \sa ISO8601ConversionOptions, IsoString::ParseISO8601DateTime()
 */
class PCL_CLASS TimePoint
{
public:

   /*!
    * Default constructor. Yields an invalid %TimePoint object that cannot be
    * used until properly initialized.
    */
   TimePoint() = default;

   /*!
    * Constructs a %TimePoint object for a given instant represented as a
    * Julian day number.
    */
   TimePoint( double jd ) :
      m_jdi( TruncInt( jd ) ), m_jdf( Frac( jd ) )
   {
   }

   /*!
    * Constructs a %TimePoint object from the separate integer and fractional
    * parts of a Julian day number, \a jdi and \a jdf respectively.
    */
   TimePoint( int jdi, double jdf ) :
      m_jdi( jdi ), m_jdf( jdf )
   {
      Normalize();
   }

   /*!
    * Constructs a %TimePoint object from separate date components.
    */
   TimePoint( int year, int month, double days )
   {
      ComplexTimeToJD( m_jdi, m_jdf, year, month, TruncInt( days ), Frac( days ) );
   }

   /*!
    * Constructs a %TimePoint object from separate date components plus a day
    * fraction.
    */
   TimePoint( int year, int month, int day, double dayFraction )
   {
      ComplexTimeToJD( m_jdi, m_jdf, year, month, day, dayFraction );
   }

   /*!
    * Constructs a %TimePoint object from separate date and time components.
    */
   TimePoint( int year, int month, int day, int hour, int minute, double seconds )
   {
      ComplexTimeToJD( m_jdi, m_jdf, year, month, day, (hour + (minute + seconds/60)/60)/24 );
   }

   /*!
    * Constructs a %TimePoint object from an ISO 8601 extended date/time
    * representation specified as an 8-bit ISO/IEC-8859-1 string.
    *
    * If the source representation specifies a nonzero time zone, it will be
    * subtracted from the time point represented by this object. In other
    * words, the newly constructed object will always transport a time point in
    * the UTC time scale.
    */
   TimePoint( const IsoString& dateTime )
   {
      int year, month, day;
      double dayf, tz;
      dateTime.ParseISO8601DateTime( year, month, day, dayf, tz );
      ComplexTimeToJD( m_jdi, m_jdf, year, month, day, dayf - tz/24 );
   }

   /*!
    * Constructs a %TimePoint object from an ISO 8601 extended date/time
    * representation specified as a UTF-16 string.
    *
    * If the source representation specifies a nonzero time zone, it will be
    * subtracted from the time point represented by this object. In other
    * words, the newly constructed object will always transport a time point in
    * the UTC time scale.
    */
   TimePoint( const String& dateTime )
   {
      int year, month, day;
      double dayf, tz;
      dateTime.ParseISO8601DateTime( year, month, day, dayf, tz );
      ComplexTimeToJD( m_jdi, m_jdf, year, month, day, dayf - tz/24 );
   }

   /*!
    * Constructs a %TimePoint object from a time_t value. The specified \a time
    * value will be interpreted as UTC.
    */
   TimePoint( time_t time );

   /*!
    * Constructs a %TimePoint object from a FileTime object.
    *
    * \param fileTime   Reference to a FileTime structure, from which date and
    *                   time elements will be obtained to construct a
    *                   %TimePoint object.
    *
    * \param localTime  True to construct a %TimePoint directly from %FileTime
    *                   components, which normally represent a local time on
    *                   systems that support time zones. False to subtract the
    *                   current time zone offset, if available, to build a
    *                   %TimePoint object as UTC without offset. This parameter
    *                   is false by default.
    */
   TimePoint( const FileTime& fileTime, bool localTime = false );

   /*!
    * Copy constructor.
    */
   TimePoint( const TimePoint& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   TimePoint& operator =( const TimePoint& ) = default;

   /*!
    * Returns true iff this is a valid %TimePoint instance. An invalid
    * %TimePoint does not represent a valid time point, generates empy string
    * representations, and cannot be used until properly initialized.
    */
   constexpr bool IsValid() const
   {
      return ((m_jdi > 0) ? m_jdi : -m_jdi) < 2147438065;
   }

   /*!
    * Extracts the individual date and day fraction components.
    *
    * \param[out] year     Reference to a variable that will receive the year
    *                      component of this time point.
    *
    * \param[out] month    Reference to a variable that will receive the month
    *                      component of this time point, in the range [1,12].
    *
    * \param[out] day      Reference to a variable that will receive the day
    *                      component of this time point, in the range [1,31].
    *
    * \param[out] dayf     Reference to a variable that will receive the day
    *                      fraction component of this time point, in the range
    *                      [0,1).
    */
   void GetComplexTime( int& year, int& month, int& day, double& dayf ) const
   {
      JDToComplexTime( year, month, day, dayf, m_jdi, m_jdf );
   }

   /*!
    * Extracts the individual date and time components.
    *
    * \param[out] year     Reference to a variable that will receive the year
    *                      component of this time point.
    *
    * \param[out] month    Reference to a variable that will receive the month
    *                      component of this time point, in the range [1,12].
    *
    * \param[out] day      Reference to a variable that will receive the day
    *                      component of this time point, in the range [1,31].
    *
    * \param[out] hour     Reference to a variable that will receive the hour
    *                      component of this time point, in the range [0,23].
    *
    * \param[out] minute   Reference to a variable that will receive the minute
    *                      component of this time point, in the range [0,59].
    *
    * \param[out] seconds  Reference to a variable that will receive the
    *                      seconds component of this time point, in the range
    *                      [0,60).
    */
   void GetComplexTime( int& year, int& month, int& day, int& hour, int& minute, double& seconds ) const
   {
      double dayf;
      JDToComplexTime( year, month, day, dayf, m_jdi, m_jdf );
      double h = Frac( dayf )*24;
      double m = Frac( h )*60;
      seconds = Frac( m )*60;
      minute = TruncInt( m );
      hour = TruncInt( h );
   }

   /*!
    * Returns the year component of this time point.
    */
   int Year() const
   {
      int year, foo; double bar;
      JDToComplexTime( year, foo, foo, bar, m_jdi, m_jdf );
      return year;
   }

   /*!
    * Returns the month component of this time point in the range [1,12].
    */
   int Month() const
   {
      int foo, month; double bar;
      JDToComplexTime( foo, month, foo, bar, m_jdi, m_jdf );
      return month;
   }

   /*!
    * Returns the day component of this time point in the range [1,31].
    */
   int Day() const
   {
      int foo, day; double bar;
      JDToComplexTime( foo, foo, day, bar, m_jdi, m_jdf );
      return day;
   }

   /*!
    * Returns the day fraction component of this time point in the range [0,1).
    */
   double DayFraction() const
   {
      int foobar; double dayf;
      JDToComplexTime( foobar, foobar, foobar, dayf, m_jdi, m_jdf );
      return dayf;
   }

   /*!
    * Returns an ISO 8601 extended date/time representation of this time point
    * as a UTF-16 string.
    *
    * \param options Optional settings to control the representation of date
    *                and time in ISO 8601 extended format.
    *
    * \param tz      Time zone offset in hours. The default value is zero.
    *
    * If this is an invalid %TimePoint (for example, a default constructed
    * instance), this function returns an empty string.
    *
    * \sa ISO8601ConversionOptions, String::ParseISO8601DateTime(), IsValid()
    */
   String ToString( const ISO8601ConversionOptions& options = ISO8601ConversionOptions(), double tz = 0 ) const
   {
      return ToString( options, tz, (String*)0 );
   }

   /*!
    * Returns an ISO 8601 extended date/time representation of this time point
    * as a UTF-16 string.
    *
    * \param timeItems  Number of represented time items. Can be 0, 1, 2 or 3.
    *                   All items but the last one are represented as integer
    *                   values. The last item is represented as a floating
    *                   point value with the specified \a precision.
    *
    * \param precision  Number of decimal digits for the last represented time
    *                   item. The default value is three decimal digits.
    *
    * \param tz         Time zone offset in hours. The default value is zero.
    *
    * \param timeZone   Whether to append a time zone specifier to the ISO 8601
    *                   representation. The default value is true.
    *
    * \param zuluTime   Whether to append the 'Z' special time zone specifier
    *                   for UTC time without offset, or the '+00:00' specifier
    *                   otherwise. The default value is true.
    *
    * If this is an invalid %TimePoint (for example, a default constructed
    * instance), this function returns an empty string.
    *
    * \sa ISO8601ConversionOptions, String::ParseISO8601DateTime(), IsValid()
    */
   String ToString( unsigned timeItems, unsigned precision = 3, double tz = 0, bool timeZone = true, bool zuluTime = true ) const
   {
      return ToString( ISO8601ConversionOptions( timeItems, precision, timeZone, zuluTime ), tz );
   }

   /*!
    * Returns an ISO 8601 extended date/time representation of this time point
    * as an 8-bit ISO/IEC-8859-1 string.
    *
    * \param options Optional settings to control the representation of date
    *                and time in ISO 8601 extended format.
     *
    * \param tz      Time zone offset in hours. The default value is zero.
   *
    * If this is an invalid %TimePoint (for example, a default constructed
    * instance), this function returns an empty string.
    *
    * \sa ISO8601ConversionOptions, IsoString::ParseISO8601DateTime(), IsValid()
    */
   IsoString ToIsoString( const ISO8601ConversionOptions& options = ISO8601ConversionOptions(), double tz = 0 ) const
   {
      return ToString( options, tz, (IsoString*)0 );
   }

   /*!
    * Returns an ISO 8601 extended date/time representation of this time point
    * as an 8-bit ISO/IEC-8859-1 string.
    *
    * \param timeItems  Number of represented time items. Can be 0, 1, 2 or 3.
    *                   All items but the last one are represented as integer
    *                   values. The last item is represented as a floating
    *                   point value with the specified \a precision.
    *
    * \param precision  Number of decimal digits for the last represented time
    *                   item. The default value is three decimal digits.
    *
    * \param tz         Time zone offset in hours. The default value is zero.
    *
    * \param timeZone   Whether to append a time zone specifier to the ISO 8601
    *                   representation. The default value is true.
    *
    * \param zuluTime   Whether to append the 'Z' special time zone specifier
    *                   for UTC time without offset, or the '+00:00' specifier
    *                   otherwise. The default value is true.
    *
    * If this is an invalid %TimePoint (for example, a default constructed
    * instance), this function returns an empty string.
    *
    * \sa ISO8601ConversionOptions, IsoString::ParseISO8601DateTime(), IsValid()
    */
   IsoString ToIsoString( unsigned timeItems, unsigned precision = 3, double tz = 0, bool timeZone = true, bool zuluTime = true ) const
   {
      return ToIsoString( ISO8601ConversionOptions( timeItems, precision, timeZone, zuluTime ), tz );
   }

   /*!
    * Returns a custom representation of this time point as a UTF-16 string.
    *
    * \param format     A date and time format string. The following format
    *                   tokens will be replaced with formatted date and time
    *                   items, as described in the following table:\n
    *    \n
    *    <table border="1" cellpadding="4" cellspacing="0">
    *    <tr>
    *       <td>\%Y</td>
    *       <td>The four-digit year.</td>
    *    </tr>
    *    <tr>
    *       <td>\%M</td>
    *       <td>Zero padded, two-digit month number, '01' through '12'.</td>
    *    </tr>
    *    <tr>
    *       <td>\%n</td>
    *       <td>The month number, '1' through '12'.</td>
    *    </tr>
    *    <tr>
    *       <td>\%N</td>
    *       <td>The English month name, 'January' through 'December'.</td>
    *    </tr>
    *    <tr>
    *       <td>\%D</td>
    *       <td>Zero padded, two-digit day of month, '01' through '31'.</td>
    *    </tr>
    *    <tr>
    *       <td>\%d</td>
    *       <td>The day of the month, '1' through '31'.</td>
    *    </tr>
    *    <tr>
    *       <td>\%h</td>
    *       <td>Zero padded, two digits of hour, '00' through '23'.</td>
    *    </tr>
    *    <tr>
    *       <td>\%m</td>
    *       <td>Zero padded, two digits of minute, '00' through '59'.</td>
    *    </tr>
    *    <tr>
    *       <td>\%s</td>
    *       <td>Zero padded, two digits of the seconds truncated to an integer,
    *       '00' through '59'.</td>
    *    </tr>
    *    <tr>
    *       <td>\%s&lt;p&gt;</td>
    *       <td>Zero padded representation of seconds rounded or truncated to
    *       &lt;p&gt; decimal digits, with &lt;p&gt; in [0,9]. Truncation
    *       instead of rounding will we used if the rounded value is equal to
    *       60 seconds.</td>
    *    </tr>
    *    </table>\n
    *    Other characters in the format string will be included literally in
    *    the result. To include a percent sign '\%', escape it by preceding it
    *    with a backslash: "\\%". To include a backslash character, it must
    *    also be escaped as two consecutive slashes: "\\\\".
    *
    * If this is an invalid %TimePoint (for example, a default constructed
    * instance), this function returns an empty string.
    *
    * Example:
    *
    * \code
    * TimePoint t = TimePoint::Now();
    * std::cout << t.ToString( "%Y-%M-%D %h:%m:%s" ) << '\n'
    *           << t.ToString( "%Y-%M-%D %h:%m:%s2" ) << '\n'
    *           << t.ToString( "Today is: %N %d, %Y %h:%m:%s" ) << '\n';
    * \endcode
    */
   String ToString( const String& format ) const;

   /*!
    * Returns a custom representation of this time point as an 8-bit
    * ISO/IEC-8859-1 string.
    *
    * See ToString( const String& ) for detailed information on the format
    * string argument.
    *
    * If this is an invalid %TimePoint (for example, a default constructed
    * instance), this function returns an empty string.
    */
   IsoString ToIsoString( const IsoString& format ) const;

   IsoString ToIsoString( const char* format ) const
   {
      return ToIsoString( IsoString( format ) );
   }

   /*!
    * String conversion operator. Equivalent to ToString().
    */
   explicit operator String() const
   {
      return ToString();
   }

   /*!
    * IsoString conversion operator. Equivalent to ToIsoString().
    */
   explicit operator IsoString() const
   {
      return ToIsoString();
   }

   /*!
    * Returns the integer part of the Julian day number corresponding to this
    * time point.
    */
   constexpr int JDI() const
   {
      return m_jdi;
   }

   /*!
    * Returns the fractional part of the Julian day number corresponding to
    * this time point.
    */
   constexpr double JDF() const
   {
      return m_jdf;
   }

   /*!
    * Returns the Julian day number corresponding to this time point.
    *
    * The time point representation returned by this member function only has
    * millisecond resolution. For higher resolution time point representations
    * up to one nanosecond, use the separate integer and fractional components
    * given by JDI() and JDF().
    *
    * If this is an invalid %TimePoint (for example, a default constructed
    * instance), the value returned by this function is meaningless as a Julian
    * day number.
    */
   constexpr double JD() const
   {
      return m_jdi + m_jdf;
   }

   /*!
    * Adds the specified number of \a days to this time point. Returns a
    * reference to this object.
    */
   TimePoint& operator +=( double days )
   {
      m_jdf += days;
      Normalize();
      return *this;
   }

   /*!
    * Subtracts the specified number of \a days from this time point. Returns a
    * reference to this object.
    */
   TimePoint& operator -=( double days )
   {
      m_jdf -= days;
      Normalize();
      return *this;
   }

   /*!
    * Returns the time interval in days elapsed since the specified time point
    * \a t.
    */
   constexpr double DaysSince( const TimePoint& t ) const
   {
      return m_jdi-t.m_jdi + m_jdf-t.m_jdf;
   }

   /*!
    * Returns the time interval in Julian years (365.25 days) elapsed since the
    * specified time point \a t.
    */
   constexpr double YearsSince( const TimePoint& t ) const
   {
      return DaysSince( t )/365.25;
   }

   /*!
    * Returns the time interval in Julian centuries (36525 days) elapsed since
    * the specified time point \a t.
    */
   constexpr double CenturiesSince( const TimePoint& t ) const
   {
      return DaysSince( t )/36525;
   }

   /*!
    * Returns the time interval in seconds elapsed since the specified time
    * point \a t.
    */
   constexpr double SecondsSince( const TimePoint& t ) const
   {
      return DaysSince( t )*86400;
   }

   /*!
    * Returns the time interval in days elapsed since the standard J2000 epoch
    * (JD 2451545.0 = 2000 January 1.5).
    */
   constexpr double DaysSinceJ2000() const
   {
      return m_jdi-2451545 + m_jdf;
   }

   /*!
    * Returns the time interval in Julian years (365.25 days) elapsed since the
    * standard J2000 epoch (JD 2451545.0 = 2000 January 1.5).
    */
   constexpr double YearsSinceJ2000() const
   {
      return DaysSinceJ2000()/365.25;
   }

   /*!
    * Returns the time interval in Julian centuries (36525 days) elapsed since
    * the standard J2000 epoch (JD 2451545.0 = 2000 January 1.5).
    */
   constexpr double CenturiesSinceJ2000() const
   {
      return DaysSinceJ2000()/36525;
   }

   /*!
    * Returns the time interval in seconds elapsed since the standard J2000
    * epoch (JD 2451545.0 = 2000 January 1.5).
    */
   constexpr double SecondsSinceJ2000() const
   {
      return DaysSinceJ2000()*86400;
   }

   /*!
    * Returns the time interval in days elapsed since the standard UNIX epoch
    * (JD 2440587.5 = 1970 January 1.0).
    */
   constexpr double DaysSinceUNIXEpoch() const
   {
      return m_jdi-2440587 + m_jdf-0.5;
   }

   /*!
    * Returns the time interval in Julian years (365.25 days) elapsed since the
    * standard UNIX epoch (JD 2440587.5 = 1970 January 1.0).
    */
   constexpr double YearsSinceUNIXEpoch() const
   {
      return DaysSinceUNIXEpoch()/365.25;
   }

   /*!
    * Returns the time interval in Julian centuries (36525 days) elapsed since
    * the standard UNIX epoch (JD 2440587.5 = 1970 January 1.0).
    */
   constexpr double CenturiesSinceUNIXEpoch() const
   {
      return DaysSinceUNIXEpoch()/36525;
   }

   /*!
    * Returns the time interval in seconds elapsed since the standard UNIX
    * epoch (JD 2440587.5 = 1970 January 1.0).
    */
   constexpr double SecondsSinceUNIXEpoch() const
   {
      return DaysSinceUNIXEpoch()*86400;
   }

   /*!
    * Returns the time interval in milliseconds elapsed since the standard UNIX
    * epoch (JD 2440587.5 = 1970 January 1.0).
    */
   constexpr double MillisecondsSinceUNIXEpoch() const
   {
      return DaysSinceUNIXEpoch()*86400000;
   }

   /*!
    * Returns a %TimePoint object corresponding to the current date and time in
    * the UTC time scale.
    */
   static TimePoint Now();

   /*!
    * Returns a %TimePoint object corresponding to the standard J2000 epoch,
    * namely JD 2451545.0, corresponding to the midday of 2000 January 1.
    */
   static TimePoint J2000()
   {
      TimePoint t;
      t.m_jdi = 2451545;
      t.m_jdf = 0;
      return t;
   }

   /*!
    * Returns a %TimePoint object corresponding to the B1950.0 epoch, namely
    * JD 2433282.42345905, corresponding to 1949 December 31.92345905.
    */
   static TimePoint B1950()
   {
      TimePoint t;
      t.m_jdi = 2433282;
      t.m_jdf = 0.42345905;
      return t;
   }

   /*!
    * Returns a %TimePoint object corresponding to the standard UNIX epoch,
    * namely JD 2440587.5, corresponding to 1970 January 1.0.
    */
   static TimePoint UNIXEpoch()
   {
      TimePoint t;
      t.m_jdi = 2440587;
      t.m_jdf = 0.5;
      return t;
   }

   /*!
    * Returns the current time zone offset of the host machine, or the
    * difference <em>local standard time minus UTC</em>, in hours.
    */
   static double SystemTimeZone();

   /*!
    * Returns a %TimePoint object constructed from an ISO 8601 extended
    * date/time representation specified as an 8-bit ISO/IEC-8859-1 string. If
    * successful, stores the evaluated time zone offset, in hours, in the
    * specified variable \a tz. Otherwise this function throws the appropriate
    * Error exception.
    *
    * If the source representation specifies a nonzero time zone, it will be
    * subtracted from the time point constructed by this function. In other
    * words, the newly constructed object will always transport a time point in
    * the UTC time scale without offset.
    *
    * \sa TryFromString( TimePoint&, double&, const IsoString& ),
    * IsoString::ParseISO8601DateTime()
    */
   static TimePoint FromString( double& tz, const IsoString& dateTime )
   {
      int year, month, day;
      double dayf;
      dateTime.ParseISO8601DateTime( year, month, day, dayf, tz );
      return TimePoint( year, month, day, dayf - tz/24 );
   }

   /*!
    * Returns a %TimePoint object constructed from an ISO 8601 extended
    * date/time representation specified as a UTF-16 string. If successful,
    * stores the evaluated time zone offset, in hours, in the specified
    * variable \a tz. Otherwise this function throws the appropriate Error
    * exception.
    *
    * If the source representation specifies a nonzero time zone, it will be
    * subtracted from the time point constructed by this function. In other
    * words, the newly constructed object will always transport a time point in
    * the UTC time scale without offset.
    *
    * \sa TryFromString( TimePoint&, double&, const String& ),
    * String::ParseISO8601DateTime()
    */
   static TimePoint FromString( double& tz, const String& dateTime )
   {
      int year, month, day;
      double dayf;
      dateTime.ParseISO8601DateTime( year, month, day, dayf, tz );
      return TimePoint( year, month, day, dayf - tz/24 );
   }

   /*!
    * Returns a %TimePoint object constructed from an ISO 8601 extended
    * date/time representation specified as an 8-bit ISO/IEC-8859-1 string.
    *
    * If the source representation specifies a nonzero time zone, it will be
    * subtracted from the time point constructed by this function. In other
    * words, the newly constructed object will always transport a time point in
    * the UTC time scale without offset.
    *
    * \sa TryFromString( TimePoint&, const IsoString& ),
    * IsoString::ParseISO8601DateTime()
    */
   static TimePoint FromString( const IsoString& dateTime )
   {
      return TimePoint( dateTime );
   }

   /*!
    * Returns a %TimePoint object constructed from an ISO 8601 extended
    * date/time representation specified as a UTF-16 string.
    *
    * If the source representation specifies a nonzero time zone, it will be
    * subtracted from the time point constructed by this function. In other
    * words, the newly constructed object will always transport a time point in
    * the UTC time scale without offset.
    *
    * \sa TryFromString( TimePoint&, const String& ),
    * String::ParseISO8601DateTime()
    */
   static TimePoint FromString( const String& dateTime )
   {
      return TimePoint( dateTime );
   }

   /*!
    * Attempts to evaluate the specified 8-bit string as a date and time
    * specification in ISO 8601 extended format. If successful, stores the
    * evaluated UTC time point in the specified variable \a t, and the
    * evaluated time zone offset, in hours, in the variable \a tz, then returns
    * \c true.
    *
    * If the specified string cannot be evaluated as a valid date and time in
    * ISO 8601 extended format, this function returns \c false and does not
    * change the passed variables \a t and \a tz. This function does not throw
    * any parsing exceptions.
    *
    * If the source string contains a time zone offset, it is subtracted from
    * the evaluated time point. In other words, the %TimePoint value stored in
    * \a t is always represented in the UTC time scale without any offset.
    *
    * \sa FromString( const IsoString& ), IsoString::TryParseISO8601DateTime()
    */
   static bool TryFromString( TimePoint& t, double& tz, const IsoString& dateTime )
   {
      int year, month, day;
      double dayf;
      if ( dateTime.TryParseISO8601DateTime( year, month, day, dayf, tz ) )
      {
         t = TimePoint( year, month, day, dayf - tz/24 );
         return true;
      }
      return false;
   }

   /*!
    * Attempts to evaluate the specified UTF-16 string as a date and time
    * specification in ISO 8601 extended format. If successful, stores the
    * evaluated UTC time point in the specified variable \a t, and the
    * evaluated time zone offset, in hours, in the variable \a tz, then returns
    * \c true.
    *
    * If the specified string cannot be evaluated as a valid date and time in
    * ISO 8601 extended format, this function returns \c false and does not
    * change the passed variables \a t and \a tz. This function does not throw
    * any parsing exceptions.
    *
    * If the source string contains a time zone offset, it is subtracted from
    * the evaluated time point. In other words, the %TimePoint value stored in
    * \a t is always represented in the UTC time scale without any offset.
    *
    * \sa FromString( const String& ), String::TryParseISO8601DateTime()
    */
   static bool TryFromString( TimePoint& t, double& tz, const String& dateTime )
   {
      int year, month, day;
      double dayf;
      if ( dateTime.TryParseISO8601DateTime( year, month, day, dayf, tz ) )
      {
         t = TimePoint( year, month, day, dayf - tz/24 );
         return true;
      }
      return false;
   }

   /*!
    * Attempts to evaluate the specified 8-bit string as a date and time
    * specification in ISO 8601 extended format. If successful, stores the
    * evaluated UTC time point in the specified variable \a t, then returns
    * \c true.
    *
    * If the specified string cannot be evaluated as a valid date and time in
    * ISO 8601 extended format, this function returns \c false and does not
    * change the passed variable \a t. This function does not throw any parsing
    * exceptions.
    *
    * If the source string contains a time zone offset, it is subtracted from
    * the evaluated time point. In other words, the %TimePoint value stored in
    * \a t is always represented in the UTC time scale without any offset.
    *
    * \sa FromString( const IsoString& ), IsoString::TryParseISO8601DateTime()
    */
   static bool TryFromString( TimePoint& t, const IsoString& dateTime )
   {
      double tz;
      return TryFromString( t, tz, dateTime );
   }

   /*!
    * Attempts to evaluate the specified UTF-16 string as a date and time
    * specification in ISO 8601 extended format. If successful, stores the
    * evaluated UTC time point in the specified variable \a t, then returns
    * \c true.
    *
    * If the specified string cannot be evaluated as a valid date and time in
    * ISO 8601 extended format, this function returns \c false and does not
    * change the passed variable \a t. This function does not throw any parsing
    * exceptions.
    *
    * If the source string contains a time zone offset, it is subtracted from
    * the evaluated time point. In other words, the %TimePoint value stored in
    * \a t is always represented in the UTC time scale without any offset.
    *
    * \sa FromString( const String& ), String::TryParseISO8601DateTime()
    */
   static bool TryFromString( TimePoint& t, const String& dateTime )
   {
      double tz;
      return TryFromString( t, tz, dateTime );
   }

private:

   int32  m_jdi = int32_max; // invalid by default
   double m_jdf = 0.0;

   /*!
    * \internal
    * Ensures that the internal Julian day separate representation is optimal
    * in terms of time resolution by constraining JDF to [0,1).
    */
   void Normalize()
   {
      if ( Abs( m_jdf ) >= 1 )
      {
         m_jdi += TruncInt( m_jdf );
         m_jdf = Frac( m_jdf );
      }
   }

   template <class S>
   S ToString( const ISO8601ConversionOptions& options, double tz, S* ) const
   {
      if ( IsValid() )
      {
         int year, month, day;
         double dayf;
         JDToComplexTime( year, month, day, dayf, m_jdi, m_jdf );
         return S::ToISO8601DateTime( year, month, day, dayf, tz, options );
      }
      return S();
   }
};

/*
 * ### N.B.: TimePoint cannot have virtual member functions.
 *     This is because sizeof( TimePoint ) _must_ be equal to
 *     sizeof( uint32 ) + sizeof( uint64 ).
 */
struct PCL_AssertTimePointSize
{
   struct TestData { uint32 x; uint64 y; };
   static_assert( sizeof( TimePoint ) == sizeof( TestData ), "Invalid sizeof( TimePoint )" );
};

/*!
 * Returns true iff two %TimePoint objects, \a t1 and \a t2, represent the same
 * time point.
 * \ingroup time_representation
 */
inline constexpr bool operator ==( const TimePoint& t1, const TimePoint& t2 )
{
   return t1.JDI() == t2.JDI() && t1.JDF() == t2.JDF();
}

/*!
 * Returns true iff a %TimePoint object \a t1 represents the specified time
 * point \a jd2, expressed as a Julian day number.
 * \ingroup time_representation
 */
inline constexpr bool operator ==( const TimePoint& t1, double jd2 )
{
   return t1.JD() == jd2;
}

/*!
 * Returns true iff a %TimePoint object \a t2 represents the specified time
 * point \a jd1, expressed as a Julian day number. This operator implements the
 * commutative property of scalar time point equality.
 * \ingroup time_representation
 */
inline constexpr bool operator ==( double jd1, const TimePoint& t2 )
{
   return t2.JD() == jd1;
}

/*!
 * Returns true iff a %TimePoint object \a t1 represents a time point that
 * precedes the time point represented by another object \a t2.
 *
 * Invalid %TimePoint instances are always in the <em>infinite past:</em> if
 * \a t1 is invalid, this function returns true; if \a t1 is valid and \a t2 is
 * not valid, this function returns false.
 *
 * \ingroup time_representation
 */
inline constexpr bool operator <( const TimePoint& t1, const TimePoint& t2 )
{
   return !t1.IsValid() || t2.IsValid() && ((t1.JDI() != t2.JDI()) ? t1.JDI() < t2.JDI() : t1.JDF() < t2.JDF());
}

/*!
 * Returns true iff a %TimePoint object \a t1 represents a time point that
 * precedes another time point \a jd2, expressed as a Julian day number.
 *
 * Invalid %TimePoint instances are always in the <em>infinite past:</em> if
 * \a t1 is invalid, this function returns true.
 *
 * \ingroup time_representation
 */
inline constexpr bool operator <( const TimePoint& t1, double jd2 )
{
   return !t1.IsValid() || t1.JD() < jd2;
}

/*!
 * Returns true iff a time point \a jd1, expressed as a Julian day number,
 * precedes the time point represented by a %TimePoint object \a t2.
 *
 * Invalid %TimePoint instances are always in the <em>infinite past:</em> if
 * \a t2 is invalid, this function returns false.
 *
 * \ingroup time_representation
 */
inline constexpr bool operator <( double jd1, const TimePoint& t2 )
{
   return t2.IsValid() && t2.JD() < jd1;
}

/*!
 * Returns the time interval in days elapsed from the specified time point
 * \a t1 to another time point \a t2. The returned value is positive if \a t2
 * precedes \a t1, negative if \a t1 precedes \a t2.
 * \ingroup time_representation
 */
inline constexpr double operator -( const TimePoint& t1, const TimePoint& t2 )
{
   return (t1.JDI() - t2.JDI()) + (t1.JDF() - t2.JDF());
}

/*!
 * Returns the time point resulting after \a d days transcurred since a time
 * point \a t.
 * \ingroup time_representation
 */
inline TimePoint operator +( const TimePoint& t, double d )
{
   return TimePoint( t.JDI(), t.JDF()+d );
}

/*!
 * Returns the time point resulting after \a d days transcurred since a time
 * point \a t. This operator implements the commutative property of scalar time
 * point addition.
 * \ingroup time_representation
 */
inline TimePoint operator +( double d, const TimePoint& t )
{
   return t + d;
}

/*!
 * Returns the time point \a d days before a time point \a t. Note that scalar
 * time point subtraction is not a commutative operation, so 'd-t' cannot be
 * used.
 * \ingroup time_representation
 */
inline TimePoint operator -( const TimePoint& t, double d )
{
   return TimePoint( t.JDI(), t.JDF()-d );
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_TimePoint_h

// ----------------------------------------------------------------------------
// EOF pcl/TimePoint.h - Released 2017-06-09T08:12:42Z
