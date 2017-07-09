//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// pcl/SurfaceSpline.cpp - Released 2017-07-09T18:07:16Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/SurfaceSpline.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

void SurfaceSplineBase::Generate( float* fx, float* fy, const float* fz, int n,
                                  int m, float r, const float* w, float* cv,
                                  double& rm, double& xm, double& ym )
{
   if ( (*API->Numerical->SurfaceSplineGenerateF)( cv, &rm, &xm, &ym, fx, fy, fz, n, m, r, w ) == api_false )
      throw APIFunctionError( "SurfaceSplineGenerateF" );
}

void SurfaceSplineBase::Generate( double* fx, double* fy, const double* fz, int n,
                                  int m, float r, const float* w, double* cv,
                                  double& rm, double& xm, double& ym )
{
   if ( (*API->Numerical->SurfaceSplineGenerateD)( cv, &rm, &xm, &ym, fx, fy, fz, n, m, r, w ) == api_false )
      throw APIFunctionError( "SurfaceSplineGenerateD" );
}

float SurfaceSplineBase::Interpolate( const float* fx, const float* fy, int n,
                                      const float* cv, int m, double x, double y )
{
   float z;
   if ( (*API->Numerical->SurfaceSplineInterpolateF)( &z, fx, fy, cv, n, m, x, y ) == api_false )
      throw APIFunctionError( "SurfaceSplineInterpolateF" );
   return z;
}

double SurfaceSplineBase::Interpolate( const double* fx, const double* fy, int n,
                                       const double* cv, int m, double x, double y )
{
   double z;
   if ( (*API->Numerical->SurfaceSplineInterpolateD)( &z, fx, fy, cv, n, m, x, y ) == api_false )
      throw APIFunctionError( "SurfaceSplineInterpolateD" );
   return z;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/SurfaceSpline.cpp - Released 2017-07-09T18:07:16Z
