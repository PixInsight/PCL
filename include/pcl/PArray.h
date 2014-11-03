// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/PArray.h - Released 2014/10/29 07:34:12 UTC
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

#ifndef __PCL_PArray_h
#define __PCL_PArray_h

/// \file pcl/PArray.h

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

#ifndef __PCL_Array_h
#include <pcl/Array.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class PArray
 * \brief Dynamic array of pointers to objects providing direct iteration and
 * access by reference.
 *
 * ### TODO: Write a detailed description for %PArray.
 *
 * \ingroup dynamic_arrays
 */
template <typename T, class A = StandardAllocator>
class PCL_CLASS PArray : public DirectContainer<T>
{
public:

   typedef IndirectArray<T, A>                              array_implementation;
   typedef typename array_implementation::block_allocator   block_allocator;
   typedef typename array_implementation::allocator         allocator;
   typedef typename array_implementation::iterator          indirect_iterator;
   typedef typename array_implementation::const_iterator    const_indirect_iterator;

   /*!
    * \class pcl::PArray::iterator
    * \brief Mutable %PArray iterator.
    */
   class iterator : public Iterator<RandomAccessIterator, T>
   {
   public:

      typedef Iterator<RandomAccessIterator, T>       iterator_base;
      typedef typename iterator_base::iterator_class  iterator_class;
      typedef typename iterator_base::item_type       item_type;

      /*!
       * Default constructor. Constructs an uninitialized iterator object.
       */
      iterator() : iterator_base(), it()
      {
      }

      /*!
       * Constructor from an IndirectArray iterator (a pointer to a pointer to
       * an object in the iterated container).
       */
      iterator( indirect_iterator i ) : iterator_base(), it( i )
      {
      }

      /*!
       * Copy constructor.
       */
      iterator( const iterator& i ) : iterator_base(), it( i.it )
      {
      }

      /*!
       * Assignment operator. Returns a reference to this object.
       */
      iterator& operator =( const iterator& i )
      {
         it = i.it;
         return *this;
      }

      /*!
       * Pointer-to-object conversion operator. Returns a pointer to the object
       * pointed to by this iterator.
       */
      operator T*() const
      {
         return *it;
      }

      /*!
       * Returns a pointer to the object pointed to by this iterator. This
       * function is equivalent to operator T*() const.
       */
      T* Ptr() const
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
       * Preincrement operator. Increments this iterator so it points to the
       * next object in the iterated container, then returns a reference to
       * this iterator.
       */
      iterator& operator ++()
      {
         ++it;
         return *this;
      }

      /*!
       * Postincrement operator. Increments this iterator so it points to the
       * next object in the iterated container. Returns a copy of the iterator
       * as it was before incrementing it.
       */
      iterator operator ++( int )
      {
         indirect_iterator it0 = it;
         ++it;
         return it0;
      }

      /*!
       * Predecrement operator. Decrements this iterator so it points to the
       * previous object in the iterated container, then returns a reference to
       * this iterator.
       */
      iterator& operator --()
      {
         --it;
         return *this;
      }

      /*!
       * Postdecrement operator. Decrements this iterator so it points to the
       * previous object in the iterated container. Returns a copy of the
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
       * Iterator subtraction operator. Returns the distance (in objects)
       * between this iterator and another iterator \a i.
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
       * Less than operator. Returns true if this iterator precedes another
       * iterator \a i.
       */
      bool operator <( const iterator& i ) const
      {
         return it < i.it;
      }

   protected:

      indirect_iterator it;

      friend class PArray<T,A>;
      friend class PArray<T,A>::const_iterator;
   };

   /*!
    * \class pcl::PArray::const_iterator
    * \brief Immutable %PArray iterator
    */
   class const_iterator : public Iterator<RandomAccessIterator, T>
   {
   public:

      typedef Iterator<RandomAccessIterator, T>       iterator_base;
      typedef typename iterator_base::iterator_class  iterator_class;
      typedef typename iterator_base::item_type       item_type;

