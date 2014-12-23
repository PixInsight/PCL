// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/Math.h - Released 2014/11/14 17:16:34 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#ifndef __PCL_Math_h
#define __PCL_Math_h

/// \file pcl/Math.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_Memory_h
#include <pcl/Memory.h>
#endif

#ifndef __PCL_Utility_h
#include <pcl/Utility.h>
#endif

#ifndef __PCL_Selection_h
#include <pcl/Selection.h>
#endif

#ifndef __PCL_Sort_h
#include <pcl/Sort.h>
#endif

#ifndef __stdlib_h
# include <stdlib.h>
# ifndef __stdlib_h
#  define __stdlib_h
# endif
#endif

#ifndef __cmath
# include <cmath>
# ifndef __cmath
#  define __cmath
# endif
#endif

#ifdef _MSC_VER
# ifndef __intrin_h
#  include <intrin.h> // for __cpuid()
#  ifndef __intrin_h
#   define __intrin_h
#  endif
# endif
#endif

#if defined( __x86_64__ ) || defined( _M_X64 ) || defined( __PCL_MACOSX )
# define __PCL_HAVE_SSE2   1
# ifndef __emmintrin_h
#  include <emmintrin.h>
#  ifndef __emmintrin_h
#   define __emmintrin_h
#  endif
# endif
#endif

// GCC 4.2 doesn't have sincos() on Mac OS X and FreeBSD
// MSVC doesn't have sincos() on Windows
#if !defined( __PCL_MACOSX ) && !defined( __PCL_FREEBSD ) && !defined( __PCL_WINDOWS )
# define __PCL_HAVE_SINCOS 1
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup hw_identification_functions Hardware Identification Routines
 */

/*!
 * Returns an integer representing the highest set of Streaming SIMD Extensions
 * instructions (SSE) supported by the running processor. This function is a
 * portable wrapper to the CPUID x86 instruction.
 *
 * The returned value can be one of:
 *
 *  0 : No SSE instructions supported \n
 *  1 : SSE instructions set supported \n
 *  2 : SSE2 instructions set supported \n
 *  3 : SSE3 instructions set supported \n
 * 41 : SSE4.1 instructions set supported \n
 * 42 : SSE4.2 instructions set supported
 *
 * \ingroup hw_identification_functions
 */
inline int MaxSSEInstructionSetSupported()
{
   int32 edxFlags = 0;
   int32 ecxFlags = 0;

#ifdef _MSC_VER
   int cpuInfo[ 4 ];
   __cpuid( cpuInfo, 1 );
   edxFlags = cpuInfo[3];
   ecxFlags = cpuInfo[2];
#else
   __asm__( "mov $0x00000001, %%eax;"
            "cpuid;"
            "mov %%edx, %0;"
            "mov %%ecx, %1;"
            : "=r" (edxFlags), "=r" (ecxFlags)  // output operands
            :                                   // input operands
            : "%eax", "%ebx", "%ecx", "%edx" ); // clobbered registers
#endif

   if ( ecxFlags & (1u << 20) ) // SSE4.2
      return 42;
   if ( ecxFlags & (1u << 19) ) // SSE4.1
      return 41;
   if ( ecxFlags & 1u )         // SSE3
      return 3;
   if ( edxFlags & (1u << 26) ) // SSE2
      return 2;
   if ( edxFlags & (1u << 25) ) // SSE
      return 1;
   return 0;
}

// ----------------------------------------------------------------------------

/*!
 * \defgroup fpclassification_functions Floating Point Number Classification
 */

#define __PCL_FLOAT_SGNMASK   0x80000000
#define __PCL_FLOAT_EXPMASK   0x7f800000
#define __PCL_FLOAT_SIGMASK   0x007fffff

/*!
 * Returns true if the specified 32-bit floating point number is \e finite.
 * A number is finite if it is neither NaN (Not a Number) nor positive or
 * negative infinity.
 *
 * \ingroup fpclassification_functions
 * \sa IsNaN( float ), IsInfinity( float )
 */
inline bool IsFinite( float x )
{
   union { float f; uint32 u; } v = { x };
   return (v.u & __PCL_FLOAT_EXPMASK) != __PCL_FLOAT_EXPMASK;
}

/*!
 * Returns true if the specified 32-bit floating point number is \e NaN. A NaN
 * (Not A Number) is a special value in the IEEE 754 standard used to represent
 * an undefined or unrepresentable value, such as the result of invalid
 * operations like 0/0, or real operations with complex results as the square
 * root of a negative number.
 *
 * \ingroup fpclassification_functions
 * \sa IsFinite( float ), IsInfinity( float )
 */
inline bool IsNaN( float x )
{
   union { float f; uint32 u; } v = { x };
   return (v.u & __PCL_FLOAT_EXPMASK) == __PCL_FLOAT_EXPMASK &&
          (v.u & __PCL_FLOAT_SIGMASK) != 0;
}

/*!
 * Returns a nonzero integer value if the specified 32-bit floating point
 * number is an \e infinity.
 *
 * This function returns:
 * +1 if \a x is <em>positive infinity</em>.
 * -1 if \a x is <em>negative infinity</em>.
 *  0 if \a x is either NaN or a finite value.
 *
 * \ingroup fpclassification_functions
 * \sa IsFinite( float ), IsNaN( float )
 */
inline int IsInfinity( float x )
{
   union { float f; uint32 u; } v = { x };
   if ( (v.u & __PCL_FLOAT_EXPMASK) == __PCL_FLOAT_EXPMASK &&
        (v.u & __PCL_FLOAT_SIGMASK) == 0 )
      return ((v.u & __PCL_FLOAT_SGNMASK) == 0) ? +1 : -1;
   return 0;
}

#define __PCL_DOUBLE_SGNMASK  0x80000000
#define __PCL_DOUBLE_EXPMASK  0x7ff00000
#define __PCL_DOUBLE_SIGMASK  0x000fffff

/*!
 * Returns true if the specified 64-bit floating point number is \e finite.
 * A number is finite if it is neither NaN (Not a Number) nor positive or
 * negative infinity.
 *
 * \ingroup fpclassification_functions
 * \sa IsNaN( double ), IsInfinity( double )
 */
inline bool IsFinite( double x )
{
   union { double d; uint32 u[2]; } v = { x };
   return (v.u[1] & __PCL_DOUBLE_EXPMASK) != __PCL_DOUBLE_EXPMASK;
}

/*!
 * Returns true if the specified 64-bit floating point number is \e NaN. A NaN
 * (Not A Number) is a special value in the IEEE 754 standard used to represent
 * an undefined or unrepresentable value, such as the result of invalid
 * operations like 0/0, or real operations with complex results as the square
 * root of a negative number.
 *
 * \ingroup fpclassification_functions
 * \sa IsFinite( double ), IsInfinity( double )
 */
inline bool IsNaN( double x )
{
   union { double d; uint32 u[2]; } v = { x };
   return (v.u[1] & __PCL_DOUBLE_EXPMASK) == __PCL_DOUBLE_EXPMASK &&
          (v.u[0] != 0 || (v.u[1] & __PCL_DOUBLE_SIGMASK) != 0);
}

/*!
 * Returns a nonzero integer value if the specified 64-bit floating point
 * number is an \e infinity.
 *
 * This function returns:
 * +1 if \a x is <em>positive infinity</em>.
 * -1 if \a x is <em>negative infinity</em>.
 *  0 if \a x is either NaN or a finite value.
 *
 * \ingroup fpclassification_functions
 * \sa IsFinite( double ), IsNaN( double )
 */
inline int IsInfinity( double x )
{
   union { double d; uint32 u[2]; } v = { x };
   if ( v.u[0] == 0 &&
       (v.u[1] & __PCL_DOUBLE_SIGMASK) == 0 &&
       (v.u[1] & __PCL_DOUBLE_EXPMASK) == __PCL_DOUBLE_EXPMASK )
      return ((v.u[1] & __PCL_DOUBLE_SGNMASK) == 0) ? +1 : -1;
   return 0;
}

// ----------------------------------------------------------------------------

/*!
 * \defgroup mathematical_functions Mathematical Functions
 */

/*!
 * Absolute value of \a x.
 * \ingroup mathematical_functions
 */
inline float Abs( float x )
{
   return std::fabs( x );
}

/*!
 * \ingroup mathematical_functions
 */
inline double Abs( double x )
{
   return std::fabs( x );
}

/*!
 * \ingroup mathematical_functions
 */
inline long double Abs( long double x )
{
   return std::fabs( x );
}

/*!
 * \ingroup mathematical_functions
 */
inline signed int Abs( signed int x )
{
   return (signed int)::abs( x );
}

/*!
 * \ingroup mathematical_functions
 */
inline signed long Abs( signed long x )
{
   return (signed long)::abs( x );
}

/*!
 * \ingroup mathematical_functions
 */
#ifdef __PCL_WINDOWS
inline __int64 Abs( __int64 x )
{
   return (x < 0) ? -x : +x;
}
#else
inline signed long long Abs( signed long long x )
{
   return (signed long long)::abs( x );
}
#endif

/*!
 * \ingroup mathematical_functions
 */
inline signed short Abs( signed short x )
{
   return (signed short)::abs( x );
}

/*!
 * \ingroup mathematical_functions
 */
inline signed char Abs( signed char x )
{
   return (signed char)::abs( x );
}

/*!
 * \ingroup mathematical_functions
 */
inline wchar_t Abs( wchar_t x )
{
   return (wchar_t)::abs( x );
}

/*!
 * \ingroup mathematical_functions
 */
inline unsigned int Abs( unsigned int x )
{
   return x;
}

/*!
 * \ingroup mathematical_functions
 */
inline unsigned long Abs( unsigned long x )
{
   return x;
}

/*!
 * \ingroup mathematical_functions
 */
#ifdef __PCL_WINDOWS
inline unsigned __int64 Abs( unsigned __int64 x )
{
   return x;
}
#else
inline unsigned long long Abs( unsigned long long x )
{
   return x;
}
#endif

/*!
 * \ingroup mathematical_functions
 */
inline unsigned short Abs( unsigned short x )
{
   return x;
}

/*!
 * \ingroup mathematical_functions
 */
inline unsigned char Abs( unsigned char x )
{
   return x;
}

// ----------------------------------------------------------------------------

/*!
 * Merges a complex angle given by degrees and arcminutes into single degrees.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Angle( int d, T m )
{
   return d + m/60;
}

/*!
 * Merges a complex angle given by degrees, arcminutes and arcseconds into
 * single degrees.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Angle( int d, int m, T s )
{
   return Angle( d, m + s/60 );
}

// ----------------------------------------------------------------------------

/*!
 * Inverse cosine function (arccosine).
 * \ingroup mathematical_functions
 */
template <typename T> inline T ArcCos( T x )
{
   PCL_PRECONDITION( T( -1 ) <= x && x <= T( 1 ) )
   return std::acos( x );
}

