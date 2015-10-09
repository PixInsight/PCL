//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// pcl/ImageView.h - Released 2015/10/08 11:24:12 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_ImageView_h
#define __PCL_ImageView_h

/// \file pcl/ImageView.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_AutoPointer_h
#include <pcl/AutoPointer.h>
#endif

#ifndef __PCL_ScrollBox_h
#include <pcl/ScrollBox.h>
#endif

#ifndef __PCL_ImageWindow_h
#include <pcl/ImageWindow.h>
#endif

#ifndef __PCL_ImageVariant_h
#include <pcl/ImageVariant.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ImageView
 * \brief Client-side interface to a PixInsight %ImageView control.
 *
 * ### TODO: Write a detailed description for %ImageView.
 *
 * \sa ImageWindow
 */
class PCL_CLASS ImageView : public ScrollBox
{
public:

   /*!
    * Represents a GUI working mode. Valid modes are defined in the ImageMode
    * namespace.
    */
   typedef ImageMode::value_type          gui_mode;

   /*!
    * Represents a display image channel supported by the PixInsight core
    * application. Valid display channels and components are defined in the
    * DisplayChannel namespace.
    */
   typedef DisplayChannel::value_type     display_channel;

   /*!
    * Represents a transparency background brush supported by the PixInsight
    * core application. Valid transparency brushes are defined in the
    * BackgroundBrush namespace.
    */
   typedef BackgroundBrush::value_type    background_brush;

   /*!
    * Represents a transparency rendering mode supported by the PixInsight core
    * application. Valid transparency rendering modes are defined in the
    * TransparencyMode namespace.
    */
   typedef TransparencyMode::value_type   transparency_mode;

   /*!
    * Creates a new %ImageView control with the specified image parameters.
    *
    * \param parent  Parent control of the newly created %ImageView.
    *
    * \param width   %Image width in pixels. Must be >= 0. There is no specific
    *                upper limit. If zero is specified, the %ImageView will
    *                contain an empty image. The default value is zero.
    *
    * \param height  %Image width in pixels. Must be >= 0. There is no specific
    *                upper limit. If zero is specified, the %ImageView will
    *                contain an empty image. The default value is zero.
    *
    * \param numberOfChannels    Number of channels in the image. Must be >= 0.
    *                There is no specific upper limit. If zero is specified,
    *                the %ImageView will contain an empty image. If a value > 0
    *                is specified, it should not be less than the number of
    *                nominal channels: 1 for grayscale images, 3 for color
    *                images. Channels in excess of nominal channels are
    *                generated as alpha channels. The default value is one.
    *
    * \param bitsPerSample    Number of bits per sample. Supported values are
    *                8, 16 and 32 for integer images; 32 and 64 for floating
    *                point images. The default value is 32.
    *
    * \param floatSample      True to create a floating point image; false to
    *                create an integer image. The default value is true.
    *
    * \param color   True to create an RGB color image; false to create a
    *                grayscale image. The default value is false.
    *
    * If not empty, all nominal and alpha channels of the newly created image
    * are initialized with zero pixel sample values (black pixels).
    */
   ImageView( Control& parent = Control::Null(),
              int width = 0, int height = 0, int numberOfChannels = 1,
              int bitsPerSample = 32, bool floatSample = true, bool color = false );

   /*!
    * Destroys an %ImageView control.
    */
   virtual ~ImageView()
   {
   }

   /*!
    * Returns an ImageVariant instance that transports the image owned by this
    * %ImageView control.
    *
    * The returned ImageVariant object transports a <em>shared image</em>. A
    * shared image is a managed alias for an actual image living in the
    * PixInsight core application. This is because a GUI control is also a
    * managed object.
    */
   ImageVariant Image() const;

   /*!
    * Returns true iff this %ImageView control holds a color image, false if it
    * is a grayscale image.
    */
   bool IsColorImage() const;

   /*!
    * Copies the width and height in pixels, and the number of channels of the
    * image in this %ImageView control to the specified variables.
    */
   void GetImageGeometry( int& width, int& height, int& numberOfChannels ) const;

   /*!
    * Returns the width in pixels of the image in this %ImageView control.
    */
   int ImageWidth() const
   {
      int w, dum; GetImageGeometry( w, dum, dum ); return w;
   }

   /*!
    * Returns the height in pixels of the image in this %ImageView control.
    */
   int ImageHeight() const
   {
      int dum, h; GetImageGeometry( dum, h, dum ); return h;
   }

   /*!
    * Returns the bounding rectangle of the image in this %ImageView control.
    * The upper left corner of the returned rectangle (x0, y0) is always (0,0).
    * The lower right corner coordinates (x1, y1) correspond to the width and
    * height of the image.
    */
   Rect ImageBounds() const
   {
      int w, h, dum; GetImageGeometry( w, h, dum ); return Rect( w, h );
   }

   /*!
    * Retrieves the pixel sample data format used by the image owned by this
    * %ImageView control.
    *
    * \param[out] bitsPerSample  On output, will be equal to the number of bits
    *                            per sample used by the image. Can be 8, 16 or
    *                            32 for an integer image; 32 or 64 for a
    *                            floating point image.
    *
    * \param[out] floatSample    On output, will be true if the image uses
    *                            floating point samples; false if it uses
    *                            integer samples.
    */
   void GetSampleFormat( int& bitsPerSample, bool& floatSample ) const;

