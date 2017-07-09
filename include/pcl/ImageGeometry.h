//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/ImageGeometry.h - Released 2017-07-09T18:07:07Z
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

#ifndef __PCL_ImageGeometry_h
#define __PCL_ImageGeometry_h

/// \file pcl/ImageGeometry.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/Point.h>
#include <pcl/Rectangle.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define m_width            m_geometry->width
#define m_height           m_geometry->height
#define m_numberOfChannels m_geometry->numberOfChannels

// ----------------------------------------------------------------------------

/*!
 * \class ImageGeometry
 * \brief Implements geometric properties of two-dimensional images.
 *
 * %ImageGeometry defines the dimensions in pixels and the channel count of a
 * two-dimensional image.
 *
 * \sa ImageColor, Abstractmage, GenericImage
 */
class PCL_CLASS ImageGeometry
{
public:

   /*!
    * Returns the width of this image in pixels.
    */
   int Width() const
   {
      return m_width;
   }

   /*!
    * Returns the height of this image in pixels.
    */
   int Height() const
   {
      return m_height;
   }

   /*!
    * Returns the total number of channels in this image, including nominal and
    * alpha channels.
    */
   int NumberOfChannels() const
   {
      return m_numberOfChannels;
   }

   /*!
    * Returns the index of the last existing channel in this image. This is the
    * largest valid channel index that can be used with this image,
    * corresponding to a nominal or alpha channel. If this image is empty, this
    * function returns -1.
    */
   int LastChannel() const
   {
      return m_numberOfChannels-1;
   }

   /*!
    * Returns true iff the specified channel index \a c is valid. A valid
    * channel index corresponds to an existing channel in this image.
    */
   bool IsValidChannelIndex( int c ) const
   {
      return c >= 0 && c < m_numberOfChannels;
   }

   /*!
    * Returns true iff this image is empty, i.e. if its area is zero.
    */
   bool IsEmpty() const
   {
      return m_width <= 0 || m_height <= 0 || m_numberOfChannels <= 0;
   }

   /*!
    * Returns the bounding rectangle of this image. The upper left corner of
    * the returned rectangle (x0, y0) is always (0,0). The lower right corner
    * coordinates (x1, y1) correspond to the width and height of the image.
    */
   Rect Bounds() const
   {
      return Rect( m_width, m_height );
   }

   /*!
    * Returns true iff this image includes the specified point \a p in image
    * coordinates.
    */
   template <typename T>
   bool Includes( const GenericPoint<T>& p ) const
   {
      return Includes( p.x, p.y );
   }

   /*!
    * Returns true iff this image includes the specified rectangle \a r in image
    * coordinates.
    */
   template <typename T>
   bool Includes( const GenericRectangle<T>& r ) const
   {
      return Includes( r.x0, r.y0, r.x1, r.y1 );
   }

   /*!
    * Returns true iff this image includes a rectangle given by its separate
    * image coordinates.
    *
    * \param x0,y0   Upper left corner coordinates (horizontal, vertical).
    * \param x1,y1   Lower right corner coordinates (horizontal, vertical).
    */
   template <typename T>
   bool Includes( T x0, T y0, T x1, T y1 ) const
   {
      pcl::OrderRect( x0, y0, x1, y1 );
      return Includes( x0, y0 ) && Includes( x1-T( 1 ), y1-T( 1 ) );
   }

   /*!
    * Returns true iff this image includes a point given by its separate image
    * coordinates.
    *
    * \param x    Horizontal coordinate.
    * \param y    Vertical coordinate.
    */
   template <typename T>
   bool Includes( T x, T y ) const
   {
      return x < m_width && y < m_height && x >= 0 && y >= 0;
   }

   /*!
    * Returns true iff this image intersects with the specified rectangle \a r
    * in image coordinates.
    */
   template <typename T>
   bool Intersects( const pcl::GenericRectangle<T>& r ) const
   {
      return Intersects( r.x0, r.y0, r.x1, r.y1 );
   }

   /*!
    * Returns true iff this image intersects with a rectangle given by its
    * separate image coordinates.
    *
    * \param x0,y0  Upper left corner coordinates (horizontal, vertical).
    * \param x1,y1  Lower right corner coordinates (horizontal, vertical).
    */
   template <typename T>
   bool Intersects( T x0, T y0, T x1, T y1 ) const
   {
      pcl::OrderRect( x0, y0, x1, y1 );
      return x0 < m_width && y0 < m_height && x1 > T( 0 ) && y1 > T( 0 );
   }

