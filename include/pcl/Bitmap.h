// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/Bitmap.h - Released 2014/10/29 07:34:07 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#ifndef __PCL_Bitmap_h
#define __PCL_Bitmap_h

/// \file pcl/Bitmap.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_UIObject_h
#include <pcl/UIObject.h>
#endif

#ifndef __PCL_Rectangle_h
#include <pcl/Rectangle.h>
#endif

#ifndef __PCL_Color_h
#include <pcl/Color.h>
#endif

#ifndef __PCL_String_h
#include <pcl/String.h>
#endif

#ifndef __PCL_ImageRenderingModes_h
#include <pcl/ImageRenderingModes.h>
#endif

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace BitmapFormat
 * \brief Bitmap pixel value interpretation modes.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>BitmapFormat::ARGB32</td>
 *     <td>AARRGGBB format. The alpha AA component is used to represent the
 *         transparency of each pìxel: from 0=transparent to 0xFF=opaque.
 *         This is the default mode.</td></tr>
 * <tr><td>BitmapFormat::RGB32</td>
 *     <td>RRGGBB format. The alpha component is not used. This mode can be
 *         used to improve the performance of graphics output when painting on
 *         controls. This is actually necessary under X11.</td></tr>
 * </table>
 */
namespace BitmapFormat
{
   enum value_type
   {
      ARGB32,  // AARRGGBB
      RGB32,   // RRGGBB - alpha not used

      NumberOfBitmapFormats,

      Invalid = -1
   };
}

// ----------------------------------------------------------------------------

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

class PCL_CLASS ImageVariant;

// ----------------------------------------------------------------------------

/*!
 * \class Bitmap
 * \brief Client-side interface to a PixInsight %Bitmap object.
 *
 * In the PixInsight platform, bitmaps are device-independent images that can
 * be used for graphics output and allow direct pixel manipulation.
 *
 * Pixels in a bitmap are always stored as 32-bit values in the following
 * format:
 *
 * AARRGGBB
 *
 * where each letter represents a 4-bit hexadecimal digit (from 0 to F). Each
 * 8-bit pair represents a pixel component in the range from 0 to 255:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>AA</td>    <td>Alpha value, or <em>pixel opacity</em>: 0 means
 *                        completely transparent, 255 corresponds to an opaque
 *                        pixel.</td></tr>
 * <tr><td>RR</td>    <td>Red pixel color component.</td></tr>
 * <tr><td>GG</td>    <td>Green pixel color component.</td></tr>
 * <tr><td>BB</td>    <td>Blue pixel color component.</td></tr>
 * </table>
 *
 * %Bitmap is a managed, high-level object that encapsulates the properties and
 * behavior of an actual bitmap living in the PixInsight core application.
 *
 * %Bitmap provides a comprehensive set of functions and utility routines to
 * manipulate bitmap images, such as direct pixel access functions, specular
 * and affine transformations, bitwise logical operations, and high-performance
 * rendering of images as bitmaps, among many others.
 *
 * Along with the Graphics class, %Bitmap can be used for off-screen graphics
 * output. Graphics offers a thorough collection of painting routines that can
 * be used to generate sophisticated graphics as bitmaps.
 *
 * As we have said, bitmaps are device-independent images in PixInsight. It
 * must be pointed out that there is no equivalent to a \e device-dependent
 * bitmap or image in PixInsight: all device dependencies are managed
 * internally by the PixInsight core application.
 */
class PCL_CLASS Bitmap : public UIObject
{
public:

   /*!
    * Represents a bitmap pixel format. See the BitmapFormat namespace for
    * supported values.
    */
   typedef BitmapFormat::value_type    pixel_format;

   /*!
    * The %display_channel type represents a display channel supported by the
    * PixInsight core application. Valid channels are defined in the
    * DisplayChannel namespace.
    */
   typedef DisplayChannel::value_type  display_channel;

   /*!
    * The %mask_mode type represents a mask rendering mode supported by
    * the PixInsight core application. Valid mask rendering modes are defined
    * in the MaskMode namespace.
    */
   typedef MaskMode::value_type        mask_mode;

   /*!
    * Constructs an empty %Bitmap object.
    */
   Bitmap();

   /*!
    * Constructs a %Bitmap object of the specified \a width and \a height
    * dimensions, and pixel format \a fmt
    *
    * For the sake of performance, bitmap pixel values are not initialized, so
    * they will contain unpredictable \e garbage values.
    *
    * \note On X11 platforms (Linux/UNIX) without composite rendering
    * extensions, bitmaps in the default BitmapFormat::ARGB32 format are
    * painted very slowly on controls. To render a bitmap on a control, the
    * BitmapFormat::RGB32 format (alpha ignored) should be used for the sake of
    * compatibility and graphics output performance.
    */
   Bitmap( int width, int height, pixel_format fmt = BitmapFormat::ARGB32 );

   /*!
    * Constructs a %Bitmap object from a source image in the XPM format.
    *
    * \param xpm  Starting address of an XPM image description.
    *
    * \b References
    *
    * \li http://en.wikipedia.org/wiki/X_PixMap
    * \li http://www.xfree86.org/current/xpm.pdf
    */
   Bitmap( const char** xpm );

