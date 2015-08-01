#!/usr/bin/env python3
""" Solve image using the astrometry.net blind solver. Relies on astrometryDotNetClient.py2

// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ExternalTool Process Module Version 01.00.02.0144
// ****************************************************************************
// astrometryDotNetClient.py3 - Released 2014/11/14 17:19:24 UTC
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
import subprocess
import tempfile
import json
import sys

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

import matplotlib.image as mpimg


def readFits(fileName):
    """read fits file. Returns (header,data) of primary hdu. Data may be None
    """
    with afits.open(fileName,#do_not_scale_image_data=True,
                    uint=True) as hdulist:
        priHdu=hdulist[0]
        priHeader=priHdu.header
        #print("FITS header=",repr(priHeader))
        priData=priHdu.data
        # roll such that it fits imshow conventions
        if priData!=None and priData.ndim==3:
            priData=np.rollaxis(priData,0,3)
    return priHeader,priData

def writeFits(sOutFileName_p, data_p,header=None):
    """write numpy array to sOutFileName as FITS file
    """
    data_p=np.rollaxis(data_p,2,0)
    if header==None:
        afits.writeto(sOutFileName_p,data_p,clobber=True)
    else:
        hdu=afits.PrimaryHDU(data=data_p,header=header,uint=True)
        hduList=afits.HDUList([hdu])
        hduList.writeto(sOutFileName_p,clobber=True)

def figureToImage(fig,dpi=72):
    """ translate figure to 8 bit image
    """
    fig.canvas.draw()
    data = np.fromstring(fig.canvas.tostring_rgb(), dtype=np.uint8, sep='')
    shape=fig.canvas.get_width_height()[::-1]+(3,)
    data = data.reshape(shape)
    return data

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
            data=figureToImage(fig,dpi)
            afits.writeto(sOutFileName,data,clobber=True)

def imshow(ax,image):
    """plot image into axes, without axis
    return matplot axesImage object
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
    #ax.set_frame_on(False)
    #ax.axis("off")
    ax.axes.get_xaxis().set_visible(False)
    ax.axes.get_yaxis().set_visible(False)

    im=ax.imshow(image,cmap=plt.get_cmap("gray"),aspect="equal")
    return im

def plotAnnotations(image,annotations,dpi,figSize,bShow):
    """ plot the contour
    :param image: image to plot as ndarray. if nDims=3, the 3rd dim is the color channel
    :param annotations: annotations suggested by solver
    :param dpi: dots per inch for image
    :param figSize: size of figure
    :param bShow: if true, show the result
    :return new image, None if bShow=True
    """

    fig=plt.figure(figsize=figSize,dpi=dpi,tight_layout={"pad":0.0,"h_pad":0.0,"w_pad":0.0,"rect":(0, 0, 1, 1)},frameon=True)
    ax=fig.add_subplot(111)
    # basic image
    imshow(ax,image)
    yDim,xDim=image.shape[:2]
    #ax.plot((0,xDim),(0,yDim),color="g",marker="+")
    # annotations
    annotations=annotations["annotations"]
    commonKeys=["type","pixelx","pixely","names","radius","vmag"]
    for annotation in annotations:
        #print(annotation)
        type,pixelx,pixely,names,radius,vmag=map(annotation.get,commonKeys)
        text="/".join(names)
        if vmag:
            text+="["+str(vmag)+"]"
        # we need to mirror coords for y because of different coordinate system in astrometry.net
        pixely=yDim-pixely
        maxRadius=min(pixelx,pixely,xDim-pixelx,yDim-pixely)
        #print("o",radius,"m",maxRadius)
        #radius=min(maxRadius,max(radius,5))
        radius=max(radius,5)
        #print("r",radius)
        # limit radius to image size
        ax.annotate(s=text,xy=(pixelx+radius,pixely),color="g",clip_on=True,annotation_clip=True)
        circle=plt.Circle(xy=(pixelx,pixely),radius=radius,color="g",clip_on=True,fill=False)
        ax.add_patch(circle)

    if bShow:
        showFigure(fig,dpi)
        return None
    image=figureToImage(fig,dpi)
    return image

def getTmpFileName(suffix=""):
    """
    :suffix optional suffix of file
    :return name of temporary file. File is already created, but currently closed.
    """
    with tempfile.NamedTemporaryFile(suffix=suffix,delete=False) as tmpFile:
        tmpFileName=tmpFile.name
    return tmpFileName

def getAnnotateWcsFromJobId(path,jobId,apiKey):
    """ retrieve annotations and WCS data based on jobId
    :return: (annotationData,wcsData)
    """
    wcsFileName=None
    annotateFileName=None
    try:
        #wcsFileName="wcs.fit"
        #annotateFileName="annotate.json"
        wcsFileName=getTmpFileName()
        annotateFileName=getTmpFileName()
        command=[path,"--apikey=%s"%apiKey,"--jobid=%s"%jobId,"--annotate=%s"%annotateFileName,"--wcs=%s"%wcsFileName]
        #print(command)
        retCode=subprocess.call(command)
        if retCode!=0:
            raise ValueError("blind solver failed with status code %s"%retCode)
        wcs=readFits(wcsFileName)
        with open(annotateFileName,"r") as annotateFile:
            annotate=json.load(annotateFile)
    finally:
        # delete files FIXME
        if wcsFileName:
            pathlib.Path(wcsFileName).unlink()
        if annotateFileName:
            pathlib.Path(annotateFileName).unlink()
    return (annotate,wcs[0])

