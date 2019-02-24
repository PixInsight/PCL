//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/ATrousWaveletTransform.h - Released 2019-01-21T12:06:07Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_ATrousWaveletTransform_h
#define __PCL_ATrousWaveletTransform_h

/// \file pcl/ATrousWaveletTransform.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/AutoPointer.h>
#include <pcl/KernelFilter.h>
#include <pcl/RedundantMultiscaleTransform.h>
#include <pcl/SeparableFilter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class InterlacedTransformation;

/*!
 * \class ATrousWaveletTransform
 * \brief Discrete isotropic à trous wavelet transform.
 *
 * The Isotropic Undecimated Wavelet Transform, also known as starlet transform
 * or <em>à trous</em> (with holes) wavelet transform, produces a coefficient
 * set {w1,w2,...,wN,cN}, where each wj is a set of zero-mean coefficients at
 * scale j, which we call <em>detail layer</em>, and cN is a large-scale
 * smoothed residual, which we call <em>residual layer</em>. Each layer has the
 * same dimensions as the input image, hence the transform is redundant.
 *
 * The wavelet function in the à trous algorithm is the difference between the
 * values of a scaling function F at two successive scales. Using the dyadic
 * scaling sequence, the wavelet function can be represented as
 * (F(x) - F(x/2)). The scaling function F can be any positive low-pass filter.
 *
 * The reconstruction algorithm consists of the sum of all wj detail layers
 * for 1 <= j <= N, plus the residual layer cN.
 *
 * \b References
 *
 * \li Jean-Luc Starck, Fionn Murtagh, Mario Bertero, <em>Handbook of
 * Mathematical Methods in Imaging</em>, ch. 34, <em>Starlet Transform in
 * Astronomical Data Processing</em>, Springer, 2011, pp. 1489-1531.
 *
 * \li Starck, J.-L., Murtagh, F. and J. Fadili, A., <em>Sparse %Image and
 * Signal Processing: Wavelets, Curvelets, Morphological Diversity</em>,
 * Cambridge University Press, 2010.
 *
 * \li Starck, J.-L., Murtagh, F., <em>Astronomical %Image and Data
 * Analysis</em>, Springer, 2002.
 *
 * \li Jean-Luc Starck, Fionn Murtagh, Albert Bijaoui, <em>%Image processing
 * and Data Analysis: The Multiscale Approach</em>, Cambridge University Press,
 * 1998.
 *
 * \b Implementation
 *
 * In our implementation, each layer in a wavelet transform is a floating-point
 * image with the same dimensions as the transformed image. Layers are indexed
 * from 0 to N. Layers at indexes from 0 to N-1 are detail layers, whose
 * elements are actually wavelet difference coefficients. Pixels in a detail
 * layer can be negative, zero or positive real values.
 *
 * The last layer, at index N, is the large-scale residual layer. Pixels in the
 * residual layer image can only be positive or zero real values.
 *
 * \ingroup multiscale_transforms
 *
 * \note The StarletTransform class is an alias for %ATrousWaveletTransform.
 */
class PCL_CLASS ATrousWaveletTransform : public RedundantMultiscaleTransform
{
public:

   /*!
    * Represents a wavelet layer.
    */
   typedef RedundantMultiscaleTransform::layer           layer;

   /*!
    * Represents a set of wavelet layers, or wavelet transform.
    */
   typedef RedundantMultiscaleTransform::transform       transform;

   /*!
    * Represents a set of layer enabled/disabled states.
    */
   typedef RedundantMultiscaleTransform::layer_state_set layer_state_set;

   /*!
    * \brief The scaling function of a wavelet transform.
    *
    * A wavelet scaling function can be either a non-separable kernel filter,
    * implemented as the KernelFilter class, or a separable filter implemented
    * as SeparableFilter.
    *
    * Separable filters are much better in terms of performance, since
    * separable convolution has O(N) complexity, as opposed to O(N^2) for
    * non-separable convolution.
    *
    * \sa KernelFilter, SeparableFilter
    */
   struct WaveletScalingFunction
   {
      AutoPointer<KernelFilter>    kernelFilter;    //!< Non-separable kernel filter
      AutoPointer<SeparableFilter> separableFilter; //!< Separable filter

      /*!
       * Default constructor. Constructs an uninitialized instance.
       */
      WaveletScalingFunction() = default;

      /*!
       * Non-separable filter constructor. The scaling function will own a
       * duplicate of the specified kernel filter.
       */
      WaveletScalingFunction( const KernelFilter& f )
      {
         kernelFilter = f.Clone();
         PCL_CHECK( !kernelFilter.IsNull() )
      }

