//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/FourierTransform.h - Released 2018-11-01T11:06:36Z
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

#ifndef __PCL_FourierTransform_h
#define __PCL_FourierTransform_h

/// \file pcl/FourierTransform.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/FFT1D.h>
#include <pcl/FFT2D.h>
#include <pcl/ImageTransformation.h>
#include <pcl/ParallelProcess.h>

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
 * pixel data. If the transformed image already has optimized dimensions, no
 * size change or reallocation occurs. See the GenericFFT::GenericFFT( int )
 * constructor for details on optimized %FFT lengths.
 *
 * \sa FourierTransform
 */
class PCL_CLASS InPlaceFourierTransform : public ImageTransformation, public ParallelProcess
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
      m_type( type )
   {
   }

   /*!
    * Copy constructor.
    */
   InPlaceFourierTransform( const InPlaceFourierTransform& ) = default;

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

protected:

   transform_type m_type;

   // In-place FFT of complex images.
   void Apply( pcl::ComplexImage& ) const override;
   void Apply( pcl::DComplexImage& ) const override;
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
   InPlaceInverseFourierTransform( const InPlaceInverseFourierTransform& ) = default;

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
class PCL_CLASS FourierTransform : public BidirectionalImageTransformation, public ParallelProcess
{
public:

   /*!
    * Default constructor.
    *
    * Creates a %FourierTransform object with an empty Fourier transform.
    */
   FourierTransform() = default;

   /*!
    * Copy constructor.
    *
    * This constructor copies the Fourier transform in the specified source
    * object. The Fourier transform is an ImageVariant object. This constructor
    * creates an %ImageVariant that references the same image as the source
    * object.
    */
   FourierTransform( const FourierTransform& ) = default;

   /*!
    * Move constructor.
    */
   FourierTransform( FourierTransform&& ) = default;

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
    * This operator copies the Fourier transform in the source object. The
    * Fourier transform is an ImageVariant object. This operator causes the
    * %ImageVariant in this object to reference the same image as the source
    * object. If this object already stores a Fourier transform, it is released
    * before assignment.
    */
   FourierTransform& operator =( const FourierTransform& ) = default;

   /*!
    * Move assignment operator. Returns a reference to this object.
    */
   FourierTransform& operator =( FourierTransform&& ) = default;

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

protected:

   // Discrete 2-D Fourier Transform
   ImageVariant m_dft;

   // Transform
   void Transform( const pcl::Image& ) override;
   void Transform( const pcl::DImage& ) override;
   void Transform( const pcl::ComplexImage& ) override;
   void Transform( const pcl::DComplexImage& ) override;
   void Transform( const pcl::UInt8Image& ) override;
   void Transform( const pcl::UInt16Image& ) override;
   void Transform( const pcl::UInt32Image& ) override;

   // Inverse transform
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::ComplexImage& ) const override;
   void Apply( pcl::DComplexImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_FourierTransform_h

// ----------------------------------------------------------------------------
// EOF pcl/FourierTransform.h - Released 2018-11-01T11:06:36Z
