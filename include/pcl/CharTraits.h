//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/CharTraits.h - Released 2017-08-01T14:23:31Z
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

#ifndef __PCL_CharTraits_h
#define __PCL_CharTraits_h

/// \file pcl/CharTraits.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Utility.h>

#include <ctype.h>
#include <memory.h>
#include <string.h>

#ifdef __PCL_WINDOWS
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#else // POSIX
#  include <wchar.h>
#  include <wctype.h>
#endif // !__PCL_WINDOWS

namespace pcl
{

// ----------------------------------------------------------------------------

char16_type PCL_FUNC PCL_ToCaseFolded( char16_type );
char16_type PCL_FUNC PCL_ToLowercase( char16_type );
char16_type PCL_FUNC PCL_ToUppercase( char16_type );

extern const uint8* PCL_DATA PCL_toLowercaseLatin1;
extern const uint8* PCL_DATA PCL_toUppercaseLatin1;

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

#define PCL_COMPARE_CODE_POINTS()                                             \
   PCL_PRECONDITION( n1 == 0 || n2 == 0 || s1 != nullptr && s2 != nullptr )   \
   if ( caseSensitive )                                                       \
   {                                                                          \
      for ( size_type n = pcl::Min( n1, n2 ); n > 0; --n, ++s1, ++s2 )        \
         if ( *s1 != *s2 )                                                    \
            return (*s1 < *s2) ? -1 : +1;                                     \
   }                                                                          \
   else                                                                       \
   {                                                                          \
      for ( size_type n = pcl::Min( n1, n2 ); n > 0; --n, ++s1, ++s2 )        \
      {                                                                       \
         char_type c1 = ToCaseFolded( *s1 ), c2 = ToCaseFolded( *s2 );        \
         if ( c1 != c2 )                                                      \
            return (c1 < c2) ? -1 : +1;                                       \
      }                                                                       \
   }                                                                          \
   return (n1 == n2) ? 0 : ((n1 < n2) ? -1 : +1)

// ----------------------------------------------------------------------------

/*
 * Wildcard string matching algorithm adapted from:
 *
 * Kirk J. Krauss (2014): Matching Wildcards: An Empirical Way to Tame an
 * Algorithm, Dr. Dobb's Magazine, October 7, 2014.
 *
 * http://www.drdobbs.com/architecture-and-design/matching-wildcards-an-empirical-way-to-t/240169123
 */

template <typename Tt, typename Tp> inline
bool WildMatch( const Tt* t, size_type nt, const Tp* p, size_type np )
{
   PCL_PRECONDITION( nt == 0 || np == 0 || t != nullptr && p != nullptr )

   if ( nt == 0 || np == 0 )
      return false;

   const Tt* et = t + nt;
   const Tp* ep = p + np;
   const Tt* bt = nullptr;
   const Tp* bp = nullptr;

   for ( ;; )
   {
      Tp c = *p;

      if ( c == Tp( '*' ) )
      {
         do
            if ( ++p == ep )
               return true;
         while ( (c = *p) == Tp( '*' ) );

         if ( c != Tp( '?' ) )
            while ( Tt( c ) != *t )
               if ( ++t == et )
                  return false;

         bp = p;
         bt = t;
      }
      else if ( Tt( c ) != *t && c != Tp( '?' ) )
      {
         if ( bp != nullptr )
         {
            if ( p != bp )
            {
               p = bp;

               if ( Tt( *p ) != *t )
               {
                  t = ++bt;
                  continue;
               }

               ++p;
            }

            if ( t < et )
            {
               ++t;
               continue;
            }
         }

         return false;
      }

      ++t;
      ++p;

      if ( t == et )
      {
         if ( p == ep )
            return true;

         while ( *p == Tp( '*' ) )
            if ( ++p == ep )
               return true;

         return false;
      }
   }
}

template <typename Tt, typename Tp, class Ut, class Up> inline
bool WildMatchIC( const Tt* t, size_type nt, const Tp* p, size_type np, Ut ut, Up up )
{
   PCL_PRECONDITION( nt == 0 || np == 0 || t != nullptr && p != nullptr && ut != nullptr && up != nullptr )

   if ( nt == 0 || np == 0 )
      return false;

   const Tt* et = t + nt;
   const Tp* ep = p + np;
   const Tt* bt = nullptr;
   const Tp* bp = nullptr;

   for ( ;; )
   {
      Tp c = *p;

      if ( c == Tp( '*' ) )
      {
         do
            if ( ++p == ep )
               return true;
         while ( (c = *p) == Tp( '*' ) );

         if ( c != Tp( '?' ) )
         {
            c = up( c );
            while ( Tt( c ) != ut( *t ) )
               if ( ++t == et )
                  return false;
         }

         bp = p;
         bt = t;
      }
      else if ( c != Tp( '?' ) )
      {
         Tt ft = ut( *t );

         if ( Tt( up( c ) ) != ft )
         {
            if ( bp != nullptr )
            {
               if ( p != bp )
               {
                  p = bp;

                  if ( Tt( up( *p ) ) != ft )
                  {
                     t = ++bt;
                     continue;
                  }

                  ++p;
               }

               if ( t < et )
               {
                  ++t;
                  continue;
               }
            }

            return false;
         }
      }

      ++t;
      ++p;

      if ( t == et )
      {
         if ( p == ep )
            return true;

         while ( *p == Tp( '*' ) )
            if ( ++p == ep )
               return true;

         return false;
      }
   }
}

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
   static constexpr size_type BytesPerChar()
   {
      return sizeof( char_type );
   }

