//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/Resample.cpp - Released 2017-08-01T14:23:38Z
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
#include <pcl/Resample.h>

namespace pcl
{

// ----------------------------------------------------------------------------

void Resample::GetNewSizes( int& width, int& height ) const
{
   switch ( m_mode )
   {
   default:
   case ResizeMode::RelativeDimensions:
      width  = RoundInt( m_xSize * width );
      height = RoundInt( m_ySize * height );
      break;

   case ResizeMode::AbsolutePixels:
   case ResizeMode::AbsoluteCentimeters:
   case ResizeMode::AbsoluteInches:
      {
         double x = m_xSize;
         double y = m_ySize;

         if ( m_mode != ResizeMode::AbsolutePixels )
         {
            x *= xResolution;
            y *= yResolution;

            if ( m_mode == ResizeMode::AbsoluteCentimeters )
            {
               if ( !metric ) // inch -> cm
               {
                  x /= 2.54F;
                  y /= 2.54F;
               }
            }
            else // inches
            {
               if ( metric ) // cm -> inch
               {
                  x *= 2.54F;
                  y *= 2.54F;
               }
            }
         }

         switch ( m_absMode )
         {
         case AbsoluteResizeMode::ForceWidthAndHeight:
            width  = RoundInt( x );
            height = RoundInt( y );
            break;
         case AbsoluteResizeMode::ForceWidth:
            if ( width != 0 && height != 0 )
            {
               double r = double( width )/height;
               width  = RoundInt( x );
               height = RoundInt( width/r );
            }
            else
               width = RoundInt( x );
            break;
         case AbsoluteResizeMode::ForceHeight:
            if ( width != 0 && height != 0 )
            {
               double r = double( height )/width;
               height = RoundInt( y );
               width  = RoundInt( height/r );
            }
            else
               height = RoundInt( y );
            break;
         default:
            break;
         }
      }
      break;

   case ResizeMode::ForceArea:
      if ( width != 0 && height != 0 )
      {
         double k = double( width )/height;
         width  = RoundInt( Sqrt( m_xSize*k ) );   // m_ySize not used
         height = RoundInt( Sqrt( m_xSize/k ) );
      }
      break;
   }
}

// ----------------------------------------------------------------------------

#if !defined( _MSC_VER ) && !defined( __clang__ )
#pragma GCC push_options
#pragma GCC optimize ("O2")
#endif

class PCL_ResampleEngine
{
public:

   template <class P> static
   void Apply( GenericImage<P>& image, const Resample& R )
   {
      int width = image.Width();
      int w0 = width;
      int height = image.Height();
      int h0 = height;

      R.GetNewSizes( width, height );

      if ( width == w0 && height == h0 )
         return;

      if ( width <= 0 || height <= 0 )
      {
         image.FreeData();
         return;
      }

      image.EnsureUnique();

      typename P::sample* f = nullptr;
      typename P::sample** f0 = nullptr;

      int n = image.NumberOfChannels();
      typename GenericImage<P>::color_space cs0 = image.ColorSpace();

      double rx = double( w0 )/width;
      double ry = double( h0 )/height;

      StatusMonitor status = image.Status();

      int numberOfThreads = R.IsParallelProcessingEnabled() ?
               Min( R.MaxProcessors(), pcl::Thread::NumberOfThreads( height, 1 ) ) : 1;
      int rowsPerThread = height/numberOfThreads;

      try
      {
         size_type N = size_type( width )*size_type( height );
         if ( status.IsInitializationEnabled() )
            status.Initialize( String().Format( "Resampling to %dx%d px, ", width, height )
                                                + R.Interpolation().Description(), size_type( n )*N );
         f0 = image.ReleaseData();

         for ( int c = 0; c < n; ++c )
         {
            ThreadData<P> data( rx, ry, width, status, N );
            data.f = f = image.Allocator().AllocatePixels( width, height );

            ReferenceArray<Thread<P> > threads;
            for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
               threads.Add( new Thread<P>( data, R.Interpolation().NewInterpolator<P>( f0[c], w0, h0, R.UsingUnclippedInterpolation() ),
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
      ThreadData( double a_xRatio, double a_yRatio, int a_width, const StatusMonitor& a_status, size_type a_count ) :
         AbstractImage::ThreadData( a_status, a_count ),
         xRatio( a_xRatio ), yRatio( a_yRatio ), width( a_width )
      {
      }

      typename P::sample* f = nullptr;
               double     xRatio;
               double     yRatio;
               int        width;
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

         typename P::sample* f = m_data.f + size_type( m_firstRow )*size_type( m_data.width );

         for ( int i = m_firstRow; i < m_endRow; ++i )
         {
            double i_ry = i*m_data.yRatio;

            for ( int j = 0; j < m_data.width; ++j, ++f )
            {
               *f = (*m_interpolator)( j*m_data.xRatio, i_ry );
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

#if !defined( _MSC_VER ) && !defined( __clang__ )
#pragma GCC pop_options
#endif

// ----------------------------------------------------------------------------

void Resample::Apply( pcl::Image& image ) const
{
   PCL_ResampleEngine::Apply( image, *this );
}

void Resample::Apply( pcl::DImage& image ) const
{
   PCL_ResampleEngine::Apply( image, *this );
}

void Resample::Apply( pcl::UInt8Image& image ) const
{
   PCL_ResampleEngine::Apply( image, *this );
}

void Resample::Apply( pcl::UInt16Image& image ) const
{
   PCL_ResampleEngine::Apply( image, *this );
}

void Resample::Apply( pcl::UInt32Image& image ) const
{
   PCL_ResampleEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Resample.cpp - Released 2017-08-01T14:23:38Z
