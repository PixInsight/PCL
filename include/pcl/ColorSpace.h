//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/ColorSpace.h - Released 2018-11-01T11:06:36Z
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

#ifndef __PCL_ColorSpace_h
#define __PCL_ColorSpace_h

/// \file pcl/ColorSpace.h

#include <pcl/Defs.h>

#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::ColorSpace
 * \brief     Supported color spaces
 *
 * Current versions of the PixInsight platform support the following color
 * spaces for images:
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>ColorSpace::Unknown</td> <td>Corresponds to an unknown or unsupported color space</td></tr>
 * <tr><td>ColorSpace::Gray</td>    <td>Grayscale monochrome space</td></tr>
 * <tr><td>ColorSpace::RGB</td>     <td>RGB color space</td></tr>
 * <tr><td>ColorSpace::CIEXYZ</td>  <td>CIE XYZ color space</td></tr>
 * <tr><td>ColorSpace::CIELab</td>  <td>CIE L*a*b* color space</td></tr>
 * <tr><td>ColorSpace::CIELch</td>  <td>CIE L*c*h* color space</td></tr>
 * <tr><td>ColorSpace::HSV</td>     <td>HSV color space: Hue, Saturation, Value</td></tr>
 * <tr><td>ColorSpace::HSI</td>     <td>HSI color space: Hue, Saturation, Intensity</td></tr>
 * </table>
 */
namespace ColorSpace
{
   enum value_type
   {
      Unknown = -1,  // Corresponds to an unknown or unsupported color space
      Gray = 0,      // Grayscale monochrome space
      RGB,           // RGB color space
      CIEXYZ,        // CIE XYZ color space
      CIELab,        // CIE L*a*b* color space
      CIELch,        // CIE L*c*h* color space
      HSV,           // HSV color space: Hue, Saturation, Value
      HSI,           // HSI color space: Hue, Saturation, Intensity
      NumberOfColorSpaces
   };

   /*!
    * Returns the number of nominal channels in the specified color space.
    */
   inline int NumberOfNominalChannels( int colorSpace )
   {
      return (colorSpace == Gray) ? 1 : 3;
   }

   /*!
    * Returns a string representing the name of a color space.
    *
    * \param colorSpace    A supported color space, identified by its
    *                      corresponding symbolic constant.
    */
   String Name( int colorSpace );

   /*!
    * Returns the identifier of a nominal channel in a specified color space.
    *
    * \param colorSpace    A supported color space, identified by its
    *                      corresponding symbolic constant.
    *
    * \param channel       The index >= 0 of a nominal channel or component.
    */
   String ChannelId( int colorSpace, int channel );
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ColorSpace_h

// ----------------------------------------------------------------------------
// EOF pcl/ColorSpace.h - Released 2018-11-01T11:06:36Z
