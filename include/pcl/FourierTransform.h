//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/FourierTransform.h - Released 2017-04-14T23:04:40Z
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

#ifndef __PCL_FourierTransform_h
#define __PCL_FourierTransform_h

/// \file pcl/FourierTransform.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/FFT1D.h>
#include <pcl/FFT2D.h>
#include <pcl/ImageTransformation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::FFTDirection
 * \brief Defines directions and types of fast Fourier transforms.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>FFTDirection::Forward</td>  <td>Fourier transform</td></tr>
 * <tr><td>FFTDirection::Backward</td> <td>Inverse Fourier transform</td></tr>
 * </table>
 */
namespace FFTDirection
{
   enum value_type
   {
      Forward,
      Backward
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class InPlaceFourierTransform
 * \brief In-place discrete Fourier transform of two-dimensional complex images.
 *
 * %InPlaceFourierTransform performs in-place, discrete Fourier transforms of
 * complex-sampled two-dimensional images using fast Fourier transform
 * algorithms.
 *
 * %InPlaceFourierTransform can only be applied to 32-bit and 64-bit floating
 * point complex images: either ComplexImage or DComplexImage.
 *
 * \note \b Important - For performance reasons, %InPlaceFourierTransform can
 * change the dimensions of a transformed image (width and/or height in pixels)
 * to their nearest optimized %FFT lengths. This may involve reallocation of
 * all pixel data. If the transformed image already has optimized dimensions,
 * no size change or reallocation occurs. See the GenericFFT::GenericFFT( int )
 * constructor for details on optimized %FFT lengths.
 *
 * \sa FourierTransform
 */
class PCL_CLASS InPlaceFourierTransform : public ImageTransformation
{
public:

   /*!
    * Represents a %FFT direction and type.
    */
   typedef FFTDirection::value_type    transform_type;

   /*!
    * Constructs an %InPlaceFourierTransform instance.
    *
    * \param type Transform direction and type. This parameter can have one of
    *             the following values:\n
    *             \n
    * <table border="1" cellpadding="4" cellspacing="0">
    * <tr><td>FFTDirection::Forward</td>
    *     <td>Fourier transform. This is the default value.</td></tr>
    * <tr><td>FFTDirection::Backward</td>
    *     <td>Inverse Fourier transform</td></tr>
    * </table>
    */
   InPlaceFourierTransform( transform_type type = FFTDirection::Forward ) :
      ImageTransformation(),
      m_type( type ), m_parallel( true ), m_maxProcessors( PCL_MAX_PROCESSORS )
   {
   }

   /*!
    * Copy constructor.
    */
   InPlaceFourierTransform( const InPlaceFourierTransform& x ) :
      ImageTransformation( x ),
      m_type( x.m_type ), m_parallel( x.m_parallel ), m_maxProcessors( x.m_maxProcessors )
   {
   }

   /*!
    * Destroys this %InPlaceFourierTransform object.
    */
   virtual ~InPlaceFourierTransform()
   {
   }

   /*!
    * Returns the type and direction of this Fourier transform. See the
    * FFTDirection namespace for possible values.
    */
   transform_type Type() const
   {
      return m_type;
   }

   /*!
    * Returns true iff this object is allowed to use multiple parallel execution
    * threads (when multiple threads are permitted and available).
    */
   bool IsParallelProcessingEnabled() const
   {
      return m_parallel;
   }

   /*!
    * Enables parallel processing for this instance of
    * %InPlaceFourierTransform.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * \param maxProcessors    The maximum number of processors allowed for this
    *                instance of %InPlaceFourierTransform. If \a enable is
    *                false this parameter is ignored. A value <= 0 is ignored.
    *                The default value is zero.
    */
   void EnableParallelProcessing( bool enable = true, int maxProcessors = 0 )
   {
      m_parallel = enable;
      if ( enable && maxProcessors > 0 )
         SetMaxProcessors( maxProcessors );
   }

   /*!
    * Disables parallel processing for this instance of
    * %InPlaceFourierTransform.
    *
    * This is a convenience function, equivalent to:
    * EnableParallelProcessing( !disable )
    */
   void DisableParallelProcessing( bool disable = true )
   {
      EnableParallelProcessing( !disable );
   }

