// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/ImageColor.h - Released 2014/10/29 07:34:13 UTC
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

#ifndef __PCL_ImageColor_h
#define __PCL_ImageColor_h

/// \file pcl/ImageColor.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_ColorSpace_h
#include <pcl/ColorSpace.h>
#endif

#ifndef __PCL_RGBColorSystem_h
#include <pcl/RGBColorSystem.h>
#endif

#ifndef __PCL_String_h
#include <pcl/String.h>
#endif

#ifndef __PCL_Exception_h
#include <pcl/Exception.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

#define m_colorSpace m_color->colorSpace
#define m_RGBWS      m_color->RGBWS

// ----------------------------------------------------------------------------

/*!
 * \class ImageColor
 * \brief Implements color space properties of images.
 *
 * %ImageColor defines the color space and the RGB Working Space for an image.
 *
 * Supported color spaces are defined and enumerated by the ColorSpace class.
 *
 * A RGB Working Space (RGBWS) is used by the PCL and the PixInsight core
 * application to perform luminance/chrominance separations and color space
 * conversions. Note that a RGBWS has nothing to do with color management and
 * ICC profiles in the PixInsight/PCL environment. A RGBWS is used for image
 * processing tasks exclusively, and \e never for color management tasks.
 *
 * RGB working spaces are implemented by the RGBColorSystem class.
 *
 * Note that for a shared image (i.e. an image living in the PixInsight core
 * application) the RGBWS is controlled exclusively by its parent ImageWindow.
 *
 * \sa ImageGeometry, AbstractImage, GenericImage
 */
class PCL_CLASS ImageColor
{
public:

   /*!
    * An enumerated type that represents the set of supported color spaces.
    * Valid constants for this enumeration are defined in the ColorSpace
    * namespace.
    */
   typedef ColorSpace::value_type   color_space;

   /*!
    * Returns a reference to the RGB working space associated with this image.
    */
   const RGBColorSystem& RGBWorkingSpace() const
   {
      return m_RGBWS;
   }

   /*!
    * Associates a given RGB working space (RGBWS) with this image.
    *
    * \note For shared images (i.e. images living in the PixInsight core
    * application), the RGB working space cannot be changed by calling this
    * function. This is because the RGBWS of a shared image is a property
    * controlled by its parent image window. See the documentation for the
    * ImageWindow class.
    */
   virtual void SetRGBWorkingSpace( const RGBColorSystem& RGBWS )
   {
      m_RGBWS = RGBWS;
   }

   /*!
    * Returns a reference to the current default RGB working space (RGBWS). If
    * you don't change it, the factory-default RGBWS corresponds to the sRGB
    * color space.
    *
    * \note This function only works for local images. For shared images, the
    * default RGBWS is controlled by the ImageWindow class.
    */
   static const RGBColorSystem& DefaultRGBWorkingSpace()
   {
      return s_defaultRGBWS;
   }

   /*!
    * Selects the specified RGB working space \a RGBWS as the default RGB
    * working space for local images. The default RGBWS is associated with
    * newly created local images; existing images won't be affected by this
    * setting.
    *
    * \note This function only works for local images. For shared images, the
    * default RGBWS is controlled exclusively by the ImageWindow class.
    */
   static void SetDefaultRGBWorkingSpace( const RGBColorSystem& RGBWS )
   {
      s_defaultRGBWS = RGBWS;
   }

   /*!
    * Resets the default RGB working space (RGBWS) to the factory-default sRGB
    * color space.
    */
   static void ResetDefaultRGBWorkingSpace()
   {
      s_defaultRGBWS = RGBColorSystem::sRGB;
   }

   /*!
    * Returns true if this is a color image. Returns false if this is a
    * grayscale monochrome image.
    */
   bool IsColor() const
   {
      return m_colorSpace != ColorSpace::Gray;
   }

   /*!
    * Returns the color space of this image. This function returns the value of
    * a symbolic constant enumerated by the ColorSpace namespace.
    */
   color_space ColorSpace() const
   {
      return m_colorSpace;
   }

   /*!
    * Returns the identifier of a nominal channel \a c in the current color
    * space of this image.
    */
   String ChannelId( int c ) const
   {
      return ColorSpace::ChannelId( m_colorSpace, c );
   }

protected:

   struct Color
   {
      color_space    colorSpace;
      RGBColorSystem RGBWS;

      Color( color_space cs = ColorSpace::Gray ) : colorSpace( cs ), RGBWS( s_defaultRGBWS )
      {
      }

      Color( const Color& x ) : colorSpace( x.colorSpace ), RGBWS( x.RGBWS )
      {
      }

      void Assign( const Color& x )
      {
         colorSpace = x.colorSpace;
         RGBWS = x.RGBWS;
      }

      void Reset()
      {
         colorSpace = ColorSpace::Gray;
      }
   };

   Color* m_color;

   /*
    * The default RGB Working Space
    *
    * Unless explicitly modified, this corresponds to the sRGB space.
    *
    * Note that shared images don't use this default RGBWS, but the global
    * RGBWS currently selected in the PixInsight core application.
    */
   static RGBColorSystem s_defaultRGBWS;

   ImageColor() : m_color( 0 )
   {
   }

   ImageColor( const ImageColor& x ) : m_color( x.m_color )
   {
   }

   virtual ~ImageColor()
   {
      m_color = 0;
   }

   ImageColor& operator =( const ImageColor& x )
   {
      m_color = x.m_color;
      return *this;
   }

   void Swap( ImageColor& image )
   {
      pcl::Swap( m_color, image.m_color );
   }
};

// ----------------------------------------------------------------------------

#undef m_colorSpace
#undef m_RGBWS

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_ImageColor_h

// ****************************************************************************
// EOF pcl/ImageColor.h - Released 2014/10/29 07:34:13 UTC
