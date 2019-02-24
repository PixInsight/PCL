//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/Array.h - Released 2019-01-21T12:06:07Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_Array_h
#define __PCL_Array_h

/// \file pcl/Array.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Allocator.h>
#include <pcl/Container.h>
#include <pcl/Indirect.h>
#include <pcl/Iterator.h>
#include <pcl/Math.h>
#include <pcl/Memory.h>
#include <pcl/ReferenceCounter.h>
#include <pcl/Relational.h>
#include <pcl/Rotate.h>
#include <pcl/Search.h>
#include <pcl/Sort.h>
#include <pcl/StdAlloc.h>
#include <pcl/Utility.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup dynamic_arrays Dynamic Arrays
 */

// ----------------------------------------------------------------------------

/*!
 * \class Array
 * \brief Generic dynamic array.
 *
 * %Array is a generic, finite ordered sequence of objects, implemented as a
 * reference-counted, dynamic array of T instances. The type A provides dynamic
 * allocation for contiguous sequences of elements of type T (StandardAllocator
 * is used by default).
 *
 * \sa SortedArray, ReferenceArray, ReferenceSortedArray, IndirectArray,
 * IndirectSortedArray
 * \ingroup dynamic_arrays
 */
template <class T, class A = StandardAllocator>
class PCL_CLASS Array : public DirectContainer<T>
{
public:

   /*! #
    */
   typedef A                        block_allocator;

   /*! #
    */
   typedef pcl::Allocator<T,A>      allocator;

   /*! #
    */
   typedef T*                       iterator;

   /*! #
    */
   typedef const T*                 const_iterator;

   /*! #
    */
   typedef ReverseRandomAccessIterator<iterator, T>
                                    reverse_iterator;

   /*! #
    */
   typedef ReverseRandomAccessIterator<const_iterator, const T>
                                    const_reverse_iterator;

   // -------------------------------------------------------------------------

   /*!
    * Constructs an empty array.
    */
   Array()
   {
      m_data = new Data;
   }

   /*!
    * Constructs an array of \a n default-constructed objects.
    */
   explicit
   Array( size_type n )
   {
      m_data = new Data;
      m_data->Allocate( n );
      m_data->Initialize( m_data->begin, m_data->end );
   }

   /*!
    * Constructs an array with \a n copies of an object \a v.
    */
   Array( size_type n, const T& v )
   {
      m_data = new Data;
      m_data->Allocate( n );
      m_data->Initialize( m_data->begin, m_data->end, v );
   }

   /*!
    * Constructs an array that stores a copy of the objects in the range [i,j)
    * of forward iterators.
    */
   template <class FI>
   Array( FI i, FI j )
   {
      m_data = new Data;
      m_data->Allocate( size_type( pcl::Distance( i, j ) ) );
      if ( m_data->begin != nullptr )
         m_data->Build( m_data->begin, i, j );
   }

   /*!
    * Constructs an array that stores a copy of the objects in the specified
    * initializer list \a l.
    *
    * This constructor is equivalent to:
    *
    * \code Array( l.begin(), l.end() ) \endcode
    */
   template <typename T1>
   Array( std::initializer_list<T1> l ) : Array( l.begin(), l.end() )
   {
   }

   /*!
    * Copy constructor.
    */
   Array( const Array& x ) : m_data( x.m_data )
   {
      if ( m_data != nullptr )
         m_data->Attach();
   }

   /*!
    * Move constructor.
    */
   Array( Array&& x ) : m_data( x.m_data )
   {
      x.m_data = nullptr;
   }

   /*!
    * Destroys an %Array object. Destroys and deallocates all contained
    * objects.
    */
   ~Array()
   {
      if ( m_data != nullptr )
      {
         DetachFromData();
         m_data = nullptr;
      }
   }

   /*!
    * Returns true iff this array uniquely references its contained data.
    */
   bool IsUnique() const
   {
      return m_data->IsUnique();
   }

   /*!
    * Returns true iff this array is an alias of an array \a x.
    *
    * Two objects are aliases if both share the same data.
    */
   bool IsAliasOf( const Array& x ) const
   {
      return m_data == x.m_data;
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
      if ( !IsUnique() )
      {
         Data* newData = new Data;
         newData->Allocate( Length() );
         newData->Build( newData->begin, m_data->begin, m_data->end );
         DetachFromData();
         m_data = newData;
      }
   }

   /*!
    * Returns the total number of bytes required to store the objects contained
    * by this dynamic array.
    */
   size_type Size() const
   {
      return m_data->Size();
   }

   /*!
    * Returns the length of this dynamic array.
    */
   size_type Length() const
   {
      return m_data->Length();
   }

   /*!
    * Returns the capacity of this array. The capacity is the maximum number of
    * objects that this array can contain without requiring a reallocation.
    */
   size_type Capacity() const
   {
      return m_data->Capacity();
   }

   /*!
    * Returns the length of the space available in this array, or zero if this
    * array cannot contain more objects. The available space is the number of
    * objects that can be added to this array without requiring a reallocation.
    * It is equal to Capacity() - Length() by definition.
    */
   size_type Available() const
   {
      return m_data->Available();
   }

   /*!
    * Returns true only if this array is valid. An array is valid if it
    * references an internal array structure, even if it is an empty array.
    *
    * In general, all %Array objects are valid with only two exceptions:
    *
    * \li Objects that have been move-copied or move-assigned to other arrays.
    * \li Objects that have been invalidated explicitly by calling Transfer().
    *
    * An invalid array object cannot be used and should be destroyed
    * immediately. Invalid arrays are always destroyed automatically during
    * move construction and move assignment operations.
    */
   bool IsValid() const
   {
      return m_data != nullptr;
   }

   /*!
    * Returns true iff this array is empty.
    */
   bool IsEmpty() const
   {
      return m_data->IsEmpty();
   }

   /*!
    * Returns the minimum legal index in this array (always zero). For empty
    * arrays, this function returns a meaningless value.
    */
   size_type LowerBound() const
   {
      return 0;
   }

