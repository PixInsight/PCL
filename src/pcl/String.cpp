// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/String.cpp - Released 2014/10/29 07:34:20 UTC
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

#include <pcl/String.h>
#include <pcl/Exception.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

// Starting length in characters of _vsnprintf buffers used in non-MSVC
// versions (e.g. GCC) of formatted output routines.
#ifndef _MSC_VER
#  ifndef __PCL_VSNPRINTF_BASE_SIZE
#  define __PCL_VSNPRINTF_BASE_SIZE 80
#  endif
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Unicode conversion routines
 */

static const uint16 ReplacementCharacter = 0xfffd;

// ----------------------------------------------------------------------------

String::string_base String::UTF8ToUTF16( const char* str, size_type p, size_type n )
{
   if ( str == 0 || n == 0 )
      return string_base();

   size_type size = ::strlen( str );

   if ( p >= size )
      return string_base();

   n = pcl::Min( n, size-p );
   str += p;

   string_base result;
   result.Reserve( n ); // worst case

   char16_type* s16 = result.Begin();

   uint32 uc = 0;
   uint32 ucMin = 0;

   int moreChars = 0;
   size_type errorIndex = 0;
   size_type i = 0;

   // Skip UTF8-encoded byte order mark
   if ( n >= 3 && uint8( str[0] ) == 0xef && uint8( str[1] ) == 0xbb && uint8( str[2] ) == 0xbf )
      i += 3;

   for ( ; i < n; ++i )
   {
      uint8 ch = uint8( str[i] );

      if ( moreChars )
      {
         if ( (ch & 0xc0) == 0x80 )
         {
            uc = (uc << 6) | (ch & 0x3f);
            if ( --moreChars == 0 )
            {
               if ( uc > 0xffffU && uc < 0x110000U )
               {
                  // surrogate pair
                  *s16++ = CharTraits::HighSurrogate( uc );
                  uc = CharTraits::LowSurrogate( uc );
               }
               else if ( uc < ucMin || uc >= 0xd800 && uc <= 0xdfff || uc >= 0xfffe )
               {
                  // Overlong seqence, UTF16 surrogate, or BOM
                  uc = ReplacementCharacter;
               }
               *s16++ = uc;
            }
         }
         else
         {
            i = errorIndex;
            moreChars = 0;
            *s16++ = ReplacementCharacter;
         }
      }
      else
      {
         if ( ch < 128 )
         {
            *s16++ = ch;
         }
         else if ( (ch & 0xe0) == 0xc0 )
         {
            uc = ch & 0x1f;
            moreChars = 1;
            errorIndex = i;
            ucMin = 0x80;
         }
         else if ( (ch & 0xf0) == 0xe0 )
         {
            uc = ch & 0x0f;
            moreChars = 2;
            errorIndex = i;
            ucMin = 0x800;
         }
         else if ( (ch & 0xf8) == 0xf0 )
         {
            uc = ch & 0x07;
            moreChars = 3;
            errorIndex = i;
            ucMin = 0x10000;
         }
         else
         {
            // Error
            *s16++ = ReplacementCharacter;
         }
      }
   }

   if ( moreChars )
   {
      // Remaining invalid characters.
      for ( size_type i = errorIndex; i < n; ++i )
         *s16++ = ReplacementCharacter;
   }

   *s16 = CharTraits::Null();
   result.Squeeze();

   return result;
}

// ----------------------------------------------------------------------------

String::sstring_base String::UTF16ToUTF8( const char16_type* str, size_type p, size_type n )
{
   if ( str == 0 || n == 0 )
      return sstring_base();

   size_type size = CharTraits::Length( str );

   if ( p >= size )
      return sstring_base();

   n = pcl::Min( n, size-p );
   str += p;

   sstring_base result;
   result.Reserve( n*3 + 1 );

   uint8* cursor = (uint8*)result.Begin();

   const char16_type* ch = str;

   for ( size_type i = 0; i < n; ++i )
   {
      char16_type u = *ch;

      if ( u < 0x80 )
         *cursor++ = uint8( u );
      else
      {
         if ( u < 0x0800 )
            *cursor++ = 0xc0 | uint8( u >> 6 );
         else
         {
            uint32 c32 = u;

            if ( CharTraits::IsHighSurrogate( u ) && i < n-1 )
            {
               char16_type low = ch[1];
               if ( CharTraits::IsLowSurrogate( low ) )
               {
                  ++ch;
                  ++i;
                  c32 = CharTraits::SurrogatePairToUTF32( u, low );
               }
            }

            if ( c32 > 0xffff )
            {
               *cursor++ = 0xf0 | uint8( c32 >> 18 );
               *cursor++ = 0x80 | (uint8( c32 >> 12 ) & 0x3f);
            }
            else
               *cursor++ = 0xe0 | uint8( c32 >> 12 );

            *cursor++ = 0x80 | (uint8( c32 >> 6 ) & 0x3f);
         }

         *cursor++ = 0x80 | uint8( u & 0x3f );
      }

      ++ch;
   }

   *cursor = uint8( IsoCharTraits::Null() );
   result.Squeeze();

   return result;
}

// ----------------------------------------------------------------------------

Array<uint32> String::UTF16ToUTF32( const char16_type* str, size_type p, size_type n )
{
   Array<uint32> a;

   if ( str != 0 && n != 0 )
   {
      size_type size = CharTraits::Length( str );

      if ( p < size )
      {
         n = pcl::Min( n, size-p );
         str += p;

         for ( size_type i = 0; i < n; ++i )
         {
            char16_type u = str[i];

            if ( CharTraits::IsHighSurrogate( u ) && i < n-1 )
            {
               char16_type low = str[i+1];
               if ( CharTraits::IsLowSurrogate( low ) )
               {
                  a.Add( CharTraits::SurrogatePairToUTF32( u, low ) );
                  ++i;
                  continue;
               }
            }

            a.Add( uint32( u ) );
         }
      }
   }

   a.Add( uint32( 0 ) );

   return a;
}

