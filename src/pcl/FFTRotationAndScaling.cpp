//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// pcl/FFTRotationAndScaling.cpp - Released 2019-01-21T12:06:21Z
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

#include <pcl/FFTRegistration.h>
#include <pcl/FourierTransform.h>
#include <pcl/PhaseMatrices.h>
#include <pcl/PolarTransform.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_FFTRotationAndScalingEngine
{
public:

   template <class P>
   static ComplexImage Initialize( const GenericImage<P>& image, bool evaluateScaling, float lowFrequencyCutoff )
   {
      // We work with square matrices.
      return Initialize( image,
                         evaluateScaling,
                         FFT2D::OptimizedLength( Max( image.Width(), image.Height() ) ),
                         lowFrequencyCutoff );
   }

   template <class P>
   static void Evaluate( float& rotationAngle, float& scalingRatio,
                         const GenericImage<P>& image, bool evaluateScaling, float lowFrequencyCutoff,
                         const ComplexImage& C0 )
   {
      bool initializationEnabled = image.Status().IsInitializationEnabled();

      try
      {
         rotationAngle = 0;
         scalingRatio = 1;

         int size = C0.Width(); // we work with square matrices

         if ( initializationEnabled )
         {
            image.Status().Initialize( evaluateScaling ? "FFT Rotation/Scaling" : "FFT Rotation",
                                       7*size + 6*size*size );
            image.Status().DisableInitialization();
         }

         // FFT of the polar or log-polar target image
         ComplexImage C = Initialize( image, evaluateScaling, size, lowFrequencyCutoff ); //*** status += 4*size + 2*size*size

         // Phase correlation matrix
         PhaseCorrelationMatrix( *C, *C + C.NumberOfPixels(), *C0, *C );

         // Inverse FFT of phase matrix
         InPlaceInverseFourierTransform() >> C;                               //*** status += 2*size

         // Working real image to store the absolute value of PCM/CPSM.
         Image R( C );

         C.FreeData(); // no longer needed

         // Normalized real PCM or CPSM
         R.Rescale();                                                         //*** size*size

         // The position of the maximum matrix element gives us the displacement
         // of the target image w.r.t. the reference image.

         // Find the coordinates of the maximum element
         Point p;
         R.LocateMaximumPixelValue( p );

         // Extract the rotation angle from the phase matrix.
         // We interpolate from three adjacent elements on the initial column
         // of the maximum, using wrapped locations if necessary.

         // Get top and bottom interpolation elements (or their wrapped
         // counterparts)
         float y0 = R.Pixel( p.x, (p.y > 0) ? p.y-1 : size-1 );
         float y1 = R.Pixel( p.x, (p.y < size-1) ? p.y+1 : 0 );

         // Interpolate vertical position of maximum
         float y = p.y - y0 + y1;  // because R is normalized to [0,1]

         // The distribution of angle increments is linear on the vertical axis
         rotationAngle = float( Const<double>::pi()*y/size );

         // Keep rotation angle in the proper range and direction: +/- 90 degrees.
         if ( rotationAngle > Const<double>::pi2() )
            rotationAngle = Const<double>::pi() - rotationAngle;
         else if ( rotationAngle < Const<double>::pi2() )
            rotationAngle = -rotationAngle;

         rotationAngle = Round( rotationAngle, 3 );

         // Obtain the scaling ratio.
         // Again, we interpolate from three adjacent elements on the initial
         // row of the maximum, reading at wrapped locations if needed.

         if ( evaluateScaling )
         {
            // Get the left and right interpolation elements (or their wrapped
            // counterparts)
            float x0 = R.Pixel( (p.x > 0) ? p.x-1 : size-1, p.y );
            float x1 = R.Pixel( (p.x < size-1) ? p.x+1 : 0, p.y );

            // Interpolate horizontal position of maximum
            float x = p.x - x0 + x1;  // because R is normalized to [0,1]

            // Logarithmic size increment corresponding to one pixel on the
            // horizontal axis.
            // The logarithm base must be the same used in log-polar conversions.
            double dX = Exp( Ln( double( size ) )/size );

            // Scaling ratios on log-polar phase matrices are deduced as follows:
            //
            // Columns 0 and size-1 both correspond to 1:1 ratio.
            //
            // Ratios increase logarithmically from the leftmost column up to the
            // central column of the PCM.
            //
            // Ratios decrease logarithmically from the rightmost column down to
            // the central column of the PCM.

            if ( x >= size/2 )
               x -= size-1;

            scalingRatio = float( Round( Pow( dX, double( x ) ), 2 ) );
         }

         image.Status() = R.Status();

         if ( initializationEnabled )
            image.Status().EnableInitialization();
      }
      catch ( ... )
      {
         if ( initializationEnabled )
            image.Status().EnableInitialization();
         throw;
      }
   }

private:

   template <class P>
   static ComplexImage Initialize( const GenericImage<P>& image, bool evaluateScaling, int size, float lowFrequencyCutoff )
   {

      Rect r = image.SelectedRectangle();
      int w = Min( r.Width(), size );
      int h = Min( r.Height(), size );

      // Place real data centered within our working space
      ComplexImage C( size, size );
      C.Zero().Mov( image, Point( (size - w) >> 1, (size - h) >> 1 ) );

      C.Status() = image.Status();

      bool initializationEnabled = image.Status().IsInitializationEnabled();
      if ( initializationEnabled )
      {
         C.Status().Initialize( evaluateScaling ? "FFT Rotation/Scaling Reference Matrix" :
                                                   "FFT Rotation Reference Matrix",
                                 4*size + 2*size*size );
         C.Status().DisableInitialization();
      }

      // Multiply by (-1)^(x+y) to shift the origin of the DFT to the center of the image.
      // In this way the central pixel will be the dc component of the DFT.
      for ( int i = 0; i < size; ++i )
      {
         fcomplex* c = C.ScanLine( i ) + (i & 1);
         for ( int j = i & 1; j < size; j += 2, c += 2 )
            *c = -*c;
      }

      // DFT of the reference or target image.
      InPlaceFourierTransform() >> C;                       //*** status += 2*size

      // Absolute value of the DFT
      Image img1( C );

      C.FreeData();

      // Low-frequency cutoff
      if ( lowFrequencyCutoff > 0 )
         LowFrequencyCutoff( img1, lowFrequencyCutoff );

      // Normalize
      img1.Rescale();                                       //***  status += size*size

      // Conversion to polar or log-polar coordinates.
      {
         // Linear interpolation ensures no artifact will be generated due to
         // oscillations in presence of wild differences between neighbor pixels.
         BilinearPixelInterpolation L;

         if ( evaluateScaling )                             //***  status += size*size
         {
            LogPolarTransform LPT( L );
            LPT.SetRotationRange( .0F, Const<float>::pi() );
            LPT >> img1;
         }
         else
         {
            PolarTransform PT( L );
            PT.SetRotationRange( .0F, Const<float>::pi() );
            PT >> img1;
         }
      }

      // DFT of polar or log-polar image.
      C.Assign( img1 );
      img1.FreeData(); // img1 no longer needed
      InPlaceFourierTransform() >> C;                       //*** status += 2*size

      image.Status() = C.Status();
      if ( initializationEnabled )
         image.Status().EnableInitialization();

      return C;
   }

   /*
    * Low-frequency cutoff to reduce rotational aliasing.
    */
   static void LowFrequencyCutoff( Image& image, float cutoff )
   {
      int w = image.Width();
      int h = image.Height();

      float w2 = 0.5*w;
      float r2 = w2*w2*cutoff;

      Image::sample* f = *image;
      for ( int y = 0; y < h; ++y )
      {
         float dy = y - w2;
         float dy2 = dy*dy;

         for ( int x = 0; x < w; ++x, ++f )
         {
            float dx = x - w2;
            if ( dx*dx + dy2 < r2 )
               *f = 0;
         }
      }
   }
};

