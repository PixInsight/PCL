//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/Utility.h - Released 2017-06-21T11:36:33Z
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

#ifndef __PCL_Utility_h
#define __PCL_Utility_h

/// \file pcl/Utility.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Association.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup utility_algorithms Utility Algorithms
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

/*!
 * Returns a reference to the smaller of two objects \a a and \a b. Returns
 * \a b if b < a. Returns \a a if a <= b.
 *
 * \ingroup utility_algorithms
 */
template <typename T> inline constexpr
const T& Min( const T& a, const T& b )
{
   return (b < a) ? b : a;
}

// ----------------------------------------------------------------------------

/*!
 * Returns a reference to the smallest of two objects \a a and \a b, as
 * specified by the binary predicate \a p. Returns \a b if p(b,a) is true;
 * returns \a a otherwise.
 *
 * \ingroup utility_algorithms
 */
template <typename T, class BP> inline
const T& Min( const T& a, const T& b, BP p )
{
   return p( b, a ) ? b : a;
}

// ----------------------------------------------------------------------------

/*!
 * Returns a reference to the largest of two objects \a a and \a b. Returns
 * \a b if a < b. Returns \a a if b <= a.
 *
 * \ingroup utility_algorithms
 */
template <typename T> inline constexpr
const T& Max( const T& a, const T& b )
{
   return (a < b) ? b : a;
}

// ----------------------------------------------------------------------------

/*!
 * Returns a reference to the largest of two objects \a a and \a b, as
 * specified by the binary predicate \a p. Returns \a b if p(a,b) is true;
 * returns \a a otherwise.
 *
 * \ingroup utility_algorithms
 */
template <typename T, class BP> inline
const T& Max( const T& a, const T& b, BP p )
{
   return p( a, b ) ? b : a;
}

// ----------------------------------------------------------------------------

/*!
 * Returns a reference to the median of three objects \a a, \a b and \a c:
 *
 * \li \a a if (c <= a < b) || (b <= a < c)
 * \li \a b if (a < b < c) || (c <= b <= a)
 * \li \a c if (a < c < b) || (b < c <= a)
 *
 * \ingroup utility_algorithms
 */
template <typename T> inline constexpr
const T& Median( const T& a, const T& b, const T& c )
{
   return (a < b) ? ((b < c) ? b : ((a < c) ? c : a)) :
                    ((a < c) ? a : ((b < c) ? c : b));
}

// ----------------------------------------------------------------------------

/*!
 * Returns a reference to the median of three objects \a a, \a b and \a c, as
 * specified by the binary predicate \a p:
 *
 * \li \a a if p(a,b) && !p(b,c) && !p(a,c) || !p(a,b) && p(a,c)
 * \li \a b if p(a,b) && p(b,c) || !p(a,b) && !p(a,c) && !p(b,c)
 * \li \a c if p(a,b) && !p(b,c) && p(a,c) || !p(a,b) && !p(a,c) && p(b,c)
 *
 * \ingroup utility_algorithms
 */
template <typename T, class BP> inline
const T& Median( const T& a, const T& b, const T& c, BP p )
{
   return p( a, b ) ? (p( b, c ) ? b : (p( a, c ) ? c : a)) :
                      (p( a, c ) ? a : (p( b, c ) ? c : b));
}

// ----------------------------------------------------------------------------

/*!
 * Returns a reference to the specified object \a x, if and only if it belongs
 * to the range [a,b]. Returns a reference to the nearest range bounding object
 * otherwise. Returns:
 *
 * \li \a x if (a <= x <= b)
 * \li \a a if (x < a)
 * \li \a b if (b < x)
 *
 * \ingroup utility_algorithms
 */
template <typename T> inline constexpr
const T& Range( const T& x, const T& a, const T& b )
{
   PCL_PRECONDITION( a < b )
   return (x < a) ? a : ((b < x) ? b : x);
}

// ----------------------------------------------------------------------------

/*!
 * Returns a reference to the specified object \a x, if and only if it belongs
 * to the range [a,b], as specified by the binary predicate \a p. Returns a
 * reference to the nearest range bounding object otherwise. Returns:
 *
 * \li \a x if !p(x,a) && !p(b,x)
 * \li \a a if p(x,a)
 * \li \a b if p(b,x)
 *
 * \ingroup utility_algorithms
 */
template <typename T, class BP> inline
const T& Range( const T& x, const T& a, const T& b, BP p )
{
   PCL_PRECONDITION( p( a, b ) )
   return p( x, a ) ? a : (p( b, x ) ? b : x);
}

// ----------------------------------------------------------------------------

/*!
 * Returns the number of elements in the specified C array \a a.
 *
 * \ingroup utility_algorithms
 */
