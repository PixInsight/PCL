//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0842
// ----------------------------------------------------------------------------
// pcl/HistogramTransformation.cpp - Released 2017-06-21T11:36:44Z
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
#include <pcl/HistogramTransformation.h>
#include <pcl/Thread.h>

namespace pcl
{

// ----------------------------------------------------------------------------

bool HistogramTransformation::IsIdentityTransformationSet() const
{
   if ( !IsIdentityTransformation() )
      return false;
   for ( const HistogramTransformation& H : m_transformChain )
      if ( !H.IsIdentityTransformationSet() )
         return false;
   return true;
}

void HistogramTransformation::UpdateFlags()
{
   m_flags.hasClipping = m_clipLow != 0 || m_clipHigh != 1;
   m_flags.hasMTF = m_midtonesBalance != 0.5;
   m_flags.hasRange = m_expandLow != 0 || m_expandHigh != 1;
   m_flags.hasDelta = false;
   if ( m_flags.hasClipping )
   {
      m_flags.d = m_clipHigh - m_clipLow;
      m_flags.hasDelta = 1 + m_flags.d != 1;
   }
   if ( m_flags.hasRange )
      m_flags.dr = m_expandHigh - m_expandLow;
}

// ----------------------------------------------------------------------------

template <class T> static PCL_HOT_FUNCTION
void ApplyHistogramTransformation( T* a, size_type n, T x0, T x1, const HistogramTransformation& H )
{
   if ( a == 0 || n == 0 )
      return;

   if ( x1 < x0 )
      pcl::Swap( x0, x1 );

   if ( x0 != T( 0 ) || x1 != T( 1 ) )
   {
      double dx = x1 - x0;
      if ( 1 + dx == 1 )
         return;

      for ( size_type i = 0; i < n; ++i )
      {
         double f = Range( double( *a - x0 )/dx, 0.0, 1.0 );
         for ( size_type j = 0; j < H.Length(); ++j )
            H[j].Transform( f );
         *a++ = T( f*dx + x0 );
      }
   }
   else
   {
      for ( size_type i = 0; i < n; ++i )
      {
         double f = Range( double( *a ), 0.0, 1.0 );
         for ( size_type j = 0; j < H.Length(); ++j )
            H[j].Transform( f );
         *a++ = T( f );
      }
   }
}

void HistogramTransformation::Apply( double* a, size_type n, double x0, double x1 ) const
{
   ApplyHistogramTransformation( a, n, x0, x1, *this );
}

void HistogramTransformation::Apply( float* a, size_type n, float x0, float x1 ) const
{
   ApplyHistogramTransformation( a, n, x0, x1, *this );
}

// ----------------------------------------------------------------------------

void HistogramTransformation::Apply( Histogram& dstH, const Histogram& srcH ) const
{
   dstH.Allocate();
   dstH.m_histogram = 0;
   dstH.m_peakLevel = 0;

   if ( srcH.IsEmpty() )
      return;

   for ( int i = 0; i < srcH.m_resolution; ++i )
   {
      double f = double( i )/(srcH.m_resolution - 1);
      Transform( f );
      for ( const HistogramTransformation& H : m_transformChain )
         H.Transform( f );
      dstH.m_histogram[RoundInt( f*(dstH.m_resolution - 1) )] += srcH.m_histogram[i];
   }

   dstH.UpdatePeakLevel();
}

// ----------------------------------------------------------------------------

void HistogramTransformation::Make8BitLUT( uint8* lut ) const
{
   if ( lut == 0 )
      return;

   for ( int i = 0; i <= uint8_max; ++i )
   {
      double f = double( i )/uint8_max;
      Transform( f );
      for ( const HistogramTransformation& H : m_transformChain )
         H.Transform( f );
      *lut++ = uint8( RoundInt( f*uint8_max ) );
   }
}

// ----------------------------------------------------------------------------

void HistogramTransformation::Make16BitLUT( uint8* lut ) const
{
   if ( lut == 0 )
      return;

   for ( int i = 0; i <= uint16_max; ++i )
   {
      double f = double( i )/uint16_max;
      Transform( f );
      for ( const HistogramTransformation& H : m_transformChain )
         H.Transform( f );
      *lut++ = uint8( RoundInt( f*uint8_max ) );
   }
}

void HistogramTransformation::Make16BitLUT( uint16* lut ) const
{
   if ( lut == 0 )
      return;

   for ( int i = 0; i <= uint16_max; ++i )
   {
      double f = double( i )/uint16_max;
      Transform( f );
      for ( const HistogramTransformation& H : m_transformChain )
         H.Transform( f );
      *lut++ = uint16( RoundInt( f*uint16_max ) );
   }
}

// ----------------------------------------------------------------------------

void HistogramTransformation::Make20BitLUT( uint8* lut ) const
{
   if ( lut == 0 )
      return;

   for ( uint32 i = 0; i <= uint20_max; ++i )
   {
      double f = double( i )/uint20_max;
      Transform( f );
      for ( const HistogramTransformation& H : m_transformChain )
         H.Transform( f );
      *lut++ = uint8( RoundInt( f*uint8_max ) );
   }
}

void HistogramTransformation::Make20BitLUT( uint16* lut ) const
{
   if ( lut == 0 )
      return;

   for ( uint32 i = 0; i <= uint20_max; ++i )
   {
      double f = double( i )/uint20_max;
      Transform( f );
      for ( const HistogramTransformation& H : m_transformChain )
         H.Transform( f );
      *lut++ = uint16( RoundInt( f*uint16_max ) );
   }
}

// ----------------------------------------------------------------------------

class LUT2408Thread : public Thread
{
public:

