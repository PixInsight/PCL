//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// pcl/LanczosInterpolation.h - Released 2015/10/08 11:24:12 UTC
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

#ifndef __PCL_LanczosInterpolation_h
#define __PCL_LanczosInterpolation_h

/// \file pcl/LanczosInterpolation.h

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

#ifndef __PCL_Vector_h
#include <pcl/Vector.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

#define m_width      this->m_width
#define m_height     this->m_height
#define m_fillBorder this->m_fillBorder
#define m_fillValue  this->m_fillValue
#define m_data       this->m_data

// ----------------------------------------------------------------------------

/*
 * Default clamping threshold for Lanczos interpolation. This value has been
 * selected as the best trade-off for a large set of test linear images.
 */
#ifndef __PCL_LANCZOS_CLAMPING_THRESHOLD
#define __PCL_LANCZOS_CLAMPING_THRESHOLD  0.3F
#endif

/*
 * LUT-based interpolations for 3rd, 4th and 5th order Lanczos functions run
 * 3x faster than the corresponding function evaluation interpolations.
 */
#ifndef __PCL_DONT_USE_LANCZOS_LUTS

// Lanczos LUTs are accurate to +/- 1 16-bit DN
#define __PCL_LANCZOS_LUT_RESOLUTION      65535

// LUT data requires a maximum of 3 MB
extern PCL_DATA float* PCL_Lanczos3_LUT;
extern PCL_DATA float* PCL_Lanczos4_LUT;
extern PCL_DATA float* PCL_Lanczos5_LUT;

// On-demand thread-safe LUT generation
void PCL_FUNC PCL_InitializeLanczosLUT( float*&, int );

#endif   // !__PCL_DONT_USE_LANCZOS_LUTS

#define PCL_LANCZOS_ACC()        \
         if ( s < 0 )            \
            sn -= s, wn -= L;    \
         else                    \
            sp += s, wp += L;

/*!
 * \class LanczosInterpolation
 * \brief Two dimensional Lanczos interpolation algorithm.
 *
 * This class uses Lanczos filters to interpolate pixel values at arbitrary
 * coordinates within a two-dimensional data matrix. A one-dimensional Lanczos
 * filter of order \e n is defined by the following equations:
 *
 * <pre>
 * L(x;n) = sinc(x)*sinc(x/n)    for |x| < n
 * L(x;n) = 0                    for |x| >= n
 * </pre>
 *
 * where sinc() is the normalized sinc function:
 *
 * <pre>
 * sinc(x;n) = 1                 for x = 0
 * sinc(x;n) = sin(pi*x)/(pi*x)  for x != 0
 * </pre>
 *
 * The Lanczos function has alternating positive and negative lobes toward
 * positive and negative infinity. The order \e n defines the number of lobes
 * preserved in the interpolation filter function: n=1 only includes the
 * central, positive lobe; n=2 includes the first two lobes (one positive and
 * one negative), and so on. The default filter order is three.
 *
 * Lanczos interpolation has excellent detail preservation performance with
 * minimal generation of aliasing patterns for noisy data. Its main drawback is
 * generation of strong undershoot (aka ringing) artifacts when negative
 * function lobes fall over bright pixels and edges. This usually happens with
 * linear data. In the current PCL implementation we have included a clamping
 * mechanism that prevents negative interpolated values and ringing problems
 * for most images.
 *
 * \sa BidimensionalInterpolation, NearestNeighborInterpolation,
 * BilinearInterpolation, BicubicSplineInterpolation,
 * BicubicBSplineInterpolation, BicubicFilterInterpolation,
 * Lanczos3LUTInterpolation, Lanczos4LUTInterpolation, Lanczos5LUTInterpolation
 */
template <typename T>
class PCL_CLASS LanczosInterpolation : public BidimensionalInterpolation<T>
{
public:

