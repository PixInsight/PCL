//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/IndirectSortedArray.h - Released 2015/12/17 18:52:09 UTC
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

#ifndef __PCL_IndirectSortedArray_h
#define __PCL_IndirectSortedArray_h

/// \file pcl/IndirectSortedArray.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_IndirectArray_h
#include <pcl/IndirectArray.h>
#endif

#ifndef __PCL_SortedArray_h
#include <pcl/SortedArray.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class IndirectSortedArray
 * \brief Generic dynamic sorted array of pointers to objects.
 *
 * ### TODO: Write a detailed description for %IndirectSortedArray.
 *
 * \ingroup dynamic_arrays
 */
template <class T, class A = StandardAllocator>
class PCL_CLASS IndirectSortedArray : public IndirectSortedContainer<T>
{
public:

   /*! #
    */
   typedef IndirectArray<T,A>       array_implementation;

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

   /*! #
    */
   typedef typename array_implementation::equal
                                    equal;

   /*! #
    */
   typedef typename array_implementation::less
                                    less;

   // -------------------------------------------------------------------------

   /*!
    * Constructs an empty indirect sorted array.
    */
   IndirectSortedArray() : m_array()
   {
   }

   /*!
    * Constructs an indirect sorted array of length \a n. All contained
    * pointers are initialized to \c nullptr.
    */
   explicit
   IndirectSortedArray( size_type n ) : m_array( n )
   {
   }

   /*!
    * Constructs an indirect sorted array that stores \a n copies of a pointer
    * \a p.
    */
   IndirectSortedArray( size_type n, const T* p ) : m_array( n, p )
   {
   }

   /*!
    * Constructs an indirect sorted array as a copy of the sequence of pointers
    * defined by the range [i,j). The stored sequence of pointers is sorted by
    * comparing the pointed objects.
    */
   template <class FI>
   IndirectSortedArray( FI i, FI j ) : m_array( i, j )
   {
      Sort();
   }

   /*!
    * Copy constructor.
    */
   IndirectSortedArray( const IndirectSortedArray& x ) : m_array( x.m_array )
   {
   }

   /*!
    * Move constructor.
    */
   IndirectSortedArray( IndirectSortedArray&& x ) : m_array( std::move( x.m_array ) )
   {
   }

   /*!
    * Destroys an %IndirectSortedArray object.
    *
    * Deallocates the internal array of pointers to objects, but does not
    * destroy the pointed objects. To destroy them, you have to call Destroy()
    * or Delete() explicitly.
    */
   ~IndirectSortedArray()
   {
   }

   /*!
    * Returns true iff this indirect sorted array uniquely references its
    * contained array of data pointers.
    */
   bool IsUnique() const
   {
      return m_array.IsUnique();
   }

   /*!
    * Returns true iff this indirect sorted array is an alias of the indirect
    * sorted array \a x.
    *
    * Two objects are aliases if both share the same data. Two indirect
    * containers are aliases if they share a unique set of data pointers.
    */
   bool IsAliasOf( const IndirectSortedArray& x ) const
   {
      return m_array.IsAliasOf( x.m_array );
   }

   /*!
    * Ensures that this indirect array uniquely references its contained data
    * pointers.
    *
    * If necessary, this member function generates a duplicate of the array
    * of pointers, references it, and then decrements the reference counter of
    * the original pointers array.
    */
   void EnsureUnique()
   {
      m_array.EnsureUnique();
   }

   /*!
    * Returns the total number of bytes required to store the array of data
    * pointers in this indirect array.
    */
   size_type Size() const
   {
      return m_array.Size();
   }

   /*! #
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
   const T* operator []( size_type i ) const
   {
      return m_array[i];
   }

   /*! #
    */
   const T* operator *() const
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

   /*! #
    */
   const T* First() const
   {
      return m_array.First();
   }

   /*! #
    */
   T* MutableFirst()
   {
      return IsEmpty() ? nullptr : *MutableBegin();
   }

   /*! #
    */
   const T* Last() const
   {
      return m_array.Last();
   }

   /*! #
    */
   T* MutableLast()
   {
      return IsEmpty() ? nullptr : *MutableReverseBegin();
   }

   /*!
    * Ensures that the specified iterator points to a uniquely referenced
    * pointer. If necessary, this function builds a new, uniquely referenced
    * copy of this indirect array by calling EnsureUnique().
    *
    * If the iterator \a i is changed, it is guaranteed to point to the pointer
    * at the same array index it was pointing to before calling this function.
    */
   void UniquifyIterator( iterator& i )
   {
      m_array.UniquifyIterator( i );
   }