   /*!
    * Returns the maximum number of processors allowed for this instance of
    * %InPlaceFourierTransform.
    *
    * Irrespective of the value returned by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   int MaxProcessors() const
   {
      return m_maxProcessors;
   }

   /*!
    * Sets the maximum number of processors allowed for this instance of
    * %InPlaceFourierTransform.
    *
    * In the current version of PCL, a module can use a maximum of 1023
    * processors. The term \e processor actually refers to the number of
    * threads a module can execute concurrently.
    *
    * Irrespective of the value specified by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   void SetMaxProcessors( int maxProcessors )
   {
      m_maxProcessors = unsigned( Range( maxProcessors, 1, PCL_MAX_PROCESSORS ) );
   }

protected:

   transform_type m_type;
   bool           m_parallel      : 1;
   unsigned       m_maxProcessors : PCL_MAX_PROCESSORS_BITCOUNT;

   // In-place FFT of complex images.
   virtual void Apply( pcl::ComplexImage& ) const;
   virtual void Apply( pcl::DComplexImage& ) const;
};

// ----------------------------------------------------------------------------

/*!
 * \class InPlaceInverseFourierTransform
 * \brief In-place discrete inverse Fourier transform of two-dimensional complex images.
 *
 * %InPlaceInverseFourierTransform performs in-place, inverse discrete Fourier
 * transforms of complex-sampled two-dimensional images using fast Fourier
 * transform algorithms.
 *
 * This is a convenience class derived from InPlaceFourierTransform. It
 * implements exactly the same functions as its parent class, but performs
 * inverse Fourier transforms by default.
 *
 * \sa InPlaceFourierTransform
 */
class PCL_CLASS InPlaceInverseFourierTransform : public InPlaceFourierTransform
{
public:

   /*!
    * Constructs an %InPlaceInverseFourierTransform instance.
    */
   InPlaceInverseFourierTransform() :
      InPlaceFourierTransform( FFTDirection::Backward )
   {
   }

   /*!
    * Copy constructor.
    */
   InPlaceInverseFourierTransform( const InPlaceInverseFourierTransform& x ) :
      InPlaceFourierTransform( x )
   {
   }

