//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.06.0853
// ----------------------------------------------------------------------------
// pcl/Sort.h - Released 2017-06-28T11:58:36Z
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

#ifndef __PCL_Sort_h
#define __PCL_Sort_h

/// \file pcl/Sort.h

#include <pcl/Defs.h>

#include <pcl/Iterator.h>
#include <pcl/Utility.h>

#define __PCL_QS_STACK_SIZE   32  // Stack size for the QuickSort algorithms
#define __PCL_QS_IS_THRESHOLD 11  // QuickSort/InsertionSort switch threshold

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup sorting_algorithms Sorting Algorithms
 *
 * Template formal parameters:
 *
 * FI   Forward iterator \n
 * BI   Bidirectional iterator \n
 * RI   Random access iterator \n
 * UP   Unary predicate \n
 * BP   Binary predicate \n
 * T    Item type \n
 * F    Function
 */

// ----------------------------------------------------------------------------

template <class BI, class T> inline
void __pcl_insertion_sort__( BI i, BI j, const T* )
{
   if ( i != j )
   {
      BI x = j, k = j;
      for ( --k; --x != i; )
         if ( *x < *--k )
            Swap( *x, *k );
      if ( ++x != j )
         for ( ; ++x != j; )
         {
            BI y = x;
            T v = *x;
            for ( k = y; v < *--k; y = k )
               *y = *k;
            *y = v;
         }
   }
}

/*!
 * Generic insertion sort algorithm.
 *
 * Sorts a range [i,j) in ascending order by the <em>insertion sort</em>
 * algorithm. Ordering of elements is defined such that for any pair a, b of
 * elements in [i,j) a < b is true if a precedes b.
 *
 * \ingroup sorting_algorithms
 */
template <class BI> inline
void InsertionSort( BI i, BI j )
{
   __pcl_insertion_sort__( i, j, ItemType( i ) );
}

// ----------------------------------------------------------------------------

template <class BI, class BP, class T> inline
void __pcl_insertion_sort__( BI i, BI j, BP p, const T* )
{
   if ( i != j )
   {
      BI x = j, k = j;
      for ( --k; --x != i; )
         if ( p( *x, *--k ) )
            Swap( *x, *k );
      if ( ++x != j )
         for ( ; ++x != j; )
         {
            BI y = x;
            T v = *x;
            for ( k = y; p( v, *--k ); y = k )
               *y = *k;
            *y = v;
         }
   }
}

/*!
 * Generic insertion sort algorithm.
 *
 * Sorts a range [i,j) in ascending order by the <em>insertion sort</em>
 * algorithm. Ordering of elements is defined such that for any pair a, b of
 * elements in [i,j) the binary predicate p(a,b) is true if a precedes b.
 *
 * \ingroup sorting_algorithms
 */
template <class BI, class BP> inline
void InsertionSort( BI i, BI j, BP p )
{
   __pcl_insertion_sort__( i, j, p, ItemType( i ) );
}

// ----------------------------------------------------------------------------

template <class RI, class T> inline
void __pcl_quick_sort__( RI i, RI j, T* )
{
   distance_type n = j - i;
   if ( n < 2 )
      return;

   distance_type tos[ 2*__PCL_QS_STACK_SIZE ];
   distance_type* sp = tos;

   for ( distance_type l = 0, r = n-1; ; )
   {
      RI x0 = i + l;
      RI y =  i + r;

      if ( r-l < __PCL_QS_IS_THRESHOLD )
      {
         for ( RI x = x0; ++x <= y; )
         {
            T v = *x;
            RI x1 = x;
            for ( ; --x1 >= x0 && v < *x1; )
               *(x1+1) = *x1;
            *(x1+1) = v;
         }

         if ( sp == tos )
            break;

         r = *--sp;
         l = *--sp;
      }
      else
      {
         RI x = x0;

         Swap( *++x, *(i + ((l+r) >> 1)) );

         if ( *y < *x0 )
            Swap( *x0, *y );
         if ( *y < *x )
            Swap( *x, *y );
         if ( *x < *x0 )
            Swap( *x, *x0 );

         T v = *x;

         for ( ;; )
         {
            while ( *++x < v );
            while ( v < *--y );
            if ( y < x )
               break;
            Swap( *x, *y );
         }

         *(x0+1) = *y;
         *y = v;

         distance_type dx = x - i;
         distance_type dy = y - i;

         if ( r-dx+1 >= dy-l )
         {
            *sp++ = dx;
            *sp++ = r;
            r = dy-1;
         }
         else
         {
            *sp++ = l;
            *sp++ = dy-1;
            l = dx;
         }
      }
   }
}

