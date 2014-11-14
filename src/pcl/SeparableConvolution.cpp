// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/SeparableConvolution.cpp - Released 2014/11/14 17:17:00 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#include <pcl/SeparableConvolution.h>
#include <pcl/Thread.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_SeparableConvolutionEngine
{
public:

   template <class P> static
   void Apply( GenericImage<P>& image, const SeparableConvolution& convolution )
   {
      if ( P::BitsPerSample() < 32 )
         ConvolveIntegerImage( image, convolution, reinterpret_cast<Image*>( 0 ) );
      else
         ConvolveIntegerImage( image, convolution, reinterpret_cast<DImage*>( 0 ) );
   }

   static void Apply( Image& image, const SeparableConvolution& convolution )
   {
      DoApply( image, convolution );
   }

   static void Apply( DImage& image, const SeparableConvolution& convolution )
   {
      DoApply( image, convolution );
   }

private:

   typedef SeparableFilter::coefficient         coefficient;
   typedef SeparableFilter::coefficient_vector  coefficient_vector;

   template <class P> static
   void DoApply( GenericImage<P>& image, const SeparableConvolution& convolution )
   {
      if ( image.IsEmptySelection() )
         return;

      if ( convolution.Filter().IsEmpty() )
         throw Error( "Attempt to perform a separable convolution with an empty filter." );

      image.SetUnique();

      int n = convolution.OverlappingDistance();
      if ( n > image.Height() || n > image.Width() )
      {
         image.Zero();
         return;
      }

      /*
       * In order to make SeparableConvolution a perfect replacement for
       * Convolution and FFTConvolution, we force it to perform the same status
       * monitoring increments, i.e., image.NumberOfSelectedSamples().
       */
      size_type N = image.NumberOfSelectedSamples();
      size_type N1 = N >> 1;
      size_type N2 = N - N1;
      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "Convolution (separable)", N );

      if ( convolution.IsRowConvolutionEnabled() )
      {
         ThreadData<P> data( image, convolution, N1 );

         int numberOfRows = image.SelectedRectangle().Height();
         int numberOfThreads = convolution.IsParallelProcessingEnabled() ?
                     Min( convolution.MaxProcessors(), Thread::NumberOfThreads( numberOfRows, 4 ) ) : 1;
         int rowsPerThread = numberOfRows/numberOfThreads;
         PArray<RowThread<P> > threads;
         for ( int i = 0, j = 1, y0 = image.SelectedRectangle().y0; i < numberOfThreads; ++i, ++j )
            threads.Add( new RowThread<P>( data,
                                           y0 + i*rowsPerThread,
                                           y0 + ((j < numberOfThreads) ? j*rowsPerThread : numberOfRows) ) );

         AbstractImage::RunThreads( threads, data );

         threads.Destroy();

         image.Status() = data.status;
      }

      if ( convolution.IsColumnConvolutionEnabled() )
      {
         ThreadData<P> data( image, convolution, N2 );

         int numberOfCols = image.SelectedRectangle().Width();
         int numberOfThreads = convolution.IsParallelProcessingEnabled() ?
                     Min( convolution.MaxProcessors(), Thread::NumberOfThreads( numberOfCols, 4 ) ) : 1;
         int colsPerThread = numberOfCols/numberOfThreads;
         PArray<ColThread<P> > threads;
         for ( int i = 0, j = 1, x0 = image.SelectedRectangle().x0; i < numberOfThreads; ++i, ++j )
            threads.Add( new ColThread<P>( data,
                                           x0 + i*colsPerThread,
                                           x0 + ((j < numberOfThreads) ? j*colsPerThread : numberOfCols) ) );

         AbstractImage::RunThreads( threads, data );

         threads.Destroy();

         image.Status() = data.status;
      }

      if ( convolution.IsRowConvolutionEnabled() && convolution.IsColumnConvolutionEnabled() )
      {
         StatusMonitor monitor = image.Status();
         image.SetStatusCallback( 0 );

         double k = convolution.FilterWeight();
         if ( k != 1 )
            image /= k;

         if ( !convolution.IsRawHighPassEnabled() )
            if ( convolution.IsHighPassFilter() )
               if ( convolution.IsHighPassRescalingEnabled() )
                  image.Normalize();
               else
                  image.Truncate();

         image.Status() = monitor;
      }
   }

   template <class P, class P1> static
   void ConvolveIntegerImage( GenericImage<P>& image, const SeparableConvolution& convolution, GenericImage<P1>* )
   {
      GenericImage<P1> tmp( image );
      DoApply( tmp, convolution );

      StatusMonitor monitor = tmp.Status();
      image.SetStatusCallback( 0 );

      image.Mov( tmp, image.SelectedRectangle().LeftTop() );

      image.Status() = monitor;
   }

   template <class P>
   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( GenericImage<P>& a_image, const SeparableConvolution& a_convolution, size_type a_count ) :
      AbstractImage::ThreadData( a_image, a_count ),
      image( a_image ), convolution( a_convolution )
      {
      }

            GenericImage<P>&      image;
      const SeparableConvolution& convolution;
   };

   template <class P>
   struct OneDimensionalConvolution
   {
      static
      void Convolve1D( typename P::sample* f, typename P::sample* t, int N, int d, int dn2, const coefficient_vector& H )
      {
         // dn2 = (n + (n - 1)*(d - 1)) >> 1;

         for ( int i = 0, j = dn2; i < dn2; ++i )
            t[i] = f[--j];
         ::memcpy( t+dn2, f, N*sizeof( *f ) );
         for ( int i = N+dn2+dn2, j = N-dn2; j < N; ++j )
            t[--i] = f[j];

         for ( int i = 0, n = H.Length(); i < N; ++i )
         {
            double r = 0;
            const typename P::sample* u = t++;
            for ( int j = 0; j < n; ++j, u += d )
               r += *u * H[j];
            *f++ = P::FloatToSample( r );
         }
      }
   };

   template <class P>
   class RowThread : public Thread, public OneDimensionalConvolution<P>
   {
   public:

      RowThread( ThreadData<P>& data, int firstRow, int endRow ) :
      Thread(), m_data( data ), m_firstRow( firstRow ), m_endRow( endRow )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         Rect r = m_data.image.SelectedRectangle();
         int width = r.Width();

         int d = m_data.convolution.InterlacingDistance();
         int dn = m_data.convolution.OverlappingDistance();
         int dn2 = dn >> 1;

         coefficient_vector H = m_data.convolution.Filter( 0 );

         GenericVector<typename P::sample> t( width + dn2+dn2 );

         for ( int c = m_data.image.FirstSelectedChannel(); c <= m_data.image.LastSelectedChannel(); ++c )
         {
            typename P::sample* f = m_data.image.PixelAddress( r.x0, m_firstRow, c );
            for ( int i = m_firstRow; i < m_endRow; ++i, f += m_data.image.Width() )
            {
               this->Convolve1D( f, *t, width, d, dn2, H );
               UPDATE_THREAD_MONITOR_CHUNK( 65536, width )
            }
         }
      }

   private:

      ThreadData<P>& m_data;
      int            m_firstRow;
      int            m_endRow;
   };

   template <class P>
   class ColThread : public Thread, public OneDimensionalConvolution<P>
   {
   public:

      ColThread( ThreadData<P>& data, int firstCol, int endCol ) :
      Thread(), m_data( data ), m_firstCol( firstCol ), m_endCol( endCol )
      {
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         Rect r = m_data.image.SelectedRectangle();
         int width = r.Width();
         int height = r.Height();

         int d = m_data.convolution.InterlacingDistance();
         int dn = m_data.convolution.OverlappingDistance();
         int dn2 = dn >> 1;

         coefficient_vector H = m_data.convolution.Filter( 1 );

         GenericVector<typename P::sample> g( height );
         GenericVector<typename P::sample> t( height + dn2+dn2 );

         for ( int c = m_data.image.FirstSelectedChannel(); c <= m_data.image.LastSelectedChannel(); ++c )
         {
            typename P::sample* f = m_data.image.PixelAddress( m_firstCol, r.y0, c );
            for ( int i = m_firstCol; i < m_endCol; ++i, ++f )
            {
               for ( int i = 0, j = 0; i < height; ++i, j += width )
                  g[i] = f[j];

               this->Convolve1D( *g, *t, height, d, dn2, H );

               for ( int i = 0, j = 0; i < height; ++i, j += width )
                  f[j] = g[i];

               UPDATE_THREAD_MONITOR_CHUNK( 65536, height )
            }
         }
      }

   private:

      ThreadData<P>& m_data;
      int            m_firstCol;
      int            m_endCol;
   };
};

// ----------------------------------------------------------------------------

void SeparableConvolution::Apply( pcl::Image& image ) const
{
   PCL_PRECONDITION( m_filter != 0 )
   ValidateFilter();
   PCL_SeparableConvolutionEngine::Apply( image, *this );
}

void SeparableConvolution::Apply( pcl::DImage& image ) const
{
   PCL_PRECONDITION( m_filter != 0 )
   ValidateFilter();
   PCL_SeparableConvolutionEngine::Apply( image, *this );
}

void SeparableConvolution::Apply( pcl::UInt8Image& image ) const
{
   PCL_PRECONDITION( m_filter != 0 )
   ValidateFilter();
   PCL_SeparableConvolutionEngine::Apply( image, *this );
}

void SeparableConvolution::Apply( pcl::UInt16Image& image ) const
{
   PCL_PRECONDITION( m_filter != 0 )
   ValidateFilter();
   PCL_SeparableConvolutionEngine::Apply( image, *this );
}

void SeparableConvolution::Apply( pcl::UInt32Image& image ) const
{
   PCL_PRECONDITION( m_filter != 0 )
   ValidateFilter();
   PCL_SeparableConvolutionEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/SeparableConvolution.cpp - Released 2014/11/14 17:17:00 UTC
