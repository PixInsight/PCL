// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/CharTraits.h - Released 2014/10/29 07:34:13 UTC
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

#ifndef __PCL_CharTraits_h
#define __PCL_CharTraits_h

/// \file pcl/CharTraits.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_Utility_h
#include <pcl/Utility.h>
#endif

#ifndef __memory_h
#include <memory.h>
#ifndef __memory_h
#define __memory_h
#endif
#endif

#ifndef __string_h
#include <string.h>
#ifndef __string_h
#define __string_h
#endif
#endif

#ifndef __ctype_h
#include <ctype.h>
#ifndef __ctype_h
#define __ctype_h
#endif
#endif

#ifdef __PCL_WINDOWS

#ifndef __windows_h
#include <windows.h>
#ifndef __windows_h
#define __windows_h
#endif
#endif

#else // POSIX

#ifndef __wchar_h
#include <wchar.h>
#ifndef __wchar_h
#define __wchar_h
#endif
#endif

#ifndef __wctype_h
#include <wctype.h>
#ifndef __wctype_h
#define __wctype_h
#endif
#endif

#endif // !__PCL_WINDOWS

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup char_trait_classes Character Traits Classes
 *
 * A <em>character traits</em> class characterizes a data type to represent a
 * single element of a string, or \e character. For example, a char traits
 * class must provide primitive routines to copy, compare and classify
 * characters and sequences of characters. Those primitives are then used as
 * the building blocks of more complex structures such as character strings and
 * other text-oriented containers.
 */

// ----------------------------------------------------------------------------

/*!
 * \class GenericCharTraits
 * \brief Generic base class of character traits classes
 *
 * %GenericCharTraits defines fundamental properties and functionality common
 * to all character types.
 *
 * The purpose of a <em>character traits</em> class is to characterize a data
 * type to represent a single element of a string, or \e character. For
 * example, a char traits class must provide primitive routines to copy,
 * compare and classify characters and sequences of characters.
 *
 * %GenericCharTraits is a template class that must be instantiated for
 * suitable data types. Two instantiations of %GenericCharTraits, namely for
 * \c char and \c char16_type, originate the IsoString and String fundamental
 * PCL classes, respectively. The versatile interface provided by
 * %GenericCharTraits makes it possible to implement string classes virtually
 * for any data type with default and copy constructor semantics.
 *
 * \ingroup char_trait_classes
 * \sa CharTraits, IsoCharTraits, String, IsoString
 */
template <typename T>
class PCL_CLASS GenericCharTraits
{
public:

   /*!
    * Represents the character data type used by this traits class.
    */
   typedef T   char_type;

   /*!
    * Number of bytes per character.
    */
   static size_type BytesPerChar()
   {
      return sizeof( char_type );
   }

   /*!
    * Returns the length of a string in characters (\e not bytes).
    *
    * All PCL strings are null-terminated, so the computed length is the
    * length of the initial contiguous sequence of characters that are not
    * equal to Null().
    */
   static size_type Length( const T* s )
   {
      if ( s == 0 )
         return 0;
      for ( size_type n = 0; ; ++n, ++s )
         if ( *s == Null() )
            return n;
   }

   /*!
    * Fills a contiguous region of characters with a constant value.
    *
    * \param s    Initial address of a character sequence.
    * \param c    Constant value to fill with.
    * \param n    Number of characters to fill.
    */
   static void Fill( T* s, T c, size_type n )
   {
      PCL_PRECONDITION( s != 0 )
      for ( ; n > 0; --n )
         *s++ = c;
   }

   /*!
    * Copies a contiguous sequence of characters to a specified location. The
    * source and destination regions do not overlap.
    *
    * \param dst  Destination location where characters will be copied.
    * \param src  Initial address of the sequence of source characters.
    * \param n    Number of characters to copy.
    *
    * If the source and destination regions overlap, this routine will produce
    * an unpredictable result.
    */
   static void Copy( T* dst, const T* src, size_type n )
   {
      PCL_PRECONDITION( dst != 0 && src != 0 )
      for ( ; n > 0; --n )
         *dst++ = *src++;
   }

