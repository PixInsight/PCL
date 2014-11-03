// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/Array.h - Released 2014/10/29 07:34:11 UTC
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

#ifndef __PCL_Array_h
#define __PCL_Array_h

/// \file pcl/Array.h

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

#ifndef __PCL_Memory_h
#include <pcl/Memory.h>
#endif

#ifndef __PCL_Rotate_h
#include <pcl/Rotate.h>
#endif

#ifndef __PCL_Search_h
#include <pcl/Search.h>
#endif

#ifndef __PCL_Sort_h
#include <pcl/Sort.h>
#endif

#ifndef __PCL_Indirect_h
#include <pcl/Indirect.h>
#endif

#ifndef __PCL_Utility_h
#include <pcl/Utility.h>
#endif

#ifndef __PCL_Relational_h
#include <pcl/Relational.h>
#endif

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
 * ### TODO: Write a detailed description for %Array.
 *
 * \ingroup dynamic_arrays
 */
template <class T, class A = StandardAllocator>
class PCL_CLASS Array : public DirectContainer<T>
{
public:

   /*! #
   */
   typedef A
      block_allocator;

   /*! #
   */
   typedef Allocator<T,A>
      allocator;

   /*! #
   */
   typedef T*
      iterator;

   /*! #
   */
   typedef const T*
      const_iterator;

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
      Constructs an empty array.
   */
   explicit
   Array( const A& a = A() ) : data( new Data( a ) )
   {
   }

   /*!
      Constructs an array of length \a n.
   */
   explicit
   Array( size_type n, const A& a = A() ) : data( new Data( a ) )
   {
      data->Allocate( n );
      data->Initialize( data->begin, data->end );
   }

   /*!
      Constructs an array of \a n objects with constant value \a v.
   */
   Array( size_type n, const T& v, const A& a = A() ) : data( new Data( a ) )
   {
      data->Allocate( n );
      data->Initialize( data->begin, data->end, v );
   }

   /*! #
   */
   template <class FI>
   Array( FI i, FI j, const A& a = A() ) : data( new Data( a ) )
   {
      data->Allocate( size_type( pcl::Distance( i, j ) ) );
      if ( data->begin != 0 )
         data->Build( data->begin, i, j );
   }

   /*!
      Copy constructor.
   */
   Array( const Array<T,A>& x ) : data( x.data )
   {
      if ( data != 0 )
         data->Attach();
   }

   /*!
      Destroys an %Array object.
      Destroys and deallocates all contained objects.
   */
   ~Array()
   {
      if ( data != 0 )
      {
         if ( !data->Detach() )
            delete data;
         data = 0;
      }
   }

   /*!
      Returns true if this array uniquely references its contained data.
   */
   bool IsUnique() const
   {
      return data->IsUnique();
   }

   /*!
      Returns true if this array is an alias of a specified array \a x.

      Two objects are aliases if both of them share the same data.
   */
   bool IsAliasOf( const Array<T,A>& x ) const
   {
      return data == x.data;
   }

   /*!
      Ensures that this array uniquely references its contained data.

      If necessary, this member function generates a duplicate of the array
      data, references it, and then decrements the reference counter of the
      original array data.
   */
   void SetUnique()
   {
      if ( !IsUnique() )
      {
         Data* newData = new Data( data->alloc );
         newData->Allocate( Length() );
         newData->Build( newData->begin, data->begin, data->end );
         if ( !data->Detach() )
            delete data;
         data = newData;
      }
   }

   /*!
      Returns the total number of bytes required to store the data contained in
      this array.
   */
   size_type Size() const
   {
      return data->Size();
   }

   /*!
      Returns the length of this array.
   */
   size_type Length() const
   {
      return data->Length();
   }

   /*!
      Returns the capacity of this array. The capacity is the maximum number of
      objects that this array can contain without requiring a reallocation.
   */
   size_type Capacity() const
   {
      return data->Capacity();
   }

   /*!
      Returns the existing available space in this array. The available space
      is the number of objects that can be added to this array without
      requiring a reallocation. It is equal to Capacity() - Length().
   */
   size_type Available() const
   {
      return data->Available();
   }

   /*!
      Returns true if this array is empty.
   */
   bool IsEmpty() const
   {
      return data->IsEmpty();
   }

   /*!
      Returns the minimum legal index in this array (always zero).
   */
   size_type LowerBound() const
   {
      return 0;
   }

   /*!
      Returns the maximum legal index in this array. It is equal to Length()-1.
      For empty arrays, UpperBound() returns a meaningless value.
   */
   size_type UpperBound() const
   {
      return Length()-1;
   }

   /*!
      Returns a reference to the allocator object used by this array.
   */
   const A& GetAllocator() const
   {
      return data->alloc;
   }

   /*!
      Sets a new allocator object for this array.
   */
   void SetAllocator( const A& a )
   {
      SetUnique();
      data->alloc = a;
   }

   /*!
      Returns an iterator located at the specified index \a i.
   */
   iterator At( size_type i )
   {
      PCL_PRECONDITION( i >= 0 && i < Length() )
      SetUnique();
      return data->begin + i;
   }

   /*!
      Returns an immutable iterator located at the specified index \a i.
   */
   const_iterator At( size_type i ) const
   {
      PCL_PRECONDITION( i >= 0 && i < Length() )
      return data->begin + i;
   }

   /*!
    * Returns a mutable iterator pointing at the same array element as the
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
      return At( i - data->begin );
   }

   /*!
      Returns a reference to the element at the specified index \a i.
   */
   T& operator []( size_type i )
   {
      return *At( i );
   }

   /*!
      Returns a constant reference to the element at the specified index \a i.
   */
   const T& operator []( size_type i ) const
   {
      return *At( i );
   }

   /*!
      Returns a reference to the first element of this array.
   */
   T& operator *()
   {
      PCL_PRECONDITION( data->begin != 0 )
      SetUnique();
      return *data->begin;
   }

   /*!
      Returns a constant reference to the first element of this array.
   */
   const T& operator *() const
   {
      PCL_PRECONDITION( data->begin != 0 )
      return *data->begin;
   }

   /*!
      Returns a mutable iterator located at the beginning of this array.
   */
   iterator Begin()
   {
      SetUnique();
      return data->begin;
   }

   /*!
      Returns an immutable iterator located at the beginning of this array.
   */
   const_iterator Begin() const
   {
      return data->begin;
   }

   /*!
      Returns an immutable iterator located at the beginning of this array.
   */
   const_iterator ConstBegin() const
   {
      return data->begin;
   }

   /*!
      Returns a mutable iterator located at the end of this array.
   */
   iterator End()
   {
      SetUnique();
      return data->end;
   }

   /*!
      Returns an immutable iterator located at the end of this array.
   */
   const_iterator End() const
   {
      return data->end;
   }

   /*!
      Returns an immutable iterator located at the end of this array.
   */
   const_iterator ConstEnd() const
   {
      return data->end;
   }

   /*!
      Returns a mutable reverse iterator located at the <em>reverse
      beginning</em> of this array.

      The reverse beginning corresponds to the last element in the array.
   */
   reverse_iterator ReverseBegin()
   {
      PCL_PRECONDITION( data->end != 0 )
      SetUnique();
      return data->end - 1;
   }

   /*!
      Returns an immutable reverse iterator located at the <em>reverse
      beginning</em> of this array.

      The reverse beginning corresponds to the last element in the array.
   */
   const_reverse_iterator ReverseBegin() const
   {
      PCL_PRECONDITION( data->end != 0 )
      return data->end - 1;
   }

   /*!
      Returns an immutable reverse iterator located at the <em>reverse
      beginning</em> of this array.

      The reverse beginning corresponds to the last element in the array.
   */
   const_reverse_iterator ConstReverseBegin() const
   {
      PCL_PRECONDITION( data->end != 0 )
      return data->end - 1;
   }

   /*!
      Returns a mutable reverse iterator located at the <em>reverse end</em> of
      this array.

      The reverse end corresponds to an (nonexistent) element immediately
      before the first element in the array.
   */
   reverse_iterator ReverseEnd()
   {
      PCL_PRECONDITION( data->begin != 0 )
      SetUnique();
      return data->begin - 1;
   }

   /*!
      Returns an immutable reverse iterator located at the <em>reverse end</em>
      of this array.

      The reverse end corresponds to an (nonexistent) element immediately
      before the first element in the array.
   */
   const_reverse_iterator ReverseEnd() const
   {
      PCL_PRECONDITION( data->begin != 0 )
      return data->begin - 1;
   }

   /*!
      Returns an immutable reverse iterator located at the <em>reverse end</em>
      of this array.

      The reverse end corresponds to an (nonexistent) element immediately
      before the first element in the array.
   */
   const_reverse_iterator ConstReverseEnd() const
   {
      PCL_PRECONDITION( data->begin != 0 )
      return data->begin - 1;
   }

   /*!
      Assignment operator.

      Causes this array to reference the same data as another array \a x.
      Returns a reference to this object.
   */
   Array<T,A>& operator =( const Array<T,A>& x )
   {
      Assign( x );
      return *this;
   }

   /*!
      Array assignment.

      Causes this array to reference the same data as another array \a x.
   */
   void Assign( const Array<T,A>& x )
   {
      if ( x.data != data )
      {
         x.data->Attach();
         if ( !data->Detach() )
            delete data;
         data = x.data;
      }
   }

