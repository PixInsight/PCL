// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/Point.h - Released 2014/11/14 17:16:34 UTC
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

#ifndef __PCL_Point_h
#define __PCL_Point_h

/// \file pcl/Point.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Math_h
#include <pcl/Math.h>
#endif

#ifndef __PCL_Relational_h
#include <pcl/Relational.h>
#endif

#ifdef __PCL_QT_INTERFACE
#  include <qglobal.h>
#  if ( QT_VERSION >= 0x040000 )
#    include <QtCore/QPoint>
#  else
#    include <qpoint.h>
#  endif
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * ### N.B.: The template class GenericPoint<T> cannot have virtual member
 *           functions. This is because some internal PCL and core routines
 *           rely on GenericPoint<int>, GenericPoint<float> and
 *           GenericPoint<double> being directly castable to int*, float* and
 *           double*, respectively.
 */

/*!
 * \class GenericPoint
 * \brief A generic point in the two-dimensional space.
 *
 * %GenericPoint implements a point in the plane, specified by its \a x and
 * \a y coordinates. The type T represents scalar point coordinates and can be
 * any real or integer numerical type.
 *
 * The coordinates of %GenericPoint are accessed directly by its x and y data
 * members. Given a %GenericPoint instance \a p, you can use \a p.x and \a p.y
 * directly to get or set coordinate values.
 *
 * \sa \ref point_types_2d "2-D Point Types",
 * \ref point_functions_2d "2-D Point Operators and Functions",
 * GenericRectangle
 */
template <typename T>
class PCL_CLASS GenericPoint
{
public:

   /*!
    * Represents the type of a point component.
    */
   typedef T   component;

   /*
    * Point coordinates
    */
   T  x; //!< Abscissa (horizontal coordinate).
   T  y; //!< Ordinate (vertical coordinate).

   /*!
    * Constructs a default %GenericPoint instance. Point coordinates are not
    * initialized, so they'll have unpredictable garbage values.
    */
   GenericPoint()
   {
   }

   /*!
    * Constructs a %GenericPoint instance given its coordinates \a xPos and
    * \a yPos in the plane.
    *
    * The type T1 can be any suitable real or integer numerical type, or a type
    * with numeric conversion semantics.
    */
   template <typename T1>
   GenericPoint( T1 xPos, T1 yPos ) : x( T( xPos ) ), y( T( yPos ) )
   {
   }

   /*!
    * Copy constructor for %GenericPoint.
    */
   GenericPoint( T p )
   {
      x = y = p;
   }

   /*!
    * Constructs a %GenericPoint instance as a copy (possibly involving a
    * conversion between numerical types) of another point \a p with different
    * template argument.
    */
   template <typename T1>
   GenericPoint( const GenericPoint<T1>& p ) : x( T( p.x ) ), y( T( p.y ) )
   {
   }

#ifdef __PCL_QT_INTERFACE
   GenericPoint( const QPoint& p ) : x( T( p.x() ) ), y( T( p.y() ) )
   {
   }
#endif

   /*!
    * Moves this point to the location of another point \a p.
    *
    * \sa MoveBy()
    */
   template <typename T1>
   void MoveTo( const GenericPoint<T1>& p )
   {
      MoveTo( p.x, p.y );
   }

   /*!
    * Moves this point to the specified absolute coordinates \a xPos and
    * \a yPos in the plane.
    *
    * \sa MoveBy()
    */
   template <typename T1>
   void MoveTo( T1 xPos, T1 yPos )
   {
      x = T( xPos ); y = T( yPos );
   }

#ifdef __PCL_QT_INTERFACE
   void MoveTo( const QPoint& p )
   {
      MoveTo( p.x(), p.y() );
   }
#endif

   /*!
    * Returns a point whose coordinates are equal to the coordinates of the
    * specified point \a d.
    */
   template <typename T1>
   GenericPoint<T> MovedTo( const GenericPoint<T1>& p ) const
   {
      return GenericPoint<T>( T( p.x ), T( p.y ) );
   }

   /*!
    * Returns a point at the specified \a xPos and \a yPos coordinates.
    */
   template <typename T1>
   GenericPoint<T> MovedTo( T1 xPos, T1 yPos ) const
   {
      return GenericPoint<T>( T( xPos ), T( yPos ) );
   }