   /*!
    * Constructs a %LanczosInterpolation instance.
    *
    * \param n       Filter order (n >= 1). The Lanczos filter interpolates
    *                from the nearest (2*n)^2 mapped source pixels for each
    *                interpolation point. The default filter order is 3, so the
    *                interpolation uses a neighborhood of 36 pixels by default.
    *
    * \param clamp   Clamping threshold. Clamping is applied to fix undershoot
    *                (aka ringing) artifacts. A value of this parameter within
    *                the [0,1] range enables clamping: The lower the clamping
    *                threshold, the more aggressive deringing effect is
    *                achieved. A negative threshold value disables the clamping
    *                feature. The default value is 0.3. For more information,
    *                refer to the documentation for the
    *                SetClampingThreshold( float ) member function.
    */
   LanczosInterpolation( int n = 3, float clamp = __PCL_LANCZOS_CLAMPING_THRESHOLD ) :
      BidimensionalInterpolation<T>(),
      m_n( Max( 1, n ) ),
      m_clamp( clamp >= 0 ), m_clampTh( Range( clamp, 0.0F, 1.0F ) ), m_clampThInv( 1 - m_clampTh ),
      m_Lx( 2*m_n )
   {
      PCL_PRECONDITION( n >= 1 )
      PCL_PRECONDITION( clamp < 0 || 0 <= clamp && clamp <= 1 )
   }

   /*!
    * Virtual destructor.
    */
   virtual ~LanczosInterpolation()
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
      PCL_PRECONDITION( m_data != nullptr )
      PCL_PRECONDITION( m_width > 0 && m_height > 0 )
      PCL_PRECONDITION( x >= 0 && x < m_width )
      PCL_PRECONDITION( y >= 0 && y < m_height )
      PCL_CHECK( m_n >= 1 )

      // Central grid coordinates
      int x0 = Range( TruncI( x ), 0, m_width-1 );
      int y0 = Range( TruncI( y ), 0, m_height-1 );

      // Interpolation increments
      double dx = x - x0;
      double dy = y - y0;

      // Precalculate horizontal filter values
      for ( int j = -m_n + 1, k = 0; j <= m_n; ++j, ++k )
         m_Lx[k] = Lanczos( j - dx );

      double sp = 0; // positive filter values
      double sn = 0; // negative filter values
      double wp = 0; // positive filter weight
      double wn = 0; // negative filter weight
      int i;         // row index

      // Clipped rows at top
      for ( i = -m_n + 1; i <= m_n; ++i )
      {
         int y = y0 + i;
         if ( y >= 0 )
            break;
         if ( m_fillBorder )
            FillRow( sp, sn, wp, wn, i - dy );
         else
            InterpolateRow( sp, sn, wp, wn, m_data - 2*int64( y )*m_width, x0, i - dy );
      }

      // Unclipped rows
      for ( ; i <= m_n; ++i )
      {
         int y = y0 + i;
         if ( y == m_height )
            break;
         InterpolateRow( sp, sn, wp, wn, m_data + int64( y )*m_width, x0, i - dy );
      }

      // Clipped rows at bottom
      for ( ; i <= m_n; ++i )
      {
         if ( m_fillBorder )
            FillRow( sp, sn, wp, wn, i - dy );
         else
            InterpolateRow( sp, sn, wp, wn, m_data + int64( 2*m_height - 2 - y0 - i )*m_width, x0, i - dy );
      }

      // Clamping
      if ( m_clamp )
      {
         // Empty data?
         if ( sp == 0 )
            return 0;

         // Clamping ratio: s-/s+
         double r = sn/sp;

         // Clamp for s- >= s+
         if ( r >= 1 )
            return sp/wp;

         // Clamp for c < s- < s+
         if ( r > m_clampTh )
         {
            r = (r - m_clampTh)/m_clampThInv;
            double c = 1 - r*r;
            sn *= c, wn *= c;
         }
      }

