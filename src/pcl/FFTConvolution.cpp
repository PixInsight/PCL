//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/FFTConvolution.cpp - Released 2015/12/17 18:52:18 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/FFTConvolution.h>
#include <pcl/FourierTransform.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_FFTConvolutionEngine
{
public:

   template <class P>
   static void Apply( GenericImage<P>& image, const FFTConvolution& F )
   {
      if ( !F.m_h )
      {
         Rect r = image.SelectedRectangle();
         if ( F.m_filter )
            const_cast<FFTConvolution&>( F ).m_h = Initialize( *F.m_filter, r.Width(), r.Height(), F.IsParallelProcessingEnabled(), F.MaxProcessors() );
         else
            const_cast<FFTConvolution&>( F ).m_h = Initialize( F.m_image, r.Width(), r.Height(), F.IsParallelProcessingEnabled(), F.MaxProcessors() );
      }

      Convolve( image, *F.m_h, F.IsParallelProcessingEnabled(), F.MaxProcessors() );
   }

private:

   template <class P>
   static void Convolve( GenericImage<P>& image, const ComplexImage& psfFFT, bool parallel, int maxProcessors )
   {
      Rect r = image.SelectedRectangle();

      int ch0 = image.FirstSelectedChannel();
      int ch1 = image.LastSelectedChannel();

      int w = psfFFT.Width();  // already optimized FFT lengths
      int h = psfFFT.Height();

      int dw = (w - r.Width()) >> 1;
      int dh = (h - r.Height()) >> 1;

      ComplexImage C( w, h );
      C.Zero();

      /*
       * In order to make FFTConvolution a perfect replacement for Convolution
       * and SeparableConvolution, we force it to perform the same status
       * monitoring increments, i.e., image.NumberOfSelectedSamples().
       */
      size_type N0  = size_type( w + h ) << 1;
      size_type N   = image.NumberOfSelectedPixels();
      size_type dN  = (N - N0) >> 2;
      size_type dN1 = N - 3*dN;

      bool statusInitialized = false;
      if ( image.Status().IsInitializationEnabled() )
      {
         image.Status().Initialize( "Convolution (FFT)", image.NumberOfSelectedSamples() );
         image.Status().DisableInitialization();
         statusInitialized = true;
      }

      try
      {
         for ( int ch = ch0; ch <= ch1; ++ch )
         {
            C.SetStatusCallback( 0 );
            C.Mov( image, Point( dw, dh ), 0, r, ch, ch );

            /*
             * Fix boundary artifacts by mirroring the image (assume Neumann
             * boundary condition).
             */
#define MIRROR_VERTICAL                                                       \
            {                                                                 \
               int i = 0;                                                     \
               ComplexImage::sample* cd = C.PixelAddress( 0, j );             \
               const ComplexImage::sample* cs = C.PixelAddress( dw << 1, y ); \
               for ( ; i < dw; ++i )                                          \
                  *cd++ = *cs--;                                              \
               cs = C.PixelAddress( i, y );                                   \
               for ( ; i < x1; ++i )                                          \
                  *cd++ = *cs++;                                              \
               cs = C.PixelAddress( x1-1, y );                                \
               for ( ; i < C.Width(); ++i )                                   \
                  *cd++ = *cs--;                                              \
            }
            int x1 = dw + r.Width();
            int y1 = dh + r.Height();
            int j = 0;
            for ( int y = dh << 1; j < dh; ++j, --y )
               MIRROR_VERTICAL
            for ( int y = dh; j < y1; ++j, ++y )
            {
               ComplexImage::sample* cd = C.PixelAddress( 0, j );
               const ComplexImage::sample* cs = C.PixelAddress( dw << 1, y );
               for ( int i = 0; i < dw; ++i )
                  *cd++ = *cs--;
               cd = C.PixelAddress( x1, j );
               cs = C.PixelAddress( x1-1, y );
               for ( int i = x1; i < C.Width(); ++i )
                  *cd++ = *cs--;
            }
            for ( int y = y1-1; j < C.Height(); ++j, --y )
               MIRROR_VERTICAL
#undef MIRROR_VERTICAL

            /*
             * Perform FFT convolution
             */
            C.Status() = image.Status();
            C.Status() += dN;

            InPlaceFourierTransform FFT;
            FFT.EnableParallelProcessing( parallel, maxProcessors );
            FFT >> C;                                 // w + h

            C.Status() += dN;

            double k = 1.0/w/h; // FFT scaling factor
                  ComplexImage::sample* c  = *C;
            const ComplexImage::sample* cN = c + C.NumberOfPixels();
            const ComplexImage::sample* p  = *psfFFT;
            for ( ; c < cN; ++c, ++p )
               *c *= k * *p;

            C.Status() += dN;

            InPlaceInverseFourierTransform IFFT;
            IFFT.EnableParallelProcessing( parallel, maxProcessors );
            IFFT >> C;    // w + h

            C.Status() += dN1;

            image.SetStatusCallback( 0 );
            image.Mov( C, Point( r.x0, r.y0 ), ch, Rect( dw, dh, r.Width()+dw, r.Height()+dh ) );

            image.Status() = C.Status();
         }

         if ( statusInitialized )
            image.Status().EnableInitialization();
      }
      catch ( ... )
      {
         if ( statusInitialized )
            image.Status().EnableInitialization();
         throw;
      }
   }

   static ComplexImage* Initialize( const KernelFilter& PSF,
                                    int targetWidth, int targetHeight, bool parallel, int maxProcessors )
   {
      PCL_CHECK( !PSF.IsEmpty() )

      int n = PSF.Size();
      if ( n == 0 )
         throw Error( "Attempt to perform an FFT-based convolution with an empty kernel filter." );

      int w = FFT2D::OptimizedLength( targetWidth + n );
      int h = FFT2D::OptimizedLength( targetHeight + n );

      ComplexImage* psfFFT = new ComplexImage( w, h );
      psfFFT->Zero();

      /*
       * Normalization factor: The sum of response function elements must be
       * equal to one.
       */
      double k = PSF.Weight();
      if ( 1 + k == 1 )
         k = 1;
      else
      {
         k = 1/k;
         if ( !IsFinite( k ) || 1 + k == 1 )
            return psfFFT;
      }

      /*
       * Normalize PSF and store it in wrap-around order.
       */
      int n2 = n >> 1;
      // 1st quadrant -> 3rd
      for ( int sy = 0, ty = h-n2; sy < n2; ++sy, ++ty )
         for ( int sx = n2, tx = 0; sx < n; ++sx, ++tx )
            (*psfFFT)( tx, ty ) = k*PSF[sy][sx];
      // 2nd quadrant -> 4th
      for ( int sy = 0, ty = h-n2; sy < n2; ++sy, ++ty )
         for ( int sx = 0, tx = w-n2; sx < n2; ++sx, ++tx )
            (*psfFFT)( tx, ty ) = k*PSF[sy][sx];
      // 3rd quadrant -> 1st
      for ( int sy = n2, ty = 0; sy < n; ++sy, ++ty )
         for ( int sx = 0, tx = w-n2; sx < n2; ++sx, ++tx )
            (*psfFFT)( tx, ty ) = k*PSF[sy][sx];
      // 4th quadrant -> 2nd
      for ( int sy = n2, ty = 0; sy < n; ++sy, ++ty )
         for ( int sx = n2, tx = 0; sx < n; ++sx, ++tx )
            (*psfFFT)( tx, ty ) = k*PSF[sy][sx];

      /*
       * Calculate the DFT of the wrap-around normalized PSF.
       */
      InPlaceFourierTransform FFT;
      FFT.EnableParallelProcessing( parallel, maxProcessors );
      FFT >> *psfFFT;

      return psfFFT;
   }

   template <class P>
   static double PSFNormalizationFactor( const GenericImage<P>& PSF )
   {
      double k = 0;
      for ( const typename P::sample* p = *PSF, * pN = p + PSF.NumberOfPixels(); p != pN; ++p )
         k += *p;
      if ( 1 + k == 1 )
         return 0;
      return 1/k;
   }

   static double PSFNormalizationFactor( const ComplexImage& PSF )
   {
      double k = 0;
      for ( const fcomplex* p = *PSF, * pN = p + PSF.NumberOfPixels(); p != pN; ++p )
         k += Abs( *p );
      if ( 1 + k == 1 )
         return 0;
      return 1/k;
   }

   static double PSFNormalizationFactor( const DComplexImage& PSF )
   {
      double k = 0;
      for ( const complex* p = *PSF, * pN = p + PSF.NumberOfPixels(); p != pN; ++p )
         k += Abs( *p );
      if ( 1 + k == 1 )
         return 0;
      return 1/k;
   }

   template <class P>
   static void GetWrapAroundNormalizedPSF( ComplexImage& psfFFT, const GenericImage<P>& PSF )
   {
      /*
       * Normalization factor: The sum of response function elements must be
       * equal to one.
       */
      double k = PSFNormalizationFactor( PSF );
      if ( !IsFinite( k ) || k == 0 )
         return;

      /*
       * Normalize PSF and store it in wrap-around order.
       */
      int w   = psfFFT.Width();
      int h   = psfFFT.Height();
      int nx  = PSF.Width();
      int ny  = PSF.Height();
      int nx2 = nx >> 1;
      int ny2 = ny >> 1;
      // 1st quadrant -> 3rd
      for ( int sy = 0, ty = h-ny2; sy < ny2; ++sy, ++ty )
         for ( int sx = nx2, tx = 0; sx < nx; ++sx, ++tx )
            psfFFT( tx, ty ) = k*PSF( sx, sy );
      // 2nd quadrant -> 4th
      for ( int sy = 0, ty = h-ny2; sy < ny2; ++sy, ++ty )
         for ( int sx = 0, tx = w-nx2; sx < nx2; ++sx, ++tx )
            psfFFT( tx, ty ) = k*PSF( sx, sy );
      // 3rd quadrant -> 1st
      for ( int sy = ny2, ty = 0; sy < ny; ++sy, ++ty )
         for ( int sx = 0, tx = w-nx2; sx < nx2; ++sx, ++tx )
            psfFFT( tx, ty ) = k*PSF( sx, sy );
      // 4th quadrant -> 2nd
      for ( int sy = ny2, ty = 0; sy < ny; ++sy, ++ty )
         for ( int sx = nx2, tx = 0; sx < nx; ++sx, ++tx )
            psfFFT( tx, ty ) = k*PSF( sx, sy );
   }

   static ComplexImage* Initialize( const ImageVariant& PSF,
                                    int targetWidth, int targetHeight, bool parallel, int maxProcessors )
   {
      if ( !PSF || PSF->IsEmpty() )
         throw Error( "Attempt to perform an FFT-based convolution with an empty response function image." );

      int w = FFT2D::OptimizedLength( targetWidth + PSF->Width() );
      int h = FFT2D::OptimizedLength( targetHeight + PSF->Height() );

      ComplexImage* psfFFT = new ComplexImage( w, h );
      psfFFT->Zero();

      /*
       * Get the PSF normalized and in wrap-around order.
       */
      if ( PSF.IsComplexSample() )
         switch ( PSF.BitsPerSample() )
         {
         case 32: GetWrapAroundNormalizedPSF( *psfFFT, static_cast<const ComplexImage&>( *PSF ) ); break;
         case 64: GetWrapAroundNormalizedPSF( *psfFFT, static_cast<const DComplexImage&>( *PSF ) ); break;
         }
      else if ( PSF.IsFloatSample() )
         switch ( PSF.BitsPerSample() )
         {
         case 32: GetWrapAroundNormalizedPSF( *psfFFT, static_cast<const Image&>( *PSF ) ); break;
         case 64: GetWrapAroundNormalizedPSF( *psfFFT, static_cast<const DImage&>( *PSF ) ); break;
         }
      else
         switch ( PSF.BitsPerSample() )
         {
         case  8: GetWrapAroundNormalizedPSF( *psfFFT, static_cast<const UInt8Image&>( *PSF ) ); break;
         case 16: GetWrapAroundNormalizedPSF( *psfFFT, static_cast<const UInt16Image&>( *PSF ) ); break;
         case 32: GetWrapAroundNormalizedPSF( *psfFFT, static_cast<const UInt32Image&>( *PSF ) ); break;
         }

      /*
       * Calculate the DFT of the wrapped-around normalized PSF.
       */
      InPlaceFourierTransform FFT;
      FFT.EnableParallelProcessing( parallel, maxProcessors );
      FFT >> *psfFFT;

      return psfFFT;
   }
};

