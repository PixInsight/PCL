#!/usr/bin/env python3
""" Create contour plot from source image

// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ExternalTool Process Module Version 01.00.02.0144
// ****************************************************************************
// genContours.py3 - Released 2014/11/14 17:19:24 UTC
// ****************************************************************************
// This file is part of the ExternalTool PixInsight module.
//
// Copyright (c) 2014-2015, Georg Viehoever. All Rights Reserved.
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
// ****************************************************************************
"""


# libs available with python3 basic installation
import argparse
import pathlib

bImportFailed=False
try:
    import matplotlib.pyplot as plt
except ImportError as e:
    print(e)
    print("You need to install matplotlib and its dependencies first, either via your systems package manager, or using pip3.")
    print("Fedora 21: This is package is available via yum as python3-matplotlib.")
    bImportFailed=True

try:
    import numpy as np
except ImportError as e:
    print(e)
    print("You need to install the numpy package, either via your systems package manager, or using pip3.")
    print("On Fedora 21, this package is available as yum package python3-numpy, and usually installed")
    print("as a dependency of python3-matplotlib.")
    bImportFailed=True

try:
    import astropy.io.fits as afits
except ImportError as e:
    print(e)
    print("You need to install the astropy package,either via your systems package manager, or using pip3.")
    print("Fedora 21: This is package is available via yum as python3-astropy.")
    bImportFailed=True

if bImportFailed:
    raise ImportError("Failed to import at least one package. See messages above.")

def readFits(fileName):
    """read fits file. Returns (header,data) of primary hdu
    """
    with afits.open(fileName,#do_not_scale_image_data=True,
                    uint=True) as hdulist:
        priHdu=hdulist[0]
        priHeader=priHdu.header
        #print("FITS header=",repr(priHeader))
        priData=priHdu.data
        # roll such that it fits imshow conventions
        if priData.ndim==3:
            priData=np.rollaxis(priData,0,3)
    return priHeader,priData

def writeFits(sOutFileName_p, data_p):
    """write numpy array to sOutFileName as FITS file
    """
    afits.writeto(sOutFileName_p,data_p,clobber=True)

def parseArgs():
    """ Parse the command line
    :return result of argparse.ArgumentParser..parse_args()
    """
    parser = argparse.ArgumentParser(description="Create contour plot of source image. If image is multichannel image, "
                        "the contour is created for the average pixel values.")
    parser.add_argument("-f","--inFile",type=str,required=True,
                        help="input file.")
    parser.add_argument("-o","--outFile",type=str,default=None,
                        help="output file. If given, plot is written to this file. Otherwise, a window opens to display the plot. "
                             "Default=None")
    parser.add_argument("levels",type=float,nargs="+",
                        help="sequence of levels for contouring. The values between 0 and 1 are normalized to range of the source image")
    # -h is implicit
    values=parser.parse_args()
    return values

def showFigure(fig,dpi=72, sOutFileName=None):
    """ show figure. if sFileName!=None; save to fits file, else show interactive display

    Otherwise, created image is stores as file/store as new image in PixInsight
    """
    if sOutFileName==None:
        plt.show()
    else:
        suffix=pathlib.PurePath(sOutFileName).suffix
        if (not suffix in [".fit",".fits"]):
            # many image formats such as png or pdf are handled by matplotlib, but not fits
            fig.savefig(sOutFileName,dpi=dpi,bbox_inches='tight',pad_inches=0)
        else:
            # writing fits file
            fig.canvas.draw()
            data = np.fromstring(fig.canvas.tostring_rgb(), dtype=np.uint8, sep='')
            shape=fig.canvas.get_width_height()[::-1]+(3,)
            data = data.reshape(shape)
            # PI is not happy with data where NAXIS1=numberOfChannels
            data = np.rollaxis(data,2)
            afits.writeto(sOutFileName,data,clobber=True)

def imshow(ax,image):
    """plot image into axes
    """
    isColor=(image.ndim==3)
    if isColor:
        # color image. ax.imshow natively displays images with bytes and float.
        # For float, it assumes that the elements are RGB values in [0,1]. For
        # integer types !=byte, we therefore need to transform the image
        dtype=image.dtype
        type=dtype.type
        if issubclass(type,(np.uint16,np.uint32,np.uint64)):
            maxVal=np.iinfo(dtype).max
            image=image.astype(np.float32)/maxVal
    # cmap gray for single channel images
    ax.imshow(image,cmap=plt.get_cmap("gray"))

def plotContour(image,levels,dpi,figSize,fileName=None):
    """ plot the contour
    :param image: image to plot as ndarray. if nDims=3, the 3rd dim is the color channel
    :param levels: list of values for contouring
    :param dpi: dots per inch for image
    :param figSize: size of figure
    :param fileName. If None, do interactive show
    """
    if(image.ndim == 2):
        contourImage=image
    elif(image.ndim == 3):
        # multichannel image, create sum of channels
        contourImage=image[:,:,0].copy()
        for i in range(1,image.shape[2]):
            contourImage+=image[:,:,i]
        contourImage=contourImage/image.shape[2]
    else:
        raise ValueError("Need 2d image or multichannel 2d image with color as 3rd channel")
    # scale levels
    minVal=contourImage.min()
    maxVal=contourImage.max()
    levels=minVal+(maxVal-minVal)*levels
    #print("minVal",minVal,",maxVal=",maxVal,",levels=",levels)
    fig=plt.figure(figsize=figSize,dpi=dpi,tight_layout=True)
    ax=fig.add_subplot(111)
    fig.tight_layout(pad=0)
    #image
    imshow(ax,image)
    # contour
    cs=ax.contour(contourImage,levels)
    ax.clabel(cs,fontsize="xlarge")
    #legend
    labels = [str(level) for level in levels]
    for i,label in enumerate(labels):
        cs.collections[i].set_label(label)
    ax.legend(loc='upper left')
    showFigure(fig,dpi,fileName)

def main():
    """evaluate args and do what is requested
    """
    # eval arguments
    parsedArgs=parseArgs()
    sourceFileName=parsedArgs.inFile
    resultFileName=parsedArgs.outFile
    levels=np.array(parsedArgs.levels)
    # read data
    header,image=readFits(sourceFileName)
    imageDims=image.shape[:2]
    # compute plot params
    minDim=min(imageDims)
    # a 1280 image is readable at 72 dpi. Compute DPI as necessary
    dpi=minDim/1280*72
    figSize=(imageDims[1]/dpi,imageDims[0]/dpi)
    plotContour(image,levels,dpi,figSize,resultFileName)

if __name__=="__main__":
    #print("Started")
    main()
    #print("Stopped")