   /*!
    * Constructs a %Bitmap object by loading an embedded resource or disk file.
    *
    * \param filePath   Path to the source image file or resource. A resource
    *             path begins with the ":/" prefix. For a list of supported
    *             image file formats, see the documentation for the
    *             Load( const String& ) member function. The source file format
    *             is always determined from the specified file extension.
    *
    * For more information on embedded resources, see
    * Bitmap::Bitmap( const char* ).
    */
   Bitmap( const String& filePath );

   /*!
    * Constructs a %Bitmap object by loading an embedded resource or disk file.
    *
    * \param filePath   Path to the source image file or resource. A resource
    *             path begins with the ":/" prefix. For a list of supported
    *             image file formats, see the documentation for the
    *             Load( const String& ) member function. The source file format
    *             is always determined from the specified file extension.
    *
    * The PixInsight core application uses the Qt resource system for embedded
    * images that can be loaded dynamically from .rcc binary files. For
    * example, the following code snippet loads one of the standard core
    * application icons:
    *
    * \code
    * Bitmap icon( ":/icons/document.png" );
    * \endcode
    *
    * \b References
    *
    * \li http://qt-project.org/doc/qt-4.8/resources.html
    */
   Bitmap( const char* filePath );

   /*!
    * Constructs a %Bitmap object by loading an embedded resource or disk file.
    * This constructor is equivalent to Bitmap::Bitmap( const char* ).
    */
   Bitmap( const IsoString& filePath );

   /*!
    * Constructs a %Bitmap object from bitmap data stored in memory.
    *
    * \param data    Starting address of the source bitmap data block.
    *
    * \param size    Length of the source data in bytes.
    *
    * \param format  A nul-terminated character string identifying the format
    *                of the source data. Currently the following formats are
    *                supported by the PixInsight Core application:
    *
    * <table border="1" cellpadding="4" cellspacing="0">
    * <tr><td>%BMP</td>   <td>Windows %Bitmap</td></tr>
    * <tr><td>%GIF</td>   <td>%Graphics Interchange Format</td></tr>
    * <tr><td>%ICO</td>   <td>Windows %Icon Format</td></tr>
    * <tr><td>%JPG</td>   <td>Joint Photographic Experts Group</td></tr>
    * <tr><td>%MNG</td>   <td>Multiple Network %Graphics</td></tr>
    * <tr><td>%PBM</td>   <td>Portable Bitmap</td></tr>
    * <tr><td>%PNG</td>   <td>Portable Network %Graphics</td></tr>
    * <tr><td>%PPM</td>   <td>Portable Pixel Map</td></tr>
    * <tr><td>%SVG</td>   <td>Scalable Vector Graphics</td></tr>
    * <tr><td>%TIFF</td>  <td>Tagged %Image %File Format</td></tr>
    * <tr><td>%TGA</td>   <td>Truevision TGA Format (TARGA)</td></tr>
    * <tr><td>%XBM</td>   <td>X11 Bitmap</td></tr>
    * <tr><td>%XPM</td>   <td>X11 Pixmap</td></tr>
    * </table>
    *
    *                The default value (when no format is specified) is SVG.
    *
    * \param flags   Currently not used. This parameter is reserved for future
    *                extension and its value must be zero (the default value).
    */
   Bitmap( const void* data, size_type size, const char* format = "SVG", uint32 flags = 0 );

   /*!
    * Constructs a %Bitmap object as a subimage of an existing %Bitmap.
    *
    * \param bmp  Source bitmap.
    *
    * \param r    Rectangle that defines the source subimage, in source
    *             bitmap coordinates.
    */
   Bitmap( const Bitmap& bmp, const pcl::Rect& r );

   /*!
    * Constructs a %Bitmap object as a subimage of an existing %Bitmap.
    *
    * \param bmp  Source bitmap.
    *
    * \param x0, y0  Coordinates of the upper left corner of the rectangle that
    *             defines the source subimage, in source bitmap coordinates.
    *
    * \param x1, y1  Coordinates of the lower right corner of the rectangle
    *             that defines the source subimage, in source bitmap
    *             coordinates.
    */
   Bitmap( const Bitmap& bmp, int x0, int y0, int x1, int y1 );

   /*!
    * Copy constructor. This object will reference the same server-side bitmap
    * as the specified instance \a b.
    */
   Bitmap( const Bitmap& bmp ) : UIObject( bmp )
   {
   }

   /*!
    * Destroys a %Bitmap object.
    *
    * The actual bitmap object that this %Bitmap instance refers to lives in
    * the PixInsight core application, and as such, it will be destroyed and
    * deallocated (garbage-collected) only when no more references exist to it
    * in other high-level managed objects, such as other instances of %Bitmap
    * living in installed modules, or some element of the graphical interface.
    */
   virtual ~Bitmap()
   {
   }

   /*!
    * Assignment operator. Returns a reference to this object.
    *
    * Makes this object reference the same server-side bitmap as the specified
    * instance \a bmp. If the previous bitmap becomes unreferenced, it will be
    * garbage-collected by the PixInsight core application.
    */
   Bitmap& operator =( const Bitmap& bmp )
   {
      SetHandle( bmp.handle );
      return *this;
   }

   /*!
    * Returns a reference to a null %Bitmap instance. A null %Bitmap does not
    * correspond to an existing bitmap in the PixInsight core application.
    */
   static Bitmap& Null();

   /*!
    * Returns the current pixel format for this bitmap.
    *
    * Supported pixel format values are enumerated in the BitmapFormat
    * namespace.
    */
   pixel_format PixelFormat() const;

