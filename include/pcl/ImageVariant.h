//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/ImageVariant.h - Released 2017-04-14T23:04:40Z
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

#ifndef __PCL_ImageVariant_h
#define __PCL_ImageVariant_h

/// \file pcl/ImageVariant.h

#include <pcl/Defs.h>

#include <pcl/Image.h>
#include <pcl/ReferenceCounter.h>

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
namespace pi
{
   class SharedImage;
}
#endif

#define SOLVE_TEMPLATE( F )                  \
   if ( IsComplexSample() )                  \
      switch ( BitsPerSample() )             \
      {                                      \
      case 32: F( ComplexImage ); break;     \
      case 64: F( DComplexImage ); break;    \
      }                                      \
   else if ( IsFloatSample() )               \
      switch ( BitsPerSample() )             \
      {                                      \
      case 32: F( Image ); break;            \
      case 64: F( DImage ); break;           \
      }                                      \
   else                                      \
      switch ( BitsPerSample() )             \
      {                                      \
      case  8: F( UInt8Image ); break;       \
      case 16: F( UInt16Image ); break;      \
      case 32: F( UInt32Image ); break;      \
      }

#define SOLVE_TEMPLATE_2( I, F )             \
   if ( I.IsComplexSample() )                \
      switch ( I.BitsPerSample() )           \
      {                                      \
      case 32: F( ComplexImage ); break;     \
      case 64: F( DComplexImage ); break;    \
      }                                      \
   else if ( I.IsFloatSample() )             \
      switch ( I.BitsPerSample() )           \
      {                                      \
      case 32: F( Image ); break;            \
      case 64: F( DImage ); break;           \
      }                                      \
   else                                      \
      switch ( I.BitsPerSample() )           \
      {                                      \
      case  8: F( UInt8Image ); break;       \
      case 16: F( UInt16Image ); break;      \
      case 32: F( UInt32Image ); break;      \
      }

#define SOLVE_TEMPLATE_REAL( F )             \
   if ( IsFloatSample() )                    \
      switch ( BitsPerSample() )             \
      {                                      \
      case 32: F( Image ); break;            \
      case 64: F( DImage ); break;           \
      }                                      \
   else                                      \
      switch ( BitsPerSample() )             \
      {                                      \
      case  8: F( UInt8Image ); break;       \
      case 16: F( UInt16Image ); break;      \
      case 32: F( UInt32Image ); break;      \
      }

#define SOLVE_TEMPLATE_REAL_2( I, F )        \
   if ( I.IsFloatSample() )                  \
      switch ( I.BitsPerSample() )           \
      {                                      \
      case 32: F( Image ); break;            \
      case 64: F( DImage ); break;           \
      }                                      \
   else                                      \
      switch ( I.BitsPerSample() )           \
      {                                      \
      case  8: F( UInt8Image ); break;       \
      case 16: F( UInt16Image ); break;      \
      case 32: F( UInt32Image ); break;      \
      }

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::SwapCompression
 * \brief     Compression algorithms for raw image file generation.
 *
 * This namespace enumerates the compression algorithms supported by the
 * ImageVariant::WriteSwapFile() and ImageVariant::WriteSwapFiles() functions.
 *
 * Supported compression algorithms:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>SwapCompression::None</td>     <td>Do not use compression.</td></tr>
 * <tr><td>SwapCompression::ZLib</td>     <td>ZLib (deflate) compression.</td></tr>
 * <tr><td>SwapCompression::ZLib_SH</td>  <td>ZLib (deflate) compression with byte shuffling.</td></tr>
 * <tr><td>SwapCompression::LZ4</td>      <td>LZ4 fast compression.</td></tr>
 * <tr><td>SwapCompression::LZ4_Sh</td>   <td>LZ4 fast compression with byte shuffling.</td></tr>
 * <tr><td>SwapCompression::LZ4HC</td>    <td>LZ4-HC compression.</td></tr>
 * <tr><td>SwapCompression::LZ4HC_Sh</td> <td>LZ4-HC compression with byte shuffling.</td></tr>
 * </table>
 */
namespace SwapCompression
{
   enum value_type
   {
      None,     // Do not use compression.
      ZLib,     // ZLib (deflate) compression.
      ZLib_SH,  // ZLib (deflate) compression with byte shuffling.
      LZ4,      // LZ4 fast compression.
      LZ4_Sh,   // LZ4 fast compression with byte shuffling.
      LZ4HC,    // LZ4-HC compression.
      LZ4HC_Sh, // LZ4-HC compression with byte shuffling.

      NumberOfAlgorithms
   };
}

/*!
 * \class ImageVariant
 * \brief Acts like a union for all types of images in PCL, with optional
 *        class-wide ownership of transported images.
 *
 * An instance of %ImageVariant owns a pointer to AbstractImage. That allows it
 * to transport and manage all template instantiations of GenericImage
 * transparently with a unique abstract interface: Image, ComplexImage,
 * UInt16Image, and so on.
 *
 *
 * <b>Image Template Instantiation Resolution</b>
 *
 * %ImageVariant provides functionality to identify the particular data type
 * that the transported image uses to represent its pixel samples. Once we know
 * the pixel data type, we can resolve the template instantiation of
 * GenericImage, which in turn gives us access to the whole data and
 * functionality of the transported image. The following code snippet shows a
 * typical example of template resolution with %ImageVariant:
 *
 * \code
 * ImageVariant image;
 * // ...
 * if ( image ) // if the ImageVariant transports a valid image
 * {
 *    if ( image.IsComplexImage() )
 *       throw Error( "Complex images are not supported in DoSomething()" );
 *    if ( image.IsFloatSample() )
 *       switch ( image.BitsPerSample() )
 *       {
 *       case 32 : DoSomething( static_cast<Image&>( *image ) ); break;
 *       case 64 : DoSomething( static_cast<DImage&>( *image ) ); break;
 *       }
 *    else
 *       switch ( image.BitsPerSample() )
 *       {
 *       case  8 : DoSomething( static_cast<UInt8Image&>( *image ) ); break;
 *       case 16 : DoSomething( static_cast<UInt16Image&>( *image ) ); break;
 *       case 32 : DoSomething( static_cast<UInt32Image&>( *image ) ); break;
 *       }
 * }
 * \endcode
 *
 * In this code, \c DoSomething is a template function to deal with an image of
 * a particular pixel sample type. In the code above, all standard PCL images
 * are supported except complex images. For example, \c DoSomething could be
 * like this function:
 *
 * \code
 * template <class P> void DoSomething( GenericImage<P>& image )
 * {
 *    image.ShiftToCenter( image.Width()+20, image.Height()+20 );
 * }
 * \endcode
 *
 * This \c DoSomething function adds a 10-pixel black frame around the passed
 * \a image.
 *
 *
 * <b>Generalized Image Manipulation</b>
 *
 * Besides identifying image template instantiations, %ImageVariant provides
 * generalized wrappers for most of the member functions of GenericImage. This
 * allows you to work with an instance of %ImageVariant just as if it were its
 * transported image, without even knowing its pixel data type in most cases.
 * The example we have seen above could be implemented in a much simpler way as
 * follows:
 *
 * \code
 * void DoSomething( ImageVariant& image )
 * {
 *    if ( image )
 *    {
 *       if ( image.IsComplexImage() )
 *          throw Error( "Complex images are not supported in DoSomething()" );
 *       image.ShiftToCenter( image.Width()+20, image.Height()+20 );
 *    }
 * }
 * \endcode
 *
 * The above code works because %ImageVariant defines a \c ShiftToCenter member
 * function as a wrapper to GenericImage::ShiftToCenter(), with implicit
 * template instantiation resolution. As we have said, virtually all of
 * GenericImage's functionality can be used transparently in a type-independent
 * way through %ImageVariant.
 *
 *
 * <b>Image Ownership</b>
 *
 * Transported images can optionally be owned by %ImageVariant. When
 * %ImageVariant owns a transported image, the image is destroyed when all
 * instances of %ImageVariant referencing that image are destroyed. It is
 * important to know that ownership of transported images is a <em>class-wide
 * property</em> of %ImageVariant, not a private property of any particular
 * %ImageVariant instance.
 *
 * Class-wide ownership means that an image is not owned only by a particular
 * instance of %ImageVariant, but by \e all existing instances that transport
 * the same image. Consider the following code snippets:
 *
 * \code
 * Image image;
 * ImageVariant var( &image ); // ImageVariant does not own image
 * \endcode
 *
 * In this first case, we have an image and create an %ImageVariant object to
 * transport it. When the \c var object gets out of scope, \c image will
 * continue existing and won't be destroyed.
 *
 * \code
 * ImageVariant var;
 * var.CreateFloatImage(); // ImageVariant owns a newly created image
 * \endcode
 *
 * In this case, we have an %ImageVariant object and tell it to create a new
 * image (a 32-bit floating point image in the example). The newly created
 * image will be transported by \c var and is now owned by %ImageVariant. when
 * the last instance of %ImageVariant transporting the image is destroyed, the
 * image will also be destroyed automatically.
 *
 * You can also switch on and off %ImageVariant's ownership:
 *
 * \code
 * Image* image = new Image;
 * ImageVariant var( image );
 * var.SetOwnership( true ); // now ImageVariant owns image
 * \endcode
 *
 * %ImageVariant implements an \e explicit data sharing mechanism, which makes
 * it different from the \e implicit data sharing implemented by rest of
 * container classes in PCL. It is important to state clearly that ownership of
 * image data is not a property of any particular object in this case, but of
 * the whole %ImageVariant class.
 *
 * \sa AbstractImage, GenericImage
 */
class PCL_CLASS ImageVariant
{
public:

   /*!
    * An enumerated type that represents the set of supported color spaces.
    * Valid constants for this enumeration are defined in the ColorSpace
    * namespace.
    */
   typedef AbstractImage::color_space     color_space;

   /*!
    * An enumerated type that represents the set of supported arithmetic and
    * bitwise pixel operations. Valid constants for this enumeration are
    * defined in the ImageOp namespace.
    */
   typedef ImageOp::value_type            image_op;

   /*
    * An enumerated type that represents a compression algorithm for raw
    * storage file generation. Valid constants for this enumeration are defined
    * in the SwapCompression namespace.
    */
   typedef SwapCompression::value_type    swap_compression;

   /*!
    * Constructs an empty %ImageVariant. An empty %ImageVariant instance does
    * not transport an image.
    */
   ImageVariant()
   {
      m_data = new Data;
   }

   /*!
    * Constructs an %ImageVariant instance to transport the specified \a image.
    *
    * By default, the transported image is not owned by %ImageVariant object.
    * To cause %ImageVariant to own the transported image, the SetOwnership()
    * member function must be called explicitly.
    */
   template <class P>
   ImageVariant( GenericImage<P>* image )
   {
      m_data = new Data;
      m_data->Update( image );
   }

   /*!
    * Copy constructor. Constructs an %ImageVariant instance as an alias of an
    * existing %ImageVariant object.
    *
    * This constructor simply increments the reference counter of the
    * transported image. When all references to a transported image are
    * removed, and the transported image is owned by %ImageVariant, it is
    * destroyed and deallocated.
    */
   ImageVariant( const ImageVariant& image ) : m_data( image.m_data )
   {
      m_data->Attach();
   }

   /*!
    * Move constructor.
    */
   ImageVariant( ImageVariant&& image ) : m_data( image.m_data )
   {
      image.m_data = nullptr;
   }

   /*!
    * Destroys an %ImageVariant instance.
    *
    * If the transported image exists ans is owned by %ImageVariant, and there
    * are no more %ImageVariant references to it, then it is also destroyed.
    */
   virtual ~ImageVariant()
   {
      if ( m_data != nullptr )
      {
         DetachFromData();
         m_data = nullptr;
      }
   }

   /*!
    * Returns a pointer to the unmodifiable image transported by this
    * %ImageVariant object.
    */
   const AbstractImage* ImagePtr() const
   {
      return m_data->image;
   }

   /*!
    * Returns a pointer to the image transported by this %ImageVariant object.
    */
   AbstractImage* ImagePtr()
   {
      return m_data->image;
   }

   /*!
    * Returns true iff this %ImageVariant transports the same image as another
    * \a image.
    *
    * This member function also returns true if \e both objects transport no
    * image.
    */
   bool IsSameImage( const ImageVariant& image ) const
   {
      return m_data->image == image.m_data->image;
   }

   /*!
    * Returns true iff this %ImageVariant instance transports a floating point
    * image.
    */
   bool IsFloatSample() const
   {
      return m_data->isFloatSample;
   }

   /*!
    * Returns true iff this %ImageVariant instance transports a complex image.
    */
   bool IsComplexSample() const
   {
      return m_data->isComplexSample;
   }

   /*!
    * Returns the number of bits per sample of the image transported by this
    * %ImageVariant object, or zero if it transports no image.
    */
   int BitsPerSample() const
   {
      return m_data->bitsPerSample;
   }

   /*!
    * Returns the number of 8-bit bytes per sample of the image transported by
    * this %ImageVariant, or zero if this object transports no image.
    */
   int BytesPerSample() const
   {
      return m_data->bitsPerSample >> 3;
   }

   /*!
    * Returns the width in pixels of the image transported by this
    * %ImageVariant, or zero if this object transports no image.
    */
   int Width() const
   {
      return m_data->image ? m_data->image->Width() : 0;
   }

   /*!
    * Returns the height in pixels of the image transported by this
    * %ImageVariant, or zero if this object transports no image.
    */
   int Height() const
   {
      return m_data->image ? m_data->image->Height() : 0;
   }

   /*!
    * Returns the number of pixels in the image transported by this
    * %ImageVariant, or zero if this object transports no image.
    *
    * The returned value is equal to Width()*Height().
    */
   size_type NumberOfPixels() const
   {
      return m_data->image ? m_data->image->NumberOfPixels() : 0;
   }

   /*!
    * Returns the total number of channels in the image transported by this
    * %ImageVariant, or zero if this object transports no image.
    */
   int NumberOfChannels() const
   {
      return m_data->image ? m_data->image->NumberOfChannels() : 0;
   }

   /*!
    * Returns the index of the last existing channel in this image. This is the
    * largest valid channel index that can be used with this image,
    * corresponding to a nominal or alpha channel. If the image transported by
    * this object is empty, or if this object does not transport an image, this
    * function returns -1.
    */
   int LastChannel() const
   {
      return m_data->image ? m_data->image->LastChannel() : -1;
   }

   /*!
    * Returns true iff the specified channel index \a c is valid. A valid
    * channel index corresponds to an existing channel in this image. Returns
    * false if this object does not transport an image.
    */
   bool IsValidChannelIndex( int c ) const
   {
      return m_data->image && m_data->image->IsValidChannelIndex( c );
   }

   /*!
    * Returns the number of nominal channels in the image transported by this
    * %ImageVariant, or zero if this object transports no image.
    *
    * Nominal channels are those defined by the color space of the image: One
    * channel for monochrome grayscale images, and three channels for RGB color
    * images. Additional channels are known as <em>alpha channels</em>.
    */
   int NumberOfNominalChannels() const
   {
      return m_data->image ? m_data->image->NumberOfNominalChannels() : 0;
   }

   /*!
    * Returns true iff this %ImageVariant object transports an image with one or
    * more alpha channels.
    *
    * Alpha channels are those in excess of the nominal channels corresponding
    * to the color space of the image; for example, a second channel in a
    * grayscale image, or a fourth channel in an RGB color image.
    */
   bool HasAlphaChannels() const
   {
      return m_data->image && m_data->image->HasAlphaChannels();
   }

   /*!
    * Returns the number of alpha channels in the image transported by this
    * %ImageVariant, or zero if this object transports no image.
    *
    * Alpha channels are those in excess of the nominal channels corresponding
    * to the color space of the image; for example, a second channel in a
    * grayscale image, or a fourth channel in an RGB color image.
    */
   int NumberOfAlphaChannels() const
   {
      return m_data->image ? m_data->image->NumberOfAlphaChannels() : 0;
   }

   /*!
    * Returns the total number of samples in the image transported by this
    * %ImageVariant, or zero if this object does not transport an image.
    *
    * The number of samples is equal to NumberOfPixels()*NumberOfChannels().
    */
   size_type NumberOfSamples() const
   {
      return m_data->image ? m_data->image->NumberOfSamples() : 0;
   }

   /*!
    * Returns the total number of nominal samples in the image transported by
    * this %ImageVariant, or zero if this object does not transport an image.
    *
    * The number of nominal samples is equal to
    * NumberOfPixels()*NumberOfNominalChannels().
    */
   size_type NumberOfNominalSamples() const
   {
      return m_data->image ? m_data->image->NumberOfNominalSamples() : 0;
   }

   /*!
    * Returns the total number of alpha samples in the image transported by
    * this %ImageVariant, or zero if this object does not transport an image.
    *
    * The number of alpha samples is equal to
    * NumberOfPixels()*NumberOfAlphaChannels().
    */
   size_type NumberOfAlphaSamples() const
   {
      return m_data->image ? m_data->image->NumberOfAlphaSamples() : 0;
   }

   /*!
    * Returns the bounding rectangle of this image.
    *
    * The upper left corner of the returned rectangle (x0, y0) is always (0,0).
    * The lower right corner coordinates (x1, y1) correspond to the width and
    * height of the image.
    *
    * If this object transports no image, this function returns an empty
    * rectangle.
    */
   Rect Bounds() const
   {
      return m_data->image ? m_data->image->Bounds() : Rect( 0 );
   }

   /*!
    * Returns true iff this image includes the specified point \a p in image
    * coordinates.
    *
    * If this object transports no image, this function returns false.
    */
   template <typename T>
   bool Includes( const GenericPoint<T>& p ) const
   {
      return m_data->image && m_data->image->Includes( p );
   }

   /*!
    * Returns true iff this image includes the specified rectangle \a r in image
    * coordinates.
    *
    * If this object transports no image, this function returns false.
    */
   template <typename T>
   bool Includes( const GenericRectangle<T>& r ) const
   {
      return m_data->image && m_data->image->Includes( r );
   }

   /*!
    * Returns true iff this image includes a rectangle given by its separate
    * image coordinates.
    *
    * \param x0,y0   Upper left corner coordinates (horizontal, vertical).
    * \param x1,y1   Lower right corner coordinates (horizontal, vertical).
    *
    * If this object transports no image, this function returns false.
    */
   template <typename T>
   bool Includes( T x0, T y0, T x1, T y1 ) const
   {
      return m_data->image && m_data->image->Includes( x0, y0, x1, y1 );
   }

   /*!
    * Returns true iff this image includes a point given by its separate image
    * coordinates.
    *
    * \param x    Horizontal coordinate.
    * \param y    Vertical coordinate.
    *
    * If this object transports no image, this function returns false.
    */
   template <typename T>
   bool Includes( T x, T y ) const
   {
      return m_data->image && m_data->image->Includes( x, y );
   }

   /*!
    * Returns true iff this image intersects with the specified rectangle \a r
    * in image coordinates.
    *
    * If this object transports no image, this function returns false.
    */
   template <typename T>
   bool Intersects( const pcl::GenericRectangle<T>& r ) const
   {
      return m_data->image && m_data->image->Intersects( r );
   }

   /*!
    * Returns true iff this image intersects with a rectangle given by its
    * separate image coordinates.
    *
    * \param x0,y0  Upper left corner coordinates (horizontal, vertical).
    * \param x1,y1  Lower right corner coordinates (horizontal, vertical).
    *
    * If this object transports no image, this function returns false.
    */
   template <typename T>
   bool Intersects( T x0, T y0, T x1, T y1 ) const
   {
      return m_data->image && m_data->image->Intersects( x0, y0, x1, y1 );
   }

   /*!
    * Constrains a point \a p to stay within the boundaries of this image.
    * Returns true iff the original point location is included in this image.
    *
    * If this object transports no image, this function does nothing and
    * returns false.
    */
   template <typename T>
   bool Clip( pcl::GenericPoint<T>& p ) const
   {
      return m_data->image && m_data->image->Clip( p );
   }

   /*!
    * Constrains two point coordinates \a x and \a y to stay within the
    * boundaries of this image.
    *
    * \param[out] x  Horizontal coordinate of the clipped point.
    * \param[out] y  Vertical coordinate of the clipped point.
    *
    * Returns true iff the original point location is included in this image.
    *
    * If this object transports no image, this function does nothing and
    * returns false.
    */
   template <typename T>
   bool Clip( T& x, T& y ) const
   {
      return m_data->image && m_data->image->Clip( x, y );
   }

   /*!
    * Constrains a rectangular region \a r in image coordinates to fit into the
    * boundaries of this image. Also ensures coherence of clipped rectangular
    * coordinates such that r.x0 <= r.x1 and r.y0 <= r.y1.
    *
    * Returns true iff the original rectangle intersects this image.
    *
    * If this object transports no image, this function does nothing and
    * returns false.
    */
   template <typename T>
   bool Clip( pcl::GenericRectangle<T>& r ) const
   {
      return m_data->image && m_data->image->Clip( r );
   }

   /*!
    * Constrains a rectangular region, given by its separate image
    * coordinates, to fit into the boundaries of this image. Also ensures
    * coherence of rectangular coordinates, such that x0 <= x1 and y0 <= y1.
    *
    * \param[out] x0,y0   Upper left corner coordinates (horizontal, vertical)
    *             of the rectangle that will be clipped.
    *
    * \param[out] x1,y1   Lower right corner coordinates (horizontal, vertical)
    *             of the rectangle that will be clipped.
    *
    * Returns true iff the original rectangle intersects the image.
    *
    * If this object transports no image, this function does nothing and
    * returns false.
    */
   template <typename T>
   bool Clip( T& x0, T& y0, T& x1, T& y1 ) const
   {
      return m_data->image && m_data->image->Clip( x0, y0, x1, y1 );
   }

   /*!
    * Selects a single channel.
    *
    * \param c    Channel index, 0 <= \a c < \a n, where \a n is the total
    *             number of channels in this image, including alpha channels.
    */
   void SelectChannel( int c ) const
   {
      if ( m_data->image )
         m_data->image->SelectChannel( c );
   }

