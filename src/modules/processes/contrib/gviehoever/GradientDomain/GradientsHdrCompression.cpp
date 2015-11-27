//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0155
// ----------------------------------------------------------------------------
// GradientsHdrCompression.cpp - Released 2015/11/26 16:00:13 UTC
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

#include "GradientsHdrCompression.h"

namespace pcl {

GradientsHdrCompression::GradientsHdrCompression()
  :GradientsBase()
{
}

GradientsHdrCompression::~GradientsHdrCompression()
{
}


void
GradientsHdrCompression::hdrCompressionSetImage(imageType_t const & rImage_p)
{
  AssertImage(rImage_p);
  createDxImage(rImage_p, m_imageDx);
  createDyImage(rImage_p, m_imageDy);
  minMaxValImage(rImage_p,m_dMinVal, m_dMaxVal);
}


//static
void
GradientsHdrCompression::minMaxValImage(imageType_t const &rImage_p, realType_t &rdMin_p,realType_t &rdMax_p)
{
  AssertImage(rImage_p);
  rImage_p.ResetSelections();
  rImage_p.GetExtremePixelValues(rdMin_p, rdMax_p);
}

//static
void
GradientsHdrCompression::clipImage(imageType_t &rImage_p, realType_t minRange_p, realType_t maxRange_p)
{
  AssertImage(rImage_p);
  Assert(maxRange_p>=minRange_p);
#if 1
  rImage_p.ResetSelections();
  rImage_p.Truncate(minRange_p, maxRange_p);
#else
  // slow version for tests
  for ( realType_t* f = rImage_p.PixelData(), * f1 = f + rImage_p.NumberOfPixels(); f != f1; ++f ){
    if ( *f < minRange_p ) {
      *f = minRange_p;
      continue;
    }
    if ( *f > maxRange_p ) {
      *f = maxRange_p;
    }
  }
#endif
}


//static
void
GradientsHdrCompression::zeroRangeImage(imageType_t &rImage_p, realType_t minRange_p, realType_t maxRange_p)
{
  AssertImage(rImage_p);
  Assert(maxRange_p>=minRange_p);
  // code from template <class P> void GenericImage<P>::Truncate( const sample* r0, const sample* r1 )

  for ( realType_t* f = rImage_p.PixelData(), * f1 = f + rImage_p.NumberOfPixels(); f != f1; ++f ){
    if ( *f > minRange_p && *f < maxRange_p) {
      *f = 0.0;
    }
  }
}

//static
void
GradientsHdrCompression::absPowImage(imageType_t &rImage_p, realType_t exponent_p)
{
  AssertImage(rImage_p);
  Assert(exponent_p>0.0);

  for ( realType_t* f = rImage_p.PixelData(), * f1 = f + rImage_p.NumberOfPixels(); f != f1; ++f ){
    if ( *f > 0) {
      *f = std::pow(*f,exponent_p);
    }else{
      *f = -std::pow(- *f,exponent_p);
    }
  }
}



void
GradientsHdrCompression::hdrCompression(realType_t maxGradient_p, realType_t minGradient_p, realType_t expGradient_p,
			   bool bRescale01_p, imageType_t &rResultImage_p) const
{
  TimeMessage startHdrCompression("Gradient Domain HDR Compression");
  imageType_t li;
  {
    TimeMessage startClipValue("Determine clip values for gradients");
    imageType_t dx(m_imageDx);
    imageType_t dy(m_imageDy);
    // now clip values
    realType_t minGradientX,maxGradientX;
    minMaxValImage(dx,minGradientX,maxGradientX);
    realType_t minGradientY,maxGradientY;
    minMaxValImage(dy,minGradientY,maxGradientY);
    realType_t minValue=Min(minGradientX, minGradientY);
    realType_t maxValue=Max(maxGradientX, maxGradientY);
    double rangeValue=Max(-minValue, maxValue);
    realType_t const clipRange=rangeValue*maxGradient_p;
    realType_t const zeroRange=rangeValue*minGradient_p;
    startClipValue.Stop();

    TimeMessage startClip("Clipping Gradients");
    clipImage(dx,-clipRange,clipRange);
    zeroRangeImage(dx,-zeroRange,zeroRange);
    clipImage(dy,-clipRange,clipRange);
    zeroRangeImage(dy,-zeroRange,zeroRange);
    startClip.Stop();

    if(expGradient_p!=1.0){
      TimeMessage start("Pow() transformation of gradients");
      absPowImage(dx,expGradient_p);
      absPowImage(dy,expGradient_p);
    }

    TimeMessage startLaplace("Computing 2nd derivative");
    createLaplaceVonNeumannImage(dx,dy,li);
  }
  TimeMessage startSolve("Solving image");
  solveImage(li,rResultImage_p);
  startSolve.Stop();

  TimeMessage startRescale("Rescaling image");
  if(bRescale01_p){
    rResultImage_p.Rescale(0.0,1.0);
  } else {
    rResultImage_p.Rescale(m_dMinVal,m_dMaxVal);
  }
}

}

// ----------------------------------------------------------------------------
// EOF GradientsHdrCompression.cpp - Released 2015/11/26 16:00:13 UTC
