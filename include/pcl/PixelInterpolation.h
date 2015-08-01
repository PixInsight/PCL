//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// pcl/PixelInterpolation.h - Released 2015/07/30 17:15:18 UTC
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

#ifndef __PCL_PixelInterpolation_h
#define __PCL_PixelInterpolation_h

/// \file pcl/PixelInterpolation.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_Exception_h
#include <pcl/Exception.h>
#endif

#ifndef __PCL_String_h
#include <pcl/String.h>
#endif

#ifndef __PCL_PixelTraits_h
#include <pcl/PixelTraits.h>
#endif

#ifndef __PCL_NearestNeighborInterpolation_h
#include <pcl/NearestNeighborInterpolation.h>
#endif

#ifndef __PCL_BilinearInterpolation_h
#include <pcl/BilinearInterpolation.h>
#endif

#ifndef __PCL_BicubicInterpolation_h
#include <pcl/BicubicInterpolation.h>
#endif

#ifndef __PCL_BicubicFilterInterpolation_h
#include <pcl/BicubicFilterInterpolation.h>
#endif

#ifndef __PCL_LanczosInterpolation_h
#include <pcl/LanczosInterpolation.h>
#endif

#ifndef __PCL_Point_h
#include <pcl/Point.h>
#endif

#ifdef _MSC_VER
#  pragma warning( push )
#  pragma warning( disable : 4267 )  // conversion from 'size_t' to ...
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class PixelInterpolation
 * \brief Abstract root base class for all pixel interpolation algorithms.
 *
 * %PixelInterpolation is the abstract base class of all pixel interpolation
 * PCL classes. A pixel interpolation class is able to generate <em>pixel
 * interpolators</em> (instances of PixelInterpolation::Interpolator), which
 * are generic objects providing a common pixel interpolation interface that is
 * independent on a particular interpolation algorithm.
 *
 * \sa NearestNeighborPixelInterpolation, BilinearPixelInterpolation,
 * BicubicSplinePixelInterpolation, BicubicBSplinePixelInterpolation,
 * BicubicFilterPixelInterpolation, LanczosPixelInterpolation,
 * Lanczos3LUTPixelInterpolation, Lanczos4LUTPixelInterpolation,
 * Lanczos5LUTPixelInterpolation
 */
class PCL_CLASS PixelInterpolation
{
public:

   /*!
    * \class pcl::PixelInterpolation::Interpolator
    * \brief Generic two-dimensional pixel interpolator.
    *
    * %Interpolator provides a common pixel interpolation interface that is
    * independent on a particular interpolation algorithm.
    */
   template <class P>
   class PCL_CLASS Interpolator
   {
   public:

      /*!
       * Represents a pixel sample value.
       */
      typedef typename P::sample sample;

      /*!
       * Constructs and initializes a new %Interpolator object that
       * interpolates a pixel sample matrix \a f of the specified \a width and
       * \a height dimensions, using the specified interpolation \a i.
       */
      Interpolator( BidimensionalInterpolation<sample>* i,
                    const sample* f, int width, int height ) : m_interpolation( i )
      {
         if ( m_interpolation != nullptr )
            m_interpolation->Initialize( f, width, height );
      }

      /*!
       * Destroys an %Interpolator object and all associated working
       * structures.
       */
      virtual ~Interpolator()
      {
         if ( m_interpolation != nullptr )
            m_interpolation->Clear(), delete m_interpolation, m_interpolation = nullptr;
      }

      /*!
       * Returns a reference to the immutable two-dimensional interpolation
       * being used by this %Interpolator object.
       */
      const BidimensionalInterpolation<sample>& Interpolation() const
      {
         PCL_CHECK( m_interpolation != nullptr )
         return *m_interpolation;
      }

      /*!
       * Returns a reference to the mutable two-dimensional interpolation being
       * used by this %Interpolator object.
       */
      BidimensionalInterpolation<sample>& Interpolation()
      {
         PCL_CHECK( m_interpolation != nullptr )
         return *m_interpolation;
      }

      /*!
       * Interpolates a pixel sample value at the specified \a x and \a y
       * coordinates.
       */
      sample operator()( double x, double y ) const
      {
         PCL_CHECK( m_interpolation != nullptr )
         double r = (*m_interpolation)( x, y );
         return (r < P::MinSampleValue()) ? P::MinSampleValue() :
            ((r > P::MaxSampleValue()) ? P::MaxSampleValue() : P::FloatToSample( r ));
      }

