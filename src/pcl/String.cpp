//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// pcl/String.cpp - Released 2016/02/21 20:22:19 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/Array.h>
#include <pcl/AutoLock.h>
#include <pcl/Exception.h>
#include <pcl/Random.h>
#include <pcl/String.h>

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>

/*
 * Starting buffer length in characters for VFormat() and AppendVFormat().
 */
#ifndef __PCL_STRING_VFORMAT_BASE_SIZE
#  define __PCL_STRING_VFORMAT_BASE_SIZE size_type( 128 )
#endif

/*
 * Platform-dependent function signatures.
 */
#ifdef __PCL_WINDOWS // VC++ and MinGW
#  define VSNPRINTF ::_vsnprintf
#  define VSWPRINTF ::_vsnwprintf
#else                // GCC and Clang on UNIX/Linux
#  define VSNPRINTF ::vsnprintf
#  define VSWPRINTF ::vswprintf
#endif

/*
 * Boolean conversions can be formalized as type independent.
 */
#define PCL_TO_BOOL_BODY()                                                    \
   const_iterator p = char_traits::SearchTrimLeft( Begin(), End() );          \
   if ( p == End() )                                                          \
      throw ParseError( "Parsing Boolean expression: empty string" );         \
   switch ( char_traits::SearchTrimRight( p, End() ) - p )                    \
   {                                                                          \
   case 1:                                                                    \
      switch ( *p )                                                           \
      {                                                                       \
      case char_type( '0' ):                                                  \
      case char_type( 'F' ):                                                  \
      case char_type( 'f' ):                                                  \
         return false;                                                        \
      case char_type( '1' ):                                                  \
      case char_type( 'T' ):                                                  \
      case char_type( 't' ):                                                  \
         return true;                                                         \
      default:                                                                \
         break;                                                               \
      }                                                                       \
      break;                                                                  \
   case 4:                                                                    \
      if ( (p[0] == char_type( 't' ) || p[0] == char_type( 'T' )) &&          \
           (p[1] == char_type( 'r' ) || p[1] == char_type( 'R' )) &&          \
           (p[2] == char_type( 'u' ) || p[2] == char_type( 'U' )) &&          \
           (p[3] == char_type( 'e' ) || p[3] == char_type( 'E' )) )           \
         return true;                                                         \
      break;                                                                  \
   case 5:                                                                    \
      if ( (p[0] == char_type( 'f' ) || p[0] == char_type( 'F' )) &&          \
           (p[1] == char_type( 'a' ) || p[1] == char_type( 'A' )) &&          \
           (p[2] == char_type( 'l' ) || p[2] == char_type( 'L' )) &&          \
           (p[3] == char_type( 's' ) || p[3] == char_type( 'S' )) &&          \
           (p[4] == char_type( 'e' ) || p[4] == char_type( 'E' )) )           \
         return false;                                                        \
      break;                                                                  \
   default:                                                                   \
      break;                                                                  \
   }                                                                          \
   throw ParseError( "Parsing Boolean expression: invalid Boolean literal: " + *this )

#define PCL_TRY_TO_BOOL_BODY()                                                \
   const_iterator p = char_traits::SearchTrimLeft( Begin(), End() );          \
   if ( p == End() )                                                          \
      return false;                                                           \
   bool ok = true;                                                            \
   switch ( char_traits::SearchTrimRight( p, End() ) - p )                    \
   {                                                                          \
   case 1:                                                                    \
      switch ( *p )                                                           \
      {                                                                       \
      case char_type( '0' ):                                                  \
      case char_type( 'F' ):                                                  \
      case char_type( 'f' ):                                                  \
         value = false;                                                       \
         break;                                                               \
      case char_type( '1' ):                                                  \
      case char_type( 'T' ):                                                  \
      case char_type( 't' ):                                                  \
         value = true;                                                        \
         break;                                                               \
      default:                                                                \
         ok = false;                                                          \
         break;                                                               \
      }                                                                       \
      break;                                                                  \
   case 4:                                                                    \
      if ( (p[0] == char_type( 't' ) || p[0] == char_type( 'T' )) &&          \
           (p[1] == char_type( 'r' ) || p[1] == char_type( 'R' )) &&          \
           (p[2] == char_type( 'u' ) || p[2] == char_type( 'U' )) &&          \
           (p[3] == char_type( 'e' ) || p[3] == char_type( 'E' )) )           \
         value = true;                                                        \
      else                                                                    \
         ok = false;                                                          \
      break;                                                                  \
   case 5:                                                                    \
      if ( (p[0] == char_type( 'f' ) || p[0] == char_type( 'F' )) &&          \
           (p[1] == char_type( 'a' ) || p[1] == char_type( 'A' )) &&          \
           (p[2] == char_type( 'l' ) || p[2] == char_type( 'L' )) &&          \
           (p[3] == char_type( 's' ) || p[3] == char_type( 'S' )) &&          \
           (p[4] == char_type( 'e' ) || p[4] == char_type( 'E' )) )           \
         value = false;                                                       \
      else                                                                    \
         ok = false;                                                          \
      break;                                                                  \
   default:                                                                   \
      ok = false;                                                             \
      break;                                                                  \
   }                                                                          \
   return ok

