//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0837
// ----------------------------------------------------------------------------
// pcl/Image.h - Released 2017-06-09T08:12:42Z
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

#ifndef __PCL_Image_h
#define __PCL_Image_h

/// \file pcl/Image.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/AbstractImage.h>
#include <pcl/AutoLock.h>
#include <pcl/Complex.h>
#include <pcl/Compression.h>
#include <pcl/File.h>
#include <pcl/Mutex.h>
#include <pcl/PixelAllocator.h>
#include <pcl/PixelTraits.h>
#include <pcl/ReferenceArray.h>
#include <pcl/Vector.h>

#ifndef __PCL_IMAGE_NO_BITMAP
#  ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
#    ifndef __PI_Bitmap_h
#      include <API/Bitmap.h> // using server-side bitmaps
#    endif
using namespace pi;
#  else
#    ifndef __PCL_Bitmap_h
#      include <pcl/Bitmap.h> // using client-side bitmaps
#    endif
#  endif
#  ifndef __PCL_Color_h
#    include <pcl/Color.h>    // for RGBA
#  endif
#endif

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
namespace pi
{
   class SharedImage;
}
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::ImageOp
 * \brief     Arithmetic, bitwise logical and pixel composition operations.
 *
 * Supported operations:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>ImageOp::Nop</td>         <td>No operation</td></tr>
 * <tr><td>ImageOp::Mov</td>         <td>Copy pixels</td></tr>
 * <tr><td>ImageOp::Add</td>         <td>Add</td></tr>
 * <tr><td>ImageOp::Sub</td>         <td>Subtract</td></tr>
 * <tr><td>ImageOp::Mul</td>         <td>Multiply</td></tr>
 * <tr><td>ImageOp::Div</td>         <td>Divide</td></tr>
 * <tr><td>ImageOp::Pow</td>         <td>Raise</td></tr>
 * <tr><td>ImageOp::Dif</td>         <td>Absolute difference</td></tr>
 * <tr><td>ImageOp::Min</td>         <td>Minimum value</td></tr>
 * <tr><td>ImageOp::Max</td>         <td>Maximum value</td></tr>
 * <tr><td>ImageOp::Or</td>          <td>Bitwise OR (inclusive OR)</td></tr>
 * <tr><td>ImageOp::And</td>         <td>Bitwise AND</td></tr>
 * <tr><td>ImageOp::Xor</td>         <td>Bitwise XOR (exclusive OR)</td></tr>
 * <tr><td>ImageOp::Not</td>         <td>Bitwise NOT</td></tr>
 * <tr><td>ImageOp::Nor</td>         <td>Bitwise NOR (inclusive NOR)</td></tr>
 * <tr><td>ImageOp::Nand</td>        <td>Bitwise NAND</td></tr>
 * <tr><td>ImageOp::Xnor</td>        <td>Bitwise XNOR (exclusive NOR)</td></tr>
 * <tr><td>ImageOp::ColorBurn</td>   <td>Color burn pixel composition operator</td></tr>
 * <tr><td>ImageOp::LinearBurn</td>  <td>Linear burn pixel composition operator</td></tr>
 * <tr><td>ImageOp::Screen</td>      <td>Screen pixel composition operator</td></tr>
 * <tr><td>ImageOp::ColorDodge</td>  <td>Color dodge pixel composition operator</td></tr>
 * <tr><td>ImageOp::Overlay</td>     <td>Overlay pixel composition operator</td></tr>
 * <tr><td>ImageOp::SoftLight</td>   <td>Soft light pixel composition operator</td></tr>
 * <tr><td>ImageOp::HardLight</td>   <td>Hard light pixel composition operator</td></tr>
 * <tr><td>ImageOp::VividLight</td>  <td>Vivid light pixel composition operator</td></tr>
 * <tr><td>ImageOp::LinearLight</td> <td>Linear light pixel composition operator</td></tr>
 * <tr><td>ImageOp::PinLight</td>    <td>Pin light pixel composition operator</td></tr>
 * <tr><td>ImageOp::Exclusion</td>   <td>Exclusion pixel composition operator</td></tr>
 * </table>
 */
namespace ImageOp
{
   enum value_type
   {
      Nop,         // No operation
      Mov,         // a = b
      Add,         // a + b
      Sub,         // a - b
      Mul,         // a * b
      Div,         // a / b
      Pow,         // Pow( a, b )
      Dif,         // |a - b|
      Min,         // Min( a, b )
      Max,         // Max( a, b )
      Or,          // a | b
      And,         // a & b
      Xor,         // a ^ b
      Not,         // ~a
      Nor,         // ~(a | b)
      Nand,        // ~(a & b)
      Xnor,        // ~(a ^ b)
      ColorBurn,   // 1 - Min( (1 - a)/b, 1 )
      LinearBurn,  // a + b - 1
      Screen,      // 1 - (1 - a)*(1 - b)
      ColorDodge,  // Min( a/(1 - b), 1 )
      Overlay,     // (a > 0.5) ? 1 - ((1 - 2*(a - 0.5)) * (1 - b)) : 2*a*b
      SoftLight,   // (b > 0.5) ? 1 - (1 - a)*(1 - b - 0.5) : a*(b + 0.5)
      HardLight,   // (b > 0.5) ? 1 - (1 - a)*(1 - 2*(b - 0.5)) : 2*a*b
      VividLight,  // (b > 0.5) ? 1 - Max( (1 - a)/(b - 0.5)/2, 1.0 ) : Min( a/(1 - 2*b ), 1.0 )
      LinearLight, // (b > 0.5) ? Max( a + 2*(b - 0.5), 1.0 ) : Max( a + 2*b - 1, 1.0 )
      PinLight,    // (b > 0.5) ? Max( a, 2*(b - 0.5) ) : Min( a, 2*b )
      Exclusion,   // 0.5 - 2*(a - 0.5)*(b - 0.5)
      NumberOfOperators
   };

   /*!
    * Returns true iff the specified operator \a op is an arithmetic operator.
    */
   inline bool IsArithmeticOperator( int op )
   {
      return op >= Add && op <= Dif;
   }

   /*!
    * Returns true iff the specified operator \a op is a bitwise logical
    * operator.
    */
   inline bool IsBitwiseLogicalOperator( int op )
   {
      return op >= Or && op <= Xnor;
   }

   /*!
    * Returns true iff the specified operator \a op is a move or replace
    * operator.
    */
   inline bool IsMoveOperator( int op )
   {
      return op == Mov || op == Min || op == Max;
   }

   /*!
    * Returns true iff the specified operator \a op is a pixel composition
    * operator.
    */
   inline bool IsPixelCompositionOperator( int op )
   {
      return op >= ColorBurn && op <= Exclusion;
   }

   /*!
    * Returns a string with the name of a pixel \a operation.
    */
   String Id( value_type operation );
}

// ----------------------------------------------------------------------------

#define m_pixelData        m_data->data
#define m_allocator        m_data->allocator

#define m_width            m_geometry->width
#define m_height           m_geometry->height
#define m_numberOfChannels m_geometry->numberOfChannels

#define m_colorSpace       m_color->colorSpace
#define m_RGBWS            m_color->RGBWS

#define m_channel          m_selected.channel
#define m_lastChannel      m_selected.lastChannel
#define m_point            m_selected.point
#define m_rectangle        m_selected.rectangle
#define m_clipLow          m_selected.clipLow
#define m_clipHigh         m_selected.clipHigh
#define m_clipped          m_selected.clipped

// ----------------------------------------------------------------------------

class PCL_CLASS ImageVariant;
class PCL_CLASS ImageTransformation;
class PCL_CLASS BidirectionalImageTransformation;

// ----------------------------------------------------------------------------

/*!
 * \class GenericImage
 * \brief Implements a generic, two-dimensional, shared or local image.
 *
 * Template class %GenericImage implements a two-dimensional image in PCL. It
 * inherits all public properties and functionality from its base classes:
 * ImageGeometry, ImageColor, and AbstractImage, and adds a rich set of
 * processing primitives through a versatile and easily adaptable interface.
 *
 * Template instantiations of %GenericImage and GenericPixelTraits provide
 * instantiable image classes adapted to particular pixel sample types. Seven
 * image types are already predefined in this way by PCL, corresponding to all
 * supported real, integer and complex two-dimensional images in the current
 * versions of the PixInsight platform:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>pcl::Image</td>         <td>32-bit floating point real image</td></tr>
 * <tr><td>pcl::DImage</td>        <td>64-bit floating point real image</td></tr>
 * <tr><td>pcl::ComplexImage</td>  <td>32-bit floating point complex image</td></tr>
 * <tr><td>pcl::DComplexImage</td> <td>64-bit floating point complex image</td></tr>
 * <tr><td>pcl::UInt8Image</td>    <td>8-bit unsigned integer image</td></tr>
 * <tr><td>pcl::UInt16Image</td>   <td>16-bit unsigned integer image</td></tr>
 * <tr><td>pcl::UInt32Image</td>   <td>32-bit unsigned integer image</td></tr>
 * </table>
 *
 * Developers can define new image types easily by writing suitable
 * instantiations of GenericPixelTraits and the corresponding instantiations of
 * %GenericImage. This system allows for the implementation of new image types
 * tightly adapted to particular requirements.
 *
 * \sa \ref image_types_2d "2-D Image Types",
 * \ref image_iterators "Image Iterators", ImageGeometry, ImageColor,
 * AbstractImage, ImageVariant, GenericPixelTraits
 */
template <class P>
class PCL_CLASS GenericImage : public AbstractImage
{
public:

   /*!
    * Represents the GenericPixelTraits instantiation used in this template
    * instantiation of %GenericImage.
    *
    * The \c pixel_traits type identifies a class implementing basic storage
    * and functional primitives adapted to a particular sample data type.
    */
   typedef P                                    pixel_traits;

   /*!
    * Represents a class responsible for allocation and deallocation of pixel
    * data in this template instantiation of %GenericImage. \c pixel_allocator
    * must be able to handle shared and local pixel data allocation and
    * deallocation in a transparent way.
    *
    * By default, %GenericImage uses an instantiation of the PixelAllocator
    * template class for \c pixel_traits.
    */
   typedef PixelAllocator<P>                    pixel_allocator;

   /*!
    * Represents the data type used to store pixel sample values in this
    * template instantiation of %GenericImage.
    */
   typedef typename pixel_traits::sample        sample;

   /*!
    * An enumerated type that represents the set of supported color spaces.
    * Valid constants for this enumeration are defined in the ColorSpace
    * namespace.
    */
   typedef AbstractImage::color_space           color_space;

   /*!
    * The type of a container class used to store rectangular and channel range
    * selections.
    */
   typedef AbstractImage::selection_stack       selection_stack;

   /*!
    * An enumerated type that represents the set of supported arithmetic and
    * bitwise pixel operations. Valid constants for this enumeration are
    * defined in the ImageOp namespace.
    */
   typedef ImageOp::value_type                  image_op;

   /*!
    * A vector of pixel sample values.
    */
   typedef GenericVector<sample>                sample_vector;

   /*!
    * A dynamic array of pixel sample values.
    */
   typedef Array<sample>                        sample_array;

   // -------------------------------------------------------------------------

   /*!
    * \defgroup image_iterators Image Iterator Classes
    */

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::GenericImage::sample_iterator
    * \brief Mutable pixel sample iterator.
    *
    * A mutable pixel sample iterator provides read/write, random access to
    * pixel samples in a single channel of an image.
    *
    * \ingroup image_iterators
    */
   class sample_iterator
   {
   public:

      /*!
       * Represents the type of the iterated image.
       */
      typedef GenericImage<P>                   image_type;

      /*!
       * Represents the pixel traits class used by the iterated image.
       */
      typedef typename image_type::pixel_traits pixel_traits;

      /*!
       * Represents a pixel sample of the iterated image.
       */
      typedef typename image_type::sample       sample;

      /*!
       * Default constructor. Initializes an invalid iterator.
       */
      sample_iterator() :
         m_image( nullptr ), m_iterator( nullptr ), m_end( nullptr )
      {
      }

      /*!
       * Constructs a mutable pixel sample iterator for one channel of an
       * image.
       *
       * \param image   The image to iterate.
       *
       * \param channel Channel index. If this parameter is a negative integer,
       *                then this object will be initialized to iterate on the
       *                currently selected channel of the \a image. If an
       *                invalid (i.e., nonexistent) channel index is specified,
       *                then the resulting iterator will also be invalid, with
       *                an empty iteration range. The default value is -1.
       */
      sample_iterator( image_type& image, int channel = -1 ) :
         m_image( &image ), m_iterator( nullptr ), m_end( nullptr )
      {
         m_image->EnsureUnique();
         if ( m_image->ParseChannel( channel ) )
         {
            m_iterator = (*m_image)[channel];
            m_end = m_iterator + m_image->NumberOfPixels();
         }
      }

      /*!
       * Constructs a mutable pixel sample iterator for a range of contiguous
       * pixel samples of an image.
       *
       * \param image   The image to iterate.
       *
       * \param i       Pointer to the first pixel sample in the iteration
       *                range. The iterator will be initialized to access this
       *                pixel sample just after construction.
       *
       * \param j       End of the iteration range. The iterator will be
       *                invalid when it reaches (or surpasses) this point.
       *
       * \note Both iteration limits \a i and \a j must be pointers to pixel
       * samples in the \e same channel of the specified \a image.
       */
      sample_iterator( image_type& image, sample* i, sample* j ) :
         m_image( &image ), m_iterator( i ), m_end( j )
      {
      }

      /*!
       * Copy constructor.
       */
      sample_iterator( const sample_iterator& ) = default;

      /*!
       * Copy assignment operator. Returns a reference to this object.
       */
      sample_iterator& operator =( const sample_iterator& ) = default;

      /*!
       * Returns true only if this iterator is valid. A valid iterator defines
       * a valid iterated image and a non-null position.
       */
      bool IsValid() const
      {
         return m_image != nullptr && m_iterator != nullptr;
      }

      /*!
       * Returns a reference to the image being iterated by this object.
       */
      image_type& Image() const
      {
         return *m_image;
      }

      /*!
       * Returns a pointer to the pixel sample pointed to by this iterator.
       */
      sample* Position() const
      {
         return m_iterator;
      }

      /*!
       * Boolean type conversion operator. Returns true if this iterator is
       * active. A pixel sample iterator is active if it has not reached (or
       * surpassed) its iteration limit: either the end of the iterated image
       * channel, or the end of the iteration range, depending on how the
       * iterator has been constructed.
       */
      operator bool() const
      {
         return m_iterator < m_end;
      }

      /*!
       * Indirection operator. Returns a reference to the pixel sample pointed
       * to by this iterator.
       */
      sample& operator *() const
      {
         return *m_iterator;
      }

      /*!
       * Pre-increment operator. Causes this iterator to point to the next
       * pixel sample in the iterated image channel. Returns a reference to
       * this iterator.
       */
      sample_iterator& operator ++()
      {
         ++m_iterator;
         return *this;
      }

      /*!
       * Post-increment operator. Causes this iterator to point to the next
       * pixel sample in the iterated image channel. Returns a copy of the
       * iterator as it was before incrementing it.
       */
      sample_iterator operator ++( int )
      {
         return sample_iterator( *m_image, m_iterator++, m_end );
      }

      /*!
       * Pre-decrement operator. Causes this iterator to point to the previous
       * pixel sample in the iterated image channel, then returns a reference
       * to this iterator.
       */
      sample_iterator& operator --()
      {
         --m_iterator;
         return *this;
      }

      /*!
       * Post-decrement operator. Causes this iterator to point to the previous
       * pixel sample in the iterated image channel. Returns a copy of the
       * iterator as it was before decrementing it.
       */
      sample_iterator operator --( int )
      {
         return sample_iterator( *m_image, m_iterator--, m_end );
      }

      /*!
       * Scalar assignment/addition operator. Increments this iterator by a
       * distance \a delta from its current position. Positive increments cause
       * this iterator to move forward by \a delta pixel samples. Negative
       * increments move this iterator backward by \a delta pixel samples.
       * Returns a reference to this iterator.
       */
      sample_iterator& operator +=( distance_type delta )
      {
         m_iterator += delta;
         return *this;
      }

      /*!
       * Scalar assignment/subtraction operator. Decrements this iterator by a
       * distance \a delta from its current position. Positive increments cause
       * this iterator to move backward by \a delta pixel samples. Negative
       * increments move this iterator forward by \a delta pixel samples.
       * Returns a reference to this iterator.
       */
      sample_iterator& operator -=( distance_type delta )
      {
         m_iterator -= delta;
         return *this;
      }

      /*!
       * Moves this iterator on the iterated image channel by the specified
       * horizontal and vertical increments in pixels, \a dx and \a dy
       * respectively, relative to its current position. Positive (negative)
       * \a dx increments move the iterator rightwards (leftwards). Positive
       * (negative) \a dy increments move the iterator downwards (upwards).
       */
      sample_iterator& MoveBy( int dx, int dy )
      {
         m_iterator += distance_type( dy )*m_image->Width() + distance_type( dx );
         return *this;
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to the specified iterator \a i incremented by a distance \a delta.
       */
      friend sample_iterator operator +( const sample_iterator& i, distance_type delta )
      {
         return sample_iterator( *i.m_image, i.m_iterator + delta, i.m_end );
      }

      /*!
       * Iterator-to-scalar addition operator. This operator implements the
       * commutative property of scalar-to-iterator addition.
       */
      friend sample_iterator operator +( distance_type delta, const sample_iterator& i )
      {
         return sample_iterator( *i.m_image, i.m_iterator + delta, i.m_end );
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * the specified iterator \a i decremented by a distance \a delta.
       */
      friend sample_iterator operator -( const sample_iterator& i, distance_type delta )
      {
         return sample_iterator( *i.m_image, i.m_iterator - delta, i.m_end );
      }

      /*!
       * Iterator subtraction operator. Returns the distance in pixels between
       * the specified iterators \a i and \a j.
       */
      friend distance_type operator -( const sample_iterator& i, const sample_iterator& j )
      {
         return i.m_iterator - j.m_iterator;
      }

      /*!
       * Returns the distance in pixels between an iterator \a i and a sample
       * pointer \a j.
       */
      friend distance_type operator -( const sample_iterator& i, const sample* j )
      {
         return i.m_iterator - j;
      }

      /*!
       * Returns the distance in pixels between a sample pointer \a i and an
       * iterator \a j.
       */
      friend distance_type operator -( const sample* i, const sample_iterator& j )
      {
         return i - j.m_iterator;
      }

      /*!
       * Equality operator. Returns true if two iterators \a i and \a j point
       * to the same pixel sample.
       */
      friend bool operator ==( const sample_iterator& i, const sample_iterator& j )
      {
         return i.m_iterator == j.m_iterator;
      }

      /*!
       * Returns true iff an iterator \a i and a sample pointer \a j point to
       * the same pixel sample.
       */
      friend bool operator ==( const sample_iterator& i, const sample* j )
      {
         return i.m_iterator == j;
      }

      /*!
       * Returns true iff a sample pointer \a i and an iterator \a j point to
       * the same pixel sample.
       */
      friend bool operator ==( const sample* i, const sample_iterator& j )
      {
         return i == j.m_iterator;
      }

      /*!
       * Less than operator. Returns true if the specified iterator \a i
       * precedes another iterator \a j.
       */
      friend bool operator <( const sample_iterator& i, const sample_iterator& j )
      {
         return i.m_iterator < j.m_iterator;
      }

      /*!
       * Returns true iff an iterator \a i precedes a sample pointer \a j.
       */
      friend bool operator <( const sample_iterator& i, const sample* j )
      {
         return i.m_iterator < j;
      }

      /*!
       * Returns true iff a sample pointer \a i precedes an iterator \a j.
       */
      friend bool operator <( const sample* i, const sample_iterator& j )
      {
         return i < j.m_iterator;
      }

   protected:

            image_type* m_image;
            sample*     m_iterator;
      const sample*     m_end;

      friend class const_sample_iterator;
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::GenericImage::const_sample_iterator
    * \brief Immutable pixel sample iterator.
    *
    * An immutable pixel sample iterator provides read-only, random access to
    * pixel samples in a single channel of an image.
    *
    * \ingroup image_iterators
    */
   class const_sample_iterator
   {
   public:

      /*!
       * Represents the type of the iterated image.
       */
      typedef GenericImage<P>                   image_type;

      /*!
       * Represents the pixel traits class used by the iterated image.
       */
      typedef typename image_type::pixel_traits pixel_traits;

      /*!
       * Represents a pixel sample of the iterated image.
       */
      typedef typename image_type::sample       sample;

      /*!
       * Default constructor. Initializes an invalid iterator.
       */
      const_sample_iterator() :
         m_image( nullptr ), m_iterator( nullptr ), m_end( nullptr )
      {
      }

      /*!
       * Constructs an immutable pixel sample iterator for one channel of an
       * image.
       *
       * \param image   The constant image to iterate.
       *
       * \param channel Channel index. If this parameter is a negative integer,
       *                then this object will be initialized to iterate on the
       *                currently selected channel of the \a image. If an
       *                invalid (i.e., nonexistent) channel index is specified,
       *                then the resulting iterator will also be invalid, with
       *                an empty iteration range. The default value is -1.
       */
      const_sample_iterator( const image_type& image, int channel = -1 ) :
         m_image( &image ), m_iterator( nullptr ), m_end( nullptr )
      {
         if ( m_image->ParseChannel( channel ) )
         {
            m_iterator = (*m_image)[channel];
            m_end = m_iterator + m_image->NumberOfPixels();
         }
      }

      /*!
       * Constructs an immutable pixel sample iterator for a range of
       * contiguous pixel samples of an image.
       *
       * \param image   The constant image to iterate.
       *
       * \param i       Pointer to the first constant pixel sample in the
       *                iteration range. The iterator will be initialized to
       *                access this pixel sample just after construction.
       *
       * \param j       End of the iteration range. The iterator will be
       *                invalid when it reaches (or surpasses) this point.
       *
       * \note Both iteration limits \a i and \a j must be pointers to constant
       * pixel samples in the \e same channel of the specified \a image.
       */
      const_sample_iterator( const image_type& image, const sample* i, const sample* j ) :
         m_image( &image ), m_iterator( i ), m_end( j )
      {
      }

      /*!
       * Constructs an immutable iterator from a mutable iterator.
       *
       * \note Don't confuse this constructor with the copy constructor for the
       * const_sample_iterator class. Note that the argument to this function
       * is a reference to an object of a different class.
       */
      const_sample_iterator( const sample_iterator& i ) :
         m_image( i.m_image ), m_iterator( i.m_iterator ), m_end( i.m_end )
      {
      }

      /*!
       * Copy constructor.
       */
      const_sample_iterator( const const_sample_iterator& ) = default;

      /*!
       * Assigns a mutable iterator to this object. Returns a reference to this
       * iterator.
       */
      const_sample_iterator& operator =( const sample_iterator& i )
      {
         m_image    = i.m_image;
         m_iterator = i.m_iterator;
         m_end      = i.m_end;
         return *this;
      }

      /*!
       * Copy assignment operator. Returns a reference to this object.
       */
      const_sample_iterator& operator =( const const_sample_iterator& ) = default;

      /*!
       * Returns true only if this iterator is valid. A valid iterator defines
       * a valid iterated image and a non-null position.
       */
      bool IsValid() const
      {
         return m_image != nullptr && m_iterator != nullptr;
      }

      /*!
       * Returns a reference to the constant image being iterated by this
       * object.
       */
      const image_type& Image() const
      {
         return *m_image;
      }

      /*!
       * Returns a pointer to the constant pixel sample pointed to by this
       * iterator.
       */
      const sample* Position() const
      {
         return m_iterator;
      }

      /*!
       * Boolean type conversion operator. Returns true if this iterator is
       * active. A pixel sample iterator is active if it has not reached (or
       * surpassed) its iteration limit: either the end of the iterated image
       * channel, or the end of the iteration range, depending on how the
       * iterator has been constructed.
       */
      operator bool() const
      {
         return m_iterator < m_end;
      }

      /*!
       * Indirection operator. Returns a reference to the constant pixel sample
       * pointed to by this iterator.
       */
      const sample& operator *() const
      {
         return *m_iterator;
      }

      /*!
       * Pre-increment operator. Causes this iterator to point to the next
       * pixel sample in the iterated image channel, then returns a reference
       * to this iterator.
       */
      const_sample_iterator& operator ++()
      {
         ++m_iterator;
         return *this;
      }

      /*!
       * Post-increment operator. Causes this iterator to point to the next
       * pixel sample in the iterated image channel. Returns a copy of the
       * iterator as it was before incrementing it.
       */
      const_sample_iterator operator ++( int )
      {
         return const_sample_iterator( *m_image, m_iterator++, m_end );
      }

      /*!
       * Pre-decrement operator. Causes this iterator to point to the previous
       * pixel sample in the iterated image channel, then returns a reference
       * to this iterator.
       */
      const_sample_iterator& operator --()
      {
         --m_iterator;
         return *this;
      }

      /*!
       * Post-decrement operator. Causes this iterator to point to the previous
       * pixel sample in the iterated image channel. Returns a copy of the
       * iterator as it was before decrementing it.
       */
      const_sample_iterator operator --( int )
      {
         return const_sample_iterator( *m_image, m_iterator--, m_end );
      }

      /*!
       * Scalar assignment/addition operator. Increments this iterator by a
       * distance \a delta from its current position. Positive increments cause
       * this iterator to move forward by \a delta pixel samples. Negative
       * increments move this iterator backward by \a delta pixel samples.
       * Returns a reference to this iterator.
       */
      const_sample_iterator& operator +=( distance_type delta )
      {
         m_iterator += delta;
         return *this;
      }

      /*!
       * Scalar assignment/subtraction operator. Decrements this iterator by a
       * distance \a delta from its current position. Positive increments cause
       * this iterator to move backward by \a delta pixel samples. Negative
       * increments move this iterator forward by \a delta pixel samples.
       * Returns a reference to this iterator.
       */
      const_sample_iterator& operator -=( distance_type delta )
      {
         m_iterator -= delta;
         return *this;
      }

      /*!
       * Moves this iterator on the iterated image channel by the specified
       * horizontal and vertical increments in pixels, \a dx and \a dy
       * respectively, relative to its current position. Positive (negative)
       * \a dx increments move the iterator rightwards (leftwards). Positive
       * (negative) \a dy increments move the iterator downwards (upwards).
       */
      const_sample_iterator& MoveBy( int dx, int dy )
      {
         m_iterator += distance_type( dy )*m_image->Width() + distance_type( dx );
         return *this;
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to the specified iterator \a i incremented by a distance \a delta.
       */
      friend const_sample_iterator operator +( const const_sample_iterator& i, distance_type delta )
      {
         return const_sample_iterator( *i.m_image, i.m_iterator + delta, i.m_end );
      }

      /*!
       * Iterator-to-scalar addition operator. This operator implements the
       * commutative property of scalar-to-iterator addition.
       */
      friend const_sample_iterator operator +( distance_type delta, const const_sample_iterator& i )
      {
         return const_sample_iterator( *i.m_image, i.m_iterator + delta, i.m_end );
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * the specified iterator \a i decremented by a distance \a delta.
       */
      friend const_sample_iterator operator -( const const_sample_iterator& i, distance_type delta )
      {
         return const_sample_iterator( *i.m_image, i.m_iterator - delta, i.m_end );
      }

      /*!
       * Iterator subtraction operator. Returns the distance in pixels between
       * the specified iterators \a i and \a j.
       */
      friend distance_type operator -( const const_sample_iterator& i, const const_sample_iterator& j )
      {
         return i.m_iterator - j.m_iterator;
      }

      /*!
       * Returns the distance in pixels between an iterator \a i and a sample
       * pointer \a j.
       */
      friend distance_type operator -( const const_sample_iterator& i, const sample* j )
      {
         return i.m_iterator - j;
      }

      /*!
       * Returns the distance in pixels between a sample pointer \a i and an
       * iterator \a j.
       */
      friend distance_type operator -( const sample* i, const const_sample_iterator& j )
      {
         return i - j.m_iterator;
      }

      /*!
       * Equality operator. Returns true if two iterators \a i and \a j point
       * to the same pixel sample.
       */
      friend bool operator ==( const const_sample_iterator& i, const const_sample_iterator& j )
      {
         return i.m_iterator == j.m_iterator;
      }

      /*!
       * Returns true iff an iterator \a i and a sample pointer \a j point to
       * the same pixel sample.
       */
      friend bool operator ==( const const_sample_iterator& i, const sample* j )
      {
         return i.m_iterator == j;
      }

      /*!
       * Returns true iff a sample pointer \a i and an iterator \a j point to
       * the same pixel sample.
       */
      friend bool operator ==( const sample* i, const const_sample_iterator& j )
      {
         return i == j.m_iterator;
      }

      /*!
       * Less than operator. Returns true if the specified iterator \a i
       * precedes another iterator \a j.
       */
      friend bool operator <( const const_sample_iterator& i, const const_sample_iterator& j )
      {
         return i.m_iterator < j.m_iterator;
      }

      /*!
       * Returns true iff an iterator \a i precedes a sample pointer \a j.
       */
      friend bool operator <( const const_sample_iterator& i, const sample* j )
      {
         return i.m_iterator < j;
      }

      /*!
       * Returns true iff a sample pointer \a i precedes an iterator \a j.
       */
      friend bool operator <( const sample* i, const const_sample_iterator& j )
      {
         return i < j.m_iterator;
      }

   protected:

      const image_type* m_image;
      const sample*     m_iterator;
      const sample*     m_end;
   };

   // -------------------------------------------------------------------------

   template <class image_type, class sample_pointer>
   class roi_sample_iterator_base
   {
   protected:

      image_type*    m_image;
      sample_pointer m_iterator;
      sample_pointer m_rowBegin;
      sample_pointer m_rowEnd;
      sample_pointer m_end;

      roi_sample_iterator_base() :
         m_image( nullptr ), m_iterator( nullptr ), m_rowBegin( nullptr ), m_rowEnd( nullptr ), m_end( nullptr )
      {
      }

      roi_sample_iterator_base( image_type& image, const Rect& rect, int channel ) :
         m_image( &image ), m_iterator( nullptr ), m_rowBegin( nullptr ), m_rowEnd( nullptr ), m_end( nullptr )
      {
         Rect r = rect;
         if ( m_image->ParseRect( r ) )
         {
            if ( m_image->ParseChannel( channel ) )
            {
               m_iterator = m_rowBegin = m_image->PixelAddress( r.x0, r.y0, channel );
               m_rowEnd = m_rowBegin + r.Width();
               m_end = m_rowEnd + ((r.Height() - 1)*m_image->Width());
            }
         }
      }

      roi_sample_iterator_base( image_type& image, sample_pointer i, sample_pointer j ) :
         m_image( &image ), m_iterator( nullptr ), m_rowBegin( nullptr ), m_rowEnd( nullptr ), m_end( nullptr )
      {
         if ( j < i )
            pcl::Swap( i, j );
         m_iterator = m_rowBegin = i;
         m_rowEnd = m_rowBegin + (j - i)%m_image->Width();
         m_end = j;
      }

      roi_sample_iterator_base( const roi_sample_iterator_base& i ) = default;

      roi_sample_iterator_base& operator =( const roi_sample_iterator_base& ) = default;

      void Increment()
      {
         if ( ++m_iterator == m_rowEnd )
         {
            m_rowBegin += m_image->Width();
            m_rowEnd += m_image->Width();
            m_iterator = m_rowBegin;
         }
      }

      void Decrement()
      {
         if ( m_iterator == m_rowBegin )
         {
            m_rowBegin -= m_image->Width();
            m_rowEnd -= m_image->Width();
            m_iterator = m_rowEnd;
         }
         --m_iterator;
      }

      void MoveBy( int cols, int rows )
      {
         cols += m_iterator - m_rowBegin;
         if ( cols != 0 )
         {
            int w = m_rowEnd - m_rowBegin;
            if ( pcl::Abs( cols ) >= w )
            {
               rows += cols/w;
               cols %= w;
            }
         }
         int dy = rows * m_image->Width();
         m_rowBegin += dy;
         m_rowEnd += dy;
         m_iterator = m_rowBegin + cols;
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::GenericImage::roi_sample_iterator
    * \brief Mutable region-of-interest pixel sample iterator.
    *
    * A mutable, region-of-interest (ROI) pixel sample iterator provides
    * read/write, random access to pixel samples within a rectangular subset of
    * a single channel of an image.
    *
    * \ingroup image_iterators
    */
   class roi_sample_iterator : private roi_sample_iterator_base<GenericImage<P>, sample*>
   {
   public:

      /*!
       * Represents the type of the iterated image.
       */
      typedef GenericImage<P>                   image_type;

      /*!
       * Represents the pixel traits class used by the iterated image.
       */
      typedef typename image_type::pixel_traits pixel_traits;

      /*!
       * Represents a pixel sample of the iterated image.
       */
      typedef typename image_type::sample       sample;

      typedef roi_sample_iterator_base<GenericImage<P>, sample*>
                                                iterator_base;

      /*!
       * Default constructor. Initializes an invalid iterator.
       */
      roi_sample_iterator() :
         iterator_base()
      {
      }

      /*!
       * Constructs a mutable, region-of-interest (ROI) pixel sample iterator
       * for one channel of an image.
       *
       * \param image   The image to iterate.
       *
       * \param rect    Region of interest. If an empty rectangle is specified,
       *                the current rectangular selection in the \a image will
       *                be used. If the specified rectangle is not empty and
       *                extends beyond image boundaries, only the intersection
       *                with the image will be used. If that intersection does
       *                not exist, then the resulting iterator will be invalid,
       *                with an empty iteration range. The default value is an
       *                empty rectangle.
       *
       * \param channel Channel index. If this parameter is a negative integer,
       *                then this object will be initialized to iterate on the
       *                currently selected channel of the \a image. If an
       *                invalid (i.e., nonexistent) channel index is specified,
       *                then the resulting iterator will also be invalid, with
       *                an empty iteration range. The default value is -1.
       */
      roi_sample_iterator( image_type& image, const Rect& rect = Rect( 0 ), int channel = -1 ) :
         iterator_base( image.EnsureUnique(), rect, channel )
      {
      }

      /*!
       * Constructs a mutable, region-of-interest (ROI) pixel sample iterator
       * for a range of pixel samples of an image.
       *
       * \param image   The image to iterate.
       *
       * \param i       Pointer to the first pixel sample in the iteration
       *                range. The iterator will be initialized to access this
       *                pixel sample just after construction.
       *
       * \param j       End of the iteration range. The iterator will be
       *                invalid when it reaches (or surpasses) this point.
       *
       * \note Both iteration limits \a i and \a j must be pointers to pixel
       * samples in the \e same channel of the specified \a image.
       */
      roi_sample_iterator( image_type& image, sample* i, sample* j ) :
         iterator_base( image, i, j )
      {
      }

      /*!
       * Copy constructor.
       */
      roi_sample_iterator( const roi_sample_iterator& ) = default;

      /*!
       * Copy assignment operator. Returns a reference to this object.
       */
      roi_sample_iterator& operator =( const roi_sample_iterator& ) = default;

      /*!
       * Returns true only if this iterator is valid. A valid iterator defines
       * a valid iterated image and a non-null position.
       */
      bool IsValid() const
      {
         return this->m_image != nullptr && this->m_iterator != nullptr;
      }

      /*!
       * Returns a reference to the image being iterated by this object.
       */
      image_type& Image() const
      {
         return *this->m_image;
      }

      /*!
       * Returns a pointer to the pixel sample pointed to by this iterator.
       */
      sample* Position() const
      {
         return this->m_iterator;
      }

      /*!
       * Boolean type conversion operator. Returns true if this iterator is
       * active. A ROI pixel sample iterator is active if it has not reached
       * (or surpassed) its iteration limit, i.e. the bottom right corner of
       * its iterated region of interest.
       */
      operator bool() const
      {
         return this->m_iterator < this->m_end;
      }

      /*!
       * Indirection operator. Returns a reference to the pixel sample pointed
       * to by this iterator.
       */
      sample& operator *() const
      {
         return *this->m_iterator;
      }

      /*!
       * Pre-increment operator. Causes this iterator to point to the next
       * pixel sample in the iterated region of interest, then returns a
       * reference to this iterator.
       */
      roi_sample_iterator& operator ++()
      {
         this->Increment();
         return *this;
      }

      /*!
       * Post-increment operator. Causes this iterator to point to the next
       * pixel sample in the iterated region of interest. Returns a copy of the
       * iterator as it was before incrementing it.
       */
      roi_sample_iterator operator ++( int )
      {
         roi_sample_iterator i0( *this );
         this->Increment();
         return i0;
      }

      /*!
       * Pre-decrement operator. Causes this iterator to point to the previous
       * pixel sample in the iterated region of interest, then returns a
       * reference to this iterator.
       */
      roi_sample_iterator& operator --()
      {
         this->Decrement();
         return *this;
      }

      /*!
       * Post-decrement operator. Causes this iterator to point to the previous
       * pixel sample in the iterated region of interest. Returns a copy of the
       * iterator as it was before decrementing it.
       */
      roi_sample_iterator operator --( int )
      {
         roi_sample_iterator i0( *this );
         this->Decrement();
         return i0;
      }

      /*!
       * Scalar assignment/addition operator. Increments this iterator by a
       * distance \a delta from its current position, within the rectangular
       * region of interest being iterated. Positive increments cause this
       * iterator to move forward (rightwards and downwards) by \a delta pixel
       * samples. Negative increments move this iterator backward (leftwards
       * and upwards) by \a delta pixel samples. Returns a reference to this
       * iterator.
       */
      roi_sample_iterator& operator +=( distance_type delta )
      {
         int w = this->m_rowEnd - this->m_rowBegin;
         iterator_base::MoveBy( delta%w, delta/w );
         return *this;
      }

      /*!
       * Scalar assignment/subtraction operator. Decrements this iterator by a
       * distance \a delta from its current position, within the rectangular
       * region of interest being iterated. Positive increments cause this
       * iterator to move backward (leftwards and upwards) by \a delta pixel
       * samples. Negative increments move this iterator forward (rightwards
       * and downwards) by \a delta pixel samples. Returns a reference to this
       * iterator.
       */
      roi_sample_iterator& operator -=( distance_type delta )
      {
         int w = this->m_rowEnd - this->m_rowBegin;
         iterator_base::MoveBy( -delta%w, -delta/w );
         return *this;
      }

      /*!
       * Moves this iterator within its rectangular region of interest by the
       * specified horizontal and vertical increments in pixels, \a dx and
       * \a dy respectively, relative to its current position. Positive
       * (negative) \a dx increments move the iterator rightwards (leftwards).
       * Positive (negative) \a dy increments move the iterator downwards
       * (upwards).
       */
      roi_sample_iterator& MoveBy( int dx, int dy )
      {
         iterator_base::MoveBy( dx, dy );
         return *this;
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to the specified iterator \a i incremented by a distance \a delta.
       */
      friend roi_sample_iterator operator +( const roi_sample_iterator& i, distance_type delta )
      {
         roi_sample_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Iterator-to-scalar addition operator. This operator implements the
       * commutative property of scalar-to-iterator addition.
       */
      friend roi_sample_iterator operator +( distance_type delta, const roi_sample_iterator& i )
      {
         roi_sample_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * the specified iterator \a i decremented by a distance \a delta.
       */
      friend roi_sample_iterator operator -( const roi_sample_iterator& i, distance_type delta )
      {
         roi_sample_iterator j( i );
         j -= delta;
         return j;
      }

      /*!
       * Equality operator. Returns true if two iterators \a i and \a j point
       * to the same pixel sample.
       */
      friend bool operator ==( const roi_sample_iterator& i, const roi_sample_iterator& j )
      {
         return i.m_iterator == j.m_iterator;
      }

      /*!
       * Returns true iff an iterator \a i and a sample pointer \a j point to
       * the same pixel sample.
       */
      friend bool operator ==( const roi_sample_iterator& i, const sample* j )
      {
         return i.m_iterator == j;
      }

      /*!
       * Returns true iff a sample pointer \a i and an iterator \a j point to
       * the same pixel sample.
       */
      friend bool operator ==( const sample* i, const roi_sample_iterator& j )
      {
         return i == j.m_iterator;
      }

      /*!
       * Less than operator. Returns true if the specified iterator \a i
       * precedes another iterator \a j.
       */
      friend bool operator <( const roi_sample_iterator& i, const roi_sample_iterator& j )
      {
         return i.m_iterator < j.m_iterator;
      }

      /*!
       * Returns true iff an iterator \a i precedes a sample pointer \a j.
       */
      friend bool operator <( const roi_sample_iterator& i, const sample* j )
      {
         return i.m_iterator < j;
      }

      /*!
       * Returns true iff a sample pointer \a i precedes an iterator \a j.
       */
      friend bool operator <( const sample* i, const roi_sample_iterator& j )
      {
         return i < j.m_iterator;
      }

      friend class const_roi_pixel_iterator;
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::GenericImage::const_roi_sample_iterator
    * \brief Immutable region-of-interest pixel sample iterator.
    *
    * An immutable, region-of-interest pixel sample iterator provides
    * read-only, random access to pixel samples within a rectangular subset of
    * a single channel of an image.
    *
    * \ingroup image_iterators
    */
   class const_roi_sample_iterator : private roi_sample_iterator_base<const GenericImage<P>, const sample*>
   {
   public:

      /*!
       * Represents the type of the iterated image.
       */
      typedef GenericImage<P>                   image_type;

      /*!
       * Represents the pixel traits class used by the iterated image.
       */
      typedef typename image_type::pixel_traits pixel_traits;

      /*!
       * Represents a pixel sample of the iterated image.
       */
      typedef typename image_type::sample       sample;

      typedef roi_sample_iterator_base<const GenericImage<P>, const sample*>
                                                iterator_base;

      /*!
       * Default constructor. Initializes an invalid iterator.
       */
      const_roi_sample_iterator() :
         iterator_base()
      {
      }

      /*!
       * Constructs an immutable, region-of-interest (ROI) pixel sample
       * iterator for one channel of an image.
       *
       * \param image   The constant image to iterate.
       *
       * \param rect    Region of interest. If an empty rectangle is specified,
       *                the current rectangular selection in the \a image will
       *                be used. If the specified rectangle is not empty and
       *                extends beyond image boundaries, only the intersection
       *                with the image will be used. If that intersection does
       *                not exist, then the resulting iterator will be invalid,
       *                with an empty iteration range. The default value is an
       *                empty rectangle.
       *
       * \param channel Channel index. If this parameter is a negative integer,
       *                then this object will be initialized to iterate on the
       *                currently selected channel of the \a image. If an
       *                invalid (i.e., nonexistent) channel index is specified,
       *                then the resulting iterator will also be invalid, with
       *                an empty iteration range. The default value is -1.
       */
      const_roi_sample_iterator( const image_type& image, const Rect& rect = Rect( 0 ), int channel = -1 ) :
         iterator_base( image, rect, channel )
      {
      }

      /*!
       * Constructs an immutable, region-of-interest (ROI) pixel sample
       * iterator for a range of pixel samples of an image.
       *
       * \param image   The constant image to iterate.
       *
       * \param i       Pointer to the first constant pixel sample in the
       *                iteration range. The iterator will be initialized to
       *                access this pixel sample just after construction.
       *
       * \param j       End of the iteration range. The iterator will be
       *                invalid when it reaches (or surpasses) this point.
       *
       * \note Both iteration limits \a i and \a j must be pointers to constant
       * pixel samples in the \e same channel of the specified \a image.
       */
      const_roi_sample_iterator( const image_type& image, const sample* i, const sample* j ) :
         iterator_base( image, i, j )
      {
      }

      /*!
       * Constructs an immutable ROI pixel sample iterator from a mutable ROI
       * pixel sample iterator.
       *
       * \note Don't confuse this constructor with the copy constructor for the
       * const_roi_sample_iterator class. Note that the argument to this
       * function is a reference to an object of a different class.
       */
      const_roi_sample_iterator( const roi_sample_iterator& i ) :
         iterator_base( i.m_image, i.m_rowBegin, i.m_end )
      {
      }

      /*!
       * Copy constructor.
       */
      const_roi_sample_iterator( const const_roi_sample_iterator& ) = default;

      /*!
       * Assigns a mutable iterator to this object. Returns a reference to this
       * iterator.
       */
      const_roi_sample_iterator& operator =( const roi_sample_iterator& i )
      {
         this->m_image    = i.m_image;
         this->m_iterator = i.m_iterator;
         this->m_rowBegin = i.m_rowBegin;
         this->m_rowEnd   = i.m_rowEnd;
         this->m_end      = i.m_end;
         return *this;
      }

      /*!
       * Copy assignment operator. Returns a reference to this object.
       */
      const_roi_sample_iterator& operator =( const const_roi_sample_iterator& ) = default;

      /*!
       * Returns true only if this iterator is valid. A valid iterator defines
       * a valid iterated image and a non-null position.
       */
      bool IsValid() const
      {
         return this->m_image != nullptr && this->m_iterator != nullptr;
      }

      /*!
       * Returns a reference to the constant image being iterated by this
       * object.
       */
      const image_type& Image() const
      {
         return *this->m_image;
      }

      /*!
       * Returns a pointer to the constant pixel sample pointed to by this
       * iterator.
       */
      const sample* Position() const
      {
         return this->m_iterator;
      }

      /*!
       * Boolean type conversion operator. Returns true if this iterator is
       * active. A ROI pixel sample iterator is active if it has not reached
       * (or surpassed) its iteration limit, i.e. the bottom right corner of
       * its iterated region of interest.
       */
      operator bool() const
      {
         return this->m_iterator < this->m_end;
      }

      /*!
       * Indirection operator. Returns a reference to the constant pixel sample
       * pointed to by this iterator.
       */
      const sample& operator *() const
      {
         return *this->m_iterator;
      }

      /*!
       * Pre-increment operator. Causes this iterator to point to the next
       * pixel sample in the iterated region of interest, then returns a
       * reference to this iterator.
       */
      const_roi_sample_iterator& operator ++()
      {
         this->Increment();
         return *this;
      }

      /*!
       * Post-increment operator. Causes this iterator to point to the next
       * pixel sample in the iterated region of interest. Returns a copy of the
       * iterator as it was before incrementing it.
       */
      const_roi_sample_iterator operator ++( int )
      {
         const_roi_sample_iterator i0( *this );
         this->Increment();
         return i0;
      }

      /*!
       * Pre-decrement operator. Causes this iterator to point to the previous
       * pixel sample in the iterated region of interest, then returns a
       * reference to this iterator.
       */
      const_roi_sample_iterator& operator --()
      {
         this->Decrement();
         return *this;
      }

      /*!
       * Post-decrement operator. Causes this iterator to point to the previous
       * pixel sample in the iterated region of interest. Returns a copy of the
       * iterator as it was before decrementing it.
       */
      const_roi_sample_iterator operator --( int )
      {
         const_roi_sample_iterator i0( *this );
         this->Decrement();
         return i0;
      }

      /*!
       * Scalar assignment/addition operator. Increments this iterator by a
       * distance \a delta from its current position, within the rectangular
       * region of interest being iterated. Positive increments cause this
       * iterator to move forward (rightwards and downwards) by \a delta pixel
       * samples. Negative increments move this iterator backward (leftwards
       * and upwards) by \a delta pixel samples. Returns a reference to this
       * iterator.
       */
      const_roi_sample_iterator& operator +=( distance_type delta )
      {
         int w = this->m_rowEnd - this->m_rowBegin;
         iterator_base::MoveBy( delta%w, delta/w );
         return *this;
      }

      /*!
       * Scalar assignment/subtraction operator. Decrements this iterator by a
       * distance \a delta from its current position, within the rectangular
       * region of interest being iterated. Positive increments cause this
       * iterator to move backward (leftwards and upwards) by \a delta pixel
       * samples. Negative increments move this iterator forward (rightwards
       * and downwards) by \a delta pixel samples. Returns a reference to this
       * iterator.
       */
      const_roi_sample_iterator& operator -=( distance_type delta )
      {
         int w = this->m_rowEnd - this->m_rowBegin;
         iterator_base::MoveBy( -delta%w, -delta/w );
         return *this;
      }

      /*!
       * Moves this iterator within its rectangular region of interest by the
       * specified horizontal and vertical increments in pixels, \a dx and
       * \a dy respectively, relative to its current position. Positive
       * (negative) \a dx increments move the iterator rightwards (leftwards).
       * Positive (negative) \a dy increments move the iterator downwards
       * (upwards).
       */
      const_roi_sample_iterator& MoveBy( int dx, int dy )
      {
         iterator_base::MoveBy( dx, dy );
         return *this;
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to the specified iterator \a i incremented by a distance \a delta.
       */
      friend const_roi_sample_iterator operator +( const const_roi_sample_iterator& i, distance_type delta )
      {
         const_roi_sample_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Iterator-to-scalar addition operator. This operator implements the
       * commutative property of scalar-to-iterator addition.
       */
      friend const_roi_sample_iterator operator +( distance_type delta, const const_roi_sample_iterator& i )
      {
         const_roi_sample_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * the specified iterator \a i decremented by a distance \a delta.
       */
      friend const_roi_sample_iterator operator -( const const_roi_sample_iterator& i, distance_type delta )
      {
         const_roi_sample_iterator j( i );
         j -= delta;
         return j;
      }

      /*!
       * Equality operator. Returns true if two iterators \a i and \a j point
       * to the same pixel sample.
       */
      friend bool operator ==( const const_roi_sample_iterator& i, const const_roi_sample_iterator& j )
      {
         return i.m_iterator == j.m_iterator;
      }

      /*!
       * Returns true iff an iterator \a i and a sample pointer \a j point to
       * the same pixel sample.
       */
      friend bool operator ==( const const_roi_sample_iterator& i, const sample* j )
      {
         return i.m_iterator == j;
      }

      /*!
       * Returns true iff a sample pointer \a i and an iterator \a j point to
       * the same pixel sample.
       */
      friend bool operator ==( const sample* i, const const_roi_sample_iterator& j )
      {
         return i == j.m_iterator;
      }

      /*!
       * Less than operator. Returns true if the specified iterator \a i
       * precedes another iterator \a j.
       */
      friend bool operator <( const const_roi_sample_iterator& i, const const_roi_sample_iterator& j )
      {
         return i.m_iterator < j.m_iterator;
      }

      /*!
       * Returns true iff an iterator \a i precedes a sample pointer \a j.
       */
      friend bool operator <( const const_roi_sample_iterator& i, const sample* j )
      {
         return i.m_iterator < j;
      }

      /*!
       * Returns true iff a sample pointer \a i precedes an iterator \a j.
       */
      friend bool operator <( const sample* i, const const_roi_sample_iterator& j )
      {
         return i < j.m_iterator;
      }
   };

   // -------------------------------------------------------------------------

   template <class image_type, class iterator_base, class sample_pointer, class filter_type>
   class filter_sample_iterator_base : public iterator_base
   {
   protected:

      filter_type    m_filter;
      sample_pointer m_begin;

      filter_sample_iterator_base() :
         iterator_base(), m_filter(), m_begin( nullptr )
      {
      }

      filter_sample_iterator_base( image_type& image, const filter_type& filter, int channel ) :
         iterator_base( image, channel ), m_filter( filter ), m_begin( iterator_base::m_iterator )
      {
         JumpToNextValidSample();
      }

      filter_sample_iterator_base( image_type& image, const filter_type& filter, sample_pointer i, sample_pointer j ) :
         iterator_base( image, i, j ), m_filter( filter ), m_begin( iterator_base::m_iterator )
      {
         JumpToNextValidSample();
      }

      filter_sample_iterator_base( const iterator_base& i, const filter_type& filter ) :
         iterator_base( i ), m_filter( filter ), m_begin( iterator_base::m_iterator )
      {
         JumpToNextValidSample();
      }

      filter_sample_iterator_base( const filter_sample_iterator_base& ) = default;

      filter_sample_iterator_base& operator =( const filter_sample_iterator_base& ) = default;

      filter_sample_iterator_base& operator =( const iterator_base& i )
      {
         (void)iterator_base::operator =( i );
         JumpToNextValidSample();
         return *this;
      }

      void JumpToNextValidSample()
      {
         while ( this->m_iterator < this->m_end && !this->m_filter( *this->m_iterator ) )
            ++this->m_iterator;
      }

      void JumpToPrevValidSample()
      {
         while ( this->m_iterator > this->m_begin && !this->m_filter( *this->m_iterator ) )
            --this->m_iterator;
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::GenericImage::filter_sample_iterator
    * \brief Mutable filter pixel sample iterator.
    *
    * A mutable pixel sample iterator provides read/write, random access to
    * pixel samples in a single channel of an image. A filter iterator uses a
    * predicate object to filter pixel samples within its iteration range.
    *
    * <b>Filter Predicates</b>
    *
    * For a mutable filter sample iterator, the class template argument F must
    * provide a function call operator of the form:
    *
    * <tt>bool F::operator()( sample& v ) const</tt>
    *
    * which returns true if the passed pixel sample value \a v is valid in the
    * context of the filter iterator. The filter iterator stores a private
    * instance of F to validate pixel samples, so the F class must provide also
    * copy constructor semantics, either explicitly or implicitly.
    *
    * <b>Filter Iterators</b>
    *
    * Each time a filter iterator is constructed, incremented, decremented, or
    * moved forward or backward, the filter predicate is called to validate the
    * pixel or pixel sample pointed to by the iterator. If the pointed item is
    * not valid, the iterator is incremented or decremented (depending on the
    * operation performed initially) until it finds a valid one, or until it
    * reaches the end of the iteration range, whichever happens first. In this
    * way a filter iterator gives access only to valid items in a completely
    * automatic and transparent fashion.
    *
    * \ingroup image_iterators
    */
   template <class F>
   class filter_sample_iterator :
      public filter_sample_iterator_base<GenericImage<P>, sample_iterator, sample*, F>
   {
   public:

      /*!
       * Represents the type of the iterated image.
       */
      typedef GenericImage<P>                   image_type;

      /*!
       * Represents the pixel traits class used by the iterated image.
       */
      typedef typename image_type::pixel_traits pixel_traits;

      /*!
       * Represents a pixel sample of the iterated image.
       */
      typedef typename image_type::sample       sample;

      /*!
       * Represents the type of the unary predicate used by this filter
       * iterator.
       */
      typedef F                                 filter_type;

      typedef filter_sample_iterator_base<GenericImage<P>, sample_iterator, sample*, F>
                                                iterator_base;

      /*!
       * Default constructor. Initializes an invalid iterator.
       */
      filter_sample_iterator() :
         iterator_base()
      {
      }

      /*!
       * Constructs a mutable filter pixel sample iterator for one channel of
       * an image.
       *
       * \param image   The image to iterate.
       *
       * \param filter  Reference to a predicate object that will be used to
       *                filter pixel sample values.
       *
       * \param channel Channel index. If this parameter is a negative integer,
       *                then this object will be initialized to iterate on the
       *                currently selected channel of the \a image. If an
       *                invalid (i.e., nonexistent) channel index is specified,
       *                then the resulting iterator will also be invalid, with
       *                an empty iteration range. The default value is -1.
       */
      filter_sample_iterator( image_type& image, const F& filter, int channel = -1 ) :
         iterator_base( image.EnsureUnique(), filter, channel )
      {
      }

      /*!
       * Constructs a mutable filter pixel sample iterator for a range of
       * contiguous pixel samples of an image.
       *
       * \param image   The image to iterate.
       *
       * \param filter  Reference to a predicate object that will be used to
       *                filter pixel sample values.
       *
       * \param i       Pointer to the first pixel sample in the iteration
       *                range.
       *
       * \param j       End of the iteration range. The iterator will be
       *                invalid when it reaches (or surpasses) this point.
       *
       * \note Both iteration limits \a i and \a j must be pointers to pixel
       * samples in the \e same channel of the specified \a image.
       */
      filter_sample_iterator( image_type& image, const F& filter, sample* i, sample* j ) :
         iterator_base( image, filter, i, j )
      {
      }

      /*!
       * Constructs a mutable filter pixel sample iterator from a sample
       * iterator and the specified \a filter.
       */
      filter_sample_iterator( const sample_iterator& i, const F& filter ) :
         iterator_base( i, filter )
      {
      }

      /*!
       * Copy constructor.
       */
      filter_sample_iterator( const filter_sample_iterator& i ) = default;

      /*!
       * Copy assignment operator. Returns a reference to this iterator.
       */
      filter_sample_iterator& operator =( const filter_sample_iterator& ) = default;

      /*!
       * Assigns a pixel sample iterator to this object. Returns a reference to
       * this iterator.
       */
      filter_sample_iterator& operator =( const sample_iterator& i )
      {
         (void)iterator_base::operator =( i );
         return *this;
      }

      /*!
       * Returns true only if this iterator is valid. A valid iterator defines
       * a valid iterated image and a non-null position.
       */
      bool IsValid() const
      {
         return this->m_image != nullptr && this->m_iterator != nullptr;
      }

      /*!
       * Returns a reference to the image being iterated by this object.
       */
      image_type& Image() const
      {
         return *this->m_image;
      }

      /*!
       * Returns a reference to the immutable predicate object used by this
       * filter iterator.
       */
      const filter_type& Filter() const
      {
         return this->m_filter;
      }

      /*!
       * Returns a reference to the mutable predicate object used by this
       * filter iterator.
       */
      filter_type& Filter()
      {
         return this->m_filter;
      }

      /*!
       * Returns a pointer to the pixel sample pointed to by this iterator.
       */
      sample* Position() const
      {
         return this->m_iterator;
      }

      /*!
       * Boolean type conversion operator. Returns true if this iterator is
       * active. A pixel sample iterator is active if it has not reached (or
       * surpassed) its iteration limit: either the end of the iterated image
       * channel, or the end of the iteration range, depending on how the
       * iterator has been constructed.
       */
      operator bool() const
      {
         return this->m_iterator < this->m_end;
      }

      /*!
       * Indirection operator. Returns a reference to the pixel sample pointed
       * to by this iterator.
       */
      sample& operator *() const
      {
         return *this->m_iterator;
      }

      /*!
       * Pre-increment operator. Causes this iterator to point to the next
       * valid pixel sample in the iterated image channel. Returns a reference
       * to this iterator.
       */
      filter_sample_iterator& operator ++()
      {
         ++this->m_iterator;
         this->JumpToNextValidSample();
         return *this;
      }

      /*!
       * Post-increment operator. Causes this iterator to point to the next
       * valid pixel sample in the iterated image channel. Returns a copy of
       * the iterator as it was before incrementing it.
       */
      filter_sample_iterator operator ++( int )
      {
         sample* i0 = this->m_iterator++;
         this->JumpToNextValidSample();
         return filter_sample_iterator( *this->m_image, this->m_filter, i0, this->m_end );
      }

      /*!
       * Pre-decrement operator. Causes this iterator to point to the previous
       * valid pixel sample in the iterated image channel, then returns a
       * reference to this iterator.
       */
      filter_sample_iterator& operator --()
      {
         --this->m_iterator;
         this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Post-decrement operator. Causes this iterator to point to the previous
       * valid pixel sample in the iterated image channel. Returns a copy of
       * the iterator as it was before decrementing it.
       */
      filter_sample_iterator operator --( int )
      {
         sample* i0 = this->m_iterator--;
         this->JumpToPrevValidSample();
         return filter_sample_iterator( *this->m_image, this->m_filter, i0, this->m_end );
      }

      /*!
       * Scalar assignment/addition operator. Increments this iterator by a
       * distance \a delta from its current position. Positive increments cause
       * this iterator to move forward by \a delta pixel samples. Negative
       * increments move this iterator backward by \a delta pixel samples.
       * Returns a reference to this iterator.
       */
      filter_sample_iterator& operator +=( distance_type delta )
      {
         this->m_iterator += delta;
         this->JumpToNextValidSample();
         return *this;
      }

      /*!
       * Scalar assignment/subtraction operator. Decrements this iterator by a
       * distance \a delta from its current position. Positive increments cause
       * this iterator to move backward by \a delta pixel samples. Negative
       * increments move this iterator forward by \a delta pixel samples.
       * Returns a reference to this iterator.
       */
      filter_sample_iterator& operator -=( distance_type delta )
      {
         this->m_iterator -= delta;
         this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Moves this iterator on the iterated image channel by the specified
       * horizontal and vertical increments in pixels, \a dx and \a dy
       * respectively, relative to its current position. Positive (negative)
       * \a dx increments move the iterator rightwards (leftwards). Positive
       * (negative) \a dy increments move the iterator downwards (upwards).
       */
      filter_sample_iterator& MoveBy( int dx, int dy )
      {
         distance_type d = distance_type( dy )*this->m_image->Width() + distance_type( dx );
         this->m_iterator += d;
         if ( d >= 0 )
            this->JumpToNextValidSample();
         else
            this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to the specified iterator \a i incremented by a distance \a delta.
       */
      friend filter_sample_iterator operator +( const filter_sample_iterator& i, distance_type delta )
      {
         return filter_sample_iterator( *i.m_image, i.m_iterator + delta, i.m_end );
      }

      /*!
       * Iterator-to-scalar addition operator. This operator implements the
       * commutative property of scalar-to-iterator addition.
       */
      friend filter_sample_iterator operator +( distance_type delta, const filter_sample_iterator& i )
      {
         return filter_sample_iterator( *i.m_image, i.m_iterator + delta, i.m_end );
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * the specified iterator \a i decremented by a distance \a delta.
       */
      friend filter_sample_iterator operator -( const filter_sample_iterator& i, distance_type delta )
      {
         return filter_sample_iterator( *i.m_image, i.m_iterator - delta, i.m_end );
      }

      /*!
       * Iterator subtraction operator. Returns the distance in pixels between
       * the specified iterators \a i and \a j.
       */
      friend distance_type operator -( const filter_sample_iterator& i, const filter_sample_iterator& j )
      {
         return i.m_iterator - j.m_iterator;
      }

      /*!
       * Equality operator. Returns true if two iterators \a i and \a j point
       * to the same pixel sample.
       */
      friend bool operator ==( const filter_sample_iterator& i, const filter_sample_iterator& j )
      {
         return i.m_iterator == j.m_iterator;
      }

      /*!
       * Returns true iff an iterator \a i and a sample pointer \a j point to
       * the same pixel sample.
       */
      friend bool operator ==( const filter_sample_iterator& i, const sample* j )
      {
         return i.m_iterator == j;
      }

      /*!
       * Returns true iff a sample pointer \a i and an iterator \a j point to
       * the same pixel sample.
       */
      friend bool operator ==( const sample* i, const filter_sample_iterator& j )
      {
         return i == j.m_iterator;
      }

      /*!
       * Less than operator. Returns true if the specified iterator \a i
       * precedes another iterator \a j.
       */
      friend bool operator <( const filter_sample_iterator& i, const filter_sample_iterator& j )
      {
         return i.m_iterator < j.m_iterator;
      }

      /*!
       * Returns true iff an iterator \a i precedes a sample pointer \a j.
       */
      friend bool operator <( const filter_sample_iterator& i, const sample* j )
      {
         return i.m_iterator < j;
      }

      /*!
       * Returns true iff a sample pointer \a i precedes an iterator \a j.
       */
      friend bool operator <( const sample* i, const filter_sample_iterator& j )
      {
         return i < j.m_iterator;
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::GenericImage::const_filter_sample_iterator
    * \brief Immutable filter pixel sample iterator.
    *
    * An immutable pixel sample iterator provides read-only, random access to
    * pixel samples in a single channel of an image. A filter iterator uses a
    * predicate object to filter pixel samples within its iteration range.
    *
    * <b>Filter Predicates</b>
    *
    * For an immutable filter sample iterator, the class template argument F
    * must provide a function call operator of the form:
    *
    * <tt>bool F::operator()( const sample& v ) const</tt>
    *
    * which returns true if the passed pixel sample value \a v is valid in the
    * context of the filter iterator. The filter iterator stores a private
    * instance of F to validate pixel samples, so the F class must provide also
    * copy constructor semantics, either explicitly or implicitly.
    *
    * <b>Filter Iterators</b>
    *
    * Each time a filter iterator is constructed, incremented, decremented, or
    * moved forward or backward, the filter predicate is called to validate the
    * pixel or pixel sample pointed to by the iterator. If the pointed item is
    * not valid, the iterator is incremented or decremented (depending on the
    * operation performed initially) until it finds a valid one, or until it
    * reaches the end of the iteration range, whichever happens first. In this
    * way a filter iterator gives access only to valid items in a completely
    * automatic and transparent fashion.
    *
    * \ingroup image_iterators
    */
   template <class F>
   class const_filter_sample_iterator :
      public filter_sample_iterator_base<const GenericImage<P>, const_sample_iterator, const sample*, F>
   {
   public:

      /*!
       * Represents the type of the iterated image.
       */
      typedef GenericImage<P>                   image_type;

      /*!
       * Represents the pixel traits class used by the iterated image.
       */
      typedef typename image_type::pixel_traits pixel_traits;

      /*!
       * Represents a pixel sample of the iterated image.
       */
      typedef typename image_type::sample       sample;

      /*!
       * Represents the type of the unary predicate used by this filter
       * iterator.
       */
      typedef F                                 filter_type;

      typedef filter_sample_iterator_base<const GenericImage<P>, const_sample_iterator, const sample*, F>
                                                iterator_base;

      /*!
       * Default constructor. Initializes an invalid iterator.
       */
      const_filter_sample_iterator() :
         iterator_base()
      {
      }

      /*!
       * Constructs an immutable filter pixel sample iterator for one channel
       * of an image.
       *
       * \param image   The constant image to iterate.
       *
       * \param filter  Reference to a predicate object that will be used to
       *                filter pixel sample values.
       *
       * \param channel Channel index. If this parameter is a negative integer,
       *                then this object will be initialized to iterate on the
       *                currently selected channel of the \a image. If an
       *                invalid (i.e., nonexistent) channel index is specified,
       *                then the resulting iterator will also be invalid, with
       *                an empty iteration range. The default value is -1.
       */
      const_filter_sample_iterator( const image_type& image, const F& filter, int channel = -1 ) :
         iterator_base( image, filter, channel )
      {
      }

      /*!
       * Constructs an immutable filter pixel sample iterator for a range of
       * contiguous pixel samples of an image.
       *
       * \param image   The constant image to iterate.
       *
       * \param filter  Reference to a predicate object that will be used to
       *                filter pixel sample values.
       *
       * \param i       Pointer to the first constant pixel sample in the
       *                iteration range.
       *
       * \param j       End of the iteration range. The iterator will be
       *                invalid when it reaches (or surpasses) this point.
       *
       * \note Both iteration limits \a i and \a j must be pointers to constant
       * pixel samples in the \e same channel of the specified \a image.
       */
      const_filter_sample_iterator( const image_type& image, const F& filter, const sample* i, const sample* j ) :
         iterator_base( image, filter, i, j )
      {
      }

      /*!
       * Constructs an immutable filter pixel sample iterator from a mutable
       * sample iterator and the specified \a filter.
       */
      const_filter_sample_iterator( const sample_iterator& i, const F& filter ) :
         iterator_base( i.m_image, filter, i.m_iterator, i.m_end )
      {
      }

      /*!
       * Constructs an immutable filter pixel sample iterator from an immutable
       * sample iterator and the specified \a filter.
       */
      const_filter_sample_iterator( const const_sample_iterator& i, const F& filter ) :
         iterator_base( i, filter )
      {
      }

      /*!
       * Constructs an immutable filter pixel sample iterator from a mutable
       * filter pixel sample iterator.
       */
      const_filter_sample_iterator( const filter_sample_iterator<F>& i ) :
         iterator_base( i )
      {
      }

      /*!
       * Copy constructor.
       */
      const_filter_sample_iterator( const const_filter_sample_iterator& ) = default;

      /*!
       * Copy assignment operator. Returns a reference to this iterator.
       */
      const_filter_sample_iterator& operator =( const const_filter_sample_iterator& ) = default;

      /*!
       * Assigns a mutable pixel sample iterator to this object. Returns a
       * reference to this iterator.
       */
      const_filter_sample_iterator& operator =( const sample_iterator& i )
      {
         (void)const_sample_iterator::operator =( i );
         this->JumpToNextValidSample();
         return *this;
      }

      /*!
       * Assigns an immutable pixel sample iterator to this object. Returns a
       * reference to this iterator.
       */
      const_filter_sample_iterator& operator =( const const_sample_iterator& i )
      {
         (void)iterator_base::operator =( i );
         return *this;
      }

      /*!
       * Returns true only if this iterator is valid. A valid iterator defines
       * a valid iterated image and a non-null position.
       */
      bool IsValid() const
      {
         return this->m_image != nullptr && this->m_iterator != nullptr;
      }

      /*!
       * Returns a reference to the constant image being iterated by this
       * object.
       */
      const image_type& Image() const
      {
         return *this->m_image;
      }

      /*!
       * Returns a reference to the immutable predicate object used by this
       * filter iterator.
       */
      const filter_type& Filter() const
      {
         return this->m_filter;
      }

      /*!
       * Returns a reference to the mutable predicate object used by this
       * filter iterator.
       */
      filter_type& Filter()
      {
         return this->m_filter;
      }

      /*!
       * Returns a pointer to the constant pixel sample pointed to by this
       * iterator.
       */
      const sample* Position() const
      {
         return this->m_iterator;
      }

      /*!
       * Boolean type conversion operator. Returns true if this iterator is
       * active. A pixel sample iterator is active if it has not reached (or
       * surpassed) its iteration limit: either the end of the iterated image
       * channel, or the end of the iteration range, depending on how the
       * iterator has been constructed.
       */
      operator bool() const
      {
         return this->m_iterator < this->m_end;
      }

      /*!
       * Indirection operator. Returns a reference to the constant pixel sample
       * pointed to by this iterator.
       */
      const sample& operator *() const
      {
         return *this->m_iterator;
      }

      /*!
       * Pre-increment operator. Causes this iterator to point to the next
       * valid pixel sample in the iterated image channel. Returns a reference
       * to this iterator.
       */
      const_filter_sample_iterator& operator ++()
      {
         ++this->m_iterator;
         this->JumpToNextValidSample();
         return *this;
      }

      /*!
       * Post-increment operator. Causes this iterator to point to the next
       * valid pixel sample in the iterated image channel. Returns a copy of
       * the iterator as it was before incrementing it.
       */
      const_filter_sample_iterator operator ++( int )
      {
         sample* i0 = this->m_iterator++;
         this->JumpToNextValidSample();
         return const_filter_sample_iterator( *this->m_image, this->m_filter, i0, this->m_end );
      }

      /*!
       * Pre-decrement operator. Causes this iterator to point to the previous
       * valid pixel sample in the iterated image channel, then returns a
       * reference to this iterator.
       */
      const_filter_sample_iterator& operator --()
      {
         --this->m_iterator;
         this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Post-decrement operator. Causes this iterator to point to the previous
       * valid pixel sample in the iterated image channel. Returns a copy of
       * the iterator as it was before decrementing it.
       */
      const_filter_sample_iterator operator --( int )
      {
         sample* i0 = this->m_iterator--;
         this->JumpToPrevValidSample();
         return const_filter_sample_iterator( *this->m_image, this->m_filter, i0, this->m_end );
      }

      /*!
       * Scalar assignment/addition operator. Increments this iterator by a
       * distance \a delta from its current position. Positive increments cause
       * this iterator to move forward by \a delta pixel samples. Negative
       * increments move this iterator backward by \a delta pixel samples.
       * Returns a reference to this iterator.
       */
      const_filter_sample_iterator& operator +=( distance_type delta )
      {
         this->m_iterator += delta;
         this->JumpToNextValidSample();
         return *this;
      }

      /*!
       * Scalar assignment/subtraction operator. Decrements this iterator by a
       * distance \a delta from its current position. Positive increments cause
       * this iterator to move backward by \a delta pixel samples. Negative
       * increments move this iterator forward by \a delta pixel samples.
       * Returns a reference to this iterator.
       */
      const_filter_sample_iterator& operator -=( distance_type delta )
      {
         this->m_iterator -= delta;
         this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Moves this iterator on the iterated image channel by the specified
       * horizontal and vertical increments in pixels, \a dx and \a dy
       * respectively, relative to its current position. Positive (negative)
       * \a dx increments move the iterator rightwards (leftwards). Positive
       * (negative) \a dy increments move the iterator downwards (upwards).
       */
      const_filter_sample_iterator& MoveBy( int dx, int dy )
      {
         distance_type d = distance_type( dy )*this->m_image->Width() + distance_type( dx );
         this->m_iterator += d;
         if ( d >= 0 )
            this->JumpToNextValidSample();
         else
            this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to the specified iterator \a i incremented by a distance \a delta.
       */
      friend const_filter_sample_iterator operator +( const const_filter_sample_iterator& i, distance_type delta )
      {
         return const_filter_sample_iterator( *i.m_image, i.m_iterator + delta, i.m_end );
      }

      /*!
       * Iterator-to-scalar addition operator. This operator implements the
       * commutative property of scalar-to-iterator addition.
       */
      friend const_filter_sample_iterator operator +( distance_type delta, const const_filter_sample_iterator& i )
      {
         return const_filter_sample_iterator( *i.m_image, i.m_iterator + delta, i.m_end );
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * the specified iterator \a i decremented by a distance \a delta.
       */
      friend const_filter_sample_iterator operator -( const const_filter_sample_iterator& i, distance_type delta )
      {
         return const_filter_sample_iterator( *i.m_image, i.m_iterator - delta, i.m_end );
      }

      /*!
       * Iterator subtraction operator. Returns the distance in pixels between
       * the specified iterators \a i and \a j.
       */
      friend distance_type operator -( const const_filter_sample_iterator& i, const const_filter_sample_iterator& j )
      {
         return i.m_iterator - j.m_iterator;
      }

      /*!
       * Equality operator. Returns true if two iterators \a i and \a j point
       * to the same pixel sample.
       */
      friend bool operator ==( const const_filter_sample_iterator& i, const const_filter_sample_iterator& j )
      {
         return i.m_iterator == j.m_iterator;
      }

      /*!
       * Returns true iff an iterator \a i and a sample pointer \a j point to
       * the same pixel sample.
       */
      friend bool operator ==( const const_filter_sample_iterator& i, const sample* j )
      {
         return i.m_iterator == j;
      }

      /*!
       * Returns true iff a sample pointer \a i and an iterator \a j point to
       * the same pixel sample.
       */
      friend bool operator ==( const sample* i, const const_filter_sample_iterator& j )
      {
         return i == j.m_iterator;
      }

      /*!
       * Less than operator. Returns true if the specified iterator \a i
       * precedes another iterator \a j.
       */
      friend bool operator <( const const_filter_sample_iterator& i, const const_filter_sample_iterator& j )
      {
         return i.m_iterator < j.m_iterator;
      }

      /*!
       * Returns true iff an iterator \a i precedes a sample pointer \a j.
       */
      friend bool operator <( const const_filter_sample_iterator& i, const sample* j )
      {
         return i.m_iterator < j;
      }

      /*!
       * Returns true iff a sample pointer \a i precedes an iterator \a j.
       */
      friend bool operator <( const sample* i, const const_filter_sample_iterator& j )
      {
         return i < j.m_iterator;
      }
   };

   // -------------------------------------------------------------------------

   template <class image_type, class sample_pointer, class filter_type>
   class roi_filter_sample_iterator_base : public roi_sample_iterator_base<image_type, sample_pointer>
   {
   protected:

      typedef roi_sample_iterator_base<image_type, sample_pointer>
                                                roi_iterator_base;

      filter_type    m_filter;
      sample_pointer m_begin;

      roi_filter_sample_iterator_base() :
         roi_iterator_base(), m_filter(), m_begin( nullptr )
      {
      }

      roi_filter_sample_iterator_base( image_type& image, const filter_type& filter, const Rect& rect, int channel ) :
         roi_iterator_base( image, rect, channel ), m_filter( filter ), m_begin( roi_iterator_base::m_iterator )
      {
         JumpToNextValidSample();
      }

      roi_filter_sample_iterator_base( image_type& image, const filter_type& filter, sample_pointer i, sample_pointer j ) :
         roi_iterator_base( image, i, j ), m_filter( filter ), m_begin( roi_iterator_base::m_iterator )
      {
         JumpToNextValidSample();
      }

      roi_filter_sample_iterator_base( const roi_iterator_base& i, const filter_type& filter ) :
         roi_iterator_base( i ), m_filter( filter ), m_begin( roi_iterator_base::m_iterator )
      {
         JumpToNextValidSample();
      }

      roi_filter_sample_iterator_base( const roi_filter_sample_iterator_base& ) = default;

      roi_filter_sample_iterator_base& operator =( const roi_filter_sample_iterator_base& i ) = default;

      roi_filter_sample_iterator_base& operator =( const roi_iterator_base& i )
      {
         (void)roi_iterator_base::operator =( i );
         JumpToNextValidSample();
         return *this;
      }

      void JumpToNextValidSample()
      {
         while ( this->m_iterator < this->m_end && !this->m_filter( *this->m_iterator ) )
            roi_iterator_base::Increment();
      }

      void JumpToPrevValidSample()
      {
         while ( this->m_iterator > this->m_begin && !this->m_filter( *this->m_iterator ) )
            roi_iterator_base::Decrement();
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::GenericImage::roi_filter_sample_iterator
    * \brief Mutable region-of-interest, filter pixel sample iterator.
    *
    * A mutable, region-of-interest (ROI), filter pixel sample iterator
    * combines the capabilities of roi_sample_iterator and
    * filter_sample_iterator in a single iterator class.
    *
    * \ingroup image_iterators
    */
   template <class F>
   class roi_filter_sample_iterator : public roi_filter_sample_iterator_base<GenericImage<P>, sample*, F>
   {
   public:

      /*!
       * Represents the type of the iterated image.
       */
      typedef GenericImage<P>                   image_type;

      /*!
       * Represents the pixel traits class used by the iterated image.
       */
      typedef typename image_type::pixel_traits pixel_traits;

      /*!
       * Represents a pixel sample of the iterated image.
       */
      typedef typename image_type::sample       sample;

      /*!
       * Represents the type of the unary predicate used by this filter
       * iterator.
       */
      typedef F                                 filter_type;

      typedef roi_filter_sample_iterator_base<GenericImage<P>, sample*, F>
                                                iterator_base;

      /*!
       * Default constructor. Initializes an invalid iterator.
       */
      roi_filter_sample_iterator() :
         iterator_base()
      {
      }

      /*!
       * Constructs a mutable, region-of-interest (ROI), filter pixel sample
       * iterator for one channel of an image.
       *
       * \param image   The image to iterate.
       *
       * \param filter  Reference to a predicate object that will be used to
       *                filter pixel sample values.
       *
       * \param rect    Region of interest. If an empty rectangle is specified,
       *                the current rectangular selection in the \a image will
       *                be used. If the specified rectangle is not empty and
       *                extends beyond image boundaries, only the intersection
       *                with the image will be used. If that intersection does
       *                not exist, then the resulting iterator will be invalid,
       *                with an empty iteration range. The default value is an
       *                empty rectangle.
       *
       * \param channel Channel index. If this parameter is a negative integer,
       *                then this object will be initialized to iterate on the
       *                currently selected channel of the \a image. If an
       *                invalid (i.e., nonexistent) channel index is specified,
       *                then the resulting iterator will also be invalid, with
       *                an empty iteration range. The default value is -1.
       */
      roi_filter_sample_iterator( image_type& image, const F& filter, const Rect& rect = Rect( 0 ), int channel = -1 ) :
         iterator_base( image.EnsureUnique(), filter, rect, channel )
      {
      }

      /*!
       * Constructs a mutable, region-of-interest (ROI), filter pixel sample
       * iterator for a range of pixel samples of an image.
       *
       * \param image   The image to iterate.
       *
       * \param filter  Reference to a predicate object that will be used to
       *                filter pixel sample values.
       *
       * \param i       Pointer to the first pixel sample in the iteration
       *                range.
       *
       * \param j       End of the iteration range. The iterator will be
       *                invalid when it reaches (or surpasses) this point.
       *
       * \note Both iteration limits \a i and \a j must be pointers to pixel
       * samples in the \e same channel of the specified \a image.
       */
      roi_filter_sample_iterator( image_type& image, const F& filter, sample* i, sample* j ) :
         iterator_base( image, filter, i, j )
      {
      }

      /*!
       * Constructs a mutable, region-of-interest (ROI), filter pixel sample
       * iterator from the specified ROI iterator \a i and \a filter.
       */
      roi_filter_sample_iterator( const roi_sample_iterator& i, const F& filter ) :
         iterator_base( i, filter )
      {
      }

      /*!
       * Copy constructor.
       */
      roi_filter_sample_iterator( const roi_filter_sample_iterator& ) = default;

      /*!
       * Copy assignment operator. Returns a reference to this object.
       */
      roi_filter_sample_iterator& operator =( const roi_filter_sample_iterator& ) = default;

      /*!
       * Assigns a ROI pixel sample iterator. Returns a reference to this
       * object.
       */
      roi_filter_sample_iterator& operator =( const roi_sample_iterator& i )
      {
         (void)iterator_base::operator =( i );
         return *this;
      }

      /*!
       * Returns true only if this iterator is valid. A valid iterator defines
       * a valid iterated image and a non-null position.
       */
      bool IsValid() const
      {
         return this->m_image != nullptr && this->m_iterator != nullptr;
      }

      /*!
       * Returns a reference to the image being iterated by this object.
       */
      image_type& Image() const
      {
         return *this->m_image;
      }

      /*!
       * Returns a reference to the immutable predicate object used by this
       * filter iterator.
       */
      const filter_type& Filter() const
      {
         return this->m_filter;
      }

      /*!
       * Returns a reference to the mutable predicate object used by this
       * filter iterator.
       */
      filter_type& Filter()
      {
         return this->m_filter;
      }

      /*!
       * Returns a pointer to the pixel sample pointed to by this iterator.
       */
      sample* Position() const
      {
         return this->m_iterator;
      }

      /*!
       * Boolean type conversion operator. Returns true if this iterator is
       * active. A ROI pixel sample iterator is active if it has not reached
       * (or surpassed) its iteration limit, i.e. the bottom right corner of
       * its iterated region of interest.
       */
      operator bool() const
      {
         return this->m_iterator < this->m_end;
      }

      /*!
       * Indirection operator. Returns a reference to the pixel sample pointed
       * to by this iterator.
       */
      sample& operator *() const
      {
         return *this->m_iterator;
      }

      /*!
       * Pre-increment operator. Causes this iterator to point to the next
       * valid pixel sample in the iterated region of interest. Returns a
       * reference to this iterator.
       */
      roi_filter_sample_iterator& operator ++()
      {
         this->Increment();
         this->JumpToNextValidSample();
         return *this;
      }

      /*!
       * Post-increment operator. Causes this iterator to point to the next
       * valid pixel sample in the iterated region of interest. Returns a copy
       * of the iterator as it was before incrementing it.
       */
      roi_filter_sample_iterator operator ++( int )
      {
         roi_filter_sample_iterator i0( *this );
         this->Increment();
         this->JumpToNextValidSample();
         return i0;
      }

      /*!
       * Pre-decrement operator. Causes this iterator to point to the previous
       * valid pixel sample in the iterated region of interest, then returns a
       * reference to this iterator.
       */
      roi_filter_sample_iterator& operator --()
      {
         this->Decrement();
         this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Post-decrement operator. Causes this iterator to point to the previous
       * valid pixel sample in the iterated region of interest. Returns a copy
       * of the iterator as it was before decrementing it.
       */
      roi_filter_sample_iterator operator --( int )
      {
         roi_filter_sample_iterator i0( *this );
         this->Decrement();
         this->JumpToPrevValidSample();
         return i0;
      }

      /*!
       * Scalar assignment/addition operator. Increments this iterator by a
       * distance \a delta from its current position, within the rectangular
       * region of interest being iterated. Positive increments cause this
       * iterator to move forward (rightwards and downwards) by \a delta pixel
       * samples. Negative increments move this iterator backward (leftwards
       * and upwards) by \a delta pixel samples. Returns a reference to this
       * iterator.
       */
      roi_filter_sample_iterator& operator +=( distance_type delta )
      {
         int w = this->m_rowEnd - this->m_rowBegin;
         return MoveBy( delta%w, delta/w );
      }

      /*!
       * Scalar assignment/subtraction operator. Decrements this iterator by a
       * distance \a delta from its current position, within the rectangular
       * region of interest being iterated. Positive increments cause this
       * iterator to move backward (leftwards and upwards) by \a delta pixel
       * samples. Negative increments move this iterator forward (rightwards
       * and downwards) by \a delta pixel samples. Returns a reference to this
       * iterator.
       */
      roi_filter_sample_iterator& operator -=( distance_type delta )
      {
         int w = this->m_rowEnd - this->m_rowBegin;
         return MoveBy( -delta%w, -delta/w );
      }

      /*!
       * Moves this iterator within its rectangular region of interest by the
       * specified horizontal and vertical increments in pixels, \a dx and
       * \a dy respectively, relative to its current position. Positive
       * (negative) \a dx increments move the iterator rightwards (leftwards).
       * Positive (negative) \a dy increments move the iterator downwards
       * (upwards).
       */
      roi_filter_sample_iterator& MoveBy( int dx, int dy )
      {
         sample* i0 = this->m_iterator;
         iterator_base::MoveBy( dx, dy );
         if ( this->m_iterator >= i0 )
            this->JumpToNextValidSample();
         else
            this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to the specified iterator \a i incremented by a distance \a delta.
       */
      friend roi_filter_sample_iterator operator +( const roi_filter_sample_iterator& i, distance_type delta )
      {
         roi_filter_sample_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Iterator-to-scalar addition operator. This operator implements the
       * commutative property of scalar-to-iterator addition.
       */
      friend roi_filter_sample_iterator operator +( distance_type delta, const roi_filter_sample_iterator& i )
      {
         roi_filter_sample_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * the specified iterator \a i decremented by a distance \a delta.
       */
      friend roi_filter_sample_iterator operator -( const roi_filter_sample_iterator& i, distance_type delta )
      {
         roi_filter_sample_iterator j( i );
         j -= delta;
         return j;
      }

      /*!
       * Equality operator. Returns true if two iterators \a i and \a j point
       * to the same pixel sample.
       */
      friend bool operator ==( const roi_filter_sample_iterator& i, const roi_filter_sample_iterator& j )
      {
         return i.m_iterator == j.m_iterator;
      }

      /*!
       * Returns true iff an iterator \a i and a sample pointer \a j point to
       * the same pixel sample.
       */
      friend bool operator ==( const roi_filter_sample_iterator& i, const sample* j )
      {
         return i.m_iterator == j;
      }

      /*!
       * Returns true iff a sample pointer \a i and an iterator \a j point to
       * the same pixel sample.
       */
      friend bool operator ==( const sample* i, const roi_filter_sample_iterator& j )
      {
         return i == j.m_iterator;
      }

      /*!
       * Less than operator. Returns true if the specified iterator \a i
       * precedes another iterator \a j.
       */
      friend bool operator <( const roi_filter_sample_iterator& i, const roi_filter_sample_iterator& j )
      {
         return i.m_iterator < j.m_iterator;
      }

      /*!
       * Returns true iff an iterator \a i precedes a sample pointer \a j.
       */
      friend bool operator <( const roi_filter_sample_iterator& i, const sample* j )
      {
         return i.m_iterator < j;
      }

      /*!
       * Returns true iff a sample pointer \a i precedes an iterator \a j.
       */
      friend bool operator <( const sample* i, const roi_filter_sample_iterator& j )
      {
         return i < j.m_iterator;
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::GenericImage::const_roi_filter_sample_iterator
    * \brief Immutable region-of-interest, filter pixel sample iterator.
    *
    * An immutable, region-of-interest (ROI), filter pixel sample iterator
    * combines the capabilities of const_roi_sample_iterator and
    * const_filter_sample_iterator in a single iterator class.
    *
    * \ingroup image_iterators
    */
   template <class F>
   class const_roi_filter_sample_iterator : public roi_filter_sample_iterator_base<const GenericImage<P>, const sample*, F>
   {
   public:

      /*!
       * Represents the type of the iterated image.
       */
      typedef GenericImage<P>                   image_type;

      /*!
       * Represents the pixel traits class used by the iterated image.
       */
      typedef typename image_type::pixel_traits pixel_traits;

      /*!
       * Represents a pixel sample of the iterated image.
       */
      typedef typename image_type::sample       sample;

      /*!
       * Represents the type of the unary predicate used by this filter
       * iterator.
       */
      typedef F                                 filter_type;

      typedef roi_filter_sample_iterator_base<const GenericImage<P>, const sample*, F>
                                                iterator_base;

      /*!
       * Default constructor. Initializes an invalid iterator.
       */
      const_roi_filter_sample_iterator() :
         iterator_base()
      {
      }

      /*!
       * Constructs an immutable, region-of-interest (ROI), filter pixel sample
       * iterator for one channel of an image.
       *
       * \param image   The constant image to iterate.
       *
       * \param filter  Reference to a predicate object that will be used to
       *                filter pixel sample values.
       *
       * \param rect    Region of interest. If an empty rectangle is specified,
       *                the current rectangular selection in the \a image will
       *                be used. If the specified rectangle is not empty and
       *                extends beyond image boundaries, only the intersection
       *                with the image will be used. If that intersection does
       *                not exist, then the resulting iterator will be invalid,
       *                with an empty iteration range. The default value is an
       *                empty rectangle.
       *
       * \param channel Channel index. If this parameter is a negative integer,
       *                then this object will be initialized to iterate on the
       *                currently selected channel of the \a image. If an
       *                invalid (i.e., nonexistent) channel index is specified,
       *                then the resulting iterator will also be invalid, with
       *                an empty iteration range. The default value is -1.
       */
      const_roi_filter_sample_iterator( const image_type& image, const F& filter, const Rect& rect = Rect( 0 ), int channel = -1 ) :
         iterator_base( image, filter, rect, channel )
      {
      }

      /*!
       * Constructs an immutable, region-of-interest (ROI), filter pixel sample
       * iterator for a range of pixel samples of an image.
       *
       * \param image   The constant image to iterate.
       *
       * \param filter  Reference to a predicate object that will be used to
       *                filter pixel sample values.
       *
       * \param i       Pointer to the first constant pixel sample in the
       *                iteration range.
       *
       * \param j       End of the iteration range. The iterator will be
       *                invalid when it reaches (or surpasses) this point.
       *
       * \note Both iteration limits \a i and \a j must be pointers to pixel
       * samples in the \e same channel of the specified \a image.
       */
      const_roi_filter_sample_iterator( const image_type& image, const F& filter, const sample* i, const sample* j ) :
         iterator_base( image, filter, i, j )
      {
      }

      /*!
       * Constructs an immutable, region-of-interest (ROI), filter pixel sample
       * iterator from the specified immutable ROI iterator \a i and \a filter.
       */
      const_roi_filter_sample_iterator( const const_roi_sample_iterator& i, const F& filter ) :
         iterator_base( i, filter )
      {
      }

      /*!
       * Copy constructor.
       */
      const_roi_filter_sample_iterator( const const_roi_filter_sample_iterator& ) = default;

      /*!
       * Copy assignment operator. Returns a reference to this object.
       */
      const_roi_filter_sample_iterator& operator =( const const_roi_filter_sample_iterator& ) = default;

      /*!
       * Assigns an immutable ROI pixel sample iterator. Returns a reference to
       * this object.
       */
      const_roi_filter_sample_iterator& operator =( const const_roi_sample_iterator& i )
      {
         (void)iterator_base::operator =( i );
         return *this;
      }

      /*!
       * Returns true only if this iterator is valid. A valid iterator defines
       * a valid iterated image and a non-null position.
       */
      bool IsValid() const
      {
         return this->m_image != nullptr && this->m_iterator != nullptr;
      }

      /*!
       * Returns a reference to the constant image being iterated by this
       * object.
       */
      const image_type& Image() const
      {
         return *this->m_image;
      }

      /*!
       * Returns a reference to the immutable predicate object used by this
       * filter iterator.
       */
      const filter_type& Filter() const
      {
         return this->m_filter;
      }

      /*!
       * Returns a reference to the mutable predicate object used by this
       * filter iterator.
       */
      filter_type& Filter()
      {
         return this->m_filter;
      }

      /*!
       * Returns a pointer to the constant pixel sample pointed to by this
       * iterator.
       */
      const sample* Position() const
      {
         return this->m_iterator;
      }

      /*!
       * Boolean type conversion operator. Returns true if this iterator is
       * active. A ROI pixel sample iterator is active if it has not reached
       * (or surpassed) its iteration limit, i.e. the bottom right corner of
       * its iterated region of interest.
       */
      operator bool() const
      {
         return this->m_iterator < this->m_end;
      }

      /*!
       * Indirection operator. Returns a reference to the constant pixel sample
       * pointed to by this iterator.
       */
      const sample& operator *() const
      {
         return *this->m_iterator;
      }

      /*!
       * Pre-increment operator. Causes this iterator to point to the next
       * valid pixel sample in the iterated region of interest. Returns a
       * reference to this iterator.
       */
      const_roi_filter_sample_iterator& operator ++()
      {
         this->Increment();
         this->JumpToNextValidSample();
         return *this;
      }

      /*!
       * Post-increment operator. Causes this iterator to point to the next
       * valid pixel sample in the iterated region of interest. Returns a copy
       * of the iterator as it was before incrementing it.
       */
      const_roi_filter_sample_iterator operator ++( int )
      {
         const_roi_filter_sample_iterator i0( *this );
         this->Increment();
         this->JumpToNextValidSample();
         return i0;
      }

      /*!
       * Pre-decrement operator. Causes this iterator to point to the previous
       * valid pixel sample in the iterated region of interest, then returns a
       * reference to this iterator.
       */
      const_roi_filter_sample_iterator& operator --()
      {
         this->Decrement();
         this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Post-decrement operator. Causes this iterator to point to the previous
       * valid pixel sample in the iterated region of interest. Returns a copy
       * of the iterator as it was before decrementing it.
       */
      const_roi_filter_sample_iterator operator --( int )
      {
         const_roi_filter_sample_iterator i0( *this );
         this->Decrement();
         this->JumpToPrevValidSample();
         return i0;
      }

      /*!
       * Scalar assignment/addition operator. Increments this iterator by a
       * distance \a delta from its current position, within the rectangular
       * region of interest being iterated. Positive increments cause this
       * iterator to move forward (rightwards and downwards) by \a delta pixel
       * samples. Negative increments move this iterator backward (leftwards
       * and upwards) by \a delta pixel samples. Returns a reference to this
       * iterator.
       */
      const_roi_filter_sample_iterator& operator +=( distance_type delta )
      {
         int w = this->m_rowEnd - this->m_rowBegin;
         return MoveBy( delta%w, delta/w );
      }

      /*!
       * Scalar assignment/subtraction operator. Decrements this iterator by a
       * distance \a delta from its current position, within the rectangular
       * region of interest being iterated. Positive increments cause this
       * iterator to move backward (leftwards and upwards) by \a delta pixel
       * samples. Negative increments move this iterator forward (rightwards
       * and downwards) by \a delta pixel samples. Returns a reference to this
       * iterator.
       */
      const_roi_filter_sample_iterator& operator -=( distance_type delta )
      {
         int w = this->m_rowEnd - this->m_rowBegin;
         return MoveBy( -delta%w, -delta/w );
      }

      /*!
       * Moves this iterator within its rectangular region of interest by the
       * specified horizontal and vertical increments in pixels, \a dx and
       * \a dy respectively, relative to its current position. Positive
       * (negative) \a dx increments move the iterator rightwards (leftwards).
       * Positive (negative) \a dy increments move the iterator downwards
       * (upwards).
       */
      const_roi_filter_sample_iterator& MoveBy( int dx, int dy )
      {
         const sample* i0 = this->m_iterator;
         iterator_base::MoveBy( dx, dy );
         if ( this->m_iterator >= i0 )
            this->JumpToNextValidSample();
         else
            this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to the specified iterator \a i incremented by a distance \a delta.
       */
      friend const_roi_filter_sample_iterator operator +( const const_roi_filter_sample_iterator& i, distance_type delta )
      {
         const_roi_filter_sample_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Iterator-to-scalar addition operator. This operator implements the
       * commutative property of scalar-to-iterator addition.
       */
      friend const_roi_filter_sample_iterator operator +( distance_type delta, const const_roi_filter_sample_iterator& i )
      {
         const_roi_filter_sample_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * the specified iterator \a i decremented by a distance \a delta.
       */
      friend const_roi_filter_sample_iterator operator -( const const_roi_filter_sample_iterator& i, distance_type delta )
      {
         const_roi_filter_sample_iterator j( i );
         j -= delta;
         return j;
      }

      /*!
       * Equality operator. Returns true if two iterators \a i and \a j point
       * to the same pixel sample.
       */
      friend bool operator ==( const const_roi_filter_sample_iterator& i, const const_roi_filter_sample_iterator& j )
      {
         return i.m_iterator == j.m_iterator;
      }

      /*!
       * Returns true iff an iterator \a i and a sample pointer \a j point to
       * the same pixel sample.
       */
      friend bool operator ==( const const_roi_filter_sample_iterator& i, const sample* j )
      {
         return i.m_iterator == j;
      }

      /*!
       * Returns true iff a sample pointer \a i and an iterator \a j point to
       * the same pixel sample.
       */
      friend bool operator ==( const sample* i, const const_roi_filter_sample_iterator& j )
      {
         return i == j.m_iterator;
      }

      /*!
       * Less than operator. Returns true if the specified iterator \a i
       * precedes another iterator \a j.
       */
      friend bool operator <( const const_roi_filter_sample_iterator& i, const const_roi_filter_sample_iterator& j )
      {
         return i.m_iterator < j.m_iterator;
      }

      /*!
       * Returns true iff an iterator \a i precedes a sample pointer \a j.
       */
      friend bool operator <( const const_roi_filter_sample_iterator& i, const sample* j )
      {
         return i.m_iterator < j;
      }

      /*!
       * Returns true iff a sample pointer \a i precedes an iterator \a j.
       */
      friend bool operator <( const sample* i, const const_roi_filter_sample_iterator& j )
      {
         return i < j.m_iterator;
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::GenericImage::pixel_iterator
    * \brief Mutable pixel iterator.
    *
    * A mutable pixel iterator provides read/write, random access to all pixels
    * of an image.
    *
    * \ingroup image_iterators
    */
   class pixel_iterator
   {
   public:

      /*!
       * Represents the type of the iterated image.
       */
      typedef GenericImage<P>                   image_type;

      /*!
       * Represents the pixel traits class used by the iterated image.
       */
      typedef typename image_type::pixel_traits pixel_traits;

      /*!
       * Represents a pixel sample of the iterated image.
       */
      typedef typename image_type::sample       sample;

      typedef GenericVector<sample*>            iterator_type;

      /*!
       * Default constructor. Initializes an invalid iterator.
       */
      pixel_iterator() :
         m_image( nullptr ), m_iterator(), m_end( nullptr )
      {
      }

      /*!
       * Constructs a mutable pixel iterator for the specified \a image.
       */
      pixel_iterator( image_type& image ) :
         m_image( &image ), m_iterator(), m_end( nullptr )
      {
         m_image->EnsureUnique();
         if ( !m_image->IsEmpty() )
         {
            m_iterator = iterator_type( m_image->NumberOfChannels() );
            for ( int i = 0; i < m_iterator.Length(); ++i )
               m_iterator[i] = (*m_image)[i];
            m_end = m_iterator[0] + m_image->NumberOfPixels();
         }
      }

      /*!
       * Copy constructor.
       */
      pixel_iterator( const pixel_iterator& ) = default;

      /*!
       * Copy assignment operator. Returns a reference to this iterator.
       */
      pixel_iterator& operator =( const pixel_iterator& ) = default;

      /*!
       * Returns true only if this iterator is valid. A valid iterator defines
       * a valid iterated image and a non-null position.
       */
      bool IsValid() const
      {
         return m_image != nullptr && !m_iterator.IsEmpty();
      }

      /*!
       * Returns a reference to the image being iterated by this object.
       */
      image_type& Image() const
      {
         return *m_image;
      }

      /*!
       * Returns a pointer to the pixel sample pointed to by this iterator in
       * the specified \a channel.
       */
      sample* Position( int channel ) const
      {
         return m_iterator[channel];
      }

      /*!
       * Boolean type conversion operator. Returns true if this iterator is
       * active. A pixel iterator is active if it has not reached (or
       * surpassed) its iteration limit.
       */
      operator bool() const
      {
         return m_iterator[0] < m_end;
      }

      /*!
       * Array subscript operator. Returns a reference to the pixel sample
       * pointed to by this iterator in the specified \a channel.
       */
      sample& operator []( int channel ) const
      {
         return *m_iterator[channel];
      }

      /*!
       * Pre-increment operator. Causes this iterator to point to the next
       * pixel in the iterated image. Returns a reference to this iterator.
       */
      pixel_iterator& operator ++()
      {
         for ( int i = 0; i < m_iterator.Length(); ++i )
            ++m_iterator[i];
         return *this;
      }

      /*!
       * Post-increment operator. Causes this iterator to point to the next
       * pixel in the iterated image. Returns a copy of the iterator as it was
       * before incrementing it.
       */
      pixel_iterator operator ++( int )
      {
         pixel_iterator i0( *this );
         for ( int i = 0; i < m_iterator.Length(); ++i )
            ++m_iterator[i];
         return i0;
      }

      /*!
       * Pre-decrement operator. Causes this iterator to point to the previous
       * pixel in the iterated image. Returns a reference to this iterator.
       */
      pixel_iterator& operator --()
      {
         for ( int i = 0; i < m_iterator.Length(); ++i )
            --m_iterator[i];
         return *this;
      }

      /*!
       * Post-decrement operator. Causes this iterator to point to the previous
       * pixel in the iterated image. Returns a copy of the iterator as it was
       * before decrementing it.
       */
      pixel_iterator operator --( int )
      {
         pixel_iterator i0( *this );
         for ( int i = 0; i < m_iterator.Length(); ++i )
            --m_iterator[i];
         return i0;
      }

      /*!
       * Scalar assignment/addition operator. Increments this iterator by a
       * distance \a delta from its current position. Positive increments cause
       * this iterator to move forward by \a delta pixels. Negative increments
       * move this iterator backward by \a delta pixels. Returns a reference to
       * this iterator.
       */
      pixel_iterator& operator +=( distance_type delta )
      {
         for ( int i = 0; i < m_iterator.Length(); ++i )
            m_iterator[i] += delta;
         return *this;
      }

      /*!
       * Scalar assignment/subtraction operator. Decrements this iterator by a
       * distance \a delta from its current position. Positive increments cause
       * this iterator to move backward by \a delta pixels. Negative increments
       * move this iterator forward by \a delta pixels. Returns a reference to
       * this iterator.
       */
      pixel_iterator& operator -=( distance_type delta )
      {
         for ( int i = 0; i < m_iterator.Length(); ++i )
            m_iterator[i] -= delta;
         return *this;
      }

      /*!
       * Moves this iterator on the iterated image by the specified horizontal
       * and vertical increments in pixels, \a dx and \a dy respectively,
       * relative to its current position. Positive (negative) \a dx increments
       * move the iterator rightwards (leftwards). Positive (negative) \a dy
       * increments move the iterator downwards (upwards).
       */
      pixel_iterator& MoveBy( int dx, int dy )
      {
         return operator +=( distance_type( dy )*m_image->Width() + distance_type( dx ) );
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to the specified iterator \a i incremented by a distance \a delta.
       */
      friend pixel_iterator operator +( const pixel_iterator& i, distance_type delta )
      {
         pixel_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Iterator-to-scalar addition operator. This operator implements the
       * commutative property of scalar-to-iterator addition.
       */
      friend pixel_iterator operator +( distance_type delta, const pixel_iterator& i )
      {
         pixel_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * the specified iterator \a i decremented by a distance \a delta.
       */
      friend pixel_iterator operator -( const pixel_iterator& i, distance_type delta )
      {
         pixel_iterator j( i );
         j -= delta;
         return j;
      }

      /*!
       * Iterator subtraction operator. Returns the distance in pixels between
       * the specified iterators \a i and \a j.
       */
      friend distance_type operator -( const pixel_iterator& i, const pixel_iterator& j )
      {
         return i.m_iterator[0] - j.m_iterator[0];
      }

      /*!
       * Equality operator. Returns true if two iterators \a i and \a j point
       * to the same pixel.
       */
      friend bool operator ==( const pixel_iterator& i, const pixel_iterator& j )
      {
         return i.m_iterator[0] == j.m_iterator[0];
      }

      /*!
       * Less than operator. Returns true if the specified iterator \a i
       * precedes another iterator \a j.
       */
      friend bool operator <( const pixel_iterator& i, const pixel_iterator& j )
      {
         return i.m_iterator[0] < j.m_iterator[0];
      }

   protected:

            image_type*   m_image;
            iterator_type m_iterator;
      const sample*       m_end;
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::GenericImage::const_pixel_iterator
    * \brief Immutable pixel iterator.
    *
    * An immutable pixel iterator provides read-only, random access to all
    * pixels of an image.
    *
    * \ingroup image_iterators
    */
   class const_pixel_iterator
   {
   public:

      /*!
       * Represents the type of the iterated image.
       */
      typedef GenericImage<P>                   image_type;

      /*!
       * Represents the pixel traits class used by the iterated image.
       */
      typedef typename image_type::pixel_traits pixel_traits;

      /*!
       * Represents a pixel sample of the iterated image.
       */
      typedef typename image_type::sample       sample;

      typedef GenericVector<const sample*>      iterator_type;

      /*!
       * Default constructor. Initializes an invalid iterator.
       */
      const_pixel_iterator() :
         m_image( nullptr ), m_iterator(), m_end( nullptr )
      {
      }

      /*!
       * Constructs an immutable pixel iterator for the specified \a image.
       */
      const_pixel_iterator( const image_type& image ) :
         m_image( &image ), m_iterator(), m_end( nullptr )
      {
         if ( !m_image->IsEmpty() )
         {
            m_iterator = iterator_type( m_image->NumberOfChannels() );
            for ( int i = 0; i < m_iterator.Length(); ++i )
               m_iterator[i] = (*m_image)[i];
            m_end = m_iterator[0] + m_image->NumberOfPixels();
         }
      }

      /*!
       * Copy constructor.
       */
      const_pixel_iterator( const const_pixel_iterator& ) = default;

      /*!
       * Copy assignment operator. Returns a reference to this iterator.
       */
      const_pixel_iterator& operator =( const const_pixel_iterator& ) = default;

      /*!
       * Returns true only if this iterator is valid. A valid iterator defines
       * a valid iterated image and a non-null position.
       */
      bool IsValid() const
      {
         return m_image != nullptr && !m_iterator.IsEmpty();
      }

      /*!
       * Returns a reference to the constant image being iterated by this
       * object.
       */
      const image_type& Image() const
      {
         return *m_image;
      }

      /*!
       * Returns a pointer to the constant pixel sample pointed to by this
       * iterator in the specified \a channel.
       */
      const sample* Position( int channel ) const
      {
         return m_iterator[channel];
      }

      /*!
       * Boolean type conversion operator. Returns true if this iterator is
       * active. A pixel iterator is active if it has not reached (or
       * surpassed) its iteration limit.
       */
      operator bool() const
      {
         return m_iterator[0] < m_end;
      }

      /*!
       * Array subscript operator. Returns a reference to the constant pixel
       * sample pointed to by this iterator in the specified \a channel.
       */
      const sample& operator []( int channel ) const
      {
         return *m_iterator[channel];
      }

      /*!
       * Pre-increment operator. Causes this iterator to point to the next
       * pixel in the iterated image. Returns a reference to this iterator.
       */
      const_pixel_iterator& operator ++()
      {
         for ( int i = 0; i < m_iterator.Length(); ++i )
            ++m_iterator[i];
         return *this;
      }

      /*!
       * Post-increment operator. Causes this iterator to point to the next
       * pixel in the iterated image. Returns a copy of the iterator as it was
       * before incrementing it.
       */
      const_pixel_iterator operator ++( int )
      {
         const_pixel_iterator i0( *this );
         for ( int i = 0; i < m_iterator.Length(); ++i )
            ++m_iterator[i];
         return i0;
      }

      /*!
       * Pre-decrement operator. Causes this iterator to point to the previous
       * pixel in the iterated image. Returns a reference to this iterator.
       */
      const_pixel_iterator& operator --()
      {
         for ( int i = 0; i < m_iterator.Length(); ++i )
            --m_iterator[i];
         return *this;
      }

      /*!
       * Post-decrement operator. Causes this iterator to point to the previous
       * pixel in the iterated image. Returns a copy of the iterator as it was
       * before decrementing it.
       */
      const_pixel_iterator operator --( int )
      {
         const_pixel_iterator i0( *this );
         for ( int i = 0; i < m_iterator.Length(); ++i )
            --m_iterator[i];
         return i0;
      }

      /*!
       * Scalar assignment/addition operator. Increments this iterator by a
       * distance \a delta from its current position. Positive increments cause
       * this iterator to move forward by \a delta pixels. Negative increments
       * move this iterator backward by \a delta pixels. Returns a reference to
       * this iterator.
       */
      const_pixel_iterator& operator +=( distance_type delta )
      {
         for ( int i = 0; i < m_iterator.Length(); ++i )
            m_iterator[i] += delta;
         return *this;
      }

      /*!
       * Scalar assignment/subtraction operator. Decrements this iterator by a
       * distance \a delta from its current position. Positive increments cause
       * this iterator to move backward by \a delta pixels. Negative increments
       * move this iterator forward by \a delta pixels. Returns a reference to
       * this iterator.
       */
      const_pixel_iterator& operator -=( distance_type delta )
      {
         for ( int i = 0; i < m_iterator.Length(); ++i )
            m_iterator[i] -= delta;
         return *this;
      }

      /*!
       * Moves this iterator on the iterated image by the specified horizontal
       * and vertical increments in pixels, \a dx and \a dy respectively,
       * relative to its current position. Positive (negative) \a dx increments
       * move the iterator rightwards (leftwards). Positive (negative) \a dy
       * increments move the iterator downwards (upwards).
       */
      const_pixel_iterator& MoveBy( int dx, int dy )
      {
         return operator +=( distance_type( dy )*m_image->Width() + distance_type( dx ) );
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to the specified iterator \a i incremented by a distance \a delta.
       */
      friend const_pixel_iterator operator +( const const_pixel_iterator& i, distance_type delta )
      {
         const_pixel_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Iterator-to-scalar addition operator. This operator implements the
       * commutative property of scalar-to-iterator addition.
       */
      friend const_pixel_iterator operator +( distance_type delta, const const_pixel_iterator& i )
      {
         const_pixel_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * the specified iterator \a i decremented by a distance \a delta.
       */
      friend const_pixel_iterator operator -( const const_pixel_iterator& i, distance_type delta )
      {
         const_pixel_iterator j( i );
         j -= delta;
         return j;
      }

      /*!
       * Iterator subtraction operator. Returns the distance in pixels between
       * the specified iterators \a i and \a j.
       */
      friend distance_type operator -( const const_pixel_iterator& i, const const_pixel_iterator& j )
      {
         return i.m_iterator[0] - j.m_iterator[0];
      }

      /*!
       * Equality operator. Returns true if two iterators \a i and \a j point
       * to the same pixel.
       */
      friend bool operator ==( const const_pixel_iterator& i, const const_pixel_iterator& j )
      {
         return i.m_iterator[0] == j.m_iterator[0];
      }

      /*!
       * Less than operator. Returns true if the specified iterator \a i
       * precedes another iterator \a j.
       */
      friend bool operator <( const const_pixel_iterator& i, const const_pixel_iterator& j )
      {
         return i.m_iterator[0] < j.m_iterator[0];
      }

   protected:

      const image_type*   m_image;
            iterator_type m_iterator;
      const sample*       m_end;
   };

   // -------------------------------------------------------------------------

   template <class image_type, class sample_pointer>
   class roi_pixel_iterator_base
   {
   protected:

      typedef GenericVector<sample_pointer>     iterator_type;

      image_type*    m_image;
      iterator_type  m_iterator;
      sample_pointer m_rowBegin;
      sample_pointer m_rowEnd;
      sample_pointer m_end;

      roi_pixel_iterator_base() :
         m_image( nullptr ), m_iterator(), m_rowBegin( nullptr ), m_rowEnd( nullptr ), m_end( nullptr )
      {
      }

      roi_pixel_iterator_base( image_type& image, const Rect& rect ) :
         m_image( &image ), m_iterator(), m_rowBegin( nullptr ), m_rowEnd( nullptr ), m_end( nullptr )
      {
         Rect r = rect;
         if ( m_image->ParseRect( r ) )
         {
            m_iterator = iterator_type( m_image->NumberOfChannels() );
            for ( int i = 0; i < m_iterator.Length(); ++i )
               m_iterator[i] = m_image->PixelAddress( r.x0, r.y0, i );
            m_rowBegin = m_iterator[0];
            m_rowEnd = m_rowBegin + r.Width();
            m_end = m_rowEnd + ((r.Height() - 1)*m_image->Width());
         }
      }

      roi_pixel_iterator_base( const roi_pixel_iterator_base& ) = default;

      roi_pixel_iterator_base& operator =( const roi_pixel_iterator_base& ) = default;

      void Increment()
      {
         for ( int i = 0; i < m_iterator.Length(); ++i )
            ++m_iterator[i];
         if ( m_iterator[0] == m_rowEnd )
         {
            int w = m_rowEnd - m_rowBegin;
            for ( int i = 0; i < m_iterator.Length(); ++i )
               m_iterator[i] += m_image->Width() - w;
            m_rowBegin += m_image->Width();
            m_rowEnd += m_image->Width();
         }
      }

      void Decrement()
      {
         if ( m_iterator[0] == m_rowBegin )
         {
            int w = m_rowEnd - m_rowBegin;
            for ( int i = 0; i < m_iterator.Length(); ++i )
               m_iterator[i] -= m_image->Width() - w;
            m_rowBegin -= m_image->Width();
            m_rowEnd -= m_image->Width();
         }
         for ( int i = 0; i < m_iterator.Length(); ++i )
            --m_iterator[i];
      }

      void MoveBy( int cols, int rows )
      {
         int dx = m_iterator[0] - m_rowBegin;
         for ( int i = 0; i < m_iterator.Length(); ++i )
            m_iterator[i] -= dx;
         cols += dx;
         if ( cols != 0 )
         {
            int w = m_rowEnd - m_rowBegin;
            if ( pcl::Abs( cols ) >= w )
            {
               rows += cols/w;
               cols %= w;
            }
         }
         int dy = rows * m_image->Width();
         for ( int i = 0; i < m_iterator.Length(); ++i )
            m_iterator[i] += dy + cols;
         m_rowBegin += dy;
         m_rowEnd += dy;
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::GenericImage::roi_pixel_iterator
    * \brief Mutable region-of-interest pixel iterator.
    *
    * A mutable, region-of-interest (ROI) pixel iterator provides read/write,
    * random access to pixels within a rectangular subset of an image.
    *
    * \ingroup image_iterators
    */
   class roi_pixel_iterator : private roi_pixel_iterator_base<GenericImage<P>, sample*>
   {
   public:

      /*!
       * Represents the type of the iterated image.
       */
      typedef GenericImage<P>                   image_type;

      /*!
       * Represents the pixel traits class used by the iterated image.
       */
      typedef typename image_type::pixel_traits pixel_traits;

      /*!
       * Represents a pixel sample of the iterated image.
       */
      typedef typename image_type::sample       sample;

      typedef roi_pixel_iterator_base<GenericImage<P>, sample*>
                                                iterator_base;

      /*!
       * Default constructor. Initializes an invalid iterator.
       */
      roi_pixel_iterator() :
         iterator_base()
      {
      }

      /*!
       * Constructs a mutable, region-of-interest (ROI) pixel iterator.
       *
       * \param image   The image to iterate.
       *
       * \param rect    Region of interest. If an empty rectangle is specified,
       *                the current rectangular selection in the \a image will
       *                be used. If the specified rectangle is not empty and
       *                extends beyond image boundaries, only the intersection
       *                with the image will be used. If that intersection does
       *                not exist, then the resulting iterator will be invalid,
       *                with an empty iteration range. The default value is an
       *                empty rectangle.
       */
      roi_pixel_iterator( image_type& image, const Rect& rect = Rect( 0 ) ) :
         iterator_base( image.EnsureUnique(), rect )
      {
      }

      /*!
       * Copy constructor.
       */
      roi_pixel_iterator( const roi_pixel_iterator& ) = default;

      /*!
       * Copy assignment operator. Returns a reference to this object.
       */
      roi_pixel_iterator& operator =( const roi_pixel_iterator& ) = default;

      /*!
       * Returns true only if this iterator is valid. A valid iterator defines
       * a valid iterated image and a non-null position.
       */
      bool IsValid() const
      {
         return this->m_image != nullptr && !this->m_iterator.IsEmpty();
      }

      /*!
       * Returns a reference to the image being iterated by this object.
       */
      image_type& Image() const
      {
         return *this->m_image;
      }

      /*!
       * Returns a pointer to the pixel sample pointed to by this iterator in
       * the specified \a channel.
       */
      sample* Position( int channel ) const
      {
         return this->m_iterator[channel];
      }

      /*!
       * Boolean type conversion operator. Returns true if this iterator is
       * active. A ROI pixel iterator is active if it has not reached (or
       * surpassed) its iteration limit, i.e. the bottom right corner of its
       * iterated region of interest.
       */
      operator bool() const
      {
         return this->m_iterator[0] < this->m_end;
      }

      /*!
       * Array subscript operator. Returns a reference to the pixel sample
       * pointed to by this iterator in the specified \a channel.
       */
      sample& operator []( int channel ) const
      {
         return *this->m_iterator[channel];
      }

      /*!
       * Pre-increment operator. Causes this iterator to point to the next
       * pixel in the iterated region of interest, then returns a reference to
       * this iterator.
       */
      roi_pixel_iterator& operator ++()
      {
         this->Increment();
         return *this;
      }

      /*!
       * Post-increment operator. Causes this iterator to point to the next
       * pixel in the iterated region of interest. Returns a copy of the
       * iterator as it was before incrementing it.
       */
      roi_pixel_iterator operator ++( int )
      {
         roi_pixel_iterator i0( *this );
         this->Increment();
         return i0;
      }

      /*!
       * Pre-decrement operator. Causes this iterator to point to the previous
       * pixel in the iterated region of interest, then returns a reference to
       * this iterator.
       */
      roi_pixel_iterator& operator --()
      {
         this->Decrement();
         return *this;
      }

      /*!
       * Post-decrement operator. Causes this iterator to point to the previous
       * pixel in the iterated region of interest. Returns a copy of the
       * iterator as it was before decrementing it.
       */
      roi_pixel_iterator operator --( int )
      {
         roi_pixel_iterator i0( *this );
         this->Decrement();
         return i0;
      }

      /*!
       * Scalar assignment/addition operator. Increments this iterator by a
       * distance \a delta from its current position, within the rectangular
       * region of interest being iterated. Positive increments cause this
       * iterator to move forward (rightwards and downwards) by \a delta pixel
       * samples. Negative increments move this iterator backward (leftwards
       * and upwards) by \a delta pixel samples. Returns a reference to this
       * iterator.
       */
      roi_pixel_iterator& operator +=( distance_type delta )
      {
         int w = this->m_rowEnd - this->m_rowBegin;
         iterator_base::MoveBy( delta%w, delta/w );
         return *this;
      }

      /*!
       * Scalar assignment/subtraction operator. Decrements this iterator by a
       * distance \a delta from its current position, within the rectangular
       * region of interest being iterated. Positive increments cause this
       * iterator to move backward (leftwards and upwards) by \a delta pixel
       * samples. Negative increments move this iterator forward (rightwards
       * and downwards) by \a delta pixel samples. Returns a reference to this
       * iterator.
       */
      roi_pixel_iterator& operator -=( distance_type delta )
      {
         int w = this->m_rowEnd - this->m_rowBegin;
         iterator_base::MoveBy( -delta%w, -delta/w );
         return *this;
      }

      /*!
       * Moves this iterator within its rectangular region of interest by the
       * specified horizontal and vertical increments in pixels, \a dx and
       * \a dy respectively, relative to its current position. Positive
       * (negative) \a dx increments move the iterator rightwards (leftwards).
       * Positive (negative) \a dy increments move the iterator downwards
       * (upwards).
       */
      roi_pixel_iterator& MoveBy( int dx, int dy )
      {
         iterator_base::MoveBy( dx, dy );
         return *this;
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to the specified iterator \a i incremented by a distance \a delta.
       */
      friend roi_pixel_iterator operator +( const roi_pixel_iterator& i, distance_type delta )
      {
         roi_pixel_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Iterator-to-scalar addition operator. This operator implements the
       * commutative property of scalar-to-iterator addition.
       */
      friend roi_pixel_iterator operator +( distance_type delta, const roi_pixel_iterator& i )
      {
         roi_pixel_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * the specified iterator \a i decremented by a distance \a delta.
       */
      friend roi_pixel_iterator operator -( const roi_pixel_iterator& i, distance_type delta )
      {
         roi_pixel_iterator j( i );
         j -= delta;
         return j;
      }

      /*!
       * Equality operator. Returns true if two iterators \a i and \a j point
       * to the same pixel.
       */
      friend bool operator ==( const roi_pixel_iterator& i, const roi_pixel_iterator& j )
      {
         return i.m_iterator[0] == j.m_iterator[0];
      }

      /*!
       * Less than operator. Returns true if the specified iterator \a i
       * precedes another iterator \a j.
       */
      friend bool operator <( const roi_pixel_iterator& i, const roi_pixel_iterator& j )
      {
         return i.m_iterator[0] < j.m_iterator[0];
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::GenericImage::const_roi_pixel_iterator
    * \brief Immutable region-of-interest pixel iterator.
    *
    * An immutable, region-of-interest (ROI) pixel iterator provides read-only,
    * random access to pixels within a rectangular subset of an image.
    *
    * \ingroup image_iterators
    */
   class const_roi_pixel_iterator : private roi_pixel_iterator_base<const GenericImage<P>, const sample*>
   {
   public:

      /*!
       * Represents the type of the iterated image.
       */
      typedef GenericImage<P>                   image_type;

      /*!
       * Represents the pixel traits class used by the iterated image.
       */
      typedef typename image_type::pixel_traits pixel_traits;

      /*!
       * Represents a pixel sample of the iterated image.
       */
      typedef typename image_type::sample       sample;

      typedef roi_pixel_iterator_base<const GenericImage<P>, const sample*>
                                                iterator_base;

      /*!
       * Default constructor. Initializes an invalid iterator.
       */
      const_roi_pixel_iterator() :
         iterator_base()
      {
      }

      /*!
       * Constructs an immutable, region-of-interest (ROI) pixel iterator.
       *
       * \param image   The constant image to iterate.
       *
       * \param rect    Region of interest. If an empty rectangle is specified,
       *                the current rectangular selection in the \a image will
       *                be used. If the specified rectangle is not empty and
       *                extends beyond image boundaries, only the intersection
       *                with the image will be used. If that intersection does
       *                not exist, then the resulting iterator will be invalid,
       *                with an empty iteration range. The default value is an
       *                empty rectangle.
       */
      const_roi_pixel_iterator( const image_type& image, const Rect& rect = Rect( 0 ) ) :
         iterator_base( image, rect )
      {
      }

      /*!
       * Copy constructor.
       */
      const_roi_pixel_iterator( const const_roi_pixel_iterator& ) = default;

      /*!
       * Copy assignment operator. Returns a reference to this object.
       */
      const_roi_pixel_iterator& operator =( const const_roi_pixel_iterator& ) = default;

      /*!
       * Returns true only if this iterator is valid. A valid iterator defines
       * a valid iterated image and a non-null position.
       */
      bool IsValid() const
      {
         return this->m_image != nullptr && !this->m_iterator.IsEmpty();
      }

      /*!
       * Returns a reference to the constant image being iterated by this
       * object.
       */
      const image_type& Image() const
      {
         return *this->m_image;
      }

      /*!
       * Returns a pointer to the constant pixel sample pointed to by this
       * iterator in the specified \a channel.
       */
      const sample* Position( int channel ) const
      {
         return this->m_iterator[channel];
      }

      /*!
       * Boolean type conversion operator. Returns true if this iterator is
       * active. A ROI pixel iterator is active if it has not reached (or
       * surpassed) its iteration limit, i.e. the bottom right corner of its
       * iterated region of interest.
       */
      operator bool() const
      {
         return this->m_iterator[0] < this->m_end;
      }

      /*!
       * Array subscript operator. Returns a reference to the constant pixel
       * sample pointed to by this iterator in the specified \a channel.
       */
      const sample& operator []( int channel ) const
      {
         return *this->m_iterator[channel];
      }

      /*!
       * Pre-increment operator. Causes this iterator to point to the next
       * pixel in the iterated region of interest, then returns a reference to
       * this iterator.
       */
      const_roi_pixel_iterator& operator ++()
      {
         this->Increment();
         return *this;
      }

      /*!
       * Post-increment operator. Causes this iterator to point to the next
       * pixel in the iterated region of interest. Returns a copy of the
       * iterator as it was before incrementing it.
       */
      const_roi_pixel_iterator operator ++( int )
      {
         const_roi_pixel_iterator i0( *this );
         this->Increment();
         return i0;
      }

      /*!
       * Pre-decrement operator. Causes this iterator to point to the previous
       * pixel in the iterated region of interest, then returns a reference to
       * this iterator.
       */
      const_roi_pixel_iterator& operator --()
      {
         this->Decrement();
         return *this;
      }

      /*!
       * Post-decrement operator. Causes this iterator to point to the previous
       * pixel in the iterated region of interest. Returns a copy of the
       * iterator as it was before decrementing it.
       */
      const_roi_pixel_iterator operator --( int )
      {
         const_roi_pixel_iterator i0( *this );
         this->Decrement();
         return i0;
      }

      /*!
       * Scalar assignment/addition operator. Increments this iterator by a
       * distance \a delta from its current position, within the rectangular
       * region of interest being iterated. Positive increments cause this
       * iterator to move forward (rightwards and downwards) by \a delta pixel
       * samples. Negative increments move this iterator backward (leftwards
       * and upwards) by \a delta pixel samples. Returns a reference to this
       * iterator.
       */
      const_roi_pixel_iterator& operator +=( distance_type delta )
      {
         int w = this->m_rowEnd - this->m_rowBegin;
         iterator_base::MoveBy( delta%w, delta/w );
         return *this;
      }

      /*!
       * Scalar assignment/subtraction operator. Decrements this iterator by a
       * distance \a delta from its current position, within the rectangular
       * region of interest being iterated. Positive increments cause this
       * iterator to move backward (leftwards and upwards) by \a delta pixel
       * samples. Negative increments move this iterator forward (rightwards
       * and downwards) by \a delta pixel samples. Returns a reference to this
       * iterator.
       */
      const_roi_pixel_iterator& operator -=( distance_type delta )
      {
         int w = this->m_rowEnd - this->m_rowBegin;
         iterator_base::MoveBy( -delta%w, -delta/w );
         return *this;
      }

      /*!
       * Moves this iterator within its rectangular region of interest by the
       * specified horizontal and vertical increments in pixels, \a dx and
       * \a dy respectively, relative to its current position. Positive
       * (negative) \a dx increments move the iterator rightwards (leftwards).
       * Positive (negative) \a dy increments move the iterator downwards
       * (upwards).
       */
      const_roi_pixel_iterator& MoveBy( int dx, int dy )
      {
         iterator_base::MoveBy( dx, dy );
         return *this;
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to the specified iterator \a i incremented by a distance \a delta.
       */
      friend const_roi_pixel_iterator operator +( const const_roi_pixel_iterator& i, distance_type delta )
      {
         const_roi_pixel_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Iterator-to-scalar addition operator. This operator implements the
       * commutative property of scalar-to-iterator addition.
       */
      friend const_roi_pixel_iterator operator +( distance_type delta, const const_roi_pixel_iterator& i )
      {
         const_roi_pixel_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * the specified iterator \a i decremented by a distance \a delta.
       */
      friend const_roi_pixel_iterator operator -( const const_roi_pixel_iterator& i, distance_type delta )
      {
         const_roi_pixel_iterator j( i );
         j -= delta;
         return j;
      }

      /*!
       * Equality operator. Returns true if two iterators \a i and \a j point
       * to the same pixel.
       */
      friend bool operator ==( const const_roi_pixel_iterator& i, const const_roi_pixel_iterator& j )
      {
         return i.m_iterator[0] == j.m_iterator[0];
      }

      /*!
       * Less than operator. Returns true if the specified iterator \a i
       * precedes another iterator \a j.
       */
      friend bool operator <( const const_roi_pixel_iterator& i, const const_roi_pixel_iterator& j )
      {
         return i.m_iterator[0] < j.m_iterator[0];
      }
   };

   // -------------------------------------------------------------------------

   template <class image_type, class iterator_base, class sample_pointer, class filter_type>
   class filter_pixel_iterator_base : public iterator_base
   {
   protected:

      filter_type    m_filter;
      sample_pointer m_begin;

      filter_pixel_iterator_base() :
         iterator_base(), m_filter(), m_begin( nullptr )
      {
      }

      filter_pixel_iterator_base( image_type& image, const filter_type& filter ) :
         iterator_base( image ), m_filter( filter ), m_begin( iterator_base::m_iterator )
      {
         JumpToNextValidSample();
      }

      filter_pixel_iterator_base( const iterator_base& i, const filter_type& filter ) :
         iterator_base( i ), m_filter( filter ), m_begin( iterator_base::m_iterator )
      {
         JumpToNextValidSample();
      }

      filter_pixel_iterator_base( const filter_pixel_iterator_base& ) = default;

      filter_pixel_iterator_base& operator =( const filter_pixel_iterator_base& ) = default;

      filter_pixel_iterator_base& operator =( const iterator_base& i )
      {
         (void)iterator_base::operator =( i );
         JumpToNextValidSample();
      }

      void JumpToNextValidSample()
      {
         while ( this->m_iterator[0] < this->m_end && !this->m_filter( this->m_iterator ) )
            for ( int i = 0; i < this->m_iterator.Length(); ++i )
               ++this->m_iterator[i];
      }

      void JumpToPrevValidSample()
      {
         while ( this->m_iterator[0] > this->m_begin && !this->m_filter( this->m_iterator ) )
            for ( int i = 0; i < this->m_iterator.Length(); ++i )
               --this->m_iterator[i];
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::GenericImage::filter_pixel_iterator
    * \brief Mutable filter pixel iterator.
    *
    * A mutable pixel iterator provides read/write, random access to all pixels
    * in an image. A filter pixel iterator uses a predicate object to filter
    * pixels within its iteration range.
    *
    * <b>Filter Predicates</b>
    *
    * For a mutable filter pixel iterator, the class template argument F must
    * provide a function call operator of the form:
    *
    * <tt>bool F::operator()( const GenericVector\<sample*\>& f ) const</tt>
    *
    * Each component of the passed vector \a f is a pointer to a pixel sample
    * in the corresponding channel of the iterated image. The function call
    * operator must return true if the pointed pixel is valid in the context of
    * the filter iterator. The filter iterator stores a private instance of F
    * to validate pixels, so the F class must provide also copy constructor
    * semantics, either explicitly or implicitly.
    *
    * <b>Filter Iterators</b>
    *
    * Each time a filter iterator is constructed, incremented, decremented, or
    * moved forward or backward, the filter predicate is called to validate the
    * pixel or pixel sample pointed to by the iterator. If the pointed item is
    * not valid, the iterator is incremented or decremented (depending on the
    * operation performed initially) until it finds a valid one, or until it
    * reaches the end of the iteration range, whichever happens first. In this
    * way a filter iterator gives access only to valid items in a completely
    * automatic and transparent fashion.
    *
    * \ingroup image_iterators
    */
   template <class F>
   class filter_pixel_iterator :
      public filter_pixel_iterator_base<GenericImage<P>, pixel_iterator, sample*, F>
   {
   public:

      /*!
       * Represents the type of the iterated image.
       */
      typedef GenericImage<P>                   image_type;

      /*!
       * Represents the pixel traits class used by the iterated image.
       */
      typedef typename image_type::pixel_traits pixel_traits;

      /*!
       * Represents a pixel sample of the iterated image.
       */
      typedef typename image_type::sample       sample;

      /*!
       * Represents the type of the unary predicate used by this filter
       * iterator.
       */
      typedef F                                 filter_type;

      typedef filter_pixel_iterator_base<GenericImage<P>, pixel_iterator, sample*, F>
                                                iterator_base;

      /*!
       * Default constructor. Initializes an invalid iterator.
       */
      filter_pixel_iterator() :
         iterator_base()
      {
      }

      /*!
       * Constructs a mutable filter pixel iterator.
       *
       * \param image   The image to iterate.
       *
       * \param filter  Reference to a predicate object that will be used to
       *                filter pixels.
       */
      filter_pixel_iterator( image_type& image, const F& filter ) :
         iterator_base( image.EnsureUnique(), filter )
      {
      }

      /*!
       * Constructs a mutable filter pixel iterator from a mutable pixel
       * iterator and the specified \a filter.
       */
      filter_pixel_iterator( const pixel_iterator& i, const F& filter ) :
         iterator_base( i, filter )
      {
      }

      /*!
       * Copy constructor.
       */
      filter_pixel_iterator( const filter_pixel_iterator& ) = default;

      /*!
       * Copy assignment operator. Returns a reference to this iterator.
       */
      filter_pixel_iterator& operator =( const filter_pixel_iterator& ) = default;

      /*!
       * Assigns a mutable pixel iterator to this object. Returns a reference
       * to this iterator.
       */
      filter_pixel_iterator& operator =( const pixel_iterator& i )
      {
         (void)iterator_base::operator =( i );
         return *this;
      }

      /*!
       * Returns true only if this iterator is valid. A valid iterator defines
       * a valid iterated image and a non-null position.
       */
      bool IsValid() const
      {
         return this->m_image != nullptr && !this->m_iterator.IsEmpty();
      }

      /*!
       * Returns a reference to the image being iterated by this object.
       */
      image_type& Image() const
      {
         return *this->m_image;
      }

      /*!
       * Returns a reference to the immutable predicate object used by this
       * filter iterator.
       */
      const filter_type& Filter() const
      {
         return this->m_filter;
      }

      /*!
       * Returns a reference to the mutable predicate object used by this
       * filter iterator.
       */
      filter_type& Filter()
      {
         return this->m_filter;
      }

      /*!
       * Returns a pointer to the pixel sample pointed to by this iterator in
       * the specified \a channel.
       */
      sample* Position( int channel ) const
      {
         return this->m_iterator[channel];
      }

      /*!
       * Boolean type conversion operator. Returns true if this iterator is
       * active. A pixel iterator is active if it has not reached (or
       * surpassed) its iteration limit.
       */
      operator bool() const
      {
         return this->m_iterator[0] < this->m_end;
      }

      /*!
       * Array subscript operator. Returns a reference to the pixel sample
       * pointed to by this iterator in the specified \a channel.
       */
      sample& operator []( int channel ) const
      {
         return *this->m_iterator[channel];
      }

      /*!
       * Pre-increment operator. Causes this iterator to point to the next
       * valid pixel in the iterated image. Returns a reference to this
       * iterator.
       */
      filter_pixel_iterator& operator ++()
      {
         for ( int i = 0; i < this->m_iterator.Length(); ++i )
            ++this->m_iterator[i];
         this->JumpToNextValidSample();
         return *this;
      }

      /*!
       * Post-increment operator. Causes this iterator to point to the next
       * valid pixel in the iterated image channel. Returns a copy of the
       * iterator as it was before incrementing it.
       */
      filter_pixel_iterator operator ++( int )
      {
         filter_pixel_iterator i0( *this );
         for ( int i = 0; i < this->m_iterator.Length(); ++i )
            ++this->m_iterator[i];
         this->JumpToNextValidSample();
         return i0;
      }

      /*!
       * Pre-decrement operator. Causes this iterator to point to the previous
       * valid pixel in the iterated image channel, then returns a reference to
       * this iterator.
       */
      filter_pixel_iterator& operator --()
      {
         for ( int i = 0; i < this->m_iterator.Length(); ++i )
            --this->m_iterator[i];
         this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Post-decrement operator. Causes this iterator to point to the previous
       * valid pixel in the iterated image channel. Returns a copy of the
       * iterator as it was before decrementing it.
       */
      filter_pixel_iterator operator --( int )
      {
         filter_pixel_iterator i0( *this );
         for ( int i = 0; i < this->m_iterator.Length(); ++i )
            --this->m_iterator[i];
         this->JumpToPrevValidSample();
         return i0;
      }

      /*!
       * Scalar assignment/addition operator. Increments this iterator by a
       * distance \a delta from its current position. Positive increments cause
       * this iterator to move forward by \a delta pixel samples. Negative
       * increments move this iterator backward by \a delta pixel samples.
       * Returns a reference to this iterator.
       */
      filter_pixel_iterator& operator +=( distance_type delta )
      {
         for ( int i = 0; i < this->m_iterator.Length(); ++i )
            this->m_iterator[i] += delta;
         this->JumpToNextValidSample();
         return *this;
      }

      /*!
       * Scalar assignment/subtraction operator. Decrements this iterator by a
       * distance \a delta from its current position. Positive increments cause
       * this iterator to move backward by \a delta pixel samples. Negative
       * increments move this iterator forward by \a delta pixel samples.
       * Returns a reference to this iterator.
       */
      filter_pixel_iterator& operator -=( distance_type delta )
      {
         for ( int i = 0; i < this->m_iterator.Length(); ++i )
            this->m_iterator[i] -= delta;
         this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Moves this iterator on the iterated image by the specified horizontal
       * and vertical increments in pixels, \a dx and \a dy respectively,
       * relative to its current position. Positive (negative) \a dx increments
       * move the iterator rightwards (leftwards). Positive (negative) \a dy
       * increments move the iterator downwards (upwards).
       */
      filter_pixel_iterator& MoveBy( int dx, int dy )
      {
         distance_type d = distance_type( dy )*this->m_image->Width() + distance_type( dx );
         for ( int i = 0; i < this->m_iterator.Length(); ++i )
            this->m_iterator[i] += d;
         if ( d >= 0 )
            this->JumpToNextValidSample();
         else
            this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to the specified iterator \a i incremented by a distance \a delta.
       */
      friend filter_pixel_iterator operator +( const filter_pixel_iterator& i, distance_type delta )
      {
         filter_pixel_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Iterator-to-scalar addition operator. This operator implements the
       * commutative property of scalar-to-iterator addition.
       */
      friend filter_pixel_iterator operator +( distance_type delta, const filter_pixel_iterator& i )
      {
         filter_pixel_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * the specified iterator \a i decremented by a distance \a delta.
       */
      friend filter_pixel_iterator operator -( const filter_pixel_iterator& i, distance_type delta )
      {
         filter_pixel_iterator j( i );
         j -= delta;
         return j;
      }

      /*!
       * Equality operator. Returns true if two iterators \a i and \a j point
       * to the same pixel.
       */
      friend bool operator ==( const filter_pixel_iterator& i, const filter_pixel_iterator& j )
      {
         return i.m_iterator[0] == j.m_iterator[0];
      }

      /*!
       * Less than operator. Returns true if the specified iterator \a i
       * precedes another iterator \a j.
       */
      friend bool operator <( const filter_pixel_iterator& i, const filter_pixel_iterator& j )
      {
         return i.m_iterator[0] < j.m_iterator[0];
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::GenericImage::const_filter_pixel_iterator
    * \brief Immutable filter pixel iterator.
    *
    * An immutable pixel iterator provides read-only, random access to all
    * pixels in an image. A filter pixel iterator uses a predicate object to
    * filter pixels within its iteration range.
    *
    * <b>Filter Predicates</b>
    *
    * For an immutable filter pixel iterator, the class template argument F
    * must provide a function call operator of the form:
    *
    * <tt>bool F::operator()( const GenericVector\<const sample*\>& f ) const</tt>
    *
    * Each component of the passed vector \a f is a pointer to a constant pixel
    * sample in the corresponding channel of the iterated image. The function
    * call operator must return true if the pointed pixel is valid in the
    * context of the filter iterator. The filter iterator stores a private
    * instance of F to validate pixels, so the F class must provide also copy
    * constructor semantics, either explicitly or implicitly.
    *
    * <b>Filter Iterators</b>
    *
    * Each time a filter iterator is constructed, incremented, decremented, or
    * moved forward or backward, the filter predicate is called to validate the
    * pixel or pixel sample pointed to by the iterator. If the pointed item is
    * not valid, the iterator is incremented or decremented (depending on the
    * operation performed initially) until it finds a valid one, or until it
    * reaches the end of the iteration range, whichever happens first. In this
    * way a filter iterator gives access only to valid items in a completely
    * automatic and transparent fashion.
    *
    * \ingroup image_iterators
    */
   template <class F>
   class const_filter_pixel_iterator :
      public filter_pixel_iterator_base<const GenericImage<P>, const_pixel_iterator, const sample*, F>
   {
   public:

      /*!
       * Represents the type of the iterated image.
       */
      typedef GenericImage<P>                   image_type;

      /*!
       * Represents the pixel traits class used by the iterated image.
       */
      typedef typename image_type::pixel_traits pixel_traits;

      /*!
       * Represents a pixel sample of the iterated image.
       */
      typedef typename image_type::sample       sample;

      /*!
       * Represents the type of the unary predicate used by this filter
       * iterator.
       */
      typedef F                                 filter_type;

      typedef filter_pixel_iterator_base<const GenericImage<P>, const_pixel_iterator, const sample*, F>
                                                iterator_base;

      /*!
       * Default constructor. Initializes an invalid iterator.
       */
      const_filter_pixel_iterator() :
         iterator_base()
      {
      }

      /*!
       * Constructs an immutable filter pixel iterator.
       *
       * \param image   The constant image to iterate.
       *
       * \param filter  Reference to a predicate object that will be used to
       *                filter pixels.
       */
      const_filter_pixel_iterator( const image_type& image, const F& filter ) :
         iterator_base( image, filter )
      {
      }

      /*!
       * Constructs an immutable filter pixel iterator from an immutable pixel
       * iterator and the specified \a filter.
       */
      const_filter_pixel_iterator( const const_pixel_iterator& i, const F& filter ) :
         iterator_base( i, filter )
      {
      }

      /*!
       * Copy constructor.
       */
      const_filter_pixel_iterator( const const_filter_pixel_iterator& ) = default;

      /*!
       * Copy assignment operator. Returns a reference to this iterator.
       */
      const_filter_pixel_iterator& operator =( const const_filter_pixel_iterator& ) = default;

      /*!
       * Assigns an immutable pixel iterator to this object. Returns a
       * reference to this iterator.
       */
      const_filter_pixel_iterator& operator =( const const_pixel_iterator& i )
      {
         (void)iterator_base::operator =( i );
         return *this;
      }

      /*!
       * Returns true only if this iterator is valid. A valid iterator defines
       * a valid iterated image and a non-null position.
       */
      bool IsValid() const
      {
         return this->m_image != nullptr && !this->m_iterator.IsEmpty();
      }

      /*!
       * Returns a reference to the constant image being iterated by this
       * object.
       */
      const image_type& Image() const
      {
         return *this->m_image;
      }

      /*!
       * Returns a reference to the immutable predicate object used by this
       * filter iterator.
       */
      const filter_type& Filter() const
      {
         return this->m_filter;
      }

      /*!
       * Returns a reference to the mutable predicate object used by this
       * filter iterator.
       */
      filter_type& Filter()
      {
         return this->m_filter;
      }

      /*!
       * Returns a pointer to the constant pixel sample pointed to by this
       * iterator in the specified \a channel.
       */
      const sample* Position( int channel ) const
      {
         return this->m_iterator[channel];
      }

      /*!
       * Boolean type conversion operator. Returns true if this iterator is
       * active. A pixel iterator is active if it has not reached (or
       * surpassed) its iteration limit.
       */
      operator bool() const
      {
         return this->m_iterator[0] < this->m_end;
      }

      /*!
       * Array subscript operator. Returns a reference to the constant pixel
       * sample pointed to by this iterator in the specified \a channel.
       */
      const sample& operator []( int channel ) const
      {
         return *this->m_iterator[channel];
      }

      /*!
       * Pre-increment operator. Causes this iterator to point to the next
       * valid pixel in the iterated image. Returns a reference to this
       * iterator.
       */
      const_filter_pixel_iterator& operator ++()
      {
         for ( int i = 0; i < this->m_iterator.Length(); ++i )
            ++this->m_iterator[i];
         this->JumpToNextValidSample();
         return *this;
      }

      /*!
       * Post-increment operator. Causes this iterator to point to the next
       * valid pixel in the iterated image channel. Returns a copy of the
       * iterator as it was before incrementing it.
       */
      const_filter_pixel_iterator operator ++( int )
      {
         const_filter_pixel_iterator i0( *this );
         for ( int i = 0; i < this->m_iterator.Length(); ++i )
            ++this->m_iterator[i];
         this->JumpToNextValidSample();
         return i0;
      }

      /*!
       * Pre-decrement operator. Causes this iterator to point to the previous
       * valid pixel in the iterated image channel, then returns a reference to
       * this iterator.
       */
      const_filter_pixel_iterator& operator --()
      {
         for ( int i = 0; i < this->m_iterator.Length(); ++i )
            --this->m_iterator[i];
         this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Post-decrement operator. Causes this iterator to point to the previous
       * valid pixel in the iterated image channel. Returns a copy of the
       * iterator as it was before decrementing it.
       */
      const_filter_pixel_iterator operator --( int )
      {
         const_filter_pixel_iterator i0( *this );
         for ( int i = 0; i < this->m_iterator.Length(); ++i )
            --this->m_iterator[i];
         this->JumpToPrevValidSample();
         return i0;
      }

      /*!
       * Scalar assignment/addition operator. Increments this iterator by a
       * distance \a delta from its current position. Positive increments cause
       * this iterator to move forward by \a delta pixel samples. Negative
       * increments move this iterator backward by \a delta pixel samples.
       * Returns a reference to this iterator.
       */
      const_filter_pixel_iterator& operator +=( distance_type delta )
      {
         for ( int i = 0; i < this->m_iterator.Length(); ++i )
            this->m_iterator[i] += delta;
         this->JumpToNextValidSample();
         return *this;
      }

      /*!
       * Scalar assignment/subtraction operator. Decrements this iterator by a
       * distance \a delta from its current position. Positive increments cause
       * this iterator to move backward by \a delta pixel samples. Negative
       * increments move this iterator forward by \a delta pixel samples.
       * Returns a reference to this iterator.
       */
      const_filter_pixel_iterator& operator -=( distance_type delta )
      {
         for ( int i = 0; i < this->m_iterator.Length(); ++i )
            this->m_iterator[i] -= delta;
         this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Moves this iterator on the iterated image by the specified horizontal
       * and vertical increments in pixels, \a dx and \a dy respectively,
       * relative to its current position. Positive (negative) \a dx increments
       * move the iterator rightwards (leftwards). Positive (negative) \a dy
       * increments move the iterator downwards (upwards).
       */
      const_filter_pixel_iterator& MoveBy( int dx, int dy )
      {
         distance_type d = distance_type( dy )*this->m_image->Width() + distance_type( dx );
         for ( int i = 0; i < this->m_iterator.Length(); ++i )
            this->m_iterator[i] += d;
         if ( d >= 0 )
            this->JumpToNextValidSample();
         else
            this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to the specified iterator \a i incremented by a distance \a delta.
       */
      friend const_filter_pixel_iterator operator +( const const_filter_pixel_iterator& i, distance_type delta )
      {
         const_filter_pixel_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Iterator-to-scalar addition operator. This operator implements the
       * commutative property of scalar-to-iterator addition.
       */
      friend const_filter_pixel_iterator operator +( distance_type delta, const const_filter_pixel_iterator& i )
      {
         const_filter_pixel_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * the specified iterator \a i decremented by a distance \a delta.
       */
      friend const_filter_pixel_iterator operator -( const const_filter_pixel_iterator& i, distance_type delta )
      {
         const_filter_pixel_iterator j( i );
         j -= delta;
         return j;
      }

      /*!
       * Equality operator. Returns true if two iterators \a i and \a j point
       * to the same pixel.
       */
      friend bool operator ==( const const_filter_pixel_iterator& i, const const_filter_pixel_iterator& j )
      {
         return i.m_iterator[0] == j.m_iterator[0];
      }

      /*!
       * Less than operator. Returns true if the specified iterator \a i
       * precedes another iterator \a j.
       */
      friend bool operator <( const const_filter_pixel_iterator& i, const const_filter_pixel_iterator& j )
      {
         return i.m_iterator[0] < j.m_iterator[0];
      }
   };

   // -------------------------------------------------------------------------

   template <class image_type, class sample_pointer, class filter_type>
   class roi_filter_pixel_iterator_base : public roi_pixel_iterator_base<image_type, sample_pointer>
   {
   protected:

      typedef roi_pixel_iterator_base<image_type, sample_pointer>
                                                roi_iterator_base;

      filter_type    m_filter;
      sample_pointer m_begin;

      roi_filter_pixel_iterator_base() :
         roi_iterator_base(), m_filter(), m_begin( nullptr )
      {
      }

      roi_filter_pixel_iterator_base( image_type& image, const filter_type& filter, const Rect& rect ) :
         roi_iterator_base( image, rect ), m_filter( filter ), m_begin( roi_iterator_base::m_iterator )
      {
         JumpToNextValidSample();
      }

      roi_filter_pixel_iterator_base( const roi_iterator_base& i, const filter_type& filter ) :
         roi_iterator_base( i ), m_filter( filter ), m_begin( roi_iterator_base::m_iterator )
      {
         JumpToNextValidSample();
      }

      roi_filter_pixel_iterator_base( const roi_filter_pixel_iterator_base& ) = default;

      roi_filter_pixel_iterator_base& operator =( const roi_filter_pixel_iterator_base& ) = default;

      roi_filter_pixel_iterator_base& operator =( const roi_iterator_base& i )
      {
         (void)roi_iterator_base::operator =( i );
         JumpToNextValidSample();
         return *this;
      }

      void JumpToNextValidSample()
      {
         while ( this->m_iterator[0] < this->m_end && !this->m_filter( this->m_iterator ) )
            roi_iterator_base::Increment();
      }

      void JumpToPrevValidSample()
      {
         while ( this->m_iterator[0] > this->m_begin && !this->m_filter( this->m_iterator ) )
            roi_iterator_base::Decrement();
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::GenericImage::roi_filter_pixel_iterator
    * \brief Mutable region-of-interest, filter pixel iterator.
    *
    * A mutable, region-of-interest (ROI), filter pixel iterator combines the
    * capabilities of roi_pixel_iterator and filter_pixel_iterator in a single
    * iterator class.
    *
    * \ingroup image_iterators
    */
   template <class F>
   class roi_filter_pixel_iterator : public roi_filter_pixel_iterator_base<GenericImage<P>, sample*, F>
   {
   public:

      /*!
       * Represents the type of the iterated image.
       */
      typedef GenericImage<P>                   image_type;

      /*!
       * Represents the pixel traits class used by the iterated image.
       */
      typedef typename image_type::pixel_traits pixel_traits;

      /*!
       * Represents a pixel sample of the iterated image.
       */
      typedef typename image_type::sample       sample;

      /*!
       * Represents the type of the unary predicate used by this filter
       * iterator.
       */
      typedef F                                 filter_type;

      typedef roi_filter_pixel_iterator_base<GenericImage<P>, sample*, F>
                                                iterator_base;

      /*!
       * Default constructor. Initializes an invalid iterator.
       */
      roi_filter_pixel_iterator() :
         iterator_base()
      {
      }

      /*!
       * Constructs a mutable, region-of-interest (ROI), filter pixel iterator.
       *
       * \param image   The image to iterate.
       *
       * \param filter  Reference to a predicate object that will be used to
       *                filter pixel sample values.
       *
       * \param rect    Region of interest. If an empty rectangle is specified,
       *                the current rectangular selection in the \a image will
       *                be used. If the specified rectangle is not empty and
       *                extends beyond image boundaries, only the intersection
       *                with the image will be used. If that intersection does
       *                not exist, then the resulting iterator will be invalid,
       *                with an empty iteration range. The default value is an
       *                empty rectangle.
       */
      roi_filter_pixel_iterator( image_type& image, const F& filter, const Rect& rect = Rect( 0 ) ) :
         iterator_base( image.EnsureUnique(), filter, rect )
      {
      }

      /*!
       * Constructs a mutable, region-of-interest (ROI), filter pixel iterator
       * from the specified ROI pixel iterator \a i and \a filter.
       */
      roi_filter_pixel_iterator( const roi_pixel_iterator& i, const F& filter ) :
         iterator_base( i, filter )
      {
      }

      /*!
       * Copy constructor.
       */
      roi_filter_pixel_iterator( const roi_filter_pixel_iterator& ) = default;

      /*!
       * Copy assignment operator. Returns a reference to this object.
       */
      roi_filter_pixel_iterator& operator =( const roi_filter_pixel_iterator& ) = default;

      /*!
       * Assigns a ROI pixel iterator. Returns a reference to this object.
       */
      roi_filter_pixel_iterator& operator =( const roi_pixel_iterator& i )
      {
         (void)iterator_base::operator =( i );
         return *this;
      }

      /*!
       * Returns true only if this iterator is valid. A valid iterator defines
       * a valid iterated image and a non-null position.
       */
      bool IsValid() const
      {
         return this->m_image != nullptr && !this->m_iterator.IsEmpty();
      }

      /*!
       * Returns a reference to the image being iterated by this object.
       */
      image_type& Image() const
      {
         return *this->m_image;
      }

      /*!
       * Returns a reference to the immutable predicate object used by this
       * filter iterator.
       */
      const filter_type& Filter() const
      {
         return this->m_filter;
      }

      /*!
       * Returns a reference to the mutable predicate object used by this
       * filter iterator.
       */
      filter_type& Filter()
      {
         return this->m_filter;
      }

      /*!
       * Returns a pointer to the pixel sample pointed to by this iterator in
       * the specified \a channel.
       */
      sample* Position( int channel ) const
      {
         return this->m_iterator[channel];
      }

      /*!
       * Boolean type conversion operator. Returns true if this iterator is
       * active. A ROI pixel iterator is active if it has not reached (or
       * surpassed) its iteration limit, i.e. the bottom right corner of its
       * iterated region of interest.
       */
      operator bool() const
      {
         return this->m_iterator[0] < this->m_end;
      }

      /*!
       * Array subscript operator. Returns a reference to the pixel sample
       * pointed to by this iterator in the specified \a channel.
       */
      sample& operator []( int channel ) const
      {
         return *this->m_iterator[channel];
      }

      /*!
       * Pre-increment operator. Causes this iterator to point to the next
       * valid pixel in the iterated region of interest. Returns a reference to
       * this iterator.
       */
      roi_filter_pixel_iterator& operator ++()
      {
         this->Increment();
         this->JumpToNextValidSample();
         return *this;
      }

      /*!
       * Post-increment operator. Causes this iterator to point to the next
       * valid pixel in the iterated region of interest. Returns a copy of the
       * iterator as it was before incrementing it.
       */
      roi_filter_pixel_iterator operator ++( int )
      {
         roi_filter_pixel_iterator i0( *this );
         this->Increment();
         this->JumpToNextValidSample();
         return i0;
      }

      /*!
       * Pre-decrement operator. Causes this iterator to point to the previous
       * valid pixel in the iterated region of interest, then returns a
       * reference to this iterator.
       */
      roi_filter_pixel_iterator& operator --()
      {
         this->Decrement();
         this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Post-decrement operator. Causes this iterator to point to the previous
       * valid pixel in the iterated region of interest. Returns a copy of the
       * iterator as it was before decrementing it.
       */
      roi_filter_pixel_iterator operator --( int )
      {
         roi_filter_pixel_iterator i0( *this );
         this->Decrement();
         this->JumpToPrevValidSample();
         return i0;
      }

      /*!
       * Scalar assignment/addition operator. Increments this iterator by a
       * distance \a delta from its current position, within the rectangular
       * region of interest being iterated. Positive increments cause this
       * iterator to move forward (rightwards and downwards) by \a delta pixel
       * samples. Negative increments move this iterator backward (leftwards
       * and upwards) by \a delta pixel samples. Returns a reference to this
       * iterator.
       */
      roi_filter_pixel_iterator& operator +=( distance_type delta )
      {
         int w = this->m_rowEnd - this->m_rowBegin;
         return MoveBy( delta%w, delta/w );
      }

      /*!
       * Scalar assignment/subtraction operator. Decrements this iterator by a
       * distance \a delta from its current position, within the rectangular
       * region of interest being iterated. Positive increments cause this
       * iterator to move backward (leftwards and upwards) by \a delta pixel
       * samples. Negative increments move this iterator forward (rightwards
       * and downwards) by \a delta pixel samples. Returns a reference to this
       * iterator.
       */
      roi_filter_pixel_iterator& operator -=( distance_type delta )
      {
         int w = this->m_rowEnd - this->m_rowBegin;
         return MoveBy( -delta%w, -delta/w );
      }

      /*!
       * Moves this iterator within its rectangular region of interest by the
       * specified horizontal and vertical increments in pixels, \a dx and
       * \a dy respectively, relative to its current position. Positive
       * (negative) \a dx increments move the iterator rightwards (leftwards).
       * Positive (negative) \a dy increments move the iterator downwards
       * (upwards).
       */
      roi_filter_pixel_iterator& MoveBy( int dx, int dy )
      {
         sample* i0 = this->m_iterator[0];
         iterator_base::MoveBy( dx, dy );
         if ( this->m_iterator[0] >= i0 )
            this->JumpToNextValidSample();
         else
            this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to the specified iterator \a i incremented by a distance \a delta.
       */
      friend roi_filter_pixel_iterator operator +( const roi_filter_pixel_iterator& i, distance_type delta )
      {
         roi_filter_pixel_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Iterator-to-scalar addition operator. This operator implements the
       * commutative property of scalar-to-iterator addition.
       */
      friend roi_filter_pixel_iterator operator +( distance_type delta, const roi_filter_pixel_iterator& i )
      {
         roi_filter_pixel_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * the specified iterator \a i decremented by a distance \a delta.
       */
      friend roi_filter_pixel_iterator operator -( const roi_filter_pixel_iterator& i, distance_type delta )
      {
         roi_filter_pixel_iterator j( i );
         j -= delta;
         return j;
      }

      /*!
       * Equality operator. Returns true if two iterators \a i and \a j point
       * to the same pixel.
       */
      friend bool operator ==( const roi_filter_pixel_iterator& i, const roi_filter_pixel_iterator& j )
      {
         return i.m_iterator[0] == j.m_iterator[0];
      }

      /*!
       * Less than operator. Returns true if the specified iterator \a i
       * precedes another iterator \a j.
       */
      friend bool operator <( const roi_filter_pixel_iterator& i, const roi_filter_pixel_iterator& j )
      {
         return i.m_iterator[0] < j.m_iterator[0];
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * \class pcl::GenericImage::const_roi_filter_pixel_iterator
    * \brief Immutable region-of-interest, filter pixel iterator.
    *
    * An immutable, region-of-interest (ROI), filter pixel iterator combines
    * the capabilities of const_roi_pixel_iterator and
    * const_filter_pixel_iterator in a single iterator class.
    *
    * \ingroup image_iterators
    */
   template <class F>
   class const_roi_filter_pixel_iterator : public roi_filter_pixel_iterator_base<const GenericImage<P>, const sample*, F>
   {
   public:

      /*!
       * Represents the type of the iterated image.
       */
      typedef GenericImage<P>                   image_type;

      /*!
       * Represents the pixel traits class used by the iterated image.
       */
      typedef typename image_type::pixel_traits pixel_traits;

      /*!
       * Represents a pixel sample of the iterated image.
       */
      typedef typename image_type::sample       sample;

      /*!
       * Represents the type of the unary predicate used by this filter
       * iterator.
       */
      typedef F                                 filter_type;

      typedef roi_filter_pixel_iterator_base<const GenericImage<P>, const sample*, F>
                                                iterator_base;

      /*!
       * Default constructor. Initializes an invalid iterator.
       */
      const_roi_filter_pixel_iterator() :
         iterator_base()
      {
      }

      /*!
       * Constructs an immutable, region-of-interest (ROI), filter pixel
       * iterator.
       *
       * \param image   The constant image to iterate.
       *
       * \param filter  Reference to a predicate object that will be used to
       *                filter pixel sample values.
       *
       * \param rect    Region of interest. If an empty rectangle is specified,
       *                the current rectangular selection in the \a image will
       *                be used. If the specified rectangle is not empty and
       *                extends beyond image boundaries, only the intersection
       *                with the image will be used. If that intersection does
       *                not exist, then the resulting iterator will be invalid,
       *                with an empty iteration range. The default value is an
       *                empty rectangle.
       */
      const_roi_filter_pixel_iterator( const image_type& image, const F& filter, const Rect& rect = Rect( 0 ) ) :
         iterator_base( image, filter, rect )
      {
      }

      /*!
       * Constructs an immutable, region-of-interest (ROI), filter pixel
       * iterator from the specified ROI pixel iterator \a i and \a filter.
       */
      const_roi_filter_pixel_iterator( const const_roi_pixel_iterator& i, const F& filter ) :
         iterator_base( i, filter )
      {
      }

      /*!
       * Copy constructor.
       */
      const_roi_filter_pixel_iterator( const const_roi_filter_pixel_iterator& ) = default;


      /*!
       * Copy assignment operator. Returns a reference to this object.
       */
      const_roi_filter_pixel_iterator& operator =( const const_roi_filter_pixel_iterator& ) = default;

      /*!
       * Assigns a ROI pixel iterator. Returns a reference to this object.
       */
      const_roi_filter_pixel_iterator& operator =( const const_roi_pixel_iterator& i )
      {
         (void)iterator_base::operator =( i );
         return *this;
      }

      /*!
       * Returns true only if this iterator is valid. A valid iterator defines
       * a valid iterated image and a non-null position.
       */
      bool IsValid() const
      {
         return this->m_image != nullptr && !this->m_iterator.IsEmpty();
      }

      /*!
       * Returns a reference to the constant image being iterated by this
       * object.
       */
      const image_type& Image() const
      {
         return *this->m_image;
      }

      /*!
       * Returns a reference to the immutable predicate object used by this
       * filter iterator.
       */
      const filter_type& Filter() const
      {
         return this->m_filter;
      }

      /*!
       * Returns a reference to the mutable predicate object used by this
       * filter iterator.
       */
      filter_type& Filter()
      {
         return this->m_filter;
      }

      /*!
       * Returns a pointer to the constant pixel sample pointed to by this
       * iterator in the specified \a channel.
       */
      const sample* Position( int channel ) const
      {
         return this->m_iterator[channel];
      }

      /*!
       * Boolean type conversion operator. Returns true if this iterator is
       * active. A ROI pixel iterator is active if it has not reached (or
       * surpassed) its iteration limit, i.e. the bottom right corner of its
       * iterated region of interest.
       */
      operator bool() const
      {
         return this->m_iterator[0] < this->m_end;
      }

      /*!
       * Array subscript operator. Returns a reference to the constant pixel
       * sample pointed to by this iterator in the specified \a channel.
       */
      const sample& operator []( int channel ) const
      {
         return *this->m_iterator[channel];
      }

      /*!
       * Pre-increment operator. Causes this iterator to point to the next
       * valid pixel in the iterated region of interest. Returns a reference to
       * this iterator.
       */
      const_roi_filter_pixel_iterator& operator ++()
      {
         this->Increment();
         this->JumpToNextValidSample();
         return *this;
      }

      /*!
       * Post-increment operator. Causes this iterator to point to the next
       * valid pixel in the iterated region of interest. Returns a copy of the
       * iterator as it was before incrementing it.
       */
      const_roi_filter_pixel_iterator operator ++( int )
      {
         const_roi_filter_pixel_iterator i0( *this );
         this->Increment();
         this->JumpToNextValidSample();
         return i0;
      }

      /*!
       * Pre-decrement operator. Causes this iterator to point to the previous
       * valid pixel in the iterated region of interest, then returns a
       * reference to this iterator.
       */
      const_roi_filter_pixel_iterator& operator --()
      {
         this->Decrement();
         this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Post-decrement operator. Causes this iterator to point to the previous
       * valid pixel in the iterated region of interest. Returns a copy of the
       * iterator as it was before decrementing it.
       */
      const_roi_filter_pixel_iterator operator --( int )
      {
         const_roi_filter_pixel_iterator i0( *this );
         this->Decrement();
         this->JumpToPrevValidSample();
         return i0;
      }

      /*!
       * Scalar assignment/addition operator. Increments this iterator by a
       * distance \a delta from its current position, within the rectangular
       * region of interest being iterated. Positive increments cause this
       * iterator to move forward (rightwards and downwards) by \a delta pixel
       * samples. Negative increments move this iterator backward (leftwards
       * and upwards) by \a delta pixel samples. Returns a reference to this
       * iterator.
       */
      const_roi_filter_pixel_iterator& operator +=( distance_type delta )
      {
         int w = this->m_rowEnd - this->m_rowBegin;
         return MoveBy( delta%w, delta/w );
      }

      /*!
       * Scalar assignment/subtraction operator. Decrements this iterator by a
       * distance \a delta from its current position, within the rectangular
       * region of interest being iterated. Positive increments cause this
       * iterator to move backward (leftwards and upwards) by \a delta pixel
       * samples. Negative increments move this iterator forward (rightwards
       * and downwards) by \a delta pixel samples. Returns a reference to this
       * iterator.
       */
      const_roi_filter_pixel_iterator& operator -=( distance_type delta )
      {
         int w = this->m_rowEnd - this->m_rowBegin;
         return MoveBy( -delta%w, -delta/w );
      }

      /*!
       * Moves this iterator within its rectangular region of interest by the
       * specified horizontal and vertical increments in pixels, \a dx and
       * \a dy respectively, relative to its current position. Positive
       * (negative) \a dx increments move the iterator rightwards (leftwards).
       * Positive (negative) \a dy increments move the iterator downwards
       * (upwards).
       */
      const_roi_filter_pixel_iterator& MoveBy( int dx, int dy )
      {
         const sample* i0 = this->m_iterator[0];
         iterator_base::MoveBy( dx, dy );
         if ( this->m_iterator[0] >= i0 )
            this->JumpToNextValidSample();
         else
            this->JumpToPrevValidSample();
         return *this;
      }

      /*!
       * Scalar-to-iterator addition operator. Returns an iterator equivalent
       * to the specified iterator \a i incremented by a distance \a delta.
       */
      friend const_roi_filter_pixel_iterator operator +( const const_roi_filter_pixel_iterator& i, distance_type delta )
      {
         const_roi_filter_pixel_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Iterator-to-scalar addition operator. This operator implements the
       * commutative property of scalar-to-iterator addition.
       */
      friend const_roi_filter_pixel_iterator operator +( distance_type delta, const const_roi_filter_pixel_iterator& i )
      {
         const_roi_filter_pixel_iterator j( i );
         j += delta;
         return j;
      }

      /*!
       * Scalar-to-iterator subtraction operator. Returns an iterator equal to
       * the specified iterator \a i decremented by a distance \a delta.
       */
      friend const_roi_filter_pixel_iterator operator -( const const_roi_filter_pixel_iterator& i, distance_type delta )
      {
         const_roi_filter_pixel_iterator j( i );
         j -= delta;
         return j;
      }

      /*!
       * Equality operator. Returns true if two iterators \a i and \a j point
       * to the same pixel.
       */
      friend bool operator ==( const const_roi_filter_pixel_iterator& i, const const_roi_filter_pixel_iterator& j )
      {
         return i.m_iterator[0] == j.m_iterator[0];
      }

      /*!
       * Less than operator. Returns true if the specified iterator \a i
       * precedes another iterator \a j.
       */
      friend bool operator <( const const_roi_filter_pixel_iterator& i, const const_roi_filter_pixel_iterator& j )
      {
         return i.m_iterator[0] < j.m_iterator[0];
      }
   };

   // -------------------------------------------------------------------------

   /*!
    * Returns true iff this %GenericImage template instantiation uses floating
    * point pixel samples; returns false if it uses integer samples.
    */
   static bool IsFloatSample()
   {
      return pixel_traits::IsFloatSample();
   }

   /*!
    * Returns true iff this %GenericImage template instantiation uses complex
    * pixel samples; returns false if it uses real samples.
    */
   static bool IsComplexSample()
   {
      return pixel_traits::IsComplexSample();
   }

   /*!
    * Returns the number of 8-bit bytes required to store a pixel sample in
    * this %GenericImage template instantiation.
    */
   static int BytesPerSample()
   {
      return P::BytesPerSample();
   }

   /*!
    * Returns the number of bits in a pixel sample of this %GenericImage
    * template instantiation.
    */
   static int BitsPerSample()
   {
      return P::BitsPerSample();
   }

   /*!
    * Returns true iff this object and another \a image use the same pixel
    * sample type:
    *
    * \li Same sample size in bits.
    * \li Both images using integer, floating point real, or complex samples.
    */
   template <class P1>
   bool SameSampleType( const GenericImage<P1>& image ) const
   {
      return image.BitsPerSample() == BitsPerSample() &&
             image.IsFloatSample() == IsFloatSample() &&
             image.IsComplexSample() == IsComplexSample();
   }

   /*!
    * Always returns true, indicating that this object and the specified
    * \a image, both of the same template instantiation, use the same pixel
    * sample type.
    */
   bool SameSampleType( const GenericImage& image ) const
   {
      return true;
   }

   // -------------------------------------------------------------------------

   /*!
    * Constructs a default local image. This object will be initialized as an
    * empty grayscale image with local storage.
    */
   GenericImage()
   {
      m_data = new Data( this );
   }

   /*!
    * Copy constructor. Constructs a local image to store a copy of the
    * currently selected pixel samples in a local or shared image. The behavior
    * of this constructor depends on the current selections and storage type of
    * the specified source \a image:
    *
    * \li If the source object is a shared image, then this constructor will
    * create a duplicate of its current selection as a local image.
    *
    * \li If the source object is a <em>completely selected</em> local image
    * (see the note below), then this object will reference the same image
    * data, and hence no duplicate of the existing pixel data will be
    * generated. This enables the copy-on-write (or implicit data sharing)
    * functionality of %GenericImage.
    *
    * \li If the source image is only partially selected, then a local image
    * will be initialized to store in this object a copy of the selected pixel
    * samples in the source image. If the selected range of channels is smaller
    * than the required number of nominal channels for the source color space,
    * then this object will be in the grayscale space. Otherwise this object
    * will inherit the color space of the source \a image.
    *
    * \note In a completely selected image, the current rectangular selection
    * includes the entire image boundaries, and the range of selected channels
    * comprises all existing channels, including all nominal and alpha
    * channels. By default, all images are completely selected. A complete
    * selection is also enabled after calling the inherited
    * ImageGeometry::ResetSelections() member function.
    */
   GenericImage( const GenericImage& image )
   {
      if ( !image.IsShared() )
         if ( image.IsCompletelySelected() )
         {
            image.m_data->Attach( this );
            m_data = image.m_data;
            m_status = image.m_status;
            ResetSelections();
            return;
         }

      m_data = new Data( this );
      (void)Assign( image );
   }

   /*!
    * Move constructor.
    */
   GenericImage( GenericImage&& image ) : AbstractImage( image ), m_data( image.m_data )
   {
      image.m_data = nullptr;
   }

   /*!
    * Constructs a local image to store a copy of the specified \a image, which
    * belongs to a different template instantiation.
    *
    * This constructor generates a uniquely referenced duplicate of the current
    * selection of pixel samples in the specified \a image, converted to the
    * sample type of this template instantiation.
    *
    * \note Don't confuse this constructor with the copy constructor for
    * %GenericImage. Note that the source \a image argument is a reference to a
    * different template instantiation of %GenericImage.
    */
   template <class P1>
   GenericImage( const GenericImage<P1>& image )
   {
      m_data = new Data( this );
      (void)Assign( image );
   }

   /*!
    * Constructs a local image to store a copy of a subset of pixel samples in
    * the specified \a image.
    *
    * \param image   Source image.
    *
    * \param rect    Source rectangular selection. If this parameter defines an
    *                empty rectangle, the current rectangular selection in the
    *                source \a image will be used. If the specified rectangle
    *                is not empty and extends beyond the boundaries of the
    *                source \a image, only the intersection with the source
    *                \a image will be used. If that intersection does not
    *                exist, this constructor will initialize an empty image.
    *
    * \param firstChannel  Zero-based index of the first channel to copy. If
    *                a negative index is specified, the first selected channel
    *                in the source \a image will be used. If the specified
    *                index is larger than or equal to the number of existing
    *                channels in the source \a image, this constructor will
    *                initialize an empty image. The default value is -1.
    *
    * \param lastChannel   Zero-based index of the last channel to copy. If
    *                a negative index is specified, the last selected channel
    *                in the source \a image will be used. If the specified
    *                index is larger than or equal to the number of existing
    *                channels in the source \a image, this constructor will
    *                initialize an empty image. The default value is -1.
    *
    * If the specified source \a image and this object use different pixel
    * sample types, this constructor will use pixel traits primitives to
    * perform all the necessary data type conversions between different pixel
    * sample types transparently.
    *
    * \note This constructor will always generate a uniquely referenced, local
    * image, irrespective of whether the \a rect, \a firstChannel and
    * \a lastChannel parameters define a complete or partial selection for the
    * specified source \a image. To make use of the implicit data sharing
    * mechanism of %GenericImage, use the copy constructor and make sure that
    * the source \a image is completely selected.
    */
   template <class P1>
   GenericImage( const GenericImage<P1>& image, const Rect& rect, int firstChannel = -1, int lastChannel = -1 )
   {
      m_data = new Data( this );
      (void)Assign( image, rect, firstChannel, lastChannel );
   }

   /*!
    * Constructs a local image with the specified dimensions and color space.
    *
    * \param width   Width in pixels of the newly created image.
    *
    * \param height  Height in pixels of the newly created image.
    *
    * \param colorSpace    Color space. See the ColorSpace namespace for
    *                symbolic constants. If this parameter is not specified, a
    *                grayscale image will be constructed.
    *
    * The number of channels in the constructed image will depend on the
    * specified color space. Grayscale images will have a single channel. In
    * general, color images will have three channels.
    *
    * \note Pixel samples in the newly constructed image are not initialized by
    * this constructor, so the image will contain unpredictable values.
    */
   GenericImage( int width, int height, color_space colorSpace = ColorSpace::Gray )
   {
      m_data = new Data( this );
      m_data->Allocate( width, height, ColorSpace::NumberOfNominalChannels( colorSpace ), colorSpace );
      ResetSelections();
   }

   /*!
    * Constructs a local image with data read from a raw-storage stream.
    *
    * \param stream  Reference to an input stream where a raw image has been
    *             previously stored (e.g. with the Write() member function).
    *
    * The input \a stream must provide sample values in the sample data type of
    * this image. The sample data type of an image is invariantly defined by
    * its template instantiation, and thus cannot be changed.
    *
    * \warning If the image stored in the input \a stream and this image use
    * different pixel sample data types, this function will throw an Error
    * exception with the appropriate error message.
    */
   explicit GenericImage( File& stream )
   {
      m_data = new Data( this );
      Read( stream );
   }

   /*!
    * Constructs a shared, client-side managed image corresponding to a
    * server-side image living in the PixInsight core application.
    *
    * \param handle  A low-level API handle to an existing image in the
    *                PixInsight core application.
    *
    * Shared images are alias objects referencing server-side images living
    * within the PixInsight core application. Shared images allocate pixel data
    * in the core application's heap, which we call <em>shared memory</em>.
    *
    * Local images exist within the user module's local heap, including all of
    * their pixel data.
    *
    * PCL implements an abstraction mechanism that allows you to handle local
    * and shared images transparently. Except in very few and special cases,
    * you don't have to know whether you're working with a shared or a local
    * image; your code will be exactly the same under both situations.
    */
   explicit GenericImage( void* handle )
   {
      m_data = new Data( this, handle );
      ResetSelections();
   }

   /*!
    * Constructs a new shared image with the specified geometry and color.
    *
    * \param width   Width in pixels of the newly created image.
    *
    * \param height  Height in pixels of the newly created image.
    *
    * \param colorSpace    Color space. See the ColorSpace namespace for
    *                symbolic constants. If this parameter is not specified, a
    *                grayscale image will be constructed.
    *
    * The first \c void* parameter is strictly formal. Its value will be
    * ignored as its sole purpose is to distinguish this constructor from the
    * corresponding local storage constructor.
    *
    * The number of channels in the constructed image will depend on the
    * specified color space. Grayscale images will have a single channel. In
    * general, color images will have three channels.
    *
    * A shared image created with this constructor can be used for intermodule
    * communication tasks, such as file I/O operations with the FileFormat and
    * FileFormatInstance classes, and execution of processes with Process and
    * ProcessInstance.
    *
    * \note Pixel samples in the newly constructed image are not initialized by
    * this constructor, so the image will contain unpredictable values.
    */
   GenericImage( void*, int width, int height, color_space colorSpace = ColorSpace::Gray )
   {
      m_data = new Data( this, width, height, ColorSpace::NumberOfNominalChannels( colorSpace ), colorSpace );
      ResetSelections();
   }

   /*!
    * Destroys a %GenericImage instance. If this is a nonempty local or shared
    * image, the image data associated with this object are dereferenced. If
    * the image data become unreferenced, they are also destroyed.
    *
    * The internal allocator object does all the necessary clean-up work for
    * both local and shared images transparently:
    *
    * \li When a local image becomes unreferenced, all the internal data
    * structures are destroyed and the memory space in use to store pixel
    * samples is released immediately in the local heap of the calling module.
    *
    * \li When a shared image becomes unreferenced, the server-side image is
    * dereferenced. When the server-side shared image becomes unreferenced, it
    * is garbage-collected and eventually destroyed by the PixInsight core
    * application.
    */
   virtual ~GenericImage()
   {
      if ( m_data != nullptr )
      {
         DetachFromData();
         m_data = nullptr;
      }
   }

   // -------------------------------------------------------------------------

   /*!
    * Returns true iff this object is a shared image. Returns false if this is
    * a local image.
    *
    * Shared images are alias objects referencing actual images living within
    * the PixInsight core application. Shared images allocate pixel data in
    * the core application's heap, which we call <em>shared memory</em>.
    *
    * Local images exist within the user module's local heap, including all of
    * their pixel data.
    *
    * PCL implements an abstraction mechanism that allows you to handle local
    * and shared images transparently. Except in very few and special cases,
    * you don't have to know whether you're working with a shared or a local
    * image; your code will be exactly the same under both situations.
    */
   bool IsShared() const
   {
      return m_data->IsShared();
   }

   /*!
    * Returns true iff this object uniquely references its pixel data. Returns
    * false if this object is sharing its pixel data with other %GenericImage
    * instances.
    *
    * \note Don't confuse uniqueness of reference with local or shared storage
    * of images. A %GenericImage can share its data with other instances living
    * in the local heap of the calling module, irrespective of whether the
    * pixel data is local or shared.
    */
   bool IsUnique() const
   {
      return m_data->IsUnique();
   }

   /*!
    * Ensures that this image uses local pixel data storage. Returns a
    * reference to this image.
    *
    * If this object is already a local image, then calling this member
    * function has no effect.
    *
    * If the image is shared, then this function creates a local duplicate,
    * dereferences the previously referenced pixel data, and references the
    * newly created local duplicate. Upon returning, this image will uniquely
    * reference its pixel data.
    */
   GenericImage& EnsureLocal()
   {
      if ( m_data->IsShared() )
      {
         GenericImage local;
         local.m_data->Allocate( m_width, m_height, m_numberOfChannels, m_colorSpace );
         local.m_RGBWS = m_RGBWS;
         local.m_selected = m_selected;
         local.m_savedSelections = m_savedSelections;
         local.m_status = m_status;
         for ( int c = 0; c < m_numberOfChannels; ++c )
            P::Copy( local.m_pixelData[c], m_pixelData[c], NumberOfPixels() );

         local.m_data->Attach( this );
         DetachFromData();
         m_data = local.m_data;
      }
      return *this;
   }

   /*!
    * Ensures that this object uniquely references its pixel data, making a
    * unique copy of its referenced pixel data if necessary. Returns a
    * reference to this image.
    *
    * If this object is not sharing its pixel data with other %GenericImage
    * instances, then calling this member function has no effect.
    *
    * If this object is sharing its pixel data with other instances, then the
    * previously referenced pixel data are dereferenced, and a newly created
    * copy of the pixel data is uniquely referenced by this image. In this case
    * this object will always be a \e local image after calling this function:
    * newly created duplicates of existing pixel data are always allocated in
    * the local heap of the calling module.
    */
   GenericImage& EnsureUnique()
   {
      if ( !m_data->IsUnique() )
      {
         Data* newData = m_data->Clone( this );
         DetachFromData();
         m_data = newData;
      }
      return *this;
   }

   /*!
    * Returns a reference to the internal pixel allocator object associated
    * with this image.
    *
    * The type of the allocator object is a template specialization of the
    * PixelAllocator class for the sample data type of this image. The
    * allocator is responsible for dynamic allocation of pixel data, including
    * <em>channel slots</em> (pointers to pixel sample blocks) and pixel sample
    * blocks (used to store channel pixel data).
    *
    * The allocator must be able to handle dynamic allocation for local and
    * shared memory transparently. This means that the allocator object knows
    * whether this is a local image (allocated in the calling module's heap) or
    * a shared image (allocated in the PixInsight core's heap), and allows you
    * to manage and allocate pixel data without distinction in both cases.
    *
    * \warning \e Never use the \c new and \c delete operators with pixel data
    * memory blocks. Always call the Allocator() member function of
    * %GenericImage to get a reference to its PixelAllocator object, then use
    * its public member functions for all allocations and deallocations of
    * pixel data.
    */
   pixel_allocator& Allocator() const
   {
      return m_allocator;
   }

   /*!
    * \internal Synchronizes this object with the server-side shared image, if
    * this is a shared client-side image. Does nothing if this is a local
    * image allocated in the calling module's heap.
    */
   void Synchronize()
   {
      m_data->SynchronizeWithSharedImage();
      ResetSelections();
   }

   /*!
    * Allocates new pixel data with the specified geometry and color space.
    * Returns a reference to this image.
    *
    * \param width               New image width in pixels.
    *
    * \param height              New image height in pixels.
    *
    * \param numberOfChannels    Number of channels. Must be greater than or
    *                equal to the number of nominal channels corresponding to
    *                the specified color space: one for grayscale images; three
    *                for color images. If this parameter is not specified, a
    *                single grayscale channel will be allocated.
    *
    * \param colorSpace          New color space. See the ColorSpace namespace
    *                for symbolic constants. If this parameter is not
    *                specified, new image data will be allocated in the
    *                grayscale space.
    *
    * If this object uniquely references its pixel data, then the previously
    * existing pixel data are destroyed, deallocated, and replaced with a newly
    * allocated local or shared image, preserving the storage space of this
    * instance.
    *
    * If this object is not unique, then the previously referenced pixel data
    * are dereferenced, and a newly created local image is uniquely referenced
    * by this object and allocated with the specified parameters.
    *
    * All image selections are reset to the default full selection after
    * calling this function.
    *
    * \note Newly allocated pixel samples are not initialized, so the image
    * will contain unpredictable values after calling this member function.
    */
   GenericImage& AllocateData( int width, int height,
                               int numberOfChannels = 1,
                               color_space colorSpace = ColorSpace::Gray )
   {
      if ( !m_data->IsUnique() )
      {
         Data* newData = new Data( this );
         DetachFromData();
         m_data = newData;
      }
      m_data->Allocate( width, height, numberOfChannels, colorSpace );
      ResetSelections();
      return *this;
   }

   /*!
    * Allocates new pixel data with the specified geometry and color space.
    * Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * AllocateData( rect.Width(), rect.Height(), numberOfChannels, colorSpace );
    * \endcode
    */
   GenericImage& AllocateData( const Rect& rect,
                               int numberOfChannels = 1,
                               color_space colorSpace = ColorSpace::Gray )
   {
      return AllocateData( rect.Width(), rect.Height(), numberOfChannels, colorSpace );
   }

   /*!
    * Causes this object to reference an empty image. Returns a reference to
    * this image.
    *
    * If this object uniquely references its pixel data, then the pixel data
    * are destroyed and deallocated immediately.
    *
    * If this object is not unique, then the previously referenced pixel data
    * are dereferenced, and a newly created empty local image is uniquely
    * referenced by this object.
    */
   GenericImage& FreeData()
   {
      if ( !m_data->IsEmpty() )
         if ( m_data->IsUnique() )
            m_data->Deallocate();
         else
         {
            Data* newData = new Data( this );
            DetachFromData();
            m_data = newData;
         }
      ResetSelections();
      return *this;
   }

   /*!
    * Forces this image to acquire the specified pixel data. Returns a
    * reference to this image.
    *
    * \param data    Starting address of the acquired pixel data. Must be
    *                either a null pointer, or a pointer to an array of
    *                contiguous blocks of pixel samples.
    *
    * \param width   Width in pixels of the acquired pixel data.
    *
    * \param height  Height in pixels of the acquired pixel data.
    *
    * \param numberOfChannels    Number of channels in the acquired data. Must
    *                be greater than or equal to the number of nominal channels
    *                corresponding to the specified color space: one for
    *                grayscale images; three for color images. If this
    *                parameter is not specified, a single grayscale channel is
    *                assumed.
    *
    * \param colorSpace          Color space of the acquired data. See the
    *                ColorSpace namespace for symbolic constants. If this
    *                parameter is not specified, the grayscale space is assumed
    *                for the acquired data.
    *
    * If this object uniquely references its pixel data, then the previously
    * existing pixel data are destroyed, deallocated, and replaced with the
    * specified local or shared \a data, preserving the storage space of this
    * instance.
    *
    * If this object is a non-unique local image, then the previously
    * referenced pixel data are dereferenced, and a newly created local image
    * is uniquely referenced by this object and forced to own the specified
    * local \a data.
    *
    * If this object is a non-unique shared image, then this member function
    * throws an Error exception with the appropriate error message: An aliased
    * shared image cannot be forced to store externally allocated pixel data.
    *
    * When successful, this function resets all image selections to the default
    * full selection.
    *
    * \warning When calling this member function for a shared image, always
    * make sure that the passed \a data array and the pointed data blocks have
    * been allocated as shared memory. If you pass a locally-allocated array or
    * block to this function (e.g., a \c new'ed block), you may cause severe
    * heap corruption in the PixInsight core application. Similarly, if the
    * image is local, only locally-allocated arrays and blocks must be used. In
    * other words: shared and local pixel data and images cannot be mixed.
    *
    * \warning \e Never use the \c new and \c delete operators with pixel data
    * memory blocks. Always call the Allocator() member function of
    * %GenericImage to get a reference to its PixelAllocator object, then use
    * its public member functions for all allocations and deallocations of
    * pixel data.
    */
   GenericImage& ImportData( sample** data, int width, int height,
                             int numberOfChannels = 1, color_space colorSpace = ColorSpace::Gray )
   {
      if ( !m_data->IsUnique() )
      {
         if ( m_data->IsShared() )
            throw Error( "GenericImage::ImportData(): Invalid operation for an aliased shared image" );
         Data* newData = new Data( this );
         DetachFromData();
         m_data = newData;
      }
      m_data->Import( data, width, height, numberOfChannels, colorSpace );
      ResetSelections();
      return *this;
   }

   /*!
    * Releases ownership of the pixel data allocated by this image.
    *
    * Returns an array of pointers to the channel data blocks that were owned
    * by the image before calling this function. The returned array is a
    * sequence of pointers p0, p1, ..., pn-1, where each pi is the starting
    * address of a contiguous block containing the sample data of an image
    * channel, and n is the number of channels in the image, including all
    * nominal and alpha channels.
    *
    * After calling this function the image will be empty, but the returned
    * data will not be deallocated; the caller will be responsible for
    * deallocating it when it is no longer needed.
    *
    * If this member function is called for an aliased image, it throws an
    * Error exception with the appropriate error message: Pixel data cannot be
    * released if two or more %GenericImage instances are referencing them.
    * The reason for this limitation is that if the data of a non-unique image
    * were released, either all instances sharing the same data would be
    * affected, which violates the implicit data sharing paradigm, or the newly
    * allocated data of a unique clone would be released, which makes no sense.
    *
    * \warning When calling ReleaseData() for a shared image, always be aware
    * that the returned \c sample** value points to a \e shared memory block.
    * If you handle such a block as if it was a block within your local heap
    * (e.g., if you call the \c delete operator to deallocate it), you may
    * cause severe heap corruption.
    *
    * \warning \e Never use the \c new and \c delete operators with pixel data
    * memory blocks. Always call the Allocator() member function of
    * %GenericImage to get a reference to its PixelAllocator object, then use
    * its public member functions for all allocations and deallocations of
    * pixel data.
    */
   sample** ReleaseData()
   {
      if ( !m_data->IsUnique() )
         throw Error( "GenericImage::ReleaseData(): Invalid operation for an aliased image" );
      sample** data = m_data->Release();
      ResetSelections();
      return data;
   }

   // -------------------------------------------------------------------------

   /*!
    * Returns the size in bytes of a row of pixels in this image (also known as
    * a <em>scan line</em>).
    */
   size_type LineSize() const
   {
      return BytesPerSample() * size_type( m_width );
   }

   /*!
    * Returns the size in bytes of a channel of this image. This is equal to
    * the area in square pixels multiplied by BytesPerSample().
    */
   size_type ChannelSize() const
   {
      return BytesPerSample() * NumberOfPixels();
   }

   /*!
    * Returns the size in bytes of all memory blocks required to store the
    * pixel data in this image, including nominal and alpha channels. This is
    * equal to the area in square pixels multiplied by the number of channels,
    * multiplied by BytesPerSample().
    */
   size_type ImageSize() const
   {
      return ChannelSize() * size_type( m_numberOfChannels );
   }

   /*!
    * Returns the size in bytes of all memory blocks required to store the
    * pixel data in the nominal channels of this image (i.e., \e excluding
    * alpha channels).
    */
   size_type NominalSize() const
   {
      return ChannelSize() * NumberOfNominalChannels();
   }

   /*!
    * Returns the size in bytes of all memory blocks required to store the
    * pixel data in the alpha channels of this image (i.e., \e excluding
    * nominal channels). Returns zero if this image has no alpha channels.
    */
   size_type AlphaSize() const
   {
      return ChannelSize() * NumberOfAlphaChannels();
   }

   /*!
    * Returns a pointer to the first pixel sample in the specified \a channel.
    *
    * \param channel    Channel index, 0 <= \a channel < \a n, where \a n is
    *                   the number of channels in this image, including nominal
    *                   and alpha channels. The default value is zero.
    *
    * The returned value is the address of the pixel at coordinates x=y=0 in
    * the specified \a channel.
    *
    * If this image is aliased, it is made unique before returning from this
    * member function. This involves making a local duplicate of the previously
    * referenced pixel data.
    */
   sample* PixelData( int channel = 0 )
   {
      PCL_PRECONDITION( 0 <= channel && channel < m_numberOfChannels )
      EnsureUnique();
      return m_pixelData[channel];
   }

   /*!
    * Returns a pointer to the constant first pixel sample in the specified
    * \a channel.
    *
    * This member function is the immutable counterpart of PixelData( int ).
    */
   const sample* PixelData( int channel = 0 ) const
   {
      PCL_PRECONDITION( 0 <= channel && channel < m_numberOfChannels )
      return m_pixelData[channel];
   }

   /*!
    * Returns true only is this is a valid, nonempty image.
    */
   operator bool() const
   {
      return m_data != nullptr && !m_data->IsEmpty();
   }

   /*!
    * Returns a pointer to the first pixel sample in the specified \a channel.
    *
    * \param channel    Channel index, 0 <= \a channel < \a n, where \a n is
    *                   the number of channels in this image, including nominal
    *                   and alpha channels.
    *
    * This operator is a convenience synonym for the PixelData( int ) member
    * function.
    */
   sample* operator []( int channel )
   {
      PCL_PRECONDITION( 0 <= channel && channel < m_numberOfChannels )
      return PixelData( channel );
   }

   /*!
    * Returns a pointer to the constant first pixel sample in the specified
    * \a channel.
    *
    * This operator function is the immutable counterpart of operator []( int ).
    */
   const sample* operator []( int channel ) const
   {
      PCL_PRECONDITION( 0 <= channel && channel < m_numberOfChannels )
      return PixelData( channel );
   }

   /*!
    * Returns a pointer to the first pixel sample in the first channel of this
    * image.
    *
    * For grayscale images, this function can be more readable and elegant than
    * operator []( int ).
    */
   sample* operator *()
   {
      PCL_PRECONDITION( 0 < m_numberOfChannels )
      return PixelData( 0 );
   }

   /*!
    * Returns a pointer to the constant first pixel sample in the first channel
    * of this image.
    *
    * This operator function is the immutable counterpart of operator *().
    */
   const sample* operator *() const
   {
      PCL_PRECONDITION( 0 < m_numberOfChannels )
      return PixelData( 0 );
   }

   /*!
    * Returns the starting address of a <em>scan line</em> (i.e., a row of
    * pixels) in this image.
    *
    * \param y          Vertical coordinate (or row index) of the desired scan
    *                   line, such that 0 <= \a y < \a h, where \a h is the
    *                   height in pixels of this image.
    *
    * \param channel    Channel index, 0 <= \a channel < \a n, where \a n is
    *                   the number of channels in this image, including nominal
    *                   and alpha channels. The default value is zero.
    *
    * The returned value is a pointer to the sample at coordinates (x=0,y) in
    * the specified \a channel.
    *
    * If this image is aliased, it is made unique before returning from this
    * member function. This involves making a local duplicate of the previously
    * referenced pixel data.
    */
   sample* ScanLine( int y, int channel = 0 )
   {
      PCL_PRECONDITION( 0 <= channel && channel < m_numberOfChannels )
      PCL_PRECONDITION( 0 <= y && y < m_height )
      EnsureUnique();
      return m_pixelData[channel] + RowOffset( y );
   }

   /*!
    * Returns the starting address of a constant <em>scan line</em> (i.e., a
    * row of pixels) in this image.
    *
    * This member function is the immutable counterpart of ScanLine( int, int ).
    */
   const sample* ScanLine( int y, int channel = 0 ) const
   {
      PCL_PRECONDITION( 0 <= channel && channel < m_numberOfChannels )
      PCL_PRECONDITION( 0 <= y && y < m_height )
      return m_pixelData[channel] + RowOffset( y );
   }

   /*!
    * Returns the address of a pixel sample in this image, given by its pixel
    * coordinates and channel index.
    *
    * \param x          Horizontal coordinate (or column index) of the desired
    *                   pixel, 0 <= \a x < \a w, where \a w is the width in
    *                   pixels of this image.
    *
    * \param y          Vertical coordinate (or row index) of the desired scan
    *                   line, 0 <= \a y < \a h, where \a h is the height in
    *                   pixels of this image.
    *
    * \param channel    Channel index, 0 <= \a channel < \a n, where \a n is
    *                   the number of channels in this image, including nominal
    *                   and alpha channels. The default value is zero.
    *
    * The returned value is a pointer to the sample at coordinates (x,y) in the
    * specified \a channel.
    *
    * If this image is aliased, it is made unique before returning from this
    * member function. This involves making a local duplicate of the previously
    * referenced pixel data.
    */
   sample* PixelAddress( int x, int y, int channel = 0 )
   {
      PCL_PRECONDITION( 0 <= channel && channel < m_numberOfChannels )
      PCL_PRECONDITION( 0 <= x && x < m_width )
      PCL_PRECONDITION( 0 <= y && y < m_height )
      EnsureUnique();
      return m_pixelData[channel] + PixelOffset( x, y );
   }

   /*!
    * Returns the address of a constant pixel sample in this image, given by
    * its pixel coordinates and channel index.
    *
    * This member function is the immutable counterpart of PixelAddress( int, int, int ).
    */
   const sample* PixelAddress( int x, int y, int channel = 0 ) const
   {
      PCL_PRECONDITION( 0 <= channel && channel < m_numberOfChannels )
      PCL_PRECONDITION( 0 <= x && x < m_width )
      PCL_PRECONDITION( 0 <= y && y < m_height )
      return m_pixelData[channel] + PixelOffset( x, y );
   }

   /*!
    * Returns the address of a pixel sample in this image, given by its pixel
    * position and channel index.
    *
    * \param p          Position of the desired pixel in image coordinates. The
    *                   specified point must be included in the image bounds
    *                   rectangle, that is, its coordinates must refer to an
    *                   existing pixel in this image.
    *
    * \param channel    Channel index, 0 <= \a channel < \a n, where \a n is
    *                   the number of channels in this image, including nominal
    *                   and alpha channels. The default value is zero.
    *
    * The returned value is a pointer to the sample at coordinates (p.x,p.y) in
    * the specified \a channel.
    *
    * If this image is aliased, it is made unique before returning from this
    * member function. This involves making a local duplicate of the previously
    * referenced pixel data.
    */
   sample* PixelAddress( const Point& p, int channel = 0 )
   {
      return PixelAddress( p.x, p.y, channel );
   }

   /*!
    * Returns the address of a constant pixel sample in this image, given by
    * its pixel position and channel index.
    *
    * This member function is the immutable counterpart of
    * PixelAddress( const Point&, int ).
    */
   const sample* PixelAddress( const Point& p, int channel = 0 ) const
   {
      return PixelAddress( p.x, p.y, channel );
   }

   /*!
    * Returns a reference to a pixel sample in this image, given by its pixel
    * coordinates and channel index.
    *
    * \param x          Horizontal coordinate (or column index) of the desired
    *                   pixel, 0 <= \a x < \a w, where \a w is the width in
    *                   pixels of this image.
    *
    * \param y          Vertical coordinate (or row index) of the desired scan
    *                   line, 0 <= \a y < \a h, where \a h is the height in
    *                   pixels of this image.
    *
    * \param channel    Channel index, 0 <= \a channel < \a n, where \a n is
    *                   the number of channels in this image, including nominal
    *                   and alpha channels. The default value is zero.
    *
    * If this image is aliased, it is made unique before returning from this
    * member function. This involves making a local duplicate of the previously
    * referenced pixel data.
    */
   sample& operator ()( int x, int y, int channel = 0 )
   {
      return *PixelAddress( x, y, channel );
   }

   /*!
    * Returns a pixel sample value, given by its pixel coordinates and channel
    * index.
    *
    * \param x          Horizontal coordinate (or column index) of the desired
    *                   pixel, 0 <= \a x < \a w, where \a w is the width in
    *                   pixels of this image.
    *
    * \param y          Vertical coordinate (or row index) of the desired scan
    *                   line, 0 <= \a y < \a h, where \a h is the height in
    *                   pixels of this image.
    *
    * \param channel    Channel index, 0 <= \a channel < \a n, where \a n is
    *                   the number of channels in this image, including nominal
    *                   and alpha channels. The default value is zero.
    */
   sample operator ()( int x, int y, int channel = 0 ) const
   {
      return *PixelAddress( x, y, channel );
   }

   /*!
    * Returns a reference to a pixel sample in this image, given by its pixel
    * position and channel index.
    *
    * \param p          Position of the desired pixel in image coordinates. The
    *                   specified point must be included in the image bounds
    *                   rectangle, that is, its coordinates must refer to an
    *                   existing pixel in this image.
    *
    * \param channel    Channel index, 0 <= \a channel < \a n, where \a n is
    *                   the number of channels in this image, including nominal
    *                   and alpha channels. The default value is zero.
    *
    * If this image is aliased, it is made unique before returning from this
    * member function. This involves making a local duplicate of the previously
    * referenced pixel data.
    */
   sample& operator ()( const Point& p, int channel = 0 )
   {
      return *PixelAddress( p, channel );
   }

   /*!
    * Returns a pixel sample value, given by its pixel position and channel
    * index.
    *
    * \param p          Position of the desired pixel in image coordinates. The
    *                   specified point must be included in the image bounds
    *                   rectangle, that is, its coordinates must refer to an
    *                   existing pixel in this image.
    *
    * \param channel    Channel index, 0 <= \a channel < \a n, where \a n is
    *                   the number of channels in this image, including nominal
    *                   and alpha channels. The default value is zero.
    */
   sample operator ()( const Point& p, int channel = 0 ) const
   {
      return *PixelAddress( p, channel );
   }

   /*!
    * Returns a reference to a pixel sample in this image, given by its pixel
    * coordinates and channel index.
    *
    * \deprecated This member function has been deprecated. Use
    * GenericImage::operator()( int, int, int ) in newly produced code.
    */
   sample& Pixel( int x, int y, int channel = 0 )
   {
      return operator()( x, y, channel );
   }

   /*!
    * Returns a pixel sample value, given by its pixel coordinates and channel
    * index.
    *
    * \deprecated This member function has been deprecated. Use
    * GenericImage::operator()( int, int, int ) const in newly produced code.
    */
   sample Pixel( int x, int y, int channel = 0 ) const
   {
      return operator()( x, y, channel );
   }

   /*!
    * Returns a reference to a pixel sample in this image, given by its pixel
    * position and channel index.
    *
    * \deprecated This member function has been deprecated. Use
    * GenericImage::operator()( const Point&, int ) in newly produced code.
    */
   sample& Pixel( const Point& p, int channel = 0 )
   {
      return operator()( p, channel );
   }

   /*!
    * Returns a pixel sample value, given by its pixel position and channel
    * index.
    *
    * \deprecated This member function has been deprecated. Use
    * GenericImage::operator()( const Point&, int ) const in newly produced
    * code.
    */
   sample Pixel( const Point& p, int channel = 0 ) const
   {
      return operator()( p, channel );
   }

   // -------------------------------------------------------------------------

   /*!
    * Associates an RGB working space \a RGBWS with this image.
    *
    * \note For shared images (i.e. images living in the PixInsight core
    * application), the RGB working space cannot be changed by calling this
    * member function.
    */
   virtual void SetRGBWorkingSpace( const RGBColorSystem& RGBWS )
   {
      if ( !IsShared() )
      {
         EnsureUnique();
         m_RGBWS = RGBWS;
      }
   }

   // -------------------------------------------------------------------------

   /*!
    * Copies pixel samples from the specified source \a image to this image.
    * Returns a reference to this image.
    *
    * \param image   Source image.
    *
    * \param rect    Source rectangular selection. If this parameter defines an
    *                empty rectangle, the current rectangular selection in the
    *                source \a image will be used. If the specified rectangle
    *                is not empty and extends beyond source \a image
    *                boundaries, only the intersection with the source \a image
    *                will be used. If that intersection does not exist, then
    *                this image will be empty after calling this member
    *                function. The default value is an empty rectangle.
    *
    * \param firstChannel  Zero-based index of the first channel to copy. If a
    *                negative value is specified, the first selected channel in
    *                the source \a image will be used. If the specified value
    *                is larger than or equal to the number of existing channels
    *                in the source \a image, then this image will be empty
    *                after calling this function. The default value is -1.
    *
    * \param lastChannel   Zero-based index of the last channel to copy. If a
    *                negative value is specified, the last selected channel in
    *                the source \a image will be used. If the specified value
    *                is larger than or equal to the number of existing channels
    *                in the source \a image, then this image will be empty
    *                after calling this function. The default value is -1.
    *
    * Previously referenced data in this image will be dereferenced, and
    * immediately destroyed if they become unreferenced.
    *
    * If this image and the specified source \a image are (a) compatible in
    * terms of pixel sample types, (b) both are local images, and (c) the
    * source \a image is completely selected, then this member function just
    * references the same pixel data, enabling the implicit data sharing
    * mechanism of %GenericImage. In this case, existing pixel data in the
    * source \a image are not duplicated, and no allocation is performed.
    *
    * If this member function causes a reallocation of pixel data, the newly
    * allocated data will be uniquely referenced by this image.
    *
    * The source \a image can be an instance of any supported template
    * instantiation of %GenericImage, not necessarily of the same instantiation
    * than this image. All the necessary data type conversions are performed
    * transparently. If pixel sample types are different, source data are
    * transformed to the data type of this image. This involves conversions
    * from the source data type P1::sample to the target data type P::sample,
    * as per pixel traits primitives.
    *
    * \note It is very important to point out that the storage class of an
    * image cannot be changed with this member function, even if this image and
    * the source image belong to the same template instantiation. For example:
    *
    * \code
    * Image aSharedImage( (void*)0, 0, 0 );
    * ...
    * Image local;
    * ...
    * local.Assign( aSharedImage ); // local continues being a local image.
    *                               // It now stores a copy of aSharedImage's
    *                               // pixel data.
    * local = aSharedImage; // Same as above, since the assignment operator is
    *                       // just an alias for the Assign() function.
    *
    * ImageVariant otherShared;
    * otherShared.CreateSharedFloatImage();
    * otherShared.AssignImage( local ); // otherShared is still a shared image.
    * \endcode
    */
   template <class P1>
   GenericImage& Assign( const GenericImage<P1>& image,
                         const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      m_status = image.Status();

      Rect r = rect;
      if ( !image.ParseSelection( r, firstChannel, lastChannel ) )
      {
         FreeData();
         return *this;
      }

      int n = 1 + lastChannel - firstChannel;
      AllocateData( r, n, (firstChannel == 0 && n >= ColorSpace::NumberOfNominalChannels( image.ColorSpace() )) ?
                           image.ColorSpace() : ColorSpace::Gray );

      if ( !IsShared() ) // ### cannot modify a shared image's RGBWS
         m_RGBWS = image.RGBWorkingSpace();

      ResetSelections();

      if ( r == image.Bounds() )
         for ( int c = 0; firstChannel <= lastChannel; ++c, ++firstChannel )
            P::Copy( m_pixelData[c], image[firstChannel], NumberOfPixels() );
      else
         for ( int c = 0; firstChannel <= lastChannel; ++c, ++firstChannel )
         {
            sample* f = m_pixelData[c];
            const typename P1::sample* g = image.PixelAddress( r.LeftTop(), firstChannel );
            for ( int y = 0; y < m_height; ++y, f += m_width, g += image.Width() )
               P::Copy( f, g, m_width );
         }

      return *this;
   }

   GenericImage& Assign( const GenericImage& image,
                         const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      m_status = image.Status();

      Rect r = rect;
      if ( !image.ParseSelection( r, firstChannel, lastChannel ) )
      {
         FreeData();
         return *this;
      }

#define completeSelection  (firstChannel == 0 && lastChannel == image.m_numberOfChannels-1 && r == image.Bounds())

      if ( m_data == image.m_data )
      {
         // Self-assignment
         if ( !completeSelection )
         {
            GenericImage result( image, r, firstChannel, lastChannel ); // ### implicit recursion
            result.m_data->Attach( this );
            DetachFromData();
            m_data = result.m_data;
         }
         ResetSelections();
         return *this;
      }

      if ( !IsShared() )
         if ( !image.IsShared() )
            if ( completeSelection )
            {
               image.m_data->Attach( this );
               DetachFromData();
               m_data = image.m_data;
               ResetSelections();
               return *this;
            }

#undef completeSelection

      int n = 1 + lastChannel - firstChannel;
      AllocateData( r, n, (firstChannel == 0 && n >= ColorSpace::NumberOfNominalChannels( image.ColorSpace() )) ?
                           image.ColorSpace() : ColorSpace::Gray );

      if ( !IsShared() ) // ### cannot modify a shared image's RGBWS
         m_RGBWS = image.m_RGBWS;

      ResetSelections();

      if ( r == image.Bounds() )
         for ( int c = 0; firstChannel <= lastChannel; ++c, ++firstChannel )
            P::Copy( m_pixelData[c], image.m_pixelData[firstChannel], NumberOfPixels() );
      else
         for ( int c = 0; firstChannel <= lastChannel; ++c, ++firstChannel )
         {
            sample* f = m_pixelData[c];
            const sample* g = image.PixelAddress( r.LeftTop(), firstChannel );
            for ( int y = 0; y < m_height; ++y, f += m_width, g += image.m_width )
               P::Copy( f, g, m_width );
         }

      return *this;
   }

   /*!
    * Copies pixel samples from the specified source \a image to this image.
    * Returns a reference to this image.
    *
    * This operator is a convenience synonym for Assign().
    */
   template <class P1>
   GenericImage& operator =( const GenericImage<P1>& image )
   {
      return Assign( image );
   }

   /*!
    * Copy assignment operator. Copies pixel samples from the specified source
    * \a image to this image. Returns a reference to this image.
    *
    * This operator is a synonym for Assign(), where both this object and the
    * source \a image are instances of the same template instantiation of
    * %GenericImage. The declaration of this operator is necessary for
    * optimization purposes. It also provides a pure copy assignment operator
    * for the %GenericImage template class.
    */
   GenericImage& operator =( const GenericImage& image )
   {
      return Assign( image );
   }

#define TRANSFER_BODY()                            \
      if ( m_data != image.m_data )                \
      {                                            \
         DetachFromData();                         \
         m_data = image.m_data;                    \
         (void)AbstractImage::operator =( image ); \
         image.m_data = nullptr;                   \
      }                                            \
      return *this

   /*!
    * Transfers data from another \a image to this object. Returns a reference
    * to this image.
    *
    * Decrements the reference counter of the current image data, and destroys
    * it if it becomes unreferenced.
    *
    * Transfers the source image to this object, leaving empty and invalid the
    * source \a image, which cannot be used and must be destroyed immediately
    * after calling this function.
    */
   GenericImage& Transfer( GenericImage& image )
   {
      TRANSFER_BODY();
   }

   /*!
    * Transfers data from another \a image to this object. Returns a reference
    * to this image.
    *
    * Decrements the reference counter of the current image data, and destroys
    * it if it becomes unreferenced.
    *
    * Transfers the source image to this object, leaving empty and invalid the
    * source \a image, which cannot be used and must be destroyed immediately
    * after calling this function.
    */
   GenericImage& Transfer( GenericImage&& image )
   {
      TRANSFER_BODY();
   }

#undef TRANSFER_BODY

   /*!
    * Move assignment operator. Returns a reference to this image.
    *
    * This operator is equivalent to Transfer( GenericImage& ).
    */
   GenericImage& operator =( GenericImage&& image )
   {
      return Transfer( image );
   }

   /*!
    * Fills the current selection (selection rectangle and channel range) with
    * a \a scalar. Returns a reference to this image.
    *
    * This operator is a convenience synonym for Fill().
    */
   GenericImage& operator =( sample scalar )
   {
      return Fill( scalar );
   }

   /*!
    * Exchanges two images \a x1 and \a x2 of the same template instantiation.
    */
   friend void Swap( GenericImage& x1, GenericImage& x2 )
   {
      x1.AbstractImage::Swap( x2 );
      pcl::Swap( x1.m_data, x2.m_data );
   }

   // -------------------------------------------------------------------------

#ifndef __PCL_NO_VECTOR_IMAGE_CONVERSION

   /*!
    * Generates a vector of pixel samples from a row of this image.
    *
    * \param y          Vertical coordinate (or row index) of the desired scan
    *                   line, 0 <= \a y < \a h, where \a h is the height in
    *                   pixels of this image.
    *
    * \param channel    Channel index. If this parameter is negative, the
    *                   currently selected channel will be used. The default
    *                   value is -1.
    *
    * The number of components in the returned vector will be equal to the
    * width in pixels of this image.
    *
    * If this image is empty, if the specified vertical coordinate is out of
    * range, or if a nonexistent channel is specified, this member function
    * returns an empty vector.
    */
   sample_vector RowVector( int y, int channel = -1 ) const
   {
      if ( channel < 0 )
         channel = m_channel;
      if ( y < 0 || y >= m_height || channel < 0 || channel >= m_numberOfChannels )
         return sample_vector();
      sample_vector row( m_width );
      P::Get( row.Begin(), ScanLine( y, channel ), m_width );
      return row;
   }

   /*!
    * Generates a vector of pixel samples from a column of this image.
    *
    * \param x          Horizontal coordinate (or column index) of the desired
    *                   column, 0 <= \a x < \a w, where \a w is the width in
    *                   pixels of this image.
    *
    * \param channel    Channel index. If this parameter is negative, the
    *                   currently selected channel will be used. The default
    *                   value is -1.
    *
    * The number of components in the returned vector will be equal to the
    * height in pixels of this image.
    *
    * If this image is empty, if the specified horizontal coordinate is out of
    * range, or if a nonexistent channel is specified, this member function
    * returns an empty vector.
    */
   sample_vector ColumnVector( int x, int channel = -1 ) const
   {
      if ( channel < 0 )
         channel = m_channel;
      if ( x < 0 || x >= m_width || channel < 0 || channel >= m_numberOfChannels )
         return sample_vector();
      sample_vector col( m_height );
      const sample* v = PixelAddress( x, 0, channel );
      for ( int y = 0; y < m_height; ++y, v += m_width )
         col[y] = *v;
      return col;
   }

   /*!
    * A convenience synonym for ColumnVector( int, int ) const.
    */
   sample_vector ColVector( int x, int channel = 0 ) const
   {
      return ColumnVector( x, channel );
   }

#endif   // !__PCL_NO_VECTOR_IMAGE_CONVERSION

   /*!
    * Stores a duplicate of a row of pixel samples in the specified array,
    * with implicit data type conversion.
    *
    * \param[out] buffer   Starting address of a contiguous block where samples
    *                   from the source row will be converted and copied. The
    *                   length of this block must be at least equal to the
    *                   width in pixels of this image.
    *
    * \param y          Vertical coordinate (or row index) of the desired scan
    *                   line, 0 <= \a y < \a h, where \a h is the height in
    *                   pixels of this image.
    *
    * \param channel    Channel index. If this parameter is negative, the
    *                   currently selected channel will be used. The default
    *                   value is -1.
    *
    * If this image is empty, if the specified vertical coordinate is out of
    * range, or if a nonexistent channel is specified, this member function
    * does nothing.
    */
   template <typename T>
   void GetRow( T* buffer, int y, int channel = -1 ) const
   {
      PCL_PRECONDITION( buffer != 0 )
      if ( channel < 0 )
         channel = m_channel;
      if ( y >= 0 && y < m_height && channel >= 0 && channel < m_numberOfChannels )
         P::Get( buffer, ScanLine( y, channel ), m_width );
   }

   /*!
    * Stores a duplicate of a column of pixel samples in the specified array,
    * with implicit data type conversion.
    *
    * \param[out] buffer   Starting address of a contiguous block where samples
    *                   from the source column will be converted and copied.
    *                   The length of this block must be at least equal to the
    *                   height in pixels of this image.
    *
    * \param x          Horizontal coordinate (or column index) of the desired
    *                   column, 0 <= \a x < \a w, where \a w is the width in
    *                   pixels of this image.
    *
    * \param channel    Channel index. If this parameter is negative, the
    *                   currently selected channel will be used. The default
    *                   value is -1.
    *
    * If this image is empty, if the specified horizontal coordinate is out of
    * range, or if a nonexistent channel is specified, this member function
    * does nothing.
    */
   template <typename T>
   void GetColumn( T* buffer, int x, int channel = -1 ) const
   {
      PCL_PRECONDITION( buffer != 0 )
      if ( channel < 0 )
         channel = m_channel;
      if ( x >= 0 && x < m_width && channel >= 0 && channel < m_numberOfChannels )
      {
         const sample* v = PixelAddress( x, 0, channel );
         for ( int y = 0; y < m_height; ++y, ++buffer, v += m_width )
            P::FromSample( *buffer, *v );
      }
   }

   /*!
    * Sets a row of pixel samples from values in the specified array, with
    * implicit data type conversion. Returns a reference to this image.
    *
    * \param buffer     Starting address of a contiguous block with source
    *                   values that will be converted and copied to the target
    *                   row in this image. The length of this block must be at
    *                   least equal to the width in pixels of this image.
    *
    * \param y          Vertical coordinate (or row index) of the desired scan
    *                   line, 0 <= \a y < \a h, where \a h is the height in
    *                   pixels of this image.
    *
    * \param channel    Channel index. If this parameter is negative, the
    *                   currently selected channel will be used. The default
    *                   value is -1.
    *
    * If this image is empty, if the specified vertical coordinate is out of
    * range, or if a nonexistent channel is specified, this member function
    * does nothing.
    */
   template <typename T>
   GenericImage& SetRow( const T* buffer, int y, int channel = -1 )
   {
      PCL_PRECONDITION( buffer != 0 )
      if ( channel < 0 )
         channel = m_channel;
      if ( y >= 0 && y < m_height && channel >= 0 && channel < m_numberOfChannels )
         P::Copy( ScanLine( y, channel ), buffer, m_width );
      return *this;
   }

   /*!
    * Sets a column of pixel samples from values in the specified array, with
    * implicit data type conversion. Returns a reference to this image.
    *
    * \param buffer     Starting address of a contiguous block with source
    *                   values that will be converted and copied to the target
    *                   column in this image. The length of this block must be
    *                   at least equal to the height in pixels of this image.
    *
    * \param x          Horizontal coordinate (or column index) of the desired
    *                   column, 0 <= \a x < \a w, where \a w is the width in
    *                   pixels of this image.
    *
    * \param channel    Channel index. If this parameter is negative, the
    *                   currently selected channel will be used. The default
    *                   value is -1.
    *
    * If this image is empty, if the specified horizontal coordinate is out of
    * range, or if a nonexistent channel is specified, this member function
    * does nothing.
    */
   template <typename T>
   GenericImage& SetColumn( const T* buffer, int x, int channel = -1 )
   {
      PCL_PRECONDITION( buffer != 0 )
      if ( channel < 0 )
         channel = m_channel;
      if ( x >= 0 && x < m_width && channel >= 0 && channel < m_numberOfChannels )
      {
         sample* v = PixelAddress( x, 0, channel );
         for ( int y = 0; y < m_height; ++y, ++buffer, v += m_width )
            *v = P::ToSample( *buffer );
      }
      return *this;
   }

   // -------------------------------------------------------------------------

   /*!
    * Creates \a n new alpha channels in this image. Alpha channels are those
    * in excess of nominal channels, e.g. a fourth channel in an RGB color
    * image, or a second channel in a grayscale image. Returns a reference to
    * this image.
    *
    * Newly created channels are not initialized, so their pixel samples will
    * contain unpredictable values.
    */
   GenericImage& CreateAlphaChannels( int n )
   {
      if ( n > 0 && m_numberOfChannels > 0 )
      {
         EnsureUnique();
         sample** oldData = m_pixelData;
         sample** newData = nullptr;
         try
         {
            newData = m_allocator.AllocateChannelSlots( m_numberOfChannels+n );
            for ( int i = 0; i < m_numberOfChannels; ++i )
               newData[i] = oldData[i];
            for ( int i = 0; i < n; ++i )
               newData[m_numberOfChannels+i] = m_allocator.AllocatePixels( m_width, m_height );
         }
         catch ( ... )
         {
            if ( newData != nullptr )
            {
               for ( int i = 0; i < n; ++i )
                  if ( newData[m_numberOfChannels+i] != nullptr )
                     m_allocator.Deallocate( newData[m_numberOfChannels+i] );
               m_allocator.Deallocate( newData );
            }
            throw;
         }

         m_allocator.SetSharedData( m_pixelData = newData );
         m_allocator.SetSharedGeometry( m_width, m_height, m_numberOfChannels += n );
         m_allocator.Deallocate( oldData );
      }

      return *this;
   }

   /*!
    * Forces this image to acquire the specified pixel sample data as a new
    * alpha channel. Returns a reference to this image.
    *
    * \param data    Starting address of the acquired pixel data. If this
    *                parameter is a null pointer, a single new alpha channel
    *                will be added to the image.
    *
    * The image will own the acquired pixel \a data. The new alpha channel is
    * appended to the existing channels.
    *
    * \warning See the ImportData() member function documentation for an
    * important warning notice that applies also to this function.
   */
   GenericImage& AddAlphaChannel( sample* data = nullptr )
   {
      // $$$ WARNING $$$
      // * If this is a shared image, data must either be null or point to a
      //   shared memory block.
      // * If this is a local image, data must either be null or point to a
      //   block allocated in the local heap.

      if ( data == nullptr )
         CreateAlphaChannels( 1 );
      else if ( m_numberOfChannels > 0 )
      {
         EnsureUnique();
         sample** oldData = m_pixelData;
         sample** newData = nullptr;
         try
         {
            newData = m_allocator.AllocateChannelSlots( m_numberOfChannels+1 );
            for ( int i = 0; i < m_numberOfChannels; ++i )
               newData[i] = oldData[i];
            newData[m_numberOfChannels] = data;
         }
         catch ( ... )
         {
            if ( newData != nullptr )
               m_allocator.Deallocate( newData );
            throw;
         }

         m_allocator.SetSharedData( m_pixelData = newData );
         m_allocator.SetSharedGeometry( m_width, m_height, ++m_numberOfChannels );
         m_allocator.Deallocate( oldData );
      }

      return *this;
   }

   /*!
    * Releases an alpha channel into another image. Returns a reference to this
    * image.
    *
    * \param image   Image that will receive the data from the released alpha
    *                channel. Its previous contents will be dereferenced and
    *                eventually destroyed.
    *
    * \param channel Zero-based index of the alpha channel to be released,
    *                0 <= \a channel < \a n, where \a n is the number of
    *                existing alpha channels before calling this function.
    *
    * \note The specified \a channel index is an <em>alpha channel index</em>,
    * not a normal channel index comprising nominal channels. That is, the
    * first alpha channel is indexed as zero by this function.
    *
    * \warning If this image is a shared instance, then the specified target
    * \a image must also be a reference to a shared image. Correspondingly, if
    * this is a local image, the target \a image must also be local. Pixel data
    * blocks cannot be transferred between local and shared images. If this
    * rule is violated, this member function will throw an Error exception.
    */
   GenericImage& ReleaseAlphaChannel( GenericImage& image, int channel )
   {
      if ( IsShared() != image.IsShared() )
         throw Error( "GenericImage::ReleaseAlphaChannel(): Cannot release pixel data between local and shared images" );

      if ( channel < 0 || channel >= NumberOfAlphaChannels() )
      {
         image.FreeData();
         return *this;
      }

      int c = NumberOfNominalChannels() + channel;

      sample** newData = nullptr;
      try
      {
         newData = image.m_allocator.AllocateChannelSlots( 1 );
         *newData = m_pixelData[c];
      }
      catch ( ... )
      {
         if ( newData != nullptr )
            image.m_allocator.Deallocate( newData );
         throw;
      }

      image.FreeData();

      image.m_pixelData = newData;
      image.m_width = m_width;
      image.m_height = m_height;
      image.m_numberOfChannels = 1;
      image.m_colorSpace = ColorSpace::Gray;
      image.m_data->UpdateSharedImage();
      image.ResetSelections();

      m_pixelData[c] = nullptr;
      ForgetAlphaChannel( channel );

      return *this;
   }

   /*!
    * Destroys an alpha channel. Returns a reference to this image.
    *
    * \param channel Zero-based index of the alpha channel to be destroyed,
    *                0 <= \a c < \a n, where \a n is the number of existing
    *                alpha channels before calling this function.
    *
    * \note The specified \a channel index is an <em>alpha channel index</em>,
    * not a normal channel index comprising nominal channels. That is, the
    * first alpha channel is indexed as zero by this function.
    */
   GenericImage& DeleteAlphaChannel( int channel )
   {
      if ( channel >= 0 && channel < NumberOfAlphaChannels() )
      {
         EnsureUnique();
         int c = NumberOfNominalChannels() + channel;
         m_allocator.Deallocate( m_pixelData[c] );
         m_pixelData[c] = nullptr;
         ForgetAlphaChannel( channel );
      }

      return *this;
   }

   /*!
    * Forces this image to discard an existing alpha channel without
    * deallocating it. Returns a reference to this image.
    *
    * \param channel Zero-based index of the alpha channel to be forgotten,
    *                0 <= \a c < \a n, where \a n is the number of existing
    *                alpha channels before calling this function.
    *
    * \warning You should have gained control over the pixel data corresponding
    * to the forgotten \a channel before calling this function, in order to be
    * able to deallocate it when appropriate, or you'll induce a memory leak.
    *
    * \note The specified \a channel index is an <em>alpha channel index</em>,
    * not a normal channel index comprising nominal channels. That is, the
    * first alpha channel is indexed as zero by this function.
    */
   GenericImage& ForgetAlphaChannel( int channel )
   {
      if ( channel >= 0 && channel < NumberOfAlphaChannels() )
      {
         EnsureUnique();
         sample** oldData = m_pixelData;
         sample** newData = m_allocator.AllocateChannelSlots( m_numberOfChannels-1 );

         int n0 = NumberOfNominalChannels();
         int c = n0 + channel;

         for ( int i = 0; i < c; ++i )
            newData[i] = oldData[i];
         for ( int i = c, j = c; ++j < m_numberOfChannels; ++i )
            newData[i] = oldData[j];

         m_allocator.SetSharedData( m_pixelData = newData );
         m_allocator.SetSharedGeometry( m_width, m_height, --m_numberOfChannels );

         if ( m_channel >= n0 || m_lastChannel >= n0 )
            ResetChannelRange();

         m_allocator.Deallocate( oldData );
      }

      return *this;
   }

   /*!
    * Destroys all existing alpha channels in this image. Returns a reference
    * to this image.
    */
   GenericImage& DeleteAlphaChannels()
   {
      int n0 = NumberOfNominalChannels();
      int n = m_numberOfChannels;
      if ( n > n0 )
      {
         EnsureUnique();
         do
            m_allocator.Deallocate( m_pixelData[--n] ), m_pixelData[n] = nullptr;
         while ( n > n0 );
         ForgetAlphaChannels();
      }

      return *this;
   }

   /*!
    * Forces this image to discard all existing alpha channels without
    * deallocating them. Returns a reference to this image.
    *
    * \warning You should have gained control over the pixel data for all alpha
    * channels in this image before calling this function, in order to be able
    * to deallocate them when appropriate, or you'll induce a memory leak.
    */
   GenericImage& ForgetAlphaChannels()
   {
      int n0 = NumberOfNominalChannels();
      if ( m_numberOfChannels > n0 )
      {
         EnsureUnique();
         sample** oldData = m_pixelData;
         sample** newData = m_allocator.AllocateChannelSlots( n0 );

         for ( int i = 0; i < n0; ++i )
            newData[i] = oldData[i];

         m_allocator.SetSharedData( m_pixelData = newData );
         m_allocator.SetSharedGeometry( m_width, m_height, m_numberOfChannels = n0 );

         if ( m_channel >= n0 || m_lastChannel >= n0 )
            ResetChannelRange();

         m_allocator.Deallocate( oldData );
      }

      return *this;
   }

   // -------------------------------------------------------------------------

   /*!
    * Fills a subset of pixel samples of this image with the specified
    * \a scalar. Returns a reference to this image.
    *
    * \param scalar  Scalar value to fill with. The specified value will be
    *                converted to the sample data type of this image.
    *
    * \param rect    Target rectangular region. If this parameter defines an
    *                empty rectangle, the current rectangular selection will be
    *                used. If the specified rectangle is not empty and extends
    *                beyond image boundaries, only the intersection with this
    *                image will be used. If that intersection does not exist,
    *                then this member function has no effect. The default value
    *                is an empty rectangle.
    *
    * \param firstChannel  Zero-based index of the first channel to fill. If a
    *                negative index is specified, the first selected channel
    *                will be used. If the specified index is larger than or
    *                equal to the number of channels in this image, then this
    *                member function has no effect. The default value is -1.
    *
    * \param lastChannel   Zero-based index of the last channel to fill. If a
    *                negative index is specified, the last selected channel
    *                will be used. If the specified index is larger than or
    *                equal to the number of channels in this image, then this
    *                member function has no effect. The default value is -1.
    *
    * Previously referenced data in this image will be dereferenced, and
    * immediately destroyed if they become unreferenced.
    *
    * If this member function causes a reallocation of pixel data, the newly
    * allocated data will be local and uniquely referenced by this image.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   template <typename T>
   GenericImage& Fill( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return *this;

      EnsureUnique();

      size_type N = size_type( r.Width() )*size_type( r.Height() );
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Filling image", N*(1 + lastChannel - firstChannel) );

      sample v = P::ToSample( scalar );

      if ( r == Bounds() )
         for ( int i = firstChannel; i <= lastChannel; ++i, m_status += N )
            P::Fill( m_pixelData[i], v, N );
      else
         for ( int i = firstChannel, w = r.Width(), h = r.Height(); i <= lastChannel; ++i, m_status += N )
         {
            sample* f = PixelAddress( r.LeftTop(), i );
            for ( int j = 0; j < h; ++j, f += m_width )
               P::Fill( f, v, w );
         }

      return *this;
   }

   /*!
    * Fills a subset of pixel samples of this image with the scalar components
    * of a vector of per-channel filling \a values. Returns a reference to this
    * image.
    *
    * \param values  %Vector of filling values. Each component of this vector
    *                is a filling value for the corresponding channel in the
    *                pixel sample selection (i.e., the first component is the
    *                filling value for the first selected channel, the second
    *                component for the second selected channel, etc.). If the
    *                vector has less components than channels in the selection,
    *                the missing values are replaced with the minimum sample
    *                value in the native range of the image (usually zero).
    *
    * The rest of parameters of this function are identical to those of
    * Fill( T, const Rect&, int, int ).
    */
   template <typename T>
   GenericImage& Fill( const GenericVector<T>& values,
                       const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return *this;

      EnsureUnique();

      size_type N = size_type( r.Width() )*size_type( r.Height() );
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Filling image", N*(1 + lastChannel - firstChannel) );

      if ( r == Bounds() )
         for ( int i = firstChannel, c = 0; i <= lastChannel; ++i, ++c, m_status += N )
         {
            sample v = (c < values.Length()) ? P::ToSample( values[c] ) : P::MinSampleValue();
            P::Fill( m_pixelData[i], v, N );
         }
      else
         for ( int i = firstChannel, c = 0, w = r.Width(), h = r.Height(); i <= lastChannel; ++i, ++c, m_status += N )
         {
            sample* f = PixelAddress( r.LeftTop(), i );
            sample v = (c < values.Length()) ? P::ToSample( values[c] ) : P::MinSampleValue();
            for ( int j = 0; j < h; ++j, f += m_width )
               P::Fill( f, v, w );
         }

      return *this;
   }

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * filled with the specified \a scalar.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill( T, const Rect&, int, int ).
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   template <typename T>
   GenericImage Filled( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Fill( scalar );
      return result;
   }

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * filled with the scalar components of a vector of per-channel filling
    * \a values.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill( const GenericVector&, const Rect&, int, int ).
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   template <typename T>
   GenericImage Filled( const GenericVector<T>& values,
                        const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Fill( values );
      return result;
   }

   /*!
    * Fills a subset of pixel samples with the constant value resulting from
    * converting the floating point scalar zero (0.0) to the sample data type
    * of the image. Returns a reference to this image.
    *
    * This member function is a convenience shortcut for:
    *
    * \code
    * Fill( P::ToSample( 0.0 ), rect, firstChannel, lastChannel );
    * \endcode
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   GenericImage& Zero( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Fill( P::ToSample( 0.0 ), rect, firstChannel, lastChannel );
   }

   /*!
    * Fills a subset of pixel samples with the constant value resulting from
    * converting the floating point scalar one (1.0) to the sample data type of
    * the image. Returns a reference to this image.
    *
    * This member function is a convenience shortcut for:
    *
    * \code
    * Fill( P::ToSample( 1.0 ), rect, firstChannel, lastChannel );
    * \endcode
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   GenericImage& One( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Fill( P::ToSample( 1.0 ), rect, firstChannel, lastChannel );
   }

   /*!
    * Fills a subset of pixel samples with the minimum sample value in the
    * native range of the image. Returns a reference to this image.
    *
    * This member function is a convenience shortcut for:
    *
    * \code
    * Fill( P::MinSampleValue(), rect, firstChannel, lastChannel );
    * \endcode
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   GenericImage& Black( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Fill( P::MinSampleValue(), rect, firstChannel, lastChannel );
   }

   /*!
    * Fills a subset of pixel samples with the maximum sample value in the
    * native range of the image. Returns a reference to this image.
    *
    * This member function is a convenience shortcut for:
    *
    * \code
    * Fill( P::MaxSampleValue(), rect, firstChannel, lastChannel );
    * \endcode
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   GenericImage& White( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Fill( P::MaxSampleValue(), rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

   /*!
    * Inverts a subset of pixel samples with respect to the specified
    * \a scalar. Returns a reference to this image.
    *
    * The inverse of a pixel sample \a v with respect to a scalar \a r is equal
    * to \a r - \a v. Normally (but not necessarily) the reference scalar used
    * is the maximum sample value in the native range of the image.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   template <typename T>
   GenericImage& Invert( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return *this;

      EnsureUnique();

      size_type N = size_type( r.Width() )*size_type( r.Height() );
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Inverting samples", N*(1 + lastChannel - firstChannel) );

      sample v = P::ToSample( scalar );

      if ( r == Bounds() )
         for ( int i = firstChannel; i <= lastChannel; ++i, m_status += N )
            for ( sample* f = m_pixelData[i], * f1 = f+N; f < f1; ++f )
               *f = v - *f;
      else
         for ( int i = firstChannel, w = r.Width(), h = r.Height(); i <= lastChannel; ++i, m_status += N )
         {
            sample* f = PixelAddress( r.LeftTop(), i );
            for ( int j = 0; j < h; ++j, f += m_width-w )
               for ( sample* f1 = f+w; f < f1; ++f )
                  *f = v - *f;
         }

      return *this;
   }

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * inverted with respect to the specified \a scalar.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   template <typename T>
   GenericImage Inverted( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Invert( scalar );
      return result;
   }

   /*!
    * Inverts a subset of pixel samples with respect to the maximum sample
    * value in the native range of the image. Returns a reference to this
    * image.
    *
    * This member function is a convenience shortcut for:
    *
    * \code
    * Invert( P::MaxSampleValue(), rect, firstChannel, lastChannel );
    * \endcode
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   GenericImage& Invert( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Invert( P::MaxSampleValue(), rect, firstChannel, lastChannel );
   }

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * inverted with respect to the maximum sample value in the native range of
    * the image.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   GenericImage Inverted( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Invert();
      return result;
   }

   /*!
    * Returns an inverted duplicate of this image. The inversion operation is
    * performed on the current pixel sample selection with respect to the
    * maximum sample value in the native range of the image.
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   GenericImage operator ~() const
   {
      GenericImage result( *this );
      (void)result.Invert();
      return result;
   }

   /*!
    * Replaces a subset of pixel samples with their bitwise logical NOT values.
    * Returns a reference to this image.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   GenericImage& Not( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return *this;

      EnsureUnique();

      size_type N = size_type( r.Width() )*size_type( r.Height() );
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Bitwise Not", N*(1 + lastChannel - firstChannel) );

      if ( r == Bounds() )
         for ( int i = firstChannel; i <= lastChannel; ++i, m_status += N )
            for ( sample* f = m_pixelData[i], * f1 = f+N; f < f1; ++f )
               P::Not( *f );
      else
         for ( int i = firstChannel, w = r.Width(), h = r.Height(); i <= lastChannel; ++i, m_status += N )
         {
            sample* f = PixelAddress( r.LeftTop(), i );
            for ( int j = 0; j < h; ++j, f += m_width-w )
               for ( sample* f1 = f+w; f < f1; ++f )
                  P::Not( *f );
         }

      return *this;
   }

   /*!
    * Truncates a subset of pixel samples to the specified range of values.
    * Returns a reference to this image.
    *
    * \param lowerBound    Lower bound of the truncation range.
    * \param upperBound    Upper bound of the truncation range.
    *
    * The specified truncation bounds are converted to the sample data type of
    * this image, using pixel traits primitives.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   template <typename T>
   GenericImage& Truncate( T lowerBound, T upperBound,
                           const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return *this;

      EnsureUnique();

      size_type N = size_type( r.Width() )*size_type( r.Height() );
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Truncating samples", N*(1 + lastChannel - firstChannel) );

      sample b0 = P::ToSample( lowerBound );
      sample b1 = P::ToSample( upperBound );
      if ( b1 < b0 )
         pcl::Swap( b0, b1 );

      if ( r == Bounds() )
         for ( int i = firstChannel; i <= lastChannel; ++i, m_status += N )
         {
            for ( sample* f = m_pixelData[i], * f1 = f+N; f < f1; ++f )
               if ( *f < b0 )
                  *f = b0;
               else if ( b1 < *f )
                  *f = b1;
         }
      else
         for ( int i = firstChannel, w = r.Width(), h = r.Height(); i <= lastChannel; ++i, m_status += N )
         {
            sample* f = PixelAddress( r.LeftTop(), i );
            for ( int j = 0; j < h; ++j, f += m_width-w )
               for ( sample* f1 = f+w; f < f1; ++f )
                  if ( *f < b0 )
                     *f = b0;
                  else if ( b1 < *f )
                     *f = b1;
         }

      return *this;
   }

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * truncated to the specified range of values.
    *
    * \param lowerBound    Lower bound of the truncation range.
    * \param upperBound    Upper bound of the truncation range.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   template <typename T>
   GenericImage Truncated( T lowerBound, T upperBound,
                           const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Truncate( lowerBound, upperBound );
      return result;
   }

   /*!
    * Truncates a subset of pixel samples to the native range of the image.
    * Returns a reference to this image.
    *
    * This member function is a convenience shortcut for:
    *
    * \code
    * Truncate( P::MinSampleValue(), P::MaxSampleValue(), rect, firstChannel, lastChannel );
    * \endcode
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * Note that this function only makes sense for floating point real or
    * complex images, where pixel samples can take arbitrary values. Integer
    * pixel samples are constrained to their native range by nature, so calling
    * this function for integer images has no effect.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   GenericImage& Truncate( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Truncate( P::MinSampleValue(), P::MaxSampleValue(), rect, firstChannel, lastChannel );
   }

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * truncated to the native range of the image.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   GenericImage Truncated( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Truncate();
      return result;
   }

   /*!
    * Rescales a subset of pixel samples to the specified range of values.
    * Returns a reference to this image.
    *
    * \param lowerBound    Lower bound of the rescaling range.
    * \param upperBound    Upper bound of the rescaling range.
    *
    * The specified rescaling bounds are converted to the sample data type of
    * this image, using pixel traits primitives.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * The rescaling operation is as follows. Given the following two ranges:
    *
    * \li \c m, \c M : The range of existing sample values in the selected set of
    *           pixel samples,
    * \li \c r0, \c r1 : The rescaling range,
    *
    * the rescaled value \c r for a given sample \c s is:
    *
    * <pre>
    * r = r0 + (s - m)*(r1 - r0)/(M - m)
    * </pre>
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   template <typename T>
   GenericImage& Rescale( T lowerBound, T upperBound,
                          const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return *this;

      size_type N = size_type( r.Width() )*size_type( r.Height() );
      size_type Ns = N*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Rescaling samples", Ns );

      sample b0 = P::ToSample( lowerBound );
      sample b1 = P::ToSample( upperBound );
      if ( b1 < b0 )
         pcl::Swap( b0, b1 );

      sample v0, v1;
      GetExtremePixelValues( v0, v1, r, firstChannel, lastChannel );

      if ( v0 == b0 && v1 == b1 )
      {
         m_status += Ns;
         return *this;
      }

      EnsureUnique();

      double d = 0;
      if ( b0 != b1 )
         if ( v0 != v1 )
            d = (double( b1 ) - double( b0 ))/(double( v1 ) - double( v0 ));

      if ( r == Bounds() )
         for ( int i = firstChannel; i <= lastChannel; ++i, m_status += N )
         {
            if ( v0 != v1 )
            {
               if ( b0 != b1 )
               {
                  if ( b0 == sample( 0 ) )
                     for ( sample* f = m_pixelData[i], * f1 = f+N; f < f1; ++f )
                        *f = P::FloatToSample( d*(*f - v0) );
                  else
                     for ( sample* f = m_pixelData[i], * f1 = f+N; f < f1; ++f )
                        *f = P::FloatToSample( d*(*f - v0) + b0 );
               }
               else
                  P::Fill( m_pixelData[i], b0, N );
            }
            else
               P::Fill( m_pixelData[i], pcl::Range( v0, b0, b1 ), N );
         }
      else
         for ( int i = firstChannel, w = r.Width(), h = r.Height(); i <= lastChannel; ++i, m_status += N )
         {
            sample* f = PixelAddress( r.LeftTop(), i );

            if ( v0 != v1 )
            {
               if ( b0 != b1 )
               {
                  if ( b0 == sample( 0 ) )
                     for ( int j = 0; j < h; ++j, f += m_width-w )
                        for ( sample* f1 = f+w; f < f1; ++f )
                           *f = P::FloatToSample( d*(*f - v0) );
                  else
                     for ( int j = 0; j < h; ++j, f += m_width-w )
                        for ( sample* f1 = f+w; f < f1; ++f )
                           *f = P::FloatToSample( d*(*f - v0) + b0 );
               }
               else
                  for ( int j = 0; j < h; ++j, f += m_width )
                     P::Fill( f, b0, w );
            }
            else
            {
               sample v = pcl::Range( v0, b0, b1 );
               for ( int j = 0; j < h; ++j, f += m_width )
                  P::Fill( f, v, w );
            }
         }

      return *this;
   }

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * rescaled to the specified range of values.
    *
    * \param lowerBound    Lower bound of the rescaling range.
    * \param upperBound    Upper bound of the rescaling range.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   template <typename T>
   GenericImage Rescaled( T lowerBound, T upperBound,
                          const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Rescale( lowerBound, upperBound );
      return result;
   }

   /*!
    * Rescales a subset of pixel samples to the native range of the image.
    * Returns a reference to this image.
    *
    * This member function is a convenience shortcut for:
    *
    * \code
    * Rescale( P::MinSampleValue(), P::MaxSampleValue(), rect, firstChannel, lastChannel );
    * \endcode
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   GenericImage& Rescale( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Rescale( P::MinSampleValue(), P::MaxSampleValue(), rect, firstChannel, lastChannel );
   }

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * rescaled to the native range of the image.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   GenericImage Rescaled( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Rescale();
      return result;
   }

   /*!
    * Normalizes a subset of pixel samples to the specified range of values.
    * Returns a reference to this image.
    *
    * \param lowerBound    Lower bound of the normalizing range.
    * \param upperBound    Upper bound of the normalizing range.
    *
    * The specified normalizing bounds are converted to the sample data type of
    * this image, using pixel traits primitives.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * Normalization is a conditional rescaling operation: Pixel sample values
    * are rescaled to the specified normalizing range only if the existing
    * extreme sample values exceed that range. Given the following two ranges:
    *
    * \li \c m, \c M : The range of existing sample values in the selected set of
    *           pixel samples,
    * \li \c n0, \c n1 : The normalizing range,
    *
    * the normalized value \c n for a given sample \c s is:
    *
    * <pre>
    * if m < n0 or M > n1
    *    n = n0 + (s - m)*(n1 - n0)/(M - m)
    * else
    *    n = s
    * endif
    * </pre>
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   template <typename T>
   GenericImage& Normalize( T lowerBound, T upperBound,
                            const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return *this;

      size_type N = size_type( r.Width() )*size_type( r.Height() );
      size_type Ns = N*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Normalizing samples", Ns );

      sample b0 = P::ToSample( lowerBound );
      sample b1 = P::ToSample( upperBound );
      if ( b1 < b0 )
         pcl::Swap( b0, b1 );

      sample v0, v1;
      GetExtremePixelValues( v0, v1, r, firstChannel, lastChannel );

      if ( v0 >= b0 && v1 <= b1 )
      {
         m_status += Ns;
         return *this;
      }

      EnsureUnique();

      double d = 0;
      if ( b0 != b1 )
         if ( v0 != v1 )
            d = (double( b1 ) - double( b0 ))/(double( v1 ) - double( v0 ));

      if ( r == Bounds() )
         for ( int i = firstChannel; i <= lastChannel; ++i, m_status += N )
         {
            if ( v0 != v1 )
            {
               if ( b0 != b1 )
               {
                  if ( b0 == sample( 0 ) )
                     for ( sample* f = m_pixelData[i], * f1 = f+N; f < f1; ++f )
                        *f = P::FloatToSample( d*(*f - v0) );
                  else
                     for ( sample* f = m_pixelData[i], * f1 = f+N; f < f1; ++f )
                        *f = P::FloatToSample( d*(*f - v0) + b0 );
               }
               else
                  P::Fill( m_pixelData[i], b0, N );
            }
            else
               P::Fill( m_pixelData[i], pcl::Range( v0, b0, b1 ), N );
         }
      else
         for ( int i = firstChannel, w = r.Width(), h = r.Height(); i <= lastChannel; ++i, m_status += N )
         {
            sample* f = PixelAddress( r.LeftTop(), i );

            if ( v0 != v1 )
            {
               if ( b0 != b1 )
               {
                  if ( b0 == sample( 0 ) )
                     for ( int j = 0; j < h; ++j, f += m_width-w )
                        for ( sample* f1 = f+w; f < f1; ++f )
                           *f = P::FloatToSample( d*(*f - v0) );
                  else
                     for ( int j = 0; j < h; ++j, f += m_width-w )
                        for ( sample* f1 = f+w; f < f1; ++f )
                           *f = P::FloatToSample( d*(*f - v0) + b0 );
               }
               else
                  for ( int j = 0; j < h; ++j, f += m_width )
                     P::Fill( f, b0, w );
            }
            else
            {
               sample v = pcl::Range( v0, b0, b1 );
               for ( int j = 0; j < h; ++j, f += m_width )
                  P::Fill( f, v, w );
            }
         }

      return *this;
   }

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * normalized to the specified range of values.
    *
    * \param lowerBound    Lower bound of the normalizing range.
    * \param upperBound    Upper bound of the normalizing range.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   template <typename T>
   GenericImage Normalized( T lowerBound, T upperBound,
                            const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Normalize( lowerBound, upperBound );
      return result;
   }

   /*!
    * Normalizes a subset of pixel samples to the native range of the image.
    * Returns a reference to this image.
    *
    * This member function is a convenience shortcut for:
    *
    * \code
    * Normalize( P::MinSampleValue(), P::MaxSampleValue(), rect, firstChannel, lastChannel );
    * \endcode
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   GenericImage& Normalize( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Normalize( P::MinSampleValue(), P::MaxSampleValue(), rect, firstChannel, lastChannel );
   }

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * normalized to the native range of the image.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   GenericImage Normalized( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Normalize();
      return result;
   }

   /*!
    * Binarizes a subset of pixel samples with respect to the specified
    * \a threshold value. Returns a reference to this image.
    *
    * \param threshold  Binarizing threshold.
    *
    * The specified binarizing threshold is converted to the sample data type
    * of this image, using pixel traits primitives.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * The binarized value \c b of a sample \c s is given by:
    *
    * <pre>
    * if s < t
    *    b = 0
    * else
    *    b = 1
    * endif
    * </pre>
    *
    * where \c t is the binarizing threshold value, and 0 and 1 represent,
    * respectively, the minimum and maximum pixel sample values in the native
    * range of the image.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   template <typename T>
   GenericImage& Binarize( T threshold,
                           const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return *this;

      EnsureUnique();

      size_type N = size_type( r.Width() )*size_type( r.Height() );
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Binarizing samples", N*(1 + lastChannel - firstChannel) );

      sample t = P::ToSample( threshold );

      if ( r == Bounds() )
         for ( int i = firstChannel; i <= lastChannel; ++i, m_status += N )
            for ( sample* f = m_pixelData[i], * f1 = f+N; f < f1; ++f )
               *f = (*f < t) ? P::MinSampleValue() : P::MaxSampleValue();
      else
         for ( int c = firstChannel, w = r.Width(), h = r.Height(); c <= lastChannel; ++c, m_status += N )
         {
            sample* f = PixelAddress( r.LeftTop(), c );
            for ( int j = 0; j < h; ++j, f += m_width-w )
               for ( sample* f1 = f+w; f < f1; ++f )
                  *f = (*f < t) ? P::MinSampleValue() : P::MaxSampleValue();
         }

      return *this;
   }

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * binarized with respect to the specified \a threshold value.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   template <typename T>
   GenericImage Binarized( T threshold,
                           const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Binarize( threshold );
      return result;
   }

   /*!
    * Binarizes a subset of pixel samples with respect to the central value of
    * the native range of the image. Returns a reference to this image.
    *
    * This member function is a convenience shortcut for:
    *
    * \code
    * Binarize( (P::MinSampleValue() + P::MaxSampleValue())/2, rect, firstChannel, lastChannel );
    * \endcode
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   GenericImage& Binarize( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Binarize( (P::MinSampleValue() + P::MaxSampleValue())/2, rect, firstChannel, lastChannel );
   }

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * binarized with respect to the central value of the native range of the
    * image.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   GenericImage Binarized( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Binarize();
      return result;
   }

   /*!
    * Replaces a subset of pixel samples with their absolute values. Returns a
    * reference to this image.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Note that this member function is a no-op for unsigned integer
    * images (e.g., UInt8Image, UInt16Image and UInt32Image).
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
   */
   GenericImage& SetAbsoluteValue( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return *this;

      EnsureUnique();

      size_type N = size_type( r.Width() )*size_type( r.Height() );
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Computing absolute value", N*(1 + lastChannel - firstChannel) );

      if ( r == Bounds() )
         for ( int i = firstChannel; i <= lastChannel; ++i, m_status += N )
            for ( sample* f = m_pixelData[i], * f1 = f+N; f < f1; ++f )
               *f = pcl::Abs( *f );
      else
         for ( int i = firstChannel, w = r.Width(), h = r.Height(); i <= lastChannel; ++i, m_status += N )
         {
            sample* f = PixelAddress( r.LeftTop(), i );
            for ( int j = 0; j < h; ++j, f += m_width-w )
               for ( sample* f1 = f+w; f < f1; ++f )
                  *f = pcl::Abs( *f );
         }

      return *this;
   }

   /*!
    * A synonym for SetAbsoluteValue().
    */
   GenericImage& Abs( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return SetAbsoluteValue( rect, firstChannel, lastChannel );
   }

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * replaced with their absolute values.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   GenericImage AbsoluteValue( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.SetAbsoluteValue();
      return result;
   }

   // -------------------------------------------------------------------------

   /*
    * Implementation of Apply( scalar ) member functions. We have to have a
    * separate implementation function to prevent infinite recursion in several
    * template specializations of the Apply() member function.
    */
   template <typename T>
   GenericImage& ApplyScalar( T scalar, image_op op = ImageOp::Mov,
                              const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( op == ImageOp::Div )
         if ( 1 + scalar == 1 )
            throw Error( "Division by zero or insignificant scalar" );

      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return *this;

      EnsureUnique();

      size_type N = size_type( r.Width() )*size_type( r.Height() );
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Applying scalar: "
                              + ImageOp::Id( op )
                              + ' ' + String( scalar ), N*(1 + lastChannel - firstChannel) );

      if ( r == Bounds() )
         for ( int i = firstChannel; i <= lastChannel; ++i, m_status += N )
         {
            sample* f = m_pixelData[i];
#define ITERATE( Op )                                       \
            for ( sample* f1 = f+N; f < f1; ++f )           \
               P::Op( *f, scalar )
            switch ( op )
            {
            case ImageOp::Mov:         ITERATE( Mov         ); break;
            case ImageOp::Add:         ITERATE( Add         ); break;
            case ImageOp::Sub:         ITERATE( Sub         ); break;
            case ImageOp::Mul:         ITERATE( Mul         ); break;
            case ImageOp::Div:         ITERATE( Div         ); break;
            case ImageOp::Pow:         ITERATE( Pow         ); break;
            case ImageOp::Dif:         ITERATE( Dif         ); break;
            case ImageOp::Min:         ITERATE( Min         ); break;
            case ImageOp::Max:         ITERATE( Max         ); break;
            case ImageOp::Not:         ITERATE( Not         ); break;
            case ImageOp::Or:          ITERATE( Or          ); break;
            case ImageOp::Nor:         ITERATE( Nor         ); break;
            case ImageOp::And:         ITERATE( And         ); break;
            case ImageOp::Nand:        ITERATE( Nand        ); break;
            case ImageOp::Xor:         ITERATE( Xor         ); break;
            case ImageOp::Xnor:        ITERATE( Xnor        ); break;
            case ImageOp::ColorBurn:   ITERATE( ColorBurn   ); break;
            case ImageOp::LinearBurn:  ITERATE( LinearBurn  ); break;
            case ImageOp::Screen:      ITERATE( Screen      ); break;
            case ImageOp::ColorDodge:  ITERATE( ColorDodge  ); break;
            case ImageOp::Overlay:     ITERATE( Overlay     ); break;
            case ImageOp::SoftLight:   ITERATE( SoftLight   ); break;
            case ImageOp::HardLight:   ITERATE( HardLight   ); break;
            case ImageOp::VividLight:  ITERATE( VividLight  ); break;
            case ImageOp::LinearLight: ITERATE( LinearLight ); break;
            case ImageOp::PinLight:    ITERATE( PinLight    ); break;
            case ImageOp::Exclusion:   ITERATE( Exclusion   ); break;
            default: break;
            }
#undef ITERATE
         }
      else
         for ( int i = firstChannel, w = r.Width(), h = r.Height(); i <= lastChannel; ++i, m_status += N )
         {
            sample* f = PixelAddress( r.LeftTop(), i );
#define ITERATE( Op )                                       \
            for ( int j = 0; j < h; ++j, f += m_width-w )   \
               for ( sample* f1 = f+w; f < f1; ++f )        \
                  P::Op( *f, scalar )
            switch ( op )
            {
            case ImageOp::Mov:         ITERATE( Mov         ); break;
            case ImageOp::Add:         ITERATE( Add         ); break;
            case ImageOp::Sub:         ITERATE( Sub         ); break;
            case ImageOp::Mul:         ITERATE( Mul         ); break;
            case ImageOp::Div:         ITERATE( Div         ); break;
            case ImageOp::Pow:         ITERATE( Pow         ); break;
            case ImageOp::Dif:         ITERATE( Dif         ); break;
            case ImageOp::Min:         ITERATE( Min         ); break;
            case ImageOp::Max:         ITERATE( Max         ); break;
            case ImageOp::Not:         ITERATE( Not         ); break;
            case ImageOp::Or:          ITERATE( Or          ); break;
            case ImageOp::Nor:         ITERATE( Nor         ); break;
            case ImageOp::And:         ITERATE( And         ); break;
            case ImageOp::Nand:        ITERATE( Nand        ); break;
            case ImageOp::Xor:         ITERATE( Xor         ); break;
            case ImageOp::Xnor:        ITERATE( Xnor        ); break;
            case ImageOp::ColorBurn:   ITERATE( ColorBurn   ); break;
            case ImageOp::LinearBurn:  ITERATE( LinearBurn  ); break;
            case ImageOp::Screen:      ITERATE( Screen      ); break;
            case ImageOp::ColorDodge:  ITERATE( ColorDodge  ); break;
            case ImageOp::Overlay:     ITERATE( Overlay     ); break;
            case ImageOp::SoftLight:   ITERATE( SoftLight   ); break;
            case ImageOp::HardLight:   ITERATE( HardLight   ); break;
            case ImageOp::VividLight:  ITERATE( VividLight  ); break;
            case ImageOp::LinearLight: ITERATE( LinearLight ); break;
            case ImageOp::PinLight:    ITERATE( PinLight    ); break;
            case ImageOp::Exclusion:   ITERATE( Exclusion   ); break;
            default: break;
            }
#undef ITERATE
         }

      return *this;
   }

   /*!
    * Replaces a subset of pixel samples with the result of an arithmetic,
    * bitwise logical or pixel composition operation with a scalar. Returns a
    * reference to this image.
    *
    * \param scalar  Right-hand operand value.
    *
    * \param op      Identifies an arithmetic, bitwise logical or pixel
    *                composition operator. For a list of supported operators,
    *                see the ImageOp namespace. The default value is
    *                ImageOp::Mov (replace target with source).
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * If the specified \a scalar is zero or insignificant, and the \a op
    * argument is ImageOp::Div, this function throws an Error exception to
    * prevent a division by zero.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   template <typename T>
   GenericImage& Apply( T scalar, image_op op = ImageOp::Mov,
                        const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return ApplyScalar( scalar, op, rect, firstChannel, lastChannel );
   }

   GenericImage& Apply( float scalar, image_op op = ImageOp::Mov,
                        const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( ImageOp::IsArithmeticOperator( op ) || ImageOp::IsPixelCompositionOperator( op ) )
         return ApplyScalar( scalar, op, rect, firstChannel, lastChannel );
      return ApplyScalar( P::ToSample( scalar ), op, rect, firstChannel, lastChannel );
   }

   GenericImage& Apply( double scalar, image_op op = ImageOp::Mov,
                        const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( ImageOp::IsArithmeticOperator( op ) || ImageOp::IsPixelCompositionOperator( op ) )
         return ApplyScalar( scalar, op, rect, firstChannel, lastChannel );
      return ApplyScalar( P::ToSample( scalar ), op, rect, firstChannel, lastChannel );
   }

   GenericImage& Apply( pcl::Complex<float> scalar, image_op op = ImageOp::Mov,
                        const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( ImageOp::IsArithmeticOperator( op ) || ImageOp::IsPixelCompositionOperator( op ) )
         return ApplyScalar( scalar, op, rect, firstChannel, lastChannel );
      return ApplyScalar( P::ToSample( scalar ), op, rect, firstChannel, lastChannel );
   }

   GenericImage& Apply( pcl::Complex<double> scalar, image_op op = ImageOp::Mov,
                        const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( ImageOp::IsArithmeticOperator( op ) || ImageOp::IsPixelCompositionOperator( op ) )
         return ApplyScalar( scalar, op, rect, firstChannel, lastChannel );
      return ApplyScalar( P::ToSample( scalar ), op, rect, firstChannel, lastChannel );
   }

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * performs an arithmetic or bitwise logical operation with a scalar.
    * Returns the resulting image.
    *
    * \param scalar  Right-hand operand value.
    *
    * \param op      Identifies an arithmetic or bitwise logical operator. For
    *                a list of supported operators, see the ImageOp namespace.
    *                The default value is ImageOp::Mov (replace target with
    *                source).
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   template <typename T>
   GenericImage Applied( T scalar, image_op op = ImageOp::Mov,
                         const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Apply( scalar, op );
      return result;
   }

   // -------------------------------------------------------------------------

   /*!
    * Performs an arithmetic or bitwise logical operation between a subset of
    * pixel samples in this image and the corresponding samples of another
    * operand \a image. Returns a reference to this image.
    *
    * \param image   Right-hand operand image.
    *
    * \param op      Identifies an arithmetic or bitwise logical operator. For
    *                a list of supported operators, see the ImageOp namespace.
    *                The default value is ImageOp::Mov (replace target with
    *                source).
    *
    * \param point   Target point in image coordinates. This is the upper left
    *                corner of the target rectangular region where the source
    *                \a image will be applied to this image. If one or both
    *                point coordinates are equal to \c int_max (the maximum
    *                possible value of type \c int), then the current anchor
    *                point selection will be used. The default value is
    *                Point( int_max ).
    *
    * \param channel Target channel index (zero-based). This is the index of
    *                the first channel where source pixel samples will be
    *                applied. If this parameter is negative, then the currently
    *                selected channel will be used. The default value is -1.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * The specified operation is performed with samples in the intersection
    * between the specified (or implicit) rectangular selections for both
    * images. Each channel in the specified channel range selection for this
    * image is operated with its counterpart channel in the right-hand operand
    * \a image, starting from the first selected channel in the right-hand
    * operand \a image.
    *
    * The operation may involve conversions to the appropriate floating point
    * data type for intermediate results, or LUT table lookups, as necessary
    * to ensure preservation of the full native range of the image. Floating
    * point conversions may take place even if both images share the same
    * sample data type, depending on the selected operator.
    *
    * \note Increments the status monitoring object by the number of pixel
    * samples in the intersection between both operand images.
    */
   template <class P1>
   GenericImage& Apply( const GenericImage<P1>& image, image_op op = ImageOp::Mov,
                        const Point& point = Point( int_max ), int channel = -1,
                        const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      Rect r = rect;
      if ( !image.ParseSelection( r, firstChannel, lastChannel ) )
         return *this;

      if ( !ParseChannel( channel ) )
         return *this;

      Point p = point;
      if ( p.x == int_max || p.y == int_max )
         p = m_point;

      if ( p.x < 0 )
      {
         if ( (r.x0 -= p.x) >= r.x1 )
            return *this;
         p.x = 0;
      }
      else if ( p.x >= m_width )
         return *this;

      if ( p.y < 0 )
      {
         if ( (r.y0 -= p.y) >= r.y1 )
            return *this;
         p.y = 0;
      }
      else if ( p.y >= m_height )
         return *this;

      r.ResizeTo( pcl::Min( m_width - p.x, r.Width() ),
                  pcl::Min( m_height - p.y, r.Height() ) );

      lastChannel = pcl::Min( lastChannel, firstChannel + m_numberOfChannels - channel - 1 );

      EnsureUnique();

      size_type N = size_type( r.Width() )*size_type( r.Height() );
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Applying image, op=" + ImageOp::Id( op ), N*(1 + lastChannel - firstChannel) );

      if ( r == Bounds() && r == image.Bounds() )
         for ( int i = channel, j = firstChannel; j <= lastChannel; ++i, ++j, m_status += N )
         {
            sample* f = m_pixelData[i];
            const typename P1::sample* g = image[j];

#define ITERATE( Op )                                       \
            for ( sample* f1 = f+N; f < f1; ++f, ++g )      \
               P::Op( *f, *g )
            switch ( op )
            {
            case ImageOp::Mov:
               P::Copy( f, g, N );
               break;
            case ImageOp::Min:
               P::CopyMin( f, g, N );
               break;
            case ImageOp::Max:
               P::CopyMax( f, g, N );
               break;
            default:
               switch ( op )
               {
               case ImageOp::Add:         ITERATE( Add         ); break;
               case ImageOp::Sub:         ITERATE( Sub         ); break;
               case ImageOp::Mul:         ITERATE( Mul         ); break;
               case ImageOp::Div:
                  for ( sample* f1 = f+N; f < f1; ++f, ++g )
                     if ( *g != 0 )
                        P::Div( *f, *g );
                     else
                        *f = P::MaxSampleValue();
                  break;
               case ImageOp::Pow:         ITERATE( Pow         ); break;
               case ImageOp::Dif:         ITERATE( Dif         ); break;
               case ImageOp::Not:         ITERATE( Not         ); break;
               case ImageOp::Or:          ITERATE( Or          ); break;
               case ImageOp::Nor:         ITERATE( Nor         ); break;
               case ImageOp::And:         ITERATE( And         ); break;
               case ImageOp::Nand:        ITERATE( Nand        ); break;
               case ImageOp::Xor:         ITERATE( Xor         ); break;
               case ImageOp::Xnor:        ITERATE( Xnor        ); break;
               case ImageOp::ColorBurn:   ITERATE( ColorBurn   ); break;
               case ImageOp::LinearBurn:  ITERATE( LinearBurn  ); break;
               case ImageOp::Screen:      ITERATE( Screen      ); break;
               case ImageOp::ColorDodge:  ITERATE( ColorDodge  ); break;
               case ImageOp::Overlay:     ITERATE( Overlay     ); break;
               case ImageOp::SoftLight:   ITERATE( SoftLight   ); break;
               case ImageOp::HardLight:   ITERATE( HardLight   ); break;
               case ImageOp::VividLight:  ITERATE( VividLight  ); break;
               case ImageOp::LinearLight: ITERATE( LinearLight ); break;
               case ImageOp::PinLight:    ITERATE( PinLight    ); break;
               case ImageOp::Exclusion:   ITERATE( Exclusion   ); break;
               default: break;
               }
#undef ITERATE
               break;
            }
         }
      else
         for ( int i = channel, j = firstChannel, w = r.Width(), h = r.Height(); j <= lastChannel; ++i, ++j, m_status += N )
         {
            sample* f = PixelAddress( p, i );
            const typename P1::sample* g = image.PixelAddress( r.LeftTop(), j );

#define ITERATE( Op )                                                            \
            for ( int k = 0; k < h; ++k, f += m_width-w, g += image.Width()-w )  \
               for ( sample* f1 = f+w; f < f1; ++f, ++g )                        \
                  P::Op( *f, *g )
            switch ( op )
            {
            case ImageOp::Mov:
               for ( int k = 0; k < h; ++k, f += m_width, g += image.Width() )
                  P::Copy( f, g, w );
               break;
            case ImageOp::Min:
               for ( int k = 0; k < h; ++k, f += m_width, g += image.Width() )
                  P::CopyMin( f, g, w );
               break;
            case ImageOp::Max:
               for ( int k = 0; k < h; ++k, f += m_width, g += image.Width() )
                  P::CopyMax( f, g, w );
               break;
            case ImageOp::Add:         ITERATE( Add         ); break;
            case ImageOp::Sub:         ITERATE( Sub         ); break;
            case ImageOp::Mul:         ITERATE( Mul         ); break;
            case ImageOp::Div:
               for ( int k = 0; k < h; ++k, f += m_width-w, g += image.Width()-w )
                  for ( sample* f1 = f+w; f < f1; ++f, ++g )
                     if ( *g != 0 )
                        P::Div( *f, *g );
                     else
                        *f = P::MaxSampleValue();
               break;
            case ImageOp::Pow:         ITERATE( Pow         ); break;
            case ImageOp::Dif:         ITERATE( Dif         ); break;
            case ImageOp::Not:         ITERATE( Not         ); break;
            case ImageOp::Or:          ITERATE( Or          ); break;
            case ImageOp::Nor:         ITERATE( Nor         ); break;
            case ImageOp::And:         ITERATE( And         ); break;
            case ImageOp::Nand:        ITERATE( Nand        ); break;
            case ImageOp::Xor:         ITERATE( Xor         ); break;
            case ImageOp::Xnor:        ITERATE( Xnor        ); break;
            case ImageOp::ColorBurn:   ITERATE( ColorBurn   ); break;
            case ImageOp::LinearBurn:  ITERATE( LinearBurn  ); break;
            case ImageOp::Screen:      ITERATE( Screen      ); break;
            case ImageOp::ColorDodge:  ITERATE( ColorDodge  ); break;
            case ImageOp::Overlay:     ITERATE( Overlay     ); break;
            case ImageOp::SoftLight:   ITERATE( SoftLight   ); break;
            case ImageOp::HardLight:   ITERATE( HardLight   ); break;
            case ImageOp::VividLight:  ITERATE( VividLight  ); break;
            case ImageOp::LinearLight: ITERATE( LinearLight ); break;
            case ImageOp::PinLight:    ITERATE( PinLight    ); break;
            case ImageOp::Exclusion:   ITERATE( Exclusion   ); break;
               break;
            default:
               break;
            }
#undef ITERATE
         }

      return *this;
   }

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * performs an arithmetic or bitwise logical operation with the
    * corresponding samples of another \a image. Returns the resulting image.
    *
    * For information on the parameters of this member function, see the
    * documentation for Apply().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   template <class P1>
   GenericImage Applied( const GenericImage<P1>& image, image_op op = ImageOp::Mov,
                         const Point& point = Point( int_max ), int channel = -1,
                         const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      int c0 = (channel < 0) ? m_channel : channel;
      int c1 = c0 + ((firstChannel < 0) ? image.FirstSelectedChannel() : firstChannel);
      int c2 = c0 + ((lastChannel < 0) ? image.LastSelectedChannel() : lastChannel);
      GenericImage result( *this, rect.MovedTo( point ), c1, c2 );
      (void)result.Apply( image, op, Point( 0 ), 0, rect, firstChannel, lastChannel );
      return result;
   }

   // -------------------------------------------------------------------------

   /*!
    * Applies an image transformation to a subset of pixel samples. Returns a
    * reference to this image.
    *
    * \param transformation   An image transformation that will be applied to
    *                         the selected subset of pixel samples.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * If the applied \a transformation is bidirectional, then this function
    * represents an <em>inverse transformation</em>, usually requiring a
    * previous <em>direct transformation</em>. See the Transform() member
    * function.
    *
    * Bidirectional transformations such as FourierTransform and
    * ATrousWaveletTransform usually ignore the specified (or implicit)
    * selections of a rectangular region and channel range. Other
    * non-invertible transformations, especially geometric transformations such
    * as Resample or Rotation, also ignore pixel sample selections.
    *
    * \note In general, the applied transformation increments the status
    * monitoring object proportionally to the number of selected pixel samples.
    * However, the specific increments depend on the applied transformations.
    *
    * \warning This member function is <b>not thread safe</b>, even if the
    * subset of pixel samples is selected directly with the \a rect,
    * \a firstChannel and \a lastChannel parameters. This is because image
    * transformations always use the current image selection, which this
    * function changes temporarily.
    *
    * \sa ImageTransformation
    */
   GenericImage& Apply( const ImageTransformation& transformation,
                        const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 );
   // ### Implemented in pcl/ImageTransformation.h (because of mutual dependencies)

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * applies an image transformation to a subset of pixel samples. Returns the
    * resulting image.
    *
    * \param transformation   An image transformation that will be applied to
    *                         the selected subset of pixel samples.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill(). For information on image transformations,
    * see Apply( const ImageTransformation& ) and ImageTransformation.
    *
    * \warning This member function is <b>not thread safe</b>, even if the
    * subset of pixel samples is selected directly with the \a rect,
    * \a firstChannel and \a lastChannel parameters. This is because image
    * transformations always use the current image selection, which this
    * function changes temporarily.
    */
   GenericImage Applied( const ImageTransformation& transformation,
                         const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Apply( transformation );
      return result;
   }

   /*!
    * Applies a \e direct image transformation to a subset of pixel samples.
    *
    * \param transform  A bidirectional image transformation that will be
    *                   applied to the selected subset of pixel samples.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * \note In general, the applied transformation increments the status
    * monitoring object proportionally to the number of selected pixel samples.
    * However, the specific increments depend on the applied transformations.
    *
    * \warning This member function is <b>not thread safe</b>, even if the
    * subset of pixel samples is selected directly with the \a rect,
    * \a firstChannel and \a lastChannel parameters. This is because image
    * transformations always use the current image selection, which this
    * function changes temporarily.
    *
    * \sa ImageTransformation
    */
   void Transform( BidirectionalImageTransformation& transform,
                   const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const;
   // ### Implemented in pcl/ImageTransformation.h (because of mutual dependencies)

   // -------------------------------------------------------------------------

#ifndef __PCL_IMAGE_NO_BITMAP

   /*!
    * Blends a rectangular region of a 32-bit RGBA \a bitmap with this image.
    * Returns a reference to this image.
    *
    * \param bitmap  Reference to a constant source bitmap.
    *
    * \param point   Target point in image coordinates. This is the upper left
    *                corner of the target rectangular region where the source
    *                \a bitmap will be applied to this image. If one or both
    *                point coordinates are equal to \c int_max (the maximum
    *                possible value of type \c int), then the current anchor
    *                point selection will be used. The default value is
    *                Point( int_max ).
    *
    * \param rect    Source rectangular region in bitmap coordinates. If this
    *                parameter defines an empty rectangle, the whole source
    *                bitmap will be applied. If the specified rectangle is not
    *                empty and extends beyond image boundaries, only the
    *                intersection with the source bitmap will be used. If that
    *                intersection does not exist, then this member function has
    *                no effect. The default value is an empty rectangle.
    *
    * The blending operation involves only nominal image channels. The alpha
    * channel of the source bitmap defines a transparency mask for combining
    * bitmap pixel values and image pixel sample values. The resulting pixel
    * sample value \c V1 after the blending operation is given in the
    * normalized real range [0,1] by the following expression:
    *
    * <pre>
    * V1 = V*(1 - A/255) + B * A/255
    * </pre>
    *
    * where \c V is the initial pixel value of this image, \c A is the alpha
    * value of the source bitmap pixel, and \c B is one of the red, green or
    * blue values in the source bitmap pixel. The expression above is applied
    * respectively for each nominal sample of this image.
    *
    * \note Increments the status monitoring object by the number of pixels in
    * the intersection between this image and the specified rectangular region
    * of the source bitmap.
    */
   GenericImage& Blend( const Bitmap& bitmap,
                        const Point& point = Point( int_max ), const Rect& rect = Rect( 0 ) )
   {
      Rect r = rect;
      if ( r.IsRect() )
         r = bitmap.Bounds().Intersection( r );
      else
         r = bitmap.Bounds();
      if ( !r.IsRect() )
         return *this;

      Point p = point;
      if ( p.x == int_max || p.y == int_max )
         p = m_point;

      if ( p.x < 0 )
      {
         if ( (r.x0 -= p.x) >= r.x1 )
            return *this;
         p.x = 0;
      }
      else if ( p.x >= m_width )
         return *this;

      if ( p.y < 0 )
      {
         if ( (r.y0 -= p.y) >= r.y1 )
            return *this;
         p.y = 0;
      }
      else if ( p.y >= m_height )
         return *this;

      r.ResizeTo( pcl::Min( m_width - p.x, r.Width() ),
                  pcl::Min( m_height - p.y, r.Height() ) );

      bool hasAlpha = HasAlphaChannels();
      int w = r.Width();
      int h = r.Height();

      EnsureUnique();

      if ( IsColor() )
      {
         if ( m_status.IsInitializationEnabled() )
            m_status.Initialize( "Blending bitmap", size_type( w )*size_type( h ) );

         sample* fR = 0, * fG = 0, * fB = 0, * fA = 0;

         for ( int i = 0; i < h; ++i, ++p.y, m_status += w )
         {
            fR = PixelAddress( p, 0 );
            fG = PixelAddress( p, 1 );
            fB = PixelAddress( p, 2 );
            if ( hasAlpha )
               fA = PixelAddress( p, 3 );

            const RGBA* g = bitmap.ScanLine( r.y0 + i ) + r.x0;

            for ( int j = 0; j < w; ++j, ++fR, ++fG, ++fB, ++fA, ++g )
            {
               RGBA rgba = *g;

               uint8 A = Alpha( rgba );

               if ( hasAlpha )
               {
                  P::Mov( *fR, Red( rgba ) );
                  P::Mov( *fG, Green( rgba ) );
                  P::Mov( *fB, Blue( rgba ) );
                  P::Mov( *fA, A );
               }
               else if ( A != 0 )
               {
                  if ( A == 0xFF )
                  {
                     P::Mov( *fR, Red( rgba ) );
                     P::Mov( *fG, Green( rgba ) );
                     P::Mov( *fB, Blue( rgba ) );
                  }
                  else
                  {
                     double k = PTLUT->pDLUTA[A];
                     double k1 = PTLUT->p1DLUT8[A];
                     double v;
                     P::FromSample( v, *fR ), P::Mov( *fR, k*Red( rgba )   + k1*v );
                     P::FromSample( v, *fG ), P::Mov( *fG, k*Green( rgba ) + k1*v );
                     P::FromSample( v, *fB ), P::Mov( *fB, k*Blue( rgba )  + k1*v );
                  }
               }
            }
         }
      }
      else
      {
         if ( m_status.IsInitializationEnabled() )
            m_status.Initialize( "Blending grayscale bitmap", size_type( w )*size_type( h ) );

         sample* fK = 0, * fA = 0;

         for ( int i = 0; i < h; ++i, ++p.y, m_status += w )
         {
            fK = PixelAddress( p, 0 );
            if ( hasAlpha )
               fA = PixelAddress( p, 1 );

            const RGBA* g = bitmap.ScanLine( r.y0 + i ) + r.x0;

            for ( int j = 0; j < w; ++j, ++fK, ++fA, ++g )
            {
               RGBA rgba = *g;
               uint8 R = Red( rgba );
               uint8 G = Green( rgba );
               uint8 B = Blue( rgba );
               uint8 A = Alpha( rgba );

               double K = 0.5*(pcl::Min( pcl::Min( R, G ), B )
                             + pcl::Max( pcl::Max( R, G ), B ));

               if ( hasAlpha )
               {
                  P::Mov( *fK, K/255 );
                  P::Mov( *fA, A );
               }
               else if ( A != 0 )
               {
                  if ( A == 0xFF )
                     P::Mov( *fK, K/255 );
                  else
                  {
                     double v;
                     P::FromSample( v, *fK );
                     P::Mov( *fK, PTLUT->pDLUTA[A]*K + PTLUT->p1DLUT8[A]*v );
                  }
               }
            }
         }
      }

      return *this;
   }

   /*!
    * Returns a local duplicate of this image blended with a rectangular region
    * of a 32-bit RGBA \a bitmap.
    *
    * For information on the parameters of this member function, see the
    * documentation for Blend().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of pixels in the intersection between this image and the
    * specified rectangular region of the source bitmap.
    */
   GenericImage Blended( const Bitmap& bitmap,
                         const Point& point = Point( int_max ), const Rect& rect = Rect( 0 ) ) const
   {
      GenericImage result( *this, Bounds(), 0, m_numberOfChannels-1 );
      (void)result.Blend( bitmap, point, rect );
      return result;
   }

#endif   // !__PCL_IMAGE_NO_BITMAP

   // -------------------------------------------------------------------------

   /*!
    * Replaces a subset of pixel samples in this image with the specified
    * constant \a scalar. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( scalar, ImageOp::Mov, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <typename T>
   GenericImage& Move( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( scalar, ImageOp::Mov, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Move().
    */
   template <typename T>
   GenericImage& Mov( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Move( scalar, rect, firstChannel, lastChannel );
   }

   /*!
    * Adds the specified \a scalar to a subset of pixel samples in this image.
    * Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( scalar, ImageOp::Add, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <typename T>
   GenericImage& Add( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( scalar, ImageOp::Add, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Add().
    */
   template <typename T>
   GenericImage& operator +=( T scalar )
   {
      return Add( scalar );
   }

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * adds a \a scalar. Returns the resulting image.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   template <typename T>
   GenericImage Added( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Add( scalar );
      return result;
   }

   /*!
    * Subtracts the specified \a scalar from a subset of pixel samples in this
    * image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( scalar, ImageOp::Sub, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <typename T>
   GenericImage& Subtract( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( scalar, ImageOp::Sub, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Subtract().
    */
   template <typename T>
   GenericImage& Sub( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Subtract( scalar, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Subtract().
    */
   template <typename T>
   GenericImage& operator -=( T scalar )
   {
      return Subtract( scalar );
   }

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * subtracts a \a scalar. Returns the resulting image.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   template <typename T>
   GenericImage Subtracted( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Subtract( scalar );
      return result;
   }

   /*!
    * Multiplies a subset of pixel samples in this image by the specified
    * \a scalar. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( scalar, ImageOp::Mul, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <typename T>
   GenericImage& Multiply( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( scalar, ImageOp::Mul, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Multiply().
    */
   template <typename T>
   GenericImage& Mul( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Multiply( scalar, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Multiply().
    */
   template <typename T>
   GenericImage& operator *=( T scalar )
   {
      return Multiply( scalar );
   }

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * multiplies it by a \a scalar. Returns the resulting image.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   template <typename T>
   GenericImage Multiplied( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Multiply( scalar );
      return result;
   }

   /*!
    * Divides a subset of pixel samples in this image by the specified
    * \a scalar. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( scalar, ImageOp::Div, rect, firstChannel, lastChannel );
    * \endcode
    *
    * \warning Make sure that the specified \a scalar is not zero or
    * insignificant.
    */
   template <typename T>
   GenericImage& Divide( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( scalar, ImageOp::Div, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Divide().
    */
   template <typename T>
   GenericImage& Div( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Divide( scalar, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Divide().
    */
   template <typename T>
   GenericImage& operator /=( T scalar )
   {
      return Divide( scalar );
   }

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * divides it by a \a scalar. Returns the resulting image.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    *
    * \warning Make sure that the specified \a scalar is not zero or
    * insignificant.
    */
   template <typename T>
   GenericImage Divided( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Divide( scalar );
      return result;
   }

   /*!
    * Raises (exponentiation operator) a subset of pixel samples in this image
    * to the specified \a scalar. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( scalar, ImageOp::Pow, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <typename T>
   GenericImage& Raise( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( scalar, ImageOp::Pow, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Raise().
    */
   template <typename T>
   GenericImage& Pow( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Raise( scalar, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Raise().
    */
   template <typename T>
   GenericImage& operator ^=( T scalar )
   {
      return Raise( scalar );
   }

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * raises it to a \a scalar. Returns the resulting image.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   template <typename T>
   GenericImage Raised( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.Raise( scalar );
      return result;
   }

   /*!
    * Replaces a subset of pixel samples in this image with the absolute
    * values of their differences with the specified \a scalar. Returns a
    * reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( scalar, ImageOp::Dif, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <typename T>
   GenericImage& SetAbsoluteDifference( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( scalar, ImageOp::Dif, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for SetAbsoluteDifference().
    */
   template <typename T>
   GenericImage& Dif( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return SetAbsoluteDifference( scalar, rect, firstChannel, lastChannel );
   }

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * computes the absolute values of their differences with the specified
    * \a scalar. Returns the resulting image.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   template <typename T>
   GenericImage AbsoluteDifference( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.SetAbsoluteDifference( scalar );
      return result;
   }

   /*!
    * Replaces a subset of pixel samples in this image with the minimum of each
    * sample and the specified \a scalar. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( scalar, ImageOp::Min, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <typename T>
   GenericImage& SetMinimum( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( scalar, ImageOp::Min, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for SetMinimum().
    */
   template <typename T>
   GenericImage& Min( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return SetMinimum( scalar, rect, firstChannel, lastChannel );
   }

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * computes the minimum of each sample and the specified \a scalar. Returns
    * the resulting image.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   template <typename T>
   GenericImage Minimum( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.SetMinimum( scalar );
      return result;
   }

   /*!
    * Replaces a subset of pixel samples in this image with the maximum of each
    * sample and the specified \a scalar. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( scalar, ImageOp::Max, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <typename T>
   GenericImage& SetMaximum( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( scalar, ImageOp::Max, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for SetMaximum().
    */
   template <typename T>
   GenericImage& Max( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return SetMaximum( scalar, rect, firstChannel, lastChannel );
   }

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * computes the maximum of each sample and the specified \a scalar. Returns
    * the resulting image.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * \note Increments the status monitoring object of the returned image by
    * the number of selected pixel samples.
    */
   template <typename T>
   GenericImage Maximum( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      GenericImage result( *this, rect, firstChannel, lastChannel );
      (void)result.SetMaximum( scalar );
      return result;
   }

   /*!
    * Performs a bitwise OR (inclusive OR) operation between a subset of pixel
    * samples in this image and the specified \a scalar. Returns a reference to
    * this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( scalar, ImageOp::Or, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <typename T>
   GenericImage& Or( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( scalar, ImageOp::Or, rect, firstChannel, lastChannel );
   }

   /*!
    * Performs a bitwise AND operation between a subset of pixel samples in
    * this image and the specified \a scalar. Returns a reference to this
    * image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( scalar, ImageOp::And, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <typename T>
   GenericImage& And( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( scalar, ImageOp::And, rect, firstChannel, lastChannel );
   }

   /*!
    * Performs a bitwise XOR (exclusive OR) operation between a subset of pixel
    * samples in this image and the specified \a scalar. Returns a reference to
    * this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( scalar, ImageOp::Or, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <typename T>
   GenericImage& Xor( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( scalar, ImageOp::Xor, rect, firstChannel, lastChannel );
   }

   /*!
    * Performs a bitwise NOR (NOT inclusive OR) operation between a subset of
    * pixel samples in this image and the specified \a scalar. Returns a
    * reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( scalar, ImageOp::Nor, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <typename T>
   GenericImage& Nor( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( scalar, ImageOp::Nor, rect, firstChannel, lastChannel );
   }

   /*!
    * Performs a bitwise NAND (NOT AND) operation between a subset of pixel
    * samples in this image and the specified \a scalar. Returns a reference to
    * this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( scalar, ImageOp::Nand, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <typename T>
   GenericImage& Nand( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( scalar, ImageOp::Nand, rect, firstChannel, lastChannel );
   }

   /*!
    * Performs a bitwise XNOR (exclusive NOR) operation between a subset of
    * pixel samples in this image and the specified \a scalar. Returns a
    * reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( scalar, ImageOp::Xnor, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <typename T>
   GenericImage& Xnor( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( scalar, ImageOp::Xnor, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

   /*!
    * Replaces a subset of pixel samples in this image with samples from the
    * specified source \a image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( image, ImageOp::Mov, point, channel, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <class P1>
   GenericImage& Move( const GenericImage<P1>& image,
                       const Point& point = Point( int_max ), int channel = -1,
                       const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( image, ImageOp::Mov, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Move().
    */
   template <class P1>
   GenericImage& Mov( const GenericImage<P1>& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Move( image, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * Adds pixel samples of the specified \a image to a subset of pixel samples
    * in this image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( image, ImageOp::Add, point, channel, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <class P1>
   GenericImage& Add( const GenericImage<P1>& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( image, ImageOp::Add, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Add().
    */
   template <class P1>
   GenericImage& operator +=( const GenericImage<P1>& image )
   {
      return Add( image );
   }

   /*!
    * Subtracts pixel samples of the specified \a image from a subset of pixel
    * samples in this image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( image, ImageOp::Sub, point, channel, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <class P1>
   GenericImage& Subtract( const GenericImage<P1>& image,
                           const Point& point = Point( int_max ), int channel = -1,
                           const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( image, ImageOp::Sub, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Subtract().
    */
   template <class P1>
   GenericImage& Sub( const GenericImage<P1>& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Subtract( image, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Subtract().
    */
   template <class P1>
   GenericImage& operator -=( const GenericImage<P1>& image )
   {
      return Subtract( image );
   }

   /*!
    * Multiplies a subset of pixel samples in this image by samples from the
    * specified source \a image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( image, ImageOp::Mul, point, channel, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <class P1>
   GenericImage& Multiply( const GenericImage<P1>& image,
                           const Point& point = Point( int_max ), int channel = -1,
                           const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( image, ImageOp::Mul, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Multiply().
    */
   template <class P1>
   GenericImage& Mul( const GenericImage<P1>& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Multiply( image, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Multiply().
    */
   template <class P1>
   GenericImage& operator *=( const GenericImage<P1>& image )
   {
      return Multiply( image );
   }

   /*!
    * Divides a subset of pixel samples in this image by samples from the
    * specified source \a image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( image, ImageOp::Div, point, channel, rect, firstChannel, lastChannel );
    * \endcode
    *
    * When a source pixel sample of the specified \a image is zero or
    * insignificant, the corresponding target pixel sample is set to the
    * maximum sample value in the native range of this image.
    */
   template <class P1>
   GenericImage& Divide( const GenericImage<P1>& image,
                         const Point& point = Point( int_max ), int channel = -1,
                         const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( image, ImageOp::Div, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Divide().
    */
   template <class P1>
   GenericImage& Div( const GenericImage<P1>& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Divide( image, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Divide().
    */
   template <class P1>
   GenericImage& operator /=( const GenericImage<P1>& image )
   {
      return Divide( image );
   }

   /*!
    * Raises (exponentiation operator) a subset of pixel samples in this image
    * to samples from the specified source \a image. Returns a reference to
    * this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( image, ImageOp::Pow, point, channel, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <class P1>
   GenericImage& Raise( const GenericImage<P1>& image,
                        const Point& point = Point( int_max ), int channel = -1,
                        const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( image, ImageOp::Pow, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Raise().
    */
   template <class P1>
   GenericImage& Pow( const GenericImage<P1>& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Raise( image, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for Raise().
    */
   template <class P1>
   GenericImage& operator ^=( const GenericImage<P1>& image )
   {
      return Raise( image );
   }

   /*!
    * Replaces a subset of pixel samples in this image with the absolute values
    * of their differences with samples from the specified source \a image.
    * Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( image, ImageOp::Dif, point, channel, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <class P1>
   GenericImage& SetAbsoluteDifference( const GenericImage<P1>& image,
                                        const Point& point = Point( int_max ), int channel = -1,
                                        const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( image, ImageOp::Dif, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for SetAbsoluteDifference().
    */
   template <class P1>
   GenericImage& Dif( const GenericImage<P1>& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return SetAbsoluteDifference( image, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * Replaces a subset of pixel samples in this image with the minimum of each
    * target sample and the corresponding source sample of the specified
    * \a image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( image, ImageOp::Min, point, channel, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <class P1>
   GenericImage& SetMinimum( const GenericImage<P1>& image,
                             const Point& point = Point( int_max ), int channel = -1,
                             const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( image, ImageOp::Min, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for SetMinimum().
    */
   template <class P1>
   GenericImage& Min( const GenericImage<P1>& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return SetMinimum( image, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * Replaces a subset of pixel samples in this image with the maximum of each
    * target sample and the corresponding source sample of the specified
    * \a image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( image, ImageOp::Min, point, channel, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <class P1>
   GenericImage& SetMaximum( const GenericImage<P1>& image,
                             const Point& point = Point( int_max ), int channel = -1,
                             const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( image, ImageOp::Max, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * A synonym for SetMaximum().
    */
   template <class P1>
   GenericImage& Max( const GenericImage<P1>& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return SetMaximum( image, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * Performs a bitwise OR (inclusive OR) operation between a subset of pixel
    * samples in this image and the corresponding samples of the specified
    * source \a image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( image, ImageOp::Or, point, channel, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <class P1>
   GenericImage& Or( const GenericImage<P1>& image,
                     const Point& point = Point( int_max ), int channel = -1,
                     const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( image, ImageOp::Or, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * Performs a bitwise AND operation between a subset of pixel samples in
    * this image and the corresponding samples of the specified source
    * \a image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( image, ImageOp::And, point, channel, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <class P1>
   GenericImage& And( const GenericImage<P1>& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( image, ImageOp::And, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * Performs a bitwise XOR (exclusive OR) operation between a subset of pixel
    * samples in this image and the corresponding samples of the specified
    * source \a image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( image, ImageOp::Xor, point, channel, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <class P1>
   GenericImage& Xor( const GenericImage<P1>& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( image, ImageOp::Xor, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * Performs a bitwise NOR (NOT inclusive OR) operation between a subset of
    * pixel samples in this image and the corresponding samples of the
    * specified source \a image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( image, ImageOp::Nor, point, channel, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <class P1>
   GenericImage& Nor( const GenericImage<P1>& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( image, ImageOp::Nor, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * Performs a bitwise NAND (NOT AND) operation between a subset of pixel
    * samples in this image and the corresponding samples of the specified
    * source \a image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( image, ImageOp::Nand, point, channel, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <class P1>
   GenericImage& Nand( const GenericImage<P1>& image,
                       const Point& point = Point( int_max ), int channel = -1,
                       const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( image, ImageOp::Nand, point, channel, rect, firstChannel, lastChannel );
   }

   /*!
    * Performs a bitwise XNOR (NOT exclusive OR) operation between a subset of
    * pixel samples in this image and the corresponding samples of the
    * specified source \a image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Apply( image, ImageOp::Xnor, point, channel, rect, firstChannel, lastChannel );
    * \endcode
    */
   template <class P1>
   GenericImage& Xnor( const GenericImage<P1>& image,
                       const Point& point = Point( int_max ), int channel = -1,
                       const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Apply( image, ImageOp::Xnor, point, channel, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

   /*!
    * Exchanges a subset of pixel samples between this image and another
    * operand \a image. Returns a reference to this image.
    *
    * \param image   Operand image, whose pixel samples will be exchanged with
    *                samples from this image.
    *
    * \param point   Target point in image coordinates. This is the upper left
    *                corner of the target rectangular region where the source
    *                \a image will be exchanged with this image. If one or both
    *                point coordinates are equal to \c int_max (the maximum
    *                possible value of type \c int), then the current anchor
    *                point selection will be used. The default value is
    *                Point( int_max ).
    *
    * \param channel Target channel index (zero-based). This is the index of
    *                the first channel of this image where pixel samples will
    *                be exchanged. If this parameter is negative, then the
    *                currently selected channel will be used. The default value
    *                is -1.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * The specified operation is performed with samples in the intersection
    * between the specified (or implicit) rectangular selections for both
    * images. Each channel in the specified channel range selection for this
    * image is operated with its counterpart channel in the operand \a image,
    * starting from the first selected channel in the operand \a image.
    *
    * The operation may involve conversions to the appropriate floating point
    * data type for intermediate results, or LUT table lookups, as necessary
    * to ensure preservation of the full native range of both images.
    *
    * \note Increments the status monitoring object by the number of pixel
    * samples in the intersection between both operand images.
    */
   template <class P1>
   GenericImage& Exchange( GenericImage<P1>& image,
                           const Point& point = Point( int_max ), int channel = -1,
                           const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      Rect r = rect;
      if ( !image.ParseSelection( r, firstChannel, lastChannel ) )
         return *this;

      if ( !ParseChannel( channel ) )
         return *this;

      Point p = point;
      if ( p.x == int_max || p.y == int_max )
         p = m_point;

      if ( p.x < 0 )
      {
         if ( (r.x0 -= p.x) >= r.x1 )
            return *this;
         p.x = 0;
      }
      else if ( p.x >= m_width )
         return *this;

      if ( p.y < 0 )
      {
         if ( (r.y0 -= p.y) >= r.y1 )
            return *this;
         p.y = 0;
      }
      else if ( p.y >= m_height )
         return *this;

      r.ResizeTo( pcl::Min( m_width - p.x, r.Width() ),
                  pcl::Min( m_height - p.y, r.Height() ) );

      lastChannel = pcl::Min( lastChannel, firstChannel + m_numberOfChannels - channel - 1 );

      EnsureUnique();
      image.EnsureUnique();

      size_type N = size_type( r.Width() )*size_type( r.Height() );
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Exchanging pixel samples", N*(1 + lastChannel - firstChannel) );

      if ( r == Bounds() && r == image.Bounds() )
         for ( int i = channel, j = firstChannel; j <= lastChannel; ++i, ++j, m_status += N )
         {
            sample* f = m_pixelData[i];
            typename P1::sample* g = image[j];
            for ( sample* f1 = f+N; f < f1; ++f, ++g )
            {
               sample t = *f;
               P1::FromSample( *f, *g );
               P::FromSample( *g, t );
            }
         }
      else
         for ( int i = channel, j = firstChannel, w = r.Width(), h = r.Height(); j <= lastChannel; ++i, ++j, m_status += N )
         {
            sample* f = PixelAddress( p, i );
            typename P1::sample* g = image.PixelAddress( r.LeftTop(), j );
            for ( int k = 0; k < h; ++k, f += m_width-w, g += image.Width()-w )
               for ( sample* f1 = f+w; f < f1; ++f, ++g )
               {
                  sample t = *f;
                  P1::FromSample( *f, *g );
                  P::FromSample( *g, t );
               }
         }

      return *this;
   }

   /*!
    * A synonym for Exchange().
    */
   template <class P1>
   GenericImage& Xchg( GenericImage<P1>& image,
                       const Point& point = Point( int_max ), int channel = -1,
                       const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Exchange( image, point, channel, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

   /*!
    * Returns the minimum value among a subset of pixel samples of this image.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account to search for the
    * minimum sample value.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   sample MinimumSampleValue( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                              int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return 0;

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Computing minimum sample value", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<MinThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new MinThread( *this, r, firstChannel, lastChannel,
                                     i*rowsPerThread,
                                     (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( MinThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( MinThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      sample min = P::MinSampleValue();
      bool initialized = false;
      for ( const MinThread& thread : threads )
         if ( thread.initialized )
            if ( initialized )
            {
               if ( thread.min < min )
                  min = thread.min;
            }
            else
            {
               min = thread.min;
               initialized = true;
            }

      threads.Destroy();
      m_status += N;
      return min;
   }

   /*!
    * \deprecated This member function has been deprecated. It is still part of
    * PCL only to keep existing code alive. In newly produced code, use
    * MinimumSampleValue() instead.
    */
   sample MinimumPixelValue( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                             int maxProcessors = 0 ) const
   {
      return MinimumSampleValue( rect, firstChannel, lastChannel, maxProcessors );
   }

   /*!
    * Returns the maximum value among a subset of pixel samples of this image.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account to search for the
    * maximum sample value.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   sample MaximumSampleValue( const Rect& rect = Rect( 0 ),
                              int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return 0;

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Computing maximum sample value", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<MaxThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new MaxThread( *this, r, firstChannel, lastChannel,
                                     i*rowsPerThread,
                                     (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( MaxThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( MaxThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      sample max = P::MinSampleValue();
      bool initialized = false;
      for ( const MaxThread& thread : threads )
         if ( thread.initialized )
            if ( initialized )
            {
               if ( max < thread.max )
                  max = thread.max;
            }
            else
            {
               max = thread.max;
               initialized = true;
            }

      threads.Destroy();
      m_status += N;
      return max;
   }

   /*!
    * \deprecated This member function has been deprecated. It is still part of
    * PCL only to keep existing code alive. In newly produced code, use
    * MaximumSampleValue() instead.
    */
   sample MaximumPixelValue( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                             int maxProcessors = 0 ) const
   {
      return MaximumSampleValue( rect, firstChannel, lastChannel, maxProcessors );
   }

   /*!
    * Obtains the extreme values among a subset of pixel samples of this image.
    *
    * \param[out] min   Minimum sample value.
    *
    * \param[out] max   Maximum sample value.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * If necessary, the output values will be converted from the pixel sample
    * data type of this image using pixel traits primitives.
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account to search for the
    * extreme sample values.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   template <typename T>
   void GetExtremeSampleValues( T& min, T& max,
                                const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
      {
         // ### Required for compatibility with PCL 1.x
         P::FromSample( min, P::MinSampleValue() );
         max = min;
         return;
      }

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Computing extreme sample values", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<MinMaxThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new MinMaxThread( *this, r, firstChannel, lastChannel,
                                     i*rowsPerThread,
                                     (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( MinMaxThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( MinMaxThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      sample vmin = P::MinSampleValue();
      sample vmax = P::MinSampleValue();
      bool initialized = false;
      for ( const MinMaxThread& thread : threads )
         if ( thread.initialized )
            if ( initialized )
            {
               if ( thread.min < vmin )
                  vmin = thread.min;
               if ( vmax < thread.max )
                  vmax = thread.max;
            }
            else
            {
               vmin = thread.min;
               vmax = thread.max;
               initialized = true;
            }

      threads.Destroy();
      m_status += N;
      P::FromSample( min, vmin );
      P::FromSample( max, vmax );
   }

   /*!
    * \deprecated This member function has been deprecated. It is still part of
    * PCL only to keep existing code alive. In newly produced code, use
    * GetExtremeSampleValues() instead.
    */
   void GetExtremePixelValues( sample& min, sample& max,
                               const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                               int maxProcessors = 0 ) const
   {
      GetExtremeSampleValues( min, max, rect, firstChannel, lastChannel, maxProcessors );
   }

   /*!
    * Returns the minimum value among a subset of pixel samples of this image,
    * and obtains the image coordinates of its first occurrence.
    *
    * \param[out] xmin  Horizontal coordinate of the first occurrence of the
    *                   returned minimum pixel sample value.
    *
    * \param[out] ymin  Vertical coordinate of the first occurrence of the
    *                   returned minimum pixel sample value.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account to search for the
    * minimum sample value.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   sample LocateMinimumSampleValue( int& xmin, int& ymin,
                                    const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                    int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
      {
         // ### Required for compatibility with PCL 1.x
         xmin = ymin = 0;
         return P::MinSampleValue();
      }

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Locating minimum sample value", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<MinPosThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new MinPosThread( *this, r, firstChannel, lastChannel,
                                     i*rowsPerThread,
                                     (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( MinPosThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( MinPosThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      sample min = P::MinSampleValue();
      bool initialized = false;
      for ( const MinPosThread& thread : threads )
         if ( thread.initialized )
            if ( initialized )
            {
               if ( thread.min < min )
               {
                  min = thread.min;
                  xmin = thread.pmin.x;
                  ymin = thread.pmin.y;
               }
            }
            else
            {
               min = thread.min;
               xmin = thread.pmin.x;
               ymin = thread.pmin.y;
               initialized = true;
            }

      threads.Destroy();

      if ( !initialized )
         xmin = ymin = -1;

      m_status += N;

      return min;
   }

   /*!
    * Returns the minimum value among a subset of pixel samples of this image,
    * and obtains the image coordinates of its first occurrence.
    *
    * \param[out] pmin  Coordinates of the first occurrence of the returned
    *                   minimum pixel sample value.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * This member function is equivalent to:
    *
    * \code
    * LocateMinimumSampleValue( pmin.x, pmin.y, rect, firstChannel, lastChannel );
    * \endcode
    */
   sample LocateMinimumSampleValue( Point& pmin,
                                    const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                    int maxProcessors = 0 ) const
   {
      return LocateMinimumSampleValue( pmin.x, pmin.y, rect, firstChannel, lastChannel, maxProcessors );
   }

   /*!
    * \deprecated This member function has been deprecated. It is still part of
    * PCL only to keep existing code alive. In newly produced code, use
    * LocateMinimumSampleValue() instead.
    */
   sample LocateMinimumPixelValue( int& xmin, int& ymin,
                                   const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                   int maxProcessors = 0 ) const
   {
      return LocateMinimumSampleValue( xmin, ymin, rect, firstChannel, lastChannel, maxProcessors );
   }

   /*!
    * \deprecated This member function has been deprecated. It is still part of
    * PCL only to keep existing code alive. In newly produced code, use
    * LocateMinimumSampleValue() instead.
    */
   sample LocateMinimumPixelValue( Point& pmin,
                                   const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                   int maxProcessors = 0 ) const
   {
      return LocateMinimumSampleValue( pmin, rect, firstChannel, lastChannel, maxProcessors );
   }

   /*!
    * Returns the maximum value among a subset of pixel samples of this image,
    * and obtains the image coordinates of its first occurrence.
    *
    * \param[out] xmax  Horizontal coordinate of the first occurrence of the
    *                   returned maximum pixel sample value.
    *
    * \param[out] ymax  Vertical coordinate of the first occurrence of the
    *                   returned maximum pixel sample value.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account to search for the
    * maximum sample value.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   sample LocateMaximumSampleValue( int& xmax, int& ymax,
                                    const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                    int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
      {
         // ### Required for compatibility with PCL 1.x
         xmax = ymax = 0;
         return P::MaxSampleValue();
      }

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Locating maximum sample value", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<MaxPosThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new MaxPosThread( *this, r, firstChannel, lastChannel,
                                     i*rowsPerThread,
                                     (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( MaxPosThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( MaxPosThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      sample max = P::MinSampleValue();
      bool initialized = false;
      for ( const MaxPosThread& thread : threads )
         if ( thread.initialized )
            if ( initialized )
            {
               if ( max < thread.max )
               {
                  max = thread.max;
                  xmax = thread.pmax.x;
                  ymax = thread.pmax.y;
               }
            }
            else
            {
               max = thread.max;
               xmax = thread.pmax.x;
               ymax = thread.pmax.y;
               initialized = true;
            }

      threads.Destroy();

      if ( !initialized )
         xmax = ymax = -1;

      m_status += N;

      return max;
   }

   /*!
    * Returns the maximum value among a subset of pixel samples of this image,
    * and obtains the image coordinates of its first occurrence.
    *
    * \param[out] pmax  Coordinates of the first occurrence of the returned
    *                   maximum pixel sample value.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the parameters of this member function, see the
    * documentation for Fill().
    *
    * This member function is equivalent to:
    *
    * \code
    * LocateMaximumSampleValue( pmax.x, pmax.y, rect, firstChannel, lastChannel );
    * \endcode
    */
   sample LocateMaximumSampleValue( Point& pmax,
                                    const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                    int maxProcessors = 0 ) const
   {
      return LocateMaximumSampleValue( pmax.x, pmax.y, rect, firstChannel, lastChannel, maxProcessors );
   }

   /*!
    * \deprecated This member function has been deprecated. It is still part of
    * PCL only to keep existing code alive. In newly produced code, use
    * LocateMaximumSampleValue() instead.
    */
   sample LocateMaximumPixelValue( int& xmax, int& ymax,
                                   const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                   int maxProcessors = 0 ) const
   {
      return LocateMaximumSampleValue( xmax, ymax, rect, firstChannel, lastChannel, maxProcessors );
   }

   /*!
    * \deprecated This member function has been deprecated. It is still part of
    * PCL only to keep existing code alive. In newly produced code, use
    * LocateMaximumSampleValue() instead.
    */
   sample LocateMaximumPixelValue( Point& pmax,
                                   const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                   int maxProcessors = 0 ) const
   {
      return LocateMaximumSampleValue( pmax, rect, firstChannel, lastChannel, maxProcessors );
   }

   /*!
    * Obtains the extreme values, and the image coordinates of their first
    * occurrences, among a subset of pixel samples of this image.
    *
    * \param[out] xmin  Horizontal coordinate of the first occurrence of the
    *                   minimum pixel sample value.
    *
    * \param[out] ymin  Vertical coordinate of the first occurrence of the
    *                   minimum pixel sample value.
    *
    * \param[out] min   Minimum pixel sample value in the selected subset.
    *
    * \param[out] xmax  Horizontal coordinate of the first occurrence of the
    *                   maximum pixel sample value.
    *
    * \param[out] ymax  Vertical coordinate of the first occurrence of the
    *                   maximum pixel sample value.
    *
    * \param[out] max   Maximum pixel sample value in the selected subset.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account to search for the
    * extreme sample values.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
   */
   template <typename T>
   void LocateExtremeSampleValues( int& xmin, int& ymin, T& min,
                                   int& xmax, int& ymax, T& max,
                                   const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                   int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
      {
         // ### Required for compatibility with PCL 1.x
         xmin = ymin = xmax = ymax = 0;
         P::FromSample( min, P::MinSampleValue() );
         max = min;
         return;
      }

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Locating extreme sample values", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<MinMaxPosThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new MinMaxPosThread( *this, r, firstChannel, lastChannel,
                                           i*rowsPerThread,
                                           (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( MinMaxPosThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( MinMaxPosThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      sample vmin = P::MinSampleValue();
      sample vmax = P::MinSampleValue();
      bool initialized = false;
      for ( const MinMaxPosThread& thread : threads )
         if ( thread.initialized )
            if ( initialized )
            {
               if ( thread.min < vmin )
               {
                  vmin = thread.min;
                  xmin = thread.pmin.x;
                  ymin = thread.pmin.y;
               }

               if ( vmax < thread.max )
               {
                  vmax = thread.max;
                  xmax = thread.pmax.x;
                  ymax = thread.pmax.y;
               }
            }
            else
            {
               vmin = thread.min;
               xmin = thread.pmin.x;
               ymin = thread.pmin.y;
               vmax = thread.max;
               xmax = thread.pmax.x;
               ymax = thread.pmax.y;
               initialized = true;
            }

      threads.Destroy();

      m_status += N;

      P::FromSample( min, vmin );
      P::FromSample( max, vmax );

      if ( !initialized )
         xmin = ymin = xmax = ymax = -1;
   }

   /*!
    * Obtains the extreme values, and the image coordinates of their first
    * occurrences, among a subset of pixel samples of this image.
    *
    * \param[out] pmin  Coordinates of the first occurrence of the minimum
    *                   pixel sample value.
    *
    * \param[out] min   Minimum pixel sample value in the selected subset.
    *
    * \param[out] pmax  Coordinates of the first occurrence of the maximum
    *                   pixel sample value.
    *
    * \param[out] max   Maximum pixel sample value in the selected subset.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account to search for the
    * extreme sample values.
   */
   template <typename T>
   void LocateExtremeSampleValues( Point& pmin, T& min,
                                   Point& pmax, T& max,
                                   const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                   int maxProcessors = 0 ) const
   {
      LocateExtremeSampleValues( pmin.x, pmin.y, min,
                                 pmax.x, pmax.y, max,
                                 rect, firstChannel, lastChannel, maxProcessors );
   }

   /*!
    * Returns the number of pixel samples selectable for statistics calculation
    * from a subset of pixel samples.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account. If range clipping
    * is disabled, the result of this member function will be the volume of the
    * specified (or implicitly selected) region.
    *
    * When range clipping is enabled and this is a complex-valued image, this
    * function uses the magnitudes (or absolute values) of all selected samples
    * to compare them with the bounds of the current clipping range.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   uint64 Count( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                 int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return 0;

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Counting samples", N );

      if ( !this->IsRangeClippingEnabled() )
      {
         m_status += N;
         return uint64( r.Width() ) * uint64( r.Height() ) * uint64( 1 + lastChannel - firstChannel );
      }

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<CountThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new CountThread( *this, r, firstChannel, lastChannel,
                                       i*rowsPerThread,
                                       (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( CountThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( CountThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      uint64 count = 0;
      for ( const CountThread& thread : threads )
         count += thread.count;

      threads.Destroy();
      m_status += N;
      return count;
   }

   /*!
    * Returns the arithmetic mean of a subset of pixel samples.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * This member function returns the mean in the normalized range [0,1],
    * irrespective of the sample data type of the image.
    *
    * For complex-valued images, this function returns the mean of the
    * magnitudes (or absolute values) of all selected samples.
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account for calculation of
    * the mean.
    *
    * This function implements a numerically stable summation algorithm to
    * reduce roundoff errors to the machine's floating point precision.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   double Mean( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return 0;

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Computing mean sample value", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<SumThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new SumThread( *this, r, firstChannel, lastChannel,
                                     i*rowsPerThread,
                                     (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( SumThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( SumThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      double s = 0;
      double e = 0;
      size_type n = 0;
      for ( const SumThread& thread : threads )
      {
         double y = thread.s - e;
         double t = s + y;
         e = (t - s) - y;
         s = t;
         n += thread.n;
      }

      threads.Destroy();

      m_status += N;

      if ( n == 0 )
         return 0;
      return s/n;
   }

   /*!
    * Returns the median of a subset of pixel samples.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * This member function returns the median in the normalized range [0,1],
    * irrespective of the sample data type of the image.
    *
    * For complex-valued images, this function returns the median of the
    * magnitudes (or absolute values) of all selected samples.
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account for calculation of
    * the median.
    *
    * For sets of pixel samples of even length less than 2^16=65536, this
    * routine returns the mean of the two central values. For the sake of
    * performance, for larger sets of even length this function returns the
    * high median, which is the order statistic of rank ceil(n/2). Note that
    * for large sets, the difference between a high median and the mean of the
    * high and low medians is statistically irrelevant (modulo special cases
    * that are irrelevant for practical matters).
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   double Median( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                  int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return 0;

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Computing median sample value", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<SmpThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new SmpThread( *this, r, firstChannel, lastChannel,
                                     i*rowsPerThread,
                                     (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( SmpThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( SmpThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      Array<sample> samples;
      for ( SmpThread& thread : threads )
         if ( !thread.samples.IsEmpty() )
         {
            samples.Add( thread.samples.Begin(), thread.samples.At( thread.n ) );
            thread.samples.Clear();
         }

      threads.Destroy();

      size_type n = samples.Length();
      if ( n < 2 )
         return 0;

      size_type n2 = n >> 1;
      double m;
      P::FromSample( m, *pcl::Select( samples.Begin(), samples.End(), n2 ) );
      if ( n < 0x10000 )
         if ( (n & 1) == 0 )
         {
            double m1;
            P::FromSample( m1, *pcl::Select( samples.Begin(), samples.End(), n2-1 ) );
            m = (m + m1)/2;
         }

      m_status += N;

      return m;
   }

   /*!
    * Returns the variance from the mean of a subset of pixel samples.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * This member function returns the variance in the normalized range [0,1],
    * irrespective of the sample data type of the image.
    *
    * For complex-valued images, this function returns the variance of the
    * magnitudes (or absolute values) of all selected samples.
    *
    * This implementation uses a two-pass compensated summation algorithm to
    * minimize roundoff errors (see References).
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account for calculation of
    * the variance.
    *
    * \b References
    *
    * William H. Press et al., <em>Numerical Recipes in C: The Art of
    * Scientific Computing, Second Edition</em> (1997 reprint) Cambridge
    * University Press, page 613.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   double Variance( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                    int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return 0;

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Computing variance", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<SumThread> sumThreads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         sumThreads.Add( new SumThread( *this, r, firstChannel, lastChannel,
                                        i*rowsPerThread,
                                        (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( SumThread& thread : sumThreads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( SumThread& thread : sumThreads )
            thread.Wait();
      }
      else
         sumThreads[0].Run();

      double s = 0;
      double e = 0;
      size_type n = 0;
      for ( const SumThread& thread : sumThreads )
      {
         double y = thread.s - e;
         double t = s + y;
         e = (t - s) - y;
         s = t;
         n += thread.n;
      }

      sumThreads.Destroy();

      if ( n < 2 )
         return 0;
      double mean = s/n;

      ReferenceArray<VarThread> varThreads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         varThreads.Add( new VarThread( *this, mean, r, firstChannel, lastChannel,
                                        i*rowsPerThread,
                                        (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( VarThread& thread : varThreads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( VarThread& thread : varThreads )
            thread.Wait();
      }
      else
         varThreads[0].Run();

      double var = 0, eps = 0;
      for ( const VarThread& thread : varThreads )
         var += thread.var, eps += thread.eps;

      varThreads.Destroy();
      m_status += N;
      return (var - eps*eps/n)/(n - 1);
   }

   /*!
    * Returns the standard deviation from the mean of a subset of pixel
    * samples.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * This member function returns the standard deviation in the normalized
    * range [0,1], irrespective of the sample data type of the image.
    *
    * For complex-valued images, this function returns the standard deviation
    * of the magnitudes (or absolute values) of all selected samples.
    *
    * This implementation uses a two-pass compensated summation algorithm to
    * minimize roundoff errors (see References).
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account for calculation of
    * the variance.
    *
    * \b References
    *
    * William H. Press et al., <em>Numerical Recipes in C: The Art of
    * Scientific Computing, Second Edition</em> (1997 reprint) Cambridge
    * University Press, page 613.
    */
   double StdDev( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                  int maxProcessors = 0 ) const
   {
      return pcl::Sqrt( Variance( rect, firstChannel, lastChannel, maxProcessors ) );
   }

   /*!
    * Returns the mean absolute deviation of a subset of pixel samples with
    * respect to the specified \a center value.
    *
    * \param center  Reference central value for computation of the mean
    *                absolute deviation in the normalized range [0,1].
    *                Typically, this parameter is set to the median value of
    *                the set of pixel samples used for calculation.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * This member function returns the mean absolute deviation with respect to
    * \a center in the normalized range [0,1], irrespective of the sample data
    * type of the image.
    *
    * For complex-valued images, this function returns the average absolute
    * deviation of the magnitudes (or absolute values) of all selected samples.
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account for calculation of
    * the average absolute deviation.
    *
    * This function implements a numerically stable summation algorithm to
    * reduce roundoff errors to the machine's floating point precision.
    *
    * \note To make the average absolute deviation about the median consistent
    * with the standard deviation of a normal distribution, it must be
    * multiplied by the constant 1.2533.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   double AvgDev( double center, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                  int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return 0;

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Computing average absolute deviation", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<SumAbsDevThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new SumAbsDevThread( *this, center, r, firstChannel, lastChannel,
                                           i*rowsPerThread,
                                           (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( SumAbsDevThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( SumAbsDevThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      double s = 0;
      double e = 0;
      size_type n = 0;
      for ( const SumAbsDevThread& thread : threads )
      {
         double y = thread.s - e;
         double t = s + y;
         e = (t - s) - y;
         s = t;
         n += thread.n;
      }

      threads.Destroy();

      m_status += N;

      if ( n == 0 )
         return 0;
      return s/n;
   }

   /*!
    * Returns the median absolute deviation (MAD) of a subset of pixel samples
    * with respect to the specified \a center value.
    *
    * \param center  Reference central value for computation of MAD in the
    *                normalized range [0,1]. Normally, this value should be the
    *                median of the selected pixel samples, or the median of the
    *                whole image.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * This member function returns MAD in the normalized range [0,1] with
    * respect to \a center, irrespective of the sample data type of the image.
    *
    * For complex-valued images, this function returns the MAD of the
    * magnitudes (or absolute values) of all selected samples.
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account for calculation of
    * the median absolute deviation.
    *
    * For sets of pixel samples of even length less than 2^16=65536, this
    * routine returns the mean of the two central values. For larger sets of
    * even length, this function returns the high median, which is the order
    * statistic of rank ceil(n/2). Note that for large lists, the difference
    * between a high median and the mean of the high and low medians is
    * statistically irrelevant.
    *
    * \note To make the MAD estimator consistent with the standard deviation of
    * a normal distribution, it must be multiplied by the constant 1.4826.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   double MAD( double center, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
               int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return 0;

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Computing median absolute deviation", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<SmpAbsDevThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new SmpAbsDevThread( *this, center, r, firstChannel, lastChannel,
                                           i*rowsPerThread,
                                           (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( SmpAbsDevThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? ++n : -1 );
         for ( SmpAbsDevThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      Array<double> devs;
      for ( SmpAbsDevThread& thread : threads )
         if ( !thread.devs.IsEmpty() )
         {
            devs.Add( thread.devs.Begin(), thread.devs.At( thread.n ) );
            thread.devs.Clear();
         }

      threads.Destroy();

      m_status += N;

      size_type n = devs.Length();
      if ( n < 2 )
         return 0;

      size_type n2 = n >> 1;
      if ( n & 1 || n > 0xffff )
         return *pcl::Select( devs.Begin(), devs.End(), n2 );
      return (*pcl::Select( devs.Begin(), devs.End(), n2 )
            + *pcl::Select( devs.Begin(), devs.End(), n2-1 ))/2;
   }

   /*!
    * Returns a biweight midvariance (BWMV) for a subset of pixel samples.
    *
    * \param center  Reference center value in the [0,1] range. Normally, the
    *                median of the selected pixel samples should be used.
    *
    * \param sigma   A reference estimate of dispersion in the [0,1] range.
    *                Normally, the median absolute deviation from the median
    *                (MAD) of the selected pixel samples should be used.
    *
    * \param k       Rejection limit in sigma units. The default value is k=9.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * The square root of the biweight midvariance is a robust estimator of
    * scale. It is an efficient estimator with respect to many statistical
    * distributions (about 87% Gaussian efficiency), and appears to have a
    * breakdown point close to 0.5 (the same as MAD).
    *
    * This member function returns BWMV in the normalized range [0,1],
    * irrespective of the sample data type of the image.
    *
    * For complex-valued images, this function returns a BWMV for the
    * magnitudes (or absolute values) of all selected samples.
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account for calculation of
    * the biweight midvariance.
    *
    * \b References
    *
    * Rand R. Wilcox (2012), <em>Introduction to Robust Estimation and Hypothesis
    * Testing, 3rd Edition</em>, Elsevier Inc., Section 3.12.1.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   double BiweightMidvariance( double center, double sigma, int k = 9,
                               const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                               int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return 0;

      double kd = k * sigma;
      if ( kd < 0 || 1 + kd == 1 )
         return 0;

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Computing biweight midvariance", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<BWMVThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new BWMVThread( *this, center, kd, r, firstChannel, lastChannel,
                                      i*rowsPerThread,
                                      (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( BWMVThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( BWMVThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      double num = 0, den = 0;
      size_type n = 0;
      for ( const BWMVThread& thread : threads )
      {
         num += thread.num;
         den += thread.den;
         n += thread.n;
      }

      threads.Destroy();

      m_status += N;

      if ( n < 2 )
         return 0;

      den *= den;
      if ( 1 + den == 1 )
         return 0;

      return n*num/den;
   }

   /*!
    * Returns a percentage bend midvariance (PBMV) for a subset of pixel
    * samples.
    *
    * \param center  Reference center value in the [0,1] range. Normally, the
    *                median of the selected pixel samples should be used.
    *
    * \param beta    Rejection parameter in the [0,0.5] range. Higher values
    *                improve robustness to outliers (i.e., increase the
    *                breakdown point of the estimator) at the expense of lower
    *                efficiency. The default value is beta=0.2.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * The square root of the percentage bend midvariance is a robust estimator
    * of scale. With the default beta=0.2, its Gaussian efficiency is 67%. With
    * beta=0.1, its efficiency is 85% but its breakdown is only 0.1.
    *
    * This member function returns PBMV in the normalized range [0,1],
    * irrespective of the sample data type of the image.
    *
    * For complex-valued images, this function returns a PBMV for the
    * magnitudes (or absolute values) of all selected samples.
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account for calculation of
    * the percentage bend midvariance.
    *
    * \b References
    *
    * Rand R. Wilcox (2012), <em>Introduction to Robust Estimation and Hypothesis
    * Testing, 3rd Edition</em>, Elsevier Inc., Section 3.12.3.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   double BendMidvariance( double center, double beta = 0.2,
                           const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                           int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return 0;

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Computing percentage bend midvariance", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<DSmpThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new DSmpThread( *this, r, firstChannel, lastChannel,
                                      i*rowsPerThread,
                                      (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( DSmpThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( DSmpThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      Array<double> values;
      for ( DSmpThread& thread : threads )
         if ( !thread.values.IsEmpty() )
         {
            values.Add( thread.values.Begin(), thread.values.At( thread.n ) );
            thread.values.Clear();
         }

      threads.Destroy();

      double pbmv = pcl::BendMidvariance( values.Begin(), values.End(), center, beta );
      m_status += N;
      return pbmv;
   }

   /*!
    * Sn scale estimator of Rousseeuw and Croux for a subset of pixel samples.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * For a sample {x0,...,x_n-1}, the Sn estimator is given by:
    *
    * Sn = c * low_median( high_median( |x_i - x_j| ) )
    *
    * where low_median() is the order statistic of rank (n + 1)/2, and
    * high_median() is the order statistic of rank n/2 + 1.
    *
    * This member function returns Sn in the normalized range [0,1],
    * irrespective of the sample data type of the image.
    *
    * For complex-valued images, this function returns the Sn estimator of the
    * magnitudes (or absolute values) of all selected samples.
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account for calculation of
    * the Sn estimator.
    *
    * The constant c = 1.1926 must be used to make the Sn estimator converge to
    * the standard deviation of a pure normal distribution. However, this
    * implementation does not apply it (it uses c=1 implicitly), for
    * consistency with other implementations of scale estimators.
    *
    * \b References
    *
    * P.J. Rousseeuw and C. Croux (1993), <em>Alternatives to the Median Absolute
    * Deviation,</em> Journal of the American Statistical Association, Vol. 88,
    * pp. 1273-1283.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   double Sn( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return 0;

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Computing Sn scale estimate", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<DSmpThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new DSmpThread( *this, r, firstChannel, lastChannel,
                                      i*rowsPerThread,
                                      (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( DSmpThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( DSmpThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      Array<double> values;
      for ( DSmpThread& thread : threads )
         if ( !thread.values.IsEmpty() )
         {
            values.Add( thread.values.Begin(), thread.values.At( thread.n ) );
            thread.values.Clear();
         }

      threads.Destroy();

      double sn = pcl::Sn( values.Begin(), values.End() );
      m_status += N;
      return sn;
   }

   /*!
    * Qn scale estimator of Rousseeuw and Croux for a subset of pixel samples.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * For a sample {x0,...,x_n-1}, the Qn estimator is given by:
    *
    * Qn = c * first_quartile( |x_i - x_j| : i < j )
    *
    * where first_quartile() is the order statistic of rank (n + 1)/4.
    *
    * This member function returns Qn in the normalized range [0,1],
    * irrespective of the sample data type of the image.
    *
    * For complex-valued images, this function returns the Qn estimator of the
    * magnitudes (or absolute values) of all selected samples.
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account for calculation of
    * the Qn estimator.
    *
    * The constant c = 2.2219 must be used to make the Qn estimator converge to
    * the standard deviation of a pure normal distribution. However, this
    * implementation does not apply it (it uses c=1 implicitly), for
    * consistency with other implementations of scale estimators.
    *
    * \b References
    *
    * P.J. Rousseeuw and C. Croux (1993), <em>Alternatives to the Median Absolute
    * Deviation,</em> Journal of the American Statistical Association, Vol. 88,
    * pp. 1273-1283.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   double Qn( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return 0;

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Computing Qn scale estimate", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<DSmpThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new DSmpThread( *this, r, firstChannel, lastChannel,
                                      i*rowsPerThread,
                                      (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( DSmpThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( DSmpThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      Array<double> values;
      for ( DSmpThread& thread : threads )
         if ( !thread.values.IsEmpty() )
         {
            values.Add( thread.values.Begin(), thread.values.At( thread.n ) );
            thread.values.Clear();
         }

      threads.Destroy();

      double qn = pcl::Qn( values.Begin(), values.End() );
      m_status += N;
      return qn;
   }

   /*!
    * Returns the norm of a subset of pixel samples.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * The norm is the sum of all selected pixel sample values. This member
    * function returns the norm in the normalized range [0,1], irrespective of
    * the sample data type of the image.
    *
    * For real-valued integer images, this function returns the sum of all
    * selected samples converted to the normalized range [0,1].
    *
    * For floating-point real images, this function simply sums all selected
    * samples, assuming that no selected samples have negative values. If there
    * are some negative values, the returned value may make not sense,
    * depending on the application.
    *
    * For complex-valued images, this function returns the sum of the
    * magnitudes (or absolute values) of all selected samples.
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account for calculation of
    * the norm.
    *
    * This function implements a numerically stable summation algorithm to
    * reduce roundoff errors to the machine's floating point precision.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   double Norm( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return 0;

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Computing norm", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<SumThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new SumThread( *this, r, firstChannel, lastChannel,
                                     i*rowsPerThread,
                                     (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( SumThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( SumThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      double s = 0;
      double e = 0;
      for ( const SumThread& thread : threads )
      {
         double y = thread.s - e;
         double t = s + y;
         e = (t - s) - y;
         s = t;
      }

      threads.Destroy();
      m_status += N;
      return (1 + s != 1) ? s : 0.0; // don't return insignificant nonzero values
   }

   /*!
    * Returns the modulus of a subset of pixel samples.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * The modulus is the sum of the absolute values of all selected pixel
    * sample values. This member function returns the modulus in the normalized
    * range [0,1], irrespective of the sample data type of the image.
    *
    * This member function only makes sense for real-valued floating point
    * images. For the rest of image types, including unsigned integer and
    * complex valued images, this function is always equivalent to Norm().
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account for calculation of
    * the modulus.
    *
    * This function implements a numerically stable summation algorithm to
    * reduce roundoff errors to the machine's floating point precision.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   double Modulus( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                   int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return 0;

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Computing modulus", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<SumAbsThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new SumAbsThread( *this, r, firstChannel, lastChannel,
                                     i*rowsPerThread,
                                     (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( SumAbsThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( SumAbsThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      double s = 0;
      double e = 0;
      for ( const SumAbsThread& thread : threads )
      {
         double y = thread.s - e;
         double t = s + y;
         e = (t - s) - y;
         s = t;
      }

      threads.Destroy();
      m_status += N;
      return (1 + s != 1) ? s : 0.0; // don't return insignificant nonzero values
   }

   /*!
    * Returns the sum of the squares of a subset of pixel samples.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * This member function returns the sum of squares in the normalized range
    * [0,1], irrespective of the sample data type of the image.
    *
    * For complex-valued images, this function returns the sum of the
    * squares of the magnitudes (or absolute values) of all selected samples.
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account for calculation of
    * the sum of squares.
    *
    * This function implements a numerically stable summation algorithm to
    * reduce roundoff errors to the machine's floating point precision.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   double SumOfSquares( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                        int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return 0;

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Computing sum of squares", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<SumSqrThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new SumSqrThread( *this, r, firstChannel, lastChannel,
                                        i*rowsPerThread,
                                        (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( SumSqrThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( SumSqrThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      double s = 0;
      double e = 0;
      for ( const SumSqrThread& thread : threads )
      {
         double y = thread.s - e;
         double t = s + y;
         e = (t - s) - y;
         s = t;
      }

      threads.Destroy();
      m_status += N;
      return (1 + s != 1) ? s : 0.0; // don't return insignificant nonzero values
   }

   /*!
    * Returns the mean of the squares of a subset of pixel samples.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    *
    * This member function returns the mean of squares in the normalized range
    * [0,1], irrespective of the sample data type of the image.
    *
    * For complex-valued images, this function returns the mean of the
    * squares of the magnitudes (or absolute values) of all selected samples.
    *
    * If range clipping is enabled for this image, only pixel samples within
    * the current clipping range will be taken into account for calculation of
    * the mean of squares.
    *
    * This function implements a numerically stable summation algorithm to
    * reduce roundoff errors to the machine's floating point precision.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    */
   double MeanOfSquares( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                         int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return 0;

      size_type N = size_type( r.Width() )*size_type( r.Height() )*(1 + lastChannel - firstChannel);
      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Computing mean of squares", N );

      int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
      int rowsPerThread = r.Height()/numberOfThreads;
      bool useAffinity = m_parallel && Thread::IsRootThread();

      ReferenceArray<SumSqrThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new SumSqrThread( *this, r, firstChannel, lastChannel,
                                        i*rowsPerThread,
                                        (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
      if ( numberOfThreads > 1 )
      {
         int n = 0;
         for ( SumSqrThread& thread : threads )
            thread.Start( ThreadPriority::DefaultMax, useAffinity ? n++ : -1 );
         for ( SumSqrThread& thread : threads )
            thread.Wait();
      }
      else
         threads[0].Run();

      double s = 0;
      double e = 0;
      size_type n = 0;
      for ( const SumSqrThread& thread : threads )
      {
         double y = thread.s - e;
         double t = s + y;
         e = (t - s) - y;
         s = t;
         n += thread.n;
      }

      threads.Destroy();

      m_status += N;

      if ( n < 1 )
         return 0;
      return s/n;
   }

   /*!
    * Returns a 64-bit non-cryptographic hash value computed for the specified
    * \a channel of this image.
    *
    * This function calls pcl::Hash64() for the internal pixel sample buffer.
    *
    * The \a seed parameter can be used to generate repeatable hash values. It
    * can also be set to a random value in compromised environments.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    */
   uint64 Hash64( int channel = -1, uint64 seed = 0 ) const
   {
      if ( !ParseChannel( channel ) )
         return 0;
      return pcl::Hash64( m_pixelData[channel], ChannelSize(), seed );
   }

   /*!
    * Returns a 32-bit non-cryptographic hash value computed for the specified
    * \a channel of this image.
    *
    * This function calls pcl::Hash32() for the internal pixel sample buffer.
    *
    * The \a seed parameter can be used to generate repeatable hash values. It
    * can also be set to a random value in compromised environments.
    *
    * For information on the rest of parameters of this member function, see
    * the documentation for Fill().
    */
   uint32 Hash32( int channel = -1, uint32 seed = 0 ) const
   {
      if ( !ParseChannel( channel ) )
         return 0;
      return pcl::Hash32( m_pixelData[channel], ChannelSize(), seed );
   }

   /*!
    * Returns a non-cryptographic hash value computed for the specified
    * \a channel of this image. This function is a synonym for Hash64().
    */
   uint64 Hash( int channel = -1, uint64 seed = 0 ) const
   {
      return Hash64( channel, seed );
   }

   // -------------------------------------------------------------------------

   /*!
    * Writes a subset of pixel samples to a raw-storage output stream.
    *
    * \param file    Output stream to write to.
    *
    * The generated raw-storage can be used to generate a new image with the
    * Read() member function.
    *
    * \note Increments the status monitoring object by the number of pixel
    * samples written to the specified \a file.
    *
    * \note This member function, together with the Read() function, implements
    * a lightweight image reading/writing system with limited error detection
    * and handling capabilities. For more powerful and sophisticated ways to
    * serialize image data, see the ImageVariant class.
    */
   void Write( File& file, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, firstChannel, lastChannel ) )
         return;

      size_type N = size_type( r.Width() )*size_type( r.Height() );
      int numberOfChannels = 1 + lastChannel - firstChannel;

      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "Writing to raw-storage image stream", N*numberOfChannels );

      file.WriteUI32( r.Width() );
      file.WriteUI32( r.Height() );
      file.WriteUI32( numberOfChannels );
      file.WriteUI32( (firstChannel == 0 && lastChannel >= 2) ? m_colorSpace : ColorSpace::Gray );

      if ( r == Bounds() )
      {
         for ( int i = firstChannel; i <= lastChannel; ++i, m_status += N )
            file.Write( (const void*)m_pixelData[i], fsize_type( ChannelSize() ) );
      }
      else
      {
         for ( int i = firstChannel, w = r.Width(), h = r.Height(); i <= lastChannel; ++i )
         {
            const sample* p = PixelAddress( r.LeftTop(), i );
            for ( int j = 0; j < h; ++j, p += m_width, m_status += w )
               file.Write( (const void*)p, w*BytesPerSample() );
         }
      }

      return;
   }

   /*!
    * Writes a subset of pixel samples to a raw-storage file.
    *
    * \param filePath   File path where a new file will be created with pixel
    *                   data from this image.
    *
    * This member function is just a wrapper to the more general version:
    * Write( File&, const Rect&, int, int ).
    */
   void Write( const String& filePath,
               const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      File file;
      file.CreateForWriting( filePath );
      (void)Write( file, rect, firstChannel, lastChannel );
      file.Close();
   }

   /*!
    * Reads an image from a raw-storage stream. Returns a reference to this
    * image.
    *
    * \param file    Input stream to read from.
    *
    * If this object uniquely references its pixel data, then the previously
    * existing pixel data are destroyed, deallocated, and replaced with a newly
    * allocated local or shared image, preserving the storage space of this
    * instance.
    *
    * The input stream must provide pixel sample values in the sample data type
    * of this image. The sample data type of an image is invariantly defined by
    * its template instantiation, and thus cannot be changed. If the image
    * stored in the input stream uses a different sample data type than this
    * image, this function will lead to unpredictable results, most likely an
    * exception being thrown by the stream I/O routines.
    *
    * If this object is not unique, then the previously referenced pixel data
    * are dereferenced, and a newly created local image is uniquely referenced
    * by this object.
    *
    * All image selections are reset to the default full selection after
    * calling this function.
    *
    * The data read from the specified \a file should have been generated by a
    * previous call to Write().
    *
    * \note Increments the status monitoring object by the number of pixel
    * samples read from the specified \a file.
    *
    * \note This function, together with the Write() function, implements a
    * lightweight image reading/writing system with limited error detection and
    * handling capabilities. For more powerful and sophisticated ways to
    * serialize image data, see the ImageVariant class.
    */
   GenericImage& Read( File& file )
   {
      int width, height, numberOfChannels, colorSpace;
      file.ReadUI32( width );
      file.ReadUI32( height );
      file.ReadUI32( numberOfChannels );
      file.ReadUI32( colorSpace );

      AllocateData( width, height, numberOfChannels, color_space( colorSpace ) );

      ResetSelections();

      size_type N = NumberOfPixels();
      if ( N > 0 )
      {
         if ( m_status.IsInitializationEnabled() )
            m_status.Initialize( "Reading from raw-storage image stream", N*m_numberOfChannels );
         for ( int i = 0; i < m_numberOfChannels; ++i, m_status += N )
            file.Read( (void*)m_pixelData[i], fsize_type( ChannelSize() ) );
      }

      return *this;
   }

   /*!
    * Reads an image from a raw-storage file. Returns a reference to this
    * image.
    *
    * \param filePath   Path to an existing file from which new pixel data will
    *                   be read. The contents of the existing file will not be
    *                   modified in any way.
    *
    * This member function is just a wrapper to the more general version:
    * Read( File& ).
    */
   GenericImage& Read( const String& filePath )
   {
      File file;
      file.OpenForReading( filePath );
      (void)Read( file );
      file.Close();
      return *this;
   }

   // -------------------------------------------------------------------------

   /*!
    * Crops or extends the entire image by applying the specified margins.
    * Returns a reference to this image.
    *
    * \param left       Left margin in pixels.
    *
    * \param top        Top margin in pixels.
    *
    * \param right      Right margin in pixels.
    *
    * \param bottom     Bottom margin in pixels.
    *
    * \param fillValues %Vector of filling sample values for extended areas,
    *                   used when one or more margins are greater than zero.
    *                   Each vector component is used for its corresponding
    *                   channel (e.g., the first component is used to fill the
    *                   red channel of an RGB image, the second component to
    *                   fill the green channel, etc.). If there are less vector
    *                   components than channels in the image, the minimum
    *                   sample value in the native range of the image (usually
    *                   zero or black) is used for the missing filling values.
    *                   The default parameter value is an empty vector, so the
    *                   extended areas are filled with black by default.
    *
    * Negative margins crop the image at their respective sides. Positive
    * margin values add extra space.
    *
    * \note Increments the status monitoring object by the area in square
    * pixels of the cropping rectangle.
    */
   template <typename T>
   GenericImage& CropBy( int left, int top, int right, int bottom, const GenericVector<T>& fillValues )
   {
      if ( left == 0 && top == 0 && right == 0 && bottom == 0 )
         return *this;

      if ( m_width+left+right <= 0 || m_height+top+bottom <= 0 )
      {
         FreeData();
         return *this;
      }

      Rect r( -left, -top, m_width+right, m_height+bottom );
      int width = r.Width();
      int height = r.Height();

      if ( !Intersects( r ) )
         return AllocateData( width, height, m_numberOfChannels, m_colorSpace ).Fill( fillValues );

      size_type N = size_type( width )*size_type( height );

      EnsureUnique();

      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( String().Format( "Crop margins: %+d, %+d, %+d, %+d",
                                               left, top, right, bottom ), N*m_numberOfChannels );

      sample** newData = nullptr;

      try
      {
         newData = m_allocator.AllocateChannelSlots( m_numberOfChannels );

         for ( int c = 0; c < m_numberOfChannels; ++c, m_status += N )
         {
            sample* f = newData[c] = m_allocator.AllocatePixels( N );
            sample v = (c < fillValues.Length()) ? P::ToSample( fillValues[c] ) : P::MinSampleValue();

            for ( int i = r.y0, j; i < r.y1; )
            {
               for ( ; i < 0; ++i )
                  for ( int j = 0; j < width; ++j )
                     *f++ = v;

               for ( j = r.x0; j < 0; ++j )
                  *f++ = v;

               for ( const sample* f0 = PixelAddress( j, i, c ); j < r.x1; )
               {
                  *f++ = *f0++;
                  if ( ++j == m_width )
                     for ( ; j < r.x1; ++j )
                        *f++ = v;
               }

               if ( ++i == m_height )
                  for ( ; i < r.y1; ++i )
                     for ( int j = 0; j < width; ++j )
                        *f++ = v;
            }
         }

         try
         {
            for ( int c = 0; c < m_numberOfChannels; ++c )
            {
               m_allocator.Deallocate( m_pixelData[c] );
               m_pixelData[c] = newData[c];
            }
            m_allocator.Deallocate( newData );
            newData = nullptr;

            m_allocator.SetSharedGeometry( m_width = width, m_height = height, m_numberOfChannels );

            ResetPoint();
            ResetSelection();
            return *this;
         }
         catch ( ... )
         {
            m_data->Deallocate();
            ResetSelections();
            throw;
         }
      }
      catch ( ... )
      {
         if ( newData != nullptr )
         {
            for ( int i = 0; i < m_numberOfChannels; ++i )
               if ( newData[i] != nullptr )
                  m_allocator.Deallocate( newData[i] ), newData[i] = nullptr;
            m_allocator.Deallocate( newData );
         }
         throw;
      }
   }

   /*!
    * Crops or extends the entire image by applying the specified margins,
    * filling extended regions with the minimum sample value in the native
    * range of the image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * CropBy( left, top, right, bottom, sample_vector() );
    * \endcode
    */
   GenericImage& CropBy( int left, int top, int right, int bottom )
   {
      return CropBy( left, top, right, bottom, sample_vector() );
   }

   /*!
    * Crops the image to the specified rectangular region. Returns a reference
    * to this image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * CropBy( -rect.x0, -rect.y0, rect.x1 - m_width, rect.y1 - m_height, fillValues );
    * \endcode
    */
   template <typename T>
   GenericImage& CropTo( const Rect& rect, const GenericVector<T>& fillValues )
   {
      Rect r = rect.Ordered();
      return CropBy( -r.x0, -r.y0, r.x1 - m_width, r.y1 - m_height, fillValues );
   }

   /*!
    * Crops the image to the specified rectangular region, filling extended
    * regions with the minimum sample value in the native range of the image.
    * Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * CropTo( rect, sample_vector() );
    * \endcode
    */
   GenericImage& CropTo( const Rect& rect )
   {
      return CropTo( rect, sample_vector() );
   }

   /*!
    * Crops the image to the specified rectangular region. Returns a reference
    * to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * CropTo( Rect( x0, y0, x1, y1 ), fillValues );
    * \endcode
    */
   template <typename T>
   GenericImage& CropTo( int x0, int y0, int x1, int y1, const GenericVector<T>& fillValues )
   {
      return CropTo( Rect( x0, y0, x1, y1 ), fillValues );
   }

   /*!
    * Crops the image to the specified rectangular region, filling extended
    * regions with the minimum sample value in the native range of the image.
    * Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * CropTo( x0, y0, x1, y1, sample_vector() );
    * \endcode
    */
   GenericImage& CropTo( int x0, int y0, int x1, int y1 )
   {
      return CropTo( x0, y0, x1, y1, sample_vector() );
   }

   /*!
    * Crops the image to its current rectangular selection. Returns a reference
    * to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * CropTo( SelectedRectangle(), fillValues );
    * \endcode
    */
   template <typename T>
   GenericImage& Crop( const GenericVector<T>& fillValues )
   {
      return CropTo( m_rectangle, fillValues );
   }

   /*!
    * Crops the image to its current rectangular selection, filling extended
    * regions with the minimum sample value in the native range of the image.
    * Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Crop( sample_vector() );
    * \endcode
    */
   GenericImage& Crop()
   {
      return Crop( sample_vector() );
   }

   /*!
    * Shifts (translates) this image by the specified increments in pixels.
    * Returns a reference to this image.
    *
    * \param dx         Horizontal displacement in pixels.
    *
    * \param dy         Vertical displacement in pixels.
    *
    * \param fillValues %Vector of filling sample values for extended areas.
    *                   See the documentation for CropBy() for more
    *                   information.
    *
    * Positive increment values shift the image toward the right and bottom
    * sides. Negative increments shift toward the left and top sides.
    *
    * This member function is equivalent to:
    *
    * \code
    * CropBy( dx, dy, -dx, -dy, fillValues );
    * \endcode
    */
   template <typename T>
   GenericImage& ShiftBy( int dx, int dy, const GenericVector<T>& fillValues )
   {
      return CropBy( dx, dy, -dx, -dy, fillValues );
   }

   /*!
    * Shifts (translates) this image by the specified increments in pixels,
    * filling extended regions with the minimum sample value in the native
    * range of the image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * ShiftBy( dx, dy, sample_vector() );
    * \endcode
    */
   GenericImage& ShiftBy( int dx, int dy )
   {
      return ShiftBy( dx, dy, sample_vector() );
   }

   /*!
    * Shifts (translates) the image to the specified coordinates. Returns a
    * reference to this image.
    *
    * A synonym for ShiftBy( int, int, sample ).
    */
   template <typename T>
   GenericImage& ShiftTo( int x, int y, const GenericVector<T>& fillValues )
   {
      return ShiftBy( x, y, fillValues );
   }

   /*!
    * Shifts (translates) the image to the specified coordinates, filling
    * extended regions with the minimum sample value in the native range of the
    * image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * ShiftTo( x, y, sample_vector() );
    * \endcode
    */
   GenericImage& ShiftTo( int x, int y )
   {
      return ShiftTo( x, y, sample_vector() );
   }

   /*!
    * Shifts (translates) the image to the specified coordinates. Returns a
    * reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * ShiftBy( p.x, p.y, fillValues );
    * \endcode
    */
   template <typename T>
   GenericImage& ShiftTo( const Point& p, const GenericVector<T>& fillValues )
   {
      return ShiftBy( p.x, p.y, fillValues );
   }

   /*!
    * Shifts (translates) the image to the specified coordinates, filling
    * extended regions with the minimum sample value in the native range of the
    * image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * ShiftTo( p, sample_vector() );
    * \endcode
    */
   GenericImage& ShiftTo( const Point& p )
   {
      return ShiftTo( p, sample_vector() );
   }

   /*!
    * Extends or crops the image centered within the specified \a width and
    * \a height in pixels. Returns a reference to this image.
    */
   template <typename T>
   GenericImage& ShiftToCenter( int width, int height, const GenericVector<T>& fillValues )
   {
      int dx2 = (width - m_width) >> 1;
      int dy2 = (height - m_height) >> 1;
      return CropBy( dx2, dy2, width-m_width-dx2, height-m_height-dy2, fillValues );
   }

   /*!
    * Extends or crops the image centered within the specified \a width and
    * \a height in pixels, filling extended regions with the minimum sample
    * value in the native range of the image. Returns a reference to this
    * image.
    *
    * This member function is equivalent to:
    *
    * \code
    * ShiftToCenter( width, height, sample_vector() );
    * \endcode
    */
   GenericImage& ShiftToCenter( int width, int height )
   {
      return ShiftToCenter( width, height, sample_vector() );
   }

   /*!
    * Extends or crops the image at its right and bottom sides to the specified
    * \a width and \a height in pixels. Returns a reference to this image.
    */
   template <typename T>
   GenericImage& ShiftToTopLeft( int width, int height, const GenericVector<T>& fillValues )
   {
      int dx = width - m_width;
      int dy = height - m_height;
      return CropBy( 0, 0, dx, dy, fillValues );
   }

   /*!
    * Extends or crops the image at its right and bottom sides to the specified
    * \a width and \a height in pixels, filling extended regions with the
    * minimum sample value in the native range of the image. Returns a
    * reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * ShiftToTopLeft( width, height, sample_vector() );
    * \endcode
    */
   GenericImage& ShiftToTopLeft( int width, int height )
   {
      return ShiftToTopLeft( width, height, sample_vector() );
   }

   /*!
    * Extends or crops the image at its left and bottom sides to the specified
    * \a width and \a height in pixels. Returns a reference to this image.
    */
   template <typename T>
   GenericImage& ShiftToTopRight( int width, int height, const GenericVector<T>& fillValues )
   {
      int dx = width - m_width;
      int dy = height - m_height;
      return CropBy( dx, 0, 0, dy, fillValues );
   }

   /*!
    * Extends or crops the image at its left and bottom sides to the specified
    * \a width and \a height in pixels, filling extended regions with the
    * minimum sample value in the native range of the image. Returns a
    * reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * ShiftToTopRight( width, height, sample_vector() );
    * \endcode
    */
   GenericImage& ShiftToTopRight( int width, int height )
   {
      return ShiftToTopRight( width, height, sample_vector() );
   }

   /*!
    * Extends or crops the image at its right and top sides to the specified
    * \a width and \a height in pixels. Returns a reference to this image.
    */
   template <typename T>
   GenericImage& ShiftToBottomLeft( int width, int height, const GenericVector<T>& fillValues )
   {
      int dx = width - m_width;
      int dy = height - m_height;
      return CropBy( 0, dy, dx, 0, fillValues );
   }

   /*!
    * Extends or crops the image at its right and top sides to the specified
    * \a width and \a height in pixels, filling extended regions with the
    * minimum sample value in the native range of the image. Returns a
    * reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * ShiftToBottomLeft( width, height, sample_vector() );
    * \endcode
    */
   GenericImage& ShiftToBottomLeft( int width, int height )
   {
      return ShiftToBottomLeft( width, height, sample_vector() );
   }

   /*!
    * Extends or crops the image at its left and top sides to the specified
    * \a width and \a height in pixels. Returns a reference to this image.
    */
   template <typename T>
   GenericImage& ShiftToBottomRight( int width, int height, const GenericVector<T>& fillValues )
   {
      int dx = width - m_width;
      int dy = height - m_height;
      return CropBy( dx, dy, 0, 0, fillValues );
   }

   /*!
    * Extends or crops the image at its left and top sides to the specified
    * \a width and \a height in pixels, filling extended regions with the
    * minimum sample value in the native range of the image. Returns a
    * reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * ShiftToBottomRight( width, height, sample_vector() );
    * \endcode
    */
   GenericImage& ShiftToBottomRight( int width, int height )
   {
      return ShiftToBottomRight( width, height, sample_vector() );
   }

   /*!
    * Shifts (translates) the image to its current anchor point selection.
    * Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * ShiftTo( SelectedPoint(), fillValues );
    * \endcode
    */
   template <typename T>
   GenericImage& Shift( const GenericVector<T>& fillValues )
   {
      return ShiftTo( m_point, fillValues );
   }

   /*!
    * Shifts (translates) the image to its current anchor point selection,
    * filling extended regions with the minimum sample value in the native
    * range of the image. Returns a reference to this image.
    *
    * This member function is equivalent to:
    *
    * \code
    * Shift( sample_vector() );
    * \endcode
    */
   GenericImage& Shift()
   {
      return Shift( sample_vector() );
   }

   // -------------------------------------------------------------------------

   /*!
    * Converts the image to the specified color space. Returns a reference to
    * this object.
    *
    * \param colorSpace  Specifies a color space to convert the image to. See
    *                    the ColorSpace namespace for symbolic constants
    *                    corresponding to all supported color spaces in this
    *                    version of PCL.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * Color space conversion is performed in the current RGB working space
    * (RGBWS) associated with this image.
    *
    * This function operates on the entire image; current selections are
    * neither used nor modified (except the channel range selection, which is
    * reset to defaults when the color space conversion involves a change in
    * the number of nominal channels). Existing alpha channels are never
    * modified by this member function.
    *
    * Before performing the requested color space conversion, this function
    * ensures that the image data is uniquely referenced by this object. If
    * necessary, the previously referenced data are dereferenced (and destroyed
    * immediately if they become unreferenced), and a new local image is
    * allocated and referenced.
    *
    * \note Increments the status monitoring object by the number of pixels
    * (\e not samples) in the image.
    */
   GenericImage& SetColorSpace( color_space colorSpace, int maxProcessors = 0 )
   {
      size_type N = NumberOfPixels();
      if ( N == 0 )
         return *this;

      if ( colorSpace == m_colorSpace )
      {
         m_status += N;
         return *this;
      }

      EnsureUnique();

      if ( m_status.IsInitializationEnabled() )
         m_status.Initialize( "In-place color space conversion: "
                            + ColorSpace::Name( m_colorSpace )
                            + " -> "
                            + ColorSpace::Name( colorSpace ), N );

      int n = m_numberOfChannels;

      if ( m_colorSpace == ColorSpace::Gray )
      {
         sample** oldData = m_pixelData;
         sample** newData = nullptr;

         try
         {
            // Make room for the G and B channels.
            newData = m_allocator.AllocateChannelSlots( 2+n );

            // Put the nominal gray channel into the R slot
            newData[0] = oldData[0];

            // Allocate and copy the G and B channels
            newData[1] = m_allocator.AllocatePixels( N );
            ::memcpy( newData[1], oldData[0], ChannelSize() );
            newData[2] = m_allocator.AllocatePixels( N );
            ::memcpy( newData[2], oldData[0], ChannelSize() );

            // Put existing alpha channels in their corresponding slots
            for ( int i = 1; i < n; ++i )
               newData[i+2] = oldData[i];

            try
            {
               m_pixelData = newData;
               newData = nullptr;
               m_numberOfChannels += 2;
               m_colorSpace = ColorSpace::RGB;
               m_data->UpdateSharedImage();

               ResetChannelRange();

               m_allocator.Deallocate( oldData );
            }
            catch ( ... )
            {
               m_data->Deallocate();
               ResetSelections();
               throw;
            }
         }
         catch ( ... )
         {
            if ( newData != nullptr )
            {
               newData[0] = nullptr;
               if ( newData[1] != nullptr )
                  m_allocator.Deallocate( newData[1] ), newData[1] = nullptr;
               if ( newData[2] != nullptr )
                  m_allocator.Deallocate( newData[2] ), newData[2] = nullptr;
               m_allocator.Deallocate( newData );
               throw;
            }
         }

         if ( colorSpace == ColorSpace::RGB )
         {
            m_status += N;
            return *this;
         }

         n += 2;
      }

      int numberOfThreads = this->NumberOfThreads( N, maxProcessors );
      size_type pixelsPerThread = N/numberOfThreads;

      ThreadData data( *this, N );

      ReferenceArray<ColorSpaceConversionThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new ColorSpaceConversionThread( *this, data, colorSpace,
                                                      i*pixelsPerThread,
                                                      (j < numberOfThreads) ? j*pixelsPerThread : N ) );
      RunThreads( threads, data );
      threads.Destroy();

      m_status = data.status;

      if ( colorSpace == ColorSpace::Gray )
      {
         sample** oldData = m_pixelData;
         sample** newData = nullptr;

         try
         {
            newData = m_allocator.AllocateChannelSlots( n-2 );
            newData[0] = oldData[0];
            for ( int i = 3; i < n; ++i )
               newData[i-2] = oldData[i];

            m_pixelData = newData;
            newData = nullptr;
            m_numberOfChannels -= 2;
            m_colorSpace = ColorSpace::Gray;
            m_data->UpdateSharedImage();

            ResetChannelRange();

            m_allocator.Deallocate( oldData[1] );
            m_allocator.Deallocate( oldData[2] );
            m_allocator.Deallocate( oldData );
         }
         catch ( ... )
         {
            m_data->Deallocate();
            ResetSelections();
            if ( newData != nullptr )
               m_allocator.Deallocate( newData );
            throw;
         }
      }
      else
      {
         m_allocator.SetSharedColor( m_colorSpace = colorSpace, m_RGBWS );
      }

      return *this;
   }

   // -------------------------------------------------------------------------

   /*!
    * Computes the CIE Y component for a subset of pixel samples and stores it
    * in a grayscale image.
    *
    * \param[out] Y     Reference to a destination image, where the computed
    *                   CIE Y component will be stored. This image can be an
    *                   instance of any supported %GenericImage template
    *                   instantiation; all the necessary data type conversions
    *                   are carried out transparently.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * This member function computes the Y component of the CIE XYZ space. CIE Y
    * component values are calculated in the RGB working space (RGBWS)
    * currently associated with the image.
    *
    * If this image is in the grayscale color space, gray source samples are
    * simply copied to the destination image, after conversion to the
    * destination sample data type, if necessary, using pixel traits
    * primitives.
    *
    * If this image is in the CIE XYZ color space, samples from the luminance
    * channel (source channel #1) are simply copied to the destination image,
    * after conversion to the destination sample data type, if necessary.
    *
    * For other color spaces, CIE Y values are obtained by means of the
    * simplest possible intermediate transformation (usually to the CIE XYZ
    * space), converted to the destination sample data type if necessary, and
    * transferred to the destination image.
    *
    * The luminance is computed for the specified subset of pixel samples, or
    * for the current rectangular selection, depending on parameter values. For
    * more information on the rest of parameters of this function, see the
    * documentation for Fill(). The current channel range selection is always
    * ignored by this function.
    *
    * The previous contents of the destination image will be dereferenced (and
    * immediately destroyed if they become unreferenced) and replaced with the
    * computed, uniquely referenced CIE Y data. The destination image will be
    * in the grayscale color space; it will have a single channel corresponding
    * to the luminance of the specified pixel sample subset in this image. The
    * dimensions of the destination image will be the same ones of the
    * specified pixel sample subset, and it will be associated with the same
    * RGBWS as this image.
    *
    * \note In general, computing luminance components as integer values is an
    * error. The GenericPixelTraits instantiation P1 should correspond to a
    * floating point real pixel sample type, either \c float or \c double,
    * depending on the source data type. The recommended types are:\n
    * \n
    * \li FloatPixelTraits for 8-bit and 16-bit integer images, as well as for
    * 32-bit floating point real and complex images.\n
    * \li DoublePixelTraits for 32-bit integer images, as well as for 64-bit
    * floating point real and complex images.
    *
    * \note Increments the status monitoring object by the number of selected
    * pixel samples.
    *
    * \warning Please note that the computed lightness is meaningful as such
    * \e exclusively in the RGB working space of this image.
    */
   template <class P1>
   void GetLuminance( GenericImage<P1>& Y, const Rect& rect = Rect( 0 ), int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseRect( r ) )
      {
         Y.FreeData();
         return;
      }

      Y.AllocateData( r );
      if ( !Y.IsShared() )
         Y.SetRGBWorkingSpace( m_RGBWS );

      size_type N = Y.NumberOfPixels();

      if ( m_colorSpace == ColorSpace::Gray || m_colorSpace == ColorSpace::CIEXYZ )
      {
         if ( m_status.IsInitializationEnabled() )
            m_status.Initialize( "Transferring pixel data", N );

         typename GenericImage<P1>::sample* g = *Y;
         int cY = (m_colorSpace == ColorSpace::Gray) ? 0 : 1;
         if ( r == Bounds() )
            P1::Copy( g, m_pixelData[cY], N );
         else
         {
            const sample* f = PixelAddress( r.LeftTop(), cY );
            for ( int i = 0; i < Y.Height(); ++i, f += m_width, g += Y.Width() )
               P1::Copy( g, f, Y.Width() );
         }

         m_status += N;
      }
      else
      {
         if ( m_status.IsInitializationEnabled() )
            m_status.Initialize( "Computing CIE Y component", N );

         int numberOfThreads = this->NumberOfThreadsForRows( Y.Height(), Y.Width(), maxProcessors );
         int rowsPerThread = Y.Height()/numberOfThreads;

         ThreadData data( *this, N );

         ReferenceArray<GetLuminanceThread<P1> > threads;
         for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
            threads.Add( new GetLuminanceThread<P1>( Y, *this, data, r,
                                                     i*rowsPerThread,
                                                     (j < numberOfThreads) ? j*rowsPerThread : Y.Height() ) );
         RunThreads( threads, data );
         threads.Destroy();

         m_status = data.status;
      }
   }

   /*!
    * Computes the CIE Y component for a subset of pixel samples and stores it
    * in a grayscale image, transported by an ImageVariant object.
    *
    * If the specified %ImageVariant does not transport an image, a new
    * floating point image will be created with the appropriate sample type
    * (either 32-bit or 64-bit floating point, depending on the sample data
    * type of this image).
    */
   void GetLuminance( ImageVariant& Y, const Rect& rect = Rect( 0 ), int maxProcessors = 0 ) const;
   // Implemented in pcl/ImageVariant.h

   /*!
    * Computes the CIE L* component for a subset of pixel samples and stores it
    * in a grayscale image.
    *
    * \param[out] L     Reference to a destination image, where the computed
    *                   CIE L* component will be stored. This image can be an
    *                   instance of any supported %GenericImage template
    *                   instantiation; all the necessary data type conversions
    *                   are carried out transparently.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * This member function computes the L* component of the CIE L*a*b* space.
    * CIE L* component values are calculated in the RGB working space (RGBWS)
    * currently associated with this image.
    *
    * If this image is in the grayscale color space, gray source samples are
    * simply copied to the destination image, after conversion to the
    * destination sample data type, if necessary, using pixel traits
    * primitives.
    *
    * If this image is in the CIE L*a*b* color space, samples from the
    * lightness channel (source channel #0) are simply copied to the
    * destination image, after conversion to the destination sample data type,
    * if necessary.
    *
    * For other color spaces, CIE L* values are obtained by means of the
    * simplest possible intermediate transformation (usually to the CIE L*a*b*
    * space), converted to the destination sample data type if necessary, and
    * transferred to the destination image.
    *
    * The lightness is computed for the specified subset of pixel samples, or
    * for the current rectangular selection, depending on parameter values. For
    * more information on the rest of parameters of this function, see the
    * documentation for Fill(). The current channel range selection is always
    * ignored by this function.
    *
    * The previous contents of the destination image will be dereferenced (and
    * immediately destroyed if they become unreferenced) and replaced with the
    * computed, uniquely referenced CIE L* data. The destination image will be
    * in the grayscale color space; it will have a single channel corresponding
    * to the lightness of the specified pixel sample subset in this image. The
    * dimensions of the destination image will be the same ones of the
    * specified pixel sample subset, and it will be associated with the same
    * RGBWS as this image.
    *
    * \note In general, computing lightness components as integer values is an
    * error. The GenericPixelTraits instantiation P1 should correspond to a
    * floating point real pixel sample type, either \c float or \c double,
    * depending on the source data type. The recommended types are:\n
    * \n
    * \li FloatPixelTraits for 8-bit and 16-bit integer images, as well as for
    * 32-bit floating point real and complex images.\n
    * \li DoublePixelTraits for 32-bit integer images, as well as for 64-bit
    * floating point real and complex images.
    *
    * \note Increments the status monitoring object by the number of modified
    * pixels (\e not samples).
    *
    * \warning Please note that the computed lightness is meaningful as such
    * \e exclusively in the RGB working space of this image.
    */
   template <class P1>
   void GetLightness( GenericImage<P1>& L, const Rect& rect = Rect( 0 ), int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseRect( r ) )
      {
         L.FreeData();
         return;
      }

      L.AllocateData( r );
      if ( !L.IsShared() )
         L.SetRGBWorkingSpace( m_RGBWS );

      size_type N = L.NumberOfPixels();

      if ( m_colorSpace == ColorSpace::Gray || m_colorSpace == ColorSpace::CIELab || m_colorSpace == ColorSpace::CIELch )
      {
         if ( m_status.IsInitializationEnabled() )
            m_status.Initialize( "Transferring pixel data", N );

         typename GenericImage<P1>::sample* g = *L;
         if ( r == Bounds() )
            P1::Copy( g, *m_pixelData, N );
         else
         {
            const sample* f = PixelAddress( r.LeftTop() );
            for ( int i = 0; i < L.Height(); ++i, f += m_width, g += L.Width() )
               P1::Copy( g, f, L.Width() );
         }

         m_status += N;
      }
      else
      {
         if ( m_status.IsInitializationEnabled() )
            m_status.Initialize( "Computing CIE L* component", N );

         int numberOfThreads = this->NumberOfThreadsForRows( L.Height(), L.Width(), maxProcessors );
         int rowsPerThread = L.Height()/numberOfThreads;

         ThreadData data( *this, N );

         ReferenceArray<GetLightnessThread<P1> > threads;
         for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
            threads.Add( new GetLightnessThread<P1>( L, *this, data, r,
                                                     i*rowsPerThread,
                                                     (j < numberOfThreads) ? j*rowsPerThread : L.Height() ) );
         RunThreads( threads, data );
         threads.Destroy();

         m_status = data.status;
      }
   }

   /*!
    * Computes the CIE L* component for a subset of pixel samples and stores it
    * in a grayscale image, transported by an ImageVariant object.
    *
    * If the specified %ImageVariant does not transport an image, a new
    * floating point image will be created with the appropriate sample type
    * (either 32-bit or 64-bit floating point, depending on the sample data
    * type of this image).
    */
   void GetLightness( ImageVariant& L, const Rect& rect = Rect( 0 ), int maxProcessors = 0 ) const;
   // Implemented in pcl/ImageVariant.h

   /*!
    * Computes the intensity HSI component for a subset of pixel samples and
    * stores it in a grayscale image.
    *
    * \param[out] I     Reference to a destination image, where the computed
    *                   intensity component will be stored. This image can be
    *                   an instance of any supported %GenericImage template
    *                   instantiation; all the necessary data type conversions
    *                   are carried out transparently.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * This member function computes the I intensity component in the HSI color
    * ordering scheme. The intensity is useful for applications where a
    * colorimetrically correct component, such as luminance or lightness, is
    * not necessary. For example, the intensity component can be used for
    * feature detection (e.g., star detection) on RGB color images.
    *
    * The intensity component I of an RGB pixel is given by:
    *
    * <tt>I = (Max(R,G,B) + Min(R,G,B))/2</tt>
    *
    * where R, G and B are the red, green and blue components of the RGB pixel,
    * respectively.
    *
    * If this image is in the grayscale or HSI spaces, gray or intensity source
    * samples are simply copied to the destination image, after conversion to
    * the destination sample data type, if necessary, using pixel traits
    * primitives.
    *
    * If this image is in the RGB color space, the intensity components for
    * each pixel is calculated as described above. For other color spaces
    * intermediate RGB components are computed on the fly.
    *
    * The intensity is computed for the specified subset of pixel samples, or
    * for the current rectangular selection, depending on parameter values. For
    * more information on the rest of parameters of this function, see the
    * documentation for Fill(). The current channel range selection is always
    * ignored by this function.
    *
    * The previous contents of the destination image will be dereferenced (and
    * immediately destroyed if they become unreferenced) and replaced with the
    * computed, uniquely referenced intensity data. The destination image will
    * be in the grayscale color space; it will have a single channel
    * corresponding to the intensity of the specified pixel sample subset in
    * this image. The dimensions of the destination image will be the same ones
    * of the specified pixel sample subset, and it will be associated with the
    * same RGBWS as this image.
    *
    * \note Increments the status monitoring object by the number of modified
    * pixels (\e not samples).
    */
   template <class P1>
   void GetIntensity( GenericImage<P1>& I, const Rect& rect = Rect( 0 ), int maxProcessors = 0 ) const
   {
      Rect r = rect;
      if ( !ParseRect( r ) )
      {
         I.FreeData();
         return;
      }

      I.AllocateData( r );
      if ( !I.IsShared() )
         I.SetRGBWorkingSpace( m_RGBWS );

      size_type N = I.NumberOfPixels();

      if ( m_colorSpace == ColorSpace::Gray || m_colorSpace == ColorSpace::HSI )
      {
         if ( m_status.IsInitializationEnabled() )
            m_status.Initialize( "Transferring pixel data", N );

         typename GenericImage<P1>::sample* g = *I;
         int cI = (m_colorSpace == ColorSpace::Gray) ? 0 : 2;
         if ( r == Bounds() )
            P1::Copy( g, m_pixelData[cI], N );
         else
         {
            const sample* f = PixelAddress( r.LeftTop(), cI );
            for ( int i = 0; i < I.Height(); ++i, f += m_width, g += I.Width() )
               P1::Copy( g, f, I.Width() );
         }

         m_status += N;
      }
      else
      {
         if ( m_status.IsInitializationEnabled() )
            m_status.Initialize( "Computing intensity component", N );

         int numberOfThreads = this->NumberOfThreadsForRows( I.Height(), I.Width(), maxProcessors );
         int rowsPerThread = I.Height()/numberOfThreads;

         ThreadData data( *this, N );

         ReferenceArray<GetIntensityThread<P1> > threads;
         for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
            threads.Add( new GetIntensityThread<P1>( I, *this, data, r,
                                                     i*rowsPerThread,
                                                     (j < numberOfThreads) ? j*rowsPerThread : I.Height() ) );
         RunThreads( threads, data );
         threads.Destroy();

         m_status = data.status;
      }
   }

   /*!
    * Computes the intensity HSI component for a subset of pixel samples and
    * stores it in a grayscale image, transported by an ImageVariant object.
    *
    * If the specified %ImageVariant does not transport an image, a new
    * floating point image will be created with the appropriate sample type
    * (either 32-bit or 64-bit floating point, depending on the sample data
    * type of this image).
    */
   void GetIntensity( ImageVariant& I, const Rect& rect = Rect( 0 ), int maxProcessors = 0 ) const;
   // Implemented in pcl/ImageVariant.h

   // -------------------------------------------------------------------------

   /*!
    * Replaces the CIE Y component of a subset of pixel samples with data
    * extracted from another image.
    *
    * \param Y          Reference to a source image, from which the CIE Y
    *                   components will be obtained. This image can be in any
    *                   supported color space, and can be an instance of any
    *                   supported template instantiation of %GenericImage; all
    *                   the necessary color space and data type conversions are
    *                   carried out transparently.
    *
    * \param point      Target point in image coordinates. This is the upper
    *                   left corner of the target rectangular region in this
    *                   image where CIE Y components will be replaced with
    *                   source values. If one or both point coordinates are
    *                   equal to \c int_max (the maximum possible value of type
    *                   \c int), then the current anchor point selection will
    *                   be used. The default value is Point( int_max ).
    *
    * \param rect       Source rectangular region. If this parameter defines an
    *                   empty rectangle, the current rectangular selection in
    *                   the source image will be used. If the specified
    *                   rectangle is not empty and extends beyond source image
    *                   boundaries, only the intersection with the source image
    *                   will be used. If that intersection does not exist, then
    *                   this member function has no effect. The default value
    *                   is an empty rectangle.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * This member function computes and replaces the Y component of the CIE XYZ
    * color space. Source CIE Y component values are calculated in the RGB
    * working space (RGBWS) associated with the source image \a Y. Replacement
    * of CIE Y components in this (target) image is performed by
    * transformations in the RGBWS associated with this image. This ensures
    * that only pure illumination data is transferred between both images,
    * without any chrominance contamination.
    *
    * If the source image is in the grayscale or CIE XYZ color spaces, samples
    * from its first or second channel, respectively, are directly taken as
    * source values, requiring no additional conversion.
    *
    * Once a source CIE Y value is available, either by direct extraction or
    * through the simplest possible intermediate transformation (usually to the
    * CIE L*a*b* or CIE XYZ spaces), it is inserted in the corresponding
    * destination pixel by means of a series of transformations in the target
    * RGBWS, whose number and complexity depend on the destination color space.
    * Additional sample data type conversions may take place using pixel traits
    * primitives, if the source and destination data types differ. Roundoff
    * errors are minimized during the whole process.
    *
    * \note Increments the status monitoring object by the number of modified
    * pixels (\e not samples).
    */
   template <class P1>
   GenericImage& SetLuminance( const GenericImage<P1>& Y,
                               const Point& point = Point( int_max ), const Rect& rect = Rect( 0 ),
                               int maxProcessors = 0 )
   {
      Rect r = rect;
      if ( !Y.ParseRect( r ) )
         return *this;

      Point p = point;
      if ( p.x == int_max || p.y == int_max )
         p = m_point;

      if ( p.x < 0 )
      {
         if ( (r.x0 -= p.x) >= r.x1 )
            return *this;
         p.x = 0;
      }
      else if ( p.x >= m_width )
         return *this;

      if ( p.y < 0 )
      {
         if ( (r.y0 -= p.y) >= r.y1 )
            return *this;
         p.y = 0;
      }
      else if ( p.y >= m_height )
         return *this;

      r.ResizeTo( pcl::Min( m_width - p.x, r.Width() ),
                  pcl::Min( m_height - p.y, r.Height() ) );

      size_type N = size_type( r.Width() )*size_type( r.Height() );

      EnsureUnique();

      if (   m_colorSpace == ColorSpace::Gray &&
          (Y.ColorSpace() == ColorSpace::Gray || Y.ColorSpace() == ColorSpace::CIEXYZ) )
      {
         if ( m_status.IsInitializationEnabled() )
            m_status.Initialize( "Transferring pixel data", N );

         int c0 = (Y.ColorSpace() == ColorSpace::Gray) ? 0 : 1;
         const typename GenericImage<P1>::sample* g = Y.PixelAddress( r.LeftTop(), c0 );
         sample* f = PixelAddress( p );
         for ( int i = 0, w = r.Width(), h = r.Height(); i < h; ++i, f += m_width, g += Y.Width(), m_status += w )
            P::Copy( f, g, w );
      }
      else
      {
         if ( m_status.IsInitializationEnabled() )
            m_status.Initialize( "Importing CIE Y component", N );

         int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
         int rowsPerThread = r.Height()/numberOfThreads;

         ThreadData data( *this, N );

         ReferenceArray<SetLuminanceThread<P1> > threads;
         for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
            threads.Add( new SetLuminanceThread<P1>( *this, Y, data, p, r,
                                                     i*rowsPerThread,
                                                     (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
         RunThreads( threads, data );
         threads.Destroy();

         m_status = data.status;
      }

      return *this;
   }

   /*!
    * Replaces the CIE Y component of a subset of pixel samples with data
    * extracted from another image, transported by an ImageVariant object.
    *
    * If the specified %ImageVariant does not transport an image, a new
    * floating point image will be created with the appropriate sample type
    * (either 32-bit or 64-bit floating point, depending on the sample data
    * type of this image).
    */
   GenericImage& SetLuminance( const ImageVariant& Y,
                               const Point& point = Point( int_max ), const Rect& rect = Rect( 0 ),
                               int maxProcessors = 0 );
   // Implemented in pcl/ImageVariant.h

   /*!
    * Replaces the CIE L* component of a subset of pixel samples with data
    * extracted from another image.
    *
    * \param L          Reference to a source image, from which the CIE L*
    *                   components will be obtained. This image can be in any
    *                   supported color space, and can be an instance of any
    *                   supported template instantiation of %GenericImage; all
    *                   the necessary color space and data type conversions are
    *                   carried out transparently.
    *
    * \param point      Target point in image coordinates. This is the upper
    *                   left corner of the target rectangular region in this
    *                   image where CIE L* components will be replaced with
    *                   source values. If one or both point coordinates are
    *                   equal to \c int_max (the maximum possible value of type
    *                   \c int), then the current anchor point selection will
    *                   be used. The default value is Point( int_max ).
    *
    * \param rect       Source rectangular region. If this parameter defines an
    *                   empty rectangle, the current rectangular selection in
    *                   the source image will be used. If the specified
    *                   rectangle is not empty and extends beyond source image
    *                   boundaries, only the intersection with the source image
    *                   will be used. If that intersection does not exist, then
    *                   this member function has no effect. The default value
    *                   is an empty rectangle.
    *
    * \param maxProcessors    If a value greater than zero is specified, it is
    *          the maximum number of concurrent threads that this function can
    *          execute. If zero or a negative value is specified, the current
    *          thread limit for this image will be used instead (see
    *          AbstractImage::SetMaxProcessors()). The default value is zero.
    *
    * This member function computes and replaces the L* component of the CIE
    * L*a*b* color space. Source CIE L* component values are calculated in the
    * RGB working space (RGBWS) associated with the source image \a L.
    * Replacement of CIE L* components in this (target) image is performed by
    * transformations in the RGBWS associated with this image. This ensures
    * that only pure illumination data is transferred between both images,
    * without any chrominance contamination.
    *
    * If the source image is in the grayscale, CIE L*a*b* or CIE L*c*h* color
    * spaces, samples from its first channel are directly taken as source
    * values, requiring no additional conversion.
    *
    * Once a source CIE L* value is available, either by direct extraction or
    * through the simplest possible intermediate transformation (usually to the
    * CIE L*a*b* or CIE XYZ spaces), it is inserted in the corresponding
    * destination pixel by means of a series of transformations in the target
    * RGBWS, whose number and complexity depend on the destination color space.
    * Additional sample data type conversions may take place using pixel traits
    * primitives, if the source and destination data types differ. Roundoff
    * errors are minimized during the whole process.
    *
    * \note Increments the status monitoring object by the number of modified
    * pixels (\e not samples).
    */
   template <class P1>
   GenericImage& SetLightness( const GenericImage<P1>& L,
                               const Point& point = Point( int_max ), const Rect& rect = Rect( 0 ),
                               int maxProcessors = 0 )
   {
      Rect r = rect;
      if ( !L.ParseRect( r ) )
         return *this;

      Point p = point;
      if ( p.x == int_max || p.y == int_max )
         p = m_point;

      if ( p.x < 0 )
      {
         if ( (r.x0 -= p.x) >= r.x1 )
            return *this;
         p.x = 0;
      }
      else if ( p.x >= m_width )
         return *this;

      if ( p.y < 0 )
      {
         if ( (r.y0 -= p.y) >= r.y1 )
            return *this;
         p.y = 0;
      }
      else if ( p.y >= m_height )
         return *this;

      r.ResizeTo( pcl::Min( m_width - p.x, r.Width() ),
                  pcl::Min( m_height - p.y, r.Height() ) );

      size_type N = size_type( r.Width() )*size_type( r.Height() );

      EnsureUnique();

      if (   m_colorSpace == ColorSpace::Gray &&
          (L.ColorSpace() == ColorSpace::Gray ||
           L.ColorSpace() == ColorSpace::CIELab || L.ColorSpace() == ColorSpace::CIELch) )
      {
         if ( m_status.IsInitializationEnabled() )
            m_status.Initialize( "Transferring pixel data", N );

         const typename GenericImage<P1>::sample* g = L.PixelAddress( r.LeftTop() );
         sample* f = PixelAddress( p );
         for ( int i = 0, w = r.Width(), h = r.Height(); i < h; ++i, f += m_width, g += L.Width(), m_status += w )
            P::Copy( f, g, w );
      }
      else
      {
         if ( m_status.IsInitializationEnabled() )
            m_status.Initialize( "Importing CIE L* component", N );

         int numberOfThreads = this->NumberOfThreadsForRows( r.Height(), r.Width(), maxProcessors );
         int rowsPerThread = r.Height()/numberOfThreads;

         ThreadData data( *this, N );

         ReferenceArray<SetLightnessThread<P1> > threads;
         for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
            threads.Add( new SetLightnessThread<P1>( *this, L, data, p, r,
                                                     i*rowsPerThread,
                                                     (j < numberOfThreads) ? j*rowsPerThread : r.Height() ) );
         RunThreads( threads, data );
         threads.Destroy();

         m_status = data.status;
      }

      return *this;
   }

   /*!
    * Replaces the CIE L* component of a subset of pixel samples with data
    * extracted from another image, transported by an ImageVariant object.
    *
    * If the specified %ImageVariant does not transport an image, a new
    * floating point image will be created with the appropriate sample type
    * (either 32-bit or 64-bit floating point, depending on the sample data
    * type of this image).
    */
   GenericImage& SetLightness( const ImageVariant& L,
                               const Point& point = Point( int_max ), const Rect& rect = Rect( 0 ),
                               int maxProcessors = 0 );
   // Implemented in pcl/ImageVariant.h

   /*!
    * Compression of image pixel samples.
    *
    * \param compressor A compression algorithm that will be used to compress
    *                   pixel sample data from this image.
    *
    * \param rect       A rectangular region in image pixel coordinates. This
    *                   region determines the subset of pixel samples that will
    *                   be compressed. If this parameter is not specified, or
    *                   if an empty rectangle is specified, this function will
    *                   compress pixel samples in the current rectangular
    *                   selection of this image, that is, SelectedRectangle().
    *
    * \param channel    Channel index. Must be the zero-based index of an
    *                   existing channel in this image, or an integer < 0. If
    *                   this parameter is not specified or a negative integer,
    *                   this function will compress pixel samples in the
    *                   currently selected channel of this image, that is,
    *                   SelectedChannel().
    *
    * \param perf       If non-null, pointer to a Compression::Performance
    *                   structure where performance data will be provided.
    *
    * Returns a dynamic array of compressed sub-blocks. Each array element is a
    * Compression::Subblock structure with the compressed data and the
    * corresponding uncompressed length in bytes. If compression succeeds, the
    * returned array will have at least one element.
    *
    * Data will be compressed for the intersection of the specified (or
    * implicitly selected) rectangular region with this image. All predefined
    * pixel sample types are supported, including integer, real and complex
    * pixels.
    *
    * If there is no intersection between the rectangular region and the image,
    * if an invalid channel index is specified, or if one or more sub-blocks
    * are not compressible with the specified compression algorithm, this
    * function returns an empty array.
    */
   Compression::subblock_list Compress( const Compression& compressor,
                                        const Rect& rect = Rect( 0 ), int channel = -1,
                                        Compression::Performance* perf = nullptr ) const
   {
      Rect r = rect;
      if ( !ParseSelection( r, channel ) )
         return Compression::subblock_list();
      if ( r == Bounds() )
         return compressor.Compress( m_pixelData[channel], ChannelSize(), perf );
      GenericImage<P> subimage( *this, r, channel, channel );
      return compressor.Compress( subimage[0], subimage.ChannelSize(), perf );
   }

   // -------------------------------------------------------------------------

private:

   /*!
    * \struct Data
    * \internal
    * Reference-counted image data structure.
    */
   struct Data : public ReferenceCounter
   {
      /*!
       * Pixel data
       *
       * Each element in the data array points to a contiguous block of pixel
       * samples that stores one channel of the image. This includes all
       * nominal and alpha channels.
       */
      sample**        data = nullptr;

      /*!
       * Pixel allocator
       *
       * The allocator is responsible for all allocations and deallocations of
       * pixel data and the associated structures. The allocator works on local
       * or external storage, depending on whether this instance is a local or
       * a shared image. For shared images, the allocator takes into account
       * ownership of the shared object for controlled destruction of shared
       * resources.
       */
      pixel_allocator allocator;

      /*!
       * Image geometry
       */
      Geometry        geometry;

      /*!
       * Image color
       */
      Color           color;

      /*!
       * Constructs an empty local image.
       */
      Data( GenericImage* image )
      {
         LinkWithClientImage( image );
      }

      /*!
       * Constructs an aliased shared image.
       */
      Data( GenericImage* image, void* handle ) : allocator( handle )
      {
         SynchronizeWithSharedImage();
         LinkWithClientImage( image );
      }

      /*!
       * Constructs a newly created shared image.
       */
      Data( GenericImage* image, int width, int height, int numberOfChannels, int colorSpace ) :
         allocator( width, height, numberOfChannels, colorSpace )
      {
         SynchronizeWithSharedImage();
         LinkWithClientImage( image );
      }

      void Attach( GenericImage* image )
      {
         ReferenceCounter::Attach();
         LinkWithClientImage( image );
      }

      ~Data()
      {
         if ( IsShared() )
            Reset();
         else
            Deallocate();
      }

      bool IsShared() const
      {
         return allocator.IsShared();
      }

      bool IsEmpty() const
      {
         return data == nullptr;
      }

      void Allocate( int width, int height, int numberOfChannels, color_space colorSpace )
      {
         if ( width <= 0 || height <= 0 || numberOfChannels <= 0 )
         {
            Deallocate();
            return;
         }

         if ( numberOfChannels < ColorSpace::NumberOfNominalChannels( colorSpace ) )
            throw Error( "GenericImage::Data::Allocate(): Insufficient number of channels" );

         if ( data != nullptr )
         {
            if ( size_type( width )*size_type( height ) == geometry.NumberOfPixels() )
            {
               if ( numberOfChannels != geometry.numberOfChannels )
               {
                  sample** newData = nullptr;
                  int m = pcl::Min( geometry.numberOfChannels, numberOfChannels );

                  try
                  {
                     newData = allocator.AllocateChannelSlots( numberOfChannels );
                     for ( int i = 0; i < m; ++i )
                        newData[i] = data[i];
                     for ( int i = m; i < numberOfChannels; ++i )
                        newData[i] = allocator.AllocatePixels( width, height );

                     try
                     {
                        for ( int i = m; i < geometry.numberOfChannels; ++i )
                           if ( data[i] != nullptr )
                              allocator.Deallocate( data[i] ), data[i] = nullptr;
                        allocator.Deallocate( data );
                        data = newData;
                        newData = nullptr;
                     }
                     catch ( ... )
                     {
                        Deallocate();
                        throw;
                     }
                  }
                  catch ( ... )
                  {
                     if ( newData != nullptr )
                     {
                        for ( int i = m; i < numberOfChannels; ++i )
                           if ( newData[i] != nullptr )
                              allocator.Deallocate( newData[i] ), newData[i] = nullptr;
                        allocator.Deallocate( newData );
                     }
                     throw;
                  }
               }
            }
            else
            {
               sample** newData = nullptr;

               try
               {
                  newData = allocator.AllocateChannelSlots( numberOfChannels );
                  for ( int i = 0; i < numberOfChannels; ++i )
                     newData[i] = allocator.AllocatePixels( width, height );

                  try
                  {
                     for ( int i = 0; i < geometry.numberOfChannels; ++i )
                        if ( data[i] != nullptr )
                           allocator.Deallocate( data[i] ), data[i] = nullptr;
                     allocator.Deallocate( data );
                     data = newData;
                     newData = nullptr;
                  }
                  catch ( ... )
                  {
                     Deallocate();
                     throw;
                  }
               }
               catch ( ... )
               {
                  if ( newData != nullptr )
                  {
                     for ( int i = 0; i < numberOfChannels; ++i )
                        if ( newData[i] != nullptr )
                           allocator.Deallocate( newData[i] ), newData[i] = nullptr;
                     allocator.Deallocate( newData );
                  }
                  throw;
               }
            }
         }
         else
         {
            try
            {
               data = allocator.AllocateChannelSlots( numberOfChannels );
               for ( int i = 0; i < numberOfChannels; ++i )
                  data[i] = allocator.AllocatePixels( width, height );
            }
            catch ( ... )
            {
               if ( data != nullptr )
               {
                  for ( int i = 0; i < numberOfChannels; ++i )
                     if ( data[i] != nullptr )
                        allocator.Deallocate( data[i] ), data[i] = nullptr;
                  allocator.Deallocate( data );
                  Reset();
               }
               throw;
            }
         }

         geometry.width = width;
         geometry.height = height;
         geometry.numberOfChannels = numberOfChannels;

         color.colorSpace = colorSpace;

         UpdateSharedImage();
      }

      void Import( sample** newData, int width, int height, int numberOfChannels, color_space colorSpace )
      {
         if ( newData != data )
         {
            Deallocate();

            if ( newData != nullptr && width > 0 && height > 0 && numberOfChannels > 0 )
            {
               if ( numberOfChannels < ColorSpace::NumberOfNominalChannels( colorSpace ) )
                  throw Error( "GenericImage::Data::Import(): Insufficient number of channels" );

               data = newData;

               geometry.width = width;
               geometry.height = height;
               geometry.numberOfChannels = numberOfChannels;

               color.colorSpace = colorSpace;

               UpdateSharedImage();
            }
         }
      }

      sample** Release()
      {
         sample** oldData = data;
         Reset();
         UpdateSharedImage();
         return oldData;
      }

      void Deallocate()
      {
         if ( data != nullptr )
         {
            for ( int i = 0; i < geometry.numberOfChannels; ++i )
               if ( data[i] != nullptr )
                  allocator.Deallocate( data[i] ), data[i] = nullptr;
            allocator.Deallocate( data );
            Reset();
            UpdateSharedImage();
         }
      }

      Data* Clone( GenericImage* image ) const
      {
         Data* clone = nullptr;
         try
         {
            clone = new Data;

            if ( !IsEmpty() )
            {
               clone->data = clone->allocator.AllocateChannelSlots( geometry.numberOfChannels );
               for ( int i = 0; i < geometry.numberOfChannels; ++i )
               {
                  clone->data[i] = clone->allocator.AllocatePixels( geometry.width, geometry.height );
                  P::Copy( clone->data[i], data[i], geometry.NumberOfPixels() );
               }

               clone->geometry.Assign( geometry );
               clone->color.Assign( color );
            }

            clone->LinkWithClientImage( image );
            return clone;
         }
         catch ( ... )
         {
            if ( clone != nullptr )
            {
               clone->Deallocate();
               delete clone;
            }
            throw;
         }
      }

      void Reset()
      {
         data = nullptr;
         geometry.Reset();
         color.Reset();
      }

      void UpdateSharedImage()
      {
         allocator.SetSharedData( data );
         allocator.SetSharedGeometry( geometry.width, geometry.height, geometry.numberOfChannels );
         allocator.SetSharedColor( color.colorSpace, color.RGBWS );
      }

      void SynchronizeWithSharedImage()
      {
         data = allocator.GetSharedData();
         allocator.GetSharedGeometry( geometry.width, geometry.height, geometry.numberOfChannels );
         allocator.GetSharedColor( color.colorSpace, color.RGBWS );
      }

   private:

      Data() :
         ReferenceCounter(),
         data( nullptr ), allocator(), geometry(), color()
      {
      }

      void LinkWithClientImage( GenericImage* image )
      {
         if ( image != nullptr )
         {
            image->m_geometry = &geometry;
            image->m_color = &color;
         }
      }
   };

   /*!
    * \internal
    * The reference-counted image data.
    */
   Data* m_data = nullptr;

   /*!
    * \internal
    * Dereferences image data and disposes it if it becomes garbage.
    */
   void DetachFromData()
   {
      if ( !m_data->Detach() )
         delete m_data;
   }

   // -------------------------------------------------------------------------

   class RectThreadBase : public Thread
   {
   public:

      RectThreadBase( const GenericImage& image, const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         Thread(),
         m_image( image ), m_rect( rect ), m_ch1( ch1 ), m_ch2( ch2 ), m_firstRow( firstRow ), m_endRow( endRow )
      {
      }

      virtual void Run()
      {
         int w = m_rect.Width();
         int dw = m_image.Width() - w;

         if ( m_image.IsRangeClippingEnabled() )
         {
            sample clipLow = P::ToSample( m_image.RangeClipLow() );
            sample clipHigh = P::ToSample( m_image.RangeClipHigh() );

            for ( int i = m_ch1; i <= m_ch2; ++i )
            {
               const sample* f = m_image.PixelAddress( m_rect.x0, m_rect.y0+m_firstRow, i );
               for ( int j = m_firstRow; j < m_endRow; ++j, f += dw )
                  for ( const sample* f1 = f+w; f < f1; ++f )
                     if ( *f > clipLow && *f < clipHigh )
                        Perform( f );
            }
         }
         else
         {
            for ( int i = m_ch1; i <= m_ch2; ++i )
            {
               const sample* f = m_image.PixelAddress( m_rect.x0, m_rect.y0+m_firstRow, i );
               for ( int j = m_firstRow; j < m_endRow; ++j, f += dw )
                  for ( const sample* f1 = f+w; f < f1; ++f )
                     Perform( f );
            }
         }

         PostProcess();
      }

   protected:

      const GenericImage& m_image;
      const Rect&         m_rect;
            int           m_ch1, m_ch2;
            int           m_firstRow, m_endRow;

      virtual void Perform( const sample* ) {} // not pure because of VC++ 'peculiarities'
      virtual void PostProcess() {}
   };

   // -------------------------------------------------------------------------

   class CountThread : public RectThreadBase
   {
   public:

      uint64 count;

      CountThread( const GenericImage& image, const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         RectThreadBase( image, rect, ch1, ch2, firstRow, endRow ), count( 0 )
      {
      }

      virtual void Run()
      {
         // These threads are only used when range clipping is enabled.
         int w = this->m_rect.Width();
         int dw = this->m_image.Width() - w;
         sample clipLow = P::ToSample( this->m_image.RangeClipLow() );
         sample clipHigh = P::ToSample( this->m_image.RangeClipHigh() );

         for ( int i = this->m_ch1; i <= this->m_ch2; ++i )
         {
            const sample* f = this->m_image.PixelAddress( this->m_rect.x0, this->m_rect.y0+this->m_firstRow, i );
            for ( int j = this->m_firstRow; j < this->m_endRow; ++j, f += dw )
               for ( const sample* f1 = f+w; f < f1; ++f )
                  if ( *f > clipLow && *f < clipHigh )
                     ++count;
         }
      }
   };

   // -------------------------------------------------------------------------

   class MinThread : public RectThreadBase
   {
   public:

      sample min;
      bool initialized : 1;

      MinThread( const GenericImage& image, const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         RectThreadBase( image, rect, ch1, ch2, firstRow, endRow ), initialized( false )
      {
      }

   private:

      virtual void Perform( const sample* f )
      {
         if ( initialized )
         {
            if ( *f < min )
               min = *f;
         }
         else
         {
            min = *f;
            initialized = true;
         }
      }
   };

   // -------------------------------------------------------------------------

   class MaxThread : public RectThreadBase
   {
   public:

      sample max;
      bool initialized : 1;

      MaxThread( const GenericImage& image, const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         RectThreadBase( image, rect, ch1, ch2, firstRow, endRow ), initialized( false )
      {
      }

   private:

      virtual void Perform( const sample* f )
      {
         if ( initialized )
         {
            if ( max < *f )
               max = *f;
         }
         else
         {
            max = *f;
            initialized = true;
         }
      }
   };

   // -------------------------------------------------------------------------

   class MinMaxThread : public RectThreadBase
   {
   public:

      sample min;
      sample max;
      bool initialized : 1;

      MinMaxThread( const GenericImage& image, const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         RectThreadBase( image, rect, ch1, ch2, firstRow, endRow ), initialized( false )
      {
      }

   private:

      virtual void Perform( const sample* f )
      {
         if ( initialized )
         {
            if ( *f < min )
               min = *f;
            else if ( max < *f )
               max = *f;
         }
         else
         {
            min = max = *f;
            initialized = true;
         }
      }
   };

   // -------------------------------------------------------------------------

   class ExtremePosThreadBase : public RectThreadBase
   {
   public:

      int cmin, cmax;
      Point pmin, pmax;
      bool initialized : 1;

      ExtremePosThreadBase( const GenericImage& image, const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         RectThreadBase( image, rect, ch1, ch2, firstRow, endRow ), initialized( false ), m_amin( 0 ), m_amax( 0 )
      {
      }

   protected:

      const sample* m_amin;
      const sample* m_amax;

      virtual void PostProcess()
      {
         if ( initialized )
         {
            if ( m_amin != 0 )
               for ( int i = this->m_ch1; i <= this->m_ch2; ++i )
                  if ( m_amin >= this->m_image[i] && m_amin < (this->m_image[i] + this->m_image.NumberOfPixels()) )
                  {
                     cmin = i;
                     pmin.x = (m_amin - this->m_image[i]) % this->m_image.Width();
                     pmin.y = (m_amin - this->m_image[i]) / this->m_image.Width();
                     break;
                  }
            if ( m_amax != 0 )
               for ( int i = this->m_ch1; i <= this->m_ch2; ++i )
                  if ( m_amax >= this->m_image[i] && m_amax < (this->m_image[i] + this->m_image.NumberOfPixels()) )
                  {
                     cmax = i;
                     pmax.x = (m_amax - this->m_image[i]) % this->m_image.Width();
                     pmax.y = (m_amax - this->m_image[i]) / this->m_image.Width();
                     break;
                  }
         }
      }
   };

   // -------------------------------------------------------------------------

   class MinPosThread : public ExtremePosThreadBase
   {
   public:

      sample min;

      MinPosThread( const GenericImage& image, const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         ExtremePosThreadBase( image, rect, ch1, ch2, firstRow, endRow )
      {
      }

   private:

      virtual void Perform( const sample* f )
      {
         if ( this->initialized )
         {
            if ( *f < min )
               min = *(this->m_amin = f);
         }
         else
         {
            min = *(this->m_amin = f);
            this->initialized = true;
         }
      }
   };

   // -------------------------------------------------------------------------

   class MaxPosThread : public ExtremePosThreadBase
   {
   public:

      sample max;

      MaxPosThread( const GenericImage& image, const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         ExtremePosThreadBase( image, rect, ch1, ch2, firstRow, endRow )
      {
      }

   private:

      virtual void Perform( const sample* f )
      {
         if ( this->initialized )
         {
            if ( max < *f )
               max = *(this->m_amax = f);
         }
         else
         {
            max = *(this->m_amax = f);
            this->initialized = true;
         }
      }
   };

   // -------------------------------------------------------------------------

   class MinMaxPosThread : public ExtremePosThreadBase
   {
   public:

      sample min, max;

      MinMaxPosThread( const GenericImage& image, const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         ExtremePosThreadBase( image, rect, ch1, ch2, firstRow, endRow )
      {
      }

   private:

      virtual void Perform( const sample* f )
      {
         if ( this->initialized )
         {
            if ( *f < min )
               min = *(this->m_amin = f);
            else if ( max < *f )
               max = *(this->m_amax = f);
         }
         else
         {
            min = max = *(this->m_amin = this->m_amax = f);
            this->initialized = true;
         }
      }
   };

   // -------------------------------------------------------------------------

   class SumThread : public RectThreadBase
   {
   public:

      double s;
      size_type n;

      SumThread( const GenericImage& image, const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         RectThreadBase( image, rect, ch1, ch2, firstRow, endRow ), s( 0 ), n( 0 ), e( 0 )
      {
      }

   protected:

      double e;

      void SumStep( double x )
      {
         double y = x - e;
         double t = s + y;
         e = (t - s) - y;
         s = t;
         ++n;
      }

      virtual void Perform( const sample* f )
      {
         double v; P::FromSample( v, *f );
         SumStep( v );
      }
   };

   // -------------------------------------------------------------------------

   class SumSqrThread : public SumThread
   {
   public:

      SumSqrThread( const GenericImage& image, const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         SumThread( image, rect, ch1, ch2, firstRow, endRow )
      {
      }

   private:

      virtual void Perform( const sample* f )
      {
         double v; P::FromSample( v, *f );
         this->SumStep( v*v );
      }
   };

   // -------------------------------------------------------------------------

   class SumAbsThread : public SumThread
   {
   public:

      SumAbsThread( const GenericImage& image, const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         SumThread( image, rect, ch1, ch2, firstRow, endRow )
      {
      }

   private:

      virtual void Perform( const sample* f )
      {
         double v; P::FromSample( v, *f );
         this->SumStep( pcl::Abs( v ) );
      }
   };

   // -------------------------------------------------------------------------

   class SmpThread : public RectThreadBase
   {
   public:

      Array<sample> samples;
      size_type n;

      SmpThread( const GenericImage& image, const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         RectThreadBase( image, rect, ch1, ch2, firstRow, endRow ), samples(), n( 0 )
      {
         size_type N = size_type(  this->m_rect.Width() )
                     * size_type( this->m_endRow - this->m_firstRow )
                     * (1 + this->m_ch2 - this->m_ch1);
         samples = Array<sample>( N );
      }

   private:

      virtual void Perform( const sample* f )
      {
         samples[n++] = *f;
      }
   };

   // -------------------------------------------------------------------------

   class VarThread : public RectThreadBase
   {
   public:

      double var;
      double eps;

      VarThread( const GenericImage& image, double mean, const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         RectThreadBase( image, rect, ch1, ch2, firstRow, endRow ), var( 0 ), eps( 0 ), m_mean( mean )
      {
      }

   private:

      double m_mean;

      virtual void Perform( const sample* f )
      {
         double d; P::FromSample( d, *f );
         d -= m_mean;
         var += d*d;
         eps += d;
      }
   };

   // -------------------------------------------------------------------------

   class SmpAbsDevThread : public RectThreadBase
   {
   public:

      Array<double> devs;
      size_type n;

      SmpAbsDevThread( const GenericImage& image, double center, const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         RectThreadBase( image, rect, ch1, ch2, firstRow, endRow ), devs(), n( 0 ), m_center( center )
      {
         size_type N = size_type(  this->m_rect.Width() )
                     * size_type( this->m_endRow - this->m_firstRow )
                     * (1 + this->m_ch2 - this->m_ch1);
         devs = Array<double>( N );
      }

   private:

      double m_center;

      virtual void Perform( const sample* f )
      {
         double v; P::FromSample( v, *f );
         devs[n++] = pcl::Abs( v - m_center );
      }
   };

   // -------------------------------------------------------------------------

   class SumAbsDevThread : public SumThread
   {
   public:

      SumAbsDevThread( const GenericImage& image, double center, const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         SumThread( image, rect, ch1, ch2, firstRow, endRow ), m_center( center )
      {
      }

   private:

      double m_center;

      virtual void Perform( const sample* f )
      {
         double v; P::FromSample( v, *f );
         this->SumStep( pcl::Abs( v - m_center ) );
      }
   };

   // -------------------------------------------------------------------------

   class BWMVThread : public RectThreadBase
   {
   public:

      double num;
      double den;
      size_type n;

      BWMVThread( const GenericImage& image, double center, double kd,
                  const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         RectThreadBase( image, rect, ch1, ch2, firstRow, endRow ), num( 0 ), den( 0 ), n( 0 ), m_center( center ), m_kd( kd )
      {
      }

   private:

      double m_center;
      double m_kd;

      virtual void Perform( const sample* f )
      {
         ++n;
         double xc; P::FromSample( xc, *f ); xc -= m_center;
         double y = xc/m_kd;
         if ( pcl::Abs( y ) < 1 )
         {
            double y2 = y*y;
            double y21 = 1 - y2;
            num += xc*xc * y21*y21*y21*y21;
            den += y21 * (1 - 5*y2);
         }
      }
   };

   // -------------------------------------------------------------------------

   class DSmpThread : public RectThreadBase
   {
   public:

      Array<double> values;
      size_type n;

      DSmpThread( const GenericImage& image, const Rect& rect, int ch1, int ch2, int firstRow, int endRow ) :
         RectThreadBase( image, rect, ch1, ch2, firstRow, endRow ), values(), n( 0 )
      {
         size_type N = size_type(  this->m_rect.Width() )
                     * size_type( this->m_endRow - this->m_firstRow )
                     * (1 + this->m_ch2 - this->m_ch1);
         values = Array<double>( N );
      }

   private:

      virtual void Perform( const sample* f )
      {
         P::FromSample( values[n++], *f );
      }
   };

   // -------------------------------------------------------------------------

   class ColorSpaceConversionThread : public Thread
   {
   public:

      ColorSpaceConversionThread( GenericImage& image, ThreadData& data,
                                  color_space toColorSpace, size_type begin, size_type end ) :
         Thread(),
         m_image( image ), m_data( data ), m_toColorSpace( toColorSpace ), m_begin( begin ), m_end( end )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         const RGBColorSystem& rgbws = m_image.RGBWorkingSpace();

         typename P::sample* f0 = m_image[0] + m_begin;
         typename P::sample* f1 = m_image[1] + m_begin;
         typename P::sample* f2 = m_image[2] + m_begin;
         typename P::sample* fN = m_image[0] + m_end;

         for ( ; f0 < fN; ++f0, ++f1, ++f2 )
         {
            RGBColorSystem::sample r0, r1, r2;
            P::FromSample( r0, *f0 );
            P::FromSample( r1, *f1 );
            P::FromSample( r2, *f2 );

            switch ( m_image.ColorSpace() )
            {
            case ColorSpace::RGB :
               switch ( m_toColorSpace )
               {
               case ColorSpace::Gray :
                  rgbws.RGBToGray( r0, r0, r1, r2 );
                  break;
               case ColorSpace::HSV :
                  rgbws.RGBToHSV( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::HSI :
                  rgbws.RGBToHSI( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIEXYZ :
                  rgbws.RGBToCIEXYZ( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIELab :
                  rgbws.RGBToCIELab( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIELch :
                  rgbws.RGBToCIELch( r0, r1, r2, r0, r1, r2 );
                  break;
               default:
                  break;
               }
               break;
            case ColorSpace::HSV :
               rgbws.HSVToRGB( r0, r1, r2, r0, r1, r2 );
               switch ( m_toColorSpace )
               {
               case ColorSpace::Gray :
                  rgbws.RGBToGray( r0, r0, r1, r2 );
                  break;
               case ColorSpace::RGB :
                  break;
               case ColorSpace::HSI :
                  rgbws.RGBToHSI( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIEXYZ :
                  rgbws.RGBToCIEXYZ( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIELab :
                  rgbws.RGBToCIELab( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIELch :
                  rgbws.RGBToCIELch( r0, r1, r2, r0, r1, r2 );
                  break;
               default:
                  break;
               }
               break;
            case ColorSpace::HSI :
               rgbws.HSIToRGB( r0, r1, r2, r0, r1, r2 );
               switch ( m_toColorSpace )
               {
               case ColorSpace::Gray :
                  rgbws.RGBToGray( r0, r0, r1, r2 );
                  break;
               case ColorSpace::RGB :
                  break;
               case ColorSpace::HSV :
                  rgbws.RGBToHSV( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIEXYZ :
                  rgbws.RGBToCIEXYZ( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIELab :
                  rgbws.RGBToCIELab( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIELch :
                  rgbws.RGBToCIELch( r0, r1, r2, r0, r1, r2 );
                  break;
               default:
                  break;
               }
               break;
            case ColorSpace::CIEXYZ :
               switch ( m_toColorSpace )
               {
               case ColorSpace::Gray :
                  rgbws.CIEXYZToRGB( r0, r1, r2, r0, r1, r2 );
                  rgbws.RGBToGray( r0, r0, r1, r2 );
                  break;
               case ColorSpace::RGB :
                  rgbws.CIEXYZToRGB( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::HSV :
                  rgbws.CIEXYZToRGB( r0, r1, r2, r0, r1, r2 );
                  rgbws.RGBToHSV( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::HSI :
                  rgbws.CIEXYZToRGB( r0, r1, r2, r0, r1, r2 );
                  rgbws.RGBToHSI( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIELab :
                  rgbws.CIEXYZToCIELab( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIELch :
                  rgbws.CIEXYZToCIELab( r0, r1, r2, r0, r1, r2 );
                  rgbws.CIELabToCIELch( r0, r1, r2, r0, r1, r2 );
                  break;
               default:
                  break;
               }
               break;
            case ColorSpace::CIELab :
               switch ( m_toColorSpace )
               {
               case ColorSpace::Gray :
                  rgbws.CIELabToRGB( r0, r1, r2, r0, r1, r2 );
                  rgbws.RGBToGray( r0, r0, r1, r2 );
                  break;
               case ColorSpace::RGB :
                  rgbws.CIELabToRGB( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::HSV :
                  rgbws.CIELabToRGB( r0, r1, r2, r0, r1, r2 );
                  rgbws.RGBToHSV( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::HSI :
                  rgbws.CIELabToRGB( r0, r1, r2, r0, r1, r2 );
                  rgbws.RGBToHSI( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIEXYZ :
                  rgbws.CIELabToCIEXYZ( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIELch :
                  rgbws.CIELabToCIELch( r0, r1, r2, r0, r1, r2 );
                  break;
               default:
                  break;
               }
               break;
            case ColorSpace::CIELch :
               switch ( m_toColorSpace )
               {
               case ColorSpace::Gray :
                  rgbws.CIELchToRGB( r0, r1, r2, r0, r1, r2 );
                  rgbws.RGBToGray( r0, r0, r1, r2 );
                  break;
               case ColorSpace::RGB :
                  rgbws.CIELchToRGB( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::HSV :
                  rgbws.CIELchToRGB( r0, r1, r2, r0, r1, r2 );
                  rgbws.RGBToHSV( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::HSI :
                  rgbws.CIELchToRGB( r0, r1, r2, r0, r1, r2 );
                  rgbws.RGBToHSI( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIEXYZ :
                  rgbws.CIELchToCIELab( r0, r1, r2, r0, r1, r2 );
                  rgbws.CIELabToCIEXYZ( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIELab :
                  rgbws.CIELchToCIELab( r0, r1, r2, r0, r1, r2 );
                  break;
               default:
                  break;
               }
               break;
            default:
               break;
            }

            *f0 = P::ToSample( r0 );

            if ( m_toColorSpace != ColorSpace::Gray )
            {
               *f1 = P::ToSample( r1 );
               *f2 = P::ToSample( r2 );
            }

            UPDATE_THREAD_MONITOR( 65536 )
         }
      }

   private:

      GenericImage& m_image;
      ThreadData&   m_data;
      color_space   m_toColorSpace;
      size_type     m_begin, m_end;
   };

   // -------------------------------------------------------------------------

   template <class P1>
   class GetLuminanceThread : public Thread
   {
   public:

      GetLuminanceThread( GenericImage<P1>& luminance, const GenericImage& image, ThreadData& data,
                          const Rect& rect, int firstRow, int endRow ) :
         Thread(),
         m_luminance( luminance ), m_image( image ), m_data( data ),
         m_rect( rect ), m_firstRow( firstRow ), m_endRow( endRow )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         const RGBColorSystem& rgbws = m_image.RGBWorkingSpace();

         const typename P::sample* f0 = m_image.PixelAddress( m_rect.x0, m_rect.y0 + m_firstRow, 0 );
         const typename P::sample* f1 = m_image.PixelAddress( m_rect.x0, m_rect.y0 + m_firstRow, 1 );
         const typename P::sample* f2 = m_image.PixelAddress( m_rect.x0, m_rect.y0 + m_firstRow, 2 );

         typename P1::sample* fY = m_luminance.ScanLine( m_firstRow );

         for ( int y = m_firstRow, dw = m_image.Width()-m_rect.Width();
               y < m_endRow;
               ++y, f0 += dw, f1 += dw, f2 += dw )
         {
            const typename P::sample* fN = f0 + m_rect.Width();

            for ( ; f0 < fN; ++f0, ++f1, ++f2, ++fY )
            {
               RGBColorSystem::sample r0, r1, r2, rY;
               P::FromSample( r0, *f0 );
               P::FromSample( r1, *f1 );
               P::FromSample( r2, *f2 );

               switch ( m_image.ColorSpace() )
               {
               case ColorSpace::RGB:
               case ColorSpace::HSV:
               case ColorSpace::HSI:
                  switch ( m_image.ColorSpace() )
                  {
                  case ColorSpace::HSV:
                     rgbws.HSVToRGB( r0, r1, r2, r0, r1, r2 );
                     break;
                  case ColorSpace::HSI:
                     rgbws.HSIToRGB( r0, r1, r2, r0, r1, r2 );
                     break;
                  default:
                     break;
                  }
                  rY = rgbws.CIEY( r0, r1, r2 );
                  break;

               case ColorSpace::CIELch:
                  rgbws.CIELchToCIELab( r0, r1, r2, r0, r1, r2 );
                  // fall through
               case ColorSpace::CIELab:
                  rgbws.CIELabToCIEXYZ( r0, rY, r2, r0, r1, r2 );
                  break;

               default: // ?!
                  rY = 0;
                  break;
               }

               *fY = P1::ToSample( rY );

               UPDATE_THREAD_MONITOR( 65536 )
            }
         }
      }

   private:

            GenericImage<P1>& m_luminance;
      const GenericImage&     m_image;
            ThreadData&       m_data;
      const Rect&             m_rect;
            int               m_firstRow, m_endRow;
   };

   // -------------------------------------------------------------------------

   template <class P1>
   class GetLightnessThread : public Thread
   {
   public:

      GetLightnessThread( GenericImage<P1>& lightness, const GenericImage& image, ThreadData& data,
                          const Rect& rect, int firstRow, int endRow ) :
         Thread(),
         m_lightness( lightness ), m_image( image ), m_data( data ),
         m_rect( rect ), m_firstRow( firstRow ), m_endRow( endRow )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         const RGBColorSystem& rgbws = m_image.RGBWorkingSpace();

         const typename P::sample* f0 = m_image.PixelAddress( m_rect.x0, m_rect.y0 + m_firstRow, 0 );
         const typename P::sample* f1 = m_image.PixelAddress( m_rect.x0, m_rect.y0 + m_firstRow, 1 );
         const typename P::sample* f2 = m_image.PixelAddress( m_rect.x0, m_rect.y0 + m_firstRow, 2 );

         typename P1::sample* fL = m_lightness.ScanLine( m_firstRow );

         for ( int y = m_firstRow, dw = m_image.Width()-m_rect.Width();
               y < m_endRow;
               ++y, f0 += dw, f1 += dw, f2 += dw )
         {
            const typename P::sample* fN = f0 + m_rect.Width();

            for ( ; f0 < fN; ++f0, ++f1, ++f2, ++fL )
            {
               RGBColorSystem::sample r0, r1, r2, rL;
               P::FromSample( r0, *f0 );
               P::FromSample( r1, *f1 );
               P::FromSample( r2, *f2 );

               switch ( m_image.ColorSpace() )
               {
               case ColorSpace::RGB:
               case ColorSpace::HSV:
               case ColorSpace::HSI:
               case ColorSpace::CIEXYZ:
                  switch ( m_image.ColorSpace() )
                  {
                  case ColorSpace::HSV:
                     rgbws.HSVToRGB( r0, r1, r2, r0, r1, r2 );
                     break;
                  case ColorSpace::HSI:
                     rgbws.HSIToRGB( r0, r1, r2, r0, r1, r2 );
                     break;
                  case ColorSpace::CIEXYZ:
                     rgbws.CIEXYZToRGB( r0, r1, r2, r0, r1, r2 );
                     break;
                  default:
                     break;
                  }
                  rL = rgbws.CIEL( r0, r1, r2 );
                  break;

               default: // ?!
                  rL = 0;
                  break;
               }

               *fL = P1::ToSample( rL );

               UPDATE_THREAD_MONITOR( 65536 )
            }
         }
      }

   private:

            GenericImage<P1>& m_lightness;
      const GenericImage&     m_image;
            ThreadData&       m_data;
      const Rect&             m_rect;
            int               m_firstRow, m_endRow;
   };

   // -------------------------------------------------------------------------

   template <class P1>
   class GetIntensityThread : public Thread
   {
   public:

      GetIntensityThread( GenericImage<P1>& luminance, const GenericImage& image, ThreadData& data,
                          const Rect& rect, int firstRow, int endRow ) :
         Thread(),
         m_intensity( luminance ), m_image( image ), m_data( data ),
         m_rect( rect ), m_firstRow( firstRow ), m_endRow( endRow )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         const RGBColorSystem& rgbws = m_image.RGBWorkingSpace();

         const typename P::sample* f0 = m_image.PixelAddress( m_rect.x0, m_rect.y0 + m_firstRow, 0 );
         const typename P::sample* f1 = m_image.PixelAddress( m_rect.x0, m_rect.y0 + m_firstRow, 1 );
         const typename P::sample* f2 = m_image.PixelAddress( m_rect.x0, m_rect.y0 + m_firstRow, 2 );

         typename P1::sample* fI = m_intensity.ScanLine( m_firstRow );

         for ( int y = m_firstRow, dw = m_image.Width()-m_rect.Width();
               y < m_endRow;
               ++y, f0 += dw, f1 += dw, f2 += dw )
         {
            const typename P::sample* fN = f0 + m_rect.Width();

            for ( ; f0 < fN; ++f0, ++f1, ++f2, ++fI )
            {
               RGBColorSystem::sample r0, r1, r2;
               P::FromSample( r0, *f0 );
               P::FromSample( r1, *f1 );
               P::FromSample( r2, *f2 );

               switch ( m_image.ColorSpace() )
               {
               case ColorSpace::RGB:
                  break;
               case ColorSpace::HSV:
                  rgbws.HSVToRGB( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIEXYZ:
                  rgbws.CIEXYZToRGB( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIELab:
                  rgbws.CIELabToRGB( r0, r1, r2, r0, r1, r2 );
                  break;
               case ColorSpace::CIELch:
                  rgbws.CIELchToRGB( r0, r1, r2, r0, r1, r2 );
                  break;
               default:
                  break;
               }

               *fI = P1::ToSample( rgbws.Intensity( r0, r1, r2 ) );

               UPDATE_THREAD_MONITOR( 65536 )
            }
         }
      }

   private:

            GenericImage<P1>& m_intensity;
      const GenericImage&     m_image;
            ThreadData&       m_data;
      const Rect&             m_rect;
            int               m_firstRow, m_endRow;
   };

   // -------------------------------------------------------------------------

   template <class P1>
   class SetLuminanceThread : public Thread
   {
   public:

      SetLuminanceThread( GenericImage& image, const GenericImage<P1>& luminance, ThreadData& data,
                          const Point& target, const Rect& rect, int firstRow, int endRow ) :
         Thread(),
         m_image( image ), m_luminance( luminance ), m_data( data ),
         m_target( target ), m_rect( rect ), m_firstRow( firstRow ), m_endRow( endRow )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         const RGBColorSystem& sourceRGBWS = m_luminance.RGBWorkingSpace();
         const RGBColorSystem& targetRGBWS = m_image.RGBWorkingSpace();

         typename P::sample* f0 = m_image.PixelAddress( m_target.x, m_target.y + m_firstRow, 0 );
         typename P::sample* f1 = m_image.PixelAddress( m_target.x, m_target.y + m_firstRow, 1 );
         typename P::sample* f2 = m_image.PixelAddress( m_target.x, m_target.y + m_firstRow, 2 );

         if ( m_luminance.ColorSpace() == ColorSpace::Gray || m_luminance.ColorSpace() == ColorSpace::CIEXYZ )
         {
            int cY = (m_luminance.ColorSpace() == ColorSpace::Gray) ? 0 : 1;
            const typename P1::sample* fY = m_luminance.PixelAddress( m_rect.x0, m_rect.y0 + m_firstRow, cY );

            for ( int y = m_firstRow, dw = m_image.Width()-m_rect.Width(), dwY = m_luminance.Width()-m_rect.Width();
                  y < m_endRow;
                  ++y, f0 += dw, f1 += dw, f2 += dw, fY += dwY )
            {
               const typename P::sample* fN = f0 + m_rect.Width();

               for ( ; f0 < fN; ++f0, ++f1, ++f2, ++fY )
               {
                  RGBColorSystem::sample r0, r1, r2, rY;
                  P::FromSample(  r0, *f0 );
                  P::FromSample(  r1, *f1 );
                  P::FromSample(  r2, *f2 );
                  P1::FromSample( rY, *fY );

                  switch ( m_image.ColorSpace() )
                  {
                  case ColorSpace::RGB:
                  case ColorSpace::HSV:
                  case ColorSpace::HSI:
                     switch ( m_image.ColorSpace() )
                     {
                     case ColorSpace::HSV:
                        targetRGBWS.HSVToRGB( r0, r1, r2, r0, r1, r2 );
                        break;
                     case ColorSpace::HSI:
                        targetRGBWS.HSIToRGB( r0, r1, r2, r0, r1, r2 );
                        break;
                     default: // RGB
                        break;
                     }
                     targetRGBWS.RGBToCIEab( r1, r2, r0, r1, r2 );
                     targetRGBWS.CIELabToRGB( r0, r1, r2, sourceRGBWS.CIEYToCIEL( rY ), r1, r2 );
                     switch ( m_image.ColorSpace() )
                     {
                     case ColorSpace::HSV:
                        targetRGBWS.RGBToHSV( r0, r1, r2, r0, r1, r2 );
                        break;
                     case ColorSpace::HSI:
                        targetRGBWS.RGBToHSI( r0, r1, r2, r0, r1, r2 );
                        break;
                     default: // RGB
                        break;
                     }
                     break;

                  case ColorSpace::CIEXYZ:
                     targetRGBWS.CIEXYZToCIELab( r0, r1, r2, r0, r1, r2 );
                     targetRGBWS.CIELabToCIEXYZ( r0, r1, r2, sourceRGBWS.CIEYToCIEL( rY ), r1, r2 );
                     break;

                  case ColorSpace::CIELab:
                  case ColorSpace::CIELch:
                     r0 = sourceRGBWS.CIEYToCIEL( rY );
                     break;

                  default: // ???
                     break;
                  }

                  *f0 = P::ToSample( r0 );
                  *f1 = P::ToSample( r1 );
                  *f2 = P::ToSample( r2 );

                  UPDATE_THREAD_MONITOR( 65536 )
               }
            }
         }
         else
         {
            const typename P1::sample* g0 = m_luminance.PixelAddress( m_rect.x0, m_rect.y0 + m_firstRow, 0 );
            const typename P1::sample* g1 = m_luminance.PixelAddress( m_rect.x0, m_rect.y0 + m_firstRow, 1 );
            const typename P1::sample* g2 = m_luminance.PixelAddress( m_rect.x0, m_rect.y0 + m_firstRow, 2 );

            for ( int y = m_firstRow, dw = m_image.Width()-m_rect.Width(), dwY = m_luminance.Width()-m_rect.Width();
                  y < m_endRow;
                  ++y, f0 += dw, f1 += dw, f2 += dw, g0 += dwY, g1 += dwY, g2 += dwY )
            {
               const typename P::sample* fN = f0 + m_rect.Width();

               for ( ; f0 < fN; ++f0, ++f1, ++f2, ++g0, ++g1, ++g2 )
               {
                  typename RGBColorSystem::sample r0, r1, r2, s0, s1, s2;
                  P::FromSample(  r0, *f0 );
                  P::FromSample(  r1, *f1 );
                  P::FromSample(  r2, *f2 );
                  P1::FromSample( s0, *g0 );
                  P1::FromSample( s1, *g1 );
                  P1::FromSample( s2, *g2 );

                  switch ( m_image.ColorSpace() )
                  {
                  case ColorSpace::RGB:
                  case ColorSpace::HSV:
                  case ColorSpace::HSI:
                  case ColorSpace::CIEXYZ:
                  case ColorSpace::CIELab:
                  case ColorSpace::CIELch:
                     switch ( m_image.ColorSpace() )
                     {
                     case ColorSpace::RGB:
                     case ColorSpace::HSV:
                     case ColorSpace::HSI:
                        switch ( m_image.ColorSpace() )
                        {
                        case ColorSpace::HSV:
                           targetRGBWS.HSVToRGB( r0, r1, r2, r0, r1, r2 );
                           break;
                        case ColorSpace::HSI:
                           targetRGBWS.HSIToRGB( r0, r1, r2, r0, r1, r2 );
                           break;
                        default:
                           break;
                        }
                        targetRGBWS.RGBToCIEab( r1, r2, r0, r1, r2 );
                        break;
                     case ColorSpace::CIEXYZ:
                        targetRGBWS.CIEXYZToCIELab( r0, r1, r2, r0, r1, r2 );
                        break;
                     case ColorSpace::CIELab: // NOOP
                     case ColorSpace::CIELch: // NOOP
                        break;
                     default: // ?!
                        break;
                     }

                     switch ( m_luminance.ColorSpace() )
                     {
                     case ColorSpace::RGB:
                     case ColorSpace::HSV:
                     case ColorSpace::HSI:
                        switch ( m_luminance.ColorSpace() )
                        {
                        case ColorSpace::HSV:
                           sourceRGBWS.HSVToRGB( s0, s1, s2, s0, s1, s2 );
                           break;
                        case ColorSpace::HSI:
                           sourceRGBWS.HSIToRGB( s0, s1, s2, s0, s1, s2 );
                           break;
                        default:
                           break;
                        }
                        r0 = sourceRGBWS.CIEL( s0, s1, s2 );
                        break;
                     case ColorSpace::CIEXYZ:
                        sourceRGBWS.CIEXYZToCIELab( r0, s1, s2, s0, s1, s2 );
                        break;
                     case ColorSpace::CIELab:
                     case ColorSpace::CIELch:
                        r0 = s0;
                        break;
                     default: // ?!
                        break;
                     }

                     switch ( m_image.ColorSpace() )
                     {
                     case ColorSpace::RGB:
                     case ColorSpace::HSV:
                     case ColorSpace::HSI:
                        targetRGBWS.CIELabToRGB( r0, r1, r2, r0, r1, r2 );
                        switch ( m_image.ColorSpace() )
                        {
                        case ColorSpace::HSV:
                           targetRGBWS.RGBToHSV( r0, r1, r2, r0, r1, r2 );
                           break;
                        case ColorSpace::HSI:
                           targetRGBWS.RGBToHSI( r0, r1, r2, r0, r1, r2 );
                           break;
                        default: // RGB
                           break;
                        }
                        break;
                     case ColorSpace::CIEXYZ:
                        targetRGBWS.CIELabToCIEXYZ( r0, r1, r2, r0, r1, r2 );
                        break;
                     case ColorSpace::CIELab: // NOOP
                     case ColorSpace::CIELch: // NOOP
                        break;
                     default: // ?!
                        break;
                     }
                     break;

                  default: // ?!
                     break;
                  }

                  *f0 = P::ToSample( r0 );
                  *f1 = P::ToSample( r1 );
                  *f2 = P::ToSample( r2 );

                  UPDATE_THREAD_MONITOR( 65536 )
               }
            }
         }
      }

   private:

            GenericImage&     m_image;
      const GenericImage<P1>& m_luminance;
            ThreadData&       m_data;
      const Point&            m_target;
      const Rect&             m_rect;
            int               m_firstRow, m_endRow;
   };

   // -------------------------------------------------------------------------

   template <class P1>
   class SetLightnessThread : public Thread
   {
   public:

      SetLightnessThread( GenericImage& image, const GenericImage<P1>& lightness, ThreadData& data,
                          const Point& target, const Rect& rect, int firstRow, int endRow ) :
         Thread(),
         m_image( image ), m_lightness( lightness ), m_data( data ),
         m_target( target ), m_rect( rect ), m_firstRow( firstRow ), m_endRow( endRow )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         const RGBColorSystem& sourceRGBWS = m_lightness.RGBWorkingSpace();
         const RGBColorSystem& targetRGBWS = m_image.RGBWorkingSpace();

         typename P::sample* f0 = m_image.PixelAddress( m_target.x, m_target.y + m_firstRow, 0 );
         typename P::sample* f1 = m_image.PixelAddress( m_target.x, m_target.y + m_firstRow, 1 );
         typename P::sample* f2 = m_image.PixelAddress( m_target.x, m_target.y + m_firstRow, 2 );

         if ( m_lightness.ColorSpace() == ColorSpace::Gray ||
              m_lightness.ColorSpace() == ColorSpace::CIELab || m_lightness.ColorSpace() == ColorSpace::CIELch )
         {
            const typename P1::sample* fL = m_lightness.PixelAddress( m_rect.x0, m_rect.y0 + m_firstRow, 0 );

            for ( int y = m_firstRow, dw = m_image.Width()-m_rect.Width(), dwL = m_lightness.Width()-m_rect.Width();
                  y < m_endRow;
                  ++y, f0 += dw, f1 += dw, f2 += dw, fL += dwL )
            {
               const typename P::sample* fN = f0 + m_rect.Width();

               for ( ; f0 < fN; ++f0, ++f1, ++f2, ++fL )
               {
                  RGBColorSystem::sample r0, r1, r2, rL;
                  P::FromSample(  r0, *f0 );
                  P::FromSample(  r1, *f1 );
                  P::FromSample(  r2, *f2 );
                  P1::FromSample( rL, *fL );

                  switch ( m_image.ColorSpace() )
                  {
                  case ColorSpace::RGB:
                  case ColorSpace::HSV:
                  case ColorSpace::HSI:
                     switch ( m_image.ColorSpace() )
                     {
                     case ColorSpace::HSV:
                        targetRGBWS.HSVToRGB( r0, r1, r2, r0, r1, r2 );
                        break;
                     case ColorSpace::HSI:
                        targetRGBWS.HSIToRGB( r0, r1, r2, r0, r1, r2 );
                        break;
                     default:
                        break;
                     }
                     targetRGBWS.RGBToCIEab( r1, r2, r0, r1, r2 );
                     targetRGBWS.CIELabToRGB( r0, r1, r2, rL, r1, r2 );
                     switch ( m_image.ColorSpace() )
                     {
                     case ColorSpace::HSV:
                        targetRGBWS.RGBToHSV( r0, r1, r2, r0, r1, r2 );
                        break;
                     case ColorSpace::HSI:
                        targetRGBWS.RGBToHSI( r0, r1, r2, r0, r1, r2 );
                        break;
                     default:
                        break;
                     }
                     break;

                  case ColorSpace::CIEXYZ:
                     r1 = targetRGBWS.CIELToCIEY( rL );
                     break;

                  case ColorSpace::CIELab:
                  case ColorSpace::CIELch:
                     r0 = rL;
                     break;

                  default: // ???
                     break;
                  }

                  *f0 = P::ToSample( r0 );
                  *f1 = P::ToSample( r1 );
                  *f2 = P::ToSample( r2 );

                  UPDATE_THREAD_MONITOR( 65536 )
               }
            }
         }
         else
         {
            const typename P1::sample* g0 = m_lightness.PixelAddress( m_rect.x0, m_rect.y0 + m_firstRow, 0 );
            const typename P1::sample* g1 = m_lightness.PixelAddress( m_rect.x0, m_rect.y0 + m_firstRow, 1 );
            const typename P1::sample* g2 = m_lightness.PixelAddress( m_rect.x0, m_rect.y0 + m_firstRow, 2 );

            for ( int y = m_firstRow, dw = m_image.Width()-m_rect.Width(), dwL = m_lightness.Width()-m_rect.Width();
                  y < m_endRow;
                  ++y, f0 += dw, f1 += dw, f2 += dw, g0 += dwL, g1 += dwL, g2 += dwL )
            {
               const typename P::sample* fN = f0 + m_rect.Width();

               for ( ; f0 < fN; ++f0, ++f1, ++f2, ++g0, ++g1, ++g2 )
               {
                  typename RGBColorSystem::sample r0, r1, r2, s0, s1, s2;
                  P::FromSample(  r0, *f0 );
                  P::FromSample(  r1, *f1 );
                  P::FromSample(  r2, *f2 );
                  P1::FromSample( s0, *g0 );
                  P1::FromSample( s1, *g1 );
                  P1::FromSample( s2, *g2 );

                  switch ( m_image.ColorSpace() )
                  {
                  case ColorSpace::RGB:
                  case ColorSpace::HSV:
                  case ColorSpace::HSI:
                  case ColorSpace::CIEXYZ:
                  case ColorSpace::CIELab:
                  case ColorSpace::CIELch:
                     switch ( m_image.ColorSpace() )
                     {
                     case ColorSpace::RGB:
                     case ColorSpace::HSV:
                     case ColorSpace::HSI:
                        switch ( m_image.ColorSpace() )
                        {
                        case ColorSpace::HSV:
                           targetRGBWS.HSVToRGB( r0, r1, r2, r0, r1, r2 );
                           break;
                        case ColorSpace::HSI:
                           targetRGBWS.HSIToRGB( r0, r1, r2, r0, r1, r2 );
                           break;
                        default:
                           break;
                        }
                        targetRGBWS.RGBToCIEab( r1, r2, r0, r1, r2 );
                        break;
                     case ColorSpace::CIEXYZ:
                        targetRGBWS.CIEXYZToCIELab( r0, r1, r2, r0, r1, r2 );
                        break;
                     case ColorSpace::CIELch:
                        targetRGBWS.CIELchToCIELab( r0, r1, r2, r0, r1, r2 );
                        break;
                     default:
                        break;
                     }

                     switch ( m_lightness.ColorSpace() )
                     {
                     case ColorSpace::RGB:
                     case ColorSpace::HSV:
                     case ColorSpace::HSI:
                        switch ( m_lightness.ColorSpace() )
                        {
                        case ColorSpace::HSV:
                           sourceRGBWS.HSVToRGB( s0, s1, s2, s0, s1, s2 );
                           break;
                        case ColorSpace::HSI:
                           sourceRGBWS.HSIToRGB( s0, s1, s2, s0, s1, s2 );
                           break;
                        default:
                           break;
                        }
                        r0 = sourceRGBWS.CIEL( s0, s1, s2 );
                        break;
                     case ColorSpace::CIEXYZ:
                        r0 = sourceRGBWS.CIEYToCIEL( s1 );
                        break;
                     default: // ???
                        break;
                     }

                     switch ( m_image.ColorSpace() )
                     {
                     case ColorSpace::RGB:
                     case ColorSpace::HSV:
                     case ColorSpace::HSI:
                        targetRGBWS.CIELabToRGB( r0, r1, r2, r0, r1, r2 );
                        switch ( m_image.ColorSpace() )
                        {
                        case ColorSpace::HSV:
                           targetRGBWS.RGBToHSV( r0, r1, r2, r0, r1, r2 );
                           break;
                        case ColorSpace::HSI:
                           targetRGBWS.RGBToHSI( r0, r1, r2, r0, r1, r2 );
                           break;
                        default:
                           break;
                        }
                        break;
                     case ColorSpace::CIEXYZ:
                        targetRGBWS.CIELabToCIEXYZ( r0, r1, r2, r0, r1, r2 );
                        break;
                     case ColorSpace::CIELch:
                        targetRGBWS.CIELabToCIELch( r0, r1, r2, r0, r1, r2 );
                        break;
                     default:
                        break;
                     }

                  default: // ???
                     break;
                  }

                  *f0 = P::ToSample( r0 );
                  *f1 = P::ToSample( r1 );
                  *f2 = P::ToSample( r2 );

                  UPDATE_THREAD_MONITOR( 65536 )
               }
            }
         }
      }

   private:

            GenericImage&     m_image;
      const GenericImage<P1>& m_lightness;
            ThreadData&       m_data;
      const Point&            m_target;
      const Rect&             m_rect;
            int               m_firstRow, m_endRow;
   };

   // -------------------------------------------------------------------------

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
   friend class pi::SharedImage;
#endif
};

#undef m_pixelData
#undef m_allocator
#undef m_width
#undef m_height
#undef m_numberOfChannels
#undef m_colorSpace
#undef m_RGBWS
#undef m_channel
#undef m_lastChannel
#undef m_point
#undef m_rectangle
#undef m_clipLow
#undef m_clipHigh
#undef m_clipped

// ----------------------------------------------------------------------------

/*!
 * \defgroup image_scalar_operators Image Scalar Operators
 */

/*!
 * Returns the sum of an \a image and a \a scalar. The operation is performed
 * on the current selection of the specified \a image.
 * \ingroup image_scalar_operators
 */
template <class P, typename T> inline
GenericImage<P> operator +( const GenericImage<P>& image, T scalar )
{
   GenericImage<P> result( image );
   (void)(result += scalar);
   return result;
}

/*!
 * Returns the sum of a \a scalar and an \a image. The operation is performed
 * on the current selection of the specified \a image.
 *
 * This operator implements the commutative property of image scalar addition.
 *
 * \ingroup image_scalar_operators
 */
template <class P, typename T> inline
GenericImage<P> operator +( T scalar, const GenericImage<P>& image )
{
   return image + scalar;
}

/*!
 * Returns the subtraction of a \a scalar from an \a image. The operation is
 * performed on the current selection of the specified \a image.
 * \ingroup image_scalar_operators
 */
template <class P, typename T> inline
GenericImage<P> operator -( const GenericImage<P>& image, T scalar )
{
   GenericImage<P> result( image );
   (void)(result -= scalar);
   return result;
}

/*!
 * Returns the product of an \a image by a \a scalar. The operation is
 * performed on the current selection of the specified \a image.
 * \ingroup image_scalar_operators
 */
template <class P, typename T> inline
GenericImage<P> operator *( const GenericImage<P>& image, T scalar )
{
   GenericImage<P> result( image );
   (void)(result *= scalar);
   return result;
}

/*!
 * Returns the product of a \a scalar by an \a image. The operation is
 * performed on the current selection of the specified \a image.
 *
 * This operator implements the commutative property of image scalar
 * multiplication.
 *
 * \ingroup image_scalar_operators
 */
template <class P, typename T> inline
GenericImage<P> operator *( T scalar, const GenericImage<P>& image )
{
   return image * scalar;
}

/*!
 * Returns the result of dividing an \a image by a \a scalar. The operation is
 * performed on the current selection of the specified \a image.
 * \ingroup image_scalar_operators
 */
template <class P, typename T> inline
GenericImage<P> operator /( const GenericImage<P>& image, T scalar )
{
   GenericImage<P> result( image );
   (void)(result /= scalar);
   return result;
}

/*!
 * Returns the result of raising an \a image to a \a scalar. The operation is
 * performed on the current selection of the specified \a image.
 * \ingroup image_scalar_operators
 */
template <class P, typename T> inline
GenericImage<P> operator ^( const GenericImage<P>& image, T scalar )
{
   GenericImage<P> result( image );
   (void)(result ^= scalar);
   return result;
}

// ----------------------------------------------------------------------------

/*!
 * \defgroup image_operators Image Operators
 */

/*!
 * Returns the sum of two images.
 *
 * The image addition operation is performed for the current selection of the
 * right-hand operand image, and is applied at the current anchor point of the
 * left-hand image.
 *
 * The result is an image of the same template instantiation as the left-hand
 * operand image.
 *
 * \ingroup image_operators
 */
template <class P1, class P2> inline
GenericImage<P1> operator +( const GenericImage<P1>& image1, const GenericImage<P2>& image2 )
{
   GenericImage<P1> result( image1 );
   (void)(result += image2);
   return result;
}

/*!
 * Returns the difference of two images.
 *
 * The image subtraction operation is performed for the current selection of
 * the right-hand operand image, and is applied at the current anchor point of
 * the left-hand image.
 *
 * The result is an image of the same template instantiation as the left-hand
 * operand image.
 *
 * \ingroup image_operators
 */
template <class P1, class P2> inline
GenericImage<P1> operator -( const GenericImage<P1>& image1, const GenericImage<P2>& image2 )
{
   GenericImage<P1> result( image1 );
   (void)(result -= image2);
   return result;
}

/*!
 * Returns the product of two images.
 *
 * The image multiplication operation is performed for the current selection of
 * the right-hand operand image, and is applied at the current anchor point of
 * the left-hand image.
 *
 * The result is an image of the same template instantiation as the left-hand
 * operand image.
 *
 * \ingroup image_operators
 */
template <class P1, class P2> inline
GenericImage<P1> operator *( const GenericImage<P1>& image1, const GenericImage<P2>& image2 )
{
   GenericImage<P1> result( image1 );
   (void)(result *= image2);
   return result;
}

/*!
 * Returns the result of dividing one image \a image1 by another image
 * \a image2.
 *
 * The image division operation is performed for the current selection of the
 * right-hand operand image, and is applied at the current anchor point of the
 * left-hand image.
 *
 * The result is an image of the same template instantiation as the left-hand
 * operand image.
 *
 * \ingroup image_operators
 */
template <class P1, class P2> inline
GenericImage<P1> operator /( const GenericImage<P1>& image1, const GenericImage<P2>& image2 )
{
   GenericImage<P1> result( image1 );
   (void)(result /= image2);
   return result;
}

/*!
 * Returns the result of raising one image \a image1 to another image
 * \a image2.
 *
 * The image exponentiation operation is performed for the current selection of
 * the right-hand operand image, and is applied at the current anchor point of
 * the left-hand image.
 *
 * The result is an image of the same template instantiation as the left-hand
 * operand image.
 *
 * \ingroup image_operators
 */
template <class P1, class P2> inline
GenericImage<P1> operator ^( const GenericImage<P1>& image1, const GenericImage<P2>& image2 )
{
   GenericImage<P1> result( image1 );
   (void)(result ^= image2);
   return result;
}

// ----------------------------------------------------------------------------

#ifndef __PCL_NO_IMAGE_INSTANTIATE

/*!
 * \defgroup image_types_2d Image Types
 */

/*!
 * \class pcl::FImage
 * \ingroup image_types_2d
 * \brief 32-bit floating point real image.
 *
 * %FImage is a template instantiation of GenericImage for FloatPixelTraits.
 */
typedef GenericImage<FloatPixelTraits>    FImage;

/*!
 * \class pcl::DImage
 * \ingroup image_types_2d
 * \brief 64-bit floating point real image.
 *
 * %DImage is a template instantiation of GenericImage for DoublePixelTraits.
 */
typedef GenericImage<DoublePixelTraits>   DImage;

/*!
 * \class pcl::FComplexImage
 * \ingroup image_types_2d
 * \brief 32-bit floating point complex image.
 *
 * %FComplexImage is a template instantiation of GenericImage for the
 * ComplexPixelTraits class.
 */
typedef GenericImage<ComplexPixelTraits>  FComplexImage;

/*!
 * \class pcl::DComplexImage
 * \ingroup image_types_2d
 * \brief 64-bit floating point complex image.
 *
 * %DComplexImage is a template instantiation of GenericImage for the
 * DComplexPixelTraits class.
 */
typedef GenericImage<DComplexPixelTraits> DComplexImage;

/*!
 * \class pcl::UInt8Image
 * \ingroup image_types_2d
 * \brief 8-bit unsigned integer image.
 *
 * %UInt8Image is a template instantiation of GenericImage for the
 * UInt8PixelTraits class.
 */
typedef GenericImage<UInt8PixelTraits>    UInt8Image;

/*!
 * \class pcl::UInt16Image
 * \ingroup image_types_2d
 * \brief 16-bit unsigned integer image.
 *
 * %UInt16Image is a template instantiation of GenericImage for the
 * UInt16PixelTraits class.
 */
typedef GenericImage<UInt16PixelTraits>   UInt16Image;

/*!
 * \class pcl::UInt32Image
 * \ingroup image_types_2d
 * \brief 32-bit unsigned integer image.
 *
 * %UInt32Image is a template instantiation of GenericImage for the
 * UInt32PixelTraits class.
 */
typedef GenericImage<UInt32PixelTraits>   UInt32Image;

/*!
 * \class pcl::Image
 * \ingroup image_types_2d
 * \brief 32-bit floating point real image.
 *
 * %Image is an alias for FImage. It is a template instantiation of
 * GenericImage for the FloatPixelTraits class.
 */
typedef FImage                            Image;

/*!
 * \class pcl::ComplexImage
 * \ingroup image_types_2d
 * \brief 32-bit floating point complex image.
 *
 * %ComplexImage is an alias for FComplexImage. It is a template instantiation
 * of GenericImage for the ComplexPixelTraits class.
 */
typedef FComplexImage                     ComplexImage;

#endif   // __PCL_NO_IMAGE_INSTANTIATE

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------

#ifndef __PCL_NO_IMAGE_VARIANT_AUTO

#ifndef __PCL_ImageVariant_h
#include <pcl/ImageVariant.h>
#endif

#endif   // __PCL_NO_IMAGE_VARIANT_AUTO

// ----------------------------------------------------------------------------

#endif   // __PCL_Image_h

// ----------------------------------------------------------------------------
// EOF pcl/Image.h - Released 2017-06-09T08:12:42Z
