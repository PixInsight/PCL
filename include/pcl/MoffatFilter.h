//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// pcl/MoffatFilter.h - Released 2018-12-12T09:24:21Z
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

#ifndef __PCL_MoffatFilter_h
#define __PCL_MoffatFilter_h

/// \file pcl/MoffatFilter.h

#include <pcl/Defs.h>

#include <pcl/KernelFilter.h>
#include <pcl/Math.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class MoffatFilter
 * \brief A kernel filter implementing a discrete Moffat distribution in two dimensions.
 *
 * A %MoffatFilter object is a specialized KernelFilter whose elements are
 * calculated as a discrete representation of an elliptical Moffat[1] function
 * centered at the origin:
 *
 * M(x,y) = 1/(1 + x^2/sx^2 + y^2/sy^2)^beta
 *
 * where sx and sy are the standard deviations of the distribution on the
 * horizontal and vertical axes, respectively, and the beta exponent controls
 * the shape of the function's profile. The lower the beta, the more peaked the
 * overall shape of the function. For beta=1, the equation above corresponds to
 * a Lorentzian function.
 *
 * %MoffatFilter also supports arbitrary rotation around the origin. When the
 * filter is rotated, the coordinates x, y in the equation above are replaced
 * by their rotated counterparts.
 *
 * A %MoffatFilter instance is formally defined by the following parameters:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>\e sigma</td>   <td>Standard deviation of the filter distribution on the X axis (sigma > 0).</td></tr>
 * <tr><td>\e rho</td>     <td>The ratio sy/sx (see equation above) of the generated filter distribution (0 <= rho <= 1).</td></tr>
 * <tr><td>\e theta</td>   <td>Rotation angle of the horizontal axis in radians (0 <= theta < PI). This parameter only makes sense when rho < 1.</td></tr>
 * <tr><td>\e beta</td>    <td>Exponent controlling the overall shape of the function (beta > 0).</td></tr>
 * <tr><td>\e epsilon</td> <td>Maximum truncation error of the computed filter coefficients (eps > 0).</td></tr>
 * </table>
 *
 * <b>References</b>
 *
 * \li [1] Moffat, A. F. J., <em>A Theoretical Investigation of Focal Stellar
 * Images in the Photographic Emulsion and Application to Photographic
 * Photometry</em>, Astronomy and Astrophysics, Vol. 3, p. 455 (1969)
 *
 * \sa KernelFilter, GaussianFilter, VariableShapeFilter, LinearFilter
 */
class PCL_CLASS MoffatFilter : public KernelFilter
{
public:

   /*!
    * Constructs an empty %MoffatFilter object with default functional
    * parameters: sigma=2, beta=4, epsilon=0.01, rho=1, theta=0.
    */
   MoffatFilter() = default;

   /*!
    * Constructs a %MoffatFilter object given the standard deviation
    * \a sigma > 0, \a beta exponent > 0 and truncation error \a epsilon > 0.
    * Assigns an optional \a name to the new filter object.
    */
   MoffatFilter( float sigma, float beta = 4, float epsilon = 0.01, const String& name = String() )
   {
      Initialize( sigma, beta, epsilon, 1, 0 );
      Rename( name );
   }

   /*!
    * Constructs a %MoffatFilter object given the standard deviation
    * \a sigma > 0, \a beta exponent > 0, truncation error \a epsilon > 0,
    * aspect ratio 0 <= \a rho <= 1, and rotation angle 0 <= \a theta <= PI in
    * radians. Assigns an optional \a name to the new filter object.
    */
   MoffatFilter( float sigma, float beta, float epsilon, float rho, float theta = 0, const String& name = String() )
   {
      Initialize( sigma, beta, epsilon, rho, theta );
      Rename( name );
   }

   /*!
    * Constructs a %MoffatFilter object given the odd kernel size \a n >= 3,
    * \a beta exponent > 0 and truncation error \a epsilon > 0. Assigns an
    * optional \a name to the new filter object.
    */
   MoffatFilter( int n, float beta = 4, float epsilon = 0.01, const String& name = String() )
   {
      Initialize( n, beta, epsilon, 1, 0 );
      Rename( name );
   }