namespace pcl
{

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

String String::UTF8ToUTF16( const char* str, size_type p, size_type n )
{
   static const char16_type s_replacementCharacter = 0xfffd;

   if ( n == 0 )
      return String();
   size_type size = IsoCharTraits::Length( str );
   if ( p >= size )
      return String();

   n = pcl::Min( n, size-p );
   str += p;

   String result;
   result.Reserve( n ); // worst case

   char16_type* s16 = result.Begin();
   uint32 c32 = 0;
   uint32 ucMin = 0;

   int moreChars = 0;
   size_type errorIndex = 0;
   size_type i = 0;

   // Skip UTF8-encoded byte order mark
   if ( n >= 3 && uint8( str[0] ) == 0xef && uint8( str[1] ) == 0xbb && uint8( str[2] ) == 0xbf )
      i += 3;

   for ( ; i < n; ++i )
   {
      uint8 c8 = uint8( str[i] );

      if ( moreChars )
      {
         if ( (c8 & 0xc0) == 0x80 )
         {
            c32 = (c32 << 6) | (c8 & 0x3f);
            if ( --moreChars == 0 )
            {
               if ( c32 > 0xffffU && c32 < 0x110000U )
               {
                  // Surrogate pair
                  *s16++ = CharTraits::HighSurrogate( c32 );
                  c32 = CharTraits::LowSurrogate( c32 );
               }
               else if ( c32 < ucMin || c32 >= 0xd800 && c32 <= 0xdfff || c32 >= 0xfffe )
               {
                  // Overlong sequence, UTF16 surrogate, or BOM
                  c32 = s_replacementCharacter;
               }
               *s16++ = char16_type( c32 );
            }
         }
         else
         {
            i = errorIndex;
            moreChars = 0;
            *s16++ = s_replacementCharacter;
         }
      }
      else
      {
         if ( c8 < 128 )
         {
            *s16++ = c8;
         }
         else if ( (c8 & 0xe0) == 0xc0 )
         {
            c32 = c8 & 0x1f;
            moreChars = 1;
            errorIndex = i;
            ucMin = 0x80;
         }
         else if ( (c8 & 0xf0) == 0xe0 )
         {
            c32 = c8 & 0x0f;
            moreChars = 2;
            errorIndex = i;
            ucMin = 0x800;
         }
         else if ( (c8 & 0xf8) == 0xf0 )
         {
            c32 = c8 & 0x07;
            moreChars = 3;
            errorIndex = i;
            ucMin = 0x10000;
         }
         else
         {
            // Error
            *s16++ = s_replacementCharacter;
         }
      }
   }

   if ( moreChars )
   {
      // Remaining invalid characters.
      for ( size_type i = errorIndex; i < n; ++i )
         *s16++ = s_replacementCharacter;
   }

   result.SetLength( s16 - result.Begin() );

   return result;
}

// ----------------------------------------------------------------------------

IsoString String::UTF16ToUTF8( const char16_type* str, size_type p, size_type n )
{
   if ( n == 0 )
      return IsoString();
   size_type size = CharTraits::Length( str );
   if ( p >= size )
      return IsoString();

   n = pcl::Min( n, size-p );
   str += p;

   IsoString result;
   result.Reserve( n*4 ); // worst case

   uint8* s8 = (uint8*)result.Begin();
   const char16_type* s16 = str;

   for ( size_type i = 0; i < n; ++i )
   {
      char16_type c16 = *s16;
      if ( c16 < 0x80 )
         *s8++ = uint8( c16 );
      else
      {
         if ( c16 < 0x0800 )
            *s8++ = 0xc0 | uint8( c16 >> 6 );
         else
         {
            uint32 c32 = c16;

            if ( CharTraits::IsHighSurrogate( c16 ) && i < n-1 )
            {
               char16_type low = s16[1];
               if ( CharTraits::IsLowSurrogate( low ) )
               {
                  ++s16;
                  ++i;
                  c32 = CharTraits::SurrogatePairToUTF32( c16, low );
               }
            }

            if ( c32 > 0xffff )
            {
               *s8++ = 0xf0 | uint8( c32 >> 18 );
               *s8++ = 0x80 | (uint8( c32 >> 12 ) & 0x3f);
            }
            else
               *s8++ = 0xe0 | uint8( c32 >> 12 );

            *s8++ = 0x80 | (uint8( c32 >> 6 ) & 0x3f);
         }

         *s8++ = 0x80 | uint8( c16 & 0x3f );
      }

      ++s16;
   }

   result.SetLength( s8 - (uint8*)result.Begin() );

   return result;
}

// ----------------------------------------------------------------------------

Array<uint32> String::UTF16ToUTF32( const char16_type* str, size_type p, size_type n )
{
   Array<uint32> a32;

   if ( n > 0 )
   {
      size_type size = CharTraits::Length( str );
      if ( p < size )
      {
         n = pcl::Min( n, size-p );
         str += p;

         for ( size_type i = 0; i < n; ++i )
         {
            char16_type c16 = str[i];

            if ( CharTraits::IsHighSurrogate( c16 ) && i < n-1 )
            {
               char16_type low = str[i+1];
               if ( CharTraits::IsLowSurrogate( low ) )
               {
                  a32.Add( CharTraits::SurrogatePairToUTF32( c16, low ) );
                  ++i;
                  continue;
               }
            }

            a32.Add( uint32( c16 ) );
         }
      }
   }

   a32.Add( uint32( 0 ) );

   return a32;
}

// ----------------------------------------------------------------------------

String String::UTF32ToUTF16( const uint32* str, size_type p, size_type n )
{
   if ( str == nullptr || n == 0 )
      return string_base();

   size_type size = 0;
   while ( str[size] != 0 )
      ++size;
   if ( p >= size )
      return string_base();

   n = pcl::Min( n, size-p );
   str += p;

   String result;
   result.Reserve( n*2 ); // worst case

   char16_type* c16 = result.Begin();
   for ( size_type i = 0; i < n; ++i )
   {
      uint32 c32 = str[i];
      if ( c32 > 0xffff )
      {
         // Decompose into a surrogate pair
         *c16++ = CharTraits::HighSurrogate( c32 );
         c32 = CharTraits::LowSurrogate( c32 );
      }
      *c16++ = char16_type( c32 );
   }

   result.SetLength( c16 - result.Begin() );

   return result;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

int IsoString::VFormat( const char* fmt, va_list argsList )
{
   if ( fmt == nullptr || *fmt == '\0' )
   {
      Clear();
      return 0;
   }

   for ( size_type count = Max( __PCL_STRING_VFORMAT_BASE_SIZE, Capacity() ); ; )
   {
      /*
       * Ensure that we have at least count characters available (plus null)
       * N.B.: Reserve() causes this string to be unique.
       */
      Reserve( count );

      va_list args;
#ifdef va_copy
      va_copy( args, argsList );
#else
      args = argsList;
#endif

      /*
       * Try to perform the formatted output. vsnprintf will return the number
       * of characters written, -1 or the required space (depending on glibc
       * versions) if the number of characters to write exceeds count.
       */
      int n = VSNPRINTF( m_data->string, count+1, fmt, args );

      va_end( args );

      /*
       * If n >= 0 and n <= count, then count chars were sufficient and the
       * formatted output was successful.
       */
      if ( n >= 0 && size_type( n ) <= count )
      {
         SetLength( n );
         return n;
      }

      /*
       * Try again with more space.
       */
      if ( n < 0 )               // ** VC++ and glibc 2.0
         count <<= 1;            //    double the available space
      else                       // ** glibc >= 2.1
         count = size_type( n ); //    vsnprintf returns the required length
   }
}

int IsoString::AppendVFormat( const char* fmt, va_list argsList )
{
   if ( fmt == nullptr || *fmt == '\0' )
      return 0;

   size_type len = Length();

   for ( size_type count = Max( __PCL_STRING_VFORMAT_BASE_SIZE, Available() ); ; )
   {
      /*
       * Ensure that we have at least the current length plus count additional
       * characters available, plus the terminating null.
       * N.B.: Reserve() causes this string to be unique.
       */
      Reserve( len + count );

      va_list args;
#ifdef va_copy
      va_copy( args, argsList );
#else
      args = argsList;
#endif

      /*
       * Try to perform the formatted output at the end of the current string.
       * vsnprintf will return the number of characters written, -1 or the
       * required space (depending on glibc versions) if the number of
       * characters to write exceeds count.
       */
      int n = VSNPRINTF( m_data->string+len, count+1, fmt, args );

      va_end( args );

      /*
       * If n >= 0 and n <= count, then count chars were sufficient and the
       * formatted output was successful.
       */
      if ( n >= 0 && size_type( n ) <= count )
      {
         SetLength( len + size_type( n ) );
         return n;
      }

      /*
       * Try again with more space.
       */
      if ( n < 0 )               // ** VC++ and glibc 2.0
         count <<= 1;            //    double the available space
      else                       // ** glibc >= 2.1
         count = size_type( n ); //    vsnprintf returns the required length
   }
}

// ----------------------------------------------------------------------------

IsoString::ustring_base IsoString::MBSToWCS() const
{
   ustring_base s;

   if ( !IsEmpty() )
   {
#ifdef __PCL_WINDOWS

      // First call to mbstowcs() to learn the required size
      size_t n = mbstowcs( nullptr, Begin(), Length()+1 );
      if ( n == size_t( -1 ) )   // check for conversion error
         return ustring_base();

      s.Reserve( n + 16 );

      n = mbstowcs( (wchar_t*)s.Begin(), Begin(), s.Capacity() );
      if ( n == size_t( -1 ) )   // check for conversion error
         return ustring_base();

      s.SetLength( n );

#else // !__PCL_WINDOWS

      Array<wchar_t> w;

      // First call to mbstowcs() to learn the required size
      size_t n = mbstowcs( nullptr, Begin(), Length()+1 );
      if ( n == size_t( -1 ) )   // check for conversion error
         return ustring_base();

      w.Add( wchar_t( 0 ), n + 16 );

      n = mbstowcs( w.Begin(), Begin(), w.Capacity() );
      if ( n == size_t( -1 ) )   // check for conversion error
         return ustring_base();

      w[n] = wchar_t( 0 );

      s = String::UTF32ToUTF16( (const uint32*)w.Begin() );

#endif   // __PCL_WINDOWS
   }

   return s;
}

// ----------------------------------------------------------------------------

bool IsoString::ToBool() const
{
   PCL_TO_BOOL_BODY();
}

bool IsoString::TryToBool( bool& value ) const
{
   PCL_TRY_TO_BOOL_BODY();
}

// ----------------------------------------------------------------------------

float IsoString::ToFloat() const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 == End() )
      throw ParseError( "Parsing 32-bit floating point expression: empty string" );
   const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
   iterator endptr = nullptr;
   errno = 0;
   float val = ::strtof( p1, &endptr );
   if ( errno == ERANGE )
      throw ParseError( "Parsing 32-bit floating point expression: out of the representable range", *this );
   if ( errno != 0 || (endptr != nullptr && endptr != p2) )
      throw ParseError( "Parsing 32-bit floating point expression: conversion error", *this,
                        (endptr != nullptr && endptr != p2) ? endptr - Begin() : -1 );
   return val;
}

