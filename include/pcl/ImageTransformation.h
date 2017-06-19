//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0841
// ----------------------------------------------------------------------------
// pcl/ImageTransformation.h - Released 2017-06-17T10:55:43Z
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

#ifndef __PCL_ImageTransformation_h
#define __PCL_ImageTransformation_h

/// \file pcl/ImageTransformation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Exception.h>
#include <pcl/ImageVariant.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ImageTransformation
 * \brief Root base class of all PCL image transformations.
 *
 * %ImageTransformation represents a generic transformation that can be applied
 * to any image type defined in PCL.
 *
 * %ImageTransformation is a base class for all PCL image transformations,
 * including both unidirectional (single-step) and bidirectional (invertible,
 * two-step) transformations.
 *
 * For single-step processes, %ImageTransformation represents a <em>direct
 * transformation</em> that applies to a target image.
 *
 * Two-step invertible processes are derived classes of
 * BidirectionalImageTransformation in PCL. For these processes
 * %ImageTransformation represents an <em>inverse transformation</em> that
 * applies to a target image after a previous <em>direct transformation</em>,
 * usually performed by the same class.
 *
 * \sa BidirectionalImageTransformation
 */
class PCL_CLASS ImageTransformation
{
public:

   /*!
    * Constructs a default %ImageTransformation object.
    */
   ImageTransformation() = default;

   /*!
    * Copy constructor.
    */
   ImageTransformation( const ImageTransformation& ) = default;

   /*!
    * Destroys an %ImageTransformation object.
    */
   virtual ~ImageTransformation()
   {
   }

   /*!
    * Applies this transformation to a target \a image. Returns a reference to
    * the target image.
    *
    * If this is a bidirectional transformation, then this function represents
    * an <em>inverse transformation</em> requiring a previous <em>direct
    * transformation</em>, usually performed by the same class.
    */
   template <class P>
   GenericImage<P>& operator >>( GenericImage<P>& image ) const
   {
      Apply( image );
      return image;
   }

   /*!
    * Applies this transformation to the current selection in a target \a image
    * transported by the specified ImageVariant object. Returns a reference to
    * the target %ImageVariant.
    *
    * If this is a bidirectional transformation, then this function represents
    * an <em>inverse transformation</em> requiring a previous <em>direct
    * transformation</em>, usually performed by the same class.
    *
    * \sa BidirectionalImageTransformation
    */
   ImageVariant& operator >>( ImageVariant& image ) const
   {
      if ( image )
         if ( image.IsFloatSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: Apply( static_cast<pcl::Image&>( *image ) ); break;
            case 64: Apply( static_cast<pcl::DImage&>( *image ) ); break;
            }
         else if ( image.IsComplexSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: Apply( static_cast<pcl::ComplexImage&>( *image ) ); break;
            case 64: Apply( static_cast<pcl::DComplexImage&>( *image ) ); break;
            }
         else
            switch ( image.BitsPerSample() )
            {
            case  8: Apply( static_cast<pcl::UInt8Image&>( *image ) ); break;
            case 16: Apply( static_cast<pcl::UInt16Image&>( *image ) ); break;
            case 32: Apply( static_cast<pcl::UInt32Image&>( *image ) ); break;
            }
      return image;
   }

protected:

   /*!
    * Applies this transformation to a 32-bit floating point \a image.
    */
   virtual void Apply( pcl::Image& image ) const
   {
      throw NotImplemented( *this, "Apply to 32-bit floating-point images" );
   }

   /*!
    * Applies this transformation to a 64-bit floating point \a image.
    */
   virtual void Apply( pcl::DImage& image ) const
   {
      throw NotImplemented( *this, "Apply to 64-bit floating-point images" );
   }

   /*!
    * Applies this transformation to a 32-bit complex \a image.
    */
   virtual void Apply( pcl::ComplexImage& image ) const
   {
      throw NotImplemented( *this, "Apply to 32-bit complex images" );
   }

   /*!
    * Applies this transformation to a 64-bit complex \a image.
    */
   virtual void Apply( pcl::DComplexImage& image ) const
   {
      throw NotImplemented( *this, "Apply to 64-bit complex images" );
   }

   /*!
    * Applies this transformation to an 8-bit unsigned integer \a image.
    */
   virtual void Apply( pcl::UInt8Image& image ) const
   {
      throw NotImplemented( *this, "Apply to 8-bit integer images" );
   }

   /*!
    * Applies this transformation to a 16-bit unsigned integer \a image.
    */
   virtual void Apply( pcl::UInt16Image& image ) const
   {
      throw NotImplemented( *this, "Apply to 16-bit integer images" );
   }