   /*!
    * Replaces the contents of this array with a sequence of \a n objects with
    * value \a v.
    */
   void Assign( const T& v, size_type n = 1 )
   {
      if ( n != 0 )
      {
         if ( !IsUnique() )
         {
            Data* newData = new Data( data->alloc );
            if ( !data->Detach() )
               delete data;
            data = newData;
         }

         if ( Capacity() < n )
         {
            data->Deallocate();
            data->Allocate( n );
         }
         else
         {
            data->Destroy( data->begin, data->end );
            data->end = data->begin + n;
         }

         data->Initialize( data->begin, data->end, v );
      }
      else
         Clear();
   }

   /*! #
   */
   template <class FI>
   void Assign( FI i, FI j )
   {
      PCL_PRECONDITION( i >= data->end || j <= data->begin )
      size_type n = size_type( pcl::Distance( i, j ) );
      if ( n != 0 )
      {
         if ( !IsUnique() )
         {
            Data* newData = new Data( data->alloc );
            if ( !data->Detach() )
               delete data;
            data = newData;
         }

         if ( Capacity() < n )
         {
            data->Deallocate();
            data->Allocate( n );
         }
         else
         {
            data->Destroy( data->begin, data->end );
            data->end = data->begin + n;
         }

         data->Build( data->begin, i, j );
      }
      else
         Clear();
   }

   /*! #
   */
   void Import( iterator i, iterator j )
   {
      PCL_PRECONDITION( i >= data->end || j <= data->begin )
      Clear();
      size_type n = size_type( pcl::Distance( i, j ) );
      if ( n != 0 )
      {
         data->begin = i;
         data->end = data->available = j;
      }
   }

   /*! #
   */
   iterator Release()
   {
      SetUnique();
      iterator b = data->begin;
      data->begin = data->end = data->available = 0;
      return b;
   }

   /*! #
   */
   iterator Grow( iterator i, size_type n = 1 )
   {
      PCL_PRECONDITION( i >= data->begin && i <= data->end )
      if ( n != 0 )
      {
         if ( !IsUnique() )
         {
            distance_type p = i - data->begin;
            SetUnique();
            i = data->begin + p;
         }
         data->Initialize( i = data->UninitializedGrow( i, n ), n );
      }
      return i;
   }

   /*! #
   */
   iterator Insert( iterator i, const Array<T,A>& x )
   {
      return Insert( i, x.data->begin, x.data->end );
   }

   /*! #
   */
   iterator Insert( iterator i, const T& v, size_type n = 1 )
   {
      PCL_PRECONDITION( i >= data->begin && i <= data->end )
      if ( n != 0 )
      {
         if ( !IsUnique() )
         {
            distance_type p = i - data->begin;
            SetUnique();
            i = data->begin + p;
         }
         data->Initialize( i = data->UninitializedGrow( i, n ), n, v );
      }
      return i;
   }

   /*! #
   */
   template <class FI>
   iterator Insert( iterator i, FI i1, FI j1 )
   {
      PCL_PRECONDITION( i >= data->begin && i <= data->end )
      size_type n = size_type( pcl::Distance( i1, j1 ) );
      if ( n != 0 )
      {
         if ( !IsUnique() )
         {
            distance_type p = i - data->begin;
            SetUnique();
            i = data->begin + p;
         }
         data->Build( i = data->UninitializedGrow( i, n ), i1, j1 );
      }
      return i;
   }

   /*! #
   */
   void Append( const Array<T,A>& x )
   {
      Insert( data->end, x );
   }

   /*! #
   */
   void Append( const T& v, size_type n = 1 )
   {
      Insert( data->end, v, n );
   }

   /*! #
   */
   template <class FI>
   void Append( FI i, FI j )
   {
      Insert( data->end, i, j );
   }

   /*! #
   */
   void Prepend( const Array<T,A>& x )
   {
      Insert( data->begin, x );
   }

   /*! #
   */
   void Prepend( const T& v, size_type n = 1 )
   {
      Insert( data->begin, v, n );
   }

   /*! #
   */
   template <class FI>
   void Prepend( FI i, FI j )
   {
      Insert( data->begin, i, j );
   }

   /*!
    * A synonym for Append( const Array& )
    */
   void Add( const Array<T,A>& x )
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
   void Add( FI i, FI j )
   {
      Append( i, j );
   }

   /*! #
   */
   void Remove( iterator i, size_type n = 1 )
   {
      Remove( i, pcl::Min( i+n, data->end ) );
   }

   /*! #
   */
   void Remove( iterator i, iterator j )
   {
      PCL_PRECONDITION( i >= data->begin && i < data->end )
      PCL_PRECONDITION( j >= data->begin && j <= data->end )
      PCL_PRECONDITION( i <= j )
      if ( i < j )
         if ( i > data->begin || j < data->end )
         {
            if ( !IsUnique() )
            {
               distance_type p = i - data->begin;
               distance_type d = j - i;
               SetUnique();
               j = (i = data->begin + p) + d;
            }

            data->Destroy( j = pcl::Copy( i, j, data->end ), data->end );
            data->end = j;
         }
         else
            Clear();
   }

   /*! #
   */
   void Remove( const T& v )
   {
      for ( iterator j = data->end, k = j; j != data->begin; )
      {
         if ( *--k == v )
         {
            if ( !IsUnique() )
            {
               distance_type p = j - data->begin;
               SetUnique();
               j = data->begin + p;
            }

            iterator i;
            for ( i = j; --i != data->begin && *--k == v; );

            Remove( i, j );
            if ( data->begin == 0 )
               return;
         }

         j = k;
      }
   }

   /*! #
   */
   template <class BP>
   void Remove( const T& v, BP p )
   {
      for ( iterator j = data->end, k = j; j != data->begin; )
      {
         if ( p( *--k, v ) )
         {
            if ( !IsUnique() )
            {
               distance_type p = j - data->begin;
               SetUnique();
               j = data->begin + p;
            }

            iterator i;
            for ( i = j; --i != data->begin && p( *--k, v ); );

            Remove( i, j );
            if ( data->begin == 0 )
               return;
         }

         j = k;
      }
   }

   /*! #
   */
   void Clear()
   {
      if ( IsUnique() )
         data->Deallocate();
      else
      {
         Data* newData = new Data( data->alloc );
         if ( !data->Detach() )
            delete data;
         data = newData;
      }
   }

   /*!
      A synonym for Clear().
   */
   void Remove() { Clear(); }

   /*! #
   */
   iterator Replace( iterator i, iterator j, const T& v, size_type n = 1 )
   {
      PCL_PRECONDITION( i >= data->begin && i < data->end )
      PCL_PRECONDITION( j >= data->begin && j <= data->end )
      PCL_PRECONDITION( i <= j )

      if ( i < j )
      {
         size_type d = size_type( j - i );

         if ( !IsUnique() )
         {
            distance_type p = i - data->begin;
            SetUnique();
            j = (i = data->begin + p) + d;
         }

         if ( d < n )
         {
            data->Destroy( i, j );
            data->Initialize( i = data->UninitializedGrow( i, n-d ), n, v );
         }
         else
         {
            iterator k = i + n;
            pcl::Fill( i, k, v );
            Remove( k, j );
            if ( data->begin == 0 )
               i = 0;
         }
      }
      return i;
   }

   /*! #
   */
   template <class FI>
   iterator Replace( iterator i, iterator j, FI i1, FI j1 )
   {
      PCL_PRECONDITION( i >= data->begin && i < data->end )
      PCL_PRECONDITION( j >= data->begin && j <= data->end )
      PCL_PRECONDITION( i <= j )

      if ( i < j )
      {
         size_type d = size_type( j - i );
         size_type n = size_type( pcl::Distance( i1, j1 ) );

         if ( !IsUnique() )
         {
            distance_type p = i - data->begin;
            SetUnique();
            j = (i = data->begin + p) + d;
         }

         if ( d < n )
         {
            data->Destroy( i, j );
            data->Build( i = data->UninitializedGrow( i, n-d ), i1, j1 );
         }
         else
         {
            Remove( pcl::Move( i, i1, j1 ), j );
            if ( data->begin == 0 )
               i = 0;
         }
      }
      return i;
   }

   /*! #
   */
   void Reserve( size_type n )
   {
      if ( n > 0 )
         if ( IsUnique() )
         {
            if ( Capacity() < n )
            {
               iterator b = data->alloc.Allocate( n );
               iterator e = data->Build( b, data->begin, data->end );
               data->Deallocate();
               data->begin = b;
               data->end = e;
               data->available = data->begin + n;
            }
         }
         else
         {
            Data* newData = new Data( data->alloc );
            newData->begin = newData->alloc.Allocate( n );
            newData->end = newData->Build( newData->begin, data->begin, data->end );
            newData->available = newData->begin + n;
            if ( !data->Detach() )
               delete data;
            data = newData;
         }
   }

   /*! #
   */
   void Squeeze()
   {
      if ( IsUnique() )
      {
         if ( Available() != 0 )
         {
            iterator b = data->alloc.Allocate( Length() );
            iterator e = data->Build( b, data->begin, data->end );
            data->Deallocate();
            data->begin = b;
            data->end = data->available = e;
         }
      }
      else
      {
         Data* newData = new Data( data->alloc );
         if ( !IsEmpty() )
         {
            newData->begin = newData->alloc.Allocate( Length() );
            newData->available = newData->end = newData->Build( newData->begin, data->begin, data->end );
         }

         if ( !data->Detach() )
            delete data;
         data = newData;
      }
   }