// ----------------------------------------------------------------------------

/*!
 * Inverse sine function (arcsine).
 * \ingroup mathematical_functions
 */
template <typename T> inline T ArcSin( T x )
{
   PCL_PRECONDITION( T( -1 ) <= x && x <= T( 1 ) )
   return std::asin( x );
}

// ----------------------------------------------------------------------------

/*!
 * Inverse tangent function (arctangent).
 * \ingroup mathematical_functions
 */
template <typename T> inline T ArcTan( T x )
{
   return std::atan( x );
}

// ----------------------------------------------------------------------------

/*!
 * Arctangent of y/x, result in the proper quadrant.
 * \ingroup mathematical_functions
 */
template <typename T> inline T ArcTan( T y, T x )
{
   return std::atan2( y, x );
}

// ----------------------------------------------------------------------------

/*!
 * Arctangent of y/x, proper quadrant, result in the interval [0,2pi[.
 * \ingroup mathematical_functions
 */
template <typename T> inline T ArcTan2Pi( T y, T x )
{
   register T r = ArcTan( y, x );
   if ( r < 0 )
      r = static_cast<T>( r + 0.62831853071795864769252867665590058e+01L );
   return r;
}

// ----------------------------------------------------------------------------

/*!
 * The ceil function: lowest integer >= x.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Ceil( T x )
{
   return std::ceil( x );
}

// ----------------------------------------------------------------------------

/*!
 * Cosine function.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Cos( T x )
{
   return std::cos( x );
}

// ----------------------------------------------------------------------------

/*!
 * Hyperbolic Cosine function.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Cosh( T x )
{
   return std::cosh( x );
}

// ----------------------------------------------------------------------------

/*!
 * Cotangent of x, equal to Cos(x)/Sin(x) or 1/Tan(x).
 * \ingroup mathematical_functions
 */
template <typename T> inline T Cotan( T x )
{
   return T( 1 )/std::tan( x );
}

// ----------------------------------------------------------------------------

/*!
 * Conversion from radians to degrees.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Deg( T x )
{
   return static_cast<T>( 0.572957795130823208767981548141051700441964e+02L * x );
}

// ----------------------------------------------------------------------------

/*!
 * The exponential function e**x.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Exp( T x )
{
   return std::exp( x );
}

// ----------------------------------------------------------------------------

/*!
 * \class Fact
 * \brief Factorial function.
 *
 * We use a static lookup table to speed up for n <= 60.
 *
 * Example of use:
 *
 * \code double factorialOfEight = Fact<double>()( 8 ); \endcode
 *
 * \ingroup mathematical_functions
 */
template <typename T> struct PCL_CLASS Fact
{
   T operator()( int n ) const
   {
      static T f[ 61 ] = { T( 1 ), T( 1 ), T( 2 ), T( 6 ), T( 24 ), T( 120 ) };
      static int last = 5;
      PCL_PRECONDITION( 0 <= n )
      if ( last < n )
      {
         int m = Min( n, 60 );
         T x = f[last];
         while ( last < m )
         {
            x *= ++last;
            f[last] = x;
         }
         while ( m < n )
            x *= ++m;
         return x;
      }
      return f[n];
   }
};

// ----------------------------------------------------------------------------

/*!
 * The floor function: highest integer <= x.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Floor( T x )
{
   return std::floor( x );
}

// ----------------------------------------------------------------------------

/*!
 * Fractional part of x.
 * The returned value is within ]-1,+1[, and has the same sign as x.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Frac( T x )
{
   return std::modf( x, &x );
}

// ----------------------------------------------------------------------------

/*!
 * Calculates base-2 mantissa and exponent.
 * The arguments \a m and \a p receive the values of the base-2 mantissa and
 * exponent, respectively, such that: 0.5 <= m < 1.0, x = m * 2**p
 * \ingroup mathematical_functions
 */
template <typename T> inline void Frexp( T x, T& m, int& p )
{
   m = std::frexp( x, &p );
}

// ----------------------------------------------------------------------------

/*!
 * Haversine function.
 *
 * <tt>Hav( x ) = (1 - Cos( x ))/2</tt>
 *
 * The haversine is nice to work with tiny angles.
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline T Hav( T x )
{
   return (1 - Cos( x ))/2;
}

// ----------------------------------------------------------------------------

/*!
 * Calculates m * 2**p.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Ldexp( T m, int p )
{
   return std::ldexp( m, p );
}

// ----------------------------------------------------------------------------

/*!
 * Natural (base e) logarithm of x.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Ln( T x )
{
   return std::log( x );
}

// ----------------------------------------------------------------------------

/*!
 * Natural (base e) logarithm of two.
 * \ingroup mathematical_functions
 */
inline long double Ln2()
{
   return (long double)( 0.6931471805599453094172321214581766e+00L );
}

// ----------------------------------------------------------------------------

/*!
 * Base 10 Logarithm of x.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Log( T x )
{
   return std::log10( x );
}

// ----------------------------------------------------------------------------

/*!
 * Base 10 Logarithm of two.
 * \ingroup mathematical_functions
 */
inline long double Log2()
{
   // Use the relation:
   //    log10(2) = ln(2)/ln(10)
   return (long double)( 0.3010299956639811952137388947244930416265e+00L );
}

// ----------------------------------------------------------------------------

/*!
 * Base 2 Logarithm of x.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Log2( T x )
{
   // Use the relation:
   //    log2(x) = ln(x)/ln(2)
   return std::log( x )/Ln2();
}

// ----------------------------------------------------------------------------

/*!
 * Base 2 Logarithm of e.
 * \ingroup mathematical_functions
 */
inline long double Log2e()
{
   // Use the relation:
   //    log2(e) = 1/ln(2)
   return (long double)( 0.14426950408889634073599246810018920709799e+01L );
}

// ----------------------------------------------------------------------------

/*!
 * Base 2 Logarithm of ten.
 * \ingroup mathematical_functions
 */
inline long double Log2T()
{
   // Use the relation:
   //    log2(10) = 1/log(2)
   return (long double)( 0.33219280948873623478703194294893900118996e+01L );
}

// ----------------------------------------------------------------------------

/*!
 * Base \a n logarithm of \a x.
 * \ingroup mathematical_functions
 */
template <typename T> inline T LogN( T n, T x )
{
   return std::log( x )/std::log( n );
}

// ----------------------------------------------------------------------------

/*!
 * Remainder of x/y.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Mod( T x, T y )
{
   return std::fmod( x, y );
}

// ----------------------------------------------------------------------------

/*!
 * The pi constant: 3.141592...
 * \ingroup mathematical_functions
 */
inline long double Pi()
{
   return (long double)( 0.31415926535897932384626433832795029e+01L );
}

// ----------------------------------------------------------------------------

/*!
 * Horner's algorithm to evaluate the polynomial function:
 *
 * <tt>y = c0 + c1*x + c2*x**2 + ... + cn*x**n</tt>
 *
 * \ingroup mathematical_functions
 */
template <typename T, typename C> inline T Poly( T x, C c, int n )
{
   PCL_PRECONDITION( n >= 0 )
   register T y;
   for ( c += n, y = *c; n != 0; --n )
   {
      y *= x;
      y += *--c;
   }
   return y;
}

// ----------------------------------------------------------------------------

/*!
 * Sign function:
 *
 * \li -1 if x < 0
 * \li 0 if x == 0
 * \li +1 if x > 0
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline int Sign( T x )
{
   return (x < 0) ? -1 : ((x > 0) ? +1 : 0);
}

// ----------------------------------------------------------------------------

/*!
 * Sign character:
 *
 * \li '-' if x < 0
 * \li ' ' if x == 0
 * \li '+' if x > 0
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline char SignChar( T x )
{
   return (x < 0) ? '-' : ((x > 0) ? '+' : ' ');
}

// ----------------------------------------------------------------------------

/*!
 * Sine function
 * \ingroup mathematical_functions
 */
template <typename T> inline T Sin( T x )
{
   return std::sin( x );
}

// ----------------------------------------------------------------------------