   /*!
    * Returns the index of the currently selected channel, or -1 if this object
    * does not transport an image.
    *
    * If the current channel selection includes more than one channel, this
    * function returns the index of the first selected channel.
    *
    * This function is a convenience synonym for FirstSelectedChannel().
    */
   int SelectedChannel() const
   {
      return m_data->image ? m_data->image->SelectedChannel() : -1;
   }

   /*!
    * Selects a range of channels by their channel indices. The selected range
    * \e includes the two channels specified.
    *
    * \param c0   Index of the first channel to select.
    * \param c1   Index of the last channel to select.
    */
   void SelectChannelRange( int c0, int c1 ) const
   {
      if ( m_data->image )
         m_data->image->SelectChannelRange( c0, c1 );
   }

   /*!
    * Sets the current channel range selection to include all nominal channels
    * exclusively, excluding alpha channels.
    */
   void SelectNominalChannels() const
   {
      if ( m_data->image )
         m_data->image->SelectNominalChannels();
   }

   /*!
    * Sets the current channel range selection to include the existing alpha
    * channels only, excluding the nominal channels.
    *
    * \note If this image has no alpha channels, this function selects the last
    * nominal channel. The channel range selection cannot be empty by design.
    */
   void SelectAlphaChannels() const
   {
      if ( m_data->image )
         m_data->image->SelectAlphaChannels();
   }

   /*!
    * Resets the channel range selection to include all existing channels (all
    * nominal and alpha channels) in this image.
    */
   void ResetChannelRange() const
   {
      if ( m_data->image )
         m_data->image->ResetChannelRange();
   }

   /*!
    * Returns the number of selected channels, or zero if this object does not
    * transport an image.
    */
   int NumberOfSelectedChannels() const
   {
      return m_data->image ? m_data->image->NumberOfSelectedChannels() : 0;
   }

   /*!
    * Returns the channel index of the first selected channel, or -1 if this
    * object does not transport an image.
    */
   int FirstSelectedChannel() const
   {
      return m_data->image ? m_data->image->FirstSelectedChannel() : -1;
   }

   /*!
    * Returns the channel index of the last selected channel, or -1 if this
    * object does not transport an image.
    */
   int LastSelectedChannel() const
   {
      return m_data->image ? m_data->image->LastSelectedChannel() : -1;
   }

   /*!
    * Copies the first and last channel indices of the current channel
    * selection to the specified variables.
    *
    * \param[out] c0 Index of the first selected channel.
    * \param[out] c1 Index of the last selected channel.
    */
   void GetSelectedChannelRange( int& c0, int& c1 ) const
   {
      if ( m_data->image )
         m_data->image->GetSelectedChannelRange( c0, c1 );
   }

   /*!
    * Selects an anchor point by its separate image coordinates.
    *
    * \param x Horizontal coordinate of the new anchor point.
    * \param y Vertical coordinate of the new anchor point.
    */
   void SelectPoint( int x, int y ) const
   {
      if ( m_data->image )
         m_data->image->SelectPoint( x, y );
   }

   /*!
    * Selects a new anchor point \a p in image coordinates.
    */
   void SelectPoint( const Point& p ) const
   {
      if ( m_data->image )
         m_data->image->SelectPoint( p );
   }

   /*!
    * Resets the anchor point to the origin of image coordinates, i.e to x=y=0.
    */
   void ResetPoint() const
   {
      if ( m_data->image )
         m_data->image->ResetPoint();
   }

   /*!
    * Returns the current anchor point, or zero if this object does not
    * transport an image.
    */
   Point SelectedPoint() const
   {
      return m_data->image ? m_data->image->SelectedPoint() : Point( 0 );
   }

   /*!
    * Defines the current rectangular selection by its separate image
    * coordinates.
    *
    * \param x0,y0   Upper left corner coordinates (horizontal, vertical) of
    *                the new rectangular selection.
    *
    * \param x1,y1   Lower right corner coordinates (horizontal, vertical) of
    *                the new rectangular selection.
    *
    * The resulting selection is constrained to stay within the image
    * boundaries.
    */
   void SelectRectangle( int x0, int y0, int x1, int y1 ) const
   {
      if ( m_data->image )
         m_data->image->SelectRectangle( x0, y0, x1, y1 );
   }

   /*!
    * Defines the current rectangular selection by its separate corner
    * points in image coordinates.
    *
    * \param p0   Position of the upper left corner of the new selection.
    *
    * \param p1   Position of the lower right corner of the new selection.
    */
   void SelectRectangle( const Point& p0, const Point& p1 ) const
   {
      if ( m_data->image )
         m_data->image->SelectRectangle( p0, p1 );
   }

   /*!
    * Defines the current rectangular selection as the specified rectangle \a r
    * in image coordinates.
    */
   void SelectRectangle( const Rect& r ) const
   {
      if ( m_data->image )
         m_data->image->SelectRectangle( r );
   }

   /*!
    * Resets the rectangular selection to include the entire image boundaries.
    */
   void ResetSelection() const
   {
      if ( m_data->image )
         m_data->image->ResetSelection();
   }

   /*!
    * Returns true iff the current selection is empty, i.e. if its area is zero.
    * Returns false if the current selection is not empty, or if this object
    * does not transport an image.
    */
   bool IsEmptySelection() const
   {
      return m_data->image ? m_data->image->IsEmptySelection() : false;
   }

   /*!
    * Returns true iff the current rectangular selection comprises the entire
    * image. Returns false if the selection is not complete, or if this object
    * does not transport an image.
    */
   bool IsFullSelection() const
   {
      return m_data->image ? m_data->image->IsFullSelection() : false;
   }

   /*!
    * Returns the current rectangular selection, or zero if this object does
    * not transport an image.
    */
   Rect SelectedRectangle() const
   {
      return m_data->image ? m_data->image->SelectedRectangle() : Rect( 0 );
   }

   /*!
    * Returns true if this image is completely selected; false if it is
    * only partially selected, or if this object does not transport an image.
    *
    * In a completely selected image, the current rectangular selection
    * includes the whole image, and the current channel range selection
    * comprises all existing channels, including nominal and alpha channels.
    */
   bool IsCompletelySelected() const
   {
      return m_data->image ? m_data->image->IsCompletelySelected() : false;
   }

   /*!
    * Returns the number of selected pixels. This is the area in square pixels
    * of the current selection rectangle. Returns zero if this object does not
    * transport an image.
    */
   size_type NumberOfSelectedPixels() const
   {
      return m_data->image ? m_data->image->NumberOfSelectedPixels() : 0;
   }

   /*!
    * Returns the number of selected samples. This is the area in square pixels
    * of the current selection rectangle multiplied by the number of selected
    * channels. Returns zero if this object does not transport an image.
    */
   size_type NumberOfSelectedSamples() const
   {
      return m_data->image ? m_data->image->NumberOfSelectedSamples() : 0;
   }

   /*!
    * Returns true iff range clipping is currently enabled for this image.
    * Returns false if range clipping is disabled, or if this object does not
    * transport an image.
    *
    * When range clipping is enabled, pixel samples outside the current
    * clipping range:
    *
    * ]RangeClipLow(),RangeClipHigh()[
    *
    * are ignored by statistics calculation routines. The clipping range is
    * always defined in the normalized [0,1] range for all pixel sample data
    * types; the necessary conversions are performed transparently.
    *
    * When range clipping is disabled, the clipping range is ignored and all
    * pixel samples are considered for statistics calculations.
    *
    * The default clipping range is the normalized [0,1] range. Range clipping
    * is disabled by default.
    */
   bool IsRangeClippingEnabled() const
   {
      return m_data->image ? m_data->image->IsRangeClippingEnabled() : false;
   }

   /*!
    * Enables range clipping for statistical calculations.
    *
    * See IsRangeClippingEnabled() for more information on range clipping.
    */
   void EnableRangeClipping( bool enable = true ) const
   {
      if ( m_data->image )
         m_data->image->EnableRangeClipping( enable );
   }

   /*!
    * Disables range clipping for statistical calculations.
    *
    * See IsRangeClippingEnabled() for more information on range clipping.
    */
   void DisableRangeClipping( bool disable = true ) const
   {
      if ( m_data->image )
         m_data->image->DisableRangeClipping( disable );
   }

   /*!
    * Returns the lower bound of the current clipping range. Returns zero if
    * this object does not transport an image.
    *
    * See IsRangeClippingEnabled() for more information on range clipping.
    */
   double RangeClipLow() const
   {
      return m_data->image ? m_data->image->RangeClipLow() : 0.0;
   }

   /*!
    * Returns the upper bound of the current clipping range. Returns zero if
    * this object does not transport an image.
    *
    * See IsRangeClippingEnabled() for more information on range clipping.
    */
   double RangeClipHigh() const
   {
      return m_data->image ? m_data->image->RangeClipHigh() : 0.0;
   }

   /*!
    * Sets the lower bound of the clipping range.
    *
    * See IsRangeClippingEnabled() for more information on range clipping.
    */
   void SetRangeClipLow( double clipLow ) const
   {
      if ( m_data->image )
         m_data->image->SetRangeClipLow( clipLow );
   }

   /*!
    * Sets the upper bound of the clipping range.
    *
    * See IsRangeClippingEnabled() for more information on range clipping.
    */
   void SetRangeClipHigh( double clipHigh ) const
   {
      if ( m_data->image )
         m_data->image->SetRangeClipHigh( clipHigh );
   }

   /*!
    * Sets the lower and upper bounds of the clipping range and enables range
    * clipping, in a single function call.
    *
    * See IsRangeClippingEnabled() for more information on range clipping.
    */
   void SetRangeClipping( double clipLow, double clipHigh ) const
   {
      if ( m_data->image )
         m_data->image->SetRangeClipping( clipLow, clipHigh );
   }

   /*!
    * Resets the range clipping parameters:
    *
    * Clipping range lower bound = 0.0
    * Clipping range upper bound = 1.0
    * Range clipping disabled
    */
   void ResetRangeClipping() const
   {
      if ( m_data->image )
         m_data->image->ResetRangeClipping();
   }

   /*!
    * Resets all image selections to default values:
    *
    * \li All channels are selected, including nominal and alpha channels.
    * \li The anchor point is located at {0,0}, that is the upper left corner.
    * \li The rectangular selection is set to comprise the entire image.
    * \li The clipping range is set to [0,1].
    * \li Range clipping is disabled.
    *
    * Calling this member function is equivalent to:
    *
    * \code
    * ResetChannelRange();
    * ResetPoint();
    * ResetSelection();
    * ResetRangeClipping();
    * \endcode
    */
   void ResetSelections() const
   {
      if ( m_data->image )
         m_data->image->ResetSelections();
   }

   /*!
    * Returns a reference to the internal ImageSelections object in the image
    * transported by this %ImageVariant.
    *
    * \note This member function assumes that this object transports a valid
    * image. Do not call it otherwise.
    */
   ImageSelections& Selections() const
   {
      return m_data->image->Selections();
   }

   /*!
    * Saves the current selections (rectangular area, channel range, anchor
    * point and range clipping), pushing them to the internal selection stack.
    */
   void PushSelections() const
   {
      if ( m_data->image )
         m_data->image->PushSelections();
   }

   /*!
    * Restores and pops (removes) the current selections (rectangular area,
    * channel range, anchor point and range clipping) from the internal
    * selection stack.
    *
    * If no selections have been previously pushed to the internal selection
    * stack, this function is ignored.
    */
   void PopSelections() const
   {
      if ( m_data->image )
         m_data->image->PopSelections();
   }

   /*!
    * Returns true if one or more selections have been pushed to the internal
    * selection stack, that is, if the PopSelections() function can be called
    * to restore them. Returns false otherwise, or if this object does not
    * transport an image.
    */
   bool CanPopSelections() const
   {
      return m_data->image ? m_data->image->CanPopSelections() : false;
   }

   /*!
    * Returns a reference to the status monitoring object associated with the
    * image transported by this %ImageVariant.
    *
    * \note This member function assumes that this object transports a valid
    * image. Do not call it otherwise.
    */
   StatusMonitor& Status() const
   {
      PCL_PRECONDITION( m_data->image != nullptr )
      return m_data->image->Status();
   }

   /*!
    * Returns the address of the status monitoring callback object currently
    * selected for the image transported by this %ImageVariant, or zero if this
    * object does not transport an image.
    */
   pcl::StatusCallback* StatusCallback() const
   {
      return m_data->image ? m_data->image->StatusCallback() : nullptr;
   }

   /*!
    * Specifies the address of an object that will be used to perform status
    * monitoring callbacks for the image transported by this %ImageVariant.
    *
    * If this object does not transport an image, calling this member function
    * has no effect.
    */
   void SetStatusCallback( pcl::StatusCallback* callback ) const
   {
      if ( m_data->image )
         m_data->image->SetStatusCallback( callback );
   }

   /*!
    * Returns true iff this image is allowed to use multiple parallel execution
    * threads (when multiple threads are permitted and available) for member
    * functions that support parallel execution.
    *
    * If this object does not transport an image, this member function returns
    * false.
    */
   bool IsParallelProcessingEnabled() const
   {
      return m_data->image != nullptr && m_data->image->IsParallelProcessingEnabled();
   }

   /*!
    * Enables parallel processing for this image.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * \param maxProcessors    The maximum number of processors allowed for this
    *                image. If \a enable is false this parameter is ignored. A
    *                value <= 0 is ignored. The default value is zero.
    *
    * If this object does not transport an image, calling this member function
    * has no effect.
    */
   void EnableParallelProcessing( bool enable = true, int maxProcessors = 0 )
   {
      if ( m_data->image )
         m_data->image->EnableParallelProcessing( enable, maxProcessors );
   }

   /*!
    * Disables parallel processing for this image.
    *
    * This is a convenience function, equivalent to:
    * EnableParallelProcessing( !disable )
    *
    * If this object does not transport an image, calling this member function
    * has no effect.
    */
   void DisableParallelProcessing( bool disable = true )
   {
      if ( m_data->image )
         m_data->image->DisableParallelProcessing( disable );
   }

   /*!
    * Returns the maximum number of processors allowed for this image, or zero
    * if this object does not transport an image.
    *
    * The returned nonzero value is the maximum number of processors that this
    * image can use in member functions that support parallel execution.
    *
    * Irrespective of the value returned by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   int MaxProcessors() const
   {
      return m_data->image ? m_data->image->MaxProcessors() : 0;
   }

   /*!
    * Sets the maximum number of processors allowed for this image.
    *
    * The specified \a maxProcessors parameter is the maximum number of
    * processors that this image can use in member functions that support
    * parallel execution.
    *
    * In the current versions of PCL, a module can use a maximum of 1023
    * processors. The term \e processor actually refers to the number of
    * threads a module can execute concurrently.
    *
    * Irrespective of the value specified by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    *
    * If this object does not transport an image, calling this member function
    * has no effect.
    */
   void SetMaxProcessors( int maxProcessors )
   {
      if ( m_data->image )
         m_data->image->SetMaxProcessors( maxProcessors );
   }

   /*!
    * Returns the size in bytes of a row of pixels (also known as a <em>scan
    * line</em>) in the image transported by this %ImageVariant image, or zero
    * if this object transports no image.
    *
    * The returned value is BytesPerSample()*Width().
    */
   size_type LineSize() const
   {
      return m_data->image ? BytesPerSample() * size_type( m_data->image->Width() ) : 0;
   }

   /*!
    * Returns the size in bytes of a channel in the image transported by this
    * %ImageVariant, or zero if this object transports no image.
    *
    * The returned value is BytesPerSample()*NumberOfPixels().
    */
   size_type ChannelSize() const
   {
      return BytesPerSample() * NumberOfPixels();
   }

   /*!
    * Returns the size in bytes of the allocated pixel data in the image
    * transported by this %ImageVariant, or zero if this object transports no
    * image.
    *
    * The returned value is NumberOfSamples()*BytesPerSample().
    */
   size_type ImageSize() const
   {
      return ChannelSize() * size_type( NumberOfChannels() );
   }

   /*!
    * Returns the size in bytes of all memory blocks required to store the
    * pixel data in the nominal channels (i.e., \e excluding alpha channels) of
    * the image transported by this %ImageVariant, or zero if this object
    * transports no image.
    *
    * The returned value is ChannelSize()*NumberOfNominalChannels().
    */
   size_type NominalSize() const
   {
      return ChannelSize() * size_type( NumberOfNominalChannels() );
   }

   /*!
    * Returns the size in bytes of all memory blocks required to store the
    * pixel data in the alpha channels  (i.e., \e excluding nominal channels)
    * of this image. Returns zero if this image has no alpha channels, or if
    * this object transports no image.
    *
    * The returned value is ChannelSize()*NumberOfAlphaChannels().
    */
   size_type AlphaSize() const
   {
      return ChannelSize() * size_type( NumberOfAlphaChannels() );
   }

   /*!
    * Returns true iff this %ImageVariant transports a color image. Returns
    * false if this object transports a grayscale image, or if it does not
    * transport any image.
    */
   bool IsColor() const
   {
      return m_data->image && m_data->image->IsColor();
   }

   /*!
    * Returns the identifier of the color space of the image transported by
    * this %ImageVariant.
    *
    * This function returns the value of a symbolic constant enumerated by the
    * ColorSpace namespace. If this object transports no image, this function
    * returns ColorSpace::Unknown.
    */
   color_space ColorSpace() const
   {
      return m_data->image ? m_data->image->ColorSpace() : ColorSpace::Unknown;
   }

   /*!
    * Returns the identifier of a nominal channel or component \a c in the
    * color space of the image transported by this %ImageVariant, or an empty
    * string if this object transports no image.
    */
   String ChannelId( int c ) const
   {
      return m_data->image ? m_data->image->ChannelId( c ) : String();
   }

   /*!
    * Returns a reference to the RGB working space associated with the image
    * transported by this %ImageVariant object.
    *
    * \warning Do not call this member function if the %ImageVariant does not
    * transport an image.
    */
   const RGBColorSystem& RGBWorkingSpace() const
   {
      PCL_PRECONDITION( m_data->image != nullptr )
      return m_data->image->RGBWorkingSpace();
   }

   /*!
    * Associates a given RGB working space ( RGBWS ) with the image transported
    * by this %ImageVariant object.
    *
    * \note For shared images ( i.e. images living in the PixInsight core
    * application ), the RGB working space cannot be changed by calling this
    * function.
    *
    * \warning If this member function is called for a shared image, it throws
    * an Error exception with the appropriate error message.
    */
   void SetRGBWorkingSpace( const RGBColorSystem& rgbws )
   {
      if ( m_data->image )
         m_data->image->SetRGBWorkingSpace( rgbws );
   }

   // -------------------------------------------------------------------------

#define __ABSOLUTE_DIFFERENCE( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).AbsoluteDifference( scalar, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * computes the absolute values of their differences with the specified
    * \a scalar. Returns the resulting image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::AbsoluteDifference()
    */
   template <typename T>
   ImageVariant AbsoluteDifference( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __ABSOLUTE_DIFFERENCE )
      return result;
   }

#undef __ABSOLUTE_DIFFERENCE

   // -------------------------------------------------------------------------

#define __ABSOLUTE_VALUE( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).AbsoluteValue( rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * replaced with their absolute values.
    *
    * This member function is a generalized wrapper for
    * GenericImage::AbsoluteValue()
    */
   ImageVariant AbsoluteValue( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __ABSOLUTE_VALUE )
      return result;
   }

#undef __ABSOLUTE_VALUE

   // -------------------------------------------------------------------------

#define __ADD( I ) \
   static_cast<pcl::I&>( **this ).Add( scalar, rect, firstChannel, lastChannel )

   /*!
    * Adds the specified \a scalar to a subset of pixel samples in this image.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Add()
    */
   template <typename T>
   ImageVariant& Add( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE( __ADD )
      return *this;
   }

#undef __ADD

   // -------------------------------------------------------------------------

#define __ADD_1( I ) \
   ImageVariant::Add( static_cast<pcl::I&>( **this ), image, point, channel, rect, firstChannel, lastChannel )

#define __ADD_2( I ) \
   image1.Add( static_cast<const pcl::I&>( *image2 ), point, channel, rect, firstChannel, lastChannel )

private:

   template <class P> static
   void Add( GenericImage<P>& image1, const ImageVariant& image2,
             const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_2( image2, __ADD_2 )
   }

public:

   /*!
    * Adds pixel samples of the specified \a image to a subset of pixel samples
    * in this image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Add()
    */
   ImageVariant& Add( const ImageVariant& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __ADD_1 )
      return *this;
   }

#undef __ADD_1
#undef __ADD_2

   // -------------------------------------------------------------------------

#define __ADDED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Added( scalar, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * adds a \a scalar. Returns the resulting image.
    *
    * This member function is a generalized wrapper for GenericImage::Added()
    */
   template <typename T>
   ImageVariant Added( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __ADDED )
      return result;
   }

#undef __ADDED

   // -------------------------------------------------------------------------

#define __ALLOCATE_DATA( I ) \
   static_cast<pcl::I&>( **this ).AllocateData( width, height, numberOfChannels, colorSpace )

   /*!
    * Allocates new pixel data with the specified geometry and color space.
    * Returns a reference to this image.
    *
    * If this object transports no image, a new local image is created in
    * 32-bit floating point format by calling CreateImage().
    *
    * This member function is a generalized wrapper for
    * GenericImage::AllocateData()
    */
   ImageVariant& AllocateData( int width, int height, int numberOfChannels = 1, color_space colorSpace = ColorSpace::Gray )
   {
      if ( !*this )
         CreateImage();
      SOLVE_TEMPLATE( __ALLOCATE_DATA )
      return *this;
   }

#undef __ALLOCATE_DATA

   // -------------------------------------------------------------------------

#define __ALLOCATE_DATA( I ) \
   static_cast<pcl::I&>( **this ).AllocateData( rect, numberOfChannels, colorSpace )

   /*!
    * Allocates new pixel data with the specified geometry and color space.
    * Returns a reference to this image.
    *
    * If this object transports no image, a new local image is created in
    * 32-bit floating point format by calling CreateImage().
    *
    * This member function is a generalized wrapper for
    * GenericImage::AllocateData()
    */
   ImageVariant& AllocateData( const Rect& rect, int numberOfChannels = 1, color_space colorSpace = ColorSpace::Gray )
   {
      if ( !*this )
         CreateImage();
      SOLVE_TEMPLATE( __ALLOCATE_DATA )
      return *this;
   }