      // Weighted convolution
      return (sp - sn)/(wp - wn);
   }

   /*!
    * Returns true iff the interpolation clamping feature has been enabled for
    * this object.
    *
    * \sa EnableClamping(), ClampingThreshold()
    */
   bool IsClampingEnabled() const
   {
      return m_clamp;
   }

   /*!
    * Enables (or disables) the interpolation clamping feature.
    *
    * \sa IsClampingEnabled(), DisableClamping(), SetClampingThreshold()
    */
   void EnableClamping( bool enable = true )
   {
      m_clamp = enable;
   }

   /*!
    * Disables (or enables) the interpolation clamping feature.
    *
    * \sa IsClampingEnabled(), EnableClamping(), SetClampingThreshold()
    */
   void DisableClamping( bool disable = true )
   {
      EnableClamping( !disable );
   }

   /*!
    * Returns the current <em>clamping threshold</em> for this object.
    *
    * See the documentation for SetClampingThreshold( float ) for a detailed
    * description of the clamping mechanism.
    *
    * \sa SetClampingThreshold(), IsClampingEnabled(), EnableClamping()
    */
   float ClampingThreshold() const
   {
      return m_clampTh;
   }

   /*!
    * Defines a threshold to trigger interpolation \e clamping.
    *
    * Lanczos interpolation generates strong undershoot (aka ringing) artifacts
    * when the negative lobes of the interpolation function fall over bright
    * isolated pixels or edges. The clamping mechanism acts by limiting the
    * high-pass component of the interpolation filter selectively to fix these
    * problems.
    *
    * The specified clamping threshold \e clamp must be in the [0,1] range.
    * Lower values cause a more aggressive deringing effect. Too strong of a
    * clamping threshold can degrade performance of the Lanczos filter to some
    * degree, since it tends to block its high-pass behavior.
    *
    * \note The interpolation clamping feature must be enabled for this
    * threshold to have any effect. See the constructor for this class and the
    * documentation for IsClampingEnabled().
    *
    * \sa ClampingThreshold(), IsClampingEnabled(), EnableClamping()
    */
   void SetClampingThreshold( float clamp )
   {
      PCL_PRECONDITION( 0 <= clamp && clamp <= 1 )
      m_clampTh = Range( clamp, 0.0F, 1.0F );
   }

private:

           int     m_n;          // filter order
           bool    m_clamp : 1;  // clamping enabled ?
           double  m_clampTh;    // clamping threshold in [0,1]
           double  m_clampThInv; // 1 - m_clampTh
   mutable DVector m_Lx;         // precalculated row of function values

   /*
    * Sinc function for x > 0
    */
   static double Sinc( double x )
   {
      x *= Const<double>::pi();
      return (x > 1.0e-07) ? Sin( x )/x : 1.0;
   }

   /*
    * Evaluate Lanczos function at x.
    */
   double Lanczos( double x ) const
   {
      if ( x < 0 )
         x = -x;
      if ( x < m_n )
         return Sinc( x ) * Sinc( x/m_n );
      return 0;
   }

   /*
    * Interpolate a row of pixels.
    * Can be either an unclipped row or a mirrored border row.
    */
   void InterpolateRow( double& sp, double& sn, double& wp, double& wn, const T* f, int x0, double y ) const
   {
      double Ly = Lanczos( y );

      int j, k;

      // Clipped pixels at the left border
      for ( j = -m_n + 1, k = 0; j <= m_n; ++j, ++k )
      {
         int x = x0 + j;
         if ( x >= 0 )
            break;
         double L = m_Lx[k] * Ly;
         double s = (m_fillBorder ? m_fillValue : double( f[-x] )) * L;
         PCL_LANCZOS_ACC()
      }

      // Unclipped pixels
      for ( ; j <= m_n; ++j, ++k )
      {
         int x = x0 + j;
         if ( x == m_width )
            break;
         double L = m_Lx[k] * Ly;
         double s = f[x] * L;
         PCL_LANCZOS_ACC()
      }

      // Clipped pixels at the right border
      for ( ; j <= m_n; ++j, ++k )
      {
         int x = x0 + j;
         double L = m_Lx[k] * Ly;
         double s = (m_fillBorder ? m_fillValue : double( f[2*m_width - 2 - x] )) * L;
         PCL_LANCZOS_ACC()
      }
   }

   /*
    * Interpolate a clipped pixel row with border filling.
    */
   void FillRow( double& sp, double& sn, double& wp, double& wn, double y ) const
   {
      double Ly = Lanczos( y );

      for ( int j = -m_n + 1, k = 0; j <= m_n; ++j, ++k )
      {
         double L = m_Lx[k] * Ly;
         double s = m_fillValue * L;
         PCL_LANCZOS_ACC()
      }
   }
};

