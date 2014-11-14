// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/String.h - Released 2014/11/14 17:16:40 UTC
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

#ifndef __PCL_String_h
#define __PCL_String_h

/// \file pcl/String.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_Container_h
#include <pcl/Container.h>
#endif

#ifndef __PCL_Allocator_h
#include <pcl/Allocator.h>
#endif

#ifndef __PCL_StdAlloc_h
#include <pcl/StdAlloc.h>
#endif

#ifndef __PCL_Iterator_h
#include <pcl/Iterator.h>
#endif

#ifndef __PCL_ReferenceCounter_h
#include <pcl/ReferenceCounter.h>
#endif

#ifndef __PCL_Utility_h
#include <pcl/Utility.h>
#endif

#ifndef __PCL_CharTraits_h
#include <pcl/CharTraits.h>
#endif

#ifndef __PCL_ByteArray_h
#include <pcl/ByteArray.h>
#endif

#ifndef __stdarg_h
#include <stdarg.h>
#ifndef __stdarg_h
#define __stdarg_h
#endif
#endif

#ifndef __PCL_NO_STRING_COMPLEX
#  include <pcl/Complex.h>
#endif

#ifndef __PCL_NO_STRING_OSTREAM
#  ifndef _MSC_VER
#    define _GLIBCXX_USE_WCHAR_T  1
#  endif
#  ifndef __ostream_h
#    include <ostream>
#    ifndef __ostream_h
#      define __ostream_h
#    endif
#  endif
#endif

