//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// pcl/SurfaceSplines.h - Released 2016/02/21 20:22:12 UTC
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

#ifndef __PCL_SurfaceSplines_h
#define __PCL_SurfaceSplines_h

/// \file pcl/SurfaceSplines.h

#include <pcl/Array.h>
#include <pcl/BicubicInterpolation.h>
#include <pcl/Console.h>
#include <pcl/File.h>
#include <pcl/Matrix.h>
#include <pcl/Point.h>
#include <pcl/Rectangle.h>
#include <pcl/ReferenceArray.h>
#include <pcl/SurfaceSpline.h>
#include <pcl/Thread.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class SerializableSurfaceSpline
 * \brief An interpolating/approximating two-dimensional surface spline
 *        serializable as drizzle data (.drz format)
 */
template <typename T>
class SerializableSurfaceSpline : public SurfaceSpline<T>
{
public:

   /*!
    * Represents a vector of coordinates or spline coefficients.
    */
   typedef typename SurfaceSpline<T>::vector_type  vector_type;

   /*!
    * Default constructor.
    */
   SerializableSurfaceSpline() = default;

   /*!
    * Constructor from base class.
    */
   SerializableSurfaceSpline( const SurfaceSpline<T>& S ) :
      SurfaceSpline<T>( S )
   {
   }

   /*!
    * Virtual destructor.
    */
   virtual ~SerializableSurfaceSpline()
   {
   }

   /*!
    * Copy assignment from base class. Returns a reference to this object.
    */
   SerializableSurfaceSpline& operator =( const SurfaceSpline<T>& S )
   {
      return SurfaceSpline<T>::operator =( S );
   }

   /*!
    * Serializes this spline as drizzle integration data in .drz plain text
    * format.
    *
    * The generated data will be appended to the specified file, which must
    * allow write access, and will contain all node coordinates, node weights
    * (for an approximating spline only), reference coordinates, and spline
    * coefficients.
    */
   void ToDrizzleData( File& f ) const
   {
      if ( this->IsValid() )
      {
         f.OutText( "x{" );
         for ( int i = 0, n = this->m_x.Length()-1; ; )
         {
            f.OutText( IsoString().Format( "%.16g,", this->m_x[i] ) );
            if ( ++i == n )
            {
               f.OutText( IsoString().Format( "%.16g}", this->m_x[n] ) );
               break;
            }
         }
         f.OutText( "y{" );
         for ( int i = 0, n = this->m_y.Length()-1; ; )
         {
            f.OutText( IsoString().Format( "%.16g,", this->m_y[i] ) );
            if ( ++i == n )
            {
               f.OutText( IsoString().Format( "%.16g}", this->m_y[n] ) );
               break;
            }
         }
         f.OutText( IsoString().Format( "r0{%.16g}", this->m_r0 ) );
         f.OutText( IsoString().Format( "x0{%.16g}", this->m_x0 ) );
         f.OutText( IsoString().Format( "y0{%.16g}", this->m_y0 ) );
         f.OutText( IsoString().Format( "m{%d}", this->m_order ) );
         f.OutText( IsoString().Format( "r{%.8g}", this->m_smoothing ) );
         if ( this->m_smoothing > 0 )
            if ( !this->m_weights.IsEmpty() )
            {
               f.OutText( "w{" );
               for ( int i = 0, n = this->m_weights.Length()-1; ; )
               {
                  f.OutText( IsoString().Format( "%.8g,", this->m_weights[i] ) );
                  if ( ++i == n )
                  {
                     f.OutText( IsoString().Format( "%.8g}", this->m_weights[n] ) );
                     break;
                  }
               }
            }
         f.OutText( "s{" );
         for ( int i = 0, n = this->m_spline.Length()-1; ; )
         {
            f.OutText( IsoString().Format( "%.16g,", this->m_spline[i] ) );
            if ( ++i == n )
            {
               f.OutText( IsoString().Format( "%.16g}", this->m_spline[n] ) );
               break;
            }
         }
      }
   }