   /*!
    * Moves this point by equal increments in the X and Y directions, relative
    * to its current position.
    *
    * \param d    Increments in the X and Y directions, given by \a d.x and
    *             \a d.y, respectively.
    *
    * \sa MoveTo()
    */
   template <typename T1>
   void MoveBy( const GenericPoint<T1>& d )
   {
      MoveBy( d.x, d.y );
   }

   /*!
    * Moves this point by increments \a dx and \a dy in the X and Y directions,
    * respectively, relative to its current position.
    *
    * Given the declaration:
    *
    * \code GenericPoint<T> p; \endcode
    *
    * These two expressions are equivalent:
    *
    * \code
    * p.MoveBy( -3, 7 );
    * p.MoveTo( p.x-3, p.y+7 );
    * \endcode
    *
    * \sa MoveTo()
    */
   template <typename T1>
   void MoveBy( T1 dx, T1 dy )
   {
      x += T( dx ); y += T( dy );
   }

   /*!
    * Moves this point by the same increment \a dxy in the X and Y directions,
    * relative to its current position.
    *
    * This function is functionally equivalent to:
    *
    * \code MoveBy( dxy, dxy ); \endcode
    *
    * \sa MoveTo()
    */
   template <typename T1>
   void MoveBy( T1 dxy )
   {
      x += T( dxy ); y += T( dxy );
   }

#ifdef __PCL_QT_INTERFACE
   void MoveBy( const QPoint& p )
   {
      MoveBy( p.x(), p.y() );
   }
#endif

   /*!
    * Returns a point whose coordinates are the coordinates of this point
    * displaced by the increments specified as the point \a d.
    */
   template <typename T1>
   GenericPoint<T> MovedBy( const GenericPoint<T1>& d ) const
   {
      return GenericPoint<T>( x + T( d.x ), y + T( d.y ) );
   }

   /*!
    * Returns a point whose coordinates are the coordinates of this point
    * displaced by the specified increments \a dx and \a dy.
    */
   template <typename T1>
   GenericPoint<T> MovedBy( T1 dx, T1 dy ) const
   {
      return GenericPoint<T>( x + T( dx ), y + T( dy ) );
   }

   /*!
    * Rounds the coordinates of this point to their corresponding nearest
    * integer coordinates.
    *
    * \sa Round( int ), Rounded(), Truncate(), RoundedToInt(), TruncatedToInt()
    */
   void Round()
   {
      x = T( pcl::Round( double( x ) ) );
      y = T( pcl::Round( double( y ) ) );
   }

   /*!
    * Rounds the coordinates of this point to \a n fractional digits (\a n >= 0).
    *
    * \sa Round(), Rounded( int ), Truncate(), RoundedToInt(), TruncatedToInt()
    */
   void Round( int n )
   {
      PCL_PRECONDITION( n >= 0 )
      x = T( pcl::Round( double( x ), n ) );
      y = T( pcl::Round( double( y ), n ) );
   }

   /*!
    * Returns a point whose coordinates are the coordinates of this point
    * rounded to their nearest integers.
    *
    * \sa Round(), Rounded( int ), Truncated(), RoundedToInt(), TruncatedToInt()
    */
   GenericPoint<T> Rounded() const
   {
      return GenericPoint<T>( T( pcl::Round( double( x ) ) ), T( pcl::Round( double( y ) ) ) );
   }

   /*!
    * Returns a point whose coordinates are the coordinates of this point
    * rounded to \a n fractional digits (\a n >= 0).
    *
    * \sa Round( int ), Rounded(), Truncated(), RoundedToInt(), TruncatedToInt()
    */
   GenericPoint<T> Rounded( int n ) const
   {
      PCL_PRECONDITION( n >= 0 )
      return GenericPoint<T>( T( pcl::Round( double( x ), n ) ), T( pcl::Round( double( y ), n ) ) );
   }

   /*!
    * Returns a point of integer template type whose coordinates are the
    * coordinates of this point rounded to their nearest integers.
    *
    * \sa Round(), Rounded(), Truncated(), TruncatedToInt()
    */
   GenericPoint<int> RoundedToInt() const
   {
      return GenericPoint<int>( pcl::RoundI( double( x ) ), pcl::RoundI( double( y ) ) );
   }

