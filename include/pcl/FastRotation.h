//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/FastRotation.h - Released 2019-01-21T12:06:07Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PCL_FastRotation_h
#define __PCL_FastRotation_h

/// \file pcl/FastRotation.h

#include <pcl/Defs.h>
#include <pcl/Diagnostics.h>

#include <pcl/GeometricTransformation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \defgroup fast_rotations Non-interpolating Rotations and Specular&nbsp;\
 * Transformations.
 *
 * <em>Fast rotations</em> are non-interpolating geometric transformations:
 * they carry out image rotation and specular projection operations exclusively
 * by copying and swapping pixels.
 *
 * Since no pixel interpolation is performed, there is absolutely no data
 * degradation after an arbitrary number of consecutive fast rotations.
 */

// ----------------------------------------------------------------------------

/*!
 * \class Rotate180
 * \brief Rotates images by 180 degrees
 *
 * \ingroup fast_rotations
 */
class PCL_CLASS Rotate180 : public GeometricTransformation
{
public:

   /*!
    */
   void GetNewSizes( int& w, int& h ) const override
   {
      // No change
   }

protected:

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::ComplexImage& ) const override;
   void Apply( pcl::DComplexImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

/*!
 * \class Rotate90CW
 * \brief Rotates images by 90 degrees (clockwise)
 *
 * \ingroup fast_rotations
 */
class PCL_CLASS Rotate90CW : public GeometricTransformation
{
public:

   /*!
    */
   void GetNewSizes( int& w, int& h ) const override
   {
      pcl::Swap( w, h ); // Permute
   }

protected:

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::ComplexImage& ) const override;
   void Apply( pcl::DComplexImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

/*!
 * \class Rotate90CCW
 * \brief Rotates images by -90 degrees (counter-clockwise)
 *
 * \ingroup fast_rotations
 */
class PCL_CLASS Rotate90CCW : public GeometricTransformation
{
public:

   /*!
    */
   void GetNewSizes( int& w, int& h ) const override
   {
      pcl::Swap( w, h ); // Permute
   }

protected:

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::ComplexImage& ) const override;
   void Apply( pcl::DComplexImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

/*!
 * \class HorizontalMirror
 * \brief Mirrors images horizontally
 *
 * \ingroup fast_rotations
 */
class PCL_CLASS HorizontalMirror : public GeometricTransformation
{
public:

   /*!
    */
   void GetNewSizes( int& w, int& h ) const override
   {
      // No change
   }

protected:

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::ComplexImage& ) const override;
   void Apply( pcl::DComplexImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

/*!
 * \class VerticalMirror
 * \brief Mirrors images vertically
 *
 * \ingroup fast_rotations
 */
class PCL_CLASS VerticalMirror : public GeometricTransformation
{
public:

   /*!
    */
   void GetNewSizes( int& w, int& h ) const override
   {
      // No change
   }

protected:

   // Inherited from ImageTransformation.
   void Apply( pcl::Image& ) const override;
   void Apply( pcl::DImage& ) const override;
   void Apply( pcl::ComplexImage& ) const override;
   void Apply( pcl::DComplexImage& ) const override;
   void Apply( pcl::UInt8Image& ) const override;
   void Apply( pcl::UInt16Image& ) const override;
   void Apply( pcl::UInt32Image& ) const override;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_FastRotation_h

// ----------------------------------------------------------------------------
// EOF pcl/FastRotation.h - Released 2019-01-21T12:06:07Z
