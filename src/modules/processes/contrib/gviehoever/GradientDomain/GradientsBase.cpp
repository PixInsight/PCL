//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0165
// ----------------------------------------------------------------------------
// GradientsBase.cpp - Released 2016/02/21 20:22:43 UTC
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

#include "GradientsBase.h"

#include <cmath>
//#include <string>

#ifdef USE_THREADS
#include <pcl/Thread.h>
#endif

#include <pcl/Console.h>
#include <pcl/ProcessInterface.h>

#include <pcl/File.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>

#include <pcl/MorphologicalTransformation.h>

#ifdef USE_FFTW
#include <fftw3.h>
#endif


#ifdef USE_PIFFT
#include "solver_dct3.h"
#endif

namespace pcl {

GradientsBase::GradientsBase()
{
}

GradientsBase::~GradientsBase()
{
}

//static
bool GradientsBase::loadFile(const String& rsFilePath_p, std::size_t index_p, imageType_t &rImage_p)
{

  // open read only
  FileFormat format( pcl::File::ExtractExtension( rsFilePath_p ), true, false );
  FileFormatInstance file(format);
  ImageDescriptionArray images;
  if ( !file.Open( images, rsFilePath_p)) //, inputHints ) )
    throw CatchedException();
  if(images.Length()<=index_p) {
    //image with this index does not exist
    return false;
  }
  TimeMessage startLoad("Loading image "+rsFilePath_p+" "+String(index_p));
  file.SelectImage(index_p);
  if (!file.ReadImage(rImage_p))
    throw CatchedException();
  return true;
}

//static
void
GradientsBase::AssertImage(imageType_t const &rImage_p)
{
  Assert(rImage_p.Height()>2);
  Assert(rImage_p.Width()>2);
  Assert(rImage_p.NumberOfChannels()==1);
}

void
GradientsBase::AssertColImage(imageType_t const &rImage_p)
{
  Assert(rImage_p.Height()>2);
  Assert(rImage_p.Width()>2);
  Assert(rImage_p.NumberOfChannels()>=1);
}

//static
void
GradientsBase::createDxImage(imageType_t const &rImage_p, imageType_t &rResultImage_p)
{
  AssertColImage(rImage_p);
  int const nRows=rImage_p.Height();
  int const nCols=rImage_p.Width();
  int const nChannels=rImage_p.NumberOfChannels();
  imageType_t::color_space colorSpace=rImage_p.ColorSpace();

  rResultImage_p.AllocateData(nCols-1,nRows,nChannels,colorSpace);
  for(int chan=0;chan<nChannels;++chan){
    for (int row=0;row<nRows;++row){
      for(int col=0;col<nCols-1;++col){
	rResultImage_p.Pixel(col,row,chan)=rImage_p.Pixel(col+1,row,chan)-rImage_p.Pixel(col,row,chan);
      }
    }
  }
}
//static
void
GradientsBase::createDyImage(imageType_t const &rImage_p, imageType_t &rResultImage_p)
{
  AssertColImage(rImage_p);
  int const nRows=rImage_p.Height();
  int const nCols=rImage_p.Width();
  int const nChannels=rImage_p.NumberOfChannels();
  imageType_t::color_space colorSpace=rImage_p.ColorSpace();

  rResultImage_p.AllocateData(nCols,nRows-1,nChannels,colorSpace);
  for(int chan=0;chan<nChannels;++chan){
    for (int row=0;row<nRows-1;++row){
      for(int col=0;col<nCols;++col){
	rResultImage_p.Pixel(col,row,chan)=rImage_p.Pixel(col,row+1,chan)-rImage_p.Pixel(col,row,chan);
      }
    }
  }
}


//static
void
GradientsBase::createLaplaceVonNeumannImage(imageType_t const & rDx_p, imageType_t const rDy_p,
					imageType_t &rResultImage_p)
{

  AssertColImage(rDx_p);
  AssertColImage(rDy_p);
  int const nRowsX=rDx_p.Height();
#ifdef DEBUG
  int const nColsX=rDx_p.Width();
  int const nRowsY=rDy_p.Height();
#endif
  int const nColsY=rDy_p.Width();
  int const nRowsRes=nRowsX;
  int const nColsRes=nColsY;
  int const nChannels=rDx_p.NumberOfChannels();
  imageType_t::color_space colorSpace=rDx_p.ColorSpace();

  Assert(nRowsX>0 && nColsX>0);
  Assert(nRowsY>0 && nColsY>0);
  Assert(nRowsY+1==nRowsX && nColsY==nColsX+1);
  Assert(nChannels==rDy_p.NumberOfChannels());

  //d2x, inner part
  rResultImage_p.AllocateData(nColsRes,nRowsRes,nChannels,colorSpace);
  for(int chan=0;chan<nChannels;++chan){
    for(int row=0;row<nRowsRes;++row){
      for(int col=1;col<nColsRes-1;++col){
	rResultImage_p.Pixel(col,row,chan)=rDx_p.Pixel(col,row,chan)-rDx_p.Pixel(col-1,row,chan);
      }
    }
    //d2x, first and last column
    for(int row=0;row<nRowsRes;++row){
      rResultImage_p.Pixel(0,row,chan)=rDx_p.Pixel(0,row,chan);
      rResultImage_p.Pixel(nColsRes-1,row,chan)=-rDx_p.Pixel(nColsRes-2,row,chan);
    }

    //d2y, inner part
    for(int row=1;row<nRowsRes-1;++row){
      for(int col=0;col<nColsRes;++col){
	rResultImage_p.Pixel(col,row,chan)+=rDy_p.Pixel(col,row,chan)-rDy_p.Pixel(col,row-1,chan);
      }
    }
    //d2y, first and last row
    for(int col=0;col<nColsRes;++col){
      rResultImage_p.Pixel(col,0,chan)+=rDy_p.Pixel(col,0,chan);
      rResultImage_p.Pixel(col,nRowsRes-1,chan)-=rDy_p.Pixel(col,nRowsRes-2,chan);
    }
  }
}

//static
void
GradientsBase::solveImage(imageType_t const &rLaplaceImage_p, imageType_t &rSolution_p)
{
  int const nRows=rLaplaceImage_p.Height();
  int const nCols=rLaplaceImage_p.Width();
  int const nChannels=rLaplaceImage_p.NumberOfChannels();
  imageType_t::color_space colorSpace=rLaplaceImage_p.ColorSpace();

#ifdef USE_FFTW
  // adapted from http://www.umiacs.umd.edu/~aagrawal/software.html,

  AssertColImage(rLaplaceImage_p);
  // just in case we accidentally change this, because code below believes in double...
  Assert(typeid(realType_t)==typeid(double));
  // check assumption of row major format
  Assert(rLaplaceImage_p.PixelAddress(0,0)+1==rLaplaceImage_p.PixelAddress(1,0));


  rSolution_p.AllocateData(nCols,nRows,nChannels,colorSpace);
  rSolution_p.ResetSelections();
  rSolution_p.Black();

#ifdef USE_THREADS
    // threaded version
    int const nElements=nRows*nCols;
    int const nThreads=Thread::NumberOfThreads(nElements);

    if(fftw_init_threads()==0){
      throw Error("Problem initilizing threads");
    }
    fftw_plan_with_nthreads(nThreads);
#endif

  for(int chan=0;chan<nChannels;++chan){
    TimeMessage startSolver(String("FFTW Solver, Channel ")+String(chan));

    // FIXME see if fttw_allocate gives us something...
    imageType_t fcos(nCols,nRows);

#if 0
    // During experiment, the higher optimization did not give us anything except for an additional delay. May change later.
    fftw_plan pForward= fftw_plan_r2r_2d(nRows, nCols, const_cast<double *>(rLaplaceImage_p.PixelData(chan)), fcos.PixelData(), FFTW_REDFT10, FFTW_REDFT10, FFTW_MEASURE);
    fftw_plan pInverse = fftw_plan_r2r_2d(nRows, nCols, fcos.PixelData(), rSolution_p.PixelData(chan), FFTW_REDFT01, FFTW_REDFT01, FFTW_ESTIMATE);
#else
    fftw_plan pForward= fftw_plan_r2r_2d(nRows, nCols, const_cast<double *>(rLaplaceImage_p.PixelData(chan)), fcos.PixelData(), FFTW_REDFT10, FFTW_REDFT10, FFTW_MEASURE);
    fftw_plan pInverse = fftw_plan_r2r_2d(nRows, nCols, fcos.PixelData(), rSolution_p.PixelData(chan), FFTW_REDFT01, FFTW_REDFT01, FFTW_ESTIMATE);
#endif

    // find DCT
    fftw_execute(pForward);

    realType_t const pi=pcl::Pi();

    for(int row = 0 ; row < nRows; ++row){
      for(int col = 0 ; col < nCols; ++col){
	fcos.Pixel(col,row) /= 2*cos(pi*col/( (double) nCols)) - 2 + 2*cos(pi*row/((double) nRows)) - 2;
      }
    }
    fcos.Pixel(0,0)=0.0;

    // Inverse DCT
    fftw_execute(pInverse);
    fftw_destroy_plan(pForward);
    fftw_destroy_plan(pInverse);
  }
#endif
#ifdef USE_PIFFT
  // use PI FFT based solver by Carlos Milovic F.
  rLaplaceImage_p.ResetSelections();
  rSolution_p.AllocateData(nCols,nRows,nChannels,colorSpace);
  rSolution_p.ResetSelections();
  // current solver handles only one channel per run.
  for(int chan=0;chan<nChannels;++chan){
    TimeMessage startSolver(String("PIFFT Solver, Channel ")+String(chan));
    imageType_t tmpImage(nCols,nRows);
    rLaplaceImage_p.SelectChannel(chan);
    tmpImage.Assign(rLaplaceImage_p);
    __SolvePoisson(tmpImage);
    rSolution_p.SelectChannel(chan);
    rSolution_p.Mov(tmpImage);
  }
#endif
}

