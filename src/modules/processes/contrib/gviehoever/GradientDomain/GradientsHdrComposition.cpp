// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard GradientDomain Process Module Version 00.06.04.0098
// ****************************************************************************
// GradientsHdrComposition.cpp - Released 2014/11/14 17:19:24 UTC
// ****************************************************************************
// This file is part of the standard GradientDomain PixInsight module.
//
// Copyright (c) Georg Viehoever, 2011-2014. Licensed under LGPL 2.1
// Copyright (c) 2003-2014 Pleiades Astrophoto S.L.
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
// ****************************************************************************

#include <cmath>

#include <pcl/MorphologicalTransformation.h>
#include <pcl/SeparableConvolution.h>
#include <pcl/LinearFilter.h>

#include "GradientsHdrComposition.h"

namespace pcl {

  /// minimum log value to use. Other gradient values are coerced to 0 to avoid numerical problems
  #define MINLOGVALUE (-20.0)

  GradientsHdrComposition::GradientsHdrComposition()
    :GradientsBase()
  {
  }

  GradientsHdrComposition::~GradientsHdrComposition()
  {
  }


  //static
  void GradientsHdrComposition::clipGradients(imageType_t &rImage_p)
  {
    for ( realType_t* f = rImage_p.PixelData(), * f1 = f + rImage_p.NumberOfPixels(); f != f1; ++f )
      {
	if ( std::abs(*f) > -MINLOGVALUE) {
	  *f = 0.0;
	}
      }
  }

  //static
  void GradientsHdrComposition::logImage(imageType_t const & rCurrentImage_p,
					 imageType_t & rResultImage_p)
  {
    int const nCols=rCurrentImage_p.Width();
    int const nRows=rCurrentImage_p.Height();
    int const nChannels=rCurrentImage_p.NumberOfChannels();
    imageType_t::color_space  colorSpace=rCurrentImage_p.ColorSpace();

    if (&rCurrentImage_p != &rResultImage_p){
      rResultImage_p.AllocateData(nCols,nRows, nChannels, colorSpace);
    }
    for(int channel=0;channel<nChannels;++channel){
      for(int row=0;row<nRows;++row){
	for(int col=0;col<nCols;++col){
	  realType_t val=rCurrentImage_p.Pixel(col,row,channel);
	  if(val<=0.0) {
	    rResultImage_p.Pixel(col,row,channel)= -1000; //this is just some value to identify extremes
	  } else {
	    val=std::log(val);
	    rResultImage_p.Pixel(col,row,channel)=val;
	  }
	}
      }
    }
  }

  //static
  void GradientsHdrComposition::expImage(imageType_t const & rCurrentImage_p,
					 imageType_t & rResultImage_p)
  {
    int const nCols=rCurrentImage_p.Width();
    int const nRows=rCurrentImage_p.Height();
    int const nChannels=rCurrentImage_p.NumberOfChannels();
    imageType_t::color_space  colorSpace=rCurrentImage_p.ColorSpace();

    if (&rCurrentImage_p != &rResultImage_p){
      rResultImage_p.AllocateData(nCols,nRows, nChannels, colorSpace);
    }
    for(int channel=0;channel<nChannels;++channel){
      for(int row=0;row<nRows;++row){
	for(int col=0;col<nCols;++col){
	  realType_t val=rCurrentImage_p.Pixel(col,row,channel);
	  rResultImage_p.Pixel(col,row,channel)=std::exp(val);
	}
      }
    }
  }