   /*! #
   */
   template <class F>
   void Apply( F f )
   {
      SetUnique();
      pcl::Apply( data->begin, data->end, f );
   }

   /*! #
   */
   template <class F>
   void Apply( F f ) const
   {
      pcl::Apply( data->begin, data->end, f );
   }

   /*! #
   */
   template <class F>
   iterator FirstThat( F f ) const
   {
      return const_cast<iterator>( pcl::FirstThat( data->begin, data->end, f ) );
   }

   /*! #
   */
   template <class F>
   iterator LastThat( F f ) const
   {
      return const_cast<iterator>( pcl::LastThat( data->begin, data->end, f ) );
   }

   /*! #
   */
   size_type Count( const T& v ) const
   {
      return pcl::Count( data->begin, data->end, v );
   }

   /*! #
   */
   template <class BP>
   size_type Count( const T& v, BP p ) const
   {
      return pcl::Count( data->begin, data->end, v, p );
   }

   /*! #
   */
   template <class UP>
   size_type CountIf( UP p ) const
   {
      return pcl::CountIf( data->begin, data->end, p );
   }

   /*! #
   */
   iterator MinItem() const
   {
      return const_cast<iterator>( pcl::MinItem( data->begin, data->end ) );
   }

   /*! #
   */
   template <class BP>
   iterator MinItem( BP p ) const
   {
      return const_cast<iterator>( pcl::MinItem( data->begin, data->end, p ) );
   }

   /*! #
   */
   iterator MaxItem() const
   {
      return const_cast<iterator>( pcl::MaxItem( data->begin, data->end ) );
   }

   /*! #
   */
   template <class BP>
   iterator MaxItem( BP p ) const
   {
      return const_cast<iterator>( pcl::MaxItem( data->begin, data->end, p ) );
   }

   /*! #
   */
   void Reverse()
   {
      SetUnique();
      pcl::Reverse( data->begin, data->end );
   }

   /*! #
   */
   void Rotate( distance_type n )
   {
      if ( !IsEmpty() && n != 0 )
      {
         SetUnique();
         if ( (n %= Length()) < 0 )
            n += Length();
         pcl::Rotate( data->begin, data->begin+n, data->end );
      }
   }

   /*! #
   */
   void ShiftLeft( const T& v, size_type n = 1 )
   {
      if ( !IsEmpty() && n != 0 )
      {
         SetUnique();
         if ( n >= Length() )
            pcl::Fill( data->begin, data->end, v );
         else
            pcl::ShiftLeft( data->begin, data->begin+n, data->end, v );
      }
   }

   /*! #
   */
   void ShiftRight( const T& v, size_type n = 1 )
   {
      if ( !IsEmpty() && n != 0 )
      {
         SetUnique();
         if ( n >= Length() )
            pcl::Fill( data->begin, data->end, v );
         else
            pcl::ShiftRight( data->begin, data->end-n, data->end, v );
      }
   }

   /*! #
   */
   iterator Search( const T& v ) const
   {
      return const_cast<iterator>( pcl::LinearSearch( data->begin, data->end, v ) );
   }

   /*! #
   */
   template <class BP>
   iterator Search( const T& v, BP p ) const
   {
      return const_cast<iterator>( pcl::LinearSearch( data->begin, data->end, v, p ) );
   }

   /*! #
   */
   iterator SearchLast( const T& v ) const
   {
      return const_cast<iterator>( pcl::LinearSearchLast( data->begin, data->end, v ) );
   }

   /*! #
   */
   template <class BP>
   iterator SearchLast( const T& v, BP p ) const
   {
      return const_cast<iterator>( pcl::LinearSearchLast( data->begin, data->end, v, p ) );
   }

   /*! #
   */
   template <class FI>
   iterator SearchSet( FI i, FI j ) const
   {
      return const_cast<iterator>( pcl::Search( data->begin, data->end, i, j ) );
   }

   /*! #
   */
   template <class FI, class BP>
   iterator SearchSet( FI i, FI j, BP p ) const
   {
      return const_cast<iterator>( pcl::Search( data->begin, data->end, i, j, p ) );
   }

   /*! #
   */
   template <class C>
   iterator SearchSet( const C& c ) const
   {
      return const_cast<iterator>( pcl::Search( data->begin, data->end, c.Begin(), c.End() ) );
   }

   /*! #
   */
   template <class C, class BP>
   iterator SearchSet( const C& c, BP p ) const
   {
      return const_cast<iterator>( pcl::Search( data->begin, data->end, c.Begin(), c.End(), p ) );
   }

   /*! #
   */
   template <class BI>
   iterator SearchSetLast( BI i, BI j ) const
   {
      return const_cast<iterator>( pcl::SearchLast( data->begin, data->end, i, j ) );
   }

   /*! #
   */
   template <class BI, class BP>
   iterator SearchSetLast( BI i, BI j, BP p ) const
   {
      return const_cast<iterator>( pcl::SearchLast( data->begin, data->end, i, j, p ) );
   }

   /*! #
   */
   template <class C>
   iterator SearchSetLast( const C& c ) const
   {
      return const_cast<iterator>( pcl::SearchLast( data->begin, data->end, c.Begin(), c.End() ) );
   }

   /*! #
   */
   template <class C, class BP>
   iterator SearchSetLast( const C& c, BP p ) const
   {
      return const_cast<iterator>( pcl::SearchLast( data->begin, data->end, c.Begin(), c.End(), p ) );
   }

   /*! #
   */
   bool Has( const T& v ) const
   {
      return Search( v ) != data->end;
   }

   /*! #
   */
   template <class BP>
   bool Has( const T& v, BP p ) const
   {
      return Search( v, p ) != data->end;
   }

   /*! #
   */
   template <class FI>
   iterator HasSet( FI i, FI j ) const
   {
      return SearchSet( i, j ) != data->end;
   }

   /*! #
   */
   template <class FI, class BP>
   iterator HasSet( FI i, FI j, BP p ) const
   {
      return SearchSet( i, j, p ) != data->end;
   }

   /*! #
   */
   template <class C>
   iterator HasSet( const C& c ) const
   {
      return SearchSet( c ) != data->end;
   }

   /*! #
   */
   template <class C, class BP>
   iterator HasSet( const C& c, BP p ) const
   {
      return SearchSet( c, p ) != data->end;
   }

   /*! #
   */
   void Sort()
   {
      SetUnique();
      pcl::QuickSort( data->begin, data->end );
   }

   /*! #
   */
   template <class BP>
   void Sort( BP p )
   {
      SetUnique();
      pcl::QuickSort( data->begin, data->end, p );
   }

   /*! #
   */
   friend
   void Swap( Array<T,A>& x1, Array<T,A>& x2 )
   {
      pcl::Swap( x1.data, x2.data );
   }

   /*! #
    */
   template <class S, typename SP>
   S& ToSeparated( S& s, SP separator ) const
   {
      if ( !data->IsEmpty() )
      {
         s.Append( S( *data->begin ) );
         for ( const_iterator i = data->begin; ++i != data->end; )
         {
            s.Append( S( separator ) );
            s.Append( S( *i ) );
         }
      }
      return s;
   }

   /*! #
    */
   template <class S, typename SP, class AF>
   S& ToSeparated( S& s, SP separator, AF append ) const
   {
      if ( !data->IsEmpty() )
      {
         append( s, S( *data->begin ) );
         for ( const_iterator i = data->begin; ++i != data->end; )
         {
            append( s, S( separator ) );
            append( s, S( *i ) );
         }
      }
      return s;
   }

   /*! #
    */
   template <class S>
   S& ToCommaSeparated( S& s ) const
   {
      return ToSeparated( s, ',' );
   }

   /*! #
    */
   template <class S>
   S& ToSpaceSeparated( S& s ) const
   {
      return ToSeparated( s, ' ' );
   }

   /*! #
    */
   template <class S>
   S& ToTabSeparated( S& s ) const
   {
      return ToSeparated( s, '\t' );
   }

   // -------------------------------------------------------------------------

private:

   struct Data : public ReferenceCounter
   {
      iterator  begin, end, available;
      allocator alloc;

      Data( const A& a = A() ) : ReferenceCounter(), begin( 0 ), end( 0 ), available( 0 ), alloc( a )
      {
      }

      ~Data()
      {
         Deallocate();
      }

      size_type Size() const
      {
         return Length()*sizeof( T );
      }

      size_type Length() const
      {
         return end - begin;
      }

      size_type Capacity() const
      {
         return available - begin;
      }

      size_type Available() const
      {
         return available - end;
      }

      bool IsEmpty() const
      {
         return begin == end;
      }

      void Allocate( size_type n )
      {
         if ( n != 0 )
         {
            size_type m = alloc.PagedLength( n );
            begin = alloc.Allocate( m );
            end = begin + n;
            available = begin + m;
         }
      }

      void Deallocate()
      {
         PCL_CHECK( (begin == 0) ? end == 0 : begin < end )
         if ( begin != 0 )
         {
            Destroy( begin, end );
            alloc.Deallocate( begin );
            begin = end = available = 0;
         }
      }

