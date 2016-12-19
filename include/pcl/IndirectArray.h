//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// pcl/IndirectArray.h - Released 2016/02/21 20:22:12 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_IndirectArray_h
#define __PCL_IndirectArray_h

/// \file pcl/IndirectArray.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_Array_h
#include <pcl/Array.h>
#endif

#ifndef __PCL_Indirect_h
#include <pcl/Indirect.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class IndirectArray
 * \brief Generic dynamic array of pointers to objects.
 *
 * ### TODO: Write a detailed description for %IndirectArray.
 *
 * \ingroup dynamic_arrays
 */
template <class T, class A = StandardAllocator>
class PCL_CLASS IndirectArray : public IndirectContainer<T>
{
public:

   /*! #
    */
   typedef A                        block_allocator;

   /*! #
    */
   typedef Allocator<T,A>           allocator;

   /*! #
    */
   typedef T**                      iterator;

   /*! #
    */
   typedef T* const*                const_iterator;

   /*! #
    */
   typedef Array<void*, A>          array_implementation;

   /*! #
    */
   typedef typename array_implementation::iterator
                                    array_iterator;

   /*! #
    */
   typedef typename array_implementation::const_iterator
                                    const_array_iterator;

   /*! #
    */
   typedef ReverseRandomAccessIterator<iterator, T*>
                                    reverse_iterator;

   /*! #
    */
   typedef ReverseRandomAccessIterator<const_iterator, const T*>
                                    const_reverse_iterator;

   /*! #
    */
   typedef IndirectEqual<const T*>  equal;

   /*! #
    */
   typedef IndirectLess<const T*>   less;

   // -------------------------------------------------------------------------

   /*!
    * Constructs an empty indirect array.
    */
   IndirectArray() : m_array()
   {
   }

   /*!
    * Constructs an indirect array of length \a n. All contained pointers are
    * initialized to \c nullptr.
    */
   explicit
   IndirectArray( size_type n ) : m_array()
   {
      m_array.Append( (void*)nullptr, n );
   }

   /*!
    * Constructs an indirect array that stores \a n copies of a pointer \a p.
    */
   IndirectArray( size_type n, const T* p ) : m_array()
   {
      m_array.Append( (void*)p, n );
   }

   /*!
    * Constructs an indirect array as a copy of the sequence of pointers
    * defined by the range [i,j) of forward iterators.
    */
   template <class FI>
   IndirectArray( FI i, FI j ) : m_array( i, j )
   {
   }

   /*!
    * Copy constructor.
    */
   IndirectArray( const IndirectArray& x ) : m_array( x.m_array )
   {
   }

   /*!
    * Move constructor.
    */
   IndirectArray( IndirectArray&& x ) : m_array( std::move( x.m_array ) )
   {
   }

   /*!
    * Destroys an %IndirectArray object.
    *
    * Deallocates the internal array of pointers to objects, but does not
    * destroy the pointed objects. To destroy them, you have to call Destroy()
    * or Delete() explicitly.
    */
   ~IndirectArray()
   {
   }

   /*!
    * Returns true iff this indirect array uniquely references its contained
    * array of pointers to objects.
    */
   bool IsUnique() const
   {
      return m_array.IsUnique();
   }

   /*!
    * Returns true iff this indirect array is an alias of the indirect array
    * \a x.
    *
    * Two objects are aliases if both share the same data. Two indirect
    * containers are aliases if they share a unique set of data pointers.
    */
   bool IsAliasOf( const IndirectArray& x ) const
   {
      return m_array.IsAliasOf( x.m_array );
   }

   /*!
    * Ensures that this indirect array uniquely references its contained data
    * pointers.
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
    * Returns the total number of bytes required to store the array of data
    * pointers contained by this indirect array.
    */
   size_type Size() const
   {
      return m_array.Size();
   }

   /*!
    * Returns the length of this indirect array.
    */
   size_type Length() const
   {
      return m_array.Length();
   }

   /*!
    * Returns the capacity of this indirect array. The capacity is the maximum
    * number of pointers to objects that this indirect array can contain
    * without requiring a reallocation.
    */
   size_type Capacity() const
   {
      return m_array.Capacity();
   }

   /*!
    * Returns the length of the space available in this indirect array, or zero
    * if this indirect array cannot contain more pointers. The available space
    * is the number of pointers to objects that can be added to this indirect
    * array without requiring a reallocation. It is equal to
    * Capacity() - Length() by definition.
    */
   size_type Available() const
   {
      return m_array.Available();
   }

   /*!
    * Returns true only if this indirect array is valid. An indirect array is
    * valid if it references an internal structure with an array of pointers,
    * even if it is an empty array.
    *
    * In general, all %IndirectArray objects are valid with only two
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
    * Returns true iff this indirect array is empty.
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
    * Returns a reference to the allocator object used by this indirect array.
    */
   const allocator& GetAllocator() const
   {
      return m_array.GetAllocator();
   }

