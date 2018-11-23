//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// pcl/ShepardInterpolation.h - Released 2018-11-23T16:14:19Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_ShepardInterpolation_h
#define __PCL_ShepardInterpolation_h

/// \file pcl/ShepardInterpolation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/BicubicInterpolation.h>
#include <pcl/ParallelProcess.h>
#include <pcl/QuadTree.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ShepardInterpolation
 * \brief Two-dimensional surface interpolation with the local Shepard method
 *
 * %ShepardInterpolation implements the Shepard method of function
 * interpolation/approximation for arbitrarily distributed input nodes in two
 * dimensions.
 *
 * This class implements local Shepard interpolation with Franke-Little
 * weights, quadtree structures for fast rectangular search of input nodes, and
 * an adaptive local interpolation search routine.
 *
 * \b References
 *
 * Shepard, Donald (1968). <em>A two-dimensional interpolation function for
 * irregularly-spaced data</em>. Proceedings of the 1968 ACM National
 * Conference, pp. 517-524.
 *
 * Franke, Richard (1982). <em>Scattered data interpolation: tests of some
 * methods</em>. Mathematics of Computation 38 (1982), pp. 181-200.
 *
 * \sa ShepardInterpolation, SurfacePolynomial
 */
template <typename T>
class PCL_CLASS ShepardInterpolation
{
public:

   /*!
    * Represents a vector of coordinates or function values.
    */
   typedef GenericVector<T>               vector_type;

   /*!
    * The numeric type used to represent coordinates and function values.
    */
   typedef typename vector_type::scalar   scalar;

   /*!
    * The class used to implement fast coordinate search operations.
    */
   typedef QuadTree<vector_type>          search_tree;

   /*!
    * Default constructor. Constructs an empty %ShepardInterpolation object.
    */
   ShepardInterpolation() = default;

   /*!
    * Copy constructor. Copy construction is disabled because this class uses
    * internal data structures that cannot be copy-constructed.
    */
   ShepardInterpolation( const ShepardInterpolation& ) = delete;