   LUT2408Thread( uint8* lut, const HistogramTransformation& transform, int start, int end ) :
      Thread(),
      m_lut( lut ),
      m_T( transform ),
      m_start( start ),
      m_end( end )
   {
   }

   virtual PCL_HOT_FUNCTION void Run()
   {
      if ( m_T.m_transformChain.IsEmpty() )
         for ( int i = m_start; i < m_end; ++i )
         {
            double f = double( i )/uint24_max;
            m_T.Transform( f );
            m_lut[i] = uint8( RoundInt( f*uint8_max ) );
         }
      else
         for ( int i = m_start; i < m_end; ++i )
         {
            double f = double( i )/uint24_max;
            m_T.Transform( f );
            for ( const HistogramTransformation& H : m_T.m_transformChain )
               H.Transform( f );
            m_lut[i] = uint8( RoundInt( f*uint8_max ) );
         }
   }

private:

         uint8*                   m_lut;
   const HistogramTransformation& m_T;
         int                      m_start, m_end;
};

void HistogramTransformation::Make24BitLUT( uint8* lut ) const
{
   if ( lut == 0 )
      return;

   int numberOfThreads = m_parallel ? Min( int( m_maxProcessors ), Thread::NumberOfThreads( uint24_max+1, 256 ) ) : 1;
   int itemsPerThread = (uint24_max + 1)/numberOfThreads;
   bool useAffinity = m_parallel && Thread::IsRootThread();

   ReferenceArray<LUT2408Thread> threads;
   for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
      threads.Add( new LUT2408Thread( lut, *this,
                                      i*itemsPerThread, (j < numberOfThreads) ? j*itemsPerThread : uint24_max+1 ) );
   if ( numberOfThreads > 1 )
   {
      for ( int i = 0; i < numberOfThreads; ++i )
         threads[i].Start( ThreadPriority::DefaultMax, useAffinity ? i : -1 );
      for ( int i = 0; i < numberOfThreads; ++i )
         threads[i].Wait();
   }
   else
      threads[0].Run();

   threads.Destroy();
}

// ----------------------------------------------------------------------------

class LUT2416Thread : public Thread
{
public:

   LUT2416Thread( uint16* lut, const HistogramTransformation& transform, int start, int end ) :
      Thread(),
      m_lut( lut ),
      m_T( transform ),
      m_start( start ),
      m_end( end )
   {
   }

   virtual PCL_HOT_FUNCTION void Run()
   {
      if ( m_T.m_transformChain.IsEmpty() )
         for ( int i = m_start; i < m_end; ++i )
         {
            double f = double( i )/uint24_max;
            m_T.Transform( f );
            m_lut[i] = uint16( RoundInt( f*uint16_max ) );
         }
      else
         for ( int i = m_start; i < m_end; ++i )
         {
            double f = double( i )/uint24_max;
            m_T.Transform( f );
            for ( const HistogramTransformation& H : m_T.m_transformChain )
               H.Transform( f );
            m_lut[i] = uint16( RoundInt( f*uint16_max ) );
         }
   }

private:

