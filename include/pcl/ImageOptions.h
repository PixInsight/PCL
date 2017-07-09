//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/ImageOptions.h - Released 2017-07-09T18:07:07Z
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

#ifndef __PCL_ImageOptions_h
#define __PCL_ImageOptions_h

/// \file pcl/ImageOptions.h

#include <pcl/Defs.h>

#include <pcl/String.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \namespace pcl::CFAType
 * \brief     Color Filter Array types.
 *
 * A CFA (Color Filter Array) is a mosaic of color filters placed in front of
 * an image sensor, such as CMOS sensors of digital cameras. This enumeration
 * defines the CFA types currently supported in PCL.
 *
 * <table border="1" cellpadding="4" cellspacing="0">
 * <tr><td>CFAType::None</td> <td>The image uses no CFA pattern, or the CFA pattern is unsupported.</td></tr>
 * <tr><td>CFAType::BGGR, CFAType::GRBG, CFAType::GBRG, CFAType::RGGB</td> <td>RGB CFA types.</td></tr>
 * <tr><td>CFAType::CYGM</td> <td>Cyan/Yellow/Green/Magenta CFA (used by a few digital cameras).</td></tr>
 * </table>
 */
namespace CFAType
{
   enum value_type { None, BGGR, GRBG, GBRG, RGGB, CYGM, NumberOfCFATypes };

   /*!
    * Returns a string representing a type of color filter array.
    *
    * \param cfa  A supported CFA type, identified by its corresponding
    *             symbolic constant.
    */
   String Name( int cfa );
}

/*!
 * \class ImageOptions
 * \brief Format-independent fundamental image stream options.
 */
class PCL_CLASS ImageOptions
{
public:

   uint8    bitsPerSample         :  8; //!< Number of bits per sample: 8, 16, 32 or 64. The default value is 16.
   bool     ieeefpSampleFormat    :  1; //!< Sample type: IEEE 754 floating point (true) or integers (false). The default value is false.
   bool     complexSample         :  1; //!< Sample type: Complex (true) or real (false) samples. The default value is false.
   bool     signedIntegers        :  1; //!< Integer sample values are signed (true) or unsigned (false).
   bool     metricResolution      :  1; //!< Resolution units: centimeter (true) or inch (false). The default value is false.
   bool     readNormalized        :  1; //!< Normalize floating-point sample values after reading. The default value is true.
   bool     embedICCProfile       :  1; //!< Embed an ICC profile (when available). The default value is true.
   bool     embedThumbnail        :  1; //!< Embed thumbnail images (when available). The default value is false.
   bool     embedProperties       :  1; //!< Embed data properties (when available). The default value is true.
   bool     embedRGBWS            :  1; //!< Embed RGB working space parameters.
   bool     embedDisplayFunction  :  1; //!< Embed display function (aka STF) parameters.
   bool     embedColorFilterArray :  1; //!< Embed color filter array (CFA) description.
   bool     embedPreviewRects     :  1; //!< Embed preview rectangles and identifiers.
   int      __rsv__               : 12; // Reserved for future extensions; must be zero.

   double   lowerRange;    //!< Lower limit to normalize read floating-point samples. The default value is 0.
   double   upperRange;    //!< Upper limit to normalize read floating-point samples. The default value is 1.

   double   xResolution;   //!< Horizontal resolution in pixels per resolution unit. The default value is 72.
   double   yResolution;   //!< Vertical resolution in pixels per resolution unit. The default value is 72.

   int      isoSpeed;      //!< ISO speed as specified in ISO 12232 (0=unknown)
   float    exposure;      //!< Exposure in seconds (0=unknown)
   float    aperture;      //!< Focal length divided by lens diameter (F/D) (0=unknown)
   float    focalLength;   //!< Focal length in millimeters (0=unknown)
   int      cfaType;       //!< Color Filter Array type. See CFAType namespace for possible values (0=none).

   /*!
    * Constructs an %ImageOptions structure with default values.
    */
   ImageOptions()
   {
      Reset();
   }

   /*!
    * Copy constructor.
    */
   ImageOptions( const ImageOptions& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   ImageOptions& operator =( const ImageOptions& ) = default;

   /*!
    * Initializes all data members to default values.
    */
   void Reset()
   {
      bitsPerSample         = 16;
      ieeefpSampleFormat    = false;
      complexSample         = false;
      signedIntegers        = false;
      metricResolution      = false;
      readNormalized        = true;
      embedICCProfile       = true;
      embedThumbnail        = false;
      embedProperties       = true;
      embedRGBWS            = true;
      embedDisplayFunction  = true;
      embedColorFilterArray = true;
      embedPreviewRects     = false;
      __rsv__               = 0;
      lowerRange            = 0;
      upperRange            = 1;
      xResolution           = 72;
      yResolution           = 72;
      isoSpeed              = 0;
      exposure              = 0;
      aperture              = 0;
      focalLength           = 0;
      cfaType               = CFAType::None;
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageOptions_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageOptions.h - Released 2017-07-09T18:07:07Z
