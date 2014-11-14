// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/MorphologicalTransformation.cpp - Released 2014/11/14 17:17:00 UTC
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

#include <pcl/MorphologicalTransformation.h>
#include <pcl/MultiVector.h>
#include <pcl/Thread.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_MorphologicalTransformationEngine
{
public:

   template <class P> static
   void Apply( GenericImage<P>& image, const MorphologicalTransformation& transformation )
   {
      if ( image.IsEmptySelection() )
         return;

      image.SetUnique();

      int n = transformation.OverlappingDistance();
      if ( n > image.Height() || n > image.Width() )
      {
         image.Zero();
         return;
      }

      /*
       * Dilation requires a reflected structure. We'll unreflect it once the
       * transformation has finished.
       */
      bool didReflect = false;
      if ( transformation.Operator().IsDilation() != transformation.Structure().IsReflected() )
      {
         const_cast<StructuringElement&>( transformation.Structure() ).Reflect();
         didReflect = true;
      }

      int numberOfRows = image.SelectedRectangle().Height();
      int numberOfThreads = transformation.IsParallelProcessingEnabled() ?
               Min( transformation.MaxProcessors(), pcl::Thread::NumberOfThreads( numberOfRows, n ) ) : 1;
      int rowsPerThread = numberOfRows/numberOfThreads;

      size_type N = image.NumberOfSelectedSamples();
      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "Morphological transformation, " + transformation.Operator().Description(), N );

      ThreadData<P> data( image, transformation, N );

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

         if ( didReflect )
            const_cast<StructuringElement&>( transformation.Structure() ).Reflect();
      }
      catch ( ... )
      {
         if ( didReflect )
            const_cast<StructuringElement&>( transformation.Structure() ).Reflect();
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

private:

   template <class P>
   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( GenericImage<P>& a_image, const MorphologicalTransformation& a_transformation, size_type a_count ) :
      AbstractImage::ThreadData( a_image, a_count ),
      image( a_image ), transformation( a_transformation )
      {
      }

      GenericImage<P>& image;
      const MorphologicalTransformation& transformation;
   };

   template <class P>
   class Thread : public pcl::Thread
   {
   public:

      Thread( ThreadData<P>& m_data, int firstRow, int endRow, GenericImage<P>* upperOvRgn, GenericImage<P>* lowerOvRgn ) :
      pcl::Thread(),
      m_data( m_data ), m_firstRow( firstRow ), m_endRow( endRow ), m_upperOvRgn( upperOvRgn ), m_lowerOvRgn( lowerOvRgn )
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

         int n = m_data.transformation.OverlappingDistance();
         int n2 = n >> 1;
         int nf0 = w + (n2 << 1);
         int nh = m_data.transformation.Structure().NumberOfElements();

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

         typename P::sample th0 = P::ToSample( m_data.transformation.LowThreshold() );
         typename P::sample th1 = P::ToSample( m_data.transformation.HighThreshold() );

         bool tz0 = 1 + th0 == 1;
         bool tz1 = 1 + th1 == 1;
         bool tz = tz0 && tz1;

         DVector W( m_data.transformation.Structure().NumberOfWays() );

         GenericVector<typename P::sample> h( nh );
         GenericVector<typename P::sample> h1( nh );

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
                  typename P::sample* hi = *h;

                  for ( int i = 0; i < n; i += m_data.transformation.InterlacingDistance() )
                  {
                     const typename P::sample* fi = f0[i].At( x );
                     for ( int j = 0; j < n; j  += m_data.transformation.InterlacingDistance(),
                                             fi += m_data.transformation.InterlacingDistance() )
                        *hi++ = *fi;
                  }

                  for ( int k = 0; k < m_data.transformation.Structure().NumberOfWays(); ++k )
                     if ( m_data.transformation.Structure().IsBox( k ) )
                        W[k] = m_data.transformation.Operator()( *h, nh );
                     else
                     {
                        int nh1;
                        m_data.transformation.Structure().PeekElements( *h1, nh1, *h, k );
                        if ( nh1 > 0 )
                           W[k] = m_data.transformation.Operator()( *h1, nh1 );
                        else
                           W[k] = 0;
                     }

                  double r = 0;
                  if ( m_data.transformation.Structure().NumberOfWays() > 1 )
                     r = m_data.transformation.Operator()( *W, W.Length() );
                  else
                     r = W[0];

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
      GenericImage<P>* m_upperOvRgn;
      GenericImage<P>* m_lowerOvRgn;
   };
};

// ----------------------------------------------------------------------------

void MorphologicalTransformation::Apply( Image& image ) const
{
   Validate();
   PCL_MorphologicalTransformationEngine::Apply( image, *this );
}

void MorphologicalTransformation::Apply( DImage& image ) const
{
   Validate();
   PCL_MorphologicalTransformationEngine::Apply( image, *this );
}

void MorphologicalTransformation::Apply( UInt8Image& image ) const
{
   Validate();
   PCL_MorphologicalTransformationEngine::Apply( image, *this );
}

void MorphologicalTransformation::Apply( UInt16Image& image ) const
{
   Validate();
   PCL_MorphologicalTransformationEngine::Apply( image, *this );
}

void MorphologicalTransformation::Apply( UInt32Image& image ) const
{
   Validate();
   PCL_MorphologicalTransformationEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

void MorphologicalTransformation::Validate() const
{
   if ( m_operator == 0 || m_structure == 0 )
      throw Error( "Invalid access to uninitialized morphological transformation." );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/MorphologicalTransformation.cpp - Released 2014/11/14 17:17:00 UTC
