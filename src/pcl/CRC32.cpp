//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/CRC32.cpp - Released 2015/12/17 18:52:18 UTC
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

#include <pcl/AutoLock.h>
#include <pcl/Checksum.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * CRC-32 calculation routine based on original code by Michael Barr.
 *
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 */

#define POLYNOMIAL         0x04C11DB7
#define INITIAL_REMAINDER  0xFFFFFFFF
#define FINAL_XOR_VALUE    0xFFFFFFFF
#define CHECK_VALUE        0xCBF43926

#define TOPBIT    (1 << 31)

#define REFLECT_DATA( X )        Reflect8( (X) )
#define REFLECT_REMAINDER( X )   Reflect32( (X) )

/*
 * Reorder the bits of a binary sequence, by reflecting them about the
 * middle position.
 */
static uint32 Reflect32( uint32 data )
{
   uint32 reflection = 0x00000000;

   // Reflect the data around the center bit.
   for ( int bit = 0; ; )
   {
      // If the LSB bit is set, set the reflection of it.
      if ( data & 0x01 )
         reflection |= (1 << (31 - bit));

      data >>= 1;

      if ( ++bit == 32 )
         break;
   }

   return reflection;

}

static uint32 Reflect8( uint8 data )
{
   uint32 reflection = 0x00;

   for ( int bit = 0; ; )
   {
      if ( data & 0x01 )
         reflection |= (1 << (7 - bit));

      data >>= 1;

      if ( ++bit == 8 )
         break;
   }

   return reflection;
}

uint32 CRC32( const void* data, size_type length )
{
   PCL_PRECONDITION( data != 0 )

   static uint32 crcTable[ 256 ];
   {
      static Mutex mutex;
      static bool initialized = false;

      volatile AutoLock lock( mutex );

      if ( !initialized )
      {
         /*
          * Populate the partial CRC lookup table.
          */

         // Compute the remainder of each possible dividend.
         for ( uint32 dividend = 0; dividend < 256; ++dividend )
         {
            // Start with the dividend followed by zeros.
            uint32 remainder = dividend << (32 - 8);

            // Perform modulo-2 division, a bit at a time.
            for ( int bit = 8; ; )
            {
               // Try to divide the current data bit.
               remainder <<= 1;
               if ( remainder & TOPBIT )
                  remainder ^= POLYNOMIAL;

               if ( --bit == 0 )
                  break;
            }

            // Store the result into the table.
            crcTable[dividend] = remainder;
         }

         initialized = true;
      }
   }

   uint32 remainder = INITIAL_REMAINDER;

   // Divide the message by the polynomial, a byte at a time.
   for ( const uint8* p = reinterpret_cast<const uint8*>( data ), * p1 = p+length; p < p1; ++p )
      remainder = crcTable[REFLECT_DATA( *p ) ^ (remainder >> (32 - 8))] ^ (remainder << 8);

   //The final remainder is the CRC.
   return REFLECT_REMAINDER( remainder ) ^ FINAL_XOR_VALUE;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/CRC32.cpp - Released 2015/12/17 18:52:18 UTC
