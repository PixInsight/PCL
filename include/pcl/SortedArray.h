//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/SortedArray.h - Released 2017-08-01T14:23:31Z
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

#ifndef __PCL_SortedArray_h
#define __PCL_SortedArray_h

/// \file pcl/SortedArray.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Array.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class SortedArray
 * \brief Generic dynamic sorted array.
 *
 * %SortedArray is a generic, finite sorted sequence of objects, implemented as
 * a reference-counted, dynamic array of T instances with automatic sorting of
 * inserted array elements. The type A provides dynamic allocation for
 * contiguous sequences of elements of type T (StandardAllocator is used by
 * default).
 *
 * \sa Array, ReferenceArray, ReferenceSortedArray, IndirectArray,
 * IndirectSortedArray
 * \ingroup dynamic_arrays
 */
template <class T, class A = StandardAllocator>
class PCL_CLASS SortedArray : public DirectSortedContainer<T>
{
public:

   /*! #
    */
   typedef Array<T,A>               array_implementation;

   /*! #
    */
   typedef typename array_implementation::block_allocator
                                    block_allocator;

   /*! #
    */
   typedef typename array_implementation::allocator
                                    allocator;

   /*! #
    */
   typedef typename array_implementation::iterator
                                    iterator;

   /*! #
    */
   typedef typename array_implementation::const_iterator
                                    const_iterator;

   /*! #
    */
   typedef typename array_implementation::reverse_iterator
                                    reverse_iterator;

   /*! #
    */
   typedef typename array_implementation::const_reverse_iterator
                                    const_reverse_iterator;

   // -------------------------------------------------------------------------

   /*!
    * Constructs an empty sorted array.
    */
   SortedArray() : m_array()
   {
   }

   /*!
    * Constructs a sorted array of \a n default-constructed objects.
    */
   explicit
   SortedArray( size_type n ) : m_array( n )
   {
   }

   /*!
    * Constructs a sorted array of \a n copies of an object \a v.
    */
   SortedArray( size_type n, const T& v ) : m_array( n, v )
   {
   }

   /*!
    * Constructs a sorted array that stores a sorted copy of the sequence of
    * objects defined by the range [i,j) of forward iterators.
    */
   template <class FI>
   SortedArray( FI i, FI j ) : m_array( i, j )
   {
      Sort();
   }

   /*!
    * Copy constructor.
    */
   SortedArray( const SortedArray& x ) : m_array( x.m_array )
   {
   }

   /*!
    * Move constructor.
    */
   SortedArray( SortedArray&& x ) : m_array( std::move( x.m_array ) )
   {
   }

   /*!
    * Destroys a %SortedArray object. Destroys and deallocates all contained
    * objects.
    */
   ~SortedArray()
   {
   }

   /*!
    * Returns true iff this array uniquely references its contained data.
    */
   bool IsUnique() const
   {
      return m_array.IsUnique();
   }

   /*!
    * Returns true iff this sorted array is an alias of a sorted array \a x.
    *
    * Two objects are aliases if both of them share the same data.
    */
   bool IsAliasOf( const SortedArray& x ) const
   {
      return m_array.IsAliasOf( x.m_array );
   }

   /*!
    * Ensures that this array uniquely references its contained data.
    *
    * If necessary, this member function generates a duplicate of the array
    * data, references it, and then decrements the reference counter of the
    * original array data.
    */
   void EnsureUnique()
   {
      m_array.EnsureUnique();
   }

   /*!
    * Returns the total number of bytes required to store the objects contained
    * by this sorted array.
    */
   size_type Size() const
   {
      return m_array.Size();
   }

   /*!
    * Returns the length of this sorted array.
    */
   size_type Length() const
   {
      return m_array.Length();
   }

   /*! #
    */
   size_type Capacity() const
   {
      return m_array.Capacity();
   }

   /*! #
    */
   size_type Available() const
   {
      return m_array.Available();
   }

   /*! #
    */
   bool IsValid() const
   {
      return m_array.IsValid();
   }

   /*! #
    */
   bool IsEmpty() const
   {
      return m_array.IsEmpty();
   }

   /*! #
    */
   size_type LowerBound() const
   {
      return m_array.LowerBound();
   }

   /*! #
    */
   size_type UpperBound() const
   {
      return m_array.UpperBound();
   }

   /*! #
    */
   const allocator& GetAllocator() const
   {
      return m_array.GetAllocator();
   }

   /*! #
    */
   void SetAllocator( const allocator& a )
   {
      m_array.SetAllocator( a );
   }

