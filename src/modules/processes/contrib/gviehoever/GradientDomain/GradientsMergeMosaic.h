//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0230
// ----------------------------------------------------------------------------
// GradientsMergeMosaic.h - Released 2018-11-23T18:45:58Z
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

#ifndef __GradientsMergeMosaic_h
#define __GradientsMergeMosaic_h

#include <pcl/Array.h>
#include "GradientsBase.h"

#include "GradientsMergeMosaicParameters.h"


namespace pcl {

/// implements merging of multiple images using gradient domain methods
class GradientsMergeMosaic: public GradientsBase
{
 public:

  /// list of file path to images to combine
  typedef Array<String> imageListType_t;


  /// constructor
  GradientsMergeMosaic();
  /// destructor: releases internal data strucutures
  ~GradientsMergeMosaic();

  /// compute merged mosaic for given images list
  ///
  /// Does a number of checks on those images. May throw Error.
  /// @param rImageList_p list of file names of images. All images need to have
  ///        the same number of channels and width and height
  /// @param shrinkCount radius of an erosion mask moving the non-background pixels
  ///        of each image inwards
  /// @param feather radius of a convolution mask smoothing the non-background pixels
  ///        of each image at the border. Changes relative weight of each image in this region
  /// @param dBlackPoint_p max. value of pixels considered transparent
  /// @param eType_p type of operation. With Overlay, the last image wins. With Average, the average of all
  ///        non-transparent pixels is used
  /// @param rResultImage_p the merged image FIXME color balance?
  /// @param rSumMask_p is visualization of the image regions, with 1 for image n, 2 for image n-1,
  ///        4 for image n-2 etc.
  ///
  /// @note there is nothing to precompute, so no two stage operation
  static void mergeMosaic(imageListType_t const & rImageList_p,
			  realType_t dBlackPoint_p,
			  pcl_enum eType_p,
			  int32 shrinkCount,
			  int32 featherRadius,
			  imageType_t &rResultImage_p,
			  sumMaskImageType_t &rSumMaskImage_p);

 private: // member functions

  /// performs convolution with same pattern as used in erodeMask, using a radius of featherRadius
  static void convolveMask(weightImageType_t &rMask_p, int32 featherRadius_p);

  /// do erosion via convolution, which currently is faster than erodeMask(). Works only for binary masks with values of 1.0 and 0.0
  // FIXME replace when separable Erosion becomes available
  static void erodeMaskConvolve(weightImageType_t & rMask_p,int32 shrinkCount_p);

  /// smooth border of mask with convolution of radius featherRadius
  static void featherMask(weightImageType_t &rMask_p, int32 featherRadius_p);

  /// defines border by setting pixels rMask_p to -1.0
  ///
  /// @param rFullMask_p mask where all contributing pixels of an image are >0.0
  /// @param rShrinkedMask_p mask that is smaller than full mask, usually due to erosion or feathering.
  ///        As a result of this operation, pixels that are >0.0 in rFullMask_p but ==0.0 in rShrinkedMask_p
  ///        are set to -1.0
  static void addBorder(weightImageType_t const & rFullMask_p, weightImageType_t &rShrinkedMask_p);

  /// add rBitMask_p to rSumMask_p
  static void addToMask(weightImageType_t const & rMask_p,
			pcl_enum eType_p,
			sumMaskImageType_t &rSumMaskImage_p);

  /// add rImage_p to rSumImageDx_p,Dy_p. In mode average, also adjust rCountImageDx_p,Dy_p
  ///
  /// @param rImage_p image to be added
  /// @param eTyped_p type of operation, overlay or average
  /// @param rMask_p mask for rImage_p as discussed in weightImageType_t
  /// @param rSumImageDx_p resulting combined dx gradient field
  /// @param rSumImageDy_p resulting combined dy gradient field
  /// @param rCountImageDx_p number of pixels/sum of weights contributing to rSumImageDx_p. For pixels of
  ///        border or background, this is always 0
  /// @param rCountImageDy_p see rCountImageDx_p
  static void addToImage(imageType_t const & rImage_p,pcl_enum eType_p,
			 weightImageType_t const & rMask_p,
			 imageType_t &rSumImageDx_p,imageType_t &rSumImageDy_p,
			 weightImageType_t &rCountImageDx_p, weightImageType_t &rCountImageDy_p);

  /// compute average of rImage_p if necessary
  static void averageImage(pcl_enum eType_p,
			   weightImageType_t const &rCountImage_p,
			   imageType_t &rImage_p);

  /// add rImage_p to set of images stored in rSumImageDxDy_p, rSumMaskImage_p_p and rCountImageDxDy_p
  ///
  /// @param rImage_p input image that is added to process
  /// @param dBlackPoint_p limit intensity of background for all channels
  /// @param eType_p kind of merge. Overlay (pixels of last image win) or Average (all relevant pixels are averaged).
  /// @param shrinkCount_p number of pixels by which image region is reduced. Useful to get
  ///        rid of borders where image slowly merges into background, e.g. due to aliasing
  /// @param featherRadius_p number of pixels by which image region is feathered. Useful to
  ///        get rid of artefacts with stars at borders
  /// @param rSumImageDx_p result, combined image gradients in dx direction
  /// @param rSumImageDy_p result, combined image gradients in dy direction
  /// @param rSumMaskImage_p bitmask indicating which images contribute to which region
  ///        later used for patching up gradients, and output to GUI
  /// @param rCountImageDx_p result, number of images for each pixel of rSumImageDx_p,
  ///        only used for eType_p=average
  /// @param rCountImageDy_p result, number of images for each pixel of rSumImageDy_p,
  ///        only used for eType_p=average
  static void mergeMosaicProcessImage(imageType_t const & rImage_p, realType_t dBlackPoint_p,
				      pcl_enum eType_p,
				      int32 shrinkCount_p,
				      int32 featherRadius_p,
				      imageType_t &rSumImageDx_p,
				      imageType_t &rSumImageDy_p,
				      sumMaskImageType_t &rSumMaskImage_p,
				      weightImageType_t &rCountImageDx_p,
				      weightImageType_t &rCountImageDy_p);
};

};
#endif

// ----------------------------------------------------------------------------
// EOF GradientsMergeMosaic.h - Released 2018-11-23T18:45:58Z