   /*!
    * Converts the image owned by this %ImageView control to the specified
    * sample data format.
    *
    * \param bitsPerSample       Number of bits per sample. Can be 8, 16 or 32
    *                            for an integer image; 32 or 64 for a floating
    *                            point image.
    *
    * \param floatSample         True to transform the image to a floating
    *                            point data format; false to transform the
    *                            image to an integer data format.
    *
    * This function does nothing if the image in this control already has the
    * specified sample data format.
    */
   void SetSampleFormat( int bitsPerSample, bool floatSample );

   /*!
    * Obtains a copy of the RGB working space (RGBWS) associated with the
    * image owned by this %ImageView control.
    *
    * \param[out] rgbws    Reference to an object where the parameters of the
    *                      current RGBWS will be copied.
    */
   void GetRGBWS( RGBColorSystem& rgbws ) const;

   /*!
    * Sets the RGB working space (RGBWS) for the image owned by this %ImageView
    * control.
    *
    * \param rgbws         Reference to a RGBWS that will be associated with
    *                      the image owned by this object.
    */
   void SetRGBWS( const RGBColorSystem& rgbws );

   /*!
    * Returns true iff color management is active for the image in this
    * %ImageView control.
    */
   bool IsColorManagementEnabled() const;

   /*!
    * Enables color management for the image in this %ImageView control.
    *
    * Color management uses ICC profiles for the image in this control (either
    * a profile associated with the image or a global profile) and the monitor
    * device to control how the image is rendered on the screen. Color
    * management transformations are applied to yield a visual representation
    * that takes into account the actual meaning of numerical pixel sample
    * values, considering the chromatic responses of the screen and of the
    * devices and color spaces that have originated the image.
    *
    * If color management is disabled for an image, its pixel sample values are
    * sent directly to the screen, avoiding ICC profile transformations. This
    * improves output performance, but may give a false representation of the
    * image on the screen.
    *
    * If this control is visible, its screen rendition is updated immediately
    * after calling this function.
    */
   void EnableColorManagement( bool = true );

   /*!
    * Disables color management for the image in this %ImageView control.
    *
    * See the documentation for EnableColorManagement() for additional
    * information on color management in PixInsight.
    *
    * If this control is visible, its screen rendition is updated immediately
    * after calling this function.
    */
   void DisableColorManagement( bool disable = true )
   {
      EnableColorManagement( !disable );
   }

   /*!
    * Returns true iff <em>soft-proofing</em> is currently enabled for the image
    * in this %ImageView control.
    */
   bool IsProofingEnabled() const;

   /*!
    * Enables <em>soft-proofing</em> for the image in this %ImageView control.
    *
    * Color proofing is used to emulate an output device (usually a printer) on
    * the screen. Proofing requires three ICC profiles: the image profile
    * (either an embedded profile or the default profile), the monitor profile,
    * and a <em>proofing profile</em> that describes the emulated device. Color
    * proofing is useful to preview the final printed result without rendering
    * images to physical media.
    *
    * If this control is visible and has color management enabled, its screen
    * rendition is updated immediately after calling this function.
    */
   void EnableProofing( bool = true );

   /*!
    * Disables <em>soft-proofing</em> for the image in this %ImageView control.
    *
    * See the documentation for EnableProofing() for additional information on
    * color proofing.
    *
    * If this control is visible and has color management enabled, its screen
    * rendition is updated immediately after calling this function.
    */
   void DisableProofing( bool disable = true )
   {
      EnableProofing( !disable );
   }

   /*!
    * Returns true iff the <em>gamut check</em> soft-proofing feature is
    * currently enabled for the image in this %ImageView control.
    */
   bool IsGamutCheckEnabled() const;

   /*!
    * Enables the <em>gamut check</em> proofing feature for the image in this
    * %ImageView control.
    *
    * The gamut check feature represents <em>out-of-gamut colors</em> (i.e.,
    * colors that are not defined in the color space of the image) with a
    * special color (selectable via global color management preferences) for
    * quick visual detection.
    *
    * If this control is visible, has color management enabled, and has soft
    * proofing enabled, its screen rendition is updated immediately after
    * calling this function. If color proofing is disabled, calling this
    * function has no effect.
    */
   void EnableGamutCheck( bool = true );

   /*!
    * Disables the <em>gamut check</em> proofing feature for the image in this
    * %ImageView control.
    *
    * See the documentation for EnableGamutCheck() for additional information
    * on the gamut check color proofing feature.
    *
    * If this control is visible, has color management enabled, and has soft
    * proofing enabled, its screen rendition is updated immediately after
    * calling this function. If color proofing is disabled, calling this
    * function has no effect.
    */
   void DisableGamutCheck( bool disable = true )
   {
      EnableGamutCheck( !disable );
   }

   /*!
    * Allows enabling or disabling all color management features at once for
    * this %ImageView control.
    *
    * \param cmEnabled     Enables (if true) or disables (if false) color
    *                      management for the image in this control.
    *
    * \param proofing      Enables (if true) or disables (if false)
    *                      soft-proofing for the image in this control.
    *
    * \param gamutCheck    Enables (if true) or disables (if false) the
    *                      gamut check proofing feature. The state of this
    *                      argument has no meaning if the \a proofing parameter
    *                      is false.
    *
    * If this control is visible, its screen rendition is updated immediately
    * after calling this function.
    */
   void SetColorManagementFeatures( bool cmEnabled, bool proofing, bool gamutCheck );

