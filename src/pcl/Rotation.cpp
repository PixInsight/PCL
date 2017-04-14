//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/Rotation.cpp - Released 2017-04-14T23:04:51Z
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

#include <pcl/AutoPointer.h>
#include <pcl/Rotation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static inline
void GetRotatedBounds( int& width, int& height, double& x0, double& y0, const Rotation& R )
{
   DPoint p1(       0.5,        0.5 );
   DPoint p2( width-0.5,        0.5 );
   DPoint p3( width-0.5, height-0.5 );
   DPoint p4(       0.5, height-0.5 );

   double sa, ca;
   pcl::SinCos( double( R.Angle() ), sa, ca );
   pcl::Rotate( p1, sa, ca, R.Center() );
   pcl::Rotate( p2, sa, ca, R.Center() );
   pcl::Rotate( p3, sa, ca, R.Center() );
   pcl::Rotate( p4, sa, ca, R.Center() );

   x0 = pcl::Min( p1.x, pcl::Min( p2.x, pcl::Min( p3.x, p4.x ) ) );
   y0 = pcl::Min( p1.y, pcl::Min( p2.y, pcl::Min( p3.y, p4.y ) ) );

   width = 1 + RoundInt( pcl::Max( p1.x, pcl::Max( p2.x, pcl::Max( p3.x, p4.x ) ) ) - x0 );
   height = 1 + RoundInt( pcl::Max( p1.y, pcl::Max( p2.y, pcl::Max( p3.y, p4.y ) ) ) - y0 );
}

void Rotation::GetNewSizes( int& width, int& height ) const
{
   double dumx, dumy;
   GetRotatedBounds( width, height, dumx, dumy, *this );
}

// ----------------------------------------------------------------------------

class PCL_RotationEngine
{
public:

   template <class P> static
   void Apply( GenericImage<P>& image, const Rotation& rotation )
   {
      if ( rotation.Angle() == 0 )
         return;

      int width = image.Width();
      int height = image.Height();
      if ( width == 0 || height == 0 )
         return;

      int w0 = width;
      int h0 = height;
      double x0, y0;
      GetRotatedBounds( width, height, x0, y0, rotation );

      image.EnsureUnique();

      typename P::sample* f = nullptr;
      typename P::sample** f0 = nullptr;

      int n = image.NumberOfChannels();
      typename GenericImage<P>::color_space cs0 = image.ColorSpace();

      double sa, ca;
      pcl::SinCos( double( -rotation.Angle() ), sa, ca );

      DPoint center = rotation.Center();

      StatusMonitor status = image.Status();

      int numberOfThreads = rotation.IsParallelProcessingEnabled() ?
               Min( rotation.MaxProcessors(), pcl::Thread::NumberOfThreads( height, 1 ) ) : 1;
      int rowsPerThread = height/numberOfThreads;

      try
      {
         size_type N = size_type( width )*size_type( height );
         if ( status.IsInitializationEnabled() )
            status.Initialize( String().Format( "Rotate %.3f deg, ",
                        pcl::Deg( rotation.Angle() ) ) + rotation.Interpolation().Description(),
                        size_type( n )*N );

         f0 = image.ReleaseData();

         for ( int c = 0; c < n; ++c )
         {
            ThreadData<P> data( sa, ca, center, DPoint( x0, y0 ), w0, h0, width, status, N );

            data.f = f = image.Allocator().AllocatePixels( size_type( width )*size_type( height ) );
            data.fillValue = (c < rotation.FillValues().Length()) ? P::ToSample( rotation.FillValues()[c] ) : P::MinSampleValue();

            ReferenceArray<Thread<P> > threads;
            for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
               threads.Add( new Thread<P>( data,
                                           rotation.Interpolation().NewInterpolator<P>( f0[c], w0, h0 ),
                                           i*rowsPerThread,
                                           (j < numberOfThreads) ? j*rowsPerThread : height ) );

            AbstractImage::RunThreads( threads, data );

            threads.Destroy();

            image.Allocator().Deallocate( f0[c] );
            f0[c] = f;
            f = nullptr;

            status = data.status;
         }

         image.ImportData( f0, width, height, n, cs0 ).Status() = status;
      }
      catch ( ... )
      {
         if ( f != nullptr )
            image.Allocator().Deallocate( f );
         if ( f0 != nullptr )
         {
            for ( int c = 0; c < n; ++c )
               if ( f0[c] != nullptr )
                  image.Allocator().Deallocate( f0[c] );
            image.Allocator().Deallocate( f0 );
         }
         image.FreeData();
         throw;
      }
   }

private:

   template <class P>
   struct ThreadData : public AbstractImage::ThreadData
   {
      ThreadData( double sinA, double cosA, const DPoint& a_center, const DPoint& a_origin,
                  int urWidth, int urHeight, int width, const StatusMonitor& a_status, size_type a_count ) :
         AbstractImage::ThreadData( a_status, a_count ),
         f( nullptr ), fillValue( P::MinSampleValue() ),
         sa( sinA ), ca( cosA ), center( a_center ), origin( a_origin ),
         w0( urWidth ), h0( urHeight ), width( width )
      {
      }

      typename P::sample* f;         // target data
      typename P::sample  fillValue; // unmapped pixel value
      double              sa, ca;    // sine and cosine of rotation angle
      DPoint              center;
      DPoint              origin;
      int                 w0;        // unrotated width
      int                 h0;        // unrotated height
      int                 width;     // rotated width
   };

   template <class P>
   class Thread : public pcl::Thread
   {
   public:

      typedef PixelInterpolation::Interpolator<P>  interpolator_type;

      Thread( ThreadData<P>& data, interpolator_type* interpolator, int firstRow, int endRow ) :
         pcl::Thread(),
         m_data( data ), m_firstRow( firstRow ), m_endRow( endRow ), m_interpolator( interpolator )
      {
      }

      virtual PCL_HOT_FUNCTION void Run()
      {
         INIT_THREAD_MONITOR()

         typename P::sample* f = m_data.f + m_firstRow*m_data.width;

         for ( int i = m_firstRow; i < m_endRow; ++i )
         {
            double oy = m_data.origin.y + i;

            for ( int j = 0; j < m_data.width; ++j, ++f )
            {
               DPoint p( m_data.origin.x+j, oy );
               Rotate( p, m_data.sa, m_data.ca, m_data.center.x, m_data.center.y );
               *f = (p.x >= 0 && p.x < m_data.w0 && p.y >= 0 && p.y < m_data.h0) ? (*m_interpolator)( p ) : m_data.fillValue;

               UPDATE_THREAD_MONITOR( 65536 )
            }
         }
      }

   private:

      ThreadData<P>&                 m_data;
      int                            m_firstRow;
      int                            m_endRow;
      AutoPointer<interpolator_type> m_interpolator;
   };
};

// ----------------------------------------------------------------------------

void Rotation::Apply( pcl::Image& image ) const
{
   PCL_RotationEngine::Apply( image, *this );
}

void Rotation::Apply( pcl::DImage& image ) const
{
   PCL_RotationEngine::Apply( image, *this );
}

void Rotation::Apply( pcl::UInt8Image& image ) const
{
   PCL_RotationEngine::Apply( image, *this );
}

void Rotation::Apply( pcl::UInt16Image& image ) const
{
   PCL_RotationEngine::Apply( image, *this );
}

void Rotation::Apply( pcl::UInt32Image& image ) const
{
   PCL_RotationEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Rotation.cpp - Released 2017-04-14T23:04:51Z
