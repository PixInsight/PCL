//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// pcl/ImageRenderingModes.h - Released 2015/07/30 17:15:18 UTC
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

#ifndef __PCL_ImageRenderingModes_h
#define __PCL_ImageRenderingModes_h

/// \file pcl/ImageRenderingModes.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Flags_h
#include <pcl/Flags.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace DisplayChannel
 * \brief     Rendering modes for image display and bitmap renditions.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>DisplayChannel::Invalid</td>             <td>Represents an invalid or undefined display mode</td></tr>
 * <tr><td>DisplayChannel::RGBK</td>                <td>Composite RGB or Gray (normal display mode)</td></tr>
 * <tr><td>DisplayChannel::Red</td>                 <td>Red channel as grayscale</td></tr>
 * <tr><td>DisplayChannel::Green</td>               <td>Green channel as grayscale</td></tr>
 * <tr><td>DisplayChannel::Blue</td>                <td>Blue channel as grayscale</td></tr>
 * <tr><td>DisplayChannel::CIE_L</td>               <td>CIE L* component</td></tr>
 * <tr><td>DisplayChannel::Lightness</td>           <td>A synomym for DisplayChannel::CIE_L</td></tr>
 * <tr><td>DisplayChannel::ChrominanceRG</td>       <td>Chrominance, CIE a*,b* as red+green composite</td></tr>
 * <tr><td>DisplayChannel::ChrominanceLFixed</td>   <td>Chrominance, CIE L* fixed to 0.5</td></tr>
 * <tr><td>DisplayChannel::CIE_a</td>               <td>CIE a* component</td></tr>
 * <tr><td>DisplayChannel::CIE_b</td>               <td>CIE b* component</td></tr>
 * <tr><td>DisplayChannel::CIE_c</td>               <td>CIE c* component</td></tr>
 * <tr><td>DisplayChannel::CIE_h</td>               <td>CIE h* component</td></tr>
 * <tr><td>DisplayChannel::CIE_X</td>               <td>CIE X component</td></tr>
 * <tr><td>DisplayChannel::CIE_Y</td>               <td>CIE Y component</td></tr>
 * <tr><td>DisplayChannel::CIE_Z</td>               <td>CIE Z component</td></tr>
 * <tr><td>DisplayChannel::Hue</td>                 <td>Hue from the HSV/HSI spaces</td></tr>
 * <tr><td>DisplayChannel::SaturationHSV</td>       <td>Saturation from HSV</td></tr>
 * <tr><td>DisplayChannel::SaturationHSI</td>       <td>Saturation from HSI</td></tr>
 * <tr><td>DisplayChannel::Value</td>               <td>Value from HSV (V = Max(R,G,B))</td></tr>
 * <tr><td>DisplayChannel::Intensity</td>           <td>Intensity from HSI (I = 0.5*(Min(R,G,B) + Max(R,G,B)))</td></tr>
 * <tr><td>DisplayChannel::Alpha</td>               <td>First alpha channel (the rest as Alpha+n)</td></tr>
 * </table>
 */
namespace DisplayChannel
{
   enum value_type
   {
      Invalid = -1,
      RGBK = 0,            // Composite RGB or Gray (normal display mode)
      Red,                 // Red channel as grayscale
      Green,               // Green channel as grayscale
      Blue,                // Blue channel as grayscale
      CIE_L,               // CIE L*
      Lightness = CIE_L,   // Lightness = CIE L*
      Luminance = CIE_L,   // ### Deprecated - retained for compatibility -> suppress in PCL 2.0
      ChrominanceRG,       // Chrominance, CIE a*,b* as red+green composite
      ChrominanceLFixed,   // Chrominance, CIE L* fixed to 0.5
      CIE_a,               // CIE a*
      CIE_b,               // CIE b*
      CIE_c,               // CIE c*
      CIE_h,               // CIE h*
      CIE_X,               // CIE X
      CIE_Y,               // CIE Y
      CIE_Z,               // CIE Z
      Hue,                 // Hue from the HSV/HSI spaces
      SaturationHSV,       // Saturation from HSV
      SaturationHSI,       // Saturation from HSI
      Value,               // Value from HSV (V = Max(R,G,B))
      Intensity,           // Intensity from HSI (I = 0.5*(Min(R,G,B) + Max(R,G,B)))
      Alpha                // First alpha channel (the rest as Alpha+n)
   };

