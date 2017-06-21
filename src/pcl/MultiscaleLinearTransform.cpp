//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/MultiscaleLinearTransform.cpp - Released 2017-06-21T11:36:44Z
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
#include <pcl/AutoPointer.h>
#include <pcl/Exception.h>
#include <pcl/FFTConvolution.h>
#include <pcl/GaussianFilter.h>
#include <pcl/MeanFilter.h>
#include <pcl/MultiscaleLinearTransform.h>
#include <pcl/SeparableConvolution.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Transform (decomposition)
 */

class MLTDecomposition
{
public:

   template <class P> static
   void Apply( const GenericImage<P>& image, MultiscaleLinearTransform& T )
   {
      bool statusInitialized = false;
      StatusMonitor& status = (StatusMonitor&)image.Status();
      try
      {
         if ( status.IsInitializationEnabled() )
         {
            status.Initialize( "Multiscale linear transform", image.NumberOfSelectedSamples()*T.m_numberOfLayers );
            status.DisableInitialization();
            statusInitialized = true;
         }

         GenericImage<P> cj0( image );
         cj0.Status().Clear();

         for ( int j = 1, j0 = 0; ; ++j, ++j0 )
         {
            GenericImage<P> cj( cj0 );
            cj.Status() = status;

            LinearFilterLayer( cj, T.FilterSize( j0 ), T.m_useMeanFilters, T.m_parallel, T.m_maxProcessors );

            status = cj.Status();
            cj.Status().Clear();

            if ( T.m_layerEnabled[j0] )
            {
               cj0 -= cj;
               T.m_transform[j0] = Image( cj0 );
            }

            if ( j == T.m_numberOfLayers )
            {
               if ( T.m_layerEnabled[j] )
                  T.m_transform[j] = Image( cj );
               break;
            }

            cj0 = cj;
         }

         if ( statusInitialized )
            status.EnableInitialization();
      }
      catch ( ... )
      {
         T.DestroyLayers();
         if ( statusInitialized )
            status.EnableInitialization();
         throw;
      }
   }

private:

   template <class P> static
   void LinearFilterLayer( GenericImage<P>& cj, int n, bool meanFilter, bool parallel, int maxProcessors )
   {
      AutoPointer<KernelFilter> H( meanFilter ?
         static_cast<KernelFilter*>( new MeanFilter( n ) ) :
         static_cast<KernelFilter*>( new GaussianFilter( n ) ) );
      if ( n >= PCL_FFT_CONVOLUTION_IS_FASTER_THAN_SEPARABLE_FILTER_SIZE || cj.Width() < n || cj.Height() < n  )
      {
         FFTConvolution Z( *H );
         Z.EnableParallelProcessing( parallel, maxProcessors );
         Z >> cj;
      }
      else
      {
         SeparableFilter S( H->AsSeparableFilter() );
         SeparableConvolution C( S );
         C.EnableParallelProcessing( parallel, maxProcessors );
         C >> cj;
      }
   }
};

#define TRANSFORM_ENTRY             \
   DestroyLayers();                 \
   if ( image.IsEmptySelection() )  \
      return;                       \

void MultiscaleLinearTransform::Transform( const Image& image )
{
   TRANSFORM_ENTRY
   MLTDecomposition::Apply( image, *this );
}

void MultiscaleLinearTransform::Transform( const DImage& image )
{
   TRANSFORM_ENTRY
   MLTDecomposition::Apply( image, *this );
}

void MultiscaleLinearTransform::Transform( const ComplexImage& image )
{
   TRANSFORM_ENTRY
   Image tmp( image );
   MLTDecomposition::Apply( tmp, *this );
}

void MultiscaleLinearTransform::Transform( const DComplexImage& image )
{
   TRANSFORM_ENTRY
   DImage tmp( image );
   MLTDecomposition::Apply( tmp, *this );
}

void MultiscaleLinearTransform::Transform( const UInt8Image& image )
{
   TRANSFORM_ENTRY
   Image tmp( image );
   MLTDecomposition::Apply( tmp, *this );
}

void MultiscaleLinearTransform::Transform( const UInt16Image& image )
{
   TRANSFORM_ENTRY
   Image tmp( image );
   MLTDecomposition::Apply( tmp, *this );
}

void MultiscaleLinearTransform::Transform( const UInt32Image& image )
{
   TRANSFORM_ENTRY
   DImage tmp( image );
   MLTDecomposition::Apply( tmp, *this );
}

#undef TRANSFORM_ENTRY

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/MultiscaleLinearTransform.cpp - Released 2017-06-21T11:36:44Z
