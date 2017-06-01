//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard GradientDomain Process Module Version 00.06.04.0184
// ----------------------------------------------------------------------------
// GradientsBase.h - Released 2017-05-02T09:43:01Z
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

#ifndef __GradientsBase_h
#define __GradientsBase_h

/// select solver based on fftw3
///
/// Mutually exclusive with USE_PIFFT
//#define USE_FFTW

/// define if you have a threading version of fftw.
///
/// Instead of libfftw3.so you need to link libfftw3_threads.so
//#define USE_THREADS

/// use FFT solver using PI internal functions
///
/// designed by Carlos Milovic F. .
/// Mutually exclusive with USE_FFTW
#define USE_PIFFT

//#include <boost/date_time/posix_time/posix_time.hpp> //header only
#ifdef __PCL_WINDOWS
#  include <time.h>
#else
#  include <sys/time.h>
#endif

#include <cfloat> //DBL_MAX
#include <pcl/Image.h>

#include "Assert.h"

namespace pcl {

/// implements base routines for gradient domain image manipulation.
///
/// Current solver is FFT based using von-Neumann boundary
/// conditions at the rectagular borders. This kind of solver
/// is fast, accurate and has decent memory consumptions. It is
/// useful for images up to 10k*10k pixels (beyond that., things get
/// real slow). Iterative solvers proved to be too slow or inaccurate.
///
/// Uses solver by Carlos Milovic F (see #define USE_PIFFT).
/// It is implemented in solver_dct3.h.
/// This one is slower and uses more memory than the FFTW solver, but it uses
/// PI internal routines only. Used with kind permission by Carlos.
///
/// As an alternative uses FFTW3 for FFT like ops, which currently is faster and more memory efficient
/// than the PI version (see #define USE_FFTW). FFTW3 http://www.fftw.org/ is free
/// for non-commercial use, and is delivered with Fedora 14. You
/// may need to install development headers ("yum install fftw-devel-3").
/// Package for Windows can be downloaded on FFTW web page.
///
/// Introduction to Gradient Domain image ops: Good overview in
/// http://www.umiacs.umd.edu/~aagrawal/ICCV2007Course/index.html
/// See ftp://ftp.umiacs.umd.edu/pub/aagrawal/ICCV07Course/AmitSection3.pdf, slide 18ff for solution methods.
///
/// See Gradient shop for ideas of how to combine  different components for useful effects:
/// http://grail.cs.washington.edu/projects/gradientshop/demos/gs_paper_TOG_2009.pdf
///
/// Developed and tested on Fedora 14. You should not have too much
/// trouble to get in running on other Linuxes or Windows.
class GradientsBase
{
 public:

  /// holds the matrix of image data in realType_t format
  ///
  /// Note: internal implementation assumes row-major format,
  /// and that image(x,y)=image(col,row)
  typedef DImage imageType_t;

  /// numeric type for images used here (double)
  typedef imageType_t::pixel_traits::sample realType_t;
  /// maximum value.
  //FIXME Should really be somewhere in the traits classses
  #define REALTYPE_T_MAX DBL_MAX


  /// holds "bitMask" for contributing. Its purpose is visualization of the contributing image regions.
  ///
  /// One possible code is this (concrete code see using routines):
  /// Value 0 is background, 1 is first image, 2 is second image, 4 image 3rd image etc..
  /// Combinations are the sum of these powers of 2. In the border region, images may be feathered
  /// and therefore get weights<1. In these regions, the sum is not unique for each combination.
  ///
  /// Codes may be per channel, or a single channel for the whole image
  // FIXME maybe change to FImage?
  typedef DImage sumMaskImageType_t;

  /// The weight of each pixel in an image.
  ///
  /// Usually 1.0 for each contributing pixels, ]0,1] for pixels in feathering region.
  /// -1.0 for pixels that belong the the border of the image.
  /// Is a single channel image.
  typedef DImage weightImageType_t;

  // type for storing the number of a contributing image
  typedef UInt16Image numImageType_t;


  /// constructor
  GradientsBase();
  /// destructor: releases internal data strucutures
  virtual ~GradientsBase();

 protected: // member functions

  /// assert NumberOfPixels>=4 and NumberOfChannels()==1
  ///
  /// single channel image
  static void AssertImage(imageType_t const &rImage_p);

  /// assert NumberOfPixels>=4 and NumberOfChannels()>=1
  ///
  /// color image
  static void AssertColImage(imageType_t const &rImage_p);

  /// compute gradient in x (col) direction
  ///
  /// Convention: The gradient values are located
  /// at the border between two image cells. For example,
  /// dx(3,5) is the gradient image(4,5)-image(3,5).
  /// The result therefore has one column less then the image.
  /// Supports multi channel images.
  static void createDxImage(imageType_t const &rImage_p, imageType_t &rResultImage_p);

  /// compute gradient in y (row) direction
  ///
  /// see createDxImage() for details
  static void createDyImage(imageType_t const &rImage_p, imageType_t &rResultImage_p);

  /// compute laplacien with von neumann boundary conditions
  ///
  /// The stencil, except for the border, id
  ///            1
  ///         1 -4 1
  ///            1
  /// Von Neumann boundary condition used here  means that
  /// the gradient continues to be the same at the borders,
  /// resulting in a laplacian of 0 in this direction.
  /// Size of result is one height(Dx_p) and width(rDy_p).
  /// Support multi channel images
  static void createLaplaceVonNeumannImage(imageType_t const & rDx_p, imageType_t const rDy_p,
					   imageType_t &rResultImage_p);


  /// solve for image that solves laplacian rLapaplaceImage_p with von Neumann conditions, using FFT
  ///
  /// Support multi channel images.
  /// @param rLaplaceImae_p laplacien to be solved.
  /// @param rSolution_p in: not used Out: solution
  static void solveImage(imageType_t const &rLaplaceImage_p, imageType_t &rSolution_p);

  /// load image with index_p from file. returns false if file with index does not exist
  static bool loadFile(const String& rsFilePath_p, std::size_t index_p, imageType_t &rImage_p);


  /// binarize image. If a pixel on all channels of currentImage_p are <dBlackPoint, rMaskImage pixel is 0.0, else 1.0
  static void binarizeImage(imageType_t const & currentImage_p, realType_t dBlackPoint_p, weightImageType_t & rMaskImage_p);

  /// performs erosion with suitably sized structure element
  ///
  /// @param shrinkCount_p number of pixels by which image region is reduced. Useful to get
  ///        rid of borders where image slowly merges into background, e.g. due to aliasing
  static void erodeMask(weightImageType_t & rMask_p,int32 shrinkCount_p);



  /// type for time message
  class TimeMessage
  {
  public:
    /// outputs start message
    TimeMessage(String const &rsTitle_p);
    /// outputs stop message with consumed time
    void Stop();
    /// if not already stopped: Stop, then destruct.
    ~TimeMessage();
  private:
    String const m_sMessage;
    timeval m_startTv;
    bool m_bStopped;
   };
};

};

#endif

// ----------------------------------------------------------------------------
// EOF GradientsBase.h - Released 2017-05-02T09:43:01Z