// ----------------------------------------------------------------------------

/*!
 * \internal
 * \class LanczosLUTInterpolationBase
 * \brief Base class of two dimensional LUT-based Lanczos interpolation algorithms.
 *
 * This is a base class for Lanczos interpolation algorithms implemented
 * through precalculated look-up tables (LUTs). The filter order \a n is
 * specified as the second template class argument. For a description of the
 * Lanczos algorithm and information on its performance and features, refer to
 * the documentation for the LanczosInterpolation class.
 *
 * LUT-based Lanczos interpolations are about three times faster than the
 * corresponding function evaluation interpolations. Interpolation from the
 * implemented LUTs provides a maximum error of +/- 1/2^16, so this class and
 * its derived classes are fully accurate for 8-bit and 16-bit integer images.
 *
 * \sa BidimensionalInterpolation, LanczosInterpolation,
 * Lanczos3LUTInterpolation, Lanczos4LUTInterpolation, Lanczos5LUTInterpolation
 */
template <typename T, int m_n>
class PCL_CLASS LanczosLUTInterpolationBase : public BidimensionalInterpolation<T>
{
public:

   /*!
    * Constructs a %LanczosLUTInterpolationBase instance.
    *
    * \param lut     Reference to the LUT corresponding to the specified
    *                filter order \a n (second template class argument).
    *
    * \param clamp   Clamping threshold. Clamping is applied to fix undershoot
    *                (aka ringing) artifacts. A value of this parameter within
    *                the [0,1] range enables clamping: The lower the clamping
    *                threshold, the more aggressive deringing effect is
    *                achieved. A negative threshold value disables the clamping
    *                feature. The default value is 0.3. For more information,
    *                refer to the documentation for the
    *                SetClampingThreshold( float ) member function.
    *
    * \note The specified nth-order Lanczos LUT is allocated and initialized
    * the first time this constructor is invoked.
    */
   LanczosLUTInterpolationBase( float*& lut, float clamp ) :
      BidimensionalInterpolation<T>(),
      m_lut( lut ),
      m_clamp( clamp >= 0 ), m_clampTh( Range( clamp, 0.0F, 1.0F ) ), m_clampThInv( 1 - m_clampTh ),
      m_Lx( 2*m_n ), m_Ly( 2*m_n )
   {
      PCL_PRECONDITION( m_n >= 1 )
      PCL_PRECONDITION( lut != 0 )
      PCL_PRECONDITION( clamp < 0 || 0 <= clamp && clamp <= 1 )
      PCL_InitializeLanczosLUT( m_lut, m_n );
   }

   /*!
    * Virtual destructor.
    */
   virtual ~LanczosLUTInterpolationBase()
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
      PCL_PRECONDITION( m_data != nullptr )
      PCL_PRECONDITION( m_width > 0 && m_height > 0 )
      PCL_PRECONDITION( x >= 0 && x < m_width )
      PCL_PRECONDITION( y >= 0 && y < m_height )

      // Central grid coordinates
      int x0 = Range( TruncI( x ), 0, m_width-1 );
      int y0 = Range( TruncI( y ), 0, m_height-1 );

      // Precalculate function values
      int dx = RoundI( (x - x0)*__PCL_LANCZOS_LUT_RESOLUTION );
      int dy = RoundI( (y - y0)*__PCL_LANCZOS_LUT_RESOLUTION );
      for ( int j = -m_n + 1, k = 0; j <= m_n; ++j, ++k )
      {
         int d0 = j*__PCL_LANCZOS_LUT_RESOLUTION;
         m_Lx[k] = m_lut[Abs( d0 - dx )];
         m_Ly[k] = m_lut[Abs( d0 - dy )];
      }

      double sp = 0; // positive filter values
      double sn = 0; // negative filter values
      double wp = 0; // positive filter weight
      double wn = 0; // negative filter weight
      int i, k;      // row and coefficient indices