/*!
 * Hyperbolic Sine function.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Sinh( T x )
{
   return std::sinh( x );
}

// ----------------------------------------------------------------------------

#ifdef __PCL_HAVE_SINCOS

inline void __pcl_sincos__( float x, float& sx, float& cx )
{
   ::sincosf( x, &sx, &cx );
}

inline void __pcl_sincos__( double x, double& sx, double& cx )
{
   ::sincos( x, &sx, &cx );
}

inline void __pcl_sincos__( long double x, long double& sx, long double& cx )
{
   ::sincosl( x, &sx, &cx );
}

#endif

/*!
 * Sine and cosine of x.
 *
 * The arguments \a sx and \a cx will receive, respectively, the values of the
 * sine and cosine of x.
 *
 * If supported by the underlying standard math library, this function is
 * roughly twice as fast as calling Sin() and Cos() separately. For code that
 * spend a significant amount of time calculating sines and cosines, this
 * optimization is critical.
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline void SinCos( T x, T& sx, T& cx )
{
#ifdef __PCL_HAVE_SINCOS
   __pcl_sincos__( x, sx, cx );
#else
   sx = std::sin( x );
   cx = std::cos( x );
#endif
}

// ----------------------------------------------------------------------------

/*!
 * Integer and fractional parts of x.
 *
 * The arguments \a i and \a f receive, respectively, the integer (truncated)
 * part and the fractional part of \a x. It holds that \a x = \a i + \a f, i.e.
 * both \a i and \a f have the same sign as \a x.
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline void Split( T x, T& i, T& f )
{
   f = std::modf( x, &i );
}

// ----------------------------------------------------------------------------

/*!
 * Square root function.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Sqrt( T x )
{
   return std::sqrt( x );
}

// ----------------------------------------------------------------------------

/*!
 * Tangent function.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Tan( T x )
{
   return std::tan( x );
}

// ----------------------------------------------------------------------------

/*!
 * Hyperbolic Tangent function.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Tanh( T x )
{
   return std::tanh( x );
}

// ----------------------------------------------------------------------------

/*!
 * Truncated integer part of \a x.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Trunc( T x )
{
   (void)std::modf( x, &x );
   return x;
}

// ----------------------------------------------------------------------------

#ifdef __PCL_HAVE_SSE2

inline int __pcl_trunci__( float x )
{
   return _mm_cvtt_ss2si( _mm_load_ss( &x ) );
}

inline int __pcl_trunci__( double x )
{
   return _mm_cvttsd_si32( _mm_load_sd( &x ) );
}

#endif

/*!
 * TruncInt function: Truncated integer part of \a x as a 32-bit signed
 * integer.
 *
 * \note This is a performance-critical routine. It has been implemented using
 * high-performance, low-level techniques that may include inline assembly code
 * and/or compiler intrinsic functions.
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline int TruncInt( T x )
{
   PCL_PRECONDITION( x >= int_min && x <= int_max )
#ifdef __PCL_NO_PERFORMANCE_CRITICAL_MATH_ROUTINES
   return int( x );
#else
# ifdef __PCL_HAVE_SSE2
   return __pcl_trunci__( x );
# else
   return int( x );
# endif
#endif
}

/*!
 * TruncI function: Truncated integer part of \a x as a 32-bit signed integer.
 * to a 32-bit signed integer.
 *
 * This function is a convenience synonym for TruncInt().
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline int TruncI( T x )
{
   return TruncInt( x );
}

#define TruncInt32( x ) TruncInt( x )
#define TruncI32( x )   TruncInt( x )

// ----------------------------------------------------------------------------

#ifdef __PCL_HAVE_SSE2

#if defined( __x86_64__ ) || defined( _M_X64 )

inline int64 __pcl_trunci64__( float x )
{
   return _mm_cvttss_si64( _mm_load_ss( &x ) );
}

inline int64 __pcl_trunci64__( double x )
{
   return _mm_cvttsd_si64( _mm_load_sd( &x ) );
}

#else

inline int64 __pcl_trunci64__( float x )
{
   return int64( _mm_cvtt_ss2si( _mm_load_ss( &x ) ) );
}

inline int64 __pcl_trunci64__( double x )
{
   return int64( x );
}

#endif

#endif // __PCL_HAVE_SSE2

/*!
 * TruncInt64 function: Truncated integer part of \a x as a 64-bit signed
 * integer.
 *
 * \note This is a performance-critical routine. It has been implemented using
 * high-performance, low-level techniques that may include inline assembly code
 * and/or compiler intrinsic functions.
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline int64 TruncInt64( T x )
{
   PCL_PRECONDITION( x >= int64_min && x <= int64_max )
#ifdef __PCL_NO_PERFORMANCE_CRITICAL_MATH_ROUTINES
   return int64( Trunc( x ) );
#else
# ifdef __PCL_HAVE_SSE2
   return __pcl_trunci64__( x );
# else
   return int64( Trunc( x ) );
# endif
#endif
}

/*!
 * TruncI64 function: Truncated integer part of \a x as a 64-bit signed
 * integer.
 *
 * This function is a convenience synonym for TruncInt64().
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline int64 TruncI64( T x )
{
   return TruncInt64( x );
}

// ----------------------------------------------------------------------------

/*!
 * General power function: \a x raised to \a y.
 *
 * When you know some of the arguments in advance, faster alternatives are:
 *
 * \li Use Pow10I<T>()( y ) when x == 10 and y is an integer
 * \li Use PowI( x, y ) when x != 10 and y is an integer
 * \li Use Pow10( y ) when x == 10 and y is not an integer
 *
 * Otherwise, you can also use: Pow2( y*Log2( x ) )
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline T Pow( T x, T y )
{
   PCL_PRECONDITION( y < T( int_max ) )
   return std::pow( x, y );
}

// ----------------------------------------------------------------------------

/*!
 * \class Pow10I
 * \brief Exponential function 10**n, n being a signed integer.
 *
 * Example of use:
 *
 * \code double x = Pow10I<double>()( 5 ); // x = 10^5 \endcode
 *
 * \ingroup mathematical_functions
 */
template <typename T> struct PCL_CLASS Pow10I
{
   T operator ()( int n ) const
   {
      // Use fast table lookups and squaring up to |n| <= 50.
      static const T lut[] =
      {
#define ___( x ) static_cast<T>( x )
      ___( 1.0e+00 ), ___( 1.0e+01 ), ___( 1.0e+02 ), ___( 1.0e+03 ), ___( 1.0e+04 ),
      ___( 1.0e+05 ), ___( 1.0e+06 ), ___( 1.0e+07 ), ___( 1.0e+08 ), ___( 1.0e+09 ),
      ___( 1.0e+10 ), ___( 1.0e+11 ), ___( 1.0e+12 ), ___( 1.0e+13 ), ___( 1.0e+14 ),
      ___( 1.0e+15 ), ___( 1.0e+16 ), ___( 1.0e+17 ), ___( 1.0e+18 ), ___( 1.0e+19 ),
      ___( 1.0e+20 ), ___( 1.0e+21 ), ___( 1.0e+22 ), ___( 1.0e+23 ), ___( 1.0e+24 ),
      ___( 1.0e+25 ), ___( 1.0e+26 ), ___( 1.0e+27 ), ___( 1.0e+28 ), ___( 1.0e+29 ),
      ___( 1.0e+30 ), ___( 1.0e+31 ), ___( 1.0e+32 ), ___( 1.0e+33 ), ___( 1.0e+34 ),
      ___( 1.0e+35 ), ___( 1.0e+36 ), ___( 1.0e+37 ), ___( 1.0e+38 ), ___( 1.0e+39 ),
      ___( 1.0e+40 ), ___( 1.0e+41 ), ___( 1.0e+42 ), ___( 1.0e+43 ), ___( 1.0e+44 ),
      ___( 1.0e+45 ), ___( 1.0e+46 ), ___( 1.0e+47 ), ___( 1.0e+48 ), ___( 1.0e+49 )
#undef ___
      };
      static const int N = ItemsInArray( lut );
      int i = ::abs( n );
      double x;
      if ( i < N )
         x = lut[i];
      else
      {
         x = lut[N-1];
         while ( (i -= N-1) >= N )
            x *= x;
         if ( i != 0 )
            x *= lut[i];
      }
      return T( (n >= 0) ? x : 1/x );
   }
};

// ----------------------------------------------------------------------------

/*!
 * The exponential function 10**x.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Pow10( T x )
{
   int i = TruncInt( x );
   return (i == x) ? Pow10I<T>()( i ) : T( std::pow( 10, x ) );
}

// ----------------------------------------------------------------------------

/*!
 * Round function: x rounded to the nearest integer (double precision version).
 *
 * \note This is a performance-critical routine. It has been implemented using
 * high-performance, low-level techniques that may include inline assembly code
 * and/or compiler intrinsic functions.
 *
 * \ingroup mathematical_functions
 */
inline double Round( double x )
{
#ifdef __PCL_NO_PERFORMANCE_CRITICAL_MATH_ROUTINES

   return floor( x + 0.5 );

#else

#  ifdef _MSC_VER
#    ifdef _M_X64
   return double( _mm_cvtsd_si64( _mm_load_sd( &x ) ) );
#    else
   __asm fld      x
   __asm frndint
#    endif
#  else
   double r;
   __asm__( "frndint;": "=t" (r) : "0" (x) );
   return r;
#  endif

#endif
}

/*!
 * Round function: x rounded to the nearest integer (single precision version).
 *
 * \note This is a performance-critical routine. It has been implemented using
 * high-performance, low-level techniques that may include inline assembly code
 * and/or compiler intrinsic functions.
 *
 * \ingroup mathematical_functions
 */
inline float Round( float x )
{
#ifdef __PCL_NO_PERFORMANCE_CRITICAL_MATH_ROUTINES

   return floorf( x + 0.5F );

#else

#  ifdef _MSC_VER
#    ifdef _M_X64
   return float( _mm_cvtss_si32( _mm_load_ss( &x ) ) );
#    else
   __asm fld      x
   __asm frndint
#    endif
#  else
   float r;
   __asm__( "frndint;": "=t" (r) : "0" (x) );
   return r;
#  endif

#endif
}

/*!
 * Round function: x rounded to the nearest integer (long double version).
 *
 * \note This is a performance-critical routine. It has been implemented using
 * high-performance, low-level techniques that may include inline assembly code
 * and/or compiler intrinsic functions.
 *
 * \ingroup mathematical_functions
 */
inline long double Round( long double x )
{
#ifdef __PCL_NO_PERFORMANCE_CRITICAL_MATH_ROUTINES

   return floorl( x + 0.5L );

#else

#  ifdef _MSC_VER
#    ifdef _M_X64
   double _x = x;
   return (long double)_mm_cvtsd_si64( _mm_load_sd( &_x ) );
#    else
   __asm fld      x
   __asm frndint
#    endif
#  else
   long double r;
   __asm__( "frndint;": "=t" (r) : "0" (x) );
   return r;
#  endif

#endif
}

// ----------------------------------------------------------------------------

/*!
 * RoundInt function: Rounds \a x to the nearest integer and converts the
 * result to a 32-bit signed integer.
 *
 * This function follows the Banker's rounding rule: a perfect half is always
 * rounded to the nearest even digit. Some examples:
 *
 * RoundInt( 0.5 ) -> 0
 * RoundInt( 1.5 ) -> 2
 * RoundInt( 2.5 ) -> 2
 * RoundInt( 3.5 ) -> 4
 *
 * By contrast, arithmetic rounding rounds a perfect half to the nearest digit,
 * either odd or even. For example:
 *
 * RoundIntArithmetic( 0.5 ) -> 1
 * RoundIntArithmetic( 1.5 ) -> 2
 * RoundIntArithmetic( 2.5 ) -> 3
 * RoundIntArithmetic( 3.5 ) -> 4
 *
 * Banker's rounding (also known as Gaussian rounding) is statistically more
 * accurate than the usual arithmetic rounding, but it causes aliasing problems
 * in some specific algorithms that depend critically on uniform rounding, such
 * as nearest neighbor upsampling.
 *
 * \note This is a performance-critical routine. It has been implemented using
 * high-performance, low-level techniques that may include inline assembly code
 * and/or compiler intrinsic functions.
 *
 * \sa RoundIntArithmetic(), RoundIntBanker()
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline int RoundInt( T x )
{
   PCL_PRECONDITION( x >= int_min && x <= int_max )

#ifdef __PCL_NO_PERFORMANCE_CRITICAL_MATH_ROUTINES

   return int( Round( x ) );

#else

   volatile union { double d; int32 i; } v = { x + 6755399441055744.0 };
   return v.i; // ### NB: Assuming little-endian architecture, i.e. Intel.

/*
   ### Deprecated code - The above code based on magic numbers is faster and
                         more portable across platforms and compilers.

   // ### N.B.: Default FPU rounding mode assumed to be nearest integer.
   int r;

#  ifdef _MSC_VER
   __asm fld      x
   __asm fistp    dword ptr r
#  else
   __asm__ __volatile__( "fistpl %0"  : "=m" (r) : "t" (x) : "st" );
#  endif

   return r;
*/

#endif
}