   /*!
    * Virtual destructor.
    */
   virtual ~InPlaceInverseFourierTransform()
   {
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class FourierTransform
 * \brief Out-of-place discrete Fourier transform of two-dimensional images.
 *
 * %FourierTransform performs out-of-place, discrete Fourier transforms of
 * complex and real two-dimensional images, using fast Fourier transform
 * algorithms.
 *
 * %FourierTransform stores a Fourier transform as either a ComplexImage
 * (32-bit floating point) or a DComplexImage (64-bit floating point). The
 * transform image is transported by an ImageVariant object. 64-bit transforms
 * are created for 64-bit floating point (both real and complex) and 32-bit
 * integer (real) images. For the rest of images a 32-bit transform is used.
 *
 * \sa InPlaceFourierTransform
 */
class PCL_CLASS FourierTransform : public BidirectionalImageTransformation
{
public:

   /*!
    * Default constructor.
    *
    * Creates a %FourierTransform object with an empty Fourier transform.
    */
   FourierTransform() :
      BidirectionalImageTransformation(),
      m_dft(), m_parallel( true ), m_maxProcessors( PCL_MAX_PROCESSORS )
   {
   }

   /*!
    * Copy constructor.
    *
    * This constructor copies the Fourier transform in the source object \a x.
    * The Fourier transform is an ImageVariant object. This constructor creates
    * an %ImageVariant that references the same image as the source object.
    */
   FourierTransform( const FourierTransform& x ) :
      BidirectionalImageTransformation( x ),
      m_dft( x.m_dft ), m_parallel( x.m_parallel ), m_maxProcessors( x.m_maxProcessors )
   {
   }

   /*!
    * Move constructor.
    */
   FourierTransform( FourierTransform&& x ) :
      BidirectionalImageTransformation( x ),
      m_dft( std::move( x.m_dft ) ), m_parallel( x.m_parallel ), m_maxProcessors( x.m_maxProcessors )
   {
   }

   /*!
    * Destroys this %FourierTransform object.
    *
    * The current Fourier transform, if it exists, is automatically released.
    * The Fourier transform is implemented as an ImageVariant object. If there
    * are no external references to the image transported by the %ImageVariant,
    * then it is also destroyed and deallocated.
    */
   virtual ~FourierTransform()
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    *
    * This operator copies the Fourier transform in the source object \a x.
    * The Fourier transform is an ImageVariant object. This operator causes the
    * %ImageVariant in this object to reference the same image as the source
    * object. If this object already stores a Fourier transform, it is released
    * before assignment.
    */
   FourierTransform& operator =( const FourierTransform& x )
   {
      (void)BidirectionalImageTransformation::operator =( x );
      m_dft = x.m_dft;
      m_parallel = x.m_parallel;
      m_maxProcessors = x.m_maxProcessors;
      return *this;
   }

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   FourierTransform& operator =( FourierTransform&& x )
   {
      (void)BidirectionalImageTransformation::operator =( x );
      m_dft = std::move( x.m_dft );
      m_parallel = x.m_parallel;
      m_maxProcessors = x.m_maxProcessors;
      return *this;
   }

   /*!
    * Returns a constant reference to the Fourier transform in this
    * %FourierTransform object.
    *
    * The Fourier transform is an ImageVariant object. It can transport either
    * a 32-bit (float) or 64-bit (double) complex image, that is, either a
    * ComplexImage or a DComplexImage, depending on the sample type of the
    * transformed image.
    */
   const ImageVariant& DFT() const
   {
      return m_dft;
   }

   /*!
    * Returns a reference to the Fourier transform in this %FourierTransform
    * object.
    *
    * The Fourier transform is an ImageVariant object. It can transport either
    * a 32-bit (float) or 64-bit (double) complex image, that is, either a
    * ComplexImage or a DComplexImage, depending on the sample type of the
    * transformed image.
    */
   ImageVariant& DFT()
   {
      return m_dft;
   }

   /*!
    * A synonym for DFT() const.
    */
   const ImageVariant& operator *() const
   {
      return DFT();
   }

   /*!
    * A synonym for DFT().
    */
   ImageVariant& operator *()
   {
      return DFT();
   }

   /*!
    * Transfers ownership of the current Fourier transform to the caller.
    *
    * Returns a copy of the ImageVariant object that transports the current
    * Fourier transform in this object. Then the internal ImageVariant of this
    * object is released. The transform itself, which is either a ComplexImage
    * or a DComplexImage, will not be destroyed, as the newly created
    * %ImageVariant will transport and reference it.
    *
    * If this object stores no Fourier transform, the returned %ImageVariant
    * will be empty (no image transported).
    */
   ImageVariant ReleaseTransform()
   {
      ImageVariant r;
      m_dft.ReleaseTo( r );
      return r;
   }

   /*!
    * Releases the current Fourier transform, if exists.
    *
    * If there are no external references to the image transported by the
    * internal ImageVariant object, then the transform matrix, which is either
    * a ComplexImage or a DComplexImage object, is destroyed.
    */
   void Clear()
   {
      m_dft.Free();
   }

   /*!
    * Returns true iff this object is allowed to use multiple parallel execution
    * threads (when multiple threads are permitted and available).
    */
   bool IsParallelProcessingEnabled() const
   {
      return m_parallel;
   }

   /*!
    * Enables parallel processing for this instance of %FourierTransform.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * \param maxProcessors    The maximum number of processors allowed for this
    *                instance of %FourierTransform. If \a enable is false
    *                this parameter is ignored. A value <= 0 is ignored. The
    *                default value is zero.
    */
   void EnableParallelProcessing( bool enable = true, int maxProcessors = 0 )
   {
      m_parallel = enable;
      if ( enable && maxProcessors > 0 )
         SetMaxProcessors( maxProcessors );
   }

   /*!
    * Disables parallel processing for this instance of %FourierTransform.
    *
    * This is a convenience function, equivalent to:
    * EnableParallelProcessing( !disable )
    */
   void DisableParallelProcessing( bool disable = true )
   {
      EnableParallelProcessing( !disable );
   }

   /*!
    * Returns the maximum number of processors allowed for this instance of
    * %FourierTransform.
    *
    * Irrespective of the value returned by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   int MaxProcessors() const
   {
      return m_maxProcessors;
   }

   /*!
    * Sets the maximum number of processors allowed for this instance of
    * %FourierTransform.
    *
    * In the current version of PCL, a module can use a maximum of 1023
    * processors. The term \e processor actually refers to the number of
    * threads a module can execute concurrently.
    *
    * Irrespective of the value specified by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   void SetMaxProcessors( int maxProcessors )
   {
      m_maxProcessors = unsigned( Range( maxProcessors, 1, PCL_MAX_PROCESSORS ) );
   }

protected:

   // Discrete 2-D Fourier Transform
   ImageVariant m_dft;
   bool         m_parallel      : 1;
   unsigned     m_maxProcessors : PCL_MAX_PROCESSORS_BITCOUNT;

   // Transform
   virtual void Transform( const pcl::Image& );
   virtual void Transform( const pcl::DImage& );
   virtual void Transform( const pcl::ComplexImage& );
   virtual void Transform( const pcl::DComplexImage& );
   virtual void Transform( const pcl::UInt8Image& );
   virtual void Transform( const pcl::UInt16Image& );
   virtual void Transform( const pcl::UInt32Image& );

   // Inverse transform
   virtual void Apply( pcl::Image& ) const;
   virtual void Apply( pcl::DImage& ) const;
   virtual void Apply( pcl::ComplexImage& ) const;
   virtual void Apply( pcl::DComplexImage& ) const;
   virtual void Apply( pcl::UInt8Image& ) const;
   virtual void Apply( pcl::UInt16Image& ) const;
   virtual void Apply( pcl::UInt32Image& ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_FourierTransform_h

// ----------------------------------------------------------------------------
// EOF pcl/FourierTransform.h - Released 2017-04-14T23:04:40Z