      /*!
       * Default constructor. Constructs an uninitialized iterator object.
       */
      const_iterator() : iterator_base(), it()
      {
      }

      /*!
       * Constructor from an IndirectArray iterator (a pointer to a pointer to
       * an object in the iterated container).
       */
      const_iterator( const_indirect_iterator i ) : iterator_base(), it( i )
      {
      }

      /*!
       * Constructor from non-const iterator.
       */
      const_iterator( const iterator& i ) : iterator_base(), it( i.it )
      {
      }

      /*!
       * Copy constructor.
       */
      const_iterator( const const_iterator& i ) : iterator_base(), it( i.it )
      {
      }

      /*!
       * Assignment operator. Returns a reference to this immutable iterator.
       */
      const_iterator& operator =( const const_iterator& i )
      {
         it = i.it;
         return *this;
      }

      /*!
       * Non-const iterator assignment operator. Returns a reference to this
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
       * Returns a pointer to the immutable object pointed to by this iterator.
       * This function is equivalent to operator const T*() const.
       */
      const T* Ptr() const
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
       * Preincrement operator. Increments this iterator so it points to the
       * next object in the iterated container, then returns a reference to
       * this iterator.
       */
      const_iterator& operator ++()
      {
         ++it;
         return *this;
      }

      /*!
       * Postincrement operator. Increments this iterator so it points to the
       * next object in the iterated container. Returns a copy of the iterator
       * as it was before incrementing it.
       */
      const_iterator operator ++( int )
      {
         const_indirect_iterator it0 = it;
         ++it;
         return it0;
      }

      /*!
       * Predecrement operator. Decrements this iterator so it points to the
       * previous object in the iterated container, then returns a reference to
       * this iterator.
       */
      const_iterator& operator --()
      {
         --it;
         return *this;
      }

      /*!
       * Postdecrement operator. Decrements this iterator so it points to the
       * previous object in the iterated container. Returns a copy of the
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
       * Iterator subtraction operator. Returns the distance (in objects)
       * between this iterator and another iterator \a i.
       */
      distance_type operator -( const const_iterator& i ) const
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
       * Less than operator. Returns true if this iterator precedes another
       * iterator \a i.
       */
      bool operator <( const const_iterator& i ) const
      {
         return it < i.it;
      }

   protected:

      const_indirect_iterator it;

      friend class PArray<T,A>;
   };

   /*!
    * \class pcl::PArray::reverse_iterator
    * \brief Mutable %PArray reverse iterator.
    */
   typedef ReverseRandomAccessIterator<iterator, T>
      reverse_iterator;

   /*!
    * \class pcl::PArray::const_reverse_iterator
    * \brief Immutable %PArray reverse iterator.
    */
   typedef ReverseRandomAccessIterator<const_iterator, const T>
      const_reverse_iterator;

   // -------------------------------------------------------------------------

   /*! #
    */
   explicit
   PArray( const A& a = A() ) : array( a )
   {
   }

   /*! #
    */
   explicit
   PArray( size_type n, const A& a = A() ) : array( n, a )
   {
   }

   /*! #
    */
   PArray( size_type n, const T* p, const A& a = A() ) : array( n, p, a)
   {
   }

   /*! #
    */
   template <class FI>
   PArray( FI i, FI j, const A& a = A() ) : array( i, j, a )
   {
   }

   /*! #
    */
   PArray( const array_implementation& x ) : array( x )
   {
   }

   /*! #
    */
   PArray( const PArray<T,A>& x ) : array( x.array )
   {
   }

   /*!
    * Destroys a %PArray object.
    *
    * Deallocates the internal list of pointers to objects, but does not
    * destroy the contained objects. To destroy them, you have to call
    * Destroy() explicitly.
    */
   ~PArray()
   {
   }

   /*!
    * Returns true if this pointer array uniquely references its contained
    * data pointers.
    */
   bool IsUnique() const
   {
      return array.IsUnique();
   }

