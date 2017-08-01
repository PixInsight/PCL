//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/PixelInterpolation.h - Released 2017-08-01T14:23:31Z
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

#ifndef __PCL_PixelInterpolation_h
#define __PCL_PixelInterpolation_h

/// \file pcl/PixelInterpolation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/AutoPointer.h>
#include <pcl/BicubicFilterInterpolation.h>
#include <pcl/BicubicInterpolation.h>
#include <pcl/BilinearInterpolation.h>
#include <pcl/Exception.h>
#include <pcl/LanczosInterpolation.h>
#include <pcl/NearestNeighborInterpolation.h>
#include <pcl/PixelTraits.h>
#include <pcl/Point.h>
#include <pcl/String.h>

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
    * \brief Generic two-dimensional pixel interpolator
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
       * Constructs and initializes a new %Interpolator object.
       *
       * \param interpolation    Pointer to a dynamically allocated instance of
       *                         the two-dimensional interpolation algorithm.
       *                         The instance will be owned by this object,
       *                         which will destroy and deallocate it upon
       *                         destruction.
       *
       * \param data             Source matrix of pixel sample values that will
       *                         be interpolated.
       *
       * \param width            Horizontal dimension (most rapidly varying
       *                         matrix subscript) of the source data matrix.
       *                         Must be greater than zero.
       *
       * \param height           Vertical dimension (most slowly varying matrix
       *                         subscript) of the source data matrix. Must be
       *                         greater than zero.
       *
       * \param unclipped        If false (default value), the interpolation
       *                         will constrain interpolated values to the
       *                         native range of the pixel sample type
       *                         represented by the template argument P. If
       *                         true, interpolated pixel values will be
       *                         returned unmodified. Note that this only makes
       *                         sense for floating point pixel sample types.
       */
      Interpolator( BidimensionalInterpolation<sample>* interpolation,
                    const sample* data, int width, int height, bool unclipped = false ) :
         m_interpolation( interpolation ),
         m_unclipped( unclipped )
      {
         if ( !m_interpolation.IsNull() )
            m_interpolation->Initialize( data, width, height );
      }

      /*!
       * Disabled copy constructor.
       */
      Interpolator( const Interpolator& ) = delete;

      /*!
       * Move constructor.
       */
      Interpolator( Interpolator&& ) = default;

      /*!
       * Destroys an %Interpolator object and all internal working structures.
       */
      virtual ~Interpolator()
      {
      }

      /*!
       * Returns a reference to the immutable two-dimensional interpolation
       * being used by this %Interpolator object.
       */
      const BidimensionalInterpolation<sample>& Interpolation() const
      {
         PCL_PRECONDITION( !m_interpolation.IsNull() )
         return *m_interpolation;
      }

      /*!
       * Returns a reference to the mutable two-dimensional interpolation being
       * used by this %Interpolator object.
       */
      BidimensionalInterpolation<sample>& Interpolation()
      {
         PCL_PRECONDITION( !m_interpolation.IsNull() )
         return *m_interpolation;
      }

      /*!
       * Interpolates a pixel sample value at the specified \a x and \a y image
       * coordinates.
       *
       * If this is an unclipped interpolator (see the class constructor), the
       * interpolated value will be returned unmodified. If this is a clipped
       * interpolator (default state), the returned value will be constrained
       * to stay within the native range of the pixel sample type represented
       * by the template argument P. Note that unclipped interpolation only
       * makes sense for floating point pixel sample types.
       */
      sample operator()( double x, double y ) const
      {
         PCL_PRECONDITION( !m_interpolation.IsNull() )
         double r = (*m_interpolation)( x, y );
         if ( !m_unclipped )
         {
            if ( r > P::MaxSampleValue() )
               return P::MaxSampleValue();
            if ( r < P::MinSampleValue() )
               return P::MinSampleValue();
         }
         return P::FloatToSample( r );
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

      AutoPointer<BidimensionalInterpolation<sample> > m_interpolation;
                                                  bool m_unclipped : 1;
   };

   // -------------------------------------------------------------------------

   /*!
    * Constructs a %PixelInterpolation object.
    */
   PixelInterpolation() = default;

   /*!
    * Copy constructor.
    */
   PixelInterpolation( const PixelInterpolation& ) = default;

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
    * Returns true iff this interpolation can be safely used in multithreaded
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
    * \param data    Two-dimensional matrix of pixel sample values that will be
    *                interpolated.
    *
    * \param width   Horizontal dimension (most rapidly varying matrix
    *                subscript) of the source data matrix.
    *
    * \param height  Vertical dimension (most slowly varying matrix subscript)
    *                of the source data matrix.
    */
   template <class P, class T>
   Interpolator<P>* NewInterpolator( const T* data, int width, int height, bool unclipped = false ) const
   {
      return new Interpolator<P>( NewInterpolation( data ), data, width, height, unclipped );
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
   NearestNeighborPixelInterpolation() = default;

   /*!
    * Copy constructor.
    */
   NearestNeighborPixelInterpolation( const NearestNeighborPixelInterpolation& ) = default;

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
   BilinearPixelInterpolation() = default;

   /*!
    * Copy constructor.
    */
   BilinearPixelInterpolation( const BilinearPixelInterpolation& ) = default;

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
    * The optional \e clamp parameter is a <em>linear clamping threshold</em>
    * for the bicubic spline interpolation algorithm. See the documentation for
    * BicubicSplineInterpolation for a detailed description of the automatic
    * linear clamping feature.
    */
   BicubicSplinePixelInterpolation( double clamp = __PCL_BICUBIC_SPLINE_CLAMPING_THRESHOLD ) :
      m_clamp( clamp )
   {
   }

   /*!
    * Copy constructor.
    */
   BicubicSplinePixelInterpolation( const BicubicSplinePixelInterpolation& ) = default;

   /*!
    */
   virtual String Description() const
   {
      return String().Format( "Bicubic spline interpolation, c=%.2f", m_clamp );
   }

private:

   float m_clamp;

   virtual BidimensionalInterpolation<FloatPixelTraits::sample>*
                  NewInterpolation( const FloatPixelTraits::sample* ) const
   {
      return new BicubicSplineInterpolation<FloatPixelTraits::sample>( m_clamp );
   }

   virtual BidimensionalInterpolation<DoublePixelTraits::sample>*
                  NewInterpolation( const DoublePixelTraits::sample* ) const
   {
      return new BicubicSplineInterpolation<DoublePixelTraits::sample>( m_clamp );
   }

   virtual BidimensionalInterpolation<UInt8PixelTraits::sample>*
                  NewInterpolation( const UInt8PixelTraits::sample* ) const
   {
      return new BicubicSplineInterpolation<UInt8PixelTraits::sample>( m_clamp );
   }

   virtual BidimensionalInterpolation<UInt16PixelTraits::sample>*
                  NewInterpolation( const UInt16PixelTraits::sample* ) const
   {
      return new BicubicSplineInterpolation<UInt16PixelTraits::sample>( m_clamp );
   }

   virtual BidimensionalInterpolation<UInt32PixelTraits::sample>*
                  NewInterpolation( const UInt32PixelTraits::sample* ) const
   {
      return new BicubicSplineInterpolation<UInt32PixelTraits::sample>( m_clamp );
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

   /*!
    * Copy constructor.
    */
   BicubicPixelInterpolation( const BicubicPixelInterpolation& ) = default;
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
   BicubicBSplinePixelInterpolation() = default;

   /*!
    * Copy constructor.
    */
   BicubicBSplinePixelInterpolation( const BicubicBSplinePixelInterpolation& ) = default;

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
    * \param filter  Reference to a CubicFilter instance that will be used as
    *                the interpolation filter.
    *
    */
   BicubicFilterPixelInterpolation( int rh, int rv, const CubicFilter& filter ) :
      m_rh( Max( 1, rh ) ),
      m_rv( Max( 1, rv ) )
   {
      PCL_PRECONDITION( rh >= 1 )
      PCL_PRECONDITION( rv >= 1 )
      m_filter = filter.Clone();
   }

   /*!
    * Copy constructor.
    */
   BicubicFilterPixelInterpolation( const BicubicFilterPixelInterpolation& x ) :
      m_rh( x.m_rh ),
      m_rv( x.m_rv )
   {
      m_filter = x.m_filter->Clone();
   }

   /*!
    * Move constructor.
    */
   BicubicFilterPixelInterpolation( BicubicFilterPixelInterpolation&& ) = default;

   /*!
    * Virtual destructor.
    */
   virtual ~BicubicFilterPixelInterpolation()
   {
   }

   /*!
    */
   virtual String Description() const
   {
      return "Bicubic interpolation, " + m_filter->Description() + String().Format( " (%dx%d)", 2*m_rh + 1, 2*m_rv + 1 );
   }

   /*!
    * Returns a reference to the immutable cubic filter being used by this
    * interpolation.
    */
   const CubicFilter& Filter() const
   {
      return *m_filter;
   }

private:

   int                      m_rh, m_rv;
   AutoPointer<CubicFilter> m_filter;

   virtual BidimensionalInterpolation<FloatPixelTraits::sample>*
                  NewInterpolation( const FloatPixelTraits::sample* ) const
   {
      return new BicubicFilterInterpolation<FloatPixelTraits::sample>( m_rh, m_rv, *m_filter );
   }

   virtual BidimensionalInterpolation<DoublePixelTraits::sample>*
                  NewInterpolation( const DoublePixelTraits::sample* ) const
   {
      return new BicubicFilterInterpolation<DoublePixelTraits::sample>( m_rh, m_rv, *m_filter );
   }

   virtual BidimensionalInterpolation<UInt8PixelTraits::sample>*
                  NewInterpolation( const UInt8PixelTraits::sample* ) const
   {
      return new BicubicFilterInterpolation<UInt8PixelTraits::sample>( m_rh, m_rv, *m_filter );
   }

   virtual BidimensionalInterpolation<UInt16PixelTraits::sample>*
                  NewInterpolation( const UInt16PixelTraits::sample* ) const
   {
      return new BicubicFilterInterpolation<UInt16PixelTraits::sample>( m_rh, m_rv, *m_filter );
   }

   virtual BidimensionalInterpolation<UInt32PixelTraits::sample>*
                  NewInterpolation( const UInt32PixelTraits::sample* ) const
   {
      return new BicubicFilterInterpolation<UInt32PixelTraits::sample>( m_rh, m_rv, *m_filter );
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
      m_n( Max( 1, n ) ),
      m_clamp( clamp )
   {
      PCL_PRECONDITION( n >= 1 )
      PCL_PRECONDITION( clamp < 0 || 0 <= clamp && clamp <= 1 )
   }

   /*!
    * Copy constructor.
    */
   LanczosPixelInterpolation( const LanczosPixelInterpolation& ) = default;

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
      m_clamp( clamp )
   {
      PCL_PRECONDITION( clamp < 0 || 0 <= clamp && clamp <= 1 )
   }

   /*!
    * Copy constructor.
    */
   Lanczos3LUTPixelInterpolation( const Lanczos3LUTPixelInterpolation& ) = default;

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
      m_clamp( Range( clamp, 0.0F, 1.0F ) )
   {
      PCL_PRECONDITION( clamp < 0 || 0 <= clamp && clamp <= 1 )
   }

   /*!
    * Copy constructor.
    */
   Lanczos4LUTPixelInterpolation( const Lanczos4LUTPixelInterpolation& ) = default;

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
      m_clamp( Range( clamp, 0.0F, 1.0F ) )
   {
      PCL_PRECONDITION( clamp < 0 || 0 <= clamp && clamp <= 1 )
   }

   /*!
    * Copy constructor.
    */
   Lanczos5LUTPixelInterpolation( const Lanczos5LUTPixelInterpolation& ) = default;

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
// EOF pcl/PixelInterpolation.h - Released 2017-08-01T14:23:31Z
