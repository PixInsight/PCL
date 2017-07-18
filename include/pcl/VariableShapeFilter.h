//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// pcl/VariableShapeFilter.h - Released 2017-07-18T16:13:52Z
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

#ifndef __PCL_VariableShapeFilter_h
#define __PCL_VariableShapeFilter_h

/// \file pcl/VariableShapeFilter.h

#include <pcl/Defs.h>

#include <pcl/AutoPointer.h>
#include <pcl/KernelFilter.h>
#include <pcl/Math.h>
#include <pcl/PixelInterpolation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class VariableShapeFilter
 * \brief A kernel filter with variable kurtosis.
 *
 * A %VariableShapeFilter object is a specialized KernelFilter whose elements
 * are calculated as a discrete representation of the following elliptical
 * function centered at the origin:
 *
 * G(x,y) = Exp( -( x^k/(k*sx^k) + y^k/(k*sy^k) ) )
 *
 * where sx and sy are the standard deviations of the filter distribution on
 * the horizontal and vertical axes, respectively, and k is a <em>shape
 * parameter</em> controlling the kurtosis of the filter function. When k < 2
 * the distribution is leptokurtic (peaked), while k > 2 leads to a platykurtic
 * (flat) distribution. When k = 2 the distribution is normal (Gaussian) and
 * the %VariableShapeFilter is equivalent to a GaussianFilter object with the
 * same standard deviations.
 *
 * %VariableShapeFilter also supports arbitrary rotation around the origin.
 * When the filter is rotated, the coordinates x, y in the equation above are
 * replaced by their rotated counterparts.
 *
 * A %VariableShapeFilter instance is formally defined by the following set of
 * parameters:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>\e sigma</td>   <td>Standard deviation of the filter distribution on the X axis (sigma > 0).</td></tr>
 * <tr><td>\e shape</td>   <td>Filter shape: 2 = Gaussian, \< 2 = leptokurtic, \> 2 = platykurtic (shape > 0).</td></tr>
 * <tr><td>\e rho</td>     <td>The ratio sy/sx (see equation above) of the generated filter distribution (0 <= rho <= 1).</td></tr>
 * <tr><td>\e theta</td>   <td>Rotation angle of the horizontal axis in radians (0 <= theta < PI). This parameter only makes sense when rho < 1.</td></tr>
 * <tr><td>\e epsilon</td> <td>Maximum truncation error of the computed filter coefficients (eps > 0).</td></tr>
 * </table>
 *
 * \sa KernelFilter, GaussianFilter, MoffatFilter, LinearFilter
 */
class PCL_CLASS VariableShapeFilter : public KernelFilter
{
public:

   /*!
    * Constructs an empty %VariableShapeFilter object with default functional
    * parameters: sigma=2, shape=2, epsilon=0.01, rho=1, theta=0.
    */
   VariableShapeFilter() :
      KernelFilter(),
      m_sigma( 2 ), m_shape( 2 ), m_rho( 1 ), m_theta( 0 ), m_epsilon( 0.01F )
   {
   }

   /*!
    * Constructs a %VariableShapeFilter object given the standard deviation
    * \a sigma > 0, \a shape > 0, and coefficient truncation \a epsilon > 0.
    * Assigns an optional \a name to the new filter object.
    */
   VariableShapeFilter( float sigma, float shape = 2, float epsilon = 0.01, const String& name = String() ) :
      KernelFilter()
   {
      Initialize( sigma, shape, epsilon, 1, 0 );
      Rename( name );
   }

   /*!
    * Constructs a %VariableShapeFilter object given the standard deviation
    * \a sigma > 0, \a shape > 0, coefficient truncation error \a epsilon > 0,
    * aspect ratio 0 <= \a rho <= 1, and rotation angle 0 <= \a theta <= PI in
    * radians. Assigns an optional \a name to the new filter object.
    */
   VariableShapeFilter( float sigma, float shape, float epsilon, float rho, float theta = 0, const String& name = String() ) :
      KernelFilter()
   {
      Initialize( sigma, shape, epsilon, rho, theta );
      Rename( name );
   }

   /*!
    * Constructs a %VariableShapeFilter object given the odd kernel size
    * \a n >= 3, \a shape > 0, and coefficient truncation error \a epsilon > 0.
    * Assigns an optional \a name to the new filter object.
    */
   VariableShapeFilter( int n, float shape = 2, float epsilon = 0.01, const String& name = String() ) :
      KernelFilter()
   {
      Initialize( n, shape, epsilon, 1, 0 );
      Rename( name );
   }