         uint16*                  m_lut;
   const HistogramTransformation& m_T;
         int                      m_start, m_end;
};

void HistogramTransformation::Make24BitLUT( uint16* lut ) const
{
   if ( lut == 0 )
      return;

   int numberOfThreads = m_parallel ? Min( int( m_maxProcessors ), Thread::NumberOfThreads( uint24_max+1, 256 ) ) : 1;
   int itemsPerThread = (uint24_max + 1)/numberOfThreads;
   bool useAffinity = m_parallel && Thread::IsRootThread();

   ReferenceArray<LUT2416Thread> threads;
   for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
      threads.Add( new LUT2416Thread( lut, *this,
                                      i*itemsPerThread, (j < numberOfThreads) ? j*itemsPerThread : uint24_max+1 ) );
   if ( numberOfThreads > 1 )
   {
      for ( int i = 0; i < numberOfThreads; ++i )
         threads[i].Start( ThreadPriority::DefaultMax, useAffinity ? i : -1 );
      for ( int i = 0; i < numberOfThreads; ++i )
         threads[i].Wait();
   }
   else
      threads[0].Run();

   threads.Destroy();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class PCL_HistogramTransformationEngine
{
public:

   template <class P> static
   void Apply( GenericImage<P>& image, const HistogramTransformation& H )
   {
      if ( image.IsEmptySelection() )
         return;

      image.EnsureUnique();

      Rect r = image.SelectedRectangle();
      int h = r.Height();

      int numberOfThreads = H.IsParallelProcessingEnabled() ? Min( H.MaxProcessors(), pcl::Thread::NumberOfThreads( h, 1 ) ) : 1;
      int rowsPerThread = h/numberOfThreads;

      size_type N = image.NumberOfSelectedSamples();
      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "Histogram transformation", N );

      ThreadData<P> data( image, H, N );

      ReferenceArray<Thread<P> > threads;
      for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
         threads.Add( new Thread<P>( data, i*rowsPerThread, (j < numberOfThreads) ? j*rowsPerThread : h ) );

      AbstractImage::RunThreads( threads, data );
      threads.Destroy();

      image.Status() = data.status;
   }

   template <class P> static PCL_HOT_FUNCTION
   void Apply( GenericImage<P>& image, const typename P::sample* lut )
   {
      if ( image.IsEmptySelection() )
         return;

      Rect r = image.SelectedRectangle();

      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "LUT-based histogram transformation", image.NumberOfSelectedSamples() );

      for ( int c = image.FirstSelectedChannel(), w = r.Width(); c <= image.LastSelectedChannel(); ++c )
         for ( int y = r.y0; y < r.y1; ++y )
            for ( typename P::sample* p = image.ScanLine( y, c ) + r.x0, * pw = p + w; p < pw; ++p )
               *p = lut[*p];

      image.Status() += image.NumberOfSelectedSamples();
   }

private:

   template <class P>
   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( GenericImage<P>& a_image, const HistogramTransformation& a_transformation, size_type a_count ) :
         AbstractImage::ThreadData( a_image, a_count ),
         image( a_image ),
         transformation( a_transformation )
      {
      }

      GenericImage<P>&               image;
      const HistogramTransformation& transformation;
   };

   template <class P>
   class Thread : public pcl::Thread
   {
   public:

      Thread( ThreadData<P>& d, int startRow, int endRow ) :
         pcl::Thread(),
         m_data( d ),
         m_firstRow( startRow ), // m_firstRow, m_endRow are relative to the current image selection
         m_endRow( endRow )
      {
      }

      virtual PCL_HOT_FUNCTION void Run()
      {
         INIT_THREAD_MONITOR()

         Rect r = m_data.image.SelectedRectangle();
         size_type w = r.Width();

         for ( int c = m_data.image.FirstSelectedChannel(); c <= m_data.image.LastSelectedChannel(); ++c )
            for ( int y = r.y0+m_firstRow, y1 = r.y0+m_endRow; y < y1; ++y )
               for ( typename P::sample* p = m_data.image.PixelAddress( r.x0, y, c ), * pw = p + w; p < pw; ++p )
               {
                  double f; P::FromSample( f, *p );
                  for ( size_type j = 0; j < m_data.transformation.Length(); ++j )
                     m_data.transformation[j].Transform( f );
                  *p = P::ToSample( f );

                  UPDATE_THREAD_MONITOR( 65536 )
               }
      }

   private:

      ThreadData<P>& m_data;
      int            m_firstRow;
      int            m_endRow;
   };
};

// ----------------------------------------------------------------------------

void HistogramTransformation::Apply( pcl::Image& image ) const
{
   PCL_HistogramTransformationEngine::Apply( image, *this );
}

void HistogramTransformation::Apply( pcl::DImage& image ) const
{
   PCL_HistogramTransformationEngine::Apply( image, *this );
}

void HistogramTransformation::Apply( pcl::UInt8Image& image ) const
{
   uint8 lut[ 256 ];
   Make8BitLUT( lut );
   PCL_HistogramTransformationEngine::Apply( image, lut );
}

void HistogramTransformation::Apply( pcl::UInt16Image& image ) const
{
   uint16 lut[ 65536 ];
   Make16BitLUT( lut );
   PCL_HistogramTransformationEngine::Apply( image, lut );
}

void HistogramTransformation::Apply( pcl::UInt32Image& image ) const
{
   PCL_HistogramTransformationEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/HistogramTransformation.cpp - Released 2017-06-21T11:36:44Z
