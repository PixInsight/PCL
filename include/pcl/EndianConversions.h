//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// pcl/EndianConversions.h - Released 2018-11-23T16:14:19Z
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

#ifndef __PCL_EndianConversions_h
#define __PCL_EndianConversions_h

/// \file pcl/EndianConversions.h

#include <pcl/Defs.h>

#include <pcl/Math.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup endianness_conversion_and_detection Endianness Conversion and Detection Functions
 */

/*!
 * Converts a 16-bit unsigned integer from big endian to little endian byte
 * storage order.
 *
 * \ingroup endianness_conversion_and_detection
 */
inline uint16 BigToLittleEndian( uint16 x )
{
   return (x << 8) | (x >> 8);
}

/*!
 * Converts a 32-bit unsigned integer from big endian to little endian byte
 * storage order.
 *
 * \ingroup endianness_conversion_and_detection
 */
inline uint32 BigToLittleEndian( uint32 x )
{
   return (RotL( x, 8 ) & 0x00ff00ffu) | (RotR( x, 8 ) & 0xff00ff00u);

}

/*!
 * Converts a 64-bit unsigned integer from big endian to little endian byte
 * storage order.
 *
 * \ingroup endianness_conversion_and_detection
 */
inline uint64 BigToLittleEndian( uint64 x )
{
   return (uint64( BigToLittleEndian( uint32( x ) ) ) << 32) | uint64( BigToLittleEndian( uint32( x >> 32 ) ) );
}

/*!
 * A convenience synonym function for little-to-big endian conversions, which
 * we define for the sake of code legibility. It is obviously equivalent to
 * BigToLittleEndian( x ).
 *
 * \ingroup endianness_conversion_and_detection
 */
template <typename T> inline T LittleToBigEndian( T x )
{
   return BigToLittleEndian( x );
}

// ----------------------------------------------------------------------------

union __pcl_endian_check__ { uint32 w; uint8 b[ sizeof( uint32 ) ]; };

constexpr __pcl_endian_check__ __pcl_endian_check_sample__ = { 0xdeadbeef };

#ifdef __clang__

inline bool IsLittleEndianMachine()
{
   return __pcl_endian_check_sample__.b[0] == 0xef;
}

inline bool IsBigEndianMachine()
{
   return __pcl_endian_check_sample__.b[0] == 0xde;
}

#else

// Clang fails here with "read of member 'b' of union with active member 'w' is
// not allowed in a constant expression".

/*!
 * Returns true iff the caller is running on a little-endian architecture.
 *
 * \ingroup endianness_conversion_and_detection
 */
constexpr bool IsLittleEndianMachine()
{
   return __pcl_endian_check_sample__.b[0] == 0xef;
}

/*!
 * Returns true iff the caller is running on a big-endian architecture.
 *
 * \ingroup endianness_conversion_and_detection
 */
constexpr bool IsBigEndianMachine()
{
   return __pcl_endian_check_sample__.b[0] == 0xde;
}

#endif // __clang__

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_EndianConversions_h

// ----------------------------------------------------------------------------
// EOF pcl/EndianConversions.h - Released 2018-11-23T16:14:19Z