   /*!
    * Returns the maximum legal index in this array. It is equal to Length()-1.
    * For empty arrays, this function returns a meaningless value.
    */
   size_type UpperBound() const
   {
      return Length()-1;
   }

   /*!
    * Returns a reference to the allocator object used by this array.
    */
   const allocator& GetAllocator() const
   {
      return m_data->alloc;
   }

   /*!
    * Sets a new allocator object for this array.
    */
   void SetAllocator( const allocator& a )
   {
      EnsureUnique();
      m_data->alloc = a;
   }

   /*!
    * Returns an array iterator located at the specified array index \a i.
    */
   iterator At( size_type i )
   {
      PCL_PRECONDITION( !IsEmpty() && i < Length() )
      EnsureUnique();
      return m_data->begin + i;
   }

   /*!
    * Returns an immutable array iterator located at the specified index \a i.
    */
   const_iterator At( size_type i ) const
   {
      PCL_PRECONDITION( !IsEmpty() && i < Length() )
      return m_data->begin + i;
   }

   /*!
    * Returns a mutable iterator pointing to the same array element as the
    * specified immutable iterator \a i.
    *
    * \warning As a side-effect of calling this function, the specified
    * immutable iterator \a i may become invalid. This happens when this
    * function is called for a shared array, since in this case getting a
    * mutable iterator involves a deep copy of the array through an implicit
    * call to EnsureUnique().
    */
   iterator MutableIterator( const_iterator i )
   {
      return At( i - m_data->begin );
   }

   /*!
    * Returns a reference to the element at the specified index \a i. No bounds
    * checking is performed.
    */
   T& operator []( size_type i )
   {
      return *At( i );
   }

   /*!
    * Returns a reference to the unmodifiable element at the specified index
    * \a i in this array. No bounds checking is performed.
    */
   const T& operator []( size_type i ) const
   {
      return *At( i );
   }

   /*!
    * Returns a reference to the first element of this array.
    */
   T& operator *()
   {
      PCL_PRECONDITION( m_data->begin != nullptr )
      EnsureUnique();
      return *m_data->begin;
   }

   /*!
    * Returns a reference to the unmodifiable first element of this array.
    */
   const T& operator *() const
   {
      PCL_PRECONDITION( m_data->begin != nullptr )
      return *m_data->begin;
   }

   /*!
    * Returns a mutable iterator located at the beginning of this array.
    */
   iterator Begin()
   {
      EnsureUnique();
      return m_data->begin;
   }

   /*!
    * Returns an immutable iterator located at the beginning of this array.
    */
   const_iterator Begin() const
   {
      return m_data->begin;
   }

   /*!
    * Returns an immutable iterator located at the beginning of this array.
    */
   const_iterator ConstBegin() const
   {
      return m_data->begin;
   }

   /*!
    * Returns a mutable iterator located at the end of this array.
    */
   iterator End()
   {
      EnsureUnique();
      return m_data->end;
   }

   /*!
    * Returns an immutable iterator located at the end of this array.
    */
   const_iterator End() const
   {
      return m_data->end;
   }

   /*!
    * Returns an immutable iterator located at the end of this array.
    */
   const_iterator ConstEnd() const
   {
      return m_data->end;
   }

   /*!
    * Returns a mutable reverse iterator located at the <em>reverse
    * beginning</em> of this array.
    *
    * The reverse beginning corresponds to the last element in the array.
    */
   reverse_iterator ReverseBegin()
   {
      PCL_PRECONDITION( m_data->end != nullptr )
      EnsureUnique();
      return m_data->end - 1;
   }

   /*!
    * Returns an immutable reverse iterator located at the <em>reverse
    * beginning</em> of this array.
    *
    * The reverse beginning corresponds to the last element in the array.
    */
   const_reverse_iterator ReverseBegin() const
   {
      PCL_PRECONDITION( m_data->end != nullptr )
      return m_data->end - 1;
   }

   /*!
    * Returns an immutable reverse iterator located at the <em>reverse
    * beginning</em> of this array.
    *
    * The reverse beginning corresponds to the last element in the array.
    */
   const_reverse_iterator ConstReverseBegin() const
   {
      PCL_PRECONDITION( m_data->end != nullptr )
      return m_data->end - 1;
   }

   /*!
    * Returns a mutable reverse iterator located at the <em>reverse end</em> of
    * this array.
    *
    * The reverse end corresponds to an (nonexistent) element immediately
    * before the first element in the array.
    */
   reverse_iterator ReverseEnd()
   {
      PCL_PRECONDITION( m_data->begin != nullptr )
      EnsureUnique();
      return m_data->begin - 1;
   }

   /*!
    * Returns an immutable reverse iterator located at the <em>reverse end</em>
    * of this array.
    *
    * The reverse end corresponds to an (nonexistent) element immediately
    * before the first element in the array.
    */
   const_reverse_iterator ReverseEnd() const
   {
      PCL_PRECONDITION( m_data->begin != nullptr )
      return m_data->begin - 1;
   }