   /*!
    * Constructs a %MoffatFilter object given the odd kernel size \a n >= 3,
    * \a beta exponent > 0, truncation error \a epsilon > 0, aspect ratio
    * 0 <= \a rho <= 1, and rotation angle 0 <= \a theta <= PI in radians.
    * Assigns an optional \a name to the new filter object.
    */
   MoffatFilter( int n, float beta, float epsilon, float rho, float theta = 0, const String& name = String() )
   {
      Initialize( n, beta, epsilon, rho, theta );
      Rename( name );
   }

   /*!
    * Copy constructor.
    */
   MoffatFilter( const MoffatFilter& ) = default;

   /*!
    * Move constructor.
    */
   MoffatFilter( MoffatFilter&& ) = default;

   /*!
    */
   KernelFilter* Clone() const override
   {
      return new MoffatFilter( *this );
   }

   /*!
    * Returns a separable filter equivalent to this kernel filter.
    *
    * %MoffatFilter's reimplementation of this virtual member function returns
    * an empty SeparableFilter object, as a Moffat filter is not separable.
    */
   SeparableFilter AsSeparableFilter( float tolerance = __PCL_DEFAULT_FILTER_SEPARABILITY_TOLERANCE ) const override
   {
      return SeparableFilter();
   }

   /*!
    * Returns true iff this filter is separable.
    *
    * %MoffatFilter's reimplementation of this virtual member function returns
    * false, as a Moffat filter is not separable.
    */
   bool IsSeparable() const override
   {
      return false;
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   MoffatFilter& operator =( const MoffatFilter& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   MoffatFilter& operator =( MoffatFilter&& ) = default;

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
    * Returns the beta exponent of the Moffat filter function. The beta
    * exponent controls the overall shape of the filter profile.
    */
   float Beta() const
   {
      return m_beta;
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
    * horizontal axis of the elliptical Moffat filter distribution.
    */
   double FWHMx() const
   {
      return 2 * Sqrt( Pow2( 1/m_beta ) - 1 ) * m_sigma;

   }

   /*!
    * Returns the full width at half maximum (FWHM), in sigma units, for the
    * vertical axis of the elliptical Moffat filter distribution.
    */
   double FWHMy() const
   {
      return m_rho * FWHMx();

   }

   /*!
    * Returns the full width at half maximum, in sigma units, for the
    * horizontal axis of the elliptical Moffat filter distribution.
    *
    * This function is an alias to FWHMx().
    */
   double FWHM() const
   {
      return FWHMx();
   }

   /*!
    * Recalculates filter coefficients for the specified sigma \a sigma > 0,
    * \a beta exponent, truncation error \a epsilon > 0, aspect ratio
    * 0 <= \a rho <= 1 and rotation angle 0 <= \a theta <= PI in radians.
    */
   void Set( float sigma, float beta, float epsilon, float rho, float theta )
   {
      Initialize( sigma, beta, epsilon, rho, theta );
   }

   /*!
    * Recalculates filter coefficients for the specified sigma \a sigma > 0,
    * \a beta exponent, truncation error \a epsilon > 0 and aspect ratio
    * 0 <= \a rho <= 1. Does not change the current rotation angle.
    */
   void Set( float sigma, float beta, float epsilon, float rho )
   {
      Initialize( sigma, beta, epsilon, rho, m_theta );
   }

   /*!
    * Recalculates filter coefficients for the specified sigma \a sigma > 0,
    * \a beta exponent and truncation error \a epsilon > 0. Does not change the
    * current aspect ratio and rotation angle.
    */
   void Set( float sigma, float beta, float epsilon )
   {
      Initialize( sigma, beta, epsilon, m_rho, m_theta );
   }

   /*!
    * Recalculates filter coefficients for the specified sigma \a sigma > 0 and
    * \a beta exponent. The current coefficient truncation error, aspect ratio
    * and rotation angle are not changed.
    */
   void Set( float sigma, float beta )
   {
      Initialize( sigma, beta, m_epsilon, m_rho, m_theta );
   }

   /*!
    * Recalculates filter coefficients for the specified sigma \a sigma > 0.
    * The current beta exponent, coefficient truncation error, aspect ratio and
    * rotation angle are not changed.
    */
   void Set( float sigma )
   {
      Initialize( sigma, m_beta, m_epsilon, m_rho, m_theta );
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
    * Set( Sigma(), beta ).
    */
   void SetBeta( float beta )
   {
      Set( m_sigma, beta );
   }

   /*!
    * This is a convenience member function, equivalent to
    * Set( Sigma(), Beta(), epsilon ).
    */
   void SetTruncation( float epsilon )
   {
      Set( m_sigma, m_beta, epsilon );
   }

   /*!
    * This is a convenience member function, equivalent to
    * Set( Sigma(), Beta(), Truncation(), rho ).
    */
   void SetAspectRatio( float rho )
   {
      Set( m_sigma, m_beta, m_epsilon, rho );
   }

   /*!
    * This is a convenience member function, equivalent to
    * Set( Sigma(), Beta(), Truncation(), AspectRatio(), a ).
    */
   void SetRotationAngle( float theta )
   {
      Set( m_sigma, m_beta, m_epsilon, m_rho, theta );
   }

   /*!
    * Recalculates filter coefficients for the given odd kernel size \a n >= 3.
    * This routine computes the required standard deviation to sample the
    * Moffat function on a matrix of the specified size, preserving the
    * current beta exponent, coefficient truncation error, aspect ratio and
    * rotation angle.
    */
   void Resize( int n ) override
   {
      Initialize( n, m_beta, m_epsilon, m_rho, m_theta );
   }

private:

   float m_sigma = 2.0F;    // standard deviation, horizontal axis
   float m_beta = 4.0F;     // beta exponent
   float m_rho = 1.0F;      // vertical:horizontal axis ratio
   float m_theta = 0.0F;    // rotation angle in radians, [0,+pi]
   float m_epsilon = 0.01F; // maximum truncation error in sigma units

   void Initialize( float s, float b, float e, float r, float a )
   {
      PCL_PRECONDITION( s > 0 )
      PCL_PRECONDITION( b > 0 )
      PCL_PRECONDITION( e > 0 )
      PCL_PRECONDITION( r >= 0 && r <= 1 )
      PCL_PRECONDITION( a >= 0 && a <= Const<float>::pi() )
      m_sigma = Abs( s );
      m_beta = Abs( b );
      m_epsilon = Abs( e );
      m_rho = Range( r, 0.0F, 1.0F );
      m_theta = Range( a, 0.0F, Const<float>::pi() );
      KernelFilter::Resize( 1 + (Max( 1, RoundInt( m_sigma*Sqrt( Pow( m_epsilon, -1/m_beta ) - 1 ) ) ) << 1) );
      Rebuild();
   }

   void Initialize( int n, float b, float e, float r, float a )
   {
      PCL_PRECONDITION( n == 0 || n >= 3 && (n & 1) != 0 )
      PCL_PRECONDITION( b > 0 )
      PCL_PRECONDITION( e > 0 )
      PCL_PRECONDITION( r >= 0 && r <= 1 )
      PCL_PRECONDITION( a >= 0 && a <= Const<float>::pi() )
      KernelFilter::Resize( n );
      m_beta = Abs( b );
      m_epsilon = Abs( e );
      m_sigma = (Size() >> 1)/Sqrt( Pow( m_epsilon, -1/m_beta ) - 1 );
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
      double mb = -m_beta;

      if ( m_theta == 0 || m_rho == 1 )
      {
         double sx2 = sx*sx;
         double sy2 = sy*sy;
         for ( int n2 = size >> 1, dy = -n2; dy <= n2; ++dy )
            if ( dy > 0 )
               for ( int dx = 0; dx < size; ++dx, ++h )
                  *h = *(h - ((dy+dy)*size));
            else
               for ( int dx = -n2; dx <= n2; ++dx, ++h )
                  *h = (dx > 0) ? *(h - (dx+dx)) : float( Pow( 1 + dx*dx/sx2 + dy*dy/sy2, mb ) );
      }
      else
      {
         double st, ct; SinCos( double( m_theta ), st, ct );
         double sct = st*ct;
         double st2 = st*st;
         double ct2 = ct*ct;
         double sx2 = sx*sx;
         double sy2 = sy*sy;
         double p1  = ct2/sx2 + st2/sy2;
         double p2  = sct/sy2 - sct/sx2;
         double p3  = st2/sx2 + ct2/sy2;
         for ( int n2 = size >> 1, dy = -n2; dy <= n2; ++dy )
         {
            double twop2dy = 2*p2*dy;
            double p3dy2   = p3*dy*dy;
            for ( int dx = -n2; dx <= n2; ++dx, ++h )
               *h = float( Pow( 1 + p1*dx*dx + twop2dy*dx + p3dy2, mb ) );
         }
      }
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_MoffatFilter_h

// ----------------------------------------------------------------------------
// EOF pcl/MoffatFilter.h - Released 2018-12-12T09:24:21Z
