//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.05.0841
// ----------------------------------------------------------------------------
// pcl/PolarTransform.cpp - Released 2017-06-17T10:55:56Z
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
#include <pcl/PolarTransform.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_PolarTransformEngine
{
public:

   template <class P> static
   void Apply( GenericImage<P>& image, const PolarTransformBase& T )
   {
      if ( image.IsEmpty() )
         return;

      int width = image.Width();
      int height = image.Height();

      /*
       * Center of rotation = geometric center of the image.
       */
      DPoint center( 0.5*width, 0.5*height );

      /*
       * Angle increment in radians for one pixel on the vertical axis.
       *
       * The range from the initial to the final angles is linearly covered by
       * the vertical size of the polar or log-polar matrix.
       */
      double dA = (T.FinalRotation() - T.InitialRotation())/height;

      /*
       * Distance increment for one pixel on the horizontal axis.
       *
       * Polar transform:
       *    Distances from 0 to the image diagonal are covered linearly on the
       *    horizontal axis of the transform.
       *
       * Log-polar transform:
       *    Distances from 0 to the image width are covered logarithmically on
       *    the horizontal axis of the transform.
       *
       * For the log-polar transform we can use an arbitrary logarithm base,
       * but the same base must be used when computing scale ratios by looking
       * at horizontal coordinates of log-polar matrices. We use natural
       * logarithms since they are slightly faster than base-10 logarithms.
       */
      double dR = T.IsLogPolar() ? Exp( Ln( double( width ) )/width ) :
                                   Sqrt( center.x*center.x + center.y*center.y )/width;

      /*
       * Precalculate sines and cosines for all angular steps.
       */
      DVector sin( height );
      DVector cos( height );
      for ( int i = 0; i < height; ++i )
         SinCos( T.InitialRotation() + i*dA, sin[i], cos[i] );

      /*
       * Precalculate radial distances for all radial distance steps.
       */
      DVector r( width );
      if ( T.IsLogPolar() )
         for ( int i = 0; i < width; ++i )
            r[i] = Pow( dR, double( i ) ) - 1;
      else
         for ( int i = 0; i < width; ++i )
            r[i] = i*dR;

      /*
       * In-place polar or log-polar transform.
       */

      int numberOfThreads = T.IsParallelProcessingEnabled() ?
               Min( T.MaxProcessors(), pcl::Thread::NumberOfThreads( height, 1 ) ) : 1;
      int rowsPerThread = height/numberOfThreads;

      size_type N = size_type( width )*size_type( height );
      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( String( "In-place " )
                                  + (T.IsLogPolar() ? "log-polar" : "polar")
                                  + " coordinate transform, "
                                  + T.Interpolation().Description(), image.NumberOfSelectedChannels()*N );

      typename GenericImage<P>::sample_array result( N );

      for ( int c = image.FirstSelectedChannel(); c <= image.LastSelectedChannel(); ++c )
      {
         ThreadData<P> data( result, sin, cos, r, center, width, height, image.Status(), N );

         ReferenceArray<Thread<P> > threads;
         for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
            threads.Add( new Thread<P>( data,
                                        T.Interpolation().NewInterpolator<P>( image[c], width, height ),
                                        i*rowsPerThread,
                                        (j < numberOfThreads) ? j*rowsPerThread : height ) );

         AbstractImage::RunThreads( threads, data );

         threads.Destroy();

         ::memcpy( image[c], result.Begin(), image.ChannelSize() );

         image.Status() = data.status;
      }
   }

private:

   template <class P>
   struct ThreadData : public AbstractImage::ThreadData
   {
      typedef typename GenericImage<P>::sample_array sample_array;

      ThreadData( sample_array& a_result,
                  const DVector& a_sin, const DVector& a_cos, const DVector& a_r,
                  const DPoint& a_center, int a_width, int a_height,
                  const StatusMonitor& a_status, size_type a_count ) :
         AbstractImage::ThreadData( a_status, a_count ),
         result( a_result ),
      sin( a_sin ), cos( a_cos ), r( a_r ), center( a_center ), width( a_width ), height( a_height )
      {
      }

            sample_array& result;
      const DVector&      sin;
      const DVector&      cos;
      const DVector&      r;
      const DPoint&       center;
      const int           width;
      const int           height;
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

      virtual void Run()
      {
         INIT_THREAD_MONITOR()

         typename P::sample* f = m_data.result.At( m_firstRow*m_data.width );

         for ( int i = m_firstRow; i < m_endRow; ++i )
         {
            // The polar angle corresponding to this row.
            double sa = m_data.sin[i];
            double ca = m_data.cos[i];

            for ( int j = 0; j < m_data.width; ++j )
            {
               // The distance to the rotation center corresponding to this column.
               double r = m_data.r[j];

               // Source pixel coordinates.
               double x = m_data.center.x + r*ca;
               double y = m_data.center.y - r*sa;

               // Interpolate source pixels if source coordinates fall within
               // the source image; set to zero otherwise.
               *f++ = (x >= 0 && x < m_data.width && y >= 0 && y < m_data.height) ?
                              (*m_interpolator)( x, y ) : typename P::sample( 0 );

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

void PolarTransform::Apply( pcl::Image& image ) const
{
   PCL_PolarTransformEngine::Apply( image, *this );
}

void PolarTransform::Apply( pcl::DImage& image ) const
{
   PCL_PolarTransformEngine::Apply( image, *this );
}

void PolarTransform::Apply( pcl::UInt8Image& image ) const
{
   PCL_PolarTransformEngine::Apply( image, *this );
}

void PolarTransform::Apply( pcl::UInt16Image& image ) const
{
   PCL_PolarTransformEngine::Apply( image, *this );
}

void PolarTransform::Apply( pcl::UInt32Image& image ) const
{
   PCL_PolarTransformEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

void LogPolarTransform::Apply( pcl::Image& image ) const
{
   PCL_PolarTransformEngine::Apply( image, *this );
}

void LogPolarTransform::Apply( pcl::DImage& image ) const
{
   PCL_PolarTransformEngine::Apply( image, *this );
}

void LogPolarTransform::Apply( pcl::UInt8Image& image ) const
{
   PCL_PolarTransformEngine::Apply( image, *this );
}

void LogPolarTransform::Apply( pcl::UInt16Image& image ) const
{
   PCL_PolarTransformEngine::Apply( image, *this );
}

void LogPolarTransform::Apply( pcl::UInt32Image& image ) const
{
   PCL_PolarTransformEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/PolarTransform.cpp - Released 2017-06-17T10:55:56Z
