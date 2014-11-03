// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard GradientDomain Process Module Version 00.06.04.0096
// ****************************************************************************
// GradientsMergeMosaic.cpp - Released 2014/10/29 07:35:25 UTC
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

#include <pcl/SeparableConvolution.h>
#include <pcl/LinearFilter.h>

#include "GradientsMergeMosaic.h"

namespace pcl {

  GradientsMergeMosaic::GradientsMergeMosaic()
    :GradientsBase()
  {
  }

  GradientsMergeMosaic::~GradientsMergeMosaic()
  {
  }



  //static
  void GradientsMergeMosaic::addToMask(weightImageType_t const & rMask_p,
				       pcl_enum eType_p,
				       sumMaskImageType_t &rSumMaskImage_p)
  {
    int const nCols=rMask_p.Width();
    int const nRows=rMask_p.Height();
    switch(eType_p) {
      // FIXME sure there must be a faster way to do this via masked ops?!?
    case GradientsMergeMosaicType::Overlay:
      for(int row=0;row<nRows;++row){
	for(int col=0;col<nCols;++col){
	  rSumMaskImage_p.Pixel(col,row)*=2.0;
	  if(rMask_p.Pixel(col,row)>0.0) {
	    //	    rSumMaskImage_p.Pixel(col,row)=rMask_p.Pixel(col,row);
	    rSumMaskImage_p.Pixel(col,row)=rMask_p.Pixel(col,row)+(1.0-rMask_p.Pixel(col,row))*rSumMaskImage_p.Pixel(col,row);
	  }
	}
      }
      break;
    case GradientsMergeMosaicType::Average:
      for(int row=0;row<nRows;++row){
	for(int col=0;col<nCols;++col){
	  rSumMaskImage_p.Pixel(col,row)*=2.0;
	  if(rMask_p.Pixel(col,row)>0.0) {
	    rSumMaskImage_p.Pixel(col,row)+=rMask_p.Pixel(col,row);
	  }
	}
      }
      break;
    default:
      Assert(false);
    }
  }


  //static
  void GradientsMergeMosaic::erodeMaskConvolve(weightImageType_t & rMask_p,int32 shrinkCount_p)
  {
    int sideLength=1+2*shrinkCount_p;
    int nElements=sideLength*sideLength;

    SeparableFilter filter( sideLength,1.0/nElements);
    SeparableConvolution convolve(filter);
    rMask_p.ResetSelections();
    convolve >> rMask_p;
    rMask_p.Binarize(0.999999);
  }

  //static
  void GradientsMergeMosaic::convolveMask(weightImageType_t &rMask_p, int32 featherRadius_p)
  {
    int sideLength=1+2*featherRadius_p;
    int nElements=sideLength*sideLength;

#if 0
#if 0
    // linear filter
    LinearFilter filter(sideLength,1.0/nElements,1.0/nElements);
#else
    // box filter
    KernelFilter filter( sideLength, 1.0/nElements );
#endif
#if 1
    std::cout<<"FilterCoefficients ";
    for (const float *val=filter.Begin();val!=filter.End();++val){
      std::cout<<*val<<",";
    }
    std::cout<<std::endl;
#endif
    Convolution convolve(filter);
#else
    // separable filter
    SeparableFilter filter( sideLength,1.0/nElements);
    SeparableConvolution convolve(filter);
#endif
    convolve >>rMask_p;
  }

