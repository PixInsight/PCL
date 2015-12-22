//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/BicubicInterpolation.h - Released 2015/12/17 18:52:09 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_BicubicInterpolation_h
#define __PCL_BicubicInterpolation_h

/// \file pcl/BicubicInterpolation.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_Utility_h
#include <pcl/Utility.h>
#endif

#ifndef __PCL_BidimensionalInterpolation_h
#include <pcl/BidimensionalInterpolation.h>
#endif

#ifndef __PCL_Math_h
#include <pcl/Math.h>
#endif

namespace pcl
{

#define m_width      this->m_width
#define m_height     this->m_height
#define m_fillBorder this->m_fillBorder
#define m_fillValue  this->m_fillValue
#define m_data       this->m_data

// ----------------------------------------------------------------------------

/*!
 * \class BicubicInterpolationBase
 * \brief Base class for bicubic interpolation algorithm implementations
 *
 * %BicubicInterpolationBase is a base class for BicubicSplineInterpolation and
 * BicubicBSplineInterpolation.
 *
 * \sa BicubicSplineInterpolation, BicubicBSplineInterpolation
 */
template <typename T>
class PCL_CLASS BicubicInterpolationBase : public BidimensionalInterpolation<T>
{
public:

   /*!
    * Constructs a new %BicubicInterpolationBase instance.
    */
   BicubicInterpolationBase() = default;

protected:

   void InitXY( int& i1, int& j1, double* p0, double* p1, double* p2, double* p3, double x, double y ) const
   {
      PCL_PRECONDITION( int( x ) >= 0 )
      PCL_PRECONDITION( int( x ) < m_width )
      PCL_PRECONDITION( int( y ) >= 0 )
      PCL_PRECONDITION( int( y ) < m_height )

      // Central grid coordinates
      i1 = pcl::Range( TruncI( y ), 0, m_height-1 );
      j1 = pcl::Range( TruncI( x ), 0, m_width-1 );

      // Set up source matrix

      int j0 = j1 - 1;
      int i0 = i1 - 1;

      int j2 = j1 + 1;
      int i2 = i1 + 1;

      int j3 = j1 + 2;
      int i3 = i1 + 2;

      const T* fp = m_data + (int64( i0 )*m_width + j0);

         // Row 0

      if ( i0 < 0 )
      {
         fp += m_width;

         if ( m_fillBorder )
         {
            p0[0] = p0[1] = p0[2] = p0[3] = m_fillValue;
            goto __row1;
         }
      }

      GetRow( p0, fp, j0, j2, j3 );

      if ( i0 >= 0 )
         fp += m_width;

__row1:  // Row 1

      GetRow( p1, fp, j0, j2, j3 );

         // Row 2

      if ( i2 < m_height )
         fp += m_width;

      GetRow( p2, fp, j0, j2, j3 );

         // Row 3

      if ( i3 < m_height )
         fp += m_width;
      else if ( m_fillBorder )
      {
         p3[0] = p3[1] = p3[2] = p3[3] = m_fillValue;
         goto __done;
      }

      GetRow( p3, fp, j0, j2, j3 );

__done:
      ;
   }

   void InitYX( int& i1, int& j1, double* p0, double* p1, double* p2, double* p3, double x, double y ) const
   {
      PCL_PRECONDITION( int( x ) >= 0 )
      PCL_PRECONDITION( int( x ) < m_width )
      PCL_PRECONDITION( int( y ) >= 0 )
      PCL_PRECONDITION( int( y ) < m_height )

      // Central grid coordinates
      i1 = pcl::Range( TruncI( y ), 0, m_height-1 );
      j1 = pcl::Range( TruncI( x ), 0, m_width-1 );

      // Set up source matrix

      int j0 = j1 - 1;
      int i0 = i1 - 1;

      int j2 = j1 + 1;
      int i2 = i1 + 1;

      int j3 = j1 + 2;
      int i3 = i1 + 2;

      const T* fp = m_data + (int64( i0 )*m_width + j0);

         // Column 0

      if ( j0 < 0 )
      {
         ++fp;

         if ( m_fillBorder )
         {
            p0[0] = p0[1] = p0[2] = p0[3] = m_fillValue;
            goto __col1;
         }
      }

      GetColumn( p0, fp, i0, i2, i3 );

      if ( j0 >= 0 )
         ++fp;

__col1:  // Column 1

      GetColumn( p1, fp, i0, i2, i3 );

         // Column 2

      if ( j2 < m_width )
         ++fp;

      GetColumn( p2, fp, i0, i2, i3 );

         // Column 3

      if ( j3 < m_width )
         ++fp;
      else if ( m_fillBorder )
      {
         p3[0] = p3[1] = p3[2] = p3[3] = m_fillValue;
         goto __done;
      }

      GetColumn( p3, fp, i0, i2, i3 );

__done:
      ;
   }

private:

   void GetRow( double* p, const T* fp, int j0, int j2, int j3 ) const
   {
      if ( m_fillBorder )
      {
           *p = (j0 >= 0) ? *fp : m_fillValue;
         *++p = *++fp;

         if ( j2 < m_width )
         {
            *++p = *++fp;
            *++p = (j3 < m_width) ? *++fp : m_fillValue;
         }
         else
            p[1] = p[2] = m_fillValue;
      }
      else
      {
         if ( j0 < 0 )
            ++fp;
         *p = *fp;
         if ( j0 >= 0 )
            ++fp;
         *++p = *fp;

         if ( j2 < m_width )
         {
            *++p = *++fp;
            if ( j3 < m_width )
               ++fp;
            *++p = *fp;
         }
         else
         {
            *++p = *fp;
            *++p = *(fp - 1);
         }
      }
   }

   void GetColumn( double* p, const T* fp, int i0, int i2, int i3 ) const
   {
      if ( m_fillBorder )
      {
           *p = (i0 >= 0) ? *fp : m_fillValue;
         *++p = *(fp += m_width);

         if ( i2 < m_height )
         {
            *++p = *(fp += m_width);
            *++p = (i3 < m_height) ? *(fp += m_width) : m_fillValue;
         }
         else
            p[1] = p[2] = m_fillValue;
      }
      else
      {
         if ( i0 < 0 )
            fp += m_width;
         *p = *fp;
         if ( i0 >= 0 )
            fp += m_width;
         *++p = *fp;

         if ( i2 < m_height )
         {
            *++p = *(fp += m_width);
            if ( i3 < m_height )
               fp += m_width;
            *++p = *fp;
         }
         else
         {
            *++p = *fp;
            *++p = *(fp - m_width);
         }
      }
   }
};

// ----------------------------------------------------------------------------

#define InitXY this->InitXY
#define InitYX this->InitYX

/*
 * Undefine the following to use any free constant value a != -1/2
 */
#define __PCL_BICUBIC_SPLINE_A_IS_MINUS_ONE_HALF   1

/*
 * The "a" constant controls the depth of the negative lobes in the bicubic
 * spline interpolation function, -1 <= a < 0. Larger values of a (in absolute
 * value) lead to more ringing (sharpness). The default is -1/2, as recommended
 * in [Keys 1981].
 */
#define __PCL_BICUBIC_SPLINE_A                    -0.5

/*
 * Default clamping threshold for linear images. This value has been optimized
 * for a = -1/2. If you use another value for a, this must also be fine tuned.
 */
#ifndef __PCL_BICUBIC_SPLINE_CLAMPING_THRESHOLD
#define __PCL_BICUBIC_SPLINE_CLAMPING_THRESHOLD    0.3F
#endif

/*!
 * \class BicubicSplineInterpolation
 * \brief Bicubic spline interpolation algorithm
 *
 * Bicubic interpolation algorithms interpolate from the nearest sixteen mapped
 * source data items. Our implementation uses the following cubic spline
 * function as a separable filter to perform a convolution interpolation:
 *
 * <pre>
 * f(x) = (a+2)*x^3 - (a+3)*x^2 + 1       for 0 <= x <= 1
 * f(x) = a*x^3 - 5*a*x^2 + 8*a*x - 4*a   for 1 < x <= 2
 * f(x) = 0                               otherwise
 * </pre>
 *
 * Reference: Keys, R. G. (1981), <em>%Cubic %Convolution %Interpolation for
 * %Digital %Image %Processing</em>, IEEE Trans. %Acoustics, %Speech & %Signal
 * Proc., Vol. 29, pp. 1153-1160.
 *
 * The a constant parameter of the cubic spline (-1 <= a < 0) controls the
 * depth of the negative lobes of the interpolation function. In this
 * implementation we have set a fixed value <em>a</em>=-1/2.
 *
 * Our implementation includes an automatic <em>linear clamping</em> mechanism
 * to avoid discontinuity problems when interpolating linear images. The cubic
 * spline function has negative lobes that can cause undershoot (aka ringing)
 * artifacts when they fall over bright pixels. This happens frequently with
 * linear CCD images. See the documentation for the
 * BicubicSplineInterpolation::SetClampingThreshold( float ) function for a
 * more detailed description of the linear clamping mechanism.
 */
template <typename T>
class PCL_CLASS BicubicSplineInterpolation : public BicubicInterpolationBase<T>
{
public:

   /*!
    * Constructs a %BicubicSplineInterpolation instance.
    *
    * The optional \e clamp parameter defines a threshold to trigger the
    * <em>linear clamping</em> mechanism. See the documentation for the
    * SetClampingThreshold( float ) function for a detailed description of the
    * automatic linear clamping mechanism. The default value of \e clamp = 0.1
    * is appropriate for most linear images.
    */
   BicubicSplineInterpolation( float clamp = __PCL_BICUBIC_SPLINE_CLAMPING_THRESHOLD ) :
      BicubicInterpolationBase<T>(), m_clamp( Range( clamp, 0.0F, 1.0F ) )
   {
      PCL_PRECONDITION( 0 <= clamp && clamp <= 1 )
   }

   /*!
    * Virtual destructor.
    */
   virtual ~BicubicSplineInterpolation()
   {
   }

   /*!
    * Returns an interpolated value at {\a x,\a y} location.
    *
    * \param x,y  %Coordinates of the interpolation point (horizontal,
    *             vertical).
    *
    * This function performs a two-dimensional interpolation via a convolution
    * with the separable cubic spline filter.
    */
   virtual double operator()( double x, double y ) const
   {
      PCL_PRECONDITION( m_data != nullptr )
      PCL_PRECONDITION( m_width > 0 && m_height > 0 )

      // Initialize grid coordinates and source matrix
      int i1, j1;
      double p0[ 4 ], p1[ 4 ], p2[ 4 ], p3[ 4 ];
      InitXY( i1, j1, p0, p1, p2, p3, x, y );

      // Cubic spline coefficients
      double C[ 4 ];
      GetSplineCoefficients( C, x-j1 );

      // Interpolate neighbor rows
      double c[ 4 ];
      c[0] = Interpolate( p0, C );
      c[1] = Interpolate( p1, C );
      c[2] = Interpolate( p2, C );
      c[3] = Interpolate( p3, C );

      // Interpolate result vertically
      GetSplineCoefficients( C, y-i1 );
      return Interpolate( c, C );
   }

   /*!
    * Horizontal interpolation.
    *
    * \param x,y        %Coordinates of the interpolation point (horizontal,
    *                   vertical).
    *
    * \param[out] fx    Address of the first element of a vector where the four
    *                   interpolated X-values will be stored.
    *
    * This function interpolates horizontally the four neighbor rows for the
    * specified \a x, \a y coordinates. This is useful when this interpolation
    * algorithm is being used to interpolate an image on the horizontal axis
    * exclusively.
    */
   void InterpolateX( double fx[], double x, double y ) const
   {
      PCL_PRECONDITION( m_data != nullptr )
      PCL_PRECONDITION( m_width > 0 && m_height > 0 )

      // Initialize grid coordinates and source matrix
      int i1, j1;
      double p0[ 4 ], p1[ 4 ], p2[ 4 ], p3[ 4 ];
      InitXY( i1, j1, p0, p1, p2, p3, x, y );

      // Cubic spline coefficients
      double C[ 4 ];
      GetSplineCoefficients( C, x-j1 );

      // Interpolate neighbor rows
      fx[0] = Interpolate( p0, C );
      fx[1] = Interpolate( p1, C );
      fx[2] = Interpolate( p2, C );
      fx[3] = Interpolate( p3, C );
   }

   /*!
    * Vertical interpolation.
    *
    * \param x,y        %Coordinates of the interpolation point (horizontal,
    *                   vertical).
    *
    * \param[out] fy    Address of the first element of a vector where the four
    *                   interpolated Y-values will be stored.
    *
    * This function interpolates vertically the four neighbor columns for the
    * specified \a x, \a y coordinates. This is useful when this interpolation
    * algorithm is being used to interpolate an image on the vertical axis
    * exclusively.
    */
   void InterpolateY( double fy[], double x, double y ) const
   {
      PCL_PRECONDITION( m_data != nullptr )
      PCL_PRECONDITION( m_width > 0 && m_height > 0 )

      // Initialize grid coordinates and source matrix
      int i1, j1;
      double p0[ 4 ], p1[ 4 ], p2[ 4 ], p3[ 4 ];
      InitYX( i1, j1, p0, p1, p2, p3, x, y );

      // Cubic spline coefficients
      double C[ 4 ];
      GetSplineCoefficients( C, y-i1 );

      // Interpolate neighbor columns
      fy[0] = Interpolate( p0, C );
      fy[1] = Interpolate( p1, C );
      fy[2] = Interpolate( p2, C );
      fy[3] = Interpolate( p3, C );
   }

