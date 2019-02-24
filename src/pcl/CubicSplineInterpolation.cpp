//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/CubicSplineInterpolation.cpp - Released 2019-01-21T12:06:21Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/CubicSplineInterpolation.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

void CubicSplineInterpolationBase::Generate( float* dy2, const float* fx, const float* fy, float dy1, float dyn, int n )
{
   if ( (*API->Numerical->CubicSplineGenerateF)( dy2, fx, fy, dy1, dyn, n ) == api_false )
      throw APIFunctionError( "CubicSplineGenerateF" );
}

void CubicSplineInterpolationBase::Generate( double* dy2, const double* fx, const double* fy, double dy1, double dyn, int n )
{
   if ( (*API->Numerical->CubicSplineGenerateD)( dy2, fx, fy, dy1, dyn, n ) == api_false )
      throw APIFunctionError( "CubicSplineGenerateD" );
}

void CubicSplineInterpolationBase::Generate( float* dy2, const float* fx, const float* fy, int n )
{
   if ( (*API->Numerical->NaturalCubicSplineGenerateF)( dy2, fx, fy, n ) == api_false )
      throw APIFunctionError( "NaturalCubicSplineGenerateF" );
}

void CubicSplineInterpolationBase::Generate( double* dy2, const double* fx, const double* fy, int n )
{
   if ( (*API->Numerical->NaturalCubicSplineGenerateD)( dy2, fx, fy, n ) == api_false )
      throw APIFunctionError( "NaturalCubicSplineGenerateD" );
}

float CubicSplineInterpolationBase::Interpolate( const float* fx, const float* fy, const float* dy2, int n, double x, int32& k )
{
   float y;
   if ( (*API->Numerical->CubicSplineInterpolateF)( &y, fx, fy, dy2, n, x, &k ) == api_false )
      throw APIFunctionError( "CubicSplineInterpolateF" );
   return y;
}

double CubicSplineInterpolationBase::Interpolate( const double* fx, const double* fy, const double* dy2, int n, double x, int32& k )
{
   double y;
   if ( (*API->Numerical->CubicSplineInterpolateD)( &y, fx, fy, dy2, n, x, &k ) == api_false )
      throw APIFunctionError( "CubicSplineInterpolateD" );
   return y;
}

void CubicSplineInterpolationBase::Generate( float* dy2, const float* fy, int n )
{
   if ( (*API->Numerical->NaturalGridCubicSplineGenerateF)( dy2, fy, n ) == api_false )
      throw APIFunctionError( "NaturalGridCubicSplineGenerateF" );
}

void CubicSplineInterpolationBase::Generate( double* dy2, const double* fy, int n )
{
   if ( (*API->Numerical->NaturalGridCubicSplineGenerateD)( dy2, fy, n ) == api_false )
      throw APIFunctionError( "NaturalGridCubicSplineGenerateD" );
}

void CubicSplineInterpolationBase::Generate( float* dy2, const uint8* fy, int n )
{
   if ( (*API->Numerical->NaturalGridCubicSplineGenerateUI8)( dy2, fy, n ) == api_false )
      throw APIFunctionError( "NaturalGridCubicSplineGenerateUI8" );
}

void CubicSplineInterpolationBase::Generate( float* dy2, const uint16* fy, int n )
{
   if ( (*API->Numerical->NaturalGridCubicSplineGenerateUI16)( dy2, fy, n ) == api_false )
      throw APIFunctionError( "NaturalGridCubicSplineGenerateUI16" );
}

void CubicSplineInterpolationBase::Generate( double* dy2, const uint32* fy, int n )
{
   if ( (*API->Numerical->NaturalGridCubicSplineGenerateUI32)( dy2, fy, n ) == api_false )
      throw APIFunctionError( "NaturalGridCubicSplineGenerateUI32" );
}

float CubicSplineInterpolationBase::Interpolate( const float* fy, const float* dy2, int n, double x )
{
   float y;
   if ( (*API->Numerical->NaturalGridCubicSplineInterpolateF)( &y, fy, dy2, n, x ) == api_false )
      throw APIFunctionError( "NaturalGridCubicSplineInterpolateF" );
   return y;
}

double CubicSplineInterpolationBase::Interpolate( const double* fy, const double* dy2, int n, double x )
{
   double y;
   if ( (*API->Numerical->NaturalGridCubicSplineInterpolateD)( &y, fy, dy2, n, x ) == api_false )
      throw APIFunctionError( "NaturalGridCubicSplineInterpolateD" );
   return y;
}

float CubicSplineInterpolationBase::Interpolate( const uint8* fy, const float* dy2, int n, double x )
{
   float y;
   if ( (*API->Numerical->NaturalGridCubicSplineInterpolateUI8)( &y, fy, dy2, n, x ) == api_false )
      throw APIFunctionError( "NaturalGridCubicSplineInterpolateUI8" );
   return y;
}

float CubicSplineInterpolationBase::Interpolate( const uint16* fy, const float* dy2, int n, double x )
{
   float y;
   if ( (*API->Numerical->NaturalGridCubicSplineInterpolateUI16)( &y, fy, dy2, n, x ) == api_false )
      throw APIFunctionError( "NaturalGridCubicSplineInterpolateUI16" );
   return y;
}

double CubicSplineInterpolationBase::Interpolate( const uint32* fy, const double* dy2, int n, double x )
{
   double y;
   if ( (*API->Numerical->NaturalGridCubicSplineInterpolateUI32)( &y, fy, dy2, n, x ) == api_false )
      throw APIFunctionError( "NaturalGridCubicSplineInterpolateUI32" );
   return y;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/CubicSplineInterpolation.cpp - Released 2019-01-21T12:06:21Z