   /*!
    * Applies this transformation to a 32-bit unsigned integer \a image.
    */
   virtual void Apply( pcl::UInt32Image& image ) const
   {
      throw NotImplemented( *this, "Apply to 32-bit integer images" );
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class BidirectionalImageTransformation
 * \brief Root base class for bidirectional PCL image transformations.
 *
 * %BidirectionalImageTransformation represents a two-step, invertible process
 * that can be applied to any image type defined in PCL.
 *
 * Two-step invertible processes can perform an <em>inverse transformation</em>
 * after a previous <em>direct transformation</em>. The inverse transformation
 * functionality is provided by the ImageTransformation base class.
 *
 * \sa ImageTransformation
 */
class PCL_CLASS BidirectionalImageTransformation : public ImageTransformation
{
public:

   /*!
    * Constructs a default %BidirectionalImageTransformation object
    */
   BidirectionalImageTransformation() = default;

   /*!
    * Copy constructor.
    */
   BidirectionalImageTransformation( const BidirectionalImageTransformation& ) = default;

   /*!
    * Destroys a %BidirectionalImageTransformation object.
    */
   virtual ~BidirectionalImageTransformation()
   {
   }

   /*!
    * Applies this transformation to the current selection in a target
    * \a image. Returns a reference to the target image.
    *
    * This function represents a <em>direct transformation</em> applied to the
    * target image. An <em>inverse transformation</em> is performed by the
    * operator >>( const GenericImage& ) member function, inherited from
    * ImageTransformation.
    */
   template <class P>
   const GenericImage<P>& operator <<( const GenericImage<P>& image )
   {
      Transform( image );
      return image;
   }

   /*!
    * Applies this transformation to the current selection in a target \a image
    * transported by the specified ImageVariant object. Returns a reference to
    * the target %ImageVariant.
    *
    * This function represents a <em>direct transformation</em> applied to the
    * target image. An <em>inverse transformation</em> is performed by the
    * operator >>( const ImageVariant& ) member function, inherited from
    * ImageTransformation.
    */
   const ImageVariant& operator <<( const ImageVariant& image )
   {
      if ( image )
         if ( image.IsFloatSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: Transform( static_cast<const pcl::Image&>( *image ) ); break;
            case 64: Transform( static_cast<const pcl::DImage&>( *image ) ); break;
            }
         else if ( image.IsComplexSample() )
            switch ( image.BitsPerSample() )
            {
            case 32: Transform( static_cast<const pcl::ComplexImage&>( *image ) ); break;
            case 64: Transform( static_cast<const pcl::DComplexImage&>( *image ) ); break;
            }
         else
            switch ( image.BitsPerSample() )
            {
            case  8: Transform( static_cast<const pcl::UInt8Image&>( *image ) ); break;
            case 16: Transform( static_cast<const pcl::UInt16Image&>( *image ) ); break;
            case 32: Transform( static_cast<const pcl::UInt32Image&>( *image ) ); break;
            }
      return image;
   }

protected:

   /*!
    * Transforms a 32-bit floating point \a image.
    */
   virtual void Transform( const pcl::Image& image )
   {
      throw NotImplemented( *this, "Transformation of 32-bit floating-point images" );
   }

   /*!
    * Transforms a 64-bit floating point \a image.
    */
   virtual void Transform( const pcl::DImage& image )
   {
      throw NotImplemented( *this, "Transformation of 64-bit floating-point images" );
   }

   /*!
    * Transforms a 32-bit complex \a image.
    */
   virtual void Transform( const pcl::ComplexImage& image )
   {
      throw NotImplemented( *this, "Transformation of 32-bit complex images" );
   }

   /*!
    * Transforms a 64-bit complex \a image.
    */
   virtual void Transform( const pcl::DComplexImage& image )
   {
      throw NotImplemented( *this, "Transformation of 64-bit complex images" );
   }

   /*!
    * Transforms an 8-bit unsigned integer \a image.
    */
   virtual void Transform( const pcl::UInt8Image& image )
   {
      throw NotImplemented( *this, "Transformation of 8-bit integer images" );
   }

   /*!
    * Transforms a 16-bit unsigned integer \a image.
    */
   virtual void Transform( const pcl::UInt16Image& image )
   {
      throw NotImplemented( *this, "Transformation of 16-bit integer images" );
   }

   /*!
    * Transforms a 32-bit unsigned integer \a image.
    */
   virtual void Transform( const pcl::UInt32Image& image )
   {
      throw NotImplemented( *this, "Transformation of 32-bit integer images" );
   }
};

// ----------------------------------------------------------------------------

template <class P> inline
GenericImage<P>& GenericImage<P>::Apply( const ImageTransformation& transformation,
                                         const Rect& rect, int firstChannel, int lastChannel )
{
   Rect r = rect;
   if ( !ParseSelection( r, firstChannel, lastChannel ) )
      return *this;

   PushSelections();
   SelectRectangle( r );
   SelectChannelRange( firstChannel, lastChannel );

   try
   {
      transformation >> *this;
      PopSelections();
   }
   catch ( ... )
   {
      PopSelections();
      throw;
   }

   return *this;
}

template <class P> inline
void GenericImage<P>::Transform( BidirectionalImageTransformation& transform,
                                 const Rect& rect, int firstChannel, int lastChannel ) const
{
   Rect r = rect;
   if ( !ParseSelection( r, firstChannel, lastChannel ) )
      return;

   PushSelections();
   SelectRectangle( r );
   SelectChannelRange( firstChannel, lastChannel );

   try
   {
      transform << *this;
      PopSelections();
   }
   catch ( ... )
   {
      PopSelections();
      throw;
   }
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageTransformation_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageTransformation.h - Released 2017-06-17T10:55:43Z
