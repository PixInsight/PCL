//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/CharTraits.cpp - Released 2015/12/17 18:52:18 UTC
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

#include <pcl/CharTraits.h>

#ifndef __PCL_WINDOWS
#  include <strings.h>
#  include <locale.h>
#  include <pcl/Array.h>
#endif

#ifdef __PCL_MACOSX
#  include <CoreFoundation/CoreFoundation.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

static const uint8 s_toLowercaseLatin1[ 256 ] =
{
     0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
    16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
    32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
    48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
    64,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
   112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,  91,  92,  93,  94,  95,
    96,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
   112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
   128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
   144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
   160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
   176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
   224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
   240, 241, 242, 243, 244, 245, 246, 215, 248, 249, 250, 251, 252, 253, 254, 223,
   224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
   240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255
};

static const uint8 s_toUppercaseLatin1[ 256 ] =
{
     0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
    16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
    32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
    48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
    64,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
    80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
    96,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
    80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90, 123, 124, 125, 126, 127,
   128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
   144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
   160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
   176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,
   192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
   208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
   192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
   208, 209, 210, 211, 212, 213, 214, 247, 216, 217, 218, 219, 220, 221, 222, 255
};

const uint8* PCL_DATA PCL_toLowercaseLatin1 = s_toLowercaseLatin1;
const uint8* PCL_DATA PCL_toUppercaseLatin1 = s_toUppercaseLatin1;

// ----------------------------------------------------------------------------

int IsoCharTraits::Compare( const char* s1, size_type n1,
                            const char* s2, size_type n2, bool caseSensitive, bool localeAware )
{
   if ( n1 == 0 || s1 == 0 || *s1 == '\0' )
      return (n2 == 0 || s2 == 0 || *s2 == '\0') ? 0 : -1;
   if ( n2 == 0 || s2 == 0 || *s2 == '\0' )
      return +1;
#ifdef __PCL_WINDOWS
   return ::CompareStringA(
      localeAware ? LOCALE_USER_DEFAULT : LOCALE_INVARIANT, // locale identifier
      caseSensitive ? 0 : NORM_IGNORECASE, // comparison-style options
      s1,                  // pointer to first string
      int( n1 ),           // size, in bytes or characters, of first string
      s2,                  // pointer to second string
      int( n2 )            // size, in bytes or characters, of second string
   ) - 2;
#endif
#ifdef __PCL_MACOSX
   int n = int( pcl::Min( n1, n2 ) );
   CFStringRef sr1 = CFStringCreateWithBytesNoCopy( kCFAllocatorDefault,
                        reinterpret_cast<const UInt8*>( s1 ), n,
                        kCFStringEncodingISOLatin1, false, kCFAllocatorNull );
   CFStringRef sr2 = CFStringCreateWithBytesNoCopy( kCFAllocatorDefault,
                        reinterpret_cast<const UInt8*>( s2 ), n,
                        kCFStringEncodingISOLatin1, false, kCFAllocatorNull );
   int flags = 0;
   if ( !caseSensitive )
      flags |= kCFCompareCaseInsensitive;
   if ( localeAware )
      flags |= kCFCompareLocalized;
   int r = CFStringCompare( sr1, sr2, flags );
   CFRelease( sr1 );
   CFRelease( sr2 );
   return (r != 0) ? r : ((n1 == n2) ? 0 : ((n1 < n2) ? -1 : +1));
#endif
#ifdef __PCL_X11
   if ( localeAware )
   {
      const char* l = ::setlocale( LC_COLLATE, 0 );
      if ( *l == 'C' && l[1] == '\0' )
         localeAware = false;
   }
   if ( !localeAware )
   {
      if ( caseSensitive )
      {
         for ( size_type n = pcl::Min( n1, n2 ); n > 0; --n, ++s1, ++s2 )
            if ( *s1 != *s2 )
               return (*s1 < *s2) ? -1 : +1;
      }
      else
      {
         for ( size_type n = pcl::Min( n1, n2 ); n > 0; --n, ++s1, ++s2 )
         {
            char c1 = ToCaseFolded( *s1 ), c2 = ToCaseFolded( *s2 );
            if ( c1 != c2 )
               return (c1 < c2) ? -1 : +1;
         }
      }
      return (n1 == n2) ? 0 : ((n1 < n2) ? -1 : +1);
   }

   /*
    * Workaround to the lack of a strncoll() POSIX function.
    */
   int r;
   size_type l1 = ::strlen( s1 );
   if ( n1 > l1 )
      n1 = l1;
   size_type l2 = ::strlen( s2 );
   if ( n2 > l2 )
      n2 = l2;
   size_type n = pcl::Min( n1, n2 );
   if ( caseSensitive && n == Min( l1, l2 ) )
   {
      r = ::strcoll( s1, s2 );
   }
   else
   {
      Array<char> t1( n+1 ), t2( n+1 );
      ::strncpy( t1.Begin(), s1, n ); t1[n] = '\0';
      ::strncpy( t2.Begin(), s2, n ); t2[n] = '\0';
      if ( !caseSensitive )
      {
         ToLowercase( t1.Begin(), n );
         ToLowercase( t2.Begin(), n );
      }
      r = ::strcoll( t1.Begin(), t2.Begin() );
   }
   return (r != 0) ? r : ((n1 == n2) ? 0 : ((n1 < n2) ? -1 : +1));
#endif
}

