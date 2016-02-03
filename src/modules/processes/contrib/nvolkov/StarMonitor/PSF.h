// ****************************************************************************
// PixInsight Class Library - PCL 01.00.96.0379
// Standard Image Process Module Version 01.02.01.0165
// ****************************************************************************
// PSF.h - Released 2011/09/06 11:04:17 UTC
// ****************************************************************************
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2016, Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __PSF_h
#define __PSF_h

#include <pcl/ImageVariant.h>
#include <pcl/Matrix.h>
#include <pcl/MetaParameter.h>   // pcl_enum, pcl_bool

namespace pcl {

// ----------------------------------------------------------------------------

struct PSFData {
   pcl_enum function; // PSF fitting function (PSFFit::Function)
   pcl_bool circular; // circular or elliptical PSF
   pcl_enum status; // error code (PSFFit::Status)
   double B; // local background
   double A; // amplitude
   DPoint c0; // centroid position
   double sx, sy; // sigma in pixels on X and Y axes, sx >= sy
   double theta; // rotation angle of the sx axis in the [0,180[ range
   double beta; // shape parameter
   double mad; // mean absolute deviation

   operator bool() const;

   String FunctionToString() const;
   String StatusToString() const;

   double FWHMx() const {
      return FWHM(sx);
   }

   double FWHMy() const {
      return FWHM(sy);
   }

   DRect Bounds() const;

   static double FWHM(double sigma);
};

class PSFFit {
public:

   enum Function {
      Gaussian,
   };

   enum Status {
      NotFitted,
      FittedOk,
      BadParameters,
      NoSolution,
      NoConvergence,
      InaccurateSolution,
      UnknownError
   };

   PSFData psf;

   PSFFit(const ImageVariant&, const DPoint&, const DRect&, Function, bool circular);

   PSFFit(const PSFFit& x) : psf(x.psf) {
   }

   PSFFit& operator =(const PSFFit& x) {
      psf = x.psf;
      return *this;
   }

   operator bool() const {
      return psf;
   }

private:

   Matrix S; // matrix of sampled data
   Vector P; // vector of function parameters
   mutable double beta0;

   double AbsoluteDeviation(Function, bool circular, double bestSoFar = 0) const;

   typedef int (*cminpack_callback)(void*, int, int, const double*, double*, int);

   // Levenberg-Marquadt - CMINPACK - Finite differences
   static int FitGaussian(void* p, int m, int n, const double* a, double* fvec, int iflag);
   static int FitCircularGaussian(void* p, int m, int n, const double* a, double* fvec, int iflag);
   static Status CminpackInfoToStatus(int info);
};

inline PSFData::operator bool() const {
   return status == PSFFit::FittedOk;
}

inline String PSFData::FunctionToString() const {
   switch (function) {
      case PSFFit::Gaussian: return "Gaussian";
      default: return "Unknown";
   }
}

inline String PSFData::StatusToString() const {
   switch (status) {
      case PSFFit::NotFitted: return "Not fitted";
      case PSFFit::FittedOk: return "Fitted Ok";
      case PSFFit::BadParameters: return "Bad parameters";
      case PSFFit::NoSolution: return "No solution";
      case PSFFit::NoConvergence: return "No convergence";
      case PSFFit::InaccurateSolution: return "Inaccurate solution";
      default:
      case PSFFit::UnknownError: return "Unknown error";
   }
}

inline DRect PSFData::Bounds() const {
   double d = (circular ? FWHMx() : Max(FWHMx(), FWHMy())) / 2;
   return DRect(c0.x - d, c0.y - d, c0.x + d, c0.y + d);
}

inline double PSFData::FWHM(double sigma) {
   return 2.3548200450309493 * sigma;
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PSF_h

// ****************************************************************************
// EOF PSF.h - Released 2011/09/06 11:04:17 UTC
