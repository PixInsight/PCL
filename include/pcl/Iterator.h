//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// pcl/Iterator.h - Released 2017-07-18T16:13:52Z
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

#ifndef __PCL_Iterator_h
#define __PCL_Iterator_h

/// \file pcl/Iterator.h

#include <pcl/Defs.h>

#include <pcl/Relational.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup iterator_classification Iterator Classification
 */

/*!
 * \class ForwardIterator
 * \brief Forward iterator class.
 * \ingroup iterator_classification
 */
class PCL_CLASS ForwardIterator
{
};

/*!
 * \class BidirectionalIterator
 * \brief Bidirectional iterator class.
 * \ingroup iterator_classification
 */
class PCL_CLASS BidirectionalIterator : public ForwardIterator
{
};

/*!
 * \class RandomAccessIterator
 * \brief Random access iterator class.
 * \ingroup iterator_classification
 */
class PCL_CLASS RandomAccessIterator : public BidirectionalIterator
{
};

// ----------------------------------------------------------------------------

/*!
 * \class Iterator
 * \brief Generic container iterator.
 */
template <class C, class T>
struct PCL_CLASS Iterator
{
   typedef C iterator_class;  //!< Represents the iterator class
   typedef T item_type;       //!< Represents the item type
};

// ----------------------------------------------------------------------------

/*!
 * Returns an instance of the iterator class corresponding to an instantiation
 * of the %Iterator class.
 */
template <class C, class T> inline
C IteratorClass( const Iterator<C,T>& )
{
   return C();
}

/*!
 * Returns an instance of the iterator class for a pointer iterator, which is
 * RandomAccessIterator.
 */
template <typename T> inline
RandomAccessIterator IteratorClass( const T* )
{
   return RandomAccessIterator();
}

// ----------------------------------------------------------------------------

/*!
 * Returns a pointer to the item type of an instantiation of the %Iterator
 * class. The returned pointer has always a \c nullptr value.
 */
template <class C, class T> inline
T* ItemType( const Iterator<C,T>& )
{
   return nullptr;
}

/*!
 * Returns a pointer to the item type of a pointer iterator, which is the same
 * pointer. The returned pointer has always a \c nullptr value.
 */