   /*!
    * Obtains a copy of the current ICC profile associated with the image in
    * this %ImageView control.
    *
    * \param[out] icc   Reference to an object where a copy of the current ICC
    *                   profile will be stored.
    */
   bool GetICCProfile( ICCProfile& icc ) const;

   /*!
    * Sets the current ICC profile for the image in this %ImageView control as
    * a copy of an existing ICC profile.
    *
    * \param icc  Reference to an object that will be used to set the current
    *             ICC profile.
    *
    * If this control is visible and has color management enabled, its screen
    * rendition is updated immediately after calling this function.
    */
   void SetICCProfile( const ICCProfile& icc );

   /*!
    * Sets the current ICC profile for the image in this %ImageView control as
    * a copy of an ICC profile loaded from a disk file.
    *
    * \param filePath   A file path specification to an ICC profile that will
    *                   be loaded and copied to the ICC profile of the image.
    *
    * If this control is visible and has color management enabled, its screen
    * rendition is updated immediately after calling this function.
    */
   void SetICCProfile( const String& filePath );

   /*!
    * Causes the image in this %ImageView control to be associated with the
    * default ICC profile globally defined in the PixInsight core application.
    *
    * If this control has its own associated ICC profile, this funtion destroys
    * it and frees its allocated space.
    *
    * If this control is visible and has color management enabled, its screen
    * rendition is updated immediately after calling this function.
    */
   void DeleteICCProfile();

   /*!
    * Returns the current GUI working mode for this %ImageView control.
    *
    * The returned value can be identified with the symbolic constants defined
    * in the ImageMode namespace.
    */
   gui_mode CurrentMode() const;

   /*!
    * Sets the current GUI working mode for this %ImageView control.
    *
    * \param mode    Specifies the new GUI mode. Supported modes are identified
    *                through symbolic constants defined in the ImageMode
    *                namespace.
    *
    * Preview edition and dynamic modes are not valid for %ImageView. The only
    * valid GUI working modes are:
    *
    * ImageMode::Readout \n
    * ImageMode::ZoomIn \n
    * ImageMode::ZoomOut \n
    * ImageMode::Pan \n
    * ImageMode::Center
    */
   void SelectMode( gui_mode mode );

   /*!
    * Returns the current display channel for this %ImageView control.
    *
    * The returned value can be identified with the symbolic constants defined
    * int the DisplayChannel namespace.
    */
   display_channel CurrentChannel() const;

   /*!
    * Returns true iff the current display channel for this %ImageView control
    * corresponds to an alpha channel of the image.
    */
   bool IsAlphaChannelDisplayed() const
   {
      return CurrentChannel() >= DisplayChannel::Alpha;
   }

   /*!
    * Returns the <em>alpha channel index</em> displayed for the image in this
    * %ImageView control. If the current display channel does not correspond to
    * an alpha channel, a negative integer value is returned.
    *
    * Alpha channel indices are relative to the first alpha channel in an
    * image. For example, the first alpha channel is always indexed as alpha
    * channel #0, and so on.
    */
   int CurrentAlphaChannel() const
   {
      return CurrentChannel() - DisplayChannel::Alpha;
   }

   /*!
    * Selects the current display channel for this %ImageView control.
    *
    * \param channel    Specifies the new display channel. Supported display
    *                   channels are enumerated in the DisplayChannel
    *                   namespace.
    *
    * Not all display channels are valid for all images in all contexts. For
    * example, if the image in the current view is a grayscale image, the only
    * valid display channels are the combined RGB/K channel
    * (DisplayChannel::RGBK) and, if the image has \a n alpha channels, values
    * in the range DisplayChannel::Alpha to DisplayChannel::Alpha+n-1. If an
    * invalid display channel is specified, this function is simply ignored.
    */
   void SelectChannel( display_channel channel );

   /*!
    * Returns the current transparency rendering mode for this %ImageView
    * control.
    *
    * Supported transparency modes are defined in the TransparencyMode
    * namespace.
    */
   transparency_mode TransparencyMode() const;

   /*!
    * Returns the color used in this %ImageView control to render transparent
    * image regions, when the transparency mode is TransparencyMode::Color.
    *
    * The returned color is an AARRGGBB value. The alpha value is always 0xff,
    * since the transparency background cannot be transparent.
    */
   RGBA TransparencyColor() const;

   /*!
    * Returns true iff transparent image areas are currently visible for this
    * %ImageView control.
    *
    * Transparencies are visible if the current rendering mode is not
    * TransparencyMode::Hide.
    */
   bool IsTransparencyVisible() const
   {
      return TransparencyMode() != pcl::TransparencyMode::Hide;
   }

   /*!
    * Sets the current transparency rendering mode for this %ImageView control.
    *
    * \param mode    Specifies the new mode that will be used to render
    *                transparent regions of images. Supported transparency
    *                modes are identified through symbolic constants defined in
    *                the TransparencyMode namespace.
    *
    * \param color   New color that will be used when the transparency mode is
    *                TransparencyMode::Color. If zero is specified (the default
    *                value), the current color will not be changed.
    */
   void SetTransparencyMode( transparency_mode mode, RGBA color = 0 );

   /*!
    * Disables transparency renditions for this %ImageView control.
    *
    * Calling this function is equivalent to:
    *
    * \code SetTransparencyMode( pcl::TransparencyMode::Hide ); \endcode
    */
   void HideTransparency()
   {
      SetTransparencyMode( pcl::TransparencyMode::Hide );
   }