      /*!
       * Separable filter constructor. The scaling function will own a
       * duplicate of the specified separable filter.
       */
      WaveletScalingFunction( const SeparableFilter& f )
      {
         separableFilter = f.Clone();
         PCL_CHECK( !separableFilter.IsNull() )
      }

      /*!
       * Copy constructor. The scaling function will own a duplicate of the
       * kernel or separable filter in the source object.
       */
      WaveletScalingFunction( const WaveletScalingFunction& s )
      {
         if ( !s.kernelFilter.IsNull() )
         {
            kernelFilter = s.kernelFilter->Clone();
            PCL_CHECK( !kernelFilter.IsNull() )
         }
         if ( !s.separableFilter.IsNull() )
         {
            separableFilter = s.separableFilter->Clone();
            PCL_CHECK( !separableFilter.IsNull() )
         }
      }

      /*!
       * Move constructor.
       */
      WaveletScalingFunction( WaveletScalingFunction&& s ) :
         kernelFilter( s.kernelFilter ),
         separableFilter( s.separableFilter )
      {
      }

      /*!
       * Destroys this scaling function object. Destroys and deallocates the
       * existing kernel or separable filter in this object.
       */
      virtual ~WaveletScalingFunction()
      {
      }

      /*!
       * Copy assignment operator. Returns a reference to this object.
       */
      WaveletScalingFunction& operator =( const WaveletScalingFunction& s )
      {
         if ( s.kernelFilter.IsNull() )
            kernelFilter.Destroy();
         else
         {
            kernelFilter = s.kernelFilter->Clone();
            PCL_CHECK( !kernelFilter.IsNull() )
         }

         if ( s.separableFilter.IsNull() )
            separableFilter.Destroy();
         else
         {
            separableFilter = s.separableFilter->Clone();
            PCL_CHECK( !separableFilter.IsNull() )
         }

         return *this;
      }

      /*!
       * Move assignment operator. Returns a reference to this object.
       */
      WaveletScalingFunction& operator =( WaveletScalingFunction&& ) = default;

      /*!
       * Returns true if this scaling function is a separable filter; false if
       * it is an invalid or non-separable kernel filter.
       */
      bool IsSeparable() const
      {
         return !separableFilter.IsNull() && !separableFilter->IsEmpty();
      }

      /*!
       * Returns true if this scaling function is a non-separable kernel
       * filter; false if it is an invalid or separable filter.
       */
      bool IsNonseparable() const
      {
         return !kernelFilter.IsNull() && !kernelFilter->IsEmpty();
      }

      /*!
       * Returns true iff this scaling function is valid, that is, if it owns a
       * nonempty filter.
       */
      bool IsValid() const
      {
         return IsSeparable() || IsNonseparable();
      }

      /*!
       * Causes this scaling function to own a duplicate of the specified
       * non-separable kernel filter. A previously existing filter is destroyed
       * and deallocated.
       */
      void Set( const KernelFilter& f )
      {
         separableFilter.Destroy();
         kernelFilter = f.Clone();
         PCL_CHECK( !kernelFilter.IsNull() )
      }

      /*!
       * Causes this scaling function to own a duplicate of the specified
       * separable filter. A previously existing filter is destroyed and
       * deallocated.
       */
      void Set( const SeparableFilter& f )
      {
         kernelFilter.Destroy();
         separableFilter = f.Clone();
         PCL_CHECK( !separableFilter.IsNull() )
      }

      /*!
       * Destroys the kernel and/or separable filter(s) owned by this object,
       * yielding an invalid instance.
       */
      void Clear()
      {
         kernelFilter.Destroy();
         separableFilter.Destroy();
      }

      /*!
       * Equality operator. Returns true only if this scaling function is equal
       * to another instance.
       */
      bool operator ==( const WaveletScalingFunction& other ) const
      {
         if ( !kernelFilter.IsNull() )
            return !other.kernelFilter.IsNull() && *kernelFilter == *other.kernelFilter;
         if ( !separableFilter.IsNull() )
            return !other.separableFilter.IsNull() && *separableFilter == *other.separableFilter;
         return other.kernelFilter.IsNull() && other.separableFilter.IsNull();
      }
   };

   /*!
    * Default constructor.
    *
    * \note This constructor yields an uninitialized instance that cannot be
    * used prior to initializing it with a reference to a filter object
    * (either KernelFilter or SeparableFilter), which will be used as the
    * scaling function of the wavelet transform.
    */
   ATrousWaveletTransform() = default;

