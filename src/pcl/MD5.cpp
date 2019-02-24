//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/MD5.cpp - Released 2019-01-21T12:06:21Z
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

/*
 * This implementation is based on public domain code by Alexander Peslyak:
 *
 * http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5
 *
 * The original implementation requires inclusion of the following copyright
 * information, conditions and disclaimers:
 *
 * ============================================================================
 * Copyright (c) 2001 Alexander Peslyak
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * There's ABSOLUTELY NO WARRANTY, express or implied.
 * ============================================================================
 */

#include <pcl/Cryptography.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

typedef struct
{
   uint32 lo, hi;
   uint32 a, b, c, d;
   uint8  buffer[ 64 ];
#if !defined( __PCL_X64 ) && !defined( __PCL_X86 )
   uint32 block[ 16 ];
#endif
}
MD5_CTX;

/*
 * The basic MD5 functions.
 *
 * F and G are optimized compared to their RFC 1321 definitions for
 * architectures that lack an AND-NOT instruction, just like in Colin Plumb's
 * implementation.
 */
#define F( x, y, z )    ((z) ^ ((x) & ((y) ^ (z))))
#define G( x, y, z )    ((y) ^ ((z) & ((x) ^ (y))))
#define H( x, y, z )    (((x) ^ (y)) ^ (z))
#define H2( x, y, z )   ((x) ^ ((y) ^ (z)))
#define I( x, y, z )    ((y) ^ ((x) | ~(z)))

/*
 * The MD5 transformation for all four rounds.
 */
#define STEP( f, a, b, c, d, x, t, s ) \
   (a) += f( (b), (c), (d) ) + (x) + (t); \
   (a) = (((a) << (s)) | (((a) & 0xffffffff) >> (32 - (s)))); \
   (a) += (b);

/*
 * SET reads 4 input bytes in little-endian byte order and stores them in a
 * properly aligned word in host byte order.
 *
 * The check for little-endian architectures that tolerate unaligned memory
 * accesses is just an optimization.  Nothing will break if it fails to detect
 * a suitable architecture.
 *
 * Unfortunately, this optimization may be a C strict aliasing rules violation
 * if the caller's data buffer has effective type that cannot be aliased by
 * uint32.  In practice, this problem may occur if these MD5 routines are
 * inlined into a calling function, or with future and dangerously advanced
 * link-time optimizations.  For the time being, keeping these MD5 routines in
 * their own translation unit avoids the problem.
 */
#if defined( __PCL_X64 ) || defined( __PCL_X86 )
# define SET( n ) \
   reinterpret_cast<const uint32*>( ptr )[n]
# define GET( n ) \
   SET( n )
#else
# define SET( n ) \
   (ctx->block[n] = \
       uint32( ptr[n*4]     )        | \
      (uint32( ptr[n*4 + 1] ) <<  8) | \
      (uint32( ptr[n*4 + 2] ) << 16) | \
      (uint32( ptr[n*4 + 3] ) << 24))
# define GET( n ) \
   (ctx->block[n])
#endif

/*
 * This processes one or more 64-byte data blocks, but does NOT update the bit
 * counters.  There are no alignment requirements.
 */