   /*!
    * Sets new viewport central location coordinates and zoom factor for this
    * %ImageView control.
    *
    * \param cx,cy   Position (horizontal, vertical) of the new central
    *                viewport location in image coordinates.
    *
    * \param zoom    New viewport zoom factor. Positive zoom factors are
    *                interpreted as magnifying factors; negative zoom factors
    *                are reduction factors:
    *
    * <pre>
    * Zoom Factor    Screen Zoom (screen pixels : image pixels)
    * ===========    ==========================================
    *      0               Current zoom factor (no change)
    *   1 or -1            1:1 (actual pixels)
    *      2               2:1 (x2 magnification)
    *      3               3:1 (x3 magnification)
    *     ...
    *     -2               1:2 (1/2 reduction)
    *     -3               1:3 (1/3 reduction)
    *     ...
    * </pre>
    *
    *                A zoom factor of -1 should not be used explicitly; it is a
    *                reserved value for internal use, and there is no guarantee
    *                that future versions of the PCL continue accepting it.
    *
    * If the specified viewport location cannot correspond to the viewport
    * central position with the specified zoom factor, the nearest image
    * coordinates are always selected automatically. For example, if you
    * pass image coordinates \a cx=0 and \a cy=0 as arguments to this function,
    * the GUI will set the viewport center to the necessary image location in
    * order to display the upper left corner of the image just at the upper
    * left corner of the viewport.
    *
    * If the \a zoom argument is not specified, the current zoom factor is not
    * changed since the default value of this parameter is zero; see the table
    * above.
    *
    * If this control is visible, calling this function causes an immediate
    * regeneration of the screen rendition for the visible viewport region.
    */
   void SetViewport( double cx, double cy, int zoom = 0 );

   /*!
    * Sets new viewport central location and zoom factor for this %ImageView
    * control.
    *
    * Calling this function is equivalent to:
    *
    * \code SetViewport( center.x, center.y, int ); \endcode
    *
    * See the documentation for SetViewport( double, double, int ).
    */
   void SetViewport( const DPoint& center, int zoom = 0 )
   {
      SetViewport( center.x, center.y, zoom );
   }

   /*!
    * Returns the current zoom factor in the viewport of this %ImageView
    * control.
    *
    * To learn how zoom factors are interpreted in PixInsight, see the
    * documentation for SetViewport( double, double, int ).
    */
   int ZoomFactor() const;

   /*!
    * Sets the zoom factor for the viewport of this %ImageView control to the
    * specified value \a z.
    *
    * To learn how zoom factors are interpreted in PixInsight, see the
    * documentation for SetViewport( double, double, int ).
    */
   void SetZoomFactor( int z );

   /*!
    * Increments the current zoom factor for the viewport of this %ImageView
    * control.
    *
    * To learn how zoom factors are interpreted in PixInsight, see the
    * documentation for SetViewport( double, double, int ).
    */
   void ZoomIn()
   {
      int z = ZoomFactor() + 1;
      SetZoomFactor( (z > 0) ? z : +1 );
   }

   /*!
    * Decrements the current zoom factor for the viewport of this %ImageView
    * control.
    *
    * To learn how zoom factors are interpreted in PixInsight, see the
    * documentation for SetViewport( double, double, int ).
    */
   void ZoomOut()
   {
      int z = ZoomFactor() - 1;
      SetZoomFactor( (z > 0 || z < -1) ? z : -2 );
   }

   /*!
    * Gets the current sizes of the viewport in this %ImageView control.
    *
    * \param[out] width    Reference to a variable where the viewport width in
    *                      pixels will be stored.
    *
    * \param[out] height   Reference to a variable where the viewport height in
    *                      pixels will be stored.
    *
    * The provided sizes represent the entire image in this control, as
    * represented on its viewport after applying the current zoom factor.
    * Viewport sizes can also be thought of as the available navigation ranges
    * in an %ImageView control.
    */
   void GetViewportSize( int& width, int& height ) const;

   /*!
    * Gets the current width of the viewport in this %ImageView control.
    */
   int ViewportWidth() const
   {
      int w, dum;
      GetViewportSize( w, dum );
      return w;
   }

   /*!
    * Gets the current height of the viewport in this %ImageView control.
    */
   int ViewportHeight() const
   {
      int dum, h;
      GetViewportSize( dum, h );
      return h;
   }

   /*!
    * Returns the current viewport position in this %ImageView control.
    *
    * The viewport position corresponds to the viewport coordinates of the
    * upper left corner of the <em>visible viewport region</em>. These
    * coordinates can be negative if the represented image is smaller than the
    * size of the viewport control, that is, when the <em>extended viewport
    * space</em> is visible around the screen rendition of the image.
    */
   Point ViewportPosition() const;

   /*!
    * Sets the viewport position in this %ImageView control to the specified
    * \a x and \a y viewport coordinates.
    *
    * If this control is visible, calling this function causes an immediate
    * regeneration of the screen rendition for the visible viewport region.
    */
   void SetViewportPosition( int x, int y );

   /*!
    * Sets the viewport position in this %ImageView control to the specified
    * position \a p in viewport coordinates.
    *
    * Calling this function is equivalent to:
    *
    * \code SetViewportPosition( p.x, p.y ); \endcode
    */
   void SetViewportPosition( const Point& p )
   {
      SetViewportPosition( p.x, p.y );
   }

   /*!
    * Gets the current visible viewport region in this %ImageView control.
    *
    * The returned rectangle corresponds to the visible region of the image in
    * viewport coordinates. It depends on the current viewport zoom factor and
    * on the positions of the viewport's scroll bars, if they are visible.
    */
   Rect VisibleViewportRect() const;

