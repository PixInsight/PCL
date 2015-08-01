#!/usr/bin/env python3
""" Launch the Aladin sky atlas (http://aladin.u-strasbg.fr/) for the current image

// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ExternalTool Process Module Version 01.00.02.0144
// ****************************************************************************
// launchAladin.py3 - Released 2014/11/14 17:19:24 UTC
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

import argparse
import shutil
import os
import pathlib
import subprocess

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


def parseArgs():
    """ Parse the command line
    :return result of argparse.ArgumentParser..parse_args()
    """
    parser = argparse.ArgumentParser(description="Launches the Aladin interactive sky atlas http://aladin.u-strasbg.fr/ on the current image. "
                                                 "Exits when you exit Aladin. On exit, the source image is copied to the result image, such that "
                                                 "the user can simply modify the source image to achieve changes in the result transfered to PixInsight. "
                                                 "Expects Aladin to be at ~/Aladin/Aladin, in $PATH, or at the location given with -p. If necessary, install the "
                                                 "software from http://aladin.u-strasbg.fr/")
    parser.add_argument("-f","--inFile",type=str,default=None,
                        help="input file. If given, this image is loaded into Aladin. If not given, Aladin and -o is given, Aladin generates a default image (see -g). "
                        "If not given and no -o is given, launch without image. Default=None.")
    parser.add_argument("-o","--outFile",type=str,default=None,
                        help="result file. This is the source file as (possibly) modified by Aladin on termination of the process.")
    parser.add_argument("-p","--path",type=str,default=getAladinPath(),
                        help="path to Aladin executable. Default: Aladin in $PATH or ~/Aladin/Aladin.")
    parser.add_argument("-g","--isGlobal",default=False,action="store_true",
                        help="on global apply, generate an initial image file that the user can use for storing his result.")
    # -h is implicit
    values=parser.parse_args()
    return values

def getAladinPath():
    """ find Aladin executable
    :return: path to Aladin executable in $HOME/Aladin or $PATH
    :throw ValueError() if path is not found
    """
    execName="Aladin"
    aladinPath=str(pathlib.Path(os.environ["HOME"])/execName)
    execPath=shutil.which(execName,path=aladinPath)
    if not execPath:
        # try PATH
        execPath=shutil.which(execName)
        if not execPath:
            raise ValueError("need Aladin installation in PATH or "+aladinPath+". Please install from http://aladin.u-strasbg.fr/, or specify path with -p.")
    return execPath


def figureToImage(fig,dpi=72):
    """ translate figure to 8 bit image
    """
    fig.canvas.draw()
    data = np.fromstring(fig.canvas.tostring_rgb(), dtype=np.uint8, sep='')
    shape=fig.canvas.get_width_height()[::-1]+(3,)
    data = data.reshape(shape)
    return data

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

def flipYImage(image):
    """ flip image along y axis
    :param image: image to be flippled about Y axis
    :return: flipped image
    """
    return image[::-1,:,:]

def flipYImageFile(fileName):
    """
    :param fileName: flip image in file along y axis
    """
    header,image=readFits(fileName)
    image=flipYImage(image)
    writeFits(fileName,image,header)

def genTemplateImage(resultFileName):
    """ generate 3 channel image with hint that the user is supposed to store the result here
    :return:generate
    """
    dimX,dimY=(1024,768)
    dpi=72
    figSize=(dimX/dpi,dimY/dpi)
    fig=plt.figure(figsize=figSize,dpi=dpi,tight_layout={"pad":0.0,"h_pad":0.0,"w_pad":0.0,"rect":(0, 0, 1, 1)},frameon=True)
    ax=fig.add_subplot(111,xlim=(0,dimX-1),ylim=(0,dimY-1))
    #ax.axes.get_xaxis().set_visible(False)
    #ax.axes.get_yaxis().set_visible(False)
    ax.annotate(s="Please store your result to this FITS file",color="g",xy=(dimX/5,dimY/2),rotation=0,size=30)
    image=figureToImage(fig,dpi)
    # mirror it so user can read
    image=image[::-1,:,:]
    writeFits(resultFileName,image)

def main():
    """evaluate args and do what is requested
    """
    # eval arguments
    parsedArgs=parseArgs()
    sourceFileName=parsedArgs.inFile
    resultFileName=parsedArgs.outFile
    bGlobal=parsedArgs.isGlobal
    execPath=parsedArgs.path

    if sourceFileName:
        if resultFileName:
            # this is where the result should go
            shutil.copyfile(sourceFileName,resultFileName)
        else:
            resultFileName=sourceFileName
        flipYImageFile(resultFileName)
    elif resultFileName:
        #generate a template image
        genTemplateImage(resultFileName)

    args=[execPath,"-nobanner"]
    if resultFileName:
        args.append(resultFileName)
    subprocess.call(args)
    if resultFileName:
        flipYImageFile(resultFileName)

if __name__=="__main__":
    main()
