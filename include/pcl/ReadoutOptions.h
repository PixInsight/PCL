//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/ReadoutOptions.h - Released 2017-04-14T23:04:40Z
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

#ifndef __PCL_ReadoutOptions_h
#define __PCL_ReadoutOptions_h

/// \file pcl/ReadoutOptions.h

#include <pcl/Defs.h>

#include <pcl/Utility.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::ReadoutData
 * \brief     Pixel readout data modes.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>ReadoutData::RGBK</td>   <td>RGB components or grayscale</td></tr>
 * <tr><td>ReadoutData::RGBL</td>   <td>RGB components + CIE L* (lightness)</td></tr>
 * <tr><td>ReadoutData::RGBY</td>   <td>RGB components + CIE Y (luminance)</td></tr>
 * <tr><td>ReadoutData::CIEXYZ</td> <td>CIE XYZ unit vectors</td></tr>
 * <tr><td>ReadoutData::CIELab</td> <td>CIE L*a*b* normalized components</td></tr>
 * <tr><td>ReadoutData::CIELch</td> <td>CIE L*c*h* normalized components</td></tr>
 * <tr><td>ReadoutData::HSV</td>    <td>HSV components</td></tr>
 * <tr><td>ReadoutData::HSI</td>    <td>HSI components</td></tr>
 * </table>
 */
namespace ReadoutData
{
   enum value_type
   {
      RGBK,            // RGB components or grayscale
      RGBL,            // RGB components + CIE L*
      RGBY,            // RGB components + CIE Y
      CIEXYZ,          // CIE XYZ unit vectors
      CIELab,          // CIE L*a*b* normalized components
      CIELch,          // CIE L*c*h* normalized components
      HSV,             // HSV components
      HSI,             // HSI components
      NumberOfModes
   };
}

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::ReadoutMode
 * \brief     Pixel readout calculation modes.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>ReadoutMode::Mean</td>    <td>Average of readout probe pixel values</td></tr>
 * <tr><td>ReadoutMode::Median</td>  <td>Median of readout probe pixel values</td></tr>
 * <tr><td>ReadoutMode::Minimum</td> <td>The maximum pixel value in the readout probe area</td></tr>
 * <tr><td>ReadoutMode::Maximum</td> <td>The minimum pixel value in the readout probe area</td></tr>
 * </table>
 */
namespace ReadoutMode
{
   enum value_type
   {
      Mean,            // Average of probe pixel values
      Median,          // Median of probe pixel values
      Minimum,         // The maximum pixel value in the probe area
      Maximum,         // The minimum pixel value in the probe area
      NumberOfModes
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class ReadoutOptions
 * \brief A structure to hold global pixel readout options.
 *
 * %ReadoutOptions holds a set of parameters that define how PixInsight
 * calculates and presents numerical pixel data obtained directly from images.
 *
 * Readouts are generated dynamically when the user moves a pointing device
 * cursor over an image in PixInsight. Generated readout data are shown on the
 * %Readout toolbar, normally located at the bottom of the core application's
 * main window, and are also shown on popup windows when the user clicks on the
 * image. Furthermore, when the user presses the mouse over an image, readout
 * values are sent to interface windows that request them (see
 * \ref readout_notifications). When those interfaces receive readout values,
 * they usually provide some visual feedback, or use them to modify their
 * behavior, as appropriate.
 *
 * With %ReadoutOptions you can specify two sets of parameters: a first set
 * that defines how readout values are calcualted, and a second set that
 * indicates how the numerical readout values are presented to the user.
 *
 * The first parameter set includes the color space in which data is generated
 * (e.g. RGB, HSV, CIE Lab, etc), the calculation mode used (average, median,
 * minimum or maximum), and the size of the square box used to calculate
 * readouts.
 *
 * The second set of parameters lets you choose between real and integer
 * readouts. Real readouts are normalized to the [0,1] interval, where 0=black
 * and 1=white. For real readouts you may also specify a resolution degree in
 * terms of the amount of decimal digits shown (from 0 to 15). Integer readouts
 * are given in the range from zero to an arbitrary maximum value that you can
 * specify from 1 to 2^32-1.
 *
 * \sa ProcessInterface
 */
class PCL_CLASS ReadoutOptions
{
public:

   /*!
    * Represents a pixel readout data mode.
    */
   typedef ReadoutData::value_type  readout_data;

   /*!
    * Represents a pixel readout calculation mode.
    */
   typedef ReadoutMode::value_type  readout_mode;

   /*!
    * Maximum allowed size of a square readout probe.
    */
   enum { MaxProbeSize = 15 };

   /*!
    * Maximum allowed number of decimal digits for real readout display.
    */
   enum { MaxPrecision = 15 };

   /*!
    * Minimum and maximum allowed sizes of a square readout preview.
    */
   enum { MinPreviewSize = 15, MaxPreviewSize = 127 };