// ----------------------------------------------------------------------------

void FFTConvolution::Apply( pcl::Image& image ) const
{
   PCL_PRECONDITION( m_filter || m_image )
   Validate();
   PCL_FFTConvolutionEngine::Apply( image, *this );
}

void FFTConvolution::Apply( pcl::DImage& image ) const
{
   PCL_PRECONDITION( m_filter || m_image )
   Validate();
   PCL_FFTConvolutionEngine::Apply( image, *this );
}

void FFTConvolution::Apply( pcl::UInt8Image& image ) const
{
   PCL_PRECONDITION( m_filter || m_image )
   Validate();
   PCL_FFTConvolutionEngine::Apply( image, *this );
}

void FFTConvolution::Apply( pcl::UInt16Image& image ) const
{
   PCL_PRECONDITION( m_filter || m_image )
   Validate();
   PCL_FFTConvolutionEngine::Apply( image, *this );
}

void FFTConvolution::Apply( pcl::UInt32Image& image ) const
{
   PCL_PRECONDITION( m_filter || m_image )
   Validate();
   PCL_FFTConvolutionEngine::Apply( image, *this );
}

void FFTConvolution::Apply( pcl::ComplexImage& image ) const
{
   PCL_PRECONDITION( m_filter || m_image )
   Validate();
   PCL_FFTConvolutionEngine::Apply( image, *this );
}

void FFTConvolution::Apply( pcl::DComplexImage& image ) const
{
   PCL_PRECONDITION( m_filter || m_image )
   Validate();
   PCL_FFTConvolutionEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

void FFTConvolution::Validate() const
{
   if ( !m_filter && !m_image )
      throw Error( "Invalid access to uninitialized FFT-based convolution." );
}

void FFTConvolution::ValidateFilter() const
{
   if ( !m_filter )
      throw Error( "Invalid access to nonexistent kernel filter in FFT-based convolution." );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/FFTConvolution.cpp - Released 2015/12/17 18:52:18 UTC