   /*!
    * Returns true if this pointer array is an alias of a specified pointer
    * array \a x.
    *
    * Two objects are aliases if both of them share the same data. Two pointer
    * arrays are aliases if they share a unique set of data pointers.
    */
   bool IsAliasOf( const PArray<T,A>& x ) const
   {
      return array.IsAliasOf( x.array );
   }

   /*!
    * Ensures that this pointer array uniquely references its contained data
    * pointers.
    *
    * If necessary, this member function generates a duplicate of the array of
    * pointers, references it, and then decrements the reference counter of the
    * original pointer array.
    */
   void SetUnique()
   {
      array.SetUnique();
   }

   /*!
    * Returns the total number of bytes required to store the array of data
    * pointers in this pointer array.
    */
   size_type Size() const
   {
      return array.Size();
   }

   /*! #
    */
   size_type Length() const
   {
      return array.Length();
   }

   /*! #
    */
   size_type Capacity() const
   {
      return array.Capacity();
   }

   /*! #
    */
   size_type Available() const
   {
      return array.Available();
   }

   /*! #
    */
   bool IsEmpty() const
   {
      return array.IsEmpty();
   }

   /*! #
    */
   size_type LowerBound() const
   {
      return array.LowerBound();
   }

   /*! #
    */
   size_type UpperBound() const
   {
      return array.UpperBound();
   }

   /*! #
    */
   const A& GetAllocator() const
   {
      return array.GetAllocator();
   }

   /*! #
    */
   void SetAllocator( const A& a )
   {
      array.SetAllocator( a );
   }

   /*! #
    */
   void Put( size_type i, T* p )
   {
      PCL_PRECONDITION( i < Length() )
      array[i] = p;
   }

   /*! #
    */
   T* Get( size_type i )
   {
      PCL_PRECONDITION( i < Length() )
      return array[i];
   }

   /*! #
    */
   const T* Get( size_type i ) const
   {
      PCL_PRECONDITION( i < Length() )
      return array[i];
   }

   /*! #
    */
   T& operator []( size_type i )
   {
      PCL_PRECONDITION( i < Length() )
      PCL_PRECONDITION( array[i] != 0 )
      return *array[i];
   }

   /*! #
    */
   const T& operator []( size_type i ) const
   {
      PCL_PRECONDITION( i < Length() )
      PCL_PRECONDITION( array[i] != 0 )
      return *array[i];
   }

   /*! #
    */
   T& operator *()
   {
      PCL_PRECONDITION( !IsEmpty() )
      PCL_PRECONDITION( array[0] != 0 )
      return **array.Begin();
   }

   /*! #
    */
   const T& operator *() const
   {
      PCL_PRECONDITION( !IsEmpty() )
      PCL_PRECONDITION( array[0] != 0 )
      return **array.Begin();
   }

   /*! #
    */
   iterator At( size_type i )
   {
      PCL_PRECONDITION( i < Length() )
      return iterator( array.At( i ) );
   }

   /*! #
    */
   const_iterator At( size_type i ) const
   {
      PCL_PRECONDITION( i < Length() )
      return const_iterator( array.At( i ) );
   }

   /*! #
    */
   iterator Begin()
   {
      return iterator( array.Begin() );
   }

   /*! #
    */
   const_iterator Begin() const
   {
      return const_iterator( array.Begin() );
   }

   /*! #
    */
   const_iterator ConstBegin() const
   {
      return const_iterator( array.ConstBegin() );
   }

   /*! #
    */
   iterator End()
   {
      return iterator( array.End() );
   }

   /*! #
    */
   const_iterator End() const
   {
      return const_iterator( array.End() );
   }

   /*! #
    */
   const_iterator ConstEnd() const
   {
      return const_iterator(  array.ConstEnd() );
   }