   /*!
    * Sets a new allocator object for this array.
    */
   void SetAllocator( const allocator& a )
   {
      m_array.SetAllocator( a );
   }

   /*!
    * Returns an array iterator located at the specified index \a i.
    */
   iterator At( size_type i )
   {
      return iterator( m_array.At( i ) );
   }

   /*!
    * Returns an immutable array iterator located at the specified index \a i.
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
      return iterator( m_array.MutableIterator( (const_array_iterator)i ) );
   }

   /*!
    * Returns a reference to the pointer stored at the specified index \a i. No
    * bounds checking is performed.
    */
   T*& operator []( size_type i )
   {
      return (T*&)m_array[i];
   }

   /*!
    * Returns a copy of the pointer to an unmodifiable object stored at the
    * specified index \a i. No bounds checking is performed.
    */
   const T* operator []( size_type i ) const
   {
      return (const T*)m_array[i];
   }

   /*!
    * Returns a reference to the first pointer stored in this indirect array.
    */
   T*& operator *()
   {
      return (T*&)*Begin();
   }

   /*!
    * Returns a copy of the first pointer to an unmodifiable object stored in
    * this indirect array.
    */
   const T* operator *() const
   {
      return (T*)*Begin();
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
      return const_iterator( m_array.ConstEnd() );
   }

   /*!
    * Returns a mutable reverse iterator located at the <em>reverse
    * beginning</em> of this indirect array.
    *
    * The reverse beginning corresponds to the last pointer in the array.
    */
   reverse_iterator ReverseBegin()
   {
      return iterator( (array_iterator)m_array.ReverseBegin() );
   }

   /*!
    * Returns an immutable reverse iterator located at the <em>reverse
    * beginning</em> of this indirect array.
    *
    * The reverse beginning corresponds to the last pointer in the array.
    */
   const_reverse_iterator ReverseBegin() const
   {
      return const_iterator( (const_array_iterator)m_array.ReverseBegin() );
   }

   /*!
    * Returns an immutable reverse iterator located at the <em>reverse
    * beginning</em> of this indirect array.
    *
    * The reverse beginning corresponds to the last pointer in the array.
    */
   const_reverse_iterator ConstReverseBegin() const
   {
      return const_iterator( (const_array_iterator)m_array.ConstReverseBegin() );
   }

   /*!
    * Returns a mutable reverse iterator located at the <em>reverse end</em> of
    * this indirect array.
    *
    * The reverse end corresponds to a (nonexistent) pointer immediately before
    * the first pointer in the array.
    */
   reverse_iterator ReverseEnd()
   {
      return iterator( (array_iterator)m_array.ReverseEnd() );
   }

   /*!
    * Returns an immutable reverse iterator located at the <em>reverse end</em>
    * of this indirect array.
    *
    * The reverse end corresponds to a (nonexistent) pointer immediately before
    * the first pointer in the array.
    */
   const_reverse_iterator ReverseEnd() const
   {
      return const_iterator( (const_array_iterator)m_array.ReverseEnd() );
   }

   /*!
    * Returns an immutable reverse iterator located at the <em>reverse end</em>
    * of this indirect array.
    *
    * The reverse end corresponds to a (nonexistent) pointer immediately before
    * the first pointer in the array.
    */
   const_reverse_iterator ConstReverseEnd() const
   {
      return const_iterator( (const_array_iterator)m_array.ConstReverseEnd() );
   }

   /*!
    * Returns a copy of the first pointer in this indirect array, or \c nullptr
    * if this array is empty.
    */
   T* First()
   {
      return IsEmpty() ? nullptr : *Begin();
   }

   /*!
    * Returns a copy of the first pointer to an unmodifiable object in this
    * indirect array, or \c nullptr if this array is empty.
    */
   const T* First() const
   {
      return IsEmpty() ? nullptr : *Begin();
   }

   /*!
    * Returns a copy of the last pointer in this indirect array, or \c nullptr
    * if this array is empty.
    */
   T* Last()
   {
      return IsEmpty() ? nullptr : *ReverseBegin();
   }