   /*!
    * Returns the width in pixels of the current visible viewport region in
    * this %ImageView control.
    */
   int VisibleViewportWidth() const
   {
      return VisibleViewportRect().Width();
   }

   /*!
    * Returns the height in pixels of the current visible viewport region in
    * this %ImageView control.
    */
   int VisibleViewportHeight() const
   {
      return VisibleViewportRect().Height();
   }

   /*!
    * Converts the specified \a x and \a y integer coordinates from the
    * viewport coordinate system to the image coordinate system.
    *
    * The converted image coordinates are rounded to the nearest integers.
    */
   void ViewportToImage( int& x, int& y ) const;

   /*!
    * Converts the specified \a x and \a y real floating point coordinates from
    * the viewport coordinate system to the image coordinate system.
    */
   void ViewportToImage( double& x, double& y ) const;

   /*!
    * Converts a point \a p in the viewport coordinate system to the image
    * coordinate system. Returns a point with the resulting floating point real
    * image coordinates.
    */
   template <typename T>
   DPoint ViewportToImage( const GenericPoint<T>& p ) const
   {
      DPoint p1 = p;
      ViewportToImage( p1.x, p1.y );
      return p1;
   }

   /*!
    * Converts a rectangle \a r in the viewport coordinate system to the image
    * coordinate system. Returns a rectangle with the resulting floating point
    * real image coordinates.
    */
   template <typename T>
   DRect ViewportToImage( const GenericRectangle<T>& r ) const
   {
      DRect r1 = r;
      ViewportToImage( r1.x0, r1.y0 );
      ViewportToImage( r1.x1, r1.y1 );
      return r1;
   }

   /*!
    * Converts a set of integer points from the viewport coordinate system to
    * the image coordinate system.
    *
    * \param p    Pointer to the first point in the array of points that will
    *             be converted from viewport coordinates to image coordinates.
    *
    * \param n    Number of points in the \a p array.
    *
    * Converted point image coordinates are rounded to the nearest integers.
    */
   void ViewportToImage( Point* p, size_type n ) const;

   /*!
    * Converts a set of real floating-point points from the viewport
    * coordinate system to the image coordinate system.
    *
    * \param p    Pointer to the first point in the array of points that will
    *             be converted from viewport coordinates to image coordinates.
    *
    * \param n    Number of points in the \a p array.
    */
   void ViewportToImage( DPoint* p, size_type n ) const;

   /*!
    * Converts a set of points in a dynamic array from the viewport coordinate
    * system to the image coordinate system.
    *
    * \param a    Reference to a dynamic array of points that will be converted
    *             from viewport coordinates to image coordinates.
    */
   template <typename T>
   void ViewportToImage( Array<GenericPoint<T> >& a ) const
   {
      ViewportToImage( a.Begin(), a.Length() );
   }

   /*!
    * Converts a set of integer scalars from the viewport coordinate system to
    * the image coordinate system.
    *
    * \param d    Pointer to the first element in the array of scalars that
    *             will be converted from viewport coordinates to image
    *             coordinates.
    *
    * \param n    Number of scalars in the \a p array.
    *
    * Converted scalars in image coordinates are rounded to the nearest
    * integers.
    */
   void ViewportScalarToImage( int* d, size_type n ) const;

   /*!
    * Converts a set of real floating-point scalars from the viewport
    * coordinate system to the image coordinate system.
    *
    * \param d    Pointer to the first element in the array of scalars that
    *             will be converted from viewport coordinates to image
    *             coordinates.
    *
    * \param n    Number of scalars in the \a p array.
    */
   void ViewportScalarToImage( double* d, size_type n ) const;

   /*!
    * Converts a scalar \a d in the viewport coordinate system to the image
    * coordinate system. Returns the resulting floating point real scalar in
    * image coordinates.
    */
   double ViewportScalarToImage( double d ) const;

   /*!
    * Converts a scalar \a d in the viewport coordinate system to the image
    * coordinate system. Returns the resulting floating point real scalar in
    * image coordinates.
    */
   template <typename T>
   double ViewportScalarToImage( T d ) const
   {
      return ViewportScalarToImage( double( d ) );
   }

   /*!
    * Converts the specified \a x and \a y integer coordinates from the image
    * coordinate system to the viewport coordinate system.
    *
    * The converted viewport coordinates are rounded to the nearest integers.
    */
   void ImageToViewport( int& x, int& y ) const;

   /*!
    * Converts the specified \a x and \a y real floating-point coordinates from
    * the image coordinate system to the viewport coordinate system.
    */
   void ImageToViewport( double& x, double& y ) const;

   /*!
    * Converts a point \a p in the image coordinate system to the viewport
    * coordinate system. Returns a point with the resulting viewport
    * coordinates.
    *
    * If the template argument T corresponds to an integer type, the converted
    * viewport coordinates in the resulting point are rounded to the nearest
    * integers.
    */
   template <typename T>
   GenericPoint<T> ImageToViewport( const GenericPoint<T>& p ) const
   {
      GenericPoint<T> p1 = p;
      ImageToViewport( p1.x, p1.y );
      return p1;
   }

