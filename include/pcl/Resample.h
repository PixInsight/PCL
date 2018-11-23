//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// pcl/Resample.h - Released 2018-11-23T16:14:19Z
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

#ifndef __PCL_Resample_h
#define __PCL_Resample_h

/// \file pcl/Resample.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/GeometricTransformation.h>
#include <pcl/ImageResolution.h>
#include <pcl/ParallelProcess.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
   \namespace pcl::ResizeMode
   \brief     Resizing modes for the Resample process.

   <table border="1" cellpadding="4" cellspacing="0">
   <tr><td>ResizeMode::RelativeDimensions</td>  <td>Resize relative to current image dimensions</td></tr>
   <tr><td>ResizeMode::AbsolutePixels</td>      <td>Resize to absolute dimensions in pixels</td></tr>
   <tr><td>ResizeMode::AbsoluteCentimeters</td> <td>Resize to absolute dimensions in centimeters</td></tr>
   <tr><td>ResizeMode::AbsoluteInches</td>      <td>Resize to absolute dimensions in inches</td></tr>
   <tr><td>ResizeMode::ForceArea</td>           <td>Force the total number of pixels and keep existing aspect ratio</td></tr>
   <tr><td>ResizeMode::Default</td>             <td>Default resizing mode, equal to ResizeMode::RelativeDimensions</td></tr>
   </table>
*/
namespace ResizeMode
{
   enum value_type
   {
      RelativeDimensions,  // Resize relative to current image dimensions
      AbsolutePixels,      // ... to absolute dimensions in pixels
      AbsoluteCentimeters, // ... to absolute dimensions in centimeters
      AbsoluteInches,      // ... to absolute dimensions in inches
      ForceArea,           // Force number of pixels, keep aspect ratio
      NumberOfResizeModes,
      Default = RelativeDimensions
   };
}

// ----------------------------------------------------------------------------

