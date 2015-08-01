//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// pcl/ReferenceSortedArray.h - Released 2015/07/30 17:15:18 UTC
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

#ifndef __PCL_ReferenceSortedArray_h
#define __PCL_ReferenceSortedArray_h

/// \file pcl/ReferenceSortedArray.h

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

#ifndef __PCL_ReferenceArray_h
#include <pcl/ReferenceArray.h>
#endif

#ifndef __PCL_SortedArray_h
#include <pcl/SortedArray.h>
#endif

#ifndef __PCL_IndirectSortedArray_h
#include <pcl/IndirectSortedArray.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ReferenceSortedArray
 * \brief Dynamic array of pointers to objects providing direct iteration and
 * element access by reference.
 *
 * ### TODO: Write a detailed description for %ReferenceSortedArray.
 *
 * \ingroup dynamic_arrays
 */
template <typename T, class A = StandardAllocator>
class PCL_CLASS ReferenceSortedArray : public DirectContainer<T>
{
public:

   /*! #
    */
   typedef ReferenceArray<T,A>      array_implementation;

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
   typedef typename array_implementation::indirect_iterator
                                    indirect_iterator;

   /*! #
    */
   typedef typename array_implementation::const_indirect_iterator
                                    const_indirect_iterator;

   // -------------------------------------------------------------------------

   /*!
    * Constructs an empty reference sorted array.
    */
   ReferenceSortedArray() : m_array()
   {
   }

   /*!
    * Constructs a reference sorted array that stores \a n copies of a non-null
    * pointer \a p.
    *
    * If \a p is \c nullptr, this function constructs an empty reference array.
    */
   ReferenceSortedArray( size_type n, const T* p ) : m_array()
   {
      PCL_PRECONDITION( p != nullptr )
      if ( p != nullptr )
         m_array = array_implementation( n, p );
   }

   /*!
    * Constructs a reference array as a copy of the sequence of non-null
    * pointers defined by the range [i,j) of forward iterators. The stored
    * sequence of pointers is sorted in ascending order by comparing the
    * pointed objects.
    *
    * If the range [i,j) contains null pointers, these are ignored and hence
    * not included in the constructed reference array.
    */
   template <class FI>
   ReferenceSortedArray( FI i, FI j ) : m_array( i, j )
   {
      Sort();
   }

   /*!
    * Copy constructor.
    */
   ReferenceSortedArray( const ReferenceSortedArray& x ) : m_array( x.m_array )
   {
   }

   /*!
    * Move constructor.
    */
   ReferenceSortedArray( ReferenceSortedArray&& x ) : m_array( std::move( x.m_array ) )
   {
   }

   /*!
    * Destroys a %ReferenceSortedArray object.
    *
    * Deallocates the internal array of pointers to objects, but does not
    * destroy the pointed objects. To destroy them, you have to call Destroy()
    * or Delete() explicitly.
    */
   ~ReferenceSortedArray()
   {
   }

   /*!
    * Returns true if this reference array uniquely references its contained
    * array of pointers to objects.
    */
   bool IsUnique() const
   {
      return m_array.IsUnique();
   }

