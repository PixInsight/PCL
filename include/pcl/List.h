// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/List.h - Released 2014/11/14 17:16:34 UTC
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

#ifndef __PCL_List_h
#define __PCL_List_h

/// \file pcl/List.h

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

#define tail (head+1)

#define IsHead( x ) (x->prev == 0)
#define IsTail( x ) (x->next == 0)
#define IsNode( x ) !IsHead( x ) && !IsTail( x )

/*!
 * \class ListLink
 * \brief Double list node link.
 */
struct PCL_CLASS ListLink
{
   ListLink* next; //!< Next node
   ListLink* prev; //!< Previous node
};

/*!
 * \class List
 * \brief Generic doubly-linked list.
 *
 * ### TODO: ***ASAP*** PCL 2.x: This class requires a deep redesign. In
 *           particular, we have to implement implicit data sharing for
 *           doubly-linked lists.
 *
 * ### TODO: Write a detailed description for %List.
 */
template <class T, class A = StandardAllocator>
class PCL_CLASS List : public DirectContainer<T>
{
private:

   /*!
    * Represents a list link.
    */
   typedef ListLink link;

   /*!
    * \class pcl::List::node
    * \brief Double list node.
    */
   struct PCL_CLASS node : public link
   {
      T data; //!< Node data
   };

public:

   /*! #
   */
   typedef A
      block_allocator;

   /*! #
   */
   typedef Allocator<T, A>
      allocator;

   /*! #
   */
   typedef Allocator<link, A>
      node_allocator;

   /*! #
   */
   class iterator;
   friend class iterator;

   /*! #
   */
   class const_iterator;
   friend class const_iterator;

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::List::iterator
    * \brief Mutable double list iterator.
    */
   class PCL_CLASS iterator : public Iterator<BidirectionalIterator, T>
   {
   public:

      friend class List<T, A>;

      /*!
         Constructs an uninitialized list iterator.
      */
      iterator()
      {
      }

      /*!
         Copy constructor.
      */
      iterator( const iterator& x ) : current( x.current )
      {
      }

      /*!
         Returns a reference to the object stored in the current list node.
      */
      T& operator *() const
      {
         PCL_PRECONDITION( IsNode( current ) )
         return static_cast<node*>( current )->data;
      }

      /*!
         Pre-increment operator. Increments this list iterator to point to the
         next list node. Returns a reference to this iterator.
      */
      iterator& operator ++()
      {
         PCL_PRECONDITION( !IsTail( current ) )
         current = current->next;
         return *this;
      }

      /*!
         Post-increment operator. Increments this list iterator to point to the
         next list node. Returns a copy of this iterator \e before increment.
      */
      iterator operator ++( int )
      {
         PCL_PRECONDITION( !IsTail( current ) )
         iterator p = *this;
         current = current->next;
         return p;
      }

      /*!
         Pre-decrement operator. Decrements this list iterator to point to the
         previous list node. Returns a reference to this iterator.
      */
      iterator& operator --()
      {
         PCL_PRECONDITION( !IsHead( current ) )
         current = current->prev;
         return *this;
      }

      /*!
         Post-decrement operator. Decrements this list iterator to point to the
         previous list node. Returns a copy of this iterator \e before
         decrement.
      */
      iterator operator --( int )
      {
         PCL_PRECONDITION( !IsHead( current ) )
         iterator p = *this;
         current = current->prev;
         return p;
      }

      friend
      bool operator ==( const iterator& x1, const iterator& x2 )
      {
         return x1.current == x2.current;
      }

   protected:

      link* current;

      iterator( link* x ) : current( x )
      {
         PCL_CHECK( current != 0 /*&& IsNode( current )*/ )
      }

      operator link*() const
      {
         return current;
      }