      // Clipped rows at top
      for ( i = -m_n + 1, k = 0; i <= m_n; ++i, ++k )
      {
         int y = y0 + i;
         if ( y >= 0 )
            break;
         if ( m_fillBorder )
            FillRow( sp, sn, wp, wn, m_Ly[k] );
         else
            InterpolateRow( sp, sn, wp, wn, m_data - 2*int64( y )*m_width, x0, m_Ly[k] );
      }

      // Unclipped rows
      for ( ; i <= m_n; ++i, ++k )
      {
         int y = y0 + i;
         if ( y == m_height )
            break;
         InterpolateRow( sp, sn, wp, wn, m_data + int64( y )*m_width, x0, m_Ly[k] );
      }

      // Clipped rows at bottom
      for ( ; i <= m_n; ++i, ++k )
      {
         if ( m_fillBorder )
            FillRow( sp, sn, wp, wn, m_Ly[k] );
         else
            InterpolateRow( sp, sn, wp, wn, m_data + int64( 2*m_height - 2 - y0 - i )*m_width, x0, m_Ly[k] );
      }

      // Clamping
      if ( m_clamp )
      {
         // Empty data?
         if ( sp == 0 )
            return 0;

         // Clamping ratio: s-/s+
         double r = sn/sp;

         // Clamp for s- >= s+
         if ( r >= 1 )
            return sp/wp;

         // Clamp for c < s- < s+
         if ( r > m_clampTh )
         {
            r = (r - m_clampTh)/m_clampThInv;
            double c = 1 - r*r;
            sn *= c, wn *= c;
         }
      }

      // Weighted convolution
      return (sp - sn)/(wp - wn);
   }

   /*!
    * Returns true iff the interpolation clamping feature has been enabled for
    * this object.
    *
    * \sa EnableClamping(), ClampingThreshold()
    */
   bool IsClampingEnabled() const
   {
      return m_clamp;
   }

   /*!
    * Enables (or disables) the interpolation clamping feature.
    *
    * \sa IsClampingEnabled(), DisableClamping(), SetClampingThreshold()
    */
   void EnableClamping( bool enable = true )
   {
      m_clamp = enable;
   }

   /*!
    * Disables (or enables) the interpolation clamping feature.
    *
    * \sa IsClampingEnabled(), EnableClamping(), SetClampingThreshold()
    */
   void DisableClamping( bool disable = true )
   {
      EnableClamping( !disable );
   }

   /*!
    * Returns the current <em>clamping threshold</em> for this object.
    *
    * See the documentation for SetClampingThreshold( float ) for a detailed
    * description of the clamping mechanism.
    *
    * \sa SetClampingThreshold(), IsClampingEnabled(), EnableClamping()
    */
   float ClampingThreshold() const
   {
      return m_clampTh;
   }

   /*!
    * Defines a threshold to trigger interpolation \e clamping.
    *
    * Lanczos interpolation generates strong undershoot (aka ringing) artifacts
    * when the negative lobes of the interpolation function fall over bright
    * isolated pixels or edges. The clamping mechanism acts by limiting the
    * high-pass component of the interpolation filter selectively to fix these
    * problems.
    *
    * The specified clamping threshold \e clamp must be in the [0,1] range.
    * Lower values cause a more aggressive deringing effect. Too strong of a
    * clamping threshold can degrade performance of the Lanczos filter to some
    * degree, since it tends to block its high-pass behavior.
    *
    * \note The interpolation clamping feature must be enabled for this
    * threshold to have any effect. See the constructor for this class and the
    * documentation for IsClampingEnabled().
    *
    * \sa ClampingThreshold(), IsClampingEnabled(), EnableClamping()
    */
   void SetClampingThreshold( float clamp )
   {
      PCL_PRECONDITION( 0 <= clamp && clamp <= 1 )
      m_clampTh = Range( clamp, 0.0F, 1.0F );
   }