   /*!
    * Integer truncation of coordinates. Sets the coordinates x, y of this
    * point to the corresponding nearest integer coordinates i, j, such that
    * i <= x and j <= y.
    *
    * \sa Truncated(), Round(), RoundedToInt(), TruncatedToInt()
    */
   void Truncate()
   {
      x = T( pcl::Trunc( double( x ) ) );
      y = T( pcl::Trunc( double( y ) ) );
   }

   /*!
    * Integer truncation of coordinates. Returns a point whose coordinates are
    * the coordinates x, y of this point truncated to their corresponding
    * nearest integer coordinates i, j, such that i <= x and j <= y.
    *
    * \sa Truncate(), Rounded(), RoundedToInt(), TruncatedToInt()
    */
   GenericPoint<T> Truncated() const
   {
      return GenericPoint<T>( T( pcl::Trunc( double( x ) ) ), T( pcl::Trunc( double( y ) ) ) );
   }

   /*!
    * Integer truncation of coordinates. Returns a point of integer template
    * type whose coordinates are the coordinates x, y of this point truncated
    * to the nearest integer coordinates i, j, such that i <= x and j <= y.
    *
    * \sa Truncate(), Truncated(), Rounded(), RoundedToInt()
    */
   GenericPoint<int> TruncatedToInt() const
   {
      return GenericPoint<int>( pcl::TruncI( double( x ) ), pcl::TruncI( double( y ) ) );
   }

   /*!
    * Assignment operator. Moves this point to the location specified as a
    * point \a p. Returns a reference to this point.
    *
    * \sa MoveTo()
    */
   template <typename T1>
   GenericPoint<T>& operator =( const GenericPoint<T1>& p )
   {
      x = T( p.x ); y = T( p.y ); return *this;
   }

   /*!
    * Assignment operator. Given a coordinate \a v, moves this point to a
    * location x = y = \a v. Returns a reference to this point.
    *
    * \sa MoveTo()
    */
   GenericPoint<T>& operator =( T v )
   {
      x = y = v; return *this;
   }

#ifdef __PCL_QT_INTERFACE
   GenericPoint<T>& operator =( const QPoint& p )
   {
      x = T( p.x() ); y = T( p.y() ); return *this;
   }
#endif

   /*!
    * Adds each coordinate of a given point \a p to its homonym coordinate in
    * this point. Returns a reference to this point.
    *
    * \sa MoveBy(), operator -=()
    */
   template <typename T1>
   GenericPoint<T>& operator +=( const GenericPoint<T1>& p )
   {
      x += T( p.x ); y += T( p.y ); return *this;
   }

   /*!
    * Adds a scalar \a d to both coordinates of this point. Returns a reference
    * to this point.
    *
    * \sa MoveBy(), operator -=()
    */
   GenericPoint<T>& operator +=( T d )
   {
      x += d; y += d; return *this;
   }

#ifdef __PCL_QT_INTERFACE
   GenericPoint<T>& operator +=( const QPoint& p )
   {
      x += T( p.x() ); y += T( p.y() ); return *this;
   }
#endif

   /*!
    * Subtracts each coordinate of a given point \a p from its homonym
    * coordinate in this point. Returns a reference to this point.
    *
    * \sa MoveBy(), operator +=()
    */
   template <typename T1>
   GenericPoint<T>& operator -=( const GenericPoint<T1>& p )
   {
      x -= T( p.x ); y -= T( p.y ); return *this;
   }

   /*!
    * Subtracts a scalar \a d from both coordinates of this point. Returns a
    * reference to this point.
    *
    * \sa MoveBy(), operator +=()
    */
   GenericPoint<T>& operator -=( T d )
   {
      x -= d; y -= d; return *this;
   }

#ifdef __PCL_QT_INTERFACE
   GenericPoint<T>& operator -=( const QPoint& p )
   {
      x -= T( p.x() ); y -= T( p.y() ); return *this;
   }
#endif

   /*!
    * Multiplies each coordinate of this point by its homonym coordinate of a
    * given point \a p. Returns a reference to this point.
    *
    * \sa operator /=(), operator +=()
    */
   template <typename T1>
   GenericPoint<T>& operator *=( const GenericPoint<T1>& p )
   {
      x *= T( p.x ); y *= T( p.y ); return *this;
   }

   /*!
    * Multiplies both coordinates of this point by a scalar \a d. Returns a
    * reference to this point.
    *
    * \sa operator /=(), operator +=()
    */
   GenericPoint<T>& operator *=( T d )
   {
      x *= d; y *= d; return *this;
   }

#ifdef __PCL_QT_INTERFACE
   GenericPoint<T>& operator *=( const QPoint& p )
   {
      x *= T( p.x() ); y *= T( p.y() ); return *this;
   }
#endif