/*!
 * RoundI function: Rounds \a x to the nearest integer and converts the result
 * to a 32-bit signed integer.
 *
 * This function is a convenience synonym for RoundInt().
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline int RoundI( T x )
{
   return RoundInt( x );
}

/*!
 * Rounds \a x to the nearest integer using the Banker's rounding rule, and
 * converts the result to a 32-bit signed integer.
 *
 * This function is a convenience synonym for RoundInt().
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline int RoundIntBanker( T x )
{
   return RoundInt( x );
}

/*!
 * Rounds \a x to the nearest integer using the arithmetic rounding rule, and
 * converts the result to a 32-bit signed integer.
 *
 * Arithmetic rounding rounds a perfect half to the nearest digit, either odd
 * or even. For example:
 *
 * RoundIntArithmetic( 0.5 ) -> 1
 * RoundIntArithmetic( 1.5 ) -> 2
 * RoundIntArithmetic( 2.5 ) -> 3
 * RoundIntArithmetic( 3.5 ) -> 4
 *
 * See the RoundInt() function for more information on rounding rules.
 *
 * \note This is a performance-critical routine. It has been implemented using
 * high-performance, low-level techniques that may include inline assembly code
 * and/or compiler intrinsic functions.
 *
 * \sa RoundInt(), RoundIntBanker()
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline int RoundIntArithmetic( T x )
{
   PCL_PRECONDITION( x >= int_min && x <= int_max )

   int i = TruncInt( x );
   if ( i < 0 )
   {
      if ( x - i <= T( -0.5 ) )
         return i-1;
   }
   else
   {
      if ( x - i >= T( +0.5 ) )
         return i+1;
   }
   return i;
}

// ----------------------------------------------------------------------------

/*!
 * RoundInt64 function: Rounds \a x to the nearest integer and converts the
 * result to a 64-bit signed integer.
 *
 * Since the default IEEE 754 rounding mode follows Banker's rounding rule,
 * this is what you should expect when calling this function. See the
 * documentation for RoundInt() for more information on rounding modes.
 *
 * \note This is a performance-critical routine. It has been implemented using
 * high-performance, low-level techniques that may include inline assembly code
 * and/or compiler intrinsic functions.
 *
 * \ingroup mathematical_functions
 */
inline int64 RoundInt64( double x )
{
#ifdef __PCL_NO_PERFORMANCE_CRITICAL_MATH_ROUTINES

   return int64( Round( x ) );

#else

   // ### N.B.: Default FPU rounding mode assumed to be nearest integer.

#  ifdef _MSC_VER
#    ifdef _M_X64
   return _mm_cvtsd_si64( _mm_load_sd( &x ) );
#    else
   int64 r;
   __asm fld      x
   __asm fistp    qword ptr r
   return r;
#    endif
#  else
   int64 r;
   __asm__ __volatile__( "fistpll %0"  : "=m" (r) : "t" (x) : "st" );
   return r;
#  endif

#endif
}

/*!
 * RoundI64 function: Rounds \a x to the nearest integer and converts the
 * result to a 64-bit signed integer.
 *
 * This function is a convenience synonym for RoundInt64().
 */
inline int64 RoundI64( double x )
{
   return RoundInt64( x );
}

/*!
 * Rounds \a x to the nearest integer using the arithmetic rounding rule, and
 * converts the result to a 64-bit signed integer.
 *
 * Arithmetic rounding rounds a perfect half to the nearest digit, either odd
 * or even. For example:
 *
 * RoundIntArithmetic( 0.5 ) -> 1
 * RoundIntArithmetic( 1.5 ) -> 2
 * RoundIntArithmetic( 2.5 ) -> 3
 * RoundIntArithmetic( 3.5 ) -> 4
 *
 * See the RoundInt() function for more information on rounding rules.
 *
 * \note This is a performance-critical routine. It has been implemented using
 * high-performance, low-level techniques that may include inline assembly code
 * and/or compiler intrinsic functions.
 *
 * \sa RoundInt(), RoundIntBanker()
 *
 * \ingroup mathematical_functions
 */
inline int64 RoundInt64Arithmetic( double x )
{
   int64 i = TruncInt64( x );
   if ( i < 0 )
   {
      if ( x - i <= -0.5 )
         return i-1;
   }
   else
   {
      if ( x - i >= +0.5 )
         return i+1;
   }
   return i;
}

// ----------------------------------------------------------------------------

/*!
 * General rounding function: \a x rounded to \a n fractional digits.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Round( T x, int n )
{
   PCL_PRECONDITION( n >= 0 )
   T p = Pow10I<T>()( n ); return Round( p*x )/p;
}

// ----------------------------------------------------------------------------

/*!
 * The exponential function 2**x.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Pow2( T x )
{
   // Use the relation:
   //    2**x = e**(x*ln(2))
   return Exp( x*Ln2() );
}

// ----------------------------------------------------------------------------

/*!
 * \class Pow2I
 * \brief Exponential function 2**n, n being a signed integer.
 *
 * Example of use:
 *
 * \code float x = Pow2I<float>()( -2 ); // x = 1/4 \endcode
 *
 * \ingroup mathematical_functions
 */
template <typename T> struct PCL_CLASS Pow2I
{
   T operator ()( int n ) const
   {
      // We shift left a single bit in 31-bit chunks.
      int i = ::abs( n ), p;
      double x = uint32( 1 ) << (p = Min( i, 31 ));
      while ( (i -= p) != 0 )
         x *= uint32( 1 ) << (p = Min( i, 31 ));
      return T( (n >= 0) ? x : 1/x );
   }
};

// ----------------------------------------------------------------------------

/*!
 * The exponential function x**n, where n is a signed integer.
 * \ingroup mathematical_functions
 */
template <typename T> inline T PowI( T x, int n )
{
   if ( n == 0 )
      return 1;

   register int i = Abs( n );
   register T r = x;
   if ( i > 1 )
   {
      do
         r *= r;
      while ( (i >>= 1) >= 2 );

      if ( (n & 1) != 0 )
         r *= x;
   }
   return (n >= 0) ? r : 1/r;
}

// ----------------------------------------------------------------------------

/*!
 * Inverse hyperbolic sine function.
 *
 * <tt>ArcSinh( x ) = Ln( x + Sqrt( 1 + x**2 ) )</tt>
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline T ArcSinh( T x )
{
   return Ln( x + Sqrt( 1 + x*x ) );
}

// ----------------------------------------------------------------------------

/*!
 * Inverse hyperbolic cosine function.
 *
 * <tt>ArcCosh( x ) = 2*Ln( Sqrt( (x + 1)/2 ) + Sqrt( (x - 1)/2 ) )</tt>
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline T ArcCosh( T x )
{
   T y = Ln( Sqrt( (x + 1)/2 ) + Sqrt( (x - 1)/2 ) );
   return y+y;
}

// ----------------------------------------------------------------------------

/*!
 * Inverse hyperbolic tangent function.
 *
 * <tt>ArcTanh( x ) = (Ln( 1 + x ) - Ln( 1 - x ))/2</tt>
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline T ArcTanh( T x )
{
   return (Ln( 1 + x ) - Ln( 1 - x ))/2;
}

// ----------------------------------------------------------------------------

/*!
 * Inverse haversine (archaversine?) function.
 *
 * <tt>ArcHav( x ) = 2*ArcSin( Sqrt( x ) )</tt>
 *
 * The haversine is nice to work with tiny angles.
 *
 * \ingroup mathematical_functions
 */
template <typename T> inline T ArcHav( T x )
{
   T y = ArcSin( Sqrt( x ) );
   return y+y;
}

// ----------------------------------------------------------------------------

/*!
 * Conversion from degrees to radians.
 * \ingroup mathematical_functions
 */
template <typename T> inline T Rad( T x )
{
   return static_cast<T>( 0.174532925199432957692369076848861272222e-01L * x );
}

// ----------------------------------------------------------------------------

/*!
 * Conversion from radians to arcminutes.
 * \ingroup mathematical_functions
 */
template <typename T> inline T RadMin( T x )
{
   return Deg( x )*60;
}

// ----------------------------------------------------------------------------

/*!
 * Conversion from radians to arcseconds.
 * \ingroup mathematical_functions
 */
template <typename T> inline T RadSec( T x )
{
   return Deg( x )*3600;
}

// ----------------------------------------------------------------------------

/*!
 * Conversion from arcminutes to radians.
 * \ingroup mathematical_functions
 */
template <typename T> inline T MinRad( T x )
{
   return Rad( x/60 );
}

// ----------------------------------------------------------------------------

/*!
 * Conversion from arcseconds to radians.
 * \ingroup mathematical_functions
 */
template <typename T> inline T SecRad( T x )
{
   return Rad( x/3600 );
}

// ----------------------------------------------------------------------------

/*!
 * Rotates a point on the plane.
 *
 * \param[out] x,y   %Point coordinates. On output, these variables will
 *                   receive the corresponding rotated coordinates.
 *
 * \param sa, ca     Sine and cosine of the rotation angle.
 *
 * \param xc,yc      Coordinates of the center of rotation.
 *
 * \ingroup mathematical_functions
 */
template <typename T, typename T1, typename T2> inline
void Rotate( T& x, T& y, T1 sa, T1 ca, T2 xc, T2 yc )
{
   register T1 dx = T1( x ) - T1( xc );
   register T1 dy = T1( y ) - T1( yc );
   x = T( T1( xc ) + ca*dx + sa*dy );
   y = T( T1( yc ) - sa*dx + ca*dy );
}

/*!
 * Rotates a point on the plane.
 *
 * This is a template instantiation of Rotate( T&, T&, T1, T1, T2, T2 ) for
 * the \c int type.
 *
 * Rotated coordinates are rounded to the nearest integers.
 *
 * \ingroup mathematical_functions
 */
template <typename T1, typename T2> inline
void Rotate( int& x, int& y, T1 sa, T1 ca, T2 xc, T2 yc )
{
   register T1 dx = T1( x ) - T1( xc );
   register T1 dy = T1( y ) - T1( yc );
   x = RoundI( T1( xc ) + ca*dx + sa*dy );
   y = RoundI( T1( yc ) - sa*dx + ca*dy );
}

/*!
 * Rotates a point on the plane.
 *
 * This is a template instantiation of Rotate( T&, T&, T1, T1, T2, T2 ) for
 * the \c long type.
 *
 * Rotated coordinates are rounded to the nearest integers.
 *
 * \ingroup mathematical_functions
 */
template <typename T1, typename T2> inline
void Rotate( long& x, long& y, T1 sa, T1 ca, T2 xc, T2 yc )
{
   register T1 dx = T1( x ) - T1( xc );
   register T1 dy = T1( y ) - T1( yc );
   x = (long)RoundI( T1( xc ) + ca*dx + sa*dy );
   y = (long)RoundI( T1( yc ) - sa*dx + ca*dy );
}

/*!
 * Rotates a point on the plane.
 *
 * This is a template instantiation of Rotate( T&, T&, T1, T1, T2, T2 ) for
 * the \c int64 type.
 *
 * Rotated coordinates are rounded to the nearest integers.
 *
 * \ingroup mathematical_functions
 */