   /*!
    * Constructs an %ATrousWaveletTransform instance using the specified
    * scaling function.
    *
    * \param f    A wavelet scaling function that can be either a non-separable
    *             filter (KernelFilter) or a separable filter (SeparableFilter).
    *
    * \param n    Number of wavelet layers. The transform will consist of \a n
    *             wavelet layers plus a residual layer, i.e. n+1 total layers.
    *
    * \param d    Scaling sequence. If \a d <= 0, the transform will use the
    *             dyadic sequence: 1, 2, 4, ... 2^i. If \a d > 0, its value is
    *             the distance in pixels between two successive scales.
    *
    * The default values for \a n and \a d are 4 and 0, respectively (four
    * wavelet layers and the dyadic scaling sequence).
    */
   ATrousWaveletTransform( const WaveletScalingFunction& f, int n = 4, int d = 0 ) :
      RedundantMultiscaleTransform( n, d ), m_scalingFunction( f )
   {
      PCL_CHECK( m_scalingFunction.IsValid() )
   }

   /*!
    * Constructs an %ATrousWaveletTransform instance that uses a non-separable
    * kernel filter as a scaling function.
    *
    * \param f    Non-separable filter that will be used as the scaling
    *             function of the transform. Must be a positive, low-pass
    *             filter function.
    *
    * \param n    Number of wavelet layers. The transform will consist of \a n
    *             wavelet layers plus a residual layer, i.e. n+1 total layers.
    *
    * \param d    Scaling sequence. If \a d <= 0, the transform will use the
    *             dyadic sequence: 1, 2, 4, ... 2^i. If \a d > 0, its value is
    *             the distance in pixels between two successive scales.
    *
    * The default values for \a n and \a d are 4 and 0, respectively (four
    * wavelet layers and the dyadic scaling sequence).
    */
   ATrousWaveletTransform( const KernelFilter& f, int n = 4, int d = 0 ) :
      RedundantMultiscaleTransform( n, d ), m_scalingFunction( f )
   {
      PCL_CHECK( m_scalingFunction.IsValid() )
   }

   /*!
    * Constructs an %ATrousWaveletTransform instance that uses a separable
    * kernel filter as a scaling function.
    *
    * \param f    Separable filter that will be used as the scaling function of
    *             the transform. Must be a positive, low-pass filter function.
    *
    * \param n    Number of wavelet layers. The transform will consist of \a n
    *             wavelet layers plus a residual layer, i.e. n+1 total layers.
    *
    * \param d    Scaling sequence. If \a d <= 0, the transform will use the
    *             dyadic sequence: 1, 2, 4, ... 2^i. If \a d > 0, its value is
    *             the distance in pixels between two successive scales.
    *
    * The default values for \a n and \a d are 4 and 0, respectively (four
    * wavelet layers and the dyadic scaling sequence).
    */
   ATrousWaveletTransform( const SeparableFilter& f, int n = 4, int d = 0 ) :
      RedundantMultiscaleTransform( n, d ), m_scalingFunction( f )
   {
      PCL_CHECK( m_scalingFunction.IsValid() )
   }

   /*!
    * Copy constructor.
    */
   ATrousWaveletTransform( const ATrousWaveletTransform& ) = default;

   /*!
    * Move constructor.
    */
   ATrousWaveletTransform( ATrousWaveletTransform&& ) = default;