  //static
  void GradientsMergeMosaic::featherMask(weightImageType_t &rMask_p, int32 featherRadius_p)
  {
#if 0
    // create test pattern
    int const nCols=rMask_p.Width();
    int const nRows=rMask_p.Height();
    int const nChannels=rMask_p.NumberOfChannels();
    rMask_p.Black();
    for(int channel=0;channel<nChannels;++channel){
      for(int col=nCols*0.25;col<nCols*0.75;++col){
	for(int row=0;row<nRows;row++){
	  rMask_p.Pixel(col,row,channel)=1.0;
	}
      }
    }
#endif
    if(featherRadius_p<=0){
      // nothing to do
      return;
    }
    // first, I need to shrink the mask, then convolve
    TimeMessage startFeatherErode("Erode mask for feathering");
    // FIXME this is quite slow for featherRadius_p>20.
    // We would need a separable erode, promised by Juan...
    //    erodeMask(rMask_p, featherRadius_p);
    // FIXME this is quick workaround, using separable convolution instead
    erodeMaskConvolve(rMask_p, featherRadius_p);
    startFeatherErode.Stop();

    TimeMessage startFeatherConvolve("ConvolveMask for feathering");
    convolveMask(rMask_p, featherRadius_p);
    startFeatherConvolve.Stop();
  }


  ///static
  void GradientsMergeMosaic::addBorder(weightImageType_t const & rFullMask_p, weightImageType_t &rShrinkedMask_p)
  {
    int const nCols=rShrinkedMask_p.Width();
    int const nRows=rShrinkedMask_p.Height();
    int const nChannels=rShrinkedMask_p.NumberOfChannels();
    Assert(nCols==rFullMask_p.Width());
    Assert(nRows==rFullMask_p.Height());
    Assert(nChannels==rFullMask_p.NumberOfChannels());

    // multiple channels really not necessary, but who cares...
    for(int channel=0;channel<nChannels;++channel){
      for(int col=0;col<nCols;++col){
	for(int row=0;row<nRows;row++){
	  if(rFullMask_p.Pixel(col,row,channel)!=0.0 && rShrinkedMask_p.Pixel(col,row,channel)==0){
	    rShrinkedMask_p.Pixel(col,row,channel)= -1.0;
	  }
	}
      }
    }
  }