   /*! #
    */
   reverse_iterator ReverseBegin()
   {
      PCL_PRECONDITION( !IsEmpty() )
      return iterator( array.End()-1 );
   }

   /*! #
    */
   const_reverse_iterator ReverseBegin() const
   {
      PCL_PRECONDITION( !IsEmpty() )
      return const_iterator( array.End()-1 );
   }

   /*! #
    */
   const_reverse_iterator ConstReverseBegin() const
   {
      PCL_PRECONDITION( !IsEmpty() )
      return const_iterator( array.End()-1 );
   }

   /*! #
    */
   reverse_iterator ReverseEnd()
   {
      PCL_PRECONDITION( !IsEmpty() )
      return iterator( array.Begin()-1 );
   }

   /*! #
    */
   const_reverse_iterator ReverseEnd() const
   {
      PCL_PRECONDITION( !IsEmpty() )
      return const_iterator( array.Begin()-1 );
   }

   /*! #
    */
   const_reverse_iterator ConstReverseEnd() const
   {
      PCL_PRECONDITION( !IsEmpty() )
      return const_iterator( array.Begin()-1 );
   }

   /*! #
    */
   T& First()
   {
      PCL_PRECONDITION( !IsEmpty() )
      PCL_PRECONDITION( *array.Begin() != 0 )
      return **array.Begin();
   }

   /*! #
    */
   const T& First() const
   {
      PCL_PRECONDITION( !IsEmpty() )
      PCL_PRECONDITION( *array.Begin() != 0 )
      return **array.Begin();
   }

   /*! #
    */
   T& Last()
   {
      PCL_PRECONDITION( !IsEmpty() )
      PCL_PRECONDITION( *array.ReverseBegin() != 0 )
      return **array.ReverseBegin();
   }

   /*! #
    */
   const T& Last() const
   {
      PCL_PRECONDITION( !IsEmpty() )
      PCL_PRECONDITION( *array.ReverseBegin() != 0 )
      return **array.ReverseBegin();
   }

   /*! #
    */
   PArray<T,A>& operator =( const PArray<T,A>& x )
   {
      array = x.array;
      return *this;
   }

   /*! #
    */
   void Assign( const PArray<T,A>& x )
   {
      array.Assign( x.array );
   }

   /*! #
    */
   void Assign( const T* p, size_type n = 1 )
   {
      array.Assign( p, n );
   }

   /*! #
    */
   template <class FI>
   void Assign( FI i, FI j )
   {
      array.Assign( i, j );
   }

   /*! #
    */
   template <class C>
   void CloneAssign( const C& x )
   {
      array.CloneAssign( x );
   }

   /*! #
    */
   void Import( iterator i, iterator j )
   {
      array.Import( i.it, j.it );
   }

   /*! #
    */
   void Import( indirect_iterator i, indirect_iterator j )
   {
      array.Import( i, j );
   }

   /*! #
    */
   indirect_iterator Release()
   {
      return array.Release();
   }

   /*! #
    */
   iterator Insert( iterator i, const PArray<T,A>& x )
   {
      return iterator( array.Insert( i.it, x.array ) );
   }

   /*! #
    */
   iterator Insert( iterator i, const T* p, size_type n = 1 )
   {
      return iterator( array.Insert( i.it, p, n ) );
   }

   /*! #
    */
   template <class FI>
   iterator Insert( iterator i, FI i1, FI j1 )
   {
      return iterator( array.Insert( i.it, i1, j1 ) );
   }

   /*! #
    */
   void Add( const PArray<T,A>& x )
   {
      array.Add( x.array );
   }

   /*! #
    */
   void Add( const T* p, size_type n = 1 )
   {
      array.Add( p, n );
   }

   /*! #
    */
   template <class FI>
   void Add( FI i, FI j )
   {
      array.Add( i, j );
   }

   /*! #
    */
   void Remove( iterator i, size_type n = 1 )
   {
      array.Remove( i.it, n );
   }