      void Initialize( iterator i, iterator j )
      {
         for ( ; i != j; ++i )
            pcl::Construct( i, alloc );
      }

      void Initialize( iterator i, size_type n )
      {
         for ( ; n > 0; ++i, --n )
            pcl::Construct( i, alloc );
      }

      void Initialize( iterator i, iterator j, const T& v )
      {
         for ( ; i != j; ++i )
            pcl::Construct( i, v, alloc );
      }

      void Initialize( iterator i, size_type n, const T& v )
      {
         for ( ; n > 0; ++i, --n )
            pcl::Construct( i, v, alloc );
      }

      template <class FI>
      iterator Build( iterator i, FI i1, FI j1 )
      {
         for ( ; i1 != j1; ++i, ++i1 )
            pcl::Construct( i, *i1, alloc );
         return i;
      }

      iterator UninitializedGrow( iterator i, size_type n )
      {
         if ( n != 0 )
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

      static void Destroy( iterator i, iterator j )
      {
         for ( ; i != j; ++i )
            pcl::Destroy( i );
      }
   };

   Data* data;
};

// ----------------------------------------------------------------------------

/*!
 * \defgroup miscellaneous_array_operators Miscellaneous Dynamic Array Operators and Functions
 */

/*!
 * Returns true if a dynamic array \a a1 is equal to another dynamic array
 * \a a2.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
bool operator ==( const Array<T,A>& a1, const Array<T,A>& a2 )
{
   return a1.Length() == a2.Length() && Equal( a1.Begin(), a2.Begin(), a2.End() );
}

/*!
 * Returns true if a dynamic array \a a1 is less than another dynamic array
 * \a a2.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
bool operator <( const Array<T,A>& a1, const Array<T,A>& a2 )
{
   return Compare( a1.Begin(), a1.End(), a2.Begin(), a2.End() ) < 0;
}

/*!
 * Appends an object \a x to a dynamic array \a a. Returns a reference to the
 * dynamic array.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
Array<T,A>& operator <<( Array<T,A>& a, const T& x )
{
   a.Append( x ); return a;
}

/*!
 * Appends a dynamic array \a a2 to a dynamic array \a a1. Returns a reference
 * to the left-hand dynamic array \a a1.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
Array<T,A>& operator <<( Array<T,A>& a1, const Array<T,A>& a2 )
{
   a1.Append( a2 ); return a1;
}

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
   typedef A
      block_allocator;

   /*! #
   */
   typedef Allocator<T,A>
      allocator;

   /*! #
   */
   typedef T**
      iterator;

   /*! #
   */
   typedef T* const*
      const_iterator;

   /*! #
   */
   typedef Array<void*, A>
      array_implementation;

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
   typedef IndirectEqual<const T*>
      equal;

   /*! #
   */
   typedef IndirectLess<const T*>
      less;

   /*! #
   */
   explicit
   IndirectArray( const A& a = A() ) : array( a )
   {
   }

   /*! #
   */
   explicit
   IndirectArray( size_type n, const A& a = A() ) : array( a )
   {
      array.Append( (void*)0, n );
   }

   /*! #
   */
   IndirectArray( size_type n, const T* p, const A& a = A() ) : array( a )
   {
      array.Append( (void*)p, n );
   }

   /*! #
   */
   template <class FI>
   IndirectArray( FI i, FI j, const A& a = A() ) : array( i, j, a )
   {
   }

   /*! #
   */
   IndirectArray( const IndirectArray<T,A>& x ) : array( x.array )
   {
   }

   /*!
    * Destroys an %IndirectArray object.
    *
    * Deallocates the internal list of pointers to objects, but does not
    * destroy the contained objects. To destroy them, you have to call
    * Destroy() explicitly.
    */
   ~IndirectArray()
   {
   }

   /*!
      Returns true if this indirect array uniquely references its contained
      data pointers.
   */
   bool IsUnique() const
   {
      return array.IsUnique();
   }

   /*!
      Returns true if this indirect array is an alias of a specified indirect
      array \a x.

      Two objects are aliases if both of them share the same data. Two indirect
      containers are aliases if they share a unique set of data pointers.
   */
   bool IsAliasOf( const IndirectArray<T,A>& x ) const
   {
      return array.IsAliasOf( x.array );
   }

   /*!
      Ensures that this indirect array uniquely references its contained data
      pointers.

      If necessary, this member function generates a duplicate of the array
      of pointers, references it, and then decrements the reference counter of
      the original pointers array.
   */
   void SetUnique()
   {
      array.SetUnique();
   }

   /*!
      Returns the total number of bytes required to store the array of data
      pointers in this indirect array.
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
   iterator At( size_type i )
   {
      return (iterator)array.At( i );
   }

   /*! #
   */
   const_iterator At( size_type i ) const
   {
      return (const_iterator)array.At( i );
   }

   /*! #
   */
   iterator MutableIterator( const_iterator i )
   {
      return (iterator)array.MutableIterator( (const_array_iterator)i );
   }

   /*! #
   */
   T*& operator []( size_type i )
   {
      return (T*&)array[i];
   }

   /*! #
   */
   const T* operator []( size_type i ) const
   {
      return (T*)array[i];
   }

   /*! #
   */
   T*& operator *()
   {
      PCL_PRECONDITION( !IsEmpty() )
      return (T*&)*Begin();
   }

   /*! #
   */
   const T* operator *() const
   {
      PCL_PRECONDITION( !IsEmpty() )
      return (T*)*Begin();
   }

   /*! #
   */
   iterator Begin()
   {
      return (iterator)array.Begin();
   }

   /*! #
   */
   const_iterator Begin() const
   {
      return (const_iterator)array.Begin();
   }

   /*! #
   */
   const_iterator ConstBegin() const
   {
      return (const_iterator)array.ConstBegin();
   }

   /*! #
   */
   iterator End()
   {
      return (iterator)array.End();
   }

   /*! #
   */
   const_iterator End() const
   {
      return (const_iterator)array.End();
   }

   /*! #
   */
   const_iterator ConstEnd() const
   {
      return (const_iterator)array.ConstEnd();
   }

   /*! #
   */
   reverse_iterator ReverseBegin()
   {
      return (iterator)(array_iterator)array.ReverseBegin();
   }

   /*! #
   */
   const_reverse_iterator ReverseBegin() const
   {
      return (const_iterator)(const_array_iterator)array.ReverseBegin();
   }

   /*! #
   */
   const_reverse_iterator ConstReverseBegin() const
   {
      return (const_iterator)(const_array_iterator)array.ConstReverseBegin();
   }

   /*! #
   */
   reverse_iterator ReverseEnd()
   {
      return (iterator)(array_iterator)array.ReverseEnd();
   }

   /*! #
   */
   const_reverse_iterator ReverseEnd() const
   {
      return (const_iterator)(const_array_iterator)array.ReverseEnd();
   }

   /*! #
   */
   const_reverse_iterator ConstReverseEnd() const
   {
      return (const_iterator)(const_array_iterator)array.ConstReverseEnd();
   }

   /*! #
   */
   T* First()
   {
      return IsEmpty() ? 0 : *Begin();
   }

   /*! #
   */
   const T* First() const
   {
      return IsEmpty() ? 0 : *Begin();
   }

   /*! #
   */
   T* Last()
   {
      return IsEmpty() ? 0 : *ReverseBegin();
   }

   /*! #
   */
   const T* Last() const
   {
      return IsEmpty() ? 0 : *ReverseBegin();
   }

   /*! #
   */
   IndirectArray<T,A>& operator =( const IndirectArray<T,A>& x )
   {
      array = x.array;
      return *this;
   }

   /*! #
   */
   void Assign( const IndirectArray<T,A>& x )
   {
      array.Assign( x.array );
   }

   /*! #
   */
   void Assign( const T* p, size_type n = 1 )
   {
      array.Assign( (void*)p, n );
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
      Clear();
      if ( x.Length() > 0 )
      {
         Reserve( x.Length() );
         Append( (const T*)0, x.Length() );
         CloneAssign( x, (C*)0 );
      }
   }

   /*! #
   */
   void CloneAssign( IndirectArray<T,A>& x )
   {
      if ( &x != this )
      {
         Clear();
         if ( x.Length() > 0 )
         {
            Reserve( x.Length() );
            Append( (const T*)0, x.Length() );
            CloneAssign( x, (IndirectArray<T,A>*)0 );
         }
      }
   }

   /*! #
   */
   void Import( iterator i, iterator j )
   {
      array.Import( (array_iterator)i, (array_iterator)j );
   }

   /*! #
   */
   iterator Release()
   {
      return (iterator)array.Release();
   }

   /*! #
   */
   iterator Grow( iterator i, size_type n = 1 )
   {
      return (iterator)array.Insert( (array_iterator)i, (void*)0, n );
   }

   /*! #
   */
   iterator Insert( iterator i, const IndirectArray<T,A>& x )
   {
      return (iterator)array.Insert( (array_iterator)i, x.array );
   }

   /*! #
   */
   iterator Insert( iterator i, const T* p, size_type n = 1 )
   {
      return (iterator)array.Insert( (array_iterator)i, (void*)p, n );
   }