template <typename T1, typename T2> inline
void Rotate( int64& x, int64& y, T1 sa, T1 ca, T2 xc, T2 yc )
{
   register T1 dx = T1( x ) - T1( xc );
   register T1 dy = T1( y ) - T1( yc );
   x = RoundI64( T1( xc ) + ca*dx + sa*dy );
   y = RoundI64( T1( yc ) - sa*dx + ca*dy );
}

/*!
 * Rotates a point on the plane.
 *
 * \param[out] x,y   %Point coordinates. On output, these variables will
 *                   receive the corresponding rotated coordinates.
 *
 * \param a          Rotation angle in radians.
 *
 * \param xc,yc      Coordinates of the center of rotation.
 *
 * Instantiations for integer types round rotated coordinated to the nearest
 * integers.
 *
 * \ingroup mathematical_functions
 */
template <typename T, typename T1, typename T2> inline
void Rotate( T& x, T& y, T1 a, T2 xc, T2 yc )
{
   T1 sa, ca; SinCos( a, sa, ca ); Rotate( x, y, sa, ca, xc, yc );
}

// ----------------------------------------------------------------------------

/*!
 * Computes the Julian day number (JD) corresponding to a time point expressed
 * as a date and a day fraction.
 *
 * \param year    The year of the date. Positive and negative years are
 *                supported. Years are counted arithmetically: the year zero is
 *                the year before the year +1, that is, what historians call
 *                the year 1 B.C.
 *
 * \param month   The month of the date. Usually in the [1,12] range but can be
 *                any integer number.
 *
 * \param day     The day of the date. Usually in the [1,31] range but can be
 *                any integer number.
 *
 * \param dayf    The day fraction. The default value is zero, which computes
 *                the JD at zero hours. Usually in the [0,1[ range but can be
 *                any real number.
 *
 * This routine, as well as JDToComplexTime(), implement modified versions of
 * the original algorithms due to Jean Meeus. Our modifications allow for
 * negative Julian day numbers, which extends the range of allowed dates to the
 * past considerably. We developed these modifications in the context of
 * large-scale solar system ephemeris calculations.
 *
 * The returned value is the JD corresponding to the specified date and day
 * fraction. Due to the numerical precision of the double type (IEEE 64-bit
 * floating point), this routine can return JD values accurate only to within
 * one millisecond.
 *
 * \b References
 *
 * Meeus, Jean (1991), <em>Astronomical Algorithms</em>, Willmann-Bell, Inc.,
 * chapter 7.
 *
 * \ingroup mathematical_functions
 */
double PCL_FUNC ComplexTimeToJD( int year, int month, int day, double dayf = 0 );

/*!
 * Computes the date and day fraction corresponding to a time point expressed
 * as a Julian day number (JD).
 *
 * \param[out] year  On output, this variable receives the year of the
 *                   resulting date.
 *
 * \param[out] month On output, this variable receives the month of the
 *                   resulting date in the range [1,12].
 *
 * \param[out] day   On output, this variable receives the day of the
 *                   resulting date in the range [1,31]. Different month day
 *                   counts and leap years are taken into account, so the
 *                   returned day corresponds to an existing calendar date.
 *
 * \param[out] dayf  On output, this variable receives the day fraction for the
 *                   specified time point, in the [0,1[ range.
 *
 * \param jd         The input time point as a Julian day number.
 *
 * For more information about the implemented algorithms, references and
 * limitations of these routines, see the documentation for ComplexTimeToJD().
 *
 * \ingroup mathematical_functions
 */
void PCL_FUNC JDToComplexTime( int& year, int& month, int& day, double& dayf, double jd );

// ----------------------------------------------------------------------------

/*!
 * \defgroup statistical_functions Statistical Functions
 */

/*!
 * Returns the sum of elements in a sequence [i,j[.
 *
 * For empty sequences, this function returns zero.
 *
 * This is a straight implementation of a floating point sum, which is subject
 * to severe roundoff errors if the number of summed elements is large. One way
 * to improve on this problem is to sort the input set by decreasing order of
 * absolute value \e before calling this function. A much better solution, but
 * computationally expensive, is a compensated summation algorithm such as
 * Kahan summation, which we have implemented in the StableSum() routine.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> inline double Sum( const T* i, const T* j )
{
   double sum = 0;
   for ( ; i < j; ++i )
      sum += double( *i );
   return sum;
}

/*!
 * Computes the sum of elements in a sequence [i,j[ using a numerically stable
 * summation algorithm to minimize roundoff error.
 *
 * For empty sequences, this function returns zero.
 *
 * In the current PCL versions, this function implements the Kahan summation
 * algorithm to reduce roundoff error to the machine's floating point error.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> inline double StableSum( const T* i, const T* j )
{
   double sum = 0;
   double eps = 0;
   for ( ; i < j; ++i )
   {
      double y = double( *i ) - eps;
      double t = sum + y;
      eps = (t - sum) - y;
      sum = t;
   }
   return sum;
}

/*
 * IMPORTANT NOTE - The routine below is an example of more accurate summation
 *                  implemented with C++11 lambda closures. We still are not
 *                  ready to release a PCL version with C++11 support, but not
 *                  far from it. That will happen during the PCL 2.x cycle.
 */

// #ifdef __PCL_CXX11
//
// /*!
//  * Returns the sum of elements in a sequence [i,j[.
//  *
//  * For empty sequences, this function returns zero.
//  *
//  * This implementation sums a temporary sorted (in decreasing order of absolute
//  * value) duplicate of the input set to minimize roundoff errors.
//  *
//  * \note This function requires a C++11 compiler and PCL C++11 support enabled.
//  *
//  * \ingroup statistical_functions mathematical_functions
//  */
// template <typename T> inline double Sum_S( const T* i, const T* j )
// {
//    Array<T> v( i, j );
//    v.Sort( [] ( const T& a, const T& b ) { return Abs( b ) < Abs( a ); }
//    double sum = 0;
//    for ( Array<T>::const_iterator i = v.Begin(); i != v.End(); ++i )
//       sum += double( *i );
//    return sum;
// }
//
// #endif // __PCL_CXX11

