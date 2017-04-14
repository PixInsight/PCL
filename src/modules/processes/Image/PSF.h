//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0361
// ----------------------------------------------------------------------------
// PSF.h - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
//
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L. All Rights Reserved.
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
// ----------------------------------------------------------------------------

#ifndef __PSF_h
#define __PSF_h

#include <pcl/ImageVariant.h>
#include <pcl/Matrix.h>
#include <pcl/MetaParameter.h>   // pcl_enum, pcl_bool

namespace pcl
{

// ----------------------------------------------------------------------------

struct PSFData
{
   pcl_enum function;   // PSF fitting function (PSFFit::Function)
   pcl_bool circular;   // circular or elliptical PSF
   pcl_enum status;     // error code (PSFFit::Status)
   double   B;          // local background
   double   A;          // amplitude
   DPoint   c0;         // centroid position
   double   sx, sy;     // sigma in pixels on X and Y axes, sx >= sy
   double   theta;      // rotation angle of the sx axis in the [0,180[ range
   double   beta;       // shape parameter
   double   mad;        // mean absolute deviation

   operator bool() const;

   String FunctionToString() const;
   String StatusToString() const;

   double FWHMx() const
   {
      return FWHM( function, sx, beta );
   }

   double FWHMy() const
   {
      return FWHM( function, sy, beta );
   }

   DRect Bounds() const;

   void ToImage( Image& ) const;

   static double FWHM( int function, double sigma, double beta );
};

class PSFFit
{
public:

   enum Function
   {
      Gaussian,
      Moffat,
      MoffatA,
      Moffat8,
      Moffat6,
      Moffat4,
      Moffat25,
      Moffat15,
      Lorentzian
   };

   enum Status
   {
      NotFitted,
      FittedOk,
      BadParameters,
      NoSolution,
      NoConvergence,
      InaccurateSolution,
      UnknownError
   };

   PSFData psf;

   PSFFit( const ImageVariant&, const DPoint&, const DRect&, Function, bool circular );

   PSFFit( const PSFFit& x ) : psf( x.psf )
   {
   }

   PSFFit& operator =( const PSFFit& x )
   {
      psf = x.psf;
      return *this;
   }

   operator bool() const
   {
      return psf;
   }

private:

   Matrix S; // matrix of sampled data
   Vector P; // vector of function parameters
   mutable double beta0;

   double AbsoluteDeviation( Function, bool circular, double bestSoFar = 0 ) const;

   typedef int (*cminpack_callback)( void*, int, int, const double*, double*, int );

   // Levenberg-Marquadt - CMINPACK - Finite differences
   static int FitGaussian( void* p, int m, int n, const double* a, double* fvec, int iflag );
   static int FitCircularGaussian( void* p, int m, int n, const double* a, double* fvec, int iflag );
   static int FitMoffat( void* p, int m, int n, const double* a, double* fvec, int iflag );
   static int FitCircularMoffat( void* p, int m, int n, const double* a, double* fvec, int iflag );
   static int FitMoffatWithFixedBeta( void* p, int m, int n, const double* a, double* fvec, int iflag );
   static int FitCircularMoffatWithFixedBeta( void* p, int m, int n, const double* a, double* fvec, int iflag );
   static Status CminpackInfoToStatus( int info );
};

inline PSFData::operator bool() const
{
   return status == PSFFit::FittedOk;
}

inline String PSFData::FunctionToString() const
{
   switch ( function )
   {
   case PSFFit::Gaussian:   return "Gaussian";
   case PSFFit::Moffat:     return "Moffat";
   case PSFFit::MoffatA:    return "Moffat10";
   case PSFFit::Moffat8:    return "Moffat8";
   case PSFFit::Moffat6:    return "Moffat6";
   case PSFFit::Moffat4:    return "Moffat4";
   case PSFFit::Moffat25:   return "Moffat25";
   case PSFFit::Moffat15:   return "Moffat15";
   case PSFFit::Lorentzian: return "Lorentzian";
   default:                 return "Unknown";
   }
}

inline String PSFData::StatusToString() const
{
   switch ( status )
   {
   case PSFFit::NotFitted:          return "Not fitted";
   case PSFFit::FittedOk:           return "Fitted Ok";
   case PSFFit::BadParameters:      return "Bad parameters";
   case PSFFit::NoSolution:         return "No solution";
   case PSFFit::NoConvergence:      return "No convergence";
   case PSFFit::InaccurateSolution: return "Inaccurate solution";
   default:
   case PSFFit::UnknownError:       return "Unknown error";
   }
}

inline DRect PSFData::Bounds() const
{
   double d = (circular ? FWHMx() : Max( FWHMx(), FWHMy() ))/2;
   return DRect( c0.x-d, c0.y-d, c0.x+d, c0.y+d );
}

inline double PSFData::FWHM( int function, double sigma, double beta )
{
   switch ( function )
   {
   case PSFFit::Gaussian:   return 2.3548200450309493 * sigma;
   case PSFFit::Moffat:     return 2 * Sqrt( Pow2( 1/beta ) - 1 ) * sigma;
   case PSFFit::MoffatA:    return 0.5358113941912513 * sigma;
   case PSFFit::Moffat8:    return 0.6016900619596693 * sigma;
   case PSFFit::Moffat6:    return 0.6998915581984769 * sigma;
   case PSFFit::Moffat4:    return 0.8699588840921645 * sigma;
   case PSFFit::Moffat25:   return 1.1305006161394060 * sigma;
   case PSFFit::Moffat15:   return 1.5328418730817597 * sigma;
   case PSFFit::Lorentzian: return 2 * sigma;
   default:                 return 0;
   }
}

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PSF_h

// ----------------------------------------------------------------------------
// EOF PSF.h - Released 2017-04-14T23:07:12Z