   /*!
    * Divides each coordinate of this point by its homonym coordinate of a
    * given point \a p. Returns a reference to this point.
    *
    * \sa operator *=(), operator -=()
    */
   template <typename T1>
   GenericPoint<T>& operator /=( const GenericPoint<T1>& p )
   {
      PCL_PRECONDITION( T( p.x ) != T( 0 ) && T( p.y ) != T( 0 ) )
      x /= T( p.x ); y /= T( p.y ); return *this;
   }

   /*!
    * Divides both coordinates of this point by a scalar \a d. Returns a
    * reference to this point.
    *
    * \sa operator *=(), operator -=()
    */
   GenericPoint<T>& operator /=( T d )
   {
      PCL_PRECONDITION( d != T( 0 ) )
      x /= d; y /= d; return *this;
   }

#ifdef __PCL_QT_INTERFACE
   GenericPoint<T>& operator /=( const QPoint& p )
   {
      PCL_PRECONDITION( T( p.x() ) != T( 0 ) && T( p.y() ) != T( 0 ) )
      x /= T( p.x() ); y /= T( p.y() ); return *this;
   }
#endif

   /*!
    * Returns a copy of this point.
    */
   GenericPoint<T> operator +() const
   {
      return *this;
   }

   /*!
    * Returns a point whose coordinates have the same magnitudes as the
    * coordinates of this point, but the opposed signs. The returned point so
    * defined represents a vector diametrically opposed to this one.
    */
   GenericPoint<T> operator -() const
   {
      return GenericPoint<T>( -x, -y );
   }

   /*!
    * Returns the dot product of this point and another point given by its
    * coordinates \a px and \a py.
    */
   template <typename T1, typename T2>
   double Dot( T1 px, T2 py ) const
   {
      return double( x )*double( px ) + double( y )*double( py );
   }

   /*!
    * Returns the dot product of this point and another point \a p.
    */
   template <typename T1>
   double Dot( const GenericPoint<T1>& p ) const
   {
      return Dot( p.x, p.y );
   }

#ifdef __PCL_QT_INTERFACE
   operator QPoint() const
   {
      return QPoint( int( x ), int( y ) );
   }
#endif
};

// ----------------------------------------------------------------------------

/*!
 * \defgroup point_functions_2d 2-D Point Operators and Functions
 */

/*!
 * Returns true if two points \a p1 and \a p2 are equal. Two points are equal
 * if their homonym coordinates are equal.
 * \ingroup point_functions_2d
 */
template <typename T1, typename T2> inline
bool operator ==( const GenericPoint<T1>& p1, const GenericPoint<T2>& p2 )
{
   return p1.x == p2.x && p1.y == p2.y;
}

/*!
 * Returns true if a point \a p1 is equal to a scalar \a d2. A point \p is
 * equal to a scalar \a d if both coordinates of \a p are equal to \a d.
 * \ingroup point_functions_2d
 */
template <typename T> inline
bool operator ==( const GenericPoint<T>& p1, T d2 )
{
   return p1.x == d2 && p1.y == d2;
}

/*!
 * Returns true if a scalar \a d1 is equal to a point \a p2. A scalar \a d is
 * equal to a point \a p if both coordinates of \a p are equal to \a d.
 * \ingroup point_functions_2d
 */
template <typename T> inline
bool operator ==( T d1, const GenericPoint<T>& p2 )
{
   return d1 == p2.x && d1 == p2.y;
}

/*!
 * Returns true if a point \a p1 is less than another point \a p2.
 *
 * Points in the plane are sorted by ordinates in the PCL. Given two points
 * \a p and \a q in the plane, it holds that:
 *
 * \a p < \a q if \a p.y < \a q.y or \a p.y == \a q.y and \a p.x < \a q.x
 *
 * \ingroup point_functions_2d
 */
template <typename T1, typename T2> inline
bool operator <( const GenericPoint<T1>& p1, const GenericPoint<T2>& p2 )
{
   return p1.y < p2.y || p1.y == p2.y && p1.x < p2.x;
}