/*!
 * Returns the sum of the absolute values of the elements in a sequence [i,j[.
 *
 * For empty sequences, this function returns zero.
 *
 * See the remarks made for the Sum() function, which are equally applicable in
 * this case. See StableModulus() for a (slow) numerically stable version of
 * this function.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> inline double Modulus( const T* i, const T* j )
{
   double S = 0;
   for ( ; i < j; ++i )
      S += Abs( double( *i ) );
   return S;
}

/*!
 * Computes the sum of the absolute values of the elements in a sequence [i,j[
 * using a numerically stable summation algorithm to minimize roundoff error.
 *
 * For empty sequences, this function returns zero.
 *
 * In the current PCL versions, this function implements the Kahan summation
 * algorithm to reduce roundoff error to the machine's floating point error.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> inline double StableModulus( const T* i, const T* j )
{
   double sum = 0;
   double eps = 0;
   for ( ; i < j; ++i )
   {
      double y = Abs( double( *i ) ) - eps;
      double t = sum + y;
      eps = (t - sum) - y;
      sum = t;
   }
   return sum;
}

/*!
 * Returns the sum of the squares of the elements in a sequence [i,j[.
 *
 * For empty sequences, this function returns zero.
 *
 * See the remarks made for the Sum() function, which are equally applicable in
 * this case. See StableSumOfSquares() for a (slow) numerically stable version
 * of this function.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> inline double SumOfSquares( const T* i, const T* j )
{
   double Q = 0;
   for ( ; i < j; ++i )
      Q += double( *i ) * *i;
   return Q;
}

/*!
 * Computes the sum of the squares of the elements in a sequence [i,j[ using a
 * numerically stable summation algorithm to minimize roundoff error.
 *
 * For empty sequences, this function returns zero.
 *
 * In the current PCL versions, this function implements the Kahan summation
 * algorithm to reduce roundoff error to the machine's floating point error.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> inline double StableSumOfSquares( const T* i, const T* j )
{
   double sum = 0;
   double eps = 0;
   for ( ; i < j; ++i )
   {
      double y = double( *i ) * *i - eps;
      double t = sum + y;
      eps = (t - sum) - y;
      sum = t;
   }
   return sum;
}

/*!
 * Returns the arithmetic mean of a sequence [i,j[.
 *
 * For empty sequences, this function returns zero.
 *
 * See the remarks made for the Sum() function, which are equally applicable in
 * this case. See StableMean() for a (slow) numerically stable version of this
 * function.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> inline double Mean( const T* i, const T* j )
{
   distance_type n = j - i;
   if ( n < 1 )
      return 0;
   return Sum( i, j )/n;
}

/*!
 * Computes the arithmetic mean of a sequence [i,j[ using a numerically stable
 * summation algorithm to minimize roundoff error.
 *
 * For empty sequences, this function returns zero.
 *
 * In the current PCL versions, this function implements the Kahan summation
 * algorithm to reduce roundoff error to the machine's floating point error.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> inline double StableMean( const T* i, const T* j )
{
   distance_type n = j - i;
   if ( n < 1 )
      return 0;
   return StableSum( i, j )/n;
}

/*!
 * Returns the variance of a sequence [i,j[ with respect to the specified
 * \a center value.
 *
 * For sequences of less than two elements, this function returns zero.
 *
 * This implementation uses a two-pass compensated summation algorithm to
 * minimize roundoff errors (see the references).
 *
 * \b References
 *
 * William H. Press et al., <em>Numerical Recipes in C: The Art of Scientific
 * Computing, Second Edition</em> (1997 reprint) Cambridge University Press,
 * page 613.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> inline double Variance( const T* i, const T* j, double center )
{
   distance_type n = j - i;
   if ( n < 2 )
      return 0;
   double var = 0, eps = 0;
   for ( ; i < j; ++i )
   {
      double d = double( *i ) - center;
      var += d*d;
      eps += d;
   }
   return (var - eps*eps/n)/(n - 1);
}

/*!
 * Returns the variance from the mean of a sequence [i,j[.
 *
 * For sequences of less than two elements, this function returns zero.
 *
 * This implementation uses a two-pass compensated summation algorithm to
 * minimize roundoff errors (see References).
 *
 * \b References
 *
 * William H. Press et al., <em>Numerical Recipes in C: The Art of Scientific
 * Computing, Second Edition</em> (1997 reprint) Cambridge University Press,
 * page 613.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> inline double Variance( const T* i, const T* j )
{
   distance_type n = j - i;
   if ( n < 2 )
      return 0;
   double m = 0;
   for ( const T* f = i; f < j; ++f )
      m += double( *f );
   m /= n;
   double var = 0, eps = 0;
   for ( const T* f = i; f < j; ++f )
   {
      double d = double( *f ) - m;
      var += d*d;
      eps += d;
   }
   return (var - eps*eps/n)/(n - 1);
}

/*!
 * Returns the standard deviation of a sequence [i,j[ with respect to the
 * specified \a center value.
 *
 * For sequences of less than two elements, this function returns zero.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> inline double StdDev( const T* i, const T* j, double center )
{
   return Sqrt( Variance( i, j, center ) );
}

/*!
 * Returns the standard deviation from the mean of a sequence [i,j[.
 *
 * For sequences of less than two elements, this function returns zero.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> inline double StdDev( const T* i, const T* j )
{
   return Sqrt( Variance( i, j ) );
}

#define CMPXCHG( a, b )  \
   if ( i[b] < i[a] ) pcl::Swap( i[a], i[b] )

#define MEAN( a, b ) \
   (double( a ) + double( b ))/2

/*!
 * Returns the median value of a sequence [i,j[.
 *
 * Use fast, hard-coded selection networks for:
 *
 * \li Sequences of 32 or less elements of standard scalar types (char, int,
 * float, double, etc.), implemented as out-of-line specialized functions.
 *
 * \li Sequences of 9 or less elements of generic types, implemented as an
 * inline template function.
 *
 * Use a quick selection algorithm elsewhere.
 *
 * \b References (selection networks)
 *
 * \li Knuth, D. E., <em>The Art of Computer Programming, volume 3:
 * Sorting and Searching,</em> Addison Wesley, 1973.
 *
 * \li Hillis, W. D., <em>Co-evolving parasites improve simulated
 * evolution as an optimization procedure.</em> Langton, C. et al. (Eds.),
 * Artificial Life II. Addison Wesley, 1992.
 *
 * \li Hugues Juille, <em>Evolution of Non-Deterministic Incremental
 * Algorithms as a New Approach for Search in State Spaces,</em> 1995
 *
 * \b References (quick select algorithm)
 *
 * \li William H. Press et al., <em>Numerical Recipes 3rd Edition: The Art of
 * Scientific Computing,</em> Cambridge University Press, 2007, Section 8.5.
 *
 * \li Robert Sedgewick, Kevin Wayne, <em>Algorithms, 4th Edition,</em>
 * Addison-Wesley Professional, 2011, pp 345-347.
 *
 * \note This is a \e destructive median calculation algorithm: it may alter
 * the initial order of items in the specified [i,j[ sequence.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> inline double Median( T* i, T* j )
{
   distance_type n = j - i;
   if ( n < 1 )
      return 0;

   switch ( n )
   {
   case  1: // !?
      return i[0];
   case  2:
      return MEAN( i[0], i[1] );
   case  3:
      CMPXCHG( 0, 1 ); CMPXCHG( 1, 2 );
      return pcl::Max( i[0], i[1] );
   case  4:
      CMPXCHG( 0, 1 ); CMPXCHG( 2, 3 ); CMPXCHG( 0, 2 );
      CMPXCHG( 1, 3 );
      return MEAN( i[1], i[2] );
   case  5:
      CMPXCHG( 0, 1 ); CMPXCHG( 3, 4 ); CMPXCHG( 0, 3 );
      CMPXCHG( 1, 4 ); CMPXCHG( 1, 2 ); CMPXCHG( 2, 3 );
      return pcl::Max( i[1], i[2] );
   case  6:
      CMPXCHG( 0, 1 ); CMPXCHG( 2, 3 ); CMPXCHG( 0, 2 );
      CMPXCHG( 1, 3 ); CMPXCHG( 1, 2 ); CMPXCHG( 4, 5 );
      CMPXCHG( 0, 4 ); CMPXCHG( 1, 5 ); CMPXCHG( 1, 4 );
      CMPXCHG( 2, 4 ); CMPXCHG( 3, 5 ); CMPXCHG( 3, 4 );
      return MEAN( i[2], i[3] );
   case  7:
      CMPXCHG( 0, 5 ); CMPXCHG( 0, 3 ); CMPXCHG( 1, 6 );
      CMPXCHG( 2, 4 ); CMPXCHG( 0, 1 ); CMPXCHG( 3, 5 );
      CMPXCHG( 2, 6 ); CMPXCHG( 2, 3 ); CMPXCHG( 3, 6 );
      CMPXCHG( 4, 5 ); CMPXCHG( 1, 4 ); CMPXCHG( 1, 3 );
      return pcl::Min( i[3], i[4] );
   case  8:
      CMPXCHG( 0, 4 ); CMPXCHG( 1, 5 ); CMPXCHG( 2, 6 );
      CMPXCHG( 3, 7 ); CMPXCHG( 0, 2 ); CMPXCHG( 1, 3 );
      CMPXCHG( 4, 6 ); CMPXCHG( 5, 7 ); CMPXCHG( 2, 4 );
      CMPXCHG( 3, 5 ); CMPXCHG( 0, 1 ); CMPXCHG( 2, 3 );
      CMPXCHG( 4, 5 ); CMPXCHG( 6, 7 ); CMPXCHG( 1, 4 );
      CMPXCHG( 3, 6 );
      return MEAN( i[3], i[4] );
   case  9:
      CMPXCHG( 1, 2 ); CMPXCHG( 4, 5 ); CMPXCHG( 7, 8 );
      CMPXCHG( 0, 1 ); CMPXCHG( 3, 4 ); CMPXCHG( 6, 7 );
      CMPXCHG( 1, 2 ); CMPXCHG( 4, 5 ); CMPXCHG( 7, 8 );
      CMPXCHG( 0, 3 ); CMPXCHG( 5, 8 ); CMPXCHG( 4, 7 );
      CMPXCHG( 3, 6 ); CMPXCHG( 1, 4 ); CMPXCHG( 2, 5 );
      CMPXCHG( 4, 7 ); CMPXCHG( 4, 2 ); CMPXCHG( 6, 4 );
      return pcl::Min( i[2], i[4] );
   default:
      {
         distance_type n2 = distance_type( n >> 1 );
         if ( n & 1 )
            return *pcl::Select( i, j, n2 );
         return MEAN( *pcl::Select( i, j, n2 ), *pcl::Select( i, j, n2-1 ) );
      }
   }
}

#undef CMPXCHG

double PCL_FUNC Median( unsigned char* i, unsigned char* j );
double PCL_FUNC Median( signed char* i, signed char* j );
double PCL_FUNC Median( wchar_t* i, wchar_t* j );
double PCL_FUNC Median( unsigned short* i, unsigned short* j );
double PCL_FUNC Median( signed short* i, signed short* j );
double PCL_FUNC Median( unsigned int* i, unsigned int* j );
double PCL_FUNC Median( signed int* i, signed int* j );
double PCL_FUNC Median( unsigned long* i, unsigned long* j );
double PCL_FUNC Median( signed long* i, signed long* j );
double PCL_FUNC Median( unsigned long long* i, unsigned long long* j );
double PCL_FUNC Median( signed long long* i, signed long long* j );
double PCL_FUNC Median( float* i, float* j );
double PCL_FUNC Median( double* i, double* j );
double PCL_FUNC Median( long double* i, long double* j );

#define CMPXCHG( a, b )  \
   if ( p( i[b], i[a] ) ) pcl::Swap( i[a], i[b] )

/*!
 * Returns the median value of a sequence [i,j[. Element comparison is given by
 * a binary predicate \a p such that p( a, b ) is true for any pair a, b of
 * elements such that a precedes b.
 *
 * We use fast, hard-coded selection networks for sequences of 9 or less
 * elements, and a quick selection algorithm for larger sets.
 *
 * See the documentation of Median( T*, T* ) for references.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T, class BP> inline double Median( T* i, T* j, BP p )
{
   distance_type n = j - i;
   if ( n < 1 )
      return 0;

   switch ( n )
   {
   case  1: // !?
      return i[0];
   case  2:
      return MEAN( i[0], i[1] );
   case  3:
      CMPXCHG( 0, 1 ); CMPXCHG( 1, 2 );
      return pcl::Max( i[0], i[1] );
   case  4:
      CMPXCHG( 0, 1 ); CMPXCHG( 2, 3 ); CMPXCHG( 0, 2 );
      CMPXCHG( 1, 3 );
      return MEAN( i[1], i[2] );
   case  5:
      CMPXCHG( 0, 1 ); CMPXCHG( 3, 4 ); CMPXCHG( 0, 3 );
      CMPXCHG( 1, 4 ); CMPXCHG( 1, 2 ); CMPXCHG( 2, 3 );
      return pcl::Max( i[1], i[2] );
   case  6:
      CMPXCHG( 0, 1 ); CMPXCHG( 2, 3 ); CMPXCHG( 0, 2 );
      CMPXCHG( 1, 3 ); CMPXCHG( 1, 2 ); CMPXCHG( 4, 5 );
      CMPXCHG( 0, 4 ); CMPXCHG( 1, 5 ); CMPXCHG( 1, 4 );
      CMPXCHG( 2, 4 ); CMPXCHG( 3, 5 ); CMPXCHG( 3, 4 );
      return MEAN( i[2], i[3] );
   case  7:
      CMPXCHG( 0, 5 ); CMPXCHG( 0, 3 ); CMPXCHG( 1, 6 );
      CMPXCHG( 2, 4 ); CMPXCHG( 0, 1 ); CMPXCHG( 3, 5 );
      CMPXCHG( 2, 6 ); CMPXCHG( 2, 3 ); CMPXCHG( 3, 6 );
      CMPXCHG( 4, 5 ); CMPXCHG( 1, 4 ); CMPXCHG( 1, 3 );
      return pcl::Min( i[3], i[4] );
   case  8:
      CMPXCHG( 0, 4 ); CMPXCHG( 1, 5 ); CMPXCHG( 2, 6 );
      CMPXCHG( 3, 7 ); CMPXCHG( 0, 2 ); CMPXCHG( 1, 3 );
      CMPXCHG( 4, 6 ); CMPXCHG( 5, 7 ); CMPXCHG( 2, 4 );
      CMPXCHG( 3, 5 ); CMPXCHG( 0, 1 ); CMPXCHG( 2, 3 );
      CMPXCHG( 4, 5 ); CMPXCHG( 6, 7 ); CMPXCHG( 1, 4 );
      CMPXCHG( 3, 6 );
      return MEAN( i[3], i[4] );
   case  9:
      CMPXCHG( 1, 2 ); CMPXCHG( 4, 5 ); CMPXCHG( 7, 8 );
      CMPXCHG( 0, 1 ); CMPXCHG( 3, 4 ); CMPXCHG( 6, 7 );
      CMPXCHG( 1, 2 ); CMPXCHG( 4, 5 ); CMPXCHG( 7, 8 );
      CMPXCHG( 0, 3 ); CMPXCHG( 5, 8 ); CMPXCHG( 4, 7 );
      CMPXCHG( 3, 6 ); CMPXCHG( 1, 4 ); CMPXCHG( 2, 5 );
      CMPXCHG( 4, 7 ); CMPXCHG( 4, 2 ); CMPXCHG( 6, 4 );
      return pcl::Min( i[2], i[4] );
   default:
      {
         distance_type n2 = distance_type( n >> 1 );
         if ( n & 1 )
            return *pcl::Select( i, j, n2, p );
         return MEAN( *pcl::Select( i, j, n2, p ), *pcl::Select( i, j, n2-1, p ) );
      }
   }
}

#undef CMPXCHG
#undef MEAN

/*!
 * Returns the average absolute deviation of the values in a sequence [i,j[
 * with respect to the specified \a center value.
 *
 * When the median of the sequence is used as the center value, this function
 * returns the average absolute deviation from the median, which is a
 * well-known estimator of dispersion.
 *
 * For sequences of less than two elements, this function returns zero.
 *
 * See the remarks made for the Sum() function, which are equally applicable in
 * this case. See StableAvgDev() for a (slow) numerically stable version of
 * this function.
 *
 * \note To make the average absolute deviation about the median consistent
 * with the standard deviation of a normal distribution, it must be
 * multiplied by the constant 1.2533.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> inline double AvgDev( const T* i, const T* j, double center )
{
   distance_type n = j - i;
   if ( n < 2 )
      return 0;
   double d = 0;
   for ( ; i < j; ++i )
      d += Abs( double( *i ) - center );
   return d/n;
}

/*!
 * Returns the average absolute deviation of the values in a sequence [i,j[
 * with respect to the specified \a center value, using a numerically stable
 * summation algorithm to minimize roundoff error.
 *
 * When the median of the sequence is used as the center value, this function
 * returns the average absolute deviation from the median, which is a
 * well-known estimator of dispersion.
 *
 * For sequences of less than two elements, this function returns zero.
 *
 * In the current PCL versions, this function implements the Kahan summation
 * algorithm to reduce roundoff error to the machine's floating point error.
 *
 * \note To make the average absolute deviation about the median consistent
 * with the standard deviation of a normal distribution, it must be
 * multiplied by the constant 1.2533.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> inline double StableAvgDev( const T* i, const T* j, double center )
{
   distance_type n = j - i;
   if ( n < 2 )
      return 0;
   double sum = 0;
   double eps = 0;
   for ( ; i < j; ++i )
   {
      double y = Abs( double( *i ) - center ) - eps;
      double t = sum + y;
      eps = (t - sum) - y;
      sum = t;
   }
   return sum/n;
}

/*!
 * Returns the average absolute deviation from the median of the values in a
 * sequence [i,j[.
 *
 * The average absolute deviation from the median is a well-known estimator of
 * dispersion.
 *
 * For sequences of less than two elements, this function returns zero.
 *
 * See the remarks made for the Sum() function, which are equally applicable in
 * this case. See StableAvgDev() for a (slow) numerically stable version of
 * this function.
 *
 * \note To make the average absolute deviation about the median consistent
 * with the standard deviation of a normal distribution, it must be
 * multiplied by the constant 1.2533.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> inline double AvgDev( const T* i, const T* j )
{
   distance_type n = j - i;
   if ( n < 2 )
      return 0;
   T* t = new T[ n ];
   pcl::Copy( t, i, j );
   double m = Median( t, t+n );
   delete [] t;
   double d = 0;
   for ( ; i < j; ++i )
      d += Abs( double( *i ) - m );
   return d/n;
}

/*!
 * Computes the average absolute deviation from the median of the values in a
 * sequence [i,j[ using a numerically stable summation algorithm to minimize
 * roundoff error.
 *
 * The average absolute deviation from the median is a well-known estimator of
 * dispersion.
 *
 * For sequences of less than two elements, this function returns zero.
 *
 * In the current PCL versions, this function implements the Kahan summation
 * algorithm to reduce roundoff error to the machine's floating point error.
 *
 * \note To make the average absolute deviation about the median consistent
 * with the standard deviation of a normal distribution, it must be
 * multiplied by the constant 1.2533.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> inline double StableAvgDev( const T* i, const T* j )
{
   distance_type n = j - i;
   if ( n < 2 )
      return 0;
   T* t = new T[ n ];
   pcl::Copy( t, i, j );
   double m = Median( t, t+n );
   delete [] t;
   return StableAvgDev( i, j, m );
}

/*!
 * Returns the median absolute deviation (MAD) of the values in a sequence
 * [i,j[ with respect to the specified \a center value.
 *
 * The MAD is a well-known robust estimator of scale.
 *
 * For sequences of less than two elements, this function returns zero.
 *
 * \note To make the MAD estimator consistent with the standard deviation of
 * a normal distribution, it must be multiplied by the constant 1.4826.
 */