#define ItemsInArray( a )  (sizeof( a )/sizeof( *a ))

// ----------------------------------------------------------------------------

/*!
 * Exchanges two objects \a a and \a b.
 *
 * \ingroup utility_algorithms
 */
template <typename T> inline
void Swap( T& a, T& b )
#ifndef _MSC_VER
   noexcept( std::is_nothrow_copy_constructible<T>::value
          && std::is_nothrow_copy_assignable<T>::value
          && std::is_nothrow_move_assignable<T>::value )
#endif
{
   T c( a ); a = b; b = std::move( c );
}

// ----------------------------------------------------------------------------

/*!
 * Applies a unary function \a f to the range [i,j). For each iterator t in the
 * range [i,j) this function performs the function call f(*t).
 *
 * \ingroup utility_algorithms
 */
template <class FI, class F> inline
void Apply( FI i, FI j, F f )
{
   for( ; i != j; ++i )
      f( *i );
}

// ----------------------------------------------------------------------------

/*!
 * Applies a binary function \a f to the range [i,j) with the specified
 * right-hand constant argument \a x. For each iterator t in the range [i,j)
 * this function performs the function call f(*t,x).
 *
 * \ingroup utility_algorithms
 */
template <class FI, class F, typename T1> inline
void Apply( FI i, FI j, F f, T1 x )
{
   for( ; i != j; ++i )
      f( *i, x );
}

// ----------------------------------------------------------------------------

/*!
 * Applies a unary function \a f to those elements in the range [i,j) that
 * satisfy a condition given by a unary predicate \a p. For each iterator t in
 * the range [i,j) this function performs the function call f(*t) if and only
 * if p(*t) is true.
 *
 * \ingroup utility_algorithms
 */
template <class FI, class F, class UP> inline
void ApplyIf( FI i, FI j, F f, UP p )
{
   for( ; i != j; ++i )
      if ( p( *i ) )
         f( *i );
}

// ----------------------------------------------------------------------------

/*!
 * Applies a unary function \a f to those elements in the range [i,j) that
 * satisfy a condition given by a unary predicate \a p, with right-hand
 * constant argument \a x. For each iterator t in the range [i,j) this function
 * performs the function call f(*t,x) if and only if p(*t) is true.
 *
 * \ingroup utility_algorithms
 */
template <class FI, class F, class UP, typename T1> inline
void ApplyIf( FI i, FI j, F f, UP p, T1 x )
{
   for( ; i != j; ++i )
      if ( p( *i ) )
         f( *i, x );
}

// ----------------------------------------------------------------------------

/*!
 * Returns the first iterator t in the range [i,j) such that the specified
 * unary predicate p(*t) is true, or j if no such iterator exists.
 *
 * \ingroup utility_algorithms
 */