   /*! #
    */
   void Remove( iterator i, iterator j )
   {
      array.Remove( i.it, j.it );
   }

   /*! #
    */
   void Remove( const T& v )
   {
      array.Remove( v );
   }

   /*! #
    */
   void Remove( const T* p )
   {
      array.Remove( p );
   }

   /*! #
    */
   void Clear()
   {
      array.Clear();
   }

   /*!
      A synonym for Clear().
    */
   void Remove() { Clear(); }

   /*! #
    */
   void Destroy( iterator i, size_type n = 1 )
   {
      array.Destroy( i.it, n );
   }

   /*! #
    */
   void Destroy( iterator i, iterator j )
   {
      array.Destroy( i.it, j.it );
   }

   /*! #
    */
   void Destroy( const T& v )
   {
      array.Destroy( v );
   }

   /*! #
    */
   void Destroy()
   {
      array.Destroy();
   }

   /*! #
    */
   iterator Replace( iterator i, iterator j, const T* p, size_type n = 1 )
   {
      return iterator( array.Replace( i.it, j.it, p, n ) );
   }

   /*! #
    */
   template <class FI>
   iterator Replace( iterator i, iterator j, FI i1, FI j1 )
   {
      return iterator( array.Replace( i.it, j.it, i1, j1 ) );
   }

   /*! #
    */
   void Reserve( size_type n )
   {
      array.Reserve( n );
   }

   /*! #
    */
   void Squeeze()
   {
      array.Squeeze();
   }

   /*! #
    */
   template <class F>
   void Apply( F f )
   {
      array.Apply( f );
   }

   /*! #
    */
   template <class F>
   void Apply( F f ) const
   {
      array.Apply( f );
   }

   /*! #
    */
   template <class F>
   iterator FirstThat( F f ) const
   {
      return iterator( array.FirstThat( f ) );
   }

   /*! #
    */
   template <class F>
   iterator LastThat( F f ) const
   {
      return iterator( array.LastThat( f ) );
   }

   /*! #
    */
   size_type Count( const T& v ) const
   {
      return array.Count( v );
   }

   /*! #
    */
   size_type Count( const T* p ) const
   {
      return array.Count( p );
   }

   /*! #
    */
   template <class BP>
   size_type Count( const T& v, BP p ) const
   {
      return array.Count( v, p );
   }

   /*! #
    */
   template <class UP>
   size_type CountIf( UP p ) const
   {
      return array.CountIf( p );
   }

   /*! #
    */
   iterator MinItem() const
   {
      return iterator( array.MinItem() );
   }

   /*! #
    */
   template <class BP>
   iterator MinItem( BP p ) const
   {
      return iterator( array.MinItem( p ) );
   }

   /*! #
    */
   iterator MaxItem() const
   {
      return iterator( array.MaxItem() );
   }

   /*! #
    */
   template <class BP>
   iterator MaxItem( BP p ) const
   {
      return iterator( array.MaxItem( p ) );
   }

   /*! #
    */
   void Reverse()
   {
      array.Reverse();
   }

   /*! #
    */
   void Rotate( distance_type n )
   {
      array.Rotate( n );
   }

   /*! #
    */
   void ShiftLeft( const T* p, size_type n = 1 )
   {
      array.ShiftLeft( p, n );
   }

   /*! #
    */
   void ShiftRight( const T* p, size_type n = 1 )
   {
      array.ShiftRight( p, n );
   }

   /*! #
    */
   iterator Search( const T& v ) const
   {
      return iterator( array.Search( v ) );
   }

   /*! #
    */
   iterator Search( const T* p ) const
   {
      return iterator( array.Search( p ) );
   }

   /*! #
    */
   template <class BP>
   iterator Search( const T& v, BP p ) const
   {
      return iterator( array.Search( v, p ) );
   }

   /*! #
    */
   iterator SearchLast( const T& v ) const
   {
      return iterator( array.SearchLast( v ) );
   }

