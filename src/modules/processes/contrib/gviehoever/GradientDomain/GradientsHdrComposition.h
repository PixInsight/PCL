//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0240
// ----------------------------------------------------------------------------
// GradientsHdrComposition.h - Released 2019-01-21T12:06:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2018. Licensed under LGPL 2.1
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L.
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

#ifndef __GradientsHdrComposition_h
#define __GradientsHdrComposition_h

#include <pcl/Array.h>
#include "GradientsBase.h"

#include "GradientsHdrCompositionParameters.h"


namespace pcl {

/// implements merging of multiple images using gradient domain methods
class GradientsHdrComposition: public GradientsBase
{
 public:

  /// holds "bitMask" for mergeMosaic. Its purpose is visualization of the contributing image regions.
  ///
  /// Value 0 is background, 1 is first image, 2 is second image, 4 image 3rd image etc..
  /// Combinations are the sum of these powers of 2. In the border region, images may be feathered
  /// and therefore get weights<1. In these regions, the sum is not unique for each combination.
  typedef DImage sumMaskImageType_t;

  /// list of file path to images to combine
  typedef Array<String> imageListType_t;


  /// constructor
  GradientsHdrComposition();
  /// destructor: releases internal data strucutures
  ~GradientsHdrComposition();

  /// compute merged mosaic for given images list
  ///
  /// Does a number of checks on those images. May throw Error.
  /// @param rImageList_p list of file names of images. All images need to have
  ///        the same number of channels and width and height
  /// @param bKeepLog_p if true, keep result in log space, otherwise transform into linear space
  /// @param dBias_p true black point of image (note: 0 is always transparent and my be different from bias)
  /// @param rResultImage_p the merged image
  /// @param rDxImage_p identifies the image that contributes to a pixel dx gradient. Values
  //         start with 1 for the first image. Same size
  ///        as rResultImage_p. Relates to the gradient between pixel (x,y) and pixel (x+1,y)
  ///        (i.e. last column is always 0).
  /// @param rDyImage_p dito for dy. Relates to gradient between pixel (x,y) and pixel (x,y+1)
  ///
  /// @note there is nothing to precompute, so no two stage operation
  static void hdrComposition(imageListType_t const & rImageList_p,
			     bool bKeepLog_p,
			     realType_t dBias_p,
			     imageType_t &rResultImage_p,
			     numImageType_t &rDxImage_p,
			     numImageType_t &rDyImage_p );


 private:
  /// add rImage_p to set of images stored in rSumImageDx/Dy_p, rSumMask_p
  ///
  /// @param rImage_p input image that is added to process
  /// @param imageNum_p number of current image, unique, >0
  /// @param dBias_p true black point of image (note: 0 is always transparent and my be different from bias)
  /// @param dBlackPoint_p limit intensity of background for all channels
  /// @param shrinkCount_p number of pixels by which image region is reduced. Useful to get
  ///        rid of borders where image slowly merges into background, e.g. due to aliasing
  /// @param rSumImageDx_p result, combined image gradients in dx direction
  /// @param rSumImageDy_p result, combined image gradients in dy direction
  /// @param rDxImage_p identifies the image that contributes to a pixel dx gradient. Values
  //         start with 1 for the first image. Same size
  ///        as rResultImage_p. Relates to the gradient between pixel (x,y) and pixel (x+1,y)
  ///        (i.e. last column is always 0).
  /// @param rDyImage_p dito for dy. Relates to gradient between pixel (x,y) and pixel (x,y+1)
  static void hdrCompositionProcessImage(
					 imageType_t const & rImage_p,
					 int imageNum_p,
					 realType_t dBias_p,
					 realType_t dBlackPoint_p,
					 int32 shrinkCount_p,
					 imageType_t &rSumImageDx_p,
					 imageType_t &rSumImageDy_p,
					 numImageType_t &rDxImage_p,
					 numImageType_t &rDyImage_p);

  /// add rImage_p to rSumImageDx_p,Dy_p. Update mask to indicate image number
  ///
  /// @param rImage_p log image to be added
  /// @param imageNum_p number of current image, unique, >0
  /// @param rMask_p mask for rImage_p as discussed in weightImageType_t
  /// @param rSumImageDx_p resulting combined dx gradient field
  /// @param rSumImageDy_p resulting combined dy gradient field
  /// @param rDxImage_p identifies the image that contributes to a pixel dx gradient. Values
  //         start with 1 for the first image. Same size
  ///        as rResultImage_p. Relates to the gradient between pixel (x,y) and pixel (x+1,y)
  ///        (i.e. last column is always 0).
  /// @param rDyImage_p dito for dy. Relates to gradient between pixel (x,y) and pixel (x,y+1)
  static void addToImage(imageType_t const & rImage_p,
			 int imageNum_p,
			 weightImageType_t const & rMask_p,
			 imageType_t &rSumImageDx_p,imageType_t &rSumImageDy_p,
			 numImageType_t &rDxImage_p,
			 numImageType_t &rDyImage_p);

  // create image by applying log(val) (i.e. natural log) to current image.
  //
  // pixels with values <=0 are just set to -REALTYPE_T_MAX.
  static void logImage(imageType_t const & rCurrentImage_p,
		       imageType_t & rResultImage_p);

  // create image by applying exp(val) to current image.
  //
  // no error checking
  static void expImage(imageType_t const & rCurrentImage_p,
		       imageType_t & rResultImage_p);

  /// clip large gradient value to 0
  static void clipGradients(imageType_t &rImage_p);

};

};
#endif

// ----------------------------------------------------------------------------
// EOF GradientsHdrComposition.h - Released 2019-01-21T12:06:42Z