  //static
  void GradientsMergeMosaic::addToImage(imageType_t const & rImage_p,pcl_enum eType_p,
					weightImageType_t const & rMask_p,
					imageType_t &rSumImageDx_p,imageType_t &rSumImageDy_p,
					weightImageType_t &rCountImageDx_p, weightImageType_t &rCountImageDy_p)
  {
    int const nCols=rImage_p.Width();
    int const nRows=rImage_p.Height();
    int const nChannels=rImage_p.NumberOfChannels();
    imageType_t dxImage, dyImage;

    const double zeroLimit=0.0; /// limit for weight that is considered zero


    TimeMessage startDx("Creating Dx");
    createDxImage(rImage_p,dxImage);
    startDx.Stop();
    TimeMessage startAddDx("Adding Dx");
    // transfer useful dx pixels
    // FIXME this is a relatively slow loop. Think about making it faster
    for(int row=0;row<nRows;++row){
      for(int col=0;col<nCols-1;++col){
	if(rMask_p.Pixel(col,row)>zeroLimit && rMask_p.Pixel(col+1,row)>zeroLimit) {
	  // we are inside of image
	  realType_t weight=(rMask_p.Pixel(col,row)+rMask_p.Pixel(col+1,row))/2.0;
	  if(eType_p==GradientsMergeMosaicType::Average){
	    if(rCountImageDx_p.Pixel(col,row)<=0.0){
	      // first foreground pixel on this location
	      for(int channel=0;channel<nChannels;++channel){
		rSumImageDx_p.Pixel(col,row,channel)=dxImage.Pixel(col,row,channel)*weight;
	      }
	    } else {
	      // there have been other pixels. Create average
	      for(int channel=0;channel<nChannels;++channel){
		rSumImageDx_p.Pixel(col,row,channel)+=dxImage.Pixel(col,row,channel)*weight;
	      }
	    }
	    rCountImageDx_p.Pixel(col,row)+=weight;
	  } else {
	    // type overlay, last gradient wins
	    if(rCountImageDx_p.Pixel(col,row)<=0.0){
		// first foreground pixel on this location
	      for(int channel=0;channel<nChannels;++channel){
		rSumImageDx_p.Pixel(col,row,channel)=dxImage.Pixel(col,row,channel);
	      }
	      rCountImageDx_p.Pixel(col,row)=1.0; //mark as used
	    } else {
	      // there have been other pixels. Blend
	      for(int channel=0;channel<nChannels;++channel){
		rSumImageDx_p.Pixel(col,row,channel)=dxImage.Pixel(col,row,channel)*weight+rSumImageDx_p.Pixel(col,row,channel)*(1.0-weight);
	      }
	    }
	  } //if type
	} else if(rCountImageDx_p.Pixel(col,row)==0.0 &&
		  (rMask_p.Pixel(col,row)<0.0 || rMask_p.Pixel(col+1,row)<0.0)) {
	  // we are at border of image and dont have values there. Just copy in gradient so if nothing else comes in, we have at least the border
	  for(int channel=0;channel<nChannels;++channel){
	    rSumImageDx_p.Pixel(col,row,channel)=dxImage.Pixel(col,row,channel);
	  }
	  // add if first should win. Otherwise last will win
	  //rCountImageDx_p.Pixel(col,row)=-1.0; // mark as background already occupied
	}
      }  //for col
    }  //for row
    dxImage.AllocateData(0,0); //save some memory
    startAddDx.Stop();

    TimeMessage startDy("Creating Dy");
    // transfer useful dy pixels
    createDyImage(rImage_p,dyImage);
    startDy.Stop();

    TimeMessage startAddDy("Adding Dy");
    for(int row=0;row<nRows-1;++row){
      for(int col=0;col<nCols;++col){
	if(rMask_p.Pixel(col,row)>zeroLimit && rMask_p.Pixel(col,row+1)>zeroLimit) {
	  realType_t weight=(rMask_p.Pixel(col,row)+rMask_p.Pixel(col,row+1))/2.0;
	  if(eType_p==GradientsMergeMosaicType::Average){
	    // type average and inside
	    if(rCountImageDy_p.Pixel(col,row)<=0.0){
	      // first foreground pixel on this location
	      for(int channel=0;channel<nChannels;++channel){
		rSumImageDy_p.Pixel(col,row,channel)=dyImage.Pixel(col,row,channel)*weight;
	      }
	    } else {
	      // we already were there. Blend
	      for(int channel=0;channel<nChannels;++channel){
		rSumImageDy_p.Pixel(col,row,channel)+=dyImage.Pixel(col,row,channel)*weight;
	      }
	    }
	    rCountImageDy_p.Pixel(col,row)+=weight;
	  } else {
	    // type overlay and inside, last gradient wins
	    if(rCountImageDy_p.Pixel(col,row)<=0.0){
	      // first foreground pixel on this location
	      for(int channel=0;channel<nChannels;++channel){
		rSumImageDy_p.Pixel(col,row,channel)=dyImage.Pixel(col,row,channel);
	      }
	      rCountImageDy_p.Pixel(col,row)=1.0; //mark as used
	    } else {
	      // we have been there, merge
	      for(int channel=0;channel<nChannels;++channel){
		rSumImageDy_p.Pixel(col,row,channel)=dyImage.Pixel(col,row,channel)*weight+rSumImageDy_p.Pixel(col,row,channel)*(1.0-weight);
	      }
	    }
	  }  //if type
	} else if(rCountImageDy_p.Pixel(col,row)==0.0 &&
		  (rMask_p.Pixel(col,row)<0.0 || rMask_p.Pixel(col,row+1)<0.0)){
	  // we are outside of image and dont have values there. Just copy in gradient
	  for(int channel=0;channel<nChannels;++channel){
	    rSumImageDy_p.Pixel(col,row,channel)=dyImage.Pixel(col,row,channel);
	  }
	  // add if first should win. Otherwise last will win
	  //rCountImageDy_p.Pixel(col,row)=-1.0;
	}
      } //for col
    } //for row
    startAddDy.Stop();
  }