      /*!
       * Interpolates a pixel sample value at the specified \a p point.
       *
       * This is an overloaded member function, provided for convenience. It is
       * equivalent to operator()( p.x, p.y ).
       */
      template <typename T>
      sample operator()( const GenericPoint<T>& p ) const
      {
         return operator()( p.x, p.y );
      }

   private:

      BidimensionalInterpolation<sample>* m_interpolation;
   };

   // -------------------------------------------------------------------------

   /*!
    * Constructs a %PixelInterpolation object.
    */
   PixelInterpolation() = default;

   /*!
    * Destroys a %PixelInterpolation object.
    */
   virtual ~PixelInterpolation()
   {
   }

   /*!
    * Returns a descriptive text string for this pixel interpolation.
    */
   virtual String Description() const = 0;

   /*!
    * Returns true if this interpolation can be safely used in multithreaded
    * contexts.
    *
    * A thread-safe interpolator can be used concurrently by multiple threads
    * without needing to lock the interpolated data.
    *
    * \note The default implementation returns true, so pixel interpolations
    * are considered thread-safe by default.
    */
   virtual bool IsThreadSafe() const
   {
      return true;
   }

   /*!
    * Creates a new Interpolator object specialized for a pixel sample type P.
    *
    * \param f       Two-dimensional matrix of pixel sample values that will be
    *                interpolated.
    *
    * \param width   Horizontal dimension (most rapidly varying matrix
    *                subscript) of the source data matrix.
    *
    * \param height  Vertical dimension (most slowly varying matrix
    *                subscript) of the source data matrix.
    *
    * \param status  Optional monitoring of interpolation initialization.
    */
   template <class P, class T>
   Interpolator<P>* NewInterpolator( const T* f, int width, int height ) const
   {
      return new Interpolator<P>( NewInterpolation( f ), f, width, height );
   }

protected:

   virtual BidimensionalInterpolation<FloatPixelTraits::sample>*
                  NewInterpolation( const FloatPixelTraits::sample* ) const
   {
      throw NotImplemented( *this, "Interpolate 32-bit floating point images" );
   }

   virtual BidimensionalInterpolation<DoublePixelTraits::sample>*
                  NewInterpolation( const DoublePixelTraits::sample* ) const
   {
      throw NotImplemented( *this, "Interpolate 64-bit floating point images" );
   }

   virtual BidimensionalInterpolation<ComplexPixelTraits::sample>*
                  NewInterpolation( const ComplexPixelTraits::sample* ) const
   {
      throw NotImplemented( *this, "Interpolate 32-bit complex images" );
   }

   virtual BidimensionalInterpolation<DComplexPixelTraits::sample>*
                  NewInterpolation( const DComplexPixelTraits::sample* ) const
   {
      throw NotImplemented( *this, "Interpolate 64-bit complex images" );
   }

   virtual BidimensionalInterpolation<UInt8PixelTraits::sample>*
                  NewInterpolation( const UInt8PixelTraits::sample* ) const
   {
      throw NotImplemented( *this, "Interpolate 8-bit integer images" );
   }

   virtual BidimensionalInterpolation<UInt16PixelTraits::sample>*
                  NewInterpolation( const UInt16PixelTraits::sample* ) const
   {
      throw NotImplemented( *this, "Interpolate 16-bit integer images" );
   }

