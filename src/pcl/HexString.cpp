//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/HexString.cpp - Released 2018-11-01T11:06:52Z
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

#include <pcl/String.h>
#include <pcl/Exception.h>

namespace pcl
{

// ----------------------------------------------------------------------------

IsoString IsoString::ToHex( const void* data, size_type length )
{
   IsoString hex;
   hex.SetLength( length << 1 );
   iterator h = hex.Begin();
   for ( const uint8* p = reinterpret_cast<const uint8*>( data ); length > 0; ++p, --length )
   {
      // MSN
      int n = (*p >> 4) & 0x0f;
      if ( n < 10 )
         *h = char( n + '0' );
      else
         *h = char( n - 10 + 'a' );
      ++h;

      // LSN
      n = *p & 0x0f;
      if ( n < 10 )
         *h = char( n + '0' );
      else
         *h = char( n - 10 + 'a' );
      ++h;
   }
   return hex;
}

// ----------------------------------------------------------------------------

inline static
int HexDigitValue( char c )
{
   switch ( c )
   {
   case '0':
   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
   case '8':
   case '9':
      return c - '0';
   case 'a':
   case 'b':
   case 'c':
   case 'd':
   case 'e':
   case 'f':
      return 10 + c - 'a';
   case 'A':
   case 'B':
   case 'C':
   case 'D':
   case 'E':
   case 'F':
      return 10 + c - 'A';
   default:
      throw ParseError( String().Format( "Invalid hexadecimal digit %%%02x", int( c ) ) );
   }
}

ByteArray IsoString::FromHex() const
{
   size_type len = Length();
   if ( len & 1 )
      throw ParseError( "Invalid hex-encoded string length" );

   ByteArray B( len >> 1 );
   ByteArray::iterator b = B.Begin();
   for ( const_iterator i = m_data->string; i < m_data->end; )
   {
      int msb = HexDigitValue( *i++ ) << 4;
      int lsb = HexDigitValue( *i++ );
      *b++ = uint8( msb | lsb );
   }
   return B;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/HexString.cpp - Released 2018-11-01T11:06:52Z
