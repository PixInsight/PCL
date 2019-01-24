//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/FFTConvolution.h - Released 2019-01-21T12:06:07Z
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

#ifndef __PCL_FFTConvolution_h
#define __PCL_FFTConvolution_h

/// \file pcl/FFTConvolution.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/AutoPointer.h>
#include <pcl/ImageTransformation.h>
#include <pcl/KernelFilter.h>
#include <pcl/ParallelProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup fft_convolution_limits_macros Constants for Automatic&nbsp;\
 * Selection of FFT-based, Separable and Nonseparable Convolutions
 */

/*!
 * \def PCL_FFT_CONVOLUTION_IS_FASTER_THAN_SEPARABLE_FILTER_SIZE
 * \brief Defines a filter size in pixels above which FFT-based convolution is
 * consistently faster than separable convolution on the current PixInsight/PCL
 * platform.
 *
 * This value has been determined experimentally, and has been optimized for
 * parallel execution on relatively fast machines with eight or more logical
 * processor cores.
 *
 * \ingroup fft_convolution_limits_macros
 */
#define PCL_FFT_CONVOLUTION_IS_FASTER_THAN_SEPARABLE_FILTER_SIZE     162

/*!
 * \def PCL_FFT_CONVOLUTION_IS_FASTER_THAN_NONSEPARABLE_FILTER_SIZE
 * \brief Defines a filter size in pixels above which FFT-based convolution is
 * consistently faster than nonseparable convolution on the current
 * PixInsight/PCL platform.
 *
 * This value has been determined experimentally, and has been optimized for
 * parallel execution on relatively fast machines with eight or more logical
 * processor cores. Note that separable convolution is \e always faster than
 * nonseparable convolution, so this value should only be considered for
 * convolution with nonseparable filters.
 *
 * \ingroup fft_convolution_limits_macros
 */
#define PCL_FFT_CONVOLUTION_IS_FASTER_THAN_NONSEPARABLE_FILTER_SIZE  12

/*!
 * \class FFTConvolution
 * \brief Fourier-based two-dimensional convolution
 *
 * %FFTConvolution implements a high-performance, two-dimensional discrete
 * convolution algorithm via fast Fourier transforms. It performs automatic
 * fixing of border artifacts by applying Neumann boundary conditions
 * (mirroring), and is able to convolve images and response functions of
 * arbitrary sizes, only limited by the available memory.
 *
 * \sa Convolution, SeparableConvolution, ATrousWaveletTransform, ImageTransformation
*/
class PCL_CLASS FFTConvolution : public ImageTransformation, public ParallelProcess
{
public:

   /*!
    * Default constructor.
    *
    * \note This constructor yields an uninitialized instance that cannot be
    * used before explicit association with a response function, specified
    * either as a KernelFilter object or as an ImageVariant.
    */
   FFTConvolution() = default;

   /*!
    * Constructs an %FFTConvolution instance with the specified kernel filter.
    *
    * \param filter  Response function, or <em>convolution filter</em>. The
    *                specified object does not have to remain valid while this
    *                %FFTConvolution instance is actively used, since it owns a
    *                private copy of the filter (note that KernelFilter is a
    *                reference-counted class).
    */
   FFTConvolution( const KernelFilter& filter ) :
      m_filter( filter.Clone() )
   {
      PCL_CHECK( bool( m_filter ) )
   }

   /*!
    * Constructs an %FFTConvolution instance with the specified response
    * function image.
    *
    * \param f    A reference to an ImageVariant whose transported image is the
    *             <em>response function</em>, or <em>convolution filter</em>.
    *             The transported image <em>must remain valid</em> while this
    *             %FFTConvolution instance is actively used, or until a new
    *             response function is associated with this instance. The
    *             specified %ImageVariant doesn't have to remain valid, since a
    *             %FFTConvolution instance owns a private %ImageVariant that
    *             transports the response function image.
    */
   FFTConvolution( const ImageVariant& f ) :
      m_image( f )
   {
      PCL_CHECK( bool( m_image ) )
   }

   /*!
    * Copy constructor.
    */
   FFTConvolution( const FFTConvolution& x ) :
      ImageTransformation( x ),
      ParallelProcess( x ),
      m_image( x.m_image ),
      m_outputRealCmp( x.m_outputRealCmp )
   {
      if ( x.m_filter )
         m_filter = x.m_filter->Clone();
   }

   /*!
    * Move constructor.
    */
   FFTConvolution( FFTConvolution&& ) = default;