bool IsoString::TryToFloat( float& value ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 < End() )
   {
      const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
      iterator endptr = nullptr;
      errno = 0;
      float val = ::strtof( p1, &endptr );
      if ( errno == 0 && (endptr == nullptr || endptr == p2) )
      {
         value = val;
         return true;
      }
   }
   return false;
}

// ----------------------------------------------------------------------------

double IsoString::ToDouble() const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 == End() )
      throw ParseError( "Parsing 64-bit floating point expression: empty string" );
   const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
   iterator endptr = nullptr;
   errno = 0;
   double val = ::strtod( p1, &endptr );
   if ( errno == ERANGE )
      throw ParseError( "Parsing 64-bit floating point expression: out of the representable range", *this );
   if ( errno != 0 || (endptr != nullptr && endptr != p2) )
      throw ParseError( "Parsing 64-bit floating point expression: conversion error", *this,
                        (endptr != nullptr && endptr != p2) ? endptr - Begin() : -1 );
   return val;
}

bool IsoString::TryToDouble( double& value ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 < End() )
   {
      const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
      iterator endptr = nullptr;
      errno = 0;
      double val = ::strtod( p1, &endptr );
      if ( errno == 0 && (endptr == nullptr || endptr == p2) )
      {
         value = val;
         return true;
      }
   }
   return false;
}

// ----------------------------------------------------------------------------

long IsoString::ToInt( int base ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 == End() )
      throw ParseError( "Parsing integer expression: empty string" );
   const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
   iterator endptr = nullptr;
   errno = 0;
   long val = ::strtol( p1, &endptr, base );
   if ( errno == ERANGE )
      throw ParseError( "Parsing integer expression: out of the representable range", *this );
   if ( errno != 0 || (endptr != nullptr && endptr != p2) )
      throw ParseError( "Parsing integer expression: conversion error", *this,
                        (endptr != nullptr && endptr != p2) ? endptr - Begin() : -1 );
   return val;
}

bool IsoString::TryToInt( int& value, int base ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 == End() )
      return false;
   const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
   iterator endptr = nullptr;
   errno = 0;
   long val = ::strtol( p1, &endptr, base );
   if ( errno == 0 && (endptr == nullptr || endptr == p2) )
   {
      value = int( val );
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------

unsigned long IsoString::ToUInt( int base ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 == End() )
      throw ParseError( "Parsing unsigned integer expression: empty string" );
   const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
   iterator endptr = nullptr;
   errno = 0;
   unsigned long val = ::strtoul( p1, &endptr, base );
   if ( errno == ERANGE )
      throw ParseError( "Parsing unsigned integer expression: out of the representable range", *this );
   if ( errno != 0 || (endptr != nullptr && endptr != p2) )
      throw ParseError( "Parsing unsigned integer expression: conversion error", *this,
                        (endptr != nullptr && endptr != p2) ? endptr - Begin() : -1 );
   return val;
}

bool IsoString::TryToUInt( unsigned& value, int base ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 == End() )
      return false;
   const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
   iterator endptr = nullptr;
   errno = 0;
   unsigned long val = ::strtoul( p1, &endptr, base );
   if ( errno == 0 && (endptr == nullptr || endptr == p2) )
   {
      value = unsigned( val );
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------

long long IsoString::ToInt64( int base ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 == End() )
      throw ParseError( "Parsing 64-bit integer expression: empty string" );
   const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
   iterator endptr = nullptr;
   errno = 0;
   long long val =
#ifdef _MSC_VER
      ::_strtoi64( p1, &endptr, base );
#else
      ::strtoll( p1, &endptr, base );
#endif
   if ( errno == ERANGE )
      throw ParseError( "Parsing 64-bit integer expression: out of the representable range", *this );
   if ( errno != 0 || (endptr != nullptr && endptr != p2) )
      throw ParseError( "Parsing 64-bit integer expression: conversion error", *this,
                        (endptr != nullptr && endptr != p2) ? endptr - Begin() : -1 );
   return val;
}

bool IsoString::TryToInt64( long long& value, int base ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 == End() )
      return false;
   const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
   iterator endptr = nullptr;
   errno = 0;
   long long val =
#ifdef _MSC_VER
      ::_strtoi64( p1, &endptr, base );
#else
      ::strtoll( p1, &endptr, base );
#endif
   if ( errno == 0 && (endptr == nullptr || endptr == p2) )
   {
      value = val;
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------

unsigned long long IsoString::ToUInt64( int base ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 == End() )
      throw ParseError( "Parsing 64-bit unsigned integer expression: empty string" );
   const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
   iterator endptr = nullptr;
   errno = 0;
   unsigned long long val =
#ifdef _MSC_VER
      ::_strtoui64( Begin(), &endptr, base );
#else
      ::strtoull( Begin(), &endptr, base );
#endif
   if ( errno == ERANGE )
      throw ParseError( "Parsing 64-bit unsigned integer expression: out of the representable range", *this );
   if ( errno != 0 || (endptr != nullptr && endptr != p2) )
      throw ParseError( "Parsing 64-bit unsigned integer expression: conversion error", *this,
                        (endptr != nullptr && endptr != p2) ? endptr - Begin() : -1 );
   return val;
}

