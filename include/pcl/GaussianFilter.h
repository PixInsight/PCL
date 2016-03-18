//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// pcl/GaussianFilter.h - Released 2016/02/21 20:22:12 UTC
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

#ifndef __PCL_GaussianFilter_h
#define __PCL_GaussianFilter_h

/// \file pcl/GaussianFilter.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Math_h
#include <pcl/Math.h>
#endif

#ifndef __PCL_KernelFilter_h
#include <pcl/KernelFilter.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class GaussianFilter
 * \brief A kernel filter implementing a discrete Gaussian distribution in two dimensions.
 *
 * A %GaussianFilter object is a specialized KernelFilter whose elements are
 * calculated as a discrete representation of an elliptical Gaussian function
 * centered at the origin:
 *
 * G(x,y) = Exp( -( x^2/(2*sx^2) + y^2/(2*sy^2) ) )
 *
 * where sx and sy are the standard deviations of the Gaussian distribution on
 * the horizontal and vertical axes, respectively.
 *
 * %GaussianFilter also supports arbitrary rotation around the origin. When the
 * filter is rotated, the coordinates x, y in the equation above are replaced
 * by their rotated counterparts.
 *
 * A %GaussianFilter instance is formally defined by the following parameters:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>\e sigma</td>   <td>Standard deviation of the filter distribution on the X axis (sigma > 0).</td></tr>
 * <tr><td>\e rho</td>     <td>The ratio sy/sx (see equation above) of the generated filter distribution (0 <= rho <= 1).</td></tr>
 * <tr><td>\e theta</td>   <td>Rotation angle of the horizontal axis in radians (0 <= theta < PI). This parameter only makes sense when rho < 1.</td></tr>
 * <tr><td>\e epsilon</td> <td>Maximum truncation error of the computed filter coefficients (eps > 0).</td></tr>
 * </table>
 *
 * \sa KernelFilter, VariableShapeFilter, MoffatFilter, LinearFilter
 */
class PCL_CLASS GaussianFilter : public KernelFilter
{
public:

   /*!
    * Constructs an empty %GaussianFilter object with default functional
    * parameters: sigma=2, epsilon=0.01, rho=1, theta=0.
    */
   GaussianFilter() :
      KernelFilter(),
      m_sigma( 2 ), m_rho( 1 ), m_theta( 0 ), m_epsilon( 0.01F )
   {
   }

   /*!
    * Constructs a %GaussianFilter object given the standard deviation
    * \a sigma > 0 and truncation error \a epsilon > 0. Assigns an optional
    * \a name to the new filter object.
    */
   GaussianFilter( float sigma, float epsilon = 0.01, const String& name = String() ) :
      KernelFilter()
   {
      Initialize( sigma, epsilon, 1, 0 );
      Rename( name );
   }

   /*!
    * Constructs a %GaussianFilter object given the standard deviation
    * \a sigma > 0, truncation error \a epsilon > 0, aspect ratio
    * 0 <= \a rho <= 1, and rotation angle 0 <= \a theta <= PI in radians.
    * Assigns an optional \a name to the new filter object.
    */
   GaussianFilter( float sigma, float epsilon, float rho, float theta = 0, const String& name = String() ) :
      KernelFilter()
   {
      Initialize( sigma, epsilon, rho, theta );
      Rename( name );
   }

   /*!
    * Constructs a %GaussianFilter object given the odd kernel size \a n >= 3
    * and truncation error \a epsilon > 0. Assigns an optional \a name to the
    * new filter object.
    */
   GaussianFilter( int n, float epsilon = 0.01, const String& name = String() ) :
      KernelFilter()
   {
      Initialize( n, epsilon, 1, 0 );
      Rename( name );
   }

   /*!
    * Constructs a %GaussianFilter object given the odd kernel size \a n >= 3,
    * truncation error \a epsilon > 0, aspect ratio 0 <= \a rho <= 1, and
    * rotation angle 0 <= \a theta <= PI in radians. Assigns an optional
    * \a name to the new filter object.
    */
   GaussianFilter( int n, float epsilon, float rho, float theta = 0, const String& name = String() ) :
      KernelFilter()
   {
      Initialize( n, epsilon, rho, theta );
      Rename( name );
   }

   /*!
    * Copy constructor.
    */
   GaussianFilter( const GaussianFilter& x ) :
      KernelFilter( x ),
      m_sigma( x.m_sigma ), m_rho( x.m_rho ), m_theta( x.m_theta ), m_epsilon( x.m_epsilon )
   {
   }

