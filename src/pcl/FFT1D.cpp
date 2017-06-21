//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/FFT1D.cpp - Released 2017-06-21T11:36:44Z
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

#include <pcl/FFT1D.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

int FFT1DBase::OptimizedLength( int n, fcomplex* )
{
   return int( (*API->Numerical->FFTComplexOptimizedLengthF)( n ) );
}

int FFT1DBase::OptimizedLength( int n, dcomplex* )
{
   return int( (*API->Numerical->FFTComplexOptimizedLengthD)( n ) );
}

int FFT1DBase::OptimizedLength( int n, float* )
{
   return int( (*API->Numerical->FFTRealOptimizedLengthF)( n ) );
}

int FFT1DBase::OptimizedLength( int n, double* )
{
   return int( (*API->Numerical->FFTRealOptimizedLengthD)( n ) );
}

void* FFT1DBase::Create( int n, fcomplex* )
{
   void* handle = (*API->Numerical->FFTCreateComplexTransformF)( n );
   if ( handle == 0 )
      throw APIFunctionError( "FFTCreateComplexTransformF" );
   return handle;
}

void* FFT1DBase::Create( int n, dcomplex* )
{
   void* handle = (*API->Numerical->FFTCreateComplexTransformD)( n );
   if ( handle == 0 )
      throw APIFunctionError( "FFTCreateComplexTransformD" );
   return handle;
}

void* FFT1DBase::Create( int n, float* )
{
   void* handle = (*API->Numerical->FFTCreateRealTransformF)( n );
   if ( handle == 0 )
      throw APIFunctionError( "FFTCreateRealTransformF" );
   return handle;
}

void* FFT1DBase::Create( int n, double* )
{
   void* handle = (*API->Numerical->FFTCreateRealTransformD)( n );
   if ( handle == 0 )
      throw APIFunctionError( "FFTCreateRealTransformD" );
   return handle;
}

void* FFT1DBase::CreateInv( int n, fcomplex* )
{
   void* handle = (*API->Numerical->FFTCreateComplexInverseTransformF)( n );
   if ( handle == 0 )
      throw APIFunctionError( "FFTCreateComplexInverseTransformF" );
   return handle;
}

void* FFT1DBase::CreateInv( int n, dcomplex* )
{
   void* handle = (*API->Numerical->FFTCreateComplexInverseTransformD)( n );
   if ( handle == 0 )
      throw APIFunctionError( "FFTCreateComplexInverseTransformD" );
   return handle;
}

void* FFT1DBase::CreateInv( int n, float* )
{
   void* handle = (*API->Numerical->FFTCreateRealInverseTransformF)( n );
   if ( handle == 0 )
      throw APIFunctionError( "FFTCreateRealInverseTransformF" );
   return handle;
}

void* FFT1DBase::CreateInv( int n, double* )
{
   void* handle = (*API->Numerical->FFTCreateRealInverseTransformD)( n );
   if ( handle == 0 )
      throw APIFunctionError( "FFTCreateRealInverseTransformD" );
   return handle;
}

void FFT1DBase::Destroy( void* handle )
{
   if ( (*API->Numerical->FFTDestroyTransform)( handle ) == api_false )
      throw APIFunctionError( "FFTDestroyTransform" );
}

void FFT1DBase::Transform( void* handle, fcomplex* y, const fcomplex* x )
{
   if ( (*API->Numerical->FFTComplexTransformF)( handle, y, x ) == api_false )
      throw APIFunctionError( "FFTComplexTransformF" );
}

void FFT1DBase::Transform( void* handle, dcomplex* y, const dcomplex* x )
{
   if ( (*API->Numerical->FFTComplexTransformD)( handle, y, x ) == api_false )
      throw APIFunctionError( "FFTComplexTransformD" );
}

void FFT1DBase::Transform( void* handle, fcomplex* y, const float* x )
{
   if ( (*API->Numerical->FFTRealTransformF)( handle, y, x ) == api_false )
      throw APIFunctionError( "FFTRealTransformF" );
}

void FFT1DBase::Transform( void* handle, dcomplex* y, const double* x )
{
   if ( (*API->Numerical->FFTRealTransformD)( handle, y, x ) == api_false )
      throw APIFunctionError( "FFTRealTransformD" );
}

void FFT1DBase::Transform( void* handle, float* y, const fcomplex* x )
{
   if ( (*API->Numerical->FFTRealInverseTransformF)( handle, y, x ) == api_false )
      throw APIFunctionError( "FFTRealInverseTransformF" );
}

void FFT1DBase::Transform( void* handle, double* y, const dcomplex* x )
{
   if ( (*API->Numerical->FFTRealInverseTransformD)( handle, y, x ) == api_false )
      throw APIFunctionError( "FFTRealInverseTransformD" );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FFT1D.cpp - Released 2017-06-21T11:36:44Z
