// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/CharTraits.cpp - Released 2014/11/14 17:17:01 UTC
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

int IsoCharTraits::Compare( const char* s1, size_type n1,
                            const char* s2, size_type n2, bool caseSensitive, bool localeAware )
{
   if ( n1 == 0 || s1 == 0 || *s1 == '\0' )
      return (n2 == 0 || s2 == 0 || *s2 == '\0') ? 0 : -1;
   if ( n2 == 0 || s2 == 0 || *s2 == '\0' )
      return (n1 == 0 || s1 == 0 || *s1 == '\0') ? 0 : +1;
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
   int r;
   if ( localeAware )
   {
      // Workaround to the lack of a strncoll() POSIX function.
      size_type l1 = ::strlen( s1 );
      if ( n1 > l1 ) n1 = l1;
      size_type l2 = ::strlen( s2 );
      if ( n2 > l2 ) n2 = l2;
      size_type n = pcl::Min( n1, n2 );
      if ( caseSensitive && n == Min( l1, l2 ) )
         r = ::strcoll( s1, s2 );
      else
      {
         Array<char> t1( n+1 ), t2( n+1 );
         ::strncpy( t1.Begin(), s1, n ); t1[n] = '\0';
         ::strncpy( t2.Begin(), s2, n ); t2[n] = '\0';
         if ( !caseSensitive )
         {
            ToLowerCase( t1.Begin(), n );
            ToLowerCase( t2.Begin(), n );
         }
         r = ::strcoll( t1.Begin(), t2.Begin() );
      }
   }
   else
   {
      if ( caseSensitive )
         r = ::strncmp( s1, s2, pcl::Min( n1, n2 ) );
      else
         r = ::strncasecmp( s1, s2, pcl::Min( n1, n2 ) );
   }
   return (r != 0) ? r : ((n1 == n2) ? 0 : ((n1 < n2) ? -1 : +1));
#endif
}

// ----------------------------------------------------------------------------

int CharTraits::Compare( const char16_type* s1, size_type n1,
                         const char16_type* s2, size_type n2, bool caseSensitive, bool localeAware )
{
   if ( n1 == 0 || s1 == 0 || *s1 == 0 )
      return (n2 == 0 || s2 == 0 || *s2 == 0) ? 0 : -1;
   if ( n2 == 0 || s2 == 0 || *s2 == 0 )
      return (n1 == 0 || s1 == 0 || *s1 == 0) ? 0 : +1;
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
      return traits_base::Compare( s1, n1, s2, n2, caseSensitive, false );

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
         if ( ::iswupper( *p1 = wchar_t( *s1 ) ) )
            *p1 = char16_type( ::towlower( *p1 ) );
         if ( ::iswupper( *p2 = wchar_t( *s2 ) ) )
            *p2 = char16_type( ::towlower( *p2 ) );
      }
   w1[n] = w2[n] = wchar_t( 0 );
   int r = ::wcscoll( w1.Begin(), w2.Begin() );
   return (r != 0) ? r : ((n1 == n2) ? 0 : ((n1 < n2) ? -1 : +1));
#endif
}

// ----------------------------------------------------------------------------

} //pcl

// ****************************************************************************
// EOF pcl/CharTraits.cpp - Released 2014/11/14 17:17:01 UTC
