//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/Checksum.h - Released 2017-08-01T14:23:31Z
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

#ifndef __PCL_Checksum_h
#define __PCL_Checksum_h

/// \file pcl/Checksum.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/ByteArray.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup checksum_functions Checksum Calculation Functions
 */

/*!
 * Returns the <em>standard checksum</em> value calculated for a data sequence.
 * The returned value is the total number of bits set in the sequence of input
 * data bytes.
 *
 * \param data    Address of the first byte in the data sequence.
 * \param length  Length in bytes of the data sequence.
 *
 * \ingroup checksum_functions
 * \sa Checksum( const C& )
 */
size_type PCL_FUNC inline Checksum( const void* data, size_type length )
{
   PCL_PRECONDITION( data != nullptr )
   size_type S = 0;
   for ( const uint8* p = (const uint8*)data, * p1 = p+length; p < p1; ++p )
   {
      uint8 b = *p;
      if ( b & 0x01 ) ++S;
      if ( b & 0x02 ) ++S;
      if ( b & 0x04 ) ++S;
      if ( b & 0x08 ) ++S;
      if ( b & 0x10 ) ++S;
      if ( b & 0x20 ) ++S;
      if ( b & 0x40 ) ++S;
      if ( b & 0x80 ) ++S;
   }
   return S;
}

/*!
 * Returns the <em>standard checksum</em> value for a container.
 *
 * \param data    Reference to a container whose standard checksum will be
 *                calculated. The checksum number will be generated for the
 *                current data bytes in this container instance.
 *
 * \ingroup checksum_functions
 * \sa Checksum( const void*, size_type )
 */
template <class C> inline
uint32 Checksum( const C& data )
{
   return Checksum( data.Begin(), sizeof( *(data.Begin()) )*data.Length() );
}

/*!
 * Returns the CRC-32 error-detecting code calculated for a data sequence.
 *
 * \param data    Address of the first byte in the data sequence.
 * \param length  Length in bytes of the data sequence.
 *
 * \b References
 *
 * CRC calculation routine based on original code by Michael Barr. The employed
 * code requires the following copyright notice:
 *
 * <em>Copyright (c) 2000 by Michael Barr.  This software is placed into the
 * public domain and may be used for any purpose.  However, this notice must
 * not be changed or removed and no warranty is either expressed or implied by
 * its publication or distribution.</em>
 *
 * \ingroup checksum_functions
 * \sa CRC32( const C& )
 */
uint32 PCL_FUNC CRC32( const void* data, size_type length );

/*!
 * Returns the CRC-32 error-detecting code for a container.
 *
 * \param data    Reference to a container whose CRC-32 checksum will be
 *                calculated. The checksum code will be generated for the
 *                current data bytes in this container instance.
 *
 * \ingroup checksum_functions
 * \sa CRC32( const void*, size_type )
 */
template <class C> inline
uint32 CRC32( const C& data )
{
   return CRC32( data.Begin(), sizeof( *(data.Begin()) )*data.Length() );
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Checksum_h

// ----------------------------------------------------------------------------
// EOF pcl/Checksum.h - Released 2017-08-01T14:23:31Z