#undef __ALLOCATE_DATA

   // -------------------------------------------------------------------------

#define __AND( I ) \
   static_cast<pcl::I&>( **this ).And( scalar, rect, firstChannel, lastChannel )

   /*!
    * Performs a bitwise AND operation between a subset of pixel samples in
    * this image and the specified \a scalar. Returns a reference to this
    * image.
    *
    * This member function is a generalized wrapper for GenericImage::And()
    */
   template <typename T>
   ImageVariant& And( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __AND )
      return *this;
   }

#undef __AND

   // -------------------------------------------------------------------------

#define __AND_1( I ) \
   ImageVariant::And( static_cast<pcl::I&>( **this ), image, point, channel, rect, firstChannel, lastChannel )

#define __AND_2( I ) \
   image1.And( static_cast<const pcl::I&>( *image2 ), point, channel, rect, firstChannel, lastChannel )

private:

   template <class P> static
   void And( GenericImage<P>& image1, const ImageVariant& image2,
             const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_REAL_2( image2, __AND_2 )
   }

public:

   /*!
    * Performs a bitwise AND operation between a subset of pixel samples in
    * this image and the corresponding samples of the specified source
    * \a image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::And()
    */
   ImageVariant& And( const ImageVariant& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE_REAL( __AND_1 )
      return *this;
   }

#undef __AND_1
#undef __AND_2

   // -------------------------------------------------------------------------

#define __APPLIED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Applied( scalar, op, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * performs an arithmetic or bitwise logical operation with a scalar.
    * Returns the resulting image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Applied( T )
    */
   template <typename T>
   ImageVariant Applied( T scalar, image_op op = ImageOp::Mov, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __APPLIED )
      return result;
   }

#undef __APPLIED

   // -------------------------------------------------------------------------

#define __APPLIED_1( I ) \
   result = ImageVariant::Applied( static_cast<const pcl::I&>( **this ), image, op, point, channel, rect, firstChannel, lastChannel )

#define __APPLIED_2( I ) \
   result.SetImage( *new pcl::I( image1.Applied( static_cast<const pcl::I&>( *image2 ), op, point, channel, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

private:

   template <class P> static
   ImageVariant Applied( const GenericImage<P>& image1, const ImageVariant& image2,
                         image_op op, const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      ImageVariant result;
      SOLVE_TEMPLATE_2( image2, __APPLIED_2 )
      return result;
   }

public:

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * performs an arithmetic or bitwise logical operation with the
    * corresponding samples of another \a image. Returns the resulting image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Applied( const GenericImage& )
    */
   ImageVariant Applied( const ImageVariant& image, image_op op = ImageOp::Mov,
                         const Point& point = Point( int_max ), int channel = -1,
                         const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __APPLIED_1 )
      return result;
   }

#undef __APPLIED_1
#undef __APPLIED_2

   // -------------------------------------------------------------------------

#define __APPLIED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Applied( transformation, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * applies an image transformation. Returns the resulting image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Applied( const ImageTransformation& )
    */
   ImageVariant Applied( const ImageTransformation& transformation,
                         const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __APPLIED )
      return result;
   }

#undef __APPLIED

   // -------------------------------------------------------------------------

#define __APPLY( I ) \
   static_cast<pcl::I&>( **this ).Apply( scalar, op, rect, firstChannel, lastChannel )

   /*!
    * Replaces a subset of pixel samples with the result of an arithmetic,
    * bitwise logical or pixel composition operation with a scalar. Returns a
    * reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Apply( T )
    */
   template <typename T>
   ImageVariant& Apply( T scalar, image_op op = ImageOp::Mov, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE( __APPLY )
      return *this;
   }

#undef __APPLY

   // -------------------------------------------------------------------------

#define __APPLY_1( I ) \
   ImageVariant::Apply( static_cast<pcl::I&>( **this ), image, op, point, channel, rect, firstChannel, lastChannel )

#define __APPLY_2( I ) \
   image1.Apply( static_cast<const pcl::I&>( *image2 ), op, point, channel, rect, firstChannel, lastChannel )

private:

   template <class P> static
   void Apply( GenericImage<P>& image1, const ImageVariant& image2,
               image_op op, const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_2( image2, __APPLY_2 )
   }

public:

   /*!
    * Performs an arithmetic, bitwise logical or pixel composition operation
    * between a subset of pixel samples in this image and the corresponding
    * samples of another operand \a image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Apply( const GenericImage& )
    */
   ImageVariant& Apply( const ImageVariant& image, image_op op = ImageOp::Mov,
                        const Point& point = Point( int_max ), int channel = -1,
                        const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __APPLY_1 )
      return *this;
   }

#undef __APPLY_1
#undef __APPLY_2

   // -------------------------------------------------------------------------

#define __APPLY( I ) \
   static_cast<pcl::I&>( **this ).Apply( transformation, rect, firstChannel, lastChannel )

   /*!
    * Applies an image transformation to a subset of pixel samples. Returns a
    * reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Apply( const ImageTransformation& )
    */
   ImageVariant& Apply( const ImageTransformation& transformation,
                        const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE( __APPLY )
      return *this;
   }

#undef __APPLY

   // -------------------------------------------------------------------------

#define __ASSIGN_IMAGE_1( I ) \
   ImageVariant::AssignImage( static_cast<pcl::I&>( **this ), image, rect, firstChannel, lastChannel )

#define __ASSIGN_IMAGE_2( I ) \
   image1.Assign( static_cast<const pcl::I&>( *image2 ), rect, firstChannel, lastChannel )

private:

   template <class P> static
   void AssignImage( GenericImage<P>& image1, const ImageVariant& image2, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_2( image2, __ASSIGN_IMAGE_2 )
   }

public:

   /*!
    * Copies pixel samples from the specified source \a image to this image.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Assign()
    */
   ImageVariant& AssignImage( const ImageVariant& image, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __ASSIGN_IMAGE_1 )
      return *this;
   }

#undef __ASSIGN_IMAGE_1
#undef __ASSIGN_IMAGE_2

   // -------------------------------------------------------------------------

#define __AVG_DEV( I ) \
   result = static_cast<const pcl::I&>( **this ).AvgDev( center, rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Returns the mean absolute deviation of a subset of pixel samples with
    * respect to the specified \a center value.
    *
    * This member function is a generalized wrapper for GenericImage::AvgDev()
    */
   double AvgDev( double center, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      double result = 0;
      if ( *this )
         SOLVE_TEMPLATE( __AVG_DEV )
      return result;
   }

#undef __AVG_DEV

   // -------------------------------------------------------------------------

#define __BINARIZE( I ) \
   static_cast<pcl::I&>( **this ).Binarize( threshold, rect, firstChannel, lastChannel )

   /*!
    * Binarizes a subset of pixel samples with respect to the specified
    * \a threshold value. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Binarize()
    */
   template <typename T>
   ImageVariant& Binarize( T threshold, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __BINARIZE )
      return *this;
   }

#undef __BINARIZE

   // -------------------------------------------------------------------------

#define __BINARIZE( I ) \
   static_cast<pcl::I&>( **this ).Binarize( rect, firstChannel, lastChannel )

   /*!
    * Binarizes a subset of pixel samples with respect to the specified
    * \a threshold value. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Binarize()
    */
   ImageVariant& Binarize( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __BINARIZE )
      return *this;
   }

#undef __BINARIZE

   // -------------------------------------------------------------------------

#define __BINARIZED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Binarized( threshold, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * binarized with respect to the specified \a threshold value.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Binarized()
    */
   template <typename T>
   ImageVariant Binarized( T threshold, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE_REAL( __BINARIZED )
      return result;
   }

#undef __BINARIZED

   // -------------------------------------------------------------------------

#define __BINARIZED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Binarized( rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Binarizes a subset of pixel samples with respect to the central value of
    * the native range of the image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Binarized()
    */
   ImageVariant Binarized( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE_REAL( __BINARIZED )
      return result;
   }

#undef __BINARIZED

   // -------------------------------------------------------------------------

#define __PBMV( I ) \
   result = static_cast<const pcl::I&>( **this ).BendMidvariance( center, beta, rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Returns a percentage bend midvariance (PBMV) for a subset of pixel
    * samples, with respect the the specified \a center value, with the
    * specified \a beta rejection parameter.
    *
    * This member function is a generalized wrapper for
    * GenericImage::BendMidvariance()
    */
   double BendMidvariance( double center, double beta = 0.2,
                           const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                           int maxProcessors = 0 ) const
   {
      double result = false;
      if ( *this )
         SOLVE_TEMPLATE( __PBMV )
      return result;
   }

#undef __PBMV

   // -------------------------------------------------------------------------

#define __BWMV( I ) \
   result = static_cast<const pcl::I&>( **this ).BiweightMidvariance( center, sigma, k, rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Returns a biweight midvariance (BWMV) for a subset of pixel samples, with
    * respect the the specified \a center value, \a sigma estimate of
    * dispersion, and \a k threshold in sigma units.
    *
    * This member function is a generalized wrapper for
    * GenericImage::BiweightMidvariance()
    */
   double BiweightMidvariance( double center, double sigma, int k = 9,
                               const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                               int maxProcessors = 0 ) const
   {
      double result = false;
      if ( *this )
         SOLVE_TEMPLATE( __BWMV )
      return result;
   }

#undef __BWMV

   // -------------------------------------------------------------------------

#define __BLACK( I ) \
   static_cast<pcl::I&>( **this ).Black( rect, firstChannel, lastChannel )

   /*!
    * Fills a subset of pixel samples with the minimum sample value in the
    * native range of the image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Black()
    */
   ImageVariant& Black( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE( __BLACK )
      return *this;
   }

#undef __BLACK

   // -------------------------------------------------------------------------

#define __BLEND( I ) \
   static_cast<pcl::I&>( **this ).Blend( bitmap, point, rect )

   /*!
    * Blends a rectangular region of a 32-bit RGBA \a bitmap with this image.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Blend()
    */
   ImageVariant& Blend( const Bitmap& bitmap, const Point& point = Point( int_max ), const Rect& rect = Rect( 0 ) )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __BLEND )
      return *this;
   }

#undef __BLEND

   // -------------------------------------------------------------------------

#define __BLENDED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Blended( bitmap, point, rect ) ) ); \
   result.SetOwnership( true )

   /*!
    * Returns a local duplicate of this image blended with a rectangular region
    * of a 32-bit RGBA \a bitmap.
    *
    * This member function is a generalized wrapper for GenericImage::Blended()
    */
   ImageVariant Blended( const Bitmap& bitmap, const Point& point = Point( int_max ), const Rect& rect = Rect( 0 ) ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE_REAL( __BLENDED )
      return result;
   }

#undef __BLENDED

   // -------------------------------------------------------------------------

#define __COUNT( I ) \
   result = static_cast<const pcl::I&>( **this ).Count( rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Returns the number of pixel samples selectable for statistics calculation
    * from a subset of pixel samples.
    *
    * This member function is a generalized wrapper for GenericImage::Count()
    */
   uint64 Count( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      uint64 result = 0;
      if ( *this )
         SOLVE_TEMPLATE( __COUNT )
      return result;
   }

#undef __COUNT

   // -------------------------------------------------------------------------

#define __CREATE_ALPHA_CHANNELS( I ) \
   static_cast<pcl::I&>( **this ).CreateAlphaChannels( n )

   /*!
    * Creates \a n new alpha channels in this image. Alpha channels are those
    * in excess of nominal channels, e.g. a fourth channel in an RGB color
    * image, or a second channel in a grayscale image. Returns a reference to
    * this image.
    *
    * Newly created channels are not initialized, so their pixel samples will
    * contain unpredictable values.
    *
    * This member function is a generalized wrapper for
    * GenericImage::CreateAlphaChannels()
    */
   void CreateAlphaChannels( int n )
   {
      if ( *this )
         SOLVE_TEMPLATE( __CREATE_ALPHA_CHANNELS )
   }

#undef __CREATE_ALPHA_CHANNELS

   // -------------------------------------------------------------------------

#define __CROP( I ) \
   static_cast<pcl::I&>( **this ).Crop( fillValues )

   /*!
    * Crops the image to its current rectangular selection. Returns a reference
    * to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Crop()
    */
   template <typename T>
   ImageVariant& Crop( const GenericVector<T>& fillValues )
   {
      if ( *this )
         SOLVE_TEMPLATE( __CROP )
      return *this;
   }

#undef __CROP

   // -------------------------------------------------------------------------

#define __CROP( I ) \
   static_cast<pcl::I&>( **this ).Crop()

   /*!
    * Crops the image to its current rectangular selection, filling extended
    * regions with the minimum sample value in the native range of the image.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Crop()
    */
   ImageVariant& Crop()
   {
      if ( *this )
         SOLVE_TEMPLATE( __CROP )
      return *this;
   }

#undef __CROP

   // -------------------------------------------------------------------------

#define __CROP_BY( I ) \
   static_cast<pcl::I&>( **this ).CropBy( left, top, right, bottom, fillValues )

   /*!
    * Crops or extends the entire image by applying the specified margins.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::CropBy()
    */
   template <typename T>
   ImageVariant& CropBy( int left, int top, int right, int bottom, const GenericVector<T>& fillValues )
   {
      if ( *this )
         SOLVE_TEMPLATE( __CROP_BY )
      return *this;
   }

#undef __CROP_BY

   // -------------------------------------------------------------------------

#define __CROP_BY( I ) \
   static_cast<pcl::I&>( **this ).CropBy( left, top, right, bottom )

   /*!
    * Crops or extends the entire image by applying the specified margins.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::CropBy()
    */
   ImageVariant& CropBy( int left, int top, int right, int bottom )
   {
      if ( *this )
         SOLVE_TEMPLATE( __CROP_BY )
      return *this;
   }

#undef __CROP_BY

   // -------------------------------------------------------------------------

#define __CROP_TO( I ) \
   static_cast<pcl::I&>( **this ).CropTo( rect, fillValues )

   /*!
    * Crops the image to the specified rectangular region. Returns a reference
    * to this image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::CropTo()
    */
   template <typename T>
   ImageVariant& CropTo( const Rect& rect, const GenericVector<T>& fillValues )
   {
      if ( *this )
         SOLVE_TEMPLATE( __CROP_TO )
      return *this;
   }

#undef __CROP_TO

   // -------------------------------------------------------------------------

#define __CROP_TO( I ) \
   static_cast<pcl::I&>( **this ).CropTo( rect )

   /*!
    * Crops the image to the specified rectangular region, filling extended
    * regions with the minimum sample value in the native range of the image.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::CropTo()
    */
   ImageVariant& CropTo( const Rect& rect )
   {
      if ( *this )
         SOLVE_TEMPLATE( __CROP_TO )
      return *this;
   }

#undef __CROP_TO

   // -------------------------------------------------------------------------

#define __CROP_TO( I ) \
   static_cast<pcl::I&>( **this ).CropTo( x0, y0, x1, y1, fillValues )

   /*!
    * Crops the image to the specified rectangular region. Returns a reference
    * to this image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::CropTo()
    */
   template <typename T>
   ImageVariant& CropTo( int x0, int y0, int x1, int y1, const GenericVector<T>& fillValues )
   {
      if ( *this )
         SOLVE_TEMPLATE( __CROP_TO )
      return *this;
   }

#undef __CROP_TO

   // -------------------------------------------------------------------------

#define __CROP_TO( I ) \
   static_cast<pcl::I&>( **this ).CropTo( x0, y0, x1, y1 )

   /*!
    * Crops the image to the specified rectangular region, filling extended
    * regions with the minimum sample value in the native range of the image.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::CropTo()
    */
   ImageVariant& CropTo( int x0, int y0, int x1, int y1 )
   {
      if ( *this )
         SOLVE_TEMPLATE( __CROP_TO )
      return *this;
   }

#undef __CROP_TO

   // -------------------------------------------------------------------------

#define __DELETE_ALPHA_CHANNEL( I ) \
   static_cast<pcl::I&>( **this ).DeleteAlphaChannel( channel )

   /*!
    * Destroys an alpha channel. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::DeleteAlphaChannel()
    */
   void DeleteAlphaChannel( int channel )
   {
      if ( *this )
         SOLVE_TEMPLATE( __DELETE_ALPHA_CHANNEL )
   }

#undef __DELETE_ALPHA_CHANNEL

   // -------------------------------------------------------------------------

#define __DELETE_ALPHA_CHANNELS( I ) \
   static_cast<pcl::I&>( **this ).DeleteAlphaChannels()

   /*!
    * Destroys all existing alpha channels in this image. Returns a reference
    * to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::DeleteAlphaChannels()
    */
   void DeleteAlphaChannels()
   {
      if ( *this )
         SOLVE_TEMPLATE( __DELETE_ALPHA_CHANNELS )
   }

#undef __DELETE_ALPHA_CHANNELS

   // -------------------------------------------------------------------------

#define __DIVIDE( I ) \
   static_cast<pcl::I&>( **this ).Divide( scalar, rect, firstChannel, lastChannel )

   /*!
    * Divides a subset of pixel samples in this image by the specified
    * \a scalar. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Divide()
    */
   template <typename T>
   ImageVariant& Divide( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE( __DIVIDE )
      return *this;
   }

#undef __DIVIDE

   /*!
    * A synonym for Divide().
    */
   template <typename T>
   ImageVariant& Div( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Divide( scalar, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

#define __DIVIDE_1( I ) \
   ImageVariant::Divide( static_cast<pcl::I&>( **this ), image, point, channel, rect, firstChannel, lastChannel )

#define __DIVIDE_2( I ) \
   image1.Divide( static_cast<const pcl::I&>( *image2 ), point, channel, rect, firstChannel, lastChannel )

private:

   template <class P> static
   void Divide( GenericImage<P>& image1, const ImageVariant& image2,
                const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_2( image2, __DIVIDE_2 )
   }

public:

   /*!
    * Divides a subset of pixel samples in this image by samples from the
    * specified source \a image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Divide()
    */
   ImageVariant& Divide( const ImageVariant& image,
                         const Point& point = Point( int_max ), int channel = -1,
                         const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __DIVIDE_1 )
      return *this;
   }

#undef __DIVIDE_1
#undef __DIVIDE_2

   /*!
    * A synonym for Divide().
    */
   ImageVariant& Div( const ImageVariant& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Divide( image, point, channel, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

#define __DIVIDED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Divided( scalar, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * divides it by a \a scalar. Returns the resulting image.
    *
    * This member function is a generalized wrapper for GenericImage::Divided()
    */
   template <typename T>
   ImageVariant Divided( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __DIVIDED )
      return result;
   }

#undef __DIVIDED

   // -------------------------------------------------------------------------

#define __EXCHANGE_1( I ) \
   ImageVariant::Exchange( static_cast<pcl::I&>( **this ), image, point, channel, rect, firstChannel, lastChannel )

#define __EXCHANGE_2( I ) \
   image1.Exchange( static_cast<pcl::I&>( *image2 ), point, channel, rect, firstChannel, lastChannel )

private:

   template <class P> static
   void Exchange( GenericImage<P>& image1, ImageVariant& image2,
                  const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_2( image2, __EXCHANGE_2 )
   }

public:

   /*!
    * Exchanges a subset of pixel samples between this image and another
    * operand \a image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Exchange()
    */
   ImageVariant& Exchange( ImageVariant& image,
                           const Point& point = Point( int_max ), int channel = -1,
                           const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __EXCHANGE_1 )
      return *this;
   }

#undef __EXCHANGE_1
#undef __EXCHANGE_2

   /*!
    * A synonym for Exchange().
    */
   ImageVariant& Xchg( ImageVariant& image,
                       const Point& point = Point( int_max ), int channel = -1,
                       const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Exchange( image, point, channel, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

#define __FILL( I ) \
   static_cast<pcl::I&>( **this ).Fill( scalar, rect, firstChannel, lastChannel )

   /*!
    * Fills a subset of pixel samples of this image with the specified
    * \a scalar. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Fill()
    */
   template <typename T>
   ImageVariant& Fill( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE( __FILL )
      return *this;
   }

#undef __FILL

   // -------------------------------------------------------------------------

#define __FILL( I ) \
   static_cast<pcl::I&>( **this ).Fill( values, rect, firstChannel, lastChannel )

   /*!
    * Fills a subset of pixel samples of this image with the scalar components
    * of a vector of per-channel filling \a values. Returns a reference to this
    * image.
    *
    * This member function is a generalized wrapper for GenericImage::Fill()
    */
   template <typename T>
   ImageVariant& Fill( const GenericVector<T>& values, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE( __FILL )
      return *this;
   }

#undef __FILL

   // -------------------------------------------------------------------------

#define __FILLED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Filled( scalar, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * filled with the specified \a scalar.
    *
    * This member function is a generalized wrapper for GenericImage::Filled()
    */
   template <typename T>
   ImageVariant Filled( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __FILLED )
      return result;
   }

#undef __FILLED

   // -------------------------------------------------------------------------

#define __FILLED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Filled( values, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * filled with the scalar components of a vector of per-channel filling
    * \a values.
    *
    * This member function is a generalized wrapper for GenericImage::Filled()
    */
   template <typename T>
   ImageVariant Filled( const GenericVector<T>& values, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __FILLED )
      return result;
   }

#undef __FILLED

   // -------------------------------------------------------------------------

#define __FORGET_ALPHA_CHANNEL( I ) \
   static_cast<pcl::I&>( **this ).ForgetAlphaChannel( channel )

   /*!
    * Forces this image to discard an existing alpha channel without
    * deallocating it. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::ForgetAlphaChannel()
    */
   void ForgetAlphaChannel( int channel )
   {
      if ( *this )
         SOLVE_TEMPLATE( __FORGET_ALPHA_CHANNEL )
   }

#undef __FORGET_ALPHA_CHANNEL

   // -------------------------------------------------------------------------

#define __FORGET_ALPHA_CHANNELS( I ) \
   static_cast<pcl::I&>( **this ).ForgetAlphaChannels()

   /*!
    * Forces this image to discard all existing alpha channels without
    * deallocating them. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::ForgetAlphaChannels()
    */
   void ForgetAlphaChannels()
   {
      if ( *this )
         SOLVE_TEMPLATE( __FORGET_ALPHA_CHANNELS )
   }

#undef __FORGET_ALPHA_CHANNELS

   // -------------------------------------------------------------------------

#define __GET_COLUMN( I ) \
   static_cast<const pcl::I&>( **this ).GetColumn( buffer, x, channel )

   /*!
    * Stores a duplicate of a column of pixel samples in the specified array,
    * with implicit data type conversion.
    *
    * This member function is a generalized wrapper for
    * GenericImage::GetColumn()
    */
   template <typename T>
   void GetColumn( T* buffer, int x, int channel = -1 ) const
   {
      PCL_PRECONDITION( buffer != 0 )
      if ( *this )
         SOLVE_TEMPLATE( __GET_COLUMN )
   }

#undef __GET_COLUMN

   // -------------------------------------------------------------------------

#define __GET_EXTREME_SAMPLE_VALUES( I ) \
   static_cast<const pcl::I&>( **this ).GetColumn( min, max, rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Obtains the extreme values among a subset of pixel samples of this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::GetExtremeSampleValues()
    */
   template <typename T>
   void GetExtremeSampleValues( T& min, T& max, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                int maxProcessors = 0 ) const
   {
      PCL_PRECONDITION( buffer != 0 )
      if ( *this )
         SOLVE_TEMPLATE( __GET_EXTREME_SAMPLE_VALUES )
   }

#undef __GET_EXTREME_SAMPLE_VALUES

   // -------------------------------------------------------------------------

#define __GET_INTENSITY_1( I ) \
   ImageVariant::GetIntensity( Y, static_cast<const pcl::I&>( **this ), rect, maxProcessors )

#define __GET_INTENSITY_2( I ) \
   image.GetIntensity( static_cast<pcl::I&>( *Y ), rect, maxProcessors )

private:

   template <class P> static
   void GetIntensity( ImageVariant& Y, const GenericImage<P>& image, const Rect& rect, int maxProcessors )
   {
      if ( !Y.IsComplexSample() )
         SOLVE_TEMPLATE_REAL_2( Y, __GET_INTENSITY_2 )
   }

public:

   /*!
    * Computes the intensity HSI component for a subset of pixel samples and
    * stores it in a grayscale image.
    *
    * \param[out] Y  Reference to a destination %ImageVariant where the
    *                computed intensity component will be stored. If this
    *                object does not transport an image, a new 32-bit floating
    *                point image will be created.
    *
    * This member function is a generalized wrapper for
    * GenericImage::GetIntensity().
    */
   void GetIntensity( ImageVariant& Y, const Rect& rect = Rect( 0 ), int maxProcessors = 0 ) const
   {
      if ( *this )
         if ( !IsComplexSample() )
         {
            if ( !Y )
               Y.CreateFloatImage( 32 );

            SOLVE_TEMPLATE_REAL( __GET_INTENSITY_1 )
         }
   }

#undef __GET_INTENSITY_1
#undef __GET_INTENSITY_2

   // -------------------------------------------------------------------------

#define __GET_LIGHTNESS_1( I ) \
   ImageVariant::GetLightness( L, static_cast<const pcl::I&>( **this ), rect, maxProcessors )

#define __GET_LIGHTNESS_2( I ) \
   image.GetLightness( static_cast<pcl::I&>( *L ), rect, maxProcessors )

private:

   template <class P> static
   void GetLightness( ImageVariant& L, const GenericImage<P>& image, const Rect& rect, int maxProcessors )
   {
      if ( !L.IsComplexSample() )
         SOLVE_TEMPLATE_REAL_2( L, __GET_LIGHTNESS_2 )
   }

public:

   /*!
    * Computes the CIE L* component for a subset of pixel samples and stores it
    * in a grayscale image.
    *
    * \param[out] L  Reference to a destination %ImageVariant where the
    *                computed CIE L* component will be stored. If this object
    *                does not transport an image, a new floating point image
    *                will be created with the appropriate sample type (either
    *                32-bit or 64-bit floating point, depending on the sample
    *                data type of this image).
    *
    * This member function is a generalized wrapper for
    * GenericImage::GetLightness().
    */
   void GetLightness( ImageVariant& L, const Rect& rect = Rect( 0 ), int maxProcessors = 0 ) const
   {
      if ( *this )
         if ( !IsComplexSample() )
         {
            if ( !L )
               L.CreateFloatImage( ( BitsPerSample() < 32 ) ? 32 : ( IsFloatSample() ? BitsPerSample() : 64 ) );

            SOLVE_TEMPLATE_REAL( __GET_LIGHTNESS_1 )
         }
   }

#undef __GET_LIGHTNESS_1
#undef __GET_LIGHTNESS_2

   // -------------------------------------------------------------------------

#define __GET_LUMINANCE_1( I ) \
   ImageVariant::GetLuminance( Y, static_cast<const pcl::I&>( **this ), rect, maxProcessors )

#define __GET_LUMINANCE_2( I ) \
   image.GetLuminance( static_cast<pcl::I&>( *Y ), rect, maxProcessors )

private:

   template <class P> static
   void GetLuminance( ImageVariant& Y, const GenericImage<P>& image, const Rect& rect, int maxProcessors )
   {
      if ( !Y.IsComplexSample() )
         SOLVE_TEMPLATE_REAL_2( Y, __GET_LUMINANCE_2 )
   }

public:

   /*!
    * Computes the CIE Y component for a subset of pixel samples and stores it
    * in a grayscale image.
    *
    * \param[out] Y  Reference to a destination %ImageVariant where the
    *                computed CIE Y component will be stored. If this object
    *                does not transport an image, a new floating point image
    *                will be created with the appropriate sample type (either
    *                32-bit or 64-bit floating point, depending on the sample
    *                data type of this image).
    *
    * This member function is a generalized wrapper for
    * GenericImage::GetLuminance().
    */
   void GetLuminance( ImageVariant& Y, const Rect& rect = Rect( 0 ), int maxProcessors = 0 ) const
   {
      if ( *this )
         if ( !IsComplexSample() )
         {
            if ( !Y )
               Y.CreateFloatImage( ( BitsPerSample() < 32 ) ? 32 : ( IsFloatSample() ? BitsPerSample() : 64 ) );

            SOLVE_TEMPLATE_REAL( __GET_LUMINANCE_1 )
         }
   }

#undef __GET_LUMINANCE_1
#undef __GET_LUMINANCE_2

   // -------------------------------------------------------------------------

#define __GET_ROW( I ) \
   static_cast<const pcl::I&>( **this ).GetRow( buffer, y, channel )

   /*!
    * Stores a duplicate of a row of pixel samples in the specified array,
    * with implicit data type conversion.
    *
    * This member function is a generalized wrapper for GenericImage::GetRow()
    */
   template <typename T>
   void GetRow( T* buffer, int y, int channel = -1 ) const
   {
      PCL_PRECONDITION( buffer != 0 )
      if ( *this )
         SOLVE_TEMPLATE( __GET_ROW )
   }

#undef __GET_ROW

   // -------------------------------------------------------------------------

#define __INVERT( I ) \
   static_cast<pcl::I&>( **this ).Invert( scalar, rect, firstChannel, lastChannel )

   /*!
    * Inverts a subset of pixel samples with respect to the specified
    * \a scalar. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Invert()
    */
   template <typename T>
   ImageVariant& Invert( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __INVERT )
      return *this;
   }

#undef __INVERT

   // -------------------------------------------------------------------------

#define __INVERT( I ) \
   static_cast<pcl::I&>( **this ).Invert( rect, firstChannel, lastChannel )

   /*!
    * Inverts a subset of pixel samples with respect to the maximum sample
    * value in the native range of the image. Returns a reference to this
    * image.
    *
    * This member function is a generalized wrapper for GenericImage::Invert()
    */
   ImageVariant& Invert( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __INVERT )
      return *this;
   }

#undef __INVERT

   // -------------------------------------------------------------------------

#define __INVERTED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Inverted( scalar, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * inverted with respect to the specified \a scalar.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Inverted()
    */
   template <typename T>
   ImageVariant Inverted( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __INVERTED )
      return result;
   }