template <typename T> inline double MAD( const T* i, const T* j, double center )
{
   distance_type n = j - i;
   if ( n < 2 )
      return 0;
   double* d = new double[ n ];
   double* p = d;
   for ( const T* f = i; f < j; ++f, ++p )
      *p = Abs( double( *f ) - center );
   double m = pcl::Median( d, d+n );
   delete [] d;
   return m;
}

/*!
 * Returns the median absolute deviation from the median (MAD) for the values
 * in a sequence [i,j[.
 *
 * The MAD is a well-known robust estimator of scale.
 *
 * For sequences of less than two elements, this function returns zero.
 *
 * \note To make the MAD estimator consistent with the standard deviation of
 * a normal distribution, it must be multiplied by the constant 1.4826.
 */
template <typename T> inline double MAD( const T* i, const T* j )
{
   distance_type n = j - i;
   if ( n < 2 )
      return 0;
   double* d = new double[ n ];
   double* p = d;
   for ( const T* f = i; f < j; ++f, ++p )
      *p = double( *f );
   double m = pcl::Median( d, d+n );
   p = d;
   for ( const T* f = i; f < j; ++f, ++p )
      *p = Abs( *f - m );
   m = pcl::Median( d, d+n );
   delete [] d;
   return m;
}

/*!
 * Returns the Sn scale estimator of Rousseeuw and Croux for a sequence [x,xn[:
 *
 * Sn = c * low_median( high_median( |x_i - x_j| ) )
 *
 * where low_median() is the order statistic of rank (n + 1)/2, and
 * high_median() is the order statistic of rank n/2 + 1. n >= 2 is the number
 * of elements in the sequence: n = xn - x.
 *
 * This implementation is a direct C++ translation of the original FORTRAN
 * code by the authors (see References). The algorithm has O(n*log_2(n)) time
 * complexity and uses O(n) additional storage.
 *
 * The constant c = 1.1926 must be used to make the Sn estimator converge to
 * the standard deviation of a pure normal distribution. However, this
 * implementation does not apply it (it uses c=1 implicitly), for
 * consistency with other implementations of scale estimators.
 *
 * \note This is a \e destructive algorithm: it may alter the initial order of
 * items in the specified [x,xn[ sequence.
 *
 * \ingroup statistical_functions mathematical_functions
 *
 * \b References
 *
 * P.J. Rousseeuw and C. Croux (1993), <em>Alternatives to the Median Absolute
 * Deviation,</em> Journal of the American Statistical Association, Vol. 88,
 * pp. 1273-1283.
 */
template <typename T> double Sn( T* x, T* xn )
{
   /*
    * NB: In the code below, lines commented with an asterisk (*) have been
    * modified with respect to the FORTRAN original to account for
    * zero-based array indices.
    */

   distance_type n = xn - x;
   if ( n < 2 )
      return 0;

   pcl::Sort( x, xn );

   T* a2 = new T[ n ];
   a2[0] = x[n >> 1] - x[0];                          // *

   distance_type nh = (n + 1) >> 1;

   for ( distance_type i = 2; i <= nh; ++i )
   {
      distance_type nA = i-1;
      distance_type nB = n - i;
      distance_type diff = nB - nA;
      distance_type leftA = 1;
      distance_type leftB = 1;
      distance_type rightA = nB;
      distance_type Amin = (diff >> 1) + 1;
      distance_type Amax = (diff >> 1) + nA;

      while ( leftA < rightA )
      {
         distance_type length = rightA - leftA + 1;
         distance_type even = (length & 1) == 0;
         distance_type half = (length - 1) >> 1;
         distance_type tryA = leftA + half;
         distance_type tryB = leftB + half;

         if ( tryA < Amin )
            leftA = tryA + even;
         else
         {
            if ( tryA > Amax )
            {
               rightA = tryA;
               leftB = tryB + even;
            }
            else
            {
               T medA = x[i-1] - x[i-2 - tryA + Amin];// *
               T medB = x[tryB + i-1] - x[i-1];       // *
               if ( medA >= medB )
               {
                  rightA = tryA;
                  leftB = tryB + even;
               }
               else
                  leftA = tryA + even;
            }
         }
      }

      if ( leftA > Amax )
         a2[i-1] = x[leftB + i-1] - x[i-1];           // *
      else
      {
         T medA = x[i-1] - x[i-2 - leftA + Amin];     // *
         T medB = x[leftB + i-1] - x[i-1];
         a2[i-1] = pcl::Min( medA, medB );            // *
      }
   }

   for ( distance_type i = nh + 1; i < n; ++i )
   {
      distance_type nA = n - i;
      distance_type nB = i - 1;
      distance_type diff = nB - nA;
      distance_type leftA = 1;
      distance_type leftB = 1;
      distance_type rightA = nB;
      distance_type Amin = (diff >> 1) + 1;
      distance_type Amax = (diff >> 1) + nA;

      while ( leftA < rightA )
      {
         distance_type length = rightA - leftA + 1;
         distance_type even = (length & 1) == 0;
         distance_type half = (length - 1) >> 1;
         distance_type tryA = leftA + half;
         distance_type tryB = leftB + half;

         if ( tryA < Amin)
            leftA = tryA + even;
         else
         {
            if ( tryA > Amax )
            {
               rightA = tryA;
               leftB = tryB + even;
            }
            else
            {
               T medA = x[i + tryA - Amin] - x[i-1];  // *
               T medB = x[i-1] - x[i-1 - tryB];       // *
               if ( medA >= medB )
               {
                  rightA = tryA;
                  leftB = tryB + even;
               }
               else
                  leftA = tryA + even;
            }
         }
      }

      if ( leftA > Amax )
         a2[i-1] = x[i-1] - x[i-1 - leftB];           // *
      else
      {
         T medA = x[i + leftA - Amin] - x[i-1];       // *
         T medB = x[i-1] - x[i-1 - leftB];            // *
         a2[i-1] = pcl::Min( medA, medB );            // *
      }
   }

   a2[n-1] = x[n-1] - x[nh-1];                        // *

   /*
    * Correction for a finite sample
    */
   double cn;
   switch ( n )
   {
   case  2: cn = 0.743; break;
   case  3: cn = 1.851; break;
   case  4: cn = 0.954; break;
   case  5: cn = 1.351; break;
   case  6: cn = 0.993; break;
   case  7: cn = 1.198; break;
   case  8: cn = 1.005; break;
   case  9: cn = 1.131; break;
   default: cn = (n & 1) ? n/(n - 0.9) : 1.0; break;
   }

   double sn = cn * double( *pcl::Select( a2, a2+n, nh-1 ) );

   delete [] a2;
   return sn;
}

