//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0841
// ----------------------------------------------------------------------------
// pcl/MultiscaleLinearTransform.h - Released 2017-06-17T10:55:43Z
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

#ifndef __PCL_MultiscaleLinearTransform_h
#define __PCL_MultiscaleLinearTransform_h

/// \file pcl/MultiscaleLinearTransform.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/RedundantMultiscaleTransform.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class MultiscaleLinearTransform
 * \brief A redundant multiscale transform using separable convolutions.
 *
 * The multiscale linear transform algorithm produces a set {w1,w2,...,wN,cN},
 * where each wj is a set of coefficients at scale j, which we call <em>detail
 * layer</em>, and cN is a large-scale smoothed residual, which we call
 * <em>residual layer</em>. Each layer has the same dimensions as the input
 * image, hence the generated multiscale transform is redundant.
 *
 * The algorithm applies successive convolutions with separable filter kernels
 * of increasing size 2*s + 1, where s grows following a monotonically
 * increasing sequence (the dyadic sequence 1, 2, 4, ... is used by default).
 * Multiscale coefficients are the differences between each pair of successive
 * convolved images. By default Gaussian filters are used, but block average
 * filters can also be used (see the class constructor) for special
 * applications.
 *
 * The reconstruction algorithm consists of the sum of all wj multiscale layers
 * for 1 <= j <= N, plus the residual layer cN.
 *
 * In our implementation, each layer in a multiscale linear transform is a
 * floating-point image with the same dimensions as the transformed image.
 * Layers are indexed from 0 to N. Layers at indexes from 0 to N-1 are detail
 * layers, whose elements are actually convolved difference coefficients.
 * Pixels in a detail layer can be negative, zero or positive real values.
 *
 * The last layer, at index N, is the large-scale <em>residual layer</em>.
 * Pixels in the residual layer image can only be positive or zero real values.
 *
 * \ingroup multiscale_transforms
 */
class PCL_CLASS MultiscaleLinearTransform : public RedundantMultiscaleTransform
{
public:

   /*!
    * Represents a multiscale transform layer.
    */
   typedef RedundantMultiscaleTransform::layer           layer;

   /*!
    * Represents a set of multiscale transform layers, or multiscale transform.
    */
   typedef RedundantMultiscaleTransform::transform       transform;

   /*!
    * Represents a set of layer enabled/disabled states.
    */
   typedef RedundantMultiscaleTransform::layer_state_set layer_state_set;

   /*!
    * Constructs a %MultiscaleLinearTransform instance.
    *
    * \param n    Number of detail layers. The transform will consist of \a n
    *             detail layers plus a residual layer, that is n+1 total
    *             layers. The default value is 4.
    *
    * \param d    Scaling sequence. If \a d <= 0, the transform will use the
    *             dyadic sequence: 1, 2, 4, ... 2^i. If \a d > 0, its value is
    *             the distance in pixels between two successive scales.
    *
    * \param useMeanFilters   If true, the transformation will use block
    *             average filters (mean) instead of Gaussian filters. Mean
    *             filters have important special applications, such as
    *             computation of multiscale local variances. Gaussian filters
    *             are always used by default.
    *
    * The default values for \a n and \a d are 4 and 0, respectively (four
    * layers and the dyadic scaling sequence).
    *
    * Successive layers are computed by applying separable convolutions with
    * kernel filters of size 2*s + 1. The scaling sequence parameter \a d
    * is interpreted as follows:
    *
    * - If the specified sequence parameter \a d is zero 0, then the transform
    *   uses the dyadic sequence: s = 1, 2, 4, ..., 2^j for 0 <= j < n.
    *
    * - If \a d > 0, then \a d is the constant increment in pixels between two
    *   successive scales (linear scaling sequence): s = d*j for 1 <= j < n.
    */
   MultiscaleLinearTransform( int n = 4, int d = 0, bool useMeanFilters = false ) :
      RedundantMultiscaleTransform( n, d ),
      m_useMeanFilters( useMeanFilters )
   {
   }

   /*!
    * Copy constructor.
    */
   MultiscaleLinearTransform( const MultiscaleLinearTransform& ) = default;

   /*!
    * Move constructor.
    */
#ifndef _MSC_VER
   MultiscaleLinearTransform( MultiscaleLinearTransform&& ) = default;
#endif

   /*!
    * Destroys this %MultiscaleLinearTransform object. All existing transform
    * layers are destroyed and deallocated.
    */
   virtual ~MultiscaleLinearTransform()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   MultiscaleLinearTransform& operator =( const MultiscaleLinearTransform& x )
   {
      (void)RedundantMultiscaleTransform::operator =( x );
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   MultiscaleLinearTransform& operator =( MultiscaleLinearTransform&& x )
   {
      (void)RedundantMultiscaleTransform::operator =( std::move( x ) );
      return *this;
   }

   /*!
    * Returns true iff this transform applies block average filters instead of
    * Gaussian filters. See the class constructor for more information.
    */
   bool UsesMeanFilters() const
   {
      return m_useMeanFilters;
   }

   /*!
    * Returns true iff this transform applies Gaussian filters instead of block
    * average filters. See the class constructor for more information.
    */
   bool UsesGaussianFilters() const
   {
      return !m_useMeanFilters;
   }

protected:

   /*
    * Whether we should use mean (block average) or Gaussian separable filters.
    */
   bool m_useMeanFilters : 1;

   /*
    * Transform (decomposition)
    */
   virtual void Transform( const pcl::Image& );
   virtual void Transform( const pcl::DImage& );
   virtual void Transform( const pcl::ComplexImage& );
   virtual void Transform( const pcl::DComplexImage& );
   virtual void Transform( const pcl::UInt8Image& );
   virtual void Transform( const pcl::UInt16Image& );
   virtual void Transform( const pcl::UInt32Image& );

   friend class MLTDecomposition;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_MultiscaleLinearTransform_h

// ----------------------------------------------------------------------------
// EOF pcl/MultiscaleLinearTransform.h - Released 2017-06-17T10:55:43Z