   inline bool IsLuminanceOrChrominance( value_type c )
   {
      return c >= Lightness && c < Hue;
   }
}

// ----------------------------------------------------------------------------

/*!
 * \namespace MaskMode
 * \brief     Mask rendering modes for image display and bitmap renditions.
 *
 * Mask rendering modes define how masked pixels are represented on bitmap
 * renditions of images. The \e traditional mode used by PixInsight has always
 * been MaskMode::Multiply, which simply multiplies each pixel of a screen
 * rendition by its mask image counterpart.
 *
 * Since PixInsight version 1.5.5, an additional set of \e overlay mask
 * rendering modes is available, providing more accurate representations in
 * several user-selectable colors. A new \e replace mode has also been
 * implemented to facilitate blinking comparisons between images and masks. The
 * default mask rendering mode has been set to MaskMode::OverlayRed.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>MaskMode::Replace</td>        <td>Replaces the image by the mask.</td></tr>
 * <tr><td>MaskMode::Multiply</td>       <td>Multiplies the image by the mask. This is the \e traditional mask rendering mode used by PixInsight before version 1.5.5.</td></tr>
 * <tr><td>MaskMode::OverlayRed</td>     <td>Represents mask-protected pixels as red pixels on screen renditions.</td></tr>
 * <tr><td>MaskMode::OverlayGreen</td>   <td>Represents mask-protected pixels as green pixels on screen renditions.</td></tr>
 * <tr><td>MaskMode::OverlayBlue</td>    <td>Represents mask-protected pixels as blue pixels on screen renditions.</td></tr>
 * <tr><td>MaskMode::OverlayYellow</td>  <td>Represents mask-protected pixels as yellow pixels (red+green) on screen renditions.</td></tr>
 * <tr><td>MaskMode::OverlayMagenta</td> <td>Represents mask-protected pixels as magenta pixels (red+blue) on screen renditions.</td></tr>
 * <tr><td>MaskMode::OverlayCyan</td>    <td>Represents mask-protected pixels as cyan pixels (green+blue) on screen renditions.</td></tr>
 * <tr><td>MaskMode::OverlayOrange</td>  <td>Represents mask-protected pixels as orange pixels (red + 1/2*green) on screen renditions.</td></tr>
 * <tr><td>MaskMode::OverlayViolet</td>  <td>Represents mask-protected pixels as violet pixels (1/2*red + blue) on screen renditions.</td></tr>
 * <tr><td>MaskMode::Default</td>        <td>Identifies the default mask rendering mode used by the PixInsight core application. Currently this corresponds to MaskMode::OverlayRed.</td></tr>
 * </table>
 */
namespace MaskMode
{
   enum value_type
   {
      Replace,             // Replaces the image by the mask
      Multiply,            // Multiplies the image by the mask (PI's traditional mode)
      OverlayRed,          // Red mask
      OverlayGreen,        // Green mask
      OverlayBlue,         // Blue mask
      OverlayYellow,       // Yellow mask (red+green)
      OverlayMagenta,      // Magenta mask (red+blue)
      OverlayCyan,         // Cyan mask (green+blue)
      OverlayOrange,       // Orange mask (red + 1/2*green)
      OverlayViolet,       // Violet mask (1/2*red + blue)

      NumberOfModes,

      Default = OverlayRed
   };
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageRenderingModes_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageRenderingModes.h - Released 2015/07/30 17:15:18 UTC
