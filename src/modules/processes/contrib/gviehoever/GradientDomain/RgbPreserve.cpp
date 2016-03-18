//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0165
// ----------------------------------------------------------------------------
// RgbPreserve.cpp - Released 2016/02/21 20:22:43 UTC
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

#include "RgbPreserve.h"
#include "Assert.h"
//#include <cassert>

namespace pcl {

RgbPreserve::RgbPreserve()
{
}

RgbPreserve::~RgbPreserve()
{
}


void
RgbPreserve::rgbTransformation(imageType_t const &rOrigLuminance_p,
			       imageType_t const &rNewLuminance_p,
			       imageType_t &rImage_p) const
{
#ifdef DEBUG
  int width=rImage_p.Width();
  int height=rImage_p.Height();
  Assert(width==rOrigLuminance_p.Width());
  Assert(height==rOrigLuminance_p.Height());
  Assert(width==rNewLuminance_p.Width());
  Assert(height==rNewLuminance_p.Height());
  Assert(rOrigLuminance_p.NumberOfNominalChannels()==1);
  Assert(rNewLuminance_p.NumberOfNominalChannels()==1);
#endif
  std::size_t channels=rImage_p.NumberOfNominalChannels();


  // this is the multiplication factor
  imageType_t propImage(rNewLuminance_p);
  propImage.ResetSelections();
  propImage.Div(rOrigLuminance_p);

  rImage_p.ResetSelections();
  // note: we need to do this on a float image. Otherwise, we
  // most likely will suffer from overflows
  for (std::size_t chan=0;chan<channels;chan++){
    rImage_p.SelectChannel(chan);
    rImage_p.Mul(propImage);
  }
  realType_t dMin,dMax;
  rNewLuminance_p.GetExtremePixelValues(dMin, dMax);
  rImage_p.ResetSelections();
  rImage_p.Rescale(dMin,dMax);
}

};

// ----------------------------------------------------------------------------
// EOF RgbPreserve.cpp - Released 2016/02/21 20:22:43 UTC