   /*!
    * Returns true if this reference array is an alias of the array \a x.
    *
    * Two objects are aliases if both share the same data. Two reference
    * containers are aliases if they share a unique set of object pointers.
    */
   bool IsAliasOf( const ReferenceSortedArray& x ) const
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
   void SetUnique()
   {
      m_array.SetUnique();
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
    * In general, all %ReferenceSortedArray objects are valid with only two
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
    * Returns true if this reference array is empty.
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
    * Returns an immutable reference array iterator located at the specified
    * array index \a i.
    */
   const_iterator At( size_type i ) const
   {
      return m_array.At( i );
   }

   /*!
    * Returns a mutable reference array iterator located at the specified
    * array index \a i.
    */
   iterator MutableAt( size_type i )
   {
      return m_array.At( i );
   }

   /*!
    * Returns a mutable iterator pointing to the same array element as the
    * specified immutable iterator \a i.
    *
    * \warning As a side-effect of calling this function, the specified
    * immutable iterator \a i may become invalid. This happens when this
    * function is called for a shared array, since in this case getting a
    * mutable iterator involves a deep copy of the array through an implicit
    * call to SetUnique().
    */
   iterator MutableIterator( const_iterator i )
   {
      return m_array.MutableIterator( i );
   }

   /*!
    * Returns a reference to the immutable object at the specified array
    * index \a i. No bounds checking is performed.
    */
   const T& operator []( size_type i ) const
   {
      return m_array[i];
   }

   /*!
    * Returns a reference to the unmodifiable first object in this reference
    * array.
    */
   const T& operator *() const
   {
      return *m_array;
   }

   /*!
    * Returns an immutable iterator located at the beginning of this array.
    */
   const_iterator Begin() const
   {
      return m_array.ConstBegin();
   }

   /*!
    * Returns a mutable iterator located at the beginning of this array.
    */
   iterator MutableBegin()
   {
      return m_array.Begin();
   }

   /*!
    * Returns an immutable iterator located at the end of this array.
    */
   const_iterator End() const
   {
      return m_array.ConstEnd();
   }

   /*!
    * Returns a mutable iterator located at the end of this array.
    */
   iterator MutableEnd()
   {
      return m_array.End();
   }

   /*!
    * Returns an immutable reverse iterator located at the <em>reverse
    * beginning</em> of this reference array.
    *
    * The reverse beginning corresponds to the last object in the array.
    */
   const_reverse_iterator ReverseBegin() const
   {
      return m_array.ConstReverseBegin();
   }

   /*!
    * Returns a mutable reverse iterator located at the <em>reverse
    * beginning</em> of this reference array.
    *
    * The reverse beginning corresponds to the last object in the array.
    */
   reverse_iterator MutableReverseBegin()
   {
      return m_array.ReverseBegin();
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
      return m_array.ConstReverseEnd();
   }

   /*!
    * Returns a mutable reverse iterator located at the <em>reverse end</em> of
    * this reference array.
    *
    * The reverse end corresponds to a (nonexistent) object immediately before
    * the first object in the array.
    */
   reverse_iterator MutableReverseEnd()
   {
      return m_array.ReverseEnd();
   }

   /*!
    * Returns a reference to the first unmodifiable object in this reference
    * array. This function should never be called for an empty array.
    */
   const T& First() const
   {
      return m_array.First();
   }

   /*!
    * Returns a reference to the first object in this reference array. This
    * function should never be called for an empty array.
    */
   T& MutableFirst()
   {
      return m_array.First();
   }

   /*!
    * Returns a reference to the last unmodifiable object in this reference
    * array. This function should never be called for an empty array.
    */
   const T& Last() const
   {
      return m_array.Last();
   }

   /*!
    * Returns a reference to the last object in this reference array. This
    * function should never be called for an empty array.
    */
   T& MutableLast()
   {
      return m_array.Last();
   }

   /*!
    * Ensures that the specified iterator represents a pointer stored in a
    * uniquely referenced indirect array. If necessary, this function builds a
    * new, uniquely referenced copy of the underlying indirect array by calling
    * SetUnique().
    *
    * If the iterator \a i is changed, it is guaranteed to point to the object
    * at the same array index it was pointing to before calling this function.
    */
   void UniquifyIterator( iterator& i )
   {
      m_array.UniquifyIterator( i );
   }

   /*!
    * Ensures that the specified iterators represents a pointer stored in a
    * uniquely referenced indirect array. If necessary, this function builds a
    * new, uniquely referenced copy of the underlying indirect array by calling
    * SetUnique().
    *
    * If the iterators \a i and \a j are changed, they are guaranteed to point
    * to the objects at the same array indices they were pointing to before
    * calling this function.
    */
   void UniquifyIterators( iterator& i, iterator& j )
   {
      m_array.UniquifyIterators( i, j );
   }

   /*!
    * Copy assignment operator.
    *
    * Causes this reference sorted array to reference the same set of objects
    * as another reference sorted array \a x. Returns a reference to this
    * object.
    */
   ReferenceSortedArray& operator =( const ReferenceSortedArray& x )
   {
      Assign( x );
      return *this;
   }

   /*!
    * Assigns a reference sorted array \a x to this reference sorted array.
    *
    * Decrements the reference counter of the current array data, and destroys
    * it if it becomes unreferenced.
    *
    * Increments the reference counter of the source array's data and
    * references it in this array.
    */
   void Assign( const ReferenceSortedArray& x )
   {
      m_array.Assign( x.m_array );
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   ReferenceSortedArray& operator =( ReferenceSortedArray&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * Transfers data from another reference sorted array \a x to this object.
    *
    * Decrements the reference counter of the current array data, and destroys
    * it if it becomes unreferenced.
    *
    * Transfers source array data to this object, leaving empty and invalid the
    * source object \a x.
    */
   void Transfer( ReferenceSortedArray& x )
   {
      m_array.Transfer( x.m_array );
   }

   /*!
    * Transfers data from another reference sorted array \a x to this object.
    *
    * Decrements the reference counter of the current array data, and destroys
    * it if it becomes unreferenced.
    *
    * Transfers source array data to this object, leaving empty and invalid the
    * source object \a x.
    */
   void Transfer( ReferenceSortedArray&& x )
   {
      m_array.Transfer( std::move( x.m_array ) );
   }

   /*!
    * Assigns a reference array \a x to this reference sorted array.
    *
    * Causes this reference sorted array to store a sorted copy of the pointers
    * in the source array \a x. Sorting is performed by comparing the pointed
    * objects, not the pointers. Returns a reference to this object.
    */
   ReferenceSortedArray& operator =( const array_implementation& x )
   {
      Assign( x );
      return *this;
   }

   /*!
    * Assigns a reference array \a x to this reference sorted array.
    *
    * Causes this reference sorted array to store a sorted copy of the pointers
    * in the source array \a x. Sorting is performed by comparing the pointed
    * objects, not the pointers.
    */
   void Assign( const array_implementation& x )
   {
      m_array.Assign( x );
      Sort();
   }

   /*!
    * Move assignment of a reference array.
    *
    * The transferred array is sorted after the assignment. Returns a reference
    * to this object.
    */
   ReferenceSortedArray& operator =( array_implementation&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * Transfers data from a reference array \a x to this object.
    *
    * Decrements the reference counter of the current array data, and destroys
    * it if it becomes unreferenced.
    *
    * Transfers source array data to this object, leaving empty and invalid the
    * source object \a x. The transferred array is sorted after the assignment.
    */
   void Transfer( array_implementation& x )
   {
      m_array.Transfer( x );
      Sort();
   }

   /*!
    * Transfers data from a reference array \a x to this object.
    *
    * Decrements the reference counter of the current array data, and destroys
    * it if it becomes unreferenced.
    *
    * Transfers source array data to this object, leaving empty and invalid the
    * source object \a x. The transferred array is sorted after the assignment.
    */
   void Transfer( array_implementation&& x )
   {
      m_array.Transfer( std::move( x ) );
      Sort();
   }

   /*!
    * Replaces the contents of this reference sorted array with a sequence of
    * \a n pointers equal to \a p.
    *
    * if \a p is \c nullptr, this function yields an empty array.
    */
   void Assign( const T* p, size_type n = 1 )
   {
      m_array.Assign( p, n );
   }

   /*!
    * Replaces the contents of this reference sorted array with a sorted copy
    * of the sequence of pointers defined by the range [i,j) of forward
    * iterators.
    *
    * If the range [i,j) contains null pointers, these are ignored and hence
    * not included in this reference array. Sorting is performed by comparing
    * the pointed objects, not the pointers.
    *
    * \note \a i and \a j must not be iterators into this array.
    */
   template <class FI>
   void Assign( FI i, FI j )
   {
      m_array.Assign( i, j );
      Sort();
   }

   /*!
    * Replaces the contents of this reference sorted array with a sorted list
    * of pointers to newly created copies of the objects stored in the
    * specified container \a x. This function works for both direct and
    * indirect containers.
    *
    * Keep in mind that after calling this function (with a reference to a
    * nonempty container) this array will store newly allocated objects. You
    * should call Destroy() to deallocate these objects before destructing this
    * reference array in order to avoid a memory leak.
    */
   template <class C>
   void CloneAssign( const C& x )
   {
      m_array.CloneAssign( x );
      Sort();
   }

   /*! #
    */
   void CloneAssign( ReferenceSortedArray& x )
   {
      m_array.CloneAssign( x.m_array );
   }

   /*! #
    */
   void CloneAssign( SortedArray<T,A>& x )
   {
      m_array.CloneAssign( x );
   }

   /*! #
    */
   void CloneAssign( IndirectSortedArray<T,A>& x )
   {
      m_array.CloneAssign( x );
   }

   /*!
    * Causes this reference sorted array to store the sequence of pointers
    * defined by the range [i,j) of iterators. The previously referenced data
    * structure is dereferenced and deallocated if it becomes unreferenced. The
    * sequence [i,j) is sorted in ascending order by comparing the pointed
    * objects.
    */
   void Import( iterator i, iterator j )
   {
      m_array.Import( i, j );
      Sort();
   }

   /*!
    * Releases the set of pointers contained by this reference sorted array.
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

   /*! #
    */
   void Add( const ReferenceSortedArray& x )
   {
      const_iterator p = x.Begin(), q = x.End();
      for ( iterator i = Begin(); i < End() && p < q; ++i )
         if ( *p < *i )
            i = m_array.Insert( i, (T*)p++ );
      if ( p < q )
         m_array.Append( p, q );
   }

   /*! #
    */
   void Add( const array_implementation& x )
   {
      Add( x.Begin(), x.End() );
   }

   /*! #
    */
   const_iterator Add( const T* p, size_type n = 1 )
   {
      if ( p != nullptr )
         return m_array.Insert( pcl::InsertionPoint( Begin(), End(), *p ), p, n );
      return const_iterator( nullptr );
   }

   /*! #
    */
   template <class FI>
   void Add( FI i, FI j )
   {
      if ( i != j )
      {
         SetUnique();
         for ( const_iterator l = Begin(), r = End(); ; )
         {
            FI h = i;
            const_iterator m = m_array.Insert( pcl::InsertionPoint( l, r, **i ), *i );

            if ( ++i == j )
               break;

            if ( **i < **h )
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

   /*!
    * Removes a sequence of \a n contiguous pointers starting at the specified
    * location \a i in this reference array.
    *
    * Only pointers are removed by this function; the pointed objects are not
    * affected in any way.
    */
   void Remove( const_iterator i, size_type n = 1 )
   {
      m_array.Remove( i, n );
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
      m_array.Remove( i, j );
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
      m_array.RemovePointer( p );
   }

   /*!
    * Removes all pointers contained by this object, yielding an empty
    * reference sorted array.
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
    * the specified location \a i in this reference sorted array.
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
      m_array.Destroy( i, n );
   }

   /*!
    * Destroys and removes the objects in a range [i,j) of iterators in this
    * reference sorted array.
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
      m_array.Destroy( i, j );
   }

   /*!
    * Destroys and removes all objects equal to \a v in this reference sorted
    * array.
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
    * Destroys and removes every object x in this reference sorted array such
    * that the binary predicate p( x, \a v ) is true.
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
    * Destroys and removes all objects in this reference sorted array, yielding
    * an empty array.
    *
    * \warning See Destroy( iterator, iterator ) for critical information on
    * this member function.
    */
   void Destroy()
   {
      m_array.Destroy();
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
      m_array.Fill( v );
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
    * Returns an immutable iterator pointing to the first object x in this
    * reference sorted array such that f( const T& x ) is true. Returns End()
    * if such pointer does not exist.
    */
   template <class F>
   const_iterator FirstThat( F f ) const
   {
      return pcl::FirstThat( Begin(), End(), f );
   }

   /*!
    * Returns an immutable iterator pointing to the last object x in this
    * reference sorted array such that f( const T& x ) is true. Returns End()
    * if such pointer does not exist.
    */
   template <class F>
   const_iterator LastThat( F f ) const
   {
      return pcl::LastThat( Begin(), End(), f );
   }

   /*!
    * Returns the number of objects equal to \a v in this reference array.
    */
   size_type Count( const T& v ) const
   {
      const_iterator i = pcl::BinarySearch( Begin(), End(), v );
      return (i != End()) ? pcl::InsertionPoint( i+1, End(), v ) - i : 0;
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
      return m_array.Count( v, p );
   }

   /*!
    * Returns the number of objects in this reference array such that for each
    * counted object x the unary predicate p( x ) returns true.
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
   const_iterator Search( const T* p ) const
   {
      return m_array.Search( p );
   }

   /*! #
    */
   template <class BP>
   const_iterator Search( const T& v, BP p ) const
   {
      return pcl::LinearSearch( Begin(), End(), v, p );
   }

   /*! #
    */
   const_iterator SearchLast( const T& v ) const
   {
      return pcl::BinarySearchLast( Begin(), End(), v );
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
      return pcl::LinearSearchLast( Begin(), End(), v, p );
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
    * Exchanges two reference sorted arrays \a x1 and \a x2.
    */
   friend void Swap( ReferenceSortedArray& x1, ReferenceSortedArray& x2 )
   {
      pcl::Swap( x1.m_array, x2.m_array );
   }

   /*!
    * Returns true only if two reference sorted arrays \a x1 and \a x2 are
    * equal.
    * \ingroup array_relational_operators
    */
   friend bool operator ==( const ReferenceSortedArray& x1, const ReferenceSortedArray& x2 )
   {
      return x1.m_array == x2.m_array;
   }

   /*!
    * Returns true only if a reference sorted array \a x1 is equal to a
    * reference array \a x2.
    * \ingroup array_relational_operators
    */
   friend bool operator ==( const ReferenceSortedArray& x1, const array_implementation& x2 )
   {
      return x1.m_array == x2;
   }

   /*!
    * Returns true only if a reference array \a x1 is equal to a reference
    * sorted array \a x2.
    * \ingroup array_relational_operators
    */
   friend bool operator ==( const array_implementation& x1, const ReferenceSortedArray& x2 )
   {
      return x1 == x2.m_array;
   }

   /*!
    * Returns true only if a reference sorted array \a x1 precedes another
    * reference sorted array \a x2.
    * \ingroup array_relational_operators
    */
   friend bool operator <( const ReferenceSortedArray& x1, const ReferenceSortedArray& x2 )
   {
      return x1.m_array < x2.m_array;
   }

   /*!
    * Returns true only if a reference sorted array \a x1 precedes a reference
    * array \a x2.
    * \ingroup array_relational_operators
    */
   friend bool operator <( const ReferenceSortedArray& x1, const array_implementation& x2 )
   {
      return x1.m_array < x2;
   }

   /*!
    * Returns true only if a reference array \a x1 precedes a reference sorted
    * array \a x2.
    * \ingroup array_relational_operators
    */
   friend bool operator <( const array_implementation& x1, const ReferenceSortedArray& x2 )
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
};

// ----------------------------------------------------------------------------

/*!
 * Adds a pointer to an object \a p to a reference sorted array \a x. Returns a
 * reference to the reference sorted array.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
ReferenceSortedArray<T,A>& operator <<( ReferenceSortedArray<T,A>& x, const T* p )
{
   x.Add( p );
   return x;
}

/*!
 * Adds a pointer to an object \a p to a reference sorted array \a x. Returns a
 * reference to the reference sorted array.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
ReferenceSortedArray<T,A>& operator <<( ReferenceSortedArray<T,A>&& x, const T* p )
{
   x.Add( p );
   return x;
}

/*!
 * Adds a reference sorted array \a x2 to another reference sorted array \a x1.
 * Returns a reference to the left-hand reference sorted array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
ReferenceSortedArray<T,A>& operator <<( ReferenceSortedArray<T,A>& x1, const ReferenceSortedArray<T,A>& x2 )
{
   x1.Add( x2 );
   return x1;
}

/*!
 * Adds a reference sorted array \a x2 to another reference sorted array \a x1.
 * Returns a reference to the left-hand reference sorted array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
ReferenceSortedArray<T,A>& operator <<( ReferenceSortedArray<T,A>&& x1, const ReferenceSortedArray<T,A>& x2 )
{
   x1.Add( x2 );
   return x1;
}

/*!
 * Adds a reference array \a x2 to a reference sorted array \a x1. Returns a
 * reference to the left-hand reference sorted array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
ReferenceSortedArray<T,A>& operator <<( ReferenceSortedArray<T,A>& x1, const ReferenceArray<T,A>& x2 )
{
   x1.Add( x2 );
   return x1;
}

/*!
 * Adds a reference array \a x2 to a reference sorted array \a x1. Returns a
 * reference to the left-hand reference sorted array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
ReferenceSortedArray<T,A>& operator <<( ReferenceSortedArray<T,A>&& x1, const ReferenceArray<T,A>& x2 )
{
   x1.Add( x2 );
   return x1;
}

/*!
 * Adds an indirect sorted array \a x2 to a reference sorted array \a x1.
 * Returns a reference to the left-hand reference sorted array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
ReferenceSortedArray<T,A>& operator <<( ReferenceSortedArray<T,A>& x1, const IndirectSortedArray<T,A>& x2 )
{
   x1.Add( x2 );
   return x1;
}

/*!
 * Adds an indirect sorted array \a x2 to a reference sorted array \a x1.
 * Returns a reference to the left-hand reference sorted array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
ReferenceSortedArray<T,A>& operator <<( ReferenceSortedArray<T,A>&& x1, const IndirectSortedArray<T,A>& x2 )
{
   x1.Add( x2 );
   return x1;
}

/*!
 * Adds an indirect array \a x2 to a reference sorted array \a x1. Returns a
 * reference to the left-hand reference sorted array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
ReferenceSortedArray<T,A>& operator <<( ReferenceSortedArray<T,A>& x1, const IndirectArray<T,A>& x2 )
{
   x1.Add( x2 );
   return x1;
}

/*!
 * Adds an indirect array \a x2 to a reference sorted array \a x1. Returns a
 * reference to the left-hand reference sorted array \a x1.
 * \ingroup array_insertion_operators
 */
template <class T, class A> inline
ReferenceSortedArray<T,A>& operator <<( ReferenceSortedArray<T,A>&& x1, const IndirectArray<T,A>& x2 )
{
   x1.Add( x2 );
   return x1;
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ReferenceSortedArray_h

// ----------------------------------------------------------------------------
// EOF pcl/ReferenceSortedArray.h - Released 2015/07/30 17:15:18 UTC