// ----------------------------------------------------------------------------

ComplexImage FFTRotationAndScaling::DoInitialize( const pcl::Image& image )
{
   return PCL_FFTRotationAndScalingEngine::Initialize( image, m_evaluateScaling, m_lowFrequencyCutoff );
}

ComplexImage FFTRotationAndScaling::DoInitialize( const pcl::DImage& image )
{
   return PCL_FFTRotationAndScalingEngine::Initialize( image, m_evaluateScaling, m_lowFrequencyCutoff );
}

ComplexImage FFTRotationAndScaling::DoInitialize( const pcl::ComplexImage& image )
{
   return PCL_FFTRotationAndScalingEngine::Initialize( image, m_evaluateScaling, m_lowFrequencyCutoff );
}

ComplexImage FFTRotationAndScaling::DoInitialize( const pcl::DComplexImage& image )
{
   return PCL_FFTRotationAndScalingEngine::Initialize( image, m_evaluateScaling, m_lowFrequencyCutoff );
}

ComplexImage FFTRotationAndScaling::DoInitialize( const pcl::UInt8Image& image )
{
   return PCL_FFTRotationAndScalingEngine::Initialize( image, m_evaluateScaling, m_lowFrequencyCutoff );
}

ComplexImage FFTRotationAndScaling::DoInitialize( const pcl::UInt16Image& image )
{
   return PCL_FFTRotationAndScalingEngine::Initialize( image, m_evaluateScaling, m_lowFrequencyCutoff );
}