   /*!
    * Returns the length of a null-terminated string in characters (\e not
    * bytes).
    *
    * The returned value is the length of the initial contiguous sequence of
    * characters that are not equal to Null().
    */
   static size_type Length( const char_type* s )
   {
      const char_type* t = s;
      if ( s != nullptr )
         for ( ; *t != Null(); ++t ) {}
      return size_type( t - s );
   }

   /*!
    * Fills a contiguous region of characters with a constant value.
    *
    * \param s    Initial address of a character sequence.
    * \param c    Constant value to fill with.
    * \param n    Number of characters to fill.
    */
   static void Fill( char_type* s, char_type c, size_type n )
   {
      PCL_PRECONDITION( n == 0 || s != nullptr )
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
    * an unpredictable result. CopyOverlapped() should be used in these cases.
    */
   static void Copy( char_type* dst, const char_type* src, size_type n )
   {
      PCL_PRECONDITION( n == 0 || dst != nullptr && src != nullptr )
      ::memcpy( dst, src, n*sizeof( char_type ) );
   }

   /*!
    * Copies a contiguous sequence of characters to a specified location. The
    * source and destination regions may safely overlap.
    *
    * \param dst  Destination location where characters will be copied.
    * \param src  Initial address of the sequence of source characters.
    * \param n    Number of characters to copy.
    */
   static void CopyOverlapped( char_type* dst, const char_type* src, size_type n )
   {
      PCL_PRECONDITION( n == 0 || dst != nullptr && src != nullptr )
      ::memmove( dst, src, n*sizeof( char_type ) );
   }

   /*!
    * Compares numeric character values between two strings.
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
    * Returns an integer code representing the comparison result:
    *
    * \li  0 if \a s1 and \a s2 are equal
    * \li -1 if \a s1 is less than \a s2
    * \li +1 if \a s1 is greater than \a s2
    *
    * This function compares the numerical values of string characters. For
    * case-insensitive comparisons, this generic routine is only valid for the
    * ISO/IEC 8859-1 character set: characters in the ranges 0x41-0x5A,
    * 0xC0-0xD6 and 0xD8-0xDE are considered identical to its counterparts in
    * the ranges 0x61-0x7A, 0xE0-0xF6 and 0xF8-0xFE, respectively.
    */
   static int CompareCodePoints( const char_type* s1, size_type n1,
                                 const char_type* s2, size_type n2, bool caseSensitive = true )
   {
      PCL_COMPARE_CODE_POINTS();
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
    *
    * \li  0 if \a s1 and \a s2 are equal
    * \li -1 if \a s1 is less than \a s2
    * \li +1 if \a s1 is greater than \a s2
    *
    * This default implementation simply calls CompareCodePoints() to compare
    * the numerical values of string characters, so it cannot be localized and
    * can't be aware of user locale settings. For more comprehensive,
    * locale-aware Unicode implementations of this static function, see the
    * IsoCharTraits and CharTraits classes.
    */
   static int Compare( const char_type* s1, size_type n1,
                       const char_type* s2, size_type n2, bool caseSensitive = true, bool localeAware = true )
   {
      return CompareCodePoints( s1, n1, s2, n2, caseSensitive );
   }

   /*!
    * Wildcard string matching algorithm.
    *
    * \param t    The string to be matched.
    *
    * \param nt   Length of the string to be matched in characters.
    *
    * \param p    The pattern string. May contain multiple instances of the
    *             wildcard characters '*' and '?'.
    *
    * \param np   Length of the pattern string in characters.
    *
    * \param caseSensitive    When true, a case-sensitive comparison is
    *                         performed; otherwise the comparison does not
    *                         distinguish between lowercase and uppercase
    *                         characters. The default value of this parameter
    *                         is true.
    *
    * Returns true iff the string \a t matches the specified pattern \a p. If
    * one of the strings is empty, this function always returns false
    * conventionally, even if the pattern is a single asterisk '*'.
    */
   static bool WildMatch( const char_type* t, size_type nt,
                          const char_type* p, size_type np, bool caseSensitive = true )
   {
      if ( caseSensitive )
         return pcl::WildMatch( t, nt, p, np );
      return pcl::WildMatchIC( t, nt, p, np, ToCaseFolded, ToCaseFolded );
   }

   /*!
    * Returns the \e case \e folded equivalent character for the specified
    * code point \a c.
    *
    * Case folding elimitates case differences, which is useful for
    * case-insensitive string comparisons.
    *
    * This default implementation is only valid for the ISO/IEC-8859-1
    * character set. For a comprehensive Unicode implementation see the
    * CharTraits class.
    */
   static char_type ToCaseFolded( char_type c )
   {
      return ToLowercase( c );
   }

   /*!
    * Returns the lowercase equivalent character for the specified code point.
    *
    * This default implementation is only valid for the ISO/IEC-8859-1
    * character set. For a comprehensive Unicode implementation see the
    * CharTraits class.
    */
   static constexpr char_type ToLowercase( char_type c )
   {
      return (c >= char_type(  65 ) && c <= char_type(  90 )
           || c >= char_type( 192 ) && c <= char_type( 214 )
           || c >= char_type( 216 ) && c <= char_type( 222 )) ? c + 32 : c;
   }

   /*!
    * Returns the uppercase equivalent character for the specified code point.
    *
    * This default implementation is only valid for the ISO/IEC-8859-1
    * character set. For a comprehensive Unicode implementation see the
    * CharTraits class.
    */
   static constexpr char_type ToUppercase( char_type c )
   {
      return (c >= char_type(  97 ) && c <= char_type( 122 )
           || c >= char_type( 224 ) && c <= char_type( 246 )
           || c >= char_type( 248 ) && c <= char_type( 254 )) ? c - 32 : c;
   }

   /*!
    * Transforms a string to lower case.
    *
    * This default implementation is only valid for the ISO/IEC-8859-1
    * character set. For a comprehensive Unicode implementation see the
    * CharTraits class.
    */
   static void ToLowercase( char_type* s, size_type n )
   {
      PCL_PRECONDITION( n == 0 || s != nullptr )
      for ( ; n > 0; --n, ++s )
         *s = ToLowercase( *s );
   }

   /*!
    * Transforms a string to upper case.
    *
    * This default implementation is only valid for the ISO/IEC-8859-1
    * character set. For a comprehensive Unicode implementation see the
    * CharTraits class.
    */
   static void ToUppercase( char_type* s, size_type n )
   {
      PCL_PRECONDITION( n == 0 || s != nullptr )
      for ( ; n > 0; --n, ++s )
         *s = ToUppercase( *s );
   }

   /*!
    * Returns the null string termination character '\\0'.
    */
   static constexpr char_type Null()
   {
      return char_type( 0 );
   }

   /*!
    * Returns the blank space character (white space).
    */
   static constexpr char_type Blank()
   {
      return char_type( ' ' );
   }

   /*!
    * Returns the horizontal tab control character '\\t'.
    */
   static constexpr char_type Tab()
   {
      return char_type( '\t' );
   }

   /*!
    * Returns the carriage return control character '\\r'.
    */
   static constexpr char_type CR()
   {
      return char_type( '\r' );
   }

   /*!
    * Returns the line feed control character '\\n'.
    */
   static constexpr char_type LF()
   {
      return char_type( '\n' );
   }

   /*!
    * Returns the comma punctuator character ','.
    */
   static constexpr char_type Comma()
   {
      return char_type( ',' );
   }

   /*!
    * Returns the colon punctuator character ':'.
    */
   static constexpr char_type Colon()
   {
      return char_type( ':' );
   }

   /*!
    * Returns the semicolon punctuator character ';'.
    */
   static constexpr char_type Semicolon()
   {
      return char_type( ';' );
   }

   /*!
    * Returns the hyphen punctuator character '-'.
    */
   static constexpr char_type Hyphen()
   {
      return char_type( '-' );
   }

   /*!
    * Returns the plus sign character '+'.
    */
   static constexpr char_type PlusSign()
   {
      return char_type( '+' );
   }

   /*!
    * Returns the minus sign character '-'.
    */
   static constexpr char_type MinusSign()
   {
      return char_type( '-' );
   }

   /*!
    * Returns the decimal separator character '.'.
    */
   static constexpr char_type DecimalSeparator()
   {
      return char_type( '.' );
   }

   /*!
    * Returns the exponent delimiter character 'e'.
    */
   static constexpr char_type ExponentDelimiter()
   {
      return char_type( 'e' );
   }

   /*!
    * Returns the underscore character '_'.
    */
   static constexpr char_type Underscore()
   {
      return char_type( '_' );
   }

   /*!
    * Returns the single quote character "'".
    */
   static constexpr char_type SingleQuote()
   {
      return char_type( '\'' );
   }

   /*!
    * Returns the double quote character '"'.
    */
   static constexpr char_type DoubleQuote()
   {
      return char_type( '\"' );
   }

   /*!
    * Returns true iff a character \a c is a null string terminator.
    */
   static constexpr bool IsNull( char_type c )
   {
      return c == Null();
   }

   /*!
    * Returns true iff a character \a c is a white space character.
    */
   static constexpr bool IsSpace( char_type c )
   {
      return c == Blank() || c == Tab() || c == CR() || c == LF();
   }

   /*!
    * Returns true iff a character \a c is a trimable character. Generally
    * equivalent to IsSpace().
    */
   static constexpr bool IsTrimable( char_type c )
   {
      return IsSpace( c );
   }

   /*!
    * Returns true iff a character \a c is a decimal digit. Decimal digits are
    * in the range [0-9].
    */
   static constexpr bool IsDigit( char_type c )
   {
      return c >= char_type( '0' ) && c <= char_type( '9' );
   }

   /*!
    * Returns true iff a character \a c is an hexadecimal digit. Hexadecimal
    * digits are in the range [a-fA-F].
    */
   static constexpr bool IsHexDigit( char_type c )
   {
      return IsDigit( c ) || c >= char_type( 'A' ) && c <= char_type( 'F' ) ||
                             c >= char_type( 'a' ) && c <= char_type( 'f' );
   }

   /*!
    * Returns true iff a character \a c is in the range [a-zA-Z].
    */
   static constexpr bool IsAlpha( char_type c )
   {
      return IsLowercaseAlpha( c ) || IsUppercaseAlpha( c );
   }

   /*!
    * Returns true iff a character \a c is in the range [a-z].
    */
   static constexpr bool IsLowercaseAlpha( char_type c )
   {
      return c >= char_type( 'a' ) && c <= char_type( 'z' );
   }

   /*!
    * Returns true iff a character \a c is in the range [A-Z].
    */
   static constexpr bool IsUppercaseAlpha( char_type c )
   {
      return c >= char_type( 'A' ) && c <= char_type( 'Z' );
   }

   /*!
    * Returns true iff a character \a c is the underscore character '_'.
    */
   static constexpr bool IsUnderscore( char_type c )
   {
      return c == Underscore();
   }

   /*!
    * Returns true iff a character \a c is a valid symbol element. Symbol digits
    * are in the range [a-zA-Z0-9_].
    */
   static constexpr bool IsSymbolDigit( char_type c )
   {
      return IsAlpha( c ) || IsDigit( c ) || IsUnderscore( c );
   }

   /*!
    * Returns true iff a character \a c is a valid starting symbol digit. A
    * starting symbol digit is in the range [a-zA-Z_].
    */
   static constexpr bool IsStartingSymbolDigit( char_type c )
   {
      return IsAlpha( c ) || IsUnderscore( c );
   }

   /*!
    * Returns true iff a character \a c is a numerical sign, either '+' or '-'.
    */
   static constexpr bool IsSign( char_type c )
   {
      return c == MinusSign() || c == PlusSign();
   }

   /*!
    * Returns true iff a character \a c is the decimal separator '.'.
    */
   static constexpr bool IsDecimalSeparator( char_type c )
   {
      return c == DecimalSeparator();
   }

   /*!
    * Returns true iff a character \a c is an exponent delimiter. Exponent
    * delimiters are in the range [eEdD]. The [dD] pair allows for FORTRAN
    * compatibility.
    */
   static constexpr bool IsExponentDelimiter( char_type c )
   {
      return c == char_type( 'e' ) || c == char_type( 'E' ) || c == char_type( 'd' ) || c == char_type( 'D' );
   }

   /*!
    * Returns true iff a character \a c is a wildcard. The wildcards are the
    * asterisk '*' and question mark '?' characters.
    */
   static constexpr bool IsWildcard( char_type c )
   {
      return c == char_type( '*' ) || c == char_type( '?' );
   }

   /*!
    * Returns a pointer to the first non-trimmable character in the sequence of
    * contiguous characters defined by the range [i,j) of pointers.
    */
   template <typename Ptr1, typename Ptr2>
   static Ptr1 SearchTrimLeft( Ptr1 i, Ptr2 j )
   {
      for ( ; i < j && IsTrimable( *i ); ++i ) {}
      return i;
   }

   /*!
    * Returns a pointer to the character \e after the last non-trimmable
    * character in the sequence of contiguous characters defined by the range
    * [i,j) of pointers.
    *
    * If there are no trimmable characters in the specified sequence, this
    * function returns the ending pointer \a j.
    */
   template <typename Ptr1, typename Ptr2>
   static Ptr2 SearchTrimRight( Ptr1 i, Ptr2 j )
   {
      for ( ; i < j && IsTrimable( *(j-1) ); --j ) {}
      return j;
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

   /*!
    * Base class of this char traits class.
    */
   typedef GenericCharTraits<char>  traits_base;

   /*!
    * Represents the character data type used by this traits class.
    */
   typedef traits_base::char_type   char_type;

   /*!
    * Returns the length of a null-terminated 8-bit string in characters
    * (\e not bytes).
    *
    * The returned value is the length of the initial contiguous sequence of
    * characters that are not equal to Null().
    */
   static constexpr size_type Length( const char_type* s )
   {
      return (s != nullptr) ? ::strlen( s ) : 0;
   }

   /*!
    * Fills a contiguous region of characters with a constant value.
    *
    * \param s    Initial address of a character sequence.
    * \param c    Constant value to fill with.
    * \param n    Number of characters to fill.
    */
   static void Fill( char_type* s, char_type c, size_type n )
   {
      PCL_PRECONDITION( n == 0 || s != nullptr )
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
    * an unpredictable result. CopyOverlapped() should be used in these cases.
    */
   static void Copy( char_type* dst, const char_type* src, size_type n )
   {
      PCL_PRECONDITION( n == 0 || dst != nullptr && src != nullptr )
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
   static void CopyOverlapped( char_type* dst, const char_type* src, size_type n )
   {
      PCL_PRECONDITION( n == 0 || dst != nullptr && src != nullptr )
      ::memmove( dst, src, n );
   }

   /*!
    * Compares numeric character values between two 8-bit strings.
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
    * Returns an integer code representing the comparison result:
    *
    * \li  0 if \a s1 and \a s2 are equal
    * \li -1 if \a s1 is less than \a s2
    * \li +1 if \a s1 is greater than \a s2
    */
   static int CompareCodePoints( const char_type* s1, size_type n1,
                                 const char_type* s2, size_type n2, bool caseSensitive = true )
   {
      PCL_COMPARE_CODE_POINTS();
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
    *                         characters (as defined by the current locale).
    *                         The default value of this parameter is true.
    *
    * \param localeAware      When true, a locale-aware comparison is
    *                         performed which takes into account the currently
    *                         selected user locale (language and variants).
    *                         When false, an invariant comparison is carried
    *                         out by comparing Unicode code points (which may
    *                         be faster). The default value is true.
    *
    * Returns an integer code representing the comparison result:
    *
    * \li  0 if \a s1 and \a s2 are equal
    * \li -1 if \a s1 is less than \a s2
    * \li +1 if \a s1 is greater than \a s2
    *
    * On OS X and Windows platforms this function invokes system API routines
    * to perform locale-aware string comparisons.
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
    */
   static int Compare( const char_type* s1, size_type n1,
                       const char_type* s2, size_type n2, bool caseSensitive = true, bool localeAware = true );

   /*!
    * Wildcard string matching algorithm.
    *
    * \param t    The string to be matched.
    *
    * \param nt   Length of the string to be matched in characters.
    *
    * \param p    The pattern string. May contain multiple instances of the
    *             wildcard characters '*' and '?'.
    *
    * \param np   Length of the pattern string in characters.
    *
    * \param caseSensitive    When true, a case-sensitive comparison is
    *                         performed; otherwise the comparison does not
    *                         distinguish between lowercase and uppercase
    *                         characters. The default value of this parameter
    *                         is true.
    *
    * Returns true iff the string \a t matches the specified pattern \a p. If
    * one of the strings is empty, this function always returns false
    * conventionally, even if the pattern is a single asterisk '*'.
    */
   static bool WildMatch( const char_type* t, size_type nt,
                          const char_type* p, size_type np, bool caseSensitive = true )
   {
      if ( caseSensitive )
         return pcl::WildMatch( t, nt, p, np );
      return pcl::WildMatchIC( t, nt, p, np,
                               []( char_type c ) { return ToCaseFolded( c ); },
                               []( char_type c ) { return ToCaseFolded( c ); } );
   }

   /*!
    * Returns the \e case \e folded equivalent character for the specified
    * ISO/IEC-8859-1 code point \a c.
    *
    * Case folding elimitates case differences, which is useful for
    * case-insensitive string comparisons.
    *
    * For more information on case folding, see Section 3.13 Default Case
    * Algorithms in The Unicode Standard.
    */
   static char_type ToCaseFolded( char_type c )
   {
      return ToLowercase( c );
   }

   /*!
    * Returns the lowercase equivalent character for the specified
    * ISO/IEC-8859-1 code point \a c.
    */
   static char_type ToLowercase( char_type c )
   {
      return char_type( PCL_toLowercaseLatin1[uint8( c )] );
   }

   /*!
    * Returns the uppercase equivalent character for the specified
    * ISO/IEC-8859-1 code point \a c.
    */
   static char_type ToUppercase( char_type c )
   {
      return char_type( PCL_toUppercaseLatin1[uint8( c )] );
   }

   /*!
    * Transforms a string to case folded.
    */
   static void ToCaseFolded( char_type* s, size_type n )
   {
      PCL_PRECONDITION( n == 0 || s != nullptr )
      for ( ; n > 0; --n, ++s )
         *s = ToCaseFolded( *s );
   }

   /*!
    * Transforms a string to lowercase.
    */
   static void ToLowercase( char_type* s, size_type n )
   {
      PCL_PRECONDITION( n == 0 || s != nullptr )
      for ( ; n > 0; --n, ++s )
         *s = ToLowercase( *s );
   }

   /*!
    * Transforms a string to uppercase.
    */
   static void ToUppercase( char_type* s, size_type n )
   {
      PCL_PRECONDITION( n == 0 || s != nullptr )
      for ( ; n > 0; --n, ++s )
         *s = ToUppercase( *s );
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

   /*!
    * Base class of this char traits class.
    */
   typedef GenericCharTraits<char16_type>    traits_base;

   /*!
    * Represents the character data type used by this traits class.
    */
   typedef traits_base::char_type            char_type;

   /*!
    * Returns the length of a null-terminated UTF-16 string in characters
    * (\e not bytes).
    *
    * The returned value is the length of the initial contiguous sequence of
    * characters that are not equal to Null().
    */
   static size_type Length( const char_type* s )
   {
#ifdef __PCL_WINDOWS
      return (s != nullptr) ? ::wcslen( reinterpret_cast<const wchar_t*>( s ) ) : 0;
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
    * an unpredictable result. CopyOverlapped() should be used in these cases.
    */
   static void Copy( char_type* dst, const char_type* src, size_type n )
   {
      PCL_PRECONDITION( n == 0 || dst != nullptr && src != nullptr )
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
   static void CopyOverlapped( char_type* dst, const char_type* src, size_type n )
   {
      PCL_PRECONDITION( n == 0 || dst != nullptr && src != nullptr )
      ::memmove( dst, src, n << 1 );
   }

   /*!
    * Compares numeric character values between two Unicode strings.
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
    * Returns an integer code representing the comparison result:
    *
    * \li  0 if \a s1 and \a s2 are equal
    * \li -1 if \a s1 is less than \a s2
    * \li +1 if \a s1 is greater than \a s2
    */
   static int CompareCodePoints( const char_type* s1, size_type n1,
                                 const char_type* s2, size_type n2, bool caseSensitive = true )
   {
      PCL_COMPARE_CODE_POINTS();
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
    *                         out by comparing Unicode code points (which may
    *                         be faster). The default value is true.
    *
    * Returns an integer code representing the comparison result:
    *
    * \li  0 if \a s1 and \a s2 are equal
    * \li -1 if \a s1 is less than \a s2
    * \li +1 if \a s1 is greater than \a s2
    *
    * On OS X and Windows platforms this function invokes system API routines
    * to perform locale-aware string comparisons.
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
    */
   static int Compare( const char_type* s1, size_type n1,
                       const char_type* s2, size_type n2, bool caseSensitive = true, bool localeAware = true );

   /*!
    * Wildcard string matching algorithm.
    *
    * \param t    The string to be matched.
    *
    * \param nt   Length of the string to be matched in characters.
    *
    * \param p    The pattern string. May contain multiple instances of the
    *             wildcard characters '*' and '?'.
    *
    * \param np   Length of the pattern string in characters.
    *
    * \param caseSensitive    When true, a case-sensitive comparison is
    *                         performed; otherwise the comparison does not
    *                         distinguish between lowercase and uppercase
    *                         characters. The default value of this parameter
    *                         is true.
    *
    * Returns true iff the string \a t matches the specified pattern \a p. If
    * one of the strings is empty, this function always returns false
    * conventionally, even if the pattern is a single asterisk '*'.
    */
   static bool WildMatch( const char_type* t, size_type nt,
                          const char_type* p, size_type np, bool caseSensitive = true )
   {
      if ( caseSensitive )
         return pcl::WildMatch( t, nt, p, np );
      return pcl::WildMatchIC( t, nt, p, np,
                               []( char_type c ) { return ToCaseFolded( c ); },
                               []( char_type c ) { return ToCaseFolded( c ); } );
   }

   /*!
    * Wildcard string matching algorithm - overloaded version with 8-bit
    * pattern string.
    */
   static bool WildMatch( const char_type* t, size_type nt,
                          const char* p, size_type np, bool caseSensitive = true )
   {
      if ( caseSensitive )
         return pcl::WildMatch( t, nt, p, np );
      return pcl::WildMatchIC( t, nt, p, np,
                               []( char_type c ) { return ToCaseFolded( c ); },
                               []( char c ) { return IsoCharTraits::ToCaseFolded( c ); } );
   }

   /*!
    * Returns the \e case \e folded equivalent character for the specified
    * UTF-16 code point \a c.
    *
    * Case folding elimitates case differences, which is useful for
    * case-insensitive string comparisons.
    *
    * We implement the \e simple \e case \e folding Unicode algorithm
    * exclusively. For more information on case folding, see Section 3.13
    * Default Case Algorithms in The Unicode Standard.
    */
   static char_type ToCaseFolded( char_type c )
   {
      if ( c < 256 )
      {
         if ( c >= 65 && c <= 90 || c >= 192 && c <= 214 || c >= 216 && c <= 222 )
            return c + 32;
         return c;
      }
      return PCL_ToCaseFolded( c );
   }

   /*!
    * Returns the lowercase equivalent character for the specified UTF-16 code
    * point \a c.
    */
   static char_type ToLowercase( char_type c )
   {
      if ( c < 256 )
      {
         if ( c >= 65 && c <= 90 || c >= 192 && c <= 214 || c >= 216 && c <= 222 )
            return c + 32;
         return c;
      }
      return PCL_ToLowercase( c );
   }

   /*!
    * Returns the uppercase equivalent character for the specified UTF-16 code
    * point \a c.
    */
   static char_type ToUppercase( char_type c )
   {
      if ( c < 256 )
      {
         if ( c >= 97 && c <= 122 || c >= 224 && c <= 246 || c >= 248 && c <= 254 )
            return c - 32;
         return c;
      }
      return PCL_ToUppercase( c );
   }

   /*!
    * Transforms a string to case folded.
    */
   static void ToCaseFolded( char_type* s, size_type n )
   {
      PCL_PRECONDITION( n == 0 || s != nullptr )
      for ( ; n > 0; --n, ++s )
         *s = ToCaseFolded( *s );
   }

   /*!
    * Transforms a string to lowercase.
    */
   static void ToLowercase( char_type* s, size_type n )
   {
      PCL_PRECONDITION( n == 0 || s != nullptr )
      for ( ; n > 0; --n, ++s )
         *s = ToLowercase( *s );
   }

   /*!
    * Transforms a string to uppercase.
    */
   static void ToUppercase( char_type* s, size_type n )
   {
      PCL_PRECONDITION( n == 0 || s != nullptr )
      for ( ; n > 0; --n, ++s )
         *s = ToUppercase( *s );
   }

   /*!
    * Returns true iff the specified UTF-16 character is a <em>high
    * surrogate</em> (the most significant word of a <em>surrogate pair</em>
    * forming a UTF-32 code point). High surrogates have values between 0xD800
    * and 0xDBFF.
    */
   static constexpr bool IsHighSurrogate( char_type c16 )
   {
      return (c16 & 0xFC00) == 0xD800;
   }

   /*!
    * Returns the <em>high surrogate</em> word of a UTF-32 code point. The
    * specified UTF-32 code must be in the range from 0x010000 to 0x10FFFF,
    * since surrogates only exist outside the Basic Multilingual Plane of
    * Unicode.
    */
   static constexpr char_type HighSurrogate( char32_type c32 )
   {
      return char_type( (c32 >> 10) + 0xD7C0 );
   }

   /*!
    * Returns true iff the specified UTF-16 character is a <em>low
    * surrogate</em> (the least significant word of a <em>surrogate pair</em>
    * forming a UTF-32 code point). Low surrogates have values between 0xDC00
    * and 0xDFFF.
    */
   static constexpr bool IsLowSurrogate( char_type c16 )
   {
      return (c16 & 0xFC00) == 0xDC00;
   }

   /*!
    * Returns the <em>low surrogate</em> word of a UTF-32 code point. The
    * specified UTF-32 code must be in the range from 0x010000 to 0x10FFFF,
    * since surrogates only exist outside the Basic Multilingual Plane of
    * Unicode.
    */
   static constexpr char_type LowSurrogate( char32_type c32 )
   {
      return char_type( (c32%0x400) + 0xDC00 );
   }

   /*!
    * Returns a UTF-32 code point from its <em>surrogate pair</em>. The
    * specified surrogate words must pertain to a valid Unicode code point
    * outside the Basic Multilingual Plane (from 0x010000 to 0x10FFFF).
    */
   static constexpr char32_type SurrogatePairToUTF32( char_type high, char_type low )
   {
      return (char32_type( high ) << 10) + low - 0x035FDC00;
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_CharTraits_h

// ----------------------------------------------------------------------------
// EOF pcl/CharTraits.h - Released 2017-08-01T14:23:31Z
