//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// pcl/Random.cpp - Released 2017-05-02T10:39:13Z
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

#include <pcl/AutoLock.h>
#include <pcl/Exception.h>
#include <pcl/Math.h>
#include <pcl/Random.h>

#include <time.h>

#ifndef __PCL_WINDOWS
#  include <fcntl.h>
#  include <unistd.h>
#  include <iostream>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

static bool GetSystemSeed( void* bytes, size_type n )
{
   if ( n < sizeof( unsigned ) )
      return false; // !?
#ifdef __PCL_WINDOWS
   unsigned* words = reinterpret_cast<unsigned*>( bytes );
   for ( size_type i = 0; i < n; i += sizeof( unsigned ), ++words )
      if ( rand_s( words ) != 0 )
         return false;
   return true;
#else
   int fn = open( "/dev/urandom", O_RDONLY );
   if ( fn == -1 || read( fn, bytes, n ) != fsize_type( n ) )
   {
      std::cout << "\n** Warning: Could not read /dev/urandom\n" << std::flush;
      return false;
   }
   close( fn );
   return true;
#endif
}

static uint64 TimeSeed()
{
   union { time_t t; uint64 u; } t;
   t.t = time( 0 );
   return t.u;
}

static uint64 XorShift64Seed( uint64& x )
{
   for ( ;; )
   {
      x ^= x >> 12; // a
      x ^= x << 25; // b
      x ^= x >> 27; // c
      uint64 r = x * 2685821657736338717ull;
      if ( r != 0 )
         return r;
   }
}