   /*!
    * Constrains a point \a p to stay within the boundaries of this image.
    * Returns true iff the original point location is included in this image.
    */
   template <typename T>
   bool Clip( pcl::GenericPoint<T>& p ) const
   {
      return Clip( p.x, p.y );
   }

   /*!
    * Constrains two point coordinates \a x and \a y to stay within the
    * boundaries of this image.
    *
    * \param[out] x  Horizontal coordinate of the clipped point.
    * \param[out] y  Vertical coordinate of the clipped point.
    *
    * Returns true iff the original point location is included in this image.
    */
   template <typename T>
   bool Clip( T& x, T& y ) const
   {
      bool in = false;

      if ( x >= T( m_width ) )  x = T( m_width-1 );
      else if ( x < T( 0 ) )    x = T( 0 );
      else                      in = true;

      if ( y >= T( m_height ) ) y = T( m_height-1 );
      else if ( y < T( 0 ) )    y = T( 0 );
      else                      return in;

      return false;
   }

   /*!
    * Constrains a rectangular region \a r in image coordinates to fit into the
    * boundaries of this image. Also ensures coherence of clipped rectangular
    * coordinates such that r.x0 <= r.x1 and r.y0 <= r.y1.
    *
    * Returns true iff the original rectangle intersects this image.
    */
   template <typename T>
   bool Clip( pcl::GenericRectangle<T>& r ) const
   {
      return Clip( r.x0, r.y0, r.x1, r.y1 );
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
    */
   template <typename T>
   bool Clip( T& x0, T& y0, T& x1, T& y1 ) const
   {
      pcl::OrderRect( x0, y0, x1, y1 );

      int out = 0;

      if ( x0 >= T( m_width ) )  x0 = T( m_width ),  ++out;
      else if ( x0 < T( 0 ) )    x0 = T( 0 );

      if ( y0 >= T( m_height ) ) y0 = T( m_height ), ++out;
      else if ( y0 < T( 0 ) )    y0 = T( 0 );

      if ( x1 > T( m_width ) )   x1 = T( m_width );
      else if ( x1 <= T( 0 ) )   x1 = T( 0 ),        ++out;

      if ( y1 > T( m_height ) )  y1 = T( m_height );
      else if ( y1 <= T( 0 ) )   y1 = T( 0 ),        ++out;

      return !out;
   }

   /*!
    * Returns the number of pixels in this image, or its area in square pixels.
    */
   size_type NumberOfPixels() const
   {
      return size_type( m_width )*size_type( m_height );
   }

   /*!
    * Returns the total number of samples in this image, or the area in square
    * pixels multiplied by the number of channels, including alpha channels.
    */
   size_type NumberOfSamples() const
   {
      return NumberOfPixels()*size_type( m_numberOfChannels );
   }

   /*!
    * Returns the offset of a given pixel row \a y (also known as <em>scan
    * line</em>) from the beginning of a channel's data block.
    */
   distance_type RowOffset( int y ) const
   {
      return distance_type( y )*distance_type( m_width );
   }

   /*!
    * Returns the offset of a given pixel from the beginning of a channel's
    * data block.
    *
    * \param x    Horizontal pixel coordinate.
    * \param y    Vertical pixel coordinate.
    */
   distance_type PixelOffset( int x, int y ) const
   {
      return RowOffset( y ) + distance_type( x );
   }

protected:

   /*
    * Image geometry
    */
   struct Geometry
   {
      int width = 0;
      int height = 0;
      int numberOfChannels = 0;

      Geometry() = default;
      Geometry( const Geometry& ) = default;

      size_type NumberOfPixels() const
      {
         return size_type( width )*size_type( height );
      }

      void Assign( const Geometry& x )
      {
         width = x.width;
         height = x.height;
         numberOfChannels = x.numberOfChannels;
      }

      void Reset()
      {
         width = height = numberOfChannels = 0;
      }
   };

   Geometry* m_geometry = nullptr;

   ImageGeometry() = default;
   ImageGeometry( const ImageGeometry& ) = default;
   ImageGeometry& operator =( const ImageGeometry& ) = default;

   virtual ~ImageGeometry()
   {
      m_geometry = nullptr;
   }

   void Swap( ImageGeometry& image )
   {
      pcl::Swap( m_geometry, image.m_geometry );
   }
};

// ----------------------------------------------------------------------------

#undef m_width
#undef m_height
#undef m_numberOfChannels

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageGeometry_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageGeometry.h - Released 2017-07-09T18:07:07Z
