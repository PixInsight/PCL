//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// pcl/MultiscaleMedianTransform.cpp - Released 2018-11-01T11:06:51Z
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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
#include <pcl/Exception.h>
#include <pcl/GaussianFilter.h>
#include <pcl/MorphologicalTransformation.h>
#include <pcl/MultiscaleMedianTransform.h>
#include <pcl/PixelInterpolation.h>
#include <pcl/Resample.h>
#include <pcl/SeparableConvolution.h>

#define MAX_STRUCTURE_SIZE 11

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * 3x3 structuring element
 */
static const char* B03[] = {  "xxx"
                              "xxx"
                              "xxx",
                              //
                              "-x-"
                              "xxx"
                              "-x-" };
/*
 * 5x5 structuring element
 */
static const char* B05[] = {  "-xxx-"
                              "xxxxx"
                              "xxxxx"
                              "xxxxx"
                              "-xxx-",
                              //
                              "--x--"
                              "-xxx-"
                              "xxxxx"
                              "-xxx-"
                              "--x--" };
/*
 * 7x7 structuring element
 */
static const char* B07[] = {  "--xxx--"
                              "-xxxxx-"
                              "xxxxxxx"
                              "xxxxxxx"
                              "xxxxxxx"
                              "-xxxxx-"
                              "--xxx--",
                              //
                              "---x---"
                              "-xxxxx-"
                              "-xxxxx-"
                              "xxxxxxx"
                              "-xxxxx-"
                              "-xxxxx-"
                              "---x---" };
/*
 * 9x9 structuring element
 */
static const char* B09[] = {  "--xxxxx--"
                              "-xxxxxxx-"
                              "xxxxxxxxx"
                              "xxxxxxxxx"
                              "xxxxxxxxx"
                              "xxxxxxxxx"
                              "xxxxxxxxx"
                              "-xxxxxxx-"
                              "--xxxxx--",
                              //
                              "----x----"
                              "--xxxxx--"
                              "-xxxxxxx-"
                              "-xxxxxxx-"
                              "xxxxxxxxx"
                              "-xxxxxxx-"
                              "-xxxxxxx-"
                              "--xxxxx--"
                              "----x----" };
/*
 * 11x11 structuring element
 */
static const char* B11[] = {  "---xxxxx---"
                              "--xxxxxxx--"
                              "-xxxxxxxxx-"
                              "xxxxxxxxxxx"
                              "xxxxxxxxxxx"
                              "xxxxxxxxxxx"
                              "xxxxxxxxxxx"
                              "xxxxxxxxxxx"
                              "-xxxxxxxxx-"
                              "--xxxxxxx--"
                              "---xxxxx---",
                              //
                              "-----x-----"
                              "---xxxxx---"
                              "--xxxxxxx--"
                              "-xxxxxxxxx-"
                              "-xxxxxxxxx-"
                              "xxxxxxxxxxx"
                              "-xxxxxxxxx-"
                              "-xxxxxxxxx-"
                              "--xxxxxxx--"
                              "---xxxxx---"
                              "-----x-----" };

// ----------------------------------------------------------------------------

static BitmapStructure* MWS03 = nullptr;
static BitmapStructure* MWS05 = nullptr;
static BitmapStructure* MWS07 = nullptr;
static BitmapStructure* MWS09 = nullptr;
static BitmapStructure* MWS11 = nullptr;

static BitmapStructure* SWS03 = nullptr;
static BitmapStructure* SWS05 = nullptr;
static BitmapStructure* SWS07 = nullptr;
static BitmapStructure* SWS09 = nullptr;
static BitmapStructure* SWS11 = nullptr;

static void InitializeStructures()
{
   static Mutex     mutex;
   static AtomicInt initialized;

   if ( !initialized )
   {
      volatile AutoLock lock( mutex );
      if ( initialized.Load() == 0 )
      {
         MWS03 = new BitmapStructure( B03,  3, 2 );
         MWS05 = new BitmapStructure( B05,  5, 2 );
         MWS07 = new BitmapStructure( B07,  7, 2 );
         MWS09 = new BitmapStructure( B09,  9, 2 );
         MWS11 = new BitmapStructure( B11, 11, 2 );

         SWS03 = new BitmapStructure( B03,  3, 1 );
         SWS05 = new BitmapStructure( B05,  5, 1 );
         SWS07 = new BitmapStructure( B07,  7, 1 );
         SWS09 = new BitmapStructure( B09,  9, 1 );
         SWS11 = new BitmapStructure( B11, 11, 1 );

         initialized.Store( 1 );
      }
   }
}

// ----------------------------------------------------------------------------

/*
 * Transform (decomposition)
 */

class MMTDecomposition
{
public:

   template <class P> static
   void Apply( const GenericImage<P>& image, MultiscaleMedianTransform& T )
   {
      InitializeStructures();

      bool statusInitialized = false;
      StatusMonitor& status = (StatusMonitor&)image.Status();
      try
      {
         if ( status.IsInitializationEnabled() )
         {
            status.Initialize( String( T.m_medianWaveletTransform ? "Median-wavelet" : "Multiscale median" ) + " transform",
                               image.NumberOfSelectedSamples()*T.m_numberOfLayers*(T.m_medianWaveletTransform ? 2 : 1) );
            status.DisableInitialization();
            statusInitialized = true;
         }

         GenericImage<P> cj0( image );
         cj0.Status().Clear();

         for ( int j = 1, j0 = 0; ; ++j, ++j0 )
         {
            GenericImage<P> cj( cj0 );
            cj.Status() = status;

            MedianFilterLayer( cj, T.FilterSize( j0 ), T.m_multiwayStructures, T.m_parallel, T.m_maxProcessors );

            if ( T.m_medianWaveletTransform )
            {
               GenericImage<P> w0( cj0 );
               GenericImage<P> d0( cj0 );
               d0 -= cj;
               for ( int c = 0; c < d0.NumberOfChannels(); ++c )
               {
                  w0.SelectChannel( c );
                  d0.SelectChannel( c );
                  cj.SelectChannel( c );
                  double t = T.m_medianWaveletThreshold*d0.MAD( d0.Median() )/0.6745;
                  for ( typename GenericImage<P>::sample_iterator iw( w0 ), id( d0 ), ic( cj ); iw; ++iw, ++id, ++ic )
                     if ( Abs( *id ) > t )
                        *iw = *ic;
               }
               w0.ResetSelections();
               cj.ResetSelections();
               w0.Status() = cj.Status();
               LinearFilterLayer( w0, T.FilterSize( j0 ), T.m_parallel, T.m_maxProcessors );
               cj = w0;
            }

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
   void MedianFilterLayer( GenericImage<P>& cj, int n, bool multiway, bool parallel, int maxProcessors )
   {
      if ( n <= 11 )
      {
         const BitmapStructure* B;
         if ( multiway )
            switch ( n )
            {
            default:
            case 3:
               B = MWS03;
               break;
            case 5:
               B = MWS05;
               break;
            case 7:
               B = MWS07;
               break;
            case 9:
               B = MWS09;
               break;
            case 11:
               B = MWS11;
               break;
            }
         else
            switch ( n )
            {
            default:
            case 3:
               B = SWS03;
               break;
            case 5:
               B = SWS05;
               break;
            case 7:
               B = SWS07;
               break;
            case 9:
               B = SWS09;
               break;
            case 11:
               B = SWS11;
               break;
            }
         MorphologicalTransformation M( MedianFilter(), *B );
         M.EnableParallelProcessing( parallel, maxProcessors );
         M >> cj;
      }
#if MAX_STRUCTURE_SIZE > 11
      else if ( n <= MAX_STRUCTURE_SIZE )
      {
         CircularStructure C( n );
         MorphologicalTransformation M( MedianFilter(), C );
         M.EnableParallelProcessing( parallel, maxProcessors );
         M >> cj;
      }
#endif
      else
      {
         int w0 = cj.Width();
         int h0 = cj.Height();
         size_type N = cj.NumberOfSamples();

         // Don't alter cj's monitor during resampling
         StatusMonitor status = cj.Status();
         cj.Status().Clear();
         {
            double r = double( n )/(MAX_STRUCTURE_SIZE - 2);
            int fr = TruncInt( 1.5*r ); // NB: fr >= 2
            PCL_CHECK( fr >= 2 )
            BicubicFilterPixelInterpolation I( fr, fr, MitchellNetravaliCubicFilter() );
            Resample R( I, 1/r );
            R.EnableParallelProcessing( parallel, maxProcessors );
            R >> cj;
         }
         N += status.Count();
         cj.Status() = status;

#if MAX_STRUCTURE_SIZE > 11
         CircularStructure C( MAX_STRUCTURE_SIZE - 2 );
         MorphologicalTransformation M( MedianFilter(), C );
#else
         MorphologicalTransformation M( MedianFilter(), *(multiway ? MWS09 : SWS09) );
#endif
         M.EnableParallelProcessing( parallel, maxProcessors );
         M >> cj;

         // Don't alter cj's monitor during resampling
         status = cj.Status();
         status += N - status.Count();
         cj.Status().Clear();
         {
            BicubicSplinePixelInterpolation I;
            Resample R( I );
            R.SetMode( ResizeMode::AbsolutePixels );
            R.SetSizes( w0, h0 );
            R.EnableParallelProcessing( parallel, maxProcessors );
            R >> cj;
         }
         cj.Status() = status;
      }
   }

   template <class P> static
   void LinearFilterLayer( GenericImage<P>& cj, int n, bool parallel, int maxProcessors )
   {
      SeparableFilter F = GaussianFilter( n ).AsSeparableFilter();
      SeparableConvolution C( F );
      C.EnableParallelProcessing( parallel, maxProcessors );
      C >> cj;
   }
};

#define TRANSFORM_ENTRY             \
   DestroyLayers();                 \
   if ( image.IsEmptySelection() )  \
      return;                       \

void MultiscaleMedianTransform::Transform( const Image& image )
{
   TRANSFORM_ENTRY
   MMTDecomposition::Apply( image, *this );
}

void MultiscaleMedianTransform::Transform( const DImage& image )
{
   TRANSFORM_ENTRY
   MMTDecomposition::Apply( image, *this );
}

void MultiscaleMedianTransform::Transform( const ComplexImage& image )
{
   TRANSFORM_ENTRY
   Image tmp( image );
   MMTDecomposition::Apply( tmp, *this );
}

void MultiscaleMedianTransform::Transform( const DComplexImage& image )
{
   TRANSFORM_ENTRY
   DImage tmp( image );
   MMTDecomposition::Apply( tmp, *this );
}

void MultiscaleMedianTransform::Transform( const UInt8Image& image )
{
   TRANSFORM_ENTRY
   Image tmp( image );
   MMTDecomposition::Apply( tmp, *this );
}

void MultiscaleMedianTransform::Transform( const UInt16Image& image )
{
   TRANSFORM_ENTRY
   Image tmp( image );
   MMTDecomposition::Apply( tmp, *this );
}

void MultiscaleMedianTransform::Transform( const UInt32Image& image )
{
   TRANSFORM_ENTRY
   DImage tmp( image );
   MMTDecomposition::Apply( tmp, *this );
}

#undef TRANSFORM_ENTRY

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/MultiscaleMedianTransform.cpp - Released 2018-11-01T11:06:51Z