  //static
  void GradientsBase::binarizeImage(imageType_t const & currentImage_p, realType_t dBlackPoint_p,
					   weightImageType_t & rMaskImage_p)
  {
#if 0
    imageType_t image(currentImage_p);
    image.ResetSelections();
    image.Binarize(dBlackPoint_p);
    // first channel
    image.ResetSelections();
    image.SelectChannel(0);
    rMaskImage_p.ResetSelections();
    rMaskImage_p.Assign(image);
    // compress into one channel
    for(int i=1;i<image.NumberOfChannels();++i){
      image.SelectChannel(i);
      rMaskImage_p.Max(image);
    }
#else
    int const nCols=currentImage_p.Width();
    int const nRows=currentImage_p.Height();
    int const nChannels=currentImage_p.NumberOfChannels();

    rMaskImage_p.AllocateData(nCols,nRows);
    rMaskImage_p.Black();
    for(int channel=0;channel<nChannels;++channel){
      for(int row=0;row<nRows;++row){
	for(int col=0;col<nCols;++col){
	  if(currentImage_p.Pixel(col,row,channel)>dBlackPoint_p) {
	    rMaskImage_p.Pixel(col,row)=1.0;
	  }
	}
      }
    }
#endif
  }

  //static
  void GradientsBase::erodeMask(weightImageType_t & rMask_p,int32 shrinkCount_p)
  {
    if(shrinkCount_p<=0){
      // nothing to do
      return;
    }
    ErosionFilter erosionFilter;
    // FIXME maybe better use circular or diamond structure? Anyway, it should be the same structure as used for convolution during feathering
    BoxStructure sK(1+2*shrinkCount_p);
    MorphologicalTransformation mt(erosionFilter,sK);

    mt >> rMask_p;
  }

#ifdef __PCL_WINDOWS

#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64

int gettimeofday( timeval* tv, void*/*not_used*/ )
{
   if ( tv != 0 )
   {
      FILETIME ft;
      GetSystemTimeAsFileTime( &ft );

      uint64 t = ft.dwHighDateTime;
      t <<= 32;
      t |= ft.dwLowDateTime;

      // convert file time to unix epoch
      t -= DELTA_EPOCH_IN_MICROSECS;
      t /= 10;  /*convert into microseconds*/
      tv->tv_sec = (long)(t / 1000000UL);
      tv->tv_usec = (long)(t % 1000000UL);
   }

   return 0;
}

#endif   // __PCL_WINDOWS

GradientsBase::TimeMessage::TimeMessage(String const &rsTitle_p)
  :m_sMessage(rsTitle_p)
  //,m_startTime(boost::posix_time::microsec_clock::universal_time())
  ,m_bStopped(false)
{
  gettimeofday( &m_startTv, 0 );
  Console console;
  console.WriteLn(String("Start: ")+m_sMessage);
  ProcessInterface::ProcessEvents ();
}

void GradientsBase::TimeMessage::Stop()
{
   /*
    * Fixed! :)
    *
    * ... but gettimeofday() is not an optimal solution. For accurate timings,
    * we should use platform-specific resources such as clock_gettime() on UNIX
    * or performance monitors on Windows. The finest grained counter on Intel
    * processors is the RDTSC instruction, which is the most accurate solution.
    * However, is this really necessary, or just an approximate time is all you
    * need here?
    */
  m_bStopped = true;
  timeval endTv;
  gettimeofday( &endTv, 0 );
  double seconds = (endTv.tv_sec - m_startTv.tv_sec) +
                   (endTv.tv_usec - m_startTv.tv_usec)/1000000.0;
  double minutes, hours;
  Split( seconds/60, minutes, seconds ); seconds *= 60;
  Split( minutes/60, hours, minutes ); minutes *= 60;
  Console().WriteLn( "Stop: "
                   + m_sMessage
                   + ". Time used: "
                   + String().Format( "%2d:%02d:%06.3f", int( hours ), int( minutes ), seconds ) );
  /*
  // FIXME I cant believe that this riduculous conversion is necessary...
  std::string const timeString(boost::posix_time::to_simple_string(boost::posix_time::microsec_clock::universal_time()-m_startTime));
  console.WriteLn(String("Finishing ")+m_sMessage+String(". Time used (h.m.s) ")+
		  String(timeString.c_str()));
   */
  ProcessInterface::ProcessEvents ();
}


GradientsBase::TimeMessage::~TimeMessage()
{
  if(!m_bStopped){
    Stop();
  }
}


};

// ----------------------------------------------------------------------------
// EOF GradientsBase.cpp - Released 2016/02/21 20:22:43 UTC