uint64 RandomSeed64()
{
   static Mutex mutex;
   static uint64 seed = 0;
   volatile AutoLock lock( mutex );
   if ( seed == 0 )
      if ( !GetSystemSeed( &seed, sizeof( seed ) ) )
         seed = TimeSeed();
   return XorShift64Seed( seed );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/*
 * SIMD-oriented Fast Mersenne Twister (SFMT)
 *
 * Mersenne Twister(MT) is a pseudorandom number generating algorithm developed
 * by Makoto Matsumoto and Takuji Nishimura. The version implemented here is
 * based on their original C code and has a period of at least 2^19937 - 1.
 */

/*
 * Mersenne Exponent. The period of the sequence is a multiple of 2^MEXP-1.
 */
#define MEXP     19937

#define POS1     122
#define SL1      18
#define SL2      1
#define SR1      11
#define SR2      1
#define MSK1     0xdfffffefU
#define MSK2     0xddfecb7fU
#define MSK3     0xbffaffffU
#define MSK4     0xbffffff6U
#define PARITY1  0x00000001U
#define PARITY2  0x00000000U
#define PARITY3  0x00000000U
#define PARITY4  0x13c9e684U

/*
 * The SFMT generator has an internal state array of N 128-bit integers.
 */
#define N   (MEXP/128 + 1)

/*
 * N32 is the size of internal state array when regarded as an array of 32-bit
 * integers.
 */
#define N32 (N * 4)

#ifdef SSE2

#include <emmintrin.h>

union w128_t
{
   __m128i si;
   uint32 u[ 4 ];
};

/*
 * SSE2 implementation of the recursion formula.
 */
inline static __m128i mm_recursion( __m128i *a, __m128i *b, __m128i c, __m128i d, __m128i mask )
{
   __m128i v, x, y, z;

   x = _mm_load_si128(a);
   y = _mm_srli_epi32(*b, SR1);
   z = _mm_srli_si128(c, SR2);
   v = _mm_slli_epi32(d, SL1);
   z = _mm_xor_si128(z, x);
   z = _mm_xor_si128(z, v);
   x = _mm_slli_si128(x, SL2);
   y = _mm_and_si128(y, mask);
   z = _mm_xor_si128(z, x);
   z = _mm_xor_si128(z, y);
   return z;
}

#else

/*
 * 128-bit SIMD-like data type for standard C
 */
struct w128_t
{
   uint32 u[ 4 ];
};

#endif // SSE2

class FastMersenneTwister
{
public:

   FastMersenneTwister( uint32 seed )
   {
      m_psfmt32 = &m_sfmt[0].u[0];

      m_psfmt32[0] = seed;
      for ( int i = 1; i < N32; i++ )
         m_psfmt32[i] = 1812433253UL * (m_psfmt32[i-1] ^ (m_psfmt32[i-1] >> 30)) + i;
      m_idx = N32;
      PeriodCertification();
   }

   /*
    * Generates a 32-bit pseudo-random number.
    */
   uint32 operator()()
   {
      if ( m_idx >= N32 )
      {
         Regenerate();
         m_idx = 0;
      }
      return m_psfmt32[m_idx++];
   }

private:

   /*
    * The 128-bit internal state array
    */
   w128_t m_sfmt[ N ];

   /*
    * The 32-bit integer pointer to the 128-bit internal state array
    */
   uint32* m_psfmt32;

   /*
    * index counter to the 32-bit internal state array
    */
   int m_idx;

   /*
    * Simulates SIMD 128-bit left shift by the standard C. The 128-bit integer
    * given in in is shifted by (shift * 8) bits. This function simulates the
    * LITTLE ENDIAN SIMD.
    */
   static void LeftShift128( w128_t *out, w128_t const *in, int shift )
   {
      uint64 th, tl, oh, ol;

      th = ((uint64)in->u[3] << 32) | ((uint64)in->u[2]);
      tl = ((uint64)in->u[1] << 32) | ((uint64)in->u[0]);

      oh = th << (shift * 8);
      ol = tl << (shift * 8);
      oh |= tl >> (64 - shift * 8);
      out->u[1] = (uint32)(ol >> 32);
      out->u[0] = (uint32)ol;
      out->u[3] = (uint32)(oh >> 32);
      out->u[2] = (uint32)oh;
   }

   /*
    * Simulates SIMD 128-bit right shift by the standard C. The 128-bit integer
    * given in in is shifted by (shift * 8) bits. This function simulates the
    * LITTLE ENDIAN SIMD.
    */
   static void RightShift128( w128_t *out, w128_t const *in, int shift )
   {
      uint64 th, tl, oh, ol;

      th = ((uint64)in->u[3] << 32) | ((uint64)in->u[2]);
      tl = ((uint64)in->u[1] << 32) | ((uint64)in->u[0]);

      oh = th >> (shift * 8);
      ol = tl >> (shift * 8);
      ol |= th << (64 - shift * 8);
      out->u[1] = (uint32)(ol >> 32);
      out->u[0] = (uint32)ol;
      out->u[3] = (uint32)(oh >> 32);
      out->u[2] = (uint32)oh;
   }

   /*
    * Recursion formula.
    */
   static void DoRecursion( w128_t *r, w128_t *a, w128_t *b, w128_t *c, w128_t *d )
   {
      w128_t x;
      w128_t y;

      LeftShift128( &x, a, SL2 );
      RightShift128( &y, c, SR2 );
      r->u[0] = a->u[0] ^ x.u[0] ^ ((b->u[0] >> SR1) & MSK1) ^ y.u[0]
      ^ (d->u[0] << SL1);
      r->u[1] = a->u[1] ^ x.u[1] ^ ((b->u[1] >> SR1) & MSK2) ^ y.u[1]
      ^ (d->u[1] << SL1);
      r->u[2] = a->u[2] ^ x.u[2] ^ ((b->u[2] >> SR1) & MSK3) ^ y.u[2]
      ^ (d->u[2] << SL1);
      r->u[3] = a->u[3] ^ x.u[3] ^ ((b->u[3] >> SR1) & MSK4) ^ y.u[3]
      ^ (d->u[3] << SL1);
   }

   /*
    * Fills the internal state array with pseudorandom integers.
    */
   void Regenerate()
   {
#ifdef SSE2

      int i;
      __m128i r, r1, r2, mask;
      mask = _mm_set_epi32( MSK4, MSK3, MSK2, MSK1 );

      r1 = _mm_load_si128( &m_sfmt[N - 2].si );
      r2 = _mm_load_si128( &m_sfmt[N - 1].si );
      for ( i = 0; i < N - POS1; i++ )
      {
         r = mm_recursion( &m_sfmt[i].si, &m_sfmt[i + POS1].si, r1, r2, mask );
         _mm_store_si128( &m_sfmt[i].si, r );
         r1 = r2;
         r2 = r;
      }

      for ( ; i < N; i++ )
      {
         r = mm_recursion( &m_sfmt[i].si, &m_sfmt[i + POS1 - N].si, r1, r2, mask );
         _mm_store_si128( &m_sfmt[i].si, r );
         r1 = r2;
         r2 = r;
      }

#else // !SSE2

      w128_t* r1 = &m_sfmt[N - 2];
      w128_t* r2 = &m_sfmt[N - 1];

      int i;

      for ( i = 0; i < N - POS1; i++ )
      {
         DoRecursion( &m_sfmt[i], &m_sfmt[i], &m_sfmt[i + POS1], r1, r2 );
         r1 = r2;
         r2 = &m_sfmt[i];
      }

      for ( ; i < N; i++ )
      {
         DoRecursion( &m_sfmt[i], &m_sfmt[i], &m_sfmt[i + POS1 - N], r1, r2 );
         r1 = r2;
         r2 = &m_sfmt[i];
      }

#endif // SSE2
   }

   /*
    * Certificates the period of 2^MEXP
    */
   void PeriodCertification()
   {
      static const uint32 parity[ 4 ] = { PARITY1, PARITY2, PARITY3, PARITY4 };

      int inner = 0;

      for ( int i = 0; i < 4; i++ )
      {
         uint32 work = m_psfmt32[i] & parity[i];
         for ( int j = 0; j < 32; j++ )
         {
            inner ^= work & 1;
            work >>= 1;
         }
      }

      // check OK
      if ( inner == 1 )
         return;

      // check NG, and modification
      for ( int i = 0; i < 4; i++ )
      {
         uint32 work = 1;
         for ( int j = 0; j < 32; j++ )
         {
            if ( (work & parity[i]) != 0 )
            {
               m_psfmt32[i] ^= work;
               return;
            }
            work <<= 1;
         }
      }
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

RandomNumberGenerator::RandomNumberGenerator( double ymax, uint32 seed ) :
   m_ymax( ymax ),
   m_normal( false )
{
   m_generator = new FastMersenneTwister( seed ? seed : RandomSeed32() );
   if ( m_ymax < 0 || 1 + m_ymax == 1 )
      m_ymax = 1;
   m_rmax = m_ymax/double( uint32_max );
}

// ----------------------------------------------------------------------------

RandomNumberGenerator::~RandomNumberGenerator()
{
}

// ----------------------------------------------------------------------------

uint32 RandomNumberGenerator::Rand32()
{
   return (*m_generator)();
}

// ----------------------------------------------------------------------------

double RandomNumberGenerator::Normal( double mean, double sigma )
{
   /*
    * Box-Muller transform, polar form:
    * http://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
    */
   if ( m_normal )
   {
      m_normal = false;
      return mean + m_vs*sigma;
   }

   m_normal = true;

   double s, u, v;
   do
   {
      u = 2*Rand1() - 1;
      v = 2*Rand1() - 1;
      s = u*u + v*v;
   }
   while ( s >= 1 || s == 0 );
   s = Sqrt( (-2*Ln( s ))/s );
   m_vs = v*s;
   return mean + u*s*sigma;
}

// ----------------------------------------------------------------------------

static double LnGamma( double x )
{
   /*
    * Adapted from Numerical Recipes in C 3rd Ed. p. 257
    */
   static const double cof[]=
   { 57.1562356658629235,     -59.5979603554754912,
     14.1360979747417471,      -0.491913816097620199,     0.339946499848118887e-4,
      0.465236289270485756e-4, -0.983744753048795646e-4,  0.158088703224912494e-3,
     -0.210264441724104883e-3,  0.217439618115212643e-3, -0.164318106536763890e-3,
      0.844182239838527433e-4, -0.261908384015814087e-4,  0.368991826595316234e-5 };

   double y = x;
   double tmp = x + 5.24218750000000000;
   tmp = (x + 0.5)*Ln( tmp ) - tmp;
   double ser = 0.999999999999997092;
   for ( size_type j = 0; j < ItemsInArray( cof ); ++j )
      ser += cof[j]/++y;
   return tmp + Ln( 2.5066282746310005*ser/x );
}

// ----------------------------------------------------------------------------

int RandomNumberGenerator::Poisson( double lambda )
{
   if ( !m_lambda )
      m_lambda = DVector( 0.0, 5 );

   if ( lambda < 12 )
   {
      /*
       * Use Knuth's algorithm.
       */
      if ( m_lambda[0] != lambda )
      {
         m_lambda[0] = lambda;
         m_lambda[4] = Exp( -lambda );
      }
      int k = -1;
      for ( double p = 1; ; ++k )
         if ( (p *= Rand1()) <= m_lambda[4] )
            return k;
   }

   /*
    * Use rejection sampling with a Lorentzian envelope.
    */
   if ( m_lambda[0] != lambda )
   {
      m_lambda[0] = lambda;
      m_lambda[1] = Sqrt( 2*lambda );
      m_lambda[2] = Ln( lambda );
      m_lambda[3] = lambda*m_lambda[2] - LnGamma( lambda+1 );
   }

   for ( ;; )
   {
      double y;
      double k;
      do
      {
         y = Tan( Const<double>::pi()*Rand1() );
         k = m_lambda[1]*y + lambda;
      }
      while ( k < 0 );

      k = Floor( k );

      if ( Rand1() <= 0.9*(1 + y*y)*Exp( k*m_lambda[2] - LnGamma( k+1 ) - m_lambda[3] ) )
         return int( k );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Random.cpp - Released 2017-05-02T10:39:13Z