   /*!
    * Destroys this %FFTConvolution object.
    */
   virtual ~FFTConvolution()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   FFTConvolution& operator =( const FFTConvolution& x )
   {
      if ( &x != this )
      {
         (void)ImageTransformation::operator =( x );
         (void)ParallelProcess::operator =( x );
         DestroyFilter();
         if ( x.m_filter )
            m_filter = x.m_filter->Clone();
         else
            m_image = x.m_image;
         m_outputRealCmp = x.m_outputRealCmp;
      }
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   FFTConvolution& operator =( FFTConvolution&& ) = default;

   /*!
    * Returns true if this %FFTConvolution uses a KernelFilter object as its
    * response function; false if it uses an image.
    */
   bool UsingFilter() const
   {
      return m_filter;
   }

   /*!
    * Returns a reference to the kernel filter currently associated with this
    * %FFTConvolution object. If this object does not use a kernel filter,
    * either because it uses an image as its response function, or because this
    * object has not been initialized, the returned filter is empty (i.e., it
    * has no filter coefficients and zero dimensions).
    */
   const KernelFilter& Filter() const
   {
      PCL_PRECONDITION( bool( m_filter ) )
      return *m_filter;
   }

   /*!
    * Returns a reference to the filter image currently associated with this
    * %FFTConvolution object. The referenced ImageVariant object will transport
    * no image if this object does not use a filter image, either because it
    * uses a kernel filter as its response function, or because this object has
    * not been initialized.
    *
    * The response function is actually the image transported by the returned
    * ImageVariant. It can be an image of any supported data type.
    */
   const ImageVariant& FilterImage() const
   {
      return m_image;
   }

   /*!
    * Sets a new kernel filter as the response function to be applied by this
    * %FFTConvolution object.
    *
    * The specified object does not have to remain valid while this
    * %FFTConvolution instance is actively used, since it owns a private copy
    * of the filter (note that KernelFilter is a reference-counted class).
    */
   void SetFilter( const KernelFilter& filter )
   {
      DestroyFilter();
      m_filter = filter.Clone();
      PCL_CHECK( bool( m_filter ) )
   }

   /*!
    * Sets a new filter image as the response function to be applied by this
    * %FFTConvolution object.
    *
    * The image transported by the specified ImageVariant must remain valid
    * while this %FFTConvolution instance is actively used, or until a new
    * response function is associated with this instance. The %ImageVariant
    * doesn't have to remain valid, since this object owns a private
    * %ImageVariant that transports the response function image.
    */
   void SetFilter( const ImageVariant& filter )
   {
      DestroyFilter();
      m_image = filter;
      PCL_CHECK( bool( m_image ) )
   }

   /*!
    * Returns a pointer to the discrete Fourier transform (DFT) of the
    * complex <em>response function</em> used internally by this
    * %FFTConvolution object, or nullptr if the response function has not been
    * created yet.
    *
    * The internal DFT of the response function is created and initialized the
    * first time this %FFTConvolution object is used to perform a convolution.
    * It is reused to save memory and CPU resources, as long as successive
    * transformations apply to target images with the same dimensions, or
    * otherwise it is re-created on the fly, as necessary. It is destroyed when
    * a new filter is associated with this object.
    *
    * This function returns a pointer to a complex image that stores the DFT
    * of the original filter after transforming it to <em>wrap around
    * order</em>. This means that the original filter data has been splitted,
    * mirrored, and redistributed to occupy the four corners of the complex 2-D
    * matrix prior to calculating its DFT.
    *
    * If this object has not been initialized, the returned pointer is nullptr.
    */
   const ComplexImage* ResponseFunctionDFT() const
   {
      return m_h;
   }

   /*!
    * Returns true if this object applies to real images by copying the real
    * component of the complex convolved result.
    *
    * Returns false if this object applies to real images by storing the
    * magnitude of the complex convolution result. This is the default
    * behavior.
    *
    * \sa IsMagnitudeOutputEnabled(), EnableRealComponentOutput()
    */
   bool IsRealComponentOutputEnabled() const
   {
      return m_outputRealCmp;
   }

   /*!
    * Returns true if this object applies to real images by storing the
    * magnitude of the complex convolution result. This is the default
    * behavior.
    *
    * Returns false if this object applies to real images by copying the real
    * component of the complex convolved result.
    *
    * \sa IsRealComponentOutputEnabled(), EnableMagnitudeOutput()
    */
   bool IsMagnitudeOutputEnabled() const
   {
      return !m_outputRealCmp;
   }

   /*!
    * Causes this object to apply to real images by copying the real component
    * of the complex convolved result.
    *
    * \sa IsRealComponentOutputEnabled(), EnableMagnitudeOutput()
    */
   void EnableRealComponentOutput( bool enable = true )
   {
      m_outputRealCmp = enable;
   }

   /*!
    * Causes this object to apply to real images by storing the magnitude of
    * the complex convolution result. This is the default behavior.
    *
    * \sa IsMagnitudeOutputEnabled(), EnableRealComponentOutput()
    */
   void EnableMagnitudeOutput( bool enable = true )
   {
      m_outputRealCmp = !enable;
   }

protected:

   /*
    * The response function for convolution is defined as either a KernelFilter
    * instance or as an image. In the latter case, the image transported by the
    * ImageVariant must remain valid while this object is actively used.
    */
   AutoPointer<KernelFilter> m_filter;
   ImageVariant              m_image;
   bool                      m_outputRealCmp = false;

   /*
    * Internal DFT of the response function. Initially zero. This matrix is
    * generated/reused/regenerated as this object is applied to convolve
    * different target images. It is destroyed when a new filter is specified.
    */
   mutable AutoPointer<ComplexImage> m_h;

   /*
    * In-place Fourier-based 2-D convolution algorithm.
    */
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
   void Apply( pcl::ComplexImage& ) const override;
   void Apply( pcl::DComplexImage& ) const override;

   friend class PCL_FFTConvolutionEngine;

private:

   void DestroyFilter()
   {
      m_filter.Destroy();
      m_image.Free();
      m_h.Destroy();
   }

   void Validate() const;
   void ValidateFilter() const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_FFTConvolution_h

// ----------------------------------------------------------------------------
// EOF pcl/FFTConvolution.h - Released 2019-01-21T12:06:07Z
