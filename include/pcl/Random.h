//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// pcl/Random.h - Released 2015/11/26 15:59:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_Random_h
#define __PCL_Random_h

/// \file pcl/Random.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_AutoPointer_h
#include <pcl/AutoPointer.h>
#endif

#ifndef __PCL_Vector_h
#include <pcl/Vector.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup random_numbers Random Numbers
 */

/*!
 * Returns a 64-bit random generator seed.
 *
 * On UNIX/Linux platforms, this function reads the /dev/urandom system device
 * to acquire a high-quality random seed. On Windows, the rand_s() CRT function
 * is invoked with the same purpose.
 *
 * In the extremely rare cases where a system random seed cannot be obtained,
 * the time() function is used to get a unique initialization value.
 *
 * Subsequent calls to this function are guaranteed to return unique values.
 *
 * This function is thread-safe. It can be safely called from multiple
 * execution threads running concurrently.
 *
 * \ingroup random_numbers
 */
extern uint64 RandomSeed64();

/*!
 * Returns a 32-bit random generator seed.
 *
 * This function simply calls RandomSeed64() and returns the XOR combination of
 * the 32-bit words in the 64-bit random seed.
 *
 * As RandomSeed64(), this function is thread-safe and is guaranteed to return
 * a unique value on each call.
 *
 * \ingroup random_numbers
 */
inline uint32 RandomSeed32()
{
   union { uint64 u64; uint32 u32[ 2 ]; } seed;
   seed.u64 = RandomSeed64();
   return seed.u32[0] ^ seed.u32[1];
}

// ----------------------------------------------------------------------------

class FastMersenneTwister;

// ----------------------------------------------------------------------------

/*!
 * \class RandomNumberGenerator
 * \brief Mersenne Twister (MT19937) pseudo-random number generator.
 *
 * Generation of pseudo-random numbers with user-selectable range and
 * probability distributions.
 *
 * This generator supports the uniform, normal (Gaussian) and Poisson
 * distributions. In addition, the upper range of generated uniform deviates
 * can be arbitrarily defined.
 *
 * %RandomNumberGenerator is a functional class. The function call operator()()
 * returns pseudo-random numbers in the range [0,ymax], where ymax is the
 * user-defined arbitrary upper range.
 *
 * Example of use:
 *
 * \code
 * RandomNumberGenerator R, R1( 10 ); // R's ymax = 1, R1's ymax = 10
 * // ...
 * double y = R();   // y = random uniform deviate in the range [0,1]
 * double z = R1();  // z = random uniform deviate in the range [0,10]
 * \endcode
 *
 * <b>References</b>
 *
 * Based on an adaptation of SIMD-oriented Fast Mersenne Twister (SFMT) by
 * Mutsuo Saito and Makoto Matsumoto (Hiroshima University).
 *
 * Currently PCL implements a SFMT generator with a period of 2^19937-1.
 *
 * SFMT Copyright (C) 2006, 2007 Mutsuo Saito, Makoto Matsumoto and Hiroshima
 * University. All rights reserved.
 *
 * \ingroup random_numbers
 */
class PCL_CLASS RandomNumberGenerator
{
public:

   /*!
    * Constructs a %RandomNumberGenerator object.
    *
    * \param ymax    Upper bound of uniform deviates. The function call
    *                operator double operator()() and the Uniform() member
    *                function (which are synonyms) will return uniform
    *                pseudo-random deviates in the range [0,ymax]. The default
    *                value is 1.0.
    *
    * \param seed    32-bit initialization seed. If this parameter is zero, a
    *                unique random seed will be generated automatically. The
    *                default value is zero.
    */
   RandomNumberGenerator( double ymax = 1.0, uint32 seed = 0 );

   /*!
    * Destroys a %RandomNumberGenerator object.
    */
   virtual ~RandomNumberGenerator();

   /*!
    * Generates a floating point uniform deviate in the range [0,UpperBound()]
    */
   double operator ()()
   {
      return m_rmax*Rand32();
   }

   /*!
    * Generates a 32-bit unsigned integer uniform deviate.
    */
   uint32 Rand32();

   /*!
    * Generates a floating point uniform deviate in the range [0,1] (i.e.,
    * ignoring UpperBound()).
    */
   double Rand1()
   {
      return double( Rand32() )/uint32_max;
   }

   /*!
    * Generates a floating point uniform deviate in the range [0,UpperBound()]
    *
    * This is a convenience alias for operator()().
    */
   double Uniform()
   {
      return operator()();
   }

   /*!
    * Generates a floating point normal deviate with the specified \a mean and
    * standard deviation \a sigma.
    */
   double Normal( double mean = 0, double sigma = 1 );

   /*!
    * Generates a floating point normal deviate with the specified \a mean and
    * standard deviation \a sigma.
    *
    * This is a convenience alias for Normal( mean, sigma ).
    */
   double Gaussian( double mean = 0, double sigma = 1 )
   {
      return Normal( mean, sigma );
   }