  //static
  void GradientsMergeMosaic::averageImage(pcl_enum eType_p,
					  weightImageType_t const &rCountImage_p,
					  imageType_t &rImage_p)
  {
    int const nCols=rImage_p.Width();
    int const nRows=rImage_p.Height();
    int const nChannels=rImage_p.NumberOfChannels();
    switch(eType_p) {
      // FIXME sure there must be a faster way to do this via masked ops?!?
    case GradientsMergeMosaicType::Overlay:
      // do nothing
      break;
    case GradientsMergeMosaicType::Average:
      // FIXME use builtin PCL for this
      for(int channel=0;channel<nChannels;++channel){
	for(int row=0;row<nRows;++row){
	  for(int col=0;col<nCols;++col){
       	    if(rCountImage_p.Pixel(col,row)>0.0){
	      rImage_p.Pixel(col,row,channel)/=static_cast<realType_t>(rCountImage_p.Pixel(col,row));
	    }
	  }
	}
      }
      break;
    default:
      Assert(false);
    }
  }

  //static
  void GradientsMergeMosaic::mergeMosaicProcessImage(imageType_t const & rImage_p, realType_t dBlackPoint_p,
						     pcl_enum eType_p,
						     int32 shrinkCount_p,
						     int32 featherRadius_p,
						     imageType_t &rSumImageDx_p,
						     imageType_t &rSumImageDy_p,
						     sumMaskImageType_t &rSumMaskImage_p,
						     weightImageType_t &rCountImageDx_p,
						     weightImageType_t &rCountImageDy_p)
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

    Assert(nCols==rSumMaskImage_p.Width());
    Assert(nRows==rSumMaskImage_p.Height());
    Assert(1==rSumMaskImage_p.NumberOfChannels());

    Assert(eType_p!=GradientsMergeMosaicType::Average || nCols==rCountImageDx_p.Width()+1);
    Assert(eType_p!=GradientsMergeMosaicType::Average || nRows==rCountImageDx_p.Height());
    Assert(eType_p!=GradientsMergeMosaicType::Average || 1==rCountImageDx_p.NumberOfChannels());
    Assert(eType_p!=GradientsMergeMosaicType::Average || nCols==rCountImageDy_p.Width());
    Assert(eType_p!=GradientsMergeMosaicType::Average || nRows==rCountImageDy_p.Height()+1);
    Assert(eType_p!=GradientsMergeMosaicType::Average || 1==rCountImageDy_p.NumberOfChannels());

    Assert(shrinkCount_p>=0);
    Assert(featherRadius_p>=0);
    Assert(dBlackPoint_p>=0.0);
    weightImageType_t maskImage;

    TimeMessage startAddImage("Adding image to data");

    TimeMessage startBinarize("Binarize Image");
    binarizeImage(rImage_p,dBlackPoint_p,maskImage);
    // save this for border computation later
    weightImageType_t fullMask(maskImage);
    startBinarize.Stop();

    // we are doing this because image after StarAlign usually contain aliased pixels.
    // These must not to be used during merge.
    TimeMessage startShrink("Shrinking mask");
    erodeMask(maskImage,shrinkCount_p);
    startShrink.Stop();
    TimeMessage startFeather("Feathering mask");
    featherMask(maskImage,featherRadius_p);
    startFeather.Stop();

    TimeMessage startBorder("Computing border");
    addBorder(fullMask,maskImage);
    fullMask.AllocateData(0,0); // save memory
    startBorder.Stop();

