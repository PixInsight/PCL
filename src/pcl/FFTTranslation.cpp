//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/FFTTranslation.cpp - Released 2017-06-21T11:36:44Z
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

#include <pcl/FFTRegistration.h>
#include <pcl/FourierTransform.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_FFTTranslationEngine
{
public:

   template <class P>
   static ComplexImage Initialize( const GenericImage<P>& image, bool largeTranslations )
   {
      // We work with square matrices.
      Rect r = image.SelectedRectangle();
      int size = Max( r.Width(), r.Height() );

      // Double dimensions to allow for translations >= size/2.
      if ( largeTranslations )
         size <<= 1;

      return PCL_FFTTranslationEngine::Initialize( image, FFT2D::OptimizedLength( size ) );
   }

   template <class P>
   static void Evaluate( FPoint& delta, float& peak, const GenericImage<P>& image, const ComplexImage& C0 )
   {
      bool initializationEnabled = image.Status().IsInitializationEnabled();

      try
      {
         delta = 0.0F;

         int size = C0.Width(); // we work with square matrices

         if ( initializationEnabled )
         {
            image.Status().Initialize( "FFT Translation", 4*size + size*size );
            image.Status().DisableInitialization();
         }

         // FFT of the target image
         ComplexImage C = PCL_FFTTranslationEngine::Initialize( image, size ); //*** status += 2*size

         // Phase correlation matrix
         PhaseCorrelationMatrix( *C, *C + C.NumberOfPixels(), *C0, *C );

         // Inverse DFT of the PCM/CPSM
         InPlaceInverseFourierTransform() >> C;       //*** status += 2*size

         // Absolute value of PCM/CPSM
         Image R( C );

         C.FreeData(); // no longer needed

         // The position of the maximum matrix element gives us the displacement
         // of the target image w.r.t. the reference image.

         // Find the coordinates of the maximum element of the PCM.
         // Fetch the peak value before normalization.
         Point p;
         peak = R.LocateMaximumPixelValue( p );

         // Normalized real PCM or CPSM
         R.Rescale();                                 //*** status += size*size

         // We interpolate the coordinates of the maximum peak in the pase matrix
         // from a square neighborhood of nine pixels centered at the maximum
         // location.

         Point p0( (p.x > 0) ? p.x-1 : size-1, (p.y > 0) ? p.y-1 : size-1 );
         Point p1( (p.x < size-1) ? p.x+1 : 0, (p.y < size-1) ? p.y+1 : 0 );

         float f00 = 0.7071*R.Pixel( p0.x, p0.y );
         float f01 =        R.Pixel( p.x,  p0.y );
         float f02 = 0.7071*R.Pixel( p1.x, p0.y );

         float f10 =        R.Pixel( p0.x, p.y  );
      //float f11 =      0*R.Pixel( p.x,  p.y  ); // = 1 due to normalization of sample values
         float f12 =        R.Pixel( p1.x, p.y  );

         float f20 = 0.7071*R.Pixel( p0.x, p1.y );
         float f21 =        R.Pixel( p.x,  p1.y );
         float f22 = 0.7071*R.Pixel( p1.x, p1.y );

         image.Status() = R.Status();

         R.FreeData();  // R no longer needed

         // Interpolate horizontal and vertical shifts.
         delta.x = p.x - f00 - f10 - f20 + f02 + f12 + f22;
         delta.y = p.y - f00 - f01 - f02 + f20 + f21 + f22;

         // Obtain displacements in the correct directions.
         //
         // If an x or y coordinate is greater than the actual working space,
         // (size/2), then it corresponds to a negative displacement.
         //
         // Remember that for this to work with large displacements >= size/2 in
         // absolute value, the working size has to be doubled.

         if ( delta.x >= size/2 )
            delta.x -= size;
         if ( delta.y >= size/2 )
            delta.y -= size;

         delta.x = Round( delta.x, 2 );
         delta.y = Round( delta.y, 2 );

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
   static ComplexImage Initialize( const GenericImage<P>& image, int size )
   {
      Rect r = image.SelectedRectangle();
      int w = Min( r.Width(), size );
      int h = Min( r.Height(), size );

      ComplexImage C( size, size );

      // Place real data centered within our working space
      C.Zero().Mov( image, Point( (size - w) >> 1, (size - h) >> 1 ) );

      C.Status() = image.Status();

      bool initializationEnabled = image.Status().IsInitializationEnabled();
      if ( initializationEnabled )
      {
         C.Status().Initialize( "FFT Translation Reference Matrix", 2*size );
         C.Status().DisableInitialization();
      }

      InPlaceFourierTransform() >> C;   // status += 2*size

      image.Status() = C.Status();
      if ( initializationEnabled )
         image.Status().EnableInitialization();

      return C;
   }
};

// ----------------------------------------------------------------------------

ComplexImage FFTTranslation::DoInitialize( const pcl::Image& image )
{
   return PCL_FFTTranslationEngine::Initialize( image, m_largeTranslations );
}

ComplexImage FFTTranslation::DoInitialize( const pcl::DImage& image )
{
   return PCL_FFTTranslationEngine::Initialize( image, m_largeTranslations );
}

ComplexImage FFTTranslation::DoInitialize( const pcl::ComplexImage& image )
{
   return PCL_FFTTranslationEngine::Initialize( image, m_largeTranslations );
}

ComplexImage FFTTranslation::DoInitialize( const pcl::DComplexImage& image )
{
   return PCL_FFTTranslationEngine::Initialize( image, m_largeTranslations );
}

ComplexImage FFTTranslation::DoInitialize( const pcl::UInt8Image& image )
{
   return PCL_FFTTranslationEngine::Initialize( image, m_largeTranslations );
}

ComplexImage FFTTranslation::DoInitialize( const pcl::UInt16Image& image )
{
   return PCL_FFTTranslationEngine::Initialize( image, m_largeTranslations );
}

ComplexImage FFTTranslation::DoInitialize( const pcl::UInt32Image& image )
{
   return PCL_FFTTranslationEngine::Initialize( image, m_largeTranslations );
}

// ----------------------------------------------------------------------------

void FFTTranslation::DoEvaluate( const pcl::Image& image )
{
   PCL_FFTTranslationEngine::Evaluate( m_delta, m_peak, image, m_fftReference );
}

void FFTTranslation::DoEvaluate( const pcl::DImage& image )
{
   PCL_FFTTranslationEngine::Evaluate( m_delta, m_peak, image, m_fftReference );
}

void FFTTranslation::DoEvaluate( const pcl::ComplexImage& image )
{
   PCL_FFTTranslationEngine::Evaluate( m_delta, m_peak, image, m_fftReference );
}

void FFTTranslation::DoEvaluate( const pcl::DComplexImage& image )
{
   PCL_FFTTranslationEngine::Evaluate( m_delta, m_peak, image, m_fftReference );
}

void FFTTranslation::DoEvaluate( const pcl::UInt8Image& image )
{
   PCL_FFTTranslationEngine::Evaluate( m_delta, m_peak, image, m_fftReference );
}

void FFTTranslation::DoEvaluate( const pcl::UInt16Image& image )
{
   PCL_FFTTranslationEngine::Evaluate( m_delta, m_peak, image, m_fftReference );
}

void FFTTranslation::DoEvaluate( const pcl::UInt32Image& image )
{
   PCL_FFTTranslationEngine::Evaluate( m_delta, m_peak, image, m_fftReference );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FFTTranslation.cpp - Released 2017-06-21T11:36:44Z
