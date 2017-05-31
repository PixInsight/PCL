//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.04.0827
// ----------------------------------------------------------------------------
// pcl/Histogram.cpp - Released 2017-05-28T08:29:05Z
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

#include <pcl/Histogram.h>
#include <pcl/Thread.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_HistogramEngine
{
public:

   static
   void Compute( Histogram::histogram_type& histogram,
                 const ImageVariant& image, const Rect& rect, int channel, bool parallel, int maxProcessors )
   {
      if ( histogram.IsEmpty() ) // ?!
         return;

      histogram = 0;

      Rect r = rect;
      if ( !r.IsRect() )
         r = image.SelectedRectangle();
      else if ( !image.Clip( r ) )
         return;
      int width = r.Width();
      int height = r.Height();

      if ( channel < 0 )
         channel = image.SelectedChannel();
      else if ( !image.IsValidChannelIndex( channel ) )
         return;

      size_type N = size_type( width ) * size_type( height );
      if ( N == 0 )
         return;
      size_type N1 = N >> 1;
      size_type N2 = N - N1;

      if ( image->Status().IsInitializationEnabled() )
         image->Status().Initialize( "Histogram generation", N );

      int numberOfThreads = parallel ? Min( maxProcessors, Thread::NumberOfThreads( height, Max( 1, 1024/width ) ) ) : 1;
      int rowsPerThread = height/numberOfThreads;
      bool useAffinity = parallel && Thread::IsRootThread();

      double min = 0, max = 1;
      if ( image.IsFloatSample() )
      {
         ReferenceArray<RealMinMaxThread> threads;
         for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
            threads.Add( new RealMinMaxThread( image, r, channel, i*rowsPerThread, (j < numberOfThreads) ? j*rowsPerThread : height ) );

         if ( numberOfThreads > 1 )
         {
            for ( int i = 0; i < numberOfThreads; ++i )
               threads[i].Start( ThreadPriority::DefaultMax, useAffinity ? i : -1 );
            for ( int i = 0; i < numberOfThreads; ++i )
               threads[i].Wait();
         }
         else
            threads[0].Run();

         min = threads[0].min;
         max = threads[0].max;
         for ( int i = 1; i < numberOfThreads; ++i )
         {
            if ( threads[i].min < min )
               min = threads[i].min;
            if ( max < threads[i].max )
               max = threads[i].max;
         }

         threads.Destroy();

         if ( 1 + (max - min) == 1 )
         {
            image->Status() += N1 + N2;
            return;
         }
      }

      image->Status() += N1;

      ReferenceArray<HistogramThread> threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new HistogramThread( image, r, channel, histogram.Length(), min, max,
                                           i*rowsPerThread, (j < numberOfThreads) ? j*rowsPerThread : height ) );
      if ( numberOfThreads > 1 )
      {
         for ( int i = 0; i < numberOfThreads; ++i )
            threads[i].Start( ThreadPriority::DefaultMax, useAffinity ? i : -1 );
         for ( int i = 0; i < numberOfThreads; ++i )
            threads[i].Wait();
      }
      else
         threads[0].Run();

      for ( int i = 0; i < numberOfThreads; ++i )
         histogram += threads[i].histogram;

      threads.Destroy();

      image->Status() += N2;
   }