   /*!
    * %Vector interpolation.
    *
    * \param c    Address of the first element of a vector of four data points.
    *
    * \param dx   %Interpolation point in the range [0,+1[. A value of zero
    *             corresponds to the second element in the f vector.
    *
    * This function interpolates four neighbor rows or columns by direct
    * convolution with the cubic spline function. The four elements of the \a c
    * vector can be raw data points, to use cubic spline interpolation in one
    * dimension, or they can be interpolated rows or columns from an arbitrary
    * 2-D matrix. For example, \a c can be generated by a previous call to
    * InterpolateX() or InterpolateY(), respectively to provide source
    * interpolated rows or columns, or by equivalent functions from a different
    * separable cubic interpolation algorithm. This is useful to mix cubic
    * spline interpolation with other interpolation algorithms, which allows
    * for flexible interpolation schemes.
    */
   double InterpolateVector( const double c[], double dx ) const
   {
      double C[ 4 ];
      GetSplineCoefficients( C, dx );
      return Interpolate( c, C );
   }

   /*!
    * Returns the current <em>linear clamping threshold</em> for this
    * interpolation object.
    *
    * See the documentation for SetClampingThreshold( float ) for a detailed
    * description of the automatic linear clamping mechanism.
    */
   float ClampingThreshold() const
   {
      return m_clamp;
   }

   /*!
    * Defines a threshold to trigger the <em>linear clamping</em> mechanism.
    * The clamping mechanism automatically switches to linear interpolation
    * when the differences between neighbor pixels are so large that the cubic
    * interpolation function may cause ringing artifacts. Ringing occurs when
    * the negative lobes of the cubic spline interpolation function fall over
    * isolated bright source pixels or bright edges. For example, ringing
    * problems happen frequently around stars in linear CCD images. For
    * nonlinear images, linear clamping is almost never necessary, as in
    * nonlinear images (e.g., stretched deep-sky images, or diurnal images)
    * there are normally no such large variations between neighbor pixels.
    *
    * Linear clamping works on a per-row or per-column basis within the
    * interpolation neighborhood of 16 pixels. This means that when the
    * clamping mechanism selects linear interpolation, it restricts its use to
    * the affected (by too strong variations) row or column, without changing
    * the bicubic interpolation scheme as a whole. This ensures artifact-free
    * interpolated images without degrading the overall performance of bicubic
    * spline interpolation.
    *
    * The specified clamping threshold \e clamp must be in the [0,1] range.
    * Lower values cause a more aggressive deringing effect.
    */
   void SetClampingThreshold( float c )
   {
      PCL_PRECONDITION( 0 <= c && c <= 1 )
      m_clamp = Range( c, 0.0F, 1.0F );
   }

private:

   double m_clamp;

   PCL_HOT_FUNCTION
   double Interpolate( const double p[], const double C[] ) const
   {
      // Unclamped code:
      //return p[0]*C[0] + p[1]*C[1] + p[2]*C[2] + p[3]*C[3];
      double f12 = p[1]*C[1] + p[2]*C[2];
      double f03 = p[0]*C[0] + p[3]*C[3];
      return (-f03 < f12*m_clamp) ? f12 + f03 : f12/(C[1] + C[2]);
   }

   PCL_HOT_FUNCTION
   void GetSplineCoefficients( double C[], double dx ) const
   {
      double dx2 = dx*dx;
      double dx3 = dx2*dx;

#ifdef __PCL_BICUBIC_SPLINE_A_IS_MINUS_ONE_HALF
      // Optimized code for a = -1/2
      // We *really* need optimization here since this routine is called twice
      // for each interpolated pixel.
      double dx1_2 = dx/2;
      double dx2_2 = dx2/2;
      double dx3_2 = dx3/2;
      double dx22 = dx2 + dx2;
      double dx315 = dx3 + dx3_2;
      C[0] =  dx2 - dx3_2 - dx1_2;
      C[1] =  dx315 - dx22 - dx2_2 + 1;
      C[2] =  dx22 - dx315 + dx1_2;
      C[3] =  dx3_2 - dx2_2;
#else
#  define a (__PCL_BICUBIC_SPLINE_A)
      C[0] =        a*dx3 -       2*a*dx2 + a*dx;
      C[1] =  (a + 2)*dx3 -   (a + 3)*dx2         + 1;
      C[2] = -(a + 2)*dx3 + (2*a + 3)*dx2 - a*dx;
      C[3] =       -a*dx3 +         a*dx2;
#  undef a
#endif
   }
};

/*!
 * \class pcl::BicubicInterpolation
 * \brief Bicubic interpolation - an alias for BicubicSplineInterpolation
 *
 * %BicubicInterpolation is a synonym for the BicubicSplineInterpolation class.
 *
 * \deprecated This class exists to support old PCL-based code; all new code
 * should use the BicubicSplineInterpolation class.
 *
 * \sa BicubicSplineInterpolation
 */
template <typename T>
class PCL_CLASS BicubicInterpolation : public BicubicSplineInterpolation<T>
{
public:

   /*!
    * Constructs a %BicubicInterpolation instance.
    */
   BicubicInterpolation() = default;

   /*!
    * Virtual destructor.
    */
   virtual ~BicubicInterpolation()
   {
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class BicubicBSplineInterpolation
 * \brief Bicubic B-Spline Interpolation Algorithm
 *
 * Like bicubic spline interpolation, the bicubic B-spline interpolation
 * algorithm also interpolates from the nearest sixteen data items. However,
 * this algorithm uses B-spline interpolating functions instead of cubic
 * splines, which yields quite (too?) smooth results.
 *
 * This implementation is based on <em>Bicubic %Interpolation for %Image
 * Scaling</em>, by Paul Bourke. It performs a convolution with a nonseparable
 * 2-D filter, so its performance is O(n^2).
 *
 * \sa BicubicSplineInterpolation, BicubicInterpolation, BilinearInterpolation,
 * NearestNeighborInterpolation
 */
template <typename T>
class PCL_CLASS BicubicBSplineInterpolation : public BicubicInterpolationBase<T>
{
public:

   /*!
    * Constructs a %BicubicBSplineInterpolation instance.
    */
   BicubicBSplineInterpolation() = default;

   /*!
    * Virtual destructor.
    */
   virtual ~BicubicBSplineInterpolation()
   {
   }

   /*!
    * Interpolated value at \a {x,y} location.
    *
    * \param x,y  %Coordinates of the interpolation point (horizontal,
    *             vertical).
    */
   virtual double operator()( double x, double y ) const
   {
      PCL_PRECONDITION( f != 0 )
      PCL_PRECONDITION( m_width > 0 && m_height > 0 )

      // Initialize grid coordinates and source matrix
      int i1, j1;
      double p0[ 4 ], p1[ 4 ], p2[ 4 ], p3[ 4 ];
      InitXY( i1, j1, p0, p1, p2, p3, x, y );

      // Bicubic B-Spline interpolation functions

      double dx  =  x - j1;
      double dx0 = -1 - dx;
      double dx1 =     -dx;
      double dx2 =  1 - dx;
      double dx3 =  2 - dx;

      double dy  =  y - i1;
      double dy0 = -1 - dy;
      double dy1 =     -dy;
      double dy2 =  1 - dy;
      double dy3 =  2 - dy;

      return
      BXY( p0, 0, dx0, dy0 ) +
      BXY( p0, 1, dx1, dy0 ) +
      BXY( p0, 2, dx2, dy0 ) +
      BXY( p0, 3, dx3, dy0 ) +
      BXY( p1, 0, dx0, dy1 ) +
      BXY( p1, 1, dx1, dy1 ) +
      BXY( p1, 2, dx2, dy1 ) +
      BXY( p1, 3, dx3, dy1 ) +
      BXY( p2, 0, dx0, dy2 ) +
      BXY( p2, 1, dx1, dy2 ) +
      BXY( p2, 2, dx2, dy2 ) +
      BXY( p2, 3, dx3, dy2 ) +
      BXY( p3, 0, dx0, dy3 ) +
      BXY( p3, 1, dx1, dy3 ) +
      BXY( p3, 2, dx2, dy3 ) +
      BXY( p3, 3, dx3, dy3 );
   }

private:

   double BXY( const double* p, int j, double x, double y ) const
   {
      return p[j]*B( x )*B( y );
   }

   double B( double x ) const
   {
      double fx = (x > 0) ? x*x*x : 0;

      double fxp1 = x + 1;
      fxp1 = (fxp1 > 0) ? fxp1*fxp1*fxp1 : 0;

      double fxp2 = x + 2;
      fxp2 = (fxp2 > 0) ? fxp2*fxp2*fxp2 : 0;

      double fxm1 = x - 1;
      fxm1 = (fxm1 > 0) ? fxm1*fxm1*fxm1 : 0;

      return (fxp2 - 4*fxp1 + 6*fx - 4*fxm1)/6;
   }
};

// ----------------------------------------------------------------------------

#undef InitXY
#undef InitYX

// ----------------------------------------------------------------------------

#undef m_width
#undef m_height
#undef m_fillBorder
#undef m_fillValue
#undef m_data

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BicubicInterpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/BicubicInterpolation.h - Released 2015/12/17 18:52:09 UTC
