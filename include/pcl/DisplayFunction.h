//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/DisplayFunction.h - Released 2017-06-21T11:36:33Z
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

#ifndef __PCL_DisplayFunction_h
#define __PCL_DisplayFunction_h

/// \file pcl/DisplayFunction.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/HistogramTransformation.h>
#include <pcl/Vector.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Default clipping increment in sigma units.
 */
#define __PCL_AUTOSTRETCH_DEFAULT_CLIP   -2.80

/*
 * Default target mean background in the [0,1] range.
 */
#define __PCL_AUTOSTRETCH_DEFAULT_TBGD    0.25

/*
 * Whether to apply a single transformation to nominal RGB channels, or
 * separate per-channel transformations.
 */
#define __PCL_AUTOSTRETCH_DEFAULT_LINK    false

// ----------------------------------------------------------------------------

/*!
 * \class DisplayFunction
 * \brief Adaptive histogram transformations for image visualization.
 *
 * %DisplayFunction implements a set of histogram transformations for
 * visualization of linear images.
 */
class PCL_CLASS DisplayFunction : public ImageTransformation
{
public:

   /*!
    * Default constructor. Constructs an identity display function.
    */
   DisplayFunction() :
      m_m( 4 ), m_s( 4 ), m_h( 4 ), m_l( 4 ), m_r( 4 )
   {
      Reset();
   }

   /*!
    * Constructs a display function with the specified parameters.
    *
    * \param m    Vector of midtones balance parameters.
    * \param s    Vector of shadows clipping point parameters.
    * \param h    Vector of highlights clipping point parameters.
    * \param l    Vector of shadows dynamic range expansion parameters.
    * \param r    Vector of highlights dynamic range expansion parameters.
    *
    * Each argument vector can have from zero to four components (additional
    * vector components are ignored, and missing components are replaced with
    * default identity transformation parameters). Vector indices 0, 1, 2 and 3
    * correspond to the red/gray, green, blue and lightness components for
    * histogram transformations.
    *
    * For detailed information on parameter values and valid ranges, see the
    * HistogramTransformation class.
    */
   template <class V>
   DisplayFunction( const V& m, const V& s, const V& h, const V& l = V(), const V& r = V() ) :
      m_m( 4 ), m_s( 4 ), m_h( 4 ), m_l( 4 ), m_r( 4 )
   {
      Reset();
      for ( int i = 0; i < 4 && i < int( m.Length() ); ++i ) m_m[i] = Range( double( m[i] ), 0.0, 1.0 );
      for ( int i = 0; i < 4 && i < int( s.Length() ); ++i ) m_s[i] = Range( double( s[i] ), 0.0, 1.0 );
      for ( int i = 0; i < 4 && i < int( h.Length() ); ++i ) m_h[i] = Range( double( h[i] ), 0.0, 1.0 );
      for ( int i = 0; i < 4 && i < int( l.Length() ); ++i ) m_l[i] = Max( 0.0, double( l[i] ) );
      for ( int i = 0; i < 4 && i < int( r.Length() ); ++i ) m_r[i] = Min( 1.0, double( r[i] ) );
      for ( int i = 0; i < 4; ++i )
         if ( m_h[i] < m_s[i] )
            pcl::Swap( m_s[i], m_h[i] );
   }

   /*!
    * Copy constructor.
    */
   DisplayFunction( const DisplayFunction& ) = default;

