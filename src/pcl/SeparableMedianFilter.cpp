//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// pcl/SeparableMedianFilter.cpp - Released 2018-11-23T16:14:32Z
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

#include <pcl/SeparableMedianFilter.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_SeparableMedianFilterEngine
{
public:

   template <class P> static
   void Apply( GenericImage<P>& image, const SeparableMedianFilter& filter )
   {
      if ( image.IsEmptySelection() )
         return;

      image.EnsureUnique();

      int n = filter.Size();
      if ( n > image.Height() || n > image.Width() )
      {
         image.Zero();
         return;
      }

      /*
       * In order to make SeparableMedianFilter a perfect replacement for
       * MorphologicalTransformation + MedianFilter, we force it to perform the
       * same status monitoring increments: image.NumberOfSelectedSamples().
       */
      size_type N = image.NumberOfSelectedSamples();
      size_type N1 = N >> 1;
      size_type N2 = N - N1;
      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "Separable median filter", N );

      for ( int dir = 0; dir < 2; ++dir )
      {
         int numberOfItems = dir ? image.SelectedRectangle().Width() : image.SelectedRectangle().Height();
         int numberOfThreads = filter.IsParallelProcessingEnabled() ?
                  Min( filter.MaxProcessors(), pcl::Thread::NumberOfThreads( numberOfItems, 4 ) ) : 1;
         int itemsPerThread = numberOfItems/numberOfThreads;

         AbstractImage::ThreadData threadData( image.Status(), dir ? N2 : N1 );

         ReferenceArray<Thread<P> > threads;
         int i0 = dir ? image.SelectedRectangle().x0 : image.SelectedRectangle().y0;
         for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         {
            int firstItem = i0 + i*itemsPerThread;
            int endItem   = i0 + ((j < numberOfThreads) ? j*itemsPerThread : numberOfItems);
            if ( dir )
               threads.Add( new SeparableMedianFilterColThread<P>( threadData, image, filter, firstItem, endItem ) );
            else
               threads.Add( new SeparableMedianFilterRowThread<P>( threadData, image, filter, firstItem, endItem ) );
         }

         AbstractImage::RunThreads( threads, threadData );

         threads.Destroy();

         image.Status() = threadData.status;
      }
   }

