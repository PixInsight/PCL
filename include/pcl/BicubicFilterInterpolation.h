//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/BicubicFilterInterpolation.h - Released 2017-04-14T23:04:40Z
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

#ifndef __PCL_BicubicFilterInterpolation_h
#define __PCL_BicubicFilterInterpolation_h

/// \file pcl/BicubicFilterInterpolation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/BidimensionalInterpolation.h>
#include <pcl/Math.h>
#include <pcl/Utility.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class CubicFilter
 * \brief Mitchell-Netravali parameterized cubic filters.
 *
 * %CubicFilter implements a two-parameter, separable cubic filter as described
 * in Don P. Mitchell, Arun N. Netravali (1988), <em>%Reconstruction Filters in
 * %Computer %Graphics</em>, %Computer %Graphics, Vol. 22, No. 4, pp. 221-228.
 *
 * The family of cubic filters designed by Mitchell and Netravali has two
 * parameters called B and C. Although these parameters can take any values,
 * the authors recommend values pertaining to the line B + 2C = 1. In
 * particular, the filter defined by B=C=1/3 has been proven to have excellent
 * characteristics for a wide range of image reconstruction tasks.
 *
 * \sa MitchellNetravaliCubicFilter, CatmullRomSplineFilter, CubicBSplineFilter
 */
class PCL_CLASS CubicFilter
{
public:

   /*!
    * Constructs a new %CubicFilter object with the specified filter
    * parameters \a B and \a C.
    */
   CubicFilter( double B, double C ) : m_B( B ), m_C( C )
   {
      m_k31 = ( 12 -  9*m_B -  6*m_C)/6;
      m_k21 = (-18 + 12*m_B +  6*m_C)/6;
      m_k01 = (  6 -  2*m_B         )/6;
      m_k32 = (        -m_B -  6*m_C)/6;
      m_k22 = (       6*m_B + 30*m_C)/6;
      m_k12 = (     -12*m_B - 48*m_C)/6;
      m_k02 = (       8*m_B + 24*m_C)/6;
   }

   /*!
    * Copy constructor.
    */
   CubicFilter( const CubicFilter& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~CubicFilter()
   {
   }

   /*!
    * Assignment operator.
    */
   CubicFilter& operator =( const CubicFilter& ) = default;

   /*!
    * Evaluates this cubic filter for \a x.
    *
    * \note For performance reasons, this function returns an invalid value
    * outside the range ]-2,+2[, which is the support of Mitchell-Netravali
    * cubic filters. Strictly, zero should be returned outside the support
    * range, but since this is a performance-critical routine, we have
    * sacrified strictness for the sake of optimization. This function should
    * never be called for Abs( \a x ) >= 2.
    */
   PCL_HOT_FUNCTION
   double operator()( double x ) const
   {
      if ( x < 0 )
         x = -x;
      return (x < 1) ? m_k01 + x*x*(m_k21 + x*m_k31) :
                       m_k02 + x*(m_k12 + x*(m_k22 + x*m_k32));
   }

   /*!
    * Returns the filter's \e width, measured from the origin to its cutoff
    * point. We define the \e support of a (symmetric) filter as the range
    * ]-width,+width[.
    *
    * Mitchell-Netravali cubic filters are zero outside the range ]-2,+2[,
    * hence this function always returns 2.
    */
   double Width() const
   {
      return 2.0;
   }

   /*!
    * Returns a descriptive text string for this cubic filter.
    */
   virtual String Description() const
   {
      return String().Format( "Cubic filter (B=%.6f, C=%.6f)", m_B, m_C );
   }

   /*!
    * Returns a pointer to a dynamically allocated duplicate of this filter.
    */
   virtual CubicFilter* Clone() const
   {
      return new CubicFilter( *this );
   }

protected:

   double m_k31, m_k21, m_k01, m_k32, m_k22, m_k12, m_k02;

private:

   double m_B, m_C; // for reference only; not used in calculations
};

/*!
 * \class MitchellNetravaliCubicFilter
 * \brief Mitchell-Netravali cubic filter with B=C=1/3
 *
 * This is the cubic filter recommended by Mitchell and Netravali (1988). It is
 * implemented as a CubicFilter with parameters B=1/3 and C=1/3.
 *
 * \sa CubicFilter, CatmullRomSplineFilter, CubicBSplineFilter
 */
class PCL_CLASS MitchellNetravaliCubicFilter : public CubicFilter
{
public:

