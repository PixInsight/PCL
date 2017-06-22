//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/SurfaceSpline.h - Released 2017-06-21T11:36:33Z
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

#ifndef __PCL_SurfaceSpline_h
#define __PCL_SurfaceSpline_h

/// \file pcl/SurfaceSpline.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class SurfaceSplineBase
 * \brief Base class of two-dimensional surface splines.
 */
class SurfaceSplineBase
{
protected:

   SurfaceSplineBase()
   {
   }

   SurfaceSplineBase( const SurfaceSplineBase& )
   {
   }

   virtual ~SurfaceSplineBase()
   {
   }

   static void Generate( float*, float*, const float*, int,
                         int, float, const float*, float*, double&, double&, double& );

   static void Generate( double*, double*, const double*, int,
                         int, float, const float*, double*, double&, double&, double& );

   static float Interpolate( const float*, const float*, int, const float*, int, double, double );

   static double Interpolate( const double*, const double*, int, const double*, int, double, double );
};

// ----------------------------------------------------------------------------

/*!
 * \class SurfaceSpline
 * \brief Two-dimensional interpolating/approximating surface spline (thin plate).
 *
 * %SurfaceSpline implements interpolating or smoothing surface splines (also
 * known as <em>thin plates</em>) for arbitrarily distributed input nodes in
 * two dimensions.
 *
 * The most distinctive property of surface splines is their high adaptability
 * to local variations, which makes them ideal to model complex two dimensional
 * functions with high accuracy. An important advantage of our implementation
 * is the possibility to control adaptability with approximating (or smoothing)
 * surface splines, as opposed to interpolating splines, and the possibility to
 * control adaptability both as a global property of the modeling device, or on
 * a point per point basis. The main drawback of surface splines is that they
 * are computationally expensive, especially for large data sets. See the
 * GridInterpolation and PointGridInterpolation classes for discretized
 * implementations with much higher efficiency.
 *
 * \sa PointSurfaceSpline, GridInterpolation, PointGridInterpolation,
 * SurfacePolynomial
 */
template <typename T>
class PCL_CLASS SurfaceSpline : private SurfaceSplineBase
{
public:

   /*!
    * Represents a vector of coordinates, function values or spline
    * coefficients.
    */
   typedef GenericVector<T>               vector_type;

   /*!
    * The numeric type used to represent coordinates, function values and
    * spline coefficients.
    */
   typedef typename vector_type::scalar   scalar;

   /*!
    * Default constructor. Constructs an empty, two-dimensional interpolating
    * surface spline of second order.
    */
   SurfaceSpline() = default;

   /*!
    * Copy constructor.
    */
   SurfaceSpline( const SurfaceSpline& ) = default;

   /*!
    * Move constructor.
    */
#ifndef _MSC_VER
   SurfaceSpline( SurfaceSpline&& ) = default;
#else
   SurfaceSpline( SurfaceSpline&& x ) :
      m_x( std::move( x.m_x ) ),
      m_y( std::move( x.m_y ) ),
      m_r0( x.m_r0 ),
      m_x0( x.m_x0 ),
      m_y0( x.m_y0 ),
      m_order( x.m_order ),
      m_smoothing( x.m_smoothing ),
      m_weights( std::move( x.m_weights ) ),
      m_spline( std::move( x.m_spline ) )
   {
   }
#endif