  //static
  void GradientsHdrComposition::addToImage(imageType_t const & rImage_p,
					   int imageNum_p,
					   weightImageType_t const & rMask_p,
					   imageType_t &rSumImageDx_p,imageType_t &rSumImageDy_p,
					   numImageType_t &rDxImage_p,
					   numImageType_t &rDyImage_p)
  {
    int const nCols=rImage_p.Width();
    int const nRows=rImage_p.Height();
    int const nChannels=rImage_p.NumberOfChannels();

    Assert(nCols==rSumImageDx_p.Width()+1);
    Assert(nRows==rSumImageDx_p.Height());
    Assert(nChannels==rSumImageDx_p.NumberOfChannels());

    Assert(nCols==rSumImageDy_p.Width());
    Assert(nRows==rSumImageDy_p.Height()+1);
    Assert(nChannels==rSumImageDy_p.NumberOfChannels());

    Assert(nCols==rDxImage_p.Width());
    Assert(nRows==rDxImage_p.Height());
    Assert(nChannels==rDxImage_p.NumberOfChannels());

    Assert(nCols==rDyImage_p.Width());
    Assert(nRows==rDyImage_p.Height());
    Assert(nChannels==rDyImage_p.NumberOfChannels());

    imageType_t dxImage, dyImage;
    const double zeroLimit=0.0; /// limit for weight that is considered zero
    // handle Dx
    TimeMessage startDx("Creating Dx");
    createDxImage(rImage_p,dxImage);
    startDx.Stop();
    TimeMessage startAddDx("Adding Dx to gradients");
    // transfer useful dx pixels
    for(int row=0;row<nRows;++row){
      for(int col=0;col<nCols-1;++col){
	if(rMask_p.Pixel(col,row)>zeroLimit && rMask_p.Pixel(col+1,row)>zeroLimit) {
	  // we are inside of image, and have useful gradient there
	  for(int channel=0;channel<nChannels;++channel){
	    realType_t currentVal=dxImage.Pixel(col,row,channel);
	    realType_t sumVal=rSumImageDx_p.Pixel(col,row,channel);
	    if(std::abs(currentVal)>std::abs(sumVal)){
	      rSumImageDx_p.Pixel(col,row,channel)=currentVal;
	      rDxImage_p.Pixel(col,row,channel)=imageNum_p;
	    } //if abs
	  } // for chan
	  // FIXME may need to add border handling
	}  //if inside
      }  //for col
    }  //for row
    dxImage.AllocateData(0,0); //save some memory
    startAddDx.Stop();

    // handle Dy just as dx
    TimeMessage startDy("Creating Dy");
    createDyImage(rImage_p,dyImage);
    startDy.Stop();
    TimeMessage startAddDy("Adding Dxy to gradients");
    // transfer useful dy pixels
    for(int row=0;row<nRows-1;++row){
      for(int col=0;col<nCols;++col){
	if(rMask_p.Pixel(col,row)>zeroLimit && rMask_p.Pixel(col,row+1)>zeroLimit) {
	  for(int channel=0;channel<nChannels;++channel){
	    // we are inside of image, and have useful gradient there
	    realType_t currentVal=dyImage.Pixel(col,row,channel);
	    realType_t sumVal=rSumImageDy_p.Pixel(col,row,channel);
	    if(std::abs(currentVal)>std::abs(sumVal)){
	      rSumImageDy_p.Pixel(col,row,channel)=currentVal;
	      rDyImage_p.Pixel(col,row,channel)=imageNum_p;
	    } //if abs()
	  } // for chan
	  // FIXME may need to add border handling
	}  // if inside
      }  //for col
    }  //for row
    startAddDy.Stop();
  }

  //static
  void GradientsHdrComposition::hdrCompositionProcessImage(
							   imageType_t const & rImage_p,
							   int imageNum_p,
							   realType_t dBias_p,
							   realType_t dBlackPoint_p,
							   int32 shrinkCount_p,
							   imageType_t &rSumImageDx_p,
							   imageType_t &rSumImageDy_p,
							   numImageType_t &rDxImage_p,
							   numImageType_t &rDyImage_p)
  {
#ifdef DEBUG
    int const nCols=rImage_p.Width();
    int const nRows=rImage_p.Height();
    int const nChannels=rImage_p.NumberOfChannels();
#endif
    Assert(nCols==rSumImageDx_p.Width()+1);
    Assert(nRows==rSumImageDx_p.Height());
    Assert(nChannels==rSumImageDx_p.NumberOfChannels());

    Assert(nCols==rSumImageDy_p.Width());
    Assert(nRows==rSumImageDy_p.Height()+1);
    Assert(nChannels==rSumImageDy_p.NumberOfChannels());

    Assert(nCols==rDxImage_p.Width());
    Assert(nRows==rDxImage_p.Height());
    Assert(nChannels==rDxImage_p.NumberOfChannels());

    Assert(nCols==rDyImage_p.Width());
    Assert(nRows==rDyImage_p.Height());
    Assert(nChannels==rDyImage_p.NumberOfChannels());

    Assert(shrinkCount_p>=0);
    Assert(dBlackPoint_p>=0.0);

    TimeMessage startAddImage("Adding image to data");

    TimeMessage startBiasImage("Applying bias()");
    imageType_t loggedImage=rImage_p;
    loggedImage.ResetSelections();
    loggedImage-=dBias_p;
    startBiasImage.Stop();

    TimeMessage startLogImage("Applying log()");
    logImage(loggedImage,loggedImage);
    startLogImage.Stop();

    // binarize image to find where background is.
    // actual image is then eroded by shrinkCount to make sure
    // that no aliased pixels are part of the image
    weightImageType_t maskImage;
    TimeMessage startBinarize("Binarize Image");
    binarizeImage(rImage_p,dBlackPoint_p,maskImage);
    TimeMessage startShrink("Shrinking mask");
    erodeMask(maskImage,shrinkCount_p);
    startShrink.Stop();

    TimeMessage startAddGradients("Adding gradients data");
    addToImage(loggedImage,imageNum_p,maskImage,rSumImageDx_p,rSumImageDy_p,rDxImage_p,rDyImage_p);
    startAddGradients.Stop();
  }