   /*!
    * Constructs a new %MitchellNetravaliCubicFilter object.
    */
   MitchellNetravaliCubicFilter() : CubicFilter( 1.0/3, 1.0/3 )
   {
   }

   /*!
    * Copy constructor.
    */
   MitchellNetravaliCubicFilter( const MitchellNetravaliCubicFilter& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~MitchellNetravaliCubicFilter()
   {
   }

   /*!
    */
   virtual String Description() const
   {
      return "Mitchell-Netravali cubic filter";
   }

   /*!
    */
   virtual CubicFilter* Clone() const
   {
      return new MitchellNetravaliCubicFilter( *this );
   }
};

/*!
 * \class CatmullRomSplineFilter
 * \brief Catmull-Rom spline filter
 *
 * The Catmull-Rom spline filter is implemented as a CubicFilter with
 * parameters B=0 and C=0.5.
 *
 * \sa CubicFilter, MitchellNetravaliCubicFilter, CubicBSplineFilter
 */
class PCL_CLASS CatmullRomSplineFilter : public CubicFilter
{
public:

   /*!
    * Constructs a new %CatmullRomSplineFilter object.
    */
   CatmullRomSplineFilter() : CubicFilter( 0, 0.5 )
   {
   }

   /*!
    * Copy constructor.
    */
   CatmullRomSplineFilter( const CatmullRomSplineFilter& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~CatmullRomSplineFilter()
   {
   }

   /*!
    */
   virtual String Description() const
   {
      return "Catmull-Rom spline filter";
   }

   /*!
    */
   virtual CubicFilter* Clone() const
   {
      return new CatmullRomSplineFilter( *this );
   }
};

/*!
 * \class CubicBSplineFilter
 * \brief Cubic B-spline filter
 *
 * The cubic B-spline filter is implemented as a CubicFilter with
 * parameters B=1 and C=0.
 *
 * \sa CubicFilter, MitchellNetravaliCubicFilter, CatmullRomSplineFilter
 */
class PCL_CLASS CubicBSplineFilter : public CubicFilter
{
public:

   /*!
    * Constructs a new %CubicBSplineFilter object.
    */
   CubicBSplineFilter() : CubicFilter( 1, 0 )
   {
   }

   /*!
    * Copy constructor.
    */
   CubicBSplineFilter( const CubicBSplineFilter& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~CubicBSplineFilter()
   {
   }

   /*!
    */
   virtual String Description() const
   {
      return "Cubic B-spline filter";
   }

   /*!
    */
   virtual CubicFilter* Clone() const
   {
      return new CubicBSplineFilter( *this );
   }
};

// ----------------------------------------------------------------------------

#define m_width      this->m_width
#define m_height     this->m_height
#define m_fillBorder this->m_fillBorder
#define m_fillValue  this->m_fillValue
#define m_data       this->m_data

// ----------------------------------------------------------------------------

/*!
 * \class BicubicFilterInterpolation
 * \brief Bicubic filter interpolation algorithms.
 *
 * Bicubic filter interpolation uses a \e cubic \e filter (an instance of
 * CubicFilter or a derived class) to interpolate pixel values in a rectangular
 * pixel matrix of the specified horizontal and vertical \e radii. Thanks to
 * the separability of cubic filters, %BicubicFilterInterpolation can be
 * applied with a different filter size on each axis.
 *
 * \sa BidimensionalInterpolation, CubicFilter, MitchellNetravaliCubicFilter,
 * CatmullRomSplineFilter, CubicBSplineFilter, NearestNeighborInterpolation,
 * BilinearInterpolation, BicubicSplineInterpolation,
 * BicubicBSplineInterpolation, LanczosInterpolation
 */
template <typename T>
class PCL_CLASS BicubicFilterInterpolation : public BidimensionalInterpolation<T>
{
public:

   /*!
    * Constructs a new %BicubicFilterInterpolation instance.
    *
    * \param rx,ry   Horizontal and vertical interpolation radii, respectively,
    *                in pixels. Both radii must be >= 1. Interpolation will
    *                take place in a rectangular pixel matrix with 2*rx + 1
    *                columns and 2*ry + 1 rows.
    *
    * \param filter  Reference to a CubicFilter instance that will be used as
    *                the interpolation filter.
    */
   BicubicFilterInterpolation( int rx, int ry, const CubicFilter& filter ) :
      BidimensionalInterpolation<T>(),
      m_rx( Max( 1, rx ) ), m_ry( Max( 1, ry ) ), m_filter( filter )
   {
      PCL_PRECONDITION( rx >= 1 )
      PCL_PRECONDITION( ry >= 1 )
      Initialize();
   }

   /*!
    * Virtual destructor.
    */
   virtual ~BicubicFilterInterpolation()
   {
   }

   /*!
    */
   virtual void Initialize( const T* data, int dataWidth, int dataHeight )
   {
      BidimensionalInterpolation<T>::Initialize( data, dataWidth, dataHeight );

      if ( m_rx > m_width || m_ry > m_height )
      {
         m_rx = Min( m_rx, m_width );
         m_ry = Min( m_ry, m_height );
         Initialize();
      }
   }

   /*!
    * Returns an interpolated value at {\a x,\a y} location.
    *
    * \param x,y  Coordinates of the interpolation point (horizontal,vertical).
    *
    * %Interpolation takes place on a rectangular matrix whose center is the
    * nearest pixel to {\a x, \a y}. The interpolation matrix has 2*rx + 1
    * columns and 2*ry + 1 rows, where rx and ry are the horizontal and
    * vertical interpolation radii, respectively.
    */
   virtual PCL_HOT_FUNCTION
   double operator()( double x, double y ) const
   {
      PCL_PRECONDITION( m_data != nullptr )
      PCL_PRECONDITION( m_width > 0 && m_height > 0 )
      PCL_PRECONDITION( x >= 0 && x < m_width )
      PCL_PRECONDITION( y >= 0 && y < m_height )
      PCL_CHECK( m_rx >= 1 && m_rx <= m_width )
      PCL_CHECK( m_ry >= 1 && m_ry <= m_height )
      PCL_CHECK( !m_k.IsEmpty() )

      int x0 = Range( RoundInt( x ), 0, m_width-1 );
      int y0 = Range( RoundInt( y ), 0, m_height-1 );

      double dx = x - x0;
      double dy = y - y0;

      int64 d = int64( y0 - m_ry )*m_width + x0 - m_rx;

      if ( m_rx >= m_ry )
      {
         double wx = 0;
         for ( int xi = -m_rx, c = 0; xi <= m_rx; ++xi, ++c )
            wx += (m_k[c] = m_filter( m_sx*(xi - dx) ));

         double sy = 0, wy = 0;
         for ( int yi = -m_ry; yi <= m_ry; ++yi )
         {
            double ky = m_filter( m_sy*(yi - dy) );
            wy += ky;

            const T* p;
            int y = y0 + yi;

            if ( y < 0 )
            {
               d += m_width;

               if ( m_fillBorder )
               {
                  sy += m_fillValue * ky;
                  continue;
               }

               p = m_data + (d - 2*int64( m_width )*(y + 1));
            }
            else if ( y >= m_height )
            {
               if ( m_fillBorder )
               {
                  sy += m_fillValue * ky;
                  continue;
               }

               p = m_data + (d -= m_width);
            }
            else
            {
               p = m_data + d;
               d += m_width;
            }

            int x  = x0 - m_rx;
            int x2 = x0 + m_rx;
            int x1 = Min( x2, m_width-1 );
            double sx = 0;
            const double* kx = m_k.Begin();

            while ( x < 0 )
            {
               ++x;
               ++p;
               sx += (m_fillBorder ? m_fillValue : double( *(p - (x + x)) )) * *kx++;
            }

            for ( ;; )
            {
               sx += *p++ * *kx++;
               if ( ++x > x1 )
                  break;
            }

            while ( x <= x2 )
            {
               sx += (m_fillBorder ? m_fillValue : double( *--p )) * *kx++;
               ++x;
            }

            sy += sx/wx * ky;
         }

         return sy/wy;
      }
      else
      {
         double wy = 0;
         for ( int yi = -m_ry, r = 0; yi <= m_ry; ++yi, ++r )
            wy += (m_k[r] = m_filter( m_sy*(yi - dy) ));

         double sx = 0, wx = 0;
         for ( int xi = -m_rx; xi <= m_rx; ++xi )
         {
            double kx = m_filter( m_sx*(xi - dx) );
            wx += kx;

            const T* p;
            int x = x0 + xi;

            if ( x < 0 )
            {
               ++d;

               if ( m_fillBorder )
               {
                  sx += m_fillValue * kx;
                  continue;
               }

               p = m_data + (d - 2*(x + 1));
            }
            else if ( x >= m_width )
            {
               if ( m_fillBorder )
               {
                  sx += m_fillValue * kx;
                  continue;
               }

               p = m_data + --d;
            }
            else
               p = m_data + d++;

            int y  = y0 - m_ry;
            int y2 = y0 + m_ry;
            int y1 = Min( y2, m_height-1 );
            double sy = 0;
            const double* ky = m_k.Begin();

            while ( y < 0 )
            {
               ++y;
               p += m_width;
               sy += (m_fillBorder ? m_fillValue : double( *(p - (y + y)*m_width) )) * *ky++;
            }

            for ( ;; )
            {
               sy += *p * *ky++;
               p += m_width;
               if ( ++y > y1 )
                  break;
            }

            while ( y <= y2 )
            {
               sy += (m_fillBorder ? m_fillValue : double( *(p -= m_width) )) * *ky++;
               ++y;
            }

            sx += sy/wy * kx;
         }

         return sx/wx;
      }
   }