// ----------------------------------------------------------------------------

int CharTraits::Compare( const char16_type* s1, size_type n1,
                         const char16_type* s2, size_type n2, bool caseSensitive, bool localeAware )
{
   if ( n1 == 0 || s1 == 0 || *s1 == char16_type( 0 ) )
      return (n2 == 0 || s2 == 0 || *s2 == 0) ? 0 : -1;
   if ( n2 == 0 || s2 == 0 || *s2 == char16_type( 0 ) )
      return +1;
#ifdef __PCL_WINDOWS
   return ::CompareStringW(
      localeAware ? LOCALE_USER_DEFAULT : LOCALE_INVARIANT, // locale identifier
      caseSensitive ? 0 : NORM_IGNORECASE, // comparison-style options
      (LPCWSTR)s1,         // pointer to first string
      int( n1 ),           // size, in bytes or characters, of first string
      (LPCWSTR)s2,         // pointer to second string
      int( n2 )            // size, in bytes or characters, of second string
   ) - 2;
#endif
#ifdef __PCL_MACOSX
   int n = int( pcl::Min( n1, n2 ) );
   CFStringRef sr1 = CFStringCreateWithCharactersNoCopy( kCFAllocatorDefault,
                        reinterpret_cast<const UniChar*>( s1 ), n, kCFAllocatorNull );
   CFStringRef sr2 = CFStringCreateWithCharactersNoCopy( kCFAllocatorDefault,
                        reinterpret_cast<const UniChar*>( s2 ), n, kCFAllocatorNull );
   int flags = 0;
   if ( !caseSensitive )
      flags |= kCFCompareCaseInsensitive;
   if ( localeAware )
      flags |= kCFCompareLocalized;
   int r = CFStringCompare( sr1, sr2, flags );
   CFRelease( sr1 );
   CFRelease( sr2 );
   return (r != 0) ? r : ((n1 == n2) ? 0 : ((n1 < n2) ? -1 : +1));
#endif
#ifdef __PCL_X11
   if ( localeAware )
   {
      const char* l = ::setlocale( LC_COLLATE, 0 );
      if ( *l == 'C' && l[1] == '\0' )
         localeAware = false;
   }
   if ( !localeAware )
   {
      if ( caseSensitive )
      {
         for ( size_type n = pcl::Min( n1, n2 ); n > 0; --n, ++s1, ++s2 )
            if ( *s1 != *s2 )
               return (*s1 < *s2) ? -1 : +1;
      }
      else
      {
         for ( size_type n = pcl::Min( n1, n2 ); n > 0; --n, ++s1, ++s2 )
         {
            char16_type c1 = ToCaseFolded( *s1 ), c2 = ToCaseFolded( *s2 );
            if ( c1 != c2 )
               return (c1 < c2) ? -1 : +1;
         }
      }
      return (n1 == n2) ? 0 : ((n1 < n2) ? -1 : +1);
   }

   size_type n = pcl::Min( pcl::Min( pcl::Min( n1, n2 ), Length( s1 ) ), Length( s2 ) );
   Array<wchar_t> w1( n+1 ), w2( n+1 );
   wchar_t* p1 = w1.Begin();
   wchar_t* p2 = w2.Begin();
   if ( caseSensitive )
      for ( size_type i = 0; i < n; ++i, ++p1, ++p2, ++s1, ++s2 )
      {
         *p1 = wchar_t( *s1 );
         *p2 = wchar_t( *s2 );
      }
   else
      for ( size_type i = 0; i < n; ++i, ++p1, ++p2, ++s1, ++s2 )
      {
         *p1 = wchar_t( ToCaseFolded( *s1 ) );
         *p2 = wchar_t( ToCaseFolded( *s2 ) );
      }
   w1[n] = w2[n] = wchar_t( 0 );
   int r = ::wcscoll( w1.Begin(), w2.Begin() );
   return (r != 0) ? r : ((n1 == n2) ? 0 : ((n1 < n2) ? -1 : +1));
#endif
}

// ----------------------------------------------------------------------------

} //pcl

// ----------------------------------------------------------------------------
// EOF pcl/CharTraits.cpp - Released 2015/12/17 18:52:18 UTC