bool IsoString::TryToUInt64( unsigned long long& value, int base ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 == End() )
      return false;
   const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
   iterator endptr = nullptr;
   errno = 0;
   unsigned long long val =
#ifdef _MSC_VER
      ::_strtoui64( Begin(), &endptr, base );
#else
      ::strtoull( Begin(), &endptr, base );
#endif
   if ( errno == 0 && (endptr == nullptr || endptr == p2) )
   {
      value = val;
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void String::Assign( const wchar_t* t )
{
   size_type len = (t != nullptr) ? ::wcslen( t ) : 0;
   if ( len > 0 )
   {
      MaybeReallocate( len );
#ifdef __PCL_WINDOWS
      CharTraits::Copy( m_data->string, (const char16_type*)t, len );
#else
      for ( iterator i = m_data->string; i < m_data->end; ++i, ++t )
         *i = (*t < 0x10000) ? char_type( *t ) : char_type( '?' );
#endif   // __PCL_WINDOWS
   }
   else
      Clear();
}

void String::Assign( const wchar_t* t, size_type i, size_type n )
{
   size_type len = (t != nullptr) ? ::wcslen( t ) : 0;
   if ( i < len )
   {
      MaybeReallocate( n = pcl::Min( n, len-i ) );
      t += i;
#ifdef __PCL_WINDOWS
      CharTraits::Copy( m_data->string, (const char16_type*)t, n );
#else
      for ( iterator i = m_data->string; i < m_data->end; ++i, ++t )
         *i = (*t < 0x10000) ? char_type( *t ) : char_type( '?' );
#endif   // __PCL_WINDOWS
   }
   else
      Clear();
}

// ----------------------------------------------------------------------------

int String::VFormat( const wchar_t* fmt, va_list argsList )
{
   if ( fmt == nullptr || *fmt == 0 )
   {
      Clear();
      return 0;
   }

#ifdef __PCL_WINDOWS

   for ( size_type count = Max( __PCL_STRING_VFORMAT_BASE_SIZE, Capacity() ); ; )
   {
      /*
       * Ensure that we have at least count characters available (plus null).
       * N.B.: Reserve() causes this string to be unique.
       */
      Reserve( count );

      va_list args;
#ifdef va_copy
      va_copy( args, argsList );
#else
      args = argsList;
#endif

      /*
       * Try to perform the formatted output. vswprintf will return the number
       * of characters written, or -1 if the number of characters to write
       * exceeds count.
       */
      int n = VSWPRINTF( (wchar_t*)m_data->string, count+1, fmt, args );

      va_end( args );

      /*
       * If n >= 0 and n <= count, then count chars were sufficient and the
       * formatted output was successful.
       */
      if ( n >= 0 && size_type( n ) <= count )
      {
         SetLength( n );
         return n;
      }

      /*
       * Try again with more space.
       */
      count <<= 1;
   }

#else  // !__PCL_WINDOWS

   Array<wchar_t> wdata;

   for ( size_type count = Max( __PCL_STRING_VFORMAT_BASE_SIZE, Capacity() ); ; )
   {
      /*
       * Ensure that we have at least count characters available (plus null).
       */
      wdata.Reserve( count+1 );

      va_list args;
#ifdef va_copy
      va_copy( args, argsList );
#else
      args = argsList;
#endif

      /*
       * Try to perform the formatted output. _vsnprintf will return the number
       * of characters written, or -1 if the number of characters to write
       * exceeds count.
       */
      int n = VSWPRINTF( wdata.Begin(), count+1, fmt, args );

      va_end( args );

      /*
       * If n >= 0 and n <= count, then count chars were sufficient and the
       * formatted output was successful.
       */
      if ( n >= 0 && size_type( n ) <= count )
      {
         if ( n > 0 )
         {
            MaybeReallocate( n );
            Array<wchar_t>::const_iterator w = wdata.Begin();
            for ( iterator i = m_data->string; i < m_data->end; ++i, ++w )
               *i = char16_type( *w );
         }
         else
            Clear();
         return n;
      }

      /*
       * Try again with more space.
       */
      count <<= 1;
   }

#endif // __PCL_WINDOWS
}

int String::AppendVFormat( const wchar_t* fmt, va_list argsList )
{
   if ( fmt == nullptr || *fmt == 0 )
      return 0;

   size_type len = Length();

#  ifdef __PCL_WINDOWS

   for ( size_type count = Max( __PCL_STRING_VFORMAT_BASE_SIZE, Available() ); ; )
   {
      /*
       * Ensure we have at least the current length plus count additional
       * characters available, plus the terminating null.
       * N.B.: Reserve() causes this string to be unique.
       */
      Reserve( len + count );

      va_list args;
#ifdef va_copy
      va_copy( args, argsList );
#else
      args = argsList;
#endif

      /*
       * Try to perform the formatted output at the end of the current string.
       * _vsnprintf will return the number of characters written, or -1 if the
       * number of characters to write exceeds count.
       */
      int n = VSWPRINTF( (wchar_t*)m_data->string+len, count+1, fmt, args );

      va_end( args );

      /*
       * If n >= 0 and n <= count, then count chars were sufficient and the
       * formatted output was successful.
       */
      if ( n >= 0 && size_type( n ) <= count )
      {
         SetLength( len + n );
         return n;
      }

      /*
       * Try again with more space.
       */
      count <<= 1;
   }

#else  // !__PCL_WINDOWS

   Array<wchar_t> wdata;

   for ( size_type count = Max( __PCL_STRING_VFORMAT_BASE_SIZE, Capacity() ); ; )
   {
      /*
       * Ensure that we have at least count characters available (plus null).
       */
      wdata.Reserve( count+1 );

      va_list args;
#ifdef va_copy
      va_copy( args, argsList );
#else
      args = argsList;
#endif

      /*
       * Try to perform the formatted output. _vsnprintf will return the number
       * of characters written, or -1 if the number of characters to write
       * exceeds count.
       */
      int n = VSWPRINTF( wdata.Begin(), count+1, fmt, args );

      va_end( args );

      /*
       * If n >= 0 and n <= count, then count chars were sufficient and the
       * formatted output was successful.
       */
      if ( n >= 0 && size_type( n ) <= count )
      {
         if ( n > 0 )
         {
            SetLength( len + n );
            Array<wchar_t>::const_iterator w = wdata.Begin();
            for ( iterator i = m_data->string+len; i < m_data->end; ++i, ++w )
               *i = char16_type( *w );
         }
         return n;
      }

      /*
       * Try again with more space. We impose a reasonable limit of 512 KiB.
       */
      count <<= 1;
   }

#endif // __PCL_WINDOWS
}

// ----------------------------------------------------------------------------

IsoString String::ToIsoString() const
{
   IsoString s;
   if ( !IsEmpty() )
   {
      s.SetLength( Length() );
      char8_iterator r = s.Begin();
      for ( const_iterator p = m_data->string; p < m_data->end; ++p, ++r )
         *r = (*p < 0x100) ? char8_type( uint8( *p ) ) : '?';
   }
   return s;
}

IsoString String::To7BitASCII() const
{
   IsoString s;
   if ( !IsEmpty() )
   {
      s.SetLength( Length() );
      char8_iterator r = s.Begin();
      for ( const_iterator p = m_data->string; p < m_data->end; ++p, ++r )
         *r = (*p < 0x80) ? char8_type( uint8( *p ) ) : '?';
   }
   return s;
}

IsoString String::ToMBS() const
{
   IsoString s;

   if ( !IsEmpty() )
   {
      size_t n;

#ifdef __PCL_WINDOWS

      // First call to wcstombs() to learn the required size
      n = wcstombs( nullptr, (const wchar_t*)Begin(), (Length() + 1)*2 );
      if ( n == size_t( -1 ) )   // check for conversion error
         return IsoString();

      s.Reserve( n + 16 );

      n = wcstombs( s.Begin(), (const wchar_t*)Begin(), s.Capacity() );
      if ( n == size_t( -1 ) )   // check for conversion error
         return IsoString();

#else // !__PCL_WINDOWS

      Array<uint32> w = ToUTF32();

      // First call to wcstombs() to learn the required size
      n = wcstombs( nullptr, (const wchar_t*)w.Begin(), (w.Length() + 1)*4 );
      if ( n == size_t( -1 ) )   // check for conversion error
         return IsoString();

      s.Reserve( n + 16 );

      n = wcstombs( s.Begin(), (const wchar_t*)w.Begin(), s.Capacity() );
      if ( n == size_t( -1 ) )   // check for conversion error
         return IsoString();

#endif   // __PCL_WINDOWS

      s.SetLength( n );
   }

   return s;
}

// ----------------------------------------------------------------------------

bool String::ToBool() const
{
   PCL_TO_BOOL_BODY();
}

bool String::TryToBool( bool& value ) const
{
   PCL_TRY_TO_BOOL_BODY();
}

// ----------------------------------------------------------------------------

float String::ToFloat() const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 == End() )
      throw ParseError( "Parsing 32-bit floating point expression: empty string" );
   const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
   wchar_t* endptr = nullptr;
   errno = 0;
