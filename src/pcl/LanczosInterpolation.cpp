//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// pcl/LanczosInterpolation.cpp - Released 2017-05-02T10:39:13Z
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

#include <pcl/AutoLock.h>
#include <pcl/LanczosInterpolation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Real Lanczos LUT data requires a maximum of about 190 KiB.
 * Integer Lanczos LUT data requires a maximum of 3 MiB.
 */
static double** PCL_Lanczos_LUT[]     = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
static float*   PCL_Lanczos_Int_LUT[] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

static void ValidateLanczosLUTOrder( int n )
{
   if ( n < 1 || n > 8 )
      throw Error( "Unsupported Lanczos LUT order. Must be 1 <= n <= 8." );
}

// ----------------------------------------------------------------------------

/*
 * Sinc function for x >= 0
 */
static double Sinc( double x )
{
   x *= Const<double>::pi();
   return (x > 1.0e-07) ? Sin( x )/x : 1.0;
}

/*
 * Evaluate Lanczos function of nth order at 0 <= x < n
 */
static double Lanczos( double x, int n )
{
   if ( x < 0 )
      x = -x;
   if ( x < n )
      return Sinc( x ) * Sinc( x/n );
   return 0;
}

// ----------------------------------------------------------------------------

/*
 * Thread-safe LUT initialization routines.
 */

const double** PCL_FUNC PCL_InitializeLanczosRealLUT( int n )
{
   ValidateLanczosLUTOrder( n );

   {
      static Mutex mutex;
      volatile AutoLock lock( mutex );

      double**& LUT = PCL_Lanczos_LUT[n-1];
      if ( LUT == nullptr )
      {
         LUT = new double*[ 2*n ];
         for ( int j = -n + 1, k = 0; j <= n; ++j, ++k )
         {
            LUT[k] = new double[ __PCL_LANCZOS_LUT_REAL_RESOLUTION ];
            for ( int i = 0; i < __PCL_LANCZOS_LUT_REAL_RESOLUTION; ++i )
               LUT[k][i] = Lanczos( j - double( i )/__PCL_LANCZOS_LUT_REAL_RESOLUTION, n );
         }
      }
      return const_cast<const double**>( reinterpret_cast<double**>( LUT ) );
   }
}

const float* PCL_FUNC PCL_InitializeLanczosIntLUT( int n )
{
   ValidateLanczosLUTOrder( n );

   {
      static Mutex mutex;
      volatile AutoLock lock( mutex );

      float*& LUT = PCL_Lanczos_Int_LUT[n-1];
      if ( LUT == nullptr )
      {
         LUT = new float[ n*__PCL_LANCZOS_LUT_INT_RESOLUTION + 1 ];
         for ( int i = 0, k = 0; i < n; ++i )
            for ( int j = 0; j < __PCL_LANCZOS_LUT_INT_RESOLUTION; ++j, ++k )
               LUT[k] = Lanczos( i + double( j )/__PCL_LANCZOS_LUT_INT_RESOLUTION, n );
         LUT[n*__PCL_LANCZOS_LUT_INT_RESOLUTION] = 0;
      }
      return reinterpret_cast<const float*>( LUT );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/LanczosInterpolation.cpp - Released 2017-05-02T10:39:13Z
