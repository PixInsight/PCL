// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/Iterator.h - Released 2014/11/14 17:16:40 UTC
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

#ifndef __PCL_Iterator_h
#define __PCL_Iterator_h

/// \file pcl/Iterator.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Relational_h
#include <pcl/Relational.h>
#endif

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
C IteratorClass( const Iterator<C, T>& )
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
 * class. The returned pointer has always a zero value.
 */
template <class C, class T> inline
T* ItemType( const Iterator<C, T>& )
{
   return 0;
}

/*!
 * Returns a pointer to the item type of a pointer iterator, which is the same
 * pointer. The returned pointer has always a zero value.
 */
template <typename T> inline
T* ItemType( const T* )
{
   return 0;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the existing distance in items from an iterator \a i to another
 * iterator \a j.
 */
template <class FI> inline
distance_type Distance( FI i, FI j )
{
   return __distance__( i, j, IteratorClass( i ) );
}

template <class FI> inline
distance_type __distance__( FI i, FI j, ForwardIterator )
{
   distance_type d = 0;
   for ( ; i != j; ++i ) ++d;
   return d;
}

template <class RI> inline
distance_type __distance__( RI i, RI j, RandomAccessIterator )
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
   __advance__( i, d, IteratorClass( i ) );
}

template <class FI> inline
void __advance__( FI& i, distance_type d, ForwardIterator )
{
   PCL_PRECONDITION( d >= 0 )
   for ( ; d > 0; --d ) ++i;
}

template <class BI> inline
void __advance__( BI& i, distance_type d, BidirectionalIterator )
{
   if ( d > 0 )
      for ( ; ++i, --d != 0; );
   else
      for ( ; d != 0; ++d ) --i;
}

template <class RI> inline
void __advance__( RI& i, distance_type d, RandomAccessIterator )
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
   __move_forward__( i, n, IteratorClass( i ) );
}

template <class FI> inline
void __move_forward__( FI& i, size_type n, ForwardIterator )
{
   for ( ; n > 0; --n ) ++i;
}

template <class RI> inline
void __move_forward__( RI& i, size_type n, RandomAccessIterator )
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
   __move_backward__( i, n, IteratorClass( i ) );
}

template <class BI> inline
void __move_backward__( BI& i, size_type n, BidirectionalIterator )
{
   for ( ; n > 0; --n ) --i;
}

template <class RI> inline
void __move_backward__( RI& i, size_type n, RandomAccessIterator )
{
   i -= n;
}

// ----------------------------------------------------------------------------

/*!
 * \class ReverseIteratorBase
 * \brief Base class of reverse iterators.
 */
template <class BI, class C, class T>
class PCL_CLASS ReverseIteratorBase : public Iterator<C, T>
{
public:

   /*!
    * Constructs a default %ReverseIteratorBase object.
    */
   ReverseIteratorBase() : Iterator<C, T>(), iterator()
   {
   }

   /*!
    * Copy constructor.
    */
   ReverseIteratorBase( const ReverseIteratorBase<BI, C, T>& i ) : Iterator<C, T>( i ), iterator( i.iterator )
   {
   }

   /*!
    * Constructs a %ReverseIteratorBase object as a duplicate of the specified
    * bidirectional iterator \a i.
    */
   ReverseIteratorBase( const BI& i ) : Iterator<C, T>(), iterator( i )
   {
   }

   /*!
    * Returns a reference to the object pointed to by this iterator.
    */
   T& operator *() const
   {
      return (T&)*iterator;
   }

   /*!
    * Returns a bidirectional iterator that points to the object pointed by
    * this %ReverseIteratorBase object.
    */
   operator BI() const
   {
      return GetIterator();
   }

   /*!
    * A synonym for operator BI() const.
    */
   BI GetIterator() const
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
 * Returns true if two reverse iterators \a i and \a j are equal. Two reverse
 * iterators are equal if they point to the same item.
 */
template <class BI, class C, class T> inline
bool operator ==( const ReverseIteratorBase<BI, C, T>& i, const ReverseIteratorBase<BI, C, T>& j )
{
   return i.GetIterator() == j.GetIterator();
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
class PCL_CLASS ReverseBidirectionalIterator : public ReverseIteratorBase<BI, BidirectionalIterator, T>
{
   typedef ReverseIteratorBase<BI, BidirectionalIterator, T>   __R__;
   typedef ReverseBidirectionalIterator<BI, T>                 __I__;

public:

   /*!
    * Constructs a %ReverseBidirectionalIterator object.
    */
   ReverseBidirectionalIterator() : __R__()
   {
   }

   /*!
    * Copy constructor.
    */
   ReverseBidirectionalIterator( const ReverseBidirectionalIterator<BI, T>& i ) : __R__( i )
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
class PCL_CLASS ReverseRandomAccessIterator : public ReverseIteratorBase<RI, RandomAccessIterator, T>
{
   typedef ReverseIteratorBase<RI, RandomAccessIterator, T>    __R__;
   typedef ReverseRandomAccessIterator<RI, T>                  __I__;

public:

   /*!
    * Constructs a %ReverseRandomAccessIterator object.
    */
   ReverseRandomAccessIterator() : __R__()
   {
   }

   /*!
    * Copy constructor.
    */
   ReverseRandomAccessIterator( const ReverseRandomAccessIterator<RI, T>& i ) : __R__( i )
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
    * of the current iterator position.
    */
   T& operator[]( size_type d ) const
   {
      return (T&)*(__R__::iterator - d);
   }

   /*!
    * Increments this interator by the specified distance \a d. Returns a
    * reference to this object.
    */
   ReverseRandomAccessIterator<RI, T>& operator +=( distance_type d )
   {
      __R__::iterator -= d;
      return *this;
   }

   /*!
    * Decrements this interator by the specified distance \a d. Returns a
    * reference to this object.
    */
   ReverseRandomAccessIterator<RI, T>& operator -=( distance_type d )
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
bool operator <( const ReverseRandomAccessIterator<RI, T>& i, const ReverseRandomAccessIterator<RI, T>& j )
{
   return j.GetIterator() < i.GetIterator();
}

/*! #
 */
template <class RI, class T> inline
ReverseRandomAccessIterator<RI, T> operator +( const ReverseRandomAccessIterator<RI, T>& i, distance_type d )
{
   RI r = i.GetIterator();
   return r -= d;
}

/*! #
 */
template <class RI, class T> inline
ReverseRandomAccessIterator<RI, T> operator +( distance_type d, const ReverseRandomAccessIterator<RI, T>& i )
{
   return i + d;
}

/*! #
 */
template <class RI, class T> inline
ReverseRandomAccessIterator<RI, T> operator -( const ReverseRandomAccessIterator<RI, T>& i, distance_type d )
{
   RI r = i.GetIterator();
   return r += d;
}

/*! #
 */
template <class RI, class T> inline
distance_type operator -( const ReverseRandomAccessIterator<RI, T>& i, const ReverseRandomAccessIterator<RI, T>& j )
{
   return j.GetIterator() - i.GetIterator();
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Iterator_h

// ****************************************************************************
// EOF pcl/Iterator.h - Released 2014/11/14 17:16:40 UTC
