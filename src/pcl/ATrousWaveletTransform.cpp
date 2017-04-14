//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/ATrousWaveletTransform.cpp - Released 2017-04-14T23:04:51Z
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

#include <pcl/ATrousWaveletTransform.h>
#include <pcl/Convolution.h>
#include <pcl/Exception.h>
#include <pcl/SeparableConvolution.h>
#include <pcl/Thread.h>

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * Transform (decomposition)
 */

class ATWTDecomposition
{
public:

   template <class P> static
   void Apply( const GenericImage<P>& image, ATrousWaveletTransform& T )
   {
      if ( T.m_scalingFunction.IsSeparable() )
      {
         SeparableConvolution C( *T.m_scalingFunction.separableFilter );
         Apply( image, T, C );
      }
      else
      {
         Convolution C( *T.m_scalingFunction.kernelFilter );
         Apply( image, T, C );
      }
   }

private:

   template <class P, class Cn> static
   void Apply( const GenericImage<P>& image, ATrousWaveletTransform& T, Cn& C )
   {
      bool statusInitialized = false;
      StatusMonitor& status = (StatusMonitor&)image.Status();

      try
      {
         C.EnableParallelProcessing( T.m_parallel, T.m_maxProcessors );

         if ( status.IsInitializationEnabled() )
         {
            status.Initialize( "Starlet transform", image.NumberOfSelectedSamples()*T.m_numberOfLayers );
            status.DisableInitialization();
            statusInitialized = true;
         }

         GenericImage<P> w0( image );
         w0.Status().Clear();

         for ( int j = 1, j0 = 0; ; ++j, ++j0 )
         {
            C.SetInterlacingDistance( (T.m_delta < 1) ? (1 << j0) : (1 + j0*T.m_delta) );

            GenericImage<P> wj( w0 );
            wj.Status() = status;
            C >> wj;

            status = wj.Status();
            wj.Status().Clear();

            if ( T.m_layerEnabled[j0] )
            {
               w0 -= wj;
               T.m_transform[j0] = Image( w0 );
            }

            if ( j == T.m_numberOfLayers )
            {
               if ( T.m_layerEnabled[j] )
                  T.m_transform[j] = Image( wj );
               break;
            }

            w0 = wj;
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
};

#define TRANSFORM_ENTRY                               \
   PCL_PRECONDITION( m_scalingFunction.IsValid() )    \
   ValidateScalingFunction();                         \
   DestroyLayers();                                   \
   if ( image.IsEmptySelection() )                    \
      return;

void ATrousWaveletTransform::Transform( const pcl::Image& image )
{
   TRANSFORM_ENTRY
   ATWTDecomposition::Apply( image, *this );
}

void ATrousWaveletTransform::Transform( const pcl::DImage& image )
{
   TRANSFORM_ENTRY
   ATWTDecomposition::Apply( image, *this );
}

void ATrousWaveletTransform::Transform( const pcl::ComplexImage& image )
{
   TRANSFORM_ENTRY
   Image tmp( image );
   ATWTDecomposition::Apply( tmp, *this );
}

void ATrousWaveletTransform::Transform( const pcl::DComplexImage& image )
{
   TRANSFORM_ENTRY
   DImage tmp( image );
   ATWTDecomposition::Apply( tmp, *this );
}

void ATrousWaveletTransform::Transform( const pcl::UInt8Image& image )
{
   TRANSFORM_ENTRY
   Image tmp( image );
   ATWTDecomposition::Apply( tmp, *this );
}

void ATrousWaveletTransform::Transform( const pcl::UInt16Image& image )
{
   TRANSFORM_ENTRY
   Image tmp( image );
   ATWTDecomposition::Apply( tmp, *this );
}

void ATrousWaveletTransform::Transform( const pcl::UInt32Image& image )
{
   TRANSFORM_ENTRY
   DImage tmp( image );
   ATWTDecomposition::Apply( tmp, *this );
}

#undef TRANSFORM_ENTRY

// ----------------------------------------------------------------------------

static double NoiseKSigmaEstimate( Array<float>& A, float K, float eps, int n, size_type* aN )
{
   bool isEps = eps > 0;
   double s0 = 0;

   for ( int it = 0; ; )
   {
      size_type N = A.Length();

      if ( aN != 0 )
         *aN = N;

      if ( N < 2 )
         return 0;

      double s = pcl::StdDev( A.Begin(), A.End() );
      if ( 1 + s == 1 )
         return 0;
      if ( ++it == n || isEps && it > 1 && (s0 - s)/s0 < eps )
         return s;

      s0 = s;

      double Ks = K*s;

      Array<float> B;
      Swap( A, B );
      A.Reserve( N );
      for ( float f : B )
         if ( Abs( f ) < Ks )
            A.Add( f );
   }
}

double ATrousWaveletTransform::NoiseKSigma( int j, float k, float eps, int n, size_type* N ) const
{
   ValidateLayerAccess( j );
   Array<float> A( m_transform[j].PixelData(), m_transform[j].PixelData() + m_transform[j].NumberOfPixels() );
   return NoiseKSigmaEstimate( A, k, eps, n, N );
}

template <class P>
static double NoiseKSigmaEstimate( const Image& wj, const GenericImage<P>& image,
                                   float low, float high, float k, float eps, int n, size_type* N )
{
   Array<float> A;
   const float* w = wj.PixelData();
   for ( typename GenericImage<P>::const_sample_iterator i( image ); i; ++i, ++w )
   {
      float v; P::FromSample( v, *i );
      if ( v > low && v < high )
         A.Add( *w );
   }

   return NoiseKSigmaEstimate( A, k, eps, n, N );
}

double ATrousWaveletTransform::NoiseKSigma( int j, const ImageVariant& image,
                                            float low, float high, float k, float eps, int n, size_type* N ) const
{
   ValidateLayerAccess( j );

   if ( !image )
      throw Error( "NoiseKSigma(): No image transported by ImageVariant." );

   if ( image->Width() != m_transform[j].Width() || image->Height() != m_transform[j].Height() )
      throw Error( "NoiseKSigma(): Incompatible image geometry." );

   if ( high < low )
      Swap( low, high );

   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: return NoiseKSigmaEstimate( m_transform[j], static_cast<const Image&>( *image ), low, high, k, eps, n, N );
      case 64: return NoiseKSigmaEstimate( m_transform[j], static_cast<const DImage&>( *image ), low, high, k, eps, n, N );
      }
   else if ( image.IsComplexSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: return NoiseKSigmaEstimate( m_transform[j], static_cast<const ComplexImage&>( *image ), low, high, k, eps, n, N );
      case 64: return NoiseKSigmaEstimate( m_transform[j], static_cast<const DComplexImage&>( *image ), low, high, k, eps, n, N );
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: return NoiseKSigmaEstimate( m_transform[j], static_cast<const UInt8Image&>( *image ), low, high, k, eps, n, N );
      case 16: return NoiseKSigmaEstimate( m_transform[j], static_cast<const UInt16Image&>( *image ), low, high, k, eps, n, N );
      case 32: return NoiseKSigmaEstimate( m_transform[j], static_cast<const UInt32Image&>( *image ), low, high, k, eps, n, N );
      }

   return 0; // ??!!
}

// ----------------------------------------------------------------------------

class PCL_NoiseMRSEngine
{
public:

   template <class P>
   static double NoiseEstimate( const ATrousWaveletTransform::transform& transform, int numberOfLayers,
                                const float sej[], const GenericImage<P>& image,
                                double sigma, float K, size_type* nn, float low, float high,
                                bool parallel, int maxProcessors )
   {
      bool statusInitialized = false;
      if ( image.Status().IsInitializationEnabled() )
      {
         image.Status().Initialize( "MRS noise evaluation" ); // unbounded monitor
         statusInitialized = true;
      }

      ThreadData<P> data( transform, numberOfLayers, image, low, high );

      size_type N = image.NumberOfSelectedPixels();
      if ( N < 9 )
      {
         if ( nn != 0 )
            *nn = 0;
         return 0;
      }

      if ( 1 + sigma == 1 )
      {
         sigma = pcl::StdDev( data.I, data.I+N );
         if ( 1 + sigma == 1 )
         {
            if ( nn != 0 )
               *nn = 0;
            return 0;
         }
      }

      ReferenceArray<Thread<P> > threads;

      int numberOfThreads = parallel ? Min( maxProcessors, pcl::Thread::NumberOfThreads( N, 16 ) ) : 1;
      size_type pixelsPerThread = N/numberOfThreads;

      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new Thread<P>( data, i*pixelsPerThread, (j < numberOfThreads) ? j*pixelsPerThread : N ) );

      Array<float> S( N );
      size_type n = 0;

      for ( int it = 0; ; ++it, ++image.Status() )
      {
         double Ks = K*sigma;
         for ( int j = 0; j < numberOfLayers; ++j )
            data.Ksj[j] = Ks * sej[j];

         AbstractImage::RunThreads( threads, data );

         n = 0;
         for ( int i = 0; i < numberOfThreads; ++i )
            if ( threads[i].n > 0 )
            {
               memcpy( S.At( n ), threads[i].S.Begin(), threads[i].n*sizeof( float ) );
               n += threads[i].n;
            }

         if ( n < 2 )
         {
            sigma = n = 0;
            break;
         }

         double newSigma = pcl::StdDev( S.Begin(), S.At( n ) );
         if ( 1 + newSigma == 1 )
         {
            sigma = n = 0;
            break;
         }

         double e = Abs( newSigma - sigma )/newSigma;
         sigma = newSigma;
         if ( e < 1e-4 )
            break;

         if ( it > 16 )
         {
            sigma = n = 0;
            break;
         }
      }

      threads.Destroy();

      if ( statusInitialized )
         image.Status().Complete();

      if ( nn != 0 )
         *nn = n;
      return sigma/0.974; // correct for 2% systematic bias
   }

private:

   template <class P>
   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( const ATrousWaveletTransform::transform& transform,
                  int numberOfLayers,
                  const GenericImage<P>& image, float low, float high ) :
      AbstractImage::ThreadData( image, 0 ) // unbounded monitoring
      {
         J = numberOfLayers;

         c = GenericVector<const float*>( J + 1 );
         for ( int j = 0; j <= J; ++j )
         {
            if ( !transform[j] )
               throw Error( "NoiseMRS(): Invalid wavelet transform." );
            c[j] = transform[j].PixelData();
         }

         Rect r = image.SelectedRectangle();
         if ( r.Width() != transform[0].Width() || r.Height() != transform[0].Height() )
            throw Error( "NoiseMRS(): Incompatible image geometry." );

         if ( r == image.Bounds() )
         {
            I = image[image.SelectedChannel()];
         }
         else
         {
            image.PushSelections();
            image.SelectChannel( image.SelectedChannel() );
            m_tmp.CopyImage( image ); // owns image
            image.PopSelections();
            I = static_cast<const GenericImage<P>&>( *m_tmp ).PixelData();
         }

         Ksj = Vector( J );

         m_low =  P::ToSample( low );
         m_high = P::ToSample( high );
      }

      int                         J;      // number of wavelet layers
      GenericVector<const float*> c;      // wavelet layers
      const typename P::sample*   I;      // pixel data
      Vector                      Ksj;    // K*sigma_j
      ImageVariant                m_tmp;  // working image for partial selections
      typename P::sample          m_low;  // clip too low pixels
      typename P::sample          m_high; // clip too high pixels
   };

