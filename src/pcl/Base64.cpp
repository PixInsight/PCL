//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0837
// ----------------------------------------------------------------------------
// pcl/Base64.cpp - Released 2017-06-09T08:12:54Z
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

#include <pcl/String.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

IsoString IsoString::ToBase64( const void* data, size_type length )
{
   static const char map[ 64 ] =
   {
      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
      'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
      'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
      'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
      'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
      'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
      '8', '9', '+', '/'
   };

   if ( length == 0 || data == 0 )
      return IsoString();

   size_type n = (length << 3)/6;

   switch( ( length << 3 ) - ( n * 6 ) )
   {
   case 2: n += 3; break;
   case 4: n += 2; break;
   default: break;
   }

   IsoString base64;
   base64.Reserve( n );

   char* p = base64.Begin();
   const uint8* d = reinterpret_cast<const uint8*>( data );
   size_type i = 0;

   for ( size_type n = (length/3)*3; i < n; i += 3 )
   {
      uint32 C1 = *d++;
      uint32 C2 = *d++;
      uint32 C3 = *d++;
      *p++ = map[( C1 >> 2 ) & 0x3F];
      *p++ = map[((( C1 &  3 ) << 4) + ( C2 >> 4 )) & 0x3F];
      *p++ = map[((( C2 & 15 ) << 2) + ( C3 >> 6 )) & 0x3F];
      *p++ = map[C3 & 0x3F];
   }

   if ( i < length )
   {
      uint32 C1 = *d++;
      uint32 C2 = ((i + 1) < length) ? *d++ : 0;
      *p++ = map[( C1 >> 2 ) & 0x3F];
      *p++ = map[((( C1 & 3 ) << 4) + ( C2 >> 4 )) & 0x3F];
      *p++ = ((i + 1) < length) ? map[((( C2 & 15 ) << 2)) & 0x3F] : '=';
      *p++ = '=';
   }

   base64.SetLength( p - base64.Begin() );
   return base64;
}

// ----------------------------------------------------------------------------

ByteArray IsoString::FromBase64() const
{
   static const uint8 map[ 128 ] =
   {
      127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
      127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
      127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
      127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
      127, 127, 127,  62, 127, 127, 127,  63,  52,  53,
       54,  55,  56,  57,  58,  59,  60,  61, 127, 127,
      127,  64, 127, 127, 127,   0,   1,   2,   3,   4,
        5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
       15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
       25, 127, 127, 127, 127, 127, 127,  26,  27,  28,
       29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
       39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
       49,  50,  51, 127, 127, 127, 127, 127
   };

   size_type length = Length();

   const uint8* d = reinterpret_cast<const uint8*>( Begin() );

   size_type i = 0, n = 0;
   for ( int j = 0; i < length; ++i )
   {
      if ( d[i] == '\n' || (length - i) >= 2 && d[i] == '\r' && d[i+1] == '\n' )
         continue;
      if ( d[i] == '=' && ++j > 2 || d[i] > 127 || map[d[i]] == 127 || map[d[i]] < 64 && j != 0 )
         throw ParseError( "Invalid Base64 character", *this, int( i ) );
      ++n;
   }

   if ( n == 0 )
      return ByteArray();

   ByteArray B( (n*6 + 7) >> 3 );
   ByteArray::iterator p = B.Begin();

   uint32 x = 0;
   for ( int j = 3, n = 0; i > 0; --i, ++d )
   {
      if ( *d == '\r' || *d == '\n' )
         continue;

      j -= (map[*d] == 64);
      x  = (x << 6) | uint32( map[*d] & 0x3F );

      if ( ++n == 4 )
      {
         n = 0;
         *p++ = uint8( x >> 16 );
         if ( j > 1 ) *p++ = uint8( x >> 8 );
         if ( j > 2 ) *p++ = uint8( x );
      }
   }

   B.Remove( p, B.End() );
   return B;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Base64.cpp - Released 2017-06-09T08:12:54Z