   /*!
    * Move constructor.
    */
   ShepardInterpolation( ShepardInterpolation&& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~ShepardInterpolation()
   {
   }

   /*!
    * Copy assignment operator. Copy assignment is disabled because this class
    * uses internal data structures that cannot be copy-assigned.
    */
   ShepardInterpolation& operator =( const ShepardInterpolation& ) = delete;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   ShepardInterpolation& operator =( ShepardInterpolation&& ) = default;

   /*!
    * Returns true iff this object is valid. A valid %ShepardInterpolation
    * object has been initialized with a sufficient number of input nodes.
    */
   bool IsValid() const
   {
      return !m_T.IsEmpty();
   }

   /*!
    * Sets the <em>power parameter</em> of the local Shepard interpolation.
    *
    * \param m Power parameter. Must be > 0.
    *
    * The power parameter is a positive real > 0 that defines the behavior of
    * the interpolation/approximation function. For large values of \a m, the
    * interpolating surface tends to be uniform within boundaries defined
    * around input nodes, and hence is more local. For values of \a m &le; 2,
    * the surface is more global, that is, interpolated values are more
    * influenced by nodes far away from the interpolation coordinates. The
    * default power parameter value is 2.
    *
    * If the specified value of \a m is invalid (&le; 0), the default value
    * \a m = 2 will be set.
    *
    * Calling this member function does not reset this %ShepardInterpolation
    * object, since no internal structures built upon initialization depend on
    * the power parameter. This facilitates the use of this class to compare
    * the results of different power parameter values applied to the same data.
    */
   void SetPower( int m )
   {
      PCL_PRECONDITION( m > 0 )
      m_mu = Max( 1, m );
   }

   /*!
    * Returns the current power parameter of this local Shepard interpolation.
    *
    * See SetPower() for more information.
    */
   int Power() const
   {
      return m_mu;
   }

   /*!
    * Sets the search radius of the local Shepard interpolation.
    *
    * \param R Search radius in the range (0,1].
    *
    * The search radius defines a distance from the interpolation point where
    * input nodes will be used to compute an interpolated function value.
    * Larger values of \a R will construct more global interpolation surfaces,
    * while smaller values will tend to generate more local interpolations.
    * Smaller search radii will also lead to faster interpolation devices,
    * since the the computational complexity is reduced as the number of input
    * nodes used for each interpolation point decreases.
    *
    * The search radius parameter is normalized to the (0,1] range in this
    * implementation, where 1 represents the largest distance between two
    * input nodes, that is, the size of the entire interpolation region. The
    * default search radius is 0.2.
    *
    * If the specified value of \a R is invalid (&le; 0), the default value
    * \a R = 0.2 will be set.
    *
    * Calling this member function does not reset this %ShepardInterpolation
    * object, since no internal structures built upon initialization depend on
    * the search radius. This facilitates the use of this class to compare the
    * results of different search radius values applied to the same data.
    */
   void SetRadius( double R )
   {
      PCL_PRECONDITION( R > 0 )
      m_R = (R < 0 || 1 + R == 1) ? 0.2 : R;
   }

   /*!
    * Returns the current search radius of this local Shepard interpolation.
    *
    * See SetRadius() for more information.
    */
   double Radius() const
   {
      return m_R;
   }

   /*!
    * Generation of a two-dimensional surface polynomial.
    *
    * \param x       X node coordinates.
    *
    * \param y       Y node coordinates.
    *
    * \param z       Node values.
    *
    * \param n       Number of nodes. Must be >= 3.
    *
    * The input nodes can be arbitrarily distributed, and they don't need to
    * follow any specific order. However, all nodes must be distinct with
    * respect to the machine epsilon for the floating point type T.
    */
   void Initialize( const T* x, const T* y, const T* z, int n )
   {
      PCL_PRECONDITION( x != nullptr && y != nullptr && z != nullptr )
      PCL_PRECONDITION( n > 2 )

      if ( n < 3 )
         throw Error( "At least three input nodes are required in ShepardInterpolation::Initialize()" );

      Clear();

      // Find mean coordinate values
      m_x0 = m_y0 = 0;
      for ( int i = 0; i < n; ++i )
      {
         m_x0 += x[i];
         m_y0 += y[i];
      }
      m_x0 /= n;
      m_y0 /= n;

      // Find radius of unit circle
      m_r0 = 0;
      for ( int i = 0; i < n; ++i )
      {
         double dx = m_x0 - x[i];
         double dy = m_y0 - y[i];
         double r = Sqrt( dx*dx + dy*dy );
         if ( r > m_r0 )
            m_r0 = r;
      }
      if ( 1 + m_r0 == 1 )
         throw Error( "ShepardInterpolation::Initialize(): Empty or insignificant interpolation space" );

      m_r0 = 1/m_r0;

      // Transform coordinates to unit circle
      Array<vector_type> V;
      for ( int i = 0; i < n; ++i )
         V << vector_type( m_r0*(x[i] - m_x0), m_r0*(y[i] - m_y0), z[i] );

      // Build the search tree
      m_T.Build( V, 16 );
   }

   /*!
    * Two-dimensional surface interpolation/approximation with the local
    * Shepard method. Returns an interpolated function value at the specified
    * \a x and \a y coordinates.
    *
    * The interpolation function includes an adaptive search routine. The
    * specified search radius is used as an initial parameter. If less than
    * three input nodes are found within the search radius distance from the
    * desired interpolation point, the radius is increased and a new search is
    * performed. This is repeated until at least three nodes are found. This
    * improves the use of this class for extrapolation, for example to replace
    * zero or undefined pixel sample values at the borders of an image with
    * statistically plausible values.
    */
   T operator ()( double x, double y ) const
   {
      PCL_PRECONDITION( !m_T.IsEmpty() )

      const double dx = m_r0*(x - m_x0);
      const double dy = m_r0*(y - m_y0);

      for ( double R = m_R; ; R += R )
      {
         int m = 0;
         double R2 = R*R, W = 0, z = 0;
         m_T.Search( DRect( dx-R, dy-R, dx+R, dy+R ),
            [&]( const vector_type& v, void* )
            {
               double x = dx - v[0];
               double y = dy - v[1];
               double r2 = x*x + y*y;
               if ( r2 < R2 )
               {
                  ++m;
                  double w = 1 - Sqrt( r2 )/R;
                  for ( int i = 1; i < m_mu; ++i )
                     w *= w;
                  W += w;
                  z += w*v[2];
               }
            },
            nullptr );
         if ( m >= 3 )
            return T( z/W );
      }

      return 0; // !?
   }

   /*!
    * Resets this %ShepardInterpolation object, deallocating all internal
    * working structures.
    */
   void Clear()
   {
      m_T.Clear();
   }

protected:

   double      m_r0 = 1;   // scaling factor for normalization of node coordinates
   double      m_x0 = 0;   // zero offset for normalization of X node coordinates
   double      m_y0 = 0;   // zero offset for normalization of Y node coordinates
   int         m_mu = 2;   // power parameter (leveling factor)
   double      m_R  = 0.2; // initial search radius
   search_tree m_T;        // input coordinates and function values
};

// ----------------------------------------------------------------------------

/*!
 * \class GridShepardInterpolation
 * \brief Discretized scalar surface Shepard interpolation/approximation in two
 *        dimensions
 *
 * This class performs the same tasks as ShepardInterpolation, but allows for
 * much faster interpolation (maybe orders of magnitude faster, depending on
 * interpolation vector lengths) with negligible accuracy loss in most
 * practical applications.
 */
class GridShepardInterpolation : public ParallelProcess
{
public:

   /*!
    * Default constructor. Yields an empty instance that cannot be used without
    * initialization.
    */
   GridShepardInterpolation() = default;

   /*!
    * Copy constructor.
    */
   GridShepardInterpolation( const GridShepardInterpolation& ) = default;

   /*!
    * Move constructor.
    */
   GridShepardInterpolation( GridShepardInterpolation&& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   GridShepardInterpolation& operator =( const GridShepardInterpolation& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   GridShepardInterpolation& operator =( GridShepardInterpolation&& ) = default;

   /*!
    * Initializes this %GridShepardInterpolation object for the specified input
    * data and interpolation parameters.
    *
    * \param rect    Reference rectangle. Interpolation will be initialized
    *                within the boundaries of this rectangle at discrete
    *                \a delta coordinate intervals.
    *
    * \param delta   Grid distance for calculation of discrete function values.
    *                Must be > 0.
    *
    * \param S       Reference to a ShepardInterpolation object that will be
    *                used as the underlying interpolation to compute
    *                interpolated values at discrete coordinate intervals. This
    *                object must be previously initialized and must be valid.
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
   template <typename T>
   void Initialize( const Rect& rect, int delta, const ShepardInterpolation<T>& S, bool verbose = true )
   {
      m_rect = rect;
      m_delta = Max( 1, delta );

      int w = m_rect.Width();
      int h = m_rect.Height();
      int rows = 1 + h/m_delta + ((h%m_delta) ? 1 : 0);
      int cols = 1 + w/m_delta + ((w%m_delta) ? 1 : 0);

      m_G = DMatrix( rows, cols );

      if ( verbose )
         Console().WriteLn( "<end><cbr>Building 2D Shepard interpolation grid...<flush>" );

      int numberOfThreads = m_parallel ? Min( m_maxProcessors, Thread::NumberOfThreads( rows, 1 ) ) : 1;
      int rowsPerThread = rows/numberOfThreads;
      ReferenceArray<GridInitializationThread<T> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new GridInitializationThread<T>( *this, S,
                                 i*rowsPerThread,
                                 (j < numberOfThreads) ? j*rowsPerThread : rows ) );
      int n = 0;
      if ( numberOfThreads > 1 )
      {
         for ( GridInitializationThread<T>& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, n++ );
         for ( GridInitializationThread<T>& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      threads.Destroy();

      m_I.Initialize( m_G.Begin(), cols, rows );
   }

   /*!
    * Returns true iff this is a valid, initialized object ready for
    * interpolation.
    */
   bool IsValid() const
   {
      return !m_G.IsEmpty();
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
    * Returns an interpolated function value at the specified coordinates.
    */
   template <typename T>
   double operator ()( T x, T y ) const
   {
      double fx = (double( x ) - m_rect.x0)/m_delta;
      double fy = (double( y ) - m_rect.y0)/m_delta;
      return m_I( fx, fy );
   }

   /*!
    * Returns an interpolated function value at \a p.x and \a p.y coordinates.
    */
   template <typename T>
   double operator ()( const GenericPoint<T>& p ) const
   {
      return operator ()( p.x, p.y );
   }

private:

   /*!
    * N.B.: Here we need a smooth interpolation function without negative
    * lobes, in order to prevent small-scale oscillations. Other options are
    * BilinearInterpolation and CubicBSplineFilter.
    */
   typedef BicubicBSplineInterpolation<double> grid_interpolation;

   Rect               m_rect = Rect( 0 );
   int                m_delta = 0;
   DMatrix            m_G;
   grid_interpolation m_I;

   template <typename T>
   class GridInitializationThread : public Thread
   {
   public:

      typedef ShepardInterpolation<T> scalar_interpolation;

      GridInitializationThread( GridShepardInterpolation& grid, const scalar_interpolation& sephard, int startRow, int endRow ) :
         m_grid( grid ), m_sephard( sephard ), m_startRow( startRow ), m_endRow( endRow )
      {
      }

      PCL_HOT_FUNCTION void Run() override
      {
         for ( int i = m_startRow; i < m_endRow; ++i )
            for ( int j = 0, dx = 0, y = m_grid.m_rect.y0 + i*m_grid.m_delta; j < m_grid.m_G.Cols(); ++j, dx += m_grid.m_delta )
               m_grid.m_G[i][j] = m_sephard( m_grid.m_rect.x0 + dx, y );
      }

   private:

            GridShepardInterpolation& m_grid;
      const scalar_interpolation&     m_sephard;
            int                       m_startRow, m_endRow;
   };
};

// ----------------------------------------------------------------------------

}  // pcl

#endif   // __PCL_ShepardInterpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/ShepardInterpolation.h - Released 2018-11-23T16:14:19Z