      link* operator ->() const
      {
         return current;
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::List::const_iterator
    * \brief Immutable double list iterator.
    */
   class PCL_CLASS const_iterator : public iterator
   {
   public:

      friend class List<T, A>;

      /*!
         Constructs an uninitialized constant list iterator.
      */
      const_iterator() : iterator()
      {
      }

      /*!
         Copy constructor.
      */
      const_iterator( const iterator& x ) : iterator( x )
      {
      }

      /*!
         Returns a constant reference to the object stored in the current list
         node.
      */
      const T& operator *() const
      {
         PCL_PRECONDITION( IsNode( iterator::current ) )
         return static_cast<node*>( iterator::current )->data;
      }

      /*!
         Pre-increment operator. Increments this list iterator to point to the
         next list node. Returns a reference to this iterator.
      */
      const_iterator& operator ++()
      {
         PCL_PRECONDITION( !IsTail( iterator::current ) )
         iterator::current = iterator::current->next;
         return *this;
      }

      /*!
         Post-increment operator. Increments this list iterator to point to the
         next list node. Returns a copy of this iterator \e before increment.
      */
      const_iterator operator ++( int )
      {
         PCL_PRECONDITION( !IsTail( iterator::current ) )
         const_iterator p = *this;
         iterator::current = iterator::current->next;
         return p;
      }

      /*!
         Pre-decrement operator. Decrements this list iterator to point to the
         previous list node. Returns a reference to this iterator.
      */
      const_iterator& operator --()
      {
         PCL_PRECONDITION( !IsHead( iterator::current ) )
         iterator::current = iterator::current->prev;
         return *this;
      }

      /*!
         Post-decrement operator. Decrements this list iterator to point to the
         previous list node. Returns a copy of this iterator \e before
         decrement.
      */
      const_iterator operator --( int )
      {
         PCL_PRECONDITION( !IsHead( iterator::current ) )
         const_iterator p = *this;
         iterator::current = iterator::current->prev;
         return p;
      }

   private:

      const_iterator( link* x ) : iterator( x )
      {
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::List::reverse_iterator
    * \brief Mutable double list reverse iterator.
    */
   typedef ReverseBidirectionalIterator<iterator, T>
      reverse_iterator;

   /*!
    * \class pcl::List::const_reverse_iterator
    * \brief Immutable double list reverse iterator.
    */
   typedef ReverseBidirectionalIterator<const_iterator, const T>
      const_reverse_iterator;

   // -------------------------------------------------------------------------

   /*!
      Constructs an empty list.
   */
   explicit
   List( const A& a = A() ) :
   head( 0 ), length( 0 ), alloc( a )
   {
      __Initialize();
   }

   /*!
      Constructs a list of length \a n.
   */
   explicit
   List( size_type n, const A& a = A() ) :
   head( 0 ), length( 0 ), alloc( a )
   {
      __Initialize();
      Grow( End(), n );
   }

   /*!
      Constructs an array of \a n objects with constant value \a v.
   */
   List( size_type n, const T& v, const A& a = A() ) :
   head( 0 ), length( 0 ), alloc( a )
   {
      __Initialize();
      Add( v, n );
   }

   /*! #
   */
   template <class FI>
   List( FI i, FI j, const A& a = A() ) :
   head( 0 ), length( 0 ), alloc( a )
   {
      __Initialize();
      Add( i, j );
   }

   /*!
      Copy constructor.
   */
   List( const List<T, A>& x ) :
   head( 0 ), length( 0 ), alloc( x.alloc )
   {
      __Initialize();
      Add( x.Begin(), x.End() );
   }

   /*!
      Destroys a %List object.
      Destroys and deallocates all contained objects.
   */
   ~List()
   {
      if ( head != 0 )
      {
         Clear();
         alloc.Deallocate( head );
         head = 0;
      }
   }

   /*!
      Returns the length of this list.
   */
   size_type Length() const
   {
      return length;
   }

   /*!
      A synonim for Length(). A list stores no unused items, so its capacity is
      always equal to its length.
   */
   size_type Capacity() const
   {
      return length;
   }

   /*!
      Returns zero. Since a list cannot store unused items, there is no
      available space in a list.
   */
   size_type Available() const
   {
      return 0;
   }

   /*!
      Returns true if this list is empty.
   */
   bool IsEmpty() const
   {
      return length == 0;
   }

   /*!
      Returns the minimum legal index in this list (always zero).
   */
   size_type LowerBound() const
   {
      return 0;
   }

   /*!
      Returns the maximum legal index in this list. It is equal to Length()-1.
      For empty lists, UpperBound() returns a meaningless value.
   */
   size_type UpperBound() const
   {
      return Length()-1;
   }

   /*!
      Returns a reference to the allocator object used by this list.
   */
   const A& GetAllocator() const
   {
      return alloc;
   }

   /*!
      Sets a new allocator object for this list.
   */
   void SetAllocator( const A& a )
   {
      alloc = a;
   }

   /*!
      Returns an iterator located at the specified index \a i.
   */
   iterator At( size_type i )
   {
      PCL_PRECONDITION( i >= 0 && i < Length() )
      iterator n;
      if ( i <= (Length() >> 1) )
      {
         n = Begin();
         MoveForward( n, i );
      }
      else
      {
         n = End();
         MoveBackward( n, Length()-i );
      }
      return n;
   }

   /*!
      Returns a constant iterator located at the specified index \a i.
   */
   const_iterator At( size_type i ) const
   {
      PCL_PRECONDITION( i >= 0 && i < Length() )
      const_iterator n;
      if ( i <= (Length() >> 1) )
      {
         n = Begin();
         MoveForward( n, i );
      }
      else
      {
         n = End();
         MoveBackward( n, Length()-i );
      }
      return n;
   }

   /*!
      Returns an iterator located at the beginning of this list.
   */
   iterator Begin()
   {
      return head->next;
   }

   /*!
      Returns a constant iterator located at the beginning of this list.
   */
   const_iterator Begin() const
   {
      return head->next;
   }

   /*!
      Returns a constant iterator located at the beginning of this list.
   */
   const_iterator ConstBegin() const
   {
      return head->next;
   }

   /*!
      Returns an iterator located at the end of this list.
   */
   iterator End()
   {
      return tail;
   }

   /*!
      Returns a constant iterator located at the end of this list.
   */
   const_iterator End() const
   {
      return tail;
   }

   /*!
      Returns a constant iterator located at the end of this list.
   */
   const_iterator ConstEnd() const
   {
      return tail;
   }

   /*!
      Returns a reverse iterator located at the <em>reverse beginning</em> of
      this list.

      The reverse beginning corresponds to the last element in the list.
   */
   reverse_iterator ReverseBegin()
   {
      return (iterator)tail->prev;
   }

   /*!
      Returns a constant reverse iterator located at the <em>reverse
      beginning</em> of this list.

      The reverse beginning corresponds to the last element in the list.
   */
   const_reverse_iterator ReverseBegin() const
   {
      return (const_iterator)tail->prev;
   }

   /*!
      Returns a constant reverse iterator located at the <em>reverse
      beginning</em> of this list.

      The reverse beginning corresponds to the last element in the list.
   */
   const_reverse_iterator ConstReverseBegin() const
   {
      return (const_iterator)tail->prev;
   }

   /*!
      Returns a reverse iterator located at the <em>reverse end</em> of this
      list.

      The reverse end corresponds to an (nonexistent) element immediately
      before the first element in the list.
   */
   reverse_iterator ReverseEnd()
   {
      return (iterator)head;
   }

   /*!
      Returns a constant reverse iterator located at the <em>reverse end</em>
      of this list.

      The reverse end corresponds to an (nonexistent) element immediately
      before the first element in the list.
   */
   const_reverse_iterator ReverseEnd() const
   {
      return (const_iterator)head;
   }

   /*!
      Returns a constant reverse iterator located at the <em>reverse end</em>
      of this list.

      The reverse end corresponds to an (nonexistent) element immediately
      before the first element in the list.
   */
   const_reverse_iterator ConstReverseEnd() const
   {
      return (const_iterator)head;
   }

   /*!
      Assignment operator. Copies the specified list \a x to this. Returns a
      reference to this object.
   */
   List<T, A>& operator =( const List<T, A>& x )
   {
      Assign( x );
      return *this;
   }

   /*!
      Assignment. Copies the specified list \a x to this.
   */
   void Assign( const List<T, A>& x )
   {
      if ( &x != this )
         Assign( x.Begin(), x.End() );
   }

   /*! #
   */
   void Assign( const T& v, size_type n = 1 )
   {
      iterator i = Begin();
      for ( ; n > 0 && i != End(); --n, ++i )
         *i = v;
      if ( n > 0 )
         Insert( i, v, n );
      else if ( i != End() )
         Remove( i, End() );
   }

   /*! #
   */
   template <class FI>
   void Assign( FI i, FI j )
   {
      iterator t = Begin();
      for ( ; t != End() && i != j; ++t, ++i )
         *t = *i;
      if ( i != j )
         Insert( t, i, j );
      else if ( t != End() )
         Remove( t, End() );
   }

   /*! #
   */
   iterator Grow( iterator i, size_type n = 1 )
   {
      PCL_PRECONDITION( !IsHead( i.current ) )
      iterator t = i->prev;
      for ( ; n > 0; --n )
         __NewNode( i );
      return ++t;
   }

   /*! #
   */
   iterator Insert( iterator i, const List<T, A>& x )
   {
      return Insert( i, x.Begin(), x.End() );
   }

   /*! #
   */
   iterator Insert( iterator i, const T& v, size_type n = 1 )
   {
      PCL_PRECONDITION( !IsHead( i.current ) )
      iterator t = i->prev;
      for ( ; n > 0; --n )
         __NewNode( i, v );
      return ++t;
   }

   /*! #
   */
   template <class FI>
   iterator Insert( iterator i, FI i1, FI j1 )
   {
      PCL_PRECONDITION( !IsHead( i.current ) )
      iterator t = i->prev;
      for ( ; i1 != j1; ++i1 )
         __NewNode( i, *i1 );
      return ++t;
   }

   /*! #
   */
   void Add( const List<T, A>& x )
   {
      Insert( End(), x );
   }

   /*! #
   */
   void Add( const T& v, size_type n = 1 )
   {
      Insert( End(), v, n );
   }

   /*! #
   */
   template <class FI>
   void Add( FI i, FI j )
   {
      Insert( End(), i, j );
   }

   /*! #
   */
   void Remove( iterator i, size_type n = 1 )
   {
      iterator j;
      for ( j = i; n > 0 && j != End(); --n, ++j );
      Remove( i, j );
   }

   /*! #
   */
   void Remove( iterator i, iterator j )
   {
      PCL_PRECONDITION( IsNode( i.current ) )
      PCL_PRECONDITION( !IsHead( j.current ) )
      i->prev->next = j;
      j->prev = i->prev;
      while ( i != j )
      {
         iterator n = i->next;
         Destroy( &(static_cast<node*>( (link*)i )->data) );
         __DeallocateNode( i );
         --length;
         i = n;
      }
   }

   /*! #
   */
   void Remove( const T& v )
   {
      for ( iterator i = Begin(); i != End(); )
         if ( *i == v )
            __DeleteNode( i++ );
         else
            ++i;
   }

   /*! #
   */
   void Clear()
   {
      if ( !IsEmpty() )
         Remove( Begin(), End() );
   }

   /*!
      A synonym for Clear().
   */
   void Remove() { Clear(); }

   /*! #
   */
   iterator Replace( iterator i, iterator j, const T& v, size_type n = 1 )
   {
      PCL_PRECONDITION( IsNode( i.current ) )
      PCL_PRECONDITION( !IsHead( j.current ) )
      iterator t = i->prev;
      for ( ; n > 0 && i != j; --n, ++i )
         *i = v;
      if ( n > 0 )
         Insert( i, v, n );
      else if ( i != j )
         Remove( i, j );
      return ++t;
   }

   /*! #
   */
   template <class FI>
   iterator Replace( iterator i, iterator j, FI i1, FI j1 )
   {
      PCL_PRECONDITION( IsNode( i.current ) )
      PCL_PRECONDITION( !IsHead( j.current ) )
      iterator t = i->prev;
      for ( ; i1 != j1 && i != j; ++i1, ++i )
         *i = *i1;
      if ( i1 != j1 )
         Insert( i, i1, j1 );
      else if ( i != j )
         Remove( i, j );
      return ++t;
   }

   /*! #
   */
   iterator Splice( iterator i, List<T, A>& x )
   {
      PCL_PRECONDITION( !IsHead( i.current ) )
      iterator t = i->prev;
      if ( &x != this && !x.IsEmpty() )
      {
         if ( alloc == x.alloc )
         {
            iterator ix = x.Begin();
            iterator jx = x.End();
            ix->prev->next = jx;
            jx->prev->next = i;
            i->prev->next = ix;
            i->prev = jx->prev;
            jx->prev = ix->prev;
            ix->prev = t;
            length += x.length;
            x.length = 0;
         }
         else
         {
            Insert( i, x.Begin(), x.End() );
            x.Clear();
         }
      }
      return ++t;
   }

   /*! #
   */
   iterator Splice( iterator i, List<T, A>& x, iterator ix )
   {
      PCL_PRECONDITION( !IsHead( i.current ) )
      PCL_PRECONDITION( !IsHead( ix.current ) )
      iterator t = i->prev;
      if ( i != ix )
      {
         if ( alloc == x.alloc )
         {
            x.__Detach( ix );
            __Attach( i, ix );
         }
         else
         {
            Insert( i, *ix );
            x.Remove( ix );
         }
      }
      return ++t;
   }

   /*! #
   */
   iterator Splice( iterator i, List<T, A>& x, iterator ix, iterator jx )
   {
      PCL_PRECONDITION( !IsHead( i.current ) )
      PCL_PRECONDITION( !IsHead( ix.current ) )
      PCL_PRECONDITION( !IsHead( jx.current ) )
      iterator t = i->prev;
      if ( ix != jx )
      {
         if ( alloc == x.alloc )
         {
            ix->prev->next = jx;
            jx->prev->next = i;
            i->prev->next = ix;
            i->prev = jx->prev;
            jx->prev = ix->prev;
            ix->prev = t;

            if ( &x != this )
            {
               size_type n = size_type( pcl::Distance( t, i ) ) - 1;
               x.length -= n;
               length += n;
            }
         }
         else
         {
            Insert( i, ix, jx );
            x.Remove( ix, jx );
         }
      }
      return ++t;
   }

   /*! #
   */
   template <class F>
   void Apply( F f )
   {
      pcl::Apply( Begin(), End(), f );
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
   iterator FirstThat( F f ) const
   {
      return (link*)pcl::FirstThat( Begin(), End(), f );
   }

   /*! #
   */
   template <class F>
   iterator LastThat( F f ) const
   {
      return (link*)pcl::LastThat( Begin(), End(), f );
   }

   /*! #
   */
   size_type Count( const T& v ) const
   {
      return pcl::Count( Begin(), End(), v );
   }

   /*! #
   */
   template <class BP>
   size_type Count( const T& v, BP p ) const
   {
      return pcl::Count( Begin(), End(), v, p );
   }

   /*! #
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
      return (link*)pcl::MinItem( Begin(), End() );
   }

   /*! #
   */
   template <class BP>
   iterator MinItem( BP p ) const
   {
      return (link*)pcl::MinItem( Begin(), End(), p );
   }

   /*! #
   */
   iterator MaxItem() const
   {
      return (link*)pcl::MaxItem( Begin(), End() );
   }

   /*! #
   */
   template <class BP>
   iterator MaxItem( BP p ) const
   {
      return (link*)pcl::MaxItem( Begin(), End(), p );
   }

   /*! #
   */
   void Reverse()
   {
      __Reverse( Begin(), End() );
   }

   /*! #
   */
   void Rotate( distance_type n )
   {
      if ( Length() > 1 )
      {
         if ( (n %= Length()) < 0 )
            n += Length();
         iterator m = Begin();
         pcl::Advance( m, n );
         __Reverse( Begin(), m );
         __Reverse( m, End() );
         Reverse();
      }
   }

   /*! #
   */
   void ShiftLeft( const T& v, size_type n = 1 )
   {
      if ( !IsEmpty() && n > 0 )
      {
         if ( n >= Length() )
            pcl::Fill( Begin(), End(), v );
         else
         {
            iterator m = Begin();
            pcl::MoveForward( m, n );
            pcl::ShiftLeft( Begin(), m, End(), v );
         }
      }
   }

   /*! #
   */
   void ShiftRight( const T& v, size_type n = 1 )
   {
      if ( !IsEmpty() && n > 0 )
      {
         if ( n >= Length() )
            pcl::Fill( Begin(), End(), v );
         else
         {
            iterator m = End();
            pcl::MoveBackward( m, n );
            pcl::ShiftRight( Begin(), m, End(), v );
         }
      }
   }

   /*! #
   */
   iterator Search( const T& v ) const
   {
      return (link*)pcl::LinearSearch( Begin(), End(), v );
   }

   /*! #
   */
   template <class BP>
   iterator Search( const T& v, BP p ) const
   {
      return (link*)pcl::LinearSearch( Begin(), End(), v, p );
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
   void Merge( List<T, A>& x )
   {
      Merge( x, x.Begin(), x.End() );
   }

   // Defined later on this file
   void Merge( List<T, A>& x, iterator ix, iterator jx );

   /*! #
   */
   template <class BP>
   void Merge( List<T, A>& x, BP p )
   {
      Merge( x, x.Begin(), x.End(), p );
   }

   // Defined later on this file
   template <class BP>
   void Merge( List<T, A>& x, iterator ix, iterator jx, BP p );

   /*! #
   */
   void Sort()
   {
      pcl::Sort( *this );
   }

   /*! #
   */
   template <class BP>
   void Sort( BP p )
   {
      pcl::Sort( *this, p );
   }

   /*! #
   */
   friend
   void Swap( List<T, A>& a, List<T, A>& b )
   {
      if ( a.alloc == b.alloc )
      {
         Swap( a.head, b.head );
         Swap( a.length, b.length );
      }
      else
      {
         List<T, A> tmp = a;
         a = b;
         b = tmp;
      }
   }

   /*! #
   */
   static void DeleteFreeList()
   {
      while ( freeList != 0 )
      {
         link* next = freeList->next;
         node_allocator().Deallocate( static_cast<node*>( freeList ) );
         freeList = next;
      }
   }

private:

   // -------------------------------------------------------------------------

   link* head; // tail = head+1
   size_type length;
   node_allocator alloc;
   static link* freeList;

   // -------------------------------------------------------------------------

   void __Initialize()
   {
      head = alloc.Allocate( 2 );
      head->next = tail;
      head->prev = 0;
      tail->next = 0;
      tail->prev = head;
      length = 0;
   }

   void __NewNode( link* i )
   {
      node* n = __AllocateNode();
      Construct( &(n->data), alloc );
      __Attach( i, n );
   }

   void __NewNode( link* i, const T& v )
   {
      node* n = __AllocateNode();
      Construct( &(n->data), v, alloc );
      __Attach( i, n );
   }

   void __DeleteNode( link* n )
   {
      __Detach( n );
      Destroy( &(static_cast<node*>( n )->data) );
      __DeallocateNode( n );
   }

   void __DeleteSubList( link* n, link* j )
   {
      n->prev->next = j;
      j->prev = n->prev;
      size_type N = 0;
      for ( ; n != j; ++N )
      {
         link* next = n->next;
         Destroy( &(static_cast<node*>( n )->data) );
         __DeallocateNode( n );
         n = next;
      }
      length -= N;
   }

   void __Reverse( iterator i, iterator j )
   {
      if ( i != j )
      {
         iterator n = i++->prev;
         while ( i != j )
         {
            iterator k = i++;
            __Detach( k );
            __Attach( n->next, k );
         }
      }
   }

   void __Attach( link* i, link* n )
   {
      n->next = i;
      n->prev = i->prev;
      n->prev->next = i->prev = n;
      ++length;
   }

   void __Detach( link* n )
   {
      n->prev->next = n->next;
      n->next->prev = n->prev;
      --length;
   }

   node* __AllocateNode()
   {
      link* n;
      if ( freeList != 0 )
         freeList = (n = freeList)->next;
      else
         n = alloc.Allocate( 1, sizeof( node )-sizeof( link ) );
      return static_cast<node*>( n );
   }

   void __DeallocateNode( link* n )
   {
      n->next = freeList;
      freeList = n;
   }
};

// ----------------------------------------------------------------------------

/*!
   The free list for List<T, A>.
*/
template <class T, class A>
ListLink* List<T, A>::freeList = 0;

// ----------------------------------------------------------------------------

/*! #
*/
template <class L> inline
void Merge( L& x1, L& x2 )
{
   Merge( x1, x2, x2.Begin(), x2.End() );
}

/*! #
*/
template <class L, class LI> inline
void Merge( L& x1, L& x2, LI i2, LI j2 )
{
   PCL_PRECONDITION( &x1 != &x2 )
   LI i1 = x1.Begin();
   LI j1 = x1.End();
   while ( i1 != j1 && i2 != j2 )
   {
      if ( *i2 < *i1 )
         x1.Splice( i1, x2, i2++ );
      else
         ++i1;
   }
   if ( i2 != j2 )
      x1.Splice( j1, x2, i2, j2 );
}

/*! #
*/
template <class L, class BP> inline
void Merge( L& x1, L& x2, BP p )
{
   Merge( x1, x2, x2.Begin(), x2.End(), p );
}

/*! #
*/
template <class L, class LI, class BP> inline
void Merge( L& x1, L& x2, LI i2, LI j2, BP p )
{
   PCL_PRECONDITION( &x1 != &x2 )
   LI i1 = x1.Begin();
   LI j1 = x1.End();
   while ( i1 != j1 && i2 != j2 )
   {
      if ( p( *i2, *i1 ) )
         x1.Splice( i1, x2, i2++ );
      else
         ++i1;
   }
   if ( i2 != j2 )
      x1.Splice( j1, x2, i2, j2 );
}

/*! #
*/
template <class T, class A> inline
void List<T, A>::Merge( List<T, A>& x, iterator ix, iterator jx )
{
   pcl::Merge( *this, x, ix, jx );
}

/*! #
*/
template <class T, class A>
template <class BP> inline
void List<T, A>::Merge( List<T, A>& x, iterator ix, iterator jx, BP p )
{
   pcl::Merge( *this, x, ix, jx, p );
}

// ----------------------------------------------------------------------------

/*! #
*/
template <class L> inline
void Sort( L& x )
{
   const int N = 15;

   if ( x.Length() > 1 )
   {
      L r( x.GetAllocator() );
      L a[ N+1 ];
      int i, n = 0;

      for ( i = N; ; )
      {
         a[i].SetAllocator( x.GetAllocator() );

         if ( --i < 0 )
            break;
      }

      while ( !x.IsEmpty() )
      {
         r.Splice( r.Begin(), x, x.Begin() );

         for ( i = 0; i < n && !a[i].IsEmpty(); ++i )
         {
            Merge( a[i], r );
            Swap( a[i], r );
         }

         Swap( a[i], r );

         if ( i == n )
            ++n;
      }

      while ( n > 0 )
         Merge( x, a[--n] );
   }
}

/*! #
*/
template <class L, class BP> inline
void Sort( L& x, BP p )
{
   const int N = 15;

   if ( x.Length() > 1 )
   {
      L r( x.GetAllocator() );
      L a[ N+1 ];
      int i, n = 0;

      for ( i = N; ; )
      {
         a[i].SetAllocator( x.GetAllocator() );

         if ( --i < 0 )
            break;
      }

      while ( !x.IsEmpty() )
      {
         r.Splice( r.Begin(), x, x.Begin() );

         for ( i = 0; i < n && !a[i].IsEmpty(); ++i )
         {
            Merge( a[i], r, p );
            Swap( a[i], r );
         }

         Swap( a[i], r );

         if ( i == n )
            ++n;
      }

      while ( n > 0 )
         Merge( x, a[--n], p );
   }
}

// ----------------------------------------------------------------------------

/*! #
*/
template <class T, class A> inline
bool operator ==( const List<T, A>& x1, const List<T, A>& x2 )
{
   return x1.Length() == x2.Length() && Equal( x1.Begin(), x2.Begin(), x2.End() );
}

/*! #
*/
template <class T, class A> inline
bool operator <( const List<T, A>& x1, const List<T, A>& x2 )
{
   return Compare( x1.Begin(), x1.End(), x2.Begin(), x2.End() ) < 0;
}

#undef tail
#undef IsHead
#undef IsTail
#undef IsNode

// ----------------------------------------------------------------------------

/*!
 * \class IndirectList
 * \brief Generic doubly-linked list of pointers to objects.
 *
 * ### TODO: PCL 2.x: This class requires a deep redesign. In particular, we
 *           have to implement implicit data sharing for doubly-linked lists.
 *
 * ### TODO: Write a detailed description for %IndirectList.
 */
template <class T, class A = StandardAllocator>
class PCL_CLASS IndirectList : public IndirectContainer<T>
{
public:

   /*! #
   */
   typedef A
      block_allocator;

   /*! #
   */
   typedef Allocator<T, A>
      allocator;

   /*! #
   */
   typedef List<void*, A>
      list_implementation;

   /*! #
   */
   typedef typename list_implementation::node_allocator
      node_allocator;

   /*! #
   */
   typedef typename list_implementation::iterator
      list_iterator;

   /*! #
   */
   typedef typename list_implementation::const_iterator
      const_list_iterator;

   // -------------------------------------------------------------------------

   class iterator;
   friend class iterator;

   class const_iterator;
   friend class const_iterator;

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::IndirectList::iterator
    * \brief Mutable indirect double list iterator.
    */
   class PCL_CLASS iterator : public Iterator<BidirectionalIterator, T>
   {
   public:

      friend class IndirectList<T, A>;

      /*! #
      */
      iterator() : it()
      {
      }

      /*! #
      */
      iterator( const iterator& x ) : it( x.it )
      {
      }

      /*! #
      */
      T*& operator *() const
      {
         return (T*&)*it;
      }

      /*! #
      */
      iterator& operator ++()
      {
         ++it; return *this;
      }

      /*! #
      */
      iterator operator ++( int )
      {
         return it++;
      }

      /*! #
      */
      iterator& operator --()
      {
         --it; return *this;
      }

      /*! #
      */
      iterator operator --( int )
      {
         return it--;
      }

      friend
      bool operator ==( const iterator& x1, const iterator& x2 )
      {
         return x1.it == x2.it;
      }

   protected:

      list_iterator it;

      iterator( const list_iterator& x ) : it( x )
      {
      }

      operator list_iterator() const
      {
         return it;
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::IndirectList::const_iterator
    * \brief Immutable indirect double list iterator.
    */
   class PCL_CLASS const_iterator : public iterator
   {
   public:

      friend class IndirectList<T, A>;

      /*! #
      */
      const_iterator() : iterator()
      {
      }

      /*! #
      */
      const_iterator( const iterator& x ) : iterator( x )
      {
      }

      /*! #
      */
      const T*& operator *() const
      {
         return (const T*&)iterator::operator *();
      }

      /*! #
      */
      const_iterator& operator ++()
      {
         ++iterator::it; return *this;
      }

      /*! #
      */
      const_iterator operator ++( int )
      {
         return iterator::it++;
      }

      /*! #
      */
      const_iterator& operator --()
      {
         --iterator::it; return *this;
      }

      /*! #
      */
      const_iterator operator --( int )
      {
         return iterator::it--;
      }

   private:

      const_iterator( const list_iterator& x ) : iterator( x )
      {
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::IndirectList::reverse_iterator
    * \brief Mutable indirect double list reverse iterator.
    */
   typedef ReverseBidirectionalIterator<iterator, T*>
      reverse_iterator;

   /*!
    * \class pcl::IndirectList::const_reverse_iterator
    * \brief Immutable indirect double list reverse iterator.
    */
   typedef ReverseBidirectionalIterator<const_iterator, const T*>
      const_reverse_iterator;

   /*! #
   */
   typedef IndirectEqual<const T*>
      equal;

   /*! #
   */
   typedef IndirectLess<const T*>
      less;

   // -------------------------------------------------------------------------

   /*! #
   */
   explicit
   IndirectList( const A& a = A() ) : list( a )
   {
   }

   /*! #
   */
   explicit
   IndirectList( size_type n, const A& a = A() ) : list( n, (void*)0, a )
   {
   }

   /*! #
   */
   IndirectList( size_type n, const T* p, const A& a = A() ) :
   list( n, (void*)p, a )
   {
   }

   /*! #
   */
   template <class FI>
   IndirectList( FI i, FI j, const A& a = A() ) : list( i, j, a )
   {
   }

   /*! #
   */
   IndirectList( const IndirectList<T, A>& x ) : list( x.list )
   {
   }

   /*!
    * Destroys an %IndirectList object.
    *
    * Deallocates the internal list of pointers to objects, but does not
    * destroy the contained objects. To destroy them, you have to call
    * Destroy() explicitly.
    */
   ~IndirectList()
   {
   }

   /*! #
   */
   size_type Length() const
   {
      return list.Length();
   }

   /*! #
   */
   size_type Capacity() const
   {
      return list.Capacity();
   }

   /*! #
   */
   size_type Available() const
   {
      return list.Available();
   }

   /*! #
   */
   bool IsEmpty() const
   {
      return list.IsEmpty();
   }

   /*! #
   */
   size_type LowerBound() const
   {
      return list.LowerBound();
   }

   /*! #
   */
   size_type UpperBound() const
   {
      return list.UpperBound();
   }

   /*! #
   */
   const A& GetAllocator() const
   {
      return list.GetAllocator();
   }

   /*! #
   */
   void SetAllocator( const A& a )
   {
      list.SetAllocator( a );
   }

   /*! #
   */
   iterator At( size_type i )
   {
      return (iterator)list.At( i );
   }

   /*! #
   */
   const_iterator At( size_type i ) const
   {
      return (const_iterator)list.At( i );
   }

   /*! #
   */
   iterator Begin()
   {
      return (iterator)list.Begin();
   }

   /*! #
   */
   const_iterator Begin() const
   {
      return (const_iterator)list.Begin();
   }

   /*! #
   */
   const_iterator ConstBegin() const
   {
      return (const_iterator)list.Begin();
   }

   /*! #
   */
   iterator End()
   {
      return (iterator)list.End();
   }

   /*! #
   */
   const_iterator End() const
   {
      return (const_iterator)list.End();
   }

   /*! #
   */
   const_iterator ConstEnd() const
   {
      return (const_iterator)list.End();
   }

   /*! #
   */
   reverse_iterator ReverseBegin()
   {
      return (iterator)(list_iterator)list.ReverseBegin();
   }

   /*! #
   */
   const_reverse_iterator ReverseBegin() const
   {
      return (const_iterator)(const_list_iterator)list.ReverseBegin();
   }

   /*! #
   */
   const_reverse_iterator ConstReverseBegin() const
   {
      return (const_iterator)(const_list_iterator)list.ReverseBegin();
   }

   /*! #
   */
   reverse_iterator ReverseEnd()
   {
      return (iterator)(list_iterator)list.ReverseEnd();
   }

   /*! #
   */
   const_reverse_iterator ReverseEnd() const
   {
      return (const_iterator)(const_list_iterator)list.ReverseEnd();
   }

   /*! #
   */
   const_reverse_iterator ConstReverseEnd() const
   {
      return (const_iterator)(const_list_iterator)list.ReverseEnd();
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
   IndirectList<T, A>& operator =( const IndirectList<T, A>& x )
   {
      list = x.list; return *this;
   }

   /*! #
   */
   void Assign( const IndirectList<T, A>& x )
   {
      list.Assign( x.list );
   }

   /*! #
   */
   void Assign( const T* p, size_type n = 1 )
   {
      list.Assign( (void*)p, n );
   }

   /*! #
   */
   template <class FI>
   void Assign( FI i, FI j )
   {
      list.Assign( i, j );
   }

   /*! #
   */
   template <class C>
   void CloneAssign( const C& x )
   {
      if ( &x != this )
      {
         Destroy();
         __CloneAssign( x, (C*)0 );
      }
   }

   /*! #
   */
   iterator Grow( iterator i, size_type n = 1 )
   {
      return (iterator)list.Insert( (list_iterator)i, (void*)0, n );
   }

   /*! #
   */
   iterator Insert( iterator i, const IndirectList<T, A>& x )
   {
      return (iterator)list.Insert( (list_iterator)i, x.list );
   }

   /*! #
   */
   iterator Insert( iterator i, const T* p, size_type n = 1 )
   {
      return (iterator)list.Insert( (list_iterator)i, (void*)p, n );
   }

   /*! #
   */
   template <class FI>
   iterator Insert( iterator i, FI i1, FI j1 )
   {
      return (iterator)list.Insert( (list_iterator)i, i1, j1 );
   }

   /*! #
   */
   void Add( const IndirectList<T, A>& x )
   {
      list.Add( x.list );
   }

   /*! #
   */
   void Add( const T* p, size_type n = 1 )
   {
      list.Add( (void*)p, n );
   }

   /*! #
   */
   template <class FI>
   void Add( FI i, FI j )
   {
      list.Add( i, j );
   }

   /*! #
   */
   void Remove( iterator i, size_type n = 1 )
   {
      list.Remove( (list_iterator)i, n );
   }

   /*! #
   */
   void Remove( iterator i, iterator j )
   {
      list.Remove( (list_iterator)i, (list_iterator)j );
   }

   /*! #
   */
   void Remove( const T& v )
   {
      for ( iterator i = Begin(); i != End(); )
      {
         iterator t = i++;
         if ( *t != 0 && **t == v )
            Remove( t );
      }
   }

   /*! #
   */
   void Remove( const T* p )
   {
      list.Remove( (void*)p );
   }

   /*! #
   */
   void Clear()
   {
      list.Clear();
   }

   /*!
      A synonym for Clear().
   */
   void Remove() { Clear(); }

   /*! #
   */
   void Delete( iterator i, size_type n = 1 )
   {
      allocator a;
      for ( ; n > 0 && i != End(); --n, ++i )
         if ( *i != 0 )
            __DeleteItem( i, a );
   }

   /*! #
   */
   void Delete( iterator i, iterator j )
   {
      allocator a;
      for ( ; i != j; ++i )
         if ( *i != 0 )
            __DeleteItem( i, a );
   }

   /*! #
   */
   void Delete( const T& v )
   {
      allocator a;
      for ( iterator i = Begin(); i != End(); ++i )
         if ( *i != 0 && **i == v )
            __DeleteItem( i, a );
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
      allocator a;
      for ( iterator i = Begin(); i != End(); )
      {
         iterator t = i++;
         if ( *t != 0 && **t == v )
         {
            __DeleteItem( t, a );
            Remove( t );
         }
      }
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
      return (iterator)list.Replace( (list_iterator)i, (list_iterator)j, (void*)p, n );
   }

   /*! #
   */
   template <class FI>
   iterator Replace( iterator i, iterator j, FI i1, FI j1 )
   {
      return (iterator)list.Replace( (list_iterator)i, (list_iterator)j, i1, j1 );
   }

   /*! #
   */
   iterator Splice( iterator i, IndirectList<T, A>& x )
   {
      return (iterator)list.Splice( (list_iterator)i, x.list );
   }

   /*! #
   */
   iterator Splice( iterator i, IndirectList<T, A>& x, iterator ix )
   {
      return (iterator)list.Splice( (list_iterator)i, x.list, (list_iterator)ix );
   }

   /*! #
   */
   iterator Splice( iterator i, IndirectList<T, A>& x, iterator ix, iterator jx)
   {
      return (iterator)list.Splice( (list_iterator)i, x.list, (list_iterator)ix, (list_iterator)jx );
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
   template <class UP>
   iterator FirstThat( UP p ) const
   {
      return iterator( pcl::FirstThat( Begin(), End(),
                                       IndirectUnaryPredicate<const T*, UP>( p ) ) );
   }

   /*! #
   */
   template <class UP>
   iterator LastThat( UP p ) const
   {
      return iterator( pcl::LastThat( Begin(), End(),
                                      IndirectUnaryPredicate<const T*, UP>( p ) ) );
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
      return list.Count( (void*)p );
   }

   /*! #
   */
   template <class BP>
   size_type Count( const T& v, BP p ) const
   {
      return pcl::Count( Begin(), End(), &v,
                         IndirectBinaryPredicate<const T*, const T*, BP>( p ) );
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
      return iterator( pcl::MinItem( Begin(), End(), less() ) );
   }

   /*! #
   */
   template <class BP>
   iterator MinItem( BP p ) const
   {
      return iterator( pcl::MinItem( Begin(), End(),
                                     IndirectBinaryPredicate<const T*, const T*, BP>( p ) ) );
   }

   /*! #
   */
   iterator MaxItem() const
   {
      return iterator( pcl::MaxItem( Begin(), End(), less() ) );
   }

   /*! #
   */
   template <class BP>
   iterator MaxItem( BP p ) const
   {
      return iterator( pcl::MaxItem( Begin(), End(),
                                     IndirectBinaryPredicate<const T*, const T*, BP>( p ) ) );
   }

   /*! #
   */
   void Reverse()
   {
      list.Reverse();
   }

   /*! #
   */
   void Rotate( distance_type n )
   {
      if ( Length() > 1 )
      {
         if ( (n %= Length()) < 0 )
            n += Length();
         iterator m = Begin();
         pcl::Advance( m, n );
         pcl::Rotate( Begin(), m, End() );
      }
   }

   /*! #
   */
   void ShiftLeft( const T* p, size_type n = 1 )
   {
      list.ShiftLeft( (void*)p, n );
   }

   /*! #
   */
   void ShiftRight( const T* p, size_type n = 1 )
   {
      list.ShiftRight( (void*)p, n );
   }

   /*! #
   */
   iterator Search( const T& v ) const
   {
      return iterator( pcl::LinearSearch( Begin(), End(), &v, equal() ) );
   }

   /*! #
   */
   iterator Search( const T* p ) const
   {
      return (iterator)list.Search( (void*)p );
   }

   /*! #
   */
   template <class BP>
   iterator Search( const T& v, BP p ) const
   {
      return iterator( pcl::LinearSearch( Begin(), End(), &v,
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
      return list.Has( (void*)p );
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
   void Merge( IndirectList<T, A>& x )
   {
      Merge( x, x.Begin(), x.End() );
   }

   /*! #
   */
   void Merge( IndirectList<T, A>& x, iterator ix, iterator jx )
   {
      pcl::Merge( *this, x, ix, jx, less() );
   }

   /*! #
   */
   template <class BP>
   void Merge( IndirectList<T, A>& x, BP p )
   {
      Merge( x, x.Begin(), x.End(), p );
   }

   /*! #
   */
   template <class BP>
   void Merge( IndirectList<T, A>& x, iterator ix, iterator jx, BP p )
   {
      pcl::Merge( *this, x, ix, jx, IndirectBinaryPredicate<const T*, const T*, BP>( p ) );
   }

   /*! #
   */
   void Sort()
   {
      pcl::Sort( *this, less() );
   }

   /*! #
   */
   template <class BP>
   void Sort( BP p )
   {
      pcl::Sort( *this, IndirectBinaryPredicate<const T*, const T*, BP>( p ) );
   }

   /*! #
   */
   friend
   void Swap( IndirectList<T, A>& a, IndirectList<T, A>& b )
   {
      pcl::Swap( a.list, b.list );
   }

   /*! #
   */
   static void DeleteFreeList()
   {
      list_implementation::DeleteFreeList();
   }

private:

   list_implementation list;

   // -------------------------------------------------------------------------

   void __DeleteItem( iterator i, allocator& a )
   {
      pcl::Destroy( *i );
      a.Deallocate( *i );
      *i = 0;
   }

   template <class C>
   void __CloneAssign( const C& x, IndirectContainer<T>* )
   {
      allocator a;
      for ( typename C::const_iterator i = x.Begin(); i != x.End(); ++i )
      {
         T* t = a.Allocate( 1 );
         Construct( t, **i, a );
         Add( t );
      }
   }

   template <class C>
   void __CloneAssign( const C& x, DirectContainer<T>* )
   {
      allocator a;
      for ( typename C::const_iterator i = x.Begin(); i != x.End(); ++i )
      {
         T* t = a.Allocate( 1 );
         Construct( t, *i, a );
         Add( t );
      }
   }
};

/*! #
*/
template <class T, class A> inline
bool operator ==( const IndirectList<T, A>& x1, const IndirectList<T, A>& x2 )
{
   return x1.Length() == x2.Length() &&
      Equal( x1.Begin(), x2.Begin(), x2.End(), IndirectList<T, A>::equal() );
}

/*! #
*/
template <class T, class A> inline
bool operator <( const IndirectList<T, A>& x1, const IndirectList<T, A>& x2 )
{
   return Compare( x1.Begin(), x1.End(), x2.Begin(), x2.End(),
      IndirectList<T, A>::less() ) < 0;
}

// ----------------------------------------------------------------------------

/*!
 * \class SortedList
 * \brief Generic sorted doubly-linked list.
 *
 * ### TODO: PCL 2.x: This class requires a deep redesign. In particular, we
 *           have to implement implicit data sharing for doubly-linked lists.
 *
 * ### TODO: Write a detailed description for %SortedList.
 */
template <class T, class A = StandardAllocator>
class PCL_CLASS SortedList : public DirectContainer<T>
{
public:

   /*! #
   */
   typedef List<T, A>
      list_implementation;

   /*! #
   */
   typedef typename list_implementation::block_allocator
      block_allocator;

   /*! #
   */
   typedef typename list_implementation::allocator
      allocator;

   /*! #
   */
   typedef typename list_implementation::node_allocator
      node_allocator;

   /*! #
   */
   typedef typename list_implementation::iterator
      iterator;

   /*! #
   */
   typedef typename list_implementation::const_iterator
      const_iterator;

   /*! #
   */
   typedef typename list_implementation::reverse_iterator
      reverse_iterator;

   /*! #
   */
   typedef typename list_implementation::const_reverse_iterator
      const_reverse_iterator;

   // -------------------------------------------------------------------------

   /*! #
   */
   explicit
   SortedList( const A& a = A() ) : list( a )
   {
   }

   /*! #
   */
   SortedList( size_type n, const T& v, const A& a = A() ) : list( n, v, a )
   {
   }

   /*! #
   */
   template <class FI>
   SortedList( FI i, FI j, const A& a = A() ) : list( i, j, a )
   {
      Sort();
   }

   /*! #
   */
   SortedList( const SortedList<T, A>& x ) : list( x.list )
   {
   }

   /*! #
   */
   SortedList( const list_implementation& x ) : list( x )
   {
      Sort();
   }

   /*!
      Destroys a %SortedList object.
      Destroys and deallocates all contained objects.
   */
   ~SortedList()
   {
   }

   /*! #
   */
   size_type Length() const
   {
      return list.Length();
   }

   /*! #
   */
   size_type Capacity() const
   {
      return list.Capacity();
   }

   /*! #
   */
   size_type Available() const
   {
      return list.Available();
   }

   /*! #
   */
   bool IsEmpty() const
   {
      return list.IsEmpty();
   }

   /*! #
   */
   size_type LowerBound() const
   {
      return list.LowerBound();
   }

   /*! #
   */
   size_type UpperBound() const
   {
      return list.UpperBound();
   }

   /*! #
   */
   const A& GetAllocator() const
   {
      return list.GetAllocator();
   }

   /*! #
   */
   void SetAllocator( const A& a )
   {
      list.SetAllocator( a );
   }

   /*! #
   */
   const_iterator At( size_type i ) const
   {
      return list.At( i );
   }

   /*! #
   */
   const_iterator Begin() const
   {
      return list.Begin();
   }

   /*! #
   */
   const_iterator End() const
   {
      return list.End();
   }

   /*! #
   */
   const_reverse_iterator ReverseBegin() const
   {
      return list.ReverseBegin();
   }

   /*! #
   */
   const_reverse_iterator ReverseEnd() const
   {
      return list.ReverseEnd();
   }

   /*! #
   */
   SortedList<T, A>& operator =( const SortedList<T, A>& x )
   {
      list = x.list; return *this;
   }

   /*! #
   */
   SortedList<T, A>& operator =( const List<T, A>& x )
   {
      list = x; Sort(); return *this;
   }

   /*! #
   */
   void Assign( const SortedList<T, A>& x )
   {
      list.Assign( x.list );
   }

   /*! #
   */
   void Assign( const list_implementation& x )
   {
      list.Assign( x ); Sort();
   }

   /*! #
   */
   void Assign( const T& v, size_type n = 1 )
   {
      list.Assign( v, n );
   }

   /*! #
   */
   template <class FI>
   void Assign( FI i, FI j )
   {
      list.Assign( i, j ); Sort();
   }

   /*! #
   */
   void Add( const SortedList<T, A>& x )
   {
      SortedList<T, A> t( GetAllocator() ); Merge( t = x );
   }

   /*! #
   */
   void Add( const list_implementation& x )
   {
      SortedList<T, A> t( GetAllocator() ); Merge( t = x );
   }

   /*! #
   */
   void Add( const T& v, size_type n = 1 )
   {
      SortedList<T, A> t( n, v, GetAllocator() ); Merge( t );
   }

   /*! #
   */
   template <class FI>
   void Add( FI i, FI j )
   {
      SortedList<T, A> t( i, j, GetAllocator() ); Merge( t );
   }

   /*! #
   */
   void Remove( const_iterator i, size_type n = 1 )
   {
      list.Remove( iterator( i ), n );
   }

   /*! #
   */
   void Remove( const_iterator i, const_iterator j )
   {
      list.Remove( iterator( i ), iterator( j ) );
   }

   /*! #
   */
   void Remove( const T& v )
   {
      for ( iterator i = Begin(); i != End(); ++i )
         if ( *i == v )
         {
            iterator j;
            for ( j = i; ++j != End() && *j == v; ) {}
            Remove( i, j );
            break;
         }
   }

   /*! #
   */
   void Clear()
   {
      list.Clear();
   }

   /*!
      A synonym for Clear().
   */
   void Remove() { Clear(); }

   /*! #
   */
   template <class F>
   void Apply( F f ) const
   {
      list.Apply( f );
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
      return list.Count( v );
   }

   /*! #
   */
   template <class BP>
   size_type Count( const T& v, BP p ) const
   {
      return list.Count( v, p );
   }

   /*! #
   */
   template <class UP>
   size_type CountIf( UP p ) const
   {
      return list.CountIf( p );
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
      const_iterator j = End();
      if ( !IsEmpty() )
         --j;
      return j;
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
      size_type n = Length();

      if ( n != 0 )
      {
         const_iterator i = Begin();
         do
         {
            size_type h = n >> 1;
            const_iterator m = i;
            pcl::MoveForward( m, h );

            if ( *m < v )
            {
               i = ++m;
               n -= h+1;
            }
            else
               n = h;
         }
         while ( n != 0 );

         if ( i != End() && !(v < *i) )
            return i;
      }

      return End();
   }

   /*! #
   */
   template <class BP>
   const_iterator Search( const T& v, BP p ) const
   {
      return list.Search( v, p );
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
   void Merge( SortedList<T, A>& x )
   {
      list.Merge( x.list );
   }

   /*! #
   */
   void Merge( SortedList<T, A>& x, const_iterator ix, const_iterator jx )
   {
      list.Merge( x.list, iterator( ix ), iterator( jx ) );
   }

   /*! #
   */
   template <class BP>
   void Merge( SortedList<T, A>& x, BP p )
   {
      list.Merge( x.list, p );
   }

   /*! #
   */
   template <class BP>
   void Merge( SortedList<T, A>& x, const_iterator ix, const_iterator jx, BP p )
   {
      list.Merge( x.list, iterator( ix ), iterator( jx ), p );
   }

   /*! #
   */
   void Merge( list_implementation& x )
   {
      list.Merge( x );
   }

   /*! #
   */
   void Merge( list_implementation& x, iterator ix, iterator jx )
   {
      list.Merge( x, ix, jx );
   }

   /*! #
   */
   template <class BP>
   void Merge( list_implementation& x, BP p )
   {
      list.Merge( x, p );
   }

   /*! #
   */
   template <class BP>
   void Merge( list_implementation& x, iterator ix, iterator jx, BP p )
   {
      list.Merge( x, ix, jx, p );
   }

   /*! #
   */
   void Sort()
   {
      list.Sort();
   }

   /*! #
   */
   template <class BP>
   void Sort( BP p )
   {
      list.Sort( p );
   }

   /*! #
   */
   friend
   void Swap( SortedList<T, A>& a, SortedList<T, A>& b )
   {
      Swap( a.list, b.list );
   }

   /*! #
   */
   friend
   bool operator ==( const SortedList<T, A>& x1, const SortedList<T, A>& x2 )
   {
      return x1.list == x2.list;
   }

   /*! #
   */
   friend
   bool operator ==( const SortedList<T, A>& x1, const list_implementation& x2 )
   {
      return x1.list == x2;
   }

   /*! #
   */
   friend
   bool operator ==( const list_implementation& x1, const SortedList<T, A>& x2 )
   {
      return x1 == x2.list;
   }

   /*! #
   */
   friend
   bool operator <( const SortedList<T, A>& x1, const SortedList<T, A>& x2 )
   {
      return x1.list < x2.list;
   }

   /*! #
   */
   friend
   bool operator <( const SortedList<T, A>& x1, const list_implementation& x2 )
   {
      return x1.list < x2;
   }

   /*! #
   */
   friend
   bool operator <( const list_implementation& x1, const SortedList<T, A>& x2 )
   {
      return x1 < x2.list;
   }

   // -------------------------------------------------------------------------

   static void DeleteFreeList()
   {
      list_implementation::DeleteFreeList();
   }

private:

   // -------------------------------------------------------------------------

   list_implementation list;
};

// ----------------------------------------------------------------------------

/*!
 * \class IndirectSortedList
 * \brief Generic sorted doubly-linked list of pointers to objects.
 *
 * ### TODO: PCL 2.x: This class requires a deep redesign. In particular, we
 *           have to implement implicit data sharing for doubly-linked lists.
 *
 * ### TODO: Write a detailed description for %SortedList.
 */
template <class T, class A = StandardAllocator>
class PCL_CLASS IndirectSortedList : public IndirectContainer<T>
{
public:

   /*! #
   */
   typedef IndirectList<T, A>
      list_implementation;

   /*! #
   */
   typedef typename list_implementation::block_allocator
      block_allocator;

   /*! #
   */
   typedef typename list_implementation::allocator
      allocator;

   /*! #
   */
   typedef typename list_implementation::node_allocator
      node_allocator;

   /*! #
   */
   typedef typename list_implementation::iterator
      iterator;

   /*! #
   */
   typedef typename list_implementation::const_iterator
      const_iterator;

   /*! #
   */
   typedef typename list_implementation::reverse_iterator
      reverse_iterator;

   /*! #
   */
   typedef typename list_implementation::const_reverse_iterator
      const_reverse_iterator;

   /*! #
   */
   typedef typename list_implementation::equal
      equal;

   /*! #
   */
   typedef typename list_implementation::less
      less;

   /*! #
   */
   explicit
   IndirectSortedList( const A& a = A() ) : list( a )
   {
   }

   /*! #
   */
   explicit
   IndirectSortedList( size_type n, const A& a = A() ) : list( n, a )
   {
   }

   /*! #
   */
   IndirectSortedList( size_type n, const T* p, const A& a = A() ) :
   list( n, p, a )
   {
      Sort();
   }

   /*! #
   */
   template <class FI>
   IndirectSortedList( FI i, FI j, const A& a = A() ) : list( i, j, a )
   {
      Sort();
   }

   /*! #
   */
   IndirectSortedList( const IndirectSortedList<T, A>& x ) : list( x.list )
   {
   }

   /*! #
   */
   IndirectSortedList( const list_implementation& x ) : list( x )
   {
      Sort();
   }

   /*!
    * Destroys an %IndirectSortedList object.
    *
    * Deallocates the internal list of pointers to objects, but does not
    * destroy the contained objects. To destroy them, you have to call
    * Destroy() explicitly.
    */
   ~IndirectSortedList()
   {
   }

   /*! #
   */
   size_type Length() const
   {
      return list.Length();
   }

   /*! #
   */
   size_type Capacity() const
   {
      return list.Capacity();
   }

   /*! #
   */
   size_type Available() const
   {
      return list.Available();
   }

   /*! #
   */
   bool IsEmpty() const
   {
      return list.IsEmpty();
   }

   /*! #
   */
   size_type LowerBound() const
   {
      return list.LowerBound();
   }

   /*! #
   */
   size_type UpperBound() const
   {
      return list.UpperBound();
   }

   /*! #
   */
   const A& GetAllocator() const
   {
      return list.GetAllocator();
   }

   /*! #
   */
   void SetAllocator( const A& a )
   {
      list.SetAllocator( a );
   }

   /*! #
   */
   const_iterator At( size_type i ) const
   {
      return list.At( i );
   }

   /*! #
   */
   const_iterator Begin() const
   {
      return list.Begin();
   }

   /*! #
   */
   const_iterator End() const
   {
      return list.End();
   }

   /*! #
   */
   const_reverse_iterator ReverseBegin() const
   {
      return list.ReverseBegin();
   }

   /*! #
   */
   const_reverse_iterator ReverseEnd() const
   {
      return list.ReverseEnd();
   }

   /*! #
   */
   const T* First() const
   {
      return IsEmpty() ? 0 : *Begin();
   }

   /*! #
   */
   const T* Last() const
   {
      return IsEmpty() ? 0 : *ReverseBegin();
   }

   /*! #
   */
   IndirectSortedList<T, A>& operator =( const IndirectSortedList<T, A>& x )
   {
      list = x.list; return *this;
   }

   /*! #
   */
   IndirectSortedList<T, A>& operator =( const list_implementation& x )
   {
      list = x; Sort(); return *this;
   }

   /*! #
   */
   void Assign( const IndirectSortedList<T, A>& x )
   {
      list.Assign( x.list );
   }

   /*! #
   */
   void Assign( const list_implementation& x )
   {
      list.Assign( x ); Sort();
   }

   /*! #
   */
   void Assign( const T* p, size_type n = 1 )
   {
      list.Assign( p, n );
   }

   /*! #
   */
   template <class FI>
   void Assign( FI i, FI j )
   {
      list.Assign( i, j ); Sort();
   }

   /*! #
   */
   void Add( const IndirectSortedList<T, A>& x )
   {
      IndirectSortedList<T, A> t( GetAllocator() ); Merge( t = x );
   }

   /*! #
   */
   void Add( const list_implementation& x )
   {
      IndirectSortedList<T, A> t( GetAllocator() ); Merge( t = x );
   }

   /*! #
   */
   void Add( const T* p, size_type n = 1 )
   {
      IndirectSortedList<T, A> t( p, n, GetAllocator() ); Merge( t );
   }

   /*! #
   */
   template <class FI>
   void Add( FI i, FI j )
   {
      IndirectSortedList<T, A> t( i, j, GetAllocator() ); Merge( t );
   }

   /*! #
   */
   void Remove( const_iterator i, size_type n = 1 )
   {
      list.Remove( iterator( i ), n );
   }

   /*! #
   */
   void Remove( const_iterator i, const_iterator j )
   {
      list.Remove( iterator( i ), iterator( j ) );
   }

   /*! #
   */
   void Remove( const T& v )
   {
      for ( iterator i = Begin(); i != End() && *i != 0; ++i )
         if ( **i == v )
         {
            iterator j;
            for ( j = i; ++j != End() && *j != 0 && **j == v; ) {}
            list.Remove( i, j );
            break;
         }
   }

   /*! #
   */
   void Remove( const T* p )
   {
      list.Remove( p );
   }

   /*! #
   */
   void Clear()
   {
      list.Clear();
   }

   /*!
      A synonym for Clear().
   */
   void Remove() { Clear(); }

   /*! #
   */
   void Delete( const_iterator i, size_type n = 1 )
   {
      iterator j = i;
      pcl::MoveForward( j, n );
      Delete( iterator( i ), j );
   }

   /*! #
   */
   void Delete( const_iterator i, const_iterator j )
   {
      list.Delete( iterator( i ), iterator( j ) );
      if ( iterator( j ) != End() )
         list.Splice( list.End(), list, iterator( i ), iterator( j ) );
   }

   /*! #
   */
   void Delete( const T& v )
   {
      for ( iterator i = Begin(); i != End() && *i != 0; ++i )
         if ( **i == v )
         {
            iterator j;
            for ( j = i; ++j != End() && *j != 0 && **j == v; ) {}
            list.Delete( i, j );
            if ( j != End() )
               list.Splice( End(), list, i, j );
            break;
         }
   }

   void Delete()
   {
      list.Delete();
   }

   /*! #
   */
   void Destroy( iterator i, size_type n = 1 )
   {
      list.Destroy( i, n );
   }

   /*! #
   */
   void Destroy( iterator i, iterator j )
   {
      list.Destroy( i, j );
   }

   /*! #
   */
   void Destroy( const T& v )
   {
      for ( iterator i = Begin(); i != End() && *i != 0; ++i )
         if ( **i == v )
         {
            iterator j;
            for ( j = i; ++j != End() && *j != 0 && **j == v; ) {}
            list.Destroy( i, j );
            break;
         }
   }

   /*! #
   */
   void Destroy()
   {
      list.Destroy();
   }

   /*! #
   */
   void Pack()
   {
      list.Pack();
   }

   /*! #
   */
   template <class F>
   void Apply( F f ) const
   {
      list.Apply( f );
   }

   /*! #
   */
   template <class UP>
   const_iterator FirstThat( UP p ) const
   {
      return list.FirstThat( p );
   }

   /*! #
   */
   template <class UP>
   const_iterator LastThat( UP p ) const
   {
      return list.LastThat( p );
   }

   /*! #
   */
   size_type Count( const T& v ) const
   {
      return list.Count( v );
   }

   /*! #
   */
   size_type Count( const T* p ) const
   {
      return list.Count( p );
   }

   /*! #
   */
   template <class BP>
   size_type Count( const T& v, BP p ) const
   {
      return list.Count( v, p );
   }

   /*! #
   */
   template <class UP>
   size_type CountIf( UP p ) const
   {
      return list.CountIf( p );
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
      return list.MinItem( p );
   }

   /*! #
   */
   const_iterator MaxItem() const
   {
      const_iterator j = End();
      if ( !IsEmpty() )
         --j;
      return j;
   }

   /*! #
   */
   template <class BP>
   const_iterator MaxItem( BP p ) const
   {
      return list.MaxItem( p );
   }

   /*! #
   */
   const_iterator Search( const T& v ) const
   {
      size_type n = Length();

      if ( n != 0 )
      {
         const_iterator i = Begin();
         do
         {
            size_type h = n >> 1;
            const_iterator m = i;
            pcl::MoveForward( m, h );

            if ( *m != 0 && **m < v )
            {
               i = ++m;
               n -= h+1;
            }
            else
               n = h;
         }
         while ( n != 0 );

         if ( i != End() && *i != 0 && !(v < **i) )
            return i;
      }

      return End();
   }

   /*! #
   */
   const_iterator Search( const T* p ) const
   {
      return const_iterator( list.Search( p ) );
   }

   /*! #
   */
   template <class BP>
   const_iterator Search( const T& v, BP p ) const
   {
      size_type n = Length();

      if ( n != 0 )
      {
         const_iterator i = Begin();
         do
         {
            size_type h = n >> 1;
            const_iterator m = i;
            pcl::MoveForward( m, h );

            if ( *m != 0 && p( **m, v ) )
            {
               i = ++m;
               n -= h+1;
            }
            else
               n = h;
         }
         while ( n != 0 );

         if ( i != End() && *i != 0 && !p( v, **i ) )
            return i;
      }

      return End();
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
      return list.Has( p );
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
   void Merge( IndirectSortedList<T, A>& x )
   {
      list.Merge( x.list );
   }

   /*! #
   */
   void Merge( IndirectSortedList<T, A>& x, const_iterator ix, const_iterator jx )
   {
      list.Merge( x.list, iterator( ix ), iterator( jx ) );
   }

   /*! #
   */
   template <class BP>
   void Merge( IndirectSortedList<T, A>& x, BP p )
   {
      Merge( x, x.Begin(), x.End(), p );
   }

   /*! #
   */
   template <class BP>
   void Merge( IndirectSortedList<T, A>& x, const_iterator ix, const_iterator jx, BP p )
   {
      list.Merge( x.list, iterator( ix ), iterator( jx ), p );
   }

   /*! #
   */
   void Merge( list_implementation& x )
   {
      list.Merge( x );
   }

   /*! #
   */
   void Merge( list_implementation& x, iterator ix, iterator jx )
   {
      list.Merge( x, ix, jx );
   }

   /*! #
   */
   template <class BP>
   void Merge( list_implementation& x, BP p )
   {
      Merge( x, x.Begin(), x.End(), p );
   }

   /*! #
   */
   template <class BP>
   void Merge( list_implementation& x, iterator ix, iterator jx, BP p )
   {
      list.Merge( x, ix, jx, p );
   }

   /*! #
   */
   void Sort()
   {
      list.Sort();
   }

   /*! #
   */
   template <class BP>
   void Sort( BP p )
   {
      list.Sort( p );
   }

   /*! #
   */
   friend
   void Swap( IndirectSortedList<T, A>& a, IndirectSortedList<T, A>& b )
   {
      pcl::Swap( a.list, b.list );
   }

   /*! #
   */
   friend
   bool operator ==( const IndirectSortedList<T, A>& x1, const IndirectSortedList<T, A>& x2 )
   {
      return x1.list == x2.list;
   }

   /*! #
   */
   friend
   bool operator ==( const IndirectSortedList<T, A>& x1, const list_implementation& x2 )
   {
      return x1.list == x2;
   }

   /*! #
   */
   friend
   bool operator ==( const list_implementation& x1, const IndirectSortedList<T, A>& x2 )
   {
      return x1 == x2.list;
   }

   /*! #
   */
   friend
   bool operator <( const IndirectSortedList<T, A>& x1, const IndirectSortedList<T, A>& x2 )
   {
      return x1.list < x2.list;
   }

   /*! #
   */
   friend
   bool operator <( const IndirectSortedList<T, A>& x1, const list_implementation& x2 )
   {
      return x1.list < x2;
   }

   /*! #
   */
   friend
   bool operator <( const list_implementation& x1, const IndirectSortedList<T, A>& x2 )
   {
      return x1 < x2.list;
   }

   /*! #
   */
   static void DeleteFreeList()
   {
      list_implementation::DeleteFreeList();
   }

private:

   // -------------------------------------------------------------------------

   list_implementation list;
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_List_h

// ****************************************************************************
// EOF pcl/List.h - Released 2014/11/14 17:16:34 UTC