   /*!
    * Returns the horizontal interpolation radius in pixels.
    */
   int HorizontalRadius() const
   {
      return m_rx;
   }

   /*!
    * Returns the vertical interpolation radius in pixels.
    */
   int VerticalRadius() const
   {
      return m_ry;
   }

   /*!
    * Sets new interpolation radii.
    *
    * \param rx,ry   Horizontal and vertical interpolation radii, respectively,
    *                in pixels. Both radii must be >= 1. Interpolation will
    *                take place in a rectangular pixel matrix with 2*rh + 1
    *                columns and 2*rv + 1 rows.
    */
   void SetRadii( int rx, int ry )
   {
      if ( rx != m_rx || ry != m_ry )
      {
         m_rx = Max( 1, rx );
         m_ry = Max( 1, ry );

         if ( this->data != nullptr )
         {
            if ( m_rx > m_width )
               m_rx = m_width;
            if ( m_ry > m_height )
               m_ry = m_height;
         }

         Initialize();
      }
   }

   /*!
    * Returns a constant reference to the cubic filter being used by this
    * interpolation.
    */
   const CubicFilter& Filter() const
   {
      return m_filter;
   }

   /*!
    * Sets a new cubic \a filter to be applied by this interpolation.
    */
   void SetFilter( const CubicFilter& filter )
   {
      m_filter = filter;
      Initialize();
   }

protected:

           int         m_rx, m_ry; // horizontal and vertical radii
           double      m_sx, m_sy; // filter scaling ratios
   mutable DVector     m_k;        // workspace for interpolated values
           CubicFilter m_filter;

   void Initialize()
   {
      m_sx = m_filter.Width()/(m_rx + 0.5);
      m_sy = m_filter.Width()/(m_ry + 0.5);
      m_k = DVector( 1 + (Max( m_rx, m_ry ) << 1) );
   }
};

// ----------------------------------------------------------------------------

#undef m_width
#undef m_height
#undef m_fillBorder
#undef m_fillValue
#undef m_data

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BicubicFilterInterpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/BicubicFilterInterpolation.h - Released 2017-04-14T23:04:40Z