   /*!
    * Ensures that the specified iterators point to uniquely referenced
    * pointers. If necessary, this function builds a new, uniquely referenced
    * copy of this indirect array by calling EnsureUnique().
    *
    * If the iterators \a i and \a j are changed, they are guaranteed to point
    * to the pointers at the same array indices they were pointing to before
    * calling this function.
    */
   void UniquifyIterators( iterator& i, iterator& j )
   {
      m_array.UniquifyIterators( i, j );
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
    * Causes this indirect array to reference the same set of pointers as
    * another array \a x. Returns a reference to this object.
    */
   IndirectSortedArray& operator =( const IndirectSortedArray& x )
   {
      Assign( x );
      return *this;
   }

   /*! #
    */
   void Assign( const IndirectSortedArray& x )
   {
      m_array.Assign( x.m_array );
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   IndirectSortedArray& operator =( IndirectSortedArray&& x )
   {
      Transfer( x );
      return *this;
   }

   /*! #
    */
   void Transfer( IndirectSortedArray& x )
   {
      m_array.Transfer( x.m_array );
   }

   /*! #
    */
   void Transfer( IndirectSortedArray&& x )
   {
      m_array.Transfer( x.m_array );
   }

   /*! #
    */
   IndirectSortedArray& operator =( const array_implementation& x )
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
   IndirectSortedArray& operator =( array_implementation&& x )
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
   void Assign( const T* p, size_type n = 1 )
   {
      m_array.Assign( p, n );
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
   template <class C>
   void CloneAssign( const C& x )
   {
      m_array.CloneAssign( x );
      Sort();
   }

   /*! #
    */
   void CloneAssign( SortedArray<T,A>& x )
   {
      m_array.CloneAssign( x );
   }

   /*! #
    */
   void CloneAssign( IndirectSortedArray& x )
   {
      m_array.CloneAssign( x );
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
   void Add( const IndirectSortedArray& x )
   {
      const_iterator p = x.m_array.Begin(), q = x.m_array.End();
      less cmp;
      for ( iterator i = m_array.Begin(); i < m_array.End() && p < q; ++i )
         if ( cmp( *p, *i ) )
            i = m_array.Insert( i, *p++ );
      if ( p < q )
         m_array.Append( p, q );
   }

   /*! #
    */
   void Add( const IndirectArray<T,A>& x )
   {
      Add( x.Begin(), x.End() );
   }

   /*! #
    */
   const_iterator Add( const T* p, size_type n = 1 )
   {
      return m_array.Insert( pcl::InsertionPoint( m_array.Begin(), m_array.End(), p, less() ), p, n );
   }

   /*! #
    */
   template <class FI>
   void Add( FI i, FI j )
   {
      if ( i != j )
      {
         m_array.EnsureUnique();
         for ( const_iterator l = m_array.Begin(), r = m_array.End(); ; )
         {
            FI h = i;
            const_iterator m = m_array.Insert( pcl::InsertionPoint( l, r, *i, less() ), *i );

            if ( ++i == j )
               break;

            if ( less()( *i, *h ) )
            {
               l = m_array.Begin();
               r = m;
            }
            else
            {
               l = m+1;
               r = m_array.End();
            }
         }
      }
   }

   /*! #
    */
   void Remove( const_iterator i, size_type n = 1 )
   {
      m_array.Remove( i, n );
   }

   /*! #
    */
   void Remove( const_iterator i, const_iterator j )
   {
      m_array.Remove( i, j );
   }

   /*! #
    */
   void Remove( const T& v )
   {
      const_iterator i = pcl::BinarySearch( Begin(), End(), &v, less() );
      if ( i != End() )
         m_array.Remove( i, pcl::InsertionPoint( i+1, End(), &v, less() ) );
   }

   /*! #
    */
   template <class BP>
   void Remove( const T& v, BP p )
   {
      m_array.Remove( v, p );
   }

   /*! #
    */
   void Remove( const T* p )
   {
      m_array.Remove( p );
   }

   /*! #
    */
   void Clear()
   {
      m_array.Clear();
   }

   /*! #
    */
   void Delete( iterator i, size_type n = 1 )
   {
      Delete( i, i+n );
   }

   /*! #
    */
   void Delete( iterator i, iterator j )
   {
      // NB: Copy-on-write must *not* happen in this function.
      if ( i < End() )
      {
         i = pcl::Max( const_cast<iterator>( Begin() ), i );
         j = pcl::Min( j, const_cast<iterator>( End() ) );
         m_array.Delete( i, j );
         if ( j < End() )
            pcl::Fill( pcl::Copy( i, j, const_cast<iterator>( End() ) ),
                       const_cast<iterator>( End() ),
                       static_cast<T*>( nullptr ) );
      }
   }

   /*! #
    */
   void Delete( const T& v )
   {
      // NB: Copy-on-write must *not* happen in this function.
      const_iterator i = pcl::BinarySearch( Begin(), End(), &v, less() );
      if ( i != End() )
         Delete( const_cast<iterator>( i ),
                 const_cast<iterator>( pcl::InsertionPoint( i+1, Begin(), &v, less() ) ) );
   }

   /*! #
    */
   template <class BP>
   void Delete( const T& v, BP p )
   {
      // NB: Copy-on-write must *not* happen in this function.
      m_array.Delete( v, p );
      pcl::QuickSort( Begin(), End(), less() );
   }

   /*! #
    */
   void Delete()
   {
      m_array.Delete();
   }

   /*! #
    */
   void Destroy( iterator i, size_type n = 1 )
   {
      m_array.Destroy( i, n );
   }

   /*! #
    */
   void Destroy( iterator i, iterator j )
   {
      m_array.Destroy( i, j );
   }

   /*! #
    */
   void Destroy( const T& v )
   {
      const_iterator i = pcl::BinarySearch( Begin(), End(), &v, less() );
      if ( i != End() )
         Destroy( const_cast<iterator>( i ),
                  const_cast<iterator>( pcl::InsertionPoint( i+1, End(), &v, less() ) ) );
   }

   /*! #
    */
   template <class BP>
   void Destroy( const T& v, BP p )
   {
      m_array.Destroy( v, p );
      pcl::QuickSort( Begin(), End(), less() );
   }

   /*! #
    */
   void Destroy()
   {
      m_array.Destroy();
   }

   /*! #
    */
   void Pack()
   {
      m_array.Pack();
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
      m_array.Apply( f );
   }

   /*! #
    */
   template <class F>
   const_iterator FirstThat( F f ) const
   {
      return m_array.FirstThat( f );
   }

   /*! #
    */
   template <class F>
   const_iterator LastThat( F f ) const
   {
      return m_array.LastThat( f );
   }

   /*! #
    */
   size_type Count( const T& v ) const
   {
      const_iterator i = pcl::BinarySearch( Begin(), End(), &v, less() );
      return (i != End()) ? pcl::InsertionPoint( i+1, End(), &v, less() ) - i : 0;
   }

   /*! #
    */
   size_type Count( const T* p ) const
   {
      return m_array.Count( p );
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
      return m_array.MinItem( p );
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
      return m_array.MaxItem( p );
   }

   /*! #
    */
   const_iterator Search( const T& v ) const
   {
      return pcl::BinarySearch( Begin(), End(), &v, less() );
   }

   /*! #
    */
   const_iterator Search( const T* p ) const
   {
      return m_array.Search( p );
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
      return pcl::BinarySearchLast( Begin(), End(), &v, less() );
   }

   /*! #
    */
   const_iterator SearchLast( const T* p ) const
   {
      return m_array.SearchLast( p );
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
   bool Contains( const T* p ) const
   {
      return m_array.Contains( p );
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
    * Exchanges two indirect sorted arrays \a x1 and \a x2.
    */
   friend void Swap( IndirectSortedArray& x1, IndirectSortedArray& x2 )
   {
      pcl::Swap( x1.m_array, x2.m_array );
   }

   /*!
    * Returns true only if two indirect sorted arrays \a x1 and \a x2 are
    * equal.
    * \ingroup array_relational_operators
    */
   friend bool operator ==( const IndirectSortedArray& x1, const IndirectSortedArray& x2 )
   {
      return x1.m_array == x2.m_array;
   }

   /*!
    * Returns true only if an indirect sorted array \a x1 is equal to an
    * indirect array \a x2.
    * \ingroup array_relational_operators
    */
   friend bool operator ==( const IndirectSortedArray& x1, const array_implementation& x2 )
   {
      return x1.m_array == x2;
   }

   /*!
    * Returns true only if an indirect array \a x1 is equal to an indirect
    * sorted array \a x2.
    * \ingroup array_relational_operators
    */
   friend bool operator ==( const array_implementation& x1, const IndirectSortedArray& x2 )
   {
      return x1 == x2.m_array;
   }

   /*!
    * Returns true only if an indirect sorted array \a x1 precedes another
    * indirect sorted array \a x2.
    * \ingroup array_relational_operators
    */
   friend bool operator <( const IndirectSortedArray& x1, const IndirectSortedArray& x2 )
   {
      return x1.m_array < x2.m_array;
   }

   /*!
    * Returns true only if an indirect sorted array \a x1 precedes an indirect
    * array \a x2.
    * \ingroup array_relational_operators
    */
   friend bool operator <( const IndirectSortedArray& x1, const array_implementation& x2 )
   {
      return x1.m_array < x2;
   }

   /*!
    * Returns true only if an indirect array \a x1 precedes an indirect sorted
    * array \a x2.
    * \ingroup array_relational_operators
    */
   friend bool operator <( const array_implementation& x1, const IndirectSortedArray& x2 )
   {
      return x1 < x2.m_array;
   }

   /*!
    * Generates a sequence of string tokens separated with the specified
    * \a separator string. Returns a reference to the target string \a s.
    *
    * For each non-null pointer in this indirect array, this function appends a
    * string representation of the pointed object (known as a \e token) to the
    * target string \a s. If the array contains more than one non-null pointer,
    * successive tokens are separated with the specified \a separator.
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
    * For each non-null pointer p in this indirect array, this function appends
    * a string representation of the pointed object (known as a \e token) to
    * the target string \a s by calling the \a append function:
    *
    *\code append( s, S( *p ) ); \endcode
    *
    * If the array contains more than one non-null pointer, successive tokens
    * are separated by calling:
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
    * Returns a 64-bit non-cryptographic hash value computed for this indirect
    * array.
    *
    * This function calls pcl::Hash64() for the internal array of pointers,
    * \e not for the pointed objects.
    *
    * The \a seed parameter can be used to generate repeatable hash values. It
    * can also be set to a random value in compromised environments.
    */
   uint64 Hash64( uint64 seed = 0 ) const
   {
      return m_array.Hash64( seed );
   }

   /*!
    * Returns a 32-bit non-cryptographic hash value computed for this indirect
    * array.
    *
    * This function calls pcl::Hash32() for the internal array of pointers,
    * \e not for the pointed objects.
    *
    * The \a seed parameter can be used to generate repeatable hash values. It
    * can also be set to a random value in compromised environments.
    */
   uint32 Hash32( uint32 seed = 0 ) const
   {
      return m_array.Hash32( seed );
   }

   /*!
    * Returns a non-cryptographic hash value computed for this indirect array.
    * This function is a synonym for Hash64().
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
 * Adds a pointer to an object \a p to an indirect sorted array \a x. Returns a
 * reference to the indirect sorted array.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
IndirectSortedArray<T,A>& operator <<( IndirectSortedArray<T,A>& x, const T* p )
{
   x.Add( p );
   return x;
}

/*!
 * Adds a pointer to an object \a p to an indirect sorted array \a x. Returns a
 * reference to the indirect sorted array.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
IndirectSortedArray<T,A>& operator <<( IndirectSortedArray<T,A>&& x, const T* p )
{
   x.Add( p );
   return x;
}

/*!
 * Adds an indirect sorted array \a x2 to another indirect sorted array \a x1.
 * Returns a reference to the left-hand indirect sorted array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
IndirectSortedArray<T,A>& operator <<( IndirectSortedArray<T,A>& x1, const IndirectSortedArray<T,A>& x2 )
{
   x1.Add( x2 );
   return x1;
}

/*!
 * Adds an indirect sorted array \a x2 to another indirect sorted array \a x1.
 * Returns a reference to the left-hand indirect sorted array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
IndirectSortedArray<T,A>& operator <<( IndirectSortedArray<T,A>&& x1, const IndirectSortedArray<T,A>& x2 )
{
   x1.Add( x2 );
   return x1;
}

/*!
 * Adds an indirect array \a x2 to an indirect sorted array \a x1. Returns a
 * reference to the left-hand indirect sorted array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
IndirectSortedArray<T,A>& operator <<( IndirectSortedArray<T,A>& x1, const IndirectArray<T,A>& x2 )
{
   x1.Add( x2 );
   return x1;
}

/*!
 * Adds an indirect array \a x2 to an indirect sorted array \a x1. Returns a
 * reference to the left-hand indirect sorted array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
IndirectSortedArray<T,A>& operator <<( IndirectSortedArray<T,A>&& x1, const IndirectArray<T,A>& x2 )
{
   x1.Add( x2 );
   return x1;
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_IndirectSortedArray_h

// ----------------------------------------------------------------------------
// EOF pcl/IndirectSortedArray.h - Released 2015/12/17 18:52:09 UTC