   /*!
    * Copies a contiguous sequence of characters to a specified location. The
    * source and destination regions may safely overlap.
    *
    * \param dst  Destination location where characters will be copied.
    * \param src  Initial address of the sequence of source characters.
    * \param n    Number of characters to copy.
    */
   static void CopyOverlapped( T* dst, const T* src, size_type n )
   {
      PCL_PRECONDITION( dst != 0 && src != 0 )
      if ( n > 0 )
         if ( dst < src || src+n <= dst )
            for ( ;; )
            {
               *dst++ = *src++;
               if ( --n == 0 )
                  break;
            }
         else
            for ( dst += n, src += n; ; )
            {
               *--dst = *--src;
               if ( --n == 0 )
                  break;
            }
   }

   /*!
    * Lexicographical comparison between two generic strings.
    *
    * \param s1   First string.
    * \param n1   Length of the first string in characters.
    * \param s2   Second string.
    * \param n2   Length of the second string in characters.
    *
    * \param caseSensitive    When true, a case-sensitive comparison is
    *                         performed; otherwise the comparison does not
    *                         distinguish between lowercase and uppercase
    *                         characters. The default value of this parameter
    *                         is true.
    *
    * \param localeAware      This parameter is ignored by GenericCharTraits.
    *                         For locale-aware string comparisons, see the
    *                         reimplementations of this static function in the
    *                         IsoCharTraits and CharTraits template class
    *                         instantiations.
    *
    * Returns an integer code representing the comparison result:
    * 0 if \a s1 and \a s2 are equal
    * -1 if \a s1 is less than \a s2
    * +1 if \a s1 is greater than \a s2
    *
    * This default implementation compares only the numerical values of string
    * characters, so it cannot be localized and can't be aware of user locale
    * settings. For case-insensitive comparisons, this routine is only valid
    * for the ISO/IEC 8859-1 character set: each character in the range 'A'-'Z'
    * is considered identical to its counterpart in the range 'a'-'z', and each
    * character in the range 0xC0-0xDF is identical to its counterpart in the
    * range 0xE0-0xFF. For more elaborated and locale-aware implementations of
    * this static function, see the IsoCharTraits and CharTraits classes.
    */
   static int Compare( const T* s1, size_type n1, const T* s2, size_type n2,
                       bool caseSensitive = true, bool localeAware = true )
   {
      PCL_PRECONDITION( s1 != 0 && s2 != 0 )
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
            T c1 = *s1, c2 = *s2;
            if ( c1 >= T( 'A' ) && c1 <= T( 'Z' ) || c1 >= T( 0xc0 ) && c1 <= T( 0xdf ) )
               c1 += 'a' - 'A';
            if ( c2 >= T( 'A' ) && c2 <= T( 'Z' ) || c2 >= T( 0xc0 ) && c2 <= T( 0xdf ) )
               c2 += 'a' - 'A';
            if ( c1 != c2 )
               return (c1 < c2) ? -1 : +1;
         }
      }
      return (n1 == n2) ? 0 : ((n1 < n2) ? -1 : +1);
   }

   /*!
    * Case-insensitive comparison between two generic strings.
    *
    * \param s1   First string
    * \param n1   Length of the first string in characters
    * \param s2   Second string
    * \param n2   Length of the second string in characters
    *
    * Returns an integer code representing the comparison result:
    * 0 if \a s1 and \a s2 are equal
    * -1 if \a s1 is less than \a s2
    * +1 if \a s1 is greater than \a s2
    *
    * \deprecated This function has been deprecated. It is kept as part of PCL
    * only to allow existing code to continue working. For newly produced code,
    * use the Compare() function instead by passing \a caseSensitive=false.
    */
   static int CompareIC( const T* s1, size_type n1, const T* s2, size_type n2 )
   {
      return Compare( s1, n1, s2, n2, false/*caseSensitive*/ );
   }

   /*!
    * Transforms a string to lower case.
    *
    * \note See the documentation for Compare() for more information on the
    * default character case comparisons.
    */
   static void ToLowerCase( T* s, size_type n )
   {
      PCL_PRECONDITION( s != 0 )
      for ( ; n > 0; --n, ++s )
         if ( *s >= T( 'A' ) && *s <= T( 'Z' ) || *s >= T( 0xc0 ) && *s <= T( 0xdf ) )
            *s += 'a' - 'A';
   }

   /*!
    * Transforms a string to upper case.
    *
    * \note See the documentation for Compare() for more information on the
    * default character case comparisons.
    */
   static void ToUpperCase( T* s, size_type n )
   {
      PCL_PRECONDITION( s != 0 )
      for ( ; n > 0; --n, ++s )
         if ( *s >= T( 'a' ) && *s <= T( 'z' ) || *s >= T( 0xe0 ) && *s <= T( 0xff ) )
            *s -= 'a' - 'A';
   }

   /*!
    * Termination null character
    */
   static T Null()
   {
      return T( 0 );
   }

   /*!
    * Blank space character (white space)
    */
   static T Blank()
   {
      return T( ' ' );
   }

   /*!
    * Horizontal tab
    */
   static T Tab()
   {
      return T( '\t' );
   }

   /*!
    * Carriage Return
    */
   static T CR()
   {
      return T( '\r' );
   }

   /*!
    * Line Feed
    */
   static T LF()
   {
      return T( '\n' );
   }

   /*!
    * Plus sign
    */
   static T PlusSign()
   {
      return T( '+' );
   }

   /*!
    * Minus sign
    */
   static T MinusSign()
   {
      return T( '-' );
   }

   /*!
    * Decimal separator
    */
   static T DecimalSeparator()
   {
      return T( '.' );
   }

   /*!
    * Exponent delimiter
    */
   static T ExponentDelimiter()
   {
      return T( 'e' );
   }

   /*!
    * Underscore
    */
   static T Underscore()
   {
      return T( '_' );
   }

   /*!
    * Returns true if a character \a c is a valid null terminator.
    */
   static bool IsNull( T c )
   {
      return c == Null();
   }

   /*!
    * Returns true if a character \a c is a white space character.
    */
   static bool IsSpace( T c )
   {
      return c == Blank() || c == Tab() || c == CR() || c == LF();
   }

   /*!
    * Returns true if a character \a c is a trimable character.
    */
   static bool IsTrimable( T c )
   {
      return IsSpace( c );
   }

   /*!
    * Returns true if a character \a c is a valid numerical decimal digit.
    */
   static bool IsDigit( T c )
   {
      return c >= T( '0' ) && c <= T( '9' );
   }

   /*!
    * Returns true if a character \a c is a valid numerical hexadecimal digit.
    */
   static bool IsHexDigit( T c )
   {
      return IsDigit( c ) || c >= T( 'A' ) && c <= T( 'F' ) ||
                             c >= T( 'a' ) && c <= T( 'f' );
   }

   /*!
    * Returns true if a character \a c is a valid symbol component.
    */
   static bool IsSymbolDigit( T c )
   {
      return c >= T( 'A' ) && c <= T( 'Z' ) ||
             c >= T( 'a' ) && c <= T( 'z' ) || c == Underscore();
   }

   /*!
    * Returns true if a character \a c is an underscore.
    */
   static bool IsUnderscore( T c )
   {
      return c == Underscore();
   }

   /*!
    * Returns true if a character \a c is a numerical sign.
    */
   static bool IsSign( T c )
   {
      return c == MinusSign() || c == PlusSign();
   }

   /*!
    * Returns true if a character \a c is a decimal separator.
    */
   static bool IsDecimalSeparator( T c )
   {
      return c == DecimalSeparator();
   }

   /*!
    * Returns true if a character \a c can be an exponent delimiter.
    */
   static bool IsExponentDelimiter( T c )
   {
      // We accept FORTRAN 'D'/'d' exponent delimiters.
      return c == T( 'E' ) || c == T( 'e' ) || c == T( 'D' ) || c == T( 'd' );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class IsoCharTraits
 * \brief A template instantiation of GenericCharTraits for the \c char type.
 *
 * %IsoCharTraits is the char traits class used by IsoString.
 *
 * \ingroup char_trait_classes
 */
class PCL_CLASS IsoCharTraits : public GenericCharTraits<char>
{
public:

   typedef GenericCharTraits<char>  traits_base;

   /*!
    * Returns the length of a string in characters (\e not bytes).
    *
    * All PCL strings are null-terminated, so the computed length is the
    * length of the initial contiguous sequence of characters that are not
    * equal to Null().
    */
   static size_type Length( const char* s )
   {
      return (s != 0) ? ::strlen( s ) : 0;
   }

   /*!
    * Fills a contiguous region of characters with a constant value.
    *
    * \param s    Initial address of a character sequence.
    * \param c    Constant value to fill with.
    * \param n    Number of characters to fill.
    */
   static void Fill( char* s, char c, size_type n )
   {
      PCL_PRECONDITION( s != 0 )
      ::memset( s, c, n );
   }

   /*!
    * Copies a contiguous sequence of characters to a specified location. The
    * source and destination regions do not overlap.
    *
    * \param dst  Destination location where characters will be copied.
    * \param src  Initial address of the sequence of source characters.
    * \param n    Number of characters to copy.
    *
    * If the source and destination regions overlap, this routine will produce
    * an unpredictable result.
    */
   static void Copy( char* dst, const char* src, size_type n )
   {
      PCL_PRECONDITION( dst != 0 && src != 0 )
      ::memcpy( dst, src, n );
   }

   /*!
    * Copies a contiguous sequence of characters to a specified location. The
    * source and destination regions may safely overlap.
    *
    * \param dst  Destination location where characters will be copied.
    * \param src  Initial address of the sequence of source characters.
    * \param n    Number of characters to copy.
    */
   static void CopyOverlapped( char* dst, const char* src, size_type n )
   {
      PCL_PRECONDITION( dst != 0 && src != 0 )
      ::memmove( dst, src, n );
   }

   /*!
    * Lexicographical comparison between two 8-bit strings.
    *
    * \param s1   First string.
    * \param n1   Length of the first string in characters.
    * \param s2   Second string.
    * \param n2   Length of the second string in characters.
    *
    * \param caseSensitive    When true, a case-sensitive comparison is
    *                         performed; otherwise the comparison does not
    *                         distinguish between lowercase and uppercase
    *                         characters (as defined by the selected locale).
    *                         The default value of this parameter is true.
    *
    * \param localeAware      When true, a locale-aware comparison is
    *                         performed which takes into account the currently
    *                         selected user locale (language and variants).
    *                         When false, an invariant comparison is carried
    *                         out by comparing Unicode code points (which is
    *                         faster). The default value is true.
    *
    * Returns an integer code representing the comparison result:
    * 0 if \a s1 and \a s2 are equal
    * -1 if \a s1 is less than \a s2
    * +1 if \a s1 is greater than \a s2
    *
    * On X11 (FreeBSD and Linux platforms), when the \a localeAware
    * parameter is true, the comparison uses the current locale as reported by
    * calling the setlocale POSIX function:
    *
    * \code
    * setlocale( LC_COLLATE, 0 );
    * \endcode
    *
    * In PixInsight on X11 platforms, the default user collation and case
    * comparison locales are used. A module can change them \e temporarily to
    * a custom locale by calling:
    *
    * \code
    * setlocale( LC_COLLATE, "<custom-locale>" );
    * setlocale( LC_CTYPE, "<custom-locale>" );
    * \endcode
    *
    * and then, after the necessary work has been done with the custom locales,
    * restore the default settings:
    *
    * \code
    * setlocale( LC_COLLATE, "" );
    * setlocale( LC_CTYPE, "" );
    * \endcode
    *
    * Failure to restore the default platform locale on X11 is considered bad
    * practice and disqualifies a module for certification.
    */
   static int Compare( const char* s1, size_type n1,
                       const char* s2, size_type n2, bool caseSensitive = true, bool localeAware = true );

   /*!
    * Case-insensitive comparison between two 8-bit strings.
    *
    * \param s1   First string
    * \param n1   Length of the first string in characters
    * \param s2   Second string
    * \param n2   Length of the second string in characters
    *
    * Returns an integer code representing the comparison result:
    * 0 if \a s1 and \a s2 are equal
    * -1 if \a s1 is less than \a s2
    * +1 if \a s1 is greater than \a s2
    *
    * \deprecated This function has been deprecated. It is kept as part of PCL
    * only to allow existing code to continue working. For newly produced code,
    * use the Compare() function instead by passing \a caseSensitive=false.
    */
   static int CompareIC( const char* s1, size_type n1, const char* s2, size_type n2 )
   {
      return Compare( s1, n1, s2, n2, false/*caseSensitive*/ );
   }

   /*!
    * Transforms a string to lower case.
    */
   static void ToLowerCase( char* s, size_type n )
   {
      PCL_PRECONDITION( s != 0 )
#ifdef __PCL_WINDOWS
      ::CharLowerBuffA( s, DWORD( n ) );
#else
      for ( ; n > 0 && *s != '\0'; --n, ++s )
         if ( ::isupper( *s ) )
            *s = ::tolower( *s );
#endif
   }

   /*!
    * Transforms a string to upper case.
    */
   static void ToUpperCase( char* s, size_type n )
   {
      PCL_PRECONDITION( s != 0 )
#ifdef __PCL_WINDOWS
      ::CharUpperBuffA( s, DWORD( n ) );
#else
      for ( ; n > 0 && *s != '\0'; --n, ++s )
         if ( ::islower( *s ) )
            *s = ::toupper( *s );
#endif
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class CharTraits
 * \brief A template instantiation of GenericCharTraits for \c char16_type.
 *
 * %CharTraits is the char traits class used by String.
 *
 * \ingroup char_trait_classes
 */
class PCL_CLASS CharTraits : public GenericCharTraits<char16_type>
{
public:

   typedef GenericCharTraits<char16_type>    traits_base;

   /*!
    * Returns the length of a string in characters (\e not bytes).
    *
    * All PCL strings are null-terminated, so the computed length is the
    * length of the initial contiguous sequence of characters that are not
    * equal to Null().
    */
   static size_type Length( const char16_type* s )
   {
#ifdef __PCL_WINDOWS
      return (s != 0) ? ::wcslen( reinterpret_cast<const wchar_t*>( s ) ) : 0;
#else
      return traits_base::Length( s );
#endif
   }

   /*!
    * Copies a contiguous sequence of characters to a specified location. The
    * source and destination regions do not overlap.
    *
    * \param dst  Destination location where characters will be copied.
    * \param src  Initial address of the sequence of source characters.
    * \param n    Number of characters to copy.
    *
    * If the source and destination regions overlap, this routine will produce
    * an unpredictable result.
    */
   static void Copy( char16_type* dst, const char16_type* src, size_type n )
   {
      PCL_PRECONDITION( dst != 0 && src != 0 )
      ::memcpy( dst, src, n << 1 );
   }

   /*!
    * Copies a contiguous sequence of characters to a specified location. The
    * source and destination regions may safely overlap.
    *
    * \param dst  Destination location where characters will be copied.
    * \param src  Initial address of the sequence of source characters.
    * \param n    Number of characters to copy.
    */
   static void CopyOverlapped( char16_type* dst, const char16_type* src, size_type n )
   {
      PCL_PRECONDITION( dst != 0 && src != 0 )
      ::memmove( dst, src, n << 1 );
   }

   /*!
    * Lexicographical comparison between two Unicode strings.
    *
    * \param s1   First string.
    * \param n1   Length of the first string in characters.
    * \param s2   Second string.
    * \param n2   Length of the second string in characters.
    *
    * \param caseSensitive    When true, a case-sensitive comparison is
    *                         performed; otherwise the comparison does not
    *                         distinguish between lowercase and uppercase
    *                         characters (as defined by the selected locale).
    *                         The default value of this parameter is true.
    *
    * \param localeAware      When true, a locale-aware comparison is
    *                         performed which takes into account the currently
    *                         selected user locale (language and variants).
    *                         When false, an invariant comparison is carried
    *                         out by comparing Unicode code points (which is
    *                         faster). The default value is true.
    *
    * Returns an integer code representing the comparison result:
    * 0 if \a s1 and \a s2 are equal
    * -1 if \a s1 is less than \a s2
    * +1 if \a s1 is greater than \a s2
    *
    * On X11 (FreeBSD and Linux platforms), when the \a localeAware
    * parameter is true, the comparison uses the current locale as reported by
    * calling the setlocale POSIX function:
    *
    * \code
    * setlocale( LC_COLLATE, 0 );
    * \endcode
    *
    * In PixInsight on X11 platforms, the default user collation and case
    * comparison locales are used. A module can change them \e temporarily to
    * a custom locale by calling:
    *
    * \code
    * setlocale( LC_COLLATE, "<custom-locale>" );
    * setlocale( LC_CTYPE, "<custom-locale>" );
    * \endcode
    *
    * and then, after the necessary work has been done with the custom locales,
    * restore the default settings:
    *
    * \code
    * setlocale( LC_COLLATE, "" );
    * setlocale( LC_CTYPE, "" );
    * \endcode
    *
    * Failure to restore the default platform locale on X11 is considered bad
    * practice and disqualifies a module for certification.
    */
   static int Compare( const char16_type* s1, size_type n1,
                       const char16_type* s2, size_type n2, bool caseSensitive = true, bool localeAware = true );

   /*!
    * Case-insensitive comparison between two Unicode strings.
    *
    * \param s1   First string
    * \param n1   Length of the first string in characters
    * \param s2   Second string
    * \param n2   Length of the second string in characters
    *
    * Returns an integer code representing the comparison result:
    * 0 if \a s1 and \a s2 are equal
    * -1 if \a s1 is less than \a s2
    * +1 if \a s1 is greater than \a s2
    *
    * \deprecated This function has been deprecated. It is kept as part of PCL
    * only to allow existing code to continue working. For newly produced code,
    * use the Compare() function instead by passing \a caseSensitive=false.
    */
   static int CompareIC( const char16_type* s1, size_type n1, const char16_type* s2, size_type n2 )
   {
      return Compare( s1, n1, s2, n2, false/*caseSensitive*/ );
   }

   /*!
    * Transforms a string to lower case.
    */
   static void ToLowerCase( char16_type* s, size_type n )
   {
      PCL_PRECONDITION( s != 0 )
#ifdef __PCL_WINDOWS
      ::CharLowerBuffW( (LPWSTR)s, DWORD( n ) );
#else
      for ( ; n > 0 && *s != 0; --n, ++s )
      {
         wchar_t w( *s );
         if ( ::iswupper( w ) )
            *s = char16_type( ::towlower( w ) );
      }
#endif
   }

   /*!
    * Transforms a string to upper case.
    */
   static void ToUpperCase( char16_type* s, size_type n )
   {
      PCL_PRECONDITION( s != 0 )
#ifdef __PCL_WINDOWS
      ::CharUpperBuffW( (LPWSTR)s, DWORD( n ) );
#else
      for ( ; n > 0 && *s != 0; --n, ++s )
      {
         wchar_t w( *s );
         if ( ::iswlower( w ) )
            *s = char16_type( ::towupper( w ) );
      }
#endif
   }

   /*!
    * Returns true if the specified UTF-16 character is a <em>high
    * surrogate</em> (the most significant word of a <em>surrogate pair</em>
    * forming a UTF-32 code point). High surrogates have values between 0xD800
    * and 0xDBFF.
    */
   static bool IsHighSurrogate( char16_type c16 )
   {
      return (c16 & 0xFC00) == 0xD800;
   }

   /*!
    * Returns the <em>high surrogate</em> word of a UTF-32 code point. The
    * specified UTF-32 code must be in the range from 0x010000 to 0x10FFFF,
    * since surrogates only exist outside the Basic Multilingual Plane of
    * Unicode.
    */
   static char16_type HighSurrogate( char32_type c32 )
   {
      return char16_type( (c32 >> 10) + 0xD7C0 );
   }

   /*!
    * Returns true if the specified UTF-16 character is a <em>low
    * surrogate</em> (the least significant word of a <em>surrogate pair</em>
    * forming a UTF-32 code point). Low surrogates have values between 0xDC00
    * and 0xDFFF.
    */
   static bool IsLowSurrogate( char16_type c16 )
   {
      return (c16 & 0xFC00) == 0xDC00;
   }

   /*!
    * Returns the <em>low surrogate</em> word of a UTF-32 code point. The
    * specified UTF-32 code must be in the range from 0x010000 to 0x10FFFF,
    * since surrogates only exist outside the Basic Multilingual Plane of
    * Unicode.
    */
   static char16_type LowSurrogate( char32_type c32 )
   {
      return char16_type( (c32%0x400) + 0xDC00 );
   }

   /*!
    * Returns a UTF-32 code point from its <em>surrogate pair</em>. The
    * specified surrogate words must pertain to a valid Unicode code point
    * outside the Basic Multilingual Plane (from 0x010000 to 0x10FFFF).
    */
   static char32_type SurrogatePairToUTF32( char16_type high, char16_type low )
   {
      return (char32_type( high ) << 10) + low - 0x035FDC00;
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_CharTraits_h

// ****************************************************************************
// EOF pcl/CharTraits.h - Released 2014/10/29 07:34:13 UTC