   /*!
    * Sets the pixel format for this bitmap. If necessary, pixel values will
    * be converted internally to match the specified format.
    *
    * Supported pixel format values are enumerated in the BitmapFormat
    * namespace.
    *
    * \note On old X11 platforms (Linux/UNIX) without composite rendering
    * extensions, bitmaps in the default BitmapFormat::ARGB32 format are
    * painted very slowly on controls. To render a bitmap on a control, the
    * BitmapFormat::RGB32 format (alpha ignored) should be used for the sake of
    * compatibility and graphics output performance.
    */
   void SetPixelFormat( pixel_format fmt );

   /*!
    * Renders an image as a bitmap.
    *
    * \param image   The source image to be rendered. It can be a PCL image of
    *             any supported sample type.
    *
    * \param zoom    Zoom factor. If zoom == 1, the image will be rendered at
    *             its actual dimensions in pixels. Positive zoom factors are
    *             enlarging ratios; for example, if zoom == 2, the image will
    *             be rendered zoomed 2:1 (twice its actual sizes). Negative
    *             zoom factors are reduction ratios: zoom = -3 means zoomed
    *             1:3, or one third of the actual image dimensions.
    *
    * \param displayChannel   See the DisplayChannel enumeration for possible
    *             values. The default mode is DisplayChannel::RGBK, which means
    *             that the image will be rendered RGB/gray composite mode.
    *
    * \param transparency     If this parameter is true and the image has one
    *             or more alpha channels, the image will be rendered over a
    *             transparent background and the first alpha channel will
    *             define pixel opacity. If this parameter is false, alpha
    *             channels will not be interpreted as transparency masks.
    *
    * \param mask    If nonzero, this is the address of an image that will be
    *             treated as a mask acting for the source image. Mask pixels
    *             modify the image rendition according to the specified mask
    *             rendering mode (see the maskMode paramter). If specified, the
    *             mask image must have the same dimensions as the source image.
    *
    * \param maskMode   This parameter defines the mask rendering mode to be
    *             used if a mask image has been specified. See the MaskMode
    *             enumeration for supported values. The default mode is
    *             MaskMode::Default, which corresponds to the red overlay mask
    *             rendering mode.
    *
    * \param maskInverted     If \a mask is nonzero and this parameter is true,
    *             mask pixels will be inverted before rendering them over
    *             source image pixels.
    *
    * \param LUT     If nonzero, this is the address of a <em>look-up
    *             table</em> that will be used to remap bitmap pixel values.
    *
    * \param fastDownsample   If this parameter is true, a fast sparse
    *             interpolation algorithm is used to render images with
    *             negative zoom factors less than -2. Fast subsampling
    *             interpolation is considerably faster than normal (slow)
    *             interpolation, but the generated renditions are less
    *             accurate. In general though, fast interpolation errors are
    *             barely noticeable. This parameter is true by default.
    *
    * \param callback   If nonzero, this is the address of a callback routine
    *             that will be invoked during the bitmap rendition procedure.
    *             If the callback routine returns false, the rendition is
    *             aborted and a null bitmap (Bitmap::Null()) is returned. By
    *             default this parameter is zero.
    *
    * This is a high-performance routine, implemented with highly optimized
    * parallel code, that can be used for real-time image visualization. If
    * allowed through global preferences, it will use all available processors
    * and processor cores, via concurrent threads.
    */
   static Bitmap Render( const ImageVariant& image,
            int zoom = 1, display_channel displayChannel = DisplayChannel::RGBK, bool transparency = true,
            const ImageVariant* mask = 0, mask_mode maskMode = MaskMode::Default, bool maskInverted = false,
            const uint8** LUT = 0, bool fastDownsample = true, bool (*callback)() = 0 );

   /*!
    * Obtains the dimensions (width, height) of this bitmap in pixels.
    */
   void GetDimensions( int& width, int& height ) const;

   /*!
    * Returns the width of this bitmap in pixels.
    */
   int Width() const
   {
      int w, dum; GetDimensions( w, dum ); return w;
   }

   /*!
    * Returns the height of this bitmap in pixels.
    */
   int Height() const
   {
      int dum, h; GetDimensions( dum, h ); return h;
   }

   /*!
    * Returns the bounding rectangle of this bitmap.
    *
    * The upper-left corner of the returned rectangle is always at (0,0). The
    * width and height of the returned rectangle are the bitmap dimensions.
    */
   pcl::Rect Bounds() const
   {
      int w, h; GetDimensions( w, h ); return pcl::Rect( w, h );
   }

   /*!
    * Returns true if this bitmap is empty. An empty bitmap does not contain an
    * image, so it has zero dimensions.
    */
   bool IsEmpty() const;

   /*!
    * Returns true if this bitmap contains an image, i.e. if it is not empty.
    *
    * \note This operator member function returns !IsEmpty().
    */
   operator bool() const
   {
      return !IsEmpty();
   }

   /*!
    * Returns the value of a bitmap pixel.
    *
    * \param x,y  Bitmap coordinates of the pixel to read. Pixel coordinates
    *             can vary in the range from (0,0) to (Width()-1, Height()-1).
    */
   RGBA Pixel( int x, int y ) const;

   /*!
    * Returns the value of a bitmap pixel.
    *
    * \param p    A Point object whose coordinates identify the pixel to read.
    *             Pixel coordinates can vary in the range from (0,0) to
    *             (Width()-1, Height()-1).
    */
   RGBA Pixel( const pcl::Point& p ) const
   {
      return Pixel( p.x, p.y );
   }

