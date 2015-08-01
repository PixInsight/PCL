#!/usr/bin/env python3
""" Plot graphical overview of visibilities of solar system objects

// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ExternalTool Process Module Version 01.00.02.0144
// ****************************************************************************
// ephemPlot.py3 - Released 2014/11/14 17:19:24 UTC
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
import sys
import tempfile
import os
import datetime
import argparse
import pathlib

# libs that may need to be installed on top of python3
bImportFailed=False
try:
    import matplotlib.pyplot as plt
except ImportError as e:
    print(e)
    print("You need to install matplotlib and its dependencies first, either via your systems package manager, or using pip3.")
    print("Fedora 21: This is package is available via yum as python3-matplotlib.")
    bImportFailed=True

try:
    import ephem
except ImportError as e:
    print(e)
    print("You need to install the ephem package first, either via your systems package manager, or using pip3.")
    print("Fedora 21: This package is not available via yum (only for python 2). Instead, you need to pip3 install ephem.")
    print("   Before doing so, install python3-devel via yum.")
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

# other imports
import ephem.cities

def printTable(name,table):
    """ print rising/setting times as table for debugging
    """
    
    print("")
    print("Table for %s"%name)
    for day,setting,rising in table:
        print("%s %s %s"%(day,setting,rising))
    print("")
        
def splitOffsets(table,bRisings):
    """ split days and offsets by None entries in offsets
    
    returns list of sequences (day, hourOffset), where each sequence
    is from a list of days where the chosen event has never been None.
    This sequence can be plotted as a partial curve for the events in table. The full
    plot can be done by plotting all sequences.
    """
    
    res=list()
    subList=list()
    for day,setting,rising in table:
        if bRisings:
            entry=rising
        else:
            entry=setting
        if entry!=None:
            subList.append( (day,entry) )
        elif len(subList)!=0:
            res.append(subList)
            subList=list()
    # add final sequence
    if len(subList)!=0:
        res.append(subList)
    return res;
            
        
def plotObject(ax,table,name,color):
    """ plot object given by table, name with color (e.g "y" for yellow)
    
    Risings are plotted as full line -, settings as broken line --.
    Legend handles are created.
    """
    
    table=tableToOffsets(table)
    settings=splitOffsets(table,False)
    risings=splitOffsets(table,True)
    legendLabel=name+" set"
    for entry in settings:
        days=[x[0] for x in entry]
        offsets=[x[1] for x in entry]
        ax.plot(days,offsets,"--",label=legendLabel,linewidth=2,color=color)
        # make sure we get only one entry for this object
        legendLabel=None
    legendLabel=name+" rise"
    for entry in risings:
        days=[x[0] for x in entry]
        offsets=[x[1] for x in entry]
        ax.plot(days,offsets,"-",label=legendLabel,linewidth=2,color=color)
        legendLabel=None

        
def calcSkyBrightness(observer,day,hour):
    """ estimate brightness of the sky for given location, day and hour
    
    This is an ad hoc formula. For serious work, use something like
    http://articles.adsabs.harvard.edu/cgi-bin/nph-iarticle_query?1991PASP..103.1033K&defaultprint=YES&filetype=.pdf
    
    Idea here: Values between 0 and 100. 100 is daylight, 0 dark night without moon. dark night astronomical darkness=
    sun 18 degrees below horizon. If moon is above horizon, add brightness 0..brightnessMul*maxMoonBrightness
    according to phase
    """
    
    #magnitude range of moon to consider
    minMoon=-6
    maxMoon=-12.8
    diffMoon=minMoon-maxMoon
    maxMoonBright=15.0
    brightnessFactor=1.0+1.0/maxMoonBright
    brightnessMul=5.0
    
    currentDate=day+hour*ephem.hour
    observer.date=currentDate
    sun=ephem.Sun()
    sun.compute(observer)
    moon=ephem.Moon()
    moon.compute(observer)
    sunAlt=sun.alt

    # angle for astronomical darkness
    darkAngle=ephem.hours(ephem.degree*18)
    brightness=0.0
    if sunAlt>0:
        # above horizon
        brightness=100
    else:
        if sunAlt>-darkAngle:
            # dusk/dawn
            brightness=(darkAngle+sunAlt)/darkAngle*100
        else:
            brightness=0
        # add moon
        if (moon.alt>0):
            # moon above horizon
            moonMag=min(minMoon,max(moon.mag,maxMoon))
            #value between 0 and 1 (for bright)
            moonBright= -(moonMag-minMoon)/diffMoon
            # empirical formula that also highlights full moon phases
            moonBright=1.0/(brightnessFactor-moonBright)*brightnessMul
            #print "moonMag",moonMag,moonBright
            # assumes range for moonMag between 
            brightness=min(brightness+moonBright,100)
    #print "Brightness",brightness 
    return brightness
            

def plotSkyBrightness(ax,observer,sunTable,hoursMin,hoursMax,hourStep):
    """ plot estimate of the brightness of the sky into ax (dark =deep blue, bright=white)
    
    - ax is the Axes object used for drawing
    - observer defines the location, 
    - sunTable is the table for the sun indicating the dates (x range) for the plot
    - hoursMin/Max is the y range of the plot 
    - hoursStep is the resolution for computations in the y range.
    """
    
    days=[x[0] for x in sunTable]
    daysMin=min(days)
    daysMax=max(days)
    hours=np.arange(hoursMin,hoursMax,hourStep)
    days,hours=np.meshgrid(days,hours)
    vecFunc=np.vectorize(calcSkyBrightness)
    brightness=vecFunc(observer,days,hours)
    # note flipped orientation in y
    brightness=np.flipud(brightness)
    ax.imshow(brightness,extent=[daysMin,daysMax,hoursMin,hoursMax],aspect="auto",cmap=plt.cm.PuBu_r)

def showFigure(fig,dpi=72, sOutFileName=None):
    """ show figure. if sFileName!=None; save to fits file, else show interactive display
    
    Otherwise, created image is stores as file/store as new image in PixInsight
    """
    if sOutFileName==None:
        #plt.Figure.show()
        plt.show()
        #time.sleep(5)
    else:
        suffix=pathlib.PurePath(sOutFileName).suffix
        #print(suffix)
        if (not suffix in [".fit",".fits"]):
            # many image formats such as png or pdf are handled by matplotlib, but not fits
            fig.savefig(sOutFileName,dpi=dpi)
        else:
            # writing fits file
            fig.canvas.draw()
            data = np.fromstring(fig.canvas.tostring_rgb(), dtype=np.uint8, sep='')
            shape=fig.canvas.get_width_height()[::-1]+(3,)
            data = data.reshape(shape)
            # PI is not happy with data where NAXIS1=numberOfChannels
            data = np.rollaxis(data,2)
            afits.writeto(sOutFileName,data,clobber=True)
      
def plotSettingRising(title,utcOffset,year,observer,sunTable,objectsTable,sOutFileName,imageDims):
    """create plot of sun and moon as background, plus plot the objects in ObjectsTable
    
    objectsTable is list of triples (name,table,color)
    utcOffset is the offset of local time compared to UTC in hours (e.g. 1 for Munich winter time)
    year is the year for which computations happened
    observer defines the location of the observer, an ephem.observer object
    sunTable is the table of setting/rising times of the sun
    objectTable is the list of tables for other objects
    outfile: Name of result file. If None, display result interactively.
    imageDims (xDim,yDim) in pixels
    dpi is dots per inch
    """
    minDim=min(imageDims)
    # a 1280 image is readable at 72 dpi. Compute DPI as necessary
    dpi=minDim/1280*72
    figSize=(imageDims[0]/dpi,imageDims[1]/dpi)
    #print(figSize)

    # plot sun
    sunTableOffsets=tableToOffsets(sunTable)
    days,settings,risings=zip(*sunTableOffsets)

    fig=plt.figure(figsize=figSize,dpi=dpi)                       #figsize=(11.69,8.27)) #A4
    ax=fig.add_subplot(111)
    ax.plot(days,settings,"--",color="gold",linewidth=3,label="Sunset")
    ax.plot(days,risings,"-",color="gold",linewidth=3,label="Sunrise")
    ylim=ax.get_ylim()
    
    # plot brightness indicator as background
    plotSkyBrightness(ax,observer,sunTable,ylim[0],ylim[1],0.25)
    
    # plot other objects, e.g. moon, planets, ...
    for name,table,color in objectsTable:
        plotObject(ax,table,name,color)

    # adjust y labels to UTC+offset time
    ax.set_ylabel("UTC%+g"%utcOffset)
    nLabels=len(ax.get_yticklabels())
    yLim=ax.get_ylim()
    step=float(yLim[1]-yLim[0])/(nLabels-1.0)
    labels=[(yLim[0]+i*step+utcOffset)%24 for i in range(0,nLabels+1)]
    ax.set_yticklabels(labels)
    
    # adjust x label and ticks to begin of month
    ax.set_xlabel("Begin of Month")
    ticks=[ephem.date( (year,month,1,0,0,0) ) for month in range(1,13)]
    ax.xaxis.set_ticks(ticks)
    ax.set_xticklabels(list(range(1,13)))
        
    ax.set_title(title)
    ax.grid(True)
          
    # legend
    handles, labels = ax.get_legend_handles_labels()
    # Shrink current axes to get spaces of labels
    box = ax.get_position()
    ax.set_position([box.x0, box.y0, box.width * 0.9, box.height])
    # Put a legend to the right of the current axis
    ax.legend(handles,labels,loc='center left', bbox_to_anchor=(1, 0.5))
    ax.set_xlim(min(days),max(days))

    # show
    showFigure(fig,dpi,sOutFileName)
    
def tableToOffsets(table):
    """ translate tables of settings and risings to ephem date 0am UTC and offset from UTC in hours float
    """

    res=list()
    for currentDay,setting,rising in table:
        cYear,cMonth,cDay,cHour,cMinute,cSecond=currentDay.tuple()
        day=datetime.datetime(cYear,cMonth,cDay,cHour,cMinute,int(cSecond))
        if setting!=None:
            sYear,sMonth,sDay,sHour,sMinute,sSecond=setting.tuple()
            settingTime=datetime.datetime(sYear,sMonth,sDay,sHour,sMinute,int(sSecond))
            # offset to begin of day in hours
            sOffset=(settingTime-day).total_seconds()/3600.0
        else:
            sOffset=None
        if rising!=None:
            rYear,rMonth,rDay,rHour,rMinute,rSecond=rising.tuple()
            risingTime=datetime.datetime(rYear,rMonth,rDay,rHour,rMinute,int(rSecond))
            rOffset=(risingTime-day).total_seconds()/3600.0
        else:
            rOffset=None
        res.append( (currentDay,sOffset,rOffset))
    return res
        
def createTableObject(observer,sunTable,object):
    """ create table of days, setting and rising times for given object
    
    This happens for the days given in the sun table. setting and rising times are
    considered only if during night, otherwise entry is None
    """
    
    res=list()
    for day,sunSet,sunRise in sunTable:
        # we are only interest in events after sunSet
        observer.date=sunSet
        nextSetting=observer.next_setting(object)
        nextRising=observer.next_rising(object)
        if nextSetting>sunRise:
            nextSetting=None
        if nextRising>sunRise:
            nextRising=None
        res.append((day,nextSetting,nextRising))   
    return res
    
          
def createTableSun(observer,year,dayStep=1):
    """ Create table with rising and setting times for the sun
    
    observer is an ephem observer, year is an int giving the desired year, dayStep int specifying the
    distance of days between computations.
    returned table contains pairs of setting/rising times for the night of each day of the year
    returned dates/times are ephem.date, i.e. UTC as float subclass.
    """
    object=ephem.Sun()
    # we start on Jan 1st 0am UTC.
    startDate=ephem.date((year,1,1,0,0) )
    # compute days in year, automatically considers leap years
    days=(datetime.date(year+1,1,1)-datetime.date(year,1,1)).days
    res=list()
    for day in range(0,days):
        currentDate=ephem.date(startDate+day)
        year,month,day,hour,minute,second=currentDate.tuple()
        observer.date=currentDate
        nextSetting=observer.next_setting(object)
        observer.date=nextSetting
        nextRising=observer.next_rising(object)
        res.append( (currentDate,nextSetting,nextRising) )
    return res

def readFits(fileName):
    """read fits file. Returns (header,data) of primary hdu
    """
    with afits.open(fileName) as hdulist:
        # print(hdulist.info())
        # print("********************")
        # for hdu in hdulist:
        #     print(repr(hdu.header))
        #     print("---------------")
        # if len(hdulist)==0:
        #     raise("No data in FITS file")
        priHdu=hdulist[0]
        priHeader=priHdu.header
        priData=priHdu.data
    return priHeader,priData

def parseArgs():
    """ Parse the command line
    :return result of argparse.ArgumentParser..parse_args()
    """
    cities=str([key for key in list(ephem.cities._city_data.keys())])
    parser = argparse.ArgumentParser(description='plot the visibilities of the main solar system objects.')
    parser.add_argument("-y","--year",type=int,default=datetime.date.today().year,
                        help="select year for ephemeris. Default is current year.")
    parser.add_argument("-c","--city",default="Munich",
                        help="select city, is overridden by -l. One of "+cities+". Default: Munich.")
    parser.add_argument("-u","--utc",default=1,type=float,
                        help="offset of local time to UTC time. Default 1.")
    parser.add_argument("-l","--loc",default=[],type=float,nargs=5,metavar="f",
                        help="define observer location. Format is: lat lon elevation temp pressure, "
                        "with f being lat=latitude (+N), lon=longitude (+E), elevation in meters, temperature in deg.C, "
                        "pressure in mBar respectively. Example for Munich: 48.15 11.5833333 508.7112 15 1000. Default: None.")
    parser.add_argument("-t","--title",default=None,
                        help="Title for plot. Default is \"Visibilities for city year\"")
    parser.add_argument("-f","--inFile",type=str,default=None,
                        help="input file. If given, used to determined the size of the resulting plot. The pixel contents "
                             "is ignored. If not given, do plot in standard size. Default=None")
    parser.add_argument("-o","--outFile",type=str,default=None,
                        help="output file. If given, plot is written to this file. Otherwise, a window opens to display the plot. "
                             "Default=None")
    parser.add_argument("-g","--isGlobal",default=False,action="store_true",
                        help="ignored.")

    # -h is implicit
    values=parser.parse_args()
    return values
    
def main():
    """evaluate args and do what is requested
    """
    # eval arguments
    parsedArgs=parseArgs()
    year=parsedArgs.year
    utcOffset=parsedArgs.utc
    bInteractive=(parsedArgs.outFile==None)
    if (len(parsedArgs.loc)==0):
        observer=ephem.city(parsedArgs.city)
    else:
        loc=parsedArgs.loc
        lon,lat,elevation,temp,pressure=loc
        observer=ephem.Observer()
        observer.long=lon/180.0*ephem.pi
        observer.lat=lat/180.0*ephem.pi
        observer.elevation=elevation
        observer.temp=temp
        observer.pressure=pressure
    if (parsedArgs.title==None):
        if(len(parsedArgs.loc)!=0):
            title="Visibilities for Lon=%f Lat=%f Year=%d"%(observer.long,observer.lat,year)
        else:
            title="Visibilities for %s %d"%(parsedArgs.city,parsedArgs.year)
    else:
        title=parsedArgs.title

    if(parsedArgs.inFile!=None):
        # get dimensions from read in image
        header,data=readFits(parsedArgs.inFile)
        imageDims=(int(header["NAXIS1"]),int(header["NAXIS2"]))
    else:
        # default size
        imageDims=(19200,12000)
        #imageDims=(1024,768)
    outFile=parsedArgs.outFile
    dayStep=1
    sunTable=createTableSun(observer,year,dayStep)
    # modify here to include the objects that are interesting for you
    # format is (object name, ephem data, color)
    objects=[("Mercury",ephem.Mercury(),"c"),
             ("Venus",ephem.Venus(),"y"),
             ("Moon",ephem.Moon(),"gray"),
             ("Mars",ephem.Mars(),"r"),
             ("Jupiter",ephem.Jupiter(),"g"),
             ("Saturn",ephem.Saturn(),"m")]
    objectsTable=list()
    for name,object,color in objects:
        table=createTableObject(observer,sunTable,object)
        objectsTable.append( (name,table,color) )
    plotSettingRising(title,utcOffset,year,observer,sunTable,objectsTable,outFile,imageDims)
    readFits(outFile)
    
if __name__=="__main__":
    main()
    