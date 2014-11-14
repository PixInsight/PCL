// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/EndianConversions.h - Released 2014/11/14 17:16:41 UTC
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

#ifndef __PCL_EndianConversions_h
#define __PCL_EndianConversions_h

/// \file pcl/EndianConversions.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup endian_conversion_functions Endianness Conversion Functions
 */

/*!
 * Converts a 16-bit unsigned integer from big endian to little endian byte
 * storage order.
 *
 * \ingroup endian_conversion_functions
 */
inline uint16 BigToLittleEndian( uint16 x )
{
   return (x << 8) | (x >> 8);
}

/*!
 * Converts a 32-bit unsigned integer from big endian to little endian byte
 * storage order.
 *
 * \ingroup endian_conversion_functions
 */
inline uint32 BigToLittleEndian( uint32 x )
{
   return (x << 24) | ((x & 0x0000FF00ul) << 8) |
                      ((x & 0x00FF0000ul) >> 8) | (x >> 24);
}

/*!
 * A convenience function for little-to-big endian conversion. It is
 * equivalent to: BigToLittleEndian( x )
 *
 * \ingroup endian_conversion_functions
 */
template <typename T> inline T LittleToBigEndian( T x )
{
   return BigToLittleEndian( x );
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_EndianConversions_h

// ****************************************************************************
// EOF pcl/EndianConversions.h - Released 2014/11/14 17:16:41 UTC