   /*!
    * Constructs a %VariableShapeFilter object given the odd kernel size
    * \a n >= 3, \a shape > 0, coefficient truncation \a epsilon > 0, aspect
    * ratio 0 <= \a rho <= 1, and rotation angle 0 <= \a theta <= PI in
    * radians. Assigns an optional \a name to the new filter object.
    */
   VariableShapeFilter( int n, float shape, float epsilon, float rho, float theta = 0, const String& name = String() ) :
      KernelFilter()
   {
      Initialize( n, shape, epsilon, rho, theta );
      Rename( name );
   }

   /*!
    * Copy constructor.
    */
   VariableShapeFilter( const VariableShapeFilter& x ) :
      KernelFilter( x ),
      m_sigma( x.m_sigma ), m_shape( x.m_shape ), m_rho( x.m_rho ), m_theta( x.m_theta ), m_epsilon( x.m_epsilon )
   {
   }

   /*!
    * Move constructor.
    */
   VariableShapeFilter( VariableShapeFilter&& x ) :
      KernelFilter( std::move( x ) ),
      m_sigma( x.m_sigma ), m_shape( x.m_shape ), m_rho( x.m_rho ), m_theta( x.m_theta ), m_epsilon( x.m_epsilon )
   {
   }

   /*!
    * Returns a pointer to a dynamically allocated duplicate of this kernel
    * filter.
    */
   virtual KernelFilter* Clone() const
   {
      return new VariableShapeFilter( *this );
   }

   /*!
    * Returns a separable filter equivalent to this %VariableShapeFilter
    * object.
    *
    * A %VariableShapeFilter is separable when it represents an undistorted
    * Gaussian distribution. This is only true when shape=2 and the filter is
    * circular (rho=1). Otherwise an empty SeparableFilter object is returned
    * because this filter is not separable.
    */
   virtual SeparableFilter AsSeparableFilter( float tolerance = __PCL_DEFAULT_FILTER_SEPARABILITY_TOLERANCE ) const
   {
      if ( m_shape == 2 && m_rho == 1 )
      {
         FVector v = coefficients.RowVector( Size()>>1 );
         return SeparableFilter( v, v );
      }
      return SeparableFilter();
   }