private:

   template <class P>
   class Thread : public pcl::Thread
   {
   public:

      Thread( AbstractImage::ThreadData& data, GenericImage<P>& image, const SeparableMedianFilter& filter ) :
      pcl::Thread(), m_image( image ), m_filter( filter ), m_data( data )
      {
      }

   protected:

#define WSORT( a, b )  \
      if ( w[b] < w[a] ) Swap( w[a], w[b] )

      void PCL_HOT_FUNCTION
      Median1D( typename P::sample* f, typename P::sample* t, typename P::sample* w, int n, int N )
      {
         int n2 = n >> 1;
         for ( int i = 0, j = n2; i < n2; ++i )
            t[i] = f[--j];
         ::memcpy( t+n2, f, N*sizeof( *f ) );
         for ( int i = N+n2+n2, j = N-n2; j < N; ++j )
            t[--i] = f[j];

         for ( int i = 0; i < N; ++i, ++f )
         {
            ::memcpy( w, t++, n*sizeof( *t ) );
            switch ( n )
            {
            case  3:
               WSORT( 0, 1 ); WSORT( 1, 2 ); WSORT( 0, 1 );
               *f = w[1];
               break;
            case  5:
               WSORT( 0, 1 ); WSORT( 3, 4 ); WSORT( 0, 3 );
               WSORT( 1, 4 ); WSORT( 1, 2 ); WSORT( 2, 3 );
               WSORT( 1, 2 );
               *f = w[2];
               break;
            case  7:
               WSORT( 0, 5 ); WSORT( 0, 3 ); WSORT( 1, 6 );
               WSORT( 2, 4 ); WSORT( 0, 1 ); WSORT( 3, 5 );
               WSORT( 2, 6 ); WSORT( 2, 3 ); WSORT( 3, 6 );
               WSORT( 4, 5 ); WSORT( 1, 4 ); WSORT( 1, 3 );
               WSORT( 3, 4 );
               *f = w[3];
               break;
            case  9:
               WSORT( 1, 2 ); WSORT( 4, 5 ); WSORT( 7, 8 );
               WSORT( 0, 1 ); WSORT( 3, 4 ); WSORT( 6, 7 );
               WSORT( 1, 2 ); WSORT( 4, 5 ); WSORT( 7, 8 );
               WSORT( 0, 3 ); WSORT( 5, 8 ); WSORT( 4, 7 );
               WSORT( 3, 6 ); WSORT( 1, 4 ); WSORT( 2, 5 );
               WSORT( 4, 7 ); WSORT( 4, 2 ); WSORT( 6, 4 );
               WSORT( 4, 2 );
               *f = w[4];
               break;
            case 25:
               WSORT(  0,  1 ); WSORT(  3,  4 ); WSORT(  2,  4 );
               WSORT(  2,  3 ); WSORT(  6,  7 ); WSORT(  5,  7 );
               WSORT(  5,  6 ); WSORT(  9, 10 ); WSORT(  8, 10 );
               WSORT(  8,  9 ); WSORT( 12, 13 ); WSORT( 11, 13 );
               WSORT( 11, 12 ); WSORT( 15, 16 ); WSORT( 14, 16 );
               WSORT( 14, 15 ); WSORT( 18, 19 ); WSORT( 17, 19 );
               WSORT( 17, 18 ); WSORT( 21, 22 ); WSORT( 20, 22 );
               WSORT( 20, 21 ); WSORT( 23, 24 ); WSORT(  2,  5 );
               WSORT(  3,  6 ); WSORT(  0,  6 ); WSORT(  0,  3 );
               WSORT(  4,  7 ); WSORT(  1,  7 ); WSORT(  1,  4 );
               WSORT( 11, 14 ); WSORT(  8, 14 ); WSORT(  8, 11 );
               WSORT( 12, 15 ); WSORT(  9, 15 ); WSORT(  9, 12 );
               WSORT( 13, 16 ); WSORT( 10, 16 ); WSORT( 10, 13 );
               WSORT( 20, 23 ); WSORT( 17, 23 ); WSORT( 17, 20 );
               WSORT( 21, 24 ); WSORT( 18, 24 ); WSORT( 18, 21 );
               WSORT( 19, 22 ); WSORT(  8, 17 ); WSORT(  9, 18 );
               WSORT(  0, 18 ); WSORT(  0,  9 ); WSORT( 10, 19 );
               WSORT(  1, 19 ); WSORT(  1, 10 ); WSORT( 11, 20 );
               WSORT(  2, 20 ); WSORT(  2, 11 ); WSORT( 12, 21 );
               WSORT(  3, 21 ); WSORT(  3, 12 ); WSORT( 13, 22 );
               WSORT(  4, 22 ); WSORT(  4, 13 ); WSORT( 14, 23 );
               WSORT(  5, 23 ); WSORT(  5, 14 ); WSORT( 15, 24 );
               WSORT(  6, 24 ); WSORT(  6, 15 ); WSORT(  7, 16 );
               WSORT(  7, 19 ); WSORT( 13, 21 ); WSORT( 15, 23 );
               WSORT(  7, 13 ); WSORT(  7, 15 ); WSORT(  1,  9 );
               WSORT(  3, 11 ); WSORT(  5, 17 ); WSORT( 11, 17 );
               WSORT(  9, 17 ); WSORT(  4, 10 ); WSORT(  6, 12 );
               WSORT(  7, 14 ); WSORT(  4,  6 ); WSORT(  4,  7 );
               WSORT( 12, 14 ); WSORT( 10, 14 ); WSORT(  6,  7 );
               WSORT( 10, 12 ); WSORT(  6, 10 ); WSORT(  6, 17 );
               WSORT( 12, 17 ); WSORT(  7, 17 ); WSORT(  7, 10 );
               WSORT( 12, 18 ); WSORT(  7, 12 ); WSORT( 10, 18 );
               WSORT( 12, 20 ); WSORT( 10, 20 ); WSORT( 10, 12 );
               *f = w[12];
               break;
            default:
               *f = *pcl::Select( w, w+n, n2 );
               break;
            }
         }
      }

#undef WSORT

            GenericImage<P>&       m_image;
      const SeparableMedianFilter& m_filter;
      AbstractImage::ThreadData&   m_data;
   };