   virtual BidimensionalInterpolation<UInt32PixelTraits::sample>*
                  NewInterpolation( const UInt32PixelTraits::sample* ) const
   {
      throw NotImplemented( *this, "Interpolate 32-bit integer images" );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class NearestNeighborPixelInterpolation
 * \brief NearestNeighbor pixel interpolation.
 *
 * %NearestNeighborPixelInterpolation uses the nearest neighbor interpolation
 * algorithm (NearestNeighborInterpolation) to interpolate pixel values as the
 * nearest neighbor source pixels.
 *
 * \sa PixelInterpolation, NearestNeighborInterpolation,
 * BilinearPixelInterpolation, BicubicSplinePixelInterpolation,
 * BicubicBSplinePixelInterpolation, BicubicFilterPixelInterpolation
 */
class PCL_CLASS NearestNeighborPixelInterpolation : public PixelInterpolation
{
public:

   /*!
    * Constructs a %NearestNeighborPixelInterpolation object.
    */
   NearestNeighborPixelInterpolation() : PixelInterpolation()
   {
   }

   /*!
    */
   virtual String Description() const
   {
      return "Nearest neighbor interpolation";
   }

private:

   virtual BidimensionalInterpolation<FloatPixelTraits::sample>*
                  NewInterpolation( const FloatPixelTraits::sample* ) const
   {
      return new NearestNeighborInterpolation<FloatPixelTraits::sample>;
   }

   virtual BidimensionalInterpolation<DoublePixelTraits::sample>*
                  NewInterpolation( const DoublePixelTraits::sample* ) const
   {
      return new NearestNeighborInterpolation<DoublePixelTraits::sample>;
   }

   virtual BidimensionalInterpolation<UInt8PixelTraits::sample>*
                  NewInterpolation( const UInt8PixelTraits::sample* ) const
   {
      return new NearestNeighborInterpolation<UInt8PixelTraits::sample>;
   }

   virtual BidimensionalInterpolation<UInt16PixelTraits::sample>*
                  NewInterpolation( const UInt16PixelTraits::sample* ) const
   {
      return new NearestNeighborInterpolation<UInt16PixelTraits::sample>;
   }

   virtual BidimensionalInterpolation<UInt32PixelTraits::sample>*
                  NewInterpolation( const UInt32PixelTraits::sample* ) const
   {
      return new NearestNeighborInterpolation<UInt32PixelTraits::sample>;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class BilinearPixelInterpolation
 * \brief Bilinear pixel interpolation.
 *
 * %BilinearPixelInterpolation uses the bilinear interpolation algorithm
 * (BilinearInterpolation) to interpolate pixel values from the nearest
 * four neighbor pixels.
 *
 * \sa PixelInterpolation, BilinearInterpolation,
 * NearestNeighborPixelInterpolation, BicubicSplinePixelInterpolation,
 * BicubicBSplinePixelInterpolation, BicubicFilterPixelInterpolation
 */
class PCL_CLASS BilinearPixelInterpolation : public PixelInterpolation
{
public:

   /*!
    * Constructs a %BilinearPixelInterpolation object.
    */
   BilinearPixelInterpolation() : PixelInterpolation()
   {
   }

   /*!
    */
   virtual String Description() const
   {
      return "Bilinear interpolation";
   }

private:

   virtual BidimensionalInterpolation<FloatPixelTraits::sample>*
                  NewInterpolation( const FloatPixelTraits::sample* ) const
   {
      return new BilinearInterpolation<FloatPixelTraits::sample>;
   }

   virtual BidimensionalInterpolation<DoublePixelTraits::sample>*
                  NewInterpolation( const DoublePixelTraits::sample* ) const
   {
      return new BilinearInterpolation<DoublePixelTraits::sample>;
   }

   virtual BidimensionalInterpolation<UInt8PixelTraits::sample>*
                  NewInterpolation( const UInt8PixelTraits::sample* ) const
   {
      return new BilinearInterpolation<UInt8PixelTraits::sample>;
   }

   virtual BidimensionalInterpolation<UInt16PixelTraits::sample>*
                  NewInterpolation( const UInt16PixelTraits::sample* ) const
   {
      return new BilinearInterpolation<UInt16PixelTraits::sample>;
   }

   virtual BidimensionalInterpolation<UInt32PixelTraits::sample>*
                  NewInterpolation( const UInt32PixelTraits::sample* ) const
   {
      return new BilinearInterpolation<UInt32PixelTraits::sample>;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class BicubicSplinePixelInterpolation
 * \brief Bicubic spline pixel interpolation.
 *
 * %BicubicSplinePixelInterpolation uses the bicubic spline interpolation
 * algorithm (BicubicSplineInterpolation) to interpolate pixel values from the
 * nearest sixteen neighbor pixels.
 *
 * \sa PixelInterpolation, BicubicSplineInterpolation,
 * NearestNeighborPixelInterpolation, BicubicPixelInterpolation,
 * BicubicBSplinePixelInterpolation, BicubicFilterPixelInterpolation
 */
class PCL_CLASS BicubicSplinePixelInterpolation : public PixelInterpolation
{
public:

   /*!
    * Constructs a %BicubicPixelInterpolation object.
    *
    * The optional \e c parameter is a <em>linear clamping threshold</em> for
    * the bicubic spline interpolation algorithm. See the documentation for
    * BicubicSplineInterpolation for a detailed description of the automatic
    * linear clamping feature.
    */
   BicubicSplinePixelInterpolation( double c = __PCL_BICUBIC_SPLINE_CLAMPING_THRESHOLD ) :
      PixelInterpolation(), clamp( c )
   {
   }

   /*!
    */
   virtual String Description() const
   {
      return String().Format( "Bicubic spline interpolation, c=%.2f", clamp );
   }

private:

   float clamp;

   virtual BidimensionalInterpolation<FloatPixelTraits::sample>*
                  NewInterpolation( const FloatPixelTraits::sample* ) const
   {
      return new BicubicSplineInterpolation<FloatPixelTraits::sample>( clamp );
   }

   virtual BidimensionalInterpolation<DoublePixelTraits::sample>*
                  NewInterpolation( const DoublePixelTraits::sample* ) const
   {
      return new BicubicSplineInterpolation<DoublePixelTraits::sample>( clamp );
   }

   virtual BidimensionalInterpolation<UInt8PixelTraits::sample>*
                  NewInterpolation( const UInt8PixelTraits::sample* ) const
   {
      return new BicubicSplineInterpolation<UInt8PixelTraits::sample>( clamp );
   }

   virtual BidimensionalInterpolation<UInt16PixelTraits::sample>*
                  NewInterpolation( const UInt16PixelTraits::sample* ) const
   {
      return new BicubicSplineInterpolation<UInt16PixelTraits::sample>( clamp );
   }

   virtual BidimensionalInterpolation<UInt32PixelTraits::sample>*
                  NewInterpolation( const UInt32PixelTraits::sample* ) const
   {
      return new BicubicSplineInterpolation<UInt32PixelTraits::sample>( clamp );
   }
};

/*!
 * \class BicubicPixelInterpolation
 * \brief Bicubic pixel interpolation - an alias for BicubicSplinePixelInterpolation.
 *
 * BicubicPixelInterpolation is a synonym for the
 * BicubicSplinePixelInterpolation class.
 *
 * \sa BicubicSplinePixelInterpolation
 */
class PCL_CLASS BicubicPixelInterpolation : public BicubicSplinePixelInterpolation
{
public:

   /*!
    * Constructs a %BicubicPixelInterpolation object.
    *
    * The optional \e c parameter is a <em>linear clamping threshold</em> for
    * the bicubic spline interpolation algorithm. See the documentation for
    * BicubicSplineInterpolation for a detailed description of the automatic
    * linear clamping feature.
    */
   BicubicPixelInterpolation( double c = __PCL_BICUBIC_SPLINE_CLAMPING_THRESHOLD ) :
      BicubicSplinePixelInterpolation( c )
   {
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class BicubicBSplinePixelInterpolation
 * \brief Bicubic B-spline pixel interpolation.
 *
 * %BicubicBSplinePixelInterpolation uses the bicubic B-spline interpolation
 * algorithm (BicubicBSplineInterpolation) to interpolate pixel values from the
 * nearest sixteen neighbor pixels.
 *
 * \sa PixelInterpolation, BicubicBSplineInterpolation,
 * NearestNeighborPixelInterpolation, BilinearPixelInterpolation,
 * BicubicSplinePixelInterpolation, BicubicFilterPixelInterpolation
 */
class PCL_CLASS BicubicBSplinePixelInterpolation : public PixelInterpolation
{
public:

   /*!
    * Constructs a %BicubicBSplinePixelInterpolation object.
    */
   BicubicBSplinePixelInterpolation() : PixelInterpolation()
   {
   }

   /*!
    */
   virtual String Description() const
   {
      return "Bicubic B-spline interpolation";
   }

private:

   virtual BidimensionalInterpolation<FloatPixelTraits::sample>*
                  NewInterpolation( const FloatPixelTraits::sample* ) const
   {
      return new BicubicBSplineInterpolation<FloatPixelTraits::sample>;
   }

   virtual BidimensionalInterpolation<DoublePixelTraits::sample>*
                  NewInterpolation( const DoublePixelTraits::sample* ) const
   {
      return new BicubicBSplineInterpolation<DoublePixelTraits::sample>;
   }

   virtual BidimensionalInterpolation<UInt8PixelTraits::sample>*
                  NewInterpolation( const UInt8PixelTraits::sample* ) const
   {
      return new BicubicBSplineInterpolation<UInt8PixelTraits::sample>;
   }

   virtual BidimensionalInterpolation<UInt16PixelTraits::sample>*
                  NewInterpolation( const UInt16PixelTraits::sample* ) const
   {
      return new BicubicBSplineInterpolation<UInt16PixelTraits::sample>;
   }

   virtual BidimensionalInterpolation<UInt32PixelTraits::sample>*
                  NewInterpolation( const UInt32PixelTraits::sample* ) const
   {
      return new BicubicBSplineInterpolation<UInt32PixelTraits::sample>;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class BicubicFilterPixelInterpolation
 * \brief Pixel interpolation based on Mitchell-Netravali parameterized bicubic
 *        filters.
 *
 * %BicubicFilterPixelInterpolation uses a \e cubic \e filter (an instance of
 * CubicFilter or a derived class) to interpolate pixel values in a square
 * pixel matrix of the specified \e radius.
 *
 * \sa PixelInterpolation, CubicFilter, BicubicFilterInterpolation,
 * NearestNeighborPixelInterpolation, BilinearPixelInterpolation,
 * BicubicSplinePixelInterpolation, BicubicBSplinePixelInterpolation
 */
class PCL_CLASS BicubicFilterPixelInterpolation : public PixelInterpolation
{
public:

   /*!
    * Constructs a %BicubicFilterPixelInterpolation object.
    *
    * \param rh,rv   Horizontal and vertical interpolation radii, respectively,
    *                in pixels. Both radii must be >= 1. Interpolation will
    *                take place in a rectangular pixel matrix whose dimensions
    *                will be width = 2*rh + 1 and height = 2*rv + 1.
    *
    * \param cf      Reference to a CubicFilter instance that will be used as
    *                the interpolation filter.
    *
    */
   BicubicFilterPixelInterpolation( int rh, int rv, const CubicFilter& cf ) :
      PixelInterpolation(), rx( Max( 1, rh ) ), ry( Max( 1, rv ) ), filter( cf.Clone() )
   {
      PCL_PRECONDITION( rh >= 1 )
      PCL_PRECONDITION( rv >= 1 )
   }

   /*!
    * Virtual destructor.
    */
   virtual ~BicubicFilterPixelInterpolation()
   {
      if ( filter != 0 )
         delete filter, filter = 0;
   }

   /*!
    */
   virtual String Description() const
   {
      return "Bicubic interpolation, " + filter->Description() + String().Format( " (%dx%d)", 2*rx + 1, 2*ry + 1 );
   }

   /*!
    * Returns a constant reference to the cubic filter being used by this
    * interpolation.
    */
   const CubicFilter& Filter() const
   {
      return *filter;
   }

private:

   int rx, ry;
   CubicFilter* filter;

   virtual BidimensionalInterpolation<FloatPixelTraits::sample>*
                  NewInterpolation( const FloatPixelTraits::sample* ) const
   {
      return new BicubicFilterInterpolation<FloatPixelTraits::sample>( rx, ry, *filter );
   }

   virtual BidimensionalInterpolation<DoublePixelTraits::sample>*
                  NewInterpolation( const DoublePixelTraits::sample* ) const
   {
      return new BicubicFilterInterpolation<DoublePixelTraits::sample>( rx, ry, *filter );
   }

   virtual BidimensionalInterpolation<UInt8PixelTraits::sample>*
                  NewInterpolation( const UInt8PixelTraits::sample* ) const
   {
      return new BicubicFilterInterpolation<UInt8PixelTraits::sample>( rx, ry, *filter );
   }

   virtual BidimensionalInterpolation<UInt16PixelTraits::sample>*
                  NewInterpolation( const UInt16PixelTraits::sample* ) const
   {
      return new BicubicFilterInterpolation<UInt16PixelTraits::sample>( rx, ry, *filter );
   }

   virtual BidimensionalInterpolation<UInt32PixelTraits::sample>*
                  NewInterpolation( const UInt32PixelTraits::sample* ) const
   {
      return new BicubicFilterInterpolation<UInt32PixelTraits::sample>( rx, ry, *filter );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class LanczosPixelInterpolation
 * \brief Lanczos pixel interpolation.
 *
 * %LanczosPixelInterpolation uses the Lanczos interpolation algorithm
 * (LanczosInterpolation) to interpolate pixel values from the
 * nearest 2*n^2 neighbor pixels, where n is the filter order.
 *
 * \sa PixelInterpolation, LanczosInterpolation,
 * BicubicBSplinePixelInterpolation, NearestNeighborPixelInterpolation,
 * BilinearPixelInterpolation, BicubicSplinePixelInterpolation,
 * BicubicFilterPixelInterpolation, Lanczos3LUTPixelInterpolation,
 * Lanczos4LUTPixelInterpolation, Lanczos5LUTPixelInterpolation
 */
class PCL_CLASS LanczosPixelInterpolation : public PixelInterpolation
{
public:

   /*!
    * Constructs a %LanczosPixelInterpolation object.
    *
    * \param n       Filter order (n >= 1). The Lanczos filter interpolates
    *                from the nearest (2*n)^2 mapped source pixels for each
    *                interpolation point. The default filter order is 3, which
    *                means that the interpolation uses a neighborhood of 36
    *                source pixels.
    *
    * \param clamp   Clamping threshold. Clamping is applied to fix undershoot
    *                (aka ringing) artifacts. A value of this parameter within
    *                the [0,1] range enables clamping: The lower the clamping
    *                threshold, the more aggresive deringing effect is
    *                achieved. A negative threshold value disables the
    *                interpolation clamping feature. The default value is 0.3.
    */
   LanczosPixelInterpolation( int n = 3, float clamp = 0.3 ) :
      PixelInterpolation(), m_n( Max( 1, n ) ), m_clamp( clamp )
   {
      PCL_PRECONDITION( n >= 1 )
      PCL_PRECONDITION( clamp < 0 || 0 <= clamp && clamp <= 1 )
   }

   /*!
    */
   virtual String Description() const
   {
      String desc = String().Format( "Lanczos-%d interpolation", m_n );
      if ( m_clamp >= 0 )
         desc.AppendFormat( ", c=%.2f", m_clamp );
      return desc;
   }

private:

   int   m_n;     // filter order
   float m_clamp; // clamping threshold (enabled if >= 0)

   virtual BidimensionalInterpolation<FloatPixelTraits::sample>*
                  NewInterpolation( const FloatPixelTraits::sample* ) const
   {
      return new LanczosInterpolation<FloatPixelTraits::sample>( m_n, m_clamp );
   }

   virtual BidimensionalInterpolation<DoublePixelTraits::sample>*
                  NewInterpolation( const DoublePixelTraits::sample* ) const
   {
      return new LanczosInterpolation<DoublePixelTraits::sample>( m_n, m_clamp );
   }

   virtual BidimensionalInterpolation<UInt8PixelTraits::sample>*
                  NewInterpolation( const UInt8PixelTraits::sample* ) const
   {
      return new LanczosInterpolation<UInt8PixelTraits::sample>( m_n, m_clamp );
   }

   virtual BidimensionalInterpolation<UInt16PixelTraits::sample>*
                  NewInterpolation( const UInt16PixelTraits::sample* ) const
   {
      return new LanczosInterpolation<UInt16PixelTraits::sample>( m_n, m_clamp );
   }

   virtual BidimensionalInterpolation<UInt32PixelTraits::sample>*
                  NewInterpolation( const UInt32PixelTraits::sample* ) const
   {
      return new LanczosInterpolation<UInt32PixelTraits::sample>( m_n, m_clamp );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class Lanczos3LUTPixelInterpolation
 * \brief Lanczos 3rd-order LUT-based pixel interpolation.
 *
 * %Lanczos3LUTPixelInterpolation uses a Lanczos 3rd-order LUT-based interpolation
 * algorithm (Lanczos3LUTInterpolation) to interpolate pixel values from the
 * nearest 36 neighbor pixels. LUT-based interpolations are about three times
 * faster than interpolations using function evaluations.
 *
 * \sa PixelInterpolation, Lanczos3LUTInterpolation, LanczosInterpolation,
 * BicubicBSplinePixelInterpolation, NearestNeighborPixelInterpolation,
 * BilinearPixelInterpolation, BicubicSplinePixelInterpolation,
 * BicubicFilterPixelInterpolation, LanczosPixelInterpolation,
 * Lanczos4LUTPixelInterpolation, Lanczos5LUTPixelInterpolation
 */
class PCL_CLASS Lanczos3LUTPixelInterpolation : public PixelInterpolation
{
public:

   /*!
    * Constructs a %Lanczos3LUTPixelInterpolation object.
    *
    * \param clamp   Clamping threshold. Clamping is applied to fix undershoot
    *                (aka ringing) artifacts. A value of this parameter within
    *                the [0,1] range enables clamping: The lower the clamping
    *                threshold, the more aggresive deringing effect is
    *                achieved. A negative threshold value disables the
    *                interpolation clamping feature. The default value is 0.3.
    */
   Lanczos3LUTPixelInterpolation( float clamp = 0.3 ) :
      PixelInterpolation(), m_clamp( clamp )
   {
      PCL_PRECONDITION( clamp < 0 || 0 <= clamp && clamp <= 1 )
   }

   /*!
    */
   virtual String Description() const
   {
      String desc( "Lanczos-3 LUT interpolation" );
      if ( m_clamp >= 0 )
         desc.AppendFormat( ", c=%.2f", m_clamp );
      return desc;
   }

private:

   float m_clamp; // clamping threshold (enabled if >= 0)

   virtual BidimensionalInterpolation<FloatPixelTraits::sample>*
                  NewInterpolation( const FloatPixelTraits::sample* ) const
   {
      return new Lanczos3LUTInterpolation<FloatPixelTraits::sample>( m_clamp );
   }

   virtual BidimensionalInterpolation<DoublePixelTraits::sample>*
                  NewInterpolation( const DoublePixelTraits::sample* ) const
   {
      return new Lanczos3LUTInterpolation<DoublePixelTraits::sample>( m_clamp );
   }

   virtual BidimensionalInterpolation<UInt8PixelTraits::sample>*
                  NewInterpolation( const UInt8PixelTraits::sample* ) const
   {
      return new Lanczos3LUTInterpolation<UInt8PixelTraits::sample>( m_clamp );
   }

   virtual BidimensionalInterpolation<UInt16PixelTraits::sample>*
                  NewInterpolation( const UInt16PixelTraits::sample* ) const
   {
      return new Lanczos3LUTInterpolation<UInt16PixelTraits::sample>( m_clamp );
   }

   virtual BidimensionalInterpolation<UInt32PixelTraits::sample>*
                  NewInterpolation( const UInt32PixelTraits::sample* ) const
   {
      return new Lanczos3LUTInterpolation<UInt32PixelTraits::sample>( m_clamp );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class Lanczos4LUTPixelInterpolation
 * \brief Lanczos 3rd-order LUT-based pixel interpolation.
 *
 * %Lanczos4LUTPixelInterpolation uses a Lanczos 4th-order LUT-based
 * interpolation algorithm (Lanczos4LUTInterpolation) to interpolate pixel
 * values from the nearest 64 neighbor pixels. LUT-based interpolations are
 * about three times faster than interpolations using function evaluations.
 *
 * \sa PixelInterpolation, Lanczos4LUTInterpolation, LanczosInterpolation,
 * BicubicBSplinePixelInterpolation, NearestNeighborPixelInterpolation,
 * BilinearPixelInterpolation, BicubicSplinePixelInterpolation,
 * BicubicFilterPixelInterpolation, LanczosPixelInterpolation,
 * Lanczos3LUTPixelInterpolation, Lanczos5LUTPixelInterpolation
 */
class PCL_CLASS Lanczos4LUTPixelInterpolation : public PixelInterpolation
{
public:

   /*!
    * Constructs a %Lanczos4LUTPixelInterpolation object.
    *
    * \param clamp   Clamping threshold. Clamping is applied to fix undershoot
    *                (aka ringing) artifacts. A value of this parameter within
    *                the [0,1] range enables clamping: The lower the clamping
    *                threshold, the more aggresive deringing effect is
    *                achieved. A negative threshold value disables the
    *                interpolation clamping feature. The default value is 0.3.
    */
   Lanczos4LUTPixelInterpolation( float clamp = 0.3 ) :
      PixelInterpolation(), m_clamp( Range( clamp, 0.0F, 1.0F ) )
   {
      PCL_PRECONDITION( clamp < 0 || 0 <= clamp && clamp <= 1 )
   }

   /*!
    */
   virtual String Description() const
   {
      String desc( "Lanczos-4 LUT interpolation" );
      if ( m_clamp >= 0 )
         desc.AppendFormat( ", c=%.2f", m_clamp );
      return desc;
   }

private:

   float m_clamp; // clamping threshold (enabled if >= 0)

   virtual BidimensionalInterpolation<FloatPixelTraits::sample>*
                  NewInterpolation( const FloatPixelTraits::sample* ) const
   {
      return new Lanczos4LUTInterpolation<FloatPixelTraits::sample>( m_clamp );
   }

   virtual BidimensionalInterpolation<DoublePixelTraits::sample>*
                  NewInterpolation( const DoublePixelTraits::sample* ) const
   {
      return new Lanczos4LUTInterpolation<DoublePixelTraits::sample>( m_clamp );
   }

   virtual BidimensionalInterpolation<UInt8PixelTraits::sample>*
                  NewInterpolation( const UInt8PixelTraits::sample* ) const
   {
      return new Lanczos4LUTInterpolation<UInt8PixelTraits::sample>( m_clamp );
   }

   virtual BidimensionalInterpolation<UInt16PixelTraits::sample>*
                  NewInterpolation( const UInt16PixelTraits::sample* ) const
   {
      return new Lanczos4LUTInterpolation<UInt16PixelTraits::sample>( m_clamp );
   }

   virtual BidimensionalInterpolation<UInt32PixelTraits::sample>*
                  NewInterpolation( const UInt32PixelTraits::sample* ) const
   {
      return new Lanczos4LUTInterpolation<UInt32PixelTraits::sample>( m_clamp );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class Lanczos5LUTPixelInterpolation
 * \brief Lanczos 5th-order LUT-based pixel interpolation.
 *
 * %Lanczos5LUTPixelInterpolation uses a Lanczos 5th-order LUT-based
 * interpolation algorithm (Lanczos5LUTInterpolation) to interpolate pixel
 * values from the nearest 100 neighbor pixels. LUT-based interpolations are
 * about three times faster than interpolations using function evaluations.
 *
 * \sa PixelInterpolation, Lanczos5LUTInterpolation, LanczosInterpolation,
 * BicubicBSplinePixelInterpolation, NearestNeighborPixelInterpolation,
 * BilinearPixelInterpolation, BicubicSplinePixelInterpolation,
 * BicubicFilterPixelInterpolation, LanczosPixelInterpolation,
 * Lanczos3LUTPixelInterpolation, Lanczos4LUTPixelInterpolation
 */
class PCL_CLASS Lanczos5LUTPixelInterpolation : public PixelInterpolation
{
public:

   /*!
    * Constructs a %Lanczos5LUTPixelInterpolation object.
    *
    * \param clamp   Clamping threshold. Clamping is applied to fix undershoot
    *                (aka ringing) artifacts. A value of this parameter within
    *                the [0,1] range enables clamping: The lower the clamping
    *                threshold, the more aggresive deringing effect is
    *                achieved. A negative threshold value disables the
    *                interpolation clamping feature. The default value is 0.3.
    */
   Lanczos5LUTPixelInterpolation( float clamp = 0.3 ) :
      PixelInterpolation(), m_clamp( Range( clamp, 0.0F, 1.0F ) )
   {
      PCL_PRECONDITION( clamp < 0 || 0 <= clamp && clamp <= 1 )
   }

   /*!
    */
   virtual String Description() const
   {
      String desc( "Lanczos-5 LUT interpolation" );
      if ( m_clamp >= 0 )
         desc.AppendFormat( ", c=%.2f", m_clamp );
      return desc;
   }

private:

   float m_clamp; // clamping threshold (enabled if >= 0)

   virtual BidimensionalInterpolation<FloatPixelTraits::sample>*
                  NewInterpolation( const FloatPixelTraits::sample* ) const
   {
      return new Lanczos5LUTInterpolation<FloatPixelTraits::sample>( m_clamp );
   }

   virtual BidimensionalInterpolation<DoublePixelTraits::sample>*
                  NewInterpolation( const DoublePixelTraits::sample* ) const
   {
      return new Lanczos5LUTInterpolation<DoublePixelTraits::sample>( m_clamp );
   }

   virtual BidimensionalInterpolation<UInt8PixelTraits::sample>*
                  NewInterpolation( const UInt8PixelTraits::sample* ) const
   {
      return new Lanczos5LUTInterpolation<UInt8PixelTraits::sample>( m_clamp );
   }

   virtual BidimensionalInterpolation<UInt16PixelTraits::sample>*
                  NewInterpolation( const UInt16PixelTraits::sample* ) const
   {
      return new Lanczos5LUTInterpolation<UInt16PixelTraits::sample>( m_clamp );
   }

   virtual BidimensionalInterpolation<UInt32PixelTraits::sample>*
                  NewInterpolation( const UInt32PixelTraits::sample* ) const
   {
      return new Lanczos5LUTInterpolation<UInt32PixelTraits::sample>( m_clamp );
   }
};

// ----------------------------------------------------------------------------

} // pcl

#ifdef _MSC_VER
#  pragma warning( pop )
#endif

#endif   // __PCL_PixelInterpolation_h

// ----------------------------------------------------------------------------
// EOF pcl/PixelInterpolation.h - Released 2015/07/30 17:15:18 UTC