/*!
 * Returns true if a point \a p1 is less than a scalar \a d2.
 *
 * Given a point \a p in the plane and a scalar \a d, it holds that:
 *
 * \a p < \a d if \a p.y < \a d or \a p.y == \a d and \a p.x < \a d
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
bool operator <( const GenericPoint<T>& p1, T d2 )
{
   return p1.y < d2 || p1.y == d2 && p1.x < d2;
}

/*!
 * Returns true if a scalar \a d1 is less than a point \a p2.
 *
 * Given a point \a p in the plane and a scalar \a d, it holds that:
 *
 * \a d < \a p if \a d < \a p.y or \a d == \a p.y and \a d < \a p.x
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
bool operator <( T d1, const GenericPoint<T>& p2 )
{
   return d1 < p2.y || d1 == p2.y && d1 < p2.x;
}

/*!
 * Adds two points \a p1 and \a p2, and returns the resulting sum point.
 *
 * Point addition consists in adding homonym coordinates. Given two points
 * \a p and \a q in the plane, the sum point \a s = \a p + \a q is given by:
 *
 * \a s.x = \a p.x + \a q.x \n
 * \a s.y = \a p.y + \a q.y
 *
 * \ingroup point_functions_2d
 */
template <typename T1, typename T2> inline
GenericPoint<T1> operator +( const GenericPoint<T1>& p1, const GenericPoint<T2>& p2 )
{
   return GenericPoint<T1>( T1( p1.x + p2.x ), T1( p1.y + p2.y ) );
}

/*!
 * Adds a point \a p1 and a scalar \a d2, and returns the resulting sum point.
 *
 * Given a point \a p in the plane and a scalar \a d, the sum point
 * \a s = \a p + \a d is given by:
 *
 * \a s.x = \a p.x + \a d \n
 * \a s.y = \a p.y + \a d
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
GenericPoint<T> operator +( const GenericPoint<T>& p1, T d2 )
{
   return GenericPoint<T>( p1.x+d2, p1.y+d2 );
}

/*!
 * Adds a scalar \a d1 and a point \a p2, and returns the resulting sum point.
 *
 * Point addition is a commutative operation, thus see
 * pcl::operator +( const GenericPoint<T>&, T ).
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
GenericPoint<T> operator +( T d1, const GenericPoint<T>& p2 )
{
   return GenericPoint<T>( p2.x+d1, p2.y+d1 );
}

/*!
 * Subtracts two points \a p1 and \a p2, and returns the resulting difference
 * point.
 *
 * Point subtraction consists in subtracting homonym coordinates. Given two
 * points \a p and \a q in the plane, the difference point \a r = \a p - \a q
 * is given by:
 *
 * \a r.x = \a p.x - \a q.x \n
 * \a r.y = \a p.y - \a q.y
 *
 * \ingroup point_functions_2d
 */
template <typename T1, typename T2> inline
GenericPoint<T1> operator -( const GenericPoint<T1>& p1, const GenericPoint<T2>& p2 )
{
   return GenericPoint<T1>( T1( p1.x - p2.x ), T1( p1.y - p2.y ) );
}

/*!
 * Subtracts a scalar \a d2 from a point \a p1, and returns the resulting
 * difference point.
 *
 * Given a point \a p in the plane and a scalar \a d, the difference point
 * \a r = \a p - \a d is given by:
 *
 * \a r.x = \a p.x - \a d \n
 * \a r.y = \a p.y - \a d
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
GenericPoint<T> operator -( const GenericPoint<T>& p1, T d2 )
{
   return GenericPoint<T>( p1.x-d2, p1.y-d2 );
}

/*!
 * Subtracts a point \a p2 from a scalar \a d1, and returns the resulting
 * difference point.
 *
 * Given a point \a p in the plane and a scalar \a d, the difference point
 * \a r = \a d - \a p is given by:
 *
 * \a r.x = \a d - \a p.x \n
 * \a r.y = \a d - \a p.y
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
GenericPoint<T> operator -( T d1, const GenericPoint<T>& p2 )
{
   return GenericPoint<T>( d1-p2.x, d1-p2.y );
}

/*!
 * Multiplies two points \a p1 and \a p2, and returns the resulting product
 * point.
 *
 * Point multiplication consists in multiplying homonym coordinates. Given two
 * points \a p and \a q in the plane, the product point \a P = \a p * \a q is
 * given by:
 *
 * \a P.x = \a p.x * \a q.x \n
 * \a P.y = \a p.y * \a q.y
 *
 * \ingroup point_functions_2d
 */