static const void* body( MD5_CTX* ctx, const void* data, unsigned long size )
{
   const uint8* ptr = reinterpret_cast<const uint8*>( data );

   uint32 a = ctx->a;
   uint32 b = ctx->b;
   uint32 c = ctx->c;
   uint32 d = ctx->d;

   for ( ; size > 0; size -= 64 )
   {
      uint32 saved_a = a;
      uint32 saved_b = b;
      uint32 saved_c = c;
      uint32 saved_d = d;

      // Round 1
      STEP( F,  a, b, c, d, SET(  0 ), 0xd76aa478,  7 )
      STEP( F,  d, a, b, c, SET(  1 ), 0xe8c7b756, 12 )
      STEP( F,  c, d, a, b, SET(  2 ), 0x242070db, 17 )
      STEP( F,  b, c, d, a, SET(  3 ), 0xc1bdceee, 22 )
      STEP( F,  a, b, c, d, SET(  4 ), 0xf57c0faf,  7 )
      STEP( F,  d, a, b, c, SET(  5 ), 0x4787c62a, 12 )
      STEP( F,  c, d, a, b, SET(  6 ), 0xa8304613, 17 )
      STEP( F,  b, c, d, a, SET(  7 ), 0xfd469501, 22 )
      STEP( F,  a, b, c, d, SET(  8 ), 0x698098d8,  7 )
      STEP( F,  d, a, b, c, SET(  9 ), 0x8b44f7af, 12 )
      STEP( F,  c, d, a, b, SET( 10 ), 0xffff5bb1, 17 )
      STEP( F,  b, c, d, a, SET( 11 ), 0x895cd7be, 22 )
      STEP( F,  a, b, c, d, SET( 12 ), 0x6b901122,  7 )
      STEP( F,  d, a, b, c, SET( 13 ), 0xfd987193, 12 )
      STEP( F,  c, d, a, b, SET( 14 ), 0xa679438e, 17 )
      STEP( F,  b, c, d, a, SET( 15 ), 0x49b40821, 22 )

      // Round 2
      STEP( G,  a, b, c, d, GET(  1 ), 0xf61e2562,  5 )
      STEP( G,  d, a, b, c, GET(  6 ), 0xc040b340,  9 )
      STEP( G,  c, d, a, b, GET( 11 ), 0x265e5a51, 14 )
      STEP( G,  b, c, d, a, GET(  0 ), 0xe9b6c7aa, 20 )
      STEP( G,  a, b, c, d, GET(  5 ), 0xd62f105d,  5 )
      STEP( G,  d, a, b, c, GET( 10 ), 0x02441453,  9 )
      STEP( G,  c, d, a, b, GET( 15 ), 0xd8a1e681, 14 )
      STEP( G,  b, c, d, a, GET(  4 ), 0xe7d3fbc8, 20 )
      STEP( G,  a, b, c, d, GET(  9 ), 0x21e1cde6,  5 )
      STEP( G,  d, a, b, c, GET( 14 ), 0xc33707d6,  9 )
      STEP( G,  c, d, a, b, GET(  3 ), 0xf4d50d87, 14 )
      STEP( G,  b, c, d, a, GET(  8 ), 0x455a14ed, 20 )
      STEP( G,  a, b, c, d, GET( 13 ), 0xa9e3e905,  5 )
      STEP( G,  d, a, b, c, GET(  2 ), 0xfcefa3f8,  9 )
      STEP( G,  c, d, a, b, GET(  7 ), 0x676f02d9, 14 )
      STEP( G,  b, c, d, a, GET( 12 ), 0x8d2a4c8a, 20 )

      // Round 3
      STEP( H,  a, b, c, d, GET(  5 ), 0xfffa3942,  4 )
      STEP( H2, d, a, b, c, GET(  8 ), 0x8771f681, 11 )
      STEP( H,  c, d, a, b, GET( 11 ), 0x6d9d6122, 16 )
      STEP( H2, b, c, d, a, GET( 14 ), 0xfde5380c, 23 )
      STEP( H,  a, b, c, d, GET(  1 ), 0xa4beea44,  4 )
      STEP( H2, d, a, b, c, GET(  4 ), 0x4bdecfa9, 11 )
      STEP( H,  c, d, a, b, GET(  7 ), 0xf6bb4b60, 16 )
      STEP( H2, b, c, d, a, GET( 10 ), 0xbebfbc70, 23 )
      STEP( H,  a, b, c, d, GET( 13 ), 0x289b7ec6,  4 )
      STEP( H2, d, a, b, c, GET(  0 ), 0xeaa127fa, 11 )
      STEP( H,  c, d, a, b, GET(  3 ), 0xd4ef3085, 16 )
      STEP( H2, b, c, d, a, GET(  6 ), 0x04881d05, 23 )
      STEP( H,  a, b, c, d, GET(  9 ), 0xd9d4d039,  4 )
      STEP( H2, d, a, b, c, GET( 12 ), 0xe6db99e5, 11 )
      STEP( H,  c, d, a, b, GET( 15 ), 0x1fa27cf8, 16 )
      STEP( H2, b, c, d, a, GET(  2 ), 0xc4ac5665, 23 )

      // Round 4
      STEP( I,  a, b, c, d, GET(  0 ), 0xf4292244,  6 )
      STEP( I,  d, a, b, c, GET(  7 ), 0x432aff97, 10 )
      STEP( I,  c, d, a, b, GET( 14 ), 0xab9423a7, 15 )
      STEP( I,  b, c, d, a, GET(  5 ), 0xfc93a039, 21 )
      STEP( I,  a, b, c, d, GET( 12 ), 0x655b59c3,  6 )
      STEP( I,  d, a, b, c, GET(  3 ), 0x8f0ccc92, 10 )
      STEP( I,  c, d, a, b, GET( 10 ), 0xffeff47d, 15 )
      STEP( I,  b, c, d, a, GET(  1 ), 0x85845dd1, 21 )
      STEP( I,  a, b, c, d, GET(  8 ), 0x6fa87e4f,  6 )
      STEP( I,  d, a, b, c, GET( 15 ), 0xfe2ce6e0, 10 )
      STEP( I,  c, d, a, b, GET(  6 ), 0xa3014314, 15 )
      STEP( I,  b, c, d, a, GET( 13 ), 0x4e0811a1, 21 )
      STEP( I,  a, b, c, d, GET(  4 ), 0xf7537e82,  6 )
      STEP( I,  d, a, b, c, GET( 11 ), 0xbd3af235, 10 )
      STEP( I,  c, d, a, b, GET(  2 ), 0x2ad7d2bb, 15 )
      STEP( I,  b, c, d, a, GET(  9 ), 0xeb86d391, 21 )

      a += saved_a;
      b += saved_b;
      c += saved_c;
      d += saved_d;

      ptr += 64;
   }

   ctx->a = a;
   ctx->b = b;
   ctx->c = c;
   ctx->d = d;

   return ptr;
}

