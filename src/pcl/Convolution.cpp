// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/Convolution.cpp - Released 2014/11/14 17:17:00 UTC
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

#include <pcl/Convolution.h>
#include <pcl/MultiVector.h>
#include <pcl/Thread.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_CorrelationEngine
{
public:

   template <class P> static
   void Apply( GenericImage<P>& image, const Convolution& convolution )
   {
      if ( convolution.IsHighPassFilter() )
      {
         if ( P::BitsPerSample() < 32 )
            HighPassIntegerImage( image, convolution, reinterpret_cast<Image*>( 0 ) );
         else
            HighPassIntegerImage( image, convolution, reinterpret_cast<DImage*>( 0 ) );
      }
      else
         DoApply( image, convolution );
   }

   static
   void Apply( Image& image, const Convolution& convolution )
   {
      DoApply( image, convolution );
      if ( convolution.IsHighPassFilter() )
         HighPassRescaleFloatImage( image, convolution );
   }

   static
   void Apply( DImage& image, const Convolution& convolution )
   {
      DoApply( image, convolution );
      if ( convolution.IsHighPassFilter() )
         HighPassRescaleFloatImage( image, convolution );
   }

private:

   template <class P> static
   void DoApply( GenericImage<P>& image, const Convolution& convolution )
   {
      if ( image.IsEmptySelection() )
         return;

      if ( convolution.Filter().IsEmpty() )
         throw Error( "Attempt to perform a convolution with an empty kernel filter." );

      image.SetUnique();

      int n = convolution.OverlappingDistance();
      if ( n > image.Height() || n > image.Width() )
      {
         image.Zero();
         return;
      }

      /*
       * We implement a correlation algorithm, so make sure that the
       * convolution filter is rotated by 180 degrees. We'll unrotate it once
       * convolution has finished.
       */
      bool didFlip = false;
      if ( !convolution.Filter().IsFlipped() )
      {
         const_cast<KernelFilter&>( convolution.Filter() ).Flip();
         didFlip = true;
      }

      int numberOfRows = image.SelectedRectangle().Height();
      int numberOfThreads = convolution.IsParallelProcessingEnabled() ?
                  Min( convolution.MaxProcessors(), pcl::Thread::NumberOfThreads( numberOfRows, n ) ) : 1;
      int rowsPerThread = numberOfRows/numberOfThreads;

      size_type N = image.NumberOfSelectedSamples();
      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "Convolution", N );

      ThreadData<P> data( image, convolution, N );

      PArray<Thread<P> > threads;
      for ( int i = 0, y0 = image.SelectedRectangle().y0; i < numberOfThreads; ++i )
      {
         int startRow = y0 + i*rowsPerThread;
         GenericImage<P>* upperOvRgn = (i > 0) ? new GenericImage<P> : 0;

         int endRow;
         GenericImage<P>* lowerOvRgn;
         if ( i < numberOfThreads-1 )
         {
            endRow = y0 + (i + 1)*rowsPerThread;
            lowerOvRgn = new GenericImage<P>;
         }
         else
         {
            endRow = y0 + numberOfRows;
            lowerOvRgn = 0;
         }

         threads.Add( new Thread<P>( data, startRow, endRow, upperOvRgn, lowerOvRgn ) );
      }

      try
      {
         AbstractImage::RunThreads( threads, data );

         if ( didFlip )
            const_cast<KernelFilter&>( convolution.Filter() ).Flip();
      }
      catch ( ... )
      {
         if ( didFlip )
            const_cast<KernelFilter&>( convolution.Filter() ).Flip();
         throw;
      }

      image.SetStatusCallback( 0 );

      int c0 = image.SelectedChannel();
      Point p0 = image.SelectedRectangle().LeftTop();

      for ( int i = 0; i < numberOfThreads; ++i )
      {
         if ( i > 0 )
            image.Mov( *threads[i].UpperOverlayRegion(),
                       Point( p0.x, p0.y + i*rowsPerThread ), c0 );
         if ( i < numberOfThreads-1 )
            image.Mov( *threads[i].LowerOverlayRegion(),
                       Point( p0.x, p0.y + (i + 1)*rowsPerThread - threads[i].LowerOverlayRegion()->Height() ), c0 );
      }

      image.Status() = data.status;

      threads.Destroy();
   }

   template <class P, class P1> static
   void HighPassIntegerImage( GenericImage<P>& image, const Convolution& convolution, GenericImage<P1>* )
   {
      GenericImage<P1> tmp( image );
      Apply( tmp, convolution );

      StatusMonitor monitor = tmp.Status();
      image.SetStatusCallback( 0 );

      image.Mov( tmp, image.SelectedRectangle().LeftTop() );

      image.Status() = monitor;
   }

   template <class P> static
   void HighPassRescaleFloatImage( GenericImage<P>& image, const Convolution& convolution )
   {
      if ( !convolution.IsRawHighPassEnabled() )
         if ( convolution.IsHighPassFilter() )
         {
            StatusMonitor monitor = image.Status();
            image.SetStatusCallback( 0 );

            if ( convolution.IsHighPassRescalingEnabled() )
               image.Normalize();
            else
               image.Truncate();

            image.Status() = monitor;
         }
   }

   template <class P>
   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( GenericImage<P>& a_image, const Convolution& a_convolution, size_type a_count ) :
      AbstractImage::ThreadData( a_image, a_count ),
      image( a_image ),
      convolution( a_convolution )
      {
      }

            GenericImage<P>& image;
      const Convolution&     convolution;
   };

   template <class P>
   class Thread : public pcl::Thread
   {
   public:

      Thread( ThreadData<P>& data, int startRow, int endRow, GenericImage<P>* upperOvRgn, GenericImage<P>* lowerOvRgn ) :
      pcl::Thread(),
      m_data( data ), m_firstRow( startRow ), m_endRow( endRow ), m_upperOvRgn( upperOvRgn ), m_lowerOvRgn( lowerOvRgn )
      {
      }

      virtual ~Thread()
      {
         if ( m_upperOvRgn != 0 )
            delete m_upperOvRgn, m_upperOvRgn = 0;
         if ( m_lowerOvRgn != 0 )
            delete m_lowerOvRgn, m_lowerOvRgn = 0;
      }

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         Rect r = m_data.image.SelectedRectangle();
         int w = r.Width();
         int dw = m_data.image.Width() - w;

         int n = m_data.convolution.OverlappingDistance();
         int n2 = n >> 1;
         int nf0 = w + (n2 << 1);

         int o0 = m_firstRow;
         if ( m_upperOvRgn != 0 )
         {
            m_upperOvRgn->AllocateData( w, n2, m_data.image.NumberOfSelectedChannels() );
            o0 += n2;
         }

         int o1 = m_endRow;
         if ( m_lowerOvRgn != 0 )
         {
            m_lowerOvRgn->AllocateData( w, n2, m_data.image.NumberOfSelectedChannels() );
            o1 -= n2;
         }

         typename P::sample th0 = P::ToSample( m_data.convolution.LowThreshold() );
         typename P::sample th1 = P::ToSample( m_data.convolution.HighThreshold() );

         bool tz0 = 1 + th0 == 1;
         bool tz1 = 1 + th1 == 1;
         bool tz = tz0 && tz1;

         bool unitWeight = m_data.convolution.FilterWeight() == 1;

         int nc = m_data.convolution.Filter().NumberOfCoefficients();
         DVector hf( nc );

         GenericMultiVector<typename P::sample> f0( P::MinSampleValue(), n, nf0 );

         for ( int c = m_data.image.FirstSelectedChannel(), cn = 0; c <= m_data.image.LastSelectedChannel(); ++c, ++cn )
         {
            typename P::sample* f = m_data.image.PixelAddress( r.x0, m_firstRow, c );
            typename P::sample* g = (m_upperOvRgn != 0) ? m_upperOvRgn->PixelData( cn ) : 0;

            for ( int i = 0, i0 = m_firstRow-n2, i1 = m_firstRow+n2-1; i < n2; ++i, ++i0, --i1 )
               ::memcpy( f0[i].At( n2 ), m_data.image.PixelAddress( r.x0, (i0 < 0) ? i1 : i0, c ), w*P::BytesPerSample() );

            for ( int i = n2, i1 = m_firstRow; i < n; ++i, ++i1 )
               ::memcpy( f0[i].At( n2 ), m_data.image.PixelAddress( r.x0, i1, c ), w*P::BytesPerSample() );

            for ( int i = 0; i < n; ++i )
            {
               typename P::sample* f0i = *f0[i];
               typename P::sample* f1i = f0i + n2+n2;
               do
                  *f0i++ = *f1i--;
               while ( f0i < f1i );

               f0i = f0[i].At( w-1 );
               f1i = f0i + n2+n2;
               do
                  *f1i-- = *f0i++;
               while ( f0i < f1i );
            }

            for ( int y = m_firstRow; ; )
            {
               for ( int x = 0; x < w; ++x )
               {
                  const float* h = m_data.convolution.Filter().Begin();
                  for ( int i = 0, k = 0; i < n; i += m_data.convolution.InterlacingDistance() )
                  {
                     const typename P::sample* fi = f0[i].At( x );
                     for ( int j = 0; j < n; j += m_data.convolution.InterlacingDistance(), fi += m_data.convolution.InterlacingDistance() )
                        hf[k++] = *h++ * *fi;
                  }

                  double r = 0;
                  for ( int k = 0; k < nc; ++k )
                     r += hf[k];
                  if ( !unitWeight )
                     r /= m_data.convolution.FilterWeight();

                  if ( !tz )
                  {
                     if ( r < *f )
                     {
                        if ( !tz0 )
                        {
                           double k = *f - r;
                           if ( k < th0 )
                           {
                              k /= th0;
                              r = k*r + (1 - k)*(*f);
                           }
                        }
                     }
                     else
                     {
                        if ( !tz1 )
                        {
                           double k = r - *f;
                           if ( k < th1 )
                           {
                              k /= th1;
                              r = k*r + (1 - k)*(*f);
                           }
                        }
                     }
                  }

                  if ( g == 0 )
                     *f = P::FloatToSample( r );
                  else
                     *g++ = P::FloatToSample( r );

                  ++f;

                  UPDATE_THREAD_MONITOR( 65536 )
               }

               if ( ++y == m_endRow )
                  break;

               f += dw;

               if ( g == 0 )
               {
                  if ( y == o1 )
                     g = m_lowerOvRgn->PixelData( cn );
               }
               else
               {
                  if ( y == o0 )
                     g = 0;
               }

               for ( int i = 1; i < n; ++i )
                  pcl::Swap( f0[i-1], f0[i] );

               if ( y+n2 < m_data.image.Height() )
               {
                  ::memcpy( f0[n-1].At( n2 ), m_data.image.PixelAddress( r.x0, y+n2, c ), w*P::BytesPerSample() );

                  typename P::sample* f0n = *f0[n-1];
                  typename P::sample* f1n = f0n + n2+n2;
                  do
                     *f0n++ = *f1n--;
                  while ( f0n < f1n );

                  f0n = f0[n-1].At( w-1 );
                  f1n = f0n + n2+n2;
                  do
                     *f1n-- = *f0n++;
                  while ( f0n < f1n );
               }
               else
               {
                  ::memcpy( *f0[n-1], *f0[n-2], nf0*P::BytesPerSample() );
                  /*
                   * ### NB: Cannot use an assignment operator here because all
                   * the f0 vectors must be unique.
                   */
                  //f0[n-1] = f0[n-2];
               }
            }
         }
      }

      const GenericImage<P>* UpperOverlayRegion() const
      {
         return m_upperOvRgn;
      }

      const GenericImage<P>* LowerOverlayRegion() const
      {
         return m_lowerOvRgn;
      }

   private:

      ThreadData<P>&   m_data;
      int              m_firstRow;
      int              m_endRow;
      GenericImage<P>* m_upperOvRgn; // upper overlapping region
      GenericImage<P>* m_lowerOvRgn; // lower overlapping region
   };
};

// ----------------------------------------------------------------------------

void Convolution::Apply( Image& image ) const
{
   PCL_PRECONDITION( m_filter != 0 )
   ValidateFilter();
   PCL_CorrelationEngine::Apply( image, *this );
}

void Convolution::Apply( DImage& image ) const
{
   PCL_PRECONDITION( m_filter != 0 )
   ValidateFilter();
   PCL_CorrelationEngine::Apply( image, *this );
}

void Convolution::Apply( UInt8Image& image ) const
{
   PCL_PRECONDITION( m_filter != 0 )
   ValidateFilter();
   PCL_CorrelationEngine::Apply( image, *this );
}

void Convolution::Apply( UInt16Image& image ) const
{
   PCL_PRECONDITION( m_filter != 0 )
   ValidateFilter();
   PCL_CorrelationEngine::Apply( image, *this );
}

void Convolution::Apply( UInt32Image& image ) const
{
   PCL_PRECONDITION( m_filter != 0 )
   ValidateFilter();
   PCL_CorrelationEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/Convolution.cpp - Released 2014/11/14 17:17:00 UTC