   friend class DrizzleSplineDecoder;
};

// ----------------------------------------------------------------------------

/*!
 * \class PointSurfaceSpline
 * \brief Vector surface spline interpolation/approximation in two dimensions
 *
 * The template parameter P represents an interpolation point in two
 * dimensions. The type P must implement 'x' and 'y' data members accessible
 * from the current %PointSurfaceSpline template specialization. These members
 * must provide the values of the horizontal and vertical coordinates,
 * respectively, of an interpolation point. In addition, the scalar types of
 * the x and y point members must support conversion to double semantics.
 */
template <class P = FPoint>
class PointSurfaceSpline
{
public:

   /*!
    * Represents an interpolation point in two dimensions.
    */
   typedef P                                 point;

   /*!
    * Represents a sequence of interpolation points.
    */
   typedef Array<point>                      point_list;

   /*!
    * Represents a coordinate interpolating/approximating surface spline.
    */
   typedef SerializableSurfaceSpline<double> spline;

   /*!
    * Default constructor. Yields an empty instance that cannot be used without
    * initialization.
    */
   PointSurfaceSpline() = default;

   /*!
    * Copy constructor.
    */
   PointSurfaceSpline( const PointSurfaceSpline& ) = default;

   /*!
    * Move constructor.
    */
#ifndef _MSC_VER
   PointSurfaceSpline( PointSurfaceSpline&& ) = default;
#endif