#define m_image  this->m_image
#define m_filter this->m_filter

   template <class P>
   class SeparableMedianFilterRowThread : public Thread<P>
   {
   public:

      SeparableMedianFilterRowThread( AbstractImage::ThreadData& data, GenericImage<P>& image,
                                      const SeparableMedianFilter& filter, int firstRow, int endRow ) :
         Thread<P>( data, image, filter ), m_firstRow( firstRow ), m_endRow( endRow )
      {
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         Rect r = m_image.SelectedRectangle();
         int N = r.Width();
         int n = m_filter.Size();
         int n2 = n >> 1;
         GenericVector<typename P::sample> t( N+n2+n2 );
         GenericVector<typename P::sample> w( n );
         for ( int c = m_image.FirstSelectedChannel(); c <= m_image.LastSelectedChannel(); ++c )
         {
            typename P::sample* f = m_image.PixelAddress( r.x0, m_firstRow, c );
            for ( int i = m_firstRow; i < m_endRow; ++i, f += m_image.Width() )
            {
               this->Median1D( f, *t, *w, n, N );
               UPDATE_THREAD_MONITOR_CHUNK( 65536, N )
            }
         }
      }

   private:

      int m_firstRow, m_endRow;
   };

   template <class P>
   class SeparableMedianFilterColThread : public Thread<P>
   {
   public:

      SeparableMedianFilterColThread( AbstractImage::ThreadData& data, GenericImage<P>& image,
                                      const SeparableMedianFilter& filter, int startCol, int endCol ) :
         Thread<P>( data, image, filter ), m_firstCol( startCol ), m_endCol( endCol )
      {
      }

      PCL_HOT_FUNCTION void Run() override
      {
         INIT_THREAD_MONITOR()

         Rect r = m_image.SelectedRectangle();
         int d = r.Width();
         int N = r.Height();
         int n = m_filter.Size();
         int n2 = n >> 1;
         GenericVector<typename P::sample> g( N );
         GenericVector<typename P::sample> t( N+n2+n2 );
         GenericVector<typename P::sample> w( n );
         for ( int c = m_image.FirstSelectedChannel(); c <= m_image.LastSelectedChannel(); ++c )
         {
            typename P::sample* f = m_image.PixelAddress( m_firstCol, r.y0, c );

            for ( int i = m_firstCol; i < m_endCol; ++i, ++f )
            {
               for ( int i = 0, j = 0; i < N; ++i, j += d )
                  g[i] = f[j];
               this->Median1D( *g, *t, *w, n, N );
               for ( int i = 0, j = 0; i < N; ++i, j += d )
                  f[j] = g[i];

               UPDATE_THREAD_MONITOR_CHUNK( 65536, N )
            }
         }
      }

   private:

      int m_firstCol, m_endCol;
   };

#undef m_image
#undef m_filter
};

// ----------------------------------------------------------------------------

void SeparableMedianFilter::Apply( Image& image ) const
{
   PCL_SeparableMedianFilterEngine::Apply( image, *this );
}

void SeparableMedianFilter::Apply( DImage& image ) const
{
   PCL_SeparableMedianFilterEngine::Apply( image, *this );
}

void SeparableMedianFilter::Apply( UInt8Image& image ) const
{
   PCL_SeparableMedianFilterEngine::Apply( image, *this );
}

void SeparableMedianFilter::Apply( UInt16Image& image ) const
{
   PCL_SeparableMedianFilterEngine::Apply( image, *this );
}

void SeparableMedianFilter::Apply( UInt32Image& image ) const
{
   PCL_SeparableMedianFilterEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/SeparableMedianFilter.cpp - Released 2018-11-23T16:14:32Z