   /*!
    * Move constructor.
    */
#ifndef _MSC_VER
   DisplayFunction( DisplayFunction&& ) = default;
#endif

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   DisplayFunction& operator =( const DisplayFunction& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
#ifndef _MSC_VER
   DisplayFunction& operator =( DisplayFunction&& ) = default;
#endif

   /*!
    * Virtual destructor.
    */
   virtual ~DisplayFunction()
   {
   }

   /*!
    * Array subscript operator. Returns an histogram transformation for the
    * specified channel index \a i, which must be in the range [0,3].
    *
    * Channel indices 0, 1, 2 and 3 correspond to the red/gray, green, blue and
    * lightness components, respectively. If an out-of-range channel index is
    * specified, this operator returns an identity histogram transformation.
    */
   HistogramTransformation operator []( int i ) const
   {
      PCL_PRECONDITION( 0 <= i && i < 4 )
      PCL_CHECK( m_m.Length() == 4 && m_s.Length() == 4 && m_h.Length() == 4 && m_l.Length() == 4 && m_r.Length() == 4 )
      if ( i >= 0 && i < 4 )
         return HistogramTransformation( m_m[i], m_s[i], m_h[i], m_l[i], m_r[i] );
      return HistogramTransformation();
   }

   /*!
    * Gets a copy of all display function parameters in the specified vectors.
    *
    * \param[out] m  Vector of midtones balance parameters.
    * \param[out] s  Vector of shadows clipping point parameters.
    * \param[out] h  Vector of highlights clipping point parameters.
    * \param[out] l  Vector of shadows dynamic range expansion parameters.
    * \param[out] r  Vector of highlights dynamic range expansion parameters.
    *
    * On output, each vector will have four components with the histogram
    * transformation parameters for the red/gray, green, blue and lightness
    * image components at vector indexes 0, 1, 2 and 3, respectively.
    */
   template <class V>
   void GetDisplayFunctionParameters( V& m, V& s, V& h, V& l, V& r ) const
   {
      m = m_m; s = m_s; h = m_h; l = m_l; r = m_r;
   }

   /*!
    * Returns true only if this object defines an identity display function for
    * the specified channel index \a i.
    *
    * An identity display function is a no-op: it does not alter the pixel data
    * or properties of the target images to which it is applied.
    */
   bool IsIdentityTransformation( int i ) const
   {
      PCL_PRECONDITION( 0 <= i && i < 4 )
      PCL_CHECK( m_m.Length() == 4 && m_s.Length() == 4 && m_h.Length() == 4 && m_l.Length() == 4 && m_r.Length() == 4 )
      return i >= 0 && i < 4 && m_m[i] == 0.5 && m_s[i] == 0 && m_h[i] == 1 && m_l[i] == 0 && m_r[i] == 1;
   }

   /*!
    * Returns true iff this is an identity display function for the four
    * image components (red/gray, green, blue and lightness).
    *
    * An identity display function is a no-op: it does not alter the pixel data
    * or properties of the target images to which it is applied.
    */
   bool IsIdentityTransformation() const
   {
      return IsIdentityTransformation( 0 )
          && IsIdentityTransformation( 1 )
          && IsIdentityTransformation( 2 )
          && IsIdentityTransformation( 3 );
   }

   /*!
    * Returns the clipping point parameter of this transformation, in sigma
    * units from the central value.
    */
   double ClippingPoint() const
   {
      return m_clip;
   }

   /*!
    * Sets the clipping point parameter of this transformation, in sigma units
    * from the central value. The default clipping point is -2.8.
    */
   void SetClippingPoint( double clip )
   {
      m_clip = clip;
   }

   /*!
    * Returns the target background parameter of this transformation in the
    * normalized [0,1] range.
    */
   double TargetBackground() const
   {
      return m_tbkg;
   }

   /*
    * Sets the target background parameter of this transformation in the
    * normalized [0,1] range. The default target background is 0.25.
    */
   void SetTargetBackground( double tbkg )
   {
      m_tbkg = Range( tbkg, 0.0, 1.0 );
   }

   /*!
    * Returns true iff this transformation will compute a single adaptive
    * histogram transformation for the nominal channels of an RGB color image.
    * Returns false if adaptive per-channel transformations will be calculated
    * separately.
    */
   bool LinkedRGB() const
   {
      return m_link;
   }

   /*!
    * Sets the 'linked RGB' parameter of this transformation. When this
    * parameter is true, a single adaptive histogram transformation will be
    * computed for the nominal channels of an RGB color image. When this
    * parameter is false, separate adaptive transformations will be calculated
    * for each channel.
    */
   void SetLinkedRGB( bool link = true )
   {
      m_link = link;
   }

   /*!
    * Computes adaptive display functions, also known as <em>auto-stretch
    * functions</em>, based on statistical estimates of scale and location.
    *
    * \param sigma   Vector of scale estimates. This is typically a vector of
    *                normalized MAD values (median absolute deviation from the
    *                median) or similar robust dispersion estimates.
    *
    * \param center  Vector of location estimates. Typically the median is used
    *                as a robust estimator of central tendency.
    *
    * Both vectors must have one or three components, respectively for a
    * monochrome (grayscale) or RGB color image. Avoid using non-robust
    * statistical estimators such as the standard deviation or the mean, which
    * can easily lead to completely wrong results.
    *
    * To obtain results coherent with other implementations, scale estimates
    * should be normalized to be consistent with the standard deviation of a
    * normal distribution. For example, if MAD values are used, they should be
    * multiplied by 1.4826 before calling this function.
    */
   template <class V>
   void ComputeAutoStretch( const V& sigma, const V& center )
   {
      PCL_CHECK( m_m.Length() == 4 && m_s.Length() == 4 && m_h.Length() == 4 && m_l.Length() == 4 && m_r.Length() == 4 )

      if ( sigma.IsEmpty() || center.IsEmpty() )
      {
         Reset();
         return;
      }

      int n = (sigma.Length() < 3 || center.Length() < 3) ? 1 : 3;

      if ( m_link )
      {
         /*
          * Try to find out how many channels look like channels of an inverted
          * image.
          *
          * We know a channel is inverted because the main histogram peak is
          * located over the right-hand half of the histogram. Seems simplistic
          * but this is consistent with most real-world images.
          */
         int invertedChannels = 0;
         for ( int i = 0; i < n; ++i )
            if ( center[i] > 0.5 )
               ++invertedChannels;

         double c = 0, m = 0;
         if ( invertedChannels < n )
         {
            // Noninverted image
            for ( int i = 0; i < n; ++i )
            {
               if ( 1 + sigma[i] != 1 )
                  c += center[i] + m_clip * sigma[i];
               m += center[i];
            }
            m_s[0] = m_s[1] = m_s[2] = Range( c/n, 0.0, 1.0 );
            m_m[0] = m_m[1] = m_m[2] = HistogramTransformation::MTF( m_tbkg, m/n - m_s[0] );
            m_l[0] = m_l[1] = m_l[2] = 0.0;
            m_h[0] = m_h[1] = m_h[2] = m_r[0] = m_r[1] = m_r[2] = 1.0;
         }
         else
         {
            // Inverted image
            for ( int i = 0; i < n; ++i )
            {
               c += (1 + sigma[i] != 1) ? center[i] - m_clip * sigma[i] : 1.0;
               m  += center[i];
            }
            m_h[0] = m_h[1] = m_h[2] = Range( c/n, 0.0, 1.0 );
            m_m[0] = m_m[1] = m_m[2] = HistogramTransformation::MTF( m_h[0] - m/n, m_tbkg );
            m_s[0] = m_s[1] = m_s[2] = m_l[0] = m_l[1] = m_l[2] = 0.0;
            m_r[0] = m_r[1] = m_r[2] = 1.0;
         }
      }
      else
      {
         /*
          * Unlinked RGB/K channnels: Compute automatic stretch functions for
          * individual RGB/K channels separately.
          */
         for ( int i = 0; i < n; ++i )
            if ( center[i] < 0.5 )
            {
               // Noninverted channel
               m_s[i] = (1 + sigma[i] != 1) ? Range( center[i] + m_clip * sigma[i], 0.0, 1.0 ) : 0.0;
               m_m[i] = HistogramTransformation::MTF( m_tbkg, center[i] - m_s[i] );
               m_l[i] = 0.0;
               m_h[i] = m_r[i] = 1.0;
            }
            else
            {
               // Inverted channel
               m_h[i] = (1 + sigma[i] != 1) ? Range( center[i] - m_clip * sigma[i], 0.0, 1.0 ) : 1.0;
               m_m[i] = HistogramTransformation::MTF( m_h[i] - center[i], m_tbkg );
               m_s[i] = m_l[i] = 0.0;
               m_r[i] = 1.0;
            }
      }
   }

   /*!
    * Resets all display function parameters to yield an identity
    * transformation.
    */
   void Reset()
   {
      m_m = 0.5;
      m_s = m_l = 0.0;
      m_h = m_r = 1.0;
   }

   /*!
    * Exchanges two %DisplayFunction objects.
    */
   friend void Swap( DisplayFunction& x, DisplayFunction& y )
   {
      pcl::Swap( x.m_m, y.m_m );
      pcl::Swap( x.m_s, y.m_s );
      pcl::Swap( x.m_h, y.m_h );
      pcl::Swap( x.m_l, y.m_l );
      pcl::Swap( x.m_r, y.m_r );
      pcl::Swap( x.m_clip, y.m_clip );
      pcl::Swap( x.m_tbkg, y.m_tbkg );
      pcl::Swap( x.m_link, y.m_link );
   }

private:

   DVector m_m;   // midtones balance
   DVector m_s;   // shadows clipping point
   DVector m_h;   // highlights clipping point
   DVector m_l;   // shadows dynamic range expansion
   DVector m_r;   // highlights dynamic range expansion
   double  m_clip = __PCL_AUTOSTRETCH_DEFAULT_CLIP; // auto-stretch clipping point
   double  m_tbkg = __PCL_AUTOSTRETCH_DEFAULT_TBGD; // auto-stretch target background
   bool    m_link = __PCL_AUTOSTRETCH_DEFAULT_LINK; // auto-stretch linked RGB

   /*
    * Display function transformation.
    */
   virtual void Apply( pcl::Image& ) const;
   virtual void Apply( pcl::DImage& ) const;
   virtual void Apply( pcl::UInt8Image& ) const;
   virtual void Apply( pcl::UInt16Image& ) const;
   virtual void Apply( pcl::UInt32Image& ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_DisplayFunction_h

// ----------------------------------------------------------------------------
// EOF pcl/DisplayFunction.h - Released 2017-06-21T11:36:33Z