   /*! #
   */
   template <class FI>
   iterator Insert( iterator i, FI i1, FI j1 )
   {
      return (iterator)array.Insert( (array_iterator)i, i1, j1 );
   }

   /*! #
   */
   void Append( const IndirectArray<T,A>& x )
   {
      array.Append( x.array );
   }

   /*! #
   */
   void Append( const T* p, size_type n = 1 )
   {
      array.Append( (void*)p, n );
   }

   /*! #
   */
   template <class FI>
   void Append( FI i, FI j )
   {
      array.Append( i, j );
   }

   /*! #
   */
   void Prepend( const IndirectArray<T,A>& x )
   {
      array.Prepend( x.array );
   }

   /*! #
   */
   void Prepend( const T* p, size_type n = 1 )
   {
      array.Prepend( (void*)p, n );
   }

   /*! #
   */
   template <class FI>
   void Prepend( FI i, FI j )
   {
      array.Prepend( i, j );
   }

   /*!
    * A synonym for Append( const IndirectArray& )
    */
   void Add( const IndirectArray<T,A>& x )
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

   /*! #
   */
   void Remove( iterator i, size_type n = 1 )
   {
      array.Remove( (array_iterator)i, n );
   }

   /*! #
   */
   void Remove( iterator i, iterator j )
   {
      array.Remove( (array_iterator)i, (array_iterator)j );
   }

   /*! #
   */
   void Remove( const T& v )
   {
      array.SetUnique();
      for ( iterator j = End(), k = j; j != Begin(); )
      {
         if ( *--k != 0 && **k == v )
         {
            iterator i;
            for ( i = j; --i != Begin() && *--k != 0 && **k == v; );

            Remove( i, j );
            if ( Begin() == 0 )
               return;
         }

         j = k;
      }
   }