   /*! #
    */
   iterator SearchLast( const T* p ) const
   {
      return iterator( array.SearchLast( p ) );
   }

   /*! #
    */
   template <class BP>
   iterator SearchLast( const T& v, BP p ) const
   {
      return iterator( array.SearchLast( v, p ) );
   }

   /*! #
    */
   template <class FI>
   iterator SearchSet( FI i, FI j ) const
   {
      return iterator( array.SearchSet( i, j ) );
   }

   /*! #
    */
   template <class FI, class BP>
   iterator SearchSet( FI i, FI j, BP p ) const
   {
      return iterator( array.SearchSet( i, j, p ) );
   }

   /*! #
    */
   template <class C>
   iterator SearchSet( const C& c ) const
   {
      return iterator( array.SearchSet( c ) );
   }

   /*! #
    */
   template <class C, class BP>
   iterator SearchSet( const C& c, BP p ) const
   {
      return iterator( array.SearchSet( c, p ) );
   }

   /*! #
    */
   template <class BI>
   iterator SearchSetLast( BI i, BI j ) const
   {
      return iterator( array.SearchSetLast( i, j ) );
   }

   /*! #
    */
   template <class BI, class BP>
   iterator SearchSetLast( BI i, BI j, BP p ) const
   {
      return iterator( array.SearchSetLast( i, j, p ) );
   }

   /*! #
    */
   template <class C>
   iterator SearchSetLast( const C& c ) const
   {
      return iterator( array.SearchSetLast( c ) );
   }

   /*! #
    */
   template <class C, class BP>
   iterator SearchSetLast( const C& c, BP p ) const
   {
      return iterator( array.SearchSetLast( c, p ) );
   }

   /*! #
    */
   bool Has( const T& v ) const
   {
      return array.Has( v );
   }

   /*! #
    */
   bool Has( const T* p ) const
   {
      return array.Has( p );
   }

   /*! #
    */
   template <class BP>
   bool Has( const T& v, BP p ) const
   {
      return array.Has( v, p );
   }

   /*! #
    */
   template <class FI>
   iterator HasSet( FI i, FI j ) const
   {
      return array.HasSet( i, j );
   }

   /*! #
    */
   template <class FI, class BP>
   iterator HasSet( FI i, FI j, BP p ) const
   {
      return array.HasSet( i, j, p );
   }

   /*! #
    */
   template <class C>
   iterator HasSet( const C& c ) const
   {
      return array.HasSet( c );
   }

   /*! #
    */
   template <class C, class BP>
   iterator HasSet( const C& c, BP p ) const
   {
      return array.HasSet( c, p );
   }

   /*! #
    */
   void Sort()
   {
      array.Sort();
   }

   /*! #
    */
   template <class BP>
   void Sort( BP p )
   {
      array.Sort( p );
   }

   /*! #
    */
   friend void Swap( PArray<T,A>& x1, PArray<T,A>& x2 )
   {
      pcl::Swap( x1.array, x2.array );
   }

   /*! #
    */
   template <class S, typename SP>
   S& ToSeparated( S& s, SP separator ) const
   {
      return array.ToSeparated( s, separator );
   }

   /*! #
    */
   template <class S, typename SP, class AF>
   S& ToSeparated( S& s, SP separator, AF append ) const
   {
      return array.ToSeparated( s, separator, append );
   }

   /*! #
    */
   template <class S>
   S& ToCommaSeparated( S& s ) const
   {
      return array.ToCommaSeparated( s );
   }

   /*! #
    */
   template <class S>
   S& ToSpaceSeparated( S& s ) const
   {
      return array.ToSpaceSeparated( s );
   }

   /*! #
    */
   template <class S>
   S& ToTabSeparated( S& s ) const
   {
      return array.ToTabSeparated( s );
   }

protected:

   array_implementation array;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_PArray_h

// ****************************************************************************
// EOF pcl/PArray.h - Released 2014/10/29 07:34:12 UTC