   /*!
    * Maximum allowed zoom factor for readout previews.
    */
   enum { MaxPreviewZoomFactor = 16 };

   /*!
    * Constructs a default %ReadoutOptions object.
    */
   ReadoutOptions() :
      data( ReadoutData::RGBK ),
      mode( ReadoutMode::Mean ),
      probeSize( 1 ),
      previewSize( 25 ),
      previewZoom( 8 ),
      precision( 4 ),
      range( uint16_max ),
      showAlpha( true ),
      showMask( true ),
      showPreview( true ),
      previewCenter( true ),
      broadcast( true ),
      real( true )
   {
   }

   /*!
    * Copy constructor.
    */
   ReadoutOptions( const ReadoutOptions& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   ReadoutOptions& operator =( const ReadoutOptions& ) = default;

   /*!
    * Returns the current readout options from the PixInsight core application.
    */
   static ReadoutOptions GetCurrentOptions();

   /*!
    * Sets new readout options in the PixInsight core application.
    */
   static void SetCurrentOptions( const ReadoutOptions& );

   /*!
    * Returns the readout data mode in this %ReadoutOptions object.
    */
   readout_data Data() const
   {
      return data;
   }

   /*!
    * Sets the readout data mode in this %ReadoutOptions object.
    */
   void SetData( readout_data d )
   {
      data = d;
   }

   /*!
    * Returns the readout calculation mode in this %ReadoutOptions object.
    */
   readout_mode Mode() const
   {
      return mode;
   }

   /*!
    * Sets the readout calculation mode in this %ReadoutOptions object.
    */
   void SetMode( readout_mode m )
   {
      mode = m;
   }

   /*!
    * Returns true iff this %ReadoutOptions object enables presentation of alpha
    * channel pixel readouts.
    */
   bool IsAlphaChannelEnabled() const
   {
      return showAlpha;
   }

   /*!
    * Enables inclusion of alpha channel values on pixel readouts.
    */
   void EnableAlphaChannel( bool enable = true )
   {
      showAlpha = enable;
   }

   /*!
    * Disables inclusion of alpha channel values on pixel readouts.
    *
    * This is a convenience function, equivalent to:
    * EnableAlphaChannel( !disable )
    */
   void DisableAlphaChannel( bool disable = true )
   {
      EnableAlphaChannel( !disable );
   }

   /*!
    * Returns true iff this %ReadoutOptions object enables presentation of mask
    * channel pixel readouts.
    */
   bool IsMaskChannelEnabled() const
   {
      return showMask;
   }

   /*!
    * Enables inclusion of mask channel values on pixel readouts.
    */
   void EnableMaskChannel( bool enable = true )
   {
      showMask = enable;
   }

   /*!
    * Disables inclusion of mask channel values on pixel readouts.
    *
    * This is a convenience function, equivalent to:
    * EnableMaskChannel( !disable )
    */
   void DisableMaskChannel( bool disable = true )
   {
      EnableMaskChannel( !disable );
   }

   /*!
    * Returns the readout probe size in pixels.
    */
   int ProbeSize() const
   {
      return probeSize;
   }

   /*!
    * Sets the readout probe size in pixels. If a value of one is specified,
    * readouts will be calculated for a single pixel. The specified size \a sz
    * must be an odd integer in the range [1,15].
    */
   void SetProbeSize( int sz )
   {
      probeSize = pcl::Range( sz, 1, int( MaxProbeSize ) ) | 1;
   }

   /*!
    * Returns the size in pixels of the real-time readout preview.
    */
   int PreviewSize() const
   {
      return previewSize;
   }

   /*!
    * Sets the size in pixels of the real-time readout preview. The specified
    * size \a sz must be an odd integer in the range [15,127].
    */
   void SetPreviewSize( int sz )
   {
      previewSize = pcl::Range( sz, int( MinPreviewSize ), int( MaxPreviewSize ) ) | 1;
   }

   /*!
    * Returns the zoom factor applied to real-time readout previews. The
    * returned value has the same meaning as for ImageWindow::ZoomFactor() and
    * ImageWindow::SetViewport().
    */
   int PreviewZoomFactor() const
   {
      return previewZoom;
   }

   /*!
    * Sets the zoom factor to apply for generation of real-time readout
    * previews. The specified \a zoom factor must be in the range [1,16], and
    * has the same meaning as zoom factors for ImageWindow::SetViewport().
    */
   void SetPreviewZoomFactor( int zoom )
   {
      previewZoom = pcl::Range( zoom, 1, int( MaxPreviewZoomFactor ) );
   }

   /*!
    * Returns true iff this %ReadoutOptions object enables generation of
    * real-time readout previews.
    */
   bool IsPreviewEnabled() const
   {
      return showPreview;
   }

   /*!
    * Enables generation of real-time readout previews.
    */
   void EnablePreview( bool enable = true )
   {
      showPreview = enable;
   }

   /*!
    * Disables generation of real-time readout previews.
    */
   void DisablePreview( bool disable = true )
   {
      EnablePreview( !disable );
   }

   /*!
    * Returns true iff this %ReadoutOptions object enables crosshair center
    * lines on real-time readout previews.
    */
   bool IsPreviewCenterEnabled() const
   {
      return previewCenter;
   }

   /*!
    * Enables or disables crosshair center lines on real-time readout previews.
    */
   void EnablePreviewCenter( bool enable = true )
   {
      previewCenter = enable;
   }

   /*!
    * Disables or enables crosshair center lines on real-time readout previews.
    */
   void DisablePreviewCenter( bool disable = true )
   {
      EnablePreviewCenter( !disable );
   }

   /*!
    * Returns true iff this %ReadoutOptions object enables broadcasting of
    * <em>readout notifications</em>.
    * \sa \ref readout_notifications "readout notifications"
    */
   bool IsBroadcastEnabled() const
   {
      return broadcast;
   }

   /*!
    * Enables or disables broadcasting of <em>readout notifications</em>.
    * \sa \ref readout_notifications "readout notifications"
    */
   void EnableBroadcast( bool enable = true )
   {
      broadcast = enable;
   }

   /*!
    * Disables or enables broadcasting of <em>readout notifications</em>.
    * \sa \ref readout_notifications "readout notifications"
    */
   void DisableBroadcast( bool disable = true )
   {
      EnableBroadcast( !disable );
   }

   /*!
    * Returns true iff this %ReadoutOptions object enables floating point real
    * pixel readouts.
    */
   bool IsReal() const
   {
      return real;
   }

   /*!
    * Enables or disables floating point real pixel readouts.
    */
   void SetReal( bool b = true )
   {
      real = b;
   }

   /*!
    * Returns true iff this %ReadoutOptions object disables floating point real
    * pixel readouts.
    *
    * This is a convenience function, equivalent to:
    * !IsReal().
    */
   bool IsInteger() const
   {
      return !IsReal();
   }

   /*!
    * Enables or disables integer pixel readouts.
    *
    * This is a convenience function, equivalent to:
    * SetReal( !b )
    */
   void SetInteger( bool b = true )
   {
      SetReal( !b );
   }

   /*!
    * Returns the precision for floating point real pixel readouts. The
    * returned value is the number of decimal digits employed to represent real
    * pixel readouts.
    */
   int Precision() const
   {
      return precision;
   }

   /*!
    * Sets the precision for floating point real pixel readouts. The specified
    * value is the number \a n of decimal digits employed to represent real
    * pixel readouts.
    */
   void SetPrecision( int n )
   {
      precision = pcl::Range( n, 0, int( MaxPrecision ) );
   }

   /*!
    * Returns the maximum value of integer pixel readouts.
    */
   unsigned IntegerRange() const
   {
      return range;
   }

   /*!
    * Sets the maximum value of integer pixel readouts.
    */
   void SetIntegerRange( unsigned n )
   {
      range = pcl::Range( uint32( n ), uint32( 1 ), ~uint32( 0 ) );
   }

   /*!
    * Selects the 8-bit integer readout range.
    *
    * This is a convenience function, equivalent to:
    * SetIntegerRange( uint8_max )
    */
   void Set8BitRange()
   {
      SetIntegerRange( uint8_max );
   }

   /*!
    * Selects the 16-bit integer readout range.
    *
    * This is a convenience function, equivalent to:
    * SetIntegerRange( uint16_max )
    */
   void Set16BitRange()
   {
      SetIntegerRange( uint16_max );
   }

   /*!
    * Selects the 32-bit integer readout range.
    *
    * This is a convenience function, equivalent to:
    * SetIntegerRange( uint32_max )
    */
   void Set32BitRange()
   {
      SetIntegerRange( uint32_max );
   }

#ifdef __PCL_BUILDING_PIXINSIGHT_APPLICATION
protected:
#else
private:
#endif

   readout_data data;
   readout_mode mode;
   int          probeSize;    // size of the square probe - must be an odd number
   int          previewSize;  // size of the square preview - must be an odd number
   int          previewZoom;  // readout preview zoom factor >= 1
   int          precision;    // number of decimal digits if real==true
   unsigned     range;        // maximum discrete value if real==false
   bool         showAlpha     : 1;  // show alpha channel readouts?
   bool         showMask      : 1;  // show mask channel readouts?
   bool         showPreview   : 1;  // show real-time readout previews?
   bool         previewCenter : 1;  // draw center crosshairs on readout previews?
   bool         broadcast     : 1;  // broadcast readouts?
   bool         real          : 1;  // true=real, false=integer
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ReadoutOptions_h

// ----------------------------------------------------------------------------
// EOF pcl/ReadoutOptions.h - Released 2017-04-14T23:04:40Z