#undef __INVERTED

   // -------------------------------------------------------------------------

#define __INVERTED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Inverted( rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * inverted with respect to the maximum sample value in the native range of
    * the image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Inverted()
    */
   ImageVariant Inverted( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __INVERTED )
      return result;
   }

#undef __INVERTED

   // -------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#define __IS_SHARED_IMAGE( I ) \
   result = static_cast<const pcl::I&>( **this ).IsShared()

   /*!
    * Returns true iff this %ImageVariant object transports a shared image.
    * Returns false if the transported image is local, or if this object
    * transports no image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::IsShared()
    */
   bool IsSharedImage() const
   {
      bool result = false;
      if ( *this )
         SOLVE_TEMPLATE( __IS_SHARED_IMAGE )
      return result;
   }

#undef __IS_SHARED_IMAGE

#endif   // !__PCL_BUILDING_PIXINSIGHT_APPLICATION

   // -------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#define __SHARED_IMAGE_HANDLE( I ) \
   handle = static_cast<const pcl::I&>( **this ).Allocator().Handle()

   /*!
    * Returns the handle to the shared image transported by this %ImageVariant
    * object, or zero (a null pointer) if the transported image is local, or if
    * this object transports no image.
    */
   void* SharedImageHandle() const
   {
      void* handle = 0;
      if ( *this )
         SOLVE_TEMPLATE( __SHARED_IMAGE_HANDLE )
      return handle;
   }

#undef __SHARED_IMAGE_HANDLE

#endif   // !__PCL_BUILDING_PIXINSIGHT_APPLICATION

   // -------------------------------------------------------------------------

#define __IS_UNIQUE_IMAGE( I ) \
   result = static_cast<const pcl::I&>( **this ).IsUnique()

   /*!
    * Returns true iff this %ImageVariant object transports an image that
    * uniquely references its pixel data. Returns false if the transported
    * image is sharing its pixel data with other instances, or if this object
    * transports no image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::IsUnique()
    */
   bool IsUniqueImage() const
   {
      bool result = false;
      if ( *this )
         SOLVE_TEMPLATE( __IS_UNIQUE_IMAGE )
      return result;
   }

#undef __IS_UNIQUE_IMAGE

   // -------------------------------------------------------------------------

#define __LOCATE_EXTREME_SAMPLE_VALUES( I ) \
   static_cast<const pcl::I&>( **this ).LocateExtremeSampleValues( xmin, ymin, min, xmax, ymax, max, rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Obtains the extreme values, and the image coordinates of their first
    * occurrences, among a subset of pixel samples of this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::LocateExtremeSampleValues()
    */
   template <typename T>
   void LocateExtremeSampleValues( int& xmin, int& ymin, T& min,
                                   int& xmax, int& ymax, T& max,
                                   const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                   int maxProcessors = 0 ) const
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __LOCATE_EXTREME_SAMPLE_VALUES )
   }

#undef __LOCATE_EXTREME_SAMPLE_VALUES

   // -------------------------------------------------------------------------

#define __LOCATE_EXTREME_SAMPLE_VALUES( I ) \
   static_cast<const pcl::I&>( **this ).LocateExtremeSampleValues( pmin, min, pmax, max, rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Obtains the extreme values, and the image coordinates of their first
    * occurrences, among a subset of pixel samples of this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::LocateExtremeSampleValues()
    */
   template <typename T>
   void LocateExtremeSampleValues( Point& pmin, T& min,
                                   Point& pmax, T& max,
                                   const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                   int maxProcessors = 0 ) const
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __LOCATE_EXTREME_SAMPLE_VALUES )
   }

#undef __LOCATE_EXTREME_SAMPLE_VALUES

   // -------------------------------------------------------------------------

#define __LOCATE_MAXIMUM_SAMPLE_VALUE( I ) \
   pcl::I::pixel_traits::FromSample( result, static_cast<const pcl::I&>( **this ).LocateMaximumSampleValue( xmax, ymax, rect, firstChannel, lastChannel, maxProcessors ) )

   /*!
    * Returns the maximum value among a subset of pixel samples of this image,
    * and obtains the image coordinates of its first occurrence.
    *
    * This member function is a generalized wrapper for
    * GenericImage::LocateMaximumSampleValue()
    */
   double LocateMaximumSampleValue( int& xmax, int& ymax, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                    int maxProcessors = 0 ) const
   {
      double result = 0;
      if ( *this )
         SOLVE_TEMPLATE( __LOCATE_MAXIMUM_SAMPLE_VALUE )
      return result;
   }

#undef __LOCATE_MAXIMUM_SAMPLE_VALUE

   // -------------------------------------------------------------------------

#define __LOCATE_MAXIMUM_SAMPLE_VALUE( I ) \
   pcl::I::pixel_traits::FromSample( result, static_cast<const pcl::I&>( **this ).LocateMaximumSampleValue( pmax, rect, firstChannel, lastChannel, maxProcessors ) )

   /*!
    * Returns the maximum value among a subset of pixel samples of this image,
    * and obtains the image coordinates of its first occurrence.
    *
    * This member function is a generalized wrapper for
    * GenericImage::LocateMaximumSampleValue()
    */
   double LocateMaximumSampleValue( Point& pmax, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                    int maxProcessors = 0 ) const
   {
      double result = 0;
      if ( *this )
         SOLVE_TEMPLATE( __LOCATE_MAXIMUM_SAMPLE_VALUE )
      return result;
   }

#undef __LOCATE_MAXIMUM_SAMPLE_VALUE

   // -------------------------------------------------------------------------

#define __LOCATE_MINIMUM_SAMPLE_VALUE( I ) \
   pcl::I::pixel_traits::FromSample( result, static_cast<const pcl::I&>( **this ).LocateMinimumSampleValue( xmin, ymin, rect, firstChannel, lastChannel, maxProcessors ) )

   /*!
    * Returns the minimum value among a subset of pixel samples of this image,
    * and obtains the image coordinates of its first occurrence.
    *
    * This member function is a generalized wrapper for
    * GenericImage::LocateMinimumSampleValue()
    */
   double LocateMinimumSampleValue( int& xmin, int& ymin,
                                    const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                    int maxProcessors = 0 ) const
   {
      double result = 0;
      if ( *this )
         SOLVE_TEMPLATE( __LOCATE_MINIMUM_SAMPLE_VALUE )
      return result;
   }

#undef __LOCATE_MINIMUM_SAMPLE_VALUE

   // -------------------------------------------------------------------------

#define __LOCATE_MINIMUM_SAMPLE_VALUE( I ) \
   pcl::I::pixel_traits::FromSample( result, static_cast<const pcl::I&>( **this ).LocateMinimumSampleValue( pmin, rect, firstChannel, lastChannel, maxProcessors ) )

   /*!
    * Returns the minimum value among a subset of pixel samples of this image,
    * and obtains the image coordinates of its first occurrence.
    *
    * This member function is a generalized wrapper for
    * GenericImage::LocateMinimumSampleValue()
    */
   double LocateMinimumSampleValue( Point& pmin, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1,
                                    int maxProcessors = 0 ) const
   {
      double result = 0;
      if ( *this )
         SOLVE_TEMPLATE( __LOCATE_MINIMUM_SAMPLE_VALUE )
      return result;
   }

#undef __LOCATE_MINIMUM_SAMPLE_VALUE

   // -------------------------------------------------------------------------

#define __MAD( I ) \
   result = static_cast<const pcl::I&>( **this ).MAD( center, rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Returns the median absolute deviation (MAD) of a subset of pixel samples
    * with respect to the specified \a center value.
    *
    * This member function is a generalized wrapper for GenericImage::MAD()
    */
   double MAD( double center, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      double result = false;
      if ( *this )
         SOLVE_TEMPLATE( __MAD )
      return result;
   }

#undef __MAD

   // -------------------------------------------------------------------------

#define __MAXIMUM( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Maximum( scalar, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * computes the maximum of each sample and the specified \a scalar. Returns
    * the resulting image.
    *
    * This member function is a generalized wrapper for GenericImage::Maximum()
    */
   template <typename T>
   ImageVariant Maximum( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __MAXIMUM )
      return result;
   }

#undef __MAXIMUM

   // -------------------------------------------------------------------------

#define __MAXIMUM_SAMPLE_VALUE( I ) \
   pcl::I::pixel_traits::FromSample( result, static_cast<const pcl::I&>( **this ).MaximumSampleValue( rect, firstChannel, lastChannel, maxProcessors ) )

   /*!
    * Returns the maximum value among a subset of pixel samples of this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::MaximumSampleValue()
    */
   double MaximumSampleValue( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      double result = 0;
      if ( *this )
         SOLVE_TEMPLATE( __MAXIMUM_SAMPLE_VALUE )
      return result;
   }

#undef __MAXIMUM_SAMPLE_VALUE

   // -------------------------------------------------------------------------

