//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// pcl/ReferenceArray.h - Released 2018-11-23T16:14:19Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_ReferenceArray_h
#define __PCL_ReferenceArray_h

/// \file pcl/ReferenceArray.h

#include <pcl/Diagnostics.h>

#include <pcl/Allocator.h>
#include <pcl/Container.h>
#include <pcl/IndirectArray.h>
#include <pcl/Iterator.h>
#include <pcl/StdAlloc.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ReferenceArray
 * \brief Dynamic array of pointers to objects providing direct iteration and
 * element access by reference.
 *
 * %ReferenceArray is a generic, finite ordered sequence of pointers to
 * objects, implemented as a reference-counted, dynamic array of pointers to T
 * instances. The type A provides dynamic allocation for contiguous sequences
 * of void* (StandardAllocator is used by default).
 *
 * Unlike IndirectArray, %ReferenceArray provides direct access to the objects
 * pointed to by its contained pointers, including direct iteration through
 * references instead of pointers. This makes %ReferenceArray a perfect
 * replacement for Array in cases where storing copies of objects is inviable
 * or impractical; for example, when the objects to be stored are unique by
 * nature, when the cost of a copy operation is excessive, or as the underlying
 * implementation of an heterogeneous container. As a prerequisite for this
 * functionality, %ReferenceArray, unlike IndirectArray, cannot contain null
 * pointers.
 *
 * \sa ReferenceSortedArray, IndirectArray, IndirectSortedArray, Array,
 * SortedArray, ReferenceCounter
 * \ingroup dynamic_arrays
 */
