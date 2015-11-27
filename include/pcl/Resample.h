//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// pcl/Resample.h - Released 2015/11/26 15:59:39 UTC
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

#ifndef __PCL_Resample_h
#define __PCL_Resample_h

/// \file pcl/Resample.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_GeometricTransformation_h
#include <pcl/GeometricTransformation.h>
#endif

#ifndef __PCL_ImageResolution_h
#include <pcl/ImageResolution.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
   \namespace ResizeMode
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
   \namespace AbsoluteResizeMode
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
                           public ImageResolution
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
      InterpolatingGeometricTransformation( p ), ImageResolution(),
      m_xSize( scale ), m_ySize( scale ),
      m_mode( ResizeMode::Default ), m_absMode( AbsoluteResizeMode::Default ),
      m_parallel( true ), m_maxProcessors( PCL_MAX_PROCESSORS )
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
      InterpolatingGeometricTransformation( p ), ImageResolution(),
      m_xSize( scaleX ), m_ySize( scaleY ),
      m_mode( ResizeMode::Default ), m_absMode( AbsoluteResizeMode::Default ),
      m_parallel( true ), m_maxProcessors( PCL_MAX_PROCESSORS )
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
   virtual void GetNewSizes( int& width, int& height ) const;

   /*!
    * Returns true iff this object is allowed to use multiple parallel execution
    * threads (when multiple threads are permitted and available).
    */
   bool IsParallelProcessingEnabled() const
   {
      return m_parallel;
   }

   /*!
    * Enables parallel processing for this instance of %Resample.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * \param maxProcessors    The maximum number of processors allowed for this
    *                instance of %Resample. If \a enable is false this
    *                parameter is ignored. A value <= 0 is ignored. The default
    *                value is zero.
    */
   void EnableParallelProcessing( bool enable = true, int maxProcessors = 0 )
   {
      m_parallel = enable;
      if ( enable && maxProcessors > 0 )
         SetMaxProcessors( maxProcessors );
   }

   /*!
    * Disables parallel processing for this instance of %Resample.
    *
    * This is a convenience function, equivalent to:
    * EnableParallelProcessing( !disable )
    */
   void DisableParallelProcessing( bool disable = true )
   {
      EnableParallelProcessing( !disable );
   }

   /*!
    * Returns the maximum number of processors allowed for this instance of
    * %Resample.
    *
    * Irrespective of the value returned by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   int MaxProcessors() const
   {
      return m_maxProcessors;
   }

   /*!
    * Sets the maximum number of processors allowed for this instance of
    * %Resample.
    *
    * In the current version of PCL, a module can use a maximum of 1023
    * processors. The term \e processor actually refers to the number of
    * threads a module can execute concurrently.
    *
    * Irrespective of the value specified by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   void SetMaxProcessors( int maxProcessors )
   {
      m_maxProcessors = unsigned( Range( maxProcessors, 1, PCL_MAX_PROCESSORS ) );
   }

protected:

   double          m_xSize;
   double          m_ySize;
   resize_mode     m_mode;
   abs_resize_mode m_absMode;
   bool            m_parallel      : 1;
   unsigned        m_maxProcessors : PCL_MAX_PROCESSORS_BITCOUNT;

   // Inherited from ImageTransformation.
   virtual void Apply( pcl::Image& ) const;
   virtual void Apply( pcl::DImage& ) const;
   virtual void Apply( pcl::UInt8Image& ) const;
   virtual void Apply( pcl::UInt16Image& ) const;
   virtual void Apply( pcl::UInt32Image& ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Resample_h

// ----------------------------------------------------------------------------
// EOF pcl/Resample.h - Released 2015/11/26 15:59:39 UTC