   /*!
    * Destroys this %ATrousWaveletTransform object. All existing wavelet layers
    * and the internal scaling function filter object are destroyed and
    * deallocated.
    */
   virtual ~ATrousWaveletTransform()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   ATrousWaveletTransform& operator =( const ATrousWaveletTransform& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   ATrousWaveletTransform& operator =( ATrousWaveletTransform&& ) = default;

   /*!
    * Returns a reference to the (immutable) scaling function used by this
    * wavelet transform.
    */
   const WaveletScalingFunction& ScalingFunction() const
   {
      return m_scalingFunction;
   }

   /*!
    * Sets a new scaling function \a f for this wavelet transform.
    *
    * \note  As a consequence of calling this member function, all existing
    * wavelet layers in this transform are destroyed.
    */
   void SetScalingFunction( const WaveletScalingFunction& f )
   {
      DestroyLayers();
      m_scalingFunction = f;
      PCL_CHECK( m_scalingFunction.IsValid() )
   }

   /*!
    * Sets a non-separable kernel filter as the scaling function \a f used by
    * this wavelet transform.
    *
    * \note  As a consequence of calling this member function, all existing
    * wavelet layers in this transform are destroyed.
    */
   void SetScalingFunction( const KernelFilter& f )
   {
      DestroyLayers();
      m_scalingFunction.Set( f );
      PCL_CHECK( m_scalingFunction.IsValid() )
   }

   /*!
    * Sets a separable kernel filter as the scaling function \a f used by this
    * wavelet transform.
    *
    * \note  As a consequence of calling this member function, all existing
    * wavelet layers in this transform are destroyed.
    */
   void SetScalingFunction( const SeparableFilter& f )
   {
      DestroyLayers();
      m_scalingFunction.Set( f );
      PCL_CHECK( m_scalingFunction.IsValid() )
   }

   /*!
    * Estimation of the standard deviation of the noise, assuming a Gaussian
    * noise distribution. This routine implements the k-sigma clipping noise
    * estimation algorithm described by Starck et al. (see the references in
    * the detailed documentation for this class). The algorithm is described
    * for example in <em>Astronomical %Image and Data Analysis</em>, pp. 37-38.
    *
    * This routine can be used to provide an initial estimate to the more
    * accurate <em>multiresolution support noise estimation algorithm</em>,
    * implemented as the NoiseMRS() routine. When used with a relative error
    * bound (see the \a eps parameter), this routine can easily yield noise
    * estimates to within 1% accuracy.
    *
    * \param j    Wavelet layer index (zero-based). The default index is 0.
    *
    * \param k    Clipping multiplier in sigma units. The default value is 3.
    *
    * \param eps  Fractional relative accuracy. If this parameter is greater
    *             than zero, the algorithm will iterate until the difference
    *             between two successive iterations is less than \a eps. The
    *             default value is 0.01, so this routine iterates to achieve an
    *             estimate to within 1% accuracy.
    *
    * \param n    Maximum number of iterations. When \a eps is zero, this is
    *             the fixed number of iterations of the noise estimation
    *             algorithm. Three iterations usually give an estimate to
    *             within 5% accuracy. 5 or 6 iterations can provide 1% accuracy
    *             in most cases. When \a eps is greater than zero, this
    *             parameter works as a security limit to prevent too long
    *             execution times when convergence is slow (which shouldn't
    *             happen under normal conditions). The default value is 10.
    *
    * \param[out] N  Pointer to a variable that will receive the total number
    *             of pixels tagged as noise during the noise evaluation
    *             process. This pointer can legally be zero, which is also
    *             the default value of this argument.
    *
    * Returns the estimated standard deviation of the noise in the specified
    * scale \a j of the wavelet transform after a relative \a eps accuracy has
    * been reached or \a n sigma clipping iterations have been performed,
    * whichever happens first.
    *
    * The returned value must be scaled by the standard deviation of the
    * Gaussian noise at the specified wavelet scale. The scaling factor depends
    * on the wavelet scaling function used to perform the wavelet decomposition
    * and must be coherent with the transform performed by this object.
    *
    * If this %ATrousWaveletTransform object does not contain a valid wavelet
    * transform, or if the specified wavelet layer has been deleted, this
    * routine throws an Error exception.
    */
   double NoiseKSigma( int j = 0, float k = 3,
                       float eps = 0.01, int n = 10, size_type* N = 0 ) const;

   /*!
    * Estimation of the standard deviation of the noise, assuming a Gaussian
    * noise distribution, for a specified range of pixel values.
    *
    * This routine implements essentially the same algorithm as its unbounded
    * counterpart:
    *
    * NoiseKSigma( int j, float k, float eps, int n, size_type* N ).
    *
    * The difference is that this version allows you to specify a valid range
    * of pixel values with the \a low, \a high and \a image parameters. The
    * standard deviation of the noise will only be computed for those pixels
    * whose values in the specified \a image pertain to the range
    * (<em>low</em>,<em>high</em>), that is, for every pixel with value \a v in
    * \a image such that the condition \a low < \e v < \a high is true.
    *
    * The specified \a image must be compatible with the wavelet transform. In
    * particular, the dimensions of \a image must be identical to those of the
    * wavelet layers in this transform; otherwise an Error exception will be
    * thrown. For selection of pixels within the specified range, only the
    * currently selected channel in \a image will be taken into account.
    * Normally, the specified \a image must be the same image that was used to
    * compute the current wavelet decomposition in this object.
    *
    * For detailed information on the rest of parameters, the implemented
    * algorithm, and special usage conditions for this routine, refer to the
    * documentation for the unbounded version of this member function.
    */
   double NoiseKSigma( int j, const ImageVariant& image,
                       float low = 0.00002F, float high = 0.99998F,
                       float k = 3, float eps = 0.01, int n = 10, size_type* N = 0 ) const;

   /*!
    * Estimation of the standard deviation of the Gaussian noise from the
    * multiresolution support. This routine implements the iterative algorithm
    * described by Jean-Luc Starck and Fionn Murtagh in their paper
    * <em>Automatic Noise Estimation from the Multiresolution Support</em>
    * (Publications of the Royal Astronomical Society of the Pacific, vol. 110,
    * February 1998, pp. 193-199).
    *
    * \param image   The original image. Normally this image should be the same
    *                image from which this wavelet transform has been
    *                calculated.
    *
    * \param sj      Noise standard deviation at each wavelet scale for a
    *                Gaussian noise distribution with unit sigma. There must be
    *                at least NumberOfLayers() elements in the array pointed to
    *                by this parameter.
    *
    * \param sigma   Initial estimate of the noise standard deviation in the
    *                image. The default value is zero. The best starting value
    *                is the result of the NoiseKSigma() routine. However, the
    *                noise estimate provided by NoiseKSigma() is relative to a
    *                particular wavelet layer, so it must be scaled as
    *                appropriate to make it coherent with the whole image.
    *
    * \param k       Clipping multiplier in sigma units. The default value is 3.
    *
    * \param[out] N  Pointer to a variable that will receive the total number
    *                of pixels tagged as noise during the noise evaluation
    *                process. This pointer can legally be zero, which is also
    *                the default value of this argument.
    *
    * \param low     Lower bound of the sampling range in the normalized [0,1]
    *                range. Pixel sample values less than or equal to \a low
    *                will be excluded from the noise evaluation process. The
    *                default value is 0.00002.
    *
    * \param high    Upper bound of the sampling range in the normalized [0,1]
    *                range. Pixel sample values greater than or equal to
    *                \a high will be excluded from the noise evaluation
    *                process. The default value is 0.99998.
    *
    * Returns the estimated standard deviation of the noise from the
    * multiresolution support, using all wavelet scales available. As long as
    * successive noise estimates converge to a stable solution, this routine
    * performs the necessary iterations until a relative fractional accuracy of
    * 1e-4 is achieved. Normally this requires between 4 and 8 iterations,
    * depending on the relation between the noise and significant structures in
    * the image.
    *
    * If no convergence is achieved after a large number of iterations, this
    * function returns zero and, if a nonzero N argument pointer is specified,
    * sets *N = 0. This should never happen if this wavelet transform defines a
    * reasonable number of wavelet layers (4 or 5 layers are recommended) and
    * the passed parameters are valid and coherent with the wavelet transform.
    *
    * If this %ATrousWaveletTransform object does not contain a valid wavelet
    * transform, if any wavelet layer has been deleted, or if the specified
    * image doesn't have the same geometry as the wavelet layers in this
    * transform, this routine throws an Error exception.
    */
   double NoiseMRS( const ImageVariant& image, const float sj[],
                    double sigma = 0, float k = 3, size_type* N = 0,
                    float low = 0.00002F, float high = 0.99998F ) const;

private:

   /*
    * Wavelet scaling function.
    */
   WaveletScalingFunction m_scalingFunction;

   /*
    * Transform (decomposition)
    */
   void Transform( const pcl::Image& ) override;
   void Transform( const pcl::DImage& ) override;
   void Transform( const pcl::ComplexImage& ) override;
   void Transform( const pcl::DComplexImage& ) override;
   void Transform( const pcl::UInt8Image& ) override;
   void Transform( const pcl::UInt16Image& ) override;
   void Transform( const pcl::UInt32Image& ) override;

   void ValidateScalingFunction() const;

   friend class ATWTDecomposition;
};

// ----------------------------------------------------------------------------

/*!
 * \class pcl::StarletTransform
 * \brief Starlet wavelet transform, a synonym for ATrousWaveletTransform.
 *
 * The isotropic stationary wavelet transform known as <em>à trous wavelet
 * transform</em> since the early publications of Mallat, Starck and Murtagh in
 * the 90's, is now known "officially" as <em>starlet transform</em>, at least
 * since 2010's <em>%Sparse %Image and %Signal %Processing</em> book.
 *
 * \ingroup multiscale_transforms
 */
typedef ATrousWaveletTransform   StarletTransform;

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ATrousWaveletTransform_h

// ----------------------------------------------------------------------------
// EOF pcl/ATrousWaveletTransform.h - Released 2019-01-21T12:06:07Z