static void MD5_Init( MD5_CTX* ctx )
{
   ctx->a = 0x67452301;
   ctx->b = 0xefcdab89;
   ctx->c = 0x98badcfe;
   ctx->d = 0x10325476;

   ctx->lo = 0;
   ctx->hi = 0;
}

static void MD5_Update( MD5_CTX* ctx, const void* data, unsigned long size )
{
   uint32 saved_lo;
   unsigned long used, available;

   saved_lo = ctx->lo;
   if ( (ctx->lo = (saved_lo + size) & 0x1fffffff) < saved_lo )
      ctx->hi++;
   ctx->hi += size >> 29;

   used = saved_lo & 0x3f;

   if ( used )
   {
      available = 64 - used;

      if (size < available)
      {
         ::memcpy(&ctx->buffer[used], data, size);
         return;
      }

      ::memcpy( &ctx->buffer[used], data, available );
      data = (const uint8*)data + available;
      size -= available;
      body( ctx, ctx->buffer, 64 );
   }

   if ( size >= 64 )
   {
      data = body( ctx, data, size & ~(unsigned long)0x3f );
      size &= 0x3f;
   }

   ::memcpy( ctx->buffer, data, size );
}

#ifdef OUT // OUT is already #defined somewhere on MSVC 2015
#undef OUT
#endif
#define OUT( dst, src ) \
   (dst)[0] = (unsigned char)(src); \
   (dst)[1] = (unsigned char)((src) >> 8); \
   (dst)[2] = (unsigned char)((src) >> 16); \
   (dst)[3] = (unsigned char)((src) >> 24);

static void MD5_Final( uint8* result, MD5_CTX* ctx )
{
   unsigned long used, available;

   used = ctx->lo & 0x3f;

   ctx->buffer[used++] = 0x80;

   available = 64 - used;

   if ( available < 8 )
   {
      memset( &ctx->buffer[used], 0, available );
      body( ctx, ctx->buffer, 64 );
      used = 0;
      available = 64;
   }

   memset( &ctx->buffer[used], 0, available - 8 );

   ctx->lo <<= 3;
   OUT( &ctx->buffer[56], ctx->lo )
   OUT( &ctx->buffer[60], ctx->hi )

   body( ctx, ctx->buffer, 64 );

   OUT( &result[0], ctx->a )
   OUT( &result[4], ctx->b )
   OUT( &result[8], ctx->c )
   OUT( &result[12], ctx->d )

   memset( ctx, 0, sizeof(*ctx) );
}

// ----------------------------------------------------------------------------

#define CTX reinterpret_cast<MD5_CTX*>( m_context )

MD5::~MD5()
{
   if ( m_context != nullptr )
      delete CTX, m_context = nullptr;
}

void MD5::Initialize()
{
   if ( m_context == nullptr )
      m_context = new MD5_CTX;
   MD5_Init( CTX );
}

void MD5::Update( const void* data, size_type size )
{
   if ( data != nullptr )
      if ( size > 0 )
      {
         if ( m_context == nullptr )
         {
            m_context = new MD5_CTX;
            MD5_Init( CTX );
         }
         MD5_Update( CTX, data, (unsigned long)size );
      }
}

void MD5::Finalize( void* hash )
{
   if ( m_context == nullptr )
      throw Error( "MD5::Finalize(): Invalid call on uninitialized object." );
   MD5_Final( reinterpret_cast<uint8*>( hash ), CTX );
   MD5_Init( CTX );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/MD5.cpp - Released 2019-01-21T12:06:21Z