  //static
  void GradientsHdrComposition::hdrComposition(imageListType_t const & rImageList_p,
					       bool bKeepLog_p,
					       realType_t dBias_p,
					       imageType_t &rResultImage_p,
					       numImageType_t &rDxImage_p,
					       numImageType_t &rDyImage_p)
  {
    Assert(rImageList_p.Length()>=1);
    bool firstImage=true;
    int nCols=0,nRows=0,nChannels=0; /// size and color space of first image
    imageType_t::color_space colorSpace;
    imageType_t sumImageDx, sumImageDy; /// combined gradients in x and y direction. Note: these gradients are *between* the pixels
                                        /// of the original image, so size is one less then original image is direction of derivative
    int nImages=0; /// number of images read
    const int enlargeSize=1; // number of pixels added at the border

    TimeMessage startHdrComposition("Gradient Domain Hdr Composition");

    TimeMessage startLoadImages("Loading images");
    for(std::size_t i=0;i<rImageList_p.Length();++i){
      imageType_t currentImage;
      int imageIndex=0;
      // allow for multi-image files
      while(loadFile(rImageList_p[i],imageIndex,currentImage)){
	++nImages;
	++imageIndex;
	// expand image dimensions so I have sufficient border for morpological transform and convolution
	TimeMessage startEnlarge("creating border");
	currentImage.CropBy(enlargeSize,enlargeSize,enlargeSize,enlargeSize);
	startEnlarge.Stop();

	if(firstImage){
	  firstImage=false;
	  // determine those parameters that must be shared by all images
	  nCols=currentImage.Width();
	  nRows=currentImage.Height();
	  nChannels=currentImage.NumberOfChannels();
	  colorSpace=currentImage.ColorSpace();

	  //allocate necessary helper images
	  rDxImage_p.AllocateData(nCols,nRows,nChannels,colorSpace);
	  rDxImage_p.ResetSelections();
	  rDxImage_p.Black();
	  rDyImage_p.AllocateData(nCols,nRows,nChannels,colorSpace);
	  rDyImage_p.ResetSelections();
	  rDyImage_p.Black();

	  sumImageDx.AllocateData(nCols-1,nRows,nChannels,colorSpace);
	  sumImageDx.ResetSelections();
	  sumImageDx.Black();
	  sumImageDy.AllocateData(nCols,nRows-1,nChannels,colorSpace);
	  sumImageDy.ResetSelections();
	  sumImageDy.Black();

	} else {
	  // FIXME I wonder if I should check color space etc as well...
	  // check if properties of this image are identical to those of the first image
	  if(nCols!=currentImage.Width()) {
	    throw Error("Current image width differs from first image width.");
	  } else if(nRows!=currentImage.Height()) {
	    throw Error("Current image height differs from first image height.");
	  }	else if(nChannels!=currentImage.NumberOfChannels()) {
	    throw Error("Current image number of channels differs from first image number of channels.");
	  }
	}
	TimeMessage startProcessImage("Processing Image"+String(nImages));
	hdrCompositionProcessImage(currentImage,nImages,dBias_p,0.0,1,sumImageDx, sumImageDy ,rDxImage_p,rDyImage_p);
      }
    }
    startLoadImages.Stop();
    // at this point:
    // sumImageDx: max log gradient of images read in x direction
    // sumImageDy: max log gradient of images read in y direction
    // rSumMaskImage_p: mask with different values for the different sources of images. 0 is background.
    //              We use this later for information of the user, but it is not needed in the following process
    TimeMessage startHdr("HDR Combining Images");
    imageType_t laplaceImage;
    TimeMessage startLaplace("Creating Laplace image");
    // eliminate gradients that come from singularities, i.e. <=0 pixels
    clipGradients(sumImageDx);
    clipGradients(sumImageDy);
    createLaplaceVonNeumannImage(sumImageDx,sumImageDy,laplaceImage);
    startLaplace.Stop();
    TimeMessage startSolve("Solving Laplace");
    solveImage(laplaceImage,rResultImage_p);
    startSolve.Stop();
    rResultImage_p.ResetSelections();
    if(!bKeepLog_p){
      TimeMessage startExp("Performing Exp()");
      realType_t dLogBias=std::log(1.0e-7);
      rResultImage_p.Rescale(dLogBias,0.0); //assumes result range is 1e-7..1
      expImage(rResultImage_p, rResultImage_p);
      startExp.Stop();
    }
    startHdr.Stop();
#if 0
    // for debugging laplaceImage
    //rResultImage_p.Assign(laplaceImage);
    rResultImage_p.Assign(sumImageDx);
#else
    TimeMessage startEnlarge("shrinking border");
    rResultImage_p.CropBy(-enlargeSize,-enlargeSize,-enlargeSize,-enlargeSize);
    rDxImage_p.CropBy(-enlargeSize,-enlargeSize,-enlargeSize,-enlargeSize);
    rDyImage_p.CropBy(-enlargeSize,-enlargeSize,-enlargeSize,-enlargeSize);
    startEnlarge.Stop();
#endif
    TimeMessage startRescale("Rescaling Result");
    rResultImage_p.Rescale(); //FIXME something more clever?
    startRescale.Stop();
  }

};

// ****************************************************************************
// EOF GradientsHdrComposition.cpp - Released 2014/11/14 17:19:24 UTC
