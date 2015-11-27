//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0155
// ----------------------------------------------------------------------------
// RgbPreserve.h - Released 2015/11/26 16:00:13 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2015. Licensed under LGPL 2.1
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
// ----------------------------------------------------------------------------

#ifndef __rgbPreserve_h
#define __rgbPreserve_h

//#include <cassert>
#include <pcl/Image.h>

namespace pcl {

/// implements base routines for changing image intensities while keeping RGB
///
/// Will transform the intensities of an RGB image such that the
/// relative amounts of R:G:B are preserved.
class RgbPreserve
{
 public:

  /// numeric type used internally
  typedef double realType_t;

  /// holds the matrix of image data in double format
  typedef pcl::DImage imageType_t;

  /// constructor
  RgbPreserve();
  /// destructor: releases internal data strucutures
  ~RgbPreserve();

  /// Transform image
  ///
  /// If the image set in setImage() is a single channel image,
  /// this will just assign rNewLuminance to rResultImage_p.
  ///
  /// @param rOrigLuminance_p is the original brightness
  /// @param rBewLuminance_p is the new brightness
  /// @param rImage_p that contains the original and the result.
  ///
  /// @note the term "Luminance" may not be exact in this context
  void rgbTransformation(imageType_t const &rOrigLuminance_p,
			 imageType_t const &rNewLuminance_p,
			 imageType_t &rImage_p) const;
};

};
#endif

// ----------------------------------------------------------------------------
// EOF RgbPreserve.h - Released 2015/11/26 16:00:13 UTC
