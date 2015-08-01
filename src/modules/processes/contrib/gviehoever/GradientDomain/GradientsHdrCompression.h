//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0117
// ----------------------------------------------------------------------------
// GradientsHdrCompression.h - Released 2015/07/31 11:49:49 UTC
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

#ifndef __GradientsHdrCompression_h
#define __GradientsHdrCompression_h

#include "GradientsBase.h"

namespace pcl {
/// implements hdr compression process
///
/// Use:
/// - call hdrCompressionSetImage() to set the image,
/// - call hdrCompression() to do the compression with different parameters
class GradientsHdrCompression: public GradientsBase
{
 public:
  /// constructor
  GradientsHdrCompression();
  /// destructor: releases internal data strucutures
  ~GradientsHdrCompression();

  /// set current image
  ///
  /// it will compute
  /// dx,dy images. Needs not be repeated until image is changes.
  /// This class will not be the owner of the image!
  void hdrCompressionSetImage(imageType_t const & rImage_p);

  /// compress dynamic range of image
  ///
  /// Called after image was set using setImage().
  /// All manipulations on gradients happen as sign(gradient)*operation(abs(gradient))!
  ///
  /// @param maxGradient_ range reduction for strong gradients, 0<=maxGradient_p<=1.0. if maxGradient is the
  ///        maximum absolute gradient value and if  abs(gradient)>maxGradiant*maxGradient_p,
  ///        then set gradient=maxGradiant*maxGradient_p. Value of 1 is neutral, 0 is strongest.
  /// @param minGradient_p clip range reduction for weak gradients. 0<=minGradient_p<maxGradient.
  ///        if maxGradient is the maximum absolute gradient value, and if abs(radient)<maxGradient*minGradient_p,
  ///        then set gradient to 0. This may remove noise, but generally also leads to postierization.
  /// @param expGradient_p gradients are scaled as pow(gradient,expGradient). Usually 0<expGradient<=1;
  /// @param bRescale01_p if true, rescale result to [0,1] range. If false, rescale to range of input image
  /// @param rResultImage_p result image
  void hdrCompression(realType_t maxGradient_p, realType_t minGradient_p, realType_t expGradient_p,
		      bool bRescale01_p, imageType_t &rResultImage_p) const;


 private: // member functions

  /// return minimum and maximum value found in image
  static void minMaxValImage(imageType_t const &rImage_p, realType_t &rdMin_P, realType_t &rdMax_p);

  /// in rImage_p, change any value smaller than minRange_p to minRange_p, and any value larger than maxRange_p to maxRange_p.
  static void clipImage(imageType_t &rImage_p, realType_t minRange_p, realType_t maxRange_p);

  /// in rImage_p, change any value smaller in range [minRange_p, maxRange_p] to 0.0
  static void zeroRangeImage(imageType_t &rImage_p, realType_t minRange_p, realType_t maxRange_p);

  /// in rImage_p, raise value to sign(value)*pow(abs(value),exponent_p)
  static void absPowImage(imageType_t &rImage_p, realType_t exponent_p);

 private: // member variables
  /// current dx gradient image
  imageType_t m_imageDx;
  /// current dy gradient image
  imageType_t m_imageDy;
  /// minimum value of image
  double m_dMinVal;
  /// maximum value of image
  double m_dMaxVal;
};

};
#endif

// ----------------------------------------------------------------------------
// EOF GradientsHdrCompression.h - Released 2015/07/31 11:49:49 UTC