   /*!
    * Returns a copy of the last pointer to an unmodifiable object in this
    * indirect array, or \c nullptr if this array is empty.
    */
   const T* Last() const
   {
      return IsEmpty() ? nullptr : *ReverseBegin();
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
      m_array.UniquifyIterator( (array_iterator&)i );
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
      m_array.UniquifyIterators( (array_iterator&)i, (array_iterator&)j );
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
    * Causes this indirect array to reference the same set of pointers as
    * another array \a x. Returns a reference to this object.
    */
   IndirectArray& operator =( const IndirectArray& x )
   {
      Assign( x );
      return *this;
   }

   /*!
    * Assigns an indirect array \a x to this indirect array.
    *
    * Decrements the reference counter of the current array data, and destroys
    * it if it becomes unreferenced.
    *
    * Increments the reference counter of the source array's data and
    * references it in this array.
    */
   void Assign( const IndirectArray& x )
   {
      m_array.Assign( x.m_array );
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   IndirectArray& operator =( IndirectArray&& x )
   {
      Transfer( x );
      return *this;
   }

   /*!
    * Transfers data from another indirect array \a x to this object.
    *
    * Decrements the reference counter of the current array data, and destroys
    * it if it becomes unreferenced.
    *
    * Transfers source array data to this object, leaving empty and invalid the
    * source object \a x.
    */
   void Transfer( IndirectArray& x )
   {
      m_array.Transfer( x.m_array );
   }

   /*!
    * Transfers data from another indirect array \a x to this object.
    *
    * Decrements the reference counter of the current array data, and destroys
    * it if it becomes unreferenced.
    *
    * Transfers source array data to this object, leaving empty and invalid the
    * source object \a x.
    */
   void Transfer( IndirectArray&& x )
   {
      m_array.Transfer( x.m_array );
   }

   /*!
    * Replaces the contents of this indirect array with a sequence of \a n
    * pointers equal to \a p.
    */
   void Assign( const T* p, size_type n = 1 )
   {
      m_array.Assign( (void*)p, n );
   }

   /*!
    * Replaces the contents of this indirect array with a copy of the sequence
    * of pointers defined by the range [i,j) of forward iterators.
    *
    * \note \a i and \a j must not be iterators into this array.
    */
   template <class FI>
   void Assign( FI i, FI j )
   {
      m_array.Assign( i, j );
   }

   /*!
    * Replaces the contents of this indirect array with a set of pointers to
    * newly created copies of the objects stored in the specified container
    * \a x. This function works for both direct and indirect containers.
    *
    * Keep in mind that after calling this function (with a reference to a
    * nonempty container) this array will store pointers to newly allocated
    * objects. You should call Delete() or Destroy() to deallocate these
    * objects before destructing this array, or copy the pointers somewhere, in
    * order to avoid memory leaks.
    */
   template <class C>
   void CloneAssign( const C& x )
   {
      PCL_ASSERT_CONTAINER( C, T );
      CloneObjects( x, (C*)nullptr );
   }

   /*!
    * Causes this indirect array to store the sequence of pointers defined by
    * the range [i,j) of indirect array iterators (pointers to pointers). The
    * previously referenced data structure is dereferenced and deallocated if
    * it becomes unreferenced.
    */
   void Import( iterator i, iterator j )
   {
      m_array.Import( (array_iterator)i, (array_iterator)j );
   }

   /*!
    * Releases the set of pointers contained by this indirect array.
    *
    * This member function returns a pointer to the internal block of pointers
    * stored in this object, after ensuring that it is uniquely referenced.
    * If the array is empty, this function may return the null pointer.
    *
    * Before returning, this member function empties this array without
    * deallocating its contained data. The caller is then responsible for
    * deallocating the returned block when it is no longer required.
    */
   iterator Release()
   {
      return iterator( m_array.Release() );
   }

   /*!
    * Inserts a contiguous sequence of \a n null pointers at the specified
    * location \a i in this indirect array.
    *
    * The insertion point \a i is constrained to stay in the range
    * [Begin(),End()) of existing array elements.
    *
    * Returns an iterator pointing to the first newly created array element, or
    * \a i if \a n is zero.
    */
   iterator Grow( const_iterator i, size_type n = 1 )
   {
      return iterator( m_array.Insert( (array_iterator)const_cast<iterator>( i ), (void*)nullptr, n ) );
   }

   /*!
    * Inserts a copy of the sequence of pointers contained by the indirect
    * array \a x at the specified location \a i in this indirect array.
    *
    * The insertion point \a i is constrained to stay in the range
    * [Begin(),End()) of existing array elements. The source array \a x can
    * safely be a reference to this array.
    *
    * Returns an iterator pointing to the first newly created array element, or
    * \a i if \a x is empty.
    */
   iterator Insert( const_iterator i, const IndirectArray& x )
   {
      return iterator( m_array.Insert( (array_iterator)const_cast<iterator>( i ), x.m_array ) );
   }

   /*!
    * Inserts a contiguous sequence of \a n pointers equal to \a p at the
    * specified location \a i.
    *
    * The insertion point \a i is constrained to stay in the range
    * [Begin(),End()) of existing array elements.
    *
    * Returns an iterator pointing to the first inserted array element, or \a i
    * if \a n is zero.
    */
   iterator Insert( const_iterator i, const T* p, size_type n = 1 )
   {
      return iterator( m_array.Insert( (array_iterator)const_cast<iterator>( i ), (void*)p, n ) );
   }

   /*!
    * Inserts a copy of the sequence of pointers defined by the range [p,q)
    * of forward iterators at the specified location \a i.
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
   iterator Insert( const_iterator i, FI p, FI q )
   {
      return iterator( m_array.Insert( (array_iterator)const_cast<iterator>( i ), p, q ) );
   }

   /*!
    * Appends a copy of the sequence of pointers contained by the indirect
    * array \a x to this indirect array.
    */
   void Append( const IndirectArray& x )
   {
      m_array.Append( x.m_array );
   }

   /*!
    * Appends a contiguous sequence of \a n pointers equal to \a p to this
    * indirect array.
    */
   void Append( const T* p, size_type n = 1 )
   {
      m_array.Append( (void*)p, n );
   }

   /*!
    * Appends a copy of the sequence of pointers defined by the range [p,q)
    * of forward iterators to this indirect array.
    *
    * \note \a p and \a q must not be iterators into this array.
    */
   template <class FI>
   void Append( FI p, FI q )
   {
      m_array.Append( p, q );
   }

   /*!
    * Inserts a copy of the sequence of pointers contained by the indirect
    * array \a x at the beginning of this indirect array.
    */
   void Prepend( const IndirectArray& x )
   {
      m_array.Prepend( x.m_array );
   }

   /*!
    * Inserts a contiguous sequence of \a n pointers equal to \a p at the
    * beginning of this indirect array.
    */
   void Prepend( const T* p, size_type n = 1 )
   {
      m_array.Prepend( (void*)p, n );
   }

   /*!
    * Inserts a copy of the sequence of pointers defined by the range [p,q) of
    * forward iterators at the beginning of this indirect array.
    *
    * \note \a p and \a q must not be iterators into this array.
    */
   template <class FI>
   void Prepend( FI p, FI q )
   {
      m_array.Prepend( p, q );
   }

   /*!
    * A synonym for Append( const IndirectArray<>& )
    */
   void Add( const IndirectArray& x )
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
    * location \a i in this indirect array.
    *
    * If the starting iterator \a i is located at or after the end of this
    * array, or if \a n is zero, this function does nothing. Otherwise \a i is
    * constrained to stay in the range [Begin(),End()) of existing array
    * elements.
    *
    * Only pointers are removed by this function; the pointed objects are not
    * affected in any way.
    */
   void Remove( const_iterator i, size_type n = 1 )
   {
      m_array.Remove( (array_iterator)const_cast<iterator>( i ), n );
   }

   /*!
    * Removes a sequence of contiguous pointers in the range [i,j) of this
    * indirect array.
    *
    * If the starting iterator \a i is located at or after the end of this
    * array, or if \a j precedes \a i, this function does nothing. Otherwise
    * the range [i,j) is constrained to stay in the range [Begin(),End()) of
    * existing array elements.
    *
    * Only pointers are removed by this function; the pointed objects are not
    * affected in any way.
    */
   void Remove( const_iterator i, const_iterator j )
   {
      m_array.Remove( (array_iterator)const_cast<iterator>( i ),
                      (array_iterator)const_cast<iterator>( j ) );
   }

   /*!
    * Removes all existing non-null pointers whose pointed objects are equal to
    * the specified value \a v in this indirect array.
    *
    * Only pointers are removed by this function; the pointed objects are not
    * affected in any way.
    */
   void Remove( const T& v )
   {
      array_implementation r;
      for ( const_iterator i = ConstBegin(); i < ConstEnd(); ++i )
         if ( *i == nullptr || **i != v )
            r.Add( (void*)*i );
      if ( r.Length() < m_array.Length() )
         m_array.Transfer( r );
   }

   /*!
    * Removes every non-null pointer x in this array such that the binary
    * predicate p( *x, \a v ) is true.
    *
    * Only pointers are removed by this function; the pointed objects are not
    * affected in any way.
    */
   template <class BP>
   void Remove( const T& v, BP p )
   {
      array_implementation r;
      for ( const_iterator i = ConstBegin(); i < ConstEnd(); ++i )
         if ( *i == nullptr || !p( **i, v ) )
            r.Add( (void*)*i );
      if ( r.Length() < m_array.Length() )
         m_array.Transfer( r );
   }

   /*!
    * Removes all contained pointers equal to \a p in this indirect array.
    *
    * Only pointers are removed by this function; the pointed objects are not
    * affected in any way.
    */
   void Remove( const T* p )
   {
      m_array.Remove( (void*)p );
   }

   /*!
    * Removes all pointers contained by this object, yielding an empty indirect
    * array.
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
    * Destroys a sequence of \a n contiguous pointed objects, starting at the
    * specified location \a i in this indirect array. Null pointers are
    * ignored.
    *
    * This function destroys and deallocates the pointed objects, then replaces
    * the corresponding pointers by \c nullptr in this array. The array length
    * is not modified.
    *
    * \warning See Delete( iterator, iterator ) for critical information on
    * this member function.
    */
   void Delete( iterator i, size_type n = 1 )
   {
      Delete( i, i+n );
   }

   /*!
    * Destroys the objects pointed to by a range [i,j) of pointers in this
    * indirect array. Null pointers are ignored.
    *
    * This function destroys and deallocates the pointed objects, then replaces
    * the corresponding pointers by \c nullptr in this array. The array length
    * is not modified.
    *
    * \warning This function can be useful but is potentially dangerous. If the
    * array contains duplicate non-null pointers in the specified range of
    * iterators, this function will lead to a crash as a result of multiple
    * deletions. To minimize the risk of multiple deletions, this function
    * ignores the normal data sharing mechanism so that all objects sharing the
    * same array data structure will correctly 'see' the deleted pointers as
    * null pointers. However, be aware of potential problems if other indirect
    * containers store pointers to deleted objects in different data
    * structures, which will be invalid after calling this function.
    */
   void Delete( iterator i, iterator j )
   {
      // NB: Copy-on-write must *not* happen in this function.
      if ( i < ConstEnd() )
      {
         i = pcl::Max( const_cast<iterator>( ConstBegin() ), i );
         j = pcl::Min( j, const_cast<iterator>( ConstEnd() ) );
         allocator a;
         for ( ; i < j; ++i )
            if ( *i != nullptr )
               DeleteObject( i, a );
      }
   }

   /*!
    * Destroys all objects equal to \a v pointed to by non-null pointers
    * contained by this array.
    *
    * This function destroys and deallocates the pointed objects, then replaces
    * the corresponding pointers by \c nullptr in this array. The array length
    * is not modified.
    *
    * \warning See Delete( iterator, iterator ) for critical information on
    * this member function.
    */
   void Delete( const T& v )
   {
      // NB: Copy-on-write must *not* happen in this function.
      allocator a;
      for ( iterator i = const_cast<iterator>( ConstBegin() ); i < ConstEnd(); ++i )
         if ( *i != nullptr && **i == v )
            DeleteObject( i, a );
   }

   /*!
    * Destroys every object pointed to by a pointer x in this array such that
    * the binary predicate p( *x, \a v ) is true.
    *
    * This function destroys and deallocates the pointed objects, then replaces
    * the corresponding pointers by \c nullptr in this array. The array length
    * is not modified.
    *
    * \warning See Delete( iterator, iterator ) for critical information on
    * this member function.
    */
   template <class BP>
   void Delete( const T& v, BP p )
   {
      // NB: Copy-on-write must *not* happen in this function.
      allocator a;
      for ( iterator i = const_cast<iterator>( ConstBegin() ); i < ConstEnd(); ++i )
         if ( *i != nullptr && p( **i, v ) )
            DeleteObject( i, a );
   }

   /*!
    * Destroys all objects pointed to by non-null pointers in this indirect
    * array. All pointers contained are set equal to \c nullptr, and the array
    * length is not modified.
    *
    * \warning See Delete( iterator, iterator ) for critical information on
    * this member function.
    */
   void Delete()
   {
      // NB: Copy-on-write must *not* happen in this function.
      allocator a;
      for ( iterator i = const_cast<iterator>( ConstBegin() ); i < ConstEnd(); ++i )
         if ( *i != nullptr )
            DeleteObject( i, a );
   }

   /*!
    * Destroys and removes a sequence of \a n contiguous pointed objects,
    * starting at the specified location \a i in this indirect array. Null
    * pointers are ignored.
    *
    * This function destroys and deallocates the pointed objects, then removes
    * the corresponding pointers in this array. The array length is decreased
    * by the number of destroyed objects.
    *
    * \warning See Delete( iterator, iterator ) for critical information on
    * this member function.
    */
   void Destroy( iterator i, size_type n = 1 )
   {
      Delete( i, n );
      Remove( i, n );
   }

   /*!
    * Destroys and removes the objects pointed to by a range [i,j) of pointers
    * in this indirect array. Null pointers are ignored.
    *
    * This function destroys and deallocates the pointed objects, then removes
    * the corresponding pointers in this array. The array length is decreased
    * by the number of destroyed objects.
    *
    * \warning See Delete( iterator, iterator ) for critical information on
    * this member function.
    */
   void Destroy( iterator i, iterator j )
   {
      Delete( i, j );
      Remove( i, j );
   }

   /*!
    * Destroys and removes all objects equal to \a v pointed to by non-null
    * pointers contained by this array.
    *
    * This function destroys and deallocates the pointed objects, then removes
    * the corresponding pointers in this array. The array length is decreased
    * by the number of destroyed objects.
    *
    * \warning See Delete( iterator, iterator ) for critical information on
    * this member function.
    */
   void Destroy( const T& v )
   {
      array_implementation r;
      allocator a;
      for ( iterator i = const_cast<iterator>( ConstBegin() ); i < ConstEnd(); ++i )
         if ( *i != nullptr && **i == v )
            DeleteObject( i, a );
         else
            r.Add( (void*)*i );
      // NB: Copy-on-write must *not* happen before this point.
      if ( r.Length() < m_array.Length() )
         m_array.Transfer( r );
   }

   /*!
    * Destroys and removes every object pointed to by a pointer x in this array
    * such that the binary predicate p( *x, \a v ) is true.
    *
    * This function destroys and deallocates the pointed objects, then removes
    * the corresponding pointers in this array. The array length is decreased
    * by the number of destroyed objects.
    *
    * \warning See Delete( iterator, iterator ) for critical information on
    * this member function.
    */
   template <class BP>
   void Destroy( const T& v, BP p )
   {
      array_implementation r;
      allocator a;
      for ( iterator i = const_cast<iterator>( ConstBegin() ); i < ConstEnd(); ++i )
         if ( *i != nullptr && p( **i, v ) )
            DeleteObject( i, a );
         else
            r.Add( (void*)*i );
      // NB: Copy-on-write must *not* happen before this point.
      if ( r.Length() < m_array.Length() )
         m_array.Transfer( r );
   }

   /*!
    * Destroys all objects pointed to by non-null pointers in this indirect
    * array, and removes all pointers, yielding an empty array.
    *
    * \warning See Delete( iterator, iterator ) for critical information on
    * this member function.
    */
   void Destroy()
   {
      Delete();
      Clear();
   }

   /*!
    * Removes all null pointers from this array.
    */
   void Pack()
   {
      m_array.Remove( (void*)nullptr );
   }

   /*!
    * Replaces a sequence of contiguous pointers defined by the range [i,j) of
    * iterators in this array by the pointers stored in an indirect array \a x.
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
   iterator Replace( const_iterator i, const_iterator j, const IndirectArray& x )
   {
      return iterator( m_array.Replace( (array_iterator)const_cast<iterator>( i ),
                                        (array_iterator)const_cast<iterator>( j ), x.m_array ) );
   }

   /*!
    * Replaces a sequence of contiguous pointers defined by the range [i,j) in
    * this indirect array by \a n copies of the specified pointer \a p.
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
   iterator Replace( const_iterator i, const_iterator j, const T* p, size_type n = 1 )
   {
      return iterator( m_array.Replace( (array_iterator)const_cast<iterator>( i ),
                                        (array_iterator)const_cast<iterator>( j ), (void*)p, n ) );
   }

   /*!
    * Replaces a sequence of contiguous pointers defined by the range [i,j) in
    * this indirect array by the sequence of pointers in the range [p,q) of
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
   iterator Replace( const_iterator i, const_iterator j, FI p, FI q )
   {
      return iterator( m_array.Replace( (array_iterator)const_cast<iterator>( i ),
                                        (array_iterator)const_cast<iterator>( j ), p, q ) );
   }

   /*!
    * Ensures that this indirect array has enough capacity to store \a n
    * pointers.
    *
    * After calling this member function with \a n > 0, this object is
    * guaranteed to uniquely reference its pointer array data.
    */
   void Reserve( size_type n )
   {
      m_array.Reserve( n );
   }

   /*!
    * Causes this indirect array to allocate the exact required memory space to
    * store its contained pointers.
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
      Apply( [v]( T* x ){ *x = v; } );
   }

   /*!
    * Calls f( T& x ) for every object x pointed to by non-null pointers in
    * this array, successively from the first contained pointer to the last.
    */
   template <class F>
   void Apply( F f )
   {
      pcl::Apply( Begin(), End(), IndirectUnaryFunction<T*, F>( f ) );
   }

   /*!
    * Calls f( const T& x ) for every object x pointed to by non-null pointers
    * in this array, successively from the first contained pointer to the last.
    */
   template <class F>
   void Apply( F f ) const
   {
      pcl::Apply( Begin(), End(), IndirectUnaryFunction<const T*, F>( f ) );
   }

   /*!
    * Returns an iterator pointing to the first non-null pointer in this array
    * that points to an object x such that f( const T& x ) is true. Returns
    * End() if such pointer does not exist.
    */
   template <class F>
   iterator FirstThat( F f ) const
   {
      return const_cast<iterator>( pcl::FirstThat( Begin(), End(), IndirectUnaryPredicate<const T*, F>( f ) ) );
   }

   /*!
    * Returns an iterator pointing to the last non-null pointer in this array
    * that points to an object x such that f( const T& x ) is true. Returns
    * End() if such pointer does not exist.
    */
   template <class F>
   iterator LastThat( F f ) const
   {
      return const_cast<iterator>( pcl::LastThat( Begin(), End(), IndirectUnaryPredicate<const T*, F>( f ) ) );
   }

   /*!
    * Returns the number of objects equal to \a v pointed to by non-null
    * pointers stored in this indirect array.
    */
   size_type Count( const T& v ) const
   {
      return pcl::Count( Begin(), End(), &v, equal() );
   }

   /*!
    * Returns the number of pointers equal to \a p stored in this indirect
    * array.
    */
   size_type Count( const T* p ) const
   {
      return m_array.Count( (void*)p );
   }

   /*!
    * Returns the number of objects pointed to by non-null pointers stored in
    * this indirect array such that for each counted pointer x the binary
    * predicate p( *x, v ) returns true.
    */
   template <class BP>
   size_type Count( const T& v, BP p ) const
   {
      return pcl::Count( Begin(), End(), &v, IndirectBinaryPredicate<const T*, const T*, BP>( p ) );
   }

   /*!
    * Returns the number of objects pointed to by non-null pointers stored in
    * this indirect array such that for each counted pointer x the unary
    * predicate p( *x ) returns true.
    */
   template <class UP>
   size_type CountIf( UP p ) const
   {
      return pcl::CountIf( Begin(), End(), IndirectUnaryPredicate<const T*, UP>( p ) );
   }

   /*! #
    */
   iterator MinItem() const
   {
      return const_cast<iterator>( pcl::MinItem( Begin(), End(), less() ) );
   }

   /*! #
    */
   template <class BP>
   iterator MinItem( BP p ) const
   {
      return const_cast<iterator>( pcl::MinItem( Begin(), End(),
                                                 IndirectBinaryPredicate<const T*, const T*, BP>( p ) ) );
   }

   /*! #
    */
   iterator MaxItem() const
   {
      return const_cast<iterator>( pcl::MaxItem( Begin(), End(), less() ) );
   }

   /*! #
    */
   template <class BP>
   iterator MaxItem( BP p ) const
   {
      return const_cast<iterator>( pcl::MaxItem( Begin(), End(),
                                                 IndirectBinaryPredicate<const T*, const T*, BP>( p ) ) );
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
      m_array.ShiftLeft( (void*)p, n );
   }

   /*! #
    */
   void ShiftRight( const T* p, size_type n = 1 )
   {
      m_array.ShiftRight( (void*)p, n );
   }

   /*! #
    */
   iterator Search( const T& v ) const
   {
      return const_cast<iterator>( pcl::LinearSearch( Begin(), End(), &v, equal() ) );
   }

   /*! #
    */
   iterator Search( const T* p ) const
   {
      return iterator( m_array.Search( (void*)p ) );
   }

   /*! #
    */
   template <class BP>
   iterator Search( const T& v, BP p ) const
   {
      return const_cast<iterator>( pcl::LinearSearch( Begin(), End(), &v,
                                                      IndirectBinaryPredicate<const T*, const T*, BP>( p ) ) );
   }

   /*! #
    */
   iterator SearchLast( const T& v ) const
   {
      return const_cast<iterator>( pcl::LinearSearchLast( Begin(), End(), &v, equal() ) );
   }

   /*! #
    */
   iterator SearchLast( const T* p ) const
   {
      return iterator( m_array.SearchLast( (void*)p ) );
   }

   /*! #
    */
   template <class BP>
   iterator SearchLast( const T& v, BP p ) const
   {
      return const_cast<iterator>( pcl::LinearSearchLast( Begin(), End(), &v,
                                                          IndirectBinaryPredicate<const T*, const T*, BP>( p ) ) );
   }

   /*! #
    */
   template <class FI>
   iterator SearchSubset( FI i, FI j ) const
   {
      return const_cast<iterator>( pcl::Search( Begin(), End(), i, j, equal() ) );
   }

   /*! #
    */
   template <class FI, class BP>
   iterator SearchSubset( FI i, FI j, BP p ) const
   {
      return const_cast<iterator>( pcl::Search( Begin(), End(), i, j,
                                                IndirectBinaryPredicate<const T*, const T*, BP>( p ) ) );
   }

   /*! #
    */
   template <class C>
   iterator SearchSubset( const C& c ) const
   {
      return const_cast<iterator>( pcl::Search( Begin(), End(), c.Begin(), c.End(), equal() ) );
   }

   /*! #
    */
   template <class C, class BP>
   iterator SearchSubset( const C& c, BP p ) const
   {
      return const_cast<iterator>( pcl::Search( Begin(), End(), c.Begin(), c.End(),
                                                IndirectBinaryPredicate<const T*, const T*, BP>( p ) ) );
   }

   /*! #
    */
   template <class BI>
   iterator SearchLastSubset( BI i, BI j ) const
   {
      return const_cast<iterator>( pcl::SearchLast( Begin(), End(), i, j, equal() ) );
   }

   /*! #
    */
   template <class BI, class BP>
   iterator SearchLastSubset( BI i, BI j, BP p ) const
   {
      return const_cast<iterator>( pcl::SearchLast( Begin(), End(), i, j,
                                                    IndirectBinaryPredicate<const T*, const T*, BP>( p ) ) );
   }

   /*! #
    */
   template <class C>
   iterator SearchLastSubset( const C& c ) const
   {
      return const_cast<iterator>( pcl::SearchLast( Begin(), End(), c.Begin(), c.End(), equal() ) );
   }

   /*! #
    */
   template <class C, class BP>
   iterator SearchLastSubset( const C& c, BP p ) const
   {
      return const_cast<iterator>( pcl::SearchLast( Begin(), End(), c.Begin(), c.End(),
                                                    IndirectBinaryPredicate<const T*, const T*, BP>( p ) ) );
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
      return m_array.Contains( (void*)p );
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
      return SearchSubset( c ) != End();
   }

   /*! #
    */
   template <class C, class BP>
   iterator ContainsSubset( const C& c, BP p ) const
   {
      return SearchSubset( c, p ) != End();
   }

   /*! #
    */
   void Sort()
   {
      pcl::QuickSort( Begin(), End(), less() );
   }

   /*! #
    */
   template <class BP>
   void Sort( BP p )
   {
      pcl::QuickSort( Begin(), End(), IndirectBinaryPredicate<const T*, const T*, BP>( p ) );
   }

   /*!
    * Exchanges two indirect arrays \a x1 and \a x2.
    */
   friend void Swap( IndirectArray& x1, IndirectArray& x2 )
   {
      pcl::Swap( x1.m_array, x2.m_array );
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
      const_iterator i = Begin();
      if ( i < End() )
      {
         while ( *i == nullptr )
            if ( ++i == End() )
               return s;
         s.Append( S( **i ) );
         if ( ++i < End() )
         {
            S p( separator );
            do
               if ( *i != nullptr )
               {
                  s.Append( p );
                  s.Append( S( **i ) );
               }
            while ( ++i < End() );
         }
      }
      return s;
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
      const_iterator i = Begin();
      if ( i < End() )
      {
         while ( *i == nullptr )
            if ( ++i == End() )
               return s;
         append( s, S( **i ) );
         if ( ++i < End() )
         {
            S p( separator );
            do
               if ( *i != nullptr )
               {
                  append( s, p );
                  append( s, S( **i ) );
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

   void DeleteObject( iterator i, allocator& a )
   {
      pcl::Destroy( *i );
      a.Deallocate( *i );
      *i = nullptr;
   }

   template <class C>
   void CloneObjects( const C& x, DirectContainer<T>* )
   {
      Clear();
      Reserve( x.Length() );
      Append( static_cast<T*>( nullptr ), x.Length() );
      iterator i = Begin(), j = End();
      typename C::const_iterator p = x.Begin(), q = x.End();
      for ( allocator a; i < j && p != q; ++i, ++p )
      {
         *i = a.Allocate( 1 );
         pcl::Construct( *i, *p, a );
      }
   }

   template <class C>
   void CloneObjects( const C& x, IndirectContainer<T>* )
   {
      Clear();
      Reserve( x.Length() );
      Append( static_cast<T*>( nullptr ), x.Length() );
      iterator i = Begin(), j = End();
      typename C::const_iterator p = x.Begin(), q = x.End();
      for ( allocator a; i < j && p != q; ++i, ++p )
         if ( *p != nullptr )
         {
            *i = a.Allocate( 1 );
            pcl::Construct( *i, **p, a );
         }
   }
};

// ----------------------------------------------------------------------------

/*!
 * Returns true only if two indirect arrays \a x1 and \a x2 are equal. This
 * operator compares the objects pointed to by the pointers stored in the
 * indirect arrays.
 * \ingroup array_relational_operators
 */
template <class T, class A> inline
bool operator ==( const IndirectArray<T,A>& x1, const IndirectArray<T,A>& x2 )
{
   return x1.Length() == x2.Length() && pcl::Equal( x1.Begin(), x2.Begin(), x2.End(), IndirectArray<T,A>::equal() );
}

/*!
 * Returns true only if an indirect array \a x1 precedes another indirect array
 * \a x2. This operator compares the objects pointed to by the pointers stored
 * in the indirect arrays.
 * \ingroup array_relational_operators
 */
template <class T, class A> inline
bool operator <( const IndirectArray<T,A>& x1, const IndirectArray<T,A>& x2 )
{
   return pcl::Compare( x1.Begin(), x1.End(), x2.Begin(), x2.End(), IndirectArray<T,A>::less() ) < 0;
}

/*!
 * Appends a pointer or indirect array \a y to an indirect array \a x. Returns
 * a reference to the left-hand indirect array.
 * \ingroup array_insertion_operators
 */
template <class T, class A, class Y> inline
IndirectArray<T,A>& operator <<( IndirectArray<T,A>& x, Y y )
{
   x.Append( y );
   return x;
}

/*!
 * Appends a pointer or indirect array \a y to an indirect array \a x. Returns
 * a reference to the left-hand indirect array.
 * \ingroup array_insertion_operators
 */
template <class T, class A, class Y> inline
IndirectArray<T,A>& operator <<( IndirectArray<T,A>&& x, Y y )
{
   x.Append( y );
   return x;
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_IndirectArray_h

// ----------------------------------------------------------------------------
// EOF pcl/IndirectArray.h - Released 2016/02/21 20:22:12 UTC