ComplexImage FFTRotationAndScaling::DoInitialize( const pcl::UInt32Image& image )
{
   return PCL_FFTRotationAndScalingEngine::Initialize( image, m_evaluateScaling, m_lowFrequencyCutoff );
}

// ----------------------------------------------------------------------------

void FFTRotationAndScaling::DoEvaluate( const pcl::Image& image )
{
   PCL_FFTRotationAndScalingEngine::Evaluate( m_rotationAngle, m_scalingRatio,
                                              image, m_evaluateScaling, m_lowFrequencyCutoff, m_fftReference );
}

void FFTRotationAndScaling::DoEvaluate( const pcl::DImage& image )
{
   PCL_FFTRotationAndScalingEngine::Evaluate( m_rotationAngle, m_scalingRatio,
                                              image, m_evaluateScaling, m_lowFrequencyCutoff, m_fftReference );
}

void FFTRotationAndScaling::DoEvaluate( const pcl::ComplexImage& image )
{
   PCL_FFTRotationAndScalingEngine::Evaluate( m_rotationAngle, m_scalingRatio,
                                              image, m_evaluateScaling, m_lowFrequencyCutoff, m_fftReference );
}

void FFTRotationAndScaling::DoEvaluate( const pcl::DComplexImage& image )
{
   PCL_FFTRotationAndScalingEngine::Evaluate( m_rotationAngle, m_scalingRatio,
                                              image, m_evaluateScaling, m_lowFrequencyCutoff, m_fftReference );
}

void FFTRotationAndScaling::DoEvaluate( const pcl::UInt8Image& image )
{
   PCL_FFTRotationAndScalingEngine::Evaluate( m_rotationAngle, m_scalingRatio,
                                              image, m_evaluateScaling, m_lowFrequencyCutoff, m_fftReference );
}

void FFTRotationAndScaling::DoEvaluate( const pcl::UInt16Image& image )
{
   PCL_FFTRotationAndScalingEngine::Evaluate( m_rotationAngle, m_scalingRatio,
                                              image, m_evaluateScaling, m_lowFrequencyCutoff, m_fftReference );
}

void FFTRotationAndScaling::DoEvaluate( const pcl::UInt32Image& image )
{
   PCL_FFTRotationAndScalingEngine::Evaluate( m_rotationAngle, m_scalingRatio,
                                              image, m_evaluateScaling, m_lowFrequencyCutoff, m_fftReference );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FFTRotationAndScaling.cpp - Released 2019-01-21T12:06:21Z
