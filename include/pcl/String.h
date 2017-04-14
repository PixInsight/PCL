//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/String.h - Released 2017-04-14T23:04:40Z
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

#ifndef __PCL_String_h
#define __PCL_String_h

/// \file pcl/String.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Allocator.h>
#include <pcl/Atomic.h>
#include <pcl/ByteArray.h>
#include <pcl/CharTraits.h>
#include <pcl/Container.h>
#include <pcl/Flags.h>
#include <pcl/Iterator.h>
#include <pcl/Math.h>
#include <pcl/ReferenceCounter.h>
#include <pcl/Sort.h>
#include <pcl/StdAlloc.h>
#include <pcl/Utility.h>

#include <stdarg.h>

#ifndef __PCL_NO_STRING_COMPLEX
#  include <pcl/Complex.h>
#endif

#ifndef __PCL_NO_STRING_OSTREAM
#  ifndef _MSC_VER
#    define _GLIBCXX_USE_WCHAR_T  1
#  endif
#  include <ostream>
#endif

#ifdef __PCL_QT_INTERFACE
#  include <QtCore/QByteArray>
#  include <QtCore/QDateTime>
#  include <QtCore/QString>
#  define PCL_GET_CHARPTR_FROM_QSTRING( qs )    (qs.toLatin1().data())
#  define PCL_GET_CHAR16PTR_FROM_QSTRING( qs )  ((char16_type*)( qs.utf16() ))
#  define PCL_GET_QSTRING_FROM_CHAR16PTR( s )   (QString::fromUtf16( (const ushort*)(s) ))
#  define PCL_GET_CHARPTR_FROM_QBYTEARRAY( qb ) (qb.data())
#  define PCL_QDATE_FMT_STR                     "yyyy/MM/dd"
#  define PCL_QDATETIME_FMT_STR                 "yyyy/MM/dd hh:mm:ss"
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::RandomizationOption
 * \brief     String randomization options
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>RandomizationOption::Lowercase</td> <td>Generate random lowercase alphabetic characters: [a..z]</td></tr>
 * <tr><td>RandomizationOption::Uppercase</td> <td>Generate random uppercase alphabetic characters: [A..Z]</td></tr>
 * <tr><td>RandomizationOption::Alpha</td>     <td>Generate random lowercase and uppercase alphabetic characters: [a..zA..Z]</td></tr>
 * <tr><td>RandomizationOption::Digits</td>    <td>Generate random decimal digits: [0..9]</td></tr>
 * <tr><td>RandomizationOption::Symbols</td>   <td>Generate random symbol characters: .#/[]() etc.</td></tr>
 * <tr><td>RandomizationOption::HexDigits</td> <td>Generate random hexadecimal digits: [0..9a..f], or [0..9A..F] if Uppercase is set.</td></tr>
 * <tr><td>RandomizationOption::BinDigits</td> <td>Generate random binary digits: [01].</td></tr>
 * <tr><td>RandomizationOption::FullRange</td> <td>Generate random characters in the whole range of code points except zero.</td></tr>
 * <tr><td>RandomizationOption::Default</td>   <td>Default options: Alpha|Digits</td></tr>
 * </table>
 */
namespace RandomizationOption
{
   enum mask_type
   {
      Lowercase = 0x00000001,  // Generate random lowercase alphabetic characters: a..z
      Uppercase = 0x00000002,  // Generate random uppercase alphabetic characters: A..Z
      Alpha     = Lowercase|Uppercase,  // Generate random alphabetic characters: a..zA..Z
      Digits    = 0x00000004,  // Generate random decimal digits: 0..9
      Symbols   = 0x00000008,  // Generate random symbol characters: .#/[]() etc
      HexDigits = 0x00000010,  // Generate hexadecimal digits: 0..9a..f or 0..9A..F
      BinDigits = 0x00000020,  // Generate binary digits: 0 and 1
      FullRange = 0x80000000,  // Generate random characters in the whole range of code points except zero
      Default = Alpha|Digits
   };
};

/*!
 * \class pcl::RandomizationOptions
 * \brief A collection of string randomization options.
 */
typedef Flags<RandomizationOption::mask_type>  RandomizationOptions;

// ----------------------------------------------------------------------------

/*!
 * \class SexagesimalConversionOptions
 * \brief Formatting options for string sexagesimal representations
 *
 * \sa IsoString::ToSexagesimal(), String::ToSexagesimal()
 */
struct PCL_CLASS SexagesimalConversionOptions
{
   /*!
    * Number of represented items. Can be 1, 2 or 3. All items but the last one
    * are represented as integer values. The last item is represented as a
    * floating point value with the specified precision. The default value is 3
    * items.
    */
   unsigned items     : 2;

   /*!
    * Number of decimal digits for the last represented sexagesimal item. The
    * default value is two decimal digits.
    */
   unsigned precision : 4;

   /*!
    * Whether to always prepend a sign character to the first represented item,
    * even '+' for positive values. This is false by default.
    */
   bool     sign      : 1;

   /*!
    * Width of the integer part of the first represented item in characters,
    * including the sign when applicable. When the specified width is larger
    * than the length of the represented item, it is left-padded with padding
    * characters as necessary. The default is zero, which means that no padding
    * is performed for the first item by default.
    */
   unsigned width     : 4;

   /*!
    * A single character used for separation of sexagesimal components. The
    * default separator is a colon character, ':'.
    */
   char     separator;

   /*!
    * A single character used for padding right-justified representations of
    * first sexagesimal components. See the width member. The default padding
    * character is the white space, ' '.
    */
   char     padding;

   /*!
    * Default constructor.
    */
   SexagesimalConversionOptions( unsigned items_     = 3,
                                 unsigned precision_ = 2,
                                 bool     sign_      = false,
                                 unsigned width_     = 0,
                                 char     separator_ = ':',
                                 char     padding_   = ' ' ) :
      items( items_ ),
      precision( precision_ ),
      sign( sign_ ),
      width( width_ ),
      separator( separator_ ),
      padding( padding_ )
   {
   }

   /*!
    * Copy constructor.
    */
   SexagesimalConversionOptions( const SexagesimalConversionOptions& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   SexagesimalConversionOptions& operator =( const SexagesimalConversionOptions& ) = default;
};

// ----------------------------------------------------------------------------

/*!
 * \class ISO8601ConversionOptions
 * \brief Formatting options for string representations of dates and times in
 * ISO 8601 format.
 *
 * \sa IsoString::ToISO8601DateTime(), String::ToISO8601DateTime()
 */
struct ISO8601ConversionOptions
{
   /*!
    * Number of represented time items. Can be 0, 1, 2 or 3. All items but the
    * last one are represented as integer values. The last item is represented
    * as a floating point value with the specified precision. The default value
    * is 3 time items.
    */
   unsigned timeItems : 2;

   /*!
    * Number of decimal digits for the last represented time item. The default
    * value is three decimal digits.
    */
   unsigned precision : 4;

   /*!
    * Whether to append a time zone specifier to the ISO 8601 representation.
    * The default value is true.
    */
   bool     timeZone  : 1;

   /*!
    * Whether to append the 'Z' special time zone specifier for UTC time, or
    * the '+00:00' specifier otherwise. The default value is true.
    */
   bool     zuluTime  : 1;

   /*!
    * Default constructor.
    */
   ISO8601ConversionOptions( unsigned timeItems_ = 3,
                             unsigned precision_ = 3,
                             bool     timeZone_  = true,
                             bool     zuluTime_  = true ) :
      timeItems( timeItems_ ),
      precision( precision_ ),
      timeZone( timeZone_ ),
      zuluTime( zuluTime_ )
   {
   }

   /*!
    * Copy constructor.
    */
   ISO8601ConversionOptions( const ISO8601ConversionOptions& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   ISO8601ConversionOptions& operator =( const ISO8601ConversionOptions& ) = default;
};

// ----------------------------------------------------------------------------

/*!
 * \class GenericString
 * \brief Generic string.
 *
 * %GenericString is a finite ordered sequence of characters implemented as a
 * reference-counted, dynamic array of T objects, whose fundamental behavior is
 * specified by an instantiation R of GenericCharTraits for the character type
 * T (typically GenericCharTraits, or a derived class such as CharTraits or
 * IsoCharTraits), and where A provides dynamic allocation for contiguous
 * sequences of elements of type T (StandardAllocator is used by default).
 *
 * On the PixInsight platform, all dynamically allocated strings have been
 * implemented as two instantiations of the %GenericString template class,
 * namely the String (UTF-16 string) and IsoString (UTF-8 or ISO/IEC-8859-1
 * string) classes.
 *
 * \sa String, IsoString, CharTraits, IsoCharTraits
 */
template <class T, class R, class A = StandardAllocator>
class PCL_CLASS GenericString : public DirectContainer<T>
{
public:

   /*!
    * Represents a string character.
    */
   typedef T                     char_type;

   /*!
    * The character traits class used by this string class.
    */
   typedef R                     char_traits;

   /*!
    * The block allocator used by this string class.
    */
   typedef A                     block_allocator;

   /*!
    * The allocator class used by this string class.
    */
   typedef pcl::Allocator<T,A>   allocator;

   /*!
    * Null-terminated string of char_type characters.
    */
   typedef T*                    c_string;

   /*!
    * Immutable null-terminated string of char_type characters.
    */
   typedef const T*              const_c_string;

   /*!
    * String iterator.
    */
   typedef T*                    iterator;

   /*!
    * Immutable string iterator.
    */
   typedef const T*              const_iterator;

   /*!
    * Reverse string iterator.
    */
   typedef ReverseRandomAccessIterator<iterator, T>
                                 reverse_iterator;

   /*!
    * Reverse immutable string iterator.
    */
   typedef ReverseRandomAccessIterator<const_iterator, const T>
                                 const_reverse_iterator;

   // -------------------------------------------------------------------------

   /*!
    * Constant used to signal unsuccessful search operations.
    */
   static const size_type notFound = ~size_type( 0 );

   /*!
    * Corresponds to the maximum possible character index in a string.
    */
   static const size_type maxPos = ~size_type( 0 );

   // -------------------------------------------------------------------------

   /*!
    * Constructs an empty string.
    */
   GenericString()
   {
      m_data = Data::New();
   }

   /*!
    * Copy constructor. Increments the reference counter of the string \a s.
    */
   GenericString( const GenericString& s ) : m_data( s.m_data )
   {
      m_data->Attach();
   }

   /*!
    * Move constructor.
    */
   GenericString( GenericString&& s ) : m_data( s.m_data )
   {
      s.m_data = nullptr;
   }

   /*!
    * Constructs a string with a copy of the null-terminated character sequence
    * stored in the specified array \a t.
    */
   GenericString( const_c_string t )
   {
      size_type len = R::Length( t );
      if ( len > 0 )
      {
         m_data = Data::New( len );
         R::Copy( m_data->string, t, len );
      }
      else
         m_data = Data::New();
   }

   /*!
    * Constructs a string with a copy of the character sequence defined by the
    * range [i,j).
    *
    * If \a i is greater than or equal to \a j, this constructor creates an
    * empty string. Otherwise it will assign the specified sequence of
    * characters.
    *
    * \b Important - Note that this constructor is \e not equivalent to:
    *
    * \code GenericString( i, 0, j-i ); \endcode
    *
    * because the above call would assign a null-terminated sequence, while
    * this function assigns the specified range unconditionally, even if it
    * contains null characters. Since this constructor does not have to scan
    * for a terminating character, it is potentially more efficient.
    */
   GenericString( const_iterator i, const_iterator j )
   {
      if ( i < j )
      {
         size_type len = j - i;
         m_data = Data::New( len );
         R::Copy( m_data->string, i, len );
      }
      else
         m_data = Data::New();
   }

   /*!
    * Constructs a string with a copy of at most \a n characters stored in the
    * null-terminated sequence \a t, starting from its \a i-th character.
    */
   GenericString( const_c_string t, size_type i, size_type n )
   {
      size_type len = R::Length( t );
      if ( i < len && (n = pcl::Min( n, len-i )) > 0 )
      {
         m_data = Data::New( n );
         R::Copy( m_data->string, t+i, n );
      }
      else
         m_data = Data::New();
   }

   /*!
    * Constructs a string with \a n copies of a character \a c.
    */
   GenericString( char_type c, size_type n = 1 )
   {
      if ( n > 0 )
      {
         m_data = Data::New( n );
         R::Fill( m_data->string, c, n );
      }
      else
         m_data = Data::New();
   }

   /*!
    * Destroys this string. Decrements the reference counter of the string data
    * and, if it becomes unreferenced, it is destroyed and deallocated.
    */
   ~GenericString()
   {
      if ( m_data != nullptr )
      {
         DetachFromData();
         m_data = nullptr;
      }
   }

   /*!
    * Returns true iff this string uniquely references its string data.
    *
    * \sa EnsureUnique(), IsAliasOf()
    */
   bool IsUnique() const
   {
      return m_data->IsUnique();
   }

   /*!
    * Returns true iff this string is an alias of another string \a s.
    *
    * Two strings are aliases if both share the same string data.
    *
    * \sa IsUnique(), EnsureUnique()
    */
   bool IsAliasOf( const GenericString& s ) const
   {
      return m_data == s.m_data;
   }

   /*!
    * Ensures that this string uniquely references its string data.
    *
    * If necessary, this member function generates a duplicate of the string
    * data, references it, and then decrements the reference counter of the
    * original string data.
    *
    * \sa IsUnique(), IsAliasOf()
    */
   void EnsureUnique()
   {
      if ( !IsUnique() )
      {
         size_type len = Length();
         Data* newData = Data::New( len );
         if ( len > 0 )
            R::Copy( newData->string, m_data->string, len );
         DetachFromData();
         m_data = newData;
      }
   }

   /*!
    * Returns the number of bytes necessary to store a single character in this
    * string.
    *
    * This member function returns the R::BytesPerChar() static member function
    * of the char traits class R.
    */
   static size_type BytesPerChar()
   {
      return R::BytesPerChar();
   }

   /*!
    * Returns the total number of bytes required to store the string data
    * referenced by this string, \e excluding the terminating null character.
    *
    * \sa Length(), Capacity()
    */
   size_type Size() const
   {
      return Length()*BytesPerChar();
   }

   /*!
    * Returns the length of this string in characters. This \e excludes the
    * terminating null character.
    *
    * \sa IsEmpty(), Available(), Capacity(), Size()
    */
   size_type Length() const
   {
      return m_data->end - m_data->string;
   }

   /*!
    * Returns the total capacity of this string in characters.
    *
    * The capacity of a string is the maximum number of characters that can be
    * stored before requiring a reallocation of string data.
    *
    * \sa Available(), Length(), Size()
    */
   size_type Capacity() const
   {
      return m_data->capacity - m_data->string;
   }

   /*!
    * Returns the number of characters available in this string.
    *
    * The number of available characters is equal to Capacity() - Length().
    *
    * \sa Capacity(), Length(), Size()
    */
   size_type Available() const
   {
      return m_data->capacity - m_data->end;
   }

   /*!
    * Returns true only if this string is valid. A string is valid if it
    * references an internal string structure, even if it is an empty string.
    *
    * In general, all %GenericString objects are valid with only two
    * exceptions:
    *
    * \li Objects that have been move-copied or move-assigned to other strings.
    * \li Objects that have been invalidated explicitly by calling Transfer().
    *
    * An invalid string object cannot be used and should be destroyed
    * immediately. Invalid strings are always destroyed automatically during
    * move construction and move assignment operations.
    *
    * \sa IsEmpty()
    */
   bool IsValid() const
   {
      return m_data != nullptr;
   }

   /*!
    * Returns true only if this string is empty.
    *
    * An empty string has zero length and stores no characters.
    *
    * \sa IsValid(), Length(), Available(), Capacity(), Size()
    */
   bool IsEmpty() const
   {
      return m_data->string == m_data->end;
   }

   /*!
    * Returns the minimum valid character index.
    *
    * This member function exists for compatibility with PCL containers. It
    * always returns zero.
    *
    * \sa UpperBound(), Length()
    */
   size_type LowerBound() const
   {
      return 0;
   }

   /*!
    * Returns the maximum valid character index.
    *
    * The returned value is equal to Length()-1. If this string is empty, the
    * index returned by this member function is invalid and equal to maxPos.
    *
    * \sa LowerBound(), Length()
    */
   size_type UpperBound() const
   {
      PCL_PRECONDITION( !IsEmpty() )
      return Length()-1;
   }

   /*!
    * Returns a reference to the (immutable) allocator object in this string.
    *
    * \sa SetAllocator()
    */
   const allocator& GetAllocator() const
   {
      return m_data->alloc;
   }

   /*!
    * Sets the allocator object used by this string to a copy of the specified
    * allocator \a a.
    *
    * If this string is not unique, it is made unique before returning from
    * this member function.
    *
    * \sa GetAllocator()
    */
   void SetAllocator( const allocator& a )
   {
      EnsureUnique();
      m_data->alloc = a;
   }

   /*!
    * Returns an iterator located at the \a i-th character of this string.
    * The character index \a i must be in the range [0,Length()).
    *
    * If this string is not unique, it is made unique before returning from
    * this member function.
    *
    * If this string is empty, or if the specified index \a i is out of range,
    * calling this member function is an error that leads to an invalid result.
    *
    * \sa operator []( size_type )
    */
   iterator At( size_type i )
   {
      PCL_PRECONDITION( i < Length() )
      EnsureUnique();
      return m_data->string + i;
   }

   /*!
    * Returns an immutable iterator located at the \a i-th character in this
    * string. The character index \a i must be in the range [0,Length()).
    *
    * If this string is empty, or if the specified index \a i is out of range,
    * calling this member function is an error that leads to an invalid result.
    *
    * \sa operator []( size_type ) const
    */
   const_iterator At( size_type i ) const
   {
      PCL_PRECONDITION( i < Length() )
      return m_data->string + i;
   }

   /*!
    * Returns a reference to the \a i-th character in this string. The
    * character index \a i must be in the range [0,Length()).
    *
    * If this string is not unique, it is made unique before returning from
    * this member function.
    *
    * If this string is empty, or if the specified index \a i is out of range,
    * calling this member function is an error that leads to an invalid result.
    *
    * \sa At(), operator *()
    */
   char_type& operator []( size_type i )
   {
      return *At( i );
   }

   /*!
    * Returns a copy of the \a i-th character in this string. The character
    * index \a i must be in the range [0,Length()).
    *
    * If this string is empty, or if the specified index \a i is out of range,
    * calling this member function is an error that leads to an invalid result.
    *
    * \sa At() const, operator *() const
    */
   char_type operator []( size_type i ) const
   {
      return *At( i );
   }

   /*!
    * Returns a reference to the first character in this string.
    *
    * This is a convenience member function, equivalent to *At( 0 ).
    *
    * \sa operator []( size_type ), At()
    */
   char_type& operator *()
   {
      EnsureUnique();
      return *m_data->string;
   }

   /*!
    * Returns a copy of the first character in this string.
    *
    * This is a convenience member function, equivalent to *At( 0 ).
    *
    * \sa operator []( size_type ) const, At()
    */
   char_type operator *() const
   {
      return *m_data->string;
   }

   /*!
    * Returns a mutable iterator pointing to the first character of this
    * string.
    *
    * If this string is not unique, it is made unique before returning from
    * this member function.
    *
    * If this string is empty, this member function returns \c nullptr.
    *
    * \sa End()
    */
   iterator Begin()
   {
      EnsureUnique();
      return m_data->string;
   }

   /*!
    * Returns an immutable iterator pointing to the first character of this
    * string.
    *
    * If this string is empty, this member function returns \c nullptr.
    *
    * \sa End() const
    */
   const_iterator Begin() const
   {
      return m_data->string;
   }

   /*!
    * Returns a mutable iterator pointing to the end of this string. The
    * returned iterator points to the next-to-last character of this string.
    *
    * If this string is not unique, it is made unique before returning from
    * this member function.
    *
    * If this string is empty, this member function returns \c nullptr.
    *
    * \sa Begin()
    */
   iterator End()
   {
      EnsureUnique();
      return m_data->end;
   }

   /*!
    * Returns an immutable iterator pointing to the end of this string. The
    * returned iterator points to the next-to-last character of this string.
    *
    * If this string is empty, this member function returns \c nullptr.
    *
    * \sa Begin() const
    */
   const_iterator End() const
   {
      return m_data->end;
   }

   /*!
    * Returns a reverse mutable iterator pointing to the <em>reverse
    * beginning</em> of this string. The returned iterator points to the last
    * character of this string.
    *
    * If this string is not unique, it is made unique before returning from
    * this member function.
    *
    * If this string is empty, this member function returns a null reverse
    * iterator.
    *
    * \sa ReverseEnd()
    */
   reverse_iterator ReverseBegin()
   {
      EnsureUnique();
      return (m_data->string < m_data->end) ? m_data->end-1 : nullptr;
   }

   /*!
    * Returns an immutable reverse iterator pointing to the <em>reverse
    * beginning</em> of this string. The returned iterator points to the last
    * character of this string.
    *
    * If this string is empty, this member function returns a null reverse
    * iterator.
    *
    * \sa ReverseEnd() const
    */
   const_reverse_iterator ReverseBegin() const
   {
      return (m_data->string < m_data->end) ? m_data->end-1 : nullptr;
   }

   /*!
    * Returns a reverse mutable iterator pointing to the <em>reverse end</em>
    * of this string. The returned iterator points to the
    * <em>previous-to-first</em> character of this string.
    *
    * If this string is not unique, it is made unique before returning from
    * this member function.
    *
    * If this string is empty, this member function returns a null reverse
    * iterator.
    *
    * \sa ReverseBegin()
    */
   reverse_iterator ReverseEnd()
   {
      EnsureUnique();
      return (m_data->string < m_data->end) ? m_data->string-1 : nullptr;
   }

   /*!
    * Returns an immutable reverse iterator pointing to the <em>reverse
    * end</em> of this string. The returned iterator points to the
    * <em>previous-to-first</em> character of this string.
    *
    * If this string is empty, this member function returns a null reverse
    * iterator.
    *
    * \sa ReverseBegin() const
    */
   const_reverse_iterator ReverseEnd() const
   {
      return (m_data->string < m_data->end) ? m_data->string-1 : nullptr;
   }

   /*!
    * Returns the zero-based character index corresponding to a valid iterator
    * \a i in this string. This is equivalent to \a i - Begin().
    *
    * The specified iterator must be posterior to or located at the starting
    * iterator of this string, as returned by Begin(). However, for performance
    * reasons this condition is neither enforced nor verified. If an invalid
    * iterator is specified, then this function, as well as any subsequent use
    * of the returned character index, may invoke undefined behavior.
    */
   size_type IndexAt( const_iterator i ) const
   {
      PCL_PRECONDITION( i >= m_data->string )
      return i - m_data->string;
   }

#ifndef __PCL_NO_STL_COMPATIBLE_ITERATORS
   /*!
    * STL-compatible iteration. Equivalent to Begin().
    */
   iterator begin()
   {
      return Begin();
   }

   /*!
    * STL-compatible iteration. Equivalent to Begin() const.
    */
   const_iterator begin() const
   {
      return Begin();
   }

   /*!
    * STL-compatible iteration. Equivalent to End().
    */
   iterator end()
   {
      return End();
   }

   /*!
    * STL-compatible iteration. Equivalent to End() const.
    */
   const_iterator end() const
   {
      return End();
   }
#endif   // !__PCL_NO_STL_COMPATIBLE_ITERATORS

   /*!
    * Returns a pointer to the immutable internal data array of this string.
    *
    * If this string is empty, this member function returns a pointer to a
    * static, null-terminated, unmodifiable empty string (the "" C string).
    * This function always returns a valid pointer to existing character data.
    */
   const_c_string c_str() const
   {
      static const char_type theNullChar = char_traits::Null();
      return IsEmpty() ? &theNullChar : Begin();
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    *
    * This operator calls Assign() with the specified source string \a s.
    */
   GenericString& operator =( const GenericString& s )
   {
      Assign( s );
      return *this;
   }

   /*!
    * Assigns a string \a s to this string.
    *
    * If this instance and the specified source instance \a s reference
    * different string data, then the data previously referenced by this object
    * is dereferenced. If the previous data becomes unreferenced, it is
    * destroyed and deallocated. Then the data being referenced by \a s is also
    * referenced by this object.
    *
    * If this instance and the specified source instance \a s already reference
    * the same string data, then this function does nothing.
    *
    * \sa Transfer()
    */
   void Assign( const GenericString& s )
   {
      s.m_data->Attach();
      DetachFromData();
      m_data = s.m_data;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    *
    * This operator calls Transfer() with the specified source string \a s.
    */
   GenericString& operator =( GenericString&& s )
   {
      Transfer( s );
      return *this;
   }

   /*!
    * Transfers data from another object \a s to this string.
    *
    * Decrements the reference counter of the current string data. If the data
    * becomes unreferenced, it is destroyed and deallocated. The string data
    * referenced by the source object \a s is then transferred to this object.
    *
    * \warning The source string \a s will be an invalid object after calling
    * this function, and hence should be destroyed immediately. Any attempt to
    * access an invalid object will most likely lead to a crash.
    *
    * \sa Assign()
    */
   void Transfer( GenericString& s )
   {
      DetachFromData();
      m_data = s.m_data;
      s.m_data = nullptr;
   }

   /*!
    * Transfers data from another object \a s to this string.
    *
    * See Transfer( GenericString& ) for detailed information.
    */
   void Transfer( GenericString&& s )
   {
      DetachFromData();
      m_data = s.m_data;
      s.m_data = nullptr;
   }

   /*!
    * Assigns a copy of the null-terminated character sequence stored in the
    * specified array \a t to this string. Returns a reference to this object.
    */
   GenericString& operator =( const_c_string t )
   {
      Assign( t );
      return *this;
   }

   /*!
    * Assigns a single character \a c to this string. Returns a reference to
    * this object.
    */
   GenericString& operator =( char_type c )
   {
      Assign( c, 1 );
      return *this;
   }

   /*!
    * Assigns a substring of at most \a n characters from a source string \a s,
    * starting from its \a i-th character, to this string.
    */
   void Assign( const GenericString& s, size_type i, size_type n )
   {
      Transfer( s.Substring( i, n ) );
   }

   /*!
    * Assigns a copy of the null-terminated character sequence stored in the
    * specified array \a t to this string.
    *
    * If \a t is a pointer to the contents of this string (or, equivalently, an
    * iterator on this string), this function invokes undefined behavior.
    */
   void Assign( const_c_string t )
   {
      size_type len = R::Length( t );
      if ( len > 0 )
      {
         MaybeReallocate( len );
         R::Copy( m_data->string, t, len );
      }
      else
         Clear();
   }

   /*!
    * Assigns a copy of the character sequence defined by the range [i,j) to
    * this string.
    *
    * If \a i is greater than or equal to \a j, this function empties the string
    * by calling Clear(). Otherwise this function will assign the specified
    * sequence of characters.
    *
    * If \a i and/or \a j are iterators on this string, this function invokes
    * undefined behavior.
    *
    * \b Important - Note that this function is \e not equivalent to:
    *
    * \code Assign( i, 0, j-i ); \endcode
    *
    * because the above call would assign a null-terminated sequence, while
    * this function assigns the specified range unconditionally, even if it
    * contains null characters. Since this function does not have to scan for
    * a terminating character, it is potentially much more efficient.
    */
   void Assign( const_iterator i, const_iterator j )
   {
      if ( i < j )
      {
         size_type len = j - i;
         MaybeReallocate( len );
         R::Copy( m_data->string, i, len );
      }
      else
         Clear();
   }

   /*!
    * Assigns a copy of at most \a n characters stored in the null-terminated
    * sequence \a t, starting from its \a i-th character, to this string.
    *
    * If \a t is the null pointer, or \a i is greater than the length of \a t,
    * or \a n is zero, this function empties the string by calling Clear().
    *
    * If \a t is a pointer to the contents of this string (or, equivalently, an
    * iterator on this string), this function invokes undefined behavior.
    *
    * The character count \a n will be constrained to copy existing characters
    * from the null-terminated source sequence.
    */
   void Assign( const_c_string t, size_type i, size_type n )
   {
      size_type len = R::Length( t );
      if ( i < len && (n = pcl::Min( n, len-i )) > 0 )
      {
         MaybeReallocate( n );
         R::Copy( m_data->string, t+i, n );
      }
      else
         Clear();
   }

   /*!
    * Assigns \a n copies of the specified character \a c to this string. If
    * either \a c is the null character or \a n is zero, this function empties
    * the string by calling Clear().
    */
   void Assign( char_type c, size_type n = 1 )
   {
      if ( !R::IsNull( c ) && n > 0 )
      {
         MaybeReallocate( n );
         R::Fill( m_data->string, c, n );
      }
      else
         Clear();
   }

   /*!
    * Exchanges this string with another string \a s.
    */
   void Swap( GenericString& s )
   {
      pcl::Swap( m_data, s.m_data );
   }

   /*!
    * Fills this string (if not empty) with the specified character \a c.
    *
    * \note The null terminating character can legally be specified as a
    * filling character with this function. In this case the internal string
    * pointer will be seen as an empty string by C library routines (such as
    * strlen or strcpy) and other applications after calling this function,
    * although it will continue storing the same allocated data block.
    */
   void Fill( char_type c )
   {
      size_type len = Length();
      if ( len > 0 )
      {
         if ( !IsUnique() )
         {
            Data* newData = Data::New( len );
            DetachFromData();
            m_data = newData;
         }
         R::Fill( m_data->string, c, len );
      }
   }

   /*!
    * Fills a segment of at most \a n contiguous characters in this string with
    * the specified character \a c, starting from the \a i-th character.
    *
    * If \a i is greater than or equal to the length of this string, then
    * calling this function has no effect. Otherwise the character count \a n
    * will be constrained to replace existing characters in this string.
    *
    * \note The null terminating character can legally be specified as a
    * filling character with this function. In this case the internal string
    * pointer will be seen as an empty string by C library routines (such as
    * strlen or strcpy) and other applications after calling this function,
    * although it will continue storing the same allocated data block.
    */
   void Fill( char_type c, size_type i, size_type n = maxPos )
   {
      size_type len = Length();
      if ( i < len )
      {
         n = pcl::Min( n, len-i );
         if ( n < len )
            EnsureUnique();
         else if ( !IsUnique() )
         {
            Data* newData = Data::New( len );
            DetachFromData();
            m_data = newData;
         }
         R::Fill( m_data->string+i, c, n );
      }
   }

   /*!
    * Securely fills this string and all instances sharing its string data with
    * the specified character \a c.
    *
    * If no filling character \a c is specified, the string will be filled with
    * zeros, or null characters.
    *
    * The normal data sharing mechanism is ignored on purpose by this function,
    * so if there are other objects sharing the same string data, all of them
    * will be affected unconditionally after calling this function.
    *
    * This function is useful to ensure that sensitive data, such as user
    * passwords and user names, are destroyed without the risk of surviving
    * duplicates as a result of implicit data sharing.
    *
    * \note If the specified filling character \a c is zero, or the null
    * character (the default value), the internal string pointer will be seen
    * as an empty string by C library routines (such as strlen or strcpy) and
    * other applications after calling this function, although it will continue
    * storing the same allocated data block.
    */
   void SecureFill( char c = '\0' )
   {
      volatile char* s = reinterpret_cast<volatile char*>( m_data->string );
      for ( size_type n = Capacity()*sizeof( char_type ); n > 0; --n )
         *s++ = c;
   }

   /*!
    * Ensures that this string has enough capacity to store \a n characters
    * plus a terminating null character.
    *
    * After calling this member function with \a n > 0, this object is
    * guaranteed to uniquely reference its string data.
    */
   void Reserve( size_type n )
   {
      if ( n > 0 )
         if ( IsUnique() )
         {
            if ( Capacity() < n+1 )
            {
               iterator old = m_data->string;
               size_type len = Length();
               m_data->Reserve( len, n );
               if ( old != nullptr )
               {
                  if ( len > 0 )
                     R::Copy( m_data->string, old, len );
                  m_data->alloc.Deallocate( old );
               }
            }
         }
         else
         {
            size_type len = Length();
            Data* newData = Data::New(); // ### FIXME: unlikely, but this is a potential leak
            newData->Reserve( len, pcl::Max( len, n ) );
            if ( len > 0 )
               R::Copy( newData->string, m_data->string, len );
            DetachFromData();
            m_data = newData;
         }
   }

   /*!
    * Causes this string to have the specified length \a n in characters.
    *
    * If \a n is zero, this function calls Clear() to empty the string.
    * Otherwise the string data will be shrunk or extended, and possibly
    * reallocated. If the resulting string is larger than the original, newly
    * allocated or reserved characters won't be initialized and will have
    * unpredictable values; in this case it is the caller's responsibility to
    * write them as appropriate.
    *
    * After calling this member function, this object is guaranteed to uniquely
    * reference its string data.
    */
   void SetLength( size_type n )
   {
      if ( n != Length() )
      {
         if ( n > 0 )
         {
            if ( !IsUnique() || m_data->ShouldReallocate( n ) )
            {
               Data* newData = Data::New( n );
               size_type m = Capacity();
               if ( m > 0 )
                  R::Copy( newData->string, m_data->string, pcl::Min( n, m ) );
               DetachFromData();
               m_data = newData;
            }
            else
               m_data->SetLength( n );
         }
         else
            Clear();
      }
      else
         EnsureUnique();
   }

   /*!
    * Returns a copy of this string resized to have the specified length \a n
    * in characters. This function is a wrapper for SetLength(); see that
    * function for important information.
    */
   GenericString SetToLength( size_type n ) const
   {
      // NB: This can be done more efficiently if this string is unique.
      GenericString s( *this );
      s.SetLength( n );
      return s;
   }

   /*!
    * Resizes this string to match the length of the internal null-terminated
    * string buffer.
    *
    * This member function scans the internal string buffer for a terminating
    * null character, then calls SetLength() to set the corresponding length in
    * characters.
    */
   void ResizeToNullTerminated()
   {
      SetLength( R::Length( m_data->string ) );
   }

   /*!
    * Returns a copy of this string resized to match the length of its internal
    * null-terminated string buffer.
    */
   GenericString ResizedToNullTerminated() const
   {
      GenericString s( *this );
      s.ResizeToNullTerminated();
      return s;
   }

   /*!
    * Causes this string to allocate the exact required memory space to store
    * its contained characters, plus a terminating null character.
    *
    * If the string has excess capacity, a new copy of its existing characters
    * is generated and stored in a newly allocated memory block that fits them
    * exactly, then the previous memory block is deallocated.
    *
    * If the string is empty, calling this function is equivalent to Clear().
    * Note that in this case a previously allocated memory block (by a call to
    * Reserve()) may also be deallocated.
    */
   void Squeeze()
   {
      if ( Available() > 0 )
      {
         size_type len = Length();
         if ( len > 0 )
         {
            if ( IsUnique() )
            {
               iterator old = m_data->string;
               m_data->Allocate( len, 0 );
               R::Copy( m_data->string, old, len );
               m_data->alloc.Deallocate( old );
            }
            else
            {
               Data* newData = Data::New( len, 0 );
               R::Copy( newData->string, m_data->string, len );
               DetachFromData();
               m_data = newData;
            }
         }
         else
            Clear();
      }
   }

   /*!
    * Returns a copy of this string allocated to the exact required memory
    * space to store its contained characters and a terminating null character.
    */
   GenericString Squeezed() const
   {
      // NB: This can be done more efficiently if this string is unique.
      GenericString s( *this );
      s.Squeeze();
      return s;
   }

   /*!
    * Releases the data in this string.
    *
    * This member function returns a pointer to the string data block
    * referenced by this object, after ensuring that it is uniquely referenced.
    * If the string is empty, this function may return the null pointer.
    *
    * Before returning, this member function empties this string without
    * deallocating its string data. The caller is then responsible for
    * destructing and/or deallocating the returned block when it is no longer
    * required.
    */
   c_string Release()
   {
      EnsureUnique();
      iterator string = m_data->string;
      m_data->Reset();
      return string;
   }

   /*!
    * Copies characters from this string to a null-terminated character array.
    *
    * \param[out] dst   Destination character array.
    *
    * \param maxCharsToCopy  The total number of characters that can be stored
    *                   at the \a dst array, \e including a null terminating
    *                   character.
    *
    * \param i          Index of the first character to be copied from this
    *                   string.
    *
    * This function copies at most \a maxCharsToCopy-1 characters from this
    * string (or Length()-i, whichever is less) to the \a dst array. Then a
    * null terminating character is appended to \a dst.
    */
   void c_copy( iterator dst, size_type maxCharsToCopy, size_type i = 0 ) const
   {
      if ( dst != nullptr )
         if ( maxCharsToCopy > 0 )
         {
            if ( --maxCharsToCopy > 0 )
            {
               size_type len = Length();
               if ( i < len )
               {
                  size_type n = pcl::Min( maxCharsToCopy, len-i );
                  R::Copy( dst, m_data->string+i, n );
                  dst += n;
               }
            }

            *dst = R::Null();
         }
   }

   /*!
    * Inserts a copy of the string \a s at the index \a i in this string.
    */
   template <class R1, class A1>
   void Insert( size_type i, const GenericString<T,R1,A1>& s )
   {
      size_type n = s.Length();
      if ( n > 0 )
      {
         UninitializedGrow( i, n ); // -> 0 <= i <= len
         R::Copy( m_data->string+i, s.m_data->string, n );
      }
   }

   /*!
    * Inserts a copy of the character sequence defined by the range [p,q) at
    * the index \a i in this string.
    *
    * If \a p is greater than or equal to \a q, calling this member function
    * has no effect.
    */
   void Insert( size_type i, const_iterator p, const_iterator q )
   {
      if ( p < q )
      {
         size_type n = q - p;
         UninitializedGrow( i, n ); // -> 0 <= i <= len
         R::Copy( m_data->string+i, p, n );
      }
   }

   /*!
    * Inserts a null-terminated character sequence \a t at the index \a i in
    * this string.
    */
   void Insert( size_type i, const_c_string t )
   {
      size_type n = R::Length( t );
      if ( n > 0 )
      {
         UninitializedGrow( i, n ); // -> 0 <= i <= len
         R::Copy( m_data->string+i, t, n );
      }
   }

   /*!
    * Inserts at most the first \a n characters of a null-terminated character
    * sequence \a t at the index \a i in this string.
    */
   void Insert( size_type i, const_c_string t, size_type n )
   {
      if ( (n = pcl::Min( n, R::Length( t ) )) > 0 )
      {
         UninitializedGrow( i, n ); // -> 0 <= i <= len
         R::Copy( m_data->string+i, t, n );
      }
   }

   /*!
    * Inserts \a n copies of a character \a c at the index \a i in this string.
    */
   void Insert( size_type i, char_type c, size_type n = 1 )
   {
      if ( n > 0 )
      {
         UninitializedGrow( i, n ); // -> 0 <= i <= len
         R::Fill( m_data->string+i, c, n );
      }
   }

   /*!
    * Appends a copy of the specified string \a s to this string.
    */
   template <class R1, class A1>
   void Append( const GenericString<T,R1,A1>& s )
   {
      Insert( maxPos, s );
   }

   /*!
    * Appends a copy of a string by calling Append( s ). Returns a reference to
    * this object.
    */
   template <class R1, class A1>
   GenericString& operator +=( const GenericString<T,R1,A1>& s )
   {
      Append( s );
      return *this;
   }

   /*!
    * Appends a copy of the character sequence defined by the range [i,j) to
    * this string.
    *
    * If \a i is greater than or equal to \a j, calling this member function
    * has no effect.
    */
   void Append( const_iterator i, const_iterator j )
   {
      Insert( maxPos, i, j );
   }

   /*!
    * Appends a copy of the first \a n characters of a null-terminated
    * character sequence \a t to this string.
    */
   void Append( const_c_string t, size_type n )
   {
      Insert( maxPos, t, n );
   }

   /*!
    * Appends a copy of a null-terminated character sequence to this string.
    */
   void Append( const_c_string t )
   {
      Insert( maxPos, t );
   }

   /*!
    * Appends a copy of a null-terminated string by calling Append( t ).
    * Returns a reference to this object.
    */
   GenericString& operator +=( const_c_string t )
   {
      Append( t );
      return *this;
   }

   /*!
    * Appends \a n copies of a character \a c to this string.
    */
   void Append( char_type c, size_type n = 1 )
   {
      Insert( maxPos, c, n );
   }

   /*!
    * A synonym for Append( const GenericString& ), provided for compatibility
    * with PCL container classes.
    */
   template <class R1, class A1>
   void Add( const GenericString<T,R1,A1>& s )
   {
      Append( s );
   }

   /*!
    * A synonym for Append( const_iterator, const_iterator ), provided for
    * compatibility with PCL container classes.
    */
   void Add( const_iterator i, const_iterator j )
   {
      Append( i, j );
   }

   /*!
    * A synonym for Append( const_c_string, size_type ), provided for compatibility
    * with PCL container classes.
    */
   void Add( const_c_string t, size_type n )
   {
      Append( t, n );
   }

   /*!
    * A synonym for Append( const_c_string ), provided for compatibility with PCL
    * container classes.
    */
   void Add( const_c_string t )
   {
      Append( t );
   }

   /*!
    * A synonym for Append( char_type, size_type ), provided for compatibility with PCL
    * container classes.
    */
   void Add( char_type c, size_type n = 1 )
   {
      Append( c, n );
   }

   /*!
    * Appends a copy of the specified character \a c to this string. Returns a
    * reference to this object.
    */
   GenericString& operator +=( char_type c )
   {
      Append( c );
      return *this;
   }

   /*!
    * Inserts a copy of the specified string \a s at the beginning of this
    * string.
    */
   template <class R1, class A1>
   void Prepend( const GenericString<T,R1,A1>& s )
   {
      Insert( 0, s );
   }

   /*!
    * Inserts a copy of a string \a s at the beginning of this string. Returns
    * a reference to this object.
    */
   template <class R1, class A1>
   GenericString& operator -=( const GenericString<T,R1,A1>& s )
   {
      Prepend( s );
      return *this;
   }

   /*!
    * Inserts a copy of the character sequence defined by the range [i,j) at
    * the beginning of this string.
    *
    * If \a i is greater than or equal to \a j, calling this member function
    * has no effect.
    */
   void Prepend( const_iterator i, const_iterator j )
   {
      Insert( 0, i, j );
   }

   /*!
    * Inserts a copy of the first \a n characters of a character array \a t at
    * the beginning of this string.
    */
   void Prepend( const_c_string t, size_type n )
   {
      Insert( 0, t, n );
   }

   /*!
    * Inserts a copy of the null-terminated character sequence \a t at the
    * beginning of this string.
    */
   void Prepend( const_c_string t )
   {
      Insert( 0, t );
   }

   /*!
    * Inserts a copy of the null-terminated sequence \a t at the beginning of
    * this string. Returns a reference to this object.
    */
   GenericString& operator -=( const_c_string t )
   {
      Prepend( t );
      return *this;
   }

   /*!
    * Inserts \a n copies of a character \a c at the beginning of this string.
    */
   void Prepend( char_type c, size_type n = 1 )
   {
      Insert( 0, c, n );
   }

   /*!
    * Inserts a copy of the specified character \a c at the beginning of this
    * string. Returns a reference to this object.
    */
   GenericString& operator -=( char_type c )
   {
      Prepend( c );
      return *this;
   }

   /*!
    * Replaces a segment of \a n contiguous characters, starting at the \a i-th
    * character in this string, with a copy of a string \a s.
    */
   template <class R1, class A1>
   void Replace( size_type i, size_type n, const GenericString<T,R1,A1>& s )
   {
      if ( n > 0 )
      {
         size_type len = Length();
         if ( i < len )
         {
            n = pcl::Min( n, len-i );
            if ( n != len )
            {
               size_type ns = s.Length();
               if ( ns > 0 )
               {
                  if ( n < ns )
                     UninitializedGrow( i, ns-n );
                  else if ( ns < n )
                     Delete( i, n-ns );
                  else
                     EnsureUnique();

                  R::Copy( m_data->string+i, s.m_data->string, ns );
               }
               else
                  Delete( i, n );
            }
            else
               Assign( s );
         }
      }
   }

   /*!
    * Replaces a segment of at most \a n contiguous characters, starting at the
    * \a i-th character in this string, with a copy of a null-terminated
    * sequence \a t.
    *
    * If \a t is a pointer to the contents of this string (or, equivalently, an
    * iterator on this string), this function invokes undefined behavior.
    */
   void Replace( size_type i, size_type n, const_c_string t )
   {
      if ( n > 0 )
      {
         size_type len = Length();
         if ( i < len )
         {
            n = pcl::Min( n, len-i );
            if ( n != len )
            {
               size_type nt = R::Length( t );
               if ( nt > 0 )
               {
                  if ( n < nt )
                     UninitializedGrow( i, nt-n );
                  else if ( nt < n )
                     Delete( i, n-nt );
                  else
                     EnsureUnique();

                  R::Copy( m_data->string+i, t, nt );
               }
               else
                  Delete( i, n );
            }
            else
               Assign( t );
         }
      }
   }

   /*!
    * Replaces a segment of at most \a n contiguous characters, starting at the
    * \a i-th character in this string, with \a nc copies of a character \a c.
    */
   void Replace( size_type i, size_type n, char_type c, size_type nc = 1 )
   {
      if ( n > 0 )
      {
         size_type len = Length();
         if ( i < len )
         {
            n = pcl::Min( n, len-i );
            if ( n != len )
            {
               if ( nc > 0 )
               {
                  if ( n < nc )
                     UninitializedGrow( i, nc-n );
                  else if ( nc < n )
                     Delete( i, n-nc );
                  else
                     EnsureUnique();

                  R::Fill( m_data->string+i, c, nc );
               }
               else
                  Delete( i, n );
            }
            else
               Assign( c, nc );
         }
      }
   }

   /*!
    * Replaces all occurrences of a character \a c1 with \a c2 in a segment of
    * \a n contiguous characters starting at the \a i-th character in this
    * string.
    */
   void ReplaceChar( char_type c1, char_type c2, size_type i = 0, size_type n = maxPos )
   {
      ReplaceChar( c1, c2, i, n, true/*case*/ );
   }

   /*!
    * Replaces all occurrences of a character \a c1 with \a c2 in a segment of
    * \a n contiguous characters starting at the \a i-th character in this
    * string.
    *
    * This member function performs case-insensitive character comparisons.
    */
   void ReplaceCharIC( char_type c1, char_type c2, size_type i = 0, size_type n = maxPos )
   {
      ReplaceChar( c1, c2, i, n, false/*case*/ );
   }

   /*!
    * Replaces all occurrences of a string \a s1 with \a s2 in a segment of
    * contiguous characters starting at the \a i-th character, and spanning to
    * the end of this string.
    */
   template <class R1, class A1, class R2, class A2>
   void ReplaceString( const GenericString<T,R1,A1>& s1,
                       const GenericString<T,R2,A2>& s2, size_type i = 0 )
   {
      ReplaceString( s1.m_data->string, s1.Length(), s2.m_data->string, s2.Length(), i, true/*case*/ );
   }

   /*!
    * Replaces all occurrences of a null-terminated string \a t1 with \a t2 in
    * a segment of contiguous characters starting at the \a i-th character, and
    * spanning to the end of this string.
    */
   void ReplaceString( const_c_string t1, const_c_string t2, size_type i = 0 )
   {
      ReplaceString( t1, R::Length( t1 ), t2, R::Length( t2 ), i, true/*case*/ );
   }

   /*!
    * Replaces all occurrences of a string \a s1 with \a s2 in a segment of
    * contiguous characters starting at the \a i-th character, and spanning to
    * the end of this string.
    *
    * This member function performs case-insensitive string comparisons.
    */
   template <class R1, class A1, class R2, class A2>
   void ReplaceStringIC( const GenericString<T,R1,A1>& s1,
                         const GenericString<T,R2,A2>& s2, size_type i = 0 )
   {
      ReplaceString( s1.m_data->string, s1.Length(), s2.m_data->string, s2.Length(), i, false/*case*/ );
   }

   /*!
    * Replaces all occurrences of a null-terminated string \a t1 with \a t2 in
    * a segment of contiguous characters starting at the \a i-th character, and
    * spanning to the end of this string.
    *
    * This member function performs case-insensitive string comparisons.
    */
   void ReplaceStringIC( const_c_string t1, const_c_string t2, size_type i = 0 )
   {
      ReplaceString( t1, R::Length( t1 ), t2, R::Length( t2 ), i, false/*case*/ );
   }

   /*!
    * Deletes a contiguous segment of at most \a n characters starting at the
    * \a i-th character in this string.
    */
   void Delete( size_type i, size_type n = 1 )
   {
      if ( n > 0 )
      {
         size_type len = Length();
         if ( i < len )
         {
            n = pcl::Min( n, len-i );
            if ( n < len )
            {
               size_type newLen = len-n;
               if ( !IsUnique() || m_data->ShouldReallocate( newLen ) )
               {
                  Data* newData = Data::New( newLen );
                  if ( i > 0 )
                     R::Copy( newData->string, m_data->string, i );
                  if ( i < newLen )
                     R::Copy( newData->string+i, m_data->string+i+n, newLen-i );
                  DetachFromData();
                  m_data = newData;
               }
               else
               {
                  if ( i < newLen )
                     R::CopyOverlapped( m_data->string+i, m_data->string+i+n, newLen-i );
                  m_data->SetLength( newLen );
               }
            }
            else
               Clear();
         }
      }
   }

   /*!
    * Deletes a segment of contiguous characters starting from the \a i-th
    * character and spanning to the end of this string.
    */
   void DeleteRight( size_type i )
   {
      Delete( i, maxPos );
   }

   /*!
    * Deletes the initial segment of at most \a i contiguous characters in this
    * string.
    */
   void DeleteLeft( size_type i )
   {
      Delete( 0, i );
   }

   /*!
    * Deletes all occurrences of the \a c character in a segment of contiguous
    * characters starting from the \a i-th character and spanning to the end of
    * this string.
    */
   void DeleteChar( char_type c, size_type i = 0 )
   {
      ReplaceString( &c, 1, nullptr, 0, i, true/*case*/ );
   }

   /*!
    * Deletes all occurrences of the \a c character in a segment of contiguous
    * characters starting from the \a i-th character and spanning to the end of
    * this string.
    *
    * This member function performs case-insensitive character comparisons.
    */
   void DeleteCharIC( char_type c, size_type i = 0 )
   {
      ReplaceString( &c, 1, nullptr, 0, i, false/*case*/ );
   }

   /*!
    * Deletes all occurrences of a string \a s in a segment of contiguous
    * characters starting from the \a i-th character and spanning to the end of
    * this string.
    */
   template <class R1, class A1>
   void DeleteString( const GenericString<T,R1,A1>& s, size_type i = 0 )
   {
      ReplaceString( s.m_data->string, s.Length(), nullptr, 0, i, true/*case*/ );
   }

   /*!
    * Deletes all occurrences of a null-terminated string \a s in a segment of
    * contiguous characters starting from the \a i-th character and spanning to
    * the end of this string.
    */
   void DeleteString( const_c_string t, size_type i = 0 )
   {
      ReplaceString( t, R::Length( t ), nullptr, 0, i, true/*case*/ );
   }

   /*!
    * Deletes all occurrences of a string \a s in a segment of contiguous
    * characters starting from the \a i-th character and spanning to the end of
    * this string.
    *
    * This member function performs case-insensitive string comparisons.
    */
   template <class R1, class A1>
   void DeleteStringIC( const GenericString<T,R1,A1>& s, size_type i = 0 )
   {
      ReplaceString( s.m_data->string, s.Length(), nullptr, 0, i, false/*case*/ );
   }

   /*!
    * Deletes all occurrences of a null-terminated string \a s in a segment of
    * contiguous characters starting from the \a i-th character and spanning to
    * the end of this string.
    *
    * This member function performs case-insensitive string comparisons.
    */
   void DeleteStringIC( const_c_string t, size_type i = 0 )
   {
      ReplaceString( t, R::Length( t ), nullptr, 0, i, false/*case*/ );
   }

   /*!
    * Removes all characters in this string, yielding an empty string.
    *
    * If this string is empty, then calling this member function has no effect.
    *
    * If this string uniquely references its string data, it is destroyed and
    * deallocated; otherwise its reference counter is decremented. Then a new,
    * empty string data structure is created and uniquely referenced.
    */
   void Clear()
   {
      if ( !IsEmpty() )
         if ( IsUnique() )
            m_data->Deallocate();
         else
         {
            Data* newData = Data::New();
            DetachFromData();
            m_data = newData;
         }
   }

   /*!
    * Returns a string with a copy of at most \a n contiguous characters
    * starting at the \a i-th character in this string.
    */
   GenericString Substring( size_type i, size_type n = maxPos ) const
   {
      size_type len = Length();
      if ( i < len )
      {
         n = pcl::Min( n, len-i );
         if ( n > 0 )
         {
            if ( n < len )
            {
               GenericString t;
               t.m_data->Allocate( n );
               R::Copy( t.m_data->string, m_data->string+i, n );
               return t;
            }
            return *this;
         }
      }
      return GenericString();
   }

   /*!
    * Returns a string with a copy of the at most \a n contiguous initial
    * characters of this string.
    */
   GenericString Left( size_type n ) const
   {
      size_type len = Length();
      n = pcl::Min( n, len );
      if ( n > 0 )
      {
         if ( n < len )
         {
            GenericString t;
            t.m_data->Allocate( n );
            R::Copy( t.m_data->string, m_data->string, n );
            return t;
         }
         return *this;
      }
      return GenericString();
   }

   /*!
    * Returns a string with a copy of the at most \a n contiguous ending
    * characters of this string.
    */
   GenericString Right( size_type n ) const
   {
      size_type len = Length();
      n = pcl::Min( n, len );
      if ( n > 0 )
      {
         if ( n < len )
         {
            GenericString t;
            t.m_data->Allocate( n );
            R::Copy( t.m_data->string, m_data->string+len-n, n );
            return t;
         }
         return *this;
      }
      return GenericString();
   }

   /*!
    * Returns the suffix of this string starting at index \a i. Calling this
    * member function is equivalent to:
    *
    * \code Right( Length()-i ) \endcode
    */
   GenericString Suffix( size_type i ) const
   {
      return Substring( i );
   }

   /*!
    * Returns the prefix of this string ending at index \a i. Calling this
    * member function is equivalent to:
    *
    * \code Left( i-1 ) \endcode
    */
   GenericString Prefix( size_type i ) const
   {
      return Left( (i > 0) ? i-1 : 0 );
   }

   /*!
    * Gets a sequence of \e tokens (substrings) extracted from this string.
    *
    * \param[out] list  The list of extracted tokens. Must be a reference to a
    *                container, such as Array or List, or a derived class.
    *                Typically, this parameter is a reference to a StringList.
    *
    * \param s       The token separator string.
    *
    * \param trim    True to \e trim the extracted tokens. If this parameter is
    *                true, existing leading and trailing whitespace characters
    *                will be removed from each extracted token.
    *
    * \param i       Starting character index.
    *
    * Returns the number of tokens extracted and added to the list.
    */
   template <class C, class R1, class A1>
   size_type Break( C& list, const GenericString<T,R1,A1>& s, bool trim = false, size_type i = 0 ) const
   {
      size_type count = 0;
      size_type len = Length();
      if ( i < len )
      {
         size_type n = s.Length();
         if ( n > 0 )
            for ( SearchEngine S( s.m_data->string, n, true/*case*/ ); ; )
            {
               size_type j = S( m_data->string, i, len );

               GenericString t;
               if ( i < j )
               {
                  const_iterator l = m_data->string + i;
                  size_type m = j - i;
                  if ( trim )
                  {
                     const_iterator r = l + m;
                     l = R::SearchTrimLeft( l, r );
                     m = R::SearchTrimRight( l, r ) - l;
                  }
                  if ( m > 0 )
                  {
                     t.m_data->Allocate( m );
                     R::Copy( t.m_data->string, l, m );
                  }
               }
               list.Add( t );
               ++count;

               if ( j == len )
                  break;

               i = j + n;
            }
      }
      return count;
   }

   /*!
    * Gets a sequence of \e tokens (substrings) extracted from this string.
    *
    * \param[out] list  The list of extracted tokens. Must be a reference to a
    *                   container, such as Array or List, or a derived class.
    *                   Typically, this parameter is a reference to a
    *                   StringList.
    *
    * \param s          The token separator null-terminated string.
    *
    * \param trim       True to \e trim the extracted tokens. If this parameter
    *                   is true, existing leading and trailing whitespace
    *                   characters will be removed from each extracted token.
    *
    * \param i          Starting character index.
    *
    * Returns the number of tokens extracted and added to the list.
    */
   template <class C>
   size_type Break( C& list, const_c_string s, bool trim = false, size_type i = 0 ) const
   {
      size_type count = 0;
      size_type len = Length();
      if ( i < len )
      {
         size_type n = R::Length( s );
         if ( n > 0 )
            for ( SearchEngine S( s, n, true/*case*/ ); ; )
            {
               size_type j = S( m_data->string, i, len );

               GenericString t;
               if ( i < j )
               {
                  const_iterator l = m_data->string + i;
                  size_type m = j - i;
                  if ( trim )
                  {
                     const_iterator r = l + m;
                     l = R::SearchTrimLeft( l, r );
                     m = R::SearchTrimRight( l, r ) - l;
                  }
                  if ( m > 0 )
                  {
                     t.m_data->Allocate( m );
                     R::Copy( t.m_data->string, l, m );
                  }
               }
               list.Add( t );
               ++count;

               if ( j == len )
                  break;

               i = j + n;
            }
      }
      return count;
   }

   /*!
    * Gets a sequence of \e tokens (substrings) extracted from this string.
    *
    * \param[out] list  The list of extracted tokens. Must be a reference to a
    *                container, such as Array or List, or a derived class.
    *                Typically, this parameter is a reference to a StringList.
    *
    * \param c       The token separator character. Tokens will be separated by
    *                sequences of one or more instances of this character.
    *
    * \param trim    True to \e trim the extracted tokens. If this parameter is
    *                true, existing leading and trailing whitespace characters
    *                will be removed from each extracted token.
    *
    * \param i       Starting character index.
    *
    * Returns the number of tokens extracted and added to the list.
    */
   template <class C>
   size_type Break( C& list, char_type c, bool trim = false, size_type i = 0 ) const
   {
      size_type count = 0;
      size_type len = Length();
      if ( i < len )
      {
         for ( ;; )
         {
            size_type j = i;
            for ( const_iterator p = m_data->string + i; j < len; ++j, ++p )
               if ( *p == c )
                  break;

            GenericString t;
            if ( i < j )
            {
               const_iterator l = m_data->string + i;
               size_type m = j - i;
               if ( trim )
               {
                  const_iterator r = l + m;
                  l = R::SearchTrimLeft( l, r );
                  m = R::SearchTrimRight( l, r ) - l;
               }
               if ( m > 0 )
               {
                  t.m_data->Allocate( m );
                  R::Copy( t.m_data->string, l, m );
               }
            }
            list.Add( t );
            ++count;

            if ( j == len )
               break;

            i = j + 1;
         }
      }
      return count;
   }

   /*!
    * Gets a sequence of \e tokens (substrings) extracted from this string by
    * performing case-insensitive comparisons with a token separation string.
    *
    * \param[out] list  The list of extracted tokens. Must be a reference to a
    *                container, such as Array or List, or a derived class.
    *                Typically, this parameter is a reference to a StringList.
    *
    * \param s       The token separator string. This function performs
    *                case-insensitive string comparisons to locate instances of
    *                this string.
    *
    * \param trim    True to \e trim the extracted tokens. If this parameter is
    *                true, existing leading and trailing whitespace characters
    *                will be removed from each extracted token.
    *
    * \param i       Starting character index.
    *
    * Returns the number of tokens extracted and added to the list.
    */
   template <class C, class R1, class A1>
   size_type BreakIC( C& list, const GenericString<T,R1,A1>& s, bool trim = false, size_type i = 0 ) const
   {
      size_type count = 0;
      size_type len = Length();
      if ( i < len )
      {
         size_type n = s.Length();
         if ( n > 0 )
         {
            for ( SearchEngine S( s.m_data->string, n, false/*case*/ ); ; )
            {
               size_type j = S( m_data->string, i, len );

               GenericString t;
               if ( i < j )
               {
                  const_iterator l = m_data->string + i;
                  size_type m = j - i;
                  if ( trim )
                  {
                     const_iterator r = l + m;
                     l = R::SearchTrimLeft( l, r );
                     m = R::SearchTrimRight( l, r ) - l;
                  }
                  if ( m > 0 )
                  {
                     t.m_data->Allocate( m );
                     R::Copy( t.m_data->string, l, m );
                  }
               }
               list.Add( t );
               ++count;

               if ( j == len )
                  break;

               i = j + n;
            }
         }
      }
      return count;
   }

   /*!
    * Gets a sequence of \e tokens (substrings) extracted from this string by
    * performing case-insensitive comparisons with a null-terminated token
    * separation string.
    *
    * \param[out] list  The list of extracted tokens. Must be a reference to a
    *                   container, such as Array or List, or a derived class.
    *                   Typically, this parameter is a reference to a
    *                   StringList.
    *
    * \param s          The token separator null-terminated string. This
    *                   function performs case-insensitive string comparisons
    *                   to locate instances of this string.
    *
    * \param trim       True to \e trim the extracted tokens. If this parameter
    *                   is true, existing leading and trailing whitespace
    *                   characters will be removed from each extracted token.
    *
    * \param i          Starting character index.
    *
    * Returns the number of tokens extracted and added to the list.
    */
   template <class C>
   size_type BreakIC( C& list, const_c_string s, bool trim = false, size_type i = 0 ) const
   {
      size_type count = 0;
      size_type len = Length();
      if ( i < len )
      {
         size_type n = R::Length( s );
         if ( n > 0 )
         {
            for ( SearchEngine S( s, n, false/*case*/ ); ; )
            {
               size_type j = S( m_data->string, i, len );

               GenericString t;
               if ( i < j )
               {
                  const_iterator l = m_data->string + i;
                  size_type m = j - i;
                  if ( trim )
                  {
                     const_iterator r = l + m;
                     l = R::SearchTrimLeft( l, r );
                     m = R::SearchTrimRight( l, r ) - l;
                  }
                  if ( m > 0 )
                  {
                     t.m_data->Allocate( m );
                     R::Copy( t.m_data->string, l, m );
                  }
               }
               list.Add( t );
               ++count;

               if ( j == len )
                  break;

               i = j + n;
            }
         }
      }
      return count;
   }

   /*!
    * Gets a sequence of \e tokens (substrings) extracted from this string by
    * performing case-insensitive comparisons with a token separator character.
    *
    * \param[out] list  The list of extracted tokens. Must be a reference to a
    *                container, such as Array or List, or a derived class.
    *                Typically, this parameter is a reference to a StringList.
    *
    * \param c       The token separator character. Tokens will be separated by
    *                sequences of one or more instances of this character. This
    *                function performs case-insensitive character comparisons
    *                to locate instances of this character.
    *
    * \param trim    True to \e trim the extracted tokens. If this parameter is
    *                true, existing leading and trailing whitespace characters
    *                will be removed from each extracted token.
    *
    * \param i       Starting character index.
    *
    * Returns the number of tokens extracted and added to the list.
    */
   template <class C>
   size_type BreakIC( C& list, char_type c, bool trim = false, size_type i = 0 ) const
   {
      size_type count = 0;
      size_type len = Length();
      if ( i < len )
      {
         c = R::ToCaseFolded( c );
         for ( ;; )
         {
            size_type j = i;
            for ( const_iterator p = m_data->string + i; j < len; ++j, ++p )
               if ( R::ToCaseFolded( *p ) == c )
                  break;

            GenericString t;
            if ( i < j )
            {
               const_iterator l = m_data->string + i;
               size_type m = j - i;
               if ( trim )
               {
                  const_iterator r = l + m;
                  l = R::SearchTrimLeft( l, r );
                  m = R::SearchTrimRight( l, r ) - l;
               }
               if ( m > 0 )
               {
                  t.m_data->Allocate( m );
                  R::Copy( t.m_data->string, l, m );
               }
            }
            list.Add( t );
            ++count;

            if ( j == len )
               break;

            i = j + 1;
         }
      }
      return count;
   }

   /*!
    * Returns the first character in this string, or a <em>null character</em>
    * (R::Null()) if this string is empty.
    */
   char_type FirstChar() const
   {
      return (m_data->string != nullptr) ? *m_data->string : R::Null();
   }

   /*!
    * Returns the last character in this string, or a <em>null character</em>
    * (R::Null()) if this string is empty.
    */
   char_type LastChar() const
   {
      return (m_data->string < m_data->end) ? *(m_data->end-1) : R::Null();
   }

   /*!
    * Returns true iff this string begins with the specified substring \a s.
    */
   template <class R1, class A1>
   bool StartsWith( const GenericString<T,R1,A1>& s ) const
   {
      if ( s.IsEmpty() || Length() < s.Length() )
         return false;
      for ( const_iterator p = m_data->string, q = s.m_data->string; q < s.m_data->end; ++p, ++q )
         if ( *p != *q )
            return false;
      return true;
   }

   /*!
    * Returns true iff this string begins with the specified null-terminated
    * sequence \a t.
    */
   bool StartsWith( const_c_string t ) const
   {
      size_type n = R::Length( t );
      if ( n == 0 || Length() < n )
         return false;
      for ( const_iterator p = m_data->string, e = p+n; p < e; ++p, ++t )
         if ( *p != *t )
            return false;
      return true;
   }

   /*!
    * Returns true iff this string begins with the specified character \a c.
    */
   bool StartsWith( char_type c ) const
   {
      return m_data->string < m_data->end && *m_data->string == c;
   }

   /*!
    * Returns true iff this string begins with the specified substring \a s,
    * performing case-insensitive character comparisons.
    */
   template <class R1, class A1>
   bool StartsWithIC( const GenericString<T,R1,A1>& s ) const
   {
      if ( s.IsEmpty() || Length() < s.Length() )
         return false;
      for ( const_iterator p = m_data->string, q = s.m_data->string; q < s.m_data->end; ++p, ++q )
         if ( R::ToCaseFolded( *p ) != R::ToCaseFolded( *q ) )
            return false;
      return true;
   }

   /*!
    * Returns true iff this string begins with the specified null-terminated
    * sequence \a t, performing case-insensitive character comparisons.
    */
   bool StartsWithIC( const_c_string t ) const
   {
      size_type n = R::Length( t );
      if ( n == 0 || Length() < n )
         return false;
      for ( const_iterator p = m_data->string, e = p+n; p < e; ++p, ++t )
         if ( R::ToCaseFolded( *p ) != R::ToCaseFolded( *t ) )
            return false;
      return true;
   }

   /*!
    * Returns true iff this string begins with the specified character \a c,
    * performing a case-insensitive character comparison.
    */
   bool StartsWithIC( char_type c ) const
   {
      return m_data->string < m_data->end && R::ToCaseFolded( *m_data->string ) == R::ToCaseFolded( c );
   }

   /*!
    * Returns true iff this string ends with the specified substring \a s.
    */
   template <class R1, class A1>
   bool EndsWith( const GenericString<T,R1,A1>& s ) const
   {
      size_type n = s.Length();
      if ( n == 0 || Length() < n )
         return false;
      for ( const_iterator p = m_data->end-n, q = s.m_data->string; p < m_data->end; ++p, ++q )
         if ( *p != *q )
            return false;
      return true;
   }

   /*!
    * Returns true iff this string ends with the specified null-terminated
    * string \a t.
    */
   bool EndsWith( const_c_string t ) const
   {
      size_type n = R::Length( t );
      if ( n == 0 || Length() < n )
         return false;
      for ( const_iterator p = m_data->end-n; p < m_data->end; ++p, ++t )
         if ( *p != *t )
            return false;
      return true;
   }

   /*!
    * Returns true iff this string ends with the specified character \a c.
    */
   bool EndsWith( char_type c ) const
   {
      return m_data->string < m_data->end && *(m_data->end-1) == c;
   }

   /*!
    * Returns true iff this string ends with the specified substring \a s,
    * performing case-insensitive character comparisons.
    */
   template <class R1, class A1>
   bool EndsWithIC( const GenericString<T,R1,A1>& s ) const
   {
      size_type n = s.Length();
      if ( n == 0 || Length() < n )
         return false;
      for ( const_iterator p = m_data->end-n, q = s.m_data->string; p < m_data->end; ++p, ++q )
         if ( R::ToCaseFolded( *p ) != R::ToCaseFolded( *q ) )
            return false;
      return true;
   }

   /*!
    * Returns true iff this string ends with the specified null-terminated
    * string \a t, performing case-insensitive character comparisons.
    */
   bool EndsWithIC( const_c_string t ) const
   {
      size_type n = R::Length( t );
      if ( n == 0 || Length() < n )
         return false;
      for ( const_iterator p = m_data->end-n; p < m_data->end; ++p, ++t )
         if ( R::ToCaseFolded( *p ) != R::ToCaseFolded( *t ) )
            return false;
      return true;
   }

   /*!
    * Returns true iff this string ends with the specified character \a c,
    * performing a case-insensitive character comparison.
    */
   bool EndsWithIC( char_type c ) const
   {
      return m_data->string < m_data->end && R::ToCaseFolded( *(m_data->end-1) ) == R::ToCaseFolded( c );
   }

   /*!
    * Returns the starting index \a k of the first occurrence of a substring
    * \a s in this string, such that \a k >= \a i. Returns notFound if such
    * occurrence does not exist.
    */
   template <class R1, class A1>
   size_type FindFirst( const GenericString<T,R1,A1>& s, size_type i = 0 ) const
   {
      size_type len = Length();
      i = SearchEngine( s.m_data->string, s.Length(), true/*case*/ )( m_data->string, i, len );
      return (i < len) ? i : notFound;
   }

   /*!
    * Returns the starting index \a k of the first occurrence of a
    * null-terminated substring \a t in this string, such that \a k >= \a i.
    * Returns notFound if such occurrence does not exist.
    */
   size_type FindFirst( const_c_string t, size_type i = 0 ) const
   {
      size_type len = Length();
      i = SearchEngine( t, R::Length( t ), true/*case*/ )( m_data->string, i, len );
      return (i < len) ? i : notFound;
   }

   /*!
    * Returns the index \a k of the first occurrence of a character \a c in
    * this string, such that \a k >= \a i. Returns notFound if such occurrence
    * does not exist.
    */
   size_type FindFirst( char_type c, size_type i = 0 ) const
   {
      for ( const_iterator p = m_data->string+i; p < m_data->end; ++p )
         if ( *p == c )
            return p - m_data->string;
      return notFound;
   }

   /*!
    * Returns the starting index \a k of the first occurrence of a substring
    * \a s in this string, such that \a k >= \a i. Returns notFound if such
    * occurrence does not exist.
    *
    * This member function performs case-insensitive string comparisons to find
    * an instance of the substring \a s.
    */
   template <class R1, class A1>
   size_type FindFirstIC( const GenericString<T,R1,A1>& s, size_type i = 0 ) const
   {
      size_type len = Length();
      i = SearchEngine( s.m_data->string, s.Length(), false/*case*/ )( m_data->string, i, len );
      return (i < len) ? i : notFound;
   }

   /*!
    * Returns the starting index \a k of the first occurrence of a
    * null-terminated substring \a s in this string, such that \a k >= \a i.
    * Returns notFound if such occurrence does not exist.
    *
    * This member function performs case-insensitive string comparisons to find
    * an instance of the substring \a s.
    */
   size_type FindFirstIC( const_c_string t, size_type i = 0 ) const
   {
      size_type len = Length();
      i = SearchEngine( t, R::Length( t ), false/*case*/ )( m_data->string, i, len );
      return (i < len) ? i : notFound;
   }

   /*!
    * Returns the index \a k of the first occurrence of a character \a c in
    * this string, such that \a k >= \a i. Returns notFound if such occurrence
    * does not exist.
    *
    * This member function performs case-insensitive character comparisons to
    * find an instance of the character \a c.
    */
   size_type FindFirstIC( char_type c, size_type i = 0 ) const
   {
      c = R::ToCaseFolded( c );
      for ( const_iterator p = m_data->string+i; p < m_data->end; ++p )
         if ( R::ToCaseFolded( *p ) == c )
            return p - m_data->string;
      return notFound;
   }

   /*!
    * A synonym for FindFirst( const GenericString<T,R1,A1>&, size_type ).
    */
   template <class R1, class A1>
   size_type Find( const GenericString<T,R1,A1>& s, size_type i = 0 ) const
   {
      return FindFirst( s, i );
   }

   /*!
    * A synonym for FindFirst( const_c_string, size_type ).
    */
   size_type Find( const_c_string t, size_type i = 0 ) const
   {
      return FindFirst( t, i );
   }

   /*!
    * A synonym for FindFirst( char_type, size_type ).
    */
   size_type Find( char_type c, size_type i = 0 ) const
   {
      return FindFirst( c, i );
   }

   /*!
    * A synonym for FindFirstIC( const GenericString<T,R1,A1>&, size_type ).
    */
   template <class R1, class A1>
   size_type FindIC( const GenericString<T,R1,A1>& s, size_type i = 0 ) const
   {
      return FindFirstIC( s, i );
   }

   /*!
    * A synonym for FindFirstIC( const_c_string, size_type ).
    */
   size_type FindIC( const_c_string t, size_type i = 0 ) const
   {
      return FindFirstIC( t, i );
   }

   /*!
    * A synonym for FindFirstIC( char_type, size_type ).
    */
   size_type FindIC( char_type c, size_type i = 0 ) const
   {
      return FindFirstIC( c, i );
   }

   /*!
    * Returns the starting index \a k of the last occurrence of a substring
    * \a s in this string, such that:
    *
    * \a k <= \a r - \a n,
    *
    * where \a n is the length of the substring \a s. Returns notFound if such
    * occurrence does not exist.
    */
   template <class R1, class A1>
   size_type FindLast( const GenericString<T,R1,A1>& s, size_type r = maxPos ) const
   {
      r = pcl::Min( r, Length() );
      size_type i = SearchEngine( s.m_data->string, s.Length(), true/*case*/, true/*last*/ )( m_data->string, 0, r );
      return (i < r) ? i : notFound;
   }

   /*!
    * Returns the starting index \a k of the last occurrence of a
    * null-terminated substring \a t in this string, such that:
    *
    * 0 <= \a k <= \a r - \a n,
    *
    * where \a n is the length of the substring \a t. Returns notFound if such
    * occurrence does not exist.
    */
   size_type FindLast( const_c_string t, size_type r = maxPos ) const
   {
      r = pcl::Min( r, Length() );
      size_type i = SearchEngine( t, R::Length( t ), true/*case*/, true/*last*/ )( m_data->string, 0, r );
      return (i < r) ? i : notFound;
   }

   /*!
    * Returns the index \a k of the last occurrence of a character \a c in this
    * string, such that \a k < \a r. Returns notFound if such occurrence does
    * not exist.
    */
   size_type FindLast( char_type c, size_type r = maxPos ) const
   {
      r = pcl::Min( r, Length() );
      for ( const_iterator p = m_data->string+r; r > 0; --r )
         if ( *--p == c )
            return r - 1;
      return notFound;
   }

   /*!
    * Returns the starting index \a k of the last occurrence of a substring
    * \a s in this string, such that:
    *
    * \a k <= \a r - \a n,
    *
    * where \a n is the length of the substring \a s. Returns notFound if such
    * occurrence does not exist.
    *
    * This member function performs case-insensitive string comparisons to
    * find an instance of the specified substring \a s.
    */
   template <class R1, class A1>
   size_type FindLastIC( const GenericString<T,R1,A1>& s, size_type r = maxPos ) const
   {
      r = pcl::Min( r, Length() );
      size_type i = SearchEngine( s.m_data->string, s.Length(), false/*case*/, true/*last*/ )( m_data->string, 0, r );
      return (i < r) ? i : notFound;
   }

   /*!
    * Returns the starting index \a k of the last occurrence of a
    * null-terminated substring \a t in this string, such that:
    *
    * 0 <= \a k <= \a r - \a n,
    *
    * where \a n is the length of the substring \a t. Returns notFound if such
    * occurrence does not exist.
    *
    * This member function performs case-insensitive string comparisons to
    * find an instance of the specified substring \a t.
    */
   size_type FindLastIC( const_c_string t, size_type r = maxPos ) const
   {
      r = pcl::Min( r, Length() );
      size_type i = SearchEngine( t, R::Length( t ), false/*case*/, true/*last*/ )( m_data->string, 0, r );
      return (i < r) ? i : notFound;
   }

   /*!
    * Returns the index \a k of the last occurrence of a character \a c in this
    * string, such that \a k < \a r. Returns notFound if such occurrence does
    * not exist.
    *
    * This member function performs case-insensitive character comparisons to
    * find an instance of the specified character \a c.
    */
   size_type FindLastIC( char_type c, size_type r = maxPos ) const
   {
      c = R::ToCaseFolded( c );
      r = pcl::Min( r, Length() );
      for ( const_iterator p = m_data->string+r; r > 0; --r )
         if ( R::ToCaseFolded( *--p ) == c )
            return r - 1;
      return notFound;
   }

   /*!
    * Returns true iff this string contains a substring \a s.
    */
   template <class R1, class A1>
   bool Contains( const GenericString<T,R1,A1>& s ) const
   {
      return Find( s ) != notFound;
   }

   /*!
    * Returns true iff this string contains a null-terminated substring \a t.
    */
   bool Contains( const_c_string t ) const
   {
      return Find( t ) != notFound;
   }

   /*!
    * Returns true iff this string contains a character \a c.
    */
   bool Contains( char_type c ) const
   {
      return Find( c ) != notFound;
   }

   /*!
    * Returns true iff this string contains a substring \a s.
    *
    * This member function performs case-insensitive string comparisons to
    * find an instance of the specified substring \a s.
    */
   template <class R1, class A1>
   bool ContainsIC( const GenericString<T,R1,A1>& s ) const
   {
      return FindIC( s ) != notFound;
   }

   /*!
    * Returns true iff this string contains a null-terminated substring \a t.
    *
    * This member function performs case-insensitive string comparisons to
    * find an instance of the specified substring \a t.
    */
   bool ContainsIC( const_c_string t ) const
   {
      return FindIC( t ) != notFound;
   }

   /*!
    * Returns true iff this string contains a character \a c.
    *
    * This member function performs case-insensitive character comparisons to
    * find an instance of the specified character \a c.
    */
   bool ContainsIC( char_type c ) const
   {
      return FindIC( c ) != notFound;
   }

   /*!
    * Removes all leading and trailing <em>trimable characters</em>.
    *
    * Trimable characters are determined by the traits class R. A character
    * \a c is trimable if R::IsTrimable( c ) is true. Generally, the set of
    * trimable characters corresponds to the set of white space characters.
    *
    * \sa Trimmed();
    */
   void Trim()
   {
      const_iterator l = R::SearchTrimLeft( m_data->string, m_data->end );
      const_iterator r = R::SearchTrimRight( l, m_data->end );
      if ( m_data->string < l || r < m_data->end )
         Trim( l, r - l );
   }

   /*!
    * Removes all leading <em>trimable characters</em>.
    *
    * Trimable characters are determined by the traits class R. A character
    * \a c is trimable if R::IsTrimable( c ) is true. Generally, the set of
    * trimable characters corresponds to the set of white space characters.
    *
    * \sa TrimmedLeft();
    */
   void TrimLeft()
   {
      const_iterator l = R::SearchTrimLeft( m_data->string, m_data->end );
      if ( m_data->string < l )
         Trim( l, m_data->end - l );
   }

   /*!
    * Removes all trailing <em>trimable characters</em>.
    *
    * Trimable characters are determined by the traits class R. A character
    * \a c is trimable if R::IsTrimable( c ) is true. Generally, the set of
    * trimable characters corresponds to the set of white space characters.
    *
    * \sa TrimmedRight();
    */
   void TrimRight()
   {
      const_iterator r = R::SearchTrimRight( m_data->string, m_data->end );
      if ( r < m_data->end )
         Trim( m_data->string, r - m_data->string );
   }

   /*!
    * Returns a duplicate of this string with all leading and trailing
    * <em>trimable characters</em> removed.
    * \sa Trim()
    */
   GenericString Trimmed() const
   {
      GenericString s( *this );
      s.Trim();
      return s;
   }

   /*!
    * Returns a duplicate of this string with all leading <em>trimable
    * characters</em> removed.
    * \sa TrimLeft()
    */
   GenericString TrimmedLeft() const
   {
      GenericString s( *this );
      s.TrimLeft();
      return s;
   }

   /*!
    * Returns a duplicate of this string with all trailing <em>trimable
    * characters</em> removed.
    * \sa TrimRight()
    */
   GenericString TrimmedRight() const
   {
      GenericString s( *this );
      s.TrimRight();
      return s;
   }

   /*!
    * Unquotes this string.
    *
    * If the string starts and ends with single quote
    * characters, the result is the same string with the quotes removed and its
    * length decremented by two. The same happens if the string starts and ends
    * with double quote characters.
    *
    * If the string does not start and end with the same quote character, this
    * function has no effect.
    *
    * \sa Unquoted()
    */
   void Unquote()
   {
      size_type len = Length();
      if ( len > 1 )
         if ( *m_data->string == R::SingleQuote() && *(m_data->end-1) == R::SingleQuote() ||
              *m_data->string == R::DoubleQuote() && *(m_data->end-1) == R::DoubleQuote() )
            if ( IsUnique() )
            {
               R::CopyOverlapped( m_data->string, m_data->string+1, len-2 );
               m_data->SetLength( len-2 );
            }
            else
            {
               Data* newData = Data::New( len-2 );
               R::Copy( newData->string, m_data->string+1, len-2 );
               DetachFromData();
               m_data = newData;
            }
   }

   /*!
    * Returns an unquoted duplicate of this string.
    * \sa Unquote()
    */
   GenericString Unquoted() const
   {
      GenericString s( *this );
      s.Unquote();
      return s;
   }

   /*!
    * Pads this string to the right, using the specified \a fill character, up
    * to the specified \a width.
    *
    * If the current length \e n of this string is less than the specified
    * \a width, \a width - \e n copies of the \a fill character will be
    * appended to the string. If the current length is greater than or equal to
    * \a width, this function has no effect.
    *
    * \sa JustifyRight(), JustifyCenter(), LeftJustified()
    */
   void JustifyLeft( size_type width, char_type fill = R::Blank() )
   {
      size_type len = Length();
      if ( len < width )
         Append( fill, width-len );
   }

   /*!
    * Pads this string to the left, using the specified \a fill character, up
    * to the specified \a width.
    *
    * If the current length \e n of this string is less than the specified
    * \a width, \a width - \e n copies of the \a fill character will be
    * prepended to the string. If the current length is greater than or equal to
    * \a width, this function has no effect.
    *
    * \sa JustifyLeft(), JustifyCenter(), RightJustified()
    */
   void JustifyRight( size_type width, char_type fill = R::Blank() )
   {
      size_type len = Length();
      if ( len < width )
         Prepend( fill, width-len );
   }

   /*!
    * Pads this string equally to the left and right, using the specified
    * \a fill character, up to the specified \a width.
    *
    * If the current length \e n of this string is less than the specified
    * \a width, let \e m = \a width - \e n, and let \e m2 = \e m/2. Then \e m2
    * copies of the \a fill character will be prepended to the string, and
    * \e m - \e m2 \a fill characters will be appended. If the current length
    * is greater than or equal to \a width, this function has no effect.
    *
    * \sa JustifyLeft(), JustifyRight(), CenterJustified()
    */
   void JustifyCenter( size_type width, char_type fill = R::Blank() )
   {
      size_type len = Length();
      if ( len < width )
      {
         size_type n = width-len;
         size_type n2 = n >> 1;
         Prepend( fill, n2 );
         Append( fill, n-n2 );
      }
   }

   /*!
    * Returns a duplicate of this string padded to the right, using the
    * specified \a fill character, up to the specified \a width.
    *
    * \sa JustifyLeft(), RightJustified(), CenterJustified()
    */
   GenericString LeftJustified( size_type width, char_type fill = R::Blank() ) const
   {
      GenericString s( *this );
      s.JustifyLeft( width, fill );
      return s;
   }

   /*!
    * Returns a duplicate of this string padded to the left, using the
    * specified \a fill character, up to the specified \a width.
    *
    * \sa JustifyRight(), LeftJustified(), CenterJustified()
    */
   GenericString RightJustified( size_type width, char_type fill = R::Blank() ) const
   {
      GenericString s( *this );
      s.JustifyRight( width, fill );
      return s;
   }

   /*!
    * Returns a duplicate of this string padded equally to the left and right,
    * using the specified \a fill character, up to the specified \a width.
    *
    * \sa JustifyCenter(), LeftJustified(), RightJustified()
    */
   GenericString CenterJustified( size_type width, char_type fill = R::Blank() ) const
   {
      GenericString s( *this );
      s.JustifyCenter( width, fill );
      return s;
   }

   /*!
    * Compares numeric character values between two strings.
    *
    * \param s                A string to which this string will be compared.
    *
    * \param caseSensitive    When true, a case-sensitive comparison is
    *                         performed; otherwise the comparison does not
    *                         distinguish between lowercase and uppercase
    *                         characters. The default value of this parameter
    *                         is true.
    *
    * Performs a character-to-character comparison of numeric character values
    * between this string and the specified string \a s, and returns an integer
    * that indicates the comparison result:
    *
    * \li  0 if both strings are equal.
    * \li +1 if this string postcedes the specified string \a s.
    * \li -1 if this string precedes the specified string \a s.
    *
    * This function ignores the current platform locale. It considers Unicode
    * code points exclusively. For case-insensitive comparisons, a standard
    * Unicode case folding transformation is applied to each character pair.
    */
   template <class R1, class A1>
   int CompareCodePoints( const GenericString<T,R1,A1>& s, bool caseSensitive = true ) const
   {
      return R::CompareCodePoints( m_data->string, Length(), s.m_data->string, s.Length(), caseSensitive );
   }

   /*!
    * Compares numeric character values a string and a null-terminated string.
    *
    * \param t                The starting address of a null-terminated string
    *                         to which this string will be compared.
    *
    * \param caseSensitive    When true, a case-sensitive comparison is
    *                         performed; otherwise the comparison does not
    *                         distinguish between lowercase and uppercase
    *                         characters. The default value of this parameter
    *                         is true.
    *
    * Performs a character-to-character comparison of numeric character values
    * between this string and the specified null-terminated string \a t, and
    * returns an integer that indicates the comparison result:
    *
    * \li  0 if both strings are equal.
    * \li +1 if this string postcedes the specified string \a t.
    * \li -1 if this string precedes the specified string \a t.
    *
    * This function ignores the current platform locale. It considers Unicode
    * code points exclusively. For case-insensitive comparisons, a standard
    * Unicode case folding transformation is applied to each character pair.
    */
   int CompareCodePoints( const_c_string t, bool caseSensitive = true ) const
   {
      return R::CompareCodePoints( m_data->string, Length(), t, R::Length( t ), caseSensitive );
   }

   /*!
    * Compares numeric character values a string and a null-terminated string.
    *
    * \param c                A single character to which this string will be
    *                         compared.
    *
    * \param caseSensitive    When true, a case-sensitive comparison is
    *                         performed; otherwise the comparison does not
    *                         distinguish between lowercase and uppercase
    *                         characters. The default value of this parameter
    *                         is true.
    *
    * Performs a comparison between this string and the specified character
    * \a c, and returns an integer that indicates the comparison result:
    *
    * \li  0 if this string is equal to \a c.
    * \li +1 if this string postcedes the character \a c.
    * \li -1 if this string precedes the character \a c.
    *
    * The performed comparison is equivalent to a comparison of this string
    * with a fictitious string of length one, whose only character was equal to
    * the specified character \a c.
    *
    * This function ignores the current platform locale. It considers Unicode
    * code points exclusively. For case-insensitive comparisons, a standard
    * Unicode case folding transformation is applied to each character pair.
    */
   int CompareCodePoints( char_type c, bool caseSensitive = true ) const
   {
      return R::CompareCodePoints( m_data->string, Length(), &c, 1, caseSensitive );
   }

   /*!
    * Lexicographical comparison between two strings.
    *
    * \param s             A string to which this string will be compared.
    *
    * \param caseSensitive When true, a case-sensitive comparison is performed;
    *                      otherwise the comparison does not distinguish
    *                      between lowercase and uppercase characters (as
    *                      defined by the selected locale). The default value
    *                      of this parameter is true.
    *
    * \param localeAware   When true, a locale-dependent comparison is done
    *                      which takes into account the currently selected user
    *                      locale (language and variants). When false, a
    *                      locale-invariant comparison is carried out by
    *                      comparing character code points (which is faster).
    *                      The default value of this parameter is true.
    *
    * Performs a character-to-character comparison between this string and the
    * specified string \a s, and returns an integer that indicates the
    * comparison result:
    *
    * \li  0 if both strings are equal.
    * \li +1 if this string postcedes the specified string \a s.
    * \li -1 if this string precedes the specified string \a s.
    */
   template <class R1, class A1>
   int Compare( const GenericString<T,R1,A1>& s,
                bool caseSensitive = true, bool localeAware = true ) const
   {
      return R::Compare( m_data->string, Length(), s.m_data->string, s.Length(), caseSensitive, localeAware );
   }

   /*!
    * Lexicographical comparison to a null-terminated string.
    *
    * \param t             The starting address of a null-terminated string to
    *                      which this string will be compared.
    *
    * \param caseSensitive When true, a case-sensitive comparison is performed;
    *                      otherwise the comparison does not distinguish
    *                      between lowercase and uppercase characters (as
    *                      defined by the selected locale). The default value
    *                      of this parameter is true.
    *
    * \param localeAware   When true, a locale-dependent comparison is done
    *                      which takes into account the currently selected user
    *                      locale (language and variants). When false, a
    *                      locale-invariant comparison is carried out by
    *                      comparing character code points (which is faster).
    *                      The default value of this parameter is true.
    *
    * Performs a character-to-character comparison between this string and the
    * specified null-terminated string \a t, and returns an integer that
    * indicates the comparison result:
    *
    * \li  0 if both strings are equal.
    * \li +1 if this string postcedes the specified string \a t.
    * \li -1 if this string precedes the specified string \a t.
    */
   int Compare( const_c_string t, bool caseSensitive = true, bool localeAware = true ) const
   {
      return R::Compare( m_data->string, Length(), t, R::Length( t ), caseSensitive, localeAware );
   }

   /*!
    * Lexicographical comparison to a single character.
    *
    * \param c             A character to which this string will be compared.
    *
    * \param caseSensitive When true, a case-sensitive comparison is performed;
    *                      otherwise the comparison does not distinguish
    *                      between lowercase and uppercase characters (as
    *                      defined by the selected locale). The default value
    *                      of this parameter is true.
    *
    * \param localeAware   When true, a locale-dependent comparison is done
    *                      which takes into account the currently selected user
    *                      locale (language and variants). When false, a
    *                      locale-invariant comparison is carried out by
    *                      comparing character code points (which is faster).
    *                      The default value of this parameter is true.
    *
    * Performs a comparison between this string and the specified character
    * \a c, and returns an integer that indicates the comparison result:
    *
    * \li  0 if this string is equal to \a c.
    * \li +1 if this string postcedes the character \a c.
    * \li -1 if this string precedes the character \a c.
    *
    * The performed comparison is equivalent to a comparison of this string
    * with a fictitious string of length one, whose only character was equal to
    * the specified character \a c.
    */
   int Compare( char_type c, bool caseSensitive = true, bool localeAware = true ) const
   {
      return R::Compare( m_data->string, Length(), &c, 1, caseSensitive, localeAware );
   }

   /*!
    * Case-insensitive lexicographical comparison between two strings.
    *
    * \param s             A string to which this string will be compared.
    *
    * \param localeAware   When true, a locale-dependent comparison is done
    *                      which takes into account the currently selected user
    *                      locale (language and variants). When false, a
    *                      locale-invariant comparison is carried out by
    *                      comparing character code points (which is faster).
    *                      The default value of this parameter is true.
    *
    * Performs a character-to-character, case-insensitive comparison between
    * this string and the specified string \a s, and returns an integer that
    * indicates the comparison result:
    *
    * \li  0 if both strings are equal.
    * \li +1 if this string postcedes the specified string \a s.
    * \li -1 if this string precedes the specified string \a s.
    */
   template <class R1, class A1>
   int CompareIC( const GenericString<T,R1,A1>& s, bool localeAware = true ) const
   {
      return R::Compare( m_data->string, Length(), s.m_data->string, s.Length(), false/*caseSensitive*/, localeAware );
   }

   /*!
    * Case-insensitive lexicographical comparison to a null-terminated string.
    *
    * \param t             The starting address of a null-terminated string to
    *                      which this string will be compared.
    *
    * \param localeAware   When true, a locale-dependent comparison is done
    *                      which takes into account the currently selected user
    *                      locale (language and variants). When false, a
    *                      locale-invariant comparison is carried out by
    *                      comparing character code points (which is faster).
    *                      The default value of this parameter is true.
    *
    * Performs a character-to-character, case-insensitive comparison between
    * this string and the specified null-terminated string \a t, and returns an
    * integer that indicates the comparison result:
    *
    * \li  0 if both strings are equal.
    * \li +1 if this string postcedes the specified string \a s.
    * \li -1 if this string precedes the specified string \a s.
    */
   int CompareIC( const_c_string t, bool localeAware = true ) const
   {
      return R::Compare( m_data->string, Length(), t, R::Length( t ), false/*caseSensitive*/, localeAware );
   }

   /*!
    * Case-insensitive lexicographical comparison to a single character.
    *
    * \param c             A character to which this string will be compared.
    *
    * \param localeAware   When true, a locale-dependent comparison is done
    *                      which takes into account the currently selected user
    *                      locale (language and variants). When false, a
    *                      locale-invariant comparison is carried out by
    *                      comparing character code points (which is faster).
    *                      The default value of this parameter is true.
    *
    * Performs a case-insensitive comparison between this string and the
    * specified character \a c, and returns an integer that indicates the
    * comparison result:
    *
    * \li  0 if this string is equal to \a c.
    * \li +1 if this string postcedes the character \a c.
    * \li -1 if this string precedes the character \a c.
    *
    * The performed comparison is equivalent to a case-insensitive comparison
    * of this string with a fictitious string of length one, whose only
    * character was equal to \a c.
    */
   int CompareIC( char_type c, bool localeAware = true ) const
   {
      return R::Compare( m_data->string, Length(), &c, 1, false/*caseSensitive*/, localeAware );
   }

   /*!
    * Wildcard string matching.
    *
    * \param pattern       The pattern string. May contain multiple instances
    *                      of the wildcard characters '*' and '?'.
    *
    * \param caseSensitive    When true, a case-sensitive comparison is
    *                      performed; otherwise the comparison does not
    *                      distinguish between lowercase and uppercase
    *                      characters. The default value is true.
    *
    * Returns true iff this string matches the specified \a pattern. If either
    * this string or the pattern is empty, this function always returns false
    * conventionally, even if the pattern is a single asterisk '*'.
    */
   template <class R1, class A1>
   bool WildMatch( const GenericString<T,R1,A1>& pattern, bool caseSensitive = true ) const
   {
      return R::WildMatch( m_data->string, Length(), pattern.m_data->string, pattern.Length(), caseSensitive );
   }

   /*!
    * Wildcard string matching (case-insensitive).
    *
    * \param pattern       The pattern string. May contain multiple instances
    *                      of the wildcard characters '*' and '?'.
    *
    * This function performs case-insensitive comparisons between string and
    * non-wild pattern characters.
    *
    * Returns true iff this string matches the specified \a pattern. If either
    * this string or the pattern is empty, this function always returns false
    * conventionally, even if the pattern is a single asterisk '*'.
    */
   template <class R1, class A1>
   bool WildMatchIC( const GenericString<T,R1,A1>& pattern ) const
   {
      return R::WildMatch( m_data->string, Length(), pattern.m_data->string, pattern.Length(), false/*caseSensitive*/ );
   }

   /*!
    * Wildcard string matching with a null-terminated pattern.
    *
    * \param pattern       The pattern string. May contain multiple instances
    *                      of the wildcard characters '*' and '?'.
    *
    * \param caseSensitive    When true, a case-sensitive comparison is
    *                      performed; otherwise the comparison does not
    *                      distinguish between lowercase and uppercase
    *                      characters. The default value is true.
    *
    * Returns true iff this string matches the specified \a pattern. If either
    * this string or the pattern is empty, this function always returns false
    * conventionally, even if the pattern is a single asterisk '*'.
    */
   bool WildMatch( const_c_string pattern, bool caseSensitive = true ) const
   {
      return R::WildMatch( m_data->string, Length(), pattern, R::Length( pattern ), caseSensitive );
   }

   /*!
    * Wildcard string matching with a null-terminated pattern
    * (case-insensitive).
    *
    * \param pattern       The pattern string. May contain multiple instances
    *                      of the wildcard characters '*' and '?'.
    *
    * This function performs case-insensitive comparisons between string and
    * non-wild pattern characters.
    *
    * Returns true iff this string matches the specified \a pattern. If either
    * this string or the pattern is empty, this function always returns false
    * conventionally, even if the pattern is a single asterisk '*'.
    */
   bool WildMatchIC( const_c_string pattern ) const
   {
      return R::WildMatch( m_data->string, Length(), pattern, R::Length( pattern ), false/*caseSensitive*/ );
   }

   /*!
    * Returns true iff this string contains one or more wildcard characters
    * (asterisk '*' or question mark '?').
    */
   bool HasWildcards() const
   {
      for ( iterator i = m_data->string; i < m_data->end; ++i )
         if ( R::IsWildcard( *i ) )
            return true;
      return false;
   }

   /*!
    * Replaces all characters in this string with their \e case \e folded
    * counterparts.
    */
   void ToCaseFolded()
   {
      size_type len = Length();
      if ( len > 0 )
      {
         EnsureUnique();
         R::ToCaseFolded( m_data->string, len );
      }
   }

   /*!
    * Replaces all \e uppercase characters in this string with their
    * \e lowercase counterparts.
    */
   void ToLowercase()
   {
      size_type len = Length();
      if ( len > 0 )
      {
         EnsureUnique();
         R::ToLowercase( m_data->string, len );
      }
   }

   /*!
    * Replaces all \e lowercase characters in this string with their
    * \e uppercase counterparts.
    */
   void ToUppercase()
   {
      size_type len = Length();
      if ( len > 0 )
      {
         EnsureUnique();
         R::ToUppercase( m_data->string, len );
      }
   }

   /*!
    * Returns a duplicate of this string with all characters replaced with
    * their case folded counterparts.
    */
   GenericString CaseFolded() const
   {
      GenericString s( *this );
      s.ToCaseFolded();
      return s;
   }

   /*!
    * Returns a duplicate of this string with all uppercase characters replaced
    * with their lowercase counterparts.
    */
   GenericString Lowercase() const
   {
      GenericString s( *this );
      s.ToLowercase();
      return s;
   }

   /*!
    * Returns a duplicate of this string with all lowercase characters replaced
    * with their uppercase counterparts.
    */
   GenericString Uppercase() const
   {
      GenericString s( *this );
      s.ToUppercase();
      return s;
   }

   /*!
    * Transposes the characters in this string, so that the first character
    * becomes the last and the last becomes the first, the second becomes the
    * next-to-last, and so on.
    */
   void Reverse()
   {
      if ( !IsEmpty() )
      {
         EnsureUnique();
         for ( iterator i = m_data->string, j = m_data->end; i < --j; ++i )
            pcl::Swap( *i, *j );
      }
   }

   /*!
    * Returns a duplicate of this string with its characters in reverse order.
    */
   GenericString Reversed() const
   {
      GenericString s( *this );
      s.Reverse();
      return s;
   }

   /*!
    * Sorts the characters of this string in ascending order.
    */
   void Sort()
   {
      if ( !IsEmpty() )
      {
         EnsureUnique();
         pcl::Sort( m_data->string, m_data->end );
      }
   }

   /*!
    * Returns a duplicate of this string with its characters sorted in
    * ascending order.
    */
   GenericString Sorted() const
   {
      GenericString s( *this );
      s.Sort();
      return s;
   }

   /*!
    * Sorts the characters of this string in ascending order. Ordering is
    * defined such that for any pair a, b of characters, the specified binary
    * predicate p( a, b ) is true if a precedes b.
    */
   template <class BP>
   void Sort( BP p )
   {
      if ( !IsEmpty() )
      {
         EnsureUnique();
         pcl::Sort( m_data->string, m_data->end, p );
      }
   }

   /*!
    * Returns a duplicate of this string with its characters sorted in
    * ascending order with the specified binary predicate \a p.
    */
   template <class BP>
   GenericString Sorted( BP p ) const
   {
      GenericString s( *this );
      s.Sort( p );
      return s;
   }

   /*!
    * Replaces the contents of this string with a sequence of tokens extracted
    * from a container \a c, separated with the specified \a separator
    * character. Returns a reference to this string.
    *
    * The container type C must have separated list generation semantics. All
    * iterable PCL containers such as Array, Vector, etc. provide the necessary
    * ToSeparated member functions.
    */
   template <class C>
   GenericString& ToSeparated( const C& c, char_type separator )
   {
      Clear();
      return c.ToSeparated( *this, separator );
   }

   /*!
    * Replaces the contents of this string with a sequence of tokens extracted
    * from a container \a c, separated with the specified \a separator
    * character, and built using an \a append binary function. Returns a
    * reference to this string.
    *
    * The binary function must be of the form:
    *
    * \code void append( GenericString& s1, const GenericString& s2 ); \endcode
    *
    * where \a s2 is being appended to \a s1.
    *
    * The container type C must have separated list generation semantics. All
    * iterable PCL containers such as Array, Vector, etc. provide the necessary
    * ToSeparated member functions.
    */
   template <class C, class AF>
   GenericString& ToSeparated( const C& c, char_type separator, AF append )
   {
      Clear();
      return c.ToSeparated( *this, separator, append );
   }

   /*!
    * Replaces the contents of this string with a sequence of tokens extracted
    * from a container \a c, separated with the specified \a separator string.
    * Returns a reference to this string.
    *
    * The container type C must have separated list generation semantics. All
    * iterable PCL containers such as Array, Vector, etc. provide the necessary
    * ToSeparated member functions.
    */
   template <class C>
   GenericString& ToSeparated( const C& c, const GenericString& separator )
   {
      Clear();
      return c.ToSeparated( *this, separator );
   }

   /*!
    * Replaces the contents of this string with a sequence of tokens extracted
    * from a container \a c, separated with the specified \a separator string,
    * and built using an \a append binary function. Returns a reference to this
    * string.
    *
    * The binary function must be of the form:
    *
    * \code void append( GenericString& s1, const GenericString& s2 ); \endcode
    *
    * where \a s2 is being appended to \a s1.
    *
    * The container type C must have separated list generation semantics. All
    * iterable PCL containers such as Array, Vector, etc. provide the necessary
    * ToSeparated member functions.
    */
   template <class C, class AF>
   GenericString& ToSeparated( const C& c, const GenericString& separator, AF append )
   {
      Clear();
      return c.ToSeparated( *this, separator, append );
   }

   /*!
    * Replaces the contents of this string with a sequence of tokens extracted
    * from a container \a c, separated with the specified \a separator
    * null-terminated string. Returns a reference to this string.
    *
    * The container type C must have separated list generation semantics. All
    * iterable PCL containers such as Array, Vector, etc. provide the necessary
    * ToSeparated member functions.
    */
   template <class C>
   GenericString& ToSeparated( const C& c, const_c_string separator )
   {
      Clear();
      return c.ToSeparated( *this, separator );
   }

   /*!
    * Replaces the contents of this string with a sequence of tokens extracted
    * from a container \a c, separated with the specified \a separator
    * null-terminated string, and built using an \a append binary function.
    * Returns a reference to this string.
    *
    * The binary function must be of the form:
    *
    * \code void append( GenericString& s1, const GenericString& s2 ); \endcode
    *
    * where \a s2 is being appended to \a s1.
    *
    * The container type C must have separated list generation semantics. All
    * iterable PCL containers such as Array, Vector, etc. provide the necessary
    * ToSeparated member functions.
    */
   template <class C, class AF>
   GenericString& ToSeparated( const C& c, const_c_string separator, AF append )
   {
      Clear();
      return c.ToSeparated( *this, separator, append );
   }

   /*!
    * Replaces the contents of this string with a sequence of comma-separated
    * tokens extracted from a container \a c. Returns a reference to this
    * string.
    *
    * This member function is equivalent to:
    *
    * \code ToSeparated( c, char_type( ',' ) ); \endcode
    */
   template <class C>
   GenericString& ToCommaSeparated( const C& c )
   {
      return ToSeparated( c, R::Comma() );
   }

   /*!
    * Replaces the contents of this string with a sequence of space-separated
    * tokens extracted from a container \a c. Returns a reference to this
    * string.
    *
    * This member function is equivalent to:
    *
    * \code ToSeparated( c, char_type( ' ' ) ); \endcode
    */
   template <class C>
   GenericString& ToSpaceSeparated( const C& c )
   {
      return ToSeparated( c, R::Blank() );
   }

   /*!
    * Replaces the contents of this string with a sequence of
    * tabulator-separated tokens extracted from a container \a c. Returns a
    * reference to this string.
    *
    * This member function is equivalent to:
    *
    * \code ToSeparated( c, char_type( '\t' ) ); \endcode
    */
   template <class C>
   GenericString& ToTabSeparated( const C& c )
   {
      return ToSeparated( c, R::Tab() );
   }

   /*!
    * Replaces the contents of this string with a sequence of
    * new line-separated tokens extracted from a container \a c. Returns a
    * reference to this string.
    *
    * This member function is equivalent to:
    *
    * \code ToSeparated( c, char_type( '\n' ) ); \endcode
    */
   template <class C>
   GenericString& ToNewLineSeparated( const C& c )
   {
      return ToSeparated( c, R::LF() );
   }

   /*!
    * Replaces the contents of this string with a sequence of
    * null-separated tokens extracted from a container \a c. Returns a
    * reference to this string.
    *
    * This member function is equivalent to:
    *
    * \code ToSeparated( c, char_type( '\0' ) ); \endcode
    */
   template <class C>
   GenericString& ToNullSeparated( const C& c )
   {
      return ToSeparated( c, R::Null() );
   }

   /*!
    * Replaces the contents of this string with a hyphenated sequence of tokens
    * extracted from a container \a c. Returns a reference to this string.
    *
    * This member function is equivalent to:
    *
    * \code ToSeparated( c, char_type( '-' ) ); \endcode
    */
   template <class C>
   GenericString& ToHyphenated( const C& c )
   {
      return ToSeparated( c, R::Hyphen() );
   }

   /*!
    * Returns true iff this string can be interpreted as a numeric literal:
    *
    * \li The string is not empty.
    *
    * \li It is not exclusively composed of trimable characters.
    *
    * \li Its first non-trimable character is either a sign character, a
    * decimal digit, or a decimal separator.
    *
    * \note This member function does \e not guarantee that this string
    * contains a \e valid numeric literal. It only checks for the \e role of
    * this string as a token in the context of a syntactic analysis.
    */
   bool IsNumeral() const
   {
      if ( IsEmpty() )
         return false;
      char_type c = *R::SearchTrimLeft( m_data->string, m_data->end );
      return R::IsDigit( c ) || R::IsSign( c ) || R::IsDecimalSeparator( c );
   }

   /*!
    * Returns true iff this string can be interpreted as a symbol identifier:
    *
    * \li The string is not empty.
    *
    * \li It is not exclusively composed of trimable characters.
    *
    * \li Its first non-trimable character is either an alphabetic character or
    * an underscore character.
    *
    * \note This member function does \e not guarantee that this string
    * contains a \e valid symbol identifier. It only checks for the \e role of
    * this string as a token in the context of a syntactic analysis.
    */
   bool IsSymbol() const
   {
      if ( IsEmpty() )
         return false;
      char_type c = *R::SearchTrimLeft( m_data->string, m_data->end );
      return R::IsSymbolDigit( c );
   }

   /*!
    * Returns true only if this string contains a valid identifier:
    *
    * \li It is not an empty string.
    *
    * \li Its first character is either an alphabetic character or an
    * underscore character.
    *
    * \li Its second and successive characters, if they exist, are all of them
    * either alphabetic characters, decimal digits, or underscores.
    *
    * If this string is not a valid identifier, the \a pos variable will be set
    * equal to the index of the first offending character (the first character
    * that violates the above conditions).
    */
   bool IsValidIdentifier( distance_type& pos ) const
   {
      if ( IsEmpty() )
      {
         pos = 0;
         return false;
      }
      const_iterator i = m_data->string;
      if ( R::IsStartingSymbolDigit( *i ) )
         for ( ;; )
         {
            if ( ++i == m_data->end )
               return true;
            if ( !R::IsSymbolDigit( *i ) )
               break;
         }
      pos = i - m_data->string;
      return false;
   }

   /*!
    * Returns true only if this string contains a valid identifier:
    *
    * \li It is not an empty string.
    *
    * \li Its first character is either an alphabetic character or an
    * underscore character.
    *
    * \li Its second and successive characters, if they exist, are all of them
    * either alphabetic characters, decimal digits, or underscores.
    */
   bool IsValidIdentifier() const
   {
      if ( !IsEmpty() )
         if ( R::IsStartingSymbolDigit( *m_data->string ) )
         {
            for ( const_iterator i = m_data->string; ++i < m_data->end; )
               if ( !R::IsSymbolDigit( *i ) )
                  return false;
            return true;
         }
      return false;
   }

   /*!
    * Returns a 64-bit non-cryptographic hash value computed for this string.
    *
    * This function calls pcl::Hash64() for the internal string buffer.
    *
    * The \a seed parameter can be used to generate repeatable hash values. It
    * can also be set to a random value in compromised environments.
    */
   uint64 Hash64( uint64 seed = 0 ) const
   {
      return pcl::Hash64( m_data->string, Size(), seed );
   }

   /*!
    * Returns a 32-bit non-cryptographic hash value computed for this string.
    *
    * This function calls pcl::Hash32() for the internal string buffer.
    *
    * The \a seed parameter can be used to generate repeatable hash values. It
    * can also be set to a random value in compromised environments.
    */
   uint32 Hash32( uint32 seed = 0 ) const
   {
      return pcl::Hash32( m_data->string, Size(), seed );
   }

   /*!
    * Returns a non-cryptographic hash value computed for this string. This
    * function is a synonym for Hash64().
    */
   uint64 Hash( uint64 seed = 0 ) const
   {
      return Hash64( seed );
   }

   /*!
    * If the internal free list of string data structures is available, this
    * function destroys it and returns the number of deleted structures.
    * Otherwise the function has no effect and returns zero.
    *
    * \note This function is \e not thread-safe.
    */
   static size_type DeleteFreeList()
   {
      return Data::DeleteFreeList();
   }

   // -------------------------------------------------------------------------

protected:

   /*!
    * \internal
    * Dereferences string data and disposes it if it becomes garbage.
    */
   void DetachFromData()
   {
      if ( !m_data->Detach() )
         Data::Dispose( m_data );
   }

   /*!
    * \internal
    * Internal string reallocation routine.
    *
    * Reallocates if necessary, ignoring current string contents. Reallocation
    * may happen due either to insufficient available space, or (in a
    * comparatively lazy fashion) to excessive unused space.
    *
    * Irrespective of reallocation, this function always updates internal data
    * pointers to reflect the specified length in characters.
    */
   void MaybeReallocate( size_type len )
   {
      if ( IsUnique() )
      {
         if ( m_data->ShouldReallocate( len ) )
         {
            m_data->Deallocate();
            m_data->Allocate( len );
         }
         else
            m_data->SetLength( len );
      }
      else
      {
         Data* newData = Data::New( len );
         DetachFromData();
         m_data = newData;
      }
   }

   /*!
    * \internal
    * Internal string reallocation routine.
    *
    * Causes this string to grow by \a n uninitialized characters at the
    * specified index \a i. The index \a i is updated as a side effect to
    * ensure that 0 <= \a i <= Length() before reallocation.
    */
   void UninitializedGrow( size_type& i, size_type n )
   {
      size_type len = Length();
      size_type newLen = len+n;
      if ( newLen > len )
      {
         if ( i > len )
            i = len;
         if ( IsUnique() )
         {
            if ( size_type( m_data->capacity - m_data->string ) < newLen+1 )
            {
               iterator old = m_data->string;
               m_data->Allocate( newLen );
               if ( old != nullptr )
               {
                  if ( i > 0 )
                     R::Copy( m_data->string, old, i );
                  if ( i < len )
                     R::Copy( m_data->string+i+n, old+i, len-i );
                  m_data->alloc.Deallocate( old );
               }
            }
            else
            {
               if ( i < len )
                  R::CopyOverlapped( m_data->string+i+n, m_data->string+i, len-i );
               m_data->SetLength( newLen );
            }
         }
         else
         {
            Data* newData = Data::New( newLen );
            if ( i > 0 )
               R::Copy( newData->string, m_data->string, i );
            if ( i < len )
               R::Copy( newData->string+i+n, m_data->string+i, len-i );
            DetachFromData();
            m_data = newData;
         }
      }
   }

   /*!
    * \internal
    * String trimming primitive.
    */
   void Trim( const_iterator left, size_type len )
   {
      if ( len > 0 )
      {
         if ( IsUnique() )
         {
            if ( m_data->ShouldReallocate( len ) )
            {
               iterator old = m_data->string;
               m_data->Allocate( len );
               R::Copy( m_data->string, left, len );
               if ( old != nullptr )
                  m_data->alloc.Deallocate( old );
            }
            else
            {
               if ( left != m_data->string ) // trim left
                  R::CopyOverlapped( m_data->string, left, len );
               m_data->SetLength( len );     // trim right
            }
         }
         else
         {
            Data* newData = Data::New( len );
            R::Copy( newData->string, left, len );
            DetachFromData();
            m_data = newData;
         }
      }
      else
         Clear();
   }

   /*!
    * \internal
    * Character replacement primitive.
    */
   void ReplaceChar( char_type c1, char_type c2, size_type i, size_type n, bool caseSensitive )
   {
      if ( n > 0 )
      {
         size_type len = Length();
         if ( i < len )
         {
            n = pcl::Min( n, len-i );
            if ( caseSensitive )
            {
               for ( iterator p = m_data->string + i, p1 = p + n; p < p1; ++p )
                  if ( *p == c1 )
                  {
                     EnsureUnique();
                     *p = c2;
                     for ( ; ++p < p1; )
                        if ( *p == c1 )
                           *p = c2;
                     break;
                  }
            }
            else
            {
               c1 = R::ToCaseFolded( c1 );
               for ( iterator p = m_data->string + i, p1 = p + n; p < p1; ++p )
                  if ( R::ToCaseFolded( *p ) == c1 )
                  {
                     EnsureUnique();
                     *p = c2;
                     for ( ; ++p < p1; )
                        if ( R::ToCaseFolded( *p ) == c1 )
                           *p = c2;
                     break;
                  }
            }
         }
      }
   }

   /*!
    * \internal
    * String replacement primitive.
    */
   void ReplaceString( const_iterator t1, size_type n1, const_iterator t2, size_type n2, size_type i, bool caseSensitive )
   {
      if ( n1 > 0 )
      {
         size_type len = Length();
         if ( i < len )
         {
            SearchEngine S( t1, n1, caseSensitive );
            if ( n1 == n2 )
            {
               EnsureUnique();
               for ( size_type p = i; (p = S( m_data->string, p, len )) < len; p += n1 )
                  R::Copy( m_data->string + p, t2, n2 );
            }
            else
            {
               Array<size_type> P;
               for ( size_type p = i; (p = S( m_data->string, p, len )) < len; p += n1 )
                  P.Add( p );
               if ( !P.IsEmpty() )
               {
                  size_type newLen = len;
                  if ( n1 < n2 )
                     newLen += P.Length()*(n2 - n1);
                  else
                     newLen -= P.Length()*(n1 - n2);

                  if ( newLen > 0 )
                  {
                     Data* newData = Data::New( newLen );
                     size_type targetIndex = 0;
                     size_type sourceIndex = 0;
                     for ( size_type p : P )
                     {
                        size_type n = p - sourceIndex;
                        if ( n > 0 )
                           R::Copy( newData->string+targetIndex, m_data->string+sourceIndex, n );
                        R::Copy( newData->string+targetIndex+n, t2, n2 );
                        targetIndex += n + n2;
                        sourceIndex = p + n1;
                     }
                     if ( sourceIndex < len )
                        R::Copy( newData->string+targetIndex, m_data->string+sourceIndex, len-sourceIndex );
                     DetachFromData();
                     m_data = newData;
                  }
                  else
                     Clear();
               }
            }
         }
      }
   }

   /*!
    * \internal
    * \class pcl::GenericString::SearchEngine
    * \brief Substring search primitive algorithm
    *
    * Implements the Boyer-Moore substring search algorithm (mismatched
    * character heuristic).
    *
    * \b References
    *
    * \li Robert Sedgewick, Kevin Wayne (2011), <em>Algorithms, 4th Edition,</em>
    * Addison-Wesley Professional, pp 770-773.
    *
    * \li Robert S. Boyer and J. Strother Moore (1977), <em>A fast string
    * searching algorithm,</em> Communcations of the ACM Vol. 20 No. 10,
    * pp 762-772.
    */
   class PCL_CLASS SearchEngine
   {
   public:

      /*!
       * Constructs a substring search engine for the specified \a pattern,
       * pattern length in characters, and case sensitivity.
       *
       * Setting the \a useBoyerMoore parameter to \c false forces use of a
       * brute-force search routine in all cases (useful for testing purposes
       * and unique searches on very short strings). Otherwise a Boyer-Moore
       * substring search algorithm will be used except for patterns of less
       * than 4 characters.
       */
      SearchEngine( const_iterator pattern, size_type patternLength,
                    bool caseSensitive = true, bool searchLast = false, bool useBoyerMoore = true ) :
         m_pattern( pattern ),
         m_patternLength( int( patternLength ) ),
         m_caseSensitive( caseSensitive ),
         m_searchLast( searchLast ),
         m_useBoyerMoore( useBoyerMoore && m_patternLength > 3 )
      {
         if ( m_useBoyerMoore )
            InitSkipList();
      }

      /*!
       * Performs a substring search in the subset of contiguous characters of
       * the specified string \a text defined by the range
       * [\a startIndex,\a endIndex). The search pattern, search direction and
       * case sensitivity are defined in the class constructor.
       *
       * Returns the character index where the first exact match has been
       * found, or \a endIndex if no match was found.
       *
       * We implement the <em>mismatched character heuristic</em> version of
       * the Boyer-Moore algorithm.
       */
      size_type operator()( const_iterator text, size_type startIndex, size_type endIndex ) const
      {
         if (    endIndex <= startIndex
              || m_patternLength <= 0
              || endIndex-startIndex < size_type( m_patternLength )
              || text == nullptr
              || m_pattern == nullptr )
            return endIndex;

         if ( m_caseSensitive )
         {
            if ( m_useBoyerMoore )
            {
               if ( m_searchLast )
               {
                  for ( size_type i = startIndex, r = endIndex-m_patternLength; i <= r; )
                  {
                     int skip = 0;
                     const_iterator t = text + r - i;
                     const_iterator p = m_pattern;
                     for ( int j = m_patternLength; --j >= 0; )
                     {
                        char_type c = *t++;
                        if ( c != *p++ )
                        {
                           skip = j - m_skipList[uint8( c )];
                           if ( skip < 1 )
                              skip = 1;
                           break;
                        }
                     }
                     if ( skip == 0 )
                        return r - i;
                     i += skip;
                  }
               }
               else
               {
                  for ( size_type i = startIndex, r = endIndex-m_patternLength; i <= r; )
                  {
                     int skip = 0;
                     const_iterator t = text + i + m_patternLength;
                     const_iterator p = m_pattern + m_patternLength;
                     for ( int j = m_patternLength; --j >= 0; )
                     {
                        char_type c = *--t;
                        if ( c != *--p )
                        {
                           // ### N.B.: Could do better with a precomputed pattern mismatch table.
                           skip = j - m_skipList[uint8( c )];
                           if ( skip < 1 )
                              skip = 1;
                           break;
                        }
                     }
                     if ( skip == 0 )
                        return i;
                     i += skip;
                  }
               }
            }
            else
            {
               // Use a brute force search for very small patterns.
               if ( m_searchLast )
               {
                  for ( size_type i = endIndex-m_patternLength; ; --i )
                  {
                     const_iterator t = text + i;
                     const_iterator p = m_pattern;
                     for ( int j = m_patternLength; ; ++t, ++p )
                     {
                        if ( *t != *p )
                           break;
                        if ( --j == 0 )
                           return i;
                     }
                     if ( i == startIndex )
                        break;
                  }
               }
               else
               {
                  for ( size_type i = startIndex, r = endIndex-m_patternLength; ; ++i )
                  {
                     const_iterator t = text + i;
                     const_iterator p = m_pattern;
                     for ( int j = m_patternLength; ; ++t, ++p )
                     {
                        if ( *t != *p )
                           break;
                        if ( --j == 0 )
                           return i;
                     }
                     if ( i == r )
                        break;
                  }
               }
            }
         }
         else
         {
            if ( m_useBoyerMoore )
            {
               if ( m_searchLast )
               {
                  for ( size_type i = startIndex, r = endIndex-m_patternLength; i <= r; )
                  {
                     int skip = 0;
                     const_iterator t = text + r - i;
                     const_iterator p = m_pattern;
                     for ( int j = m_patternLength; --j >= 0; )
                     {
                        char_type c = R::ToCaseFolded( *t++ );
                        if ( c != R::ToCaseFolded( *p++ ) )
                        {
                           skip = j - m_skipList[uint8( c )];
                           if ( skip < 1 )
                              skip = 1;
                           break;
                        }
                     }
                     if ( skip == 0 )
                        return r - i;
                     i += skip;
                  }
               }
               else
               {
                  for ( size_type i = startIndex, r = endIndex-m_patternLength; i <= r; )
                  {
                     int skip = 0;
                     const_iterator t = text + i + m_patternLength;
                     const_iterator p = m_pattern + m_patternLength;
                     for ( int j = m_patternLength; --j >= 0; )
                     {
                        char_type c = R::ToCaseFolded( *--t );
                        if ( c != R::ToCaseFolded( *--p ) )
                        {
                           // ### N.B.: Could do better with a precomputed pattern mismatch table.
                           skip = j - m_skipList[uint8( c )];
                           if ( skip < 1 )
                              skip = 1;
                           break;
                        }
                     }
                     if ( skip == 0 )
                        return i;
                     i += skip;
                  }
               }
            }
            else
            {
               // Use a brute force search for very small patterns.
               if ( m_searchLast )
               {
                  for ( size_type i = endIndex-m_patternLength; ; --i )
                  {
                     const_iterator t = text + i;
                     const_iterator p = m_pattern;
                     for ( int j = m_patternLength; ; ++t, ++p )
                     {
                        if ( R::ToCaseFolded( *t ) != R::ToCaseFolded( *p ) )
                           break;
                        if ( --j == 0 )
                           return i;
                     }
                     if ( i == startIndex )
                        break;
                  }
               }
               else
               {
                  for ( size_type i = startIndex, r = endIndex-m_patternLength; ; ++i )
                  {
                     const_iterator t = text + i;
                     const_iterator p = m_pattern;
                     for ( int j = m_patternLength; ; ++t, ++p )
                     {
                        if ( R::ToCaseFolded( *t ) != R::ToCaseFolded( *p ) )
                           break;
                        if ( --j == 0 )
                           return i;
                     }
                     if ( i == r )
                        break;
                  }
               }
            }
         }
         return endIndex;
      }

   private:

      int            m_skipList[ 256 ];
      const_iterator m_pattern;
      int            m_patternLength;
      bool           m_caseSensitive : 1;
      bool           m_searchLast    : 1;
      bool           m_useBoyerMoore : 1;

      void InitSkipList()
      {
         ::memset( m_skipList, 0xff, sizeof( m_skipList ) ); // fill with -1
         if ( m_searchLast )
         {
            const_iterator p = m_pattern + m_patternLength;
            if ( m_caseSensitive )
               for ( int i = 0; i < m_patternLength; ++i )
                  m_skipList[uint8( *--p )] = i;
            else
               for ( int i = 0; i < m_patternLength; ++i )
                  m_skipList[uint8( R::ToCaseFolded( *--p ) )] = i;
         }
         else
         {
            const_iterator p = m_pattern;
            if ( m_caseSensitive )
               for ( int i = 0; i < m_patternLength; ++i )
                  m_skipList[uint8( *p++ )] = i;
            else
               for ( int i = 0; i < m_patternLength; ++i )
                  m_skipList[uint8( R::ToCaseFolded( *p++ ) )] = i;
         }
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * \internal
    * \class pcl::GenericString::Data
    * \brief Reference-counted string data structure
    */
   class PCL_CLASS Data : public ReferenceCounter
   {
   private:

#ifndef __PCL_NO_STRING_FREE_LIST
      static Data*     freeList; //!< Singly-linked list of free string data structures.
      static AtomicInt freeLock; //!< Access synchronization for the free list.
#endif

   public:

      iterator  string   = nullptr; //!< Beginning of the null-terminated string
      iterator  end      = nullptr; //!< End of the string
      iterator  capacity = nullptr; //!< End of the allocated block or link in free list
      allocator alloc;              //!< The allocator object

      /*!
       * Constructs an empty string data structure.
       */
      Data() = default;

      /*!
       * Constructs a string of \a len characters.
       */
      Data( size_type len )
      {
         Allocate( len );
      }

      /*!
       * Constructs a string of \a len characters and space allocated for a
       * maximum of \a total characters (plus terminating null).
       */
      Data( size_type len, size_type total )
      {
         Allocate( len, total );
      }

      /*!
       * Destroys a string data structure.
       */
      ~Data()
      {
         Deallocate();
      }

      /*!
       * Allocates space to store at least \a total characters plus a
       * terminating null character, and updates internal pointers to define a
       * string of \a len characters.
       *
       * If \a total < \a len this routine allocates strictly the space
       * required to store \a len + 1 characters. Otherwise the allocated block
       * will have an optimized length, generally greater than the number
       * \a len + 1 of storable characters.
       */
      void Allocate( size_type len, size_type total )
      {
         total = (len <= total) ? alloc.PagedLength( total+1 ) : len+1; // +1 is room for a null terminating character
         string = alloc.Allocate( total );
         capacity = string + total;
         SetLength( len );
      }

      /*!
       * Allocates space to store at least \a len characters plus a terminating
       * null character. This function is a shortcut for Allocate( len, len ).
       */
      void Allocate( size_type len )
      {
         Allocate( len, len );
      }

      /*!
       * Allocates space to store exactly \a total characters plus a
       * terminating null character, and updates internal pointers to define a
       * string of \a len <= \a total characters.
       *
       * This routine allocates strictly the space required to store
       * \a total + 1 characters without length optimization.
       */
      void Reserve( size_type len, size_type total )
      {
         PCL_PRECONDITION( len <= total )
         string = alloc.Allocate( total+1 );
         capacity = string + total+1;
         SetLength( pcl::Min( len, total ) );
      }

      /*!
       * Deallocates string data, yielding an empty structure.
       */
      void Deallocate()
      {
         PCL_CHECK( (string == nullptr) ? end == nullptr : string < end )
         if ( string != nullptr )
         {
            alloc.Deallocate( string );
            Reset();
         }
      }

      /*!
       * Returns true iff a reallocation of string data should happen in order
       * to change the string's length to \a len characters.
       */
      bool ShouldReallocate( size_type len ) const
      {
         size_type m = capacity - string;
         return m <= len || alloc.ReallocatedLength( m, len+1 ) < (m >> 1);
      }

      /*!
       * Sets the length of the string. Arranges internal pointers and writes a
       * null string terminating character.
       */
      void SetLength( size_type len )
      {
         *(end = (string + len)) = R::Null();
      }

      /*!
       * Sets all string pointers to \c nullptr.
       */
      void Reset()
      {
         string = end = capacity = nullptr;
      }

      /*!
       * Returns a pointer to a free string data structure, or \c nullptr if
       * none is available. The returned structure must be initialized.
       *
       * This function is thread-safe.
       */
#ifndef __PCL_NO_STRING_FREE_LIST
      static Data* NextFree()
      {
         if ( freeLock.TestAndSet( 0, 1 ) )
         {
            Data* data = freeList;
            if ( data != nullptr )
               freeList = reinterpret_cast<Data*>( data->string );
            freeLock.Store( 0 );
            return data;
         }
         return nullptr;
      }
#endif // !__PCL_NO_STRING_FREE_LIST

      /*!
       * Returns a free empty string data structure. Retrieves an existing data
       * structure from the internal free list if available, or creates a new
       * one otherwise.
       *
       * This function is thread-safe.
       */
      static Data* New()
      {
#ifndef __PCL_NO_STRING_FREE_LIST
         Data* data = NextFree();
         if ( data != nullptr )
         {
            data->string = nullptr;
            return data;
         }
#endif // !__PCL_NO_STRING_FREE_LIST
         return new Data;
      }

      /*!
       * Returns a free string data structure of the specified length in
       * characters. Retrieves an existing data structure from the internal
       * free list if available, or creates a new one otherwise.
       *
       * This function is thread-safe.
       */
      static Data* New( size_type len )
      {
#ifndef __PCL_NO_STRING_FREE_LIST
         Data* data = NextFree();
         if ( data != nullptr )
         {
            data->Allocate( len ); // ### FIXME: If allocation fails, data is a leak.
            return data;
         }
#endif // !__PCL_NO_STRING_FREE_LIST
         return new Data( len );
      }

      /*!
       * Returns a free string data structure with the specified length and
       * total available space. Retrieves an existing data structure from the
       * internal free list if available, or creates a new one otherwise.
       *
       * This function is thread-safe.
       */
      static Data* New( size_type len, size_type total )
      {
#ifndef __PCL_NO_STRING_FREE_LIST
         Data* data = NextFree();
         if ( data != nullptr )
         {
            data->Allocate( len, total ); // ### FIXME: If allocation fails, data is a leak.
            return data;
         }
#endif // !__PCL_NO_STRING_FREE_LIST
         return new Data( len, total );
      }

      /*!
       * Deallocates and disposes a string data structure. If the internal free
       * list is available, the deallocated structure is added to the list for
       * subsequent reuse. Otherwise the structure is destroyed.
       *
       * This function is thread-safe.
       */
      static void Dispose( Data* data )
      {
         PCL_PRECONDITION( data != nullptr )
         PCL_CHECK( data->RefCount() == 0 )
#ifndef __PCL_NO_STRING_FREE_LIST
         if ( freeLock.TestAndSet( 0, 1 ) )
         {
            data->Attach();
            data->Deallocate();
            data->string = reinterpret_cast<iterator>( freeList );
            freeList = data;
            freeLock.Store( 0 );
         }
         else
#endif // !__PCL_NO_STRING_FREE_LIST
            delete data;
      }

      /*!
       * If the internal free list is available, this function destroys it and
       * returns the number of deleted string data structures. Otherwise the
       * function has no effect and returns zero.
       *
       * \note This function is \e not thread-safe.
       */
      static size_type DeleteFreeList()
      {
         size_type count = 0;
#ifndef __PCL_NO_STRING_FREE_LIST
         while ( freeList != nullptr )
         {
            Data* data = freeList;
            freeList = reinterpret_cast<Data*>( data->string );
            data->string = nullptr;
            delete data;
            ++count;
         }
#endif // !__PCL_NO_STRING_FREE_LIST
         return count;
      }
   };

   /*
    * The reference-counted string data.
    */
   Data* m_data = nullptr;
};

#ifndef __PCL_NO_STRING_FREE_LIST

template <class T, class R, class A>
typename GenericString<T,R,A>::Data* GenericString<T,R,A>::Data::freeList = nullptr;

template <class T, class R, class A>
AtomicInt GenericString<T,R,A>::Data::freeLock;

#endif // !__PCL_NO_STRING_FREE_LIST

/*!
 * Exchanges two strings.
 */
template <class T, class R, class A> inline
void Swap( GenericString<T,R,A>& s1, GenericString<T,R,A>& s2 )
{
   s1.Swap( s2 );
}

// ----------------------------------------------------------------------------

/*!
 * \defgroup genericstring_relational_ops GenericString Relational Operators
 */

// ----------------------------------------------------------------------------

/*!
 * Returns true iff two strings \a s1 and \a s2 are equal.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R1, class A1, class R2, class A2> inline
bool operator ==( const GenericString<T,R1,A1>& s1, const GenericString<T,R2,A2>& s2 )
{
   return s1.CompareCodePoints( s2 ) == 0;
}

/*!
 * Returns true iff a string \a s1 is less than a string \a s2. This function
 * performs a character-to-character, locale-unaware comparison of numeric
 * character values. See GenericString<>::CompareCodePoints() for more
 * information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R1, class A1, class R2, class A2> inline
bool operator  <( const GenericString<T,R1,A1>& s1, const GenericString<T,R2,A2>& s2 )
{
   return s1.CompareCodePoints( s2 ) < 0;
}

/*!
 * Returns true iff a string \a s1 is less than or equal to a string \a s2.
 * This function performs a character-to-character, locale-unaware comparison
 * of numeric character values. See GenericString<>::CompareCodePoints() for
 * more information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R1, class A1, class R2, class A2> inline
bool operator <=( const GenericString<T,R1,A1>& s1, const GenericString<T,R2,A2>& s2 )
{
   return s1.CompareCodePoints( s2 ) <= 0;
}

/*!
 * Returns true iff a string \a s1 is greater than a string \a s2. This
 * function performs a character-to-character, locale-unaware comparison of
 * numeric character values. See GenericString<>::CompareCodePoints() for more
 * information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R1, class A1, class R2, class A2> inline
bool operator  >( const GenericString<T,R1,A1>& s1, const GenericString<T,R2,A2>& s2 )
{
   return s1.CompareCodePoints( s2 ) > 0;
}

/*!
 * Returns true iff a string \a s1 is greater than or equal to a string \a s2.
 * This function performs a character-to-character, locale-unaware comparison
 * of numeric character values. See GenericString<>::CompareCodePoints() for
 * more information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R1, class A1, class R2, class A2> inline
bool operator >=( const GenericString<T,R1,A1>& s1, const GenericString<T,R2,A2>& s2 )
{
   return s1.CompareCodePoints( s2 ) >= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Returns true iff a string \a s1 is equal to a null-terminated string \a t2.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator ==( const GenericString<T,R,A>& s1, typename GenericString<T,R,A>::const_c_string t2 )
{
   return s1.CompareCodePoints( t2 ) == 0;
}

/*!
 * Returns true iff a string \a s1 is less than a null-terminated string \a t2.
 * This function performs a character-to-character, locale-unaware comparison
 * of numeric character values. See GenericString<>::CompareCodePoints() for
 * more information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator  <( const GenericString<T,R,A>& s1, typename GenericString<T,R,A>::const_c_string t2 )
{
   return s1.CompareCodePoints( t2 ) < 0;
}

/*!
 * Returns true iff a string \a s1 is less than or equal to a null-terminated
 * string \a t2. This function performs a character-to-character,
 * locale-unaware comparison of numeric character values. See
 * GenericString<>::CompareCodePoints() for more information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator <=( const GenericString<T,R,A>& s1, typename GenericString<T,R,A>::const_c_string t2 )
{
   return s1.CompareCodePoints( t2 ) <= 0;
}

/*!
 * Returns true iff a string \a s1 is greater than a null-terminated string
 * \a t2. This function performs a character-to-character, locale-unaware
 * comparison of numeric character values. See
 * GenericString<>::CompareCodePoints() for more information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator  >( const GenericString<T,R,A>& s1, typename GenericString<T,R,A>::const_c_string t2 )
{
   return s1.CompareCodePoints( t2 ) > 0;
}

/*!
 * Returns true iff a string \a s1 is greater than or equal to a
 * null-terminated string \a t2. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator >=( const GenericString<T,R,A>& s1, typename GenericString<T,R,A>::const_c_string t2 )
{
   return s1.CompareCodePoints( t2 ) >= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Returns true iff a null-terminated string \a t1 is equal to a string \a s2.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator ==( typename GenericString<T,R,A>::const_c_string t1, const GenericString<T,R,A>& s2 )
{
   return s2.CompareCodePoints( t1 ) == 0;
}

/*!
 * Returns true iff a null-terminated string \a t1 is less than a string \a s2.
 * This function performs a character-to-character, locale-unaware comparison
 * of numeric character values. See GenericString<>::CompareCodePoints() for
 * more information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator  <( typename GenericString<T,R,A>::const_c_string t1, const GenericString<T,R,A>& s2 )
{
   return s2.CompareCodePoints( t1 ) > 0;
}

/*!
 * Returns true iff a null-terminated string \a t1 is less than or equal to a
 * string \a s2. This function performs a character-to-character,
 * locale-unaware comparison of numeric character values. See
 * GenericString<>::CompareCodePoints() for more information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator <=( typename GenericString<T,R,A>::const_c_string t1, const GenericString<T,R,A>& s2 )
{
   return s2.CompareCodePoints( t1 ) >= 0;
}

/*!
 * Returns true iff a null-terminated string \a t1 is greater than a string
 * \a s2. This function performs a character-to-character, locale-unaware
 * comparison of numeric character values. See
 * GenericString<>::CompareCodePoints() for more information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator  >( typename GenericString<T,R,A>::const_c_string t1, const GenericString<T,R,A>& s2 )
{
   return s2.CompareCodePoints( t1 ) < 0;
}

/*!
 * Returns true iff a null-terminated string \a t1 is greater than or equal to
 * a string \a s2. This function performs a character-to-character,
 * locale-unaware comparison of numeric character values. See
 * GenericString<>::CompareCodePoints() for more information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator >=( typename GenericString<T,R,A>::const_c_string t1, const GenericString<T,R,A>& s2 )
{
   return s2.CompareCodePoints( t1 ) <= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Returns true iff a string \a s1 is equal to a character \a c2.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator ==( const GenericString<T,R,A>& s1, typename GenericString<T,R,A>::char_type c2 )
{
   return s1.CompareCodePoints( c2 ) == 0;
}

/*!
 * Returns true iff a string \a s1 is less than a character \a c2. This
 * function performs a character-to-character, locale-unaware comparison of
 * numeric character values. See GenericString<>::CompareCodePoints() for more
 * information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator  <( const GenericString<T,R,A>& s1, typename GenericString<T,R,A>::char_type c2 )
{
   return s1.CompareCodePoints( c2 ) < 0;
}

/*!
 * Returns true iff a string \a s1 is less than or equal to a character \a c2.
 * This function performs a character-to-character, locale-unaware comparison
 * of numeric character values. See GenericString<>::CompareCodePoints() for
 * more information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator <=( const GenericString<T,R,A>& s1, typename GenericString<T,R,A>::char_type c2 )
{
   return s1.CompareCodePoints( c2 ) <= 0;
}

/*!
 * Returns true iff a string \a s1 is greater than a character \a c2. This
 * function performs a character-to-character, locale-unaware comparison of
 * numeric character values. See GenericString<>::CompareCodePoints() for more
 * information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator  >( const GenericString<T,R,A>& s1, typename GenericString<T,R,A>::char_type c2 )
{
   return s1.CompareCodePoints( c2 ) > 0;
}

/*!
 * Returns true iff a string \a s1 is greater than or equal to a character
 * \a c2. This function performs a character-to-character, locale-unaware
 * comparison of numeric character values. See
 * GenericString<>::CompareCodePoints() for more information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator >=( const GenericString<T,R,A>& s1, typename GenericString<T,R,A>::char_type c2 )
{
   return s1.CompareCodePoints( c2 ) >= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Returns true iff a character \a c1 is equal to a string \a s2.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator ==( typename GenericString<T,R,A>::char_type c1, const GenericString<T,R,A>& s2 )
{
   return s2.CompareCodePoints( c1 ) == 0;
}

/*!
 * Returns true iff a character \a c1 is less than a string \a s2. This
 * function performs a character-to-character, locale-unaware comparison of
 * numeric character values. See GenericString<>::CompareCodePoints() for more
 * information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator  <( typename GenericString<T,R,A>::char_type c1, const GenericString<T,R,A>& s2 )
{
   return s2.CompareCodePoints( c1 ) > 0;
}

/*!
 * Returns true iff a character \a c1 is less than or equal to a string \a s2.
 * This function performs a character-to-character, locale-unaware comparison
 * of numeric character values. See GenericString<>::CompareCodePoints() for
 * more information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator <=( typename GenericString<T,R,A>::char_type c1, const GenericString<T,R,A>& s2 )
{
   return s2.CompareCodePoints( c1 ) >= 0;
}

/*!
 * Returns true iff a character \a c1 is greater than a string \a s2. This
 * function performs a character-to-character, locale-unaware comparison of
 * numeric character values. See GenericString<>::CompareCodePoints() for more
 * information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator  >( typename GenericString<T,R,A>::char_type c1, const GenericString<T,R,A>& s2 )
{
   return s2.CompareCodePoints( c1 ) < 0;
}

/*!
 * Returns true iff a character \a c1 is greater than or equal to a string
 * \a s2. This function performs a character-to-character, locale-unaware
 * comparison of numeric character values. See
 * GenericString<>::CompareCodePoints() for more information.
 * \ingroup genericstring_relational_ops
 */
template <class T, class R, class A> inline
bool operator >=( typename GenericString<T,R,A>::char_type c1, const GenericString<T,R,A>& s2 )
{
   return s2.CompareCodePoints( c1 ) <= 0;
}

// ----------------------------------------------------------------------------

/*!
 * \class IsoString
 * \brief Eight-bit string (ISO/IEC-8859-1 or UTF-8 string).
 *
 * %IsoString derives from a template instantiation of GenericString for the
 * \c char type. On the PixInsight platform, %IsoString represents a dynamic
 * ISO/IEC-8859-1 string, an 8-bit Unicode Transformation Format (UTF-8)
 * string, or even a sequence of ASCII (or Latin-1) characters, depending on
 * the context and type of transformations applied.
 *
 * \sa String
 */
class PCL_CLASS IsoString : public GenericString<char, IsoCharTraits, StandardAllocator>
{
public:

   /*!
    * Base class of %IsoString.
    */
   typedef GenericString<char, IsoCharTraits, StandardAllocator>
                                                string_base;

   /*!
    * Represents a character pertaining to an %IsoString object.
    */
   typedef string_base::char_type               char_type;

   /*!
    * The character traits class used by this template instantiation.
    */
   typedef string_base::char_traits             char_traits;

   /*!
    * The block allocator used by this template instantiation.
    */
   typedef string_base::block_allocator         block_allocator;

   /*!
    * The allocator class used by this template instantiation.
    */
   typedef string_base::allocator               allocator;

   /*!
    * Null-terminated sequence of 8-bit characters.
    */
   typedef string_base::c_string                c_string;

   /*!
    * Immutable null-terminated sequence of 8-bit characters.
    */
   typedef string_base::const_c_string          const_c_string;

   /*!
    * %IsoString iterator.
    */
   typedef string_base::iterator                iterator;

   /*!
    * Immutable %IsoString iterator.
    */
   typedef string_base::const_iterator          const_iterator;

   /*!
    * Reverse %IsoString iterator.
    */
   typedef string_base::reverse_iterator        reverse_iterator;

   /*!
    * Immutable reverse %IsoString iterator.
    */
   typedef string_base::const_reverse_iterator  const_reverse_iterator;

   /*!
    * Represents a Unicode (UTF-16) string.
    * \note This type must be defined as the same template instantiation used
    * for the String class.
    */
   typedef GenericString<char16_type, CharTraits, StandardAllocator>
                                                ustring_base;

   /*!
    * Represents a Unicode (UTF-16) character.
    */
   typedef ustring_base::char_type              uchar_type;

   /*!
    * Unicode (UTF-16) character traits class.
    */
   typedef ustring_base::char_traits            uchar_traits;

   /*!
    * Null-terminated sequence of UTF-16 characters.
    */
   typedef ustring_base::c_string               c_ustring;

   /*!
    * Immutable null-terminated sequence of UTF-16 characters.
    */
   typedef ustring_base::const_c_string         const_c_ustring;

   /*!
    * Unicode (UTF-16) string iterator.
    */
   typedef ustring_base::iterator               uchar_iterator;

   /*!
    * Immutable Unicode (UTF-16) string iterator.
    */
   typedef ustring_base::const_iterator         const_uchar_iterator;

   // -------------------------------------------------------------------------

   /*!
    * Constructs an empty %IsoString object.
    */
   IsoString() : string_base()
   {
   }

   /*!
    * Constructs an %IsoString object as a shallow copy of the specified
    * \c string_base string \a s (copy constructor from the base class).
    */
   IsoString( const string_base& s ) : string_base( s )
   {
   }

   /*!
    * Copy constructor.
    */
   IsoString( const IsoString& s ) : string_base( s )
   {
   }

   /*!
    * Constructs an %IsoString object by transferring data from the specified
    * \c string_base string \a s (move constructor from the base class).
    */
   IsoString( string_base&& s ) : string_base( std::move( s ) )
   {
   }

   /*!
    * Move constructor.
    */
   IsoString( const IsoString&& s ) : string_base( std::move( s ) )
   {
   }

   /*!
    * Constructs an %IsoString as a copy of the specified Unicode string \a s,
    * transformed to ISO/IEC-8859-1. Characters in the Unicode string that
    * cannot be converted to ISO/IEC-8859-1 (that is, code points greater than
    * 0x100) will have undefined values in the 8-bit string. Undefined values
    * are represented with question mark characters (?).
    */
   explicit
   IsoString( const ustring_base& s ) : string_base()
   {
      (void)operator =( s );
   }

   /*!
    * Constructs an %IsoString as a copy of a null-terminated string \a t.
    */
   IsoString( const_c_string t ) : string_base( t )
   {
   }

   /*!
    * Constructs an %IsoString with the \a n first characters of the
    * null-terminated string \a t, starting from its \a i-th character.
    */
   IsoString( const_c_string t, size_type i, size_type n ) : string_base( t, i, n )
   {
   }

   /*!
    * Constructs an %IsoString with \a n copies of a character \a c.
    */
   IsoString( char_type c, size_type n = 1 ) : string_base( c, n )
   {
   }

   /*!
    * Constructs an %IsoString with a copy of the character sequence defined by
    * the range [i,j).
    */
   IsoString( const_iterator i, const_iterator j ) : string_base( i, j )
   {
   }

   /*!
    * Constructs an %IsoString as a copy of the null-terminated Unicode
    * (UTF-16) string \a t, transformed to ISO/IEC-8859-1.
    *
    * If the specified Unicode string \a t contains characters that cannot be
    * converted to ISO/IEC-8859-1 characters, the corresponding characters in
    * this string will have undefined values. For conversions from UTF-16 to
    * UTF-8, see String::ToUTF8() and String::UTF16ToUTF8().
    */
   explicit
   IsoString( const_c_ustring t ) : string_base()
   {
      (void)operator =( t );
   }

   /*!
    * Constructs an %IsoString with the \a n first characters of the
    * null-terminated Unicode (UTF-16) string \a t, starting from its \a i-th
    * character, transformed to ISO/IEC-8859-1.
    *
    * If the specified Unicode string \a t contains characters that cannot be
    * converted to ISO/IEC-8859-1 characters, the corresponding characters in
    * this string will have undefined values. For conversions from UTF-16 to
    * UTF-8, see String::ToUTF8() and String::UTF16ToUTF8().
    */
   IsoString( const_c_ustring t, size_type i, size_type n );

   /*!
    * Constructs an %IsoString as a literal representation of a \c bool value.
    */
   explicit
   IsoString( bool x ) : string_base( x ? "true" : "false" )
   {
   }

   /*!
    * Constructs an %IsoString as a literal representation of a
    * \c signed \c short value.
    */
   explicit
   IsoString( short x ) : string_base()
   {
      (void)Format( "%hd", x );
   }

   /*!
    * Constructs an %IsoString as a literal representation of an
    * \c unsigned \c short value.
    */
   explicit
   IsoString( unsigned short x ) : string_base()
   {
      (void)Format( "%hu", x );
   }

   /*!
    * Constructs an %IsoString as a literal representation of a
    * \c signed \c int value.
    */
   explicit
   IsoString( int x ) : string_base()
   {
      (void)Format( "%i", x );
   }

   /*!
    * Constructs an %IsoString as a literal representation of an
    * \c unsigned \c int value.
    */
   explicit
   IsoString( unsigned int x ) : string_base()
   {
      (void)Format( "%u", x );
   }

   /*!
    * Constructs an %IsoString as a literal representation of a
    * \c signed \c long value.
    */
   explicit
   IsoString( long x ) : string_base()
   {
      (void)Format( "%ld", x );
   }

   /*!
    * Constructs an %IsoString as a literal representation of an
    * \c unsigned \c long value.
    */
   explicit
   IsoString( unsigned long x ) : string_base()
   {
      (void)Format( "%lu", x );
   }

   /*!
    * Constructs an %IsoString as a literal representation of a
    * \c signed \c long \c long value.
    */
   explicit
   IsoString( long long x ) : string_base()
   {
      (void)Format( "%lli", x );
   }

   /*!
    * Constructs an %IsoString as a literal representation of an
    * \c unsigned \c long \c long value.
    */
   explicit
   IsoString( unsigned long long x ) : string_base()
   {
      (void)Format( "%llu", x );
   }

   /*!
    * Constructs an %IsoString as a literal representation of a
    * \c float value.
    */
   explicit
   IsoString( float x ) : string_base()
   {
      (void)Format( "%.7g", x );
   }

   /*!
    * Constructs an %IsoString as a literal representation of a
    * \c double value.
    */
   explicit
   IsoString( double x ) : string_base()
   {
      (void)Format( "%.16lg", x );
   }

   /*!
    * Constructs an %IsoString as a literal representation of a
    * \c long \c double value.
    */
   explicit
   IsoString( long double x ) : string_base()
   {
#ifdef _MSC_VER
      (void)Format( "%.16Lg", x );
#else
      (void)Format( "%.18Lg", x );
#endif
   }

#ifndef __PCL_NO_STRING_COMPLEX

   /*!
    * Constructs an %IsoString as a literal representation of an
    * \c fcomplex value.
    */
   explicit
   IsoString( Complex<float>& x ) : string_base()
   {
      (void)Format( "{%.7g,%.7g}", x.Real(), x.Imag() );
   }

   /*!
    * Constructs an %IsoString as a literal representation of a
    * \c dcomplex value.
    */
   explicit
   IsoString( Complex<double>& x ) : string_base()
   {
      (void)Format( "{%.16lg,%.16lg}", x.Real(), x.Imag() );
   }

   /*!
    * Constructs an %IsoString as a literal representation of an
    * \c lcomplex value.
    */
   explicit
   IsoString( Complex<long double>& x ) : string_base()
   {
#ifdef _MSC_VER
      (void)Format( "{%.16Lg,%.16Lg}", x.Real(), x.Imag() );
#else
      (void)Format( "{%.18Lg,%.18Lg}", x.Real(), x.Imag() );
#endif
   }

#endif   // !__PCL_NO_STRING_COMPLEX

#ifdef __PCL_QT_INTERFACE

   explicit
   IsoString( const QString& qs ) :
      string_base( qs.isEmpty() ? iterator( nullptr ) : iterator( PCL_GET_CHARPTR_FROM_QSTRING( qs ) ) )
   {
   }

   explicit
   IsoString( const QByteArray& qb ) :
      string_base( qb.isEmpty() ? iterator( nullptr ) : iterator( PCL_GET_CHARPTR_FROM_QBYTEARRAY( qb ) ) )
   {
   }

   explicit
   IsoString( const QDate& qd ) :
      string_base( iterator( PCL_GET_CHARPTR_FROM_QSTRING( qd.toString( PCL_QDATE_FMT_STR ) ) ) )
   {
   }

   explicit
   IsoString( const QDateTime& qdt ) :
      string_base( iterator( PCL_GET_CHARPTR_FROM_QSTRING( qdt.toString( PCL_QDATETIME_FMT_STR ) ) ) )
   {
   }

#endif

   // -------------------------------------------------------------------------

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   IsoString& operator =( const IsoString& s )
   {
      Assign( s );
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   IsoString& operator =( IsoString&& s )
   {
      Transfer( s );
      return *this;
   }

   /*!
    * Assigns a shallow copy of the \c string_base string \a s to this string.
    * Returns a reference to this object.
    */
   IsoString& operator =( const string_base& s )
   {
      Assign( s );
      return *this;
   }

   /*!
    * Transfers the data from the \c string_base string \a s to this string
    * (move assignment from base class). Returns a reference to this object.
    */
   IsoString& operator =( string_base&& s )
   {
      Transfer( s );
      return *this;
   }

   /*!
    * Assigns a copy of the Unicode string \a s (UTF-16) to this string, after
    * converting source UTF-16 characters to ISO/IEC-8859-1 characters. Returns
    * a reference to this object.
    *
    * If the specified UTF-16 string contains characters that cannot be
    * converted to ISO/IEC-8859-1, the corresponding characters in this string
    * will have undefined values. For conversions from UTF-16 to UTF-8, see
    * String::ToUTF8() and String::UTF16ToUTF8().
    */
   IsoString& operator =( const ustring_base& s )
   {
      return operator =( s.Begin() );
   }

   /*!
    * Assigns a copy of the null-terminated string \a t to this string. Returns
    * a reference to this object.
    */
   IsoString& operator =( const_c_string t )
   {
      Assign( t );
      return *this;
   }

   /*!
    * Assigns a single copy of a character \a c to this string. Returns a
    * reference to this object.
    */
   IsoString& operator =( char_type c )
   {
      Assign( c, 1 );
      return *this;
   }

   /*!
    * Assigns a copy of the null-terminated Unicode string \a s (UTF-16) to
    * this string, after converting source UTF-16 characters to ISO/IEC-8859-1
    * characters. Returns a reference to this object.
    *
    * If the specified UTF-16 string contains characters that cannot be
    * converted to ISO/IEC-8859-1, the corresponding characters in this string
    * will have undefined values. For conversions from UTF-16 to UTF-8, see
    * String::ToUTF8() and String::UTF16ToUTF8().
    */
   IsoString& operator =( const_c_ustring t );

#ifdef __PCL_QT_INTERFACE

   IsoString& operator =( const QString& qs )
   {
      if ( qs.isEmpty() )
         Clear();
      else
         Assign( PCL_GET_CHARPTR_FROM_QSTRING( qs ) );
      return *this;
   }

   IsoString& operator =( const QByteArray& qb )
   {
      if ( qb.isEmpty() )
         Clear();
      else
         Assign( PCL_GET_CHARPTR_FROM_QBYTEARRAY( qb ) );
      return *this;
   }

   IsoString& operator =( const QDate& qd )
   {
      Assign( PCL_GET_CHARPTR_FROM_QSTRING( qd.toString( PCL_QDATE_FMT_STR ) ) );
      return *this;
   }

   IsoString& operator =( const QDateTime& qdt )
   {
      Assign( PCL_GET_CHARPTR_FROM_QSTRING( qdt.toString( PCL_QDATETIME_FMT_STR ) ) );
      return *this;
   }

#endif

   // -------------------------------------------------------------------------

   IsoString SetToLength( size_type n ) const
   {
      return string_base::SetToLength( n );
   }

   IsoString ResizedToNullTerminated() const
   {
      return string_base::ResizedToNullTerminated();
   }

   IsoString Squeezed() const
   {
      return string_base::Squeezed();
   }

   // -------------------------------------------------------------------------

   IsoString Substring( size_type i, size_type n = maxPos ) const
   {
      return string_base::Substring( i, n );
   }

   IsoString Left( size_type n ) const
   {
      return string_base::Left( n );
   }

   IsoString Right( size_type n ) const
   {
      return string_base::Right( n );
   }

   IsoString Suffix( size_type i ) const
   {
      return string_base::Suffix( i );
   }

   IsoString Prefix( size_type i ) const
   {
      return string_base::Prefix( i );
   }

   // -------------------------------------------------------------------------

   IsoString Trimmed() const
   {
      return string_base::Trimmed();
   }

   IsoString TrimmedLeft() const
   {
      return string_base::TrimmedLeft();
   }

   IsoString TrimmedRight() const
   {
      return string_base::TrimmedRight();
   }

   // -------------------------------------------------------------------------

   IsoString LeftJustified( size_type width, char_type fill = IsoCharTraits::Blank() ) const
   {
      return string_base::LeftJustified( width, fill );
   }

   IsoString RightJustified( size_type width, char_type fill = IsoCharTraits::Blank() ) const
   {
      return string_base::RightJustified( width, fill );
   }

   IsoString CenterJustified( size_type width, char_type fill = IsoCharTraits::Blank() ) const
   {
      return string_base::CenterJustified( width, fill );
   }

   // -------------------------------------------------------------------------

   IsoString CaseFolded() const
   {
      return string_base::CaseFolded();
   }

   IsoString Lowercase() const
   {
      return string_base::Lowercase();
   }

   IsoString Uppercase() const
   {
      return string_base::Uppercase();
   }

   // -------------------------------------------------------------------------

   IsoString Reversed() const
   {
      return string_base::Reversed();
   }

   IsoString Sorted() const
   {
      return string_base::Sorted();
   }

   template <class BP>
   IsoString Sorted( BP p ) const
   {
      return string_base::Sorted( p );
   }

   // -------------------------------------------------------------------------

   template <class C>
   IsoString& ToSeparated( const C& c, char_type separator )
   {
      (void)string_base::ToSeparated( c, separator );
      return *this;
   }

   template <class C, class AF>
   IsoString& ToSeparated( const C& c, char_type separator, AF append )
   {
      (void)string_base::ToSeparated( c, separator, append );
      return *this;
   }

   template <class C>
   IsoString& ToSeparated( const C& c, const IsoString& separator )
   {
      (void)string_base::ToSeparated( c, separator );
      return *this;
   }

   template <class C, class AF>
   IsoString& ToSeparated( const C& c, const IsoString& separator, AF append )
   {
      (void)string_base::ToSeparated( c, separator, append );
      return *this;
   }

   template <class C>
   IsoString& ToSeparated( const C& c, const_c_string separator )
   {
      (void)string_base::ToSeparated( c, separator );
      return *this;
   }

   template <class C, class AF>
   IsoString& ToSeparated( const C& c, const_c_string separator, AF append )
   {
      (void)string_base::ToSeparated( c, separator, append );
      return *this;
   }

   template <class C>
   IsoString& ToCommaSeparated( const C& c )
   {
      (void)string_base::ToCommaSeparated( c );
      return *this;
   }

   template <class C>
   IsoString& ToSpaceSeparated( const C& c )
   {
      (void)string_base::ToSpaceSeparated( c );
      return *this;
   }

   template <class C>
   IsoString& ToTabSeparated( const C& c )
   {
      (void)string_base::ToTabSeparated( c );
      return *this;
   }

   template <class C>
   IsoString& ToNewLineSeparated( const C& c )
   {
      (void)string_base::ToNewLineSeparated( c );
      return *this;
   }

   template <class C>
   IsoString& ToNullSeparated( const C& c )
   {
      (void)string_base::ToNullSeparated( c );
      return *this;
   }

   template <class C>
   IsoString& ToHyphenated( const C& c )
   {
      (void)string_base::ToHyphenated( c );
      return *this;
   }

   // -------------------------------------------------------------------------

   /*!
    * Replaces all occurrences of HTML special characters in this string with
    * valid HTML entities. Returns a reference to this string.
    *
    * The following replacements are performed:
    *
    * '&' (ampersand) becomes "\&amp;" \n
    * '"' (double quote) becomes "\&quot;" \n
    * "'" (single quote) becomes "\&#039;" \n
    * '<' (less than) becomes "\&lt;" \n
    * '>' (greater than) becomes "\&gt;"
    */
   IsoString& ToEncodedHTMLSpecialChars();

   /*!
    * Returns a duplicate of this string with all occurrences of HTML special
    * characters replaced with valid HTML entities.
    *
    * \sa ToEncodedHTMLSpecialChars()
    */
   IsoString EncodedHTMLSpecialChars() const
   {
      return IsoString( *this ).ToEncodedHTMLSpecialChars();
   }

   /*!
    * Replaces all occurrences of special HTML entities in this string with
    * their corresponding plain text character equivalents. Returns a reference
    * to this string.
    *
    * The following replacements are performed:
    *
    * "\&amp;" (ampersand) becomes '&' \n
    * "\&quot;" (double quote) becomes '"' \n
    * "\&#039;" (single quote) becomes "'" \n
    * "\&apos;" (apostrophe) becomes "'" \n
    * "\&lt;" (less than) becomes '<' \n
    * "\&gt;" (greater than) becomes '>'
    */
   IsoString& ToDecodedHTMLSpecialChars();

   /*!
    * Returns a duplicate of this string with all occurrences of special HTML
    * entities replaced with their corresponding plain text character
    * equivalents.
    *
    * \sa ToDecodedHTMLSpecialChars()
    */
   IsoString DecodedHTMLSpecialChars() const
   {
      return IsoString( *this ).ToDecodedHTMLSpecialChars();
   }

   // -------------------------------------------------------------------------

#ifdef __PCL_QT_INTERFACE

   operator QString() const
   {
      return QString( c_str() );
   }

   operator QByteArray() const
   {
      return QByteArray( c_str() );
   }

   operator QDate() const
   {
      return QDate::fromString( c_str(), PCL_QDATE_FMT_STR );
   }

   operator QDateTime() const
   {
      return QDateTime::fromString( c_str(), PCL_QDATETIME_FMT_STR );
   }

#endif

   /*!
    * Replaces the contents of this string with a formatted representation of a
    * variable-length set of values. Returns a reference to this string.
    *
    * The \a fmt null-terminated string is a standard <em>printf format
    * string</em>. It follows the same rules as its counterpart parameter in
    * the standard printf( const char* fmt, ... ) C runtime function.
    *
    * The required space to store the resulting formatted output is calculated
    * and allocated transparently.
    */
   IsoString& Format( const_c_string fmt, ... )
   {
      va_list paramList;
      va_start( paramList, fmt );

      (void)VFormat( fmt, paramList );

      va_end( paramList );
      return *this;
   }

   /*!
    * Appends a formatted representation of a variable-length set of values to
    * the current contents of this string. Returns a reference to this string.
    *
    * The \a fmt null-terminated string is a standard <em>printf format
    * string</em>. It follows the same rules as its counterpart parameter in
    * the standard printf( const char* fmt, ... ) C runtime function.
    *
    * The required space to store the resulting formatted output is calculated
    * and allocated transparently.
    */
   IsoString& AppendFormat( const_c_string fmt, ... )
   {
      va_list paramList;
      va_start( paramList, fmt );

      (void)AppendVFormat( fmt, paramList );

      va_end( paramList );
      return *this;
   }

   /*!
    * Replaces the contents of this string with a formatted representation of a
    * variable-length set of values, specified as a \c va_list standard
    * parameter list. Returns the number of characters generated.
    *
    * The \a fmt null-terminated string is a standard <em>printf format
    * string</em>. It follows the same rules as its counterpart parameter in
    * the standard printf( const char* fmt, ... ) C runtime function.
    *
    * The required space to store the resulting formatted output is calculated
    * and allocated transparently.
    */
   int VFormat( const_c_string fmt, va_list paramList );

   /*!
    * Appends a formatted representation of a variable-length set of values,
    * specified as a \c va_list standard parameter list, to the current
    * contents of this string. Returns the number of characters appended.
    *
    * The \a fmt null-terminated string is a standard <em>printf format
    * string</em>. It follows the same rules as its counterpart parameter in
    * the standard printf( const char* fmt, ... ) C runtime function.
    *
    * The required space to store the resulting formatted output is calculated
    * and allocated transparently.
    */
   int AppendVFormat( const_c_string fmt, va_list paramList );

   // -------------------------------------------------------------------------

   /*!
    * Returns a UTF-16 string with a converted copy of this %IsoString object.
    *
    * This function assumes that this %IsoString object contains only valid
    * ISO/IEC-8859-1 characters. To perform UTF-8 to UTF-16 conversions, see
    * UTF8ToUTF16() and MBSToWCS().
    *
    * \sa UTF8ToUTF16(), MBSToWCS()
    */
   ustring_base ToString() const
   {
      ustring_base s;
      s.SetLength( Length() );
      uchar_iterator p = s.Begin();
      for ( const_iterator i = m_data->string; i < m_data->end; ++p, ++i )
         *p = uchar_type( *i );
      return s;
   }

   /*!
    * Returns an UTF-16 string with a representation of a subset of \a n
    * contiguous UTF-8 characters from this %IsoString object, starting at the
    * \a i-th character.
    *
    * \sa ToString(), MBSToWCS()
    */
   ustring_base UTF8ToUTF16( size_type i = 0, size_type n = maxPos ) const; // implemented inline after String

   /*!
    * Returns a copy of this <em>multibyte string</em> converted to an UTF-16
    * string. This conversion is dependent on the current locale.
    *
    * In the event of conversion error (if there are invalid multibyte
    * characters in the source string) this routine returns an empty string.
    *
    * This member function is a convenience wrapper for the mbstowcs() routine
    * of the standard C runtime library. Note that on platforms where the size
    * of wchar_t is four bytes (e.g. Linux) this routine performs an additional
    * conversion from UTF-32 to UTF-16. On platforms where the size of wchar_t
    * is two bytes (e.g. Windows), the conversion is direct.
    *
    * \sa UTF8ToUTF16(), ToString()
    */
   ustring_base MBSToWCS() const;

#ifdef __PCL_QT_INTERFACE

   QString ToQString() const
   {
      return operator QString();
   }

   QByteArray ToQByteArray() const
   {
      return operator QByteArray();
   }

   QDate ToQDate() const
   {
      return operator QDate();
   }

   QDateTime ToQDateTime() const
   {
      return operator QDateTime();
   }

#endif

   /*!
    * Evaluates this string as a Boolean literal, and returns the result as a
    * \c bool value.
    *
    * Returns \c true if this string is equal to "1", "true", "TRUE" or "T".
    * Returns \c false if this string is equal to "0", "false", "FALSE" or "F".
    * Otherwise this function throws a ParseError exception.
    *
    * \sa TryToBool()
    */
   bool ToBool() const;

   /*!
    * Attempts to evaluate this string as a Boolean literal.
    *
    * If this string can legally be converted to a Boolean value, this function
    * returns \c true and stores the evaluation result in the \a value
    * variable. A string can only be converted to Boolean type if it is equal
    * to either "1", "true", "TRUE", "T", "0", "false", "FALSE" or "F".
    *
    * If this string cannot be converted to a Boolean value, this function
    * returns \c false and does not change the \a value variable. This function
    * does not throw any exception.
    *
    * \sa ToBool()
    */
   bool TryToBool( bool& value ) const;

   /*!
    * Evaluates this string as a floating point numeric literal, and returns
    * the result as a \c float value.
    *
    * For information about the legal syntax of a floating point literal, see
    * the documentation for the ToDouble() member function.
    *
    * If this string doesn't contain a valid floating point literal, or if the
    * range of \c float is exceeded, this member function throws a ParseError
    * exception.
    *
    * \sa TryToFloat()
    */
   float ToFloat() const;

   /*!
    * Attempts to evaluate this string as a floating point numeric literal.
    *
    * If this string can legally be converted to a floating point number, this
    * function returns \c true and stores the evaluation result in the \a value
    * variable. For information about the legal syntax of a floating point
    * literal, see the documentation for the ToDouble() member function.
    *
    * If this string cannot be converted to a floating point number, this
    * function returns \c false and does not change the \a value variable. This
    * function does not throw any exception.
    *
    * \sa ToFloat()
    */
   bool TryToFloat( float& value ) const;

   /*!
    * Evaluates this string as a floating point literal, and returns the result
    * as a \c double value.
    *
    * The source string is expected to have the following format (informal
    * format specification):
    *
    * [+|-][\<integer-part\>][.[\<decimal-part\>]][\<exp\>[+|-]\<exponent\>]
    *
    * where \<integer-part\> and \<decimal-part\> are optional sequences of
    * decimal digits from 0 to 9, \<exp\> is an <em>exponent specifier</em>
    * (the letter 'e' (or 'E')), and \<exponent\> is a sequence of decimal
    * digits specifying a power of ten that multiplies the preceding numeric
    * constant. At least a one-digit integer part, or a one-digit decimal part,
    * is mandatory. FORTRAN exponent specifiers ('d' and 'f' (or 'D' and 'F'))
    * are also recognized by this implementation.
    *
    * If this string doesn't contain a valid floating point literal, or if the
    * range of \c double is exceeded, this member function throws a ParseError
    * exception.
    *
    * \sa TryToDouble()
    */
   double ToDouble() const;

   /*!
    * Attempts to evaluate this string as a floating point numeric literal.
    *
    * If this string can legally be converted to a floating point number, this
    * function returns \c true and stores the evaluation result in the \a value
    * variable. For information about the legal syntax of a floating point
    * literal, see the documentation for the ToDouble() member function.
    *
    * If this string cannot be converted to a floating point number, this
    * function returns \c false and does not change the \a value variable. This
    * function does not throw any exception.
    *
    * \sa ToDouble()
    */
   bool TryToDouble( double& value ) const;

   /*!
    * Evaluates this string as an integer literal, and returns the result as a
    * \c long value.
    *
    * Calling this member function for a string \c s is equivalent to:
    *
    * \code long n = s.ToInt( 0 ); \endcode
    *
    * See the documentation under ToInt( int ) for information about automatic
    * base conversions when \a base=0 is specified.
    *
    * If this string doesn't contain a valid integer literal, or if the range
    * of \c long is exceeded, this member function throws a ParseError
    * exception.
    *
    * \sa TryToInt()
    */
   long ToInt() const
   {
      return ToInt( 0 );
   }

   /*!
    * Attempts to evaluate this string as an integer numeric literal.
    *
    * If this string can legally be converted to an integer number, this
    * function returns \c true and stores the evaluation result in the \a value
    * variable.
    *
    * Calling this member function for a string \c s is equivalent to:
    *
    * \code
    * int v;
    * if ( s.TryToInt( v, 0 ) ) ...
    * \endcode
    *
    * See the documentation under ToInt( int ) for information about automatic
    * base conversions when \a base=0 is specified.
    *
    * If this string cannot be converted to an integer number, this function
    * returns \c false and does not change the \a value variable. This function
    * does not throw any exception.
    *
    * \sa ToInt()
    */
   bool TryToInt( int& value ) const
   {
      return TryToInt( value, 0 );
   }

   /*!
    * Evaluates this string as an integer literal in the specified \a base, and
    * returns the result as a \c long value.
    *
    * If \a base is 0, the source string is expected to represent either a
    * decimal constant, an octal constant, or a hexadecimal constant, any of
    * which optionally preceded by a sign character (+ or -). A decimal
    * constant begins with a non-zero digit, and consists of a sequence of
    * decimal digits from '0' to '9'. An octal begins with the prefix '0',
    * optionally followed by a sequence of the digits 0 to 7 only. A
    * hexadecimal constant begins with the prefix '0x' or '0X', which must be
    * followed by a sequence of decimal digits and letters from 'a' (or 'A')
    * to 'f' (or 'F'), whose corresponding decimal values are from 10 to 15,
    * respectively.
    *
    * Other legal values of \a base, from 2 to 36, specify the expected base of
    * the integer constant represented by the source string. Decimal digits and
    * letters from 'a' (or 'A') to 'z' (or 'Z') are used to represent all
    * possible digits in the specified base, as necessary.
    *
    * If this string doesn't contain a valid integer literal in the specified
    * \a base, if an illegal \a base is specified, or if the range of \c long
    * is exceeded, this member function throws a ParseError exception.
    *
    * \sa TryToInt( int&, int ) const
    */
   long ToInt( int base ) const;

   /*!
    * Attempts to evaluate this string as an integer numeric literal in the
    * specified \a base.
    *
    * If this string can legally be converted to an integer number in the
    * specified \a base, this function returns \c true and stores the
    * evaluation result in the \a value variable.
    *
    * See the documentation under ToInt( int ) for information about automatic
    * base conversions when \a base=0 is specified.
    *
    * If this string cannot be converted to an integer number in the specified
    * \a base, this function returns \c false and does not change the \a value
    * variable. This function does not throw any exception.
    *
    * \sa ToInt( int ) const
    */
   bool TryToInt( int& value, int base ) const;

   /*!
    * Evaluates this string as an unsigned integer literal, and returns the
    * result as an \c unsigned \c long value.
    *
    * Calling this member function for a string \c s is equivalent to:
    *
    * \code unsigned long n = s.ToUInt( 0 ); \endcode
    *
    * See the documentation under ToInt( int ) for information about automatic
    * base conversions when \a base=0 is specified.
    *
    * If this string doesn't contain a valid unsigned integer literal, or if
    * the range of \c unsigned \c long is exceeded, this member function throws
    * a ParseError exception.
    *
    * \sa TryToUInt()
    */
   unsigned long ToUInt() const
   {
      return ToUInt( 0 );
   }

   /*!
    * Attempts to evaluate this string as an unsigned integer literal.
    *
    * If this string can legally be converted to an unsigned integer number,
    * this function returns \c true and stores the evaluation result in the
    * \a value variable.
    *
    * Calling this member function for a string \c s is equivalent to:
    *
    * \code
    * unsigned v;
    * if ( s.TryToUInt( v, 0 ) ) ...
    * \endcode
    *
    * See the documentation under ToInt( int ) for information about automatic
    * base conversions when \a base=0 is specified.
    *
    * If this string cannot be converted to an integer number, this function
    * returns \c false and does not change the \a value variable. This function
    * does not throw any exception.
    *
    * \sa ToUInt()
    */
   bool TryToUInt( unsigned& value ) const
   {
      return TryToUInt( value, 0 );
   }

   /*!
    * Evaluates this string as an unsigned integer literal in the specified
    * \a base, and returns the result as an \c unsigned \c long value.
    *
    * For information about possible values of \a base and how these are
    * interpreted, see the documentation under ToInt( int ). The only exception
    * is that for this member function, only a + sign is legal preceding the
    * numeric constant represented by the source string.
    *
    * If this string doesn't contain a valid integer literal in the specified
    * \a base, if an illegal \a base is specified, or if the range of
    * \c unsigned \c long is exceeded, this member function throws a ParseError
    * exception.
    *
    * \sa TryToUInt( unsigned&, int ) const
    */
   unsigned long ToUInt( int base ) const;

   /*!
    * Attempts to evaluate this string as an unsigned integer literal in the
    * specified \a base.
    *
    * If this string can legally be converted to an unsigned integer number in
    * the specified \a base, this function returns \c true and stores the
    * evaluation result in the \a value variable.
    *
    * See the documentation under ToUInt( int ) for information about automatic
    * base conversions when \a base=0 is specified.
    *
    * If this string cannot be converted to an unsigned integer number in the
    * specified \a base, this function returns \c false and does not change the
    * \a value variable. This function does not throw any exception.
    *
    * \sa ToUInt( int ) const
    */
   bool TryToUInt( unsigned& value, int base ) const;

   /*!
    * Evaluates this string as an integer literal, and returns the result as a
    * \c long \c long value.
    *
    * Calling this member function for a string \c s is equivalent to:
    *
    * \code long long n = s.ToInt64( 0 ); \endcode
    *
    * If this string doesn't contain a valid integer literal, or if the range
    * of \c long \c long is exceeded, this member function throws a ParseError
    * exception.
    *
    * \sa TryToInt64()
    */
   long long ToInt64() const
   {
      return ToInt64( 0 );
   }

   /*!
    * Attempts to evaluate this string as a 64-bit integer numeric literal in
    * the specified \a base.
    *
    * If this string can legally be converted to a 64-bit integer number in the
    * specified \a base, this function returns \c true and stores the
    * evaluation result in the \a value variable.
    *
    * Calling this member function for a string \c s is equivalent to:
    *
    * \code
    * long long v;
    * if ( s.TryToInt64( v, 0 ) ) ...
    * \endcode
    *
    * See the documentation under ToInt( int ) for information about automatic
    * base conversions when \a base=0 is specified.
    *
    * If this string cannot be converted to a 64-bit integer number, this
    * function returns \c false and does not change the \a value variable. This
    * function does not throw any exception.
    *
    * \sa ToInt64()
    */
   bool TryToInt64( long long& value ) const
   {
      return TryToInt64( value, 0 );
   }

   /*!
    * Evaluates this string as an unsigned integer literal in the specified
    * \a base, and returns the result as a \c long \c long value.
    *
    * For information about possible values of \a base and how these are
    * interpreted, see the documentation under ToInt( int ).
    *
    * If this string doesn't contain a valid integer literal in the specified
    * \a base, if an illegal \a base is specified, or if the range of
    * \c long \c long is exceeded, this member function throws a ParseError
    * exception.
    *
    * \sa TryToInt64( long long&, int ) const
    */
   long long ToInt64( int base ) const;

   /*!
    * Attempts to evaluate this string as a 64-bit integer numeric literal in
    * the specified \a base.
    *
    * If this string can legally be converted to a 64-bit integer number in the
    * specified \a base, this function returns \c true and stores the
    * evaluation result in the \a value variable.
    *
    * For information about possible values of \a base and how these are
    * interpreted, see the documentation under ToInt( int ).
    *
    * If this string cannot be converted to a 64-bit integer number in the
    * specified \a base, this function returns \c false and does not change the
    * \a value variable. This function does not throw any exception.
    *
    * \sa ToInt64( int ) const
    */
   bool TryToInt64( long long& value, int base ) const;

   /*!
    * Evaluates this string as an unsigned integer literal in the specified
    * \a base, and returns the result as an \c unsigned \c long \c long value.
    *
    * Calling this member function for a string \c s is equivalent to:
    *
    * \code unsigned long long n = s.ToUInt64( 0 ); \endcode
    *
    * If this string doesn't contain a valid unsigned integer literal, or if
    * the range of \c unsigned \c long \c long is exceeded, this member
    * function throws a ParseError exception.
    *
    * \sa TryToUInt64()
    */
   unsigned long long ToUInt64() const
   {
      return ToUInt64( 0 );
   }

   /*!
    * Attempts to evaluate this string as a 64-bit unsigned integer numeric
    * literal in the specified \a base.
    *
    * If this string can legally be converted to a 64-bit unsigned integer
    * number in the specified \a base, this function returns \c true and stores
    * the evaluation result in the \a value variable.
    *
    * Calling this member function for a string \c s is equivalent to:
    *
    * \code
    * unsigned long long v;
    * if ( s.TryToUInt64( v, 0 ) ) ...
    * \endcode
    *
    * See the documentation under ToInt( int ) for information about automatic
    * base conversions when \a base=0 is specified.
    *
    * If this string cannot be converted to a 64-bit unsigned integer number,
    * this function returns \c false and does not change the \a value variable.
    * This function does not throw any exception.
    *
    * \sa ToUInt64()
    */
   bool TryToUInt64( unsigned long long& value ) const
   {
      return TryToUInt64( value, 0 );
   }

   /*!
    * Evaluates this string as an unsigned integer literal in the specified
    * \a base, and returns the result as an \c unsigned \c long \c long value.
    *
    * For information about possible values of \a base and how these are
    * interpreted, see the documentation under ToInt( int ).
    *
    * If this string doesn't contain a valid unsigned integer literal in the
    * specified \a base, if an illegal \a base is specified, or if the range of
    * \c unsigned \c long \c long is exceeded, this member function throws a
    * ParseError exception.
    *
    * \sa TryToUInt64( unsigned long long&, int ) const
    */
   unsigned long long ToUInt64( int base ) const;

   /*!
    * Attempts to evaluate this string as an unsigned 64-bit integer numeric
    * literal in the specified \a base.
    *
    * If this string can legally be converted to an unsigned 64-bit integer
    * number in the specified \a base, this function returns \c true and stores
    * the evaluation result in the \a value variable.
    *
    * For information about possible values of \a base and how these are
    * interpreted, see the documentation under ToInt( int ).
    *
    * If this string cannot be converted to an unsigned 64-bit integer number
    * in the specified \a base, this function returns \c false and does not
    * change the \a value variable. This function does not throw any exception.
    *
    * \sa ToUInt64( int ) const
    */
   bool TryToUInt64( unsigned long long& value, int base ) const;

   /*!
    * Evaluates this string as a sexagesimal numeric literal representation,
    * and returns the result as a \c double value.
    *
    * The source string is expected to have the following format:
    *
    * [+|-]\<s1\>[\<sep\>\<s2\>[\<sep\>\<s3\>]][.[\<decimal-part\>]]
    *
    * where \<s1\>, \<s2\>, \<s3\> and \<decimal-part\> are sequences of
    * decimal digits from 0 to 9, and \<sep\> is any sequence of characters
    * specified as the \a separator argument. The default separator is the
    * colon character, ':'.
    *
    * The result is equal to:
    *
    * \<sign\> * (\<s1\> + \<s2\>/60 + \<s3\>/3600)
    *
    * where omitted optional components are taken as zero, and \<sign\> is -1
    * iff a minus sign '-' leads the representation, or +1 otherwise.
    *
    * If this string doesn't contain a valid sexagesimal numeric literal, or if
    * the range of \c double is exceeded, this member function throws a
    * ParseError exception.
    *
    * \sa TrySexagesimalToDouble(), ParseSexagesimal(), TryParseSexagesimal()
    */
   double SexagesimalToDouble( const IsoString& separator = ':' ) const
   {
      int sign, s1, s2; double s3;
      ParseSexagesimal( sign, s1, s2, s3, separator );
      return sign*(s1 + (s2 + s3/60)/60);
   }

   template <class S>
   double SexagesimalToDouble( const S& separator ) const
   {
      return SexagesimalToDouble( IsoString( separator ) );
   }

   /*!
    * Attempts to evaluate this string as a sexagesimal numeric literal with
    * the specified \a separator.
    *
    * If this string can legally be evaluated as a sexagesimal literal and
    * converted to a floating point number, this function stores the evaluation
    * result in the specified \a value variable and returns \c true. For
    * information about the legal syntax of a sexagesimal literal, see the
    * documentation for SexagesimalToDouble().
    *
    * If this string cannot be evaluated as a sexagesimal literal, this
    * function returns \c false and does not change the \a value variable. This
    * function does not throw any exception.
    *
    * \sa SexagesimalToDouble(), ParseSexagesimal(), TryParseSexagesimal()
    */
   bool TrySexagesimalToDouble( double& value, const IsoString& separator = ':' ) const
   {
      int sign, s1, s2; double s3;
      if ( TryParseSexagesimal( sign, s1, s2, s3, separator ) )
      {
         value = sign*(s1 + (s2 + s3/60)/60);
         return true;
      }
      return false;
   }

   template <class S>
   bool TrySexagesimalToDouble( double& value, const S& separator ) const
   {
      return TrySexagesimalToDouble( value, IsoString( separator ) );
   }

   /*!
    * Evaluates this string as a sexagesimal numeric literal representation,
    * using the specified \a separator, and stores the resulting components in
    * the specified \a sign, \a s1, \a s2 and \a s3 variables.
    *
    * The output value of \a sign is either -1 or +1, respectively for negative
    * and positive values.
    *
    * For information about the legal syntax of a sexagesimal literal, see the
    * documentation for SexagesimalToDouble().
    *
    * This function stores \e canonical minutes and seconds components,
    * irrespective of the actual component values represented by this string.
    * The output sexagesimal component values are such that:
    *
    * 0 &le; \a s1 \n
    * 0 &le; \a s2 &lt; 60 \n
    * 0 &le; \a s3 &lt; 60 \n
    *
    * If this string doesn't contain a valid sexagesimal numeric literal, or if
    * the range of \c double is exceeded, this member function throws a
    * ParseError exception.
    *
    * \sa TryParseSexagesimal(), SexagesimalToDouble(), TrySexagesimalToDouble()
    */
   void ParseSexagesimal( int& sign, int& s1, int& s2, double& s3, const IsoString& separator = ':' ) const;

   template <class S>
   void ParseSexagesimal( int& sign, int& s1, int& s2, double& s3, const S& separator ) const
   {
      ParseSexagesimal( sign, s1, s2, s3, IsoString( separator ) );
   }

   /*!
    * Attempts to evaluate this string as a sexagesimal numeric literal
    * representation, using the specified \a separator. If successful, stores
    * the resulting components in the specified \a sign, \a s1, \a s2 and \a s3
    * variables, and returns \c true. For more information on syntax and
    * output values and ranges, see the documentation for ParseSexagesimal().
    *
    * If this string cannot be evaluated as a sexagesimal literal, this
    * function returns \c false and does not change any of the \a sign, \a s1,
    * \a s2 and \a s3 variables. This function does not throw any exception.
    *
    * \sa ParseSexagesimal(), SexagesimalToDouble(), TrySexagesimalToDouble()
    */
   bool TryParseSexagesimal( int& sign, int& s1, int& s2, double& s3, const IsoString& separator = ':' ) const;

   template <class S>
   bool TryParseSexagesimal( int& sign, int& s1, int& s2, double& s3, const S& separator ) const
   {
      return TryParseSexagesimal( sign, s1, s2, s3, IsoString( separator ) );
   }

   /*!
    * Returns a sexagesimal ASCII representation of the specified components
    * \a sign, \a s1, \a s2 and \a s3, generated with the specified conversion
    * \a options.
    *
    * The generated representation will correspond to a negative value iff
    * \a sign < 0, positive otherwise.
    *
    * The generated representation is of the form:
    *
    * [+|-]\<s1\>[\<sep\>\<s2\>[\<sep\>\<s3\>]][.[\<decimal-part\>]]
    *
    * where the second and third sexagesimal components are canonicalized:
    *
    * 0 &le; \<s2\> &lt; 60
    * 0 &le; \<s3\> &lt; 60
    *
    * irrespective of the original \a s2 and \a s3 argument values.
    *
    * \sa SexagesimalToDouble(), SexagesimalConversionOptions
    */
   static IsoString ToSexagesimal( int sign, double s1, double s2, double s3,
                                   const SexagesimalConversionOptions& options = SexagesimalConversionOptions() );

   /*!
    * Returns a sexagesimal ASCII representation of the specified decimal value
    * \a d, generated with the specified conversion \a options.
    *
    * Calling this member function is equivalent to:
    *
    * \code ToSexagesimal( (d < 0) ? -1 : +1, Abs( d ), 0, 0, options ); \endcode
    */
   static IsoString ToSexagesimal( double d, const SexagesimalConversionOptions& options = SexagesimalConversionOptions() )
   {
      return ToSexagesimal( (d < 0) ? -1 : +1, Abs( d ), 0, 0, options );
   }

   /*!
    * Evaluates this string as a date and time specification in ISO 8601
    * extended format, and stores the resulting components in the specified
    * variables.
    *
    * \param year    On output, the year of the date.
    *
    * \param month   On output, the month of the date in the range [1,12].
    *
    * \param day     On output, the day of the date in the range [1,31].
    *
    * \param dayf    On output, the day fraction corresponding to the time
    *                specification, in the range [0,1).
    *
    * \param tz      On output, the time zone offset in hours, in the range
    *                [-12,+12].
    *
    * In ISO 8601 extended representations, decimal fractions must be divided
    * from integer parts exclusively by the full stop or dot character ('.',
    * ASCII code point 46(10) = 2E(16)).
    *
    * \sa TryParseISO8601DateTime(), TimePoint::FromString()
    */
   void ParseISO8601DateTime( int& year, int& month, int& day, double& dayf, double& tz ) const;

   /*!
    * Attempts to evaluate this string as a date and time specification in ISO
    * 8601 extended format. If successful, stores the resulting components in
    * the specified \a year, \a month, \a day and \a dayf and \a tz variables,
    * and returns \c true. For more information on syntax and
    * output values and ranges, see the ParseISO8601DateTime().
    *
    * If this string cannot be evaluated as a valid date and time in ISO 8601
    * format, this function returns \c false and does not change any of the
    * passed variables. This function does not throw any exception.
    *
    * \sa ParseISO8601DateTime(), TimePoint::TryFromString()
    */
   bool TryParseISO8601DateTime( int& year, int& month, int& day, double& dayf, double& tz ) const;

   /*!
    * Returns an ASCII representation of a date and time in ISO 8601 extended
    * format.
    *
    * \param year    The year of the date.
    *
    * \param month   The month of the date in the range [1,12].
    *
    * \param day     The day of the date in the range [1,31].
    *
    * \param dayf    The day fraction corresponding to the time specification,
    *                in the range [0,1).
    *
    * \param tz      The time zone offset in hours, in the range [-12,+12]. The
    *                default value is zero, to be interpreted as UTC.
    *
    * \param options Optional settings to control the representation of date
    *                and time in ISO 8601 format.
    *
    * \sa CurrentUTCISO8601DateTime(), CurrentLocalISO8601DateTime(),
    *     ParseISO8601DateTime(), ISO8601ConversionOptions,
    *     TimePoint::ToIsoString()
    */
   static IsoString ToISO8601DateTime( int year, int month, int day, double dayf, double tz = 0,
                                       const ISO8601ConversionOptions& options = ISO8601ConversionOptions() );

   /*!
    * Returns an ASCII representation of the current UTC date and time in ISO
    * 8601 extended format.
    *
    * \param options Optional settings to control the representation of date
    *                and time in ISO 8601 format.
    *
    * \sa CurrentLocalISO8601DateTime(), ToISO8601DateTime(),
    *     ISO8601ConversionOptions, TimePoint::Now()
    */
   static IsoString CurrentUTCISO8601DateTime( const ISO8601ConversionOptions& options = ISO8601ConversionOptions() );

   /*!
    * Returns an ASCII representation of the current local date and time in ISO
    * 8601 extended format.
    *
    * \param options Optional settings to control the representation of date
    *                and time in ISO 8601 format.
    *
    * \sa CurrentUTCISO8601DateTime(), ToISO8601DateTime(),
    *     ISO8601ConversionOptions
    */
   static IsoString CurrentLocalISO8601DateTime( const ISO8601ConversionOptions& options = ISO8601ConversionOptions() );

   /*!
    * Returns an hex-encoded string for a binary \a data block of the specified
    * \a length in bytes. The hex-encoded string is composed of hexadecimal
    * digits: 0-9 and a-f, and its length is twice that of the input length.
    *
    * \sa ToHex( const C& ), ToBase64()
    */
   static IsoString ToHex( const void* data, size_type length );

   /*!
    * Returns an hex-encoded string for a container \a c, whose contents are
    * treated as <em>raw binary data</em>. The objects stored in the container
    * are considered as a sequence of bytes, irrespective of their actual data
    * types or the classes they are instances of.
    *
    * The type C represents an array of contiguous objects, and must provide
    * PCL container semantics: the Begin() and Length() standard container
    * functions are required.
    *
    * \sa ToHex( const void*, size_type )
    */
   template <class C>
   static IsoString ToHex( const C& c )
   {
      return ToHex( c.Begin(), c.Length()*sizeof( *c.Begin() ) );
   }

   /*!
    * Returns a Base64-encoded string for a binary \a data block of the
    * specified \a length in bytes.
    *
    * Base64 is a MIME content transfer encoding of binary data translated into
    * a base 64 representation composed of printable characters. See RFC 1421
    * (http://tools.ietf.org/html/rfc1421).
    *
    * \sa ToBase64( const C& ), ToHex()
    */
   static IsoString ToBase64( const void* data, size_type length );

   /*!
    * Returns a Base64-encoded string for a container \a c, whose contents are
    * treated as <em>raw binary data</em>. The objects stored in the container
    * are considered as a sequence of bytes, irrespective of their actual data
    * types or the classes they are instances of.
    *
    * The type C represents an array of contiguous objects, and must provide
    * PCL container semantics: the Begin() and Length() standard container
    * functions are required.
    *
    * \sa ToBase64( const void*, size_type )
    */
   template <class C>
   static IsoString ToBase64( const C& c )
   {
      return ToBase64( c.Begin(), c.Length()*sizeof( *c.Begin() ) );
   }

   /*!
    * Decodes an hex-encoded string, and returns the decoded binary raw data
    * stored in a ByteArray object.
    *
    * This function expects this string to be hex-encoded; if it isn't, or if
    * it contains invalid hexadecimal digits, a ParseError exception will be
    * thrown.
    *
    * If this string is empty, this function returns an empty %ByteArray.
    *
    * \sa ToHex(), FromBase64()
    */
   ByteArray FromHex() const;

   /*!
    * Decodes a Base64-encoded string, and returns the decoded binary raw data
    * stored in a ByteArray object.
    *
    * This function expects this string to be Base64-encoded; if it isn't, or
    * if it contains an invalid Base64 representation, a ParseError exception
    * will be thrown.
    *
    * If this string is empty, this function returns an empty %ByteArray.
    *
    * \sa ToBase64(), FromHex()
    */
   ByteArray FromBase64() const;

   /*!
    * Generates a string of \a n random 8-bit code points, with character types
    * and ranges as prescribed by the specified \a options.
    *
    * \sa RandomizationOption
    */
   static IsoString Random( size_type n, RandomizationOptions options = RandomizationOption::Default );

   /*!
    * Generates a universally unique identifier (UUID) in canonical form.
    *
    * The canonical UUID has 36 characters with the following format:
    *
    * xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
    *
    * where 'x' represents a lowercase hexadecimal digit, '4' is the UUID
    * version indicator (version 4 = truly random UUID), and 'y' is one of '8',
    * '9', 'a', or 'b'.
    */
   static IsoString UUID();
};

// ----------------------------------------------------------------------------

/*!
 * \defgroup isostring_concatenation_ops IsoString Concatenation Operators
 */

/*!
 * Returns a string with the concatenation of two strings \a s1 and \a s2.
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( const IsoString::string_base& s1, const IsoString::string_base& s2 )
{
   IsoString s( s1 );
   s.Append( s2 );
   return s;
}

/*!
 * Returns a string with the concatenation of two strings \a s1 (rvalue
 * reference) and \a s2.
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( IsoString::string_base&& s1, const IsoString::string_base& s2 )
{
   s1.Append( s2 );
   return IsoString( std::move( s1 ) );
}

/*!
 * Returns a string with the concatenation of two strings \a s1 (rvalue
 * reference) and \a s2.
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( IsoString&& s1, const IsoString::string_base& s2 )
{
   s1.Append( s2 );
   return s1;
}

/*!
 * Returns a string with the concatenation of two strings \a s1 and \a s2
 * (rvalue reference).
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( const IsoString::string_base& s1, IsoString::string_base&& s2 )
{
   s2.Prepend( s1 );
   return IsoString( std::move( s2 ) );
}

/*!
 * Returns a string with the concatenation of two strings \a s1 and \a s2
 * (rvalue reference).
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( const IsoString::string_base& s1, IsoString&& s2 )
{
   s2.Prepend( s1 );
   return s2;
}

/*!
 * Returns a string with the concatenation of two strings \a s1 (rvalue
 * reference) and \a s2 (rvalue reference).
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( IsoString::string_base&& s1, IsoString::string_base&& s2 )
{
   s1.Append( s2 );
   return IsoString( std::move( s1 ) );
}

/*!
 * Returns a string with the concatenation of two strings \a s1 (rvalue
 * reference) and \a s2 (rvalue reference).
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( IsoString&& s1, IsoString::string_base&& s2 )
{
   s1.Append( s2 );
   return s1;
}

/*!
 * Returns a string with the concatenation of two strings \a s1 (rvalue
 * reference) and \a s2 (rvalue reference).
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( IsoString::string_base&& s1, IsoString&& s2 )
{
   s1.Append( s2 );
   return IsoString( std::move( s1 ) );
}

/*!
 * Returns a string with the concatenation of two strings \a s1 (rvalue
 * reference) and \a s2 (rvalue reference).
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( IsoString&& s1, IsoString&& s2 )
{
   s1.Append( s2 );
   return s1;
}

// ----------------------------------------------------------------------------

/*!
 * Returns a string with the concatenation of a string \a s1 and a
 * null-terminated string \a t2.
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( const IsoString::string_base& s1, IsoString::const_c_string t2 )
{
   IsoString s = s1;
   s.Append( t2 );
   return s;
}

/*!
 * Returns a string with the concatenation of a string \a s1 (rvalue reference)
 * and a null-terminated string \a t2.
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( IsoString::string_base&& s1, IsoString::const_c_string t2 )
{
   s1.Append( t2 );
   return IsoString( std::move( s1 ) );
}

/*!
 * Returns a string with the concatenation of a string \a s1 (rvalue reference)
 * and a null-terminated string \a t2.
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( IsoString&& s1, IsoString::const_c_string t2 )
{
   s1.Append( t2 );
   return s1;
}

/*!
 * Returns a string with the concatenation of a null-terminated string \a t1
 * and a string \a s2.
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( IsoString::const_c_string t1, const IsoString::string_base& s2 )
{
   IsoString s = s2;
   s.Prepend( t1 );
   return s;
}

/*!
 * Returns a string with the concatenation of a null-terminated string \a t1
 * and a string \a s2 (rvalue reference).
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( IsoString::const_c_string t1, IsoString::string_base&& s2 )
{
   s2.Prepend( t1 );
   return IsoString( std::move( s2 ) );
}

/*!
 * Returns a string with the concatenation of a null-terminated string \a t1
 * and a string \a s2 (rvalue reference).
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( IsoString::const_c_string t1, IsoString&& s2 )
{
   s2.Prepend( t1 );
   return s2;
}

// ----------------------------------------------------------------------------

/*!
 * Returns a string with the concatenation of a string \a s1 and a single
 * character \a c2.
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( const IsoString::string_base& s1, IsoString::char_type c2 )
{
   IsoString s = s1;
   s.Append( c2 );
   return s;
}

/*!
 * Returns a string with the concatenation of a string \a s1 (rvalue reference)
 * and a single character \a c2.
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( IsoString::string_base&& s1, IsoString::char_type c2 )
{
   s1.Append( c2 );
   return IsoString( std::move( s1 ) );
}

/*!
 * Returns a string with the concatenation of a string \a s1 (rvalue reference)
 * and a single character \a c2.
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( IsoString&& s1, IsoString::char_type c2 )
{
   s1.Append( c2 );
   return s1;
}

/*!
 * Returns a string with the concatenation of a single character \a c1 and a
 * string \a s2.
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( IsoString::char_type c1, const IsoString::string_base& s2 )
{
   IsoString s = s2;
   s.Prepend( c1 );
   return s;
}

/*!
 * Returns a string with the concatenation of a single character \a c1 and a
 * string \a s2 (rvalue reference).
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( IsoString::char_type c1, IsoString::string_base&& s2 )
{
   s2.Prepend( c1 );
   return IsoString( std::move( s2 ) );
}

/*!
 * Returns a string with the concatenation of a single character \a c1 and a
 * string \a s2 (rvalue reference).
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( IsoString::char_type c1, IsoString&& s2 )
{
   s2.Prepend( c1 );
   return s2;
}

// ----------------------------------------------------------------------------

/*!
 * Appends a string \a s2 to a string \a s1. Returns a reference to the
 * left-hand operand string \a s1
 * \ingroup isostring_concatenation_ops
 */
inline IsoString& operator <<( IsoString& s1, const IsoString::string_base& s2 )
{
   s1.Append( s2 );
   return s1;
}

/*!
 * Appends a string \a s2 to a string \a s1. Returns a reference to the
 * left-hand operand string \a s1
 * \ingroup isostring_concatenation_ops
 */
inline IsoString& operator <<( IsoString&& s1, const IsoString::string_base& s2 )
{
   s1.Append( s2 );
   return s1;
}

/*!
 * Appends a null-terminated string \a t2 to a string \a s1. Returns a
 * reference to the left-hand operand string \a s1
 * \ingroup isostring_concatenation_ops
 */
inline IsoString& operator <<( IsoString& s1, IsoString::const_c_string t2 )
{
   s1.Append( t2 );
   return s1;
}

/*!
 * Appends a null-terminated string \a t2 to a string \a s1. Returns a
 * reference to the left-hand operand string \a s1
 * \ingroup isostring_concatenation_ops
 */
inline IsoString& operator <<( IsoString&& s1, IsoString::const_c_string t2 )
{
   s1.Append( t2 );
   return s1;
}

/*!
 * Appends a single character \a c2 to a string \a s1. Returns a reference to
 * the left-hand operand string \a s1
 * \ingroup isostring_concatenation_ops
 */
inline IsoString& operator <<( IsoString& s1, IsoString::char_type c2 )
{
   s1.Append( c2 );
   return s1;
}

/*!
 * Appends a single character \a c2 to a string \a s1. Returns a reference to
 * the left-hand operand string \a s1
 * \ingroup isostring_concatenation_ops
 */
inline IsoString& operator <<( IsoString&& s1, IsoString::char_type c2 )
{
   s1.Append( c2 );
   return s1;
}

// ----------------------------------------------------------------------------

#ifndef __PCL_NO_STRING_OSTREAM

inline std::ostream& operator <<( std::ostream& o, const IsoString& s )
{
   return o << s.c_str();
}

#endif

// ----------------------------------------------------------------------------

/*!
 * \class String
 * \brief Unicode (UTF-16) string.
 *
 * %String derives from an instantiation of GenericString for \c char16_type.
 * It represents a dynamic string of characters in <em>16-bit Unicode
 * Transformation Format</em> (UTF-16) on the PixInsight platform.
 *
 * \sa IsoString
 */
class PCL_CLASS String : public GenericString<char16_type, CharTraits, StandardAllocator>
{
public:

   /*!
    * Base class of %String.
    */
   typedef GenericString<char16_type, CharTraits, StandardAllocator>
                                                string_base;

   /*!
    * Represents a character pertaining to a %String object.
    */
   typedef string_base::char_type               char_type;

   /*!
    * The character traits class used by this template instantiation.
    */
   typedef string_base::char_traits             char_traits;

   /*!
    * The block allocator used by this template instantiation.
    */
   typedef string_base::block_allocator         block_allocator;

   /*!
    * The allocator class used by this template instantiation.
    */
   typedef string_base::allocator               allocator;

   /*!
    * Null-terminated sequence of UTF-16 characters.
    */
   typedef string_base::c_string                c_string;

   /*!
    * Immutable null-terminated sequence of UTF-16 characters.
    */
   typedef string_base::const_c_string          const_c_string;

   /*!
    * %String iterator.
    */
   typedef string_base::iterator                iterator;

   /*!
    * Immutable %String iterator.
    */
   typedef string_base::const_iterator          const_iterator;

   /*!
    * Reverse %String iterator.
    */
   typedef string_base::reverse_iterator        reverse_iterator;

   /*!
    * Immutable reverse %String iterator.
    */
   typedef string_base::const_reverse_iterator  const_reverse_iterator;

   /*!
    * Represents an 8-bit string. Depending on the context, this type
    * represents a sequence of ISO/IEC-8859-1, ASCII, or UTF-8 code points.
    *
    * \note This type must be defined as the same template instantiation used
    * for the IsoString class.
    */
   typedef GenericString<char, IsoCharTraits, StandardAllocator>
                                                string8_base;

   /*!
    * Represents an 8-bit character (ISO/IEC-8859-1, ASCII or UTF-8).
    */
   typedef string8_base::char_type              char8_type;

   /*!
    * 8-bit character traits class.
    */
   typedef string8_base::char_traits            char8_traits;

   /*!
    * Null-terminated sequence of 8-bit characters.
    */
   typedef string8_base::c_string               c_string8;

   /*!
    * Immutable null-terminated sequence of 8-bit characters.
    */
   typedef string8_base::const_c_string         const_c_string8;

   /*!
    * 8-bit string iterator.
    */
   typedef string8_base::iterator               char8_iterator;

   /*!
    * Immutable 8-bit string iterator.
    */
   typedef string8_base::const_iterator         const_char8_iterator;

   // -------------------------------------------------------------------------

   /*!
    * Constructs an empty %String object.
    */
   String() : string_base()
   {
   }

   /*!
    * Constructs a %String object as a copy of the specified \c string_base
    * string \a s (copy constructor from the base class).
    */
   String( const string_base& s ) : string_base( s )
   {
   }

   /*!
    * Copy constructor.
    */
   String( const String& s ) : string_base( s )
   {
   }

   /*!
    * Constructs a %String object by transferring data from the specified
    * \c string_base string \a s (move constructor from the base class).
    */
   String( string_base&& s ) : string_base( std::move( s ) )
   {
   }

   /*!
    * Move constructor.
    */
   String( String&& s ) : string_base( std::move( s ) )
   {
   }

   /*!
    * Constructs a %String as a transformed copy of the specified 8-bit,
    * ISO/IEC-8859-1 string \a s.
    */
   String( const string8_base& s ) : string_base()
   {
      Assign( s );
   }

   /*!
    * Constructs a %String as a copy of a null-terminated string \a t.
    */
   String( const_iterator t ) : string_base( t )
   {
   }

   /*!
    * Constructs a %String with the \a n first characters of the
    * null-terminated string \a t, starting from its \a i-th character.
    */
   String( const_iterator t, size_type i, size_type n ) : string_base( t, i, n )
   {
   }

   /*!
    * Constructs a %String with \a n copies of a UTF-16 character \a c.
    */
   String( char_type c, size_type n ) : string_base( c, n )
   {
   }

   /*!
    * Constructs a %String with a copy of the character sequence defined by
    * the range [i,j).
    */
   String( const_iterator i, const_iterator j ) : string_base( i, j )
   {
   }

   /*!
    * Constructs a %String as a copy of a null-terminated string \a t of
    * \c wchar_t characters.
    */
   String( const wchar_t* t ) : string_base()
   {
      Assign( t );
   }

   /*!
    * Constructs a %String with the \a n first characters of the
    * null-terminated string \a t of \c wchar_t characters, starting from its
    * \a i-th character.
    */
   String( const wchar_t* t, size_type i, size_type n ) : string_base()
   {
      Assign( t, i, n );
   }

   /*!
    * Constructs a %String with \a n copies of a \c wchar_t character \a c.
    */
   String( wchar_t c, size_type n ) : string_base( char_type( c ), n )
   {
   }

   /*!
    * Constructs a %String as a transformed copy of the null-terminated 8-bit,
    * ISO/IEC-8859-1 string \a t.
    */
   String( const_c_string8 t ) : string_base()
   {
      Assign( t );
   }

   /*!
    * Constructs a %String with the \a n first characters of the
    * null-terminated, 8-bit ISO/IEC-8859-1 string \a t, starting from its
    * \a i-th character.
    */
   String( const_c_string8 t, size_type i, size_type n ) : string_base()
   {
      Assign( t, i, n );
   }

   /*!
    * Constructs a %String with the sequence of 8-bit characters in the range
    * [i,j) of 8-bit string iterators.
    */
   String( const_char8_iterator i, const_char8_iterator j ) : string_base()
   {
      Assign( i, j );
   }

   /*!
    * Constructs a %String object with \a n copies of an ISO/IEC-8859-1
    * character \a c.
    */
   String( char8_type c, size_type n = 1 ) : string_base( char_type( c ), n )
   {
   }

   /*!
    * Constructs a %String as a literal representation of a \c bool value.
    */
   explicit
   String( bool x ) : string_base()
   {
      Assign( x ? "true" : "false" );
   }

   /*!
    * Constructs a %String as a literal representation of a
    * \c signed \c short value.
    */
   explicit
   String( short x ) : string_base()
   {
      (void)Format( L"%hd", x );
   }

   /*!
    * Constructs a %String as a literal representation of an
    * \c unsigned \c short value.
    */
   explicit
   String( unsigned short x ) : string_base()
   {
      (void)Format( L"%hu", x );
   }

   /*!
    * Constructs a %String as a literal representation of a
    * \c signed \c int value.
    */
   explicit
   String( int x ) : string_base()
   {
      (void)Format( L"%i", x );
   }

   /*!
    * Constructs a %String as a literal representation of an
    * \c unsigned \c int value.
    */
   explicit
   String( unsigned int x ) : string_base()
   {
      (void)Format( L"%u", x );
   }

   /*!
    * Constructs a %String as a literal representation of a
    * \c signed \c long value.
    */
   explicit
   String( long x ) : string_base()
   {
      (void)Format( L"%ld", x );
   }

   /*!
    * Constructs a %String as a literal representation of an
    * \c unsigned \c long value.
    */
   explicit
   String( unsigned long x ) : string_base()
   {
      (void)Format( L"%lu", x );
   }

   /*!
    * Constructs a %String as a literal representation of a
    * \c signed \c long \c long value.
    */
   explicit
   String( long long x ) : string_base()
   {
      (void)Format( L"%lli", x );
   }

   /*!
    * Constructs a %String as a literal representation of an
    * \c unsigned \c long \c long value.
    */
   explicit
   String( unsigned long long x ) : string_base()
   {
      (void)Format( L"%llu", x );
   }

   /*!
    * Constructs a %String as a literal representation of a
    * \c float value.
    */
   explicit
   String( float x ) : string_base()
   {
      (void)Format( L"%.7g", x );
   }

   /*!
    * Constructs a %String as a literal representation of a
    * \c double value.
    */
   explicit
   String( double x ) : string_base()
   {
      (void)Format( L"%.16lg", x );
   }

   /*!
    * Constructs a %String as a literal representation of a
    * \c long \c double value.
    */
   explicit
   String( long double x ) : string_base()
   {
#ifdef _MSC_VER
      (void)Format( L"%.16Lg", x );
#else
      (void)Format( L"%.18Lg", x );
#endif
   }

#ifndef __PCL_NO_STRING_COMPLEX

   /*!
    * Constructs a %String as a literal representation of an \c fcomplex value.
    */
   explicit
   String( Complex<float>& x ) : string_base()
   {
      (void)Format( L"{%.7g,%.7g}", x.Real(), x.Imag() );
   }

   /*!
    * Constructs a %String as a literal representation of a \c dcomplex value.
    */
   explicit
   String( Complex<double>& x ) : string_base()
   {
      (void)Format( L"{%.16lg,%.16lg}", x.Real(), x.Imag() );
   }

   /*!
    * Constructs a %String as a literal representation of an \c lcomplex value.
    */
   explicit
   String( Complex<long double>& x ) : string_base()
   {
#ifdef _MSC_VER
      (void)Format( L"{%.16Lg,%.16Lg}", x.Real(), x.Imag() );
#else
      (void)Format( L"{%.18Lg,%.18Lg}", x.Real(), x.Imag() );
#endif
   }

#endif   // !__PCL_NO_STRING_COMPLEX

#ifdef __PCL_QT_INTERFACE

   explicit
   String( const QString& qs ) :
      string_base( qs.isEmpty() ? iterator( nullptr ) : iterator( PCL_GET_CHAR16PTR_FROM_QSTRING( qs ) ) )
   {
   }

   explicit
   String( const QDate& qd ) :
      string_base( iterator( PCL_GET_CHAR16PTR_FROM_QSTRING( qd.toString( PCL_QDATE_FMT_STR ) ) ) )
   {
   }

   explicit
   String( const QDateTime& qdt ) :
      string_base( iterator( PCL_GET_CHAR16PTR_FROM_QSTRING( qdt.toString( PCL_QDATETIME_FMT_STR ) ) ) )
   {
   }

#endif

   // -------------------------------------------------------------------------

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   String& operator =( const String& s )
   {
      string_base::Assign( s );
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   String& operator =( String&& s )
   {
      string_base::Transfer( s );
      return *this;
   }

   /*!
    * Assigns a shallow copy of the \c string_base string \a s to this string.
    * Returns a reference to this object.
    */
   String& operator =( const string_base& s )
   {
      string_base::Assign( s );
      return *this;
   }

   /*!
    * Transfers the data from the \c string_base string \a s to this string
    * (move assignment from base class). Returns a reference to this object.
    */
   String& operator =( string_base&& s )
   {
      string_base::Transfer( s );
      return *this;
   }

   /*!
    * Assigns a copy of the 8-bit ISO/IEC-8859-1 string \a s to this string.
    * Returns a reference to this object.
    */
   String& operator =( const string8_base& s )
   {
      Assign( s );
      return *this;
   }

   /*!
    * Assigns a copy of the null-terminated string \a t to this string. Returns
    * a reference to this object.
    */
   String& operator =( const_iterator t )
   {
      string_base::Assign( t );
      return *this;
   }

   /*!
    * Assigns a single copy of a character \a c to this string. Returns a
    * reference to this object.
    */
   String& operator =( char_type c )
   {
      string_base::Assign( c );
      return *this;
   }

   /*!
    * Assigns a copy of the null-terminated string \a t of \c wchar_t to this
    * string. Returns a reference to this object.
    */
   String& operator =( const wchar_t* t )
   {
      Assign( t );
      return *this;
   }

   /*!
    * Assigns a single copy of a \c wchar_t character \a c to this string.
    * Returns a reference to this object.
    */
   String& operator =( wchar_t c )
   {
      Assign( c );
      return *this;
   }

   /*!
    * Assigns a copy of the null-terminated 8-bit ISO/IEC-8859-1 string \a t to
    * this string. Returns a reference to this object.
    */
   String& operator =( const_c_string8 t )
   {
      Assign( t );
      return *this;
   }

   /*!
    * Assigns a single copy of an ISO/IEC-8859-1 character \a c to this string.
    * Returns a reference to this object.
    */
   String& operator =( char8_type c )
   {
      Assign( c );
      return *this;
   }

#ifdef __PCL_QT_INTERFACE

   String& operator =( const QString& qs )
   {
      if ( qs.isEmpty() )
         Clear();
      else
         string_base::Assign( PCL_GET_CHAR16PTR_FROM_QSTRING( qs ) );
      return *this;
   }

   String& operator =( const QDate& qd )
   {
      string_base::Assign( PCL_GET_CHAR16PTR_FROM_QSTRING( qd.toString( PCL_QDATE_FMT_STR ) ) );
      return *this;
   }

   String& operator =( const QDateTime& qdt )
   {
      string_base::Assign( PCL_GET_CHAR16PTR_FROM_QSTRING( qdt.toString( PCL_QDATETIME_FMT_STR ) ) );
      return *this;
   }

#endif

   // -------------------------------------------------------------------------

   /*!
    * Assigns a string \a s to this string.
    */
   void Assign( const String& s )
   {
      string_base::Assign( s );
   }

   /*!
    * Assigns a contiguous segment of \a n characters of a string \a s,
    * starting from its \a i-th character, to this string.
    */
   void Assign( const String& s, size_type i, size_type n )
   {
      string_base::Assign( s, i, n );
   }

   /*!
    * Assigns a null-terminated string \a t to this string.
    */
   void Assign( const_iterator t )
   {
      string_base::Assign( t );
   }

   /*!
    * Assigns a sequence of characters defined by the range [i,j) to this
    * string.
    */
   void Assign( const_iterator i, const_iterator j )
   {
      string_base::Assign( i, j );
   }

   /*!
    * Assigns a contiguous segment of at most \a n characters of a
    * null-terminated sequence \a t, starting from its \a i-th character, to
    * this string.
    */
   void Assign( const_iterator t, size_type i, size_type n )
   {
      string_base::Assign( t, i, n );
   }

   /*!
    * Assigns \a n copies of a character \a c to this string.
    */
   void Assign( char_type c, size_type n = 1 )
   {
      string_base::Assign( c, n );
   }

   /*!
    * Assigns a null-terminated string \a t of \c wchar_t to this string.
    */
   void Assign( const wchar_t* t );

   /*!
    * Assigns a contiguous segment of \a n characters of a null-terminated
    * string \a t of \c wchar_t, starting from its \a i-th character, to this
    * string.
    */
   void Assign( const wchar_t* t, size_type i, size_type n );

   /*!
    * Assigns \a n copies of a \c wchar_t character \a c to this string.
    */
   void Assign( wchar_t c, size_type n = 1 )
   {
      string_base::Assign( char_type( c ), n );
   }

   /*!
    * Assigns a copy of the 8-bit ISO/IEC-8859-1 string \a s to this string.
    */
   void Assign( const string8_base& s )
   {
      size_type n = s.Length();
      if ( n > 0 )
      {
         MaybeReallocate( n );
         const_char8_iterator t = s.Begin();
         for ( iterator i = m_data->string; i < m_data->end; ++i, ++t )
            *i = char_type( uint8( *t ) );
      }
      else
         Clear();
   }

   /*!
    * Assigns a null-terminated 8-bit ISO/IEC-8859-1 string \a t to this
    * object.
    */
   void Assign( const_c_string8 t )
   {
      size_type n = char8_traits::Length( t );
      if ( n > 0 )
      {
         MaybeReallocate( n );
         for ( iterator i = m_data->string; i < m_data->end; ++i, ++t )
            *i = char_type( uint8( *t ) );
      }
      else
         Clear();
   }

   /*!
    * Assigns a contiguous segment of \a n characters of a null-terminated
    * 8-bit ISO/IEC-8859-1 string \a t, starting from its \a i-th character, to
    * this string.
    */
   void Assign( const_c_string8 t, size_type i, size_type n )
   {
      size_type len = char8_traits::Length( t );
      if ( i < len )
      {
         n = pcl::Min( n, len-i );
         MaybeReallocate( n );
         t += i;
         for ( iterator i = m_data->string; i < m_data->end; ++i, ++t )
            *i = char_type( uint8( *t ) );
      }
      else
         Clear();
   }

   /*!
    * Assigns a contiguous sequence of 8-bit ISO/IEC-8859-1 characters in the
    * range [i,j) to this string, replacing its previous contents.
    */
   void Assign( const_char8_iterator p, const_char8_iterator q )
   {
      if ( p < q )
      {
         MaybeReallocate( q - p );
         for ( iterator i = m_data->string; i < m_data->end; ++i, ++p )
            *i = char_type( uint8( *p ) );
      }
      else
         Clear();
   }

   /*!
    * Assigns \a n copies of an ISO/IEC-8859-1 character \a c to this string.
    */
   void Assign( char8_type c, size_type n = 1 )
   {
      string_base::Assign( char_type( c ), n );
   }

   // -------------------------------------------------------------------------

   void Insert( size_type i, const String& s )
   {
      string_base::Insert( i, s );
   }

   void Insert( size_type i, const_iterator p, const_iterator q )
   {
      string_base::Insert( i, p, q );
   }

   void Insert( size_type i, const_iterator t )
   {
      string_base::Insert( i, t );
   }

   void Insert( size_type i, const_iterator t, size_type n )
   {
      string_base::Insert( i, t, n );
   }

   void Insert( size_type i, char_type c, size_type n = 1 )
   {
      string_base::Insert( i, c, n );
   }

   void Insert( size_type i, const wchar_t* t )
   {
#ifdef __PCL_WINDOWS
      string_base::Insert( i, reinterpret_cast<const_iterator>( t ) );
#else
      string_base::Insert( i, String( t ) );
#endif
   }

   void Insert( size_type i, wchar_t c, size_type n = 1 )
   {
      string_base::Insert( i, String( c, n ) );
   }

   void Insert( size_type i, const string8_base& s, size_type n )
   {
      n = pcl::Min( n, s.Length() );
      if ( n > 0 )
      {
         UninitializedGrow( i, n ); // -> 0 <= i <= len
         const_char8_iterator t = s.Begin();
         for ( iterator p = m_data->string+i, q = p+n; p < q; ++p, ++t )
            *p = char_type( uint8( *t ) );
      }
   }

   void Insert( size_type i, const string8_base& s )
   {
      size_type n = s.Length();
      if ( n > 0 )
      {
         UninitializedGrow( i, n ); // -> 0 <= i <= len
         const_char8_iterator t = s.Begin();
         for ( iterator p = m_data->string+i, q = p+n; p < q; ++p, ++t )
            *p = char_type( uint8( *t ) );
      }
   }

   void Insert( size_type i, const_c_string8 t )
   {
      size_type n = char8_traits::Length( t );
      if ( n > 0 )
      {
         UninitializedGrow( i, n ); // -> 0 <= i <= len
         for ( iterator p = m_data->string+i, q = p+n; p < q; ++p, ++t )
            *p = char_type( uint8( *t ) );
      }
   }

   void Insert( size_type i, const_c_string8 t, size_type n )
   {
      n = pcl::Min( n, char8_traits::Length( t ) );
      if ( n > 0 )
      {
         UninitializedGrow( i, n ); // -> 0 <= i <= len
         for ( iterator p = m_data->string+i, q = p+n; p < q; ++p, ++t )
            *p = char_type( uint8( *t ) );
      }
   }

   void Insert( size_type i, const_char8_iterator p, const_char8_iterator q )
   {
      if ( p < q )
      {
         size_type n = q - p;
         UninitializedGrow( i, n ); // -> 0 <= i <= len
         for ( iterator r = m_data->string+i, s = r+n; r < s; ++r, ++p )
            *r = char_type( uint8( *p ) );
      }
   }

   void Insert( size_type i, char8_type c, size_type n = 1 )
   {
      string_base::Insert( i, char_type( c ), n );
   }

   // -------------------------------------------------------------------------

   void Append( const String& s )
   {
      string_base::Append( s );
   }

   String& operator +=( const String& s )
   {
      Append( s );
      return *this;
   }

   void Append( const_iterator i, const_iterator j )
   {
      string_base::Append( i, j );
   }

   void Append( const_iterator t )
   {
      string_base::Append( t );
   }

   String& operator +=( const_iterator t )
   {
      Append( t );
      return *this;
   }

   void Append( const_iterator t, size_type n )
   {
      string_base::Append( t, n );
   }

   void Append( char_type c, size_type n = 1 )
   {
      string_base::Append( c, n );
   }

   String& operator +=( char_type c )
   {
      Append( c );
      return *this;
   }

   void Append( const wchar_t* t )
   {
#ifdef __PCL_WINDOWS
      string_base::Append( reinterpret_cast<const_iterator>( t ) );
#else
      string_base::Append( String( t ) );
#endif
   }

   String& operator +=( const wchar_t* t )
   {
      Append( t );
      return *this;
   }

   void Append( wchar_t c, size_type n = 1 )
   {
      string_base::Append( char_type( c ), n );
   }

   String& operator +=( wchar_t c )
   {
      Append( c );
      return *this;
   }

   void Append( const string8_base& s )
   {
      Insert( maxPos, s );
   }

   String& operator +=( const string8_base& s )
   {
      Append( s );
      return *this;
   }

   void Append( const string8_base& s, size_type n )
   {
      Insert( maxPos, s, n );
   }

   void Append( const_c_string8 t )
   {
      Insert( maxPos, t );
   }

   String& operator +=( const_c_string8 t )
   {
      Append( t );
      return *this;
   }

   void Append( const_c_string8 t, size_type n )
   {
      Insert( maxPos, t, n );
   }

   void Append( const_char8_iterator p, const_char8_iterator q )
   {
      Insert( maxPos, p, q );
   }

   void Append( char8_type c, size_type n = 1 )
   {
      string_base::Append( char_type( c ), n );
   }

   String& operator +=( char8_type c )
   {
      Append( c );
      return *this;
   }

   void Add( const String& s )
   {
      Append( s );
   }

   void Add( const_iterator i, const_iterator j )
   {
      Append( i, j );
   }

   void Add( const_iterator t )
   {
      Append( t );
   }

   void Add( const_iterator t, size_type n )
   {
      Append( t, n );
   }

   void Add( char_type c, size_type n = 1 )
   {
      Append( c, n );
   }

   void Add( const wchar_t* t )
   {
      Append( t );
   }

   void Add( wchar_t c, size_type n = 1 )
   {
      Append( c, n );
   }

   void Add( const string8_base& s )
   {
      Append( s );
   }

   void Add( const string8_base& s, size_type n )
   {
      Append( s, n );
   }

   void Add( const_c_string8 t )
   {
      Append( t );
   }

   void Add( const_c_string8 t, size_type n )
   {
      Append( t, n );
   }

   void Add( const_char8_iterator p, const_char8_iterator q )
   {
      Append( p, q );
   }

   void Add( char8_type c, size_type n = 1 )
   {
      Append( c, n );
   }

   // -------------------------------------------------------------------------

   void Prepend( const String& s )
   {
      string_base::Prepend( s );
   }

   String& operator -=( const String& s )
   {
      Prepend( s );
      return *this;
   }

   void Prepend( const_iterator i, const_iterator j )
   {
      string_base::Prepend( i, j );
   }

   void Prepend( const_iterator t )
   {
      string_base::Prepend( t );
   }

   String& operator -=( const_iterator t )
   {
      Prepend( t );
      return *this;
   }

   void Prepend( const_iterator t, size_type n )
   {
      string_base::Prepend( t, n );
   }

   void Prepend( char_type c, size_type n = 1 )
   {
      string_base::Prepend( c, n );
   }

   String& operator -=( char_type c )
   {
      Prepend( c );
      return *this;
   }

   void Prepend( const wchar_t* t )
   {
#ifdef __PCL_WINDOWS
      string_base::Prepend( reinterpret_cast<const_iterator>( t ) );
#else
      string_base::Prepend( String( t ) );
#endif
   }

   String& operator -=( const wchar_t* t )
   {
      Prepend( t );
      return *this;
   }

   void Prepend( wchar_t c, size_type n = 1 )
   {
      string_base::Prepend( char_type( c ), n );
   }

   String& operator -=( wchar_t c )
   {
      Prepend( c );
      return *this;
   }

   void Prepend( const string8_base& s )
   {
      Insert( 0, s );
   }

   String& operator -=( const string8_base& s )
   {
      Prepend( s );
      return *this;
   }

   void Prepend( const string8_base& s, size_type n )
   {
      Insert( 0, s, n );
   }

   void Prepend( const_c_string8 t )
   {
      Insert( 0, t );
   }

   String& operator -=( const_c_string8 t )
   {
      Prepend( t );
      return *this;
   }

   void Prepend( const_c_string8 t, size_type n )
   {
      Insert( 0, t, n );
   }

   void Prepend( const_char8_iterator p, const_char8_iterator q )
   {
      Insert( 0, p, q );
   }

   void Prepend( char8_type c, size_type n = 1 )
   {
      string_base::Prepend( String( c, n ) );
   }

   String& operator -=( char8_type c )
   {
      Prepend( c );
      return *this;
   }

   // -------------------------------------------------------------------------

   void Replace( size_type i, size_type n, const String& s )
   {
      string_base::Replace( i, n, s );
   }

   void Replace( size_type i, size_type n, const_iterator t )
   {
      string_base::Replace( i, n, t );
   }

   void Replace( size_type i, size_type n, char_type c, size_type nc = 1 )
   {
      string_base::Replace( i, n, c, nc );
   }

   void Replace( size_type i, size_type n, const wchar_t* t )
   {
#ifdef __PCL_WINDOWS
      string_base::Replace( i, n, reinterpret_cast<const_iterator>( t ) );
#else
      string_base::Replace( i, n, String( t ) );
#endif
   }

   void Replace( size_type i, size_type n, wchar_t c, size_type nc = 1 )
   {
      string_base::Replace( i, n, char_type( c ), nc );
   }

   void Replace( size_type i, size_type n, const_c_string8 t )
   {
      if ( n > 0 )
      {
         size_type len = Length();
         if ( i < len )
         {
            n = pcl::Min( n, len-i );
            if ( n == len )
               Assign( t );
            else
            {
               size_type nt = char8_traits::Length( t );
               if ( nt > 0 )
               {
                  if ( n < nt )
                     UninitializedGrow( i, nt-n );
                  else if ( nt < n )
                     Delete( i, n-nt );
                  else
                     EnsureUnique();

                  for ( iterator p = m_data->string+i, q = p+nt; p < q; ++p, ++t )
                     *p = char_type( *t );
               }
               else
                  Delete( i, n );
            }
         }
      }
   }

   void Replace( size_type i, size_type n, char8_type c, size_type nc = 1 )
   {
      string_base::Replace( i, n, char_type( c ), nc );
   }

   // -------------------------------------------------------------------------

   void ReplaceChar( char_type c1, char_type c2, size_type i = 0, size_type n = maxPos )
   {
      string_base::ReplaceChar( c1, c2, i, n );
   }

   void ReplaceCharIC( char_type c1, char_type c2, size_type i = 0, size_type n = maxPos )
   {
      string_base::ReplaceCharIC( c1, c2, i, n );
   }

   void ReplaceChar( wchar_t c1, wchar_t c2, size_type i = 0, size_type n = maxPos )
   {
      string_base::ReplaceChar( char_type( c1 ), char_type( c2 ), i, n );
   }

   void ReplaceCharIC( wchar_t c1, wchar_t c2, size_type i = 0, size_type n = maxPos )
   {
      string_base::ReplaceCharIC( char_type( c1 ), char_type( c2 ), i, n );
   }

   void ReplaceChar( char8_type c1, char8_type c2, size_type i = 0, size_type n = maxPos )
   {
      string_base::ReplaceChar( char_type( c1 ), char_type( c2 ), i, n );
   }

   void ReplaceCharIC( char8_type c1, char8_type c2, size_type i = 0, size_type n = maxPos )
   {
      string_base::ReplaceCharIC( char_type( c1 ), char_type( c2 ), i, n );
   }

   // -------------------------------------------------------------------------

   void ReplaceString( const String& s1, const String& s2, size_type i = 0 )
   {
      string_base::ReplaceString( s1, s2, i );
   }

   void ReplaceStringIC( const String& s1, const String& s2, size_type i = 0 )
   {
      string_base::ReplaceStringIC( s1, s2, i );
   }

   void ReplaceString( const_iterator t1, const_iterator t2, size_type i = 0 )
   {
      string_base::ReplaceString( t1, t2, i );
   }

   void ReplaceStringIC( const_iterator t1, const_iterator t2, size_type i = 0 )
   {
      string_base::ReplaceStringIC( t1, t2, i );
   }

   void ReplaceString( const wchar_t* t1, const wchar_t* t2, size_type i = 0 )
   {
#ifdef __PCL_WINDOWS
      string_base::ReplaceString( reinterpret_cast<const_iterator>( t1 ),
                                  reinterpret_cast<const_iterator>( t2 ), i );
#else
      string_base::ReplaceString( String( t1 ), String( t2 ), i );
#endif
   }

   void ReplaceStringIC( const wchar_t* t1, const wchar_t* t2, size_type i = 0 )
   {
#ifdef __PCL_WINDOWS
      string_base::ReplaceStringIC( reinterpret_cast<const_iterator>( t1 ),
                                    reinterpret_cast<const_iterator>( t2 ), i );
#else
      string_base::ReplaceStringIC( String( t1 ), String( t2 ), i );
#endif
   }

   void ReplaceString( const_c_string8 t1, const_c_string8 t2, size_type i = 0 )
   {
      string_base::ReplaceString( String( t1 ), String( t2 ), i );
   }

   void ReplaceStringIC( const_c_string8 t1, const_c_string8 t2, size_type i = 0 )
   {
      string_base::ReplaceStringIC( String( t1 ), String( t2 ), i );
   }

   // -------------------------------------------------------------------------

   void DeleteChar( char_type c, size_type i = 0 )
   {
      string_base::DeleteChar( c, i );
   }

   void DeleteCharIC( char_type c, size_type i = 0 )
   {
      string_base::DeleteCharIC( c, i );
   }

   void DeleteChar( wchar_t c, size_type i = 0 )
   {
      string_base::DeleteChar( char_type( c ), i );
   }

   void DeleteCharIC( wchar_t c, size_type i = 0 )
   {
      string_base::DeleteCharIC( char_type( c ), i );
   }

   void DeleteChar( char8_type c, size_type i = 0 )
   {
      string_base::DeleteChar( char_type( c ), i );
   }

   void DeleteCharIC( char8_type c, size_type i = 0 )
   {
      string_base::DeleteCharIC( char_type( c ), i );
   }

   // -------------------------------------------------------------------------

   void DeleteString( const String& s, size_type i = 0 )
   {
      string_base::DeleteString( s, i );
   }

   void DeleteStringIC( const String& s, size_type i = 0 )
   {
      string_base::DeleteStringIC( s, i );
   }

   void DeleteString( const_iterator t, size_type i = 0 )
   {
      string_base::DeleteString( t, i );
   }

   void DeleteStringIC( const_iterator t, size_type i = 0 )
   {
      string_base::DeleteStringIC( t, i );
   }

   void DeleteString( const wchar_t* t, size_type i = 0 )
   {
#ifdef __PCL_WINDOWS
      string_base::DeleteString( reinterpret_cast<const_iterator>( t ), i );
#else
      string_base::DeleteString( String( t ), i );
#endif
   }

   void DeleteStringIC( const wchar_t* t, size_type i = 0 )
   {
#ifdef __PCL_WINDOWS
      string_base::DeleteStringIC( reinterpret_cast<const_iterator>( t ), i );
#else
      string_base::DeleteStringIC( String( t ), i );
#endif
   }

   void DeleteString( const_c_string8 t, size_type i = 0 )
   {
      string_base::DeleteString( String( t ), i );
   }

   void DeleteStringIC( const_c_string8 t, size_type i = 0 )
   {
      string_base::DeleteStringIC( String( t ), i );
   }

   // -------------------------------------------------------------------------

   bool StartsWith( const String& s ) const
   {
      return string_base::StartsWith( s );
   }

   bool StartsWith( const_iterator t ) const
   {
      return string_base::StartsWith( t );
   }

   bool StartsWith( char_type c ) const
   {
      return string_base::StartsWith( c );
   }

   bool StartsWithIC( const String& s ) const
   {
      return string_base::StartsWithIC( s );
   }

   bool StartsWithIC( const_iterator t ) const
   {
      return string_base::StartsWithIC( t );
   }

   bool StartsWithIC( char_type c ) const
   {
      return string_base::StartsWithIC( c );
   }

   bool StartsWith( const wchar_t* t ) const
   {
#ifdef __PCL_WINDOWS
      return string_base::StartsWith( reinterpret_cast<const_iterator>( t ) );
#else
      return string_base::StartsWith( String( t ) );
#endif
   }

   bool StartsWith( wchar_t c ) const
   {
      return string_base::StartsWith( char_type( c ) );
   }

   bool StartsWithIC( const wchar_t* t ) const
   {
#ifdef __PCL_WINDOWS
      return string_base::StartsWithIC( reinterpret_cast<const_iterator>( t ) );
#else
      return string_base::StartsWithIC( String( t ) );
#endif
   }

   bool StartsWithIC( wchar_t c ) const
   {
      return string_base::StartsWithIC( char_type( c ) );
   }

   bool StartsWith( const_c_string8 t ) const
   {
      size_type n = char8_traits::Length( t );
      if ( n == 0 || Length() < n )
         return false;
      for ( const_iterator p = m_data->string, q = p+n; p < q; ++p, ++t )
         if ( *p != char_type( *t ) )
            return false;
      return true;
   }

   bool StartsWith( char8_type c ) const
   {
      return string_base::StartsWith( char_type( c ) );
   }

   bool StartsWithIC( const_c_string8 t ) const
   {
      size_type n = char8_traits::Length( t );
      if ( n == 0 || Length() < n )
         return false;
      for ( const_iterator p = m_data->string, q = p+n; p < q; ++p, ++t )
         if ( char_traits::ToCaseFolded( *p ) != char_type( char8_traits::ToCaseFolded( *t ) ) )
            return false;
      return true;
   }

   bool StartsWithIC( char8_type c ) const
   {
      return string_base::StartsWithIC( char_type( c ) );
   }

   // -------------------------------------------------------------------------

   bool EndsWith( const String& s ) const
   {
      return string_base::EndsWith( s );
   }

   bool EndsWith( const_iterator t ) const
   {
      return string_base::EndsWith( t );
   }

   bool EndsWith( char_type c ) const
   {
      return string_base::EndsWith( c );
   }

   bool EndsWithIC( const String& s ) const
   {
      return string_base::EndsWithIC( s );
   }

   bool EndsWithIC( const_iterator t ) const
   {
      return string_base::EndsWithIC( t );
   }

   bool EndsWithIC( char_type c ) const
   {
      return string_base::EndsWithIC( c );
   }

    bool EndsWith( const wchar_t* t ) const
   {
#ifdef __PCL_WINDOWS
      return string_base::EndsWith( reinterpret_cast<const_iterator>( t ) );
#else
      return string_base::EndsWith( String( t ) );
#endif
   }

   bool EndsWith( wchar_t c ) const
   {
      return string_base::EndsWith( char_type( c ) );
   }

   bool EndsWithIC( const wchar_t* t ) const
   {
#ifdef __PCL_WINDOWS
      return string_base::EndsWithIC( reinterpret_cast<const_iterator>( t ) );
#else
      return string_base::EndsWithIC( String( t ) );
#endif
   }

   bool EndsWithIC( wchar_t c ) const
   {
      return string_base::EndsWithIC( char_type( c ) );
   }

   bool EndsWith( const_c_string8 t ) const
   {
      size_type n = char8_traits::Length( t );
      if ( n == 0 || Length() < n )
         return false;
      for ( const_iterator p = m_data->end-n; p < m_data->end; ++p, ++t )
         if ( *p != char_type( *t ) )
            return false;
      return true;
   }

   bool EndsWith( char8_type c ) const
   {
      return string_base::EndsWith( char_type( c ) );
   }

   bool EndsWithIC( const_c_string8 t ) const
   {
      size_type n = char8_traits::Length( t );
      if ( n == 0 || Length() < n )
         return false;
      for ( const_iterator p = m_data->end-n; p < m_data->end; ++p, ++t )
         if ( char_traits::ToCaseFolded( *p ) != char_type( char8_traits::ToCaseFolded( *t ) ) )
            return false;
      return true;
   }

   bool EndsWithIC( char8_type c ) const
   {
      return string_base::EndsWithIC( char_type( c ) );
   }

   // -------------------------------------------------------------------------

   size_type FindFirst( const String& s, size_type i = 0 ) const
   {
      return string_base::FindFirst( s, i );
   }

   size_type FindFirst( const_iterator t, size_type i = 0 ) const
   {
      return string_base::FindFirst( t, i );
   }

   size_type FindFirst( char_type c, size_type i = 0 ) const
   {
      return string_base::FindFirst( c, i );
   }

   size_type FindFirstIC( const String& s, size_type i = 0 ) const
   {
      return string_base::FindFirstIC( s, i );
   }

   size_type FindFirstIC( const_iterator t, size_type i = 0 ) const
   {
      return string_base::FindFirstIC( t, i );
   }

   size_type FindFirstIC( char_type c, size_type i = 0 ) const
   {
      return string_base::FindFirstIC( c, i );
   }

   size_type FindFirst( const wchar_t* t, size_type i = 0 ) const
   {
#ifdef __PCL_WINDOWS
      return string_base::FindFirst( reinterpret_cast<const_iterator>( t ), i );
#else
      return string_base::FindFirst( String( t ), i );
#endif
   }

   size_type FindFirst( wchar_t c, size_type i = 0 ) const
   {
      return string_base::FindFirst( char_type( c ), i );
   }

   size_type FindFirstIC( const wchar_t* t, size_type i = 0 ) const
   {
#ifdef __PCL_WINDOWS
      return string_base::FindFirstIC( reinterpret_cast<const_iterator>( t ), i );
#else
      return string_base::FindFirstIC( String( t ), i );
#endif
   }

   size_type FindFirstIC( wchar_t c, size_type i = 0 ) const
   {
      return string_base::FindFirstIC( char_type( c ), i );
   }

   size_type FindFirst( const_c_string8 t, size_type i = 0 ) const
   {
      return string_base::FindFirst( String( t ), i );
   }

   size_type FindFirst( char8_type c, size_type i = 0 ) const
   {
      return string_base::FindFirst( char_type( c ), i );
   }

   size_type FindFirstIC( const_c_string8 t, size_type i = 0 ) const
   {
      return string_base::FindFirstIC( String( t ), i );
   }

   size_type FindFirstIC( char8_type c, size_type i = 0 ) const
   {
      return string_base::FindFirstIC( char_type( c ), i );
   }

   //

   size_type Find( const String& s, size_type i = 0 ) const
   {
      return FindFirst( s, i );
   }

   size_type Find( const_iterator t, size_type i = 0 ) const
   {
      return FindFirst( t, i );
   }

   size_type Find( char_type c, size_type i = 0 ) const
   {
      return FindFirst( c, i );
   }

   size_type Find( const wchar_t* t, size_type i = 0 ) const
   {
      return FindFirst( t, i );
   }

   size_type Find( wchar_t c, size_type i = 0 ) const
   {
      return FindFirst( c, i );
   }

   size_type Find( const_c_string8 t, size_type i = 0 ) const
   {
      return FindFirst( t, i );
   }

   size_type Find( char8_type c, size_type i = 0 ) const
   {
      return FindFirst( c, i );
   }

   size_type FindIC( const String& s, size_type i = 0 ) const
   {
      return FindFirstIC( s, i );
   }

   size_type FindIC( const_iterator t, size_type i = 0 ) const
   {
      return FindFirstIC( t, i );
   }

   size_type FindIC( char_type c, size_type i = 0 ) const
   {
      return FindFirstIC( c, i );
   }

   size_type FindIC( const wchar_t* t, size_type i = 0 ) const
   {
      return FindFirstIC( t, i );
   }

   size_type FindIC( wchar_t c, size_type i = 0 ) const
   {
      return FindFirstIC( c, i );
   }

   size_type FindIC( const_c_string8 t, size_type i = 0 ) const
   {
      return FindFirstIC( t, i );
   }

   size_type FindIC( char8_type c, size_type i = 0 ) const
   {
      return FindFirstIC( c, i );
   }

   // -------------------------------------------------------------------------

   size_type FindLast( const String& s, size_type r = maxPos ) const
   {
      return string_base::FindLast( s, r );
   }

   size_type FindLast( const_iterator t, size_type r = maxPos ) const
   {
      return string_base::FindLast( t, r );
   }

   size_type FindLast( char_type c, size_type r = maxPos ) const
   {
      return string_base::FindLast( c, r );
   }

   size_type FindLastIC( const String& s, size_type r = maxPos ) const
   {
      return string_base::FindLastIC( s, r );
   }

   size_type FindLastIC( const_iterator t, size_type r = maxPos ) const
   {
      return string_base::FindLastIC( t, r );
   }

   size_type FindLastIC( char_type c, size_type r = maxPos ) const
   {
      return string_base::FindLastIC( c, r );
   }

   size_type FindLast( const wchar_t* t, size_type r = maxPos ) const
   {
#ifdef __PCL_WINDOWS
      return string_base::FindLast( reinterpret_cast<const_iterator>( t ), r );
#else
      return string_base::FindLast( String( t ), r );
#endif
   }

   size_type FindLast( wchar_t c, size_type r = maxPos ) const
   {
      return string_base::FindLast( char_type( c ), r );
   }

   size_type FindLastIC( const wchar_t* t, size_type r = maxPos ) const
   {
#ifdef __PCL_WINDOWS
      return string_base::FindLastIC( reinterpret_cast<const_iterator>( t ), r );
#else
      return string_base::FindLastIC( String( t ), r );
#endif
   }

   size_type FindLastIC( wchar_t c, size_type r = maxPos ) const
   {
      return string_base::FindLastIC( char_type( c ), r );
   }

   size_type FindLast( const_c_string8 t, size_type r = maxPos ) const
   {
      return string_base::FindLast( String( t ), r );
   }

   size_type FindLast( char8_type c, size_type r = maxPos ) const
   {
      return string_base::FindLast( char_type( c ), r );
   }

   size_type FindLastIC( const_c_string8 t, size_type r = maxPos ) const
   {
      return string_base::FindLastIC( String( t ), r );
   }

   size_type FindLastIC( char8_type c, size_type r = maxPos ) const
   {
      return string_base::FindLastIC( char_type( c ), r );
   }

   // -------------------------------------------------------------------------

   bool Contains( const String& s ) const
   {
      return string_base::Contains( s );
   }

   bool Contains( const_iterator t ) const
   {
      return string_base::Contains( t );
   }

   bool Contains( char_type c ) const
   {
      return string_base::Contains( c );
   }

   bool ContainsIC( const String& s ) const
   {
      return string_base::ContainsIC( s );
   }

   bool ContainsIC( const_iterator t ) const
   {
      return string_base::ContainsIC( t );
   }

   bool ContainsIC( char_type c ) const
   {
      return string_base::ContainsIC( c );
   }

   bool Contains( const wchar_t* t ) const
   {
#ifdef __PCL_WINDOWS
      return string_base::Contains( reinterpret_cast<const_iterator>( t ) );
#else
      return string_base::Contains( String( t ) );
#endif
   }

   bool Contains( wchar_t c ) const
   {
      return string_base::Contains( char_type( c ) );
   }

   bool ContainsIC( const wchar_t* t ) const
   {
#ifdef __PCL_WINDOWS
      return string_base::ContainsIC( reinterpret_cast<const_iterator>( t ) );
#else
      return string_base::ContainsIC( String( t ) );
#endif
   }

   bool ContainsIC( wchar_t c ) const
   {
      return string_base::ContainsIC( char_type( c ) );
   }

   bool Contains( const_c_string8 t ) const
   {
      return string_base::Contains( String( t ) );
   }

   bool Contains( char8_type c ) const
   {
      return string_base::Contains( char_type( c ) );
   }

   bool ContainsIC( const_c_string8 t ) const
   {
      return string_base::ContainsIC( String( t ) );
   }

   bool ContainsIC( char8_type c ) const
   {
      return string_base::ContainsIC( char_type( c ) );
   }

   // -------------------------------------------------------------------------

   int CompareCodePoints( const String& s, bool caseSensitive = true ) const
   {
      return string_base::CompareCodePoints( s, caseSensitive );
   }

   int CompareCodePoints( const_iterator t, bool caseSensitive = true ) const
   {
      return string_base::CompareCodePoints( t, caseSensitive );
   }

   int CompareCodePoints( char_type c, bool caseSensitive = true ) const
   {
      return string_base::CompareCodePoints( c, caseSensitive );
   }

   int CompareCodePoints( const wchar_t* t, bool caseSensitive = true ) const
   {
#ifdef __PCL_WINDOWS
      return string_base::CompareCodePoints( reinterpret_cast<const_iterator>( t ), caseSensitive );
#else
      return string_base::CompareCodePoints( String( t ), caseSensitive );
#endif
   }

   int CompareCodePoints( wchar_t c, bool caseSensitive = true ) const
   {
      return string_base::CompareCodePoints( char_type( c ), caseSensitive );
   }

   int CompareCodePoints( const_c_string8 t, bool caseSensitive = true ) const
   {
      return string_base::CompareCodePoints( String( t ), caseSensitive );
   }

   int CompareCodePoints( char8_type c, bool caseSensitive = true ) const
   {
      return string_base::CompareCodePoints( char_type( c ), caseSensitive );
   }

   // -------------------------------------------------------------------------

   int Compare( const String& s, bool caseSensitive = true, bool localeAware = true ) const
   {
      return string_base::Compare( s, caseSensitive, localeAware );
   }

   int Compare( const_iterator t, bool caseSensitive = true, bool localeAware = true ) const
   {
      return string_base::Compare( t, caseSensitive, localeAware );
   }

   int Compare( char_type c, bool caseSensitive = true, bool localeAware = true ) const
   {
      return string_base::Compare( c, caseSensitive, localeAware );
   }

   int CompareIC( const String& s, bool localeAware = true ) const
   {
      return string_base::CompareIC( s, localeAware );
   }

   int CompareIC( const_iterator t, bool localeAware = true ) const
   {
      return string_base::CompareIC( t, localeAware );
   }

   int CompareIC( char_type c, bool localeAware = true ) const
   {
      return string_base::CompareIC( c, localeAware );
   }

   int Compare( const wchar_t* t, bool caseSensitive = true, bool localeAware = true ) const
   {
#ifdef __PCL_WINDOWS
      return string_base::Compare( reinterpret_cast<const_iterator>( t ), caseSensitive, localeAware );
#else
      return string_base::Compare( String( t ), caseSensitive, localeAware );
#endif
   }

   int Compare( wchar_t c, bool caseSensitive = true, bool localeAware = true ) const
   {
      return string_base::Compare( char_type( c ), caseSensitive, localeAware );
   }

   int CompareIC( const wchar_t* t, bool localeAware = true ) const
   {
#ifdef __PCL_WINDOWS
      return string_base::CompareIC( reinterpret_cast<const_iterator>( t ), localeAware );
#else
      return string_base::CompareIC( String( t ), localeAware );
#endif
   }

   int CompareIC( wchar_t c, bool localeAware = true ) const
   {
      return string_base::CompareIC( char_type( c ), localeAware );
   }

   int Compare( const_c_string8 t, bool caseSensitive = true, bool localeAware = true ) const
   {
      return string_base::Compare( String( t ), caseSensitive, localeAware );
   }

   int Compare( char8_type c, bool caseSensitive = true, bool localeAware = true ) const
   {
      return string_base::Compare( char_type( c ), caseSensitive, localeAware );
   }

   int CompareIC( const_c_string8 t, bool localeAware = true ) const
   {
      return string_base::CompareIC( String( t ), localeAware );
   }

   int CompareIC( char8_type c, bool localeAware = true ) const
   {
      return string_base::CompareIC( char_type( c ), localeAware );
   }

   // -------------------------------------------------------------------------

   bool WildMatch( const String& pattern, bool caseSensitive = true ) const
   {
      return string_base::WildMatch( pattern, caseSensitive );
   }

   bool WildMatchIC( const String& pattern ) const
   {
      return string_base::WildMatchIC( pattern );
   }

   bool WildMatch( const_iterator pattern, bool caseSensitive = true ) const
   {
      return string_base::WildMatch( pattern, caseSensitive );
   }

   bool WildMatchIC( const_iterator pattern ) const
   {
      return string_base::WildMatchIC( pattern );
   }

   bool WildMatch( const string8_base& pattern, bool caseSensitive = true ) const
   {
      return char_traits::WildMatch( m_data->string, Length(), pattern.Begin(), pattern.Length(), caseSensitive );
   }

   bool WildMatchIC( const string8_base& pattern ) const
   {
      return char_traits::WildMatch( m_data->string, Length(), pattern.Begin(), pattern.Length(), false/*caseSensitive*/ );
   }

   bool WildMatch( const_c_string8 pattern, bool caseSensitive = true ) const
   {
      return char_traits::WildMatch( m_data->string, Length(), pattern, char8_traits::Length( pattern ), caseSensitive );
   }

   bool WildMatchIC( const_c_string8 pattern ) const
   {
      return char_traits::WildMatch( m_data->string, Length(), pattern, char8_traits::Length( pattern ), false/*caseSensitive*/ );
   }

   // -------------------------------------------------------------------------

   String SetToLength( size_type n ) const
   {
      return string_base::SetToLength( n );
   }

   String ResizedToNullTerminated() const
   {
      return string_base::ResizedToNullTerminated();
   }

   String Squeezed() const
   {
      return string_base::Squeezed();
   }

   // -------------------------------------------------------------------------

   String Substring( size_type i, size_type n = maxPos ) const
   {
      return string_base::Substring( i, n );
   }

   String Left( size_type n ) const
   {
      return string_base::Left( n );
   }

   String Right( size_type n ) const
   {
      return string_base::Right( n );
   }

   String Suffix( size_type i ) const
   {
      return string_base::Suffix( i );
   }

   String Prefix( size_type i ) const
   {
      return string_base::Prefix( i );
   }

   // -------------------------------------------------------------------------

   template <class C>
   size_type Break( C& list, const String& s, bool trim = false, size_type i = 0 ) const
   {
      return string_base::Break( list, s, trim, i );
   }

   template <class C>
   size_type Break( C& list, const string8_base& s, bool trim = false, size_type i = 0 ) const
   {
      return string_base::Break( list, String( s ), trim, i );
   }

   template <class C>
   size_type Break( C& list, const_c_string8 s, bool trim = false, size_type i = 0 ) const
   {
      return string_base::Break( list, String( s ), trim, i );
   }

   template <class C>
   size_type Break( C& list, char8_type s, bool trim = false, size_type i = 0 ) const
   {
      return string_base::Break( list, char_type( s ), trim, i );
   }

   // -------------------------------------------------------------------------

   String Trimmed() const
   {
      return string_base::Trimmed();
   }

   String TrimmedLeft() const
   {
      return string_base::TrimmedLeft();
   }

   String TrimmedRight() const
   {
      return string_base::TrimmedRight();
   }

   // -------------------------------------------------------------------------

   String LeftJustified( size_type width, char_type fill = CharTraits::Blank() ) const
   {
      return string_base::LeftJustified( width, fill );
   }

   String RightJustified( size_type width, char_type fill = CharTraits::Blank() ) const
   {
      return string_base::RightJustified( width, fill );
   }

   String CenterJustified( size_type width, char_type fill = CharTraits::Blank() ) const
   {
      return string_base::CenterJustified( width, fill );
   }

   // -------------------------------------------------------------------------

   String CaseFolded() const
   {
      return string_base::CaseFolded();
   }

   String Lowercase() const
   {
      return string_base::Lowercase();
   }

   String Uppercase() const
   {
      return string_base::Uppercase();
   }

   // -------------------------------------------------------------------------

   String Reversed() const
   {
      return string_base::Reversed();
   }

   String Sorted() const
   {
      return string_base::Sorted();
   }

   template <class BP>
   String Sorted( BP p ) const
   {
      return string_base::Sorted( p );
   }

   // -------------------------------------------------------------------------

   template <class C>
   String& ToSeparated( const C& c, char_type separator )
   {
      (void)string_base::ToSeparated( c, separator );
      return *this;
   }

   template <class C, class AF>
   String& ToSeparated( const C& c, char_type separator, AF append )
   {
      (void)string_base::ToSeparated( c, separator, append );
      return *this;
   }

   template <class C>
   String& ToSeparated( const C& c, const String& separator )
   {
      (void)string_base::ToSeparated( c, separator );
      return *this;
   }

   template <class C, class AF>
   String& ToSeparated( const C& c, const String& separator, AF append )
   {
      (void)string_base::ToSeparated( c, separator, append );
      return *this;
   }

   template <class C>
   String& ToSeparated( const C& c, char8_type separator )
   {
      (void)string_base::ToSeparated( c, separator );
      return *this;
   }

   template <class C, class AF>
   String& ToSeparated( const C& c, char8_type separator, AF append )
   {
      (void)string_base::ToSeparated( c, separator, append );
      return *this;
   }

   template <class C>
   String& ToSeparated( const C& c, const string8_base& separator )
   {
      (void)string_base::ToSeparated( c, separator );
      return *this;
   }

   template <class C, class AF>
   String& ToSeparated( const C& c, const string8_base& separator, AF append )
   {
      (void)string_base::ToSeparated( c, separator, append );
      return *this;
   }

   template <class C>
   String& ToSeparated( const C& c, const_c_string8 separator )
   {
      (void)string_base::ToSeparated( c, separator );
      return *this;
   }

   template <class C, class AF>
   String& ToSeparated( const C& c, const_c_string8 separator, AF append )
   {
      (void)string_base::ToSeparated( c, separator, append );
      return *this;
   }

   template <class C>
   String& ToCommaSeparated( const C& c )
   {
      (void)string_base::ToCommaSeparated( c );
      return *this;
   }

   template <class C>
   String& ToSpaceSeparated( const C& c )
   {
      (void)string_base::ToSpaceSeparated( c );
      return *this;
   }

   template <class C>
   String& ToTabSeparated( const C& c )
   {
      (void)string_base::ToTabSeparated( c );
      return *this;
   }

   template <class C>
   String& ToNewLineSeparated( const C& c )
   {
      (void)string_base::ToNewLineSeparated( c );
      return *this;
   }

   template <class C>
   String& ToNullSeparated( const C& c )
   {
      (void)string_base::ToNullSeparated( c );
      return *this;
   }

   template <class C>
   String& ToHyphenated( const C& c )
   {
      (void)string_base::ToHyphenated( c );
      return *this;
   }

   // -------------------------------------------------------------------------

   /*!
    * Replaces all occurrences of HTML special characters in this string with
    * valid HTML entities. Returns a reference to this string.
    *
    * The following replacements are performed:
    *
    * '&' (ampersand) becomes "\&amp;" \n
    * '"' (double quote) becomes "\&quot;" \n
    * "'" (single quote) becomes "\&#039;" \n
    * '<' (less than) becomes "\&lt;" \n
    * '>' (greater than) becomes "\&gt;"
    */
   String& ToEncodedHTMLSpecialChars();

   /*!
    * Returns a duplicate of this string with all occurrences of HTML special
    * characters replaced with valid HTML entities.
    *
    * \sa ToEncodedHTMLSpecialChars()
    */
   String EncodedHTMLSpecialChars() const
   {
      return String( *this ).ToEncodedHTMLSpecialChars();
   }

   /*!
    * Replaces all occurrences of special HTML entities in this string with
    * their corresponding plain text character equivalents. Returns a reference
    * to this string.
    *
    * The following replacements are performed:
    *
    * "\&amp;" (ampersand) becomes '&' \n
    * "\&quot;" (double quote) becomes '"' \n
    * "\&#039;" (single quote) becomes "'" \n
    * "\&apos;" (apostrophe) becomes "'" \n
    * "\&lt;" (less than) becomes '<' \n
    * "\&gt;" (greater than) becomes '>'
    */
   String& ToDecodedHTMLSpecialChars();

   /*!
    * Returns a duplicate of this string with all occurrences of special HTML
    * entities replaced with their corresponding plain text character
    * equivalents.
    *
    * \sa ToDecodedHTMLSpecialChars()
    */
   String DecodedHTMLSpecialChars() const
   {
      return String( *this ).ToDecodedHTMLSpecialChars();
   }

   // -------------------------------------------------------------------------

#ifdef __PCL_QT_INTERFACE

   operator QString() const
   {
      return PCL_GET_QSTRING_FROM_CHAR16PTR( c_str() );
   }

   operator QDate() const
   {
      return QDate::fromString( operator QString(), PCL_QDATE_FMT_STR );
   }

   operator QDateTime() const
   {
      return QDateTime::fromString( operator QString(), PCL_QDATETIME_FMT_STR );
   }

#endif

   /*!
    * Replaces the contents of this string with a formatted representation of a
    * variable-length set of values. Returns a reference to this string.
    *
    * The \a fmt null-terminated string is a standard <em>printf format
    * string</em>. It follows the same rules as its counterpart parameter in
    * the standard printf( const char* fmt, ... ) C runtime function.
    *
    * The required space to store the resulting formatted output is calculated
    * and allocated transparently.
    */
   String& Format( const_c_string8 fmt, ... )
   {
      va_list paramList;
      va_start( paramList, fmt );

      (void)VFormat( fmt, paramList );

      va_end( paramList );
      return *this;
   }

   /*!
    * Appends a formatted representation of a variable-length set of values to
    * the current contents of this string. Returns a reference to this string.
    *
    * The \a fmt null-terminated string is a standard <em>printf format
    * string</em>. It follows the same rules as its counterpart parameter in
    * the standard printf( const char* fmt, ... ) C runtime function.
    *
    * The required space to store the resulting formatted output is calculated
    * and allocated transparently.
    */
   String& AppendFormat( const_c_string8 fmt, ... )
   {
      va_list paramList;
      va_start( paramList, fmt );

      (void)AppendVFormat( fmt, paramList );

      va_end( paramList );
      return *this;
   }

   /*!
    * Replaces the contents of this string with a formatted representation of a
    * variable-length set of values, specified as a \c va_list standard
    * parameter list. Returns the number of characters generated.
    *
    * The \a fmt null-terminated string is a standard <em>printf format
    * string</em>. It follows the same rules as its counterpart parameter in
    * the standard printf( const char* fmt, ... ) C runtime function.
    *
    * The required space to store the resulting formatted output is calculated
    * and allocated transparently.
    */
   int VFormat( const_c_string8 fmt, va_list paramList )
   {
      IsoString s;
      int count = s.VFormat( fmt, paramList );
      Assign( s );
      return count;
   }

   /*!
    * Appends a formatted representation of a variable-length set of values,
    * specified as a \c va_list standard parameter list, to the current
    * contents of this string. Returns the number of characters appended.
    *
    * The \a fmt null-terminated string is a standard <em>printf format
    * string</em>. It follows the same rules as its counterpart parameter in
    * the standard printf( const char* fmt, ... ) C runtime function.
    *
    * The required space to store the resulting formatted output is calculated
    * and allocated transparently.
    */
   int AppendVFormat( const_c_string8 fmt, va_list paramList )
   {
      IsoString s;
      int count = s.VFormat( fmt, paramList );
      Append( s );
      return count;
   }

   /*!
    * Replaces the contents of this string with a formatted representation of a
    * variable-length set of values. Returns a reference to this string.
    *
    * The \a fmt null-terminated string is a standard <em>printf format
    * string</em> of \c wchar_t characters. It follows the same rules as its
    * counterpart parameter in the standard wprintf( const wchar_t* fmt, ... )
    * C runtime function.
    *
    * The required space to store the resulting formatted output is calculated
    * and allocated transparently.
    */
   String& Format( const wchar_t* fmt, ... )
   {
      va_list paramList;
      va_start( paramList, fmt );

      (void)VFormat( fmt, paramList );

      va_end( paramList );
      return *this;
   }

   /*!
    * Appends a formatted representation of a variable-length set of values to
    * the current contents of this string. Returns a reference to this string.
    *
    * The \a fmt null-terminated string is a standard <em>printf format
    * string</em> of \c wchar_t characters. It follows the same rules as its
    * counterpart parameter in the standard wprintf( const wchar_t* fmt, ... )
    * C runtime function.
    *
    * The required space to store the resulting formatted output is calculated
    * and allocated transparently.
    */
   String& AppendFormat( const wchar_t* fmt, ... )
   {
      va_list paramList;
      va_start( paramList, fmt );

      (void)AppendVFormat( fmt, paramList );

      va_end( paramList );
      return *this;
   }

   /*!
    * Replaces the contents of this string with a formatted representation of a
    * variable-length set of values, specified as a \c va_list standard
    * parameter list.  Returns the number of characters generated.
    *
    * The \a fmt null-terminated string is a standard <em>printf format
    * string</em> of \c wchar_t characters. It follows the same rules as its
    * counterpart parameter in the standard wprintf( const wchar_t* fmt, ... )
    * C runtime function.
    *
    * The required space to store the resulting formatted output is calculated
    * and allocated transparently.
    */
   int VFormat( const wchar_t* fmt, va_list paramList );

   /*!
    * Appends a formatted representation of a variable-length set of values,
    * specified as a \c va_list standard parameter list, to the current
    * contents of this string.  Returns the number of characters appended.
    *
    * The \a fmt null-terminated string is a standard <em>printf format
    * string</em> of \c wchar_t characters. It follows the same rules as its
    * counterpart parameter in the standard wprintf( const wchar_t* fmt, ... )
    * C runtime function.
    *
    * The required space to store the resulting formatted output is calculated
    * and allocated transparently.
    */
   int AppendVFormat( const wchar_t* fmt, va_list paramList );

   // -------------------------------------------------------------------------

   /*!
    * Conversion of an UTF-8 substring to an UTF-16 string.
    *
    * Converts a contiguous sequence of \a n characters starting at the \a i-th
    * position of the specified null-terminated UTF-8 \a string. Returns the
    * resulting UTF-16 string.
    *
    * \sa UTF16ToUTF8(), UTF16ToUTF32(), UTF32ToUTF16()
    */
   static String UTF8ToUTF16( const_c_string8 string, size_type i = 0, size_type n = maxPos );

   /*!
    * Conversion of an UTF-16 substring to an UTF-8 string.
    *
    * Converts a contiguous sequence of \a n characters starting at the \a i-th
    * position of the specified null-terminated UTF-16 \a string. Returns the
    * resulting UTF-8 string.
    *
    * \sa UTF8ToUTF16(), UTF16ToUTF32(), UTF32ToUTF16()
    */
   static IsoString UTF16ToUTF8( const_c_string string, size_type i = 0, size_type n = maxPos );

   /*!
    * Conversion of an UTF-16 substring to an UTF-32 string.
    *
    * Converts a contiguous sequence of \a n characters starting at the \a i-th
    * position of the specified null-terminated UTF-16 \a string. Returns the
    * resulting UTF-32 string as a dynamic array.
    *
    * \sa UTF8ToUTF16(), UTF16ToUTF8(), UTF32ToUTF16()
    */
   static Array<uint32> UTF16ToUTF32( const_c_string string, size_type i = 0, size_type n = maxPos );

   /*!
    * Conversion of an UTF-32 substring to an UTF-16 string.
    *
    * Converts a contiguous sequence of \a n characters starting at the \a i-th
    * position of the specified null-terminated UTF-32 \a string. Returns the
    * resulting UTF-16 string.
    *
    * \sa UTF8ToUTF16(), UTF16ToUTF8(), UTF16ToUTF32()
    */
   static String UTF32ToUTF16( const uint32* string, size_type i = 0, size_type n = maxPos );

   // -------------------------------------------------------------------------

   /*!
    * Returns an 8-bit, ISO/IEC-8859-1 string with a converted copy of this
    * %String object. Characters in this string that cannot be converted to
    * ISO-8859-1 (that is, characters with code points greater than 0x100) will
    * have undefined values in the returned string. Undefined values are
    * represented with question mark characters (?).
    *
    * \sa ToUTF8(), ToMBS(), ToUTF32(), To7BitASCII()
    */
   IsoString ToIsoString() const;

   /*!
    * Returns a 7-bit ASCII string with a converted copy of this %String
    * object. Characters in this string that cannot be converted to the 7-bit
    * ASCII set (that is, characters with code points greater than 0x80) will
    * have undefined values in the returned string. Undefined values are
    * represented with question mark characters (?).
    *
    * \sa ToIsoString(), ToUTF8(), ToMBS(), ToUTF32()
    */
   IsoString To7BitASCII() const;

   /*!
    * Returns an 8-bit string with an UTF-8 representation of a subset of \a n
    * contiguous UTF-16 characters from this %String object, starting at the
    * \a i-th character.
    *
    * \sa ToMBS(), ToLocal8Bit(), ToIsoString(), ToUTF32()
    */
   IsoString ToUTF8( size_type i = 0, size_type n = maxPos ) const
   {
      return UTF16ToUTF8( Begin(), i, n );
   }

   /*!
    * Returns a copy of this Unicode string converted to a <em>multibyte
    * string</em>. This conversion is dependent on the current locale.
    *
    * In the event of conversion error (if there are invalid wide characters in
    * the source string) this routine returns an empty string.
    *
    * This member function is a convenience wrapper for the wcstombs() routine
    * of the standard C runtime library. Note that on platforms where the size
    * of wchar_t is four bytes (e.g. Linux) this routine performs an additional
    * conversion from UTF-16 to UTF-32. On platforms where the size of wchar_t
    * is two bytes (e.g. Windows), the conversion is direct.
    *
    * \sa ToUTF8(), ToLocal8Bit(), ToIsoString(), ToUTF32();
    */
   IsoString ToMBS() const;

   /*!
    * Returns a copy of this Unicode string converted to an 8-bit
    * locale-dependent string. On UNIX/Linux systems (FreeBSD, Linux, Mac OS X)
    * this function is equivalent to ToUTF8(). On Windows, this function
    * returns ToMBS().
    *
    * \sa ToUTF8(), ToMBS();
    */
   IsoString ToLocal8Bit() const
   {
#ifdef __PCL_WINDOWS
      return ToMBS();
#else
      return ToUTF8();
#endif
   }

   /*!
    * Returns a copy of a contiguous segment of \a n characters of this string,
    * starting from its \a i-th character, as a dynamic array of \c wchar_t. A
    * null terminating character (L'\\0' specifically) is always appended to
    * the resulting array.
    *
    * Depending on the platform, the \c wchar_t type may be 16-bit wide
    * (Windows) or 32-bit wide (UNIX/Linux). The char16_type used by %String is
    * always a 16-bit character (UTF-16) on all supported platforms.
    *
    * This member function provides a platform-independent way to obtain the
    * contents of a %String object as a standard null-terminated string of
    * \c wchar_t characters.
    *
    * On platforms where wchar_t occupies four bytes (UNIX/Linux), this
    * function assumes that this %String object contains no surrogates. For a
    * generalized conversion from UTF-16 to UTF-32, see ToUTF32().
    *
    * \sa ToUTF32(), ToUTF8(), ToMBS(), ToIsoString()
    */
   Array<wchar_t> ToWCharArray( size_type i = 0, size_type n = maxPos ) const
   {
      if ( n > 0 )
      {
         size_type len = Length();
         if ( i < len )
         {
            n = pcl::Min( n, len-i );
            Array<wchar_t> a( n+1, wchar_t( 0 ) );
#ifdef __PCL_WINDOWS
            char_traits::Copy( reinterpret_cast<iterator>( a.Begin() ), m_data->string+i, n );
#else
            Array<wchar_t>::iterator w = a.Begin();
            for ( const_iterator s = m_data->string+i, e = s+n; s < e; ++w, ++s )
               *w = wchar_t( *s );
#endif // __PCL_WINDOWS
            return a;
         }
      }

      return Array<wchar_t>( size_type( 1 ), wchar_t( 0 ) );
   }

   /*!
    * Returns a dynamic array of 32-bit integers with an UTF-32 representation
    * of a subset of \a n contiguous UTF-16 characters from this %String
    * object, starting at the \a i-th character. A null terminating character
    * (uint32( 0 ) specifically) is always appended to the resulting array.
    *
    * \sa ToWCharArray(), ToUTF8(), ToMBS(), ToIsoString()
    */
   Array<uint32> ToUTF32( size_type i = 0, size_type n = maxPos ) const
   {
      return UTF16ToUTF32( Begin(), i, n );
   }

#ifdef __PCL_QT_INTERFACE

   QString ToQString() const
   {
      return operator QString();
   }

   QDate ToQDate() const
   {
      return operator QDate();
   }

   QDateTime ToQDateTime() const
   {
      return operator QDateTime();
   }

#endif

   /*!
    * Evaluates this string as a Boolean literal, and returns the result as a
    * \c bool value.
    *
    * Returns \c true if this string is equal to "1", "true", "TRUE" or "T".
    * Returns \c false if this string is equal to "0", "false", "FALSE" or "F".
    * Otherwise this function throws a ParseError exception.
    *
    * \sa TryToBool()
    */
   bool ToBool() const;

   /*!
    * Attempts to evaluate this string as a Boolean literal.
    *
    * If this string can legally be converted to a Boolean value, this function
    * returns \c true and stores the evaluation result in the \a value
    * variable. A string can only be converted to Boolean type if it is equal
    * to either "1", "true", "TRUE", "T", "0", "false", "FALSE" or "F".
    *
    * If this string cannot be converted to a Boolean value, this function
    * returns \c false and does not change the \a value variable. This function
    * does not throw any exception.
    *
    * \sa ToBool()
    */
   bool TryToBool( bool& value ) const;

   /*!
    * Evaluates this string as a floating point numeric literal, and returns
    * the result as a \c float value.
    *
    * For information about the legal syntax of a floating point literal, see
    * the documentation for the ToDouble() member function.
    *
    * If this string doesn't contain a valid floating point literal, or if the
    * range of \c float is exceeded, this member function throws a ParseError
    * exception.
    *
    * \sa TryToFloat()
    */
   float ToFloat() const;

   /*!
    * Attempts to evaluate this string as a floating point numeric literal.
    *
    * If this string can legally be converted to a floating point number, this
    * function returns \c true and stores the evaluation result in the \a value
    * variable. For information about the legal syntax of a floating point
    * literal, see the documentation for the ToDouble() member function.
    *
    * If this string cannot be converted to a floating point number, this
    * function returns \c false and does not change the \a value variable. This
    * function does not throw any exception.
    *
    * \sa ToFloat()
    */
   bool TryToFloat( float& value ) const;

   /*!
    * Evaluates this string as a floating point literal, and returns the result
    * as a \c double value.
    *
    * The source string is expected to have the following format (informal
    * format specification):
    *
    * [+|-][\<integer-part\>][.[\<decimal-part\>]][\<exp\>[+|-]\<exponent\>]
    *
    * where \<integer-part\> and \<decimal-part\> are optional sequences of
    * decimal digits from 0 to 9, \<exp\> is an <em>exponent specifier</em>
    * (the letter 'e' (or 'E')), and \<exponent\> is a sequence of decimal
    * digits specifying a power of ten that multiplies the preceding numeric
    * constant. At least a one-digit integer part, or a one-digit decimal part,
    * is mandatory. FORTRAN exponent specifiers ('d' and 'f' (or 'D' and 'F'))
    * are also recognized by this implementation.
    *
    * If this string doesn't contain a valid floating point literal, or if the
    * range of \c double is exceeded, this member function throws a ParseError
    * exception.
    *
    * \sa TryToDouble()
    */
   double ToDouble() const;

   /*!
    * Attempts to evaluate this string as a floating point numeric literal.
    *
    * If this string can legally be converted to a floating point number, this
    * function returns \c true and stores the evaluation result in the \a value
    * variable. For information about the legal syntax of a floating point
    * literal, see the documentation for the ToDouble() member function.
    *
    * If this string cannot be converted to a floating point number, this
    * function returns \c false and does not change the \a value variable. This
    * function does not throw any exception.
    *
    * \sa ToDouble()
    */
   bool TryToDouble( double& value ) const;

   /*!
    * Evaluates this string as an integer literal, and returns the result as a
    * \c long value.
    *
    * Calling this member function for a string \c s is equivalent to:
    *
    * \code long n = s.ToInt( 0 ); \endcode
    *
    * See the documentation under ToInt( int ) for information about automatic
    * base conversions when \a base=0 is specified.
    *
    * If this string doesn't contain a valid integer literal, or if the range
    * of \c long is exceeded, this member function throws a ParseError exception.
    *
    * \sa TryToInt()
    */
   long ToInt() const
   {
      return ToInt( 0 );
   }

   /*!
    * Attempts to evaluate this string as an integer numeric literal.
    *
    * If this string can legally be converted to an integer number, this
    * function returns \c true and stores the evaluation result in the \a value
    * variable.
    *
    * Calling this member function for a string \c s is equivalent to:
    *
    * \code
    * int v;
    * if ( s.TryToInt( v, 0 ) ) ...
    * \endcode
    *
    * See the documentation under ToInt( int ) for information about automatic
    * base conversions when \a base=0 is specified.
    *
    * If this string cannot be converted to an integer number, this function
    * returns \c false and does not change the \a value variable. This function
    * does not throw any exception.
    *
    * \sa ToInt()
    */
   bool TryToInt( int& value ) const
   {
      return TryToInt( value, 0 );
   }

   /*!
    * Evaluates this string as an integer literal in the specified \a base, and
    * returns the result as a \c long value.
    *
    * If \a base is 0, the source string is expected to represent either a
    * decimal constant, an octal constant, or a hexadecimal constant, any of
    * which optionally preceded by a sign character (+ or -). A decimal
    * constant begins with a non-zero digit, and consists of a sequence of
    * decimal digits from '0' to '9'. An octal begins with the prefix '0',
    * optionally followed by a sequence of the digits 0 to 7 only. A
    * hexadecimal constant begins with the prefix '0x' or '0X', which must be
    * followed by a sequence of decimal digits and letters from 'a' (or 'A')
    * to 'f' (or 'F'), whose corresponding decimal values are from 10 to 15,
    * respectively.
    *
    * Other legal values of \a base, from 2 to 36, specify the expected base of
    * the integer constant represented by the source string. Decimal digits and
    * letters from 'a' (or 'A') to 'z' (or 'Z') are used to represent all
    * possible digits in the specified base, as necessary.
    *
    * If this string doesn't contain a valid integer literal in the specified
    * \a base, if an illegal \a base is specified, or if the range of \c long
    * is exceeded, this member function throws a ParseError exception.
    *
    * \sa TryToInt( int&, int ) const
    */
   long ToInt( int base ) const;

   /*!
    * Attempts to evaluate this string as an integer numeric literal in the
    * specified \a base.
    *
    * If this string can legally be converted to an integer number in the
    * specified \a base, this function returns \c true and stores the
    * evaluation result in the \a value variable.
    *
    * See the documentation under ToInt( int ) for information about automatic
    * base conversions when \a base=0 is specified.
    *
    * If this string cannot be converted to an integer number in the specified
    * \a base, this function returns \c false and does not change the \a value
    * variable. This function does not throw any exception.
    *
    * \sa ToInt( int ) const
    */
   bool TryToInt( int& value, int base ) const;

   /*!
    * Evaluates this string as an unsigned integer literal, and returns the
    * result as an \c unsigned \c long value.
    *
    * Calling this member function for a string \c s is equivalent to:
    *
    * \code unsigned long n = s.ToUInt( 0 ); \endcode
    *
    * See the documentation under ToInt( int ) for information about automatic
    * base conversions when \a base=0 is specified.
    *
    * If this string doesn't contain a valid unsigned integer literal, or if
    * the range of \c unsigned \c long is exceeded, this member function throws
    * a ParseError exception.
    *
    * \sa TryToUInt()
    */
   unsigned long ToUInt() const
   {
      return ToUInt( 0 );
   }

   /*!
    * Attempts to evaluate this string as an unsigned integer literal.
    *
    * If this string can legally be converted to an unsigned integer number,
    * this function returns \c true and stores the evaluation result in the
    * \a value variable.
    *
    * Calling this member function for a string \c s is equivalent to:
    *
    * \code
    * unsigned v;
    * if ( s.TryToUInt( v, 0 ) ) ...
    * \endcode
    *
    * See the documentation under ToInt( int ) for information about automatic
    * base conversions when \a base=0 is specified.
    *
    * If this string cannot be converted to an integer number, this function
    * returns \c false and does not change the \a value variable. This function
    * does not throw any exception.
    *
    * \sa ToUInt()
    */
   bool TryToUInt( unsigned& value ) const
   {
      return TryToUInt( value, 0 );
   }

   /*!
    * Evaluates this string as an unsigned integer literal in the specified
    * \a base, and returns the result as an \c unsigned \c long value.
    *
    * For information about possible values of \a base and how these are
    * interpreted, see the documentation under ToInt( int ). The only exception
    * is that for this member function, only a + sign is legal preceding the
    * numeric constant represented by the source string.
    *
    * If this string doesn't contain a valid integer literal in the specified
    * \a base, if an illegal \a base is specified, or if the range of
    * \c unsigned \c long is exceeded, this member function throws a ParseError
    * exception.
    *
    * \sa TryToUInt( unsigned&, int ) const
    */
   unsigned long ToUInt( int base ) const;

   /*!
    * Attempts to evaluate this string as an unsigned integer literal in the
    * specified \a base.
    *
    * If this string can legally be converted to an unsigned integer number in
    * the specified \a base, this function returns \c true and stores the
    * evaluation result in the \a value variable.
    *
    * See the documentation under ToUInt( int ) for information about automatic
    * base conversions when \a base=0 is specified.
    *
    * If this string cannot be converted to an unsigned integer number in the
    * specified \a base, this function returns \c false and does not change the
    * \a value variable. This function does not throw any exception.
    *
    * \sa ToUInt( int ) const
    */
   bool TryToUInt( unsigned& value, int base ) const;

   /*!
    * Evaluates this string as an integer literal, and returns the result as a
    * \c long \c long value.
    *
    * Calling this member function for a string \c s is equivalent to:
    *
    * \code long long n = s.ToInt64( 0 ); \endcode
    *
    * If this string doesn't contain a valid integer literal, or if the range
    * of \c long \c long is exceeded, this member function throws a ParseError
    * exception.
    *
    * \sa TryToInt64()
    */
   long long ToInt64() const
   {
      return ToInt64( 0 );
   }

   /*!
    * Attempts to evaluate this string as a 64-bit integer numeric literal in
    * the specified \a base.
    *
    * If this string can legally be converted to a 64-bit integer number in the
    * specified \a base, this function returns \c true and stores the
    * evaluation result in the \a value variable.
    *
    * Calling this member function for a string \c s is equivalent to:
    *
    * \code
    * long long v;
    * if ( s.TryToInt64( v, 0 ) ) ...
    * \endcode
    *
    * See the documentation under ToInt( int ) for information about automatic
    * base conversions when \a base=0 is specified.
    *
    * If this string cannot be converted to a 64-bit integer number, this
    * function returns \c false and does not change the \a value variable. This
    * function does not throw any exception.
    *
    * \sa ToInt64()
    */
   bool TryToInt64( long long& value ) const
   {
      return TryToInt64( value, 0 );
   }

   /*!
    * Evaluates this string as an unsigned integer literal in the specified
    * \a base, and returns the result as a \c long \c long value.
    *
    * For information about possible values of \a base and how these are
    * interpreted, see the documentation under ToInt( int ).
    *
    * If this string doesn't contain a valid integer literal in the specified
    * \a base, if an illegal \a base is specified, or if the range of
    * \c long \c long is exceeded, this member function throws a ParseError
    * exception.
    *
    * \sa TryToInt64( long long&, int ) const
    */
   long long ToInt64( int base ) const;

   /*!
    * Attempts to evaluate this string as a 64-bit integer numeric literal in
    * the specified \a base.
    *
    * If this string can legally be converted to a 64-bit integer number in the
    * specified \a base, this function returns \c true and stores the
    * evaluation result in the \a value variable.
    *
    * For information about possible values of \a base and how these are
    * interpreted, see the documentation under ToInt( int ).
    *
    * If this string cannot be converted to a 64-bit integer number in the
    * specified \a base, this function returns \c false and does not change the
    * \a value variable. This function does not throw any exception.
    *
    * \sa ToInt64( int ) const
    */
   bool TryToInt64( long long& value, int base ) const;

   /*!
    * Evaluates this string as an unsigned integer literal in the specified
    * \a base, and returns the result as an \c unsigned \c long \c long value.
    *
    * Calling this member function for a string \c s is equivalent to:
    *
    * \code unsigned long long n = s.ToUInt64( 0 ); \endcode
    *
    * If this string doesn't contain a valid unsigned integer literal, or if
    * the range of \c unsigned \c long \c long is exceeded, this member
    * function throws a ParseError exception.
    *
    * \sa TryToUInt64()
    */
   unsigned long long ToUInt64() const
   {
      return ToUInt64( 0 );
   }

   /*!
    * Attempts to evaluate this string as a 64-bit unsigned integer numeric
    * literal in the specified \a base.
    *
    * If this string can legally be converted to a 64-bit unsigned integer
    * number in the specified \a base, this function returns \c true and stores
    * the evaluation result in the \a value variable.
    *
    * Calling this member function for a string \c s is equivalent to:
    *
    * \code
    * unsigned long long v;
    * if ( s.TryToUInt64( v, 0 ) ) ...
    * \endcode
    *
    * See the documentation under ToInt( int ) for information about automatic
    * base conversions when \a base=0 is specified.
    *
    * If this string cannot be converted to a 64-bit unsigned integer number,
    * this function returns \c false and does not change the \a value variable.
    * This function does not throw any exception.
    *
    * \sa ToUInt64()
    */
   bool TryToUInt64( unsigned long long& value ) const
   {
      return TryToUInt64( value, 0 );
   }

   /*!
    * Evaluates this string as an unsigned integer literal in the specified
    * \a base, and returns the result as an \c unsigned \c long \c long value.
    *
    * For information about possible values of \a base and how these are
    * interpreted, see the documentation under ToInt( int ).
    *
    * If this string doesn't contain a valid unsigned integer literal in the
    * specified \a base, if an illegal \a base is specified, or if the range of
    * \c unsigned \c long \c long is exceeded, this member function throws a
    * ParseError exception.
    *
    * \sa TryToUInt64( unsigned long long&, int ) const
    */
   unsigned long long ToUInt64( int base ) const;

   /*!
    * Attempts to evaluate this string as an unsigned 64-bit integer numeric
    * literal in the specified \a base.
    *
    * If this string can legally be converted to an unsigned 64-bit integer
    * number in the specified \a base, this function returns \c true and stores
    * the evaluation result in the \a value variable.
    *
    * For information about possible values of \a base and how these are
    * interpreted, see the documentation under ToInt( int ).
    *
    * If this string cannot be converted to an unsigned 64-bit integer number
    * in the specified \a base, this function returns \c false and does not
    * change the \a value variable. This function does not throw any exception.
    *
    * \sa ToUInt64( int ) const
    */
   bool TryToUInt64( unsigned long long& value, int base ) const;

   /*!
    * Evaluates this string as a sexagesimal numeric literal representation,
    * and returns the result as a \c double value.
    *
    * The source string is expected to have the following format:
    *
    * [+|-]\<s1\>[\<sep\>\<s2\>[\<sep\>\<s3\>]][.[\<decimal-part\>]]
    *
    * where \<s1\>, \<s2\>, \<s3\> and \<decimal-part\> are sequences of
    * decimal digits from 0 to 9, and \<sep\> is any sequence of characters
    * specified as the \a separator argument. The default separator is the
    * colon character, ':'.
    *
    * The result is equal to:
    *
    * \<sign\> * (\<s1\> + \<s2\>/60 + \<s3\>/3600)
    *
    * where omitted optional components are taken as zero, and \<sign\> is -1
    * iff a minus sign '-' leads the representation, or +1 otherwise.
    *
    * If this string doesn't contain a valid sexagesimal numeric literal, or if
    * the range of \c double is exceeded, this member function throws a
    * ParseError exception.
    *
    * \sa TrySexagesimalToDouble(), ParseSexagesimal(), TryParseSexagesimal()
    */
   double SexagesimalToDouble( const String& separator = ':' ) const
   {
      int sign, s1, s2; double s3;
      ParseSexagesimal( sign, s1, s2, s3, separator );
      return sign*(s1 + (s2 + s3/60)/60);
   }

   template <class S>
   double SexagesimalToDouble( const S& separator ) const
   {
      return SexagesimalToDouble( String( separator ) );
   }

   /*!
    * Attempts to evaluate this string as a sexagesimal numeric literal with
    * the specified \a separator.
    *
    * If this string can legally be evaluated as a sexagesimal literal and
    * converted to a floating point number, this function stores the evaluation
    * result in the specified \a value variable and returns \c true. For
    * information about the legal syntax of a sexagesimal literal, see the
    * documentation for SexagesimalToDouble().
    *
    * If this string cannot be evaluated as a sexagesimal literal, this
    * function returns \c false and does not change the \a value variable. This
    * function does not throw any exception.
    *
    * \sa SexagesimalToDouble(), ParseSexagesimal(), TryParseSexagesimal()
    */
   bool TrySexagesimalToDouble( double& value, const String& separator = ':' ) const
   {
      int sign, s1, s2; double s3;
      if ( TryParseSexagesimal( sign, s1, s2, s3, separator ) )
      {
         value = sign*(s1 + (s2 + s3/60)/60);
         return true;
      }
      return false;
   }

   template <class S>
   bool TrySexagesimalToDouble( double& value, const S& separator ) const
   {
      return TrySexagesimalToDouble( value, String( separator ) );
   }

   /*!
    * Evaluates this string as a sexagesimal numeric literal representation,
    * using the specified \a separator, and stores the resulting components in
    * the specified \a sign, \a s1, \a s2 and \a s3 variables.
    *
    * The output value of \a sign is either -1 or +1, respectively for negative
    * and positive values.
    *
    * For information about the legal syntax of a sexagesimal literal, see the
    * documentation for SexagesimalToDouble().
    *
    * This function stores \e canonical minutes and seconds components,
    * irrespective of the actual component values represented by this string.
    * The output sexagesimal component values are such that:
    *
    * 0 &le; \a s1 \n
    * 0 &le; \a s2 &lt; 60 \n
    * 0 &le; \a s3 &lt; 60 \n
    *
    * If this string doesn't contain a valid sexagesimal numeric literal, or if
    * the range of \c double is exceeded, this member function throws a
    * ParseError exception.
    *
    * \sa TryParseSexagesimal(), SexagesimalToDouble(), TrySexagesimalToDouble()
    */
   void ParseSexagesimal( int& sign, int& s1, int& s2, double& s3, const String& separator = ':' ) const;

   template <class S>
   void ParseSexagesimal( int& sign, int& s1, int& s2, double& s3, const S& separator ) const
   {
      ParseSexagesimal( sign, s1, s2, s3, String( separator ) );
   }

   /*!
    * Attempts to evaluate this string as a sexagesimal numeric literal
    * representation, using the specified \a separator. If successful, stores
    * the resulting components in the specified \a sign, \a s1, \a s2 and \a s3
    * variables, and returns \c true. For more information on syntax and
    * output values and ranges, see the documentation for ParseSexagesimal().
    *
    * If this string cannot be evaluated as a sexagesimal literal, this
    * function returns \c false and does not change any of the \a sign, \a s1,
    * \a s2 and \a s3 variables. This function does not throw any exception.
    *
    * \sa ParseSexagesimal(), SexagesimalToDouble(), TrySexagesimalToDouble()
    */
   bool TryParseSexagesimal( int& sign, int& s1, int& s2, double& s3, const String& separator = ':' ) const;

   template <class S>
   bool TryParseSexagesimal( int& sign, int& s1, int& s2, double& s3, const S& separator ) const
   {
      return TryParseSexagesimal( sign, s1, s2, s3, String( separator ) );
   }

   /*!
    * Returns a sexagesimal ASCII representation of the specified components
    * \a sign, \a s1, \a s2 and \a s3, generated with the specified conversion
    * \a options.
    *
    * The generated representation will correspond to a negative value iff
    * \a sign < 0, positive otherwise.
    *
    * The generated representation is of the form:
    *
    * [+|-]\<s1\>[\<sep\>\<s2\>[\<sep\>\<s3\>]][.[\<decimal-part\>]]
    *
    * where the second and third sexagesimal components are canonicalized:
    *
    * 0 &le; \<s2\> &lt; 60
    * 0 &le; \<s3\> &lt; 60
    *
    * irrespective of the original \a s2 and \a s3 argument values.
    *
    * \sa SexagesimalToDouble(), SexagesimalConversionOptions
    */
   static String ToSexagesimal( int sign, double s1, double s2, double s3,
                                const SexagesimalConversionOptions& options = SexagesimalConversionOptions() );

   /*!
    * Returns a sexagesimal ASCII representation of the specified decimal value
    * \a d, generated with the specified conversion \a options.
    *
    * Calling this member function is equivalent to:
    *
    * \code ToSexagesimal( (d < 0) ? -1 : +1, Abs( d ), 0, 0, options ); \endcode
    */
   static String ToSexagesimal( double d, const SexagesimalConversionOptions& options = SexagesimalConversionOptions() )
   {
      return ToSexagesimal( (d < 0) ? -1 : +1, Abs( d ), 0, 0, options );
   }

   /*!
    * Evaluates this string as a date and time specification in ISO 8601
    * extended format, and stores the resulting components in the specified
    * variables.
    *
    * \param year    On output, the year of the date.
    *
    * \param month   On output, the month of the date in the range [1,12].
    *
    * \param day     On output, the day of the date in the range [1,31].
    *
    * \param dayf    On output, the day fraction corresponding to the time
    *                specification, in the range [0,1).
    *
    * \param tz      On output, the time zone offset in hours, in the range
    *                [-12,+12].
    *
    * In ISO 8601 extended representations, decimal fractions must be divided
    * from integer parts exclusively by the full stop or dot character ('.',
    * ASCII code point 46(10) = 2E(16)).
    *
    * \sa TryParseISO8601DateTime(), TimePoint::FromString()
    */
   void ParseISO8601DateTime( int& year, int& month, int& day, double& dayf, double& tz ) const;

   /*!
    * Attempts to evaluate this string as a date and time specification in ISO
    * 8601 extended format. If successful, stores the resulting components in
    * the specified \a year, \a month, \a day and \a dayf and \a tz variables,
    * and returns \c true. For more information on syntax and
    * output values and ranges, see the ParseISO8601DateTime().
    *
    * If this string cannot be evaluated as a valid date and time in ISO 8601
    * format, this function returns \c false and does not change any of the
    * passed variables. This function does not throw any exception.
    *
    * \sa ParseISO8601DateTime(), TimePoint::TryFromString()
    */
   bool TryParseISO8601DateTime( int& year, int& month, int& day, double& dayf, double& tz ) const;

   /*!
    * Returns a string representation of a date and time in ISO 8601 extended
    * format.
    *
    * \param year    The year of the date.
    *
    * \param month   The month of the date in the range [1,12].
    *
    * \param day     The day of the date in the range [1,31].
    *
    * \param dayf    The day fraction corresponding to the time specification,
    *                in the range [0,1).
    *
    * \param tz      The time zone offset in hours, in the range [-12,+12]. The
    *                default value is zero, to be interpreted as UTC.
    *
    * \param options Optional settings to control the representation of date
    *                and time in ISO 8601 format.
    *
    * \sa CurrentUTCISO8601DateTime(), CurrentLocalISO8601DateTime(),
    *     ParseISO8601DateTime(), ISO8601ConversionOptions,
    *     TimePoint::ToString()
    */
   static String ToISO8601DateTime( int year, int month, int day, double dayf, double tz = 0,
                                    const ISO8601ConversionOptions& options = ISO8601ConversionOptions() );

   /*!
    * Returns an ASCII representation of the current UTC date and time in ISO
    * 8601 extended format.
    *
    * \param options Optional settings to control the representation of date
    *                and time in ISO 8601 format.
    *
    * \sa CurrentLocalISO8601DateTime(), ToISO8601DateTime(),
    *     ISO8601ConversionOptions, TimePoint::Now()
    */
   static String CurrentUTCISO8601DateTime( const ISO8601ConversionOptions& options = ISO8601ConversionOptions() );

   /*!
    * Returns an ASCII representation of the current local date and time in ISO
    * 8601 extended format.
    *
    * \param options Optional settings to control the representation of date
    *                and time in ISO 8601 format.
    *
    * \sa CurrentUTCISO8601DateTime(), ToISO8601DateTime(),
    *     ISO8601ConversionOptions
    */
   static String CurrentLocalISO8601DateTime( const ISO8601ConversionOptions& options = ISO8601ConversionOptions() );

   /*!
    * Generates a string of \a n random 16-bit code points, with character
    * types and ranges as prescribed by the specified \a options.
    *
    * \sa RandomizationOption
    */
   static String Random( size_type n, RandomizationOptions options = RandomizationOption::Default );

   /*!
    * Generates a universally unique identifier (UUID) in canonical form.
    *
    * The canonical UUID has 36 characters with the following format:
    *
    * xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx
    *
    * where 'x' represents a lowercase hexadecimal digit, '4' is the UUID
    * version indicator (version 4 = truly random UUID), and 'y' is one of '8',
    * '9', 'a', or 'b'.
    */
   static String UUID();
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

inline IsoString::IsoString( const_c_ustring t, size_type p, size_type n ) : string_base()
{
   size_type len = uchar_traits::Length( t );
   if ( p < len )
   {
      m_data->Allocate( n = pcl::Min( n, len-p ) );
      t += p;
      for ( iterator i = m_data->string; i < m_data->end; ++i, ++t )
         *i = char_type( uint8( *t ) );
   }
}

inline IsoString& IsoString::operator =( const_c_ustring t )
{
   size_type len = uchar_traits::Length( t );
   if ( len > 0 )
   {
      MaybeReallocate( len );
      for ( iterator i = m_data->string; i < m_data->end; ++i, ++t )
         *i = char_type( uint8( *t ) );
   }
   else
      Clear();

   return *this;
}

inline IsoString::ustring_base IsoString::UTF8ToUTF16( size_type i, size_type n ) const
{
   return String::UTF8ToUTF16( Begin(), i, n );
}

// ----------------------------------------------------------------------------

/*!
 * \defgroup string_concatenation_ops String Concatenation Operators
 */

/*!
 * Returns a UTF-16 string with the concatenation of two UTF-16 strings \a s1
 * and \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, const String::string_base& s2 )
{
   String s = s1;
   s.Append( s2 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of two UTF-16 strings \a s1
 * (rvalue reference) and \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::string_base&& s1, const String::string_base& s2 )
{
   s1.Append( s2 );
   return String( std::move( s1 ) );
}

/*!
 * Returns a UTF-16 string with the concatenation of two UTF-16 strings \a s1
 * (rvalue reference) and \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String&& s1, const String::string_base& s2 )
{
   s1.Append( s2 );
   return s1;
}

/*!
 * Returns a UTF-16 string with the concatenation of two UTF-16 strings \a s1
 * and \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, String::string_base&& s2 )
{
   s2.Prepend( s1 );
   return String( std::move( s2 ) );
}

/*!
 * Returns a UTF-16 string with the concatenation of two UTF-16 strings \a s1
 * and \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, String&& s2 )
{
   s2.Prepend( s1 );
   return s2;
}

/*!
 * Returns a UTF-16 string with the concatenation of two UTF-16 strings \a s1
 * (rvalue reference) and \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::string_base&& s1, String::string_base&& s2 )
{
   s1.Append( s2 );
   return String( std::move( s1 ) );
}

/*!
 * Returns a UTF-16 string with the concatenation of two UTF-16 strings \a s1
 * (rvalue reference) and \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( String&& s1, String::string_base&& s2 )
{
   s1.Append( s2 );
   return s1;
}

/*!
 * Returns a UTF-16 string with the concatenation of two UTF-16 strings \a s1
 * (rvalue reference) and \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::string_base&& s1, String&& s2 )
{
   s1.Append( s2 );
   return String( std::move( s1 ) );
}

/*!
 * Returns a UTF-16 string with the concatenation of two UTF-16 strings \a s1
 * (rvalue reference) and \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( String&& s1, String&& s2 )
{
   s1.Append( s2 );
   return s1;
}

// ----------------------------------------------------------------------------

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1 and
 * a null-terminated UTF-16 string \a t2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, String::const_iterator t2 )
{
   String s = s1;
   s.Append( t2 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1
 * (rvalue reference) and a null-terminated UTF-16 string \a t2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::string_base&& s1, String::const_iterator t2 )
{
   s1.Append( t2 );
   return String( std::move( s1 ) );
}

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1
 * (rvalue reference) and a null-terminated UTF-16 string \a t2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String&& s1, String::const_iterator t2 )
{
   s1.Append( t2 );
   return s1;
}

/*!
 * Returns a UTF-16 string with the concatenation of a null-terminated UTF-16
 * string \a t1 and a UTF-16 string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::const_iterator t1, const String::string_base& s2 )
{
   String s = s2;
   s.Prepend( t1 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a null-terminated UTF-16
 * string \a t1 and a UTF-16 string \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::const_iterator t1, String::string_base&& s2 )
{
   s2.Prepend( t1 );
   return String( std::move( s2 ) );
}

/*!
 * Returns a UTF-16 string with the concatenation of a null-terminated UTF-16
 * string \a t1 and a UTF-16 string \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::const_iterator t1, String&& s2 )
{
   s2.Prepend( t1 );
   return s2;
}

// ----------------------------------------------------------------------------

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1 and
 * a single UTF-16 character \a c2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, String::char_type c2 )
{
   String s = s1;
   s.Append( c2 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1
 * (rvalue reference) and a single UTF-16 character \a c2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::string_base&& s1, String::char_type c2 )
{
   s1.Append( c2 );
   return String( std::move( s1 ) );
}

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1
 * (rvalue reference) and a single UTF-16 character \a c2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String&& s1, String::char_type c2 )
{
   s1.Append( c2 );
   return s1;
}

/*!
 * Returns a UTF-16 string with the concatenation of a single UTF-16 character
 * \a c1 and a UTF-16 string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::char_type c1, const String::string_base& s2 )
{
   String s = s2;
   s.Prepend( c1 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a single UTF-16 character
 * \a c1 and a UTF-16 string \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::char_type c1, String::string_base&& s2 )
{
   s2.Prepend( c1 );
   return String( std::move( s2 ) );
}

/*!
 * Returns a UTF-16 string with the concatenation of a single UTF-16 character
 * \a c1 and a UTF-16 string \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::char_type c1, String&& s2 )
{
   s2.Prepend( c1 );
   return s2;
}

// ----------------------------------------------------------------------------

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1 and
 * a null-terminated string of \c wchar_t \a t2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, const wchar_t* t2 )
{
   String s = s1;
   s.Append( t2 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1
 * (rvalue reference) and a null-terminated string of \c wchar_t \a t2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::string_base&& s1, const wchar_t* t2 )
{
   String s = std::move( s1 );
   s.Append( t2 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1
 * (rvalue reference) and a null-terminated string of \c wchar_t \a t2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String&& s1, const wchar_t* t2 )
{
   s1.Append( t2 );
   return s1;
}

/*!
 * Returns a UTF-16 string with the concatenation of a null-terminated string
 * of \c wchar_t \a t1 and a UTF-16 string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const wchar_t* t1, const String::string_base& s2 )
{
   String s = s2;
   s.Prepend( t1 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a null-terminated string
 * of \c wchar_t \a t1 and a UTF-16 string \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( const wchar_t* t1, String::string_base&& s2 )
{
   String s = std::move( s2 );
   s.Prepend( t1 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a null-terminated string
 * of \c wchar_t \a t1 and a UTF-16 string \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( const wchar_t* t1, String&& s2 )
{
   s2.Prepend( t1 );
   return s2;
}

// ----------------------------------------------------------------------------

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1 and
 * a single \c wchar_t character \a c2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, wchar_t c2 )
{
   String s = s1;
   s.Append( c2 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1
 * (rvalue reference) and a single \c wchar_t character \a c2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::string_base&& s1, wchar_t c2 )
{
   String s = std::move( s1 );
   s.Append( c2 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1
 * (rvalue reference) and a single \c wchar_t character \a c2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String&& s1, wchar_t c2 )
{
   s1.Append( c2 );
   return s1;
}

/*!
 * Returns a UTF-16 string with the concatenation of a single \c wchar_t
 * character \a c1 and a UTF-16 string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( wchar_t c1, const String::string_base& s2 )
{
   String s = s2;
   s.Prepend( c1 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a single \c wchar_t
 * character \a c1 and a UTF-16 string \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( wchar_t c1, String::string_base&& s2 )
{
   String s = std::move( s2 );
   s.Prepend( c1 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a single \c wchar_t
 * character \a c1 and a UTF-16 string \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( wchar_t c1, String&& s2 )
{
   s2.Prepend( c1 );
   return s2;
}

// ----------------------------------------------------------------------------

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1 and
 * an 8-bit string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, const String::string8_base& s2 )
{
   String s = s1;
   s.Append( s2 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1
 * (rvalue reference) and an 8-bit string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::string_base&& s1, const String::string8_base& s2 )
{
   String s = std::move( s1 );
   s.Append( s2 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1
 * (rvalue reference) and an 8-bit string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String&& s1, const String::string8_base& s2 )
{
   s1.Append( s2 );
   return s1;
}

/*!
 * Returns a UTF-16 string with the concatenation of an 8-bit string \a s1 and
 * a UTF-16 string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string8_base& s1, const String::string_base& s2 )
{
   String s = s2;
   s.Prepend( s1 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of an 8-bit string \a s1 and
 * a UTF-16 string \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string8_base& s1, String::string_base&& s2 )
{
   String s = std::move( s2 );
   s.Prepend( s1 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of an 8-bit string \a s1 and
 * a UTF-16 string \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string8_base& s1, String&& s2 )
{
   s2.Prepend( s1 );
   return s2;
}

// ----------------------------------------------------------------------------

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1 and
 * a null-terminated 8-bit string \a t2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, String::const_c_string8 t2 )
{
   String s = s1;
   s.Append( t2 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1
 * (rvalue reference) and a null-terminated 8-bit string \a t2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::string_base&& s1, String::const_c_string8 t2 )
{
   String s = std::move( s1 );
   s.Append( t2 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1
 * (rvalue reference) and a null-terminated 8-bit string \a t2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String&& s1, String::const_c_string8 t2 )
{
   s1.Append( t2 );
   return s1;
}

/*!
 * Returns a UTF-16 string with the concatenation of a null-terminated 8-bit
 * string \a t1 and a UTF-16 string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::const_c_string8 t1, const String::string_base& s2 )
{
   String s = s2;
   s.Prepend( t1 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a null-terminated 8-bit
 * string \a t1 and a UTF-16 string \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::const_c_string8 t1, String::string_base&& s2 )
{
   String s = std::move( s2 );
   s.Prepend( t1 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a null-terminated 8-bit
 * string \a t1 and a UTF-16 string \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::const_c_string8 t1, String&& s2 )
{
   s2.Prepend( t1 );
   return s2;
}

// ----------------------------------------------------------------------------

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1 and
 * a single 8-bit character \a c2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, String::char8_type c2 )
{
   String s = s1;
   s.Append( c2 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1
 * (rvalue reference) and a single 8-bit character \a c2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::string_base&& s1, String::char8_type c2 )
{
   s1.Append( String::char_type( c2 ) );
   return String( std::move( s1 ) );
}

/*!
 * Returns a UTF-16 string with the concatenation of a UTF-16 string \a s1
 * (rvalue reference) and a single 8-bit character \a c2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String&& s1, String::char8_type c2 )
{
   s1.Append( c2 );
   return s1;
}

/*!
 * Returns a UTF-16 string with the concatenation of a single 8-bit character
 * \a c1 and a UTF-16 string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::char8_type c1, const String::string_base& s2 )
{
   String s = s2;
   s.Prepend( c1 );
   return s;
}

/*!
 * Returns a UTF-16 string with the concatenation of a single 8-bit character
 * \a c1 and a UTF-16 string \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::char8_type c1, String::string_base&& s2 )
{
   s2.Prepend( String::char_type( c1 ) );
   return String( std::move( s2 ) );
}

/*!
 * Returns a UTF-16 string with the concatenation of a single 8-bit character
 * \a c1 and a UTF-16 string \a s2 (rvalue reference).
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::char8_type c1, String&& s2 )
{
   s2.Prepend( c1 );
   return s2;
}

// ----------------------------------------------------------------------------

/*!
 * Appends a UTF-16 string \a s2 to a UTF-16 string \a s1. Returns a reference
 * to the left-hand operand string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String& s1, const String::string_base& s2 )
{
   s1.Append( s2 );
   return s1;
}

/*!
 * Appends a UTF-16 string \a s2 to a UTF-16 string \a s1. Returns a reference
 * to the left-hand operand string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String&& s1, const String::string_base& s2 )
{
   s1.Append( s2 );
   return s1;
}

/*!
 * Appends a null-terminated UTF-16 string \a t2 to a UTF-16 string \a s1.
 * Returns a reference to the left-hand operand string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String& s1, String::const_iterator& t2 )
{
   s1.Append( t2 );
   return s1;
}

/*!
 * Appends a null-terminated UTF-16 string \a t2 to a UTF-16 string \a s1.
 * Returns a reference to the left-hand operand string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String&& s1, String::const_iterator& t2 )
{
   s1.Append( t2 );
   return s1;
}

/*!
 * Appends a single UTF-16 character \a c2 to a UTF-16 string \a s1. Returns a
 * reference to the left-hand operand string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String& s1, String::char_type c2 )
{
   s1.Append( c2 );
   return s1;
}

/*!
 * Appends a single UTF-16 character \a c2 to a UTF-16 string \a s1. Returns a
 * reference to the left-hand operand string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String&& s1, String::char_type c2 )
{
   s1.Append( c2 );
   return s1;
}

/*!
 * Appends a null-terminated string \a t2 of \c wchar_t to a UTF-16 string
 * \a s1. Returns a reference to the left-hand operand string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String& s1, const wchar_t* t2 )
{
   s1.Append( t2 );
   return s1;
}

/*!
 * Appends a null-terminated string \a t2 of \c wchar_t to a UTF-16 string
 * \a s1. Returns a reference to the left-hand operand string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String&& s1, const wchar_t* t2 )
{
   s1.Append( t2 );
   return s1;
}

/*!
 * Appends a single \c wchar_t character \a c2 to a UTF-16 string \a s1.
 * Returns a reference to the left-hand operand string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String& s1, wchar_t c2 )
{
   s1.Append( c2 );
   return s1;
}

/*!
 * Appends a single \c wchar_t character \a c2 to a UTF-16 string \a s1.
 * Returns a reference to the left-hand operand string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String&& s1, wchar_t c2 )
{
   s1.Append( c2 );
   return s1;
}

/*!
 * Appends an 8-bit string \a s2 to a UTF-16 string \a s1. Returns a reference
 * to the left-hand operand Unicode string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String& s1, const String::string8_base& s2 )
{
   s1.Append( s2 );
   return s1;
}

/*!
 * Appends an 8-bit string \a s2 to a UTF-16 string \a s1. Returns a reference
 * to the left-hand operand Unicode string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String&& s1, const String::string8_base& s2 )
{
   s1.Append( s2 );
   return s1;
}

/*!
 * Appends a null-terminated 8-bit string \a t2 to a UTF-16 string \a s1.
 * Returns a reference to the left-hand operand Unicode string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String& s1, String::const_c_string8 t2 )
{
   s1.Append( t2 );
   return s1;
}

/*!
 * Appends a null-terminated 8-bit string \a t2 to a UTF-16 string \a s1.
 * Returns a reference to the left-hand operand Unicode string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String&& s1, String::const_c_string8 t2 )
{
   s1.Append( t2 );
   return s1;
}

/*!
 * Appends a single 8-bit character \a c2 to a UTF-16 string \a s1. Returns a
 * reference to the left-hand operand Unicode string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String& s1, String::char8_type c2 )
{
   s1.Append( c2 );
   return s1;
}

/*!
 * Appends a single 8-bit character \a c2 to a UTF-16 string \a s1. Returns a
 * reference to the left-hand operand Unicode string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String&& s1, String::char8_type c2 )
{
   s1.Append( c2 );
   return s1;
}

// ----------------------------------------------------------------------------

/*!
 * \defgroup string_relational_ops String Relational Operators
 */

/*!
 * Equality operator.
 * \ingroup string_relational_ops
 */
inline bool operator ==( const String& s1, const wchar_t* t2 )
{
   return s1.CompareCodePoints( t2 ) == 0;
}

/*!
 * <em>Less than</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator  <( const String& s1, const wchar_t* t2 )
{
   return s1.CompareCodePoints( t2 ) < 0;
}

/*!
 * <em>Less than or equal</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator <=( const String& s1, const wchar_t* t2 )
{
   return s1.CompareCodePoints( t2 ) <= 0;
}

/*!
 * <em>Greater than</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator  >( const String& s1, const wchar_t* t2 )
{
   return s1.CompareCodePoints( t2 ) > 0;
}

/*!
 * <em>Greater than or equal</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator >=( const String& s1, const wchar_t* t2 )
{
   return s1.CompareCodePoints( t2 ) >= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Equality operator.
 * \ingroup string_relational_ops
 */
inline bool operator ==( const wchar_t* t1, const String& s2 )
{
   return s2.CompareCodePoints( t1 ) == 0;
}

/*!
 * <em>Less than</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator  <( const wchar_t* t1, const String& s2 )
{
   return s2.CompareCodePoints( t1 ) > 0;
}

/*!
 * <em>Less than or equal</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator <=( const wchar_t* t1, const String& s2 )
{
   return s2.CompareCodePoints( t1 ) >= 0;
}

/*!
 * <em>Greater than</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator  >( const wchar_t* t1, const String& s2 )
{
   return s2.CompareCodePoints( t1 ) < 0;
}

/*!
 * <em>Greater than or equal</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator >=( const wchar_t* t1, const String& s2 )
{
   return s2.CompareCodePoints( t1 ) <= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Equality operator.
 * \ingroup string_relational_ops
 */
inline bool operator ==( const String& s1, wchar_t c2 )
{
   return s1.CompareCodePoints( c2 ) == 0;
}

/*!
 * <em>Less than</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator  <( const String& s1, wchar_t c2 )
{
   return s1.CompareCodePoints( c2 ) < 0;
}

/*!
 * <em>Less than or equal</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator <=( const String& s1, wchar_t c2 )
{
   return s1.CompareCodePoints( c2 ) <= 0;
}

/*!
 * <em>Greater than</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator  >( const String& s1, wchar_t c2 )
{
   return s1.CompareCodePoints( c2 ) > 0;
}

/*!
 * <em>Greater than or equal</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator >=( const String& s1, wchar_t c2 )
{
   return s1.CompareCodePoints( c2 ) >= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Equality operator.
 * \ingroup string_relational_ops
 */
inline bool operator ==( wchar_t c1, const String& s2 )
{
   return s2.CompareCodePoints( c1 ) == 0;
}

/*!
 * <em>Less than</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator  <( wchar_t c1, const String& s2 )
{
   return s2.CompareCodePoints( c1 ) > 0;
}

/*!
 * <em>Less than or equal</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator <=( wchar_t c1, const String& s2 )
{
   return s2.CompareCodePoints( c1 ) >= 0;
}

/*!
 * <em>Greater than</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator  >( wchar_t c1, const String& s2 )
{
   return s2.CompareCodePoints( c1 ) < 0;
}

/*!
 * <em>Greater than or equal</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator >=( wchar_t c1, const String& s2 )
{
   return s2.CompareCodePoints( c1 ) <= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Equality operator.
 * \ingroup string_relational_ops
 */
inline bool operator ==( const String& s1, String::const_c_string8 t2 )
{
   return s1.CompareCodePoints( t2 ) == 0;
}

/*!
 * <em>Less than</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator  <( const String& s1, String::const_c_string8 t2 )
{
   return s1.CompareCodePoints( t2 ) < 0;
}

/*!
 * <em>Less than or equal</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator <=( const String& s1, String::const_c_string8 t2 )
{
   return s1.CompareCodePoints( t2 ) <= 0;
}

/*!
 * <em>Greater than</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator  >( const String& s1, String::const_c_string8 t2 )
{
   return s1.CompareCodePoints( t2 ) > 0;
}

/*!
 * <em>Greater than or equal</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator >=( const String& s1, String::const_c_string8 t2 )
{
   return s1.CompareCodePoints( t2 ) >= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Equality operator.
 * \ingroup string_relational_ops
 */
inline bool operator ==( String::const_c_string8 t1, const String& s2 )
{
   return s2.CompareCodePoints( t1 ) == 0;
}

/*!
 * <em>Less than</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator  <( String::const_c_string8 t1, const String& s2 )
{
   return s2.CompareCodePoints( t1 ) > 0;
}

/*!
 * <em>Less than or equal</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator <=( String::const_c_string8 t1, const String& s2 )
{
   return s2.CompareCodePoints( t1 ) >= 0;
}

/*!
 * <em>Greater than</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator  >( String::const_c_string8 t1, const String& s2 )
{
   return s2.CompareCodePoints( t1 ) < 0;
}

/*!
 * <em>Greater than or equal</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator >=( String::const_c_string8 t1, const String& s2 )
{
   return s2.CompareCodePoints( t1 ) <= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Equality operator.
 * \ingroup string_relational_ops
 */
inline bool operator ==( const String& s1, String::char8_type c2 )
{
   return s1.CompareCodePoints( c2 ) == 0;
}

/*!
 * <em>Less than</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator  <( const String& s1, String::char8_type c2 )
{
   return s1.CompareCodePoints( c2 ) < 0;
}

/*!
 * <em>Less than or equal</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator <=( const String& s1, String::char8_type c2 )
{
   return s1.CompareCodePoints( c2 ) <= 0;
}

/*!
 * <em>Greater than</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator  >( const String& s1, String::char8_type c2 )
{
   return s1.CompareCodePoints( c2 ) > 0;
}

/*!
 * <em>Greater than or equal</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator >=( const String& s1, String::char8_type c2 )
{
   return s1.CompareCodePoints( c2 ) >= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Equality operator.
 * \ingroup string_relational_ops
 */
inline bool operator ==( String::char8_type c1, const String& s2 )
{
   return s2.CompareCodePoints( c1 ) == 0;
}

/*!
 * <em>Less than</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator  <( String::char8_type c1, const String& s2 )
{
   return s2.CompareCodePoints( c1 ) > 0;
}

/*!
 * <em>Less than or equal</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator <=( String::char8_type c1, const String& s2 )
{
   return s2.CompareCodePoints( c1 ) >= 0;
}

/*!
 * <em>Greater than</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator  >( String::char8_type c1, const String& s2 )
{
   return s2.CompareCodePoints( c1 ) < 0;
}

/*!
 * <em>Greater than or equal</em> operator. This function performs a
 * character-to-character, locale-unaware comparison of numeric character
 * values. See GenericString<>::CompareCodePoints() for more information.
 * \ingroup string_relational_ops
 */
inline bool operator >=( String::char8_type c1, const String& s2 )
{
   return s2.CompareCodePoints( c1 ) <= 0;
}

// ----------------------------------------------------------------------------

#ifndef __PCL_NO_STRING_OSTREAM

inline std::wostream& operator <<( std::wostream& o, const String& s )
{
#ifdef __PCL_WINDOWS
   return o << reinterpret_cast<const wchar_t*>( s.c_str() );
#else
   Array<wchar_t> w = s.ToWCharArray();
   return o << w.Begin();
#endif
}

inline std::ostream& operator <<( std::ostream& o, const String& s )
{
   return o << IsoString( s );
}

#endif   // __PCL_NO_STRING_OSTREAM

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_String_h

// ----------------------------------------------------------------------------
// EOF pcl/String.h - Released 2017-04-14T23:04:40Z