#define __MEAN( I ) \
   result = static_cast<const pcl::I&>( **this ).Mean( rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Returns the arithmetic mean of a subset of pixel samples.
    *
    * This member function is a generalized wrapper for GenericImage::Mean()
    */
   double Mean( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      double result = 0;
      if ( *this )
         SOLVE_TEMPLATE( __MEAN )
      return result;
   }

#undef __MEAN

   // -------------------------------------------------------------------------

#define __MEAN_OF_SQUARES( I ) \
   result = static_cast<const pcl::I&>( **this ).MeanOfSquares( rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Returns the mean of the squares of a subset of pixel samples.
    *
    * This member function is a generalized wrapper for
    * GenericImage::MeanOfSquares()
    */
   double MeanOfSquares( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      double result = 0;
      if ( *this )
         SOLVE_TEMPLATE( __MEAN_OF_SQUARES )
      return result;
   }

#undef __MEAN_OF_SQUARES

   // -------------------------------------------------------------------------

#define __MEDIAN( I ) \
   result = static_cast<const pcl::I&>( **this ).Median( rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Returns the median of a subset of pixel samples.
    *
    * This member function is a generalized wrapper for GenericImage::Median()
    */
   double Median( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      double result = 0;
      if ( *this )
         SOLVE_TEMPLATE( __MEDIAN )
      return result;
   }

#undef __MEDIAN

   // -------------------------------------------------------------------------

#define __MINIMUM( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Minimum( scalar, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * computes the minimum of each sample and the specified \a scalar. Returns
    * the resulting image.
    *
    * This member function is a generalized wrapper for GenericImage::Minimum()
    */
   template <typename T>
   ImageVariant Minimum( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __MINIMUM )
      return result;
   }

#undef __MINIMUM

   // -------------------------------------------------------------------------

#define __MINIMUM_SAMPLE_VALUE( I ) \
   pcl::I::pixel_traits::FromSample( result, static_cast<const pcl::I&>( **this ).MinimumSampleValue( rect, firstChannel, lastChannel, maxProcessors ) )

   /*!
    * Returns the minimum value among a subset of pixel samples of this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::MinimumSampleValue()
    */
   double MinimumSampleValue( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      double result = 0;
      if ( *this )
         SOLVE_TEMPLATE( __MINIMUM_SAMPLE_VALUE )
      return result;
   }

#undef __MINIMUM_SAMPLE_VALUE

   // -------------------------------------------------------------------------

#define __MODULUS( I ) \
   result = static_cast<const pcl::I&>( **this ).Modulus( rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Returns the modulus of a subset of pixel samples. The modulus is the sum
    * of the absolute values of all selected pixel sample values.
    *
    * This member function is a generalized wrapper for GenericImage::Modulus()
    */
   double Modulus( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      double result = 0;
      if ( *this )
         SOLVE_TEMPLATE( __MODULUS )
      return result;
   }

#undef __MODULUS

   // -------------------------------------------------------------------------

#define __MOVE( I ) \
   static_cast<pcl::I&>( **this ).Move( scalar, rect, firstChannel, lastChannel )

   /*!
    * Replaces a subset of pixel samples in this image with the specified
    * constant \a scalar. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Move()
    */
   template <typename T>
   ImageVariant& Move( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE( __MOVE )
      return *this;
   }

#undef __MOVE

   /*!
    * A synonym for Move().
    */
   template <typename T>
   ImageVariant& Mov( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Move( scalar, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

#define __MOVE_1( I ) \
   ImageVariant::Move( static_cast<pcl::I&>( **this ), image, point, channel, rect, firstChannel, lastChannel )

#define __MOVE_2( I ) \
   image1.Move( static_cast<const pcl::I&>( *image2 ), point, channel, rect, firstChannel, lastChannel )

private:

   template <class P> static
   void Move( GenericImage<P>& image1, const ImageVariant& image2,
             const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_2( image2, __MOVE_2 )
   }

public:

   /*!
    * Replaces a subset of pixel samples in this image with samples from the
    * specified source \a image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Move()
    */
   ImageVariant& Move( const ImageVariant& image,
                       const Point& point = Point( int_max ), int channel = -1,
                       const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __MOVE_1 )
      return *this;
   }

#undef __MOVE_1
#undef __MOVE_2

   ImageVariant& Mov( const ImageVariant& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Move( image, point, channel, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

#define __MULTIPLY( I ) \
   static_cast<pcl::I&>( **this ).Multiply( scalar, rect, firstChannel, lastChannel )

   /*!
    * Multiplies a subset of pixel samples in this image by the specified
    * \a scalar. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Multiply()
    */
   template <typename T>
   ImageVariant& Multiply( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE( __MULTIPLY )
      return *this;
   }

#undef __MULTIPLY

   /*!
    * A synonym for Multiply().
    */
   template <typename T>
   ImageVariant& Mul( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Multiply( scalar, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

#define __MULTIPLY_1( I ) \
   ImageVariant::Multiply( static_cast<pcl::I&>( **this ), image, point, channel, rect, firstChannel, lastChannel )

#define __MULTIPLY_2( I ) \
   image1.Multiply( static_cast<const pcl::I&>( *image2 ), point, channel, rect, firstChannel, lastChannel )

private:

   template <class P> static
   void Multiply( GenericImage<P>& image1, const ImageVariant& image2,
             const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_2( image2, __MULTIPLY_2 )
   }

public:

   /*!
    * Multiplies a subset of pixel samples in this image by samples from the
    * specified source \a image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Multiply()
    */
   ImageVariant& Multiply( const ImageVariant& image,
                           const Point& point = Point( int_max ), int channel = -1,
                           const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __MULTIPLY_1 )
      return *this;
   }

#undef __MULTIPLY_1
#undef __MULTIPLY_2

   /*!
    * A synonym for Multiply().
    */
   ImageVariant& Mul( const ImageVariant& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Multiply( image, point, channel, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

#define __MULTIPLIED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Multiplied( scalar, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * multiplies it by a \a scalar. Returns the resulting image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Multiplied()
    */
   template <typename T>
   ImageVariant Multiplied( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __MULTIPLIED )
      return result;
   }

#undef __MULTIPLIED

   // -------------------------------------------------------------------------

#define __NAND( I ) \
   static_cast<pcl::I&>( **this ).Nand( scalar, rect, firstChannel, lastChannel )

   /*!
    * Performs a bitwise NAND (NOT AND) operation between a subset of pixel
    * samples in this image and the specified \a scalar. Returns a reference to
    * this image.
    *
    * This member function is a generalized wrapper for GenericImage::Nand()
    */
   template <typename T>
   ImageVariant& Nand( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __NAND )
      return *this;
   }

#undef __NAND

   // -------------------------------------------------------------------------

#define __NAND_1( I ) \
   ImageVariant::Nand( static_cast<pcl::I&>( **this ), image, point, channel, rect, firstChannel, lastChannel )

#define __NAND_2( I ) \
   image1.Nand( static_cast<const pcl::I&>( *image2 ), point, channel, rect, firstChannel, lastChannel )

private:

   template <class P> static
   void Nand( GenericImage<P>& image1, const ImageVariant& image2,
             const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_REAL_2( image2, __NAND_2 )
   }

public:

   /*!
    * Performs a bitwise NAND (NOT AND) operation between a subset of pixel
    * samples in this image and the corresponding samples of the specified
    * source \a image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Nand()
    */
   ImageVariant& Nand( const ImageVariant& image,
                       const Point& point = Point( int_max ), int channel = -1,
                       const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE_REAL( __NAND_1 )
      return *this;
   }

#undef __NAND_1
#undef __NAND_2

   // -------------------------------------------------------------------------

#define __NOR( I ) \
   static_cast<pcl::I&>( **this ).Nor( scalar, rect, firstChannel, lastChannel )

   /*!
    * Performs a bitwise NOR (NOT inclusive OR) operation between a subset of
    * pixel samples in this image and the specified \a scalar. Returns a
    * reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Nor()
    */
   template <typename T>
   ImageVariant& Nor( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __NOR )
      return *this;
   }

#undef __NOR

   // -------------------------------------------------------------------------

#define __NOR_1( I ) \
   ImageVariant::Nor( static_cast<pcl::I&>( **this ), image, point, channel, rect, firstChannel, lastChannel )

#define __NOR_2( I ) \
   image1.Nor( static_cast<const pcl::I&>( *image2 ), point, channel, rect, firstChannel, lastChannel )

private:

   template <class P> static
   void Nor( GenericImage<P>& image1, const ImageVariant& image2,
             const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_REAL_2( image2, __NOR_2 )
   }

public:

   /*!
    * Performs a bitwise NOR (NOT inclusive OR) operation between a subset of
    * pixel samples in this image and the corresponding samples of the
    * specified source \a image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Nor()
    */
   ImageVariant& Nor( const ImageVariant& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE_REAL( __NOR_1 )
      return *this;
   }

#undef __NOR_1
#undef __NOR_2

   // -------------------------------------------------------------------------

#define __NORM( I ) \
   result = static_cast<const pcl::I&>( **this ).Norm( rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Returns the norm of a subset of pixel samples. The norm is the sum of all
    * selected pixel sample values.
    *
    * This member function is a generalized wrapper for GenericImage::Norm()
    */
   double Norm( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      double result = 0;
      if ( *this )
         SOLVE_TEMPLATE( __NORM )
      return result;
   }

#undef __NORM

   // -------------------------------------------------------------------------

#define __NORMALIZE( I ) \
   static_cast<pcl::I&>( **this ).Normalize( lowerBound, upperBound, rect, firstChannel, lastChannel )

   /*!
    * Normalizes a subset of pixel samples to the specified range of values.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Normalize()
    */
   template <typename T>
   ImageVariant& Normalize( T lowerBound, T upperBound, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __NORMALIZE )
      return *this;
   }

#undef __NORMALIZE

   // -------------------------------------------------------------------------

#define __NORMALIZE( I ) \
   static_cast<pcl::I&>( **this ).Normalize( rect, firstChannel, lastChannel )

   /*!
    * Normalizes a subset of pixel samples to the native range of the image.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Normalize()
    */
   ImageVariant& Normalize( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __NORMALIZE )
      return *this;
   }

#undef __NORMALIZE

   // -------------------------------------------------------------------------

#define __NORMALIZED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Normalized( lowerBound, upperBound, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * normalized to the specified range of values.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Normalized()
    */
   template <typename T>
   ImageVariant Normalized( T lowerBound, T upperBound, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE_REAL( __NORMALIZED )
      return result;
   }

#undef __NORMALIZED

   // -------------------------------------------------------------------------

#define __NORMALIZED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Normalized( rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * normalized to the native range of the image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Normalized()
    */
   ImageVariant Normalized( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE_REAL( __NORMALIZED )
      return result;
   }

#undef __NORMALIZED

   // -------------------------------------------------------------------------

#define __NOT( I ) \
   static_cast<pcl::I&>( **this ).Not( rect, firstChannel, lastChannel )

   /*!
    * Replaces a subset of pixel samples with their bitwise logical NOT values.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Not()
    */
   ImageVariant& Not( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __NOT )
      return *this;
   }

#undef __NOT

   // -------------------------------------------------------------------------

#define __ONE( I ) \
   static_cast<pcl::I&>( **this ).One( rect, firstChannel, lastChannel )

   /*!
    * Fills a subset of pixel samples with the constant value resulting from
    * converting the scalar one (1) to the sample data type of the image.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::One()
    */
   ImageVariant& One( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __ONE )
      return *this;
   }

#undef __ONE

   // -------------------------------------------------------------------------

#define __MULTIPLY( I ) \
   static_cast<pcl::I&>( **this ) *= scalar

   /*!
    * A synonym for Multiply().
    */
   template <typename T>
   ImageVariant& operator *=( T scalar )
   {
      if ( *this )
         SOLVE_TEMPLATE( __MULTIPLY )
      return *this;
   }

#undef __MULTIPLY

   // -------------------------------------------------------------------------

#define __MULTIPLY_1( I ) \
   ImageVariant::Multiply( static_cast<pcl::I&>( **this ), image )

#define __MULTIPLY_2( I ) \
   image1 *= static_cast<const pcl::I&>( *image2 )

private:

   template <class P> static
   void Multiply( GenericImage<P>& image1, const ImageVariant& image2 )
   {
      SOLVE_TEMPLATE_2( image2, __MULTIPLY_2 )
   }

public:

   /*!
    * A synonym for Multiply().
    */
   ImageVariant& operator *=( const ImageVariant& image )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __MULTIPLY_1 )
      return *this;
   }

#undef __MULTIPLY_1
#undef __MULTIPLY_2

   // -------------------------------------------------------------------------

#define __ADD( I ) \
   static_cast<pcl::I&>( **this ) += scalar

   /*!
    * A synonym for Add().
    */
   template <typename T>
   ImageVariant& operator +=( T scalar )
   {
      if ( *this )
         SOLVE_TEMPLATE( __ADD )
      return *this;
   }

#undef __ADD

   // -------------------------------------------------------------------------

#define __ADD_1( I ) \
   ImageVariant::Add( static_cast<pcl::I&>( **this ), image )

#define __ADD_2( I ) \
   image1 += static_cast<const pcl::I&>( *image2 )

private:

   template <class P> static
   void Add( GenericImage<P>& image1, const ImageVariant& image2 )
   {
      SOLVE_TEMPLATE_2( image2, __ADD_2 )
   }

public:

   /*!
    * A synonym for Add().
    */
   ImageVariant& operator +=( const ImageVariant& image )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __ADD_1 )
      return *this;
   }

#undef __ADD_1
#undef __ADD_2

   // -------------------------------------------------------------------------

#define __SUBTRACT( I ) \
   static_cast<pcl::I&>( **this ) -= scalar

   /*!
    * A synonym for Subtract().
    */
   template <typename T>
   ImageVariant& operator -=( T scalar )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SUBTRACT )
      return *this;
   }

#undef __SUBTRACT

   // -------------------------------------------------------------------------

#define __SUBTRACT_1( I ) \
   ImageVariant::Subtract( static_cast<pcl::I&>( **this ), image )

#define __SUBTRACT_2( I ) \
   image1 -= static_cast<const pcl::I&>( *image2 )

private:

   template <class P> static
   void Subtract( GenericImage<P>& image1, const ImageVariant& image2 )
   {
      SOLVE_TEMPLATE_2( image2, __SUBTRACT_2 )
   }

public:

   /*!
    * A synonym for Subtract().
    */
   ImageVariant& operator -=( const ImageVariant& image )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __SUBTRACT_1 )
      return *this;
   }

#undef __SUBTRACT_1
#undef __SUBTRACT_2

   // -------------------------------------------------------------------------

#define __DIVIDE( I ) \
   static_cast<pcl::I&>( **this ) /= scalar

   /*!
    * A synonym for Divide().
    */
   template <typename T>
   ImageVariant& operator /=( T scalar )
   {
      if ( *this )
         SOLVE_TEMPLATE( __DIVIDE )
      return *this;
   }

#undef __DIVIDE

   // -------------------------------------------------------------------------

#define __DIVIDE_1( I ) \
   ImageVariant::Divide( static_cast<pcl::I&>( **this ), image )

#define __DIVIDE_2( I ) \
   image1 /= static_cast<const pcl::I&>( *image2 )

private:

   template <class P> static
   void Divide( GenericImage<P>& image1, const ImageVariant& image2 )
   {
      SOLVE_TEMPLATE_2( image2, __DIVIDE_2 )
   }

public:

   /*!
    * A synonym for Divide().
    */
   ImageVariant& operator /=( const ImageVariant& image )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __DIVIDE_1 )
      return *this;
   }

#undef __DIVIDE_1
#undef __DIVIDE_2

   // -------------------------------------------------------------------------

#define __RAISE( I ) \
   static_cast<pcl::I&>( **this ) ^= scalar

   /*!
    * A synonym for Raise().
    */
   template <typename T>
   ImageVariant& operator ^=( T scalar )
   {
      if ( *this )
         SOLVE_TEMPLATE( __RAISE )
      return *this;
   }

#undef __RAISE

   // -------------------------------------------------------------------------

#define __RAISE_1( I ) \
   ImageVariant::Raise( static_cast<pcl::I&>( **this ), image )

#define __RAISE_2( I ) \
   image1 ^= static_cast<const pcl::I&>( *image2 )

private:

   template <class P> static
   void Raise( GenericImage<P>& image1, const ImageVariant& image2 )
   {
      SOLVE_TEMPLATE_2( image2, __RAISE_2 )
   }

public:

   /*!
    * A synonym for Raise().
    */
   ImageVariant& operator ^=( const ImageVariant& image )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __RAISE_1 )
      return *this;
   }

#undef __RAISE_1
#undef __RAISE_2

   // -------------------------------------------------------------------------

#define __INVERTED( I ) \
   result.SetImage( *new pcl::I( ~static_cast<const pcl::I&>( **this ) ) ); \
   result.SetOwnership( true )

   /*!
    * Returns an inverted duplicate of this image. The inversion operation is
    * performed on the current pixel sample selection with respect to the
    * maximum sample value in the native range of the image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::operator ~()
    */
   ImageVariant operator ~() const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __INVERTED )
      return result;
   }

#undef __INVERTED

   // -------------------------------------------------------------------------

#define __OR( I ) \
   static_cast<pcl::I&>( **this ).Or( scalar, rect, firstChannel, lastChannel )

   /*!
    * Performs a bitwise OR (inclusive OR) operation between a subset of pixel
    * samples in this image and the specified \a scalar. Returns a reference to
    * this image.
    *
    * This member function is a generalized wrapper for GenericImage::Or()
    */
   template <typename T>
   ImageVariant& Or( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __OR )
      return *this;
   }

#undef __OR

   // -------------------------------------------------------------------------

#define __OR_1( I ) \
   ImageVariant::Or( static_cast<pcl::I&>( **this ), image, point, channel, rect, firstChannel, lastChannel )

#define __OR_2( I ) \
   image1.Or( static_cast<const pcl::I&>( *image2 ), point, channel, rect, firstChannel, lastChannel )

private:

   template <class P> static
   void Or( GenericImage<P>& image1, const ImageVariant& image2,
            const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_REAL_2( image2, __OR_2 )
   }

public:

   /*!
    * Performs a bitwise OR (inclusive OR) operation between a subset of pixel
    * samples in this image and the corresponding samples of the specified
    * source \a image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Or()
    */
   ImageVariant& Or( const ImageVariant& image,
                     const Point& point = Point( int_max ), int channel = -1,
                     const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE_REAL( __OR_1 )
      return *this;
   }

#undef __OR_1
#undef __OR_2

   // -------------------------------------------------------------------------

#define __RAISE( I ) \
   static_cast<pcl::I&>( **this ).Raise( scalar, rect, firstChannel, lastChannel )

   /*!
    * Raises (exponentiation operator) a subset of pixel samples in this image
    * to the specified \a scalar. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Raise()
    */
   template <typename T>
   ImageVariant& Raise( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE( __RAISE )
      return *this;
   }

#undef __RAISE

   /*!
    * A synonym for Raise().
    */
   template <typename T>
   ImageVariant& Pow( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Raise( scalar, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

#define __Qn( I ) \
   result = static_cast<const pcl::I&>( **this ).Qn( rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Returns the Qn scale estimator of Rousseeuw and Croux for a subset of
    * pixel samples.
    *
    * This member function is a generalized wrapper for GenericImage::Qn()
    */
   double Qn( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      double result = false;
      if ( *this )
         SOLVE_TEMPLATE( __Qn )
      return result;
   }

#undef __Qn

   // -------------------------------------------------------------------------

#define __RAISE_1( I ) \
   ImageVariant::Raise( static_cast<pcl::I&>( **this ), image, point, channel, rect, firstChannel, lastChannel )

#define __RAISE_2( I ) \
   image1.Raise( static_cast<const pcl::I&>( *image2 ), point, channel, rect, firstChannel, lastChannel )

private:

   template <class P> static
   void Raise( GenericImage<P>& image1, const ImageVariant& image2,
             const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_2( image2, __RAISE_2 )
   }

public:

   /*!
    * Raises (exponentiation operator) a subset of pixel samples in this image
    * to samples from the specified source \a image. Returns a reference to
    * this image.
    *
    * This member function is a generalized wrapper for GenericImage::Raise()
    */
   ImageVariant& Raise( const ImageVariant& image,
                        const Point& point = Point( int_max ), int channel = -1,
                        const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __RAISE_1 )
      return *this;
   }

#undef __RAISE_1
#undef __RAISE_2

   /*!
    * A synonym for Raise().
    */
   ImageVariant& Pow( const ImageVariant& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Raise( image, point, channel, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

#define __RAISED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Raised( scalar, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * raises it to a \a scalar. Returns the resulting image.
    *
    * This member function is a generalized wrapper for GenericImage::Raised()
    */
   template <typename T>
   ImageVariant Raised( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __RAISED )
      return result;
   }

#undef __RAISED

   // -------------------------------------------------------------------------

#define __READ( I ) \
   static_cast<pcl::I&>( **this ).Read( file )

   /*!
    * Reads an image from a raw-storage stream. Returns a reference to this
    * image.
    *
    * This member function is a generalized wrapper for GenericImage::Read()
    */
   ImageVariant& Read( File& file )
   {
      if ( *this )
         SOLVE_TEMPLATE( __READ )
      return *this;
   }

#undef __READ

   // -------------------------------------------------------------------------

#define __READ( I ) \
   static_cast<pcl::I&>( **this ).Read( filePath )

   /*!
    * Reads an image from a raw-storage file. Returns a reference to this
    * image.
    *
    * This member function is a generalized wrapper for GenericImage::Read()
    */
   ImageVariant& Read( const String& filePath )
   {
      if ( *this )
         SOLVE_TEMPLATE( __READ )
      return *this;
   }

#undef __READ

   // -------------------------------------------------------------------------

#define __RESCALE( I ) \
   static_cast<pcl::I&>( **this ).Rescale( lowerBound, upperBound, rect, firstChannel, lastChannel )

   /*!
    * Rescales a subset of pixel samples to the specified range of values.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Rescale()
    */
   template <typename T>
   ImageVariant& Rescale( T lowerBound, T upperBound, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __RESCALE )
      return *this;
   }

#undef __RESCALE

   // -------------------------------------------------------------------------

#define __RESCALE( I ) \
   static_cast<pcl::I&>( **this ).Rescale( rect, firstChannel, lastChannel )

   /*!
    * Rescales a subset of pixel samples to the native range of the image.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Rescale()
    */
   ImageVariant& Rescale( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __RESCALE )
      return *this;
   }

#undef __RESCALE

   // -------------------------------------------------------------------------

#define __RESCALED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Rescaled( lowerBound, upperBound, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * rescaled to the specified range of values.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Rescaled()
    */
   template <typename T>
   ImageVariant Rescaled( T lowerBound, T upperBound, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE_REAL( __RESCALED )
      return result;
   }

#undef __RESCALED

   // -------------------------------------------------------------------------

#define __RESCALED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Rescaled( rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * rescaled to the native range of the image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Rescaled()
    */
   ImageVariant Rescaled( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE_REAL( __RESCALED )
      return result;
   }

#undef __RESCALED

   // -------------------------------------------------------------------------

#define __SET_ABSOLUTE_DIFFERENCE( I ) \
   static_cast<pcl::I&>( **this ).SetAbsoluteDifference( scalar, rect, firstChannel, lastChannel )

   /*!
    * Replaces a subset of pixel samples in this image with the absolute
    * values of their differences with the specified \a scalar. Returns a
    * reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::SetAbsoluteDifference()
    */
   template <typename T>
   ImageVariant& SetAbsoluteDifference( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SET_ABSOLUTE_DIFFERENCE )
      return *this;
   }

#undef __SET_ABSOLUTE_DIFFERENCE

   /*!
    * A synonym for SetAbsoluteDifference().
    */
   template <typename T>
   ImageVariant& Dif( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return SetAbsoluteDifference( scalar, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

#define __SET_ABSOLUTE_DIFFERENCE_1( I ) \
   ImageVariant::SetAbsoluteDifference( static_cast<pcl::I&>( **this ), image, point, channel, rect, firstChannel, lastChannel )

#define __SET_ABSOLUTE_DIFFERENCE_2( I ) \
   image1.SetAbsoluteDifference( static_cast<const pcl::I&>( *image2 ), point, channel, rect, firstChannel, lastChannel )

private:

   template <class P> static
   void SetAbsoluteDifference( GenericImage<P>& image1, const ImageVariant& image2,
                               const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_2( image2, __SET_ABSOLUTE_DIFFERENCE_2 )
   }

public:

   /*!
    * Replaces a subset of pixel samples in this image with the absolute values
    * of their differences with samples from the specified source \a image.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::SetAbsoluteDifference()
    */
   ImageVariant& SetAbsoluteDifference( const ImageVariant& image,
                                        const Point& point = Point( int_max ), int channel = -1,
                                        const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __SET_ABSOLUTE_DIFFERENCE_1 )
      return *this;
   }

#undef __SET_ABSOLUTE_DIFFERENCE_1
#undef __SET_ABSOLUTE_DIFFERENCE_2

   ImageVariant& Dif( const ImageVariant& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return SetAbsoluteDifference( image, point, channel, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

#define __SET_ABSOLUTE_VALUE( I ) \
   static_cast<pcl::I&>( **this ).SetAbsoluteValue( rect, firstChannel, lastChannel )

   /*!
    * Replaces a subset of pixel samples with their absolute values. Returns a
    * reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::SetAbsoluteValue()
    */
   ImageVariant& SetAbsoluteValue( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SET_ABSOLUTE_VALUE )
      return *this;
   }

   ImageVariant& Abs( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return SetAbsoluteValue( rect, firstChannel, lastChannel );
   }

#undef __SET_ABSOLUTE_VALUE

   // -------------------------------------------------------------------------

#define __SET_COLOR_SPACE( I ) \
   static_cast<pcl::I&>( **this ).SetColorSpace( colorSpace, maxProcessors )

   /*!
    * Converts the image to the specified color space. Returns a reference to
    * this object.
    *
    * This member function is a generalized wrapper for
    * GenericImage::SetColorSpace()
    */
   ImageVariant& SetColorSpace( color_space colorSpace, int maxProcessors = 0 )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SET_COLOR_SPACE )
      return *this;
   }

#undef __SET_COLOR_SPACE

   // -------------------------------------------------------------------------

#define __SET_COLUMN( I ) \
   static_cast<pcl::I&>( **this ).SetColumn( buffer, x, channel )

   /*!
    * Sets a column of pixel samples from values in the specified array, with
    * implicit data type conversion. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::SetColumn()
    */
   template <typename T>
   ImageVariant& SetColumn( const T* buffer, int x, int channel = -1 )
   {
      PCL_PRECONDITION( buffer != 0 )
      if ( *this )
         SOLVE_TEMPLATE( __SET_COLUMN )
      return *this;
   }

#undef __SET_COLUMN

   // -------------------------------------------------------------------------

#define __SET_LIGHTNESS_1( I ) \
   ImageVariant::SetLightness( static_cast<pcl::I&>( **this ), L, point, rect, maxProcessors )