   /*!
    * Virtual destructor.
    */
   virtual ~SurfaceSpline()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   SurfaceSpline& operator =( const SurfaceSpline& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
#ifndef _MSC_VER
   SurfaceSpline& operator =( SurfaceSpline&& ) = default;
#else
   SurfaceSpline& operator =( SurfaceSpline&& x )
   {
      m_x = std::move( x.m_x );
      m_y = std::move( x.m_y );
      m_r0 = x.m_r0;
      m_x0 = x.m_x0;
      m_y0 = x.m_y0;
      m_order = x.m_order;
      m_smoothing = x.m_smoothing;
      m_weights = std::move( x.m_weights );
      m_spline = std::move( x.m_spline );
      return *this;
   }
#endif

   /*!
    * Returns true iff this surface spline is valid. A valid surface spline has
    * been initialized with three or more nodes.
    */
   bool IsValid() const
   {
      return m_x.Length() == m_y.Length() && m_x.Length() >= 3;
   }

   /*!
    * Returns the number of nodes used by this surface spline interpolation.
    */
   int Length() const
   {
      return m_x.Length();
   }

   /*!
    * Returns a vector with the X node coordinates used to initialize this
    * surface spline. If this object has not been initialized, this function
    * returns an empty vector.
    */
   vector_type X() const
   {
      vector_type x( m_x.Length() );
      if ( IsValid() )
         for ( int i = 0; i < m_x.Length(); ++i )
            x[i] = m_x0 + m_x[i]/m_r0;
      return x;
   }

   /*!
    * Returns a vector with the Y node coordinates used to initialize this
    * surface spline. If this object has not been initialized, this function
    * returns an empty vector.
    */
   vector_type Y() const
   {
      vector_type y( m_y.Length() );
      if ( IsValid() )
         for ( int i = 0; i < m_y.Length(); ++i )
            y[i] = m_y0 + m_y[i]/m_r0;
      return y;
   }

   /*!
    * Returns the derivative order of this surface spline.
    */
   int Order() const
   {
      return m_order;
   }

   /*!
    * Sets the derivative order of this surface spline.
    *
    * \param order   Derivative order. Must be >= 1.
    *
    * Calling this member function implicitly resets this %SurfaceSpline
    * object and destroys all internal working structures.
    *
    * The surface spline will be continuously differentiable up to the
    * specified order \a m. If this order is too high, an ill-conditioned
    * linear system may result.
    *
    * The default order is 2. Recommended values are 2 and 3.
    */
   void SetOrder( int order )
   {
      PCL_PRECONDITION( order >= 1 )
      Clear();
      m_order = pcl::Max( 1, order );
   }

   /*!
    * Returns the <em>smoothing factor</em> of this surface spline.
    */
   float Smoothing() const
   {
      return m_smoothing;
   }

   /*!
    * Sets the <em>smoothing factor</em> of this surface spline.
    *
    * \param s    Smoothing factor. Must be >= 0.
    *
    * For \a s = 0, an interpolating spline will be generated: all node values
    * will be reproduced exactly at their respective coordinates.
    *
    * For \a s > 0, a smoothing (or approximating) spline will be generated:
    * increasing \a s values will generate splines closer to the reference
    * plane of the input node set.
    */
   void SetSmoothing( float s )
   {
      PCL_PRECONDITION( s >= 0 )
      Clear();
      m_smoothing = pcl::Max( 0.0F, s );
   }

   /*!
    * Generation of a two-dimensional surface spline (thin plate).
    *
    * \param x       X node coordinates.
    *
    * \param y       Y node coordinates.
    *
    * \param z       Node values.
    *
    * \param n       Number of nodes. Must be >= 3
    *                (3 nodes * 2 coordinates = six degrees of freedom).
    *
    * \param w       When the smoothing factor of this spline is > 0, this is a
    *                vector of positive weights > 0 corresponding to the
    *                specified input nodes. If this parameter is \c nullptr,
    *                unit weights are assumed for all input nodes. When the
    *                smoothing factor is zero (interpolating spline), this
    *                parameter is ignored.
    *
    * The input nodes can be arbitrarily distributed, and they don't need to
    * follow any specific order. However, all nodes must be distinct with
    * respect to the machine epsilon for the floating point type T.
    *
    * For an interpolating surface spline (smoothness = 0), all node values
    * will be reproduced exactly at their respective coordinates. In this case
    * the vector \a w of node weights will be ignored.
    *
    * For an approximating surface spline (smoothness > 0), the specified
    * vector \a w of node weights will be used to assign a different
    * interpolation \e strength to each interpolation node. In this case the
    * vector \a w must have at least \a n values greater than zero. A node
    * weight larger than one will reduce the smoothness of the interpolating
    * surface at the corresponding node coordinates, or in other words, it will
    * give more prominence to the corresponding data point. A node weight of
    * one will apply the current surface smoothness at its node position. A
    * node weight smaller than one will increase the interpolation smoothness.
    */
   void Initialize( const T* x, const T* y, const T* z, int n, const float* w = nullptr )
   {
      PCL_PRECONDITION( x != nullptr && y != nullptr && z != nullptr )
      PCL_PRECONDITION( n > 2 )

      if ( n < 3 )
         throw Error( "At least three input nodes are required in SurfaceSpline::Initialize()" );

      Clear();

      try
      {
         m_x = vector_type( x, n );
         m_y = vector_type( y, n );

         if ( m_smoothing > 0 && w != nullptr )
         {
            m_weights = FVector( w, n );
            for ( float& w : m_weights )
               if ( w <= 0 )
                  w = 1.0F;
         }

         m_spline = vector_type( T( 0 ), n + ((m_order*(m_order + 1)) >> 1) );

         Generate( m_x.Begin(), m_y.Begin(), z, n, m_order, m_smoothing, m_weights.Begin(), m_spline.Begin(), m_r0, m_x0, m_y0 );
      }
      catch ( ... )
      {
         Clear();
         throw;
      }
   }

   /*!
    * Two-dimensional surface spline interpolation. Returns an interpolated
    * value at the specified \a x and \a y coordinates.
    */
   T operator ()( double x, double y ) const
   {
      PCL_PRECONDITION( !m_x.IsEmpty() && !m_y.IsEmpty() )
      PCL_PRECONDITION( m_order >= 1 )
      PCL_PRECONDITION( !m_spline.IsEmpty() )
      return Interpolate( m_x.Begin(), m_y.Begin(), m_x.Length(), m_spline.Begin(), m_order, m_r0*(x - m_x0), m_r0*(y - m_y0) );
   }

   /*!
    * Resets this surface spline interpolation, deallocating all internal
    * working structures.
    */
   void Clear()
   {
      m_x.Clear();
      m_y.Clear();
      m_weights.Clear();
      m_spline.Clear();
   }

protected:

   vector_type m_x;             // vector of normalized X node coordinates
   vector_type m_y;             // vector of normalized Y node coordinates
   double      m_r0        = 1; // scaling factor for normalization of node coordinates
   double      m_x0        = 0; // zero offset for normalization of X node coordinates
   double      m_y0        = 0; // zero offset for normalization of Y node coordinates
   int         m_order     = 2; // derivative order > 0
   float       m_smoothing = 0; // smoothing factor, or interpolating 2-D spline if m_smoothing == 0
   FVector     m_weights;       // vector of node weights if m_smoothing != 0, otherwise ignored (empty)
   vector_type m_spline;        // coefficients of the 2-D surface spline

   friend class DrizzleData;
   friend class DrizzleDataDecoder;
};

// ----------------------------------------------------------------------------

}  // pcl

#endif   // __PCL_SurfaceSpline_h

// ----------------------------------------------------------------------------
// EOF pcl/SurfaceSpline.h - Released 2017-06-21T11:36:33Z