   /*!
    * Returns an immutable reverse iterator located at the <em>reverse end</em>
    * of this array.
    *
    * The reverse end corresponds to an (nonexistent) element immediately
    * before the first element in the array.
    */
   const_reverse_iterator ConstReverseEnd() const
   {
      PCL_PRECONDITION( m_data->begin != nullptr )
      return m_data->begin - 1;
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
      PCL_PRECONDITION( i >= m_data->begin && i <= m_data->end )
      if ( !IsUnique() )
      {
         distance_type d = i - m_data->begin;
         EnsureUnique();
         i = m_data->begin + d;
      }
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
      PCL_PRECONDITION( i >= m_data->begin && i <= m_data->end )
      PCL_PRECONDITION( j >= m_data->begin && j <= m_data->end )
      if ( !IsUnique() )
      {
         distance_type d = i - m_data->begin;
         distance_type r = j - i;
         EnsureUnique();
         j = (i = m_data->begin + d) + r;
      }
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
    * Copy assignment operator.
    *
    * Causes this array to reference the same data as another array \a x.
    * Returns a reference to this object.
    */
   Array& operator =( const Array& x )
   {
      Assign( x );
      return *this;
   }

   /*!
    * Assigns an array \a x to this array.
    *
    * Decrements the reference counter of the current array data, and destroys
    * it if it becomes unreferenced.
    *
    * Increments the reference counter of the source array's data and
    * references it in this array.
    */
   void Assign( const Array& x )
   {
      x.m_data->Attach();
      DetachFromData();
      m_data = x.m_data;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   Array& operator =( Array&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * Transfers data from another array \a x to this object.
    *
    * Decrements the reference counter of the current array data, and destroys
    * it if it becomes unreferenced.
    *
    * Transfers source array data to this object, leaving empty and invalid the
    * source object \a x.
    */
   void Transfer( Array& x )
   {
      DetachFromData();
      m_data = x.m_data;
      x.m_data = nullptr;
   }

   /*!
    * Transfers data from another array \a x to this object.
    *
    * Decrements the reference counter of the current array data, and destroys
    * it if it becomes unreferenced.
    *
    * Transfers source array data to this object, leaving empty and invalid the
    * source object \a x.
    */
   void Transfer( Array&& x )
   {
      DetachFromData();
      m_data = x.m_data;
      x.m_data = nullptr;
   }

   /*!
    * Replaces the contents of this array with a sequence of \a n copies of an
    * object \a v.
    */
   void Assign( const T& v, size_type n = 1 )
   {
      if ( n > 0 )
      {
         if ( !IsUnique() )
         {
            Data* newData = new Data;
            DetachFromData();
            m_data = newData;
         }

         if ( Capacity() < n )
         {
            m_data->Deallocate();
            m_data->Allocate( n );
         }
         else
         {
            m_data->Destroy( m_data->begin, m_data->end );
            m_data->end = m_data->begin + n;
         }

         m_data->Initialize( m_data->begin, m_data->end, v );
      }
      else
         Clear();
   }

   /*!
    * Replaces the contents of this array with a copy of the sequence defined
    * by the range [i,j) of forward iterators.
    *
    * \note \a i and \a j must not be iterators into this array.
    */
   template <class FI>
   void Assign( FI i, FI j )
   {
      size_type n = size_type( pcl::Distance( i, j ) );
      if ( n > 0 )
      {
         if ( !IsUnique() )
         {
            Data* newData = new Data;
            DetachFromData();
            m_data = newData;
         }

         if ( Capacity() < n )
         {
            m_data->Deallocate();
            m_data->Allocate( n );
         }
         else
         {
            m_data->Destroy( m_data->begin, m_data->end );
            m_data->end = m_data->begin + n;
         }

         m_data->Build( m_data->begin, i, j );
      }
      else
         Clear();
   }

   /*!
    * Causes this array to contain the sequence of objects defined by the range
    * [i,j) of array iterators. The previously referenced data structure is
    * dereferenced and destroyed/deallocated if it becomes unreferenced.
    *
    * After calling this function, this array, or a subsequently created alias,
    * will own and eventually destroy and deallocate the specified sequence of
    * contiguous objects.
    */
   void Import( iterator i, iterator j )
   {
      if ( i >= m_data->available || j <= m_data->begin )
      {
         Clear();
         size_type n = size_type( pcl::Distance( i, j ) );
         if ( n > 0 )
         {
            EnsureUnique();
            m_data->begin = i;
            m_data->end = m_data->available = j;
         }
      }
   }

   /*!
    * Releases the data contained by this array.
    *
    * This member function returns a pointer to the internal data block
    * referenced by this object, after ensuring that it is uniquely referenced.
    * If the array is empty, this function may return the null pointer.
    *
    * Before returning, this member function empties this array without
    * deallocating its contained data. The caller is then responsible for
    * destructing and/or deallocating the returned block when it is no longer
    * required.
    */
   iterator Release()
   {
      EnsureUnique();
      iterator b = m_data->begin;
      m_data->begin = m_data->end = m_data->available = nullptr;
      return b;
   }

   /*!
    * Inserts a contiguous sequence of \a n default-constructed objects at the
    * specified location \a i in this array.
    *
    * The insertion point \a i is constrained to stay in the range
    * [Begin(),End()) of existing array elements.
    *
    * Returns an iterator pointing to the first newly created array element, or
    * \a i if \a n is zero.
    */
   iterator Grow( iterator i, size_type n = 1 )
   {
      i = pcl::Range( i, m_data->begin, m_data->end );
      if ( n > 0 )
      {
         UniquifyIterator( i );
         m_data->Initialize( i = m_data->UninitializedGrow( i, n ), n );
      }
      return i;
   }

   /*!
    * Appends a contiguous sequence of \a n default-constructed objects to this
    * array. This operation is equivalent to:
    *
    * \code Grow( End(), n ) \endcode
    *
    * Returns an iterator pointing to the first newly created array element, or
    * End() if \a n is zero.
    */
   iterator Expand( size_type n = 1 )
   {
      return Grow( m_data->end, n );
   }

   /*!
    * Removes a contiguous trailing sequence of \a n existing objects from this
    * array. This operation is equivalent to:
    *
    * \code Truncate( End() - n ) \endcode
    *
    * If the specified count \a n is greater than or equal to the length of
    * this array, this function calls Clear() to yield an empty array.
    */
   void Shrink( size_type n = 1 )
   {
      if ( n < m_data->Length() )
         Truncate( m_data->end - n );
      else
         Clear();
   }

   /*!
    * Resizes this array to the specified length \a n, either by appending new
    * default-constructed objects, or by removing existing trailing objects.
    * This operation is equivalent to:
    *
    * \code
    * if ( n > Length() )
    *    Expand( n - Length() );
    * else
    *    Shrink( Length() - n );
    * \endcode
    */
   void Resize( size_type n )
   {
      size_type l = m_data->Length();
      if ( n > l )
         Expand( n - l );
      else
         Shrink( l - n );
   }

   /*!
    * Inserts a copy of the objects in a direct container \a x at the specified
    * location \a i in this array.
    *
    * The insertion point \a i is constrained to stay in the range
    * [Begin(),End()) of existing array elements. The source array \a x can
    * safely be a reference to this array.
    *
    * Returns an iterator pointing to the first newly created array element, or
    * \a i if \a x is empty.
    */
   iterator Insert( iterator i, const Array& x )
   {
      if ( &x != this )
         return Insert( i, x.Begin(), x.End() );
      Array t( *this );
      t.EnsureUnique();
      return Insert( i, t.m_data->begin, t.m_data->end );
   }

   /*!
    * Inserts a contiguous sequence of \a n copies of the object \a v at the
    * specified location \a i in this array.
    *
    * The insertion point \a i is constrained to stay in the range
    * [Begin(),End()) of existing array elements.
    *
    * Returns an iterator pointing to the first inserted array element, or \a i
    * if \a n is zero.
    */
   iterator Insert( iterator i, const T& v, size_type n = 1 )
   {
      i = pcl::Range( i, m_data->begin, m_data->end );
      if ( n > 0 )
      {
         UniquifyIterator( i );
         m_data->Initialize( i = m_data->UninitializedGrow( i, n ), n, v );
      }
      return i;
   }

   /*!
    * Inserts a copy of the sequence of objects defined by the range [p,q) of
    * forward iterators at the specified location \a i in this array.
    *
    * The insertion point \a i is constrained to stay in the range
    * [Begin(),End()) of existing array elements.
    *
    * Returns an iterator pointing to the first inserted array element, or \a i
    * if \a q <= \a p.
    *
    * \note \a p and \a q must not be iterators into this array.
    */
   template <class FI>
   iterator Insert( iterator i, FI p, FI q )
   {
      i = pcl::Range( i, m_data->begin, m_data->end );
      size_type n = size_type( pcl::Distance( p, q ) );
      if ( n > 0 )
      {
         UniquifyIterator( i );
         m_data->Build( i = m_data->UninitializedGrow( i, n ), p, q );
      }
      return i;
   }

   /*!
    * Appends a copy of the objects stored in the array \a x to this array.
    */
   void Append( const Array& x )
   {
      Insert( m_data->end, x );
   }

   /*!
    * Appends a contiguous sequence of \a n copies of the object \a v to this
    * array.
    */
   void Append( const T& v, size_type n = 1 )
   {
      Insert( m_data->end, v, n );
   }

   /*!
    * Appends a copy of the sequence of objects defined by the range [p,q)
    * of forward iterators to this array.
    *
    * \note \a p and \a q must not be iterators into this array.
    */
   template <class FI>
   void Append( FI p, FI q )
   {
      Insert( m_data->end, p, q );
   }

   /*!
    * Inserts a copy of the objects stored in the array \a x at the beginning
    * of this array.
    */
   void Prepend( const Array& x )
   {
      Insert( m_data->begin, x );
   }

   /*!
    * Inserts a contiguous sequence of \a n copies of the object \a v at
    * the beginning of this array.
    */
   void Prepend( const T& v, size_type n = 1 )
   {
      Insert( m_data->begin, v, n );
   }

   /*!
    * Inserts a copy of the sequence of objects defined by the range [p,q) of
    * forward iterators at the beginning of this array.
    *
    * \note \a p and \a q must not be iterators into this array.
    */
   template <class FI>
   void Prepend( FI p, FI q )
   {
      Insert( m_data->begin, p, q );
   }

   /*!
    * A synonym for Append( const Array<>& ).
    */
   void Add( const Array& x )
   {
      Append( x );
   }

   /*!
    * A synonym for Append( const T&, size_type )
    */
   void Add( const T& v, size_type n = 1 )
   {
      Append( v, n );
   }

   /*!
    * A synonym for Append( FI, FI )
    */
   template <class FI>
   void Add( FI p, FI q )
   {
      Append( p, q );
   }

   /*!
    * Destroys and removes a sequence of \a n contiguous objects starting at
    * the specified location \a i in this array.
    *
    * If the starting iterator \a i is located at or after the end of this
    * array, or if \a n is zero, this function does nothing. Otherwise \a i is
    * constrained to stay in the range [Begin(),End()) of existing array
    * elements.
    */
   void Remove( iterator i, size_type n = 1 )
   {
      Remove( i, i+n );
   }

   /*!
    * Destroys and removes a sequence of contiguous objects in the range [i,j)
    * of this array.
    *
    * If the starting iterator \a i is located at or after the end of this
    * array, or if \a j precedes \a i, this function does nothing. Otherwise
    * the range [i,j) is constrained to stay in the range [Begin(),End()) of
    * existing array elements.
    */
   void Remove( iterator i, iterator j )
   {
      if ( i < m_data->end )
         if ( i < j )
         {
            i = pcl::Max( m_data->begin, i );
            j = pcl::Min( j, m_data->end );
            if ( i > m_data->begin || j < m_data->end )
            {
               UniquifyIterators( i, j );
               m_data->Destroy( j = pcl::Copy( i, j, m_data->end ), m_data->end );
               m_data->end = j;
            }
            else
               Clear();
         }
   }

   /*!
    * Destroys and removes a trailing sequence of contiguous objects from the
    * specified iterator of this array. This operation is equivalent to:
    *
    * \code Remove( i, End() ) \endcode
    *
    * If the specified iterator is located at or after the end of this array,
    * this function does nothing. Otherwise the iterator is constrained to stay
    * in the range [Begin(),End()) of existing array elements.
    */
   void Truncate( iterator i )
   {
      Remove( i, m_data->end );
   }

   /*!
    * Destroys and removes all existing objects equal to the specified value
    * \a v in this array.
    */
   void Remove( const T& v )
   {
      Array a;
      for ( iterator i = m_data->begin, j = i; ; ++j )
      {
         if ( j == m_data->end )
         {
            if ( i != m_data->begin )
            {
               a.Add( i, j );
               Transfer( a );
            }
            break;
         }

         if ( *j == v )
         {
            a.Add( i, j );
            i = j;
            ++i;
         }
      }
   }

   /*!
    * Destroys and removes every object x in this array such that the binary
    * predicate p( x, \a v ) is true.
    */
   template <class BP>
   void Remove( const T& v, BP p )
   {
      Array a;
      for ( iterator i = m_data->begin, j = i; ; ++j )
      {
         if ( j == m_data->end )
         {
            if ( i != m_data->begin )
            {
               a.Add( i, j );
               Transfer( a );
            }
            break;
         }

         if ( p( *j, v ) )
         {
            a.Add( i, j );
            i = j;
            ++i;
         }
      }
   }

   /*!
    * Removes and possibly destroys all objects contained, yielding an empty
    * array.
    *
    * If this array is empty, then calling this member function has no effect.
    *
    * If this array uniquely references its internal array data structure, all
    * objects contained are destroyed and deallocated; otherwise its reference
    * counter is decremented and a new, empty array data structure is created
    * and uniquely referenced.
    */
   void Clear()
   {
      if ( !IsEmpty() )
         if ( IsUnique() )
            m_data->Deallocate();
         else
         {
            Data* newData = new Data;
            DetachFromData();
            m_data = newData;
         }
   }

   /*!
    * Replaces a sequence of contiguous objects defined by the range [i,j) of
    * iterators in this array by the objects stored in an array \a x.
    *
    * If the starting iterator \a i is located at or after the end of this
    * array, or if \a j precedes \a i, this function does nothing. Otherwise
    * the range [i,j) is constrained to stay in the range [Begin(),End()) of
    * existing array elements.
    *
    * Returns an iterator pointing to the first replaced array element, \a i
    * if no elements are replaced, or \c nullptr if the resulting array is
    * empty.
    */
   iterator Replace( iterator i, iterator j, const Array& x )
   {
      if ( &x != this )
         return Replace( i, j, x.Begin(), x.End() );
      Array t( *this );
      t.EnsureUnique();
      return Replace( i, j, t.ConstBegin(), t.ConstEnd() );
   }

   /*!
    * Replaces a sequence of contiguous objects defined by the range [i,j) of
    * iterators in this array by \a n copies of the specified object \a v.
    *
    * If the starting iterator \a i is located at or after the end of this
    * array, or if \a j precedes \a i, this function does nothing. Otherwise
    * the range [i,j) is constrained to stay in the range [Begin(),End()) of
    * existing array elements.
    *
    * Returns an iterator pointing to the first replaced array element, \a i
    * if no elements are replaced, or \c nullptr if the resulting array is
    * empty.
    */
   iterator Replace( iterator i, iterator j, const T& v, size_type n = 1 )
   {
      i = pcl::Range( i, m_data->begin, m_data->end );
      j = pcl::Range( j, m_data->begin, m_data->end );
      if ( i < j )
         if ( i < m_data->end )
         {
            UniquifyIterators( i, j );
            size_type d = size_type( j - i );
            if ( d < n )
            {
               m_data->Destroy( i, j );
               m_data->Initialize( i = m_data->UninitializedGrow( i, n-d ), n, v );
            }
            else
            {
               iterator k = i + n;
               pcl::Fill( i, k, v );
               Remove( k, j );
               if ( m_data->begin == nullptr )
                  i = nullptr;
            }
         }
      return i;
   }

   /*!
    * Replaces a sequence of contiguous objects defined by the range [i,j) of
    * iterators in this array by the sequence of objects in the range [p,q) of
    * forward iterators.
    *
    * If the starting iterator \a i is located at or after the end of this
    * array, or if \a j precedes \a i, this function does nothing. Otherwise
    * the range [i,j) is constrained to stay in the range [Begin(),End()) of
    * existing array elements.
    *
    * Returns an iterator pointing to the first replaced array element, \a i
    * if no elements are replaced, or \c nullptr if the resulting array is
    * empty.
    *
    * \note \a p and \a q must not be iterators into this array.
    */
   template <class FI>
   iterator Replace( iterator i, iterator j, FI p, FI q )
   {
      i = pcl::Range( i, m_data->begin, m_data->end );
      j = pcl::Range( j, m_data->begin, m_data->end );
      if ( i < j )
         if ( i < m_data->end )
         {
            UniquifyIterators( i, j );
            size_type d = size_type( j - i );
            size_type n = size_type( pcl::Distance( p, q ) );
            if ( d < n )
            {
               m_data->Destroy( i, j );
               m_data->Build( i = m_data->UninitializedGrow( i, n-d ), p, q );
            }
            else
            {
               Remove( pcl::Move( i, p, q ), j );
               if ( m_data->begin == nullptr )
                  i = nullptr;
            }
         }
      return i;
   }

   /*!
    * Ensures that this array has enough capacity to store \a n objects.
    *
    * After calling this member function with \a n > 0, this object is
    * guaranteed to uniquely reference its array data.
    */
   void Reserve( size_type n )
   {
      if ( n > 0 )
         if ( IsUnique() )
         {
            if ( Capacity() < n )
            {
               iterator b = m_data->alloc.Allocate( n );
               iterator e = m_data->Build( b, m_data->begin, m_data->end );
               m_data->Deallocate();
               m_data->begin = b;
               m_data->end = e;
               m_data->available = m_data->begin + n;
            }
         }
         else
         {
            Data* newData = new Data;
            newData->begin = newData->alloc.Allocate( n = pcl::Max( Length(), n ) );
            newData->end = newData->Build( newData->begin, m_data->begin, m_data->end );
            newData->available = newData->begin + n;
            DetachFromData();
            m_data = newData;
         }
   }

   /*!
    * Causes this array to allocate the exact required memory space to store
    * its contained objects.
    *
    * If the array has excess capacity, a new copy of its contained objects
    * is generated and stored in a newly allocated memory block that fits them
    * exactly, then the previous memory block is deallocated.
    *
    * If the array is empty, calling this function is equivalent to Clear().
    * Note that in this case a previously allocated memory block (by a call to
    * Reserve()) may also be deallocated.
    */
   void Squeeze()
   {
      if ( IsUnique() )
      {
         if ( Available() > 0 )
         {
            iterator b = m_data->alloc.Allocate( Length() );
            iterator e = m_data->Build( b, m_data->begin, m_data->end );
            m_data->Deallocate();
            m_data->begin = b;
            m_data->end = m_data->available = e;
         }
      }
      else
      {
         Data* newData = new Data;
         if ( !IsEmpty() )
         {
            newData->begin = newData->alloc.Allocate( Length() );
            newData->available = newData->end = newData->Build( newData->begin, m_data->begin, m_data->end );
         }
         DetachFromData();
         m_data = newData;
      }
   }

   /*!
    * Sets all objects contained by this array equal to \a v.
    */
   void Fill( const T& v )
   {
      EnsureUnique();
      pcl::Fill( m_data->begin, m_data->end, v );
   }

   /*!
    * Securely fills this array and all instances sharing its data with the
    * specified value \a v.
    *
    * The normal data sharing mechanism is ignored on purpose by this member
    * function, so if there are other objects sharing the same array data, all
    * of them will be affected unconditionally.
    *
    * This function is useful to ensure that sensitive data, such as user
    * passwords and user names, are destroyed without the risk of surviving
    * duplicates as a result of implicit data sharing.
    */
   void SecureFill( const T& v )
   {
      pcl::Fill( m_data->begin, m_data->end, v );
   }

   /*!
    * Calls f( T& x ) for every object x contained by this array, successively
    * from the first contained object to the last one.
    */
   template <class F>
   void Apply( F f )
   {
      EnsureUnique();
      pcl::Apply( m_data->begin, m_data->end, f );
   }

   /*!
    * Calls f( const T& x ) for every object x contained by this array,
    * successively from the first contained object to the last one.
    */
   template <class F>
   void Apply( F f ) const
   {
      pcl::Apply( m_data->begin, m_data->end, f );
   }

   /*!
    * Returns an iterator pointing to the first object x in this array such
    * that f( const T& x ) is true. Returns End() if such object does not
    * exist.
    */
   template <class F>
   iterator FirstThat( F f ) const
   {
      return const_cast<iterator>( pcl::FirstThat( m_data->begin, m_data->end, f ) );
   }

   /*!
    * Returns an iterator pointing to the last object x in this array such
    * that f( const T& x ) is true. Returns End() if such object does not
    * exist.
    */
   template <class F>
   iterator LastThat( F f ) const
   {
      return const_cast<iterator>( pcl::LastThat( m_data->begin, m_data->end, f ) );
   }

   /*! #
    */
   size_type Count( const T& v ) const
   {
      return pcl::Count( m_data->begin, m_data->end, v );
   }

   /*! #
    */
   template <class BP>
   size_type Count( const T& v, BP p ) const
   {
      return pcl::Count( m_data->begin, m_data->end, v, p );
   }

   /*! #
    */
   template <class UP>
   size_type CountIf( UP p ) const
   {
      return pcl::CountIf( m_data->begin, m_data->end, p );
   }

   /*! #
    */
   iterator MinItem() const
   {
      return const_cast<iterator>( pcl::MinItem( m_data->begin, m_data->end ) );
   }

   /*! #
    */
   template <class BP>
   iterator MinItem( BP p ) const
   {
      return const_cast<iterator>( pcl::MinItem( m_data->begin, m_data->end, p ) );
   }

   /*! #
    */
   iterator MaxItem() const
   {
      return const_cast<iterator>( pcl::MaxItem( m_data->begin, m_data->end ) );
   }

   /*! #
    */
   template <class BP>
   iterator MaxItem( BP p ) const
   {
      return const_cast<iterator>( pcl::MaxItem( m_data->begin, m_data->end, p ) );
   }

   /*! #
    */
   void Reverse()
   {
      EnsureUnique();
      pcl::Reverse( m_data->begin, m_data->end );
   }

   /*! #
    */
   void Rotate( distance_type n )
   {
      if ( Length() > 1 && n != 0 )
      {
         EnsureUnique();
         if ( (n %= Length()) < 0 )
            n += Length();
         pcl::Rotate( m_data->begin, m_data->begin+n, m_data->end );
      }
   }

   /*! #
    */
   void ShiftLeft( const T& v, size_type n = 1 )
   {
      if ( !IsEmpty() && n > 0 )
      {
         EnsureUnique();
         if ( n >= Length() )
            pcl::Fill( m_data->begin, m_data->end, v );
         else
            pcl::ShiftLeft( m_data->begin, m_data->begin+n, m_data->end, v );
      }
   }

   /*! #
    */
   void ShiftRight( const T& v, size_type n = 1 )
   {
      if ( !IsEmpty() && n > 0 )
      {
         EnsureUnique();
         if ( n >= Length() )
            pcl::Fill( m_data->begin, m_data->end, v );
         else
            pcl::ShiftRight( m_data->begin, m_data->end-n, m_data->end, v );
      }
   }

   /*! #
    */
   iterator Search( const T& v ) const
   {
      return const_cast<iterator>( pcl::LinearSearch( m_data->begin, m_data->end, v ) );
   }

   /*! #
    */
   template <class BP>
   iterator Search( const T& v, BP p ) const
   {
      return const_cast<iterator>( pcl::LinearSearch( m_data->begin, m_data->end, v, p ) );
   }

   /*! #
    */
   iterator SearchLast( const T& v ) const
   {
      return const_cast<iterator>( pcl::LinearSearchLast( m_data->begin, m_data->end, v ) );
   }

   /*! #
    */
   template <class BP>
   iterator SearchLast( const T& v, BP p ) const
   {
      return const_cast<iterator>( pcl::LinearSearchLast( m_data->begin, m_data->end, v, p ) );
   }

   /*! #
    */
   template <class FI>
   iterator SearchSubset( FI i, FI j ) const
   {
      return const_cast<iterator>( pcl::Search( m_data->begin, m_data->end, i, j ) );
   }

   /*! #
    */
   template <class FI, class BP>
   iterator SearchSubset( FI i, FI j, BP p ) const
   {
      return const_cast<iterator>( pcl::Search( m_data->begin, m_data->end, i, j, p ) );
   }

   /*! #
    */
   template <class C>
   iterator SearchSubset( const C& x ) const
   {
      PCL_ASSERT_DIRECT_CONTAINER( C, T );
      return const_cast<iterator>( pcl::Search( m_data->begin, m_data->end, x.Begin(), x.End() ) );
   }

   /*! #
    */
   template <class C, class BP>
   iterator SearchSubset( const C& x, BP p ) const
   {
      PCL_ASSERT_DIRECT_CONTAINER( C, T );
      return const_cast<iterator>( pcl::Search( m_data->begin, m_data->end, x.Begin(), x.End(), p ) );
   }

   /*! #
    */
   template <class BI>
   iterator SearchLastSubset( BI i, BI j ) const
   {
      return const_cast<iterator>( pcl::SearchLast( m_data->begin, m_data->end, i, j ) );
   }

   /*! #
    */
   template <class BI, class BP>
   iterator SearchLastSubset( BI i, BI j, BP p ) const
   {
      return const_cast<iterator>( pcl::SearchLast( m_data->begin, m_data->end, i, j, p ) );
   }

   /*! #
    */
   template <class C>
   iterator SearchLastSubset( const C& x ) const
   {
      PCL_ASSERT_DIRECT_CONTAINER( C, T );
      return const_cast<iterator>( pcl::SearchLast( m_data->begin, m_data->end, x.Begin(), x.End() ) );
   }

   /*! #
    */
   template <class C, class BP>
   iterator SearchLastSubset( const C& x, BP p ) const
   {
      PCL_ASSERT_DIRECT_CONTAINER( C, T );
      return const_cast<iterator>( pcl::SearchLast( m_data->begin, m_data->end, x.Begin(), x.End(), p ) );
   }

   /*! #
    */
   bool Contains( const T& v ) const
   {
      return Search( v ) != m_data->end;
   }

   /*! #
    */
   template <class BP>
   bool Contains( const T& v, BP p ) const
   {
      return Search( v, p ) != m_data->end;
   }

   /*! #
    */
   template <class FI>
   iterator ContainsSubset( FI i, FI j ) const
   {
      return SearchSubset( i, j ) != m_data->end;
   }

   /*! #
    */
   template <class FI, class BP>
   iterator ContainsSubset( FI i, FI j, BP p ) const
   {
      return SearchSubset( i, j, p ) != m_data->end;
   }

   /*! #
    */
   template <class C>
   iterator ContainsSubset( const C& c ) const
   {
      return SearchSubset( c ) != m_data->end;
   }

   /*! #
    */
   template <class C, class BP>
   iterator ContainsSubset( const C& c, BP p ) const
   {
      return SearchSubset( c, p ) != m_data->end;
   }

   /*! #
    */
   void Sort()
   {
      EnsureUnique();
      pcl::QuickSort( m_data->begin, m_data->end );
   }

   /*! #
    */
   template <class BP>
   void Sort( BP p )
   {
      EnsureUnique();
      pcl::QuickSort( m_data->begin, m_data->end, p );
   }

   /*!
    * Exchanges two dynamic arrays \a x1 and \a x2.
    */
   friend void Swap( Array& x1, Array& x2 )
   {
      pcl::Swap( x1.m_data, x2.m_data );
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
      const_iterator i = m_data->begin;
      if ( i < m_data->end )
      {
         s.Append( S( *i ) );
         if ( ++i < m_data->end )
            do
            {
               s.Append( separator );
               s.Append( S( *i ) );
            }
            while ( ++i < m_data->end );
      }
      return s;
   }

   /*!
    * Generates a sequence of string tokens separated with the specified
    * \a separator string by calling an \a append function. Returns a reference
    * to the target string \a s.
    *
    * For each element x in this array, this function appends a string
    * representation (known as a \e token) to the target string \a s by calling
    * the \a append function:
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
      const_iterator i = m_data->begin;
      if ( i < m_data->end )
      {
         append( s, S( *i ) );
         if ( ++i < m_data->end )
         {
            S p( separator );
            do
            {
               append( s, p );
               append( s, S( *i ) );
            }
            while ( ++i < m_data->end );
         }
      }
      return s;
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
      return ToSeparated( s, ',' );
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
      return ToSeparated( s, ' ' );
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
      return ToSeparated( s, '\t' );
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
      return pcl::Hash64( m_data->begin, m_data->Size(), seed );
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
      return pcl::Hash32( m_data->begin, m_data->Size(), seed );
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

   /*!
    * \struct Data
    * \internal
    * Reference-counted array data structure.
    */
   struct Data : public ReferenceCounter
   {
      iterator  begin     = nullptr; //!< Beginning of the dynamic array
      iterator  end       = nullptr; //!< End of the array
      iterator  available = nullptr; //!< End of the allocated block
      allocator alloc;               //!< The allocator object

      /*!
       * Constructs an empty array data structure.
       */
      Data() = default;

      /*!
       * Destroys an array data structure.
       */
      ~Data()
      {
         Deallocate();
      }

      /*!
       * Returns the size in bytes of the array.
       */
      size_type Size() const
      {
         return Length()*sizeof( T );
      }

      /*!
       * Returns the number of elements in the array.
       */
      size_type Length() const
      {
         return end - begin;
      }

      /*!
       * Returns the total capacity of the allocated block in array elements.
       */
      size_type Capacity() const
      {
         return available - begin;
      }

      /*!
       * Returns the number of array elements available in the allocated block.
       */
      size_type Available() const
      {
         return available - end;
      }

      /*!
       * Returns true iff the array is empty.
       */
      bool IsEmpty() const
      {
         return begin == end;
      }

      /*!
       * Allocates space to store at least \a n array elements, and updates
       * internal pointers to define an array of \a n elements.
       */
      void Allocate( size_type n )
      {
         if ( n > 0 )
         {
            size_type m = alloc.PagedLength( n );
            begin = alloc.Allocate( m );
            end = begin + n;
            available = begin + m;
         }
      }

      /*!
       * Deallocates array data, yielding an empty structure.
       */
      void Deallocate()
      {
         PCL_CHECK( (begin == nullptr) ? end == nullptr : begin < end )
         if ( begin != nullptr )
         {
            Destroy( begin, end );
            alloc.Deallocate( begin );
            begin = end = available = nullptr;
         }
      }

      /*!
       * Constructs array elements with default values in the range [i,j).
       */
      void Initialize( iterator i, iterator j )
      {
         for ( ; i < j; ++i )
            pcl::Construct( i, alloc );
      }

      /*!
       * Constructs \a n consecutive array elements with default values,
       * starting from the element pointed to by \a i.
       */
      void Initialize( iterator i, size_type n )
      {
         for ( ; n > 0; ++i, --n )
            pcl::Construct( i, alloc );
      }

      /*!
       * Constructs array elements with the specified value \a v in the range
       * [i,j).
       */
      void Initialize( iterator i, iterator j, const T& v )
      {
         for ( ; i < j; ++i )
            pcl::Construct( i, v, alloc );
      }

      /*!
       * Constructs \a n consecutive array elements with the specified value
       * \a v, starting from the element pointed to by \a i.
       */
      void Initialize( iterator i, size_type n, const T& v )
      {
         for ( ; n > 0; ++i, --n )
            pcl::Construct( i, v, alloc );
      }

      /*!
       * Copy-constructs array elements starting from the element pointed to by
       * \a i, with source elements in the range [p,q) of iterators. Returns an
       * iterator after the last constructed element.
       */
      template <class FI>
      iterator Build( iterator i, FI p, FI q )
      {
         for ( ; p != q; ++i, ++p )
            pcl::Construct( i, *p, alloc );
         return i;
      }

      /*!
       * Causes this array to grow by \a n uninitialized elements at the
       * array location corresponding to the specified iterator \a i. Returns
       * an iterator pointing to the first newly created element.
       */
      iterator UninitializedGrow( iterator i, size_type n )
      {
         if ( n > 0 )
            if ( Available() >= n )
            {
               if ( i < end )
               {
                  iterator j1 = end;
                  iterator j2 = end + n;

                  for ( ;; )
                  {
                     pcl::Construct( --j2, *--j1, alloc );

                     if ( j1 == i )
                     {
                        j2 = end;
                        break;
                     }
                     else if ( j2 == end )
                     {
                        do
                           *--j2 = *--j1;
                        while ( j1 != i );
                        break;
                     }
                  }

                  Destroy( i, j2 );
               }
               end += n;
            }
            else
            {
               size_type m = alloc.PagedLength( Length()+n );
               iterator b = alloc.Allocate( m );
               iterator r = Build( b, begin, i );
               iterator e = Build( r+n, i, end );

               Deallocate();
               begin = b;
               end = e;
               available = b + m;
               i = r;
            }

         return i;
      }

      /*!
       * Destroys array elements in the range [i,j).
       */
      static void Destroy( iterator i, iterator j )
      {
         pcl::Destroy( i, j );
      }
   };

   /*!
    * \internal
    * The reference-counted array data.
    */
   Data* m_data = nullptr;

   /*!
    * \internal
    * Dereferences array data and disposes it if it becomes garbage.
    */
   void DetachFromData()
   {
      if ( !m_data->Detach() )
         delete m_data;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \defgroup array_relational_operators Array Relational Operators
 */

/*!
 * Returns true only if two arrays \a x1 and \a x2 are equal. This operator
 * compares the objects contained by both arrays.
 * \ingroup array_relational_operators
 */
template <class T, class A> inline
bool operator ==( const Array<T,A>& x1, const Array<T,A>& x2 )
{
   return x1.Length() == x2.Length() && pcl::Equal( x1.Begin(), x2.Begin(), x2.End() );
}

/*!
 * Returns true only if an array \a x1 precedes another array \a x2. This
 * operator compares the objects contained by both arrays.
 * \ingroup array_relational_operators
 */
template <class T, class A> inline
bool operator <( const Array<T,A>& x1, const Array<T,A>& x2 )
{
   return pcl::Compare( x1.Begin(), x1.End(), x2.Begin(), x2.End() ) < 0;
}

/*!
 * \defgroup array_insertion_operators Array Insertion Operators
 */

/*!
 * Appends an object \a v to an array \a x. Returns a reference to the array.
 *
 * The template argument type T must have conversion semantics from the type V,
 * such as T::T( const V& ) or equivalent.
 * \ingroup array_insertion_operators
 */
template <class T, class A, class V> inline
Array<T,A>& operator <<( Array<T,A>& x, const V& v )
{
   x.Append( T( v ) );
   return x;
}

/*!
 * Appends an object \a v to an array \a x. Returns a reference to the array.
 *
 * The template argument type T must have conversion semantics from the type V,
 * such as T::T( const V& ) or equivalent.
 * \ingroup array_insertion_operators
 */
template <class T, class A, class V> inline
Array<T,A>& operator <<( Array<T,A>&& x, const V& v )
{
   x.Append( T( v ) );
   return x;
}

/*!
 * Appends an array \a x2 to an array \a x1. Returns a reference to the
 * left-hand array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
Array<T,A>& operator <<( Array<T,A>& x1, const Array<T,A>& x2 )
{
   x1.Append( x2 );
   return x1;
}

/*!
 * Appends an array \a x2 to a temporary array \a x1. Returns a reference to
 * the left-hand array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
Array<T,A>& operator <<( Array<T,A>&& x1, const Array<T,A>& x2 )
{
   x1.Append( x2 );
   return x1;
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Array_h

// ----------------------------------------------------------------------------
// EOF pcl/Array.h - Released 2019-01-21T12:06:07Z