#ifdef __PCL_QT_INTERFACE
#  include <qglobal.h>
#  if ( QT_VERSION >= 0x040000 )
#    include <QtCore/QByteArray>
#    include <QtCore/QDateTime>
#    include <QtCore/QString>
#    define PCL_GET_CHARPTR_FROM_QSTRING( qs )     (qs.toLatin1().data())
#    define PCL_GET_CHAR16PTR_FROM_QSTRING( qs )   ((char16_type*)( qs.utf16() ))
#    define PCL_GET_QSTRING_FROM_CHAR16PTR( s )    (QString::fromUtf16( (const ushort*)(s) ))
#  else
#    include <qbytearray.h>
#    include <qdatetime.h>
#    include <qstring.h>
#    define PCL_GET_CHARPTR_FROM_QSTRING( qs )     (qs.ascii())
#    define PCL_GET_CHAR16PTR_FROM_QSTRING( qs )   ((char16_type*)qs.ucs2())
#    define PCL_GET_QSTRING_FROM_CHAR16PTR( s )    (QString::fromUcs2( (const ushort*)(s) ))
#  endif
#  define PCL_GET_CHARPTR_FROM_QBYTEARRAY( qb )    (qb.data())
#  define PCL_QDATE_FMT_STR                        "yyyy/MM/dd"
#  define PCL_QDATETIME_FMT_STR                    "yyyy/MM/dd hh:mm:ss"
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class GenericString
 * \brief Generic string.
 *
 * %GenericString represents a string of characters of type T, whose
 * fundamental behavior is specified by an instantiation R of GenericCharTraits
 * for a character type T (typically GenericCharTraits, or a derived class such
 * as CharTraits or IsoCharTraits), and where A provides dynamic allocation for
 * contiguous sequences of elements of type T (StandardAllocator by default).
 *
 * In the PixInsight platform, all dynamically allocated strings have been
 * implemented as instantiations of the %GenericString template class.
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
   typedef T
      char_type;

   /*!
    * The character traits class used by this string class.
    */
   typedef R
      char_traits;

   /*!
    * The block allocator used by this string class.
    */
   typedef A
      block_allocator;

   /*!
    * The allocator class used by this string class.
    */
   typedef Allocator<T, A>
      allocator;

   /*!
    * String iterator.
    */
   typedef T*
      iterator;

   /*!
    * Constant string iterator.
    */
   typedef const T*
      const_iterator;

   /*!
    * Reverse string iterator.
    */
   typedef ReverseRandomAccessIterator<iterator, T>
      reverse_iterator;

   /*!
    * Reverse constant string iterator.
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
   GenericString() : data( new Data )
   {
   }

   /*!
    * Copy constructor.
    * Increments the reference counter of the source string \a s.
    */
   GenericString( const GenericString<T,R,A>& s ) : data( s.data )
   {
      if ( data != 0 )
         data->Attach();
   }

   /*!
    * Constructs a string with a copy of the null-terminated character sequence
    * stored in the specified array \a t.
    */
   GenericString( const T* t ) : data( new Data )
   {
      Assign( t );
   }

   /*!
    * Constructs a string with a copy of the character sequence defined by the
    * [i,j[ range.
    */
   GenericString( const T* i, const T* j ) : data( new Data )
   {
      Assign( i, j );
   }

   /*!
    * Constructs a string with a copy of the \a n first characters stored in
    * the \a t array, starting from its \a i-th character.
    */
   GenericString( const T* t, size_type i, size_type n ) : data( new Data )
   {
      Assign( t, i, n );
   }

   /*!
    * Constructs a string with \a n copies of a character \a c.
    */
   GenericString( T c, size_type n = 1 ) : data( new Data )
   {
      Assign( c, n );
   }

   /*!
    * Destroys this string. Decrements the reference counter of the string data
    * and, if it becomes unreferenced, it is destroyed and deallocated.
    */
   virtual ~GenericString()
   {
      if ( data != 0 )
      {
         if ( !data->Detach() )
            delete data;
         data = 0;
      }
   }

   /*!
    * Returns true if this string uniquely references its string data.
    *
    * \sa SetUnique(), IsAliasOf()
    */
   bool IsUnique() const
   {
      return data->IsUnique();
   }

   /*!
    * Returns true if this string is an alias of another string \a s.
    *
    * Two strings are aliases if both share the same string data.
    *
    * \sa IsUnique(), SetUnique()
    */
   bool IsAliasOf( const GenericString<T,R,A>& s ) const
   {
      return data == s.data;
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
   void SetUnique()
   {
      if ( !IsUnique() )
      {
         Data* newData = new Data;

         size_type len = R::Length( data->string );
         if ( len > 0 )
         {
            newData->Allocate( len );
            R::Copy( newData->string, data->string, len+1 );
         }

         if ( !data->Detach() )
            delete data;
         data = newData;
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
    * Returns the length of this string in characters.
    *
    * \sa IsEmpty(), Available(), Capacity(), Size()
    */
   size_type Length() const
   {
      return R::Length( data->string );
   }

   /*!
    * Returns the \e capacity of this string in characters.
    *
    * The capacity of a string is the maximum number of characters that can be
    * stored before requiring a reallocation of the string data.
    *
    * \sa Available(), Length(), Size()
    */
   size_type Capacity() const
   {
      return data->capacity;
   }

   /*!
    * Returns the number of available characters in this string.
    *
    * The number of available characters is equal to Capacity() - Length().
    *
    * \sa Capacity(), Length(), Size()
    */
   size_type Available() const
   {
      return data->capacity - Length();
   }

   /*!
    * Returns true if this string is empty.
    *
    * An empty string stores no characters, and its length is zero.
    *
    * \sa Length(), Available(), Capacity(), Size()
    */
   bool IsEmpty() const
   {
      return data->string == 0 || R::IsNull( *data->string );
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
    * The returned value is equal to Length()-1, and is undefined if this
    * string is empty.
    *
    * \sa LowerBound(), Length()
    */
   size_type UpperBound() const
   {
      PCL_PRECONDITION( !IsEmpty() )
      return Length()-1;
   }

   /*!
    * Returns a constant reference to the allocator object used by this string.
    *
    * \sa SetAllocator()
    */
   const A& GetAllocator() const
   {
      return data->alloc;
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
   void SetAllocator( const A& a )
   {
      SetUnique();
      data->alloc = a;
   }

   /*!
    * Returns an iterator located at the \a i-th character of this string.
    * The character index \a i must be in the range [0,Length()[.
    *
    * If this string is not unique, it is made unique before returning from
    * this member function.
    *
    * If this string is empty, calling this member function is an error that
    * leads to an invalid result.
    *
    * \sa operator []( size_type )
    */
   iterator At( size_type i )
   {
      PCL_PRECONDITION( i < Length() )
      SetUnique();
      return data->string + i;
   }

   /*!
    * Returns a constant iterator located at the \a i-th character in this
    * string. The character index \a i must be in the range [0,Length()[.
    *
    * If this string is empty, calling this member function is an error that
    * leads to an invalid result.
    *
    * \sa operator []( size_type ) const
    */
   const_iterator At( size_type i ) const
   {
      PCL_PRECONDITION( i < Length() )
      return data->string + i;
   }

   /*!
    * Returns a reference to the \a i-th character in this string.
    * The character index \a i must be in the range [0,Length()[.
    *
    * If this string is not unique, it is made unique before returning from
    * this member function.
    *
    * If this string is empty, calling this member function is an error that
    * leads to an invalid result.
    *
    * \sa At(), operator *()
    */
   T& operator []( size_type i )
   {
      return *At( i );
   }

   /*!
    * Returns a copy of the \a i-th character in this string.
    * The character index \a i must be in the range [0,Length()[.
    *
    * If this string is empty, calling this member function is an error that
    * leads to an invalid result.
    *
    * \sa At() const, operator *() const
    */
   T operator []( size_type i ) const
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
   T& operator *()
   {
      return *At( 0 );
   }

   /*!
    * Returns a copy of the first character in this string.
    *
    * This is a convenience member function, equivalent to *At( 0 ).
    *
    * \sa operator []( size_type ) const, At()
    */
   T operator *() const
   {
      return *At( 0 );
   }

   /*!
    * Returns an iterator located at the first character of this string.
    *
    * If this string is not unique, it is made unique before returning from
    * this member function.
    *
    * If this string is empty, this member function returns zero.
    */
   iterator Begin()
   {
      SetUnique();
      return data->string;
   }

   /*!
    * Returns a constant iterator located at the first character of this
    * string.
    *
    * If this string is empty, this member function returns zero.
    */
   const_iterator Begin() const
   {
      return data->string;
   }

   /*!
    * Returns an iterator located at the end of this string. The returned
    * iterator points to the next-to-last character of this string.
    *
    * If this string is not unique, it is made unique before returning from
    * this member function.
    *
    * If this string is empty, this member function returns zero.
    */
   iterator End()
   {
      SetUnique();
      return data->string + R::Length( data->string );
   }

   /*!
    * Returns a constant iterator located at the end of this string. The
    * returned iterator points to the next-to-last character of this string.
    *
    * If this string is empty, this member function returns zero.
    */
   const_iterator End() const
   {
      return data->string + R::Length( data->string );
   }

   /*!
    * Returns a reverse iterator located at the <em>reverse beginning</em> of
    * this string. The returned iterator points to the last character of this
    * string.
    *
    * If this string is not unique, it is made unique before returning from
    * this member function.
    *
    * If this string is empty, this member function returns zero.
    */
   reverse_iterator ReverseBegin()
   {
      iterator end = End();
      return (end != 0) ? end-1 : 0;
   }

   /*!
    * Returns a constant reverse iterator located at the <em>reverse
    * beginning</em> of this string. The returned iterator points to the last
    * character of this string.
    *
    * If this string is empty, this member function returns zero.
    */
   const_reverse_iterator ReverseBegin() const
   {
      const_iterator end = End();
      return (end != 0) ? end-1 : 0;
   }

   /*!
    * Returns a reverse iterator located at the <em>reverse end</em> of this
    * string. The returned iterator points to the <em>previous-to-first</em>
    * character of this string.
    *
    * If this string is not unique, it is made unique before returning from
    * this member function.
    *
    * If this string is empty, this member function returns zero.
    */
   reverse_iterator ReverseEnd()
   {
      iterator begin = Begin();
      return (begin != 0) ? begin-1 : 0;
   }

   /*!
    * Returns a constant reverse iterator located at the <em>reverse end</em>
    * of this string. The returned iterator points to the
    * <em>previous-to-first</em> character of this string.
    *
    * If this string is empty, this member function returns zero.
    */
   const_reverse_iterator ReverseEnd() const
   {
      const_iterator begin = Begin();
      return (begin != 0) ? begin-1 : 0;
   }

   /*!
    * Assignment iterator. Returns a reference to this object.
    *
    * This operator calls Assign() with the specified source string \a s.
    */
   GenericString<T,R,A>& operator =( const GenericString<T,R,A>& s )
   {
      Assign( s );
      return *this;
   }

   /*!
    * Assigns a copy of the null-terminated character sequence stored in the
    * specified array \a t. Returns a reference to this object.
    */
   GenericString<T,R,A>& operator =( const T* t )
   {
      Assign( t );
      return *this;
   }

   /*!
    * Assigns a single character \a c to this string. Returns a reference to
    * this object.
    */
   GenericString<T,R,A>& operator =( T c )
   {
      Assign( c, 1 );
      return *this;
   }

   /*!
    * Assigns an string \a s to this string.
    *
    * Decrements the reference counter of the current string data, and destroys
    * it if it becomes unreferenced.
    *
    * Increments the reference counter of the source string's data and
    * references it in this string.
    *
    * After calling this member function, this string will never be unique.
    */
   void Assign( const GenericString<T,R,A>& s )
   {
      if ( s.data != data )
      {
         s.data->Attach();
         if ( !data->Detach() )
            delete data;
         data = s.data;
      }
   }

   /*!
    * Assigns a substring of \a n characters from a source string \a s,
    * starting from its \a i-th character.
    */
   void Assign( const GenericString<T,R,A>& s, size_type i, size_type n )
   {
      Assign( s.SubString( i, n ) );
   }

   /*!
    * Assigns a copy of the null-terminated character sequence stored in the
    * specified array \a t.
    */
   void Assign( const T* t )
   {
      size_type n = R::Length( t );
      if ( n > 0 ) // s must be non-empty
      {
         // Ensure that this GenericString is unique
         if ( !IsUnique() )
         {
            if ( !data->Detach() )
               delete data;
            data = new Data;
         }

         // Reallocate if necessary.
         MaybeReallocate( n );

         // Copy characters, including the null terminator.
         R::Copy( data->string, t, n+1 );

         return;
      }

      Clear();
   }

   /*!
    * Assigns a copy of the character sequence defined by the [i,j[ range.
    */
   void Assign( const T* i, const T* j )
   {
      if ( i < j )
         Assign( i, 0, j-i );
      else
         Clear();
   }

   /*!
    * Assigns a copy of the \a n first characters stored in the \a t array,
    * starting from its \a i-th character.
    */
   void Assign( const T* t, size_type i, size_type n )
   {
      if ( n > 0 )
      {
         size_type len = R::Length( t );
         if ( i < len ) // index must be valid, and s must be non-empty
         {
            n = pcl::Min( n, len-i );

            // Ensure that this GenericString is unique
            if ( !IsUnique() )
            {
               if ( !data->Detach() )
                  delete data;
               data = new Data;
            }

            // Reallocate if necessary.
            MaybeReallocate( n );

            // Copy segment of characters.
            R::Copy( data->string, t+i, n );

            // Place the null terminator after the last character.
            data->string[n] = R::Null();

            return;
         }
      }

      Clear();
   }

   /*!
    * Assigns \a n copies of the specified character \a c.
    */
   void Assign( T c, size_type n = 1 )
   {
      if ( R::IsNull( c ) || n == 0 ) // assigning null is emptying the string
         Clear();
      else
      {
         // Ensure that this GenericString is unique
         if ( !IsUnique() )
         {
            if ( !data->Detach() )
               delete data;
            data = new Data;
         }

         // Reallocate if necessary.
         MaybeReallocate( n );

         // Fill with char c
         R::Fill( data->string, c, n );

         // Place the null terminator at the end of the string
         data->string[n] = R::Null();
      }
   }

   /*!
    * Fills this string (if not empty) with the specified character \a c.
    *
    * \note If the specified filling character \a c is zero, or the null
    * character, the string will be reported as an empty string by Length() and
    * IsEmpty() after calling this function, although it will continue storing
    * the same allocated data block.
    */
   void Fill( T c )
   {
      size_type n = Length();
      if ( n > 0 )
      {
         if ( !IsUnique() )
         {
            if ( !data->Detach() )
               delete data;
            data = new Data;
            data->Allocate( n );
         }

         R::Fill( data->string, c, n );
      }
   }

   /*!
    * Fills a segment of \a n contiguous characters in this string with the
    * specified character \a c, starting from the \a i-th character.
    *
    * \note If the specified filling character \a c is zero, or the null
    * character, the string will be reported as an empty string by Length() and
    * IsEmpty() after calling this function, although it will continue storing
    * the same allocated data block.
    */
   void Fill( T c, size_type i, size_type n = maxPos )
   {
      size_type len = Length();
      if ( i < len )
      {
         n = pcl::Min( n, len-i );
         if ( n == len )
            Fill( c );
         else
         {
            SetUnique();
            R::Fill( data->string+i, c, n );
         }
      }
   }

   /*!
    * Securely fills this string and all instances sharing its string data with
    * the specified character \a c.
    *
    * If no filling character \a c is specified, the string will be filled with
    * zeros, or null characters.
    *
    * This function is useful to ensure that sensitive data, such as user
    * passwords and user names, are destroyed without the risk of surviving
    * duplicates as a result of data sharing.
    *
    * \note The normal data sharing mechanism is ignored by this function, so if
    * there are other objects sharing the same string data, all of them will be
    * affected unconditionally after calling this function.
    *
    * \note If the specified filling character \a c is zero, or the null
    * character (the default value), the string will be reported as an empty
    * string by Length() and IsEmpty() after calling this function, although it
    * will continue storing the same allocated data block.
    */
   void SecureFill( T c = T( 0 ) )
   {
      if ( data->string != 0 )
         ::memset( data->string, c, Length()*sizeof( T ) );
   }

   /*!
    * Ensures that this string has enough capacity to store at least \a n
    * characters plus a terminating null character.
    *
    * After calling this member function, this object uniquely references its
    * string data.
    */
   void Reserve( size_type n )
   {
      if ( n > 0 )
         if ( IsUnique() )
         {
            if ( data->capacity < n+1 )
            {
               T* old = data->string;

               data->Allocate( n );

               n = R::Length( old );
               R::Copy( data->string, old, n );
               data->string[n] = R::Null();

               data->alloc.Deallocate( old );
            }
         }
         else
         {
            size_type len = R::Length( data->string );
            n = pcl::Max( len, n );

            Data* newData = new Data;
            newData->Allocate( n );

            R::Copy( newData->string, data->string, len );
            newData->string[len] = R::Null();

            if ( !data->Detach() )
               delete data;
            data = newData;
         }
   }

   /*!
    * Causes this string to allocate the exact required memory space to store
    * its contained characters, plus a terminating nul character.
    *
    * If the string has excess capacity, a new copy of its existing characters
    * is generated and stored in a newly allocated memory block that fits them
    * exactly, and the previous memory block is deallocated.
    *
    * If the string is empty, calling this function is equivalent to Clear().
    * Note that in this case a previously allocated memory block (by a call to
    * Reserve()) may also be deallocated.
    *
    * After calling this member function, this object uniquely references its
    * string data.
    */
   void Squeeze()
   {
      size_type n = Length();
      if ( n == 0 )
      {
         if ( data->capacity > 0 )
            Clear();
      }
      else
      {
         if ( IsUnique() )
         {
            if ( data->capacity > n+1 )
            {
               T* old = data->string;
               data->Allocate( n );
               R::Copy( data->string, old, n );
               data->string[n] = R::Null();
               data->alloc.Deallocate( old );
            }
         }
         else
         {
            Data* newData = new Data;
            newData->Allocate( n );
            R::Copy( newData->string, data->string, n );
            newData->string[n] = R::Null();

            if ( !data->Detach() )
               delete data;
            data = newData;
         }
      }
   }

   /*!
    * Releases the string data.
    *
    * This member function returns a pointer to the referenced string data,
    * after ensuring that it is uniquely referenced by this string.
    *
    * The caller is responsible for destructing and deallocating the returned
    * block when it is no longer required.
    *
    * Before returning, this member function empties this string without
    * deallocating the string data.
    */
   T* Release()
   {
      if ( IsEmpty() )
         return 0;

      SetUnique();

      T* old = data->string;

      data->string = 0;
      data->capacity = 0;

      return old;
   }

   /*!
    * Inserts a copy of the string \a s at the index \a i in this string.
    */
   template <class R1, class A1>
   void Insert( size_type i, const GenericString<T,R1,A1>& s )
   {
      Insert( i, s.data->string );
   }

   /*!
    * Inserts a null-terminated string \a t at the index \a i in this string.
    */
   void Insert( size_type i, const T* t )
   {
      Insert( i, t, R::Length( t ) );
   }

   /*!
    * Inserts the first \a n characters of a null-terminated string \a t at the
    * index \a i in this string.
    */
   void Insert( size_type i, const T* t, size_type n )
   {
      if ( n > 0 && t != 0 )
      {
         UninitializedGrow( i, n );
         R::Copy( data->string+i, t, n );
      }
   }

   /*!
    * Inserts \a n copies of a character \a c at the index \a i in this string.
    */
   void Insert( size_type i, T c, size_type n = 1 )
   {
      if ( n > 0 && !R::IsNull( c ) )
      {
         UninitializedGrow( i, n );
         R::Fill( data->string+i, c, n );
      }
   }

   /*!
    * Appends a copy of the specified string \a s to this string.
    */
   template <class R1, class A1>
   void Append( const GenericString<T,R1,A1>& s )
   {
      Append( s.data->string );
   }

   /*!
    * Appends a copy of a string by calling Append( s ).
    * Returns a reference to this object.
    */
   template <class R1, class A1>
   GenericString<T,R,A>& operator +=( const GenericString<T,R1,A1>& s )
   {
      Append( s ); return *this;
   }

   /*!
    * Appends a copy of the first \a n characters of a null-terminated
    * character sequence \a t to this string.
    */
   void Append( const T* t, size_type n )
   {
      Insert( maxPos, t, n );
   }

   /*!
    * Appends a copy of a null-terminated character sequence to this string.
    */
   void Append( const T* t )
   {
      Insert( maxPos, t );
   }

   /*!
    * Appends a copy of a null-terminated string by calling Append( t ).
    * Returns a reference to this object.
    */
   GenericString<T,R,A>& operator +=( const T* t )
   {
      Append( t ); return *this;
   }

   /*!
    * Appends \a n copies of a character \a c to this string.
    */
   void Append( T c, size_type n = 1 )
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
    * A synonym for Append( const T*, size_type ), provided for compatibility
    * with PCL container classes.
    */
   void Add( const T* t, size_type n )
   {
      Append( t, n );
   }

   /*!
    * A synonym for Append( const T* ), provided for compatibility with PCL
    * container classes.
    */
   void Add( const T* t )
   {
      Append( t );
   }

   /*!
    * A synonym for Append( T, size_type ), provided for compatibility with PCL
    * container classes.
    */
   void Add( T c, size_type n = 1 )
   {
      Append( c, n );
   }

   /*!
    * Appends a copy of the specified character \a c to this string.
    * Returns a reference to this object.
    */
   GenericString<T,R,A>& operator +=( T c )
   {
      Append( c ); return *this;
   }

   /*!
    * Prepends a copy of the specified string \a s to this string.
    */
   template <class R1, class A1>
   void Prepend( const GenericString<T,R1,A1>& s )
   {
      Prepend( s.data->string );
   }

   /*!
    * Prepends a copy of a string by calling Append( s ).
    * Returns a reference to this object.
    */
   template <class R1, class A1>
   GenericString<T,R,A>& operator -=( const GenericString<T,R1,A1>& s )
   {
      Prepend( s ); return *this;
   }

   /*!
    * Prepends a copy of the first \a n characters of a null-terminated
    * character sequence \a t to this string.
    */
   void Prepend( const T* t, size_type n )
   {
      Insert( 0, t, n );
   }

   /*!
    * Prepends a copy of a null-terminated character sequence to this string.
    */
   void Prepend( const T* t )
   {
      Insert( 0, t );
   }

   /*!
    * Prepends a copy of a null-terminated string by calling Prepend( t ).
    * Returns a reference to this object.
    */
   GenericString<T,R,A>& operator -=( const T* t )
   {
      Prepend( t ); return *this;
   }

   /*!
    * Prepends \a n copies of a character \a c to this string.
    */
   void Prepend( T c, size_type n = 1 )
   {
      Insert( 0, c, n );
   }

   /*!
    * Prepends a copy of the specified character \a c to this string.
    * Returns a reference to this object.
    */
   GenericString<T,R,A>& operator -=( T c )
   {
      Prepend( c ); return *this;
   }

   /*!
    * Replaces a segment of \a n contiguous characters, starting at the \a i-th
    * character in this string, with a copy of a string \a s.
    */
   template <class R1, class A1>
   void Replace( size_type i, size_type n, const GenericString<T,R1,A1>& s )
   {
      Replace( i, n, s.data->string );
   }

   /*!
    * Replaces a segment of \a n contiguous characters, starting at the \a i-th
    * character in this string, with a copy of a null-terminated string \a t.
    */
   void Replace( size_type i, size_type n, const T* t )
   {
      if ( n > 0 )
      {
         size_type len = Length();
         if ( i < len ) // index must be valid
         {
            n = pcl::Min( n, len-i );  // constrain to replace existing chars.
            if ( n == len )   // replacing the entire string
               Assign( t );
            else
            {
               size_type nt = R::Length( t );
               if ( nt > 0 )
               {
                  if ( n < nt )
                     UninitializedGrow( i, nt-n );
                  else if ( nt < n )
                     Delete( i, n-nt );
                  else
                     SetUnique();

                  R::Copy( data->string+i, t, nt );
               }
               else
                  Delete( i, n );
            }
         }
      }
   }

   /*!
    * Replaces a segment of \a n contiguous characters, starting at the \a i-th
    * character in this string, with \a nc copies of a character \a c.
    */
   void Replace( size_type i, size_type n, T c, size_type nc = 1 )
   {
      if ( n > 0 )  // segment cannot be void
      {
         size_type len = Length();
         if ( i < len ) // index must be valid
         {
            n = pcl::Min( n, len-i );  // constrain to replace existing chars.
            if ( n == len )   // replacing the entire string
               Assign( c, nc );
            else
            {
               if ( R::IsNull( c ) || nc == 0 ) // replacing with nothing
                  Delete( i, n );
               else
               {
                  if ( n < nc )
                     UninitializedGrow( i, nc-n );
                  else if ( nc < n )
                     Delete( i, n-nc );
                  else
                     SetUnique();

                  R::Fill( data->string+i, c, nc );
               }
            }
         }
      }
   }

   /*!
    * Replaces all occurrences of a character \a c1 with \a c2 in a segment of
    * \a n contiguous characters starting at the \a i-th character in this
    * string.
    */
   void ReplaceChar( T c1, T c2, size_type i = 0, size_type n = maxPos )
   {
      ReplaceChar( c1, c2, i, n, true );
   }

   /*!
    * Replaces all occurrences of a character \a c1 with \a c2 in a segment of
    * \a n contiguous characters starting at the \a i-th character in this
    * string.
    *
    * This member function performs case-insensitive character comparisons.
    */
   void ReplaceCharIC( T c1, T c2, size_type i = 0, size_type n = maxPos )
   {
      ReplaceChar( c1, c2, i, n, false );
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
      ReplaceString( s1.data->string, s2.data->string, i );
   }

   /*!
    * Replaces all occurrences of a null-terminated string \a t1 with \a t2 in
    * a segment of contiguous characters starting at the \a i-th character, and
    * spanning to the end of this string.
    */
   void ReplaceString( const T* t1, const T* t2, size_type i = 0 )
   {
      ReplaceString( t1, t2, i, true );
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
      ReplaceStringIC( s1.data->string, s2.data->string, i );
   }

   /*!
    * Replaces all occurrences of a null-terminated string \a t1 with \a t2 in
    * a segment of contiguous characters starting at the \a i-th character, and
    * spanning to the end of this string.
    *
    * This member function performs case-insensitive string comparisons.
    */
   void ReplaceStringIC( const T* t1, const T* t2, size_type i = 0 )
   {
      ReplaceString( t1, t2, i, false );
   }

   /*!
    * Deletes a contiguous segment of \a n characters starting at the \a i-th
    * character in this string.
    */
   void Delete( size_type i, size_type n = 1 )
   {
      if ( n > 0 )
      {
         size_type len = Length();
         if ( i < len ) // index must be valid
         {
            n = pcl::Min( n, len-i );  // constrain to delete existing chars.
            if ( n == len )   // deleting the entire string
               Clear();
            else
            {
               SetUnique();   // ensure that only we own these chars.

               size_type newLen = len-n;
               T* old = data->string;

               // Reallocate if the resulting unused space would be prohibitive.
               if ( data->ShouldReallocate( newLen ) )
               {
                  data->Allocate( newLen );
                  // Keep the initial (not deleted) segment, if exists.
                  R::Copy( data->string, old, i );
               }

               // Copy the final (not deleted) segment, if exists.
               if ( i < newLen )
                  R::Copy( data->string+i, old+i+n, newLen-i );

               // If reallocated, free the previous block.
               if ( old != data->string )
                  data->alloc.Deallocate( old );

               // Don't forget the terminating null char.
               data->string[newLen] = R::Null();
            }
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
    * Deletes the initial segment of \a i contiguous characters in this
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
   void DeleteChar( T c, size_type i = 0 )
   {
      T s1[ 2 ]; *s1 = c, s1[1] = R::Null();
      ReplaceString( s1, 0, i, true );
   }

   /*!
    * Deletes all occurrences of the \a c character in a segment of contiguous
    * characters starting from the \a i-th character and spanning to the end of
    * this string.
    *
    * This member function performs case-insensitive character comparisons.
    */
   void DeleteCharIC( T c, size_type i = 0 )
   {
      T s1[ 2 ]; *s1 = c, s1[1] = R::Null();
      ReplaceString( s1, 0, i, false );
   }

   /*!
    * Deletes all occurrences of a string \a s in a segment of contiguous
    * characters starting from the \a i-th character and spanning to the end of
    * this string.
    */
   template <class R1, class A1>
   void DeleteString( const GenericString<T,R1,A1>& s, size_type i = 0 )
   {
      ReplaceString( s.data->string, 0, i, true );
   }

   /*!
    * Deletes all occurrences of a null-terminated string \a s in a segment of
    * contiguous characters starting from the \a i-th character and spanning to
    * the end of this string.
    */
   void DeleteString( const T* t, size_type i = 0 )
   {
      ReplaceString( t, 0, i, true );
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
      ReplaceString( s.data->string, 0, i, false );
   }

   /*!
    * Deletes all occurrences of a null-terminated string \a s in a segment of
    * contiguous characters starting from the \a i-th character and spanning to
    * the end of this string.
    *
    * This member function performs case-insensitive string comparisons.
    */
   void DeleteStringIC( const T* t, size_type i = 0 )
   {
      ReplaceString( t, 0, i, false );
   }

   /*!
    * Removes all characters in this string.
    *
    * If this string uniquely references its string data, it is destroyed and
    * deallocated; otherwise its reference counter is decremented. Then a new,
    * empty data structure is created and uniquely referenced.
    *
    * \note Clear(), Erase(), Delete(), Empty() and Remove() are all synonyms
    * in the GenericString class.
    */
   void Clear()
   {
      if ( IsUnique() )
         data->Deallocate();
      else
      {
         if ( !data->Detach() )
         {
            data->Deallocate();
            data->Attach();
         }
         else
            data = new Data;
      }
   }

   /*!
    * A synonym for Clear().
    */
   void Erase() { Clear(); }

   /*!
    * A synonym for Clear().
    */
   void Delete() { Clear(); }

   /*!
    * A synonym for Clear().
    */
   void Empty() { Clear(); }

   /*!
    * A synonym for Clear().
    */
   void Remove() { Clear(); }

   /*!
    * Returns a string with a copy of \a n contiguous characters starting at
    * the \a i-th character in this string.
    */
   GenericString<T,R,A> SubString( size_type i, size_type n = maxPos ) const
   {
      GenericString<T,R,A> s;   // returned GenericString is in the stack
      if ( n > 0 ) // if there's something to extract
      {
         size_type len = Length();
         if ( i < len ) // index must be valid
         {
            n = pcl::Min( n, len-i );  // constrain to extract n characters
            s.data->Allocate( n );
            R::Copy( s.data->string, data->string+i, n );
            s.data->string[n] = R::Null();
         }
      }
      return s;
   }

   /*!
    * Returns a string with a copy of the \a n contiguous initial characters of
    * this string.
    */
   GenericString<T,R,A> Left( size_type n ) const
   {
      return SubString( 0, n );
   }

   /*!
    * Returns a string with a copy of the \a n contiguous ending characters of
    * this string.
    */
   GenericString<T,R,A> Right( size_type n ) const
   {
      size_type len = Length();
      n = pcl::Min( n, len );
      return SubString( len-n, n );
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
    */
   template <class C, class R1, class A1>
   void Break( C& list, const GenericString<T,R1,A1>& s, bool trim = false, size_type i = 0 ) const
   {
      Break( list, s.data->string, trim, i );
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
    */
   template <class C>
   void Break( C& list, const T* s, bool trim = false, size_type i = 0 ) const
   {
      size_type len = Length();
      if ( i < len )
      {
         size_type n = R::Length( s );
         if ( n > 0 )
         {
            for ( ;; )
            {
               size_type j = Find( s, i );
               if ( j == notFound )
                  break;

               GenericString<T,R,A> t;
               if ( j > i )
               {
                  size_type m = j - i;
                  t.data->Allocate( m );
                  R::Copy( t.data->string, data->string+i, m );
                  t.data->string[m] = R::Null();
                  if ( trim )
                     t.Trim();
               }
               list.Add( t );

               if ( (i = j+n) == len )
                  return;
            }

            GenericString<T,R,A> t;
            t.data->Allocate( n = len-i );
            R::Copy( t.data->string, data->string+i, n );
            t.data->string[n] = R::Null();
            if ( trim )
               t.Trim();
            list.Add( t );
         }
      }
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
    */
   template <class C>
   void Break( C& list, T c, bool trim = false, size_type i = 0 ) const
   {
      size_type len = Length();
      if ( len > 0 && i < len )
      {
         for ( ;; )
         {
            size_type j = Find( c, i );
            if ( j == notFound )
               break;

            GenericString<T,R,A> t;

            size_type n = j-i;
            if ( n > 0 )
            {
               t.data->Allocate( n );
               R::Copy( t.data->string, data->string+i, n );
               t.data->string[n] = R::Null();
               if ( trim )
                  t.Trim();
            }
            list.Add( t );

            if ( (i = j+1) == len )
               return;
         }

         GenericString<T,R,A> t;
         size_type n = len-i;
         t.data->Allocate( n );
         R::Copy( t.data->string, data->string+i, n );
         t.data->string[n] = R::Null();
         if ( trim )
            t.Trim();
         list.Add( t );
      }
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
    */
   template <class C, class R1, class A1>
   void BreakIC( C& list, const GenericString<T,R1,A1>& s, bool trim = false, size_type i = 0 ) const
   {
      BreakIC( list, s.data->string, trim, i );
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
    */
   template <class C>
   void BreakIC( C& list, const T* s, bool trim = false, size_type i = 0 ) const
   {
      size_type len = Length();
      if ( i < len )
      {
         size_type n = R::Length( s );
         if ( n > 0 )
         {
            for ( ;; )
            {
               size_type j = FindIC( s, i );
               if ( j == notFound )
                  break;

               GenericString<T,R,A> t;
               if ( j > i )
               {
                  size_type m = j - i;
                  t.data->Allocate( m );
                  R::Copy( t.data->string, data->string+i, m );
                  t.data->string[m] = R::Null();
                  if ( trim )
                     t.Trim();
               }
               list.Add( t );

               if ( (i = j+n) == len )
                  return;
            }

            GenericString<T,R,A> t;
            t.data->Allocate( n = len-i );
            R::Copy( t.data->string, data->string+i, n );
            t.data->string[n] = R::Null();
            if ( trim )
               t.Trim();
            list.Add( t );
         }
      }
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
    */
   template <class C>
   void BreakIC( C& list, T c, bool trim = false, size_type i = 0 ) const
   {
      size_type len = Length();
      if ( i < len )
      {
         for ( ;; )
         {
            size_type j = FindIC( c, i );
            if ( j == notFound )
               break;

            GenericString<T,R,A> t;
            size_type n = j-i;
            if ( n > 0 )
            {
               t.data->Allocate( n );
               R::Copy( t.data->string, data->string+i, n );
               t.data->string[n] = R::Null();
               if ( trim )
                  t.Trim();
            }
            list.Add( t );

            if ( (i = j+1) == len )
               return;
         }

         GenericString<T,R,A> t;
         size_type n = len-i;
         t.data->Allocate( n );
         R::Copy( t.data->string, data->string+i, n );
         t.data->string[n] = R::Null();
         if ( trim )
            t.Trim();
         list.Add( t );
      }
   }

   /*!
    * Returns the first character in this string, or a <em>null character</em>
    * (R::Null()) if this string is empty.
    */
   T FirstChar() const
   {
      return IsEmpty() ? R::Null() : *data->string;
   }

   /*!
    * Returns the last character in this string, or a <em>null character</em>
    * (R::Null()) if this string is empty.
    */
   T LastChar() const
   {
      return IsEmpty() ? R::Null() : *(data->string + R::Length( data->string ) - 1);
   }

   /*!
    * Returns true if this string begins with the specified substring \a s.
    */
   template <class R1, class A1>
   bool BeginsWith( const GenericString<T,R1,A1>& s ) const
   {
      return FindFirst( s ) == 0;
   }

   /*!
    * Returns true if this string begins with the specified null-terminated
    * substring \a t.
    */
   bool BeginsWith( const T* t ) const
   {
      return FindFirst( t ) == 0;
   }

   /*!
    * Returns true if this string begins with the specified character \a c.
    */
   bool BeginsWith( T c ) const
   {
      return !IsEmpty() && *data->string == c;
   }

   /*!
    * Returns true if this string begins with the specified substring \a s.
    *
    * This member function performs case-insensitive string comparisons to find
    * an instance of the substring \a s.
    */
   template <class R1, class A1>
   bool BeginsWithIC( const GenericString<T,R1,A1>& s ) const
   {
      return FindFirstIC( s ) == 0;
   }

   /*!
    * Returns true if this string begins with the specified null-terminated
    * substring \a t.
    *
    * This member function performs case-insensitive string comparisons to find
    * an instance of the substring \a s.
    */
   bool BeginsWithIC( const T* t ) const
   {
      return FindFirstIC( t ) == 0;
   }

   /*!
    * Returns true if this string begins with the specified character \a c.
    *
    * This member function performs case-insensitive character comparisons to
    * find an instance of the character \a c.
    */
   bool BeginsWithIC( T c ) const
   {
      if ( !IsEmpty() )
      {
         T c1 = *data->string; R::ToLowerCase( &c1, 1 );
         T c2 = c; R::ToLowerCase( &c2, 1 );
         return c1 == c2;
      }
      return false;
   }

   /*!
    * Returns true if this string ends with the specified substring \a s.
    */
   template <class R1, class A1>
   bool EndsWith( const GenericString<T,R1,A1>& s ) const
   {
      size_type p = FindLast( s );
      return p != notFound && p == Length() - s.Length();
   }

   /*!
    * Returns true if this string ends with the specified null-terminated
    * substring \a t.
    */
   bool EndsWith( const T* t ) const
   {
      size_type p = FindLast( t );
      return p != notFound && p == Length() - R::Length( t );
   }

   /*!
    * Returns true if this string ends with the specified character \a c.
    */
   bool EndsWith( T c ) const
   {
      return !IsEmpty() && *(data->string + R::Length( data->string ) - 1) == c;
   }

   /*!
    * Returns true if this string ends with the specified substring \a s.
    *
    * This member function performs case-insensitive string comparisons to find
    * an instance of the substring \a s.
    */
   template <class R1, class A1>
   bool EndsWithIC( const GenericString<T,R1,A1>& s ) const
   {
      size_type p = FindLastIC( s );
      return p != notFound && p == Length() - s.Length();
   }

   /*!
    * Returns true if this string ends with the specified null-terminated
    * substring \a t.
    *
    * This member function performs case-insensitive string comparisons to find
    * an instance of the substring \a s.
    */
   bool EndsWithIC( const T* t ) const
   {
      size_type p = FindLastIC( t );
      return p != notFound && p == Length() - R::Length( t );
   }

   /*!
    * Returns true if this string ends with the specified character \a c.
    *
    * This member function performs case-insensitive character comparisons to
    * find an instance of the character \a c.
    */
   bool EndsWithIC( T c ) const
   {
      if ( !IsEmpty() )
      {
         T c1 = *(data->string + R::Length( data->string ) - 1); R::ToLowerCase( &c1, 1 );
         T c2 = c; R::ToLowerCase( &c2, 1 );
         return c1 == c2;
      }
      return false;
   }

   /*!
    * Returns the starting index \a k of the first occurrence of a substring
    * \a s in this string, such that \a k >= \a i. Returns notFound if such
    * occurrence does not exist.
    */
   template <class R1, class A1>
   size_type FindFirst( const GenericString<T,R1,A1>& s, size_type i = 0 ) const
   {
      return FindFirst( s.data->string, i );
   }

   /*!
    * Returns the starting index \a k of the first occurrence of a
    * null-terminated substring \a s in this string, such that \a k >= \a i.
    * Returns notFound if such occurrence does not exist.
    */
   size_type FindFirst( const T* t, size_type i = 0 ) const
   {
      size_type n = R::Length( t );
      if ( n > 0 )
      {
         size_type len = Length();
         if ( i < len && n <= len-i )
            for ( size_type maxIdx = len-n; ; )
            {
               if ( data->string[i] == *t )
                  for ( size_type j = 1, k = i; ; ++j )
                  {
                     if ( j == n )
                        return i;

                     if ( data->string[++k] != t[j] )
                        break;
                  }

               if ( ++i > maxIdx )
                  break;
            }
      }

      return notFound;
   }

   /*!
    * Returns the index \a k of the first occurrence of a character \a c in
    * this string, such that \a k >= \a i. Returns notFound if such occurrence
    * does not exist.
    */
   size_type FindFirst( T c, size_type i = 0 ) const
   {
      if ( i < Length() )
         for ( iterator p = data->string+i; ; ++p )
         {
            T cp = *p;
            if ( cp == c )
               return p - data->string;
            if ( R::IsNull( cp ) )
               break;
         }

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
      return FindFirstIC( s.data->string, i );
   }

   /*!
    * Returns the starting index \a k of the first occurrence of a
    * null-terminated substring \a s in this string, such that \a k >= \a i.
    * Returns notFound if such occurrence does not exist.
    *
    * This member function performs case-insensitive string comparisons to find
    * an instance of the substring \a s.
    */
   size_type FindFirstIC( const T* t, size_type i = 0 ) const
   {
      size_type n = R::Length( t );
      if ( n > 0 )
      {
         size_type len = Length();
         if ( i < len && n <= len-i )
            for ( size_type maxIdx = len-n; ; )
            {
               if ( R::Compare( data->string+i, n, t, n, false, false ) == 0 )
                  return i;
               if ( ++i > maxIdx )
                  break;
            }
      }

      return notFound;
   }

   /*!
    * Returns the index \a k of the first occurrence of a character \a c in
    * this string, such that \a k >= \a i. Returns notFound if such occurrence
    * does not exist.
    *
    * This member function performs case-insensitive character comparisons to
    * find an instance of the character \a c.
    */
   size_type FindFirstIC( T c, size_type i = 0 ) const
   {
      if ( i < Length() )
      {
         T c2 = c; R::ToLowerCase( &c2, 1 );
         for ( iterator p = data->string+i; ; ++p )
         {
            T c1 = *p; R::ToLowerCase( &c1, 1 );
            if ( c1 == c2 )
               return p - data->string;
            if ( R::IsNull( *p ) )
               break;
         }
      }
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
    * A synonym for FindFirst( const T*, size_type ).
    */
   size_type Find( const T* t, size_type i = 0 ) const
   {
      return FindFirst( t, i );
   }

   /*!
    * A synonym for FindFirst( T, size_type ).
    */
   size_type Find( T c, size_type i = 0 ) const
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
    * A synonym for FindFirstIC( const T*, size_type ).
    */
   size_type FindIC( const T* t, size_type i = 0 ) const
   {
      return FindFirstIC( t, i );
   }

   /*!
    * A synonym for FindFirstIC( T, size_type ).
    */
   size_type FindIC( T c, size_type i = 0 ) const
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
      return FindLast( s.data->string, r );
   }

   /*!
    * Returns the starting index \a k of the last occurrence of a
    * null-terminated substring \a t in this string, such that:
    *
    * \a k <= \a r - \a n,
    *
    * where \a n is the length of the substring \a t. Returns notFound if such
    * occurrence does not exist.
    */
   size_type FindLast( const T* t, size_type r = maxPos ) const
   {
      size_type n = R::Length( t );
      if ( n > 0 ) // cannot find an empty string
      {
         size_type len = Length();
         if ( len > 0 ) // cannot find in an empty string
         {
            r = pcl::Min( r, len-1 ) + 1; // ### avoid r+1 if r==maxPos
            if ( n <= r ) // wouldn't find a string longer than this
            {
               // Search for the first char of s, in reverse order.
               for ( size_type i = r-n; ; --i )
               {
                  if ( data->string[i] == *t ) // found first char at i
                     // See if this is an occurrence of s, starting from i+1
                     for ( size_type j = 1, k = i; ; ++j )
                     {
                        if ( j == n )  // complete sequence found
                           return i;   //    return 1st char index
                        if ( data->string[++k] != t[j] ) // found a difference
                           break;
                     }

                  if ( i == 0 )  // end of the source string
                     break;
               }
            }
         }
      }

      return notFound;
   }

   /*!
    * Returns the index \a k of the last occurrence of a character \a c in this
    * string, such that \a k < \a r. Returns notFound if such occurrence does
    * not exist.
    */
   size_type FindLast( T c, size_type r = maxPos ) const
   {
      size_type len = Length();
      if ( len > 0 ) // cannot search into an empty string
      {
         // Search for c in reverse order
         r = pcl::Min( r, len-1 );
         for ( iterator i = data->string+r; ; --i )
         {
            T ci = *i;
            if ( ci == c )           // found char!
               return i - data->string; //    return index, zero based
            if ( i == data->string ) // end of source string
               break;
         }
      }

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
      return FindLastIC( s.data->string, r );
   }

   /*!
    * Returns the starting index \a k of the last occurrence of a
    * null-terminated substring \a t in this string, such that:
    *
    * \a k <= \a r - \a n,
    *
    * where \a n is the length of the substring \a t. Returns notFound if such
    * occurrence does not exist.
    *
    * This member function performs case-insensitive string comparisons to
    * find an instance of the specified substring \a t.
    */
   size_type FindLastIC( const T* t, size_type r = maxPos ) const
   {
      size_type n = R::Length( t );
      if ( n > 0 ) // cannot find an empty string
      {
         size_type len = Length();
         if ( len > 0 ) // cannot find in an empty string
         {
            r = pcl::Min( r, len-1 ) + 1; // ### avoid r+1 if r==maxPos
            if ( n <= r ) // wouldn't find a string longer than this
            {
               // Search for an occurrence of t, in reverse order.
               for ( size_type i = r-n; ; --i )
               {
                  if ( R::Compare( data->string+i, n, t, n, false, false ) == 0 )
                     return i;   // found occurrence at index i
                  if ( i == 0 )  // end of source string
                     break;
               }
            }
         }
      }

      return notFound;
   }

   /*!
    * Returns the index \a k of the last occurrence of a character \a c in this
    * string, such that \a k < \a r. Returns notFound if such occurrence does
    * not exist.
    *
    * This member function performs case-insensitive character comparisons to
    * find an instance of the specified character \a c.
    */
   size_type FindLastIC( T c, size_type r = maxPos ) const
   {
      size_type len = Length();
      if ( len > 0 ) // cannot search into an empty string
      {
         // Search for c in reverse order
         T c2 = c; R::ToLowerCase( &c2, 1 );
         r = pcl::Min( r, len-1 );
         for ( iterator i = data->string+r; ; --i )
         {
            T c1 = *i; R::ToLowerCase( &c1, 1 );
            if ( c1 == c2 )
               return i - data->string; // found c: return index, zero based
            if ( i == data->string ) // end of source string
               break;
         }
      }

      return notFound;
   }

   /*!
    * Returns true if this string contains a substring \a s.
    */
   template <class R1, class A1>
   bool Has( const GenericString<T,R1,A1>& s ) const
   {
      return Has( s.data->string );
   }

   /*!
    * Returns true if this string contains a null-terminated substring \a t.
    */
   bool Has( const T* t ) const
   {
      return Find( t ) != notFound;
   }

   /*!
    * Returns true if this string contains a character \a c.
    */
   bool Has( T c ) const
   {
      return Find( c ) != notFound;
   }

   /*!
    * Returns true if this string contains a substring \a s.
    *
    * This member function performs case-insensitive string comparisons to
    * find an instance of the specified substring \a s.
    */
   template <class R1, class A1>
   bool HasIC( const GenericString<T,R1,A1>& s ) const
   {
      return HasIC( s.data->string );
   }

   /*!
    * Returns true if this string contains a null-terminated substring \a t.
    *
    * This member function performs case-insensitive string comparisons to
    * find an instance of the specified substring \a t.
    */
   bool HasIC( const T* t ) const
   {
      return FindIC( t ) != notFound;
   }

   /*!
    * Returns true if this string contains a character \a c.
    *
    * This member function performs case-insensitive character comparisons to
    * find an instance of the specified character \a c.
    */
   bool HasIC( T c ) const
   {
      return FindIC( c ) != notFound;
   }

   /*!
    * Removes all leading and trailing <em>trimable characters</em>.
    *
    * Trimable characters are determined by the traits class R. A character
    * \a c is trimable if R::IsTrimable( c ) is true. Generally, the set of
    * trimable characters corresponds to the set of white space characters.
    */
   void Trim()
   {
      size_type len = R::Length( data->string );
      if ( len > 0 )
      {
         // Look for an initial segment of trimable chars.
         iterator i1 = SearchTrimLeft();
         if ( R::IsNull( *i1 ) ) // the whole string is trimable -> empty it
            Clear();
         else
         {
            // Look for a terminating segment of trimable chars., starting from
            // the end of the string, in reverse order.
            iterator i2;
            for ( i2 = data->string + len; R::IsTrimable( *--i2 ); ) {}

            // Calculate the new trimmed length
            size_type newLen = size_type( 1 + i2 - i1 );

            // Remove trimable characters from this string
            if ( newLen != len )
               Trim( i1, newLen );
         }
      }
   }

   /*!
    * Removes all leading <em>trimable characters</em>.
    *
    * Trimable characters are determined by the traits class R. A character
    * \a c is trimable if R::IsTrimable( c ) is true. Generally, the set of
    * trimable characters corresponds to the set of white space characters.
    */
   void TrimLeft()
   {
      size_type len = R::Length( data->string );
      if ( len > 0 )
      {
         // Look for an initial segment of trimable chars.
         iterator i1 = SearchTrimLeft();
         if ( R::IsNull( *i1 ) ) // the whole string is trimable -> empty it
            Clear();
         else
         {
            // If there are some trimable characters, remove them.
            size_type count = size_type( i1 - data->string );
            if ( count > 0 )
               Trim( i1, len - count );
         }
      }
   }

   /*!
    * Removes all trailing <em>trimable characters</em>.
    *
    * Trimable characters are determined by the traits class R. A character
    * \a c is trimable if R::IsTrimable( c ) is true. Generally, the set of
    * trimable characters corresponds to the set of white space characters.
    */
   void TrimRight()
   {
      size_type len = R::Length( data->string );
      if ( len > 0 )
      {
         // Look for a terminating segment of trimable chars., starting from
         // the end of the string, in reverse order.
         iterator i2;
         for ( i2 = data->string + len; R::IsTrimable( *--i2 ); )
            if ( i2 == data->string )  // reached (reverse) end of string
            {
               Clear();                //    the whole string is trimable -> empty
               return;
            }

         // If there are some trimable characters, remove them.
         size_type newLen = size_type( 1 + i2 - data->string );
         if ( newLen != len )
            Trim( data->string, newLen );
      }
   }

   /*!
    * Returns a duplicate of this string with all leading and trailing
    * <em>trimable characters</em> removed.
    * \sa Trim()
    */
   GenericString<T,R,A> Trimmed() const
   {
      GenericString<T,R,A> s( *this );
      s.Trim();
      return s;
   }

   /*!
    * Returns a duplicate of this string with all leading <em>trimable
    * characters</em> removed.
    * \sa TrimLeft()
    */
   GenericString<T,R,A> TrimmedLeft() const
   {
      GenericString<T,R,A> s( *this );
      s.TrimLeft();
      return s;
   }

   /*!
    * Returns a duplicate of this string with all trailing <em>trimable
    * characters</em> removed.
    * \sa TrimRight()
    */
   GenericString<T,R,A> TrimmedRight() const
   {
      GenericString<T,R,A> s( *this );
      s.TrimRight();
      return s;
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
    * \li 0 if both strings are equal.
    * \li +1 if this string is greater than the specified string \a s
    * \li -1 if this string is less than the specified string \a s.
    */
   template <class R1, class A1>
   int Compare( const GenericString<T,R1,A1>& s,
                bool caseSensitive = true, bool localeAware = true ) const
   {
      return Compare( s.data->string, caseSensitive, localeAware );
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
    * \li 0 if both strings are equal.
    * \li +1 if this string is greater than the specified string \a s.
    * \li -1 if this string is less than the specified string \a s.
    */
   int Compare( const T* t, bool caseSensitive = true, bool localeAware = true ) const
   {
      return CompareString( t, caseSensitive, localeAware );
   }

   /*!
    * Lexicographical comparison to a single character.
    *
    * \param s             A character to which this string will be compared.
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
    * \li 0 if this string is equal to \a c.
    * \li +1 if this string is greater than \a c.
    * \li -1 if this string is less than the \a c.
    *
    * The performed comparison is equivalent to a comparison of this string
    * with a fictitious string of length one, whose only character was equal to
    * the specified character \a c.
    */
   int Compare( T c, bool caseSensitive = true, bool localeAware = true ) const
   {
      return CompareChar( c, caseSensitive, localeAware );
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
    * \li 0 if both strings are equal.
    * \li +1 if this string is greater than the specified string \a s.
    * \li -1 if this string is less than the specified string \a s.
    */
   template <class R1, class A1>
   int CompareIC( const GenericString<T,R1,A1>& s, bool localeAware = true ) const
   {
      return CompareIC( s.data->string, localeAware );
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
    * \li 0 if both strings are equal.
    * \li +1 if this string is greater than the specified string \a s.
    * \li -1 if this string is less than the specified string \a s.
    */
   int CompareIC( const T* t, bool localeAware = true ) const
   {
      return CompareString( t, false/*caseSensitive*/, localeAware );
   }

   /*!
    * Case-insensitive lexicographical comparison to a single character.
    *
    * \param s             A character to which this string will be compared.
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
    * \li 0 if this string is equal to \a c.
    * \li +1 if this string is greater than \a c.
    * \li -1 if this string is less than the \a c.
    *
    * The performed comparison is equivalent to a case-insensitive comparison
    * of this string with a fictitious string of length one, whose only
    * character was equal to \a c.
    */
   int CompareIC( T c, bool localeAware = true ) const
   {
      return CompareChar( c, false/*caseSensitive*/, localeAware );
   }

   /*!
    * Replaces all \e uppercase characters in this string with their
    * \e lowercase counterparts.
    */
   void ToLowerCase()
   {
      if ( !IsEmpty() )
      {
         SetUnique();
         R::ToLowerCase( data->string, R::Length( data->string ) );
      }
   }

   /*!
    * Replaces all \e lowercase characters in this string with their
    * \e uppercase counterparts.
    */
   void ToUpperCase()
   {
      if ( !IsEmpty() )
      {
         SetUnique();
         R::ToUpperCase( data->string, R::Length( data->string ) );
      }
   }

   /*!
    * Returns a duplicate of this string with all uppercase characters replaced
    * with their lowercase counterparts.
    */
   GenericString<T,R,A> LowerCase() const
   {
      GenericString<T,R,A> s( *this );
      s.ToLowerCase();
      return s;
   }

   /*!
    * Returns a duplicate of this string with all lowercase characters replaced
    * with their uppercase counterparts.
    */
   GenericString<T,R,A> UpperCase() const
   {
      GenericString<T,R,A> s( *this );
      s.ToUpperCase();
      return s;
   }

   // -------------------------------------------------------------------------

   /*!
    * Returns true if this string can be interpreted as a numeric literal:
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
      if ( data->string == 0 )
         return false;
      // Numerals start with a decimal digit, a decimal point, or a sign.
      T c = *SearchTrimLeft();
      return !R::IsNull( c ) &&
         (R::IsDigit( c ) || R::IsSign( c ) || R::IsDecimalSeparator( c ));
   }

   /*!
    * Returns true if this string can be interpreted as a symbol identifier:
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
      if ( data->string == 0 )
         return false;
      // Symbols start with an alphabetic or underscore character.
      T c = *SearchTrimLeft();
      return !R::IsNull( c ) && R::IsSymbolDigit( c );
   }

   /*!
    * Returns true if this string contains a valid identifier:
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
      const_iterator i = Begin();
      if ( !IsEmpty() && R::IsSymbolDigit( *i ) )
         for ( ;; )
         {
            if ( ++i == End() )
               return true;
            if ( !R::IsSymbolDigit( *i ) && !R::IsDigit( *i ) )
               break;
         }
      pos = pcl::Distance( Begin(), i );
      return false;
   }

   /*!
    * Returns true if this string contains a valid identifier:
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
      distance_type dum;
      return IsValidIdentifier( dum );
   }

   // -------------------------------------------------------------------------

protected:

   /*
    * Reallocate if necessary, ignoring current string contents.
    * Reallocation may happen due either to insufficient available space, or to
    * excessive unused space.
    */
   void MaybeReallocate( size_type newLen )
   {
      if ( data->ShouldReallocate( newLen ) )
      {
         data->Deallocate();
         data->Allocate( newLen );
      }
   }

   /*
    * Auxiliary routines for Insert() and Replace().
    * Index i is updated to ensure 0 <= i <= Length()
    */

   void UninitializedGrow( size_type& i, size_type n )
   {
      size_type len = Length();
      size_type newLen = len+n;
      if ( newLen > len )
      {
         i = pcl::Min( i, len );

         if ( IsUnique() )
         {
            if ( data->capacity < newLen+1 )
               ReallocateToGrow( i, len, n, data->string, true );
            else if ( i < len )
               R::CopyOverlapped( data->string+i+n, data->string+i, len-i );
         }
         else
         {
            Data* oldData = data;
            data = new Data;
            ReallocateToGrow( i, len, n, oldData->string, false );
            if ( !oldData->Detach() )
               delete oldData;
         }

         data->string[newLen] = R::Null();
      }
   }

   void ReallocateToGrow( size_type i, size_type len, size_type n, T* old, bool deleteOld )
   {
      data->Allocate( len+n );
      R::Copy( data->string, old, i );
      if ( i < len )
         R::Copy( data->string+i+n, old+i, len-i );
      if ( deleteOld && old != 0 )
         data->alloc.Deallocate( old );
   }

   /*
    * Auxiliary routines for Trim(), TrimLeft() and TrimRight().
    */

   iterator SearchTrimLeft() const
   {
      PCL_PRECONDITION( data->string != 0 )
      iterator i;
      for ( i = data->string; !R::IsNull( *i ) && R::IsTrimable( *i ); ++i ) {}
      return i;
   }

   iterator SearchTrimRight() const
   {
      PCL_PRECONDITION( data->string != 0 )
      iterator i;
      for ( i = data->string + Length(); i > data->string && R::IsTrimable( i[-1] ); --i ) {}
      return i;
   }

   void Trim( iterator i1, size_type newLen )
   {
      PCL_PRECONDITION( data->string != 0 )

      if ( IsUnique() )
      {
         if ( i1 != data->string )
         {
            T* old = data->string;

            if ( data->ShouldReallocate( newLen ) )
               data->Allocate( newLen );

            R::Copy( data->string, i1, newLen );

            if ( old != data->string )
               data->alloc.Deallocate( old );
         }
      }
      else
      {
         Data* newData = new Data;
         newData->Allocate( newLen );
         R::Copy( newData->string, i1, newLen );

         if ( !data->Detach() )
            delete data;
         data = newData;
      }

      data->string[newLen] = R::Null();
   }

   /*
    * String comparison.
    */
   int CompareString( const T* t, bool caseSensitive, bool localeAware ) const
   {
      return R::Compare( data->string, R::Length( data->string ), t, R::Length( t ),
                         caseSensitive, localeAware );
   }

   /*
    * Character comparison.
    */
   int CompareChar( T c, bool caseSensitive, bool localeAware ) const
   {
      return R::Compare( data->string, 1, &c, 1, caseSensitive, localeAware );
   }

   /*
    * Character replacement routine.
    */
   void ReplaceChar( T c1, T c2, size_type i, size_type n, bool caseSensitive )
   {
      if ( n > 0 && !R::IsNull( c1 ) && !R::IsNull( c2 ) )
      {                 // segment cannot be void and characters cannot be null
         size_type len = Length();
         if ( i < len ) // index must be valid
         {
            n = pcl::Min( n, len-i );  // constrain to replace existing chars.
            if ( caseSensitive )
            {
               for ( iterator p = data->string + i, p1 = p + n; p < p1; ++p )
                  if ( *p == c1 )
                  {
                     SetUnique();
                     *p = c2;
                     for ( ; ++p < p1; )
                        if ( *p == c1 )
                           *p = c2;
                     break;
                  }
            }
            else
            {
               R::ToLowerCase( &c1, 1 );
               for ( iterator p = data->string + i, p1 = p + n; p < p1; ++p )
               {
                  T c = *p;
                  R::ToLowerCase( &c, 1 );
                  if ( c == c1 )
                  {
                     SetUnique();
                     *p = c2;
                     for ( ; ++p < p1; )
                     {
                        c = *p;
                        R::ToLowerCase( &c, 1 );
                        if ( c == c1 )
                           *p = c2;
                     }
                     break;
                  }
               }
            }
         }
      }
   }

   /*
    * String replacement routine.
    */
   void ReplaceString( const T* t1, const T* t2, size_type i, bool caseSensitive )
   {
      // ### TODO: Optimize!!!
      size_type n1 = R::Length( t1 );
      if ( n1 > 0 ) // cannot replace an empty string
         for ( size_type n2 = R::Length( t2 ), p = i; ; )
         {
            p = caseSensitive ? Find( t1, p ) : FindIC( t1, p );
            if ( p == notFound )
               break;
            Delete( p, n1 );
            if ( n2 > 0 )
               Insert( p, t2 );
            p += n2;
         }
   }

protected:

   /*
    * GenericString<T,R,A>::Data
    */
   struct Data : public ReferenceCounter
   {
      T*        string;
      size_type capacity;
      allocator alloc;

      Data() : ReferenceCounter(), string( 0 ), capacity( 0 ), alloc( A() )
      {
      }

      ~Data()
      {
         Deallocate();
      }

      void Allocate( size_type n )
      {
         if ( n > 0 )
            string = alloc.Allocate( capacity = alloc.PagedLength( n+1 ) );
            // +1 = room for a null termination element
      }

      void Deallocate()
      {
         if ( string != 0 )
         {
            alloc.Deallocate( string );
            string = 0;
            capacity = 0;
         }
      }

      bool ShouldReallocate( size_type newLen )
      {
         return capacity < newLen+1 || alloc.PagedLength( newLen+1 ) < capacity >> 1;
      }
   };

   Data* data;
};

// ----------------------------------------------------------------------------

/*!
 * \defgroup generic_string_relational_ops Generic String Relational Operators
 */

// ----------------------------------------------------------------------------

/*!
 * Returns true if two strings \a s1 and \a s2 are equal.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R1, class A1, class R2, class A2> inline
bool operator ==( const GenericString<T,R1,A1>& s1, const GenericString<T,R2,A2>& s2 )
{
   return s1.Compare( s2 ) == 0;
}

/*!
 * Returns true if a string \a s1 is less than a string \a s2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R1, class A1, class R2, class A2> inline
bool operator  <( const GenericString<T,R1,A1>& s1, const GenericString<T,R2,A2>& s2 )
{
   return s1.Compare( s2 ) < 0;
}

/*!
 * Returns true if a string \a s1 is less than or equal to a string \a s2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R1, class A1, class R2, class A2> inline
bool operator <=( const GenericString<T,R1,A1>& s1, const GenericString<T,R2,A2>& s2 )
{
   return s1.Compare( s2 ) <= 0;
}

/*!
 * Returns true if a string \a s1 is greater than a string \a s2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R1, class A1, class R2, class A2> inline
bool operator  >( const GenericString<T,R1,A1>& s1, const GenericString<T,R2,A2>& s2 )
{
   return s1.Compare( s2 ) > 0;
}

/*!
 * Returns true if a string \a s1 is greater than or equal to a string \a s2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R1, class A1, class R2, class A2> inline
bool operator >=( const GenericString<T,R1,A1>& s1, const GenericString<T,R2,A2>& s2 )
{
   return s1.Compare( s2 ) >= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Returns true if a string \a s1 is equal to a null-terminated string \a t2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator ==( const GenericString<T,R,A>& s1, const T* t2 )
{
   return s1.Compare( t2 ) == 0;
}

/*!
 * Returns true if a string \a s1 is less than a null-terminated string \a t2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator  <( const GenericString<T,R,A>& s1, const T* t2 )
{
   return s1.Compare( t2 ) < 0;
}

/*!
 * Returns true if a string \a s1 is less than or equal to a null-terminated
 * string \a t2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator <=( const GenericString<T,R,A>& s1, const T* t2 )
{
   return s1.Compare( t2 ) <= 0;
}

/*!
 * Returns true if a string \a s1 is greater than a null-terminated
 * string \a t2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator  >( const GenericString<T,R,A>& s1, const T* t2 )
{
   return s1.Compare( t2 ) > 0;
}

/*!
 * Returns true if a string \a s1 is greater than or equal to a null-terminated
 * string \a t2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator >=( const GenericString<T,R,A>& s1, const T* t2 )
{
   return s1.Compare( t2 ) >= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Returns true if a null-terminated string \a t1 is equal to a string \a s2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator ==( const T* t1, const GenericString<T,R,A>& s2 )
{
   return s2.Compare( t1 ) == 0;
}

/*!
 * Returns true if a null-terminated string \a t1 is less than a string \a s2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator  <( const T* t1, const GenericString<T,R,A>& s2 )
{
   return s2.Compare( t1 ) > 0;
}

/*!
 * Returns true if a null-terminated string \a t1 is less than or equal to a
 * string \a s2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator <=( const T* t1, const GenericString<T,R,A>& s2 )
{
   return s2.Compare( t1 ) >= 0;
}

/*!
 * Returns true if a null-terminated string \a t1 is greater than a
 * string \a s2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator  >( const T* t1, const GenericString<T,R,A>& s2 )
{
   return s2.Compare( t1 ) < 0;
}

/*!
 * Returns true if a null-terminated string \a t1 is greater than or equal to a
 * string \a s2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator >=( const T* t1, const GenericString<T,R,A>& s2 )
{
   return s2.Compare( t1 ) <= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Returns true if a string \a s1 is equal to a character \a c2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator ==( const GenericString<T,R,A>& s1, T c2 )
{
   return s1.Compare( c2 ) == 0;
}

/*!
 * Returns true if a string \a s1 is less than a character \a c2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator  <( const GenericString<T,R,A>& s1, T c2 )
{
   return s1.Compare( c2 ) < 0;
}

/*!
 * Returns true if a string \a s1 is less than or equal to a character \a c2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator <=( const GenericString<T,R,A>& s1, T c2 )
{
   return s1.Compare( c2 ) <= 0;
}

/*!
 * Returns true if a string \a s1 is greater than a character \a c2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator  >( const GenericString<T,R,A>& s1, T c2 )
{
   return s1.Compare( c2 ) > 0;
}

/*!
 * Returns true if a string \a s1 is greater than or equal to a character \a c2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator >=( const GenericString<T,R,A>& s1, T c2 )
{
   return s1.Compare( c2 ) >= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Returns true if a character \a c1 is equal to a string \a s2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator ==( T c1, const GenericString<T,R,A>& s2 )
{
   return s2.Compare( c1 ) == 0;
}

/*!
 * Returns true if a character \a c1 is less than a string \a s2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator  <( T c1, const GenericString<T,R,A>& s2 )
{
   return s2.Compare( c1 ) > 0;
}

/*!
 * Returns true if a character \a c1 is less than or equal to a string \a s2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator <=( T c1, const GenericString<T,R,A>& s2 )
{
   return s2.Compare( c1 ) >= 0;
}

/*!
 * Returns true if a character \a c1 is greater than a string \a s2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator  >( T c1, const GenericString<T,R,A>& s2 )
{
   return s2.Compare( c1 ) < 0;
}

/*!
 * Returns true if a character \a c1 is greater than or equal to a string \a s2.
 * \ingroup generic_string_relational_ops
 */
template <class T, class R, class A> inline
bool operator >=( T c1, const GenericString<T,R,A>& s2 )
{
   return s2.Compare( c1 ) <= 0;
}

// ----------------------------------------------------------------------------

/*!
 * \class IsoString
 * \brief Eight-bit string (ISO-8859-1 or UTF-8 string).
 *
 * %IsoString derives from an instantiation of GenericString for the \c char
 * type. In the PixInsight platform, %IsoString represents a dynamic ISO-8859-1
 * string, an <em>8-bit Unicode Transformation Format</em> (UTF-8) string, or
 * even a sequence of ASCII characters, depending on the context and type of
 * transformations applied.
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
    * %IsoString iterator.
    */
   typedef string_base::iterator                iterator;

   /*!
    * Constant %IsoString iterator.
    */
   typedef string_base::const_iterator          const_iterator;

   /*!
    * Reverse %IsoString iterator.
    */
   typedef string_base::reverse_iterator        reverse_iterator;

   /*!
    * Constant reverse %IsoString iterator.
    */
   typedef string_base::const_reverse_iterator  const_reverse_iterator;

   /*!
    * Represents a Unicode (UTF-16) string.
    * \note This type must be defined as the same template instantiation used
    * for the String class.
    */
   typedef GenericString<char16_type, CharTraits, StandardAllocator> wstring_base;

   /*!
    * Represents a Unicode (UTF-16) character.
    */
   typedef wstring_base::char_type              wchar_type;

   /*!
    * Unicode (UTF-16) character traits class.
    */
   typedef wstring_base::char_traits            wchar_traits;

   /*!
    * Unicode (UTF-16) string iterator.
    */
   typedef wstring_base::iterator               wchar_iterator;

   /*!
    * Constant Unicode (UTF-16) string iterator.
    */
   typedef wstring_base::const_iterator         const_wchar_iterator;

   // -------------------------------------------------------------------------

   /*!
    * Constructs an empty %IsoString.
    */
   IsoString() : string_base()
   {
   }

   /*!
    * Constructs an %IsoString as a copy of the specified \c string_base
    * string \a s.
    */
   IsoString( const string_base& s ) : string_base( s )
   {
   }

   /*!
    * Constructs an %IsoString as a transformed copy of the specified Unicode
    * string \a s.
    */
   IsoString( const wstring_base& s ) : string_base()
   {
      (void)operator =( s );
   }

   /*!
    * Constructs an %IsoString as a copy of a null-terminated string \a t.
    */
   IsoString( const_iterator t ) : string_base( t )
   {
   }

   /*!
    * Constructs an %IsoString with the \a n first characters of the
    * null-terminated string \a t, starting from its \a i-th character.
    */
   IsoString( const_iterator t, size_type i, size_type n ) : string_base( t, i, n )
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
    * the [i,j[ range.
    */
   IsoString( const_iterator i, const_iterator j ) : string_base( i, j )
   {
   }

   /*!
    * Constructs an %IsoString as a copy of the null-terminated Unicode
    * (UTF-16) string \a t, transformed to ISO-8859-1.
    *
    * If the specified Unicode string \a t contains characters that cannot be
    * converted to ISO-8859-1 characters, the corresponding characters in this
    * string will have undefined values. For conversions from UTF-16 to UTF-8,
    * see String::ToUTF8() and String::UTF16ToUTF8().
    */
   explicit
   IsoString( const_wchar_iterator t ) : string_base()
   {
      (void)operator =( t );
   }

   /*!
    * Constructs an %IsoString with the \a n first characters of the
    * null-terminated Unicode (UTF-16) string \a t, starting from its \a i-th
    * character, transformed to ISO-8859-1.
    *
    * If the specified Unicode string \a t contains characters that cannot be
    * converted to ISO-8859-1 characters, the corresponding characters in this
    * string will have undefined values. For conversions from UTF-16 to UTF-8,
    * see String::ToUTF8() and String::UTF16ToUTF8().
    */
   explicit
   IsoString( const_wchar_iterator t, size_type i, size_type n );

   /*!
    * Constructs an %IsoString with \a n copies of a Unicode (UTF-16)
    * character \a c.
    *
    * If the specified Unicode character \a c cannot be converted to a valid
    * ISO-8859-1 character, the contents of this string will be undefined. For
    * conversions from UTF-16 to UTF-8, see String::ToUTF8() and
    * String::UTF16ToUTF8().
    */
   explicit
   IsoString( wchar_type c, size_type n ) :
   string_base( char_type( pcl::Min( c, wchar_type( 0xffu ) ) ), n )
   {
   }

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
   string_base( qs.isEmpty() ? iterator( 0 ) : iterator( PCL_GET_CHARPTR_FROM_QSTRING( qs ) ) )
   {
   }

   explicit
   IsoString( const QByteArray& qb ) :
   string_base( qb.isEmpty() ? iterator( 0 ) : iterator( PCL_GET_CHARPTR_FROM_QBYTEARRAY( qb ) ) )
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

   /*!
    * Destroys an %IsoString object.
    */
   virtual ~IsoString()
   {
   }

   // -------------------------------------------------------------------------

   /*!
    * Assignment operator. Returns a reference to this object.
    */
   IsoString& operator =( const IsoString& s )
   {
      Assign( s );
      return *this;
   }

   /*!
    * Assigns a copy of the \c string_base string \a s to this string. Returns
    * a reference to this object.
    */
   IsoString& operator =( const string_base& s )
   {
      Assign( s );
      return *this;
   }

   /*!
    * Assigns a copy of the Unicode string \a s (UTF-16) to this string, after
    * converting source UTF-16 characters to ISO-8859-1 characters. Returns a
    * reference to this object.
    *
    * If the specified UTF-16 string contains characters that cannot be
    * converted to ISO-8859-1, the corresponding characters in this string will
    * have undefined values. For conversions from UTF-16 to UTF-8, see
    * String::ToUTF8() and String::UTF16ToUTF8().
    */
   IsoString& operator =( const wstring_base& s )
   {
      return operator =( s.Begin() );
   }

   /*!
    * Assigns a copy of the null-terminated string \a t to this string. Returns
    * a reference to this object.
    */
   IsoString& operator =( const_iterator t )
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
    * this string, after converting source UTF-16 characters to ISO-8859-1
    * characters. Returns a reference to this object.
    *
    * If the specified UTF-16 string contains characters that cannot be
    * converted to ISO-8859-1, the corresponding characters in this string will
    * have undefined values. For conversions from UTF-16 to UTF-8, see
    * String::ToUTF8() and String::UTF16ToUTF8().
    */
   IsoString& operator =( const_wchar_iterator t );

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

   IsoString SubString( size_type i, size_type n = maxPos ) const
   {
      return string_base::SubString( i, n );
   }

   IsoString Left( size_type n ) const
   {
      return string_base::Left( n );
   }

   IsoString Right( size_type n ) const
   {
      return string_base::Right( n );
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

   IsoString LowerCase() const
   {
      return string_base::LowerCase();
   }

   IsoString UpperCase() const
   {
      return string_base::UpperCase();
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
    * Returns a pointer to the internal data array of this string.
    *
    * If this string is empty, this member function returns a pointer to a
    * static, null-terminated empty string (the "" C string).
    *
    * If this string is not unique, it is made unique before returning from
    * this member function.
    */
   char* c_str()
   {
      iterator p = Begin();
      return (p != 0) ? p : const_cast<char*>( "" );
   }

   /*!
    * Returns a pointer to the constant internal data array of this string.
    *
    * If this string is empty, this member function returns a pointer to a
    * static, null-terminated constant empty string (the "" C string).
    */
   const char* c_str() const
   {
      const_iterator p = Begin();
      return (p != 0) ? p : "";
   }

   /*!
    * Returns a pointer to the internal data array of this string,
    * reinterpreted as a pointer to unsigned 8-bit integers (uint8*).
    *
    * If this string is empty, this member function returns a pointer to a
    * static, null-terminated empty string (the "" C string) reinterpreted as a
    * pointer to an uint8.
    *
    * If this string is not unique, it is made unique before returning from
    * this member function.
    */
   uint8* uc_str()
   {
      return reinterpret_cast<uint8*>( c_str() );
   }

   /*!
    * Returns a pointer to the constant internal data array of this string,
    * reinterpreted as a pointer to constant unsigned 8-bit integers
    * (const uint8*).
    *
    * If this string is empty, this member function returns a pointer to a
    * static, null-terminated constant empty string (the "" C string).
    * reinterpreted as a pointer to a constant uint8.
    */
   const uint8* uc_str() const
   {
      return reinterpret_cast<const uint8*>( c_str() );
   }

   /*!
    * Copies characters from this string to a null-terminated string.
    *
    * \param[out] dst   Destination character array.
    *
    * \param available  The total number of characters that can be stored at
    *                   the \a dst array, including a null terminating
    *                   character.
    *
    * \param i    Index of the first character to be copied.
    *
    * This function copies at most \a available-1 characters from this string
    * to the \a dst array. Then a null character is appended to \a dst.
    */
   void c_copy( char* dst, size_type available, size_type i = 0 ) const;

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
   IsoString& Format( const char* fmt, ... )
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
   IsoString& AppendFormat( const char* fmt, ... )
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
   int VFormat( const char* fmt, va_list paramList );

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
   int AppendVFormat( const char* fmt, va_list paramList );

   // -------------------------------------------------------------------------

   /*!
    * Returns a UTF-16 string with a converted copy of this %IsoString object.
    *
    * This function assumes that this %IsoString object contains only valid
    * ISO-8859-1 characters. To perform UTF-8 to UTF-16 conversions, see
    * UTF8ToUTF16() and MBSToWCS().
    *
    * \sa UTF8ToUTF16(), MBSToWCS()
    */
   wstring_base ToString() const;

   /*!
    * Returns an UTF-16 string with a representation of a subset of \a n
    * contiguous UTF-8 characters from this %IsoString object, starting at the
    * \a i-th character.
    *
    * \sa ToString(), MBSToWCS()
    */
   wstring_base UTF8ToUTF16( size_type i = 0, size_type n = maxPos ) const; // implemented inline after String

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
   wstring_base MBSToWCS() const;

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
    * Otherwise, this function throws a ParseError exception.
    *
    * \sa TryToBool( bool& )
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
    * \sa TryToFloat( float& )
    */
   float ToFloat() const
   {
      return float( ToDouble() );
   }

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
    * \sa TryToDouble( double& )
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
    * \sa TryToInt( int& )
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
    * \sa TryToInt( int&, int )
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
    * \sa ToInt( int )
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
    * \sa TryToUInt( unsigned& )
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
    * \sa TryToUInt( unsigned&, int )
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
    * \sa ToUInt( int )
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
    * \sa TryToInt64( long long& )
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
    * \sa TryToInt64( long long&, int )
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
    * \sa ToInt64( int )
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
    * \sa TryToUInt64( long long& )
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
    * \sa TryToUInt64( unsigned long long&, int )
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
    * \sa ToUInt64( int )
    */
   bool TryToUInt64( unsigned long long& value, int base ) const;

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
   IsoString s = s1;
   s.Append( s2 );
   return s;
}

/*!
 * Returns a string with the concatenation of a string \a s1 and a
 * null-terminated string \a t2.
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( const IsoString::string_base& s1, const IsoString::const_iterator& t2 )
{
   IsoString s = s1;
   s.Append( t2 );
   return s;
}

/*!
 * Returns a string with the concatenation of a null-terminated string \a t1
 * and a string \a s2.
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( const IsoString::const_iterator& t1, const IsoString::string_base& s2 )
{
   IsoString s = s2;
   s.Prepend( t1 );
   return s;
}

/*!
 * Returns a string with the concatenation of a string \a s1 and a single
 * character \a c2.
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( const IsoString::string_base& s1, const IsoString::char_type& c2 )
{
   IsoString s = s1;
   s.Append( c2 );
   return s;
}

/*!
 * Returns a string with the concatenation of a single character \a c1 and a
 * string \a s2.
 * \ingroup isostring_concatenation_ops
 */
inline IsoString operator +( const IsoString::char_type& c1, const IsoString::string_base& s2 )
{
   IsoString s = s2;
   s.Prepend( c1 );
   return s;
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
 * Appends a null-terminated string \a t2 to a string \a s1. Returns a
 * reference to the left-hand operand string \a s1
 * \ingroup isostring_concatenation_ops
 */
inline IsoString& operator <<( IsoString& s1, IsoString::const_iterator t2 )
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
 * It represents a dynamic <em>16-bit Unicode Transformation Format</em>
 * (UTF-16) string of characters in the PixInsight platform.
 *
 * \sa IsoString
 */
class PCL_CLASS String : public GenericString<char16_type, CharTraits, StandardAllocator>
{
public:

   /*!
    * Base class of %String.
    */
   typedef GenericString<char16_type, CharTraits, StandardAllocator> string_base;

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
    * %String iterator.
    */
   typedef string_base::iterator                iterator;

   /*!
    * Constant %String iterator.
    */
   typedef string_base::const_iterator          const_iterator;

   /*!
    * Reverse %String iterator.
    */
   typedef string_base::reverse_iterator        reverse_iterator;

   /*!
    * Constant reverse %String iterator.
    */
   typedef string_base::const_reverse_iterator  const_reverse_iterator;

   /*!
    * Represents an 8-bit string (ISO-8859-1 or UTF-8).
    * \note This type must be defined as the same template instantiation used
    * for the IsoString class.
    */
   typedef GenericString<char, IsoCharTraits, StandardAllocator>  sstring_base;

   /*!
    * Represents an 8-bit character (ISO-8859-1 or UTF-8).
    */
   typedef sstring_base::char_type              schar_type;

   /*!
    * 8-bit character traits class.
    */
   typedef sstring_base::char_traits            schar_traits;

   /*!
    * 8-bit string iterator.
    */
   typedef sstring_base::iterator               schar_iterator;

   /*!
    * Constant 8-bit string iterator.
    */
   typedef sstring_base::const_iterator         const_schar_iterator;

   // -------------------------------------------------------------------------

   /*!
    * Constructs an empty %String.
    */
   String() : string_base()
   {
   }

   /*!
    * Constructs a %String as a copy of the specified \c string_base
    * string \a s.
    */
   String( const string_base& s ) : string_base( s )
   {
   }

   /*!
    * Constructs a %String as a transformed copy of the specified 8-bit,
    * ISO-8859-1 string \a s.
    */
   String( const sstring_base& s ) : string_base()
   {
      Assign( s.Begin() );
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
    * Constructs a %String with \a n copies of a character \a c.
    */
   String( char_type c, size_type n = 1 ) : string_base( c, n )
   {
   }

   /*!
    * Constructs a %String with a copy of the character sequence defined by
    * the [i,j[ range.
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
   String( wchar_t c, size_type n = 1 ) : string_base( char_type( c ), n )
   {
   }

   /*!
    * Constructs a %String as a transformed copy of the null-terminated 8-bit,
    * ISO-8859-1 string \a t.
    */
   String( const_schar_iterator t ) : string_base()
   {
      Assign( t );
   }

   /*!
    * Constructs a %String with the \a n first characters of the
    * null-terminated, 8-bit ISO-8859-1 string \a t, starting from its \a i-th
    * character.
    */
   String( const_schar_iterator t, size_type i, size_type n ) : string_base()
   {
      Assign( t, i, n );
   }

   /*!
    * Constructs a %String with \a n copies of an ISO-8859-1 character \a c.
    */
   String( schar_type c, size_type n = 1 ) : string_base( char_type( c ), n )
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
   string_base( qs.isEmpty() ? iterator( 0 ) : iterator( PCL_GET_CHAR16PTR_FROM_QSTRING( qs ) ) )
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

   /*!
    * Destroys a %String object.
    */
   virtual ~String()
   {
   }

   // -------------------------------------------------------------------------

   /*!
    * Assignment operator. Returns a reference to this object.
    */
   String& operator =( const String& s )
   {
      string_base::Assign( s );
      return *this;
   }

   /*!
    * Assigns a copy of the \c string_base string \a s to this string. Returns
    * a reference to this object.
    */
   String& operator =( const string_base& s )
   {
      string_base::Assign( s );
      return *this;
   }

   /*!
    * Assigns a copy of the 8-bit ISO-8859-1 string \a s to this string.
    * Returns a reference to this object.
    */
   String& operator =( const sstring_base& s )
   {
      Assign( s.Begin() );
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
    * Assigns a copy of the null-terminated 8-bit ISO-8859-1 string \a t to
    * this string. Returns a reference to this object.
    */
   String& operator =( const_schar_iterator t )
   {
      Assign( t );
      return *this;
   }

   /*!
    * Assigns a single copy of an ISO-8859-1 character \a c to this string.
    * Returns a reference to this object.
    */
   String& operator =( schar_type c )
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
    * Assigns a sequence of characters defined by the range [i,j[ to this
    * string.
    */
   void Assign( const_iterator i, const_iterator j )
   {
      string_base::Assign( i, j );
   }

   /*!
    * Assigns a contiguous segment of \a n characters of a null-terminated
    * string \a t, starting from its \a i-th character, to this string.
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
    * Assigns a null-terminated 8-bit ISO-8859-1 string \a t to this string.
    */
   void Assign( const_schar_iterator t );

   /*!
    * Assigns a contiguous segment of \a n characters of a null-terminated
    * 8-bit ISO-8859-1 string \a t, starting from its \a i-th character, to
    * this string.
    */
   void Assign( const_schar_iterator t, size_type i, size_type n );

   /*!
    * Assigns \a n copies of a ISO-8859-1 character \a c to this string.
    */
   void Assign( schar_type c, size_type n = 1 )
   {
      string_base::Assign( char_type( c ), n );
   }

   // -------------------------------------------------------------------------

   void Insert( size_type i, const String& s )
   {
      string_base::Insert( i, s );
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

   void Insert( size_type i, const_schar_iterator t )
   {
      string_base::Insert( i, String( t ) );
   }

   void Insert( size_type i, schar_type c, size_type n = 1 )
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

   void Append( const_schar_iterator t )
   {
      string_base::Append( String( t ) );
   }

   String& operator +=( const_schar_iterator t )
   {
      Append( t );
      return *this;
   }

   void Append( schar_type c, size_type n = 1 )
   {
      string_base::Append( char_type( c ), n );
   }

   String& operator +=( schar_type c )
   {
      Append( c );
      return *this;
   }

   void Add( const String& s )
   {
      Append( s );
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

   void Add( const_schar_iterator t )
   {
      Append( t );
   }

   void Add( schar_type c, size_type n = 1 )
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

   void Prepend( const_schar_iterator t )
   {
      string_base::Prepend( String( t ) );
   }

   String& operator -=( const_schar_iterator t )
   {
      Prepend( t );
      return *this;
   }

   void Prepend( schar_type c, size_type n = 1 )
   {
      string_base::Prepend( String( c, n ) );
   }

   String& operator -=( schar_type c )
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

   void Replace( size_type i, size_type n, const_schar_iterator t )
   {
      string_base::Replace( i, n, String( t ) );
   }

   void Replace( size_type i, size_type n, schar_type c, size_type nc = 1 )
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

   void ReplaceChar( schar_type c1, schar_type c2, size_type i = 0, size_type n = maxPos )
   {
      string_base::ReplaceChar( char_type( c1 ), char_type( c2 ), i, n );
   }

   void ReplaceCharIC( schar_type c1, schar_type c2, size_type i = 0, size_type n = maxPos )
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

   void ReplaceString( const_schar_iterator t1, const_schar_iterator t2, size_type i = 0 )
   {
      string_base::ReplaceString( String( t1 ), String( t2 ), i );
   }

   void ReplaceStringIC( const_schar_iterator t1, const_schar_iterator t2, size_type i = 0 )
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

   void DeleteChar( schar_type c, size_type i = 0 )
   {
      string_base::DeleteChar( char_type( c ), i );
   }

   void DeleteCharIC( schar_type c, size_type i = 0 )
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

   void DeleteString( const_schar_iterator t, size_type i = 0 )
   {
      string_base::DeleteString( String( t ), i );
   }

   void DeleteStringIC( const_schar_iterator t, size_type i = 0 )
   {
      string_base::DeleteStringIC( String( t ), i );
   }

   // -------------------------------------------------------------------------

   bool BeginsWith( const String& s ) const
   {
      return string_base::BeginsWith( s );
   }

   bool BeginsWith( const_iterator t ) const
   {
      return string_base::BeginsWith( t );
   }

   bool BeginsWith( char_type c ) const
   {
      return string_base::BeginsWith( c );
   }

   bool BeginsWithIC( const String& s ) const
   {
      return string_base::BeginsWithIC( s );
   }

   bool BeginsWithIC( const_iterator t ) const
   {
      return string_base::BeginsWithIC( t );
   }

   bool BeginsWithIC( char_type c ) const
   {
      return string_base::BeginsWithIC( c );
   }

   bool BeginsWith( const wchar_t* t ) const
   {
#ifdef __PCL_WINDOWS
      return string_base::BeginsWith( reinterpret_cast<const_iterator>( t ) );
#else
      return string_base::BeginsWith( String( t ) );
#endif
   }

   bool BeginsWith( wchar_t c ) const
   {
      return string_base::BeginsWith( char_type( c ) );
   }

   bool BeginsWithIC( const wchar_t* t ) const
   {
#ifdef __PCL_WINDOWS
      return string_base::BeginsWithIC( reinterpret_cast<const_iterator>( t ) );
#else
      return string_base::BeginsWithIC( String( t ) );
#endif
   }

   bool BeginsWithIC( wchar_t c ) const
   {
      return string_base::BeginsWithIC( char_type( c ) );
   }

   bool BeginsWith( const_schar_iterator t ) const
   {
      return string_base::BeginsWith( String( t ) );
   }

   bool BeginsWith( schar_type c ) const
   {
      return string_base::BeginsWith( char_type( c ) );
   }

   bool BeginsWithIC( const_schar_iterator t ) const
   {
      return string_base::BeginsWithIC( String( t ) );
   }

   bool BeginsWithIC( schar_type c ) const
   {
      return string_base::BeginsWithIC( char_type( c ) );
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

   bool EndsWith( const_schar_iterator t ) const
   {
      return string_base::EndsWith( String( t ) );
   }

   bool EndsWith( schar_type c ) const
   {
      return string_base::EndsWith( char_type( c ) );
   }

   bool EndsWithIC( const_schar_iterator t ) const
   {
      return string_base::EndsWithIC( String( t ) );
   }

   bool EndsWithIC( schar_type c ) const
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

   size_type FindFirst( const_schar_iterator t, size_type i = 0 ) const
   {
      return string_base::FindFirst( String( t ), i );
   }

   size_type FindFirst( schar_type c, size_type i = 0 ) const
   {
      return string_base::FindFirst( char_type( c ), i );
   }

   size_type FindFirstIC( const_schar_iterator t, size_type i = 0 ) const
   {
      return string_base::FindFirstIC( String( t ), i );
   }

   size_type FindFirstIC( schar_type c, size_type i = 0 ) const
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

   size_type Find( const_schar_iterator t, size_type i = 0 ) const
   {
      return FindFirst( t, i );
   }

   size_type Find( schar_type c, size_type i = 0 ) const
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

   size_type FindIC( const_schar_iterator t, size_type i = 0 ) const
   {
      return FindFirstIC( t, i );
   }

   size_type FindIC( schar_type c, size_type i = 0 ) const
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

   size_type FindLast( const_schar_iterator t, size_type r = maxPos ) const
   {
      return string_base::FindLast( String( t ), r );
   }

   size_type FindLast( schar_type c, size_type r = maxPos ) const
   {
      return string_base::FindLast( char_type( c ), r );
   }

   size_type FindLastIC( const_schar_iterator t, size_type r = maxPos ) const
   {
      return string_base::FindLastIC( String( t ), r );
   }

   size_type FindLastIC( schar_type c, size_type r = maxPos ) const
   {
      return string_base::FindLastIC( char_type( c ), r );
   }

   // -------------------------------------------------------------------------

   bool Has( const String& s ) const
   {
      return string_base::Has( s );
   }

   bool Has( const_iterator t ) const
   {
      return string_base::Has( t );
   }

   bool Has( char_type c ) const
   {
      return string_base::Has( c );
   }

   bool HasIC( const String& s ) const
   {
      return string_base::HasIC( s );
   }

   bool HasIC( const_iterator t ) const
   {
      return string_base::HasIC( t );
   }

   bool HasIC( char_type c ) const
   {
      return string_base::HasIC( c );
   }

   bool Has( const wchar_t* t ) const
   {
#ifdef __PCL_WINDOWS
      return string_base::Has( reinterpret_cast<const_iterator>( t ) );
#else
      return string_base::Has( String( t ) );
#endif
   }

   bool Has( wchar_t c ) const
   {
      return string_base::Has( char_type( c ) );
   }

   bool HasIC( const wchar_t* t ) const
   {
#ifdef __PCL_WINDOWS
      return string_base::HasIC( reinterpret_cast<const_iterator>( t ) );
#else
      return string_base::HasIC( String( t ) );
#endif
   }

   bool HasIC( wchar_t c ) const
   {
      return string_base::HasIC( char_type( c ) );
   }

   bool Has( const_schar_iterator t ) const
   {
      return string_base::Has( String( t ) );
   }

   bool Has( schar_type c ) const
   {
      return string_base::Has( char_type( c ) );
   }

   bool HasIC( const_schar_iterator t ) const
   {
      return string_base::HasIC( String( t ) );
   }

   bool HasIC( schar_type c ) const
   {
      return string_base::HasIC( char_type( c ) );
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

   int Compare( const_schar_iterator t, bool caseSensitive = true, bool localeAware = true ) const
   {
      return string_base::Compare( String( t ), caseSensitive, localeAware );
   }

   int Compare( schar_type c, bool caseSensitive = true, bool localeAware = true ) const
   {
      return string_base::Compare( char_type( c ), caseSensitive, localeAware );
   }

   int CompareIC( const_schar_iterator t, bool localeAware = true ) const
   {
      return string_base::CompareIC( String( t ), localeAware );
   }

   int CompareIC( schar_type c, bool localeAware = true ) const
   {
      return string_base::CompareIC( char_type( c ), localeAware );
   }

   // -------------------------------------------------------------------------

   String SubString( size_type i, size_type n = maxPos ) const
   {
      return string_base::SubString( i, n );
   }

   String Left( size_type n ) const
   {
      return string_base::Left( n );
   }

   String Right( size_type n ) const
   {
      return string_base::Right( n );
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

   String LowerCase() const
   {
      return string_base::LowerCase();
   }

   String UpperCase() const
   {
      return string_base::UpperCase();
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
    * Returns a pointer to the internal data array of this string.
    *
    * If this string is empty, this member function returns a pointer to a
    * static, null-terminated empty string.
    *
    * If this string is not unique, it is made unique before returning from
    * this member function.
    */
   char16_type* c_str()
   {
      static char16_type the_null_char16_type_char = 0;
      iterator p = Begin();
      return (p != 0) ? p : &the_null_char16_type_char;
   }

   /*!
    * Returns a pointer to the constant internal data array of this string.
    *
    * If this string is empty, this member function returns a pointer to a
    * static, null-terminated constant empty string.
    */
   const char16_type* c_str() const
   {
      static const char16_type the_null_char16_type_const_char = 0;
      const_iterator p = Begin();
      return (p != 0) ? p : &the_null_char16_type_const_char;
   }

   /*!
    * Copies characters from this string to a null-terminated string.
    *
    * \param[out] dst   Destination character array.
    *
    * \param available  The total number of characters that can be stored at
    *                   the \a dst array, including a null terminating
    *                   character.
    *
    * \param i    Index of the first character to be copied.
    *
    * This function copies at most \a available-1 characters from this string
    * to the \a dst array. Then a null character is appended to \a dst.
    */
   void c_copy( char16_type* dst, size_type available, size_type i = 0 ) const;

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
   String& Format( const char* fmt, ... )
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
   String& AppendFormat( const char* fmt, ... )
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
   int VFormat( const char* fmt, va_list paramList )
   {
      IsoString s;
      int count = s.VFormat( fmt, paramList );
      Assign( String( s ) );
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
   int AppendVFormat( const char* fmt, va_list paramList )
   {
      IsoString s;
      int count = s.VFormat( fmt, paramList );
      Append( String( s ) );
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
   static string_base UTF8ToUTF16( const char* string, size_type i = 0, size_type n = maxPos );

   /*!
    * Conversion of an UTF-16 substring to an UTF-8 string.
    *
    * Converts a contiguous sequence of \a n characters starting at the \a i-th
    * position of the specified null-terminated UTF-16 \a string. Returns the
    * resulting UTF-8 string.
    *
    * \sa UTF8ToUTF16(), UTF16ToUTF32(), UTF32ToUTF16()
    */
   static sstring_base UTF16ToUTF8( const char16_type* string, size_type i = 0, size_type n = maxPos );

   /*!
    * Conversion of an UTF-16 substring to an UTF-32 string.
    *
    * Converts a contiguous sequence of \a n characters starting at the \a i-th
    * position of the specified null-terminated UTF-16 \a string. Returns the
    * resulting UTF-32 string.
    *
    * \sa UTF8ToUTF16(), UTF16ToUTF8(), UTF32ToUTF16()
    */
   static Array<uint32> UTF16ToUTF32( const char16_type* string, size_type i = 0, size_type n = maxPos );

   /*!
    * Conversion of an UTF-32 substring to an UTF-16 string.
    *
    * Converts a contiguous sequence of \a n characters starting at the \a i-th
    * position of the specified null-terminated UTF-32 \a string. Returns the
    * resulting UTF-16 string.
    *
    * \sa UTF8ToUTF16(), UTF16ToUTF8(), UTF16ToUTF32()
    */
   static string_base UTF32ToUTF16( const uint32* string, size_type i = 0, size_type n = maxPos );

   // -------------------------------------------------------------------------

   /*!
    * Returns an 8-bit, ISO-8859-1 string with a converted copy of this %String
    * object. Characters in this string that cannot be converted to ISO-8859-1
    * (that is, characters with code points larger than 0x100) will have
    * undefined values in the returned string. Undefined values are represented
    * with question mark characters (?).
    *
    * \sa ToUTF8(), ToMBS(), ToUTF32(), ToASCII(), To7BitASCII()
    */
   sstring_base ToIsoString() const;

   /*!
    * Returns a 7-bit ASCII string with a converted copy of this %String
    * object. Characters in this string that cannot be converted to the 7-bit
    * ASCII set (that is, characters with code points larger than 0x80) will
    * have undefined values in the returned string. Undefined values are
    * represented with question mark characters (?).
    *
    * \sa ToIsoString(), ToUTF8(), ToMBS(), ToUTF32(), ToASCII()
    */
   sstring_base To7BitASCII() const;

   /*!
    * Returns an 8-bit string with an UTF-8 representation of a subset of \a n
    * contiguous UTF-16 characters from this %String object, starting at the
    * \a i-th character.
    *
    * \sa ToMBS(), ToLocal8Bit(), ToIsoString(), ToUTF32()
    */
   sstring_base ToUTF8( size_type i = 0, size_type n = maxPos ) const
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
   sstring_base ToMBS() const;

   /*!
    * Returns a copy of this Unicode string converted to an 8-bit
    * locale-dependent string. On UNIX/Linux systems (FreeBSD, Linux, Mac OS X)
    * this function is equivalent to ToUTF8(). On Windows, this function
    * returns ToMBS().
    *
    * \sa ToUTF8(), ToMBS();
    */
   sstring_base ToLocal8Bit() const;

   /*!
    * Returns a copy of a contiguous segment of \a n characters of this string,
    * starting from its \a i-th character, as a dynamic array of \c wchar_t. A
    * null terminating character (L'\\0' specifically) is always appended to the
    * resulting array.
    *
    * Depending on the platform, the \c wchar_t type may be 16-bit wide
    * (Windows) or 32-bit wide (Linux, Mac OS X). The char16_type used by
    * %String is always a 16-bit character (UTF-16) on all supported platforms.
    *
    * This member function provides a platform-independent way to obtain the
    * contents of a %String object as a standard null-terminated string of
    * \c wchar_t characters.
    *
    * On platforms where wchar_t occupies four bytes (Linux), this function
    * assumes that this %String object contains no surrogates. For a
    * generalized conversion from UTF-16 to UTF-32, see ToUTF32().
    *
    * \sa ToUTF32(), ToUTF8(), ToMBS(), ToIsoString()
    */
   Array<wchar_t> ToWCharArray( size_type i = 0, size_type n = maxPos ) const;

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
    * Otherwise, this function throws a ParseError exception.
    *
    * \sa TryToBool( bool& )
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
    * \sa TryToFloat( float& )
    */
   float ToFloat() const
   {
      return float( ToDouble() );
   }

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
    * \sa TryToDouble( double& )
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
    * \sa TryToInt( int& )
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
    * \sa TryToInt( int&, int )
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
    * \sa ToInt( int )
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
    * \sa TryToUInt( unsigned& )
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
    * \sa TryToUInt( unsigned&, int )
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
    * \sa ToUInt( int )
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
    * \sa TryToInt64( long long& )
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
    * \sa TryToInt64( long long&, int )
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
    * \sa ToInt64( int )
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
    * \sa TryToUInt64( long long& )
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
    * \sa TryToUInt64( unsigned long long&, int )
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
    * \sa ToUInt64( int )
    */
   bool TryToUInt64( unsigned long long& value, int base ) const;
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

inline IsoString::wstring_base IsoString::UTF8ToUTF16( size_type i, size_type n ) const
{
   return String::UTF8ToUTF16( Begin(), i, n );
}

// ----------------------------------------------------------------------------

/*!
 * \defgroup string_concatenation_ops String Concatenation Operators
 */

/*!
 * Returns a string with the concatenation of two strings \a s1 and \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, const String::string_base& s2 )
{
   String s = s1;
   s.Append( s2 );
   return s;
}

/*!
 * Returns a string with the concatenation of a string \a s1 and a
 * null-terminated string \a t2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, String::const_iterator t2 )
{
   String s = s1;
   s.Append( t2 );
   return s;
}

/*!
 * Returns a string with the concatenation of a null-terminated string \a t1
 * and a string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::const_iterator t1, const String::string_base& s2 )
{
   String s = s2;
   s.Prepend( t1 );
   return s;
}

/*!
 * Returns a string with the concatenation of a string \a s1 and a single
 * character \a c2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, String::char_type c2 )
{
   String s = s1;
   s.Append( c2 );
   return s;
}

/*!
 * Returns a string with the concatenation of a single character \a c1 and a
 * string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::char_type c1, const String::string_base& s2 )
{
   String s = s2;
   s.Prepend( c1 );
   return s;
}

/*!
 * Returns a string with the concatenation of a string \a s1 and a
 * null-terminated string \a t2 of \c wchar_t.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, const wchar_t* t2 )
{
   String s = s1;
   s.Append( t2 );
   return s;
}

/*!
 * Returns a string with the concatenation of a null-terminated string \a t1 of
 * \c wchar_t and a string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const wchar_t* t1, const String::string_base& s2 )
{
   String s = s2;
   s.Prepend( t1 );
   return s;
}

/*!
 * Returns a string with the concatenation of a string \a s1 and a single
 * \c wchar_t character \a c2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, wchar_t c2 )
{
   String s = s1;
   s.Append( c2 );
   return s;
}

/*!
 * Returns a string with the concatenation of a single \c wchar_t character
 * \a c1 and a string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( wchar_t c1, const String::string_base& s2 )
{
   String s = s2;
   s.Prepend( c1 );
   return s;
}

/*!
 * Returns a string with the concatenation of a Unicode string \a s1 and an
 * 8-bit string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, const String::sstring_base& s2 )
{
   String s = s1;
   s.Append( s2.Begin() );
   return s;
}

/*!
 * Returns a string with the concatenation of an 8-bit string \a s1 and a
 * Unicode string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::sstring_base& s1, const String::string_base& s2 )
{
   String s = s2;
   s.Prepend( s1.Begin() );
   return s;
}

/*!
 * Returns a string with the concatenation of a Unicode string \a s1 and a
 * null-terminated 8-bit string \a t2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, String::const_schar_iterator t2 )
{
   String s = s1;
   s.Append( t2 );
   return s;
}

/*!
 * Returns a Unicode string with the concatenation of a null-terminated 8-bit
 * string \a t1 and a Unicode string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::const_schar_iterator t1, const String::string_base& s2 )
{
   String s = s2;
   s.Prepend( t1 );
   return s;
}

/*!
 * Returns a Unicode string with the concatenation of a Unicode string \a s1
 * and a single ISO-8859-1 character \a c2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( const String::string_base& s1, String::schar_type c2 )
{
   String s = s1;
   s.Append( c2 );
   return s;
}

/*!
 * Returns a Unicode string with the concatenation of a single ISO-8859-1
 * character \a c1 and a Unicode string \a s2.
 * \ingroup string_concatenation_ops
 */
inline String operator +( String::schar_type c1, const String::string_base& s2 )
{
   String s = s2;
   s.Prepend( c1 );
   return s;
}

// ----------------------------------------------------------------------------

/*!
 * Appends a string \a s2 to a string \a s1. Returns a reference to the
 * left-hand operand string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String& s1, const String::string_base& s2 )
{
   s1.Append( s2 );
   return s1;
}

/*!
 * Appends a null-terminated string \a t2 to a string \a s1. Returns a
 * reference to the left-hand operand string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String& s1, String::const_iterator& t2 )
{
   s1.Append( t2 );
   return s1;
}

/*!
 * Appends a single character \a c2 to a string \a s1. Returns a reference to
 * the left-hand operand string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String& s1, String::char_type c2 )
{
   s1.Append( c2 );
   return s1;
}

/*!
 * Appends a null-terminated string \a t2 of \c wchar_t to a string \a s1.
 * Returns a reference to the left-hand operand string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String& s1, const wchar_t* t2 )
{
   s1.Append( t2 );
   return s1;
}

/*!
 * Appends a single \c wchar_t character \a c2 to a string \a s1. Returns a
 * reference to the left-hand operand string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String& s1, wchar_t c2 )
{
   s1.Append( c2 );
   return s1;
}

/*!
 * Appends an 8-bit string \a s2 to a Unicode string \a s1. Returns a reference
 * to the left-hand operand Unicode string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String& s1, const String::sstring_base& s2 )
{
   s1.Append( s2 );
   return s1;
}

/*!
 * Appends a null-terminated 8-bit string \a t2 to a Unicode string \a s1.
 * Returns a reference to the left-hand operand Unicode string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String& s1, String::const_schar_iterator t2 )
{
   s1.Append( t2 );
   return s1;
}

/*!
 * Appends a single ISO-8859-1 character \a c2 to a Unicode string \a s1.
 * Returns a reference to the left-hand operand Unicode string \a s1.
 * \ingroup string_concatenation_ops
 */
inline String& operator <<( String& s1, String::schar_type c2 )
{
   s1.Append( c2 );
   return s1;
}

// ----------------------------------------------------------------------------

/*!
 * \defgroup string_relational_operators String Relational Operators
 */

/*!
 * Equality operator.
 * \ingroup string_relational_operators
 */
inline bool operator ==( const String& s1, const wchar_t* t2 )
{
   return s1.Compare( t2 ) == 0;
}

/*!
 * <em>Less than</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator  <( const String& s1, const wchar_t* t2 )
{
   return s1.Compare( t2 ) < 0;
}

/*!
 * <em>Less than or equal</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator <=( const String& s1, const wchar_t* t2 )
{
   return s1.Compare( t2 ) <= 0;
}

/*!
 * <em>Greater than</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator  >( const String& s1, const wchar_t* t2 )
{
   return s1.Compare( t2 ) > 0;
}

/*!
 * <em>Greater than or equal</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator >=( const String& s1, const wchar_t* t2 )
{
   return s1.Compare( t2 ) >= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Equality operator.
 * \ingroup string_relational_operators
 */
inline bool operator ==( const wchar_t* t1, const String& s2 )
{
   return s2.Compare( t1 ) == 0;
}

/*!
 * <em>Less than</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator  <( const wchar_t* t1, const String& s2 )
{
   return s2.Compare( t1 ) > 0;
}

/*!
 * <em>Less than or equal</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator <=( const wchar_t* t1, const String& s2 )
{
   return s2.Compare( t1 ) >= 0;
}

/*!
 * <em>Greater than</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator  >( const wchar_t* t1, const String& s2 )
{
   return s2.Compare( t1 ) < 0;
}

/*!
 * <em>Greater than or equal</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator >=( const wchar_t* t1, const String& s2 )
{
   return s2.Compare( t1 ) <= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Equality operator.
 * \ingroup string_relational_operators
 */
inline bool operator ==( const String& s1, wchar_t c2 )
{
   return s1.Compare( c2 ) == 0;
}

/*!
 * <em>Less than</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator  <( const String& s1, wchar_t c2 )
{
   return s1.Compare( c2 ) < 0;
}

/*!
 * <em>Less than or equal</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator <=( const String& s1, wchar_t c2 )
{
   return s1.Compare( c2 ) <= 0;
}

/*!
 * <em>Greater than</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator  >( const String& s1, wchar_t c2 )
{
   return s1.Compare( c2 ) > 0;
}

/*!
 * <em>Greater than or equal</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator >=( const String& s1, wchar_t c2 )
{
   return s1.Compare( c2 ) >= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Equality operator.
 * \ingroup string_relational_operators
 */
inline bool operator ==( wchar_t c1, const String& s2 )
{
   return s2.Compare( c1 ) == 0;
}

/*!
 * <em>Less than</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator  <( wchar_t c1, const String& s2 )
{
   return s2.Compare( c1 ) > 0;
}

/*!
 * <em>Less than or equal</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator <=( wchar_t c1, const String& s2 )
{
   return s2.Compare( c1 ) >= 0;
}

/*!
 * <em>Greater than</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator  >( wchar_t c1, const String& s2 )
{
   return s2.Compare( c1 ) < 0;
}

/*!
 * <em>Greater than or equal</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator >=( wchar_t c1, const String& s2 )
{
   return s2.Compare( c1 ) <= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Equality operator.
 * \ingroup string_relational_operators
 */
inline bool operator ==( const String& s1, String::const_schar_iterator t2 )
{
   return s1.Compare( t2 ) == 0;
}

/*!
 * <em>Less than</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator  <( const String& s1, String::const_schar_iterator t2 )
{
   return s1.Compare( t2 ) < 0;
}

/*!
 * <em>Less than or equal</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator <=( const String& s1, String::const_schar_iterator t2 )
{
   return s1.Compare( t2 ) <= 0;
}

/*!
 * <em>Greater than</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator  >( const String& s1, String::const_schar_iterator t2 )
{
   return s1.Compare( t2 ) > 0;
}

/*!
 * <em>Greater than or equal</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator >=( const String& s1, String::const_schar_iterator t2 )
{
   return s1.Compare( t2 ) >= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Equality operator.
 * \ingroup string_relational_operators
 */
inline bool operator ==( String::const_schar_iterator t1, const String& s2 )
{
   return s2.Compare( t1 ) == 0;
}

/*!
 * <em>Less than</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator  <( String::const_schar_iterator t1, const String& s2 )
{
   return s2.Compare( t1 ) > 0;
}

/*!
 * <em>Less than or equal</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator <=( String::const_schar_iterator t1, const String& s2 )
{
   return s2.Compare( t1 ) >= 0;
}

/*!
 * <em>Greater than</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator  >( String::const_schar_iterator t1, const String& s2 )
{
   return s2.Compare( t1 ) < 0;
}

/*!
 * <em>Greater than or equal</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator >=( String::const_schar_iterator t1, const String& s2 )
{
   return s2.Compare( t1 ) <= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Equality operator.
 * \ingroup string_relational_operators
 */
inline bool operator ==( const String& s1, String::schar_type c2 )
{
   return s1.Compare( c2 ) == 0;
}

/*!
 * <em>Less than</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator  <( const String& s1, String::schar_type c2 )
{
   return s1.Compare( c2 ) < 0;
}

/*!
 * <em>Less than or equal</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator <=( const String& s1, String::schar_type c2 )
{
   return s1.Compare( c2 ) <= 0;
}

/*!
 * <em>Greater than</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator  >( const String& s1, String::schar_type c2 )
{
   return s1.Compare( c2 ) > 0;
}

/*!
 * <em>Greater than or equal</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator >=( const String& s1, String::schar_type c2 )
{
   return s1.Compare( c2 ) >= 0;
}

// ----------------------------------------------------------------------------

/*!
 * Equality operator.
 * \ingroup string_relational_operators
 */
inline bool operator ==( String::schar_type c1, const String& s2 )
{
   return s2.Compare( c1 ) == 0;
}

/*!
 * <em>Less than</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator  <( String::schar_type c1, const String& s2 )
{
   return s2.Compare( c1 ) > 0;
}

/*!
 * <em>Less than or equal</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator <=( String::schar_type c1, const String& s2 )
{
   return s2.Compare( c1 ) >= 0;
}

/*!
 * <em>Greater than</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator  >( String::schar_type c1, const String& s2 )
{
   return s2.Compare( c1 ) < 0;
}

/*!
 * <em>Greater than or equal</em> operator.
 * \ingroup string_relational_operators
 */
inline bool operator >=( String::schar_type c1, const String& s2 )
{
   return s2.Compare( c1 ) <= 0;
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

// ****************************************************************************
// EOF pcl/String.h - Released 2014/11/14 17:16:40 UTC
