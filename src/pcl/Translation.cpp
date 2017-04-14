//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// pcl/Translation.cpp - Released 2017-04-14T23:04:51Z
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
#include <pcl/Translation.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_TranslationEngine
{
public:

   template <class P> static
   void Apply( GenericImage<P>& image, const Translation& translation )
   {
      if ( translation.Delta() == 0.0 )
         return;

      int width = image.Width();
      int height = image.Height();
      if ( width == 0 || height == 0 )
         return;

      image.EnsureUnique();

      typename P::sample* f = nullptr;
      typename P::sample** f0 = nullptr;

      int n = image.NumberOfChannels();
      typename GenericImage<P>::color_space cs0 = image.ColorSpace();

      StatusMonitor status = image.Status();

      int numberOfThreads = translation.IsParallelProcessingEnabled() ?
               Min( translation.MaxProcessors(), pcl::Thread::NumberOfThreads( height, 1 ) ) : 1;
      int rowsPerThread = height/numberOfThreads;

      try
      {
         size_type N = size_type( width )*size_type( height );
         if ( status.IsInitializationEnabled() )
            status.Initialize( String().Format( "Translate dx=%.3lf, dy=%.3lf, ",
                        translation.Delta().x, translation.Delta().y ) + translation.Interpolation().Description(),
                        size_type( n )*N );

         f0 = image.ReleaseData();

         for ( int c = 0; c < n; ++c )
         {
            ThreadData<P> data( translation.Delta(), width, height, status, N );

            data.f = f = image.Allocator().AllocatePixels( size_type( width )*size_type( height ) );
            data.fillValue = (c < translation.FillValues().Length()) ? P::ToSample( translation.FillValues()[c] ) : P::MinSampleValue();

            ReferenceArray<Thread<P> > threads;
            for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
               threads.Add( new Thread<P>( data,
                                           translation.Interpolation().NewInterpolator<P>( f0[c], width, height ),
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
      ThreadData( const DPoint& a_delta, int a_width, int a_height,
                  const StatusMonitor& a_status, size_type a_count ) :
         AbstractImage::ThreadData( a_status, a_count ),
         f( nullptr ), fillValue( P::MinSampleValue() ),
         delta( a_delta ), width( a_width ), height( a_height )
      {
      }

      typename P::sample* f;         // target data
      typename P::sample  fillValue; // unmapped pixel value
      DPoint              delta;
      int                 width;
      int                 height;
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
            double dy = m_data.delta.y + i;

            for ( int j = 0; j < m_data.width; ++j, ++f )
            {
               DPoint p( m_data.delta.x+j, dy );
               *f = (p.x >= 0 && p.x < m_data.width && p.y >= 0 && p.y < m_data.height) ? (*m_interpolator)( p ) : m_data.fillValue;

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

void Translation::Apply( Image& img ) const
{
   PCL_TranslationEngine::Apply( img, *this );
}

void Translation::Apply( DImage& img ) const
{
   PCL_TranslationEngine::Apply( img, *this );
}

void Translation::Apply( UInt8Image& img ) const
{
   PCL_TranslationEngine::Apply( img, *this );
}

void Translation::Apply( UInt16Image& img ) const
{
   PCL_TranslationEngine::Apply( img, *this );
}

void Translation::Apply( UInt32Image& img ) const
{
   PCL_TranslationEngine::Apply( img, *this );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Translation.cpp - Released 2017-04-14T23:04:51Z