template <typename T, class A = StandardAllocator>
class PCL_CLASS ReferenceArray : public DirectContainer<T>
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
                                    indirect_iterator;

   /*! #
    */
   typedef typename array_implementation::const_iterator
                                    const_indirect_iterator;

   /*!
    * \class pcl::ReferenceArray::iterator
    * \brief Mutable %ReferenceArray iterator
    */
   class iterator : public Iterator<RandomAccessIterator, T>
   {
   public:

      typedef Iterator<RandomAccessIterator, T> iterator_base;

      typedef typename iterator_base::iterator_class
                                                iterator_class;

      typedef typename iterator_base::item_type item_type;

      /*!
       * Default constructor. Constructs an uninitialized iterator object.
       */
      iterator() : iterator_base(), it()
      {
      }

      /*!
       * Copy constructor.
       */
      iterator( const iterator& ) = default;

      /*!
       * Constructs a null iterator.
       */
      iterator( std::nullptr_t ) : iterator_base(), it( nullptr )
      {
      }

      /*!
       * Copy assignment operator. Returns a reference to this object.
       */
      iterator& operator =( const iterator& ) = default;

      /*!
       * Pointer-to-object conversion operator. Returns a pointer to the object
       * pointed to by this iterator.
       */
      operator T*() const
      {
         return *it;
      }

      /*!
       * Indirection operator. Returns a reference to the object pointed to by
       * this iterator.
       */
      T& operator *() const
      {
         return **it;
      }

      /*!
       * Structure selection operator. Returns a pointer to the object pointed
       * to by this iterator.
       */
      T* operator ->() const
      {
         return *it;
      }

      /*!
       * Preincrement operator. Increments this iterator so that it points to
       * the next object in the iterated container, then returns a reference to
       * this iterator.
       */
      iterator& operator ++()
      {
         ++it;
         return *this;
      }

      /*!
       * Postincrement operator. Increments this iterator so that it points to
       * the next object in the iterated container. Returns a copy of the
       * iterator as it was before incrementing it.
       */
      iterator operator ++( int )
      {
         indirect_iterator it0 = it;
         ++it;
         return it0;
      }

      /*!
       * Predecrement operator. Decrements this iterator so that it points to
       * the previous object in the iterated container, then returns a
       * reference to this iterator.
       */
      iterator& operator --()
      {
         --it;
         return *this;
      }

      /*!
       * Postdecrement operator. Decrements this iterator so that it points to
       * the previous object in the iterated container. Returns a copy of the
       * iterator as it was before decrementing it.
       */
      iterator operator --( int )
      {
         indirect_iterator it0 = it;
         --it;
         return it0;
      }

      /*!
       * Assignment/addition operator. Increments this iterator by a distance
       * \a d from its current position. Positive increments cause this
       * iterator to move forward by \a d elements. Negative increments move
       * this iterator backward by \a d elements. Returns a reference to this
       * iterator.
       */
      iterator& operator +=( distance_type d )
      {
         it += d;
         return *this;
      }

      /*!
       * Assignment/subtraction operator. Decrements this iterator by a
       * distance \a d from its current position. Positive increments cause
       * this iterator to move backward by \a d elements. Negative increments
       * move this iterator forward by \a d elements. Returns a reference to
       * this iterator.
       */
      iterator& operator -=( distance_type d )
      {
         it -= d;
         return *this;
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to this iterator incremented by a distance \a d.
       */
      iterator operator +( distance_type d ) const
      {
         return iterator( it + d );
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * this iterator decremented by a distance \a d.
       */
      iterator operator -( distance_type d ) const
      {
         return iterator( it - d );
      }

      /*!
       * Iterator subtraction operator. Returns the distance (in container
       * elements) between this iterator and another iterator \a i.
       */
      distance_type operator -( const iterator& i ) const
      {
         return it - i.it;
      }

      /*!
       * Equality operator. Returns true if this iterator points to the same
       * object as another iterator \a i.
       */
      bool operator ==( const iterator& i ) const
      {
         return it == i.it;
      }

      /*!
       * Less than operator. Returns true if this iterator points to a
       * container element that precedes another iterator \a i.
       */
      bool operator <( const iterator& i ) const
      {
         return it < i.it;
      }

   private:

      indirect_iterator it;

      /*!
       * Constructor from an IndirectArray iterator (a pointer to a pointer to
       * an object in the iterated container).
       */
      explicit
      iterator( indirect_iterator i ) : iterator_base(), it( i )
      {
      }

      friend class ReferenceArray<T,A>;
      friend class ReferenceArray<T,A>::const_iterator;
   };

   /*!
    * \class pcl::ReferenceArray::const_iterator
    * \brief Immutable %ReferenceArray iterator
    */
   class const_iterator : public Iterator<RandomAccessIterator, T>
   {
   public:

      typedef Iterator<RandomAccessIterator, T> iterator_base;

      typedef typename iterator_base::iterator_class
                                                iterator_class;

      typedef typename iterator_base::item_type item_type;

      /*!
       * Default constructor. Constructs an uninitialized iterator object.
       */
      const_iterator() : iterator_base(), it()
      {
      }

      /*!
       * Constructor from non-const iterator.
       */
      const_iterator( const iterator& i ) : iterator_base(), it( i.it )
      {
      }

      /*!
       * Constructs a null iterator.
       */
      const_iterator( std::nullptr_t ) : iterator_base(), it( nullptr )
      {
      }

      /*!
       * Copy constructor.
       */
      const_iterator( const const_iterator& ) = default;

      /*!
       * Copy assignment operator. Returns a reference to this immutable
       * iterator.
       */
      const_iterator& operator =( const const_iterator& ) = default;

      /*!
       * Mutable iterator assignment operator. Returns a reference to this
       * immutable iterator object.
       */
      const_iterator& operator =( const iterator& i )
      {
         it = i.it;
         return *this;
      }

      /*!
       * Pointer-to-const-object conversion operator. Returns a pointer to the
       * immutable object pointed to by this iterator.
       */
      operator const T*() const
      {
         return *it;
      }

      /*!
       * Indirection operator. Returns a reference to the object pointed to by
       * this iterator.
       */
      const T& operator *() const
      {
         return **it;
      }

      /*!
       * Structure selection operator. Returns a pointer to the object pointed
       * to by this iterator.
       */
      const T* operator ->() const
      {
         return *it;
      }

      /*!
       * Preincrement operator. Increments this iterator so that it points to
       * the next object in the iterated container, then returns a reference to
       * this iterator.
       */
      const_iterator& operator ++()
      {
         ++it;
         return *this;
      }

      /*!
       * Postincrement operator. Increments this iterator so that it points to
       * the next object in the iterated container. Returns a copy of the
       * iterator as it was before incrementing it.
       */
      const_iterator operator ++( int )
      {
         const_indirect_iterator it0 = it;
         ++it;
         return it0;
      }

      /*!
       * Predecrement operator. Decrements this iterator so that it points to
       * the previous object in the iterated container, then returns a
       * reference to this iterator.
       */
      const_iterator& operator --()
      {
         --it;
         return *this;
      }

      /*!
       * Postdecrement operator. Decrements this iterator so that it points to
       * the previous object in the iterated container. Returns a copy of the
       * iterator as it was before decrementing it.
       */
      const_iterator operator --( int )
      {
         const_indirect_iterator it0 = it;
         --it;
         return it0;
      }

      /*!
       * Assignment/addition operator. Increments this iterator by a distance
       * \a d from its current position. Positive increments cause this
       * iterator to move forward by \a d elements. Negative increments move
       * this iterator backward by \a d elements. Returns a reference to this
       * iterator.
       */
      const_iterator& operator +=( distance_type d )
      {
         it += d;
         return *this;
      }

      /*!
       * Assignment/subtraction operator. Decrements this iterator by a
       * distance \a d from its current position. Positive increments cause
       * this iterator to move backward by \a d elements. Negative increments
       * move this iterator forward by \a d elements. Returns a reference to
       * this iterator.
       */
      const_iterator& operator -=( distance_type d )
      {
         it -= d;
         return *this;
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to this iterator incremented by a distance \a d.
       */
      const_iterator operator +( distance_type d ) const
      {
         return const_iterator( it + d );
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * this iterator decremented by a distance \a d.
       */
      const_iterator operator -( distance_type d ) const
      {
         return const_iterator( it - d );
      }

      /*!
       * Iterator subtraction operator. Returns the distance (in container
       * elements) between this iterator and another iterator \a i.
       */
      distance_type operator -( const const_iterator& i ) const
      {
         return it - i.it;
      }

      /*!
       * Iterator subtraction operator. Returns the distance (in container
       * elements) between this iterator and a mutable iterator \a i.
       */
      distance_type operator -( const iterator& i ) const
      {
         return it - i.it;
      }

      /*!
       * Equality operator. Returns true if this iterator points to the same
       * object as another iterator \a i.
       */
      bool operator ==( const const_iterator& i ) const
      {
         return it == i.it;
      }

      /*!
       * Equality operator. Returns true if this iterator points to the same
       * object as a mutable iterator \a i.
       */
      bool operator ==( const iterator& i ) const
      {
         return it == i.it;
      }

      /*!
       * Less than operator. Returns true if this iterator precedes another
       * iterator \a i.
       */
      bool operator <( const const_iterator& i ) const
      {
         return it < i.it;
      }

      /*!
       * Less than operator. Returns true if this iterator points to a
       * container element that precedes a mutable iterator \a i.
       */
      bool operator <( const iterator& i ) const
      {
         return it < i.it;
      }

   private:

      const_indirect_iterator it;

      /*!
       * Constructor from an IndirectArray iterator (a pointer to a pointer to
       * an object in the iterated container).
       */
      explicit
      const_iterator( const_indirect_iterator i ) : iterator_base(), it( i )
      {
      }

      friend class ReferenceArray<T,A>;
   };

   /*!
    * \class pcl::ReferenceArray::reverse_iterator
    * \brief Mutable %ReferenceArray reverse iterator.
    */
   typedef ReverseRandomAccessIterator<iterator, T>
                                    reverse_iterator;

   /*!
    * \class pcl::ReferenceArray::const_reverse_iterator
    * \brief Immutable %ReferenceArray reverse iterator.
    */
   typedef ReverseRandomAccessIterator<const_iterator, const T>
                                    const_reverse_iterator;

   // -------------------------------------------------------------------------

   /*!
    * Constructs an empty reference array.
    */
   ReferenceArray() : m_array()
   {
   }

   /*!
    * Constructs a reference array that stores \a n copies of a non-null
    * pointer \a p.
    *
    * If \a p is \c nullptr, this function constructs an empty reference array.
    */
   ReferenceArray( size_type n, const T* p ) : m_array()
   {
      PCL_PRECONDITION( p != nullptr )
      if ( p != nullptr )
         m_array = array_implementation( n, p );
   }

   /*!
    * Constructs a reference array as a copy of the sequence of non-null
    * pointers defined by the range [i,j) of forward iterators.
    *
    * If the range [i,j) contains null pointers, these are ignored and hence
    * not included in the constructed reference array.
    */
   template <class FI>
   ReferenceArray( FI i, FI j ) : m_array()
   {
      for ( ; i != j; ++i )
         if ( *i != nullptr )
            m_array.Append( *i );
   }

   /*!
    * Copy constructor.
    */
   ReferenceArray( const ReferenceArray& x ) : m_array( x.m_array )
   {
   }

   /*!
    * Move constructor.
    */
   ReferenceArray( ReferenceArray&& x ) : m_array( std::move( x.m_array ) )
   {
   }

   /*!
    * Destroys a %ReferenceArray object.
    *
    * Deallocates the internal array of pointers to objects, but does not
    * destroy the pointed objects. To destroy them, you have to call Destroy()
    * or Delete() explicitly.
    */
   ~ReferenceArray()
   {
   }

   /*!
    * Returns true iff this reference array uniquely references its contained
    * array of pointers to objects.
    */
   bool IsUnique() const
   {
      return m_array.IsUnique();
   }

   /*!
    * Returns true iff this reference array is an alias of the array \a x.
    *
    * Two objects are aliases if both share the same data. Two reference
    * containers are aliases if they share a unique set of object pointers.
    */
   bool IsAliasOf( const ReferenceArray& x ) const
   {
      return m_array.IsAliasOf( x.m_array );
   }

   /*!
    * Ensures that this reference array uniquely references its contained
    * object pointers.
    *
    * If necessary, this member function generates a duplicate of the array
    * of pointers, references it, and then decrements the reference counter of
    * the original pointer array.
    */
   void EnsureUnique()
   {
      m_array.EnsureUnique();
   }

   /*!
    * Returns the total number of bytes required to store the array of object
    * pointers contained by this reference array.
    */
   size_type Size() const
   {
      return m_array.Size();
   }

   /*!
    * Returns the length of this reference array.
    */
   size_type Length() const
   {
      return m_array.Length();
   }

   /*!
    * Returns the capacity of this reference array. The capacity is the maximum
    * number of pointers to objects that this reference array can contain
    * without requiring a reallocation.
    */
   size_type Capacity() const
   {
      return m_array.Capacity();
   }

   /*!
    * Returns the length of the space available in this reference array, or
    * zero if this reference array cannot contain more pointers. The available
    * space is the number of pointers to objects that can be added to this
    * reference array without requiring a reallocation. It is equal to
    * Capacity() - Length() by definition.
    */
   size_type Available() const
   {
      return m_array.Available();
   }

   /*!
    * Returns true only if this reference array is valid. A reference array is
    * valid if it references an internal structure with an array of pointers,
    * even if it is an empty array.
    *
    * In general, all %ReferenceArray objects are valid with only two
    * exceptions:
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
      return m_array.IsValid();
   }

   /*!
    * Returns true iff this reference array is empty.
    */
   bool IsEmpty() const
   {
      return m_array.IsEmpty();
   }

   /*!
    * Returns the minimum legal index in this array (always zero). For empty
    * arrays, this function returns a meaningless value.
    */
   size_type LowerBound() const
   {
      return m_array.LowerBound();
   }

   /*!
    * Returns the maximum legal index in this array. It is equal to Length()-1.
    * For empty arrays, this function returns a meaningless value.
    */
   size_type UpperBound() const
   {
      return m_array.UpperBound();
   }

   /*!
    * Returns a reference to the allocator object used by this reference array.
    */
   const allocator& GetAllocator() const
   {
      return m_array.GetAllocator();
   }

   /*!
    * Sets a new allocator object for this reference array.
    */
   void SetAllocator( const allocator& a )
   {
      m_array.SetAllocator( a );
   }

   /*!
    * Returns a mutable reference array iterator located at the specified array
    * index \a i.
    */
   iterator At( size_type i )
   {
      return iterator( m_array.At( i ) );
   }

   /*!
    * Returns an immutable reference array iterator located at the specified
    * array index \a i.
    */
   const_iterator At( size_type i ) const
   {
      return const_iterator( m_array.At( i ) );
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
      return iterator( m_array.MutableIterator( i.it ) );
   }

   /*!
    * Returns a reference to the object at the specified array index
    * \a i. No bounds checking is performed.
    */
   T& operator []( size_type i )
   {
      return *m_array[i];
   }

   /*!
    * Returns a reference to the immutable object at the specified array
    * index \a i. No bounds checking is performed.
    */
   const T& operator []( size_type i ) const
   {
      return *m_array[i];
   }

   /*!
    * Returns a reference to the first object in this reference array.
    */
   T& operator *()
   {
      return **m_array.Begin();
   }

   /*!
    * Returns a reference to the unmodifiable first object in this reference
    * array.
    */
   const T& operator *() const
   {
      return **m_array.Begin();
   }

   /*!
    * Returns a mutable iterator located at the beginning of this array.
    */
   iterator Begin()
   {
      return iterator( m_array.Begin() );
   }

   /*!
    * Returns an immutable iterator located at the beginning of this array.
    */
   const_iterator Begin() const
   {
      return const_iterator( m_array.Begin() );
   }

   /*!
    * Returns an immutable iterator located at the beginning of this array.
    */
   const_iterator ConstBegin() const
   {
      return const_iterator( m_array.ConstBegin() );
   }

   /*!
    * Returns a mutable iterator located at the end of this array.
    */
   iterator End()
   {
      return iterator( m_array.End() );
   }

   /*!
    * Returns an immutable iterator located at the end of this array.
    */
   const_iterator End() const
   {
      return const_iterator( m_array.End() );
   }

   /*!
    * Returns an immutable iterator located at the end of this array.
    */
   const_iterator ConstEnd() const
   {
      return const_iterator(  m_array.ConstEnd() );
   }

   /*!
    * Returns a mutable reverse iterator located at the <em>reverse
    * beginning</em> of this reference array.
    *
    * The reverse beginning corresponds to the last object in the array.
    */
   reverse_iterator ReverseBegin()
   {
      return iterator( m_array.End()-1 );
   }

   /*!
    * Returns an immutable reverse iterator located at the <em>reverse
    * beginning</em> of this reference array.
    *
    * The reverse beginning corresponds to the last object in the array.
    */
   const_reverse_iterator ReverseBegin() const
   {
      return const_iterator( m_array.End()-1 );
   }

   /*!
    * Returns an immutable reverse iterator located at the <em>reverse
    * beginning</em> of this indirect array.
    *
    * The reverse beginning corresponds to the last pointer in the array.
    */
   const_reverse_iterator ConstReverseBegin() const
   {
      PCL_PRECONDITION( !IsEmpty() )
      return const_iterator( m_array.End()-1 );
   }

   /*!
    * Returns a mutable reverse iterator located at the <em>reverse end</em> of
    * this reference array.
    *
    * The reverse end corresponds to a (nonexistent) object immediately before
    * the first object in the array.
    */
   reverse_iterator ReverseEnd()
   {
      PCL_PRECONDITION( !IsEmpty() )
      return iterator( m_array.Begin()-1 );
   }

   /*!
    * Returns an immutable reverse iterator located at the <em>reverse end</em>
    * of this reference array.
    *
    * The reverse end corresponds to a (nonexistent) object immediately before
    * the first object in the array.
    */
   const_reverse_iterator ReverseEnd() const
   {
      PCL_PRECONDITION( !IsEmpty() )
      return const_iterator( m_array.Begin()-1 );
   }

   /*!
    * Returns an immutable reverse iterator located at the <em>reverse end</em>
    * of this reference array.
    *
    * The reverse end corresponds to a (nonexistent) object immediately before
    * the first object in the array.
    */
   const_reverse_iterator ConstReverseEnd() const
   {
      PCL_PRECONDITION( !IsEmpty() )
      return const_iterator( m_array.Begin()-1 );
   }

   /*!
    * Returns a reference to the first object in this reference array. This
    * function should never be called for an empty array.
    */
   T& First()
   {
      return **m_array.Begin();
   }

   /*!
    * Returns a reference to the first unmodifiable object in this reference
    * array. This function should never be called for an empty array.
    */
   const T& First() const
   {
      return **m_array.Begin();
   }

   /*!
    * Returns a reference to the last object in this reference array. This
    * function should never be called for an empty array.
    */
   T& Last()
   {
      return **m_array.ReverseBegin();
   }

   /*!
    * Returns a reference to the last unmodifiable object in this reference
    * array. This function should never be called for an empty array.
    */
   const T& Last() const
   {
      return **m_array.ReverseBegin();
   }

   /*!
    * Ensures that the specified iterator represents a pointer stored in a
    * uniquely referenced indirect array. If necessary, this function builds a
    * new, uniquely referenced copy of the underlying indirect array by calling
    * EnsureUnique().
    *
    * If the iterator \a i is changed, it is guaranteed to point to the object
    * at the same array index it was pointing to before calling this function.
    */
   void UniquifyIterator( iterator& i )
   {
      m_array.UniquifyIterator( i.it );
   }

   /*!
    * Ensures that the specified iterators represents a pointer stored in a
    * uniquely referenced indirect array. If necessary, this function builds a
    * new, uniquely referenced copy of the underlying indirect array by calling
    * EnsureUnique().
    *
    * If the iterators \a i and \a j are changed, they are guaranteed to point
    * to the objects at the same array indices they were pointing to before
    * calling this function.
    */
   void UniquifyIterators( iterator& i, iterator& j )
   {
      m_array.UniquifyIterators( i.it, j.it );
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
    * Causes this reference array to reference the same set of objects as
    * another array \a x. Returns a reference to this object.
    */
   ReferenceArray& operator =( const ReferenceArray& x )
   {
      Assign( x );
      return *this;
   }

   /*!
    * Assigns a reference array \a x to this reference array.
    *
    * Decrements the reference counter of the current array data, and destroys
    * it if it becomes unreferenced.
    *
    * Increments the reference counter of the source array's data and
    * references it in this array.
    */
   void Assign( const ReferenceArray& x )
   {
      m_array.Assign( x.m_array );
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   ReferenceArray& operator =( ReferenceArray&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * Transfers data from another reference array \a x to this object.
    *
    * Decrements the reference counter of the current array data, and destroys
    * it if it becomes unreferenced.
    *
    * Transfers source array data to this object, leaving empty and invalid the
    * source object \a x.
    */
   void Transfer( ReferenceArray& x )
   {
      m_array.Transfer( x.m_array );
   }

   /*!
    * Transfers data from another reference array \a x to this object.
    *
    * Decrements the reference counter of the current array data, and destroys
    * it if it becomes unreferenced.
    *
    * Transfers source array data to this object, leaving empty and invalid the
    * source object \a x.
    */
   void Transfer( ReferenceArray&& x )
   {
      m_array.Transfer( std::move( x.m_array ) );
   }

   /*!
    * Replaces the contents of this reference array with a sequence of \a n
    * pointers equal to \a p.
    *
    * if \a p is \c nullptr, this function yields an empty array.
    */
   void Assign( const T* p, size_type n = 1 )
   {
      if ( p != nullptr )
         m_array.Assign( p, n );
      else
         m_array.Clear();
   }

   /*!
    * Replaces the contents of this reference array with a copy of the sequence
    * of pointers defined by the range [i,j) of forward iterators.
    *
    * If the range [i,j) contains null pointers, these are ignored and hence
    * not included in this reference array.
    *
    * \note \a i and \a j must not be iterators into this array.
    */
   template <class FI>
   void Assign( FI i, FI j )
   {
      m_array.Clear();
      for ( ; i != j; ++i )
         if ( *i != nullptr )
            m_array.Append( *i );
   }

   /*!
    * Replaces the contents of this reference array with a set of pointers to
    * newly created copies of the objects stored in the specified container
    * \a x. This function works for both direct and indirect containers.
    *
    * Keep in mind that after calling this function (with a reference to a
    * nonempty container) this array will store newly allocated objects. You
    * should call Destroy() to deallocate these objects before destructing this
    * reference array in order to avoid a memory leak.
    */
   template <class C>
   void CloneAssign( const C& x )
   {
      PCL_ASSERT_CONTAINER( C, T );
      CloneObjects( x, (C*)nullptr );
   }

   /*!
    * Causes this reference array to store the sequence of pointers defined by
    * the range [i,j) of iterators. The previously referenced data structure is
    * dereferenced and deallocated if it becomes unreferenced.
    */
   void Import( iterator i, iterator j )
   {
      m_array.Import( i.it, j.it );
   }

   /*!
    * Releases the set of pointers contained by this reference array.
    *
    * This member function returns a pointer to the internal block of pointers
    * stored in this object, after ensuring that it is uniquely referenced.
    * If the array is empty, this function may return the null pointer.
    *
    * Before returning, this member function empties this array without
    * deallocating its contained data. The caller is then responsible for
    * deallocating the returned block when it is no longer required.
    */
   indirect_iterator Release()
   {
      return m_array.Release();
   }

   /*!
    * Inserts a copy of the sequence of pointers contained by the reference
    * array \a x at the specified location \a i in this reference array.
    *
    * The insertion point \a i is constrained to stay in the range
    * [Begin(),End()) of existing array elements. The source array \a x can
    * safely be a reference to this array.
    *
    * Returns an iterator pointing to the first newly created array element, or
    * \a i if \a x is empty.
    */
   iterator Insert( const_iterator i, const ReferenceArray& x )
   {
      return iterator( m_array.Insert( i.it, x.m_array ) );
   }

   /*!
    * Inserts a contiguous sequence of \a n non-null pointers equal to \a p at
    * the specified location \a i.
    *
    * If \a p is \c nullptr, this function has no effect. The insertion point
    * \a i is constrained to stay in the range [Begin(),End()) of existing
    * array elements.
    *
    * Returns an iterator pointing to the first inserted array element, or \a i
    * if \a n is zero or \a p is \c nullptr.
    */
   iterator Insert( const_iterator i, const T* p, size_type n = 1 )
   {
      return (p != nullptr) ? iterator( m_array.Insert( i.it, p, n ) ) :
                              iterator( const_cast<indirect_iterator>( i.it ) );
   }

   /*!
    * Inserts a copy of the sequence of pointers defined by the range [p,q)
    * of forward iterators at the specified location \a i in this reference
    * array.
    *
    * If the range [p,q) contains null pointers, these are ignored and not
    * inserted in this array. The insertion point \a i is constrained to stay
    * in the range [Begin(),End()) of existing array elements.
    *
    * Returns an iterator pointing to the first inserted array element, or \a i
    * if \a q <= \a p or no element in [p,q) is a non-null pointer.
    *
    * \note \a p and \a q must not be iterators into this array.
    */
   template <class FI>
   iterator Insert( const_iterator i, FI p, FI q )
   {
      const_indirect_iterator it = i.it;
      for ( ; p != q; ++p )
         if ( *p != nullptr )
            it = m_array.Insert( it, *p );
      return iterator( const_cast<indirect_iterator>( it ) );
   }

   /*!
    * Appends a copy of the sequence of pointers contained by the reference
    * array \a x to this array.
    */
   void Append( const ReferenceArray& x )
   {
      m_array.Append( x.m_array );
   }

   /*!
    * Appends a contiguous sequence of \a n pointers equal to \a p to this
    * reference array.
    *
    * If \a p is \c nullptr, this function has no effect.
    */
   void Append( const T* p, size_type n = 1 )
   {
      if ( p != nullptr )
         m_array.Append( p, n );
   }

   /*!
    * Appends a copy of the sequence of pointers defined by the range [p,q)
    * of forward iterators to this reference array.
    *
    * If the range [p,q) contains null pointers, these are ignored and not
    * inserted in this array.
    *
    * \note \a p and \a q must not be iterators into this array.
    */
   template <class FI>
   void Append( FI p, FI q )
   {
      for ( ; p != q; ++p )
         if ( *p != nullptr )
            m_array.Append( *p );
   }

   /*!
    * Inserts a copy of the sequence of pointers contained by the reference
    * array \a x at the beginning of this reference array.
    */
   void Prepend( const ReferenceArray& x )
   {
      m_array.Prepend( x.m_array );
   }

   /*!
    * Inserts a contiguous sequence of \a n pointers equal to \a p at the
    * beginning of this indirect array.
    *
    * If \a p is \c nullptr, this function has no effect.
    */
   void Prepend( const T* p, size_type n = 1 )
   {
      if ( p != nullptr )
         m_array.Prepend( p, n );
   }

   /*!
    * Inserts a copy of the sequence of pointers defined by the range [p,q) of
    * forward iterators at the beginning of this indirect array.
    *
    * If the range [p,q) contains null pointers, these are ignored and not
    * inserted in this array.
    *
    * \note \a p and \a q must not be iterators into this array.
    */
   template <class FI>
   void Prepend( FI p, FI q )
   {
      for ( ; p != q; ++p )
         if ( *p != nullptr )
            m_array.Prepend( *p );
   }

   /*!
    * A synonym for Append( const ReferenceArray<>& )
    */
   void Add( const ReferenceArray& x )
   {
      Append( x );
   }

   /*!
    * A synonym for Append( const T*, size_type )
    */
   void Add( const T* p, size_type n = 1 )
   {
      Append( p, n );
   }

   /*!
    * A synonym for Append( FI, FI )
    */
   template <class FI>
   void Add( FI i, FI j )
   {
      Append( i, j );
   }

   /*!
    * Removes a sequence of \a n contiguous pointers starting at the specified
    * location \a i in this reference array.
    *
    * Only pointers are removed by this function; the pointed objects are not
    * affected in any way.
    */
   void Remove( const_iterator i, size_type n = 1 )
   {
      m_array.Remove( i.it, n );
   }

   /*!
    * Removes a sequence of contiguous pointers in the range [i,j) of this
    * reference array.
    *
    * Only pointers are removed by this function; the pointed objects are not
    * affected in any way.
    */
   void Remove( const_iterator i, const_iterator j )
   {
      m_array.Remove( i.it, j.it );
   }

   /*!
    * Removes a trailing sequence of contiguous pointers from the specified
    * iterator of this reference array. This operation is equivalent to:
    *
    * \code Remove( i, End() ) \endcode
    *
    * If the specified iterator is located at or after the end of this array,
    * this function does nothing. Otherwise the iterator is constrained to stay
    * in the range [Begin(),End()) of existing array elements.
    *
    * Only pointers are removed by this function; the pointed objects are not
    * affected in any way.
    */
   void Truncate( const_iterator i )
   {
      m_array.Truncate( i.it );
   }

   /*!
    * Removes a contiguous trailing sequence of \a n existing pointers from
    * this reference array. This operation is equivalent to:
    *
    * \code Truncate( End() - n ) \endcode
    *
    * If the specified count \a n is greater than or equal to the length of
    * this array, this function calls Clear() to yield an empty array.
    *
    * Only pointers are removed by this function; the pointed objects are not
    * affected in any way.
    */
   void Shrink( size_type n = 1 )
   {
      m_array.Shrink( n );
   }

   /*!
    * Removes all existing pointers whose pointed objects are equal to the
    * specified value \a v in this reference array.
    *
    * Only pointers are removed by this function; the pointed objects are not
    * affected in any way.
    */
   void Remove( const T& v )
   {
      m_array.Remove( v );
   }

   /*!
    * Removes every pointer x in this reference array such that the binary
    * predicate p( *x, \a v ) is true.
    *
    * Only pointers are removed by this function; the pointed objects are not
    * affected in any way.
    */
   template <class BP>
   void Remove( const T& v, BP p )
   {
      m_array.Remove( v, p );
   }

   /*!
    * Removes all contained pointers equal to \a p in this reference array.
    *
    * Only pointers are removed by this function; the pointed objects are not
    * affected in any way.
    */
   void RemovePointer( const T* p )
   {
      m_array.Remove( p );
   }

   /*!
    * Removes all pointers contained by this object, yielding an empty
    * reference array.
    *
    * If this array is empty, then calling this member function has no effect.
    *
    * If this array uniquely references its internal array data structure, all
    * pointers contained are deallocated; otherwise its reference counter is
    * decremented. Then a new, empty array data structure is created and
    * uniquely referenced.
    *
    * Only pointers are removed by this function; the pointed objects are not
    * affected in any way.
    */
   void Clear()
   {
      m_array.Clear();
   }

   /*!
    * Destroys and removes a sequence of \a n contiguous objects, starting at
    * the specified location \a i in this reference array.
    *
    * This function destroys and deallocates the pointed objects, then removes
    * the corresponding pointers in this array. The array length is decreased
    * by the number of destroyed objects.
    *
    * \warning See Destroy( iterator, iterator ) for critical information on
    * this member function.
    */
   void Destroy( iterator i, size_type n = 1 )
   {
      m_array.Destroy( i.it, n );
   }

   /*!
    * Destroys and removes the objects in a range [i,j) of iterators in this
    * reference array.
    *
    * This function destroys and deallocates the pointed objects, then removes
    * the corresponding pointers in this array. The array length is decreased
    * by the number of destroyed objects.
    *
    * \warning This function is potentially dangerous. If the array contains
    * duplicate pointers in the specified range of iterators, this function
    * will lead to a crash as a result of multiple deletions. To minimize the
    * risk of multiple deletions, this function ignores the normal data sharing
    * mechanism so that all objects sharing the same array data structure will
    * correctly have the destroyed objects removed. However, be aware of
    * potential problems if other reference or indirect containers store
    * pointers to deleted objects in different data structures, which will be
    * invalid after calling this function.
    */
   void Destroy( iterator i, iterator j )
   {
      m_array.Destroy( i.it, j.it );
   }

   /*!
    * Destroys and removes all objects equal to \a v in this reference array.
    *
    * This function destroys and deallocates the pointed objects, then removes
    * the corresponding pointers in this array. The array length is decreased
    * by the number of destroyed objects.
    *
    * \warning See Destroy( iterator, iterator ) for critical information on
    * this member function.
    */
   void Destroy( const T& v )
   {
      m_array.Destroy( v );
   }

   /*!
    * Destroys and removes every object x in this reference array such that the
    * binary predicate p( x, \a v ) is true.
    *
    * This function destroys and deallocates the pointed objects, then removes
    * the corresponding pointers in this array. The array length is decreased
    * by the number of destroyed objects.
    *
    * \warning See Destroy( iterator, iterator ) for critical information on
    * this member function.
    */
   template <class BP>
   void Destroy( const T& v, BP p )
   {
      m_array.Destroy( v, p );
   }

   /*!
    * Destroys and removes all objects in this reference array, yielding an
    * empty array.
    *
    * \warning See Destroy( iterator, iterator ) for critical information on
    * this member function.
    */
   void Destroy()
   {
      m_array.Destroy();
   }

   /*!
    * Replaces a sequence of contiguous pointers defined by the range [i,j) of
    * iterators in this array by the pointers stored in a reference array \a x.
    *
    * If the starting iterator \a i is located at or after the end of this
    * array, or if \a j precedes \a i, this function does nothing. Otherwise
    * the range [i,j) is constrained to stay in the range [Begin(),End()) of
    * existing array elements.
    *
    * Returns an iterator pointing to the first replaced array element, \a i
    * if no elements are replaced, or a null iterator if the resulting array is
    * empty.
    */
   iterator Replace( const_iterator i, const_iterator j, const ReferenceArray& x )
   {
      return iterator( m_array.Replace( i.it, j.it, x.m_array ) );
   }

   /*!
    * Replaces a sequence of contiguous pointers defined by the range [i,j) in
    * this reference array by \a n copies of the specified non-null pointer
    * \a p.
    *
    * If \a p is \c nullptr, this function removes the subset [i,j) from this
    * array, as if \a n = 0 had been specified.
    */
   iterator Replace( const_iterator i, const_iterator j, const T* p, size_type n = 1 )
   {
      return iterator( m_array.Replace( i.it, j.it, p, (p != nullptr) ? n : size_type( 0 ) ) );
   }

   /*!
    * Replaces a sequence of contiguous pointers defined by the range [i,j) in
    * this reference array by the sequence of pointers in the range [p,q) of
    * forward iterators.
    *
    * If the range [p,q) contains null pointers, these are ignored and not
    * inserted in this array. If the starting iterator \a i is located at or
    * after the end of this array, or if \a j precedes \a i, this function does
    * nothing. Otherwise the range [i,j) is constrained to stay in the range
    * [Begin(),End()) of existing array elements.
    *
    * Returns an iterator pointing to the first replaced array element, \a i
    * if no elements are replaced, or a null iterator if the resulting array is
    * empty.
    *
    * \note \a p and \a q must not be iterators into this array.
    */
   template <class FI>
   iterator Replace( const_iterator i, const_iterator j, FI p, FI q )
   {
      array_implementation m;
      for ( ; p != q; ++p )
         if ( *p != nullptr )
            m.Append( *p );
      return iterator( m_array.Replace( i.it, j.it, m ) );
   }

   /*!
    * Ensures that this reference array has enough capacity to store \a n
    * pointers.
    *
    * After calling this member function with \a n > 0, this object is
    * guaranteed to uniquely reference its array data.
    */
   void Reserve( size_type n )
   {
      m_array.Reserve( n );
   }

   /*!
    * Causes this reference array to allocate the exact required memory space
    * to store its contained pointers.
    *
    * If the array has excess capacity, a new copy of its contained pointers is
    * generated and stored in a newly allocated memory block that fits them
    * exactly, then the previous memory block is deallocated.
    *
    * If the array is empty, calling this function is equivalent to Clear().
    * Note that in this case a previously allocated memory block (by a call to
    * Reserve()) may also be deallocated.
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
      pcl::Fill( Begin(), End(), v );
   }

   /*!
    * Calls f( T& x ) for every object x in this reference array, successively
    * from the first contained object to the last.
    */
   template <class F>
   void Apply( F f )
   {
      pcl::Apply( Begin(), End(), f );
   }

   /*!
    * Calls f( const T& x ) for every unmodifiable object x in this reference
    * array, successively from the first contained object to the last.
    */
   template <class F>
   void Apply( F f ) const
   {
      pcl::Apply( Begin(), End(), f );
   }

   /*!
    * Returns an iterator pointing to the first object x in this reference
    * array such that f( const T& x ) is true. Returns End() if such pointer
    * does not exist.
    */
   template <class F>
   iterator FirstThat( F f ) const
   {
      return iterator( const_cast<indirect_iterator>( pcl::FirstThat( Begin(), End(), f ).it ) );
   }

   /*!
    * Returns an iterator pointing to the last object x in this reference array
    * such that f( const T& x ) is true. Returns End() if such pointer does not
    * exist.
    */
   template <class F>
   iterator LastThat( F f ) const
   {
      return iterator( const_cast<indirect_iterator>( pcl::LastThat( Begin(), End(), f ).it ) );
   }

   /*!
    * Returns the number of objects equal to \a v in this reference array.
    */
   size_type Count( const T& v ) const
   {
      return pcl::Count( Begin(), End(), v );
   }

   /*!
    * Returns the number of pointers equal to \a p stored in this reference
    * array.
    *
    * If \a p is \c nullptr, this function \e should return zero --- or you are
    * in serious trouble!
    */
   size_type Count( const T* p ) const
   {
      return m_array.Count( p );
   }

   /*!
    * Returns the number of objects in this reference array such that for each
    * counted object x the binary predicate p( x, v ) returns true.
    */
   template <class BP>
   size_type Count( const T& v, BP p ) const
   {
      return pcl::Count( Begin(), End(), v, p );
   }

   /*!
    * Returns the number of objects in this reference array such that for each
    * counted object x the unary predicate p( x ) returns true.
    */
   template <class UP>
   size_type CountIf( UP p ) const
   {
      return pcl::CountIf( Begin(), End(), p );
   }

   /*! #
    */
   iterator MinItem() const
   {
      return iterator( const_cast<indirect_iterator>( pcl::MinItem( Begin(), End() ).it ) );
   }

   /*! #
    */
   template <class BP>
   iterator MinItem( BP p ) const
   {
      return iterator( const_cast<indirect_iterator>( pcl::MinItem( Begin(), End(), p ).it ) );
   }

   /*! #
    */
   iterator MaxItem() const
   {
      return iterator( const_cast<indirect_iterator>( pcl::MaxItem( Begin(), End() ).it ) );
   }

   /*! #
    */
   template <class BP>
   iterator MaxItem( BP p ) const
   {
      return iterator( const_cast<indirect_iterator>( pcl::MaxItem( Begin(), End(), p ).it ) );
   }

   /*! #
    */
   void Reverse()
   {
      m_array.Reverse();
   }

   /*! #
    */
   void Rotate( distance_type n )
   {
      m_array.Rotate( n );
   }

   /*! #
    */
   void ShiftLeft( const T* p, size_type n = 1 )
   {
      m_array.ShiftLeft( p, n );
   }

   /*! #
    */
   void ShiftRight( const T* p, size_type n = 1 )
   {
      m_array.ShiftRight( p, n );
   }

   /*! #
    */
   iterator Search( const T& v ) const
   {
      return iterator( const_cast<indirect_iterator>( pcl::LinearSearch( Begin(), End(), v ).it ) );
   }

   /*! #
    */
   iterator Search( const T* p ) const
   {
      return iterator( m_array.Search( p ) );
   }

   /*! #
    */
   template <class BP>
   iterator Search( const T& v, BP p ) const
   {
      return iterator( const_cast<indirect_iterator>( pcl::LinearSearch( Begin(), End(), v, p ).it ) );
   }

   /*! #
    */
   iterator SearchLast( const T& v ) const
   {
      return iterator( const_cast<indirect_iterator>( pcl::LinearSearchLast( Begin(), End(), v ).it ) );
   }

   /*! #
    */
   iterator SearchLast( const T* p ) const
   {
      return iterator( m_array.SearchLast( p ) );
   }

   /*! #
    */
   template <class BP>
   iterator SearchLast( const T& v, BP p ) const
   {
      return iterator( const_cast<indirect_iterator>( pcl::LinearSearchLast( Begin(), End(), v, p ).it ) );
   }

   /*! #
    */
   template <class FI>
   iterator SearchSubset( FI i, FI j ) const
   {
      return iterator( const_cast<indirect_iterator>( pcl::Search( Begin(), End(), i, j ).it ) );
   }

   /*! #
    */
   template <class FI, class BP>
   iterator SearchSubset( FI i, FI j, BP p ) const
   {
      return iterator( const_cast<indirect_iterator>( pcl::Search( Begin(), End(), i, j, p ).it ) );
   }

   /*! #
    */
   template <class C>
   iterator SearchSubset( const C& c ) const
   {
      return iterator( const_cast<indirect_iterator>( pcl::Search( Begin(), End(), c.Begin(), c.End() ).it ) );
   }

   /*! #
    */
   template <class C, class BP>
   iterator SearchSubset( const C& c, BP p ) const
   {
      return iterator( const_cast<indirect_iterator>( pcl::Search( Begin(), End(), c.Begin(), c.End(), p ).it ) );
   }

   /*! #
    */
   template <class BI>
   iterator SearchLastSubset( BI i, BI j ) const
   {
      return iterator( const_cast<indirect_iterator>( pcl::SearchLast( Begin(), End(), i, j ).it ) );
   }

   /*! #
    */
   template <class BI, class BP>
   iterator SearchLastSubset( BI i, BI j, BP p ) const
   {
      return iterator( const_cast<indirect_iterator>( pcl::SearchLast( Begin(), End(), i, j, p ).it ) );
   }

   /*! #
    */
   template <class C>
   iterator SearchLastSubset( const C& c ) const
   {
      return iterator( const_cast<indirect_iterator>( pcl::SearchLast( Begin(), End(), c.Begin(), c.End() ).it ) );
   }

   /*! #
    */
   template <class C, class BP>
   iterator SearchLastSubset( const C& c, BP p ) const
   {
      return iterator( const_cast<indirect_iterator>( pcl::SearchLast( Begin(), End(), c.Begin(), c.End(), p ).it ) );
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
   template <class FI>
   iterator ContainsSubset( FI i, FI j ) const
   {
      return SearchSubset( i, j ) != End();
   }

   /*! #
    */
   template <class FI, class BP>
   iterator ContainsSubset( FI i, FI j, BP p ) const
   {
      return SearchSubset( i, j, p ) != End();
   }

   /*! #
    */
   template <class C>
   iterator ContainsSubset( const C& c ) const
   {
      return m_array.ContainsSubset( c );
   }

   /*! #
    */
   template <class C, class BP>
   iterator ContainsSubset( const C& c, BP p ) const
   {
      return SearchSubset( c ) != End();
   }

   /*! #
    */
   void Sort()
   {
      pcl::QuickSort( m_array.Begin(), m_array.End(),
                      []( const T* a, const T* b ){ return *a < *b; } );
   }

   /*! #
    */
   template <class BP>
   void Sort( BP p )
   {
      pcl::QuickSort( m_array.Begin(), m_array.End(),
                      [p]( const T* a, const T* b ){ return p( *a, *b ); } );
   }

   /*!
    * Exchanges two reference arrays \a x1 and \a x2.
    */
   friend void Swap( ReferenceArray& x1, ReferenceArray& x2 )
   {
      pcl::Swap( x1.m_array, x2.m_array );
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
      const_iterator i = Begin();
      if ( i < End() )
      {
         s.Append( S( *i ) );
         if ( ++i < End() )
            do
            {
               s.Append( separator );
               s.Append( S( *i ) );
            }
            while ( ++i < End() );
      }
      return s;
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
      const_iterator i = Begin();
      if ( i < End() )
      {
         append( s, S( *i ) );
         if ( ++i < End() )
         {
            S p( separator );
            do
            {
               append( s, p );
               append( s, S( *i ) );
            }
            while ( ++i < End() );
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
    * Returns a 64-bit non-cryptographic hash value computed for this reference
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
    * Returns a 32-bit non-cryptographic hash value computed for this reference
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
    * Returns a non-cryptographic hash value computed for this reference array.
    * This function is a synonym for Hash64().
    */
   uint64 Hash( uint64 seed = 0 ) const
   {
      return Hash64( seed );
   }

private:

   array_implementation m_array;

   template <class C>
   void CloneObjects( const C& x, DirectContainer<T>* )
   {
      m_array.CloneAssign( x );
   }

   template <class C>
   void CloneObjects( const C& x, IndirectContainer<T>* )
   {
      m_array.Clear();
      m_array.Reserve( x.Length() );
      allocator a;
      for ( typename C::const_iterator p = x.Begin(); p != x.End(); ++p )
         if ( *p != nullptr )
         {
            T* o = a.Allocate( 1 );
            pcl::Construct( o, **p, a );
            m_array.Append( o );
         }
   }
};

// ----------------------------------------------------------------------------

/*!
 * Returns true iff two reference arrays \a x1 and \a x2 are equal. This
 * operator compares the objects pointed to by the pointers stored in the
 * reference arrays.
 * \ingroup array_relational_operators
 */
template <class T, class A> inline
bool operator ==( const ReferenceArray<T,A>& x1, const ReferenceArray<T,A>& x2 )
{
   return x1.Length() == x2.Length() && pcl::Equal( x1.Begin(), x2.Begin(), x2.End() );
}

/*!
 * Returns true iff a reference array \a x1 precedes another reference array
 * \a x2. This operator compares the objects pointed to by the pointers stored
 * in the reference arrays.
 * \ingroup array_relational_operators
 */
template <class T, class A> inline
bool operator <( const ReferenceArray<T,A>& x1, const ReferenceArray<T,A>& x2 )
{
   return pcl::Compare( x1.Begin(), x1.End(), x2.Begin(), x2.End() ) < 0;
}

/*!
 * Appends a non-null pointer \a p to a reference array \a x. Returns a
 * reference to the left-hand reference array.
 *
 * A pointer to the template argument type V must be statically castable to T*.
 * \ingroup array_insertion_operators
 */
template <class T, class A, class V> inline
ReferenceArray<T,A>& operator <<( ReferenceArray<T,A>& x, const V* p )
{
   x.Append( static_cast<const T*>( p ) );
   return x;
}

/*!
 * Appends a non-null pointer \a p to a temporary reference array \a x. Returns
 * a reference to the left-hand reference array.
 *
 * A pointer to the template argument type V must be statically castable to T*.
 * \ingroup array_insertion_operators
 */
template <class T, class A, class V> inline
ReferenceArray<T,A>& operator <<( ReferenceArray<T,A>&& x, const V* p )
{
   x.Append( static_cast<const T*>( p ) );
   return x;
}

/*!
 * Appends a reference array \a x2 to a reference array \a x1. Returns a
 * reference to the left-hand reference array.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
ReferenceArray<T,A>& operator <<( ReferenceArray<T,A>& x1, const ReferenceArray<T,A>& x2 )
{
   x1.Append( x2 );
   return x1;
}

/*!
 * Appends a reference array \a x2 to a temporary reference array \a x1.
 * Returns a reference to the left-hand reference array.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
ReferenceArray<T,A>& operator <<( ReferenceArray<T,A>&& x1, const ReferenceArray<T,A>& x2 )
{
   x1.Append( x2 );
   return x1;
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ReferenceArray_h

// ----------------------------------------------------------------------------
// EOF pcl/ReferenceArray.h - Released 2018-11-23T16:14:19Z