   /*!
    * Constructs a %PointSurfaceSpline object initialized for the specified
    * input data and interpolation parameters.
    *
    * See the Initialize() member function for a detailed description of
    * parameters.
    */
   PointSurfaceSpline( const point_list& P1, const point_list& P2, float smoothness = 0, int order = 2 )
   {
      Initialize( P1, P2, smoothness, order );
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   PointSurfaceSpline& operator =( const PointSurfaceSpline& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
#ifndef _MSC_VER
   PointSurfaceSpline& operator =( PointSurfaceSpline&& ) = default;
#endif

   /*!
    * Initializes this %PointSurfaceSpline object for the specified input data
    * and interpolation parameters.
    *
    * \param P1         A sequence of distinct interpolation node points.
    *
    * \param P2         A sequence of interpolation values. For each point in
    *                   \a P1, the coordinates of its counterpart point in
    *                   \a P2 will be used as the interpolation node values in
    *                   the X and Y directions.
    *
    * \param smoothness Smoothing factor. Must be >= 0. The default value is 0.
    *
    * \param W          Reference to a vector of positive node \e weights > 0,
    *                   when the smoothing factor is > 1. For an interpolation
    *                   spline, this parameter will be ignored (see below).
    *
    * \param order      Derivative order. Must be >= 1. The default value is 2.
    *
    * For \a smoothness <= 0, an interpolating spline will be generated: all
    * node values will be reproduced exactly at their respective coordinates.
    * In this case the \a W parameter will be ignored.
    *
    * For \a smoothness > 0, a smoothing (or approximating) spline will be
    * generated: increasing \a smoothness values will generate splines closer
    * to the reference plane of the input node set. If nonempty, the specified
    * vector \a W of node weights will be used to assign a different
    * interpolation \e strength to each interpolation node. In this case the
    * vector \a W must have at least the same length as the shortest of the
    * \a P1 and \a P2 vectors, and must contain values greater than zero. A
    * node weight larger than one will reduce the smoothness of the
    * interpolating surface at the corresponding node coordinates, or in other
    * words, it will give more prominence to the corresponding data point. A
    * node weight of one will apply the specified \a smoothness at its node
    * position. Contrarily, a node weight smaller than one will increase the
    * interpolation smoothness.
    *
    * The surface spline will be continuously differentiable up to the
    * specified \a order. If this order is too high, an ill-conditioned
    * linear system may result, especially for large data sets. The recommended
    * values are 2, 3, 4 or 5, but order 2 is usually the best and safest
    * option for most applications. Derivative orders greater than 3 may yield
    * numerically unstable interpolation devices, which should always be used
    * with care.
    */
   void Initialize( const point_list& P1, const point_list& P2,
                    float smoothness = 0, const FVector& W = FVector(), int order = 2 )
   {
      PCL_PRECONDITION( P1.Length() >= 3 )
      PCL_PRECONDITION( P1.Length() <= P2.Length() )
      PCL_PRECONDITION( order >= 2 )

      m_Sx.Clear();
      m_Sy.Clear();

      m_Sx.SetOrder( order );
      m_Sy.SetOrder( order );

      m_Sx.SetSmoothing( smoothness );
      m_Sy.SetSmoothing( smoothness );

      if ( P1.Length() < 3 || P2.Length() < 3 )
         return;

      DVector X( P1.Length() ),
              Y( P1.Length() ),
              Zx( P1.Length() ),
              Zy( P1.Length() );
      for ( int i = 0; i < X.Length(); ++i )
      {
          X[i] = P1[i].x;
          Y[i] = P1[i].y;
         Zx[i] = P2[i].x;
         Zy[i] = P2[i].y;
      }
      m_Sx.Initialize( X.Begin(), Y.Begin(), Zx.Begin(), X.Length(), W.Begin() );
      m_Sy.Initialize( X.Begin(), Y.Begin(), Zy.Begin(), X.Length(), W.Begin() );
   }

   /*!
    * Returns an approximation to the inverse surface spline of this object.
    *
    * The returned object can be used to perform an inverse interpolation:
    * Given an interpolation point P2, the returned spline will interpolate the
    * corresponding node point P1. See Initialize() for more information on
    * spline initialization.
    *
    * In general, the returned object can only provide an approximation to the
    * inverse of the underlying coordinate transformation. In particular, if
    * this object has been initialized as an approximating surface spline, its
    * inverse spline will compute node point coordinates from approximate
    * (smoothed) interpolated coordinates, instead of the original ones.
    *
    * If two or more interpolation points were identical when this object was
    * initialized, calling this member function may lead to an ill-conditioned
    * linear system. In such case, an Error exception will be thrown.
    *
    * If this object has not been initialized, this function returns an
    * uninitialized %PointSurfaceSpline object.
    */
   PointSurfaceSpline Inverse() const
   {
      PointSurfaceSpline inverse;
      if ( IsValid() )
      {
         DVector X = m_Sx.X(),
                 Y = m_Sx.Y(),
                 Zx( X.Length() ),
                 Zy( X.Length() );
         for ( int i = 0; i < X.Length(); ++i )
         {
            Zx[i] = m_Sx( X[i], Y[i] );
            Zy[i] = m_Sy( X[i], Y[i] );
         }
         inverse.m_Sx.SetOrder( m_Sx.Order() );
         inverse.m_Sy.SetOrder( m_Sy.Order() );
         inverse.m_Sx.Initialize( Zx.Begin(), Zy.Begin(), X.Begin(), X.Length() );
         inverse.m_Sy.Initialize( Zx.Begin(), Zy.Begin(), Y.Begin(), X.Length() );
      }
      return inverse;
   }

   /*!
    * Deallocates internal structures, yielding an empty spline that cannot be
    * used before a new call to Initialize().
    */
   void Clear()
   {
      m_Sx.Clear();
      m_Sy.Clear();
   }

   /*!
    * Returns true iff this is a valid, initialized object ready for
    * interpolation.
    */
   bool IsValid() const
   {
      return m_Sx.IsValid() && m_Sy.IsValid();
   }

   /*!
    * Serializes this point spline as drizzle integration data in .drz plain
    * text format.
    *
    * The generated data will be appended to the specified file, which must
    * allow write access, and will contain all node coordinates, node weights
    * (for an approximating spline only), reference coordinates, and spline
    * coefficients in the X and Y directions.
    */
   void ToDrizzleData( File& f ) const
   {
      if ( IsValid() )
      {
         f.OutText( "Sx{" );
         m_Sx.ToDrizzleData( f );
         f.OutText( "}Sy{" );
         m_Sy.ToDrizzleData( f );
         f.OutText( "}" );
      }
   }

   /*!
    * Returns an interpolated point at the specified coordinates.
    */
   template <typename T>
   DPoint operator ()( T x, T y ) const
   {
      return DPoint( m_Sx( x, y ), m_Sy( x, y ) );
   }

   /*!
    * Returns an interpolated point at the \a p.x and \a p.y coordinates.
    */
   template <typename T>
   DPoint operator ()( const GenericPoint<T>& p ) const
   {
      return operator ()( p.x, p.y );
   }

private:

   spline m_Sx, m_Sy;   // the surface splines in the X and Y directions.

   friend class DrizzleDataDecoder;
};

// ----------------------------------------------------------------------------

/*!
 * \class PointGridInterpolation
 * \brief Discretized vector surface spline interpolation/approximation in two
 *        dimensions
 *
 * This class performs the same tasks as PointSurfaceSpline, but allows for
 * much faster point interpolation (maybe orders of magnitude faster, depending
 * on interpolation vector lengths) with negligible accuracy loss in most
 * practical applications.
 */
class PointGridInterpolation
{
public:

   /*!
    * Default constructor. Yields an empty instance that cannot be used without
    * initialization.
    */
   PointGridInterpolation() : m_parallel( true )
   {
   }

   /*!
    * Copy constructor.
    */
   PointGridInterpolation( const PointGridInterpolation& ) = default;

   /*!
    * Move constructor.
    */
#ifndef _MSC_VER
   PointGridInterpolation( PointGridInterpolation&& ) = default;
#endif

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   PointGridInterpolation& operator =( const PointGridInterpolation& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
#ifndef _MSC_VER
   PointGridInterpolation& operator =( PointGridInterpolation&& ) = default;
#endif

   /*!
    * Initializes this %PointGridInterpolation object for the specified input data
    * and interpolation parameters.
    *
    * \param rect    Reference rectangle. Interpolation will be initialized
    *                within the boundaries of this rectangle at discrete
    *                \a delta coordinate intervals.
    *
    * \param delta   Grid distance for calculation of discrete function values.
    *                Must be > 0.
    *
    * \param S       Reference to a PointSurfaceSpline object that will be used
    *                as the underlying interpolation to compute interpolation
    *                values at discrete coordinate intervals. This object must
    *                be previously initialized and must be valid.
    *
    * \param verbose If true, this function will write information to the
    *                standard PixInsight console to provide some feedback to
    *                the user during the (potentially long) initialization
    *                process. If false, no feedback will be provided.
    *
    * If parallel processing is allowed for this object, this function executes
    * the interpolation initialization process using multiple concurrent
    * threads (see EnableParallelProcessing()).
    */
   template <class P>
   void Initialize( const Rect& rect, int delta, const PointSurfaceSpline<P>& S, bool verbose = true )
   {
      m_rect = rect;
      m_delta = delta;

      int w = rect.Width();
      int h = rect.Height();
      int rows = 1 + h/m_delta + ((h%m_delta) ? 1 : 0);
      int cols = 1 + w/m_delta + ((w%m_delta) ? 1 : 0);

      m_Gx = DMatrix( rows, cols );
      m_Gy = DMatrix( rows, cols );

      if ( verbose )
         Console().WriteLn( "<end><cbr>Building 2D surface interpolation grid...<flush>" );

      int numberOfThreads = m_parallel ? Thread::NumberOfThreads( rows, 1 ) : 1;
      int rowsPerThread = rows/numberOfThreads;
      ReferenceArray<GridInitializationThread<P> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new GridInitializationThread<P>( *this, S,
                                                       i*rowsPerThread,
                                                       (j < numberOfThreads) ? j*rowsPerThread : rows ) );
      for ( int i = 0; i < numberOfThreads; ++i )
         threads[i].Start( ThreadPriority::DefaultMax, i );
      for ( int i = 0; i < numberOfThreads; ++i )
         threads[i].Wait();
      threads.Destroy();

      m_Ix.Initialize( m_Gx.Begin(), cols, rows );
      m_Iy.Initialize( m_Gy.Begin(), cols, rows );
   }

   /*!
    * Returns true iff this is a valid, initialized object ready for
    * interpolation.
    */
   bool IsValid() const
   {
      return !(m_Gx.IsEmpty() || m_Gy.IsEmpty());
   }

   /*!
    * Returns the current interpolation reference rectangle. See Initialize()
    * for more information.
    */
   const Rect& ReferenceRect() const
   {
      return m_rect;
   }

   /*!
    * Returns the current grid distance for calculation of discrete function
    * values. See Initialize() for more information.
    */
   int Delta() const
   {
      return m_delta;
   }

   /*!
    * Returns an interpolated point at the specified coordinates.
    */
   template <typename T>
   DPoint operator ()( T x, T y ) const
   {
      double fx = (double( x ) - m_rect.x0)/m_delta;
      double fy = (double( y ) - m_rect.y0)/m_delta;
      return DPoint( m_Ix( fx, fy ), m_Iy( fx, fy ) );
   }

   /*!
    * Returns an interpolated point at the \a p.x and \a p.y coordinates.
    */
   template <typename T>
   DPoint operator ()( const GenericPoint<T>& p ) const
   {
      return operator ()( p.x, p.y );
   }

   /*!
    * Returns true iff this object is allowed to use multiple parallel
    * execution threads (when multiple threads are permitted and available).
    */
   bool IsParallelProcessingEnabled() const
   {
      return m_parallel;
   }

   /*!
    * Enables parallel processing for this instance.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * Parallel processing is applied during the interpolation initialization
    * process (see the Initialize() member function).
    */
   void EnableParallelProcessing( bool enable ) // ### TODO: Add a maxProcessors parameter
   {
      m_parallel = enable;
   }

   /*!
    * Disables parallel processing for this instance.
    *
    * This is a convenience function, equivalent to:
    * EnableParallelProcessing( !disable )
    */
   void DisableParallelProcessing( bool disable )
   {
      EnableParallelProcessing( !disable );
   }

private:

   typedef BicubicSplineInterpolation<double> grid_interpolation;

   Rect               m_rect;
   int                m_delta;
   DMatrix            m_Gx, m_Gy;
   grid_interpolation m_Ix, m_Iy;
   bool               m_parallel : 1;

   template <class P>
   class GridInitializationThread : public Thread
   {
   public:

      typedef PointSurfaceSpline<P> point_interpolation;

      GridInitializationThread( PointGridInterpolation& grid, const point_interpolation& splines, int startRow, int endRow ) :
         m_grid( grid ), m_splines( splines ), m_startRow( startRow ), m_endRow( endRow )
      {
      }

      virtual PCL_HOT_FUNCTION void Run()
      {
         for ( int i = m_startRow; i < m_endRow; ++i )
            for ( int j = 0, dx = 0, y = m_grid.m_rect.y0 + i*m_grid.m_delta; j < m_grid.m_Gx.Cols(); ++j, dx += m_grid.m_delta )
            {
               DPoint p = m_splines( m_grid.m_rect.x0 + dx, y );
               m_grid.m_Gx[i][j] = p.x;
               m_grid.m_Gy[i][j] = p.y;
            }
      }

   private:

      PointGridInterpolation&    m_grid;
      const point_interpolation& m_splines;
      int                        m_startRow, m_endRow;
   };
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_SurfaceSplines_h

// ----------------------------------------------------------------------------
// EOF pcl/SurfaceSplines.h - Released 2016/02/21 20:22:12 UTC