#ifdef __PCL_WINDOWS
   float val = ::wcstof( (const wchar_t*)p1, &endptr );
#else
   Array<wchar_t> a = ToWCharArray( p1-Begin(), p2-p1 );
   float val = ::wcstof( a.Begin(), &endptr );
#endif // __PCL_WINDOWS
   if ( errno == ERANGE )
      throw ParseError( "Parsing 32-bit floating point expression: out of the representable range", *this );
#ifdef __PCL_WINDOWS
   if ( errno != 0 || (endptr != nullptr && endptr != (const wchar_t*)p2) )
      throw ParseError( "Parsing 32-bit floating point expression: conversion error", *this,
                        (endptr != nullptr && endptr != (const wchar_t*)p2) ? endptr - (const wchar_t*)Begin() : -1 );
#else
   if ( errno != 0 || (endptr != nullptr && endptr != a.End()-1) )
      throw ParseError( "Parsing 32-bit floating point expression: conversion error", *this,
                        (endptr != nullptr && endptr != a.End()-1) ? endptr - a.Begin() : -1 );
#endif // __PCL_WINDOWS
   return val;
}

bool String::TryToFloat( float& value ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 < End() )
   {
      const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
      wchar_t* endptr = nullptr;
      errno = 0;
#ifdef __PCL_WINDOWS
      float val = ::wcstof( (const wchar_t*)p1, &endptr );
      if ( errno == 0 && (endptr == nullptr || endptr == (const wchar_t*)p2) )
#else
      Array<wchar_t> a = ToWCharArray( p1-Begin(), p2-p1 );
      float val = ::wcstof( a.Begin(), &endptr );
      if ( errno == 0 && (endptr == nullptr || endptr == a.End()-1) )
#endif // __PCL_WINDOWS
      {
         value = val;
         return true;
      }
   }
   return false;
}

// ----------------------------------------------------------------------------

double String::ToDouble() const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 == End() )
      throw ParseError( "Parsing 64-bit floating point expression: empty string" );
   const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
   wchar_t* endptr = nullptr;
   errno = 0;
#ifdef __PCL_WINDOWS
   double val = ::wcstod( (const wchar_t*)p1, &endptr );
#else
   Array<wchar_t> a = ToWCharArray( p1-Begin(), p2-p1 );
   double val = ::wcstod( a.Begin(), &endptr );
#endif // __PCL_WINDOWS
   if ( errno == ERANGE )
      throw ParseError( "Parsing 64-bit floating point expression: out of the representable range", *this );
#ifdef __PCL_WINDOWS
   if ( errno != 0 || (endptr != nullptr && endptr != (const wchar_t*)p2) )
      throw ParseError( "Parsing 64-bit floating point expression: conversion error", *this,
                        (endptr != nullptr && endptr != (const wchar_t*)p2) ? endptr - (const wchar_t*)Begin() : -1 );
#else
   if ( errno != 0 || (endptr != nullptr && endptr != a.End()-1) )
      throw ParseError( "Parsing 64-bit floating point expression: conversion error", *this,
                        (endptr != nullptr && endptr != a.End()-1) ? endptr - a.Begin() : -1 );
#endif // __PCL_WINDOWS
   return val;
}

bool String::TryToDouble( double& value ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 < End() )
   {
      const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
      wchar_t* endptr = nullptr;
      errno = 0;
#ifdef __PCL_WINDOWS
      double val = ::wcstod( (const wchar_t*)p1, &endptr );
      if ( errno == 0 && (endptr == nullptr || endptr == (const wchar_t*)p2) )
#else
      Array<wchar_t> a = ToWCharArray( p1-Begin(), p2-p1 );
      double val = ::wcstod( a.Begin(), &endptr );
      if ( errno == 0 && (endptr == nullptr || endptr == a.End()-1) )
#endif // __PCL_WINDOWS
      {
         value = val;
         return true;
      }
   }
   return false;
}

// ----------------------------------------------------------------------------

long String::ToInt( int base ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 == End() )
      throw ParseError( "Parsing integer expression: empty string" );
   const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
   wchar_t* endptr = nullptr;
   errno = 0;
#ifdef __PCL_WINDOWS
   long val = ::wcstol( (const wchar_t*)p1, &endptr, base );
#else
   Array<wchar_t> a = ToWCharArray( p1-Begin(), p2-p1 );
   long val = ::wcstol( a.Begin(), &endptr, base );
#endif   // __PCL_WINDOWS
   if ( errno == ERANGE )
      throw ParseError( "Parsing integer expression: out of the representable range", *this );
#ifdef __PCL_WINDOWS
   if ( errno != 0 || (endptr != nullptr && endptr != (const wchar_t*)p2) )
      throw ParseError( "Parsing integer expression: conversion error", *this,
                        (endptr != nullptr && endptr != (const wchar_t*)p2) ? endptr - (const wchar_t*)Begin() : -1 );