// ----------------------------------------------------------------------------

String::string_base String::UTF32ToUTF16( const uint32* str, size_type p, size_type n )
{
   if ( str == 0 || n == 0 )
      return string_base();

   size_type size = 0;
   while ( str[size] != 0 )
      ++size;

   if ( p >= size )
      return string_base();

   n = pcl::Min( n, size-p );
   str += p;

   string_base result;
   result.Reserve( n*2 ); // worst case

   char16_type* c16 = result.Begin();
   for ( size_type i = 0; i < n; ++i )
   {
      uint32 u = str[i];
      if ( u > 0xffff )
      {
         // Decompose into a surrogate pair
         *c16++ = CharTraits::HighSurrogate( u );
         u = CharTraits::LowSurrogate( u );
      }
      *c16++ = u;
   }

   *c16 = CharTraits::Null();
   result.Squeeze();

   return result;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

IsoString::IsoString( const_wchar_iterator t, size_type p, size_type n ) : string_base()
{
   if ( t != 0 && n > 0 )
   {
      size_type len = wchar_traits::Length( t );

      if ( p < len ) // index must be valid, and t must be non-empty
      {
         Reserve( n = pcl::Min( n, len-p ) );

         iterator j = Begin();
         for ( t += p; n > 0; ++t, ++j, --n )
            *j = char( *t & 0xff );
         *j = char_traits::Null();
      }
   }
}

// ----------------------------------------------------------------------------

IsoString& IsoString::operator =( const_wchar_iterator t )
{
   size_type n = (t != 0) ? wchar_traits::Length( t ) : 0;

   if ( n > 0 )
   {
      if ( !IsUnique() )
      {
         if ( !data->Detach() )
            delete data;
         data = new Data;
      }

      MaybeReallocate( n );

      iterator j = Begin();
      for ( ; n > 0; ++t, ++j, --n )
         *j = char( *t & 0xff );
      *j = char_traits::Null();
   }
   else
      Clear();

   return *this;
}

// ----------------------------------------------------------------------------

void IsoString::c_copy( char* dst, size_type available, size_type i ) const
{
   if ( dst != 0 && available != 0 )
   {
      if ( --available != 0 )
      {
         size_type len = Length();

         if ( i < len )
         {
            size_type n = pcl::Min( available, len-i );
            char_traits::Copy( dst, c_str()+i, n );
            dst += n;
         }
      }

      *dst = char_traits::Null();
   }
}

// ----------------------------------------------------------------------------

#ifdef _MSC_VER

// The MS VC++ compiler has the _vscprintf function, that is very nice to
// generate formatted output dynamically. This function precalculates the
// required length of the string buffer without actually performing output.

int IsoString::VFormat( const char* fmt, va_list argsList )
{
   // Learn the required space.
   int count = ::_vscprintf( fmt, argsList );

   if ( count > 0 )
   {
      // Ensure that we have at least count characters available (plus null)
      // N.B.: Reserve() causes this string to be unique.
      Reserve( count );

      // Do the actual formatted output
      return ::vsprintf( data->string, fmt, argsList );
   }

   return count;
}

int IsoString::AppendVFormat( const char* fmt, va_list argsList )
{
   int count = ::_vscprintf( fmt, argsList );

   if ( count > 0 )
   {
      size_type len = Length();

      // Ensure we have at least the current length plus count additional
      // characters available, plus the terminating null.
      // N.B.: Reserve() causes this string to be unique.
      Reserve( len + count );

      // Do the actual formatted output at the end of current string.
      return ::vsprintf( data->string+len, fmt, argsList );
   }

   return count;
}

#else    // !_MSC_VER

// The GCC compiler does not have _vscprintf or an equivalent function. In this
// case we use an iterative approach based on vsnprintf. Indeed, for small
// strings (output length < 80) this version may be faster since it requires
// only a formatted output operation. However, for larger strings it may be
// considerably slower... fortunately small formatted strings are by far much
// more frequent.

#ifdef __PCL_WINDOWS
#define VSNPRINTF ::_vsnprintf   // MinGW
#else
#define VSNPRINTF ::vsnprintf    // GCC on Linux
#endif

int IsoString::VFormat( const char* fmt, va_list argsList )
{
   if ( fmt == 0 || *fmt == '\0' )
   {
      Clear();
      return 0;
   }

   int count = pcl::Max( __PCL_VSNPRINTF_BASE_SIZE, 2*int( ::strlen( fmt ) ) );

   for ( ;; )
   {
      // Ensure that we have at least count characters available (plus null)
      // N.B.: Reserve() causes this string to be unique.
      Reserve( count );

      va_list args;
#ifdef va_copy
      va_copy( args, argsList );
#else
      args = argsList;
#endif

      // Try to perform the formatted output. vsnprintf will return the
      // number of characters written, -1 or the required space (depending on
      // glibc version) if the number of characters to write exceeds count.
      int n = VSNPRINTF( data->string, size_t( count+1 ), fmt, args );

      va_end( args );

      // If n >= 0 and n <= count, then count chars were sufficient and the
      // formatted output was successful.
      if ( n >= 0 && n <= count )
         return n;

      // Try again with more space
      if ( n < 0 )      // ** glibc 2.0
      {
         count <<= 1;   // double the available space

         // We impose a more than reasonable limit of 256 KiB
         if ( count > 0x40000 )
         {
            Clear();
            return -1;
         }
      }
      else              // ** glibc >= 2.1
         count = n;     // vsnprintf returns the required length
   }
}

int IsoString::AppendVFormat( const char* fmt, va_list argsList )
{
   if ( fmt == 0 || *fmt == '\0' )
      return 0;

   int count = pcl::Max( __PCL_VSNPRINTF_BASE_SIZE, 2*int( ::strlen( fmt ) ) );
   size_type len = Length();

   for ( ;; )
   {
      // Ensure we have at least the current length plus count additional
      // characters available, plus the terminating null.
      // N.B.: Reserve() causes this string to be unique.
      Reserve( len + count );

      va_list args;
#ifdef va_copy
      va_copy( args, argsList );
#else
      args = argsList;
#endif

      // Try to perform the formatted output at the end of the current
      // string. vsnprintf will return the number of characters written, -1 or
      // the required space (depending on glibc version) if the number of
      // characters to write exceeds count.
      int n = VSNPRINTF( data->string+len, size_t( count+1 ), fmt, args );

      va_end( args );

      // If n >= 0 and n <= count, then count chars were sufficient and the
      // formatted output was successful.
      if ( n >= 0 && n <= count )
         return n;

      // Try again with more space
      if ( n < 0 )      // ** glibc 2.0
      {
         count <<= 1;   // double the available space

         // We impose a more than reasonable limit of 256 KiB
         if ( count > 0x40000 )
         {
            Clear();
            return -1;
         }
      }
      else              // ** glibc >= 2.1
         count = n;     // vsnprintf returns the required length
   }
}

#endif   // _MSC_VER

// ----------------------------------------------------------------------------

IsoString::wstring_base IsoString::ToString() const
{
   wstring_base s;

   size_type n = Length();
   if ( n != 0 )
   {
      s.Reserve( n );
      register wchar_iterator p = s.Begin();
      register const uint8* j = reinterpret_cast<const uint8*>( Begin() );
      for ( ; n > 0; ++p, ++j, --n )
         *p = wchar_type( *j );
      *p = wchar_traits::Null();
   }

   return s;
}

// ----------------------------------------------------------------------------

IsoString::wstring_base IsoString::MBSToWCS() const
{
   wstring_base s;

   if ( !IsEmpty() )
   {
      size_t n;

#ifdef __PCL_WINDOWS

      // First call to mbstowcs() to learn the required size
      n = mbstowcs( 0, Begin(), Length()+1 );
      if ( n == size_t( -1 ) )   // check for conversion error
         return wstring_base();

      s.Reserve( n + 16 );

      n = mbstowcs( (wchar_t*)s.Begin(), Begin(), s.Capacity() );
      if ( n == size_t( -1 ) )   // check for conversion error
         return wstring_base();

      s[n] = wchar_traits::Null();

#else // !__PCL_WINDOWS

      Array<wchar_t> w;

      // First call to mbstowcs() to learn the required size
      n = mbstowcs( 0, Begin(), Length()+1 );
      if ( n == size_t( -1 ) )   // check for conversion error
         return wstring_base();

      w.Add( wchar_t( 0 ), n + 16 );

      n = mbstowcs( w.Begin(), Begin(), w.Capacity() );
      if ( n == size_t( -1 ) )   // check for conversion error
         return wstring_base();

      w[n] = wchar_t( 0 );

      s = String::UTF32ToUTF16( (const uint32*)w.Begin() );

#endif   // __PCL_WINDOWS
   }

   return s;
}

// ----------------------------------------------------------------------------

bool IsoString::ToBool() const
{
   if ( CompareIC( "true" ) == 0 )
      return true;
   if ( CompareIC( "false" ) == 0 )
      return false;
   if ( Compare( "1" ) == 0 )
      return true;
   if ( Compare( "0" ) == 0 )
      return false;
   if ( Compare( "T" ) == 0 )
      return true;
   if ( Compare( "F" ) == 0 )
      return false;
   throw ParseError( "Invalid boolean literal: " + *this );
}

bool IsoString::TryToBool( bool& value ) const
{
   if ( CompareIC( "true" ) == 0 )
      value = true;
   else if ( CompareIC( "false" ) == 0 )
      value = false;
   else if ( Compare( "1" ) == 0 )
      value = true;
   else if ( Compare( "0" ) == 0 )
      value = false;
   else if ( Compare( "T" ) == 0 )
      value = true;
   else if ( Compare( "F" ) == 0 )
      value = false;
   else
      return false;
   return true;
}

// ----------------------------------------------------------------------------

double IsoString::ToDouble() const
{
   if ( data->string == 0 )
      throw ParseError( "Parsing floating point expression: empty string" );

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      throw ParseError( "Parsing floating point expression: empty string" );

   iterator p2 = SearchTrimRight();

   iterator endptr = 0;
   errno = 0;
   double val = ::strtod( p1, &endptr );

   if ( errno == ERANGE )
      throw ParseError( "Parsing floating point expression: out of the representable range", *this );

   if ( errno != 0 || (endptr != 0 && endptr != p2) )
      throw ParseError( "Parsing floating point expression: conversion error", *this,
                        (endptr != 0 && endptr != p2) ? endptr - Begin() : -1 );

   return val;
}

bool IsoString::TryToDouble( double& value ) const
{
   if ( data->string == 0 )
      return false;

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      return false;

   iterator p2 = SearchTrimRight();

   iterator endptr = 0;
   errno = 0;
   double val = ::strtod( p1, &endptr );

   if ( errno == 0 && (endptr == 0 || endptr == p2) )
   {
      value = val;
      return true;
   }

   return false;
}

bool IsoString::TryToFloat( float& value ) const
{
   double d = value;
   if ( TryToDouble( d ) )
   {
      value = d;
      return true;
   }

   return false;
}

// ----------------------------------------------------------------------------

long IsoString::ToInt( int base ) const
{
   if ( data->string == 0 )
      throw ParseError( "Parsing integer expression: empty string" );

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      throw ParseError( "Parsing integer expression: empty string" );

   iterator p2 = SearchTrimRight();

   iterator endptr = 0;
   errno = 0;
   long val = ::strtol( p1, &endptr, base );

   if ( errno == ERANGE )
      throw ParseError( "Parsing integer expression: out of the representable range", *this );

   if ( errno != 0 || (endptr != 0 && endptr != p2) )
      throw ParseError( "Parsing integer expression: conversion error", *this,
                        (endptr != 0 && endptr != p2) ? endptr - Begin() : -1 );

   return val;
}

bool IsoString::TryToInt( int& value, int base ) const
{
   if ( data->string == 0 )
      return false;

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      return false;

   iterator p2 = SearchTrimRight();

   iterator endptr = 0;
   errno = 0;
   long val = ::strtol( p1, &endptr, base );

   if ( errno == 0 && (endptr == 0 || endptr == p2) )
   {
      value = int( val );
      return true;
   }

   return false;
}

// ----------------------------------------------------------------------------

unsigned long IsoString::ToUInt( int base ) const
{
   if ( data->string == 0 )
      throw ParseError( "Parsing unsigned integer expression: empty string" );

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      throw ParseError( "Parsing unsigned integer expression: empty string" );

   iterator p2 = SearchTrimRight();

   iterator endptr = 0;
   errno = 0;
   unsigned long val = ::strtoul( p1, &endptr, base );

   if ( errno == ERANGE )
      throw ParseError( "Parsing unsigned integer expression: out of the representable range", *this );

   if ( errno != 0 || (endptr != 0 && endptr != p2) )
      throw ParseError( "Parsing unsigned integer expression: conversion error", *this,
                        (endptr != 0 && endptr != p2) ? endptr - Begin() : -1 );

   return val;
}

bool IsoString::TryToUInt( unsigned& value, int base ) const
{
   if ( data->string == 0 )
      return false;

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      return false;

   iterator p2 = SearchTrimRight();

   iterator endptr = 0;
   errno = 0;
   unsigned long val = ::strtoul( p1, &endptr, base );

   if ( errno == 0 && (endptr == 0 || endptr == p2) )
   {
      value = unsigned( val );
      return true;
   }

   return false;
}

// ----------------------------------------------------------------------------

long long IsoString::ToInt64( int base ) const
{
   if ( data->string == 0 )
      throw ParseError( "Parsing 64-bit integer expression: empty string" );

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      throw ParseError( "Parsing 64-bit integer expression: empty string" );

   iterator p2 = SearchTrimRight();

   iterator endptr = 0;
   errno = 0;
   long long val =
#ifdef _MSC_VER
      ::_strtoi64( p1, &endptr, base );
#else
      ::strtoll( p1, &endptr, base );
#endif

   if ( errno == ERANGE )
      throw ParseError( "Parsing 64-bit integer expression: out of the representable range", *this );

   if ( errno != 0 || (endptr != 0 && endptr != p2) )
      throw ParseError( "Parsing 64-bit integer expression: conversion error", *this,
                        (endptr != 0 && endptr != p2) ? endptr - Begin() : -1 );

   return val;
}

bool IsoString::TryToInt64( long long& value, int base ) const
{
   if ( data->string == 0 )
      return false;

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      return false;

   iterator p2 = SearchTrimRight();

   iterator endptr = 0;
   errno = 0;
   long long val =
#ifdef _MSC_VER
      ::_strtoi64( p1, &endptr, base );
#else
      ::strtoll( p1, &endptr, base );
#endif

   if ( errno == 0 && (endptr == 0 || endptr == p2) )
   {
      value = val;
      return true;
   }

   return false;
}

// ----------------------------------------------------------------------------

unsigned long long IsoString::ToUInt64( int base ) const
{
   if ( data->string == 0 )
      throw ParseError( "Parsing 64-bit unsigned integer expression: empty string" );

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      throw ParseError( "Parsing 64-bit unsigned integer expression: empty string" );

   iterator p2 = SearchTrimRight();

   iterator endptr = 0;
   errno = 0;
   unsigned long long val =
#ifdef _MSC_VER
      ::_strtoui64( Begin(), &endptr, base );
#else
      ::strtoull( Begin(), &endptr, base );
#endif

   if ( errno == ERANGE )
      throw ParseError( "Parsing 64-bit unsigned integer expression: out of the representable range", *this );

   if ( errno != 0 || (endptr != 0 && endptr != p2) )
      throw ParseError( "Parsing 64-bit unsigned integer expression: conversion error", *this,
                        (endptr != 0 && endptr != p2) ? endptr - Begin() : -1 );

   return val;
}

bool IsoString::TryToUInt64( unsigned long long& value, int base ) const
{
   if ( data->string == 0 )
      return false;

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      return false;

   iterator p2 = SearchTrimRight();

   iterator endptr = 0;
   errno = 0;
   unsigned long long val =
#ifdef _MSC_VER
      ::_strtoui64( Begin(), &endptr, base );
#else
      ::strtoull( Begin(), &endptr, base );
#endif

   if ( errno == 0 && (endptr == 0 || endptr == p2) )
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
   size_type n = (t != 0) ? ::wcslen( t ) : 0;

   if ( n != 0 )
   {
      if ( !IsUnique() )
      {
         if ( !data->Detach() )
            delete data;
         data = new Data;
      }

      MaybeReallocate( n );

#ifdef __PCL_WINDOWS
      ::memcpy( Begin(), t, (n + 1) << 1 );
#else
      register iterator i = Begin();
      for ( ; n > 0; ++i, ++t, --n )
         *i = (*t < 0x10000) ? char_type( *t ) : char_type( '?' );
      *i = char_traits::Null();
#endif   // __PCL_WINDOWS
   }
   else
      Clear();
}

void String::Assign( const wchar_t* t, size_type i, size_type n )
{
   size_type len = (t != 0) ? ::wcslen( t ) : 0;

   if ( i < len )
   {
      if ( !IsUnique() )
      {
         if ( !data->Detach() )
            delete data;
         data = new Data;
      }

      MaybeReallocate( n = pcl::Min( n, len-i ) );

#ifdef __PCL_WINDOWS
      ::memcpy( Begin(), t+i, (n + 1) << 1 );
#else
      register iterator p = Begin();
      for ( t += i; n > 0; ++p, ++t, --n )
         *p = (*t < 0x10000) ? char_type( *t ) : char_type( '?' );
      *p = char_traits::Null();
#endif   // __PCL_WINDOWS
   }
   else
      Clear();
}

void String::Assign( String::const_schar_iterator t )
{
   size_type n = (t != 0) ? schar_traits::Length( t ) : 0;

   if ( n != 0 )
   {
      if ( !IsUnique() )
      {
         if ( !data->Detach() )
            delete data;
         data = new Data;
      }

      MaybeReallocate( n );

      register iterator p = Begin();
      register const uint8* j = reinterpret_cast<const uint8*>( t );
      for ( ; n > 0; ++p, ++j, --n )
         *p = char_type( *j );
      *p = char_traits::Null();
   }
   else
      Clear();
}

void String::Assign( String::const_schar_iterator t, size_type i, size_type n )
{
   size_type len = (t != 0) ? schar_traits::Length( t ) : 0;

   if ( i < len )
   {
      if ( !IsUnique() )
      {
         if ( !data->Detach() )
            delete data;
         data = new Data;
      }

      MaybeReallocate( n = pcl::Min( n, len-i ) );

      register iterator p = Begin();
      register const uint8* j = reinterpret_cast<const uint8*>( t ) + i;
      for ( ; n > 0; ++p, ++j, --n )
         *p = char_type( *j );
      *p = char_traits::Null();
   }
   else
      Clear();
}

// ----------------------------------------------------------------------------

void String::c_copy( char16_type* dst, size_type available, size_type i ) const
{
   if ( dst != 0 && available != 0 )
   {
      if ( --available != 0 )
      {
         size_type len = Length();

         if ( i < len )
         {
            size_type n = pcl::Min( available, len-i );
            char_traits::Copy( dst, c_str()+i, n );
            dst += n;
         }
      }

      *dst = char_traits::Null();
   }
}

// ----------------------------------------------------------------------------

#ifdef _MSC_VER

// See the comments for the counterparts of these functions in IsoString:
// The same is true for _vscwprintf and _vsnwprintf.

int String::VFormat( const wchar_t* fmt, va_list argsList )
{
   // Learn the required space.
   int count = ::_vscwprintf( fmt, argsList );

   if ( count > 0 )
   {
      // Ensure that we have at least count characters available (plus null)
      // N.B.: Reserve() causes this string to be unique.
      Reserve( count );

      // Do the actual formatted output
      return ::vswprintf( (wchar_t*)data->string, size_t( count+1 ), fmt, argsList );
   }

   return count;
}

int String::AppendVFormat( const wchar_t* fmt, va_list argsList )
{
   int count = ::_vscwprintf( fmt, argsList );

   if ( count > 0 )
   {
      size_type len = Length();

      // Ensure we have at least the current length plus count additional
      // characters available, plus the terminating null.
      // N.B.: Reserve() causes this string to be unique.
      Reserve( len + count );

      // Do the actual formatted output at the end of current string.
      return ::vswprintf( (wchar_t*)data->string + len, size_t( count+1 ), fmt, argsList );
   }

   return count;
}

#else    // !_MSC_VER

// See the comments for the counterparts of these functions in IsoString:
// The same is true for _vscwprintf and vsnwprintf.

#ifdef __PCL_WINDOWS
#define VSWPRINTF ::_vsnwprintf  // MinGW
#else
#define VSWPRINTF ::vswprintf    // GCC on Linux
#endif

int String::VFormat( const wchar_t* fmt, va_list argsList )
{
   if ( fmt == 0 || *fmt == 0 )
   {
      Clear();
      return 0;
   }

   int count = pcl::Max( __PCL_VSNPRINTF_BASE_SIZE, 2*int( ::wcslen( fmt ) ) );

#  ifdef __PCL_WINDOWS

   for ( ;; )
   {
      // Ensure that we have at least count characters available (plus null)
      // N.B.: Reserve() causes this string to be unique.
      Reserve( count );

      va_list args;
#ifdef va_copy
      va_copy( args, argsList );
#else
      args = argsList;
#endif

      // Try to perform the formatted output. vswprintf will return the
      // number of characters written, or -1 if the number of characters to
      // write exceeds count.
      int n = VSWPRINTF( (wchar_t*)data->string, size_t( count+1 ), fmt, args );

      va_end( args );

      // If n >= 0 and n <= count, then count chars were sufficient and the
      // formatted output was successful.
      if ( n >= 0 && n <= count )
         return n;

      // Try again with more space
      // We impose a more than reasonable limit of 512 KiB
      if ( (count <<= 1) > 0x40000 )
      {
         Clear();
         return -1;
      }
   }

#  else  // !__PCL_WINDOWS

   Array<wchar_t> wdata;

   for ( ;; )
   {
      // Ensure that we have at least count characters available (plus null)
      wdata.Reserve( count+1 );

      va_list args;
#ifdef va_copy
      va_copy( args, argsList );
#else
      args = argsList;
#endif

      // Try to perform the formatted output. _vsnprintf will return the
      // number of characters written, or -1 if the number of characters to
      // write exceeds count.
      int n = VSWPRINTF( wdata.Begin(), size_t( count+1 ), fmt, args );

      va_end( args );

      // If n >= 0 and n <= count, then count chars were sufficient and the
      // formatted output was successful.
      if ( n >= 0 && n <= count )
      {
         if ( n == 0 )
            Clear();
         else
         {
            Reserve( n );
            Array<wchar_t>::const_iterator i = wdata.Begin();
            iterator j = data->string;
            while ( *i != 0 )
               *j++ = (char16_type)*i++;
            *j = char_traits::Null();
         }

         return n;
      }

      // Try again with more space
      // We impose a more than reasonable limit of 512 KiB
      if ( (count <<= 1) > 0x40000 )
      {
         Clear();
         return -1;
      }
   }

#  endif // __PCL_WINDOWS
}

int String::AppendVFormat( const wchar_t* fmt, va_list argsList )
{
   if ( fmt == 0 || *fmt == 0 )
      return 0;

   int count = pcl::Max( __PCL_VSNPRINTF_BASE_SIZE, 2*int( ::wcslen( fmt ) ) );
   size_type len = Length();

#  ifdef __PCL_WINDOWS

   for ( ;; )
   {
      // Ensure we have at least the current length plus count additional
      // characters available, plus the terminating null.
      // N.B.: Reserve() causes this string to be unique.
      Reserve( len + count );

      va_list args;
#ifdef va_copy
      va_copy( args, argsList );
#else
      args = argsList;
#endif

      // Try to perform the formatted output at the end of the current
      // string. _vsnprintf will return the number of characters written, or
      // -1 if the number of characters to write exceeds count.
      int n = VSWPRINTF( (wchar_t*)data->string+len, size_t( count+1 ), fmt, args );

      va_end( args );

      // If n >= 0 and n <= count, then count chars were sufficient and the
      // formatted output was successful.
      if ( n >= 0 && n <= count )
         return n;

      // Try again with more space
      // We impose a more than reasonable limit of 512 KiB
      if ( (count <<= 1) > 0x40000 )
      {
         Clear();
         return -1;
      }
   }

#  else  // !__PCL_WINDOWS

   Array<wchar_t> wdata;

   for ( ;; )
   {
      // Ensure that we have at least count characters available (plus null)
      wdata.Reserve( count+1 );

      va_list args;
#ifdef va_copy
      va_copy( args, argsList );
#else
      args = argsList;
#endif

      // Try to perform the formatted output. _vsnprintf will return the
      // number of characters written, or -1 if the number of characters to
      // write exceeds count.
      int n = VSWPRINTF( wdata.Begin(), size_t( count+1 ), fmt, args );

      va_end( args );

      // If n >= 0 and n <= count, then count chars were sufficient and the
      // formatted output was successful.
      if ( n >= 0 && n <= count )
      {
         if ( n != 0 )
         {
            Reserve( len + n );
            Array<wchar_t>::const_iterator i = wdata.Begin();
            iterator j = data->string + len;
            while ( *i != 0 )
               *j++ = (char16_type)*i++;
            *j = char_traits::Null();
         }

         return n;
      }

      // Try again with more space
      // We impose a more than reasonable limit of 512 KiB
      if ( (count <<= 1) > 0x40000 )
      {
         Clear();
         return -1;
      }
   }

#  endif // __PCL_WINDOWS
}

#endif   // _MSC_VER

// ----------------------------------------------------------------------------

String::sstring_base String::ToIsoString() const
{
   sstring_base s;

   if ( !IsEmpty() )
   {
      register size_type n = Length();
      s.Reserve( n );
      register const_iterator i = Begin();
      register schar_iterator j = s.Begin();
      for ( ; n > 0; ++i, ++j, --n )
         *j = (*i < 0x100) ? schar_type( *i ) : '?';
      *j = schar_traits::Null();
   }

   return s;
}

String::sstring_base String::To7BitASCII() const
{
   sstring_base s;

   if ( !IsEmpty() )
   {
      register size_type n = Length();
      s.Reserve( n );
      register const_iterator i = Begin();
      register schar_iterator j = s.Begin();
      for ( ; n > 0; ++i, ++j, --n )
         *j = (*i < 0x80) ? schar_type( *i ) : '?';
      *j = schar_traits::Null();
   }

   return s;
}

String::sstring_base String::ToMBS() const
{
   sstring_base s;

   if ( !IsEmpty() )
   {
      size_t n;

#ifdef __PCL_WINDOWS

      // First call to wcstombs() to learn the required size
      n = wcstombs( 0, (wchar_t*)Begin(), (Length() + 1)*2 );
      if ( n == size_t( -1 ) )   // check for conversion error
         return sstring_base();

      s.Reserve( n + 16 );

      n = wcstombs( s.Begin(), (wchar_t*)Begin(), s.Capacity() );
      if ( n == size_t( -1 ) )   // check for conversion error
         return sstring_base();

#else // !__PCL_WINDOWS

      Array<uint32> w = ToUTF32();

      // First call to wcstombs() to learn the required size
      n = wcstombs( 0, (wchar_t*)w.Begin(), (w.Length() + 1)*4 );
      if ( n == size_t( -1 ) )   // check for conversion error
         return sstring_base();

      s.Reserve( n + 16 );

      n = wcstombs( s.Begin(), (wchar_t*)w.Begin(), s.Capacity() );
      if ( n == size_t( -1 ) )   // check for conversion error
         return sstring_base();

#endif   // __PCL_WINDOWS

      s[n] = schar_traits::Null();
   }

   return s;
}

String::sstring_base String::ToLocal8Bit() const
{
#ifdef __PCL_WINDOWS
   return ToMBS();
#else
   return ToUTF8();
#endif
}

Array<wchar_t> String::ToWCharArray( size_type p, size_type n ) const
{
   Array<wchar_t> a;

   if ( n != 0 )
   {
      size_type len = Length();
      if ( p < len )
      {
         n = pcl::Min( n, len-p );

         a.Add( wchar_t( 0 ), n );

#ifdef __PCL_WINDOWS
         char_traits::Copy( reinterpret_cast<iterator>( a.Begin() ), c_str()+p, n );
#else
         const_iterator i = c_str() + p;
         Array<wchar_t>::iterator j = a.Begin();
         for ( ; n > 0; ++i, ++j, --n )
            *j = wchar_t( *i );
#endif   // __PCL_WINDOWS
      }
   }

   a.Add( wchar_t( 0 ) );
   return a;
}

// ----------------------------------------------------------------------------

bool String::ToBool() const
{
   if ( CompareIC( "true" ) == 0 )
      return true;
   if ( CompareIC( "false" ) == 0 )
      return false;
   if ( Compare( "1" ) == 0 )
      return true;
   if ( Compare( "0" ) == 0 )
      return false;
   if ( Compare( "T" ) == 0 )
      return true;
   if ( Compare( "F" ) == 0 )
      return false;
   throw ParseError( "Invalid boolean literal: " + *this );
}

bool String::TryToBool( bool& value ) const
{
   if ( CompareIC( "true" ) == 0 )
      value = true;
   else if ( CompareIC( "false" ) == 0 )
      value = false;
   else if ( Compare( "1" ) == 0 )
      value = true;
   else if ( Compare( "0" ) == 0 )
      value = false;
   else if ( Compare( "T" ) == 0 )
      value = true;
   else if ( Compare( "F" ) == 0 )
      value = false;
   else
      return false;
   return true;
}

// ----------------------------------------------------------------------------

double String::ToDouble() const
{
#ifdef __PCL_WINDOWS

   if ( data->string == 0 )
      throw ParseError( "Parsing floating point expression: empty string" );

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      throw ParseError( "Parsing floating point expression: empty string" );

   iterator p2 = SearchTrimRight();

   wchar_t* endptr = 0;
   errno = 0;
   double val = ::wcstod( (wchar_t*)p1, &endptr );

   if ( errno == ERANGE )
      throw ParseError( "Parsing floating point expression: out of the representable range", *this );

   if ( errno != 0 || (endptr != 0 && endptr != (wchar_t*)p2) )
      throw ParseError( "Parsing floating point expression: conversion error", *this,
                        (endptr != 0 && endptr != (wchar_t*)p2) ? endptr - (const wchar_t*)Begin() : -1 );

   return val;

#else

   return IsoString( *this ).ToDouble();

#endif   // __PCL_WINDOWS
}

bool String::TryToDouble( double& value ) const
{
#ifdef __PCL_WINDOWS

   if ( data->string == 0 )
      return false;

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      return false;

   iterator p2 = SearchTrimRight();

   wchar_t* endptr = 0;
   errno = 0;
   double val = ::wcstod( (wchar_t*)p1, &endptr );

   if ( errno == 0 && (endptr == 0 || endptr == (wchar_t*)p2) )
   {
      value = val;
      return true;
   }

   return false;

#else

   return IsoString( *this ).TryToDouble( value );

#endif   // __PCL_WINDOWS
}

bool String::TryToFloat( float& value ) const
{
   double d = value;
   if ( TryToDouble( d ) )
   {
      value = d;
      return true;
   }

   return false;
}

// ----------------------------------------------------------------------------

long String::ToInt( int base ) const
{
#ifdef __PCL_WINDOWS

   if ( data->string == 0 )
      throw ParseError( "Parsing integer expression: empty string" );

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      throw ParseError( "Parsing integer expression: empty string" );

   iterator p2 = SearchTrimRight();

   wchar_t* endptr = 0;
   errno = 0;
   long val = ::wcstol( (wchar_t*)p1, &endptr, base );

   if ( errno == ERANGE )
      throw ParseError( "Parsing integer expression: out of the representable range", *this );

   if ( errno != 0 || (endptr != 0 && endptr != (wchar_t*)p2) )
      throw ParseError( "Parsing integer expression: conversion error", *this,
                        (endptr != 0 && endptr != (wchar_t*)p2) ? endptr - (const wchar_t*)Begin() : -1 );

   return val;

#else

   return IsoString( *this ).ToInt( base );

#endif   // __PCL_WINDOWS
}

bool String::TryToInt( int& value, int base ) const
{
#ifdef __PCL_WINDOWS

   if ( data->string == 0 )
      return false;

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      return false;

   iterator p2 = SearchTrimRight();

   wchar_t* endptr = 0;
   errno = 0;
   long val = ::wcstol( (wchar_t*)p1, &endptr, base );

   if ( errno == 0 && (endptr == 0 || endptr == (wchar_t*)p2) )
   {
      value = int( val );
      return true;
   }

   return false;

#else

   return IsoString( *this ).TryToInt( value, base );

#endif   // __PCL_WINDOWS
}

// ----------------------------------------------------------------------------

unsigned long String::ToUInt( int base ) const
{
#ifdef __PCL_WINDOWS

   if ( data->string == 0 )
      throw ParseError( "Parsing unsigned integer expression: empty string" );

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      throw ParseError( "Parsing unsigned integer expression: empty string" );

   iterator p2 = SearchTrimRight();

   wchar_t* endptr = 0;
   errno = 0;
   unsigned long val = ::wcstoul( (wchar_t*)p1, &endptr, base );

   if ( errno == ERANGE )
      throw ParseError( "Parsing unsigned integer expression: out of the representable range", *this );

   if ( errno != 0 || (endptr != 0 && endptr != (wchar_t*)p2) )
      throw ParseError( "Parsing unsigned integer expression: conversion error", *this,
                        (endptr != 0 && endptr != (wchar_t*)p2) ? endptr - (const wchar_t*)Begin() : -1 );

   return val;

#else

   return IsoString( *this ).ToUInt( base );

#endif   // __PCL_WINDOWS
}

bool String::TryToUInt( unsigned& value, int base ) const
{
#ifdef __PCL_WINDOWS

   if ( data->string == 0 )
      return false;

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      return false;

   iterator p2 = SearchTrimRight();

   wchar_t* endptr = 0;
   errno = 0;
   unsigned long val = ::wcstoul( (wchar_t*)p1, &endptr, base );

   if ( errno == 0 && (endptr == 0 || endptr == (wchar_t*)p2) )
   {
      value = unsigned( val );
      return true;
   }

   return false;

#else

   return IsoString( *this ).TryToUInt( value, base );

#endif   // __PCL_WINDOWS
}

// ----------------------------------------------------------------------------

long long String::ToInt64( int base ) const
{
#ifdef __PCL_WINDOWS

   if ( data->string == 0 )
      throw ParseError( "Parsing 64-bit integer expression: empty string" );

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      throw ParseError( "Parsing 64-bit integer expression: empty string" );

   iterator p2 = SearchTrimRight();

   wchar_t* endptr = 0;
   errno = 0;

   long long val;

#ifdef _MSC_VER
   val = ::_wcstoi64( (wchar_t*)p1, &endptr, base );
#else
   size_type p;
   if ( (p = Find( '.' )) != notFound ||
        (p = Find( 'e' )) != notFound ||
        (p = Find( 'E' )) != notFound ||
        (p = Find( 'd' )) != notFound ||
        (p = Find( 'D' )) != notFound )
   {
      endptr = const_cast<wchar_t*>( (const wchar_t*)(Begin() + p) );
      val = 0;
   }
   else
      val = int64( ::wcstod( (wchar_t*)p1, &endptr ) );
#endif   // _MSC_VER

   if ( errno == ERANGE )
      throw ParseError( "Parsing 64-bit integer expression: out of the representable range", *this );

   if ( errno != 0 || (endptr != 0 && endptr != (wchar_t*)p2) )
      throw ParseError( "Parsing 64-bit integer expression: conversion error", *this,
                        (endptr != 0 && endptr != (wchar_t*)p2) ? endptr - (const wchar_t*)Begin() : -1 );

   return val;

#else

   return IsoString( *this ).ToInt64( base );

#endif   // __PCL_WINDOWS
}

bool String::TryToInt64( long long& value, int base ) const
{
#ifdef __PCL_WINDOWS

   if ( data->string == 0 )
      return false;

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      return false;

   iterator p2 = SearchTrimRight();

   wchar_t* endptr = 0;
   errno = 0;

   long long val;

#ifdef _MSC_VER
   val = ::_wcstoi64( (wchar_t*)p1, &endptr, base );
#else
   size_type p;
   if ( (p = Find( '.' )) != notFound ||
        (p = Find( 'e' )) != notFound ||
        (p = Find( 'E' )) != notFound ||
        (p = Find( 'd' )) != notFound ||
        (p = Find( 'D' )) != notFound )
   {
      return false;
   }

   val = int64( ::wcstod( (wchar_t*)p1, &endptr ) );
#endif   // _MSC_VER

   if ( errno == 0 && (endptr == 0 || endptr == (wchar_t*)p2) )
   {
      value = val;
      return true;
   }

   return false;

#else

   return IsoString( *this ).TryToInt64( value, base );

#endif   // __PCL_WINDOWS
}

// ----------------------------------------------------------------------------

unsigned long long String::ToUInt64( int base ) const
{
#ifdef __PCL_WINDOWS

   if ( data->string == 0 )
      throw ParseError( "Parsing 64-bit unsigned integer expression: empty string" );

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      throw ParseError( "Parsing 64-bit unsigned integer expression: empty string" );

   iterator p2 = SearchTrimRight();

   wchar_t* endptr = 0;
   errno = 0;

   unsigned long long val;

#ifdef _MSC_VER
   val = ::_wcstoui64( (wchar_t*)p1, &endptr, base );
#else
   size_type p;
   if ( (p = Find( '.' )) != notFound ||
        (p = Find( '-' )) != notFound ||
        (p = Find( 'e' )) != notFound ||
        (p = Find( 'E' )) != notFound ||
        (p = Find( 'd' )) != notFound ||
        (p = Find( 'D' )) != notFound )
   {
      endptr = const_cast<wchar_t*>( (const wchar_t*)(Begin() + p) );
      val = 0;
   }
   else
      val = uint64( ::wcstod( (wchar_t*)p1, &endptr ) );
#endif   // _MSC_VER

   if ( errno == ERANGE )
      throw ParseError( "Parsing 64-bit unsigned integer expression: out of the representable range", *this );

   if ( errno != 0 || (endptr != 0 && endptr != (wchar_t*)p2) )
      throw ParseError( "Parsing 64-bit unsigned integer expression: conversion error", *this,
                        (endptr != 0 && endptr != (wchar_t*)p2) ? endptr - (const wchar_t*)Begin() : -1 );

   return val;

#else

   return IsoString( *this ).ToUInt64( base );

#endif   // __PCL_WINDOWS
}

bool String::TryToUInt64( unsigned long long& value, int base ) const
{
#ifdef __PCL_WINDOWS

   if ( data->string == 0 )
      return false;

   iterator p1 = SearchTrimLeft();
   if ( char_traits::IsNull( *p1 ) )
      return false;

   iterator p2 = SearchTrimRight();

   wchar_t* endptr = 0;
   errno = 0;

   unsigned long long val;

#ifdef _MSC_VER
   val = ::_wcstoui64( (wchar_t*)p1, &endptr, base );
#else
   size_type p;
   if ( (p = Find( '.' )) != notFound ||
        (p = Find( '-' )) != notFound ||
        (p = Find( 'e' )) != notFound ||
        (p = Find( 'E' )) != notFound ||
        (p = Find( 'd' )) != notFound ||
        (p = Find( 'D' )) != notFound )
   {
      return false;
   }

   val = uint64( ::wcstod( (wchar_t*)p1, &endptr ) );
#endif   // _MSC_VER

   if ( errno == 0 && (endptr == 0 || endptr == (wchar_t*)p2) )
   {
      value = val;
      return true;
   }

   return false;

#else

   return IsoString( *this ).TryToUInt64( value, base );

#endif   // __PCL_WINDOWS
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/String.cpp - Released 2014/10/29 07:34:20 UTC