template <class FI, class UP> inline
FI FirstThat( FI i, FI j, UP p )
{
   for ( ; i != j; ++i )
      if ( p( *i ) )
         break;
   return i;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the first iterator t in the range [i,j) such that the specified
 * unary predicate p(*t,x) is true, or j if no such iterator exists.
 *
 * \ingroup utility_algorithms
 */
template <class FI, class UP, typename T1> inline
FI FirstThat( FI i, FI j, UP p, T1 x )
{
   for ( ; i != j; ++i )
      if ( p( *i, x ) )
         break;
   return i;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the last iterator t in the range [i,j) such that the specified
 * unary predicate p(*t) is true, or j if no such iterator exists.
 *
 * \ingroup utility_algorithms
 */
template <class BI, class UP> inline
BI LastThat( BI i, BI j, UP p )
{
   for ( BI k = j; i != k; )
      if ( p( *--k ) )
         return k;
   return j;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the last iterator t in the range [i,j) such that the specified
 * unary predicate p(*t,x) is true, or j if no such iterator exists.
 *
 * \ingroup utility_algorithms
 */
template <class BI, class UP, typename T1> inline
BI LastThat( BI i, BI j, UP p, T1 x )
{
   for ( BI k = j; i != k; )
      if ( p( *--k, x ) )
         return k;
   return j;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the total number of objects in the range [i,j) that are equal to
 * the specified constant object \a v.
 *
 * \ingroup utility_algorithms
 */
template <class FI, typename T> inline
size_type Count( FI i, FI j, const T& v )
{
   size_type N = 0;
   for( ; i != j; ++i )
      if ( *i == v )
         ++N;
   return N;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the total number of objects in the range [i,j) that are equal to
 * a constant object \a v, as specified by the binary predicate \a p. For each
 * iterator t in the range [i,j), counts the number of objects for which
 * p(*t,v) is true.
 *
 * \ingroup utility_algorithms
 */
template <class FI, typename T, class BP> inline
size_type Count( FI i, FI j, const T& v, BP p )
{
   size_type N = 0;
   for( ; i != j; ++i )
      if ( p( *i, v ) )
         ++N;
   return N;
}

// ----------------------------------------------------------------------------

/*!
 * Returns the total number of objects in the range [i,j) that satisfy a
 * condition given by a unary predicate \a p. For each iterator t in the range
 * [i,j), counts the number of objects for which p(*t) is true.
 *
 * \ingroup utility_algorithms
 */
template <class FI, class UP> inline
size_type CountIf( FI i, FI j, UP p )
{
   size_type N = 0;
   for( ; i != j; ++i )
      if ( p( *i ) )
         ++N;
   return N;
}

// ----------------------------------------------------------------------------

/*!
 * Returns an iterator m in the range [i,j) such that *m <= *t for any t != m
 * in [i,j). Returns j if and only if i == j.
 *
 * \ingroup utility_algorithms
 */
template <class FI> inline
FI MinItem( FI i, FI j )
{
   FI k = i;
   if ( i != j )
      while ( ++i != j )
         if ( *i < *k )
            k = i;
   return k;
}

// ----------------------------------------------------------------------------

/*!
 * Returns an iterator m in the range [i,j) such that p(*t,*m) is false for any
 * t != m in [i,j). Returns j if and only if i == j.
 *
 * \ingroup utility_algorithms
 */
template <class FI, class BP> inline
FI MinItem( FI i, FI j, BP p )
{
   FI k = i;
   if ( i != j )
      while ( ++i != j )
         if ( p( *i, *k ) )
            k = i;
   return k;
}

// ----------------------------------------------------------------------------

/*!
 * Returns an iterator m in the range [i,j) such that *m >= *t for any t != m
 * in [i,j). Returns j if and only if i == j.
 *
 * \ingroup utility_algorithms
 */
template <class FI> inline
FI MaxItem( FI i, FI j )
{
   FI k = i;
   if ( i != j )
      while ( ++i != j )
         if ( *k < *i )
            k = i;
   return k;
}

// ----------------------------------------------------------------------------

/*!
 * Returns an iterator m in the range [i,j) such that p(*m,*t) is false for any
 * t != m in [i,j). Returns j if and only if i == j.
 *
 * \ingroup utility_algorithms
 */
template <class FI, class BP> inline
FI MaxItem( FI i, FI j, BP p )
{
   FI k = i;
   if ( i != j )
      while ( ++i != j )
         if ( p( *k, *i ) )
            k = i;
   return k;
}

// ----------------------------------------------------------------------------

/*!
 * Finds two iterators n and m in the range [i,j) such that *n <= *u for any
 * u != n in [i,j) and *m >= *v for any v != m in [i,j).
 *
 * \ingroup utility_algorithms
 */
template <class FI> inline
void FindExtremeItems( FI& kmin, FI& kmax, FI i, FI j )
{
   kmin = kmax = i;
   if ( i != j )
      while ( ++i != j )
      {
         if ( *i < *kmin )
            kmin = i;
         if ( *kmax < *i )
            kmax = i;
      }
}

// ----------------------------------------------------------------------------

/*!
 * Finds two iterators n and m in the range [i,j) such that p(*u,*n) is false
 * for any u != n in [i,j) and p(*m,*v) is false for any v != m in [i,j).
 *
 * \ingroup utility_algorithms
 */
template <class FI, class BP> inline
void FindExtremeItems( FI& kmin, FI& kmax, FI i, FI j, BP p )
{
   kmin = kmax = i;
   if ( i != j )
      while ( ++i != j )
      {
         if ( p( *i, *kmin ) )
            kmin = i;
         if ( p( *kmax, *i ) )
            kmax = i;
      }
}

// ----------------------------------------------------------------------------

/*!
 * Returns a tuple {i,j} such that *i != *j, where i >= i1 and j is in the
 * range [i2,j2), or a tuple {i>=i1,j2} if no such j exists.
 *
 * \ingroup utility_algorithms
 */
template <class FI1, class FI2> inline
Association<FI1, FI2> FindNotEqual( FI1 i1, FI2 i2, FI2 j2 )
{
   for ( ; i2 != j2 && *i1 == *i2; ++i1, ++i2 ) {}
   return Associate( i1, i2 );
}

// ----------------------------------------------------------------------------

/*!
 * Returns a tuple {i,j} such that p(*i,*j) is false, where i >= i1 and j is in
 * the range [i2,j2), or a tuple {i>=i1,j2} if no such j exists.
 *
 * \ingroup utility_algorithms
 */
template <class FI1, class FI2, class BP> inline
Association<FI1, FI2> FindNotEqual( FI1 i1, FI2 i2, FI2 j2, BP p )
{
   for ( ; i2 != j2 && p( *i1, *i2 ); ++i1, ++i2 ) {}
   return Associate( i1, i2 );
}

// ----------------------------------------------------------------------------

/*!
 * Returns true iff the objects in the range [i1,j1) are equal to the
 * corresponding objects in the range [i2,j2), with
 * j1 = Advance(i1,Distance(i2,j2)).
 *
 * \ingroup utility_algorithms
 */
template <class FI1, class FI2> inline
bool Equal( FI1 i1, FI2 i2, FI2 j2 )
{
   return FindNotEqual( i1, i2, j2 ).second == j2;
}

// ----------------------------------------------------------------------------

/*!
 * Returns true iff the objects in the range [i1,j1) satisfy the condition
 * specified by the binary predicate \a p for the corresponding objects in the
 * range [i2,j2), with j1 = Advance(i1,Distance(i2,j2)).
 *
 * \ingroup utility_algorithms
 */
template <class FI1, class FI2, class BP> inline
bool Equal( FI1 i1, FI2 i2, FI2 j2, BP p )
{
   return FindNotEqual( i1, i2, j2, p ).second == j2;
}

// ----------------------------------------------------------------------------

/*!
 * Performs a comparison of the objects in the ranges [i1,j1) and [i2,j2).
 * Returns the result of the comparison encoded as an integer:
 *
 * Returns 0 if:
 *
 * \li (1) Distance(i1,j1) == Distance(i2,j2).
 * \li (2) For each pair {u,v} of iterators such that u in [i1,j1) and v in
 * [i2,j2) and Distance(i1,u) == Distance(i2,v), *u == *v.
 *
 * Returns -1 if:
 *
 * \li (3) A pair {u,v} of iterators exists such that u in [i1,j1) and v in
 * [i2,j2) and Distance(i1,u) == Distance(i2,v) and *u < *v.
 * \li (4) Condition (2) is true and Distance(i1,j1) < Distance(i2,j2).
 *
 * Returns +1 if:
 *
 * \li (5) A pair {u,v} of iterators exists such that u in [i1,j1) and v in
 * [i2,j2) and Distance(i1,u) == Distance(i2,v) and *v < *u.
 * \li (6) Condition (2) is true and Distance(i1,j1) > Distance(i2,j2).
 *
 * \ingroup utility_algorithms
 */
template <class FI1, class FI2> inline
int Compare( FI1 i1, FI1 j1, FI2 i2, FI2 j2 )
{
   for ( ; ; ++i1, ++i2 )
   {
      if ( i1 == j1 )
         return (i2 == j2) ? 0 : -1;
      if ( i2 == j2 )
         return +1;
      if ( *i1 < *i2 )
         return -1;
      if ( *i2 < *i1 )
         return +1;
   }
}

// ----------------------------------------------------------------------------

/*!
 * Performs a comparison of the objects in the ranges [i1,j1) and [i2,j2) as
 * specified by a binary predicate \a p. Returns the result of the comparison
 * encoded as an integer:
 *
 * Returns 0 if:
 *
 * \li (1) Distance(i1,j1) == Distance(i2,j2).
 * \li (2) For each pair {u,v} of iterators such that u in [i1,j1) and v in
 * [i2,j2) and Distance(i1,u) == Distance(i2,v), both p(*u,*v) and p(*v,*u) are
 * false.
 *
 * Returns -1 if:
 *
 * \li (3) A pair {u,v} of iterators exists such that u in [i1,j1) and v in
 * [i2,j2) and Distance(i1,u) == Distance(i2,v) and p(*u,*v) is true.
 * \li (4) Condition (2) is true and Distance(i1,j1) < Distance(i2,j2).
 *
 * Returns +1 if:
 *
 * \li (5) A pair {u,v} of iterators exists such that u in [i1,j1) and v in
 * [i2,j2) and Distance(i1,u) == Distance(i2,v) and p(*v,*u) is true.
 * \li (6) Condition (2) is true and Distance(i1,j1) > Distance(i2,j2).
 *
 * \ingroup utility_algorithms
 */
template <class FI1, class FI2, class BP> inline
int Compare( FI1 i1, FI1 j1, FI2 i2, FI2 j2, BP p )
{
   for ( ; ; ++i1, ++i2 )
   {
      if ( i1 == j1 )
         return (i2 == j2) ? 0 : -1;
      if ( i2 == j2 )
         return +1;
      if ( p( *i1, *i2 ) )
         return -1;
      if ( p( *i2, *i1 ) )
         return +1;
   }
}

// ----------------------------------------------------------------------------

}  // pcl

#endif  // __PCL_Utility_h

// ----------------------------------------------------------------------------
// EOF pcl/Utility.h - Released 2017-06-21T11:36:33Z