/*
 * Auxiliary routine for Qn().
 *
 * Algorithm to compute the weighted high median in O(n) time.
 *
 * The weighted high median is defined as the smallest a[j] such that the sum
 * of the weights of all a[i] <= a[j] is strictly greater than half of the
 * total weight.
 */
inline double __pcl_whimed__( double* a, distance_type* iw, distance_type n,
                              double* acand, distance_type* iwcand )
{
   distance_type wtotal = 0;
   for ( distance_type i = 0; i < n; ++i )
      wtotal += iw[i];

   for ( distance_type nn = n, wrest = 0; ; )
   {
      double trial = *pcl::Select( a, a+nn, nn >> 1 ); // *

      distance_type wleft = 0;
      distance_type wmid = 0;
      distance_type wright = 0;
      for ( distance_type i = 0; i < nn; ++i )
         if ( a[i] < trial )
            wleft += iw[i];
         else if ( a[i] > trial )
            wright += iw[i];
         else
            wmid += iw[i];

      if ( 2*(wrest + wleft) > wtotal )
      {
         distance_type kcand = 0;
         for ( distance_type i = 0; i < nn; ++i )
            if ( a[i] < trial )
            {
               acand[kcand] = a[i];
               iwcand[kcand] = iw[i];
               ++kcand;
            }
          nn = kcand;
      }
      else
      {
         if ( 2*(wrest + wleft + wmid) > wtotal )
            return trial;

         distance_type kcand = 0;
         for ( distance_type i = 0; i < nn; ++i )
            if ( a[i] > trial )
            {
               acand[kcand] = a[i];
               iwcand[kcand] = iw[i];
               ++kcand;
            }
         nn = kcand;
         wrest += wleft + wmid;
      }

      for ( distance_type i = 0; i < nn; ++i )
      {
         a[i] = acand[i];
         iw[i] = iwcand[i];
      }
   }
}

/*!
 * Returns the Qn scale estimator of Rousseeuw and Croux for a sequence [x,xn[:
 *
 * Qn = c * first_quartile( |x_i - x_j| : i < j )
 *
 * where first_quartile() is the order statistic of rank (n + 1)/4. n >= 2 is
 * the number of elements in the sequence: n = xn - x.
 *
 * This implementation is a C++ translation of the original FORTRAN code by the
 * authors (see References). The algorithm has O(n*log_2(n)) time complexity
 * and requires about O(8*n) additional storage.
 *
 * The constant c = 2.2219 must be used to make the Qn estimator converge to
 * the standard deviation of a pure normal distribution. However, this
 * implementation does not apply it (it uses c=1 implicitly), for consistency
 * with other implementations of scale estimators.
 *
 * \note This is a \e destructive algorithm: it may alter the initial order of
 * items in the specified [x,xn[ sequence.
 *
 * \ingroup statistical_functions mathematical_functions
 *
 * \b References
 *
 * P.J. Rousseeuw and C. Croux (1993), <em>Alternatives to the Median Absolute
 * Deviation,</em> Journal of the American Statistical Association, Vol. 88,
 * pp. 1273-1283.
 */
template <typename T> double Qn( T* x, T* xn )
{
   distance_type n = xn - x;
   if ( n < 2 )
      return 0;

   T*             y      = new T[ n ];
   double*        work   = new double[ n ];
   double*        acand  = new double[ n ];
   distance_type* iwcand = new distance_type[ n ];
   distance_type* left   = new distance_type[ n ];
   distance_type* right  = new distance_type[ n ];
   distance_type* P      = new distance_type[ n ];
   distance_type* Q      = new distance_type[ n ];
   distance_type* weight = new distance_type[ n ];

   distance_type h = (n >> 1) + 1;
   distance_type k = (h*(h - 1)) >> 1;
   for ( distance_type i = 0; i < n; ++i )
   {
      y[i] = x[i];
      left[i] = n - i + 1; // *
      right[i] = (i <= h) ? n : n - i + h; // the original code is "right[i] = n"
   }

   pcl::Sort( y, y+n );

   distance_type nL = (n*(n + 1)) >> 1;
   distance_type nR = n*n;
   distance_type knew = k + nL;

   bool found = false;
   double qn;

   while ( nR-nL > n )
   {
      distance_type j = 0; // *
      for ( distance_type i = 1; i < n; ++i ) // *
         if ( left[i] <= right[i] )
         {
            weight[j] = right[i] - left[i] + 1;
            work[j] = double( y[i] ) - y[n - left[i] - (weight[j] >> 1)];
            ++j;
         }
      qn = __pcl_whimed__( work, weight, j, acand, iwcand );

      for ( distance_type i = n-1, j = 0; i >= 0; --i ) // *
      {
         while ( j < n && double( y[i] ) - y[n-j-1] < qn )
            ++j;
         P[i] = j;
      }

      for ( distance_type i = 0, j = n+1; i < n; ++i ) // *
      {
         while ( double( y[i] ) - y[n-j+1] > qn )
            --j;
         Q[i] = j;
      }

      double sumP = 0;
      double sumQ = 0;
      for ( distance_type i = 0; i < n; ++i )
      {
         sumP += P[i];
         sumQ += Q[i] - 1;
      }

      if ( knew <= sumP )
      {
         for ( distance_type i = 0; i < n; ++i )
            right[i] = P[i];
         nR = sumP;
      }
      else if ( knew > sumQ )
      {
         for ( distance_type i = 0; i < n; ++i )
            left[i] = Q[i];
         nL = sumQ;
      }
      else
      {
         found = true;
         break;
      }
   }

   if ( !found )
   {
      distance_type j = 0;
      for ( distance_type i = 1; i < n; ++i )
         for ( distance_type jj = left[i]; jj <= right[i]; ++jj, ++j )
            work[j] = double( y[i] ) - y[n-jj]; // *
      qn = *pcl::Select( work, work+j, knew-nL-1 ); // *
   }

   /*
    * Correction for a finite sample
    */
   double dn;
   switch ( n )
   {
   case  2: dn = 0.399; break;
   case  3: dn = 0.994; break;
   case  4: dn = 0.512; break;
   case  5: dn = 0.844; break;
   case  6: dn = 0.611; break;
   case  7: dn = 0.857; break;
   case  8: dn = 0.669; break;
   case  9: dn = 0.872; break;
   default: dn = (n & 1) ? n/(n + 1.4) : n/(n + 3.8); break;
   }
   qn *= dn;

   delete [] y;
   delete [] work;
   delete [] acand;
   delete [] iwcand;
   delete [] left;
   delete [] right;
   delete [] P;
   delete [] Q;
   delete [] weight;

   return qn;
}

/*!
 * Returns a biweight midvariance (BWMV) for the elements in a sequence [x,xn[.
 *
 * \param x, xn   Define a sequence of sample data points for which the BWMV
 *                estimator will be calculated.
 *
 * \param center  Reference center value. Normally, the median of the sample
 *                should be used.
 *
 * \param sigma   A reference estimate of dispersion. Normally, the median
 *                absolute deviation from the median (MAD) of the sample should
 *                be used.
 *
 * \param k       Rejection limit in sigma units. The default value is k=9.
 *
 * The square root of the biweight midvariance is a robust estimator of scale.
 * It is an efficient estimator with respect to many statistical distributions
 * (about 87% Gaussian efficiency), and appears to have a breakdown point close
 * to 0.5 (the same as MAD).
 *
 * For sequences of less than two elements, this function returns zero.
 *
 * \b References
 *
 * Rand R. Wilcox (2012), <em>Introduction to Robust Estimation and Hypothesis
 * Testing, 3rd Edition</em>, Elsevier Inc., Section 3.12.1.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> double BiweightMidvariance( const T* x, const T* xn, double center, double sigma, int k = 9 )
{
   distance_type n = xn - x;
   if ( n < 2 )
      return 0;

   double kd = k * sigma;
   if ( kd < 0 || 1 + kd == 1 )
      return 0;

   double num = 0, den = 0;
   for ( ; x < xn; ++x )
   {
      double xc = *x - center;
      double y = xc/kd;
      if ( Abs( y ) < 1 )
      {
         double y2 = y*y;
         double y21 = 1 - y2;
         num += xc*xc * y21*y21*y21*y21;
         den += y21 * (1 - 5*y2);
      }
   }

   den *= den;
   if ( 1 + den == 1 )
      return 0;
   return n*num/den;
}

/*!
 * Returns a percentage bend midvariance (PBMV) for the elements in a sequence
 * [x,xn[.
 *
 * \param x, xn   Define a sequence of sample data points for which the PBWV
 *                estimator will be calculated.
 *
 * \param center  Reference center value. Normally, the median of the sample
 *                should be used.
 *
 * \param beta    Rejection parameter in the [0,0.5] range. Higher values
 *                improve robustness to outliers (i.e., increase the breakdown
 *                point of the estimator) at the expense of lower efficiency.
 *                The default value is beta=0.2.
 *
 * The square root of the percentage bend midvariance is a robust estimator of
 * scale. With the default beta=0.2, its Gaussian efficiency is 67%. With
 * beta=0.1, its efficiency is 85% but its breakdown is only 0.1.
 *
 * For sequences of less than two elements, this function returns zero.
 *
 * \b References
 *
 * Rand R. Wilcox (2012), <em>Introduction to Robust Estimation and Hypothesis
 * Testing, 3rd Edition</em>, Elsevier Inc., Section 3.12.3.
 *
 * \ingroup statistical_functions mathematical_functions
 */
template <typename T> double BendMidvariance( const T* x, const T* xn, double center, double beta = 0.2 )
{
   distance_type n = xn - x;
   if ( n < 2 )
      return 0;

   beta = Range( beta, 0.0, 0.5 );
   distance_type m = Floor( (1 - beta)*n + 0.5 );

   double* w = new double[ n ];
   for ( distance_type i = 0; i < n; ++i )
      w[i] = Abs( double( x[i] ) - center );
   double wb = *pcl::Select( w, w+n, m );
   delete [] w;
   if ( 1 + wb == 1 )
      return 0;

   double num = 0;
   distance_type den = 0;
   for ( ; x < xn; ++x )
   {
      double y = (double( *x ) - center)/wb;
      double f = Max( -1.0, Min( 1.0, y ) );
      num += f*f;
      if ( Abs( y ) < 1 )
         ++den;
   }

   if ( den == 0 )
      return 0;
   return n*wb*wb*num/den/den;
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Math_h

// ****************************************************************************
// EOF pcl/Math.h - Released 2014/11/14 17:16:34 UTC