   template <class P>
   class Thread : public pcl::Thread
   {
   public:

      Array<float> S;   // noise pixels
      size_type    n;   // number of noise pixels

      Thread( const ThreadData<P>& data, size_type start, size_type end ) :
      pcl::Thread(), S( end - start ), n( 0 ), m_data( data ), m_start( start ), m_end( end )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         n = 0;
         for ( size_type k = m_start; k < m_end; ++k )
         {
            typename P::sample v = m_data.I[k];
            if ( v > m_data.m_low && v < m_data.m_high )
            {
               bool isNoise = true;
               for ( int j = m_data.J; --j >= 0; )
                  if ( Abs( m_data.c[j][k] ) > m_data.Ksj[j] )
                  {
                     isNoise = false;
                     break;
                  }

               if ( isNoise )
                  S[n++] = v - m_data.c[m_data.J][k];
            }

            UPDATE_THREAD_MONITOR( 65536 )
         }
      }

   private:

      const ThreadData<P>& m_data;
      size_type            m_start;
      size_type            m_end;
   };
};

// ----------------------------------------------------------------------------

double ATrousWaveletTransform::NoiseMRS( const ImageVariant& image, const float sej[],
                                         double sigma, float K, size_type* N,
                                         float low, float high ) const
{
   if ( image.IsFloatSample() )
   {
      switch ( image.BitsPerSample() )
      {
      case 32 : return PCL_NoiseMRSEngine::NoiseEstimate( m_transform, m_numberOfLayers, sej,
                                                          static_cast<const Image&>( *image ), sigma, K, N, low, high,
                                                          IsParallelProcessingEnabled(), MaxProcessors() );

      case 64 : return PCL_NoiseMRSEngine::NoiseEstimate( m_transform, m_numberOfLayers, sej,
                                                          static_cast<const DImage&>( *image ), sigma, K, N, low, high,
                                                          IsParallelProcessingEnabled(), MaxProcessors() );
      default : return 0; // ?!
      }
   }
   else
   {
      ImageVariant tmp;
      tmp.CreateFloatImage();
      tmp.CopyImage( image );
      return NoiseMRS( tmp, sej, sigma, K, N );
   }
}

// ----------------------------------------------------------------------------

void ATrousWaveletTransform::ValidateScalingFunction() const
{
   if ( !m_scalingFunction.IsValid() )
      throw Error( "Invalid access to uninitialized wavelet transform." );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/ATrousWaveletTransform.cpp - Released 2017-04-14T23:04:51Z