#define __SET_LIGHTNESS_2( I ) \
   image.SetLightness( static_cast<const pcl::I&>( *L ), point, rect, maxProcessors )

private:

   template <class P> static
   void SetLightness( GenericImage<P>& image, const ImageVariant& L,
                      const Point& point, const Rect& rect, int maxProcessors )
   {
      if ( !L.IsComplexSample() )
         SOLVE_TEMPLATE_REAL_2( L, __SET_LIGHTNESS_2 )
   }

public:

   /*!
    * Replaces the CIE L* component of a subset of pixel samples with data
    * extracted from another image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::SetLightness()
    */
   ImageVariant& SetLightness( const ImageVariant& L,
                               const Point& point = Point( int_max ),
                               const Rect& rect = Rect( 0 ), int maxProcessors = 0 )
   {
      if ( *this )
         if ( !IsComplexSample() )
            if ( L )
               SOLVE_TEMPLATE_REAL( __SET_LIGHTNESS_1 )
      return *this;
   }

#undef __SET_LIGHTNESS_1
#undef __SET_LIGHTNESS_2

   // -------------------------------------------------------------------------

#define __SET_LUMINANCE_1( I ) \
   ImageVariant::SetLuminance( static_cast<pcl::I&>( **this ), Y, point, rect, maxProcessors )

#define __SET_LUMINANCE_2( I ) \
   image.SetLuminance( static_cast<const pcl::I&>( *Y ), point, rect, maxProcessors )

private:

   template <class P> static
   void SetLuminance( GenericImage<P>& image, const ImageVariant& Y,
                      const Point& point, const Rect& rect, int maxProcessors )
   {
      if ( !Y.IsComplexSample() )
         SOLVE_TEMPLATE_REAL_2( Y, __SET_LUMINANCE_2 )
   }

public:

   /*!
    * Replaces the CIE Y component of a subset of pixel samples with data
    * extracted from another image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::SetLuminance()
    */
   ImageVariant& SetLuminance( const ImageVariant& Y,
                               const Point& point = Point( int_max ),
                               const Rect& rect = Rect( 0 ), int maxProcessors = 0 )
   {
      if ( *this )
         if ( !IsComplexSample() )
            if ( Y )
               SOLVE_TEMPLATE_REAL( __SET_LUMINANCE_1 )
      return *this;
   }

#undef __SET_LUMINANCE_1
#undef __SET_LUMINANCE_2

   // -------------------------------------------------------------------------

#define __SET_MAXIMUM( I ) \
   static_cast<pcl::I&>( **this ).SetMaximum( scalar, rect, firstChannel, lastChannel )

   /*!
    * Replaces a subset of pixel samples in this image with the maximum of each
    * sample and the specified \a scalar. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::SetMaximum()
    */
   template <typename T>
   ImageVariant& SetMaximum( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SET_MAXIMUM )
      return *this;
   }

#undef __SET_MAXIMUM

   /*!
    * A synonym for SetMaximum().
    */
   template <typename T>
   ImageVariant& Max( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return SetMaximum( scalar, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

#define __SET_MAXIMUM_1( I ) \
   ImageVariant::SetMaximum( static_cast<pcl::I&>( **this ), image, point, channel, rect, firstChannel, lastChannel )

#define __SET_MAXIMUM_2( I ) \
   image1.SetMaximum( static_cast<const pcl::I&>( *image2 ), point, channel, rect, firstChannel, lastChannel )

private:

   template <class P> static
   void SetMaximum( GenericImage<P>& image1, const ImageVariant& image2,
             const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_2( image2, __SET_MAXIMUM_2 )
   }

public:

   /*!
    * Replaces a subset of pixel samples in this image with the maximum of each
    * target sample and the corresponding source sample of the specified
    * \a image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::SetMaximum()
    */
   ImageVariant& SetMaximum( const ImageVariant& image,
                             const Point& point = Point( int_max ), int channel = -1,
                             const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __SET_MAXIMUM_1 )
      return *this;
   }

#undef __SET_MAXIMUM_1
#undef __SET_MAXIMUM_2

   /*!
    * A synonym for SetMaximum().
    */
   ImageVariant& Max( const ImageVariant& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return SetMaximum( image, point, channel, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

#define __SET_MINIMUM( I ) \
   static_cast<pcl::I&>( **this ).SetMinimum( scalar, rect, firstChannel, lastChannel )

   /*!
    * Replaces a subset of pixel samples in this image with the minimum of each
    * sample and the specified \a scalar. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::SetMinimum()
    */
   template <typename T>
   ImageVariant& SetMinimum( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SET_MINIMUM )
      return *this;
   }

#undef __SET_MINIMUM

   /*!
    * A synonym for SetMinimum().
    */
   template <typename T>
   ImageVariant& Min( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return SetMinimum( scalar, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

#define __SET_MINIMUM_1( I ) \
   ImageVariant::SetMinimum( static_cast<pcl::I&>( **this ), image, point, channel, rect, firstChannel, lastChannel )

#define __SET_MINIMUM_2( I ) \
   image1.SetMinimum( static_cast<const pcl::I&>( *image2 ), point, channel, rect, firstChannel, lastChannel )

private:

   template <class P> static
   void SetMinimum( GenericImage<P>& image1, const ImageVariant& image2,
             const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_2( image2, __SET_MINIMUM_2 )
   }

public:

   /*!
    * Replaces a subset of pixel samples in this image with the minimum of each
    * target sample and the corresponding source sample of the specified
    * \a image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::SetMinimum()
    */
   ImageVariant& SetMinimum( const ImageVariant& image,
                             const Point& point = Point( int_max ), int channel = -1,
                             const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __SET_MINIMUM_1 )
      return *this;
   }

#undef __SET_MINIMUM_1
#undef __SET_MINIMUM_2

   /*!
    * A synonym for SetMinimum().
    */
   ImageVariant& Min( const ImageVariant& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return SetMinimum( image, point, channel, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

#define __Sn( I ) \
   result = static_cast<const pcl::I&>( **this ).Sn( rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Returns the Sn scale estimator of Rousseeuw and Croux for a subset of
    * pixel samples.
    *
    * This member function is a generalized wrapper for GenericImage::Sn()
    */
   double Sn( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      double result = false;
      if ( *this )
         SOLVE_TEMPLATE( __Sn )
      return result;
   }

#undef __Sn

   // -------------------------------------------------------------------------

#define __SET_ROW( I ) \
   static_cast<pcl::I&>( **this ).SetRow( buffer, y, channel )

   /*!
    * Sets a row of pixel samples from values in the specified array, with
    * implicit data type conversion. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::SetRow()
    */
   template <typename T>
   ImageVariant& SetRow( const T* buffer, int y, int channel = -1 )
   {
      PCL_PRECONDITION( buffer != 0 )
      if ( *this )
         SOLVE_TEMPLATE( __SET_ROW )
      return *this;
   }

#undef __SET_ROW

   // -------------------------------------------------------------------------

#define __ENSURE_UNIQUE_IMAGE( I ) \
   static_cast<pcl::I&>( **this ).EnsureUnique()

   /*!
    * Ensures that the image transported by this %ImageVariant object uniquely
    * references its pixel data, making a unique copy of its referenced pixel
    * data if necessary. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::EnsureUnique()
    */
   ImageVariant& EnsureUniqueImage()
   {
      if ( *this )
         SOLVE_TEMPLATE( __ENSURE_UNIQUE_IMAGE )
      return *this;
   }

#undef __ENSURE_UNIQUE_IMAGE

   // -------------------------------------------------------------------------

#define __ENSURE_LOCAL_IMAGE( I ) \
   static_cast<pcl::I&>( **this ).EnsureLocal()

   /*!
    * Ensures that the image transported by this %ImageVariant object uses
    * local pixel data storage. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::EnsureLocal()
    */
   ImageVariant& EnsureLocalImage()
   {
      if ( *this )
         SOLVE_TEMPLATE( __ENSURE_LOCAL_IMAGE )
      return *this;
   }

#undef __ENSURE_LOCAL_IMAGE

   // -------------------------------------------------------------------------

#define __SHIFT( I ) \
   static_cast<pcl::I&>( **this ).Shift( fillValues )

   /*!
    * Shifts (translates) the image to its current anchor point selection.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Shift()
    */
   template <typename T>
   ImageVariant& Shift( const GenericVector<T>& fillValues )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT )
      return *this;
   }

#undef __SHIFT

   // -------------------------------------------------------------------------

#define __SHIFT( I ) \
   static_cast<pcl::I&>( **this ).Shift()

   /*!
    * Shifts (translates) the image to its current anchor point selection,
    * filling extended regions with the minimum sample value in the native
    * range of the image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Shift()
    */
   ImageVariant& Shift()
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT )
      return *this;
   }

#undef __SHIFT

   // -------------------------------------------------------------------------

#define __SHIFT_BY( I ) \
   static_cast<pcl::I&>( **this ).ShiftBy( dx, dy, fillValues )

   /*!
    * Shifts (translates) this image by the specified increments in pixels.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::ShiftBy()
    */
   template <typename T>
   ImageVariant& ShiftBy( int dx, int dy, const GenericVector<T>& fillValues )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT_BY )
      return *this;
   }

#undef __SHIFT_BY

   // -------------------------------------------------------------------------

#define __SHIFT_BY( I ) \
   static_cast<pcl::I&>( **this ).ShiftBy( dx, dy )

   /*!
    * Shifts (translates) this image by the specified increments in pixels,
    * filling extended regions with the minimum sample value in the native
    * range of the image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::ShiftBy()
    */
   ImageVariant& ShiftBy( int dx, int dy )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT_BY )
      return *this;
   }

#undef __SHIFT_BY

   // -------------------------------------------------------------------------

#define __SHIFT_TO( I ) \
   static_cast<pcl::I&>( **this ).ShiftTo( x, y, fillValues )

   /*!
    * Shifts (translates) the image to the specified coordinates. Returns a
    * reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::ShiftTo()
    */
   template <typename T>
   ImageVariant& ShiftTo( int x, int y, const GenericVector<T>& fillValues )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT_TO )
      return *this;
   }

#undef __SHIFT_TO

   // -------------------------------------------------------------------------

#define __SHIFT_TO( I ) \
   static_cast<pcl::I&>( **this ).ShiftTo( x, y )

   /*!
    * Shifts (translates) the image to the specified coordinates, filling
    * extended regions with the minimum sample value in the native range of the
    * image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::ShiftTo()
    */
   ImageVariant& ShiftTo( int x, int y )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT_TO )
      return *this;
   }

#undef __SHIFT_TO

   // -------------------------------------------------------------------------

#define __SHIFT_TO( I ) \
   static_cast<pcl::I&>( **this ).ShiftTo( p, fillValues )

   /*!
    * Shifts (translates) the image to the specified coordinates. Returns a
    * reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::ShiftTo()
    */
   template <typename T>
   ImageVariant& ShiftTo( const Point& p, const GenericVector<T>& fillValues )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT_TO )
      return *this;
   }

#undef __SHIFT_TO

   // -------------------------------------------------------------------------

#define __SHIFT_TO( I ) \
   static_cast<pcl::I&>( **this ).ShiftTo( p )

   /*!
    * Shifts (translates) the image to the specified coordinates, filling
    * extended regions with the minimum sample value in the native range of the
    * image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::ShiftTo()
    */
   ImageVariant& ShiftTo( const Point& p )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT_TO )
      return *this;
   }

#undef __SHIFT_TO

   // -------------------------------------------------------------------------

#define __SHIFT_TO_BOTTOM_LEFT( I ) \
   static_cast<pcl::I&>( **this ).ShiftToBottomLeft( width, height, fillValues )

   /*!
    * Extends or crops the image at its right and top sides to the specified
    * \a width and \a height in pixels. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::ShiftToBottomLeft()
    */
   template <typename T>
   ImageVariant& ShiftToBottomLeft( int width, int height, const GenericVector<T>& fillValues )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT_TO_BOTTOM_LEFT )
      return *this;
   }

#undef __SHIFT_TO_BOTTOM_LEFT

   // -------------------------------------------------------------------------

#define __SHIFT_TO_BOTTOM_LEFT( I ) \
   static_cast<pcl::I&>( **this ).ShiftToBottomLeft( width, height )

   /*!
    * Extends or crops the image at its right and top sides to the specified
    * \a width and \a height in pixels, filling extended regions with the
    * minimum sample value in the native range of the image. Returns a
    * reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::ShiftToBottomLeft()
    */
   ImageVariant& ShiftToBottomLeft( int width, int height )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT_TO_BOTTOM_LEFT )
      return *this;
   }

#undef __SHIFT_TO_BOTTOM_LEFT

   // -------------------------------------------------------------------------

#define __SHIFT_TO_BOTTOM_RIGHT( I ) \
   static_cast<pcl::I&>( **this ).ShiftToBottomRight( width, height, fillValues )

   /*!
    * Extends or crops the image at its left and top sides to the specified
    * \a width and \a height in pixels. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::ShiftToBottomRight()
    */
   template <typename T>
   ImageVariant& ShiftToBottomRight( int width, int height, const GenericVector<T>& fillValues )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT_TO_BOTTOM_RIGHT )
      return *this;
   }

#undef __SHIFT_TO_BOTTOM_RIGHT

   // -------------------------------------------------------------------------

#define __SHIFT_TO_BOTTOM_RIGHT( I ) \
   static_cast<pcl::I&>( **this ).ShiftToBottomRight( width, height )

   /*!
    * Extends or crops the image at its left and top sides to the specified
    * \a width and \a height in pixels, filling extended regions with the
    * minimum sample value in the native range of the image. Returns a
    * reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::ShiftToBottomRight()
    */
   ImageVariant& ShiftToBottomRight( int width, int height )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT_TO_BOTTOM_RIGHT )
      return *this;
   }

#undef __SHIFT_TO_BOTTOM_RIGHT

   // -------------------------------------------------------------------------

#define __SHIFT_TO_BOTTOM_CENTER( I ) \
   static_cast<pcl::I&>( **this ).ShiftToCenter( width, height, fillValues )

   /*!
    * Extends or crops the image centered within the specified \a width and
    * \a height in pixels. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::ShiftToCenter()
    */
   template <typename T>
   ImageVariant& ShiftToCenter( int width, int height, const GenericVector<T>& fillValues )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT_TO_BOTTOM_CENTER )
      return *this;
   }

#undef __SHIFT_TO_BOTTOM_CENTER

   // -------------------------------------------------------------------------

#define __SHIFT_TO_BOTTOM_CENTER( I ) \
   static_cast<pcl::I&>( **this ).ShiftToCenter( width, height )

   /*!
    * Extends or crops the image centered within the specified \a width and
    * \a height in pixels, filling extended regions with the minimum sample
    * value in the native range of the image. Returns a reference to this
    * image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::ShiftToCenter()
    */
   ImageVariant& ShiftToCenter( int width, int height )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT_TO_BOTTOM_CENTER )
      return *this;
   }

#undef __SHIFT_TO_BOTTOM_CENTER

   // -------------------------------------------------------------------------

#define __SHIFT_TO_TOP_LEFT( I ) \
   static_cast<pcl::I&>( **this ).ShiftToTopLeft( width, height, fillValues )

   /*!
    * Extends or crops the image at its right and bottom sides to the specified
    * \a width and \a height in pixels. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::ShiftToTopLeft()
    */
   template <typename T>
   ImageVariant& ShiftToTopLeft( int width, int height, const GenericVector<T>& fillValues )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT_TO_TOP_LEFT )
      return *this;
   }

#undef __SHIFT_TO_TOP_LEFT

   // -------------------------------------------------------------------------

#define __SHIFT_TO_TOP_LEFT( I ) \
   static_cast<pcl::I&>( **this ).ShiftToTopLeft( width, height )

   /*!
    * Extends or crops the image at its right and bottom sides to the specified
    * \a width and \a height in pixels, filling extended regions with the
    * minimum sample value in the native range of the image. Returns a
    * reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::ShiftToTopLeft()
    */
   ImageVariant& ShiftToTopLeft( int width, int height )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT_TO_TOP_LEFT )
      return *this;
   }

#undef __SHIFT_TO_TOP_LEFT

   // -------------------------------------------------------------------------

#define __SHIFT_TO_TOP_RIGHT( I ) \
   static_cast<pcl::I&>( **this ).ShiftToTopRight( width, height, fillValues )

   /*!
    * Extends or crops the image at its left and bottom sides to the specified
    * \a width and \a height in pixels. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::ShiftToTopRight()
    */
   template <typename T>
   ImageVariant& ShiftToTopRight( int width, int height, const GenericVector< T >& fillValues )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT_TO_TOP_RIGHT )
      return *this;
   }

#undef __SHIFT_TO_TOP_RIGHT

   // -------------------------------------------------------------------------

#define __SHIFT_TO_TOP_RIGHT( I ) \
   static_cast<pcl::I&>( **this ).ShiftToTopRight( width, height )

   /*!
    * Extends or crops the image at its left and bottom sides to the specified
    * \a width and \a height in pixels, filling extended regions with the
    * minimum sample value in the native range of the image. Returns a
    * reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::ShiftToTopRight()
    */
   ImageVariant& ShiftToTopRight( int width, int height )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SHIFT_TO_TOP_RIGHT )
      return *this;
   }

#undef __SHIFT_TO_TOP_RIGHT

   // -------------------------------------------------------------------------

#define __STD_DEV( I ) \
   result = static_cast<const pcl::I&>( **this ).StdDev( rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Returns the standard deviation of a subset of pixel samples.
    *
    * This member function is a generalized wrapper for GenericImage::StdDev()
    */
   double StdDev( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      double result = 0;
      if ( *this )
         SOLVE_TEMPLATE( __STD_DEV )
      return result;
   }

#undef __STD_DEV

   // -------------------------------------------------------------------------

#define __SUBTRACT( I ) \
   static_cast<pcl::I&>( **this ).Subtract( scalar, rect, firstChannel, lastChannel )

   /*!
    * Subtracts the specified \a scalar from a subset of pixel samples in this
    * image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Subtract()
    */
   template <typename T>
   ImageVariant& Subtract( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE( __SUBTRACT )
      return *this;
   }

#undef __SUBTRACT

   /*!
    * A synonym for Subtract().
    */
   template <typename T>
   ImageVariant& Sub( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Subtract( scalar, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

#define __SUBTRACT_1( I ) \
   ImageVariant::Subtract( static_cast<pcl::I&>( **this ), image, point, channel, rect, firstChannel, lastChannel )

#define __SUBTRACT_2( I ) \
   image1.Subtract( static_cast<const pcl::I&>( *image2 ), point, channel, rect, firstChannel, lastChannel )

private:

   template <class P> static
   void Subtract( GenericImage<P>& image1, const ImageVariant& image2,
                  const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_2( image2, __SUBTRACT_2 )
   }

public:

   /*!
    * Subtracts pixel samples of the specified \a image from a subset of pixel
    * samples in this image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Subtract()
    */
   ImageVariant& Subtract( const ImageVariant& image,
                           const Point& point = Point( int_max ), int channel = -1,
                           const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __SUBTRACT_1 )
      return *this;
   }

#undef __SUBTRACT_1
#undef __SUBTRACT_2

   /*!
    * A synonym for Subtract().
    */
   ImageVariant& Sub( const ImageVariant& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      return Subtract( image, point, channel, rect, firstChannel, lastChannel );
   }

   // -------------------------------------------------------------------------

#define __SUBTRACTED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Subtracted( scalar, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Creates a local image with a subset of pixel samples from this image, and
    * subtracts a \a scalar. Returns the resulting image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Subtracted()
    */
   template <typename T>
   ImageVariant Subtracted( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __SUBTRACTED )
      return result;
   }

#undef __SUBTRACTED

   // -------------------------------------------------------------------------

#define __SUM_OF_SQUARES( I ) \
   result = static_cast<const pcl::I&>( **this ).SumOfSquares( rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Returns the sum of the squares of a subset of pixel samples.
    *
    * This member function is a generalized wrapper for
    * GenericImage::SumOfSquares()
    */
   double SumOfSquares( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      double result = 0;
      if ( *this )
         SOLVE_TEMPLATE( __SUM_OF_SQUARES )
      return result;
   }

#undef __SUM_OF_SQUARES

   // -------------------------------------------------------------------------

#define __TRANSFER_IMAGE_1( I ) \
   ImageVariant::TransferImage( static_cast<pcl::I&>( **this ), image )

#define __TRANSFER_IMAGE_2( I ) \
   image1.Transfer( static_cast<pcl::I&>( *image2 ) )

private:

   template <class P> static
   void TransferImage( GenericImage<P>& image1, ImageVariant& image2 )
   {
      SOLVE_TEMPLATE_2( image2, __TRANSFER_IMAGE_2 )
   }

public:

   /*!
    * Transfers pixel data to this image from another \a image, which is an
    * instance of a different template instantiation of %GenericImage. Returns
    * a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Transfer()
    */
   ImageVariant& TransferImage( ImageVariant& image )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE( __TRANSFER_IMAGE_1 )
      return *this;
   }

#undef __TRANSFER_IMAGE_1
#undef __TRANSFER_IMAGE_2

   // -------------------------------------------------------------------------

#define __TRANSFORM( I ) \
   static_cast<const pcl::I&>( **this ).Transform( transform, rect, firstChannel, lastChannel )

   /*!
    * Applies a \e direct image transformation to a subset of pixel samples.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Transform()
    */
   void Transform( BidirectionalImageTransformation& transform,
                   const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      if ( *this )
         SOLVE_TEMPLATE( __TRANSFORM )
   }

#undef __TRANSFORM

   // -------------------------------------------------------------------------

#define __TRUNCATE( I ) \
   static_cast<pcl::I&>( **this ).Truncate( lowerBound, upperBound, rect, firstChannel, lastChannel )

   /*!
    * Truncates a subset of pixel samples to the specified range of values.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Truncate()
    */
   template <typename T>
   ImageVariant& Truncate( T lowerBound, T upperBound, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __TRUNCATE )
      return *this;
   }