   /*!
    * Converts a rectangle \a r in the image coordinate system to the viewport
    * coordinate system. Returns a rectangle with the resulting viewport
    * coordinates.
    *
    * If the template argument T corresponds to an integer type, the converted
    * viewport coordinates in the resulting rectangle are rounded to the
    * nearest integers.
    */
   template <typename T>
   GenericRectangle<T> ImageToViewport( const GenericRectangle<T>& r ) const
   {
      GenericRectangle<T> r1 = r;
      ImageToViewport( r1.x0, r1.y0 );
      ImageToViewport( r1.x1, r1.y1 );
      return r1;
   }

   /*!
    * Converts a set of integer points from the image coordinate system to
    * the viewport coordinate system.
    *
    * \param p    Pointer to the first point in the array of points that will
    *             be converted from image coordinates to viewport coordinates.
    *
    * \param n    Number of points in the \a p array.
    *
    * Converted point viewport coordinates are rounded to the nearest integers.
    */
   void ImageToViewport( Point* p, size_type n ) const;

   /*!
    * Converts a set of real floating-point points from the image coordinate
    * system to the viewport coordinate system.
    *
    * \param p    Pointer to the first point in the array of points that will
    *             be converted from image coordinates to viewport coordinates.
    *
    * \param n    Number of points in the \a p array.
    */
   void ImageToViewport( DPoint* p, size_type n ) const;

   /*!
    * Converts a set of points in a dynamic array from the image coordinate
    * system to the viewport coordinate system.
    *
    * \param a    Reference to a dynamic array of points that will be converted
    *             from image coordinates to viewport coordinates.
    */
   template <typename T>
   void ImageToViewport( Array<GenericPoint<T> >& a ) const
   {
      ImageToViewport( a.Begin(), a.Length() );
   }

   /*!
    * Converts a set of integer scalars from the image coordinate system to
    * the viewport coordinate system.
    *
    * \param d    Pointer to the first element in the array of scalars that
    *             will be converted from image coordinates to viewport
    *             coordinates.
    *
    * \param n    Number of scalars in the \a p array.
    *
    * Converted scalars in viewport coordinates are rounded to the nearest
    * integers.
    */
   void ImageScalarToViewport( int* d, size_type n ) const;

   /*!
    * Converts a set of real floating-point scalars from the image coordinate
    * system to the viewport coordinate system.
    *
    * \param d    Pointer to the first element in the array of scalars that
    *             will be converted from image coordinates to viewport
    *             coordinates.
    *
    * \param n    Number of scalars in the \a p array.
    */
   void ImageScalarToViewport( double* d, size_type n ) const;

   /*!
    * Converts an integer scalar \a d in the image coordinate system to the
    * viewport coordinate system. Returns the resulting scalar in viewport
    * coordinates.
    *
    * The resulting scalar in viewport coordinates is rounded to the nearest
    * integer.
    */
   int ImageScalarToViewport( int ) const;

   /*!
    * Converts a real floating-point scalar \a d in the image coordinate system
    * to the viewport coordinate system. Returns the resulting scalar in
    * viewport coordinates.
    */
   double ImageScalarToViewport( double ) const;

   /*!
    * Converts the specified \a x and \a y integer coordinates from the
    * viewport coordinate system to the global coordinate system.
    *
    * The converted global coordinates are rounded to the nearest integers.
    */
   void ViewportToGlobal( int& x, int& y ) const;

   /*!
    * Converts a point \a p in the viewport coordinate system to the global
    * coordinate system. Returns a point with the resulting global coordinates.
    */
   Point ViewportToGlobal( const Point& p ) const
   {
      Point p1 = p;
      ViewportToGlobal( p1.x, p1.y );
      return p1;
   }

   /*!
    * Converts a rectangle \a r in the viewport coordinate system to the global
    * coordinate system. Returns a rectangle with the resulting global
    * coordinates.
    */
   Rect ViewportToGlobal( const Rect& r ) const
   {
      Rect r1 = r;
      ViewportToGlobal( r1.x0, r1.y0 );
      ViewportToGlobal( r1.x1, r1.y1 );
      return r1;
   }

   /*!
    * Converts the specified \a x and \a y integer coordinates from the global
    * coordinate system to the viewport coordinate system.
    *
    * The converted viewport coordinates are rounded to the nearest integers.
    */
   void GlobalToViewport( int& x, int& y ) const;

   /*!
    * Converts a point \a p in the global coordinate system to the viewport
    * coordinate system. Returns a point with the resulting viewport
    * coordinates.
    */
   Point GlobalToViewport( const Point& p ) const
   {
      Point p1 = p;
      GlobalToViewport( p1.x, p1.y );
      return p1;
   }

   /*!
    * Converts a rectangle \a r in the global coordinate system to the viewport
    * coordinate system. Returns a rectangle with the resulting viewport
    * coordinates.
    */
   Rect GlobalToViewport( const Rect& r ) const
   {
      Rect r1 = r;
      GlobalToViewport( r1.x0, r1.y0 );
      GlobalToViewport( r1.x1, r1.y1 );
      return r1;
   }

   /*!
    * Causes a complete regeneration of the whole viewport and its associated
    * UI resources.
    *
    * Calling this function may be necessary to force a regeneration after
    * changing the geometry of the image owned by this %ImageView control.
    */
   void Reset()
   {
      SetZoomFactor( ZoomFactor() );
   }

   /*!
    * Regenerates the screen rendition for the visible viewport region in this
    * %ImageView control.
    */
   void Regenerate();