    TimeMessage startSumMask("Creating combined mask");
    addToMask(maskImage,eType_p,rSumMaskImage_p);
    startSumMask.Stop();
    TimeMessage startAddGradients("Adding gradients data");
    addToImage(rImage_p,eType_p,maskImage,rSumImageDx_p,rSumImageDy_p,rCountImageDx_p, rCountImageDy_p);
    startAddGradients.Stop();
  }


  //static
  void GradientsMergeMosaic::mergeMosaic(imageListType_t const & rImageList_p,
					 realType_t dBlackPoint_p,
					 pcl_enum eType_p,
					 int32 shrinkCount_p,
					 int32 featherRadius_p,
					 imageType_t &rResultImage_p,
					 sumMaskImageType_t &rSumMaskImage_p)
  {
    Assert(rImageList_p.Length()>=1);
    bool firstImage=true;
    int nCols=0,nRows=0,nChannels=0; /// size and color space of first image
    imageType_t::color_space colorSpace;
    weightImageType_t countImageDx, countImageDy; /// number of pixels that contributed to sumImageDx,Dy in average mode
    imageType_t sumImageDx, sumImageDy; /// combined gradients in x and y direction. Note: these gradients are *between* the pixels
                                        /// of the original image, so size is one less then original image is direction of derivative
    int nImages=0; /// number of images read
    const int enlargeSize=1; // number of pixels added at the border

    TimeMessage startMergeMosaic("Gradient Domain Merge Mosaic");

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
	  rSumMaskImage_p.AllocateData(nCols,nRows);
	  rSumMaskImage_p.ResetSelections();
	  rSumMaskImage_p.Black();
	  sumImageDx.AllocateData(nCols-1,nRows,nChannels,colorSpace);
	  sumImageDx.ResetSelections();
	  sumImageDx.Black();
	  sumImageDy.AllocateData(nCols,nRows-1,nChannels,colorSpace);
	  sumImageDy.ResetSelections();
	  sumImageDy.Black();

	  countImageDx.AllocateData(nCols-1,nRows);
	  countImageDx.Black();
	  countImageDy.AllocateData(nCols,nRows-1);
	  countImageDy.Black();
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
	mergeMosaicProcessImage(currentImage,dBlackPoint_p,eType_p,shrinkCount_p,featherRadius_p,sumImageDx, sumImageDy ,rSumMaskImage_p,countImageDx, countImageDy);
      }
    }
    startLoadImages.Stop();
    if (eType_p==GradientsMergeMosaicType::Average) {
      TimeMessage startAverage("Averaging images");
      averageImage(eType_p,countImageDx,sumImageDx);
      averageImage(eType_p,countImageDy,sumImageDy);
      // we do not need count images any longer
      countImageDx.AllocateData(0,0);
      countImageDy.AllocateData(0,0);
    }
    // at this point:
    // sumImageDx: Average or overlay of gradients of images read in x direction
    // sumImageDy: Average or overlay of gradients of images read in y direction
    // rSumMaskImage_p: mask with different values for the different sources of images. 0 is background.
    //              We use this later for information of the user, but it is not needed in the following process
    TimeMessage startMerge("Merging Images");
    imageType_t laplaceImage;
    TimeMessage startLaplace("Creating Laplace image");
    createLaplaceVonNeumannImage(sumImageDx,sumImageDy,laplaceImage);
    startLaplace.Stop();
    TimeMessage startSolve("Solving Laplace");
    solveImage(laplaceImage,rResultImage_p);
    startSolve.Stop();
    startMerge.Stop();
    rResultImage_p.ResetSelections();
#if 0
    // for debugging laplaceImage
    //    rResultImage_p.Assign(laplaceImage);
    rResultImage_p.Assign(sumImageDx);
#else
    TimeMessage startEnlarge("shrinking border");
    rResultImage_p.CropBy(-enlargeSize,-enlargeSize,-enlargeSize,-enlargeSize);
    rSumMaskImage_p.CropBy(-enlargeSize,-enlargeSize,-enlargeSize,-enlargeSize);
    startEnlarge.Stop();
#endif
    TimeMessage startRescale("Rescaling Result");
    rResultImage_p.Rescale(); //FIXME something more clever?
    startRescale.Stop();
  }

};

// ****************************************************************************
// EOF GradientsMergeMosaic.cpp - Released 2014/10/29 07:35:25 UTC