   /*!
    * Sets the value of a bitmap pixel.
    *
    * \param x,y  Bitmap coordinates of the pixel to write. Pixel coordinates
    *             can vary in the range from (0,0) to (Width()-1, Height()-1).
    *
    * \param v    32-bit pixel value encoded in the AARRGGBB format: AA is the
    *             alpha (transparency) value, RR is the red component, GG is
    *             green and BB is blue. Each element is an 8-bit value.
    */
   void SetPixel( int x, int y, RGBA v );

   /*!
    * Sets the value of a bitmap pixel.
    *
    * \param p    A Point object whose coordinates identify the pixel to write.
    *             Pixel coordinates can vary in the range from (0,0) to
    *             (Width()-1, Height()-1).
    *
    * \param v    32-bit pixel value encoded in the AARRGGBB format: AA is the
    *             alpha (transparency) value, RR is the red component, GG is
    *             green and BB is blue. Each element is an 8-bit value.
    */
   void SetPixel( const pcl::Point& p, RGBA v )
   {
      SetPixel( p.x, p.y, v );
   }

   /*!
    * Returns the (constant) starting address of a row of pixels in this
    * bitmap.
    *
    * \param y    Vertical coordinate of the row of pixels. Vertical
    *             coordinates can vary in the range from 0 to Height()-1.
    */
   const RGBA* ScanLine( int y ) const;

   /*!
    * Returns the starting address of a row of pixels in this bitmap.
    *
    * \param y    Vertical coordinate of the row of pixels. Vertical
    *             coordinates can vary in the range from 0 to Height()-1.
    *
    * The returned address can be used to modify pixel values.
    */
   RGBA* ScanLine( int y );

   /*!
    * \defgroup bitmap_specular_transformations Specular Bitmap Transformations
    */

   /*!
    * Returns a mirrored duplicate of this bitmap. The original (this) bitmap
    * is not modified.
    *
    * \ingroup bitmap_specular_transformations
    */
   Bitmap Mirrored() const;

   /*!
    * Returns a duplicate of this bitmap, mirrored horizontally. The original
    * (this) bitmap is not modified.
    *
    * \ingroup bitmap_specular_transformations
    */
   Bitmap MirroredHorizontally() const;

   /*!
    * Returns a duplicate of this bitmap, mirrored vertically. The original
    * (this) bitmap is not modified.
    *
    * \ingroup bitmap_specular_transformations
    */
   Bitmap MirroredVertically() const;

   /*!
    * \defgroup bitmap_affine_transformations Affine Bitmap Transformations
    */

   /*!
    * Returns a scaled duplicate of this bitmap.
    *
    * \param sx,sy   Scaling factors in the X and Y axes.
    *
    * \param precise If this parameter is true, the scaled bitmap will be
    *                generated using a precise, smooth interpolation algorithm.
    *                This parameter is true by default.
    *
    * \ingroup bitmap_affine_transformations
    */
   Bitmap Scaled( double sx, double sy, bool precise = true ) const;

   /*!
    * Returns a scaled duplicate of this bitmap.
    *
    * \param sxy     Scaling factor applied to both X and Y axes.
    *
    * \param precise If this parameter is true, the scaled bitmap will be
    *                generated using a precise, smooth interpolation algorithm.
    *                This parameter is true by default.
    *
    * \ingroup bitmap_affine_transformations
    */
   Bitmap Scaled( double sxy, bool precise = true ) const;

   /*!
    * Returns a scaled duplicate of this bitmap, with its width equal to a
    * prescribed value in pixels.
    *
    * \param width   Width in pixels of the returned bitmap. If this parameter
    *                is zero, an empty bitmap will be returned.
    *
    * \param precise If this parameter is true, the scaled bitmap will be
    *                generated using a precise, smooth interpolation algorithm.
    *                This parameter is true by default.
    *
    * \ingroup bitmap_affine_transformations
    */
   Bitmap ScaledToWidth( int width, bool precise = true ) const;

   /*!
    * Returns a scaled duplicate of this bitmap, with its height equal to a
    * prescribed value in pixels.
    *
    * \param height  Height in pixels of the returned bitmap. If this parameter
    *                is zero, an empty bitmap will be returned.
    *
    * \param precise If this parameter is true, the scaled bitmap will be
    *                generated using a precise, smooth interpolation algorithm.
    *                This parameter is true by default.
    *
    * \ingroup bitmap_affine_transformations
    */
   Bitmap ScaledToHeight( int height, bool precise = true ) const;

   /*!
    * Returns a scaled duplicate of this bitmap, with its width and height
    * equal to prescribed values in pixels.
    *
    * \param width, height    Width and height in pixels of the returned
    *                bitmap. If one or both of these parameters are zero, an
    *                empty bitmap will be returned.
    *
    * \param precise If this parameter is true, the scaled bitmap will be
    *                generated using a precise, smooth interpolation algorithm.
    *                This parameter is true by default.
    *
    * \ingroup bitmap_affine_transformations
    */
   Bitmap ScaledToSize( int width, int height, bool precise = true ) const;

   /*!
    * Returns a rotated duplicate of this bitmap.
    *
    * \param angle   %Rotation angle in radians.
    *
    * \param precise If this parameter is true, the rotated bitmap will be
    *                generated using a precise, smooth interpolation algorithm.
    *                This parameter is true by default.
    *
    * \ingroup bitmap_affine_transformations
    */
   Bitmap Rotated( double angle, bool precise = true ) const;