template <typename T1, typename T2> inline
GenericPoint<T1> operator *( const GenericPoint<T1>& p1, const GenericPoint<T2>& p2 )
{
   return GenericPoint<T1>( T1( p1.x * p2.x ), T1( p1.y * p2.y ) );
}

/*!
 * Multiplies a point \a p1 by a scalar \a d2, and returns the resulting
 * product point.
 *
 * Given a point \a p in the plane and a scalar \a d, the product point
 * \a P = \a p * \a d is given by:
 *
 * \a P.x = \a p.x * \a d \n
 * \a P.y = \a p.y * \a d
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
GenericPoint<T> operator *( const GenericPoint<T>& p1, T d2 )
{
   return GenericPoint<T>( p1.x*d2, p1.y*d2 );
}

/*!
 * Multiplies a scalar \a d1 by a point \a p2, and returns the resulting
 * product point.
 *
 * Point multiplication is a commutative operation, thus see
 * pcl::operator *( const GenericPoint<T>&, T ).
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
GenericPoint<T> operator *( T d1, const GenericPoint<T>& p2 )
{
   return GenericPoint<T>( p2.x*d1, p2.y*d1 );
}

/*!
 * Divides two points \a p1 and \a p2, and returns the resulting quotient point.
 *
 * Given two points \a p and \a q in the plane, the quotient point
 * \a Q = \a p / \a q is given by:
 *
 * \a Q.x = \a p.x / \a q.x \n
 * \a Q.y = \a p.y / \a q.y
 *
 * \ingroup point_functions_2d
 */
template <typename T1, typename T2> inline
GenericPoint<T1> operator /( const GenericPoint<T1>& p1, const GenericPoint<T2>& p2 )
{
   PCL_PRECONDITION( p2.x != T2( 0 ) && p2.y != T2( 0 ) )
   return GenericPoint<T1>( T1( p1.x / p2.x ), T1( p1.y / p2.y ) );
}

/*!
 * Divides a point \a p1 by a scalar \a d2, and returns the resulting
 * quotient point.
 *
 * Given a point \a p in the plane and a scalar \a d, the quotient point
 * \a Q = \a p / \a d is given by:
 *
 * \a Q.x = \a p.x / \a d \n
 * \a Q.y = \a p.y / \a d
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
GenericPoint<T> operator /( const GenericPoint<T>& p1, T d2 )
{
   PCL_PRECONDITION( d2 != T( 0 ) )
   return GenericPoint<T>( p1.x/d2, p1.y/d2 );
}

/*!
 * Divides a scalar \a d1 by a point \a p2, and returns the resulting
 * quotient point.
 *
 * Given a point \a p in the plane and a scalar \a d, the quotient point
 * \a Q = \a d / \a q is given by:
 *
 * \a Q.x = \a d / \a p.x \n
 * \a Q.y = \a d / \a p.y
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
GenericPoint<T> operator /( T d1, const GenericPoint<T>& p2 )
{
   PCL_PRECONDITION( p2.x != T( 0 ) && p2.y != T( 0 ) )
   return GenericPoint<T>( d1/p2.x, d1/p2.y );
}

/*!
 * Returns the distance between two points \a p1 and \a p2 in the plane.
 * \ingroup point_functions_2d
 */
// ### FIXME: We cannot use two template arguments here due to a conflict with
//            Distance( FI, FI ) defined in Iterator.h
//template <typename T1, typename T2> inline
template <typename T> inline
double Distance( const GenericPoint<T>& p1, const GenericPoint<T>& p2 )
{
   register double dx = p2.x - p1.x;
   register double dy = p2.y - p1.y;
   return pcl::Sqrt( dx*dx + dy*dy );
}

/*!
 * Returns the Manhattan distance between two points \a p1 and \a p2 in the
 * plane.
 * \ingroup point_functions_2d
 */
template <typename T1, typename T2> inline
double ManhattanDistance( const GenericPoint<T1>& p1, const GenericPoint<T2>& p2 )
{
   return Abs( p2.x - p1.x ) + Abs( p2.y - p1.y );
}

/*!
 * Rotates a point in the plane, given a rotation angle and the coordinates of
 * a center of rotation.
 *
 * \param p    Reference to a point that will be rotated.
 * \param a    Rotation angle in radians. Positive angles are measured
 *             counter-clockwise.
 * \param xc   Abscissa (x coordinate) of the center of rotation.
 * \param yc   Ordinate (y coordinate) of the center of rotation.
 *
 * The coordinates \a p.x and \a p.y are replaced by their corresponding
 * rotated values.
 *
 * \ingroup point_functions_2d
 */