   /*!
    * Returns true iff this filter is separable.
    *
    * A %VariableShapeFilter is separable only when it represents an undistorted
    * Gaussian distribution. This is only true when shape=2 and the filter is
    * circular (rho=1).
    */
   virtual bool IsSeparable() const
   {
      return m_shape == 2 && m_rho == 1;
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   VariableShapeFilter& operator =( const VariableShapeFilter& g )
   {
      (void)KernelFilter::operator =( g );
      m_sigma = g.m_sigma;
      m_shape = g.m_shape;
      m_rho = g.m_rho;
      m_theta = g.m_theta;
      m_epsilon = g.m_epsilon;
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   VariableShapeFilter& operator =( VariableShapeFilter&& g )
   {
      (void)KernelFilter::operator =( std::move( g ) );
      m_sigma = g.m_sigma;
      m_shape = g.m_shape;
      m_rho = g.m_rho;
      m_theta = g.m_theta;
      m_epsilon = g.m_epsilon;
      return *this;
   }

   /*!
    * Returns the standard deviation of the filter distribution on the X
    * (horizontal) axis.
    */
   float SigmaX() const
   {
      return m_sigma;
   }

   /*!
    * Returns the standard deviation of the filter distribution on the Y
    * (vertical) axis.
    */
   float SigmaY() const
   {
      return m_rho*m_sigma;
   }

   /*!
    * Returns the standard deviation of the filter distribution on the X
    * (horizontal) axis.
    *
    * This function is an alias to SigmaX().
    */
   float Sigma() const
   {
      return SigmaX();
   }

   /*!
    * Returns the \e shape parameter of this %VariableShapeFilter object. The
    * shape parameter controls the kurtosis of the filter distribution:
    *
    * <table border="1" cellpadding="4" cellspacing="0">
    * <tr><td>shape \< 2</td> <td>Leptokurtic distribution</td></tr>
    * <tr><td>shape = 2</td>  <td>Normal (Gaussian) distribution</td></tr>
    * <tr><td>shape \> 2</td> <td>Platykurtic distribution</td></tr>
    * </table>
    */
   float Shape() const
   {
      return m_shape;
   }

   /*!
    * Returns the maximum truncation error of calculated filter coefficients.
    */
   float Truncation() const
   {
      return m_epsilon;
   }

   /*!
    * Returns the aspect ratio of the filter distribution. This is the ratio
    * vertical:horizontal between filter axes in the range [0,1].
    */
   float AspectRatio() const
   {
      return m_rho;
   }

   /*!
    * Returns the rotation angle of the filter distribution. This is the
    * rotation angle in radians with respect to the central pixel, in the
    * range [0,+PI].
    */
   float RotationAngle() const
   {
      return m_theta;
   }

   /*!
    * Returns the full width at half maximum (FWHM), in sigma units, for the
    * horizontal axis of the elliptical filter distribution.
    */
   double FWHMx() const
   {
      return 2 * m_sigma * Pow( m_shape*0.6931471805599453, 1.0/m_shape );

   }

   /*!
    * Returns the full width at half maximum (FWHM), in sigma units, for the
    * vertical axis of the elliptical filter distribution.
    */
   double FWHMy() const
   {
      return m_rho * FWHMx();

   }

   /*!
    * Returns the full width at half maximum, in sigma units, for the
    * horizontal axis of the elliptical filter distribution.
    *
    * This function is an alias to FWHMx().
    */
   double FWHM() const
   {
      return FWHMx();
   }

   /*!
    * Recalculates filter coefficients for the specified \a sigma > 0,
    * \a shape > 0, coefficient truncation error \a epsilon > 0, aspect ratio
    * 0 <= \a rho <= 1, and rotation angle 0 <= \a theta <= PI in radians.
    */
   void Set( float sigma, float shape, float epsilon, float rho, float theta )
   {
      Initialize( sigma, shape, epsilon, rho, theta );
   }

   /*!
    * Recalculates filter coefficients for the specified \a sigma > 0,
    * \a shape > 0, coefficient truncation error \a epsilon > 0, and aspect
    * ratio 0 <= \a rho <= 1. Does not change the current rotation angle.
    */
   void Set( float sigma, float shape, float epsilon, float rho )
   {
      Initialize( sigma, shape, epsilon, rho, m_theta );
   }

   /*!
    * Recalculates filter coefficients for the specified \a sigma > 0,
    * \a shape > 0, and coefficient truncation error \a epsilon > 0. Does not
    * change the current aspect ratio and rotation angle.
    */
   void Set( float sigma, float shape, float epsilon )
   {
      Initialize( sigma, shape, epsilon, m_rho, m_theta );
   }

   /*!
    * Recalculates filter coefficients for the specified \a sigma > 0 and
    * \a shape > 0. Coefficient truncation, aspect ratio and rotation angle are
    * not changed.
    */
   void Set( float sigma, float shape )
   {
      Initialize( sigma, shape, m_epsilon, m_rho, m_theta );
   }

   /*!
    * Recalculates filter coefficients for the specified \a sigma > 0. The
    * current shape, coefficient truncation error, aspect ratio and rotation
    * angle are not changed.
    */
   void Set( float sigma )
   {
      Initialize( sigma, m_shape, m_epsilon, m_rho, m_theta );
   }

   /*!
    * This is a convenience member function, equivalent to Set( sigma ).
    */
   void SetSigma( float sigma )
   {
      Set( sigma );
   }

   /*!
    * This is a convenience member function, equivalent to
    * Set( Sigma(), shape ).
    */
   void SetShape( float shape )
   {
      Set( m_sigma, shape );
   }

   /*!
    * This is a convenience member function, equivalent to
    * Set( Sigma(), Shape(), epsilon ).
    */
   void SetTruncation( float epsilon )
   {
      Set( m_sigma, m_shape, epsilon );
   }

   /*!
    * This is a convenience member function, equivalent to
    * Set( Sigma(), Shape(), Truncation(), rho ).
    */
   void SetAspectRatio( float rho )
   {
      Set( m_sigma, m_shape, m_epsilon, rho );
   }

   /*!
    * This is a convenience member function, equivalent to
    * Set( Sigma(), Shape(), Truncation(), AspectRatio(), theta ).
    */
   void SetRotationAngle( float theta )
   {
      Set( m_sigma, m_shape, m_epsilon, m_rho, theta );
   }

   /*!
    * Recalculates filter coefficients for the given odd kernel size \a n >= 3.
    * This routine computes the required standard deviation to sample the
    * filter function on a matrix of the specified size, preserving the
    * current shape, coefficient truncation, aspect ratio and rotation angle.
    */
   virtual void Resize( int n )  // Inherited from KernelFilter
   {
      Initialize( n, m_shape, m_epsilon, m_rho, m_theta );
   }

   // -------------------------------------------------------------------------

private:

   float m_sigma;   // standard deviation, horizontal axis
   float m_shape;   // controls the kurtosis of the filter distribution (2=normal)
   float m_rho;     // vertical:horizontal axes ratio
   float m_theta;   // rotation angle in radians, [0,+pi]
   float m_epsilon; // maximum truncation error in sigma units

   void Initialize( float s, float k, float e, float r, float a )
   {
      PCL_PRECONDITION( s > 0 )
      PCL_PRECONDITION( k > 0 )
      PCL_PRECONDITION( e > 0 )
      PCL_PRECONDITION( r >= 0 && r <= 1 )
      PCL_PRECONDITION( a >= 0 && a <= Const<float>::pi() )
      m_sigma = s;
      m_shape = k;
      m_epsilon = Abs( e );
      m_rho = Range( r, 0.0F, 1.0F );
      m_theta = Range( a, 0.0F, Const<float>::pi() );
      KernelFilter::Resize( 1 + (Max( 1, RoundInt( Pow( -m_shape*Pow( m_sigma, m_shape )*Ln( m_epsilon ), 1/m_shape ) ) ) << 1) );
      Rebuild();
   }

   void Initialize( int n, float k, float e, float r, float a )
   {
      PCL_PRECONDITION( n == 0 || n >= 3 && (n & 1) != 0 )
      PCL_PRECONDITION( k > 0 )
      PCL_PRECONDITION( e > 0 )
      PCL_PRECONDITION( r >= 0 && r <= 1 )
      PCL_PRECONDITION( a >= 0 && a <= Const<float>::pi() )
      KernelFilter::Resize( n );
      m_shape = k;
      m_epsilon = Abs( e );
      m_sigma = (Size() >> 1)/Pow( -m_shape*Ln( m_epsilon ), 1/m_shape );
      m_rho = Range( r, 0.0F, 1.0F );
      m_theta = Range( a, 0.0F, Const<float>::pi() );
      Rebuild();
   }

   void Rebuild()
   {
      int size = Size();
      if ( size == 0 )
         return;

      float* h = *coefficients;
      float rk = m_shape * Pow( m_sigma, m_shape );

      for ( int n2 = size >> 1, y = -n2; y <= n2; ++y )
         if ( y > 0 )
            for ( int x = 0; x < size; ++x, ++h )
               *h = *(h - ((y+y)*size));
         else
            for ( int x = -n2; x <= n2; ++x, ++h )
               *h = (x > 0) ? *(h - (x+x)) : float( Exp( -Pow( Sqrt( float( x*x + y*y ) ), m_shape )/rk ) );

      if ( m_rho != 1 )
      {
         BicubicPixelInterpolation B;

         {
            coefficient_matrix ctemp( coefficients );
            ctemp.EnsureUnique(); // because we need invariant *ctemp for interpolation
            AutoPointer<PixelInterpolation::Interpolator<FloatPixelTraits> >
               interpolator( B.NewInterpolator<FloatPixelTraits>( *ctemp, size, size ) );
            h = *coefficients;
            float r = Max( 0.5F/size, m_rho );
            for ( int n2 = size >> 1, i = -n2; i <= n2; ++i )
            {
               float y = float( i )/r + n2;
               for ( int x = 0; x < size; ++x )
                  *h++ = (y < 0 || y >= size) ? 0.0F : (*interpolator)( float( x ), y );
            }
         }

         if ( m_theta != 0 )
         {
            coefficient_matrix ctemp( coefficients );
            ctemp.EnsureUnique(); // because we need invariant *ctemp for interpolation
            AutoPointer<PixelInterpolation::Interpolator<FloatPixelTraits> >
               interpolator( B.NewInterpolator<FloatPixelTraits>( *ctemp, size, size ) );
            h = *coefficients;
            float sa, ca;
            SinCos( -m_theta, sa, ca );
            for ( int n2 = size >> 1, i = -n2; i <= n2; ++i )
               for ( int j = -n2; j <= n2; ++j )
               {
                  float x =  ca*j + sa*i + n2;
                  float y = -sa*j + ca*i + n2;
                  *h++ = (x < 0 || y < 0 || x >= size || y >= size) ? 0.0F : (*interpolator)( x, y );
               }
         }
      }
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_VariableShapeFilter_h

// ----------------------------------------------------------------------------
// EOF pcl/VariableShapeFilter.h - Released 2017-07-18T16:13:52Z