   /*! #
   */
   void Remove( const T* p )
   {
      array.Remove( (void*)p );
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
   void Delete( iterator i, size_type n = 1 )
   {
      Delete( i, pcl::Min( i+n, End() ) );
   }

   /*! #
   */
   void Delete( iterator i, iterator j )
   {
      allocator a;
      for ( ; i < j; ++i )
         if ( *i != 0 )
            DeleteItem( i, a );
   }

   /*! #
   */
   void Delete( const T& v )
   {
      allocator a;
      for ( iterator i = Begin(); i != End(); ++i )
         if ( *i != 0 && **i == v )
            DeleteItem( i, a );
   }

   /*! #
   */
   void Delete()
   {
      Delete( Begin(), End() );
   }

   /*! #
   */
   void Destroy( iterator i, size_type n = 1 )
   {
      Delete( i, n );
      Remove( i, n );
   }

   /*! #
   */
   void Destroy( iterator i, iterator j )
   {
      Delete( i, j );
      Remove( i, j );
   }

   /*! #
   */
   void Destroy( const T& v )
   {
      array_implementation tmp( array );
      tmp.SetUnique();
      allocator a;
      for ( iterator i = const_cast<iterator>( ConstBegin() ), t = tmp.Begin(); i != ConstEnd(); ++i, ++t )
         if ( *i != 0 && **i == v )
         {
            DeleteItem( i, a );
            *t = (T*)1;
         }
      tmp.Remove( (void*)1 );
      array = tmp;
   }

   /*! #
   */
   void Destroy()
   {
      Delete();
      Clear();
   }

   /*! #
   */
   void Pack()
   {
      Remove( (T*)0 );
   }

   /*! #
   */
   iterator Replace( iterator i, iterator j, const T* p, size_type n = 1 )
   {
      return (iterator)array.Replace( (array_iterator)i, (array_iterator)j, (void*)p, n );
   }

   /*! #
   */
   template <class FI>
   iterator Replace( iterator i, iterator j, FI i1, FI j1 )
   {
      return (iterator)array.Replace( (array_iterator)i, (array_iterator)j, i1, j1 );
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
      pcl::Apply( Begin(), End(), IndirectUnaryFunction<T*, F>( f ) );
   }

   /*! #
   */
   template <class F>
   void Apply( F f ) const
   {
      pcl::Apply( Begin(), End(), IndirectUnaryFunction<const T*, F>( f ) );
   }

   /*! #
   */
   template <class F>
   iterator FirstThat( F f ) const
   {
      return const_cast<iterator>( pcl::FirstThat( Begin(), End(), IndirectUnaryPredicate<const T*, F>( f ) ) );
   }

   /*! #
   */
   template <class F>
   iterator LastThat( F f ) const
   {
      return const_cast<iterator>( pcl::LastThat( Begin(), End(), IndirectUnaryPredicate<const T*, F>( f ) ) );
   }

   /*! #
   */
   size_type Count( const T& v ) const
   {
      return pcl::Count( Begin(), End(), &v, equal() );
   }

   /*! #
   */
   size_type Count( const T* p ) const
   {
      return array.Count( (T* const&)p );
   }

   /*! #
   */
   template <class BP>
   size_type Count( const T& v, BP p ) const
   {
      return pcl::Count( Begin(), End(), &v, IndirectBinaryPredicate<const T*, const T*, BP>( p ) );
   }

   /*! #
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
      array.ShiftLeft( (void*)p, n );
   }

   /*! #
   */
   void ShiftRight( const T* p, size_type n = 1 )
   {
      array.ShiftRight( (void*)p, n );
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
      return (iterator)array.Search( (void*)p );
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
      return (iterator)array.SearchLast( (void*)p );
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
   iterator SearchSet( FI i, FI j ) const
   {
      return const_cast<iterator>( pcl::Search( Begin(), End(), i, j, equal() ) );
   }

   /*! #
   */
   template <class FI, class BP>
   iterator SearchSet( FI i, FI j, BP p ) const
   {
      return const_cast<iterator>( pcl::Search( Begin(), End(), i, j,
                                   IndirectBinaryPredicate<const T*, const T*, BP>( p ) ) );
   }

   /*! #
   */
   template <class C>
   iterator SearchSet( const C& c ) const
   {
      return const_cast<iterator>( pcl::Search( Begin(), End(), c.Begin(), c.End(), equal() ) );
   }

   /*! #
   */
   template <class C, class BP>
   iterator SearchSet( const C& c, BP p ) const
   {
      return const_cast<iterator>( pcl::Search( Begin(), End(), c.Begin(), c.End(),
                                   IndirectBinaryPredicate<const T*, const T*, BP>( p ) ) );
   }

   /*! #
   */
   template <class BI>
   iterator SearchSetLast( BI i, BI j ) const
   {
      return const_cast<iterator>( pcl::SearchLast( Begin(), End(), i, j, equal() ) );
   }

   /*! #
   */
   template <class BI, class BP>
   iterator SearchSetLast( BI i, BI j, BP p ) const
   {
      return const_cast<iterator>( pcl::SearchLast( Begin(), End(), i, j,
                                   IndirectBinaryPredicate<const T*, const T*, BP>( p ) ) );
   }

   /*! #
   */
   template <class C>
   iterator SearchSetLast( const C& c ) const
   {
      return const_cast<iterator>( pcl::SearchLast( Begin(), End(), c.Begin(), c.End(), equal() ) );
   }

   /*! #
   */
   template <class C, class BP>
   iterator SearchSetLast( const C& c, BP p ) const
   {
      return const_cast<iterator>( pcl::SearchLast( Begin(), End(), c.Begin(), c.End(),
                                   IndirectBinaryPredicate<const T*, const T*, BP>( p ) ) );
   }

   /*! #
   */
   bool Has( const T& v ) const
   {
      return Search( v ) != End();
   }

   /*! #
   */
   bool Has( const T* p ) const
   {
      return array.Has( (void*)p );
   }

   /*! #
   */
   template <class BP>
   bool Has( const T& v, BP p ) const
   {
      return Search( v, p ) != End();
   }

   /*! #
   */
   template <class FI>
   iterator HasSet( FI i, FI j ) const
   {
      return SearchSet( i, j ) != End();
   }

   /*! #
   */
   template <class FI, class BP>
   iterator HasSet( FI i, FI j, BP p ) const
   {
      return SearchSet( i, j, p ) != End();
   }

   /*! #
   */
   template <class C>
   iterator HasSet( const C& c ) const
   {
      return SearchSet( c ) != End();
   }

   /*! #
   */
   template <class C, class BP>
   iterator HasSet( const C& c, BP p ) const
   {
      return SearchSet( c, p ) != End();
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

   /*! #
   */
   friend
   void Swap( IndirectArray<T,A>& x1, IndirectArray<T,A>& x2 )
   {
      pcl::Swap( x1.array, x2.array );
   }

   /*! #
    */
   template <class S, typename SP>
   S& ToSeparated( S& s, SP separator ) const
   {
      if ( !IsEmpty() )
      {
         s.Append( S( **Begin() ) );
         for ( const_iterator i = Begin(); ++i != End(); )
         {
            s.Append( S( separator ) );
            s.Append( S( **i ) );
         }
      }
      return s;
   }

   /*! #
    */
   template <class S, typename SP, class AF>
   S& ToSeparated( S& s, SP separator, AF append ) const
   {
      if ( !IsEmpty() )
      {
         append( s, S( **Begin() ) );
         for ( const_iterator i = Begin(); ++i != End(); )
         {
            append( s, S( separator ) );
            append( s, S( **i ) );
         }
      }
      return s;
   }

   /*! #
    */
   template <class S>
   S& ToCommaSeparated( S& s ) const
   {
      return ToSeparated( s, ',' );
   }

   /*! #
    */
   template <class S>
   S& ToSpaceSeparated( S& s ) const
   {
      return ToSeparated( s, ' ' );
   }

   /*! #
    */
   template <class S>
   S& ToTabSeparated( S& s ) const
   {
      return ToSeparated( s, '\t' );
   }

   // -------------------------------------------------------------------------

private:

   array_implementation array;

   void DeleteItem( iterator i, allocator& a )
   {
      pcl::Destroy( *i );
      a.Deallocate( *i );
      *i = 0;
   }

   template <class C>
   void CloneAssign( const C& x, IndirectContainer<T>* )
   {
      iterator i = Begin();
      typename C::const_iterator j = x.Begin();
      allocator a;
      do
      {
         if ( *j != 0 )
         {
            *i = a.Allocate( 1 );
            pcl::Construct( *i, **j, a );
         }
         else
            *i = 0;
      }
      while ( ++i != End() && ++j != x.End() );
   }

   template <class C>
   void CloneAssign( const C& x, DirectContainer<T>* )
   {
      iterator i = Begin();
      typename C::const_iterator j = x.Begin();
      allocator a;
      do
      {
         *i = a.Allocate( 1 );
         pcl::Construct( *i, *j, a );
      }
      while ( ++i != End() && ++j != x.End() );
   }
};

// ----------------------------------------------------------------------------

/*!
 * Returns true if an indirect dynamic array \a a1 is equal to another indirect
 * dynamic array \a a2.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
bool operator ==( const IndirectArray<T,A>& x1, const IndirectArray<T,A>& x2 )
{
   return x1.Length() == x2.Length() && Equal( x1.Begin(), x2.Begin(), x2.End(), IndirectArray<T,A>::equal() );
}

/*!
 * Returns true if an indirect dynamic array \a a1 is less than another
 * indirect dynamic array \a a2.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
bool operator <( const IndirectArray<T,A>& x1, const IndirectArray<T,A>& x2 )
{
   return Compare( x1.Begin(), x1.End(), x2.Begin(), x2.End(), IndirectArray<T,A>::less() ) < 0;
}

/*!
 * Appends a pointer to an object \a p to an indirect dynamic array \a a.
 * Returns a reference to the indirect dynamic array.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
IndirectArray<T,A>& operator <<( IndirectArray<T,A>& a, const T* p )
{
   a.Append( p ); return a;
}

/*!
 * Appends an indirect dynamic array \a a2 to an indirect dynamic array \a a1.
 * Returns a reference to the left-hand indirect dynamic array \a a1.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
IndirectArray<T,A>& operator <<( IndirectArray<T,A>& a1, const IndirectArray<T,A>& a2 )
{
   a1.Append( a2 ); return a1;
}

// ----------------------------------------------------------------------------

/*!
 * \class SortedArray
 * \brief Generic dynamic sorted array.
 *
 * ### TODO: Write a detailed description for %SortedArray.
 *
 * \ingroup dynamic_arrays
 */
template <class T, class A = StandardAllocator>
class PCL_CLASS SortedArray : public DirectContainer<T>
{
public:

   /*! #
   */
   typedef Array<T,A>
      array_implementation;

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
   explicit
   SortedArray( const A& a = A() ) : array( a )
   {
   }

   /*! #
   */
   SortedArray( size_type n, const T& v, const A& a = A() ) : array( n, v, a )
   {
   }

   /*! #
   */
   template <class FI>
   SortedArray( FI i, FI j, const A& a = A() ) : array( i, j, a )
   {
      Sort();
   }

   /*! #
   */
   SortedArray( const SortedArray<T,A>& x ) : array( x.array )
   {
   }

   /*! #
   */
   explicit
   SortedArray( const array_implementation& x ) : array( x )
   {
      Sort();
   }

   /*!
    * Destroys a %SortedArray object.
    *
    * Destroys and deallocates all contained objects.
    */
   ~SortedArray()
   {
   }

   /*!
      Returns true if this array uniquely references its contained data.
   */
   bool IsUnique() const
   {
      return array.IsUnique();
   }

   /*!
      Returns true if this sorted array is an alias of a specified sorted
      array \a x.

      Two objects are aliases if both of them share the same data.
   */
   bool IsAliasOf( const SortedArray<T,A>& x ) const
   {
      return array.IsAliasOf( x.array );
   }

   /*!
      Ensures that this array uniquely references its contained data.

      If necessary, this member function generates a duplicate of the array
      data, references it, and then decrements the reference counter of the
      original array data.
   */
   void SetUnique()
   {
      array.SetUnique();
   }

   /*!
      Returns the total number of bytes required to store the data contained in
      this array.
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
   const_iterator At( size_type i ) const
   {
      return array.At( i );
   }

   /*! #
   */
   iterator MutableAt( size_type i )
   {
      return array.At( i );
   }

   /*! #
   */
   iterator MutableIterator( const_iterator i )
   {
      return array.MutableIterator( i );
   }

   /*! #
   */
   const T& operator []( size_type i ) const
   {
      return array[i];
   }

   /*! #
   */
   const T& operator *() const
   {
      PCL_PRECONDITION( !IsEmpty() )
      return *Begin();
   }

   /*! #
   */
   const_iterator Begin() const
   {
      return array.Begin();
   }

   /*! #
   */
   iterator MutableBegin()
   {
      return array.Begin();
   }

   /*! #
   */
   const_iterator End() const
   {
      return array.End();
   }

   /*! #
   */
   iterator MutableEnd()
   {
      return array.End();
   }

   /*! #
   */
   const_reverse_iterator ReverseBegin() const
   {
      return array.ReverseBegin();
   }

   /*! #
   */
   reverse_iterator MutableReverseBegin()
   {
      return array.ReverseBegin();
   }

   /*! #
   */
   const_reverse_iterator ReverseEnd() const
   {
      return array.ReverseEnd();
   }

   /*! #
   */
   reverse_iterator MutableReverseEnd()
   {
      return array.ReverseEnd();
   }

   /*! #
   */
   SortedArray<T,A>& operator =( const SortedArray<T,A>& x )
   {
      array = x.array;
      return *this;
   }

   /*! #
   */
   SortedArray<T,A>& operator =( const array_implementation& x )
   {
      array = x;
      Sort();
      return *this;
   }

   /*! #
   */
   void Assign( const array_implementation& x )
   {
      array.Assign( x );
      Sort();
   }

   /*! #
   */
   void Assign( const SortedArray<T,A>& x )
   {
      array.Assign( x.array );
   }

   /*! #
   */
   void Assign( const T& v, size_type n = 1 )
   {
      array.Assign( v, n );
   }

   /*! #
   */
   template <class FI>
   void Assign( FI i, FI j )
   {
      array.Assign( i, j );
      Sort();
   }

   /*! #
   */
   void Import( iterator i, iterator j )
   {
      array.Import( i, j );
      Sort();
   }

   /*! #
   */
   iterator Release()
   {
      return array.Release();
   }

   /*! #
   */
   void Add( const array_implementation& x )
   {
      Add( x.Begin(), x.End() );
   }

   /*! #
   */
   void Add( const SortedArray<T,A>& x )
   {
      Add( x.Begin(), x.End() );
   }

   /*! #
   */
   const_iterator Add( const T& v, size_type n = 1 )
   {
      return array.Insert( pcl::InsertionPoint( array.Begin(), array.End(), v ), v, n );
   }

   /*! #
   */
   template <class FI>
   void Add( FI i, FI j )
   {
      if ( i != j )
      {
         array.SetUnique();
         for ( iterator l = array.Begin(), r = array.End(); ; )
         {
            FI i0 = i;
            iterator m = array.Insert( pcl::InsertionPoint( l, r, *i ), *i );

            if ( ++i == j )
               break;

            if ( *i < *i0 )
            {
               l = array.Begin();
               r = m;
            }
            else
            {
               l = m + 1;
               r = array.End();
            }
         }
      }
   }

   /*! #
   */
   void Remove( const_iterator i, size_type n = 1 )
   {
      array.Remove( const_cast<iterator>( i ), n );
   }

   /*! #
   */
   void Remove( const_iterator i, const_iterator j )
   {
      array.Remove( const_cast<iterator>( i ), const_cast<iterator>( j ) );
   }

   /*! #
   */
   void Remove( const T& v )
   {
      const_iterator i = pcl::BinarySearch( Begin(), End(), v );
      if ( i != End() )
         Remove( const_cast<iterator>( i ), pcl::InsertionPoint( const_cast<iterator>( i )+1, array.End(), v ) );
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
      return pcl::BinarySearch( Begin(), End(), v, p );
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
      return pcl::BinarySearchLast( Begin(), End(), v, p );
   }

   /*! #
   */
   bool Has( const T& v ) const
   {
      return Search( v ) != End();
   }

   /*! #
   */
   template <class BP>
   bool Has( const T& v, BP p ) const
   {
      return Search( v, p ) != End();
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
   friend
   void Swap( SortedArray<T,A>& x1, SortedArray<T,A>& x2 )
   {
      pcl::Swap( x1.array, x2.array );
   }

   /*! #
   */
   friend
   bool operator ==( const SortedArray<T,A>& x1, const SortedArray<T,A>& x2 )
   {
      return x1.array == x2.array;
   }

   /*! #
   */
   friend
   bool operator ==( const SortedArray<T,A>& x1, const array_implementation& x2 )
   {
      return x1.array == x2;
   }

   /*! #
   */
   friend
   bool operator ==( const array_implementation& x1, const SortedArray<T,A>& x2 )
   {
      return x1 == x2.array;
   }

   /*! #
   */
   friend
   bool operator <( const SortedArray<T,A>& x1, const SortedArray<T,A>& x2 )
   {
      return x1.array < x2.array;
   }

   /*! #
   */
   friend
   bool operator <( const SortedArray<T,A>& x1, const array_implementation& x2 )
   {
      return x1.array < x2;
   }

   /*! #
   */
   friend
   bool operator <( const array_implementation& x1, const SortedArray<T,A>& x2 )
   {
      return x1 < x2.array;
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

   // -------------------------------------------------------------------------

private:

   array_implementation array;
};

// ----------------------------------------------------------------------------

/*!
 * Returns true if a dynamic sorted array \a a1 is equal to another sorted
 * dynamic array \a a2.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
bool operator ==( const SortedArray<T,A>& a1, const SortedArray<T,A>& a2 )
{
   return a1.Length() == a2.Length() && Equal( a1.Begin(), a2.Begin(), a2.End() );
}

/*!
 * Returns true if a dynamic sorted array \a a1 is equal to a dynamic array
 * \a a2.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
bool operator ==( const SortedArray<T,A>& a1, const Array<T,A>& a2 )
{
   return a1.Length() == a2.Length() && Equal( a1.Begin(), a2.Begin(), a2.End() );
}

/*!
 * Returns true if a dynamic array \a a1 is equal to a dynamic sorted array
 * \a a2.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
bool operator ==( const Array<T,A>& a1, const SortedArray<T,A>& a2 )
{
   return a1.Length() == a2.Length() && Equal( a1.Begin(), a2.Begin(), a2.End() );
}

/*!
 * Returns true if a dynamic sorted array \a a1 is less than another dynamic
 * sorted array \a a2.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
bool operator <( const SortedArray<T,A>& a1, const SortedArray<T,A>& a2 )
{
   return Compare( a1.Begin(), a1.End(), a2.Begin(), a2.End() ) < 0;
}

/*!
 * Returns true if a dynamic sorted array \a a1 is less than a dynamic array
 * \a a2.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
bool operator <( const SortedArray<T,A>& a1, const Array<T,A>& a2 )
{
   return Compare( a1.Begin(), a1.End(), a2.Begin(), a2.End() ) < 0;
}

/*!
 * Returns true if a dynamic array \a a1 is less than a dynamic sorted array
 * \a a2.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
bool operator <( const Array<T,A>& a1, const SortedArray<T,A>& a2 )
{
   return Compare( a1.Begin(), a1.End(), a2.Begin(), a2.End() ) < 0;
}

/*!
 * Adds an object \a x to a dynamic sorted array \a a. Returns a reference to
 * the dynamic sorted array.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
SortedArray<T,A>& operator <<( SortedArray<T,A>& a, const T& x )
{
   a.Add( x ); return a;
}

/*!
 * Adds a dynamic array \a a2 to a dynamic sorted array \a a1. Returns a
 * reference to the left-hand dynamic sorted array \a a1.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
SortedArray<T,A>& operator <<( SortedArray<T,A>& a1, const Array<T,A>& a2 )
{
   a1.Add( a2 ); return a1;
}

/*!
 * Adds a dynamic sorted array \a a2 to a dynamic sorted array \a a1. Returns a
 * reference to the left-hand dynamic sorted array \a a1.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
SortedArray<T,A>& operator <<( SortedArray<T,A>& a1, const SortedArray<T,A>& a2 )
{
   a1.Add( a2 ); return a1;
}

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
class PCL_CLASS IndirectSortedArray : public IndirectContainer<T>
{
public:

   /*! #
   */
   typedef IndirectArray<T,A>
      array_implementation;

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

   /*! #
   */
   explicit
   IndirectSortedArray( const A& a = A() ) : array( a )
   {
   }

   /*! #
   */
   explicit
   IndirectSortedArray( size_type n, const A& a = A() ) : array( n, a )
   {
   }

   /*! #
   */
   IndirectSortedArray( size_type n, const T* p, const A& a = A() ) : array( n, p, a )
   {
   }

   /*! #
   */
   template <class FI>
   IndirectSortedArray( FI i, FI j, const A& a = A() ) : array( i, j, a )
   {
      Sort();
   }

   /*! #
   */
   IndirectSortedArray( const IndirectSortedArray<T,A>& x ) : array( x.array )
   {
   }

   /*! #
   */
   IndirectSortedArray( const array_implementation& x ) : array( x )
   {
      Sort();
   }

   /*!
    * Destroys an %IndirectSortedArray object.
    *
    * Deallocates the internal list of pointers to objects, but does not
    * destroy the contained objects. To destroy them, you have to call
    * Destroy() explicitly.
    */
   ~IndirectSortedArray()
   {
   }

   /*!
      Returns true if this indirect array uniquely references its contained
      data pointers.
   */
   bool IsUnique() const
   {
      return array.IsUnique();
   }

   /*!
      Returns true if this indirect sorted array is an alias of a specified
      indirect sorted array \a x.

      Two objects are aliases if both of them share the same data. Two indirect
      containers are aliases if they share a unique set of data pointers.
   */
   bool IsAliasOf( const IndirectSortedArray<T,A>& x ) const
   {
      return array.IsAliasOf( x.array );
   }

   /*!
      Ensures that this indirect array uniquely references its contained data
      pointers.

      If necessary, this member function generates a duplicate of the array
      of pointers, references it, and then decrements the reference counter of
      the original pointers array.
   */
   void SetUnique()
   {
      array.SetUnique();
   }

   /*!
      Returns the total number of bytes required to store the array of data
      pointers in this indirect array.
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
   const_iterator At( size_type i ) const
   {
      return array.At( i );
   }

   /*! #
   */
   iterator MutableAt( size_type i )
   {
      return array.At( i );
   }

   /*! #
   */
   iterator MutableIterator( const_iterator i )
   {
      return array.MutableIterator( i );
   }

   /*! #
   */
   const T* operator []( size_type i ) const
   {
      return array[i];
   }

   /*! #
   */
   const T* operator *() const
   {
      PCL_PRECONDITION( !IsEmpty() )
      return *Begin();
   }

   /*! #
   */
   const_iterator Begin() const
   {
      return array.Begin();
   }

   /*! #
   */
   iterator MutableBegin()
   {
      return array.Begin();
   }

   /*! #
   */
   const_iterator End() const
   {
      return array.End();
   }

   /*! #
   */
   iterator MutableEnd()
   {
      return array.End();
   }

   /*! #
   */
   const_reverse_iterator ReverseBegin() const
   {
      return array.ReverseBegin();
   }

   /*! #
   */
   reverse_iterator MutableReverseBegin()
   {
      return array.ReverseBegin();
   }

   /*! #
   */
   const_reverse_iterator ReverseEnd() const
   {
      return array.ReverseEnd();
   }

   /*! #
   */
   reverse_iterator MutableReverseEnd()
   {
      return array.ReverseEnd();
   }

   /*! #
   */
   const T* First() const
   {
      return IsEmpty() ? 0 : *Begin();
   }

   /*! #
   */
   T* MutableFirst()
   {
      return IsEmpty() ? 0 : *MutableBegin();
   }

   /*! #
   */
   const T* Last() const
   {
      return IsEmpty() ? 0 : *ReverseBegin();
   }

   /*! #
   */
   T* MutableLast()
   {
      return IsEmpty() ? 0 : *MutableReverseBegin();
   }

   /*! #
   */
   IndirectSortedArray<T,A>& operator =( const IndirectSortedArray<T,A>& x )
   {
      array = x.array;
      return *this;
   }

   /*! #
   */
   IndirectSortedArray<T,A>& operator =( const array_implementation& x )
   {
      array = x;
      Sort();
      return *this;
   }

   /*! #
   */
   void Assign( const IndirectSortedArray<T,A>& x )
   {
      array.Assign( x.array );
   }

   /*! #
   */
   void Assign( const array_implementation& x )
   {
      array.Assign( x );
      Sort();
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
      Sort();
   }

   /*! #
   */
   template <class C>
   void CloneAssign( const C& x )
   {
      array.CloneAssign( x );
      Sort();
   }

   /*! #
   */
   template <class T1>
   void CloneAssign( SortedArray<T1,A>& x )
   {
      array.CloneAssign( x );
   }

   /*! #
   */
   template <class T1>
   void CloneAssign( IndirectSortedArray<T1,A>& x )
   {
      array.CloneAssign( x );
   }

   /*! #
   */
   void Import( iterator i, iterator j )
   {
      array.Import( i, j );
      Sort();
   }

   /*! #
   */
   iterator Release()
   {
      return array.Release();
   }

   /*! #
   */
   void Add( const IndirectSortedArray<T,A>& x )
   {
      Add( x.Begin(), x.End() );
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
      return array.Insert( pcl::InsertionPoint( array.Begin(), array.End(), p, less() ), p, n );
   }

   /*! #
   */
   template <class FI>
   void Add( FI i, FI j )
   {
      if ( i != j )
      {
         array.SetUnique();
         for ( iterator l = array.Begin(), r = array.End(); ; )
         {
            FI i0 = i;
            iterator m = array.Insert( pcl::InsertionPoint( l, r, *i, less() ), *i );

            if ( ++i == j )
               break;

            if ( less()( *i, *i0 ) )
            {
               l = array.Begin();
               r = m;
            }
            else
            {
               l = m+1;
               r = array.End();
            }
         }
      }
   }

   /*! #
   */
   void Remove( const_iterator i, size_type n = 1 )
   {
      array.Remove( const_cast<iterator>( i ), n );
   }

   /*! #
   */
   void Remove( const_iterator i, const_iterator j )
   {
      array.Remove( const_cast<iterator>( i ), const_cast<iterator>( j ) );
   }

   /*! #
   */
   void Remove( const T& v )
   {
      const_iterator i = pcl::BinarySearch( Begin(), End(), &v, less() );
      if ( i != End() )
         Remove( const_cast<iterator>( i ),
                 pcl::InsertionPoint( const_cast<iterator>( i )+1, array.End(), &v, less() ) );
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
   void Delete( const_iterator i, size_type n = 1 )
   {
      Delete( i, pcl::Min( i+n, End() ) );
   }

   /*! #
   */
   void Delete( const_iterator i, const_iterator j )
   {
      array.Delete( const_cast<iterator>( i ), const_cast<iterator>( j ) );
      if ( j != End() )
         pcl::Fill( pcl::Copy( const_cast<iterator>( i ),
                               const_cast<iterator>( j ), array.End() ), array.End(), (T*)0 );
   }

   /*! #
   */
   void Delete( const T& v )
   {
      const_iterator i = pcl::BinarySearch( Begin(), End(), &v, less() );
      if ( i != End() )
         Delete( const_cast<iterator>( i ),
                 pcl::InsertionPoint( const_cast<iterator>( i )+1, array.End(), &v, less() ) );
   }

   /*! #
   */
   void Delete()
   {
      array.Delete();
   }

   /*! #
   */
   void Destroy( const_iterator i, size_type n = 1 )
   {
      array.Destroy( const_cast<iterator>( i ), n );
   }

   /*! #
   */
   void Destroy( const_iterator i, const_iterator j )
   {
      array.Destroy( const_cast<iterator>( i ), const_cast<iterator>( j ) );
   }

   /*! #
   */
   void Destroy( const T& v )
   {
      const_iterator i = pcl::BinarySearch( Begin(), End(), &v, less() );
      if ( i != End() )
         Destroy( const_cast<iterator>( i ),
                  pcl::InsertionPoint( const_cast<iterator>( i )+1, array.End(), &v, less() ) );
   }

   /*! #
   */
   void Destroy()
   {
      array.Destroy();
   }

   /*! #
   */
   void Pack()
   {
      array.Pack();
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
   void Apply( F f ) const
   {
      array.Apply( f );
   }

   /*! #
   */
   template <class F>
   const_iterator FirstThat( F f ) const
   {
      return array.FirstThat( f );
   }

   /*! #
   */
   template <class F>
   const_iterator LastThat( F f ) const
   {
      return array.LastThat( f );
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
      return array.Count( (T* const&)p );
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
   const_iterator MinItem() const
   {
      return Begin();
   }

   /*! #
   */
   template <class BP>
   const_iterator MinItem( BP p ) const
   {
      return array.MinItem( p );
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
      return array.MaxItem( p );
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
      return array.Search( p );
   }

   /*! #
   */
   template <class BP>
   const_iterator Search( const T& v, BP p ) const
   {
      return pcl::BinarySearch( Begin(), End(), &v, IndirectBinaryPredicate<const T*, const T*, BP>( p ) );
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
      return array.SearchLast( p );
   }

   /*! #
   */
   template <class BP>
   const_iterator SearchLast( const T& v, BP p ) const
   {
      return pcl::BinarySearchLast( Begin(), End(), &v, IndirectBinaryPredicate<const T*, const T*, BP>( p ) );
   }

   /*! #
   */
   bool Has( const T& v ) const
   {
      return Search( v ) != End();
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
      return Search( v, p ) != End();
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
   friend
   void Swap( IndirectSortedArray<T,A>& x1, IndirectSortedArray<T,A>& x2 )
   {
      pcl::Swap( x1.array, x2.array );
   }

   /*! #
   */
   friend
   bool operator ==( const IndirectSortedArray<T,A>& x1, const IndirectSortedArray<T,A>& x2 )
   {
      return x1.array == x2.array;
   }

   /*! #
   */
   friend
   bool operator ==( const IndirectSortedArray<T,A>& x1, const array_implementation& x2 )
   {
      return x1.array == x2;
   }

   /*! #
   */
   friend
   bool operator ==( const array_implementation& x1, const IndirectSortedArray<T,A>& x2 )
   {
      return x1 == x2.array;
   }

   /*! #
   */
   friend
   bool operator <( const IndirectSortedArray<T,A>& x1, const IndirectSortedArray<T,A>& x2 )
   {
      return x1.array < x2.array;
   }

   /*! #
   */
   friend
   bool operator <( const IndirectSortedArray<T,A>& x1, const array_implementation& x2 )
   {
      return x1.array < x2;
   }

   /*! #
   */
   friend
   bool operator <( const array_implementation& x1, const IndirectSortedArray<T,A>& x2 )
   {
      return x1 < x2.array;
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

   // -------------------------------------------------------------------------

private:

   array_implementation array;
};

// ----------------------------------------------------------------------------

/*!
 * Returns true if an indirect dynamic sorted array \a a1 is equal to another
 * indirect dynamic sorted array \a a2.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
bool operator ==( const IndirectSortedArray<T,A>& x1, const IndirectSortedArray<T,A>& x2 )
{
   return x1.Length() == x2.Length() && Equal( x1.Begin(), x2.Begin(), x2.End(), IndirectArray<T,A>::equal() );
}

/*!
 * Returns true if an indirect dynamic sorted array \a a1 is equal to an
 * indirect dynamic array \a a2.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
bool operator ==( const IndirectSortedArray<T,A>& x1, const IndirectArray<T,A>& x2 )
{
   return x1.Length() == x2.Length() && Equal( x1.Begin(), x2.Begin(), x2.End(), IndirectArray<T,A>::equal() );
}

/*!
 * Returns true if an indirect dynamic array \a a1 is equal to an indirect
 * dynamic sorted array \a a2.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
bool operator ==( const IndirectArray<T,A>& x1, const IndirectSortedArray<T,A>& x2 )
{
   return x1.Length() == x2.Length() && Equal( x1.Begin(), x2.Begin(), x2.End(), IndirectArray<T,A>::equal() );
}

/*!
 * Returns true if an indirect dynamic sorted array \a a1 is less than another
 * indirect dynamic sorted array \a a2.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
bool operator <( const IndirectSortedArray<T,A>& x1, const IndirectSortedArray<T,A>& x2 )
{
   return Compare( x1.Begin(), x1.End(), x2.Begin(), x2.End(), IndirectArray<T,A>::less() ) < 0;
}

/*!
 * Returns true if an indirect dynamic sorted array \a a1 is less than an
 * indirect dynamic array \a a2.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
bool operator <( const IndirectSortedArray<T,A>& x1, const IndirectArray<T,A>& x2 )
{
   return Compare( x1.Begin(), x1.End(), x2.Begin(), x2.End(), IndirectArray<T,A>::less() ) < 0;
}

/*!
 * Returns true if an indirect dynamic array \a a1 is less than an indirect
 * dynamic sorted array \a a2.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
bool operator <( const IndirectArray<T,A>& x1, const IndirectSortedArray<T,A>& x2 )
{
   return Compare( x1.Begin(), x1.End(), x2.Begin(), x2.End(), IndirectArray<T,A>::less() ) < 0;
}

/*!
 * Appends a pointer to an object \a p to an indirect dynamic sorted array
 * \a a. Returns a reference to the indirect dynamic sorted array.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
IndirectSortedArray<T,A>& operator <<( IndirectSortedArray<T,A>& a, const T* p )
{
   a.Append( p ); return a;
}

/*!
 * Appends an indirect dynamic sorted array \a a2 to another indirect dynamic
 * sorted array \a a1. Returns a reference to the left-hand indirect dynamic
 * sorted array \a a1.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
IndirectSortedArray<T,A>& operator <<( IndirectSortedArray<T,A>& a1, const IndirectSortedArray<T,A>& a2 )
{
   a1.Append( a2 ); return a1;
}

/*!
 * Appends an indirect dynamic array \a a2 to an indirect dynamic sorted array
 * \a a1. Returns a reference to the left-hand indirect dynamic sorted array
 * \a a1.
 * \ingroup miscellaneous_array_operators
 */
template <class T, class A> inline
IndirectSortedArray<T,A>& operator <<( IndirectSortedArray<T,A>& a1, const IndirectArray<T,A>& a2 )
{
   a1.Append( a2 ); return a1;
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Array_h

// ****************************************************************************
// EOF pcl/Array.h - Released 2014/10/29 07:34:11 UTC