   /*!
    * Regenerates the screen rendition for a rectangular region of the
    * viewport, specified in viewport coordinates.
    *
    * Calling this function is equivalent to:
    *
    * \code RegenerateViewportRect( r.x0, r.y0, r.x1, r.y1 ); \endcode
    */
   void RegenerateViewportRect( const Rect& r )
   {
      RegenerateViewportRect( r.x0, r.y0, r.x1, r.y1 );
   }

   /*!
    * Regenerates the screen rendition for a rectangular region of the
    * viewport, specified in viewport coordinates.
    *
    * \param x0,y0   %Viewport coordinates (horizontal, vertical) of the upper
    *                left corner of the regenerated region.
    *
    * \param x1,y1   %Viewport coordinates (horizontal, vertical) of the lower
    *                right corner of the regenerated region.
    *
    * The specified region will be updated on the screen. Only the visible
    * portion of the specified region will be regenerated and updated, that is,
    * its intersection with the visible viewport region.
    */
   void RegenerateViewportRect( int x0, int y0, int x1, int y1 );

   /*!
    * Regenerates the screen rendition for a rectangular region of the
    * viewport, specified in image coordinates.
    *
    * Calling this function is equivalent to:
    *
    * \code RegenerateImageRect( r.x0, r.y0, r.x1, r.y1 ); \endcode
    */
   void RegenerateImageRect( const pcl::DRect& r )
   {
      RegenerateImageRect( r.x0, r.y0, r.x1, r.y1 );
   }

   /*!
    * Regenerates the screen rendition for a rectangular region of the
    * viewport, specified in image coordinates.
    *
    * \param x0,y0   %Image coordinates (horizontal, vertical) of the upper
    *                left corner of the regenerated region.
    *
    * \param x1,y1   %Image coordinates (horizontal, vertical) of the lower
    *                right corner of the regenerated region.
    *
    * The specified region will be updated on the screen. Only the visible
    * portion of the specified region will be regenerated and updated, that is,
    * its intersection with the visible viewport region.
    */
   void RegenerateImageRect( double x0, double y0, double x1, double y1 );

   /*!
    * Requests a screen update for the entire visible viewport region in this
    * %ImageView control.
    */
   void UpdateViewport();

   /*!
    * Requests a screen update for a rectangular region of the viewport,
    * specified in viewport coordinates.
    *
    * Calling this function is equivalent to:
    *
    * \code UpdateViewportRect( r.x0, r.y0, r.x1, r.y1 ); \endcode
    */
   void UpdateViewportRect( const Rect& r )
   {
      UpdateViewportRect( r.x0, r.y0, r.x1, r.y1 );
   }

   /*!
    * Requests a screen update for a rectangular region of the viewport,
    * specified in viewport coordinates.
    *
    * \param x0,y0   %Viewport coordinates (horizontal, vertical) of the upper
    *                left corner of the update region.
    *
    * \param x1,y1   %Viewport coordinates (horizontal, vertical) of the lower
    *                right corner of the update region.
    *
    * The specified region will be updated on the screen. Only the visible
    * portion of the specified region will be updated, that is, its
    * intersection with the visible viewport region.
    */
   void UpdateViewportRect( int x0, int y0, int x1, int y1 );

   /*!
    * Requests a screen update for a rectangular region of the viewport,
    * specified in image coordinates.
    *
    * Calling this function is equivalent to:
    *
    * \code UpdateImageRect( r.x0, r.y0, r.x1, r.y1 ); \endcode
    */
   void UpdateImageRect( const pcl::DRect& r )
   {
      UpdateImageRect( r.x0, r.y0, r.x1, r.y1 );
   }

   /*!
    * Requests a screen update for a rectangular region of the viewport,
    * specified in image coordinates.
    *
    * \param x0,y0   %Image coordinates (horizontal, vertical) of the upper
    *                left corner of the update region.
    *
    * \param x1,y1   %Image coordinates (horizontal, vertical) of the lower
    *                right corner of the update region.
    *
    * The specified region will be updated on the screen. Only the visible
    * portion of the specified region will be updated, that is, its
    * intersection with the visible viewport region in the image coordinate
    * system.
    */
   void UpdateImageRect( double x0, double y0, double x1, double y1 );

   /*!
    * Returns true iff there are pending viewport updates for this %ImageView
    * control.
    */
   bool HasPendingUpdates() const;

   /*!
    * Executes all pending viewport update requests for this %ImageView
    * control.
    *
    * Viewport update requests are not executed immediately by the core
    * PixInsight application. For the sake of performance, The GUI tries to
    * optimize screen updates by performing the minimum possible redrawing
    * work.
    *
    * This means that update requests are placed on a queue, waiting while the
    * main GUI thread is busy or until a sufficiently large update region
    * becomes available.
    *
    * With this function, an interface can force the immediate screen update
    * for all pending update viewport regions. If multiple update regions are
    * pending, the core application will still minimize the required drawing
    * work by grouping all of them into the minimum possible amount of larger
    * regions.
    */
   void CommitPendingUpdates();

   /*!
    * Returns a rectangle in viewport coordinates that includes all pending
    * viewport update regions for this %ImageView control.
    */
   Rect ViewportUpdateRect() const;

   /*!
    * Returns a RGBA bitmap with a screen rendition of a viewport region of
    * this %ImageView control.
    *
    * Calling this function is equivalent to:
    *
    * \code ViewportBitmap( r.x0, r.y0, r.x1, r.y1, flags ); \endcode
    */
   Bitmap ViewportBitmap( const Rect& r, uint32 flags = 0 ) const
   {
      return ViewportBitmap( r.x0, r.y0, r.x1, r.y1, flags );
   }