template <typename T> inline
T* ItemType( const T* )
{
   return nullptr;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the existing distance in items from an iterator \a i to another
 * iterator \a j.
 */
template <class FI> inline
distance_type Distance( FI i, FI j )
{
   return __pcl_distance__( i, j, IteratorClass( i ) );
}

template <class FI> inline
distance_type __pcl_distance__( FI i, FI j, ForwardIterator )
{
   distance_type d = 0;
   for ( ; i != j; ++i, ++d ) {}
   return d;
}

template <class RI> inline
distance_type __pcl_distance__( RI i, RI j, RandomAccessIterator )
{
   return j - i;
}

// ----------------------------------------------------------------------------

/*!
 * Advances an iterator \a i by the specified distance \a d.
 */
template <class FI> inline
void Advance( FI& i, distance_type d )
{
   __pcl_advance__( i, d, IteratorClass( i ) );
}

template <class FI> inline
void __pcl_advance__( FI& i, distance_type d, ForwardIterator )
{
   PCL_PRECONDITION( d >= 0 )
   for ( ; d > 0; --d, ++i ) {}
}

template <class BI> inline
void __pcl_advance__( BI& i, distance_type d, BidirectionalIterator )
{
   if ( d > 0 )
      for ( ; ++i, --d > 0; ) {}
   else
      for ( ; d < 0; ++d, --i ) {}
}

template <class RI> inline
void __pcl_advance__( RI& i, distance_type d, RandomAccessIterator )
{
   i += d;
}

// ----------------------------------------------------------------------------

/*!
 * Moves an iterator \a i forward by the specified number \a n of items.
 */
template <class FI> inline
void MoveForward( FI& i, size_type n )
{
   __pcl_move_forward__( i, n, IteratorClass( i ) );
}

template <class FI> inline
void __pcl_move_forward__( FI& i, size_type n, ForwardIterator )
{
   for ( ; n > 0; --n, ++i ) {}
}

template <class RI> inline
void __pcl_move_forward__( RI& i, size_type n, RandomAccessIterator )
{
   i += n;
}

// ----------------------------------------------------------------------------

/*!
 * Moves an iterator \a i backward by the specified number \a n of items.
 */
template <class BI> inline
void MoveBackward( BI& i, size_type n )
{
   __pcl_move_backward__( i, n, IteratorClass( i ) );
}

template <class BI> inline
void __pcl_move_backward__( BI& i, size_type n, BidirectionalIterator )
{
   for ( ; n > 0; --n, --i ) {}
}

template <class RI> inline
void __pcl_move_backward__( RI& i, size_type n, RandomAccessIterator )
{
   i -= n;
}

// ----------------------------------------------------------------------------

/*!
 * \class ReverseIteratorBase
 * \brief Base class of reverse iterators.
 */
template <class BI, class C, class T>
class PCL_CLASS ReverseIteratorBase : public pcl::Iterator<C,T>
{
public:

   /*!
    * Constructs a default %ReverseIteratorBase object.
    */
   ReverseIteratorBase() = default;

   /*!
    * Copy constructor.
    */
   ReverseIteratorBase( const ReverseIteratorBase& i ) : pcl::Iterator<C,T>( i ), iterator( i.iterator )
   {
   }

   /*!
    * Constructs a %ReverseIteratorBase object as a duplicate of the specified
    * bidirectional iterator \a i.
    */
   ReverseIteratorBase( const BI& i ) : pcl::Iterator<C,T>(), iterator( i )
   {
   }

   /*!
    * Indirection operator. Returns a reference to the object pointed to by
    * this reverse iterator.
    */
   T& operator *() const
   {
      return (T&)*iterator;
   }

   /*!
    * Structure selection operator. Returns a copy of the bidirectional
    * iterator contained by this reverse iterator.
    */
   BI operator ->() const
   {
      return this->Iterator();
   }

   /*!
    * Returns a bidirectional iterator that points to the object pointed by
    * this %ReverseIteratorBase object.
    */
   operator BI() const
   {
      return this->Iterator();
   }

   /*!
    * A synonym for operator BI() const.
    */
   BI Iterator() const
   {
      return iterator;
   }

protected:

   BI iterator;

   void PreIncrement()
   {
      --iterator;
   }

   BI PostIncrement()
   {
      BI tmp = iterator;
      --iterator;
      return tmp;
   }

   void PreDecrement()
   {
      ++iterator;
   }

   BI PostDecrement()
   {
      BI tmp = iterator;
      ++iterator;
      return tmp;
   }
};

/*!
 * Returns true iff two reverse iterators \a i and \a j are equal. Two reverse
 * iterators are equal if they point to the same item.
 */
template <class BI, class C, class T> inline
bool operator ==( const ReverseIteratorBase<BI,C,T>& i, const ReverseIteratorBase<BI,C,T>& j )
{
   return i.Iterator() == j.Iterator();
}

// ----------------------------------------------------------------------------

#define IMPLEMENT_INCDEC_OPERATORS                                         \
   __I__& operator ++()      { __R__::PreIncrement(); return *this; }      \
   __I__  operator ++( int ) { return __I__( __R__::PostIncrement() ); }   \
   __I__& operator --()      { __R__::PreDecrement(); return *this; }      \
   __I__  operator --( int ) { return __I__( __R__::PostDecrement() ); }

/*!
 * \class ReverseBidirectionalIterator
 * \brief Reverse bidirectional iterator.
 */
template <class BI, class T>
class PCL_CLASS ReverseBidirectionalIterator : public ReverseIteratorBase<BI,BidirectionalIterator,T>
{
   typedef ReverseIteratorBase<BI,BidirectionalIterator,T>  __R__;
   typedef ReverseBidirectionalIterator                     __I__;

public:

   /*!
    * Constructs a %ReverseBidirectionalIterator object.
    */
   ReverseBidirectionalIterator() = default;

   /*!
    * Copy constructor.
    */
   ReverseBidirectionalIterator( const ReverseBidirectionalIterator& i ) : __R__( i )
   {
   }

   /*!
    * Constructs a %ReverseBidirectionalIterator object as a duplicate of the
    * specified bidirectional iterator \a i.
    */
   ReverseBidirectionalIterator( const BI& i ) : __R__( i )
   {
   }

   IMPLEMENT_INCDEC_OPERATORS
};

// ----------------------------------------------------------------------------

/*!
 * \class ReverseRandomAccessIterator
 * \brief Reverse random access iterator.
 */
template <class RI, class T>
class PCL_CLASS ReverseRandomAccessIterator : public ReverseIteratorBase<RI,RandomAccessIterator,T>
{
   typedef ReverseIteratorBase<RI,RandomAccessIterator,T>   __R__;
   typedef ReverseRandomAccessIterator                      __I__;

public:

   /*!
    * Constructs a %ReverseRandomAccessIterator object.
    */
   ReverseRandomAccessIterator() = default;

   /*!
    * Copy constructor.
    */
   ReverseRandomAccessIterator( const ReverseRandomAccessIterator& i ) : __R__( i )
   {
   }

   /*!
    * Constructs a %ReverseRandomAccessIterator object as a duplicate of the
    * specified random access iterator \a i.
    */
   ReverseRandomAccessIterator( const RI& i ) : __R__( i )
   {
   }

   /*!
    * Subscript operator. Returns a reference to the object at a distance \a d
    * from the current iterator position.
    */
   T& operator[]( size_type d ) const
   {
      return (T&)*(__R__::iterator - d);
   }

   /*!
    * Increments this interator by the specified distance \a d. Returns a
    * reference to this object.
    */
   ReverseRandomAccessIterator& operator +=( distance_type d )
   {
      __R__::iterator -= d;
      return *this;
   }

   /*!
    * Decrements this interator by the specified distance \a d. Returns a
    * reference to this object.
    */
   ReverseRandomAccessIterator& operator -=( distance_type d )
   {
      __R__::iterator += d;
      return *this;
   }

   IMPLEMENT_INCDEC_OPERATORS
};

#undef IMPLEMENT_INCDEC_OPERATORS

/*! #
 */
template <class RI, class T> inline
bool operator <( const ReverseRandomAccessIterator<RI,T>& i, const ReverseRandomAccessIterator<RI,T>& j )
{
   return j.Iterator() < i.Iterator();
}

/*! #
 */
template <class RI, class T> inline
ReverseRandomAccessIterator<RI,T> operator +( const ReverseRandomAccessIterator<RI,T>& i, distance_type d )
{
   RI r = i.Iterator();
   return r -= d;
}

/*! #
 */
template <class RI, class T> inline
ReverseRandomAccessIterator<RI,T> operator +( distance_type d, const ReverseRandomAccessIterator<RI,T>& i )
{
   return i + d;
}

/*! #
 */
template <class RI, class T> inline
ReverseRandomAccessIterator<RI,T> operator -( const ReverseRandomAccessIterator<RI,T>& i, distance_type d )
{
   RI r = i.Iterator();
   return r += d;
}

/*! #
 */
template <class RI, class T> inline
distance_type operator -( const ReverseRandomAccessIterator<RI,T>& i, const ReverseRandomAccessIterator<RI,T>& j )
{
   return j.Iterator() - i.Iterator();
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Iterator_h

// ----------------------------------------------------------------------------
// EOF pcl/Iterator.h - Released 2017-07-18T16:13:52Z