private:

   class RealMinMaxThread : public Thread
   {
   public:

      double min, max;

      RealMinMaxThread( const ImageVariant& image, const Rect& rect, int channel, int startRow, int endRow ) :
      Thread(), m_image( image ), m_rect( rect ), m_channel( channel ), m_startRow( startRow ), m_endRow( endRow )
      {
      }

      virtual void Run()
      {
         if ( m_image.IsFloatSample() )
            switch ( m_image.BitsPerSample() )
            {
            case 32 : RealMinMax( static_cast<const Image&>( *m_image ), m_rect, m_channel, min, max, m_startRow, m_endRow ); break;
            case 64 : RealMinMax( static_cast<const DImage&>( *m_image ), m_rect, m_channel, min, max, m_startRow, m_endRow ); break;
            }
      }

   private:

      const ImageVariant& m_image;
      const Rect&         m_rect;
            int           m_channel;
            int           m_startRow, m_endRow;

      template <class P> static
      void RealMinMax( const GenericImage<P>& image, const Rect& r, int c, double& min, double& max, int y0, int y1 )
      {
         int w = r.Width();
         min = max = image( r.LeftTop(), c );
         for ( int y = r.y0+y0, y01 = r.y0+y1; y < y01; ++y )
         {
            const typename P::sample* f  = image.ScanLine( y, c ) + r.x0;
            const typename P::sample* fw = f + w;
            do
            {
               if ( *f < min )
                  min = *f;
               else if ( max < *f )
                  max = *f;
            }
            while ( ++f < fw );
         }
      }
   };

   class HistogramThread : public Thread
   {
   public:

      Histogram::histogram_type histogram;

      HistogramThread( const ImageVariant& image, const Rect& rect, int channel,
                       int resolution, double min, double max, int startRow, int endRow ) :
      Thread(),
      histogram( 0, resolution ), m_image( image ), m_rect( rect ), m_channel( channel ),
      m_min( min ), m_max( max ), m_startRow( startRow ), m_endRow( endRow )
      {
      }

      virtual void Run()
      {
         if ( m_image.IsFloatSample() )
            switch ( m_image.BitsPerSample() )
            {
            case 32: RealHistogram( static_cast<const Image&>( *m_image ), m_rect, m_channel, histogram, m_min, m_max, m_startRow, m_endRow );
               break;
            case 64: RealHistogram( static_cast<const DImage&>( *m_image ), m_rect, m_channel, histogram, m_min, m_max, m_startRow, m_endRow );
               break;
            }
         else
            switch ( m_image.BitsPerSample() )
            {
            case  8: IntegerHistogram( static_cast<const UInt8Image&>( *m_image ), m_rect, m_channel, histogram, m_startRow, m_endRow );
               break;
            case 16: IntegerHistogram( static_cast<const UInt16Image&>( *m_image ), m_rect, m_channel, histogram, m_startRow, m_endRow );
               break;
            case 32: IntegerHistogram( static_cast<const UInt32Image&>( *m_image ), m_rect, m_channel, histogram, m_startRow, m_endRow );
               break;
            }
      }

   private:

      const ImageVariant& m_image;
      const Rect&         m_rect;
            int           m_channel;
            double        m_min, m_max;
            int           m_startRow, m_endRow;

      template <class P> static
      void IntegerHistogram( const GenericImage<P>& image,
                             const Rect& r, int c, Histogram::histogram_type& histogram, int y0, int y1 )
      {
         if ( size_type( P::MaxSampleValue() ) == size_type( histogram.Length()-1 ) )
         {
            // The image and the histogram use the same sample range (e.g. UInt16Image and a 16-bit histogram).
            for ( typename GenericImage<P>::const_roi_sample_iterator i( image, Rect( r.x0, r.y0+y0, r.x1, r.y0+y1 ), c ); i; ++i )
               ++histogram[int( *i )];
         }
         else
         {
            // The image and the histogram use different sample ranges.
            double k = double( histogram.Length()-1 )/P::MaxSampleValue();
            for ( typename GenericImage<P>::const_roi_sample_iterator i( image, Rect( r.x0, r.y0+y0, r.x1, r.y0+y1 ), c ); i; ++i )
               ++histogram[pcl::RoundInt( *i * k )];
         }
      }

      template <class P> static
      void RealHistogram( const GenericImage<P>& image,
                          const Rect& r, int c, Histogram::histogram_type& histogram, double min, double max, int y0, int y1 )
      {
         if ( min >= 0 && min <= 1 && max >= 0 && max <= 1 )
         {
            // Normalized real image.
            int k = histogram.Length() - 1;
            for ( typename GenericImage<P>::const_roi_sample_iterator i( image, Rect( r.x0, r.y0+y0, r.x1, r.y0+y1 ), c ); i; ++i )
               ++histogram[pcl::RoundInt( *i * k )];
         }
         else
         {
            // Unnormalized real image.
            double k = (histogram.Length() - 1)/(max - min);
            for ( typename GenericImage<P>::const_roi_sample_iterator i( image, Rect( r.x0, r.y0+y0, r.x1, r.y0+y1 ), c ); i; ++i )
               ++histogram[pcl::RoundInt( k*(*i - min) )];
         }
      }
   };
};

// ----------------------------------------------------------------------------

#define IMPLEMENT_HISTOGRAM_GENERATION                                                             \
   Allocate();                                                                                     \
   PCL_HistogramEngine::Compute( m_histogram, v, m_rect, m_channel, m_parallel, m_maxProcessors ); \
   UpdatePeakLevel();

const pcl::Image& Histogram::operator <<( const pcl::Image& image )
{
   ImageVariant v( const_cast<pcl::Image*>( &image ) );
   IMPLEMENT_HISTOGRAM_GENERATION
   return image;
}

const pcl::DImage& Histogram::operator <<( const pcl::DImage& image )
{
   ImageVariant v( const_cast<pcl::DImage*>( &image ) );
   IMPLEMENT_HISTOGRAM_GENERATION
   return image;
}

const pcl::UInt8Image& Histogram::operator <<( const pcl::UInt8Image& image )
{
   ImageVariant v( const_cast<pcl::UInt8Image*>( &image ) );
   IMPLEMENT_HISTOGRAM_GENERATION
   return image;
}

const pcl::UInt16Image& Histogram::operator <<( const pcl::UInt16Image& image )
{
   ImageVariant v( const_cast<pcl::UInt16Image*>( &image ) );
   IMPLEMENT_HISTOGRAM_GENERATION
   return image;
}

const pcl::UInt32Image& Histogram::operator <<( const pcl::UInt32Image& image )
{
   ImageVariant v( const_cast<pcl::UInt32Image*>( &image ) );
   IMPLEMENT_HISTOGRAM_GENERATION
   return image;
}

const pcl::ImageVariant& Histogram::operator <<( const pcl::ImageVariant& v )
{
   IMPLEMENT_HISTOGRAM_GENERATION
   return v;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Histogram.cpp - Released 2017-05-28T08:29:05Z