   /*!
    * Returns a RGBA bitmap with a screen rendition of a viewport region of
    * this %ImageView control.
    *
    * \param x0,y0   %Viewport coordinates (horizontal, vertical) of the upper
    *                left corner of the source viewport region.
    *
    * \param x1,y1   %Viewport coordinates (horizontal, vertical) of the lower
    *                right corner of the source viewport region.
    *
    * \param flags   Currently not used; reserved for future extensions of PCL.
    *                Must be set to zero.
    *
    * The returned bitmap will contain a screen rendition for the specified
    * source region of the image in this %ImageView control. It will be
    * magnified or reduced according to the current zoom factor.
    */
   Bitmap ViewportBitmap( int x0, int y0, int x1, int y1, uint32 flags = 0 ) const;

   /*!
    * Begins a new interactive rectangular selection procedure on the viewport
    * of this %ImageView control.
    *
    * Calling this function is equivalent to:
    *
    * \code BeginSelection( p.x, p.y, flags ); \endcode
    */
   void BeginSelection( const Point& p, uint32 flags = 0 )
   {
      BeginSelection( p.x, p.y, flags );
   }

   /*!
    * Starts a new interactive rectangular selection procedure on the viewport
    * of this %ImageView control.
    *
    * \param x,y     %Image coordinates of the starting point of a new
    *                rectangular selection.
    *
    * \param flags   Currently not used; reserved for future extensions of PCL.
    *                Must be set to zero.
    *
    * Interfaces typically use BeginSelection() in response to an OnMousePress
    * event to start a rectangular selection. ModifySelection() is then called
    * from an OnMouseMove event handler, and the final selection coordinates
    * are obtained by calling SelectionRect() from an OnMouseRelease event
    * handler.
    */
   void BeginSelection( int x, int y, uint32 flags = 0 );

   /*!
    * Updates an ongoing interactive rectangular selection procedure on the
    * viewport of this %ImageView control.
    *
    * Calling this function is equivalent to:
    *
    * \code ModifySelection( p.x, p.y, flags ); \endcode
    */
   void ModifySelection( const Point& p, uint32 flags = 0 )
   {
      ModifySelection( p.x, p.y, flags );
   }

   /*!
    * Updates an ongoing interactive rectangular selection procedure on the
    * viewport of this %ImageView control.
    *
    * \param x,y     %Image coordinates to update the current rectangular
    *                selection. The selection rectangle is defined by this
    *                position and the starting position defined in the call to
    *                BeginSelection() that originated the current selection
    *                procedure.
    *
    * \param flags   Currently not used; reserved for future extensions of PCL.
    *                Must be set to zero.
    */
   void ModifySelection( int x, int y, uint32 flags = 0 );

   /*!
    * Requests a screen viewport update for the current rectangular selection.
    */
   void UpdateSelection();

   /*!
    * Aborts the current interactive selection procedure.
    */
   void CancelSelection();

   /*!
    * Terminates the current interactive selection procedure.
    */
   void EndSelection();

   /*!
    * Returns the current rectangular selection in image coordinates.
    *
    * \param flags   Currently not used; reserved for future extensions of PCL.
    *                Must be set to zero.
    *
    * Before calling this function, a rectangular selection procedure should
    * have been started with BeginSelection().
    */
   Rect SelectionRect( uint32* flags = 0 ) const;

   /*!
    * Returns true iff an interactive selection procedure is currently active on
    * the viewport of this %ImageView control.
    */
   bool IsSelection() const;

   // -------------------------------------------------------------------------
   // Event handlers
   //
   // void OnScrollViewport( ImageView& sender, int dx, int dy );

   /*!
    * \defgroup image_view_event_handlers ImageView Event Handlers
    */

   /*!
    * Defines the prototype of a <em>scroll event handler</em>.
    *
    * A scroll event is generated when the viewport in this %ImageView control
    * has been moved with respect to the control's origin, e.g. when the values
    * of one or both scroll bars have been changed. The event is generated just
    * \e before updating the viewport, so this event handler can be implemented
    * for optimization purposes.
    *
    * \param sender  The control that sends a scroll event.
    *
    * \param dx      Horizontal scroll distance in viewport pixels. Positive
    *                values mean scrolling in the rightward direction.
    *
    * \param dy      Vertical scroll distance in viewport pixels. Positive
    *                values mean scrolling in the downward direction.
    *
    * \ingroup image_view_event_handlers
    */
   typedef void (Control::*scroll_event_handler)( ImageView& sender, int dx, int dy );

   /*!
    * Sets the scroll event handler for this %ImageView control.
    *
    * \param handler    The scroll event handler. Must be a member function of
    *                   the receiver object's class.
    *
    * \param receiver   The control that will receive scroll events from this
    *                   %ImageView object.
    *
    * \ingroup image_view_event_handlers
    */
   void OnScrollViewport( scroll_event_handler handler, Control& receiver );

private:

   struct EventHandlers
   {
      scroll_event_handler onScrollViewport = nullptr;

      EventHandlers() = default;
      EventHandlers( const EventHandlers& ) = default;
      EventHandlers& operator =( const EventHandlers& ) = default;
   };

   AutoPointer<EventHandlers> m_handlers;

protected:

   /*!
    * \internal
    */
   ImageView( void* );

   /*!
    * \internal
    */
   ImageView( void*, void* );

   friend class ImageViewEventDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_ImageView_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageView.h - Released 2015/10/08 11:24:12 UTC