   /*! #
    */
   const_iterator At( size_type i ) const
   {
      return m_array.At( i );
   }

   /*! #
    */
   iterator MutableAt( size_type i )
   {
      return m_array.At( i );
   }

   /*! #
    */
   iterator MutableIterator( const_iterator i )
   {
      return m_array.MutableIterator( i );
   }

   /*! #
    */
   const T& operator []( size_type i ) const
   {
      return m_array[i];
   }

   /*! #
    */
   const T& operator *() const
   {
      return *Begin();
   }

   /*! #
    */
   const_iterator Begin() const
   {
      return m_array.ConstBegin();
   }

   /*! #
    */
   iterator MutableBegin()
   {
      return m_array.Begin();
   }

   /*! #
    */
   const_iterator End() const
   {
      return m_array.ConstEnd();
   }

   /*! #
    */
   iterator MutableEnd()
   {
      return m_array.End();
   }

   /*! #
    */
   const_reverse_iterator ReverseBegin() const
   {
      return m_array.ConstReverseBegin();
   }

   /*! #
    */
   reverse_iterator MutableReverseBegin()
   {
      return m_array.ReverseBegin();
   }

   /*! #
    */
   const_reverse_iterator ReverseEnd() const
   {
      return m_array.ConstReverseEnd();
   }

   /*! #
    */
   reverse_iterator MutableReverseEnd()
   {
      return m_array.ReverseEnd();
   }

   /*!
    * Ensures that the specified iterator points to a uniquely referenced
    * object. If necessary, this function builds a new, uniquely referenced
    * copy of this array by calling EnsureUnique().
    *
    * If the iterator \a i is changed, it is guaranteed to point to the object
    * at the same array index it was pointing to before calling this function.
    */
   void UniquifyIterator( iterator& i )
   {
      return m_array.UniquifyIterator( i );
   }