   /*!
    * \defgroup bitmap_file_io Bitmap File I/O Routines
    */

   /*!
    * Loads a disk image file in this bitmap. Returns true if the file was
    * successfully loaded; false in the event of error.
    *
    * \param filePath   %File path to the input image file.
    *
    * The input file format is always determined by the specified file
    * extension (in \a filePath). Supported input formats:
    *
    * <table border="1" cellpadding="4" cellspacing="0">
    * <tr><td>%BMP</td>   <td>Windows %Bitmap (.bmp)</td></tr>
    * <tr><td>%GIF</td>   <td>%Graphics Interchange Format (.gif) (read-only)</td></tr>
    * <tr><td>%ICO</td>   <td>Windows %Icon Format (.ico)</td></tr>
    * <tr><td>%JPEG</td>  <td>Joint Photographic Experts Group (.jpg, .jpeg)</td></tr>
    * <tr><td>%MNG</td>   <td>Multiple Network %Graphics (.mng) (read-only)</td></tr>
    * <tr><td>%PBM</td>   <td>Portable BitMap (.pbm) (read-only)</td></tr>
    * <tr><td>%PNG</td>   <td>Portable Network %Graphics (.png)</td></tr>
    * <tr><td>%PPM</td>   <td>Portable Pixel Map (.ppm)</td></tr>
    * <tr><td>%SVG</td>   <td>Scalable Vector Graphics (.svg) (read-only)</td></tr>
    * <tr><td>%TIFF</td>  <td>Tagged %Image %File Format (.tif, .tiff).</td></tr>
    * <tr><td>%TGA</td>   <td>Truevision TGA Format (TARGA) (.tga, .tpic).</td></tr>
    * <tr><td>%XBM</td>   <td>X BitMap (.xbm)</td></tr>
    * <tr><td>%XPM</td>   <td>X PixMap (.xpm)</td></tr>
    * </table>
    *
    * \ingroup bitmap_file_io
    */
   bool Load( const String& filePath );

   /*!
    * Saves this bitmap to a disk image file. Returns true if the file was
    * successfully written; false in the event of error.
    *
    * \param filePath   %File path to the output image file.
    *
    * \param quality    %JPEG quality in the range from 0 (lowest quality) to
    *             100 (highest quality). If the output file is not in the JPEG
    *             format, this parameter is ignored. If this parameter is < 0,
    *             a default quality is used.
    *
    * The output file format is always determined by the specified file
    * extension (in \a filePath). Supported output formats include:
    *
    * <table border="1" cellpadding="4" cellspacing="0">
    * <tr><td>%BMP</td>   <td>Windows %Bitmap (.bmp)</td></tr>
    * <tr><td>%ICO</td>   <td>Windows %Icon Format (.ico)</td></tr>
    * <tr><td>%JPEG</td>  <td>Joint Photographic Experts Group (.jpg, .jpeg)</td></tr>
    * <tr><td>%PNG</td>   <td>Portable Network %Graphics (.png)</td></tr>
    * <tr><td>%PPM</td>   <td>Portable Pixel Map (.ppm)</td></tr>
    * <tr><td>%TIFF</td>  <td>Tagged %Image %File Format (.tif, .tiff).</td></tr>
    * <tr><td>%XBM</td>   <td>X BitMap (.xbm)</td></tr>
    * <tr><td>%XPM</td>   <td>X PixMap (.xpm)</td></tr>
    * </table>
    *
    * \ingroup bitmap_file_io
    */
   bool Save( const String& filePath, int quality = -1 ) const;

   /*!
    * Loads this bitmap from data stored in memory.
    *
    * \param data    Starting address of the source bitmap data block.
    *
    * \param size    Length of the source data in bytes.
    *
    * \param format  A nul-terminated character string identifying the format
    *                of the source data. Currently the only supported format is
    *                Scalable Vector Graphics (SVG), and this is also the
    *                default value of this parameter ("SVG").
    *
    * \param flags   Currently not used. This parameter is reserved for future
    *                extension and its value must be zero (the default value).
    */
   bool Load( const void* data, size_type size, const char* format = "SVG", uint32 flags = 0 );

   /*!
    * Returns a subimage of this bitmap.
    *
    * \param x0, y0  Coordinates of the upper left corner of the source
    *             rectangular region.
    *
    * \param x1, y1  Coordinates of the lower right corner of the source
    *             rectangular region.
    *
    * The rectangular region defines the size and position of the subimage that
    * is extracted and returned as a new %Bitmap object.
    */
   Bitmap Subimage( int x0, int y0, int x1, int y1 ) const;

   /*!
    * Returns a subimage of this bitmap.
    *
    * \param r    A Rect object defining subimage boundaries.
    *
    * The rectangular area defines the size and position of the subimage that
    * is extracted and returned as a new %Bitmap object.
    */
   Bitmap Subimage( const pcl::Rect& r ) const
   {
      return Subimage( r.x0, r.y0, r.x1, r.y1 );
   }

   /*!
    * \defgroup bitmap_bitwise_ops Bitmap Bitwise Logical Operations
    */