#undef __TRUNCATE

   // -------------------------------------------------------------------------

#define __TRUNCATE( I ) \
   static_cast<pcl::I&>( **this ).Truncate( rect, firstChannel, lastChannel )

   /*!
    * Truncates a subset of pixel samples to the native range of the image.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Truncate()
    */
   ImageVariant& Truncate( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __TRUNCATE )
      return *this;
   }

#undef __TRUNCATE

   // -------------------------------------------------------------------------

#define __TRUNCATED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Truncated( lowerBound, upperBound, rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * truncated to the specified range of values.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Truncated()
    */
   template <typename T>
   ImageVariant Truncated( T lowerBound, T upperBound, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __TRUNCATED )
      return result;
   }

#undef __TRUNCATED

   // -------------------------------------------------------------------------

#define __TRUNCATED( I ) \
   result.SetImage( *new pcl::I( static_cast<const pcl::I&>( **this ).Truncated( rect, firstChannel, lastChannel ) ) ); \
   result.SetOwnership( true )

   /*!
    * Returns a local image with a subset of pixel samples from this image,
    * truncated to the native range of the image.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Truncated()
    */
   ImageVariant Truncated( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      ImageVariant result;
      if ( *this )
         SOLVE_TEMPLATE( __TRUNCATED )
      return result;
   }

#undef __TRUNCATED

   // -------------------------------------------------------------------------

#define __VARIANCE( I ) \
   result = static_cast<const pcl::I&>( **this ).Variance( rect, firstChannel, lastChannel, maxProcessors )

   /*!
    * Returns the variance from the mean of a subset of pixel samples.
    *
    * This member function is a generalized wrapper for GenericImage::Variance()
    */
   double Variance( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1, int maxProcessors = 0 ) const
   {
      double result = 0;
      if ( *this )
         SOLVE_TEMPLATE( __VARIANCE )
      return result;
   }

#undef __VARIANCE

   // -------------------------------------------------------------------------

#define __WHITE( I ) \
   static_cast<pcl::I&>( **this ).White( rect, firstChannel, lastChannel )

   /*!
    * Fills a subset of pixel samples with the maximum sample value in the
    * native range of the image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::White()
    */
   ImageVariant& White( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __WHITE )
      return *this;
   }

#undef __WHITE

   // -------------------------------------------------------------------------

#define __WRITE( I ) \
   static_cast<const pcl::I&>( **this ).Write( file, rect, firstChannel, lastChannel )

   /*!
    * Writes a subset of pixel samples to a raw-storage output stream.
    *
    * This member function is a generalized wrapper for GenericImage::Write()
    */
   void Write( File& file, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      if ( *this )
         SOLVE_TEMPLATE( __WRITE )
   }

#undef __WRITE

   // -------------------------------------------------------------------------

#define __WRITE( I ) \
   static_cast<const pcl::I&>( **this ).Write( filePath, rect, firstChannel, lastChannel )

   /*!
    * Writes a subset of pixel samples to a raw-storage output stream.
    *
    * This member function is a generalized wrapper for GenericImage::Write()
    */
   void Write( const String& filePath, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 ) const
   {
      if ( *this )
         SOLVE_TEMPLATE( __WRITE )
   }

#undef __WRITE

   // -------------------------------------------------------------------------

#define __XNOR( I ) \
   static_cast<pcl::I&>( **this ).Xnor( scalar, rect, firstChannel, lastChannel )

   /*!
    * Performs a bitwise XNOR (exclusive NOR) operation between a subset of
    * pixel samples in this image and the specified \a scalar. Returns a
    * reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Xnor()
    */
   template <typename T>
   ImageVariant& Xnor( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __XNOR )
      return *this;
   }

#undef __XNOR

   // -------------------------------------------------------------------------

#define __XNOR_1( I ) \
   ImageVariant::Xnor( static_cast<pcl::I&>( **this ), image, point, channel, rect, firstChannel, lastChannel )

#define __XNOR_2( I ) \
   image1.Xnor( static_cast<const pcl::I&>( *image2 ), point, channel, rect, firstChannel, lastChannel )

private:

   template <class P> static
   void Xnor( GenericImage<P>& image1, const ImageVariant& image2,
              const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_REAL_2( image2, __XNOR_2 )
   }

public:

   /*!
    * Performs a bitwise XNOR (NOT exclusive OR) operation between a subset of
    * pixel samples in this image and the corresponding samples of the
    * specified source \a image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Xnor()
    */
   ImageVariant& Xnor( const ImageVariant& image,
                       const Point& point = Point( int_max ), int channel = -1,
                       const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE_REAL( __XNOR_1 )
      return *this;
   }

#undef __XNOR_1
#undef __XNOR_2

   // -------------------------------------------------------------------------

#define __XOR( I ) \
   static_cast<pcl::I&>( **this ).Xor( scalar, rect, firstChannel, lastChannel )

   /*!
    * Performs a bitwise XOR (exclusive OR) operation between a subset of pixel
    * samples in this image and the specified \a scalar. Returns a reference to
    * this image.
    *
    * This member function is a generalized wrapper for GenericImage::Xor()
    */
   template <typename T>
   ImageVariant& Xor( T scalar, const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __XOR )
      return *this;
   }

#undef __XOR

   // -------------------------------------------------------------------------

#define __XOR_1( I ) \
   ImageVariant::Xor( static_cast<pcl::I&>( **this ), image, point, channel, rect, firstChannel, lastChannel )

#define __XOR_2( I ) \
   image1.Xor( static_cast<const pcl::I&>( *image2 ), point, channel, rect, firstChannel, lastChannel )

private:

   template <class P> static
   void Xor( GenericImage<P>& image1, const ImageVariant& image2,
             const Point& point, int channel, const Rect& rect, int firstChannel, int lastChannel )
   {
      SOLVE_TEMPLATE_REAL_2( image2, __XOR_2 )
   }

public:

   /*!
    * Performs a bitwise XOR (exclusive OR) operation between a subset of pixel
    * samples in this image and the corresponding samples of the specified
    * source \a image. Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Xor()
    */
   ImageVariant& Xor( const ImageVariant& image,
                      const Point& point = Point( int_max ), int channel = -1,
                      const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         if ( image )
            SOLVE_TEMPLATE_REAL( __XOR_1 )
      return *this;
   }

#undef __XOR_1
#undef __XOR_2

   // -------------------------------------------------------------------------

#define __ZERO( I ) \
   static_cast<pcl::I&>( **this ).Zero( rect, firstChannel, lastChannel )

   /*!
    * Fills a subset of pixel samples with the constant value resulting from
    * converting the scalar zero (0) to the sample data type of the image.
    * Returns a reference to this image.
    *
    * This member function is a generalized wrapper for GenericImage::Zero()
    */
   ImageVariant& Zero( const Rect& rect = Rect( 0 ), int firstChannel = -1, int lastChannel = -1 )
   {
      if ( *this )
         SOLVE_TEMPLATE_REAL( __ZERO )
      return *this;
   }

#undef __ZERO

   // -------------------------------------------------------------------------

#define __COMPRESS( I ) \
   return static_cast<const pcl::I&>( **this ).Compress( compressor, rect, channel, perf )

   /*!
    * Compression of pixel samples from the image transported by an
    * %ImageVariant. Returns a list of compressed sub-blocks.
    *
    * If this %ImageVariant object does not transport an image, this function
    * returns an empty array.
    *
    * This member function is a generalized wrapper for
    * GenericImage::Compress().
    */
   Compression::subblock_list Compress( const Compression& compressor,
                                        const Rect& rect = Rect( 0 ), int channel = -1,
                                        Compression::Performance* perf = nullptr ) const
   {
      if ( *this )
         SOLVE_TEMPLATE( __COMPRESS )
      return Compression::subblock_list();
   }