   /*!
    * Ensures that the specified iterators point to uniquely referenced
    * objects. If necessary, this function builds a new, uniquely referenced
    * copy of this array by calling EnsureUnique().
    *
    * If the iterators \a i and \a j are changed, they are guaranteed to point
    * to the objects at the same array indices they were pointing to before
    * calling this function.
    */
   void UniquifyIterators( iterator& i, iterator& j )
   {
      return m_array.UniquifyIterators( i, j );
   }

#ifndef __PCL_NO_STL_COMPATIBLE_ITERATORS
   /*!
    * STL-compatible iteration. Equivalent to Begin() const.
    */
   const_iterator begin() const
   {
      return Begin();
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
    * Copy assignment operator.
    *
    * Causes this sorted array to reference the same data as another sorted
    * array \a x. Returns a reference to this object.
    */
   SortedArray& operator =( const SortedArray& x )
   {
      Assign( x );
      return *this;
   }

   /*! #
    */
   void Assign( const SortedArray& x )
   {
      m_array.Assign( x.m_array );
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   SortedArray& operator =( SortedArray&& x )
   {
      Transfer( x );
      return *this;
   }

   /*! #
    */
   void Transfer( SortedArray& x )
   {
      m_array.Transfer( x.m_array );
   }

   /*! #
    */
   void Transfer( SortedArray&& x )
   {
      m_array.Transfer( x.m_array );
   }

   /*! #
    */
   SortedArray& operator =( const array_implementation& x )
   {
      Assign( x );
      return *this;
   }

   /*! #
    */
   void Assign( const array_implementation& x )
   {
      m_array.Assign( x );
      Sort();
   }

   /*! #
    */
   SortedArray& operator =( array_implementation&& x )
   {
      Transfer( x );
      return *this;
   }

   /*! #
    */
   void Transfer( array_implementation& x )
   {
      m_array.Transfer( x );
      Sort();
   }

   /*! #
    */
   void Transfer( array_implementation&& x )
   {
      m_array.Transfer( x );
      Sort();
   }

   /*! #
    */
   void Assign( const T& v, size_type n = 1 )
   {
      m_array.Assign( v, n );
   }

   /*! #
    */
   template <class FI>
   void Assign( FI i, FI j )
   {
      m_array.Assign( i, j );
      Sort();
   }

   /*! #
    */
   void Import( iterator i, iterator j )
   {
      m_array.Import( i, j );
      Sort();
   }

   /*! #
    */
   iterator Release()
   {
      return m_array.Release();
   }

   /*! #
    */
   void Add( const SortedArray& x )
   {
      const_iterator p = x.Begin(), q = x.End();
      for ( iterator i = m_array.Begin(); i < m_array.End() && p < q; ++i )
         if ( *p < *i )
            i = m_array.Insert( i, *p++ );
      if ( p < q )
         m_array.Append( p, q );
   }

   /*! #
    */
   void Add( const Array<T,A>& x )
   {
      Add( x.Begin(), x.End() );
   }

   /*! #
    */
   const_iterator Add( const T& v, size_type n = 1 )
   {
      return m_array.Insert( pcl::InsertionPoint( m_array.Begin(), m_array.End(), v ), v, n );
   }

   /*! #
    */
   template <class FI>
   void Add( FI i, FI j )
   {
      if ( i != j )
      {
         m_array.EnsureUnique();
         for ( iterator l = m_array.Begin(), r = m_array.End(); ; )
         {
            FI h = i;
            iterator m = m_array.Insert( pcl::InsertionPoint( l, r, *i ), *i );

            if ( ++i == j )
               break;

            if ( *i < *h )
            {
               l = m_array.Begin();
               r = m;
            }
            else
            {
               l = m + 1;
               r = m_array.End();
            }
         }
      }
   }

   /*! #
    */
   void Remove( const_iterator i, size_type n = 1 )
   {
      m_array.Remove( const_cast<iterator>( i ), n );
   }

   /*! #
    */
   void Remove( const_iterator i, const_iterator j )
   {
      m_array.Remove( const_cast<iterator>( i ), const_cast<iterator>( j ) );
   }

   /*!
    * Destroys and removes a trailing sequence of contiguous objects from the
    * specified iterator of this array. This operation is equivalent to:
    *
    * \code Remove( i, End() ) \endcode
    *
    * If the specified iterator \a i is located at or after the end of this
    * array, this function does nothing. Otherwise the iterator is constrained
    * to stay in the range [Begin(),End()) of existing array elements.
    */
   void Truncate( const_iterator i )
   {
      m_array.Truncate( const_cast<iterator>( i ) );
   }

   /*!
    * Removes a contiguous trailing sequence of \a n existing objects from this
    * sorted array. This operation is equivalent to:
    *
    * \code Truncate( End() - n ) \endcode
    *
    * If the specified count \a n is greater than or equal to the length of
    * this array, this function calls Clear() to yield an empty array.
    */
   void Shrink( size_type n = 1 )
   {
      m_array.Shrink( n );
   }

   /*! #
    */
   void Remove( const T& v )
   {
      const_iterator i = pcl::BinarySearch( Begin(), End(), v );
      if ( i != End() )
         Remove( i, pcl::InsertionPoint( i+1, End(), v ) );
   }

   /*! #
    */
   void Clear()
   {
      m_array.Clear();
   }

   /*! #
    */
   void Reserve( size_type n )
   {
      m_array.Reserve( n );
   }

   /*! #
    */
   void Squeeze()
   {
      m_array.Squeeze();
   }

   /*!
    * Sets all objects contained by this array equal to \a v.
    */
   void Fill( const T& v )
   {
      m_array.Fill( v );
   }

   /*! #
    */
   template <class F>
   void Apply( F f ) const
   {
      pcl::Apply( Begin(), End(), f );
   }

   /*! #
    */
   template <class F>
   const_iterator FirstThat( F f ) const
   {
      return pcl::FirstThat( Begin(), End(), f );
   }

   /*! #
    */
   template <class F>
   const_iterator LastThat( F f ) const
   {
      return pcl::LastThat( Begin(), End(), f );
   }

   /*! #
    */
   size_type Count( const T& v ) const
   {
      const_iterator i = pcl::BinarySearch( Begin(), End(), v );
      return (i != End()) ? pcl::InsertionPoint( i+1, End(), v ) - i : 0;
   }

   /*! #
    */
   template <class BP>
   size_type Count( const T& v, BP p ) const
   {
      return m_array.Count( v, p );
   }

   /*! #
    */
   template <class UP>
   size_type CountIf( UP p ) const
   {
      return m_array.CountIf( p );
   }

   /*! #
    */
   const_iterator MinItem() const
   {
      return Begin();
   }

   /*! #
    */
   template <class BP>
   const_iterator MinItem( BP p ) const
   {
      return pcl::MinItem( Begin(), End(), p );
   }

   /*! #
    */
   const_iterator MaxItem() const
   {
      return IsEmpty() ? End() : End()-1;
   }

   /*! #
    */
   template <class BP>
   const_iterator MaxItem( BP p ) const
   {
      return pcl::MaxItem( Begin(), End(), p );
   }

   /*! #
    */
   const_iterator Search( const T& v ) const
   {
      return pcl::BinarySearch( Begin(), End(), v );
   }

   /*! #
    */
   template <class BP>
   const_iterator Search( const T& v, BP p ) const
   {
      return m_array.Search( v, p );
   }

   /*! #
    */
   const_iterator SearchLast( const T& v ) const
   {
      return pcl::BinarySearchLast( Begin(), End(), v );
   }

   /*! #
    */
   template <class BP>
   const_iterator SearchLast( const T& v, BP p ) const
   {
      return m_array.SearchLast( v, p );
   }

   /*! #
    */
   bool Contains( const T& v ) const
   {
      return Search( v ) != End();
   }

   /*! #
    */
   template <class BP>
   bool Contains( const T& v, BP p ) const
   {
      return Search( v, p ) != End();
   }

   /*! #
    */
   void Sort()
   {
      m_array.Sort();
   }

   /*!
    * Exchanges two sorted arrays \a x1 and \a x2.
    */
   friend void Swap( SortedArray& x1, SortedArray& x2 )
   {
      pcl::Swap( x1.m_array, x2.m_array );
   }

   /*!
    * Returns true only if two sorted arrays \a x1 and \a x2 are equal.
    * \ingroup array_relational_operators
    */
   friend bool operator ==( const SortedArray& x1, const SortedArray& x2 )
   {
      return x1.m_array == x2.m_array;
   }

   /*!
    * Returns true only if a sorted array \a x1 is equal to an array \a x2.
    * \ingroup array_relational_operators
    */
   friend bool operator ==( const SortedArray& x1, const array_implementation& x2 )
   {
      return x1.m_array == x2;
   }

   /*!
    * Returns true only if an array \a x1 is equal to a sorted array \a x2.
    * \ingroup array_relational_operators
    */
   friend bool operator ==( const array_implementation& x1, const SortedArray& x2 )
   {
      return x1 == x2.m_array;
   }

   /*!
    * Returns true only if a sorted array \a x1 precedes another sorted array
    * \a x2.
    * \ingroup array_relational_operators
    */
   friend bool operator <( const SortedArray& x1, const SortedArray& x2 )
   {
      return x1.m_array < x2.m_array;
   }

   /*!
    * Returns true only if a sorted array \a x1 precedes an array \a x2.
    * \ingroup array_relational_operators
    */
   friend bool operator <( const SortedArray& x1, const array_implementation& x2 )
   {
      return x1.m_array < x2;
   }

   /*!
    * Returns true only if an array \a x1 precedes a sorted array \a x2.
    * \ingroup array_relational_operators
    */
   friend bool operator <( const array_implementation& x1, const SortedArray& x2 )
   {
      return x1 < x2.m_array;
   }

   /*!
    * Generates a sequence of string tokens separated with the specified
    * \a separator string. Returns a reference to the target string \a s.
    *
    * For each element in this array, this function appends a string
    * representation (known as a \e token) to the target string \a s. If the
    * array contains more than one element, successive tokens are separated
    * with the specified \a separator.
    *
    * The string type S must have a meaningful %Append() member function and
    * type conversion semantics to transform an array element to a string. The
    * standard String and IsoString PCL classes provide the required
    * functionality for most scalar types, although it is probably better to
    * use String::ToSeparated() and IsoString::ToSeparated() instead of calling
    * these functions directly.
    */
   template <class S, typename SP>
   S& ToSeparated( S& s, SP separator ) const
   {
      return m_array.ToSeparated( s, separator );
   }

   /*!
    * Generates a sequence of string tokens separated with the specified
    * \a separator string by calling an \a append function. Returns a reference
    * to the target string \a s.
    *
    * For each element x in this array, this function appends a string
    * representation (known as a \e token) to the target string \a s by
    * calling the \a append function:
    *
    *\code append( s, S( x ) ); \endcode
    *
    * If the array contains more than one element, successive tokens are
    * separated by calling:
    *
    * \code append( s, S( separator ) ); \endcode
    *
    * The string type S must have type conversion semantics to transform an
    * array element to a string. The standard String and IsoString PCL classes
    * provide the required functionality for most scalar types, although it is
    * probably easier to use String::ToSeparated() and IsoString::ToSeparated()
    * instead of calling these functions directly.
    */
   template <class S, typename SP, class AF>
   S& ToSeparated( S& s, SP separator, AF append ) const
   {
      return m_array.ToSeparated( s, separator, append );
   }

   /*!
    * Generates a comma-separated sequence of string tokens. Returns a
    * reference to the target string \a s.
    *
    * This function is equivalent to:
    *
    * \code ToSeparated( s, ',' ); \endcode
    */
   template <class S>
   S& ToCommaSeparated( S& s ) const
   {
      return m_array.ToCommaSeparated( s );
   }

   /*!
    * Generates a space-separated sequence of string tokens. Returns a
    * reference to the target string \a s.
    *
    * This function is equivalent to:
    *
    * \code ToSeparated( s, ' ' ); \endcode
    */
   template <class S>
   S& ToSpaceSeparated( S& s ) const
   {
      return m_array.ToSpaceSeparated( s );
   }

   /*!
    * Generates a tabulator-separated sequence of string tokens. Returns a
    * reference to the target string \a s.
    *
    * This function is equivalent to:
    *
    * \code ToSeparated( s, '\t' ); \endcode
    */
   template <class S>
   S& ToTabSeparated( S& s ) const
   {
      return m_array.ToTabSeparated( s );
   }

   /*!
    * Returns a 64-bit non-cryptographic hash value computed for this array.
    *
    * This function calls pcl::Hash64() for the internal array buffer.
    *
    * The \a seed parameter can be used to generate repeatable hash values. It
    * can also be set to a random value in compromised environments.
    */
   uint64 Hash64( uint64 seed = 0 ) const
   {
      return m_array.Hash64( seed );
   }

   /*!
    * Returns a 32-bit non-cryptographic hash value computed for this array.
    *
    * This function calls pcl::Hash32() for the internal array buffer.
    *
    * The \a seed parameter can be used to generate repeatable hash values. It
    * can also be set to a random value in compromised environments.
    */
   uint32 Hash32( uint32 seed = 0 ) const
   {
      return m_array.Hash32( seed );
   }

   /*!
    * Returns a non-cryptographic hash value computed for this array. This
    * function is a synonym for Hash64().
    */
   uint64 Hash( uint64 seed = 0 ) const
   {
      return Hash64( seed );
   }

   // -------------------------------------------------------------------------

private:

   array_implementation m_array;
};

// ----------------------------------------------------------------------------

/*!
 * Adds an object \a v to a sorted array \a x. Returns a reference to the
 * sorted array.
 *
 * The template argument type T must have conversion semantics from the type V,
 * such as T::T( const V& ) or equivalent.
 * \ingroup array_insertion_operators
 */
template <class T, class A, class V> inline
SortedArray<T,A>& operator <<( SortedArray<T,A>& x, const V& v )
{
   x.Add( T( v ) );
   return x;
}

/*!
 * Adds an object \a v to a temporary sorted array \a x. Returns a reference to
 * the sorted array.
 *
 * The template argument type T must have conversion semantics from the type V,
 * such as T::T( const V& ) or equivalent.
 * \ingroup array_insertion_operators
 */
template <class T, class A, class V> inline
SortedArray<T,A>& operator <<( SortedArray<T,A>&& x, const V& v )
{
   x.Add( T( v ) );
   return x;
}

/*!
 * Adds a sorted array \a x2 to a sorted array \a x1. Returns a reference to
 * the left-hand sorted array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
SortedArray<T,A>& operator <<( SortedArray<T,A>& x1, const SortedArray<T,A>& x2 )
{
   x1.Add( x2 );
   return x1;
}

/*!
 * Adds a sorted array \a x2 to a temporary sorted array \a x1. Returns a
 * reference to the left-hand sorted array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
SortedArray<T,A>& operator <<( SortedArray<T,A>&& x1, const SortedArray<T,A>& x2 )
{
   x1.Add( x2 );
   return x1;
}

/*!
 * Adds an array \a x2 to a sorted array \a x1. Returns a reference to the
 * left-hand sorted array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
SortedArray<T,A>& operator <<( SortedArray<T,A>& x1, const Array<T,A>& x2 )
{
   x1.Add( x2 );
   return x1;
}

/*!
 * Adds an array \a x2 to a temporary sorted array \a x1. Returns a reference
 * to the left-hand sorted array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
SortedArray<T,A>& operator <<( SortedArray<T,A>&& x1, const Array<T,A>& x2 )
{
   x1.Add( x2 );
   return x1;
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_SortedArray_h

// ----------------------------------------------------------------------------
// EOF pcl/SortedArray.h - Released 2017-08-01T14:23:31Z