   /*!
    * Copies a region of a source bitmap to the specified location on this
    * bitmap.
    *
    * \param p    Position of the upper left corner of the target rectangle on
    *             this bitmap.
    *
    * \param src  Source bitmap, whose pixels will be copied to this bitmap.
    *
    * \param r    Defines the rectangular area that will be copied, in source
    *             bitmap coordinates.
    *
    * Target pixels on this bitmap are replaced with pixels read from the
    * source bitmap \a src. Source pixels falling outside the boundaries of
    * this bitmap are ignored.
    *
    * \ingroup bitmap_bitwise_ops
    */
   void Copy( const pcl::Point& p, const Bitmap& src, const pcl::Rect& r );

   /*!
    * Copies all pixels from a source bitmap to the specified location on this
    * bitmap.
    *
    * \param p    Position of the upper left corner of the target rectangle on
    *             this bitmap.
    *
    * \param src  Source bitmap, whose pixels will be copied to this bitmap.
    *
    * Target pixels on this bitmap are replaced with pixels read from the
    * source bitmap \a src. Source pixels falling outside the boundaries of
    * this bitmap are ignored.
    *
    * This function is equivalent to:
    *
    * Copy( p, src, src.Bounds() );
    *
    * \ingroup bitmap_bitwise_ops
    */
   void Copy( const pcl::Point& p, const Bitmap& src )
   {
      Copy( p, src, src.Bounds() );
   }

   /*!
    * Copies all pixels from a source bitmap to this bitmap.
    *
    * \param src  Source bitmap, whose pixels will be copied to this bitmap.
    *
    * Target pixels on this bitmap are replaced with pixels read from the
    * source bitmap \a src. Pixels are copied at the upper left corner of this
    * image (0,0). Source pixels falling outside the boundaries of this bitmap
    * are ignored.
    *
    * This function is equivalent to:
    *
    * Copy( pcl::Point( 0, 0 ), src );
    *
    * \ingroup bitmap_bitwise_ops
    */
   void Copy( const Bitmap& src )
   {
      Copy( pcl::Point( 0 ), src );
   }

   /*!
    * Fills a rectangular region of this bitmap with a constant value.
    *
    * \param rect       Rectangular region to be filled.
    * \param fillWith   Pixel value to fill with.
    *
    * \ingroup bitmap_bitwise_ops
    */
   void Fill( const pcl::Rect& rect, RGBA fillWith );

   /*!
    * Fills the entire bitmap with a constant value \a fillWith. This
    * function is equivalent to:
    *
    * Fill( Bounds(), fillWith );
    *
    * \ingroup bitmap_bitwise_ops
    */
   void Fill( RGBA fillWith )
   {
      Fill( Bounds(), fillWith );
   }

   /*!
    * Replaces each pixel in a rectangular region of this bitmap with the
    * result of the bitwise inclusive OR operation between its current value
    * and a specified constant value.
    *
    * \param rect       Rectangular region to be operated.
    * \param orWith     Value to be ORed with current pixel values.
    *
    * \ingroup bitmap_bitwise_ops
    */
   void Or( const pcl::Rect& rect, RGBA orWith );

   /*!
    * Replaces each pixel in this bitmap with the result of the bitwise
    * inclusive OR operation between its current value and a specified constant
    * value \a andWith.
    *
    * This function is equivalent to:
    *
    * Or( Bounds(), orWith );
    *
    * \ingroup bitmap_bitwise_ops
    */
   void Or( RGBA orWith )
   {
      Or( Bounds(), orWith );
   }

   /*!
    * Replaces a rectangular region of this bitmap with the bitwise inclusive
    * OR operation between its current pixel values and the pixels of a
    * corresponding region of a source bitmap.
    *
    * \param p    Position of the upper left corner of the target rectangle on
    *             this bitmap.
    *
    * \param src  Source bitmap whose pixels will be ORed with this bitmap.
    *
    * \param r    Defines the rectangular area that will be operated, in source
    *             bitmap coordinates.
    *
    * Target pixels on this bitmap are replaced with the result of the bitwise
    * OR operation with the corresponding pixels read from the source bitmap
    * \a src. Source pixels falling outside the boundaries of this bitmap are
    * ignored.
    *
    * \ingroup bitmap_bitwise_ops
    */
   void Or( const pcl::Point& p, const Bitmap& src, const pcl::Rect& r );

   /*!
    * Replaces a rectangular region of this bitmap with the bitwise inclusive
    * OR operation between its current pixel values and the corresponding
    * pixels of a source bitmap.
    *
    * \param p    Position of the upper left corner of the target rectangle on
    *             this bitmap.
    *
    * \param src  Source bitmap whose pixels will be ORed with this bitmap.
    *
    * Target pixels on this bitmap are replaced with the result of the bitwise
    * OR operation with the corresponding pixels read from the source bitmap
    * \a src. Source pixels falling outside the boundaries of this bitmap are
    * ignored.
    *
    * This function is equivalent to:
    *
    * Or( p, src, src.Bounds() );
    *
    * \ingroup bitmap_bitwise_ops
    */
   void Or( const pcl::Point& p, const Bitmap& src )
   {
      Or( p, src, src.Bounds() );
   }

   /*!
    * Replaces all pixels in this bitmap with the bitwise inclusive OR
    * operation between their current values and the pixels of a source bitmap
    * \a src. This function is equivalent to:
    *
    * Or( Point( 0, 0 ), src );
    *
    * \ingroup bitmap_bitwise_ops
    */
   void Or( const Bitmap& src )
   {
      Or( pcl::Point( 0 ), src );
   }