#else
   if ( errno != 0 || (endptr != nullptr && endptr != a.End()-1) )
      throw ParseError( "Parsing integer expression: conversion error", *this,
                        (endptr != nullptr && endptr != a.End()-1) ? endptr - a.Begin() : -1 );
#endif   // __PCL_WINDOWS
   return val;
}

bool String::TryToInt( int& value, int base ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 < End() )
   {
      const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
      wchar_t* endptr = nullptr;
      errno = 0;
#ifdef __PCL_WINDOWS
      long val = ::wcstol( (const wchar_t*)p1, &endptr, base );
      if ( errno == 0 && (endptr == nullptr || endptr == (const wchar_t*)p2) )
#else
      Array<wchar_t> a = ToWCharArray( p1-Begin(), p2-p1 );
      long val = ::wcstol( a.Begin(), &endptr, base );
      if ( errno == 0 && (endptr == nullptr || endptr == a.End()-1) )
#endif   // __PCL_WINDOWS
      {
         value = int( val );
         return true;
      }
   }
   return false;
}

// ----------------------------------------------------------------------------

unsigned long String::ToUInt( int base ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 == End() )
      throw ParseError( "Parsing unsigned integer expression: empty string" );
   const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
   wchar_t* endptr = nullptr;
   errno = 0;
#ifdef __PCL_WINDOWS
   unsigned long val = ::wcstoul( (const wchar_t*)p1, &endptr, base );
#else
   Array<wchar_t> a = ToWCharArray( p1-Begin(), p2-p1 );
   unsigned long val = ::wcstoul( a.Begin(), &endptr, base );
#endif   // __PCL_WINDOWS
   if ( errno == ERANGE )
      throw ParseError( "Parsing unsigned integer expression: out of the representable range", *this );
#ifdef __PCL_WINDOWS
   if ( errno != 0 || (endptr != nullptr && endptr != (const wchar_t*)p2) )
      throw ParseError( "Parsing unsigned integer expression: conversion error", *this,
                        (endptr != nullptr && endptr != (const wchar_t*)p2) ? endptr - (const wchar_t*)Begin() : -1 );
#else
   if ( errno != 0 || (endptr != nullptr && endptr != a.End()-1) )
      throw ParseError( "Parsing unsigned integer expression: conversion error", *this,
                        (endptr != nullptr && endptr != a.End()-1) ? endptr - a.Begin() : -1 );
#endif   // __PCL_WINDOWS
   return val;
}

bool String::TryToUInt( unsigned& value, int base ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 < End() )
   {
      const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
      wchar_t* endptr = nullptr;
      errno = 0;
#ifdef __PCL_WINDOWS
      unsigned long val = ::wcstoul( (const wchar_t*)p1, &endptr, base );
      if ( errno == 0 && (endptr == nullptr || endptr == (const wchar_t*)p2) )
#else
      Array<wchar_t> a = ToWCharArray( p1-Begin(), p2-p1 );
      unsigned long val = ::wcstoul( a.Begin(), &endptr, base );
      if ( errno == 0 && (endptr == nullptr || endptr == a.End()-1) )
#endif   // __PCL_WINDOWS
      {
         value = unsigned( val );
         return true;
      }
   }
   return false;
}

// ----------------------------------------------------------------------------

long long String::ToInt64( int base ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 == End() )
      throw ParseError( "Parsing 64-bit integer expression: empty string" );
   const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
   wchar_t* endptr = nullptr;
   errno = 0;
   long long val;
#ifdef __PCL_WINDOWS
   val = ::_wcstoi64( (const wchar_t*)p1, &endptr, base );
#else
   Array<wchar_t> a = ToWCharArray( p1-Begin(), p2-p1 );
   val = ::wcstoll( a.Begin(), &endptr, base );
#endif   // __PCL_WINDOWS
   if ( errno == ERANGE )
      throw ParseError( "Parsing 64-bit integer expression: out of the representable range", *this );
#ifdef __PCL_WINDOWS
   if ( errno != 0 || (endptr != nullptr && endptr != (const wchar_t*)p2) )
      throw ParseError( "Parsing 64-bit integer expression: conversion error", *this,
                        (endptr != nullptr && endptr != (const wchar_t*)p2) ? endptr - (const wchar_t*)Begin() : -1 );
#else
   if ( errno != 0 || (endptr != nullptr && endptr != a.End()-1) )
      throw ParseError( "Parsing 64-bit integer expression: conversion error", *this,
                        (endptr != nullptr && endptr != a.End()-1) ? endptr - a.Begin() : -1 );
#endif   // __PCL_WINDOWS
   return val;
}

bool String::TryToInt64( long long& value, int base ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 < End() )
   {
      const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
      wchar_t* endptr = nullptr;
      errno = 0;
      long long val;
#ifdef __PCL_WINDOWS
      val = ::_wcstoi64( (const wchar_t*)p1, &endptr, base );
      if ( errno == 0 && (endptr == nullptr || endptr == (const wchar_t*)p2) )
#else
      Array<wchar_t> a = ToWCharArray( p1-Begin(), p2-p1 );
      val = ::wcstoll( a.Begin(), &endptr, base );
      if ( errno == 0 && (endptr == nullptr || endptr == a.End()-1) )
#endif   // __PCL_WINDOWS
      {
         value = val;
         return true;
      }
   }
   return false;
}

// ----------------------------------------------------------------------------

unsigned long long String::ToUInt64( int base ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 == End() )
      throw ParseError( "Parsing 64-bit unsigned integer expression: empty string" );
   const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
   wchar_t* endptr = nullptr;
   errno = 0;
   unsigned long long val;
#ifdef __PCL_WINDOWS
   val = ::_wcstoui64( (const wchar_t*)p1, &endptr, base );
#else
   Array<wchar_t> a = ToWCharArray( p1-Begin(), p2-p1 );
   val = ::wcstoull( a.Begin(), &endptr, base );
#endif   // __PCL_WINDOWS
   if ( errno == ERANGE )
      throw ParseError( "Parsing 64-bit unsigned integer expression: out of the representable range", *this );
#ifdef __PCL_WINDOWS
   if ( errno != 0 || (endptr != nullptr && endptr != (const wchar_t*)p2) )
      throw ParseError( "Parsing 64-bit unsigned integer expression: conversion error", *this,
                        (endptr != nullptr && endptr != (wchar_t*)p2) ? endptr - (const wchar_t*)Begin() : -1 );
#else
   if ( errno != 0 || (endptr != nullptr && endptr != a.End()-1) )
      throw ParseError( "Parsing 64-bit unsigned integer expression: conversion error", *this,
                        (endptr != nullptr && endptr != a.End()-1) ? endptr - a.Begin() : -1 );
#endif   // __PCL_WINDOWS
   return val;
}

bool String::TryToUInt64( unsigned long long& value, int base ) const
{
   const_iterator p1 = char_traits::SearchTrimLeft( Begin(), End() );
   if ( p1 < End() )
   {
      const_iterator p2 = char_traits::SearchTrimRight( p1, End() );
      wchar_t* endptr = nullptr;
      errno = 0;
      unsigned long long val;
#ifdef __PCL_WINDOWS
      val = ::_wcstoui64( (const wchar_t*)p1, &endptr, base );
      if ( errno == 0 && (endptr == nullptr || endptr == (const wchar_t*)p2) )
#else
      Array<wchar_t> a = ToWCharArray( p1-Begin(), p2-p1 );
      val = ::wcstoull( a.Begin(), &endptr, base );
      if ( errno == 0 && (endptr == nullptr || endptr == a.End()-1) )
#endif   // __PCL_WINDOWS
      {
         value = val;
         return true;
      }
   }
   return false;
}