template <typename T, typename T1, typename T2> inline
void Rotate( GenericPoint<T>& p, T1 a, T2 xc, T2 yc )
{
   pcl::Rotate( p.x, p.y, a, xc, yc );
}

/*!
 * Rotates a point in the plane, given a rotation angle and a rotation center
 * point.
 *
 * \param p    Reference to a point that will be rotated.
 * \param a    Rotation angle in radians. Positive angles are measured
 *             counter-clockwise.
 * \param c    Reference to a point that will be taken as the center of
 *             rotation.
 *
 * The coordinates \a p.x and \a p.y are replaced by their corresponding
 * rotated values.
 *
 * \ingroup point_functions_2d
 */
template <typename T, typename T1, typename T2> inline
void Rotate( GenericPoint<T>& p, T1 a, const GenericPoint<T2>& c )
{
   pcl::Rotate( p, a, c.x, c.y );
}

/*!
 * Rotates a point in the plane, given a rotation angle by its sine and
 * cosine, and the coordinates of a center of rotation.
 *
 * \param p    Reference to a point that will be rotated.
 * \param sa   Sine of the rotation angle.
 * \param ca   Cosine of the rotation angle.
 * \param xc   Abscissa (x coordinate) of the center of rotation.
 * \param yc   Ordinate (y coordinate) of the center of rotation.
 *
 * The coordinates \a p.x and \a p.y are replaced by their corresponding
 * rotated values.
 *
 * \ingroup point_functions_2d
 */
template <typename T, typename T1, typename T2> inline
void Rotate( GenericPoint<T>& p, T1 sa, T1 ca, T2 xc, T2 yc )
{
   pcl::Rotate( p.x, p.y, sa, ca, xc, yc );
}

/*!
 * Rotates a point in the plane, given a rotation angle by its sine and
 * cosine, and a rotation center point.
 *
 * \param p    Reference to a point that will be rotated.
 * \param sa   Sine of the rotation angle.
 * \param ca   Cosine of the rotation angle.
 * \param c    Reference to a point that will be taken as the center of
 *             rotation.
 *
 * The coordinates \a p.x and \a p.y are replaced by their corresponding
 * rotated values.
 *
 * \ingroup point_functions_2d
 */
template <typename T, typename T1, typename T2> inline
void Rotate( GenericPoint<T>& p, T1 sa, T1 ca, const GenericPoint<T2>& c )
{
   pcl::Rotate( p, sa, ca, c.x, c.y );
}

/*!
 * Exchanges two points \a p1 and \a p2 in the plane. Calling this function is
 * equivalent to:
 *
 * \code Swap( p1.x, p2.x ); Swap( p1.y, p2.y ); \endcode
 *
 * \ingroup point_functions_2d
 */
template <typename T> inline
void Swap( GenericPoint<T>& p1, GenericPoint<T>& p2 )
{
   pcl::Swap( p1.x, p2.x ); pcl::Swap( p1.y, p2.y );
}

// ----------------------------------------------------------------------------

#ifndef __PCL_NO_POINT_INSTANTIATE

/*!
 * \defgroup point_types_2d 2-D Point Types
 */

/*!
 * \class pcl::Point
 * \ingroup point_types_2d
 * \brief Discrete (or integer) point in the plane.
 *
 * Point is a template instantiation of GenericPoint for the \c int type.
 */
typedef GenericPoint<int>           Point;

/*!
 * \class pcl::FPoint
 * \ingroup point_types_2d
 * \brief 32-bit floating-point point in the R^2 space.
 *
 * FPoint is a template instantiation of GenericPoint for the \c float type.
 */
typedef GenericPoint<float>         FPoint;

/*!
 * \class pcl::DPoint
 * \ingroup point_types_2d
 * \brief 64-bit floating-point point in the R^2 space.
 *
 * DPoint is a template instantiation of GenericPoint for the \c double type.
 */
typedef GenericPoint<double>        DPoint;

#endif   // !__PCL_NO_POINT_INSTANTIATE

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_Point_h

// ****************************************************************************
// EOF pcl/Point.h - Released 2014/11/14 17:16:34 UTC