/*!
 * Generic quick sort algorithm.
 *
 * Sorts a range [i,j) in ascending order by the <em>quick sort</em> algorithm
 * (median of three variant). Ordering of elements is defined such that for any
 * pair a, b of elements in [i,j) a < b is true if a precedes b.
 *
 * \ingroup sorting_algorithms
 */
template <class RI> inline
void QuickSort( RI i, RI j )
{
   __pcl_quick_sort__( i, j, ItemType( i ) );
}

// ----------------------------------------------------------------------------

template <class RI, class BP, class T> inline
void __pcl_quick_sort__( RI i, RI j, BP p, T* )
{
   distance_type n = j - i;
   if ( n < 2 )
      return;

   distance_type tos[ 2*__PCL_QS_STACK_SIZE ];
   distance_type* sp = tos;

   for ( distance_type l = 0, r = n-1; ; )
   {
      RI x0 = i + l;
      RI y =  i + r;

      if ( r-l < __PCL_QS_IS_THRESHOLD )
      {
         for ( RI x = x0; ++x <= y; )
         {
            T v = *x;
            RI x1 = x;
            for ( ; --x1 >= x0 && p( v, *x1 ); )
               *(x1+1) = *x1;
            *(x1+1) = v;
         }

         if ( sp == tos )
            break;

         r = *--sp;
         l = *--sp;
      }
      else
      {
         RI x = x0;

         Swap( *++x, *(i + ((l+r) >> 1)) );

         if ( p( *y, *x0 ) )
            Swap( *x0, *y );
         if ( p( *y, *x ) )
            Swap( *x, *y );
         if ( p( *x, *x0 ) )
            Swap( *x, *x0 );

         T v = *x;

         for ( ;; )
         {
            while ( p( *++x, v ) );
            while ( p( v, *--y ) );
            if ( y < x )
               break;
            Swap( *x, *y );
         }

         *(x0+1) = *y;
         *y = v;

         distance_type dx = x - i;
         distance_type dy = y - i;

         if ( r-dx+1 >= dy-l )
         {
            *sp++ = dx;
            *sp++ = r;
            r = dy-1;
         }
         else
         {
            *sp++ = l;
            *sp++ = dy-1;
            l = dx;
         }
      }
   }
}

/*!
 * Generic quick sort algorithm.
 *
 * Sorts a range [i,j) in ascending order by the <em>quick sort</em> algorithm
 * (median of three variant). Ordering of elements is defined such that for any
 * pair a, b of elements in [i,j) the binary predicate p(a,b) is true if a
 * precedes b.
 *
 * \ingroup sorting_algorithms
 */
template <class RI, class BP> inline
void QuickSort( RI i, RI j, BP p )
{
   __pcl_quick_sort__( i, j, p, ItemType( i ) );
}

// ----------------------------------------------------------------------------

template <class RI, class T> inline
void __pcl_heap_sort__( RI i, RI j, T* )
{
   distance_type dj = j - i;
   if ( dj < 2 )
      return;

   T v;
   distance_type di = 1 + (dj >> 1);

   for ( i += di-1, --j; ; )
   {
      if ( di > 1 )
      {
         v = *--i;
         --di;
      }
      else
      {
         v = *j;
         *j = *i;

         if ( --dj == 0 )
         {
            *i = v;
            break;
         }

         --j;
      }

      RI x = i;
      RI y = i;

      for ( distance_type dy2 = di, dy = di; !(dj < (dy <<= 1)); dy2 = dy )
      {
         y += dy2;

         if ( dy < dj && *y < *(y+1) )
         {
            ++y;
            ++dy;
         }

         if ( v < *y )
         {
            *x = *y;
            x = y;
         }
         else
            break;
      }

      *x = v;
   }
}

/*!
 * Generic heap sort algorithm.
 *
 * Sorts a range [i,j) in ascending order by the <em>heap sort</em> algorithm.
 * Ordering of elements is defined such that for any pair a, b of elements in
 * [i,j) a < b is true if a precedes b.
 *
 * \ingroup sorting_algorithms
 */
