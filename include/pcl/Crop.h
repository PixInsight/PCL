//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/Crop.h - Released 2017-06-21T11:36:33Z
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

#ifndef __PCL_Crop_h
#define __PCL_Crop_h

/// \file pcl/Crop.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/GeometricTransformation.h>
#include <pcl/ImageResolution.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
   \namespace pcl::CropMode
   \brief Image cropping modes for the Crop geometric transformation

   <table border="1" cellpadding="4" cellspacing="0">
   <tr><td>CropMode::RelativeMargins</td>     <td>Cropping margins are relative to current image dimensions</td></tr>
   <tr><td>CropMode::AbsolutePixels</td>      <td>Absolute cropping margins in pixels</td></tr>
   <tr><td>CropMode::AbsoluteCentimeters</td> <td>Absolute cropping margins in centimeters</td></tr>
   <tr><td>CropMode::AbsoluteInches</td>      <td>Absolute cropping margins in inches</td></tr>
   </table>
*/
namespace CropMode
{
   enum value_type
   {
      RelativeMargins,     // Cropping margins are relative to current image dimensions
      AbsolutePixels,      // Absolute cropping margins in pixels
      AbsoluteCentimeters, // Absolute cropping margins in centimeters
      AbsoluteInches       // Absolute cropping margins in inches
   };
}

// ----------------------------------------------------------------------------

/*!
 * \class Crop
 * \brief %Image cropping/expansion algorithm
 *
 * %Crop is a noninterpolating geometric transformation class that allows
 * cropping an image or expanding it by specifying separate cropping margins
 * for each side. When a cropping margin is negative the image is cropped, and
 * when it is positive, the image is expanded.
 */
class PCL_CLASS Crop : public GeometricTransformation,
                       public ImageResolution
{
public:

   /*!
    * Represents a cropping mode.
    */
   typedef CropMode::value_type  crop_mode;

   /*!
    * Constructs a %Crop object with the specified cropping margins for
    * the \a left, \a top, \a right and \a bottom sides.
    */
   Crop( double left = 0, double top = 0, double right = 0, double bottom = 0 ) :
      GeometricTransformation(), ImageResolution(),
      m_margins( left, top, right, bottom ), m_mode( CropMode::RelativeMargins ), m_fillValues()
   {
   }

   /*!
    * Constructs a %Crop object whose cropping margins are defined by the
    * components of a rectangle \a r.
    */
   template <typename T>
   Crop( const GenericRectangle<T>& r ) :
      GeometricTransformation(), ImageResolution(),
      m_margins( r ), m_mode( CropMode::RelativeMargins ), m_fillValues()
   {
   }

   /*!
    * Copy constructor.
    */
   Crop( const Crop& x ) :
      GeometricTransformation( x ), ImageResolution( x ),
      m_margins( x.m_margins ), m_mode( x.m_mode ), m_fillValues( x.m_fillValues )
   {
   }

   /*!
    * Copy assignment operator. Returns a reference to this object.
    */
   Crop& operator =( const Crop& x )
   {
      (void)GeometricTransformation::operator =( x );
      (void)ImageResolution::operator =( x );
      m_margins = x.m_margins;
      m_mode = x.m_mode;
      m_fillValues = x.m_fillValues;
      return *this;
   }

   /*!
    * Returns a rectangle whose components correspond to the current cropping
    * margins of this %Crop object.
    */
   DRect Margins() const
   {
      return m_margins;
   }

   /*!
    * Sets the cropping margins equal to the components of a specified
    * rectangle \a r.
    */
   template <typename T>
   void SetMargins( const GenericRectangle<T>& r )
   {
      m_margins = r;
   }

   /*!
    * Sets the specified \a left, \a top, \a right and \a bottom cropping
    * margins.
    */
   void SetMargins( int left, int top, int right, int bottom )
   {
      m_margins = Rect( left, top, right, bottom );
   }

   /*!
    * Returns the current cropping mode of this %Crop object.
    */
   crop_mode Mode() const
   {
      return m_mode;
   }

   /*!
    * Returns true iff this %Crop object interprets its cropping margins
    * relative to current image dimensions. Returns false if cropping margins
    * are interpreted as absolute increments.
    */
   bool IsRelative() const
   {
      return m_mode == CropMode::RelativeMargins;
   }

   /*!
    * Returns true iff this %Crop object interprets its cropping margins as
    * absolute increments. Returns false if cropping margins are interpreted
    * relative to current image dimensions.
    */
   bool IsAbsolute() const
   {
      return !IsRelative();
   }

   /*!
    * Sets the cropping \a mode for this %Crop object.
    */
   void SetMode( crop_mode mode )
   {
      m_mode = mode;
   }

   /*!
    * Returns the current vector of per-channel filling values for uncovered
    * image regions.
    *
    * See the documentation for SetFillValues() for more information.
    */
   const DVector& FillValues() const
   {
      return m_fillValues;
   }

   /*!
    * Sets a vector of per-channel filling values for uncovered image regions.
    *
    * Uncovered regions result when a %Crop instance extends an image due to
    * positive cropping margins.
    *
    * By default, there are no filling values defined (and hence the returned
    * vector is empty by default). When the %Crop instance is executed and a
    * filling value is not defined for a channel of the target image, uncovered
    * regions are filled with the minimum sample value in the native range of
    * the image (usually zero).
    */
   void SetFillValues( const DVector& fillValues )
   {
      m_fillValues = fillValues;
   }

   /*!
    */
   virtual void GetNewSizes( int& width, int& height ) const;

protected:

   DRect     m_margins;
   crop_mode m_mode;
   DVector   m_fillValues;

   // Inherited from ImageTransformation.
   virtual void Apply( pcl::Image& ) const;
   virtual void Apply( pcl::DImage& ) const;
   virtual void Apply( pcl::ComplexImage& ) const;
   virtual void Apply( pcl::DComplexImage& ) const;
   virtual void Apply( pcl::UInt8Image& ) const;
   virtual void Apply( pcl::UInt16Image& ) const;
   virtual void Apply( pcl::UInt32Image& ) const;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Crop_h

// ----------------------------------------------------------------------------
// EOF pcl/Crop.h - Released 2017-06-21T11:36:33Z