   /*!
    * Replaces each pixel in a rectangular region of this bitmap with the
    * result of the bitwise AND operation between its current value and a
    * specified constant value.
    *
    * \param rect       Rectangular region to be operated.
    * \param andWith    Value to be ANDed with current pixel values.
    *
    * \ingroup bitmap_bitwise_ops
    */
   void And( const pcl::Rect& rect, RGBA andWith );

   /*!
    * Replaces each pixel in this bitmap with the result of the bitwise
    * AND operation between its current value and a specified constant value
    * \a andWith.
    *
    * This function is equivalent to:
    *
    * And( Bounds(), andWith );
    *
    * \ingroup bitmap_bitwise_ops
    */
   void And( RGBA andWith )
   {
      And( Bounds(), andWith );
   }

   /*!
    * Replaces a rectangular region of this bitmap with the bitwise AND
    * operation between its current pixel values and the pixels of a
    * corresponding region of a source bitmap.
    *
    * \param p    Position of the upper left corner of the target rectangle on
    *             this bitmap.
    *
    * \param src  Source bitmap whose pixels will be ANDed with this bitmap.
    *
    * \param r    Defines the rectangular area that will be operated, in source
    *             bitmap coordinates.
    *
    * Target pixels on this bitmap are replaced with the result of the bitwise
    * AND operation with the corresponding pixels read from the source bitmap
    * \a src. Source pixels falling outside the boundaries of this bitmap are
    * ignored.
    *
    * \ingroup bitmap_bitwise_ops
    */
   void And( const pcl::Point& p, const Bitmap& src, const pcl::Rect& r );

   /*!
    * Replaces a rectangular region of this bitmap with the bitwise AND
    * operation between its current pixel values and the corresponding pixels
    * of a source bitmap.
    *
    * \param p    Position of the upper left corner of the target rectangle on
    *             this bitmap.
    *
    * \param src  Source bitmap whose pixels will be ANDed with this bitmap.
    *
    * Target pixels on this bitmap are replaced with the result of the bitwise
    * AND operation with the corresponding pixels read from the source bitmap
    * \a src. Source pixels falling outside the boundaries of this bitmap are
    * ignored.
    *
    * This function is equivalent to:
    *
    * And( p, src, src.Bounds() );
    *
    * \ingroup bitmap_bitwise_ops
    */
   void And( const pcl::Point& p, const Bitmap& src )
   {
      And( p, src, src.Bounds() );
   }

   /*!
    * Replaces all pixels in this bitmap with the bitwise AND operation between
    * their current values and the pixels of a source bitmap \a src. This
    * function is equivalent to:
    *
    * And( Point( 0, 0 ), src );
    *
    * \ingroup bitmap_bitwise_ops
    */
   void And( const Bitmap& src )
   {
      And( pcl::Point( 0 ), src );
   }

   /*!
    * Replaces each pixel in a rectangular region of this bitmap with the
    * result of the bitwise exclusive OR (XOR) operation between its current
    * value and a specified constant value.
    *
    * \param rect       Rectangular region to be operated.
    * \param xorWith    Value to be XORed with current pixel values.
    *
    * \ingroup bitmap_bitwise_ops
    */
   void Xor( const pcl::Rect& rect, RGBA xorWith );

   /*!
    * Replaces each pixel in this bitmap with the result of the bitwise
    * exclusive OR (XOR) operation between its current value and a specified
    * constant value \a xorWith.
    *
    * This function is equivalent to:
    *
    * Xor( Bounds(), xorWith );
    *
    * \ingroup bitmap_bitwise_ops
    */
   void Xor( RGBA xorWith )
   {
      Xor( Bounds(), xorWith );
   }

   /*!
    * Replaces a rectangular region of this bitmap with the bitwise exclusive
    * OR (XOR) operation between its current pixel values and the pixels of a
    * corresponding region of a source bitmap.
    *
    * \param p    Position of the upper left corner of the target rectangle on
    *             this bitmap.
    *
    * \param src  Source bitmap whose pixels will be XORed with this bitmap.
    *
    * \param r    Defines the rectangular area that will be operated, in source
    *             bitmap coordinates.
    *
    * Target pixels on this bitmap are replaced with the result of the XOR
    * operation with the corresponding pixels read from the source bitmap
    * \a src. Source pixels falling outside the boundaries of this bitmap are
    * ignored.
    *
    * \ingroup bitmap_bitwise_ops
    */
   void Xor( const pcl::Point& p, const Bitmap& src, const pcl::Rect& r );

   /*!
    * Replaces a rectangular region of this bitmap with the bitwise exclusive
    * OR (XOR) operation between its current pixel values and the corresponding
    * pixels of a source bitmap.
    *
    * \param p    Position of the upper left corner of the target rectangle on
    *             this bitmap.
    *
    * \param src  Source bitmap whose pixels will be XORed with this bitmap.
    *
    * Target pixels on this bitmap are replaced with the result of the
    * exclusive OR (XOR) operation with the corresponding pixels read from the
    * source bitmap \a src. Source pixels falling outside the boundaries of
    * this bitmap are ignored.
    *
    * This function is equivalent to:
    *
    * Xor( p, src, src.Bounds() );
    *
    * \ingroup bitmap_bitwise_ops
    */
   void Xor( const pcl::Point& p, const Bitmap& src )
   {
      Xor( p, src, src.Bounds() );
   }