private:

           float*& m_lut;        // filter LUT
           bool    m_clamp : 1;  // clamping enabled ?
           double  m_clampTh;    // clamping threshold in [0,1]
           double  m_clampThInv; // 1 - m_clampTh
   mutable FVector m_Lx, m_Ly;   // precalculated function values

   /*
    * Interpolate a row of pixels.
    * Can be either an unclipped row or a mirrored border row.
    */
   void InterpolateRow( double& sp, double& sn, double& wp, double& wn, const T* f, int x0, float Ly ) const
   {
      int j, k;

      // Clipped pixels at the left border
      for ( j = -m_n + 1, k = 0; j <= m_n; ++j, ++k )
      {
         int x = x0 + j;
         if ( x >= 0 )
            break;
         double L = m_Lx[k] * Ly;
         double s = (m_fillBorder ? m_fillValue : double( f[-x] )) * L;
         PCL_LANCZOS_ACC()
      }

      // Unclipped pixels
      for ( ; j <= m_n; ++j, ++k )
      {
         int x = x0 + j;
         if ( x == m_width )
            break;
         double L = m_Lx[k] * Ly;
         double s = f[x] * L;
         PCL_LANCZOS_ACC()
      }

      // Clipped pixels at the right border
      for ( ; j <= m_n; ++j, ++k )
      {
         int x = x0 + j;
         double L = m_Lx[k] * Ly;
         double s = (m_fillBorder ? m_fillValue : double( f[2*m_width - 2 - x] )) * L;
         PCL_LANCZOS_ACC()
      }
   }

   /*
    * Interpolate a clipped pixel row with border filling.
    */
   void FillRow( double& sp, double& sn, double& wp, double& wn, float Ly ) const
   {
      for ( int j = -m_n + 1, k = 0; j <= m_n; ++j, ++k )
      {
         double L = m_Lx[k] * Ly;
         double s = m_fillValue * L;
         PCL_LANCZOS_ACC()
      }
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class Lanczos3LUTInterpolation
 * \brief Two dimensional LUT-based 3rd-order Lanczos interpolation algorithm.
 *
 * This class implements 3rd-order Lanczos interpolation through precalculated
 * look-up tables. For a description of the Lanczos algorithm and information
 * on its performance and features, refer to the documentation for the
 * LanczosInterpolation class.
 *
 * LUT-based Lanczos interpolations are about three times faster than the
 * corresponding function evaluation interpolations. Interpolation from the
 * implemented LUTs provides a maximum error of +/- 1/2^16, so this class is
 * fully accurate for 8-bit and 16-bit integer images.
 *
 * \sa LanczosInterpolation, LanczosLUTInterpolationBase,
 * Lanczos4LUTInterpolation, Lanczos5LUTInterpolation
 */
template <typename T>
class PCL_CLASS Lanczos3LUTInterpolation : public LanczosLUTInterpolationBase<T, 3>
{
public:

   /*!
    * Constructs a %Lanczos3LUTInterpolation instance.
    *
    * \param clamp   Clamping threshold. Clamping is applied to fix undershoot
    *                (aka ringing) artifacts. A value of this parameter within
    *                the [0,1] range enables clamping: The lower the clamping
    *                threshold, the more aggressive deringing effect is
    *                achieved. A negative threshold value disables the clamping
    *                feature. The default value is 0.3. For more information,
    *                refer to the documentation for the
    *                SetClampingThreshold( float ) member function.
    *
    * \note The 3rd-order Lanczos LUT is allocated and initialized the first
    * time this constructor is invoked. LUT allocation and initialization are
    * thread-safe.
    */
   Lanczos3LUTInterpolation( float clamp = __PCL_LANCZOS_CLAMPING_THRESHOLD ) :
      LanczosLUTInterpolationBase<T, 3>( PCL_Lanczos3_LUT, clamp )
   {
      PCL_PRECONDITION( 0 <= clamp && clamp <= 1 )
   }

   /*!
    * Virtual destructor.
    */
   virtual ~Lanczos3LUTInterpolation()
   {
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class Lanczos4LUTInterpolation
 * \brief Two dimensional LUT-based 4th-order Lanczos interpolation algorithm.
 *
 * This class implements 4th-order Lanczos interpolation through precalculated
 * look-up tables. For a description of the Lanczos algorithm and information
 * on its performance and features, refer to the documentation for the
 * LanczosInterpolation class.
 *
 * LUT-based Lanczos interpolations are about three times faster than the
 * corresponding function evaluation interpolations. Interpolation from the
 * implemented LUTs provides a maximum error of +/- 1/2^16, so this class is
 * fully accurate for 8-bit and 16-bit integer images.
 *
 * \sa LanczosInterpolation, LanczosLUTInterpolationBase,
 * Lanczos3LUTInterpolation, Lanczos5LUTInterpolation
 */
template <typename T>
class PCL_CLASS Lanczos4LUTInterpolation : public LanczosLUTInterpolationBase<T, 4>
{
public:

   /*!
    * Constructs a %Lanczos4LUTInterpolation instance.
    *
    * \param clamp   Clamping threshold. Clamping is applied to fix undershoot
    *                (aka ringing) artifacts. A value of this parameter within
    *                the [0,1] range enables clamping: The lower the clamping
    *                threshold, the more aggressive deringing effect is
    *                achieved. A negative threshold value disables the clamping
    *                feature. The default value is 0.3. For more information,
    *                refer to the documentation for the
    *                SetClampingThreshold( float ) member function.
    *
    * \note The 4th-order Lanczos LUT is allocated and initialized the first
    * time this constructor is invoked. LUT allocation and initialization are
    * thread-safe.
    */
   Lanczos4LUTInterpolation( float clamp = __PCL_LANCZOS_CLAMPING_THRESHOLD ) :
      LanczosLUTInterpolationBase<T, 4>( PCL_Lanczos4_LUT, clamp )
   {
      PCL_PRECONDITION( 0 <= clamp && clamp <= 1 )
   }

   /*!
    * Virtual destructor.
    */
   virtual ~Lanczos4LUTInterpolation()
   {
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class Lanczos5LUTInterpolation
 * \brief Two dimensional LUT-based 5th-order Lanczos interpolation algorithm.
 *
 * This class implements 5th-order Lanczos interpolation through precalculated
 * look-up tables. For a description of the Lanczos algorithm and information
 * on its performance and features, refer to the documentation for the
 * LanczosInterpolation class.
 *
 * LUT-based Lanczos interpolations are about three times faster than the
 * corresponding function evaluation interpolations. Interpolation from the
 * implemented LUTs provides a maximum error of +/- 1/2^16, so this class is
 * fully accurate for 8-bit and 16-bit integer images.
 *
 * \sa LanczosInterpolation, LanczosLUTInterpolationBase,
 * Lanczos3LUTInterpolation, Lanczos4LUTInterpolation
 */
template <typename T>
class PCL_CLASS Lanczos5LUTInterpolation : public LanczosLUTInterpolationBase<T, 5>
{
public:

   /*!
    * Constructs a %Lanczos5LUTInterpolation instance.
    *
    * \param clamp   Clamping threshold. Clamping is applied to fix undershoot
    *                (aka ringing) artifacts. A value of this parameter within
    *                the [0,1] range enables clamping: The lower the clamping
    *                threshold, the more aggressive deringing effect is
    *                achieved. A negative threshold value disables the clamping
    *                feature. The default value is 0.3. For more information,
    *                refer to the documentation for the
    *                SetClampingThreshold( float ) member function.
    *
    * \note The 5th-order Lanczos LUT is allocated and initialized the first
    * time this constructor is invoked. LUT allocation and initialization are
    * thread-safe.
    */
   Lanczos5LUTInterpolation( float clamp = __PCL_LANCZOS_CLAMPING_THRESHOLD ) :
      LanczosLUTInterpolationBase<T, 5>( PCL_Lanczos5_LUT, clamp )
   {
      PCL_PRECONDITION( 0 <= clamp && clamp <= 1 )
   }

   /*!
    * Virtual destructor.
    */
   virtual ~Lanczos5LUTInterpolation()
   {
   }
};

// ----------------------------------------------------------------------------

#undef PCL_LANCZOS_ACC

#undef m_width
#undef m_height
#undef m_fillBorder
#undef m_fillValue
#undef m_data

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_LanczosInterpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/LanczosInterpolation.h - Released 2015/10/08 11:24:12 UTC