def writePng(fileName,image,bMirrorX=False,bMirrorY=False):
    """
    :param image: image to save
    :param fileName: filename under which to save
    :param bMirrorX: mirror the X coordinates (i.e. mirror along vertical mirror at central x coordinate)
    :param bMirrorY: mirror the Y corrdinates (i.e. miror along horizontal mirror at central y coordinate)
    """
    xStep=-1 if bMirrorX else 1
    yStep=-1 if bMirrorY else 1
    saveImage=image[::yStep,::xStep,:]
    mpimg.imsave(fileName, saveImage)

def getAnnotateWcsFromImage(path,image,apiKey):
    """
    get annotations and WCS data by solving image
    :param path: path to blind solver client
    :param image: image as array
    :param apiKey: api key for blind solver
    :return:(annotationData,wcsData)
    """
    wcsFileName=None
    annotateFileName=None
    pngFileName=None
    try:
        #wcsFileName="wcs.fit"
        #annotateFileName="annotate.json"
        #pngFileName="image.png"
        wcsFileName=getTmpFileName()
        annotateFileName=getTmpFileName()
        pngFileName=getTmpFileName(".png")
        #coordinate system in Astrometry.net is with flipped y coordinate
        writePng(pngFileName,image,False,True)
        command=[path,"--apikey=%s"%apiKey,"--private","--no_mod","--no_commercial","--upload=%s"%pngFileName,
                 "--annotate=%s"%annotateFileName,"--wcs=%s"%wcsFileName]
        retCode=subprocess.call(command)
        if retCode!=0:
            raise ValueError("blind solver failed with status code %s"%retCode)
        wcs=readFits(wcsFileName)
        with open(annotateFileName,"r") as annotateFile:
            annotate=json.load(annotateFile)
    finally:
        # delete files
        if wcsFileName:
            pathlib.Path(wcsFileName).unlink()
        if annotateFileName:
            pathlib.Path(annotateFileName).unlink()
        if pngFileName:
            pathlib.Path(pngFileName).unlink()
    return (annotate,wcs[0])

def mergeHeader(headerOrig,wcsHeader):
    """ merge the original header with the WCS header
    """
    new=headerOrig.copy()
    for key,entry in wcsHeader.items():
        #print(key,entry)
        new[key]=entry
    return new

def defaultClientPath():
    """ returns the default client path to the astrometry.net client, assumed to be in same directory as this script
    :return path
    """
    clientName="astrometryDotNetClient.py2"
    myName=sys.argv[0]
    clientPath=pathlib.Path(myName).absolute().parent/clientName
    return str(clientPath)

def parseArgs():
    """ Parse the command line
    :return result of argparse.ArgumentParser..parse_args()
    """
    parser = argparse.ArgumentParser(description="Solve image using the astrometry.net web service. Add the WCS data to the result file. "
                                                 "Before sending to astrometry.net, your image is converted to PNG to save bandwidth. "
                                                 "Optionally annotate the image.")
    parser.add_argument("-k","--apiKey",type=str,required=True,
                        help="API key required by astrometry.net. This is a seemingly random string such as sayfxqzyxelyhvjr assigned to your login "
                             "to Astrometry.net. See http://nova.astrometry.net/api_help for details.")
    parser.add_argument("-p","--path",type=str,default=defaultClientPath(),
                        help="name of astrometry.net client program. Default=%(default)s")
    parser.add_argument("-f","--inFile",type=str,required=True,
                        help="input file. Translated to PNG before sending to astrometry.net to save network bandwidth.")
    parser.add_argument("-o","--outFile",type=str,default=None,
                        help="output file. If given, plot is written to this file. Otherwise, a window opens to display the result. "
                             "Default=None")
    parser.add_argument("-a","--annotate",default=False,action="store_true",
                        help="annotate the solved image. The output format will be uint8. Without annotation, the input format is preserved. Default=False")
    parser.add_argument("-j","--jobId",type=int,default=None,
                        help="use solution from job with given job id instead of re-running the solver. Default=None")
    # -h is implicit
    values=parser.parse_args()
    return values

def main():
    """evaluate args and do what is requested
    """
    # eval arguments
    parsedArgs=parseArgs()
    sourceFileName=parsedArgs.inFile
    resultFileName=parsedArgs.outFile
    apiKey=parsedArgs.apiKey
    path=parsedArgs.path
    bAnnotate=parsedArgs.annotate
    jobId=parsedArgs.jobId
    # read image to handle
    header,image=readFits(sourceFileName)
    #writeFits(resultFileName,image)
    #import sys
    #sys.exit(1)
    # solve image, or retrieve results from previous job
    if jobId:
        # we have a jobId. We can get annotation and WCS data without solving again
        annotations,wcs=getAnnotateWcsFromJobId(path,jobId,apiKey)
    else:
        # solve
        annotations,wcs=getAnnotateWcsFromImage(path,image,apiKey)
    if bAnnotate:
        # do annotation
        # compute plot params
        dimY,dimX=image.shape[:2]
        #minDim=min(dimX,dimYDimY)
        # a 1280 image is readable at 72 dpi. Compute figSize as necessary
        dpi=72
        figSize=(dimX/dpi,dimY/dpi)
        image=plotAnnotations(image,annotations,dpi,figSize,resultFileName==None)
    if resultFileName:
        newHeader=mergeHeader(header,wcs)
        writeFits(resultFileName,image,newHeader)

if __name__=="__main__":
    main()