template <class RI> inline
void HeapSort( RI i, RI j )
{
   __pcl_heap_sort__( i, j, ItemType( i ) );
}

// ----------------------------------------------------------------------------

template <class RI, class BP, class T> inline
void __pcl_heap_sort__( RI i, RI j, BP p, T* )
{
   distance_type dj = j - i;
   if ( dj < 2 )
      return;

   T v;
   distance_type di = 1 + (dj >> 1);

   for ( i += di-1, --j; ; )
   {
      if ( di > 1 )
      {
         v = *--i;
         --di;
      }
      else
      {
         v = *j;
         *j = *i;

         if ( --dj == 0 )
         {
            *i = v;
            break;
         }

         --j;
      }

      RI x = i;
      RI y = i;

      for ( distance_type dy2 = di, dy = di; !(dj < (dy <<= 1)); dy2 = dy )
      {
         y += dy2;

         if ( dy < dj && p( *y, *(y+1) ) )
         {
            ++y;
            ++dy;
         }

         if ( p( v, *y ) )
         {
            *x = *y;
            x = y;
         }
         else
            break;
      }

      *x = v;
   }
}

/*!
 * Generic heap sort algorithm.
 *
 * Sorts a range [i,j) in ascending order by the <em>heap sort</em> algorithm.
 * Ordering of elements is defined such that for any pair a, b of elements in
 * [i,j) the binary predicate p(a,b) is true if a precedes b.
 *
 * \ingroup sorting_algorithms
 */
template <class RI, class BP> inline
void HeapSort( RI i, RI j, BP p )
{
   __pcl_heap_sort__( i, j, p, ItemType( i ) );
}

// ----------------------------------------------------------------------------

template <class BI> inline
void __pcl_sort__( BI i, BI j, BidirectionalIterator )
{
   InsertionSort( i, j );
}

template <class RI> inline
void __pcl_sort__( RI i, RI j, RandomAccessIterator )
{
#ifdef __PCL_PREFER_HEAPSORT
   HeapSort( i, j );
#else
   QuickSort( i, j );
#endif
}

/*!
 * Generic sort algorithm.
 *
 * Sorts a range [i,j) in ascending order. Ordering of elements is defined such
 * that for any pair a, b of elements in [i,j) a < b is true if a precedes b.
 *
 * This function sorts the specified input sequence employing the fastest
 * (known) sorting algorithm for the iterator class BI. Insertion sort is
 * used for bidirectional iterators without random access, and the quick sort
 * algorithm (median of three variant) is used for random access iterators.
 *
 * If you want to use the heap sort algorithm instead of quick sort (e.g. for
 * performance testing purposes), define the __PCL_PREFER_HEAPSORT macro.
 *
 * \ingroup sorting_algorithms
 */
template <class BI> inline
void Sort( BI i, BI j )
{
   __pcl_sort__( i, j, IteratorClass( i ) );
}

// ----------------------------------------------------------------------------

template <class BI, class BP> inline
void __pcl_sort__( BI i, BI j, BP p, BidirectionalIterator )
{
   InsertionSort( i, j, p );
}

template <class RI, class BP> inline
void __pcl_sort__( RI i, RI j, BP p, RandomAccessIterator )
{
#ifdef __PCL_PREFER_HEAPSORT
   HeapSort( i, j, p );
#else
   QuickSort( i, j, p );
#endif
}

/*!
 * Generic sort algorithm.
 *
 * Sorts a range [i,j) in ascending order. Ordering of elements is defined such
 * that for any pair a, b of elements in [i,j) the binary predicate p(a,b) is
 * true if a precedes b.
 *
 * This function sorts the specified input sequence employing the fastest
 * (known) sorting algorithm for the iterator class BI. Insertion sort is
 * used for bidirectional iterators without random access, and the quick sort
 * algorithm (median of three variant) is used for random access iterators.
 *
 * If you want to use the heap sort algorithm instead of quick sort (e.g. for
 * performance testing purposes), define the __PCL_PREFER_HEAPSORT macro.
 *
 * \ingroup sorting_algorithms
 */
template <class BI, class BP> inline
void Sort( BI i, BI j, BP p )
{
   __pcl_sort__( i, j, p, IteratorClass( i ) );
}

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Sort_h

// ----------------------------------------------------------------------------
// EOF pcl/Sort.h - Released 2017-06-28T11:58:36Z