// ----------------------------------------------------------------------------

template <typename T>
static void RandomChars( T* t, size_type n, RandomizationOptions options )
{
   static const char* data = //                                             1234567890123456789012 3456789
   "0123456789abcdefghijklmnopqrstuvwxyzGHIJKLMNOPQRSTUVWXYZABCDEF0123456789!#$%&()*+,-./:;<=>?@[\\]^_{|}~";
//  1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234 5678901
//  0        11        2         3      3  4         5      5  6  6      7  7      8         9          0
//  h         L                         U                   H     D         S                           1

   XorShift1024 rg;

   if ( options.IsFlagSet( RandomizationOption::FullRange ) )
   {
      for ( ; n > 0; --n )
         *t++ = T( 1 + rg.UIN( ~T( 0 ) ) );
      return;
   }

   T* s = t;
   switch ( unsigned( options ) )
   {
   case RandomizationOption::Lowercase:
      for ( ; n > 0; --n )
         *s++ = T( data[10 + rg.UIN( 26 )] );
      break;
   case RandomizationOption::Uppercase:
      for ( ; n > 0; --n )
         *s++ = T( data[36 + rg.UIN( 26 )] );
      break;
   case RandomizationOption::Alpha:
      for ( ; n > 0; --n )
         *s++ = T( data[10 + rg.UIN( 52 )] );
      break;
   case RandomizationOption::Digits:
      for ( ; n > 0; --n )
         *s++ = T( data[rg.UIN( 10 )] );
      break;
   case RandomizationOption::Symbols:
      for ( ; n > 0; --n )
         *s++ = T( data[72 + rg.UIN( 29 )] );
      break;
   case unsigned( RandomizationOption::Alpha )|unsigned( RandomizationOption::Digits ):
      for ( ; n > 0; --n )
         *s++ = T( data[rg.UIN( 62 )] );
      break;
   case unsigned( RandomizationOption::Lowercase )|unsigned( RandomizationOption::Digits ):
      for ( ; n > 0; --n )
         *s++ = T( data[rg.UIN( 36 )] );
      break;
   case unsigned( RandomizationOption::Uppercase )|unsigned( RandomizationOption::Digits ):
      for ( ; n > 0; --n )
         *s++ = T( data[36 + rg.UIN( 36 )] );
      break;
   case unsigned( RandomizationOption::Digits )|unsigned( RandomizationOption::Symbols ):
      for ( ; n > 0; --n )
         *s++ = T( data[62 + rg.UIN( 39 )] );
      break;
   default:
      if ( options.IsFlagSet( RandomizationOption::HexDigits ) )
      {
         if ( options.IsFlagSet( RandomizationOption::Uppercase ) )
            for ( ; n > 0; --n )
               *s++ = T( data[56 + rg.UIN( 16 )] );
         else
            for ( ; n > 0; --n )
               *s++ = T( data[rg.UIN( 16 )] );
      }
      else if ( options.IsFlagSet( RandomizationOption::BinDigits ) )
      {
         for ( ; n > 0; --n )
            *s++ = T( data[rg.UIN( 2 )] );
      }
      else
      {
         for ( ; n > 0; --n )
         {
            char c;
            for ( ;; ) //
            {
               c = data[10 + rg.UIN( 91 )];
               if (  (options.IsFlagSet( RandomizationOption::Lowercase ) || c < 'a' || c > 'z')
                  && (options.IsFlagSet( RandomizationOption::Uppercase ) || c < 'A' || c > 'Z')
                  && (options.IsFlagSet( RandomizationOption::Digits    ) || c < '0' || c > '9')
                  && (options.IsFlagSet( RandomizationOption::Symbols   ) ||
                           c >= '0' && c <= '9' || c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z') )
                  break;
            }
            *s++ = T( c );
         }
      }
      break;
   }

   for ( size_type i = 4; i < n; ++i )
   {
      pcl::Swap( t[rg.UI64N( n )], t[rg.UI64N( n )] );
      pcl::Swap( t[rg.UI64N( n )], t[rg.UI64N( n )] );
      pcl::Swap( t[rg.UI64N( n )], t[rg.UI64N( n )] );
      pcl::Swap( t[rg.UI64N( n )], t[rg.UI64N( n )] );
   }
}

String String::Random( size_type n, RandomizationOptions options )
{
   String s;
   s.SetLength( n );
   RandomChars( s.Begin(), n, options );
   return s;
}

IsoString IsoString::Random( size_type n, RandomizationOptions options )
{
   IsoString s;
   s.SetLength( n );
   RandomChars( s.Begin(), n, options );
   return s;
}

// ----------------------------------------------------------------------------

template <typename T>
static void UUID4Chars( T* uuid )
{
// 12345678 1234 1234 1234 123456789012
// xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
// 012345678901234567890123456789012345
//           1         2         3

   RandomChars( uuid, 36, RandomizationOption::HexDigits );

   // Set dash separators.
   uuid[8] = uuid[13] = uuid[18] = uuid[23] = T( '-' );

   // Set UUID version = 4 - truly random generation.
   uuid[14] = T( '4' );

   // Constrain UUID variant to [89ab].
   char y = uuid[19];
   y = (((y < 'a') ? y - '0' : y + 10 - 'a') & 0x03) | 0x08;
   uuid[19] = T( (y < 10) ? y + '0' : y - 10 + 'a' );
}

String String::UUID()
{
   String uuid;
   uuid.SetLength( 36 );
   UUID4Chars( uuid.Begin() );
   return uuid;
}

IsoString IsoString::UUID()
{
   IsoString uuid;
   uuid.SetLength( 36 );
   UUID4Chars( uuid.Begin() );
   return uuid;
}

// ----------------------------------------------------------------------------

#define PCL_TO_ENCODED_HTML_SPECIAL_CHARS_BODY()   \
   ReplaceString( "&", "&amp;" );                  \
   ReplaceString( "\"", "&quot;" );                \
   ReplaceString( "\'", "&#039;" );                \
   ReplaceString( "<", "&lt;" );                   \
   ReplaceString( ">", "&gt;" )

#define PCL_TO_DECODED_HTML_SPECIAL_CHARS_BODY()   \
   ReplaceStringIC( "&amp;",  "&" );               \
   ReplaceStringIC( "&quot;", "\"" );              \
   ReplaceStringIC( "&#039;", "\'" );              \
   ReplaceStringIC( "&apos;", "\'" );              \
   ReplaceStringIC( "&lt;",   "<" );               \
   ReplaceStringIC( "&gt;",   ">" );

String& String::ToEncodedHTMLSpecialChars()
{
   PCL_TO_ENCODED_HTML_SPECIAL_CHARS_BODY();
   return *this;
}

IsoString& IsoString::ToEncodedHTMLSpecialChars()
{
   PCL_TO_ENCODED_HTML_SPECIAL_CHARS_BODY();
   return *this;
}

String& String::ToDecodedHTMLSpecialChars()
{
   PCL_TO_DECODED_HTML_SPECIAL_CHARS_BODY();
   return *this;
}

IsoString& IsoString::ToDecodedHTMLSpecialChars()
{
   PCL_TO_DECODED_HTML_SPECIAL_CHARS_BODY();
   return *this;
}

// ----------------------------------------------------------------------------

template <class T> static
void ParseSexagesimal( int& sign, int& s1, int& s2, double& s3, const T& separator, const T& str )
{
   Array<T> tokens;
   str.Break( tokens, separator, true/*trim*/ );
   int n = int( tokens.Length() );
   if ( n < 1 )
      throw ParseError( "Parsing sexagesimal expression: empty string" );
   if ( n > 3 )
      throw ParseError( "Parsing sexagesimal expression: too many components" );
   for ( int i = 0; i < n; ++i )
      if ( tokens[i].IsEmpty() )
         throw ParseError( "Parsing sexagesimal expression: empty component(s)" );
   try
   {
      double t1;
      if ( n == 1 )
      {
         t1 = Abs( tokens[0].ToDouble() );
      }
      else
      {
         int d = Abs( tokens[0].ToInt() );
         if ( n == 2 )
         {
            double m = tokens[1].ToDouble();
            if ( m < 0 /*|| m >= 60*/ ) // uncomment for strict component range checks
               throw ParseError( "second component out of range", tokens[1] );
            t1 = d + m/60;
         }
         else
         {
            int m = tokens[1].ToInt();
            if ( m < 0 /*|| m >= 60*/ ) // uncomment for strict component range checks
               throw ParseError( "second component out of range", tokens[1] );
            double s = tokens[2].ToDouble();
            if ( s < 0 /*|| s >= 60*/ ) // uncomment for strict component range checks
               throw ParseError( "third component out of range", tokens[2] );
            t1 = d + (m + s/60)/60;
         }
      }
      double t2 = Frac( t1 )*60;
      sign = tokens[0].StartsWith( '-' ) ? -1 : +1;
      s1 = TruncInt( t1 );
      s2 = TruncInt( t2 );
      s3 = Frac( t2 )*60;
   }
   catch ( const ParseError& e )
   {
      throw ParseError( "Parsing sexagesimal expression: " + e.Message() );
   }
   catch ( ... )
   {
      throw;
   }
}

void String::ParseSexagesimal( int& sign, int& s1, int& s2, double& s3, const String& separator ) const
{
   pcl::ParseSexagesimal( sign, s1, s2, s3, separator, *this );
}

void IsoString::ParseSexagesimal( int& sign, int& s1, int& s2, double& s3, const IsoString& separator ) const
{
   pcl::ParseSexagesimal( sign, s1, s2, s3, separator, *this );
}

// ----------------------------------------------------------------------------

template <class T> static
bool TryParseSexagesimal( int& sign, int& s1, int& s2, double& s3, const T& separator, const T& str )
{
   Array<T> tokens;
   str.Break( tokens, separator, true/*trim*/ );
   int n = int( tokens.Length() );
   if ( n < 1 || n > 3 )
      return false;
   for ( int i = 0; i < n; ++i )
      if ( tokens[i].IsEmpty() )
         return false;

   double t1;
   if ( n == 1 )
   {
      if ( !tokens[0].TryToDouble( t1 ) )
         return false;
      t1 = Abs( t1 );
   }
   else
   {
      int d;
      if ( !tokens[0].TryToInt( d ) )
         return false;
      d = Abs( d );

      if ( n == 2 )
      {
         double m;
         if ( !tokens[1].TryToDouble( m ) )
            return false;
         if ( m < 0 /*|| m >= 60*/ ) // uncomment for strict component range checks
            return false;
         t1 = d + m/60;
      }
      else
      {
         int m;
         if ( !tokens[1].TryToInt( m ) )
            return false;
         if ( m < 0 /*|| m >= 60*/ ) // uncomment for strict component range checks
            return false;
         double s;
         if ( !tokens[2].TryToDouble( s ) )
            return false;
         if ( s < 0 /*|| s >= 60*/ ) // uncomment for strict component range checks
            return false;
         t1 = d + (m + s/60)/60;
      }
   }
   double t2 = Frac( t1 )*60;
   sign = tokens[0].StartsWith( '-' ) ? -1 : +1;
   s1 = TruncInt( t1 );
   s2 = TruncInt( t2 );
   s3 = Frac( t2 )*60;
   return true;
}

bool String::TryParseSexagesimal( int& sign, int& s1, int& s2, double& s3, const String& separator ) const
{
   return pcl::TryParseSexagesimal( sign, s1, s2, s3, separator, *this );
}

bool IsoString::TryParseSexagesimal( int& sign, int& s1, int& s2, double& s3, const IsoString& separator ) const
{
   return pcl::TryParseSexagesimal( sign, s1, s2, s3, separator, *this );
}

// ----------------------------------------------------------------------------

template <class T> static
T ToSexagesimal( int sign, double s1, double s2, double s3, const SexagesimalConversionOptions& options )
{
   s1 = Abs( s1 ) + (s2 + s3/60)/60;
   s2 = Frac( s1 )*60;
   s3 = Frac( s2 )*60;
   s1 = TruncInt( s1 );
   s2 = TruncInt( s2 );

   Array<T> tokens;
   switch ( options.items )
   {
   default:
   case 3:
      s3 = Round( s3, options.precision );
      if ( s3 >= 60 )
      {
         s3 = 0;
         s2 += 1;
         if ( s2 >= 60 )
         {
            s2 = 0;
            s1 += 1;
         }
      }
      tokens << T().Format( "%.0lf", s1 )
             << T().Format( "%02.0lf", s2 )
             << T().Format( "%0*.*lf", 3+options.precision, options.precision, s3 );
      break;

   case 2:
      s2 = Round( s2 + s3/60, options.precision );
      if ( s2 >= 60 )
      {
         s2 = 0;
         s1 += 1;
      }
      tokens << T().Format( "%.0lf", s1 )
             << T().Format( "%0*.*lf", 3+options.precision, options.precision, s2 );
      break;

   case 1:
      tokens << T().Format( "%.*lf", options.precision, s1 + (s2 + s3/60)/60 );
      break;
   }

   if ( options.sign || sign < 0 )
      tokens[0].Prepend( (sign < 0) ? '-' : '+' );

   if ( options.width > 0 )
      tokens[0].JustifyRight( options.width, options.padding );

   return T().ToSeparated( tokens, options.separator );
}

String String::ToSexagesimal( int sign, double s1, double s2, double s3, const SexagesimalConversionOptions& options )
{
   return pcl::ToSexagesimal<String>( sign, s1, s2, s3, options );
}

IsoString IsoString::ToSexagesimal( int sign, double s1, double s2, double s3, const SexagesimalConversionOptions& options )
{
   return pcl::ToSexagesimal<IsoString>( sign, s1, s2, s3, options );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/String.cpp - Released 2016/02/21 20:22:19 UTC
