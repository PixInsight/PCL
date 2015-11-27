//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// pcl/ImageInfo.h - Released 2015/11/26 15:59:39 UTC
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

#ifndef __PCL_ImageInfo_h
#define __PCL_ImageInfo_h

/// \file pcl/ImageInfo.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Relational_h
#include <pcl/Relational.h>
#endif

#ifndef __PCL_AbstractImage_h
#include <pcl/AbstractImage.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class ImageInfo
 * \brief A simple structure to hold basic information about images
 */
class PCL_CLASS ImageInfo
{
public:

   int   width;            //!< %Image width in pixels
   int   height;           //!< %Image height in pixels
   int   numberOfChannels; //!< Number of channels, including alpha channels
   int   colorSpace;       //!< %Color space, compatible with ImageColor::color_space
   bool  supported;        //!< Whether we support this image

   /*!
    * Constructs an %ImageInfo object with default values.
    */
   ImageInfo()
   {
      Reset();
   }

   /*!
    * Copy constructor.
    */
   ImageInfo( const ImageInfo& ) = default;

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   ImageInfo& operator =( const ImageInfo& ) = default;

   /*!
    * Constructs an %ImageInfo object with data obtained from the specified
    * \a image.
    *
    * The width, height and numberOfChannels data members are set to the
    * dimensions of the selected rectangle and the number of selected channels
    * in \a image, respectively.
    */
   ImageInfo( const AbstractImage& image )
   {
      width            = image.SelectedRectangle().Width();
      height           = image.SelectedRectangle().Height();
      numberOfChannels = image.NumberOfSelectedChannels();
      colorSpace       = image.ColorSpace();
      supported        = IsValid();
   }

   /*!
    * Returns the number of pixels in the image, or width*height.
    */
   size_type NumberOfPixels() const
   {
      return size_type( width )*size_type( height );
   }

   /*!
    * Returns the number of samples in the image, or
    * width*height*numberOfChannels.
    */
   size_type NumberOfSamples() const
   {
      return NumberOfPixels()*size_type( numberOfChannels );
   }

   /*!
    * Initializes this %ImageInfo structure with default values, corresponding
    * to an empty image.
    */
   void Reset()
   {
      width = height = numberOfChannels = 0;
      colorSpace = ColorSpace::Unknown;
      supported = false;
   }

   /*!
    * Returns true iff this object describes a valid nonempty image.
    */
   bool IsValid() const
   {
      return width > 0 &&
             height > 0 &&
             colorSpace != ColorSpace::Unknown &&
             numberOfChannels >= ((colorSpace == ColorSpace::Gray) ? 1 : 3);
   }
};

// ----------------------------------------------------------------------------

/*!
 * Returns true iff two ImageInfo structures \a a and \a b are equal.
 */
inline bool operator ==( const ImageInfo& a, const ImageInfo& b )
{
   return a.width == b.width &&
          a.height == b.height &&
          a.numberOfChannels == b.numberOfChannels &&
          a.colorSpace == b.colorSpace;
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageInfo_h

// ----------------------------------------------------------------------------
// EOF pcl/ImageInfo.h - Released 2015/11/26 15:59:39 UTC