#undef __COMPRESS

   // -------------------------------------------------------------------------

   /*!
    * Returns true iff this %ImageVariant object transports an image that is an
    * instance of the same template instantiation of the specified image.
    */
   template <class P>
   bool IsAs( const pcl::GenericImage<P>& ) const
   {
      return m_data->image && m_data->isFloatSample == P::IsFloatSample() &&
                            m_data->isComplexSample == P::IsComplexSample() &&
                            m_data->bitsPerSample == P::BitsPerSample();
   }

   /*!
    * Assigns another instance of %ImageVariant to this object. Returns a
    * reference to this object.
    *
    * If the previously transported image (if any) was owned by %ImageVariant,
    * and there are no more ImageVariant references to it, then it is destroyed
    * before assignment.
    */
   ImageVariant& Assign( const ImageVariant& image )
   {
      image.m_data->Attach();
      DetachFromData();
      m_data = image.m_data;
      return *this;
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    *
    * This operator calls Assign() with the specified source \a image.
    */
   ImageVariant& operator =( const ImageVariant& image )
   {
      return Assign( image );
   }

#define TRANSFER_BODY()                                                                      \
   if ( &image != this ) /* N.B.: Self-movement incompatible with server-side SharedImage */ \
   {                                                                                         \
      DetachFromData();                                                                      \
      m_data = image.m_data;                                                                 \
      image.m_data = nullptr;                                                                \
   }                                                                                         \
   return *this

   /*!
    * Transfers the image transported by other %ImageVariant to this object.
    * Returns a reference to this object.
    *
    * If the previously transported image (if any) was owned by %ImageVariant,
    * and there are no more ImageVariant references to it, then it is destroyed
    * before assignment.
    *
    * After calling this function, the specified source \a image is left in an
    * invalid state and should be destroyed immediately.
    */
   ImageVariant& Transfer( ImageVariant& image )
   {
      TRANSFER_BODY();
   }

   /*!
    * Transfers the image transported by other %ImageVariant to this object.
    * Returns a reference to this object.
    *
    * If the previously transported image (if any) was owned by %ImageVariant,
    * and there are no more ImageVariant references to it, then it is destroyed
    * before assignment.
    *
    * After calling this function, the specified source \a image is left in an
    * invalid state and should be destroyed immediately.
    */
   ImageVariant& Transfer( ImageVariant&& image )
   {
      TRANSFER_BODY();
   }

#undef TRANSFER_BODY

   /*!
    * Move assignment operator. Returns a reference to this object.
    *
    * This operator calls Transfer() with the specified source \a image.
    */
   ImageVariant& operator =( ImageVariant&& image )
   {
      return Transfer( image );
   }

   /*!
    * Releases the image transported by this object and forces another
    * %ImageVariant instance to transport it.
    *
    * Unlike move assignment (or, equivalently, the Transfer() member
    * function), this object will be in a valid empty state after calling this
    * function.
    */
   void ReleaseTo( ImageVariant& image )
   {
      if ( &image != this ) // N.B.: Self-release incompatible with server-side SharedImage.
      {
         Data* newData = new Data;
         image.DetachFromData();
         image.m_data = m_data;
         m_data = newData;
      }
   }

   /*!
    * Forces this instance of %ImageVariant to transport the specified image.
    * Returns a reference to this object.
    *
    * If the previously transported image (if any) was owned by %ImageVariant,
    * and there are no more %ImageVariant references to it, then it is
    * destroyed before referencing the specified image.
    */
   template <class P>
   ImageVariant& SetImage( GenericImage<P>& image )
   {
      if ( &image != m_data->image )
      {
         Free();
         m_data->Update( &image );
      }
      return *this;
   }

   /*!
    * Exchanges two %ImageVariant instances \a x1 and \a x2.
    */
   friend void Swap( ImageVariant& x1, ImageVariant& x2 )
   {
      pcl::Swap( x1.m_data, x2.m_data );
   }

   /*!
    * Creates a new image with the specified sample data type. Returns a
    * reference to this object.
    *
    * \param isFloat    If true, floating-point real samples will be used.
    *
    * \param isComplex  If true, complex pixel samples (implicitly
    *                   floating-point) will be used.
    *
    * \param bitSize    Number of bits per sample. The supported combinations
    *                   are:\n
    *                   \n
    *       \li For floating-point samples (real or complex): 32 or 64.\n
    *       \li For integer samples: 8, 16 or 32.
    *
    * If the previously transported image (if any) was owned by %ImageVariant,
    * and there are no more %ImageVariant references to it, then it is
    * destroyed before creating the new image.
    *
    * The newly created image will be empty. To allocate pixel data, you must
    * call GenericImage::AllocateData() explicitly.
    *
    * The newly created image will be owned by %ImageVariant.
    */
   ImageVariant& CreateImage( bool isFloat, bool isComplex, int bitSize )
   {
      if ( isFloat )
      {
         if ( isComplex )
            CreateComplexImage( bitSize );
         else
            CreateFloatImage( bitSize );
      }
      else
      {
         if ( !isComplex )
            CreateUIntImage( bitSize );
         /*
         else
         {
            ### Yeah this is true, but everybody already knows so we don't want
                to replicate this useless message everywhere!
            throw Error( "Integer-valued complex images not supported by this PCL version." );
         }
         */
      }

      return *this;
   }

#define CREATE_IMAGE( I )  m_data->Update( new pcl::I )

   /*!
    * Creates a new floating point real image with the specified sample size
    * in bits. Returns a reference to this object.
    *
    * \param bitSize    Sample size in bits for the newly created real image.
    *                   Valid argument values are 32 and 64. The default value
    *                   is 32.
    *
    * This function is a convenience shortcut for:
    *
    * \code CreateImage( true, false, bitSize ); \endcode
    */
   ImageVariant& CreateFloatImage( int bitSize = 32 )
   {
      PCL_PRECONDITION( bitSize == 32 || bitSize == 64 )
      Free();
      switch ( bitSize )
      {
      case 32 : CREATE_IMAGE( Image ); break;
      case 64 : CREATE_IMAGE( DImage ); break;
      }
      m_data->ownsImage = m_data->image != nullptr;
      return *this;
   }

   /*!
    * Creates a new 32-bit floating point real image. Returns a reference to
    * this object.
    *
    * This function is a convenience shortcut for:
    *
    * \code CreateFloatImage(); \endcode
    */
   ImageVariant& CreateImage()
   {
      return CreateFloatImage();
   }

   /*!
    * Creates a new floating point complex image with the specified sample
    * size in bits. Returns a reference to this object.
    *
    * \param bitSize    Sample size in bits for the newly created complex
    *                   image. Valid argument values are 32 and 64.
    *
    * This function is a convenience shortcut for:
    *
    * \code CreateImage( true, true, bitSize ); \endcode
    */
   ImageVariant& CreateComplexImage( int bitSize = 32 )
   {
      PCL_PRECONDITION( bitSize == 32 || bitSize == 64 )
      Free();
      switch ( bitSize )
      {
      case 32 : CREATE_IMAGE( ComplexImage ); break;
      case 64 : CREATE_IMAGE( DComplexImage ); break;
      }
      m_data->ownsImage = m_data->image != nullptr;
      return *this;
   }

   /*!
    * Creates a new unsigned integer image with the specified sample size in
    * bits. Returns a reference to this object.
    *
    * \param bitSize    Sample size in bits for the newly integer image. Valid
    *                   argument values are 8, 16 and 32.
    *
    * This function is a convenience shortcut for:
    *
    * \code CreateImage( false, false, bitSize ); \endcode
    */
   ImageVariant& CreateUIntImage( int bitSize = 16 )
   {
      PCL_PRECONDITION( bitSize == 8 || bitSize == 16 || bitSize == 32 )
      Free();
      switch ( bitSize )
      {
      case  8 : CREATE_IMAGE( UInt8Image ); break;
      case 16 : CREATE_IMAGE( UInt16Image ); break;
      case 32 : CREATE_IMAGE( UInt32Image ); break;
      }
      m_data->ownsImage = m_data->image != nullptr;
      return *this;
   }

#undef CREATE_IMAGE

   /*!
    * Creates a new image with the same sample data type (template
    * instantiation) as the specified image. Returns a reference to this
    * object.
    *
    * \param image  %Image to obtain sample data type parameters from.
    *
    * This function is a convenience shortcut for:
    *
    * \code
    * CreateImage( P::IsFloatSample(), P::IsComplexSample(), P::BitsPerSample() );
    * \endcode
    */
   template <class P>
   ImageVariant& CreateImageAs( const pcl::GenericImage<P>& image )
   {
      return CreateImage( P::IsFloatSample(), P::IsComplexSample(), P::BitsPerSample() );
   }

   /*!
    * Creates a new image with the same sample data type (template
    * instantiation) as the specified %ImageVariant object. Returns a reference
    * to this object.
    *
    * \param image   %ImageVariant to obtain sample data type parameters from.
    *
    * This function is a convenience shortcut for:
    *
    * \code
    * CreateImage( image.IsFloatSample(), image.IsComplexSample(), image.BitsPerSample() );
    * \endcode
    */
   ImageVariant& CreateImageAs( const ImageVariant& image )
   {
      return CreateImage( image.IsFloatSample(), image.IsComplexSample(), image.BitsPerSample() );
   }

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

   /*!
    * Creates a new shared image with the specified sample data type. Returns a
    * reference to this object.
    *
    * \param isFloat    If true, floating-point real pixels will be used.
    *
    * \param isComplex  If true, complex pixels ( implicitly floating point )
    *                   will be used.
    *
    * \param bitSize    Number of bits per sample. The supported combinations
    *                   are:\n
    *                   \n
    *       \li For floating-point samples ( real or complex ): 32 or 64.\n
    *       \li For integer samples: 8, 16 or 32.
    *
    * This member function works in an equivalent way to:
    *
    * \code CreateImage( bool isFloat, bool isComplex, int bitSize ); \endcode
    *
    * except that this function creates a <em>shared image</em>, while
    * CreateImage() creates a <em>local image</em>. A shared image is an alias
    * to an image that lives in the PixInsight core application, while a local
    * image is a private object in the local heap of the calling module.
    *
    * Shared images are particularly useful for processes involving
    * <em>intermodule communication</em> in the PixInsight platform. An example
    * is the FileFormatInstance class, which can be used to perform file I/O
    * operations by invoking any installed file format support module on the
    * platform. %FileFormatInstance requires shared images to read/write images
    * from/to disk files ( you can actually pass a local image to
    * %FileFormatInstance, but then PCL will generate and use a temporary
    * \e shared working image on the fly, wasting memory unnecessarily ).
    */
   ImageVariant& CreateSharedImage( bool isFloat, bool isComplex, int bitSize )
   {
      if ( isFloat )
      {
         if ( isComplex )
            CreateSharedComplexImage( bitSize );
         else
            CreateSharedFloatImage( bitSize );
      }
      else
      {
         if ( !isComplex )
            CreateSharedUIntImage( bitSize );
      }

      return *this;
   }

#define CREATE_SHARED_IMAGE( I ) m_data->Update( new pcl::I( (void*)0, 0, 0 ) )

   /*!
    * Creates a new shared, floating point real image with the specified sample
    * size in bits. Returns a reference to this object.
    *
    * \param bitSize    Sample size in bits for the newly created real image.
    *                   Valid argument values are 32 and 64.
    *
    * This function is a convenience shortcut for:
    *
    * \code CreateSharedImage( true, false, bitSize ); \endcode
    *
    * For an equivalent member function that creates a local image, see
    * CreateFloatImage().
    */
   ImageVariant& CreateSharedFloatImage( int bitSize = 32 )
   {
      PCL_PRECONDITION( bitSize == 32 || bitSize == 64 )
      Free();
      switch ( bitSize )
      {
      case 32 : CREATE_SHARED_IMAGE( Image ); break;
      case 64 : CREATE_SHARED_IMAGE( DImage ); break;
      }
      m_data->ownsImage = m_data->image != nullptr;
      return *this;
   }

   /*!
    * Creates a new shared, 32-bit floating point real image. Returns a
    * reference to this object.
    *
    * This function is a convenience shortcut for:
    *
    * \code CreateSharedFloatImage( 32 ); \endcode
    *
    * For an equivalent member function that creates a local image, see
    * CreateImage().
    */
   ImageVariant& CreateSharedImage()
   {
      return CreateSharedFloatImage();
   }

   /*!
    * Creates a new shared, floating point complex image with the specified
    * sample size in bits. Returns a reference to this object.
    *
    * \param bitSize    Sample size in bits for the newly created complex
    *                   image. Valid argument values are 32 and 64.
    *
    * This function is a convenience shortcut for:
    *
    * \code CreateSharedImage( true, true, bitSize ); \endcode
    *
    * For an equivalent member function that creates a local image, see
    * CreateComplexImage().
    */
   ImageVariant& CreateSharedComplexImage( int bitSize = 32 )
   {
      PCL_PRECONDITION( bitSize == 32 || bitSize == 64 )
      Free();
      switch ( bitSize )
      {
      case 32 : CREATE_SHARED_IMAGE( ComplexImage ); break;
      case 64 : CREATE_SHARED_IMAGE( DComplexImage ); break;
      }
      m_data->ownsImage = m_data->image != nullptr;
      return *this;
   }

   /*!
    * Creates a new shared, unsigned integer image with the specified sample
    * size in bits. Returns a reference to this object.
    *
    * \param bitSize    Sample size in bits for the newly integer image. Valid
    *                   argument values are 8, 16 and 32.
    *
    * This function is a convenience shortcut for:
    *
    * \code CreateImage( false, false, bitSize ); \endcode
    *
    * For an equivalent member function that creates a local image, see
    * CreateUIntImage().
    */
   ImageVariant& CreateSharedUIntImage( int bitSize = 16 )
   {
      PCL_PRECONDITION( bitSize == 8 || bitSize == 16 || bitSize == 32 )
      Free();
      switch ( bitSize )
      {
      case  8 : CREATE_SHARED_IMAGE( UInt8Image ); break;
      case 16 : CREATE_SHARED_IMAGE( UInt16Image ); break;
      case 32 : CREATE_SHARED_IMAGE( UInt32Image ); break;
      }
      m_data->ownsImage = m_data->image != nullptr;
      return *this;
   }

#undef CREATE_SHARED_IMAGE

   /*!
    * Creates a new shared image with the same sample data type ( template
    * instantiation ) as the specified image. Returns a reference to this
    * object.
    *
    * \param image  %Image to obtain sample data type parameters from.
    *
    * This function is a convenience shortcut for:
    *
    * \code
    * CreateSharedImage( P::IsFloatSample(), P::IsComplexSample(), P::BitsPerSample() );
    * \endcode
    *
    * For an equivalent member function that creates a local image, see
    * CreateImageAs( const pcl::GenericImage& ).
    */
   template <class P>
   ImageVariant& CreateSharedImageAs( const pcl::GenericImage<P>& image )
   {
      return CreateSharedImage( P::IsFloatSample(), P::IsComplexSample(), P::BitsPerSample() );
   }

   /*!
    * Creates a new shared image with the same sample data type ( template
    * instantiation ) as the specified %ImageVariant object. Returns a
    * reference to this object.
    *
    * \param image   %ImageVariant to obtain sample data type parameters from.
    *
    * This function is a convenience shortcut for:
    *
    * \code
    * CreateSharedImage( image.IsFloatSample(), image.IsComplexSample(), image.BitsPerSample() );
    * \endcode
    *
    * For an equivalent member function that creates a local image, see
    * CreateImageAs( const ImageVariant& ).
    */
   ImageVariant& CreateSharedImageAs( const ImageVariant& image )
   {
      return CreateSharedImage( image.IsFloatSample(), image.IsComplexSample(), image.BitsPerSample() );
   }

#endif // !__PCL_BUILDING_PIXINSIGHT_APPLICATION

#define ALLOCATE_IMAGE( I ) \
   static_cast<pcl::I&>( **this ).AllocateData( width, height, numberOfChannels, colorSpace )

   /*!
    * Allocates pixel data in the image transported by this %ImageVariant
    * instance, with the specified geometry and color space. Returns a
    * reference to this object.
    *
    * \param width              Width of the newly created image in pixels.
    *
    * \param height             Height of the newly created image in pixels.
    *
    * \param numberOfChannels   Number of channels. Must be larger or equal to
    *                           the number of nominal channels, as imposed by
    *                           the specified color space.
    *
    * \param colorSpace         Color space. See the ColorSpace namespace for
    *                           symbolic constants.
    *
    * If this %ImageVariant object transports no image, a new one is created in
    * 32-bit floating point format. %ImageVariant will own the newly created
    * image in this case.
    *
    * This function is useful to allocate pixel data without having to resolve
    * the template instantiation of the transported image. PCL allocates pixels
    * of the appropriate sample data type transparently.
    */
   ImageVariant& AllocateImage( int width, int height, int numberOfChannels, color_space colorSpace )
   {
      if ( !*this )
         CreateImage();
      SOLVE_TEMPLATE( ALLOCATE_IMAGE )
      return *this;
   }

#undef ALLOCATE_IMAGE

#define COPY_IMAGE( I ) static_cast<pcl::I&>( **this ).Assign( image )

   /*!
    * Assigns an image to the image transported by this %ImageVariant instance.
    * Returns a reference to this object.
    *
    * \param image   %Image to assign.
    *
    * The assigned image can be an instance of any supported template
    * instantiation of GenericImage. If this %ImageVariant object transports no
    * image, a new one is created as an instance of the same template
    * instantiation as the specified source image. In this case, %ImageVariant
    * will own the newly created image.
    *
    * This function is useful to assign an image without having to resolve the
    * template instantiation of the transported image. PCL performs the
    * assignment between different image types transparently.
    */
   template <class P>
   ImageVariant& CopyImage( const GenericImage<P>& image )
   {
      if ( !*this )
         CreateImageAs( image );
      SOLVE_TEMPLATE( COPY_IMAGE )
      return *this;
   }

#undef COPY_IMAGE

#define COPY_IMAGE( I ) CopyImage( static_cast<const pcl::I&>( *image ) )

   /*!
    * Assigns an image transported by another %ImageVariant instance to the
    * image transported by this object. Returns a reference to this object.
    *
    * \param image   Source %ImageVariant instance whose transported image will
    *                be assigned.
    *
    * This function calls CopyImage( const I& ) after resolving the template
    * instantiation of the transported image. See the documentation of that
    * function for details.
    */
   ImageVariant& CopyImage( const ImageVariant& image )
   {
      if ( image )
         SOLVE_TEMPLATE_2( image, COPY_IMAGE )
      else
         FreeImage();
      return *this;
   }

#undef COPY_IMAGE

#define FREE_IMAGE( I ) static_cast<pcl::I&>( **this ).FreeData()

   /*!
    * Destroys the existing pixel data in the image transported by this
    * %ImageVariant instance. Returns a reference to this object.
    *
    * This function is useful to deallocate pixel data without having to
    * resolve the template instantiation of the transported image. PCL
    * deallocates pixel data of any supported pixel sample type transparently.
    *
    * If this %ImageVariant transports no image, this function has no effect.
    *
    * \note   Unlike the Free() member function, this function cannot destroy
    * the transported image, but only its pixel data. Note that this function
    * destroys pixel data irrespective of whether %ImageVariant owns the
    * transported image.
    */
   ImageVariant& FreeImage()
   {
      if ( *this )
         SOLVE_TEMPLATE( FREE_IMAGE )
      return *this;
   }

   /*!
    * Returns a pointer to a dynamically allocated Compression object. The
    * returned object implements the specified raw storage compression
    * \a algorithm for compression of a contiguous sequence of elements with
    * \a itemSize bytes each.
    */
   static Compression* NewCompression( swap_compression algorithm, int itemSize = 1 );

#undef FREE_IMAGE

   /*!
    * Writes the image transported by this %ImageVariant instance to a raw
    * storage file.
    *
    * \param filePath      A file path specification where the output raw
    *                      storage file will be created.
    *
    * \param compression   Compression algorithm. If specified, all stored
    *                      pixel data will be compressed using this algorithm.
    *                      Compression is disabled by default
    *
    * \param perf          If non-null, pointer to a Compression::Performance
    *                      structure where compression performance data will be
    *                      provided if the raw file is compressed.
    *
    * \param processEvents If true, Module->ProcessEvents() will be called at
    *                      regular intervals during the file write operation.
    *                      This is useful to keep the graphical interface
    *                      responsive during long disk operations. The default
    *                      value is false.
    *
    * The generated file can be employed to construct a new image instance by
    * calling the ReadSwapFile() member function with the same file path
    * specification.
    *
    * This function is useful to write pixel data without having to resolve the
    * template instantiation of the transported image. PCL writes pixel data of
    * any supported sample type transparently.
    */
   void WriteSwapFile( const String& filePath,
                       swap_compression compression = SwapCompression::None,
                       Compression::Performance* perf = nullptr,
                       bool processEvents = false ) const;

   /*!
    * Writes the image transported by this %ImageVariant object to a set of
    * raw storage files using parallel disk write operations.
    *
    * \param fileName      Base file name for the raw storage files.
    *
    * \param directories   A list of directories where the set of raw files
    *                      will be created by this function.
    *
    * \param compression   Compression algorithm. If specified, all stored
    *                      pixel data will be compressed using this algorithm.
    *                      Compression is disabled by default
    *
    * \param perf          If non-null, pointer to a Compression::Performance
    *                      structure where compression performance data will be
    *                      provided if the raw files are compressed. Output
    *                      performance values are the averages of the same
    *                      values computed for all threads.
    *
    * \param processEvents If true, Module->ProcessEvents() will be called at
    *                      regular intervals during the file write operation.
    *                      This is useful to keep the graphical interface
    *                      responsive during long disk operations. The default
    *                      value is false.
    *
    * Each string in the \a directories string list must be a full path
    * specification to an existing directory. On each directory, an output file
    * will be created and an independent execution thread will write the
    * corresponding section of the pixel data from the image transported by
    * this %ImageVariant. To retrieve the data, call the ReadSwapFiles() member
    * function with the same base file name and output directories.
    *
    * \warning This function is not thread-safe: it can only be called from the
    * root thread. This function will throw an Error exception if it is called
    * from a local thread.
    */
   void WriteSwapFiles( const String& fileName, const StringList& directories,
                        swap_compression compression = SwapCompression::None,
                        Compression::Performance* perf = nullptr,
                        bool processEvents = false ) const;
   /*!
    * Reads the image transported by this %ImageVariant instance from a raw
    * storage file.
    *
    * \param filePath      A file path specification to an existing raw storage
    *                      image file.
    *
    * \param perf          If non-null, pointer to a Compression::Performance
    *                      structure where compression performance data will be
    *                      provided if the raw file is compressed.
    *
    * \param processEvents If true, Module->ProcessEvents() will be called at
    *                      regular intervals during the file read operation.
    *                      This is useful to keep the graphical interface
    *                      responsive during long disk operations. The default
    *                      value is false.
    *
    * If necessary, the transported image is re-created as an instance of a
    * different template instantiation, to match the sample data type stored
    * in the input file.
    *
    * In any case, if %ImageVariant owned the previously transported image ( if
    * any ), it is destroyed before reading the new one.
    *
    * This function is useful to read pixel data without having to resolve
    * template instantiation, neither of the input file nor of the transported
    * image. PCL does all the necessary data reading, decompression, allocation
    * and deallocation transparently.
    */
   void ReadSwapFile( const String& filePath,
                      Compression::Performance* perf = nullptr,
                      bool processEvents = false );

   /*!
    * Reads an image from a set of raw storage files using parallel disk read
    * operations.
    *
    * \param fileName      Base file name for the raw storage files.
    *
    * \param directories   A list of directories where the set of raw files
    *                      have been created by a previous call to
    *                      WriteSwapFiles( const String&, const StringList& ).
    *
    * \param perf          If non-null, pointer to a Compression::Performance
    *                      structure where compression performance data will be
    *                      provided if the raw files are compressed. Output
    *                      performance values are the averages of the same
    *                      values computed for all threads.
    *
    * \param processEvents If true, Module->ProcessEvents() will be called at
    *                      regular intervals during the file read operation.
    *                      This is useful to keep the graphical interface
    *                      responsive during long disk operations. The default
    *                      value is false.
    *
    * The read image will be transported and owned by this %ImageVariant
    * instance. If an image is already transported by this object before
    * calling this function, it is removed by a call to Free() prior to the
    * reading operation.
    *
    * If necessary, the transported image is re-created as an instance of a
    * different template instantiation, to match the sample data type stored
    * in the input file(s).
    *
    * \warning This function is not thread-safe: it can only be called from the
    * root thread. This function will throw an Error exception if it is called
    * from a local thread.
    */
   void ReadSwapFiles( const String& fileName, const StringList& directories,
                       Compression::Performance* perf = nullptr,
                       bool processEvents = false );

   /*!
    * Merges the image transported by this object with pixel data read from a
    * raw storage file, masking it with the image transported by another
    * %ImageVariant instance.
    *
    * \param filePath      A file path specification to an existing raw storage
    *                      image file.
    *
    * \param mask          A reference to an %ImageVariant instance whose
    *                      transported image will act as a mask for the merge
    *                      operation.
    *
    * \param invert        If this argument is true, mask pixels will be
    *                      inverted before carrying out the merge operation.
    *
    * \param processEvents If true, Module->ProcessEvents() will be called at
    *                      regular intervals during the file read operation.
    *                      This is useful to keep the graphical interface
    *                      responsive during long disk operations. The default
    *                      value is false.
    *
    * Given a pixel \b V of the image transported by this instance, a pixel
    * \b S read from the input file, and a pixel \b M of the mask image, the
    * original pixel \b V is replaced by the result \b V' of the following
    * expression:
    *
    * <pre>
    * if invert
    *    M = 1 - M
    * V' = V*( 1 - M ) + S*M
    * </pre>
    *
    * in the normalized real range [0,1], where \b 1 represents a saturated
    * (usually white) pixel sample value.
    */
   void MaskFromSwapFile( const String& filePath, const ImageVariant& mask, bool invert = false,
                          bool processEvents = false );

   /*!
    * Merges the image transported by this object with pixel data read from a
    * set of raw storage files through parallel disk read operations, masking
    * it with the image transported by another %ImageVariant instance.
    *
    * \param fileName      Base file name for the raw storage files from which
    *                      source pixels will be read to be merged with this
    *                      image.
    *
    * \param directories   A list of directories where the set of raw files
    *                      have been created by a previous call to
    *                      WriteSwapFiles( const String&, const StringList& ).
    *
    * \param mask          A reference to an %ImageVariant instance whose
    *                      transported image will act as a mask for the merge
    *                      operation.
    *
    * \param invert        If this argument is true, mask pixels will be
    *                      inverted before carrying out the merge operation.
    *
    * \param processEvents If true, Module->ProcessEvents() will be called at
    *                      regular intervals during the file read operation.
    *                      This is useful to keep the graphical interface
    *                      responsive during long disk operations. The default
    *                      value is false.
    *
    * This routine produces exactly the same result as its single-threaded
    * counterpart MaskFromSwapFile().
    *
    * \warning This function is not thread-safe: it can only be called from the
    * root thread. This function will throw an Error exception if it is called
    * from a local thread.
    */
   void MaskFromSwapFiles( const String& fileName, const StringList& directories,
                           const ImageVariant& mask, bool invert = false,
                           bool processEvents = false );

   /*!
    * Deletes all raw storage files previously created by a call to the
    * WriteSwapFile() member function.
    */
   static void DeleteSwapFile( const String& filePath );

   /*!
    * Deletes all raw storage files previously created by a call to the
    * WriteSwapFiles() member function.
    *
    * \warning This function is not thread-safe: it can only be called from the
    * root thread. This function will throw an Error exception if it is called
    * from a local thread.
    */
   static void DeleteSwapFiles( const String& fileName, const StringList& directories );

   /*!
    * Returns the total size in bytes occupied by a raw storage image file.
    *
    * \param filePath      A file path specification to an existing raw storage
    *                      image file.
    *
    * This function does not read any pixel data. All the required data to
    * compute the returned file size is retrieved from existing file headers.
    */
   static uint64 SwapFileSize( const String& filePath );

   /*!
    * Returns the total size in bytes occupied by a set of raw storage files.
    *
    * \param fileName      Base file name for the raw storage files.
    *
    * \param directories   A list of directories where the set of raw files
    *                      have been created by a previous call to
    *                      WriteSwapFiles( const String&, const StringList& ).
    *
    * This function does not read any pixel data. All the required data to
    * compute the returned file size is retrieved from existing file headers.
    */
   static uint64 SwapFilesSize( const String& fileName, const StringList& directories );

   /*!
    * Merges the image transported by this object with the image transported by
    * another %ImageVariant instance, masking it with the image transported by
    * a third %ImageVariant instance.
    *
    * \param src     A reference to an %ImageVariant instance whose transported
    *                image will be the source of the merge operation.
    *
    * \param mask    A reference to an %ImageVariant instance whose transported
    *                image will act as a mask for the merge operation.
    *
    * \param invert  If this argument is true, mask pixels will be inverted
    *                before carrying out the merge operation.
    *
    * Given a pixel \b V of the image transported by this instance, a pixel
    * \b S of the image transported by the source %ImageVariant instance, and a
    * pixel \b M of the mask image, the original pixel \b V is replaced by the
    * result \b V' of the following expression:
    *
    * <pre>
    * if invert
    *    M = 1 - M
    * V' = V*( 1 - M ) + S*M
    * </pre>
    *
    * in the normalized real range [0,1], where \b 1 represents a saturated
    * (usually white) pixel sample value.
    */
   void MaskImage( const ImageVariant& src, const ImageVariant& mask, bool invert = false );

   /*!
    * Returns true iff %ImageVariant owns the image transported by this
    * instance, if any.
    */
   bool OwnsImage() const
   {
      return m_data->ownsImage;
   }

   /*!
    * Causes %ImageVariant to own the transported image, if any. Returns a
    * reference to this object.
    *
    * \note If %ImageVariant owns a transported image, it is destroyed when all
    * instances of %ImageVariant referencing that image are destroyed. It is
    * very important to point out that ownership of transported images is a
    * <em>class-wide property</em> of %ImageVariant, not a private property of
    * any particular %ImageVariant instance.
    */
   ImageVariant& SetOwnership( bool owner = true )
   {
      m_data->ownsImage = owner && m_data->image;
      return *this;
   }

   /*!
    * Removes the transported image, if there is one, from this %ImageVariant
    * object. Returns a reference to this object.
    *
    * If the transported image is owned by %ImageVariant, and there are no more
    * %ImageVariant references to it, then it is also destroyed.
    */
   ImageVariant& Free()
   {
      if ( m_data->IsUnique() )
         m_data->Free();
      else
      {
         Data* newData = new Data;
         DetachFromData();
         m_data = newData;
      }
      return *this;
   }

   /*!
    * Structure member selection operator. Returns a pointer to the constant
    * image transported by this %ImageVariant object.
    *
    * \warning Do not call this member function if the %ImageVariant does not
    * transport an image.
    */
   const AbstractImage* operator ->() const
   {
      PCL_PRECONDITION( m_data->image != nullptr )
      return m_data->image;
   }

   /*!
    * Structure member selection operator. Returns a pointer to the image
    * transported by this %ImageVariant object.
    *
    * \warning Do not call this member function if the %ImageVariant does not
    * transport an image.
    */
   AbstractImage* operator ->()
   {
      PCL_PRECONDITION( m_data->image != nullptr )
      return m_data->image;
   }

   /*!
    * Dereference operator. Returns a reference to the constant image
    * transported by this %ImageVariant object.
    *
    * \warning Do not call this member function if the %ImageVariant does not
    * transport an image.
    */
   const AbstractImage& operator *() const
   {
      PCL_PRECONDITION( m_data->image != nullptr )
      return *m_data->image;
   }

   /*!
    * Dereference operator. Returns a reference to the image transported by
    * this %ImageVariant object.
    *
    * \warning Do not call this member function if the %ImageVariant does not
    * transport an image.
    */
   AbstractImage& operator *()
   {
      PCL_PRECONDITION( m_data->image != nullptr )
      return *m_data->image;
   }

   /*!
    * Returns true iff this %ImageVariant instance transports an image.
    */
   operator bool() const
   {
      return m_data->image != nullptr;
   }

#define __PIXEL_ACCESS_OPERATOR( I ) \
   pcl::I::pixel_traits::FromSample( result, *static_cast<const pcl::I&>( **this ).PixelAddress( x, y, channel ) )

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
    *
    * This member function returns the requested pixel sample in the normalized
    * range [0,1], irrespective of the sample data type of the image. This
    * function conventionally returns zero if this object transports no image.
    *
    * \note For the sake of performance, this function does not check validity
    * of coordinates and channel indexes. Specifying invalid coordinates may
    * lead to unpredictable results, most likey raising an access violation
    * signal or exception.
    */
   double operator ()( int x, int y, int channel = 0 ) const
   {
      double result = 0;
      if ( *this )
         SOLVE_TEMPLATE( __PIXEL_ACCESS_OPERATOR )
      return result;
   }

#undef __PIXEL_ACCESS_OPERATOR

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION

   pi::SharedImage* GetSharedImage() const
   {
      return m_shared;
   }

   bool IsShared() const
   {
      return m_shared != nullptr;
   }

   // Implemented in SharedImage.cpp
   pi::SharedImage* NewSharedImage( void* owner, bool rdOnly = false );

#endif

private:

   struct Data : public ReferenceCounter
   {
      AbstractImage* image           = nullptr;
      bool           isFloatSample   = false;
      bool           isComplexSample = false;
      uint8          bitsPerSample   = 0;
      bool           ownsImage       = false;

      Data() = default;

      ~Data()
      {
         Free();
      }

      template <class P>
      void Update( GenericImage<P>* a_image )
      {
         image = a_image;
         isFloatSample = P::IsFloatSample();
         isComplexSample = P::IsComplexSample();
         bitsPerSample = uint8( P::BitsPerSample() );
      }

      void Free()
      {
         if ( ownsImage )
            delete image;
         image = nullptr;
         isFloatSample = isComplexSample = false;
         bitsPerSample = 0;
         //ownsImage = ownsImage; ### N.B.: Ownership must *not* change here
      }
   };

   Data* m_data = nullptr;

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
   // This is the server-side part of the image sharing mechanism
   pi::SharedImage* m_shared = nullptr;
#else
   const void*      m_shared = nullptr;
#endif

   void DetachFromData()
   {
      if ( !m_data->Detach() )
         delete m_data;
   }

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION
   // Server-side private ctor. used by View
   template <class P>
   ImageVariant( GenericImage<P>* image, int )
   {
      m_data = new Data;
      m_data->Update( image );
      m_data->ownsImage = true;
   }
#endif

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
   friend class pi::SharedImage;
#else
   friend class View;
   friend class ImageView;
#endif
};

// ----------------------------------------------------------------------------

/*
 * Implementation of member functions of GenericImage requiring a full
 * declaration of ImageVariant.
 */

template <class P> inline
void GenericImage<P>::GetLuminance( ImageVariant& Y, const Rect& rect, int maxProcessors ) const
{
   ImageVariant( const_cast<GenericImage<P>*>( this ) ).GetLuminance( Y, rect, maxProcessors );
}

template <class P> inline
void GenericImage<P>::GetLightness( ImageVariant& L, const Rect& rect, int maxProcessors ) const
{
   ImageVariant( const_cast<GenericImage<P>*>( this ) ).GetLightness( L, rect, maxProcessors );
}

template <class P> inline
void GenericImage<P>::GetIntensity( ImageVariant& I, const Rect& rect, int maxProcessors ) const
{
   ImageVariant( const_cast<GenericImage<P>*>( this ) ).GetIntensity( I, rect, maxProcessors );
}

template <class P> inline
GenericImage<P>& GenericImage<P>::SetLuminance( const ImageVariant& Y, const Point& point, const Rect& rect, int maxProcessors )
{
   (void)ImageVariant( this ).SetLuminance( Y, point, rect, maxProcessors );
   return *this;
}

template <class P> inline
GenericImage<P>& GenericImage<P>::SetLightness( const ImageVariant& L, const Point& point, const Rect& rect, int maxProcessors )
{
   (void)ImageVariant( this ).SetLightness( L, point, rect, maxProcessors );
   return *this;
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageVariant_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageVariant.h - Released 2017-04-14T23:04:40Z