   /*!
    * Generates a discrete random deviate from a Poisson distribution with the
    * specified expected value \a lambda.
    */
   int Poisson( double lambda );

   /*!
    * Returns the current upper bound of this random number generator.
    */
   double UpperBound() const
   {
      return m_ymax;
   }

   /*!
    * Sets the upper bound \a ymax > 0 for this random number generator.
    */
   void SetUpperBound( double ymax )
   {
      PCL_PRECONDITION( ymax > 0 )
      PCL_PRECONDITION( 1 + ymax != 1 )
      m_rmax = (m_ymax = ymax)/double( uint32_max );
      m_normal = false;
   }

private:

   AutoPointer<FastMersenneTwister> m_generator;
   double                           m_ymax;
   double                           m_rmax;
   bool                             m_normal : 1;
   double                           m_vs;     // second result from Box–Muller transform
   DVector                          m_lambda; // precalculated for current Poisson lambda
};

// ----------------------------------------------------------------------------

/*!
 * \class XorShift1024
 * \brief Implementation of the XorShift1024* pseudo-random number generator.
 *
 * Generation of pseudo-random uniform deviates using the xorshift1024*
 * generator developed in 2014 by Sebastiano Vigna. This generator has a period
 * of 2^1024-1 and passes strong statistical test suites.
 *
 * Examples of use:
 *
 * \code
 * XorShift1024 X; // initialized automatically
 * // ...
 * double x = X();      // x = random uniform deviate in the range [0,1)
 * uint64 y = X.UI64(); // y = 64-bit unsigned integer random uniform deviate
 * uint32 z = X.UI32(); // z = 32-bit unsigned integer random uniform deviate
 * uint32 t = X.UIN( 100 ); // t = integer uniform deviate in the range [0,99]
 * \endcode
 *
 * <b>References</b>
 *
 * Sebastiano Vigna (2014), <em>An experimental exploration of Marsaglia's
 * xorshift generators, scrambled</em>, arXiv:1402.6246
 *
 * Sebastiano Vigna (2014), <em>Further scramblings of Marsaglia's xorshift
 * generators</em>, arXiv:1404.0390
 *
 * See also: http://xorshift.di.unimi.it/
 *
 * \ingroup random_numbers
 */
class XorShift1024
{
public:

   /*!
    * Constructs a %XorShift1024 random generator.
    *
    * \param seed    64-bit initialization seed. If this parameter is zero, a
    *                unique random seed will be generated automatically. The
    *                default value is zero.
    */
   XorShift1024( uint64 seed = 0 )
   {
      Initialize( seed );
   }

   /*!
    * Returns a double precision uniform random deviate in the [0,1) range.
    */
   double operator()()
   {
      return 5.4210108624275221703311e-20 * UI64(); // 1.0/(2^64 -1)
   }

   /*!
    * Returns a 64-bit unsigned integer uniform random deviate.
    */
   uint64 UI64()
   {
      uint64 s0 = m_s[m_p];
      uint64 s1 = m_s[m_p = (m_p + 1) & 15];
      s1 ^= s1 << 31; // a
      s1 ^= s1 >> 11; // b
      s0 ^= s0 >> 30; // c
      return (m_s[m_p] = s0 ^ s1) * 1181783497276652981ull;
   }

   /*!
    * Returns a 32-bit unsigned integer uniform random deviate.
    */
   uint32 UI32()
   {
      return uint32( UI64() );
   }

   /*!
    * Returns a 64-bit unsigned integer uniform random deviate in the range
    * [0,n-1].
    */
   uint64 UI64N( uint64 n )
   {
      return UI64() % n;
   }

   /*!
    * Returns an unsigned integer uniform random deviate in the range [0,n-1].
    */
   uint32 UIN( uint32 n )
   {
      return UI64() % n;
   }

   /*!
    * A synonym for UIN().
    */
   uint32 UI32N( uint32 n )
   {
      return UIN( n );
   }

   /*
    * Reinitializes this generator with a new \a seed. If the specified \a seed
    * is zero, a unique random seed will be generated automatically.
    */
   void Initialize( uint64 x )
   {
      if ( x == 0 )
         x = RandomSeed64();
      // Use a xorshift64* generator to initialize the state space.
      for ( int i = 0; i < 16; ++i )
      {
         x ^= x >> 12; // a
         x ^= x << 25; // b
         x ^= x >> 27; // c
         m_s[i] = x * 2685821657736338717ull;
      }
      m_p = 0;
   }

private:

   uint64 m_s[ 16 ]; // state space
   int m_p;          // current index
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Random_h

// ----------------------------------------------------------------------------
// EOF pcl/Random.h - Released 2015/11/26 15:59:39 UTC