/*!
   \namespace pcl::AbsoluteResizeMode
   \brief     Absolute resizing modes for the Resample process.

   Absolute resize modes are applicable only when the main resize mode is
   ResizeMode::AbsolutePixels, ResizeMode::AbsoluteCentimeters or
   ResizeMode::AbsoluteInches.

   <table border="1" cellpadding="4" cellspacing="0">
   <tr><td>AbsoluteResizeMode::ForceWidthAndHeight</td> <td>Force both image dimensions</td></tr>
   <tr><td>AbsoluteResizeMode::ForceWidth</td>          <td>Force width</td> preserve existing aspect ratio</td></tr>
   <tr><td>AbsoluteResizeMode::ForceHeight</td>         <td>Force height</td> preserve existing aspect ratio</td></tr>
   <tr><td>AbsoluteResizeMode::Default</td>             <td>Default absolute resizing mode</td> equal to AbsoluteResizeMode::ForceWidthAndHeight</td></tr>
   </table>
*/
namespace AbsoluteResizeMode
{
   enum value_type
   {
      ForceWidthAndHeight, // Force both dimensions
      ForceWidth,          // Force width, preserve aspect ratio
      ForceHeight,         // Force height, preserve aspect ratio
      NumberOfAbsoluteResizeModes,
      Default = ForceWidthAndHeight
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class Resample
 * \brief %Image resampling algorithm.
 *
 * ### TODO: Write a detailed description for %Resample
 */
class PCL_CLASS Resample : public InterpolatingGeometricTransformation,
                           public ImageResolution,
                           public ParallelProcess
{
public:

   /*!
    * Represents a resizing mode.
    */
   typedef ResizeMode::value_type         resize_mode;

   /*!
    * Represents an absolute resizing mode.
    */
   typedef AbsoluteResizeMode::value_type abs_resize_mode;

   /*!
    * Constructs a default %Resample object using the specified pixel
    * interpolation \a p and a single scaling ratio for both axes.
    *
    * The specified pixel interpolation object \a p must remain valid while
    * this object exists.
    *
    * The resizing mode is set relative to current image dimensions.
    */
   Resample( PixelInterpolation& p, double scale = 1.0 ) :
      InterpolatingGeometricTransformation( p ),
      m_xSize( scale ), m_ySize( scale )
   {
   }

   /*!
    * Constructs a default %Resample object using the specified pixel
    * interpolation \a p and separate scaling ratios for the X and Y axes,
    * \a scaleX and \a scaleY respectively.
    *
    * The specified pixel interpolation object \a p must remain valid while
    * this object exists.
    *
    * The resizing mode is set relative to current image dimensions.
    */
   Resample( PixelInterpolation& p, double scaleX, double scaleY ) :
      InterpolatingGeometricTransformation( p ),
      m_xSize( scaleX ), m_ySize( scaleY )
   {
   }

   /*!
    * Copy constructor.
    */
   Resample( const Resample& ) = default;

   /*!
    * Gets the resampled image \a width and \a height.
    *
    * Use GetSizes(), XSize() and YSize() semantics to define a %Resample
    * instance that resizes images to fixed dimensions.
    */
   void GetSizes( double& width, double& height ) const
   {
      width = m_xSize; height = m_ySize;
   }

   /*!
    * Returns the resampled image width.
    *
    * Use GetSizes(), XSize() and YSize() semantics to define a %Resample
    * instance that resizes images to fixed dimensions.
    */
   double XSize() const
   {
      return m_xSize;
   }

   /*!
    * Returns the resampled image height.
    *
    * Use GetSizes(), XSize() and YSize() semantics to define a %Resample
    * instance that resizes images to fixed dimensions.
    */
   double YSize() const
   {
      return m_ySize;
   }

   /*!
    * Sets the resampled image \a width and \a height.
    *
    * Use SetSizes(), SetXSize() and SetYSize() semantics to define a %Resample
    * instance that resizes images to fixed dimensions.
    */
   void SetSizes( double width, double height )
   {
      m_xSize = width; m_ySize = height;
   }

   /*!
    * Sets the resampled image \a width.
    *
    * Use SetSizes(), SetXSize() and SetYSize() semantics to define a %Resample
    * instance that resizes images to fixed dimensions.
    */
   void SetXSize( double width )
   {
      m_xSize = width;
   }

   /*!
    * Sets the resampled image \a height.
    *
    * Use SetSizes(), SetXSize() and SetYSize() semantics to define a %Resample
    * instance that resizes images to fixed dimensions.
    */
   void SetYSize( double height )
   {
      m_ySize = height;
   }

   /*!
    * Gets the scaling factors \a sx and \a sy for the X and Y axes,
    * respectively.
    *
    * Use GetScalingFactors(), XScale() and YScale() semantics to define a
    * %Resample instance that resizes images relative to their current
    * dimensions.
    */
   void GetScalingFactors( double& sx, double& sy ) const
   {
      GetSizes( sx, sy );
   }

   /*!
    * Returns the horizontal scaling factor.
    *
    * Use GetScalingFactors(), XScale() and YScale() semantics to define a
    * %Resample instance that resizes images relative to their current
    * dimensions.
    */
   double XScale() const
   {
      return XSize();
   }

   /*!
    * Returns the vertical scaling factor.
    *
    * Use GetScalingFactors(), XScale() and YScale() semantics to define a
    * %Resample instance that resizes images relative to their current
    * dimensions.
    */
   double YScale() const
   {
      return YSize();
   }

   /*!
    * Sets the scaling factors \a sx and \a sy for the X and Y axes,
    * respectively.
    *
    * Use SetScalingFactors(), SetXScale() and SetYScale() semantics to define
    * a %Resample instance that resizes images relative to their current
    * dimensions.
    */
   void SetScalingFactors( double sx, double sy )
   {
      SetSizes( sx, sy );
   }

   /*!
    * Sets the horizontal scaling factor.
    *
    * Use SetScalingFactors(), SetXScale() and SetYScale() semantics to define
    * a %Resample instance that resizes images relative to their current
    * dimensions.
    */
   void SetXScale( double sx )
   {
      SetXSize( sx );
   }

   /*!
    * Sets the vertical scaling factor.
    *
    * Use SetScalingFactors(), SetXScale() and SetYScale() semantics to define
    * a %Resample instance that resizes images relative to their current
    * dimensions.
    */
   void SetYScale( double sy )
   {
      SetYSize( sy );
   }

   /*!
    * Returns the current resize mode.
    */
   resize_mode Mode() const
   {
      return m_mode;
   }

   /*!
    * Returns true iff this %Resample object resizes images relative to their
    * current dimensions, i.e. when Mode() is equal to
    * ResizeMode::RelativeDimensions.
    */
   bool IsRelative() const
   {
      return m_mode == ResizeMode::RelativeDimensions;
   }

   /*!
    * Returns true iff this %Resample object resizes images to fixed dimensions,
    * i.e. when Mode() is not equal to ResizeMode::RelativeDimensions.
    */
   bool IsAbsolute() const
   {
      return !IsRelative();
   }

   /*!
    * Sets the current resize mode.
    */
   void SetMode( resize_mode mode )
   {
      m_mode = mode;
   }

   /*!
    * Returns the current absolute resizing mode. The absolute resizing mode is
    * only applied when Mode() is not equal to ResizeMode::RelativeDimensions.
    */
   abs_resize_mode AbsMode() const
   {
      return m_absMode;
   }

   /*!
    * Sets the absolute resizing mode. The absolute resizing mode is only
    * applied when Mode() is not equal to ResizeMode::RelativeDimensions.
    */
   void SetAbsMode( abs_resize_mode absMode )
   {
      m_absMode = absMode;
   }

   /*!
    */
   void GetNewSizes( int& width, int& height ) const override;

protected:

   double          m_xSize = 1.0;
   double          m_ySize = 1.0;
   resize_mode     m_mode = ResizeMode::Default;
   abs_resize_mode m_absMode = AbsoluteResizeMode::Default;

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Resample_h

// ----------------------------------------------------------------------------
// EOF pcl/Resample.h - Released 2018-11-23T16:14:19Z