   /*!
    * Replaces all pixels in this bitmap with the bitwise exclusive OR (XOR)
    * operation between their current values and the pixels of a source bitmap
    * \a src. This function is equivalent to:
    *
    * Xor( Point( 0, 0 ), src );
    *
    * \ingroup bitmap_bitwise_ops
    */
   void Xor( const Bitmap& src )
   {
      Xor( pcl::Point( 0 ), src );
   }

   /*!
    * Replaces the pixels pertaining to a rectangle defined on this bitmap with
    * the bitwise exclusive OR (XOR) operation between their current values and
    * a specified constant value.
    *
    * \param rect       Rectangle that will be operated.
    *
    * \param xorWith    Value to be XORed with current pixel values.
    *
    * This function does not operate on a rectangular region; only the \e sides
    * of the specified rectangle are XORed: two one-pixel columns and two
    * one-pixel rows.
    *
    * \ingroup bitmap_bitwise_ops
    */
   void XorRect( const pcl::Rect& rect, RGBA xorWith );

   /*!
    * Replaces the pixels pertaining to the bounding rectangle of this bitmap
    * with the bitwise exclusive OR (XOR) operation between their current
    * values and a specified constant value.
    *
    * \param xorWith    Value to be XORed with current pixel values.
    *
    * This function is equivalent to:
    *
    * XorRect( Bounds(), xorWith );
    *
    * \ingroup bitmap_bitwise_ops
    */
   void XorRect( RGBA xorWith )
   {
      XorRect( Bounds(), xorWith );
   }

   /*!
    * Inverts a rectangular region of this bitmap.
    *
    * \param rect    Rectangular region to be inverted.
    *
    * This function does not invert alpha pixel components, but just color
    * components. It is equivalent to:
    *
    * Xor( rect, 0x00ffffff );
    *
    * \ingroup bitmap_bitwise_ops
    */
   void Invert( const pcl::Rect& rect )
   {
      Xor( rect, 0x00ffffff );
   }

   /*!
    * Inverts all pixels of this bitmap.
    *
    * This function does not invert alpha pixel components, but just color
    * components. It is equivalent to:
    *
    * Invert( Bounds() );
    *
    * \ingroup bitmap_bitwise_ops
    */
   void Invert()
   {
      Invert( Bounds() );
   }

   /*!
    * Inverts the pixels pertaining to a rectangle \a rect defined on this
    * bitmap.
    *
    * This function does not invert alpha pixel components, but just color
    * components. It is equivalent to:
    *
    * XorRect( rect, 0x00ffffff );
    *
    * \ingroup bitmap_bitwise_ops
    */
   void InvertRect( const pcl::Rect& rect )
   {
      XorRect( rect, 0x00ffffff );
   }

   /*!
    * Inverts the pixels pertaining to the bounding rectangle of this bitmap.
    *
    * This function does not invert alpha pixel components, but just color
    * components. It is equivalent to:
    *
    * InvertRect( Bounds() );
    *
    * \ingroup bitmap_bitwise_ops
    */
   void InvertRect()
   {
      InvertRect( Bounds() );
   }

   /*!
    * Replaces all occurrences of a specified pixel value in a rectangular
    * region of this bitmap with a new value.
    *
    * \param rect          Rectangular region.
    * \param replaceThis   Pixel value to be replaced.
    * \param replaceWith   New pixel value for replacement.
    */
   void ReplaceColor( const pcl::Rect& rect, RGBA replaceThis, RGBA replaceWith );

   /*!
    * Replaces all occurrences of a specified pixel value in this bitmap with a
    * new value.
    *
    * \param replaceThis   Pixel value to be replaced.
    * \param replaceWith   New pixel value for replacement.
    *
    * This function is equivalent to:
    *
    * ReplaceColor( Bounds(), replaceThis, replaceWith );
    */
   void ReplaceColor( RGBA replaceThis, RGBA replaceWith )
   {
      ReplaceColor( Bounds(), replaceThis, replaceWith );
   }

   /*!
    * Replaces the alpha (transparency) components of all pixels in a
    * rectangular region of this bitmap with a new alpha value.
    *
    * \param rect          Rectangular region.
    * \param newAlpha      New alpha (transparency) value in the range [0,255].
    */
   void SetAlpha( const pcl::Rect& rect, uint8 newAlpha );

   /*!
    * Replaces the alpha (transparency) components of all pixels in this bitmap
    * with a new alpha value.
    *
    * \param newAlpha      New alpha (transparency) value in the range [0,255].
    *
    * This function is equivalent to:
    *
    * SetAlpha( Bounds(), newAlpha );
    */
   void SetAlpha( uint8 newAlpha )
   {
      SetAlpha( Bounds(), newAlpha );
   }

private:

   Bitmap( void* );
   virtual void* CloneHandle() const;

   friend class GraphicsContextBase;
   friend class Graphics;
   friend class VectorGraphics;
   friend class Action;
   friend class Brush;
   friend class Cursor;
   friend class Button;
   friend class PushButton;
   friend class ToolButton;
   friend class BitmapBox;
   friend class TabBox;
   friend class ComboBox;
   friend class TreeBox;
   friend class ImageView;
   friend class ImageWindow;
   friend class Process;
   friend class FileFormat;
};

// ----------------------------------------------------------------------------

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

} // pcl

#endif   // __PCL_Bitmap_h

// ****************************************************************************
// EOF pcl/Bitmap.h - Released 2014/10/29 07:34:07 UTC