   /*!
    * Move constructor.
    */
   GaussianFilter( GaussianFilter&& x ) :
      KernelFilter( std::move( x ) ),
      m_sigma( x.m_sigma ), m_rho( x.m_rho ), m_theta( x.m_theta ), m_epsilon( x.m_epsilon )
   {
   }

   /*!
    */
   virtual KernelFilter* Clone() const
   {
      return new GaussianFilter( *this );
   }

   /*!
    * Returns a separable filter equivalent to this Gaussian kernel filter.
    *
    * A Gaussian filter is the only circularly symmetric, two-dimensional
    * separable filter. This reimplementation returns a separable filter only
    * if this object represents an undistorted/unrotated Gaussian function,
    * that is, when its aspect ratio is 1. Otherwise an empty SeparableFilter
    * object is returned since the filter matrix is not separable.
    */
   virtual SeparableFilter AsSeparableFilter( float tolerance = __PCL_DEFAULT_FILTER_SEPARABILITY_TOLERANCE ) const
   {
      if ( m_rho == 1 )
      {
         FVector v = coefficients.RowVector( Size()>>1 );
         return SeparableFilter( v, v );
      }
      return SeparableFilter();
   }

   /*!
    * Returns true iff this filter is separable,
    *
    * A Gaussian filter is separable. As reimplemented in %GaussianFilter, this
    * member function returns true for undistorted/unrotated Gaussian filters,
    * that is, when this object's aspect ratio is 1.
    */
   virtual bool IsSeparable() const
   {
      return m_rho == 1;
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   GaussianFilter& operator =( const GaussianFilter& x )
   {
      (void)KernelFilter::operator =( x );
      m_sigma   = x.m_sigma;
      m_epsilon = x.m_epsilon;
      m_rho     = x.m_rho;
      m_theta   = x.m_theta;
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   GaussianFilter& operator =( GaussianFilter&& x )
   {
      (void)KernelFilter::operator =( std::move( x ) );
      m_sigma   = x.m_sigma;
      m_epsilon = x.m_epsilon;
      m_rho     = x.m_rho;
      m_theta   = x.m_theta;
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
    * Returns the maximum truncation error of the filter coefficients.
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
    * horizontal axis of the elliptical Gaussian filter distribution.
    */
   double FWHMx() const
   {
      return 2.3548200450309493 * m_sigma;

   }

   /*!
    * Returns the full width at half maximum (FWHM), in sigma units, for the
    * vertical axis of the elliptical Gaussian filter distribution.
    */
   double FWHMy() const
   {
      return m_rho * FWHMx();

   }

   /*!
    * Returns the full width at half maximum, in sigma units, for the
    * horizontal axis of the elliptical Gaussian filter distribution.
    *
    * This function is an alias to FWHMx().
    */
   double FWHM() const
   {
      return FWHMx();
   }

   /*!
    * Recalculates filter coefficients for the specified sigma \a sigma > 0,
    * truncation error \a epsilon > 0, aspect ratio 0 <= \a rho <= 1, and
    * rotation angle 0 <= \a theta <= PI in radians.
    */
   void Set( float sigma, float epsilon, float rho, float theta )
   {
      Initialize( sigma, epsilon, rho, theta );
   }

   /*!
    * Recalculates filter coefficients for the specified sigma \a sigma > 0,
    * truncation error \a epsilon > 0, and aspect ratio 0 <= \a rho <= 1. Does
    * not change the current rotation angle.
    */
   void Set( float sigma, float epsilon, float rho )
   {
      Initialize( sigma, epsilon, rho, m_theta );
   }

   /*!
    * Recalculates filter coefficients for the specified sigma \a sigma > 0 and
    * truncation error \a epsilon > 0. Does not change the current aspect ratio
    * and rotation angle.
    */
   void Set( float sigma, float epsilon )
   {
      Initialize( sigma, epsilon, m_rho, m_theta );
   }

   /*!
    * Recalculates filter coefficients for the specified sigma \a sigma > 0.
    * The current coefficient truncation error, aspect ratio and rotation angle
    * are not changed.
    */
   void Set( float sigma )
   {
      Initialize( sigma, m_epsilon, m_rho, m_theta );
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
    * Set( Sigma(), epsilon ).
    */
   void SetTruncation( float epsilon )
   {
      Set( m_sigma, epsilon );
   }

   /*!
    * This is a convenience member function, equivalent to
    * Set( Sigma(), Truncation(), rho ).
    */
   void SetAspectRatio( float rho )
   {
      Set( m_sigma, m_epsilon, rho );
   }

   /*!
    * This is a convenience member function, equivalent to
    * Set( Sigma(), Truncation(), AspectRatio(), theta ).
    */
   void SetRotationAngle( float theta )
   {
      Set( m_sigma, m_epsilon, m_rho, theta );
   }

   /*!
    * Recalculates filter coefficients for the given odd kernel size \a n >= 3.
    * This routine computes the required standard deviation to sample the
    * Gaussian function on a matrix of the specified size, preserving the
    * current coefficient truncation, aspect ratio and rotation angle.
    */
   virtual void Resize( int n )  // Inherited from KernelFilter
   {
      Initialize( n, m_epsilon, m_rho, m_theta );
   }

   /*!
    * Exchanges two Gaussian filters \a x1 and \a x2.
    */
   friend void Swap( GaussianFilter& x1, GaussianFilter& x2 )
   {
      pcl::Swap( static_cast<KernelFilter&>( x1 ), static_cast<KernelFilter&>( x2 ) );
      pcl::Swap( x1.m_sigma,   x2.m_sigma );
      pcl::Swap( x1.m_rho,     x2.m_rho );
      pcl::Swap( x1.m_theta,   x2.m_theta );
      pcl::Swap( x1.m_epsilon, x2.m_epsilon );
   }

private:

   float m_sigma;   // standard deviation, horizontal axis
   float m_rho;     // vertical:horizontal axes ratio
   float m_theta;   // rotation angle in radians, [0,+pi]
   float m_epsilon; // maximum truncation error in sigma units

   void Initialize( float s, float e, float r, float a )
   {
      PCL_PRECONDITION( s > 0 )
      PCL_PRECONDITION( e > 0 )
      PCL_PRECONDITION( r >= 0 && r <= 1 )
      PCL_PRECONDITION( a >= 0 && a <= Const<float>::pi() )
      m_sigma = Abs( s );
      m_epsilon = Abs( e );
      m_rho = Range( r, 0.0F, 1.0F );
      m_theta = Range( a, 0.0F, Const<float>::pi() );
      KernelFilter::Resize( 1 + (Max( 1, RoundI( m_sigma * Sqrt( -2*Ln( m_epsilon ) ) ) ) << 1) );
      Rebuild();
   }

   void Initialize( int n, float e, float r, float a )
   {
      PCL_PRECONDITION( n == 0 || n >= 3 && (n & 1) != 0 )
      PCL_PRECONDITION( e > 0 )
      PCL_PRECONDITION( r >= 0 && r <= 1 )
      PCL_PRECONDITION( a >= 0 && a <= Const<float>::pi() )
      KernelFilter::Resize( n );
      m_epsilon = Abs( e );
      m_sigma = (Size() >> 1)/Sqrt( -2*Ln( m_epsilon ) );
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
      double sx = m_sigma;
      double sy = m_rho * sx;

      if ( m_theta == 0 || m_rho == 1 )
      {
         double twosx2 = 2*sx*sx;
         double twosy2 = 2*sy*sy;
         for ( int n2 = size >> 1, dy = -n2; dy <= n2; ++dy )
            if ( dy > 0 )
               for ( int dx = 0; dx < size; ++dx, ++h )
                  *h = *(h - ((dy+dy)*size));
            else
               for ( int dx = -n2; dx <= n2; ++dx, ++h )
                  *h = (dx > 0) ? *(h - (dx+dx)) : float( Exp( -(dx*dx/twosx2 + dy*dy/twosy2) ) );
      }
      else
      {
         double st, ct; SinCos( double( m_theta ), st, ct );
         double sct    = st*ct;
         double st2    = st*st;
         double ct2    = ct*ct;
         double twosx2 = 2*sx*sx;
         double twosy2 = 2*sy*sy;
         double p1     = ct2/twosx2 + st2/twosy2;
         double p2     = sct/twosy2 - sct/twosx2;
         double p3     = st2/twosx2 + ct2/twosy2;
         for ( int n2 = size >> 1, dy = -n2; dy <= n2; ++dy )
         {
            double twop2dy = 2*p2*dy;
            double p3dy2   = p3*dy*dy;
            for ( int dx = -n2; dx <= n2; ++dx, ++h )
               *h = float( Exp( -(p1*dx*dx + twop2dy*dx + p3dy2) ) );
         }
      }
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_GaussianFilter_h

// ----------------------------------------------------------------------------
// EOF pcl/GaussianFilter.h - Released 2016/02/21 20:22:12 UTC
