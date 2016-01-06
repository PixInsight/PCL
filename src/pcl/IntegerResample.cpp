//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// pcl/IntegerResample.cpp - Released 2015/12/17 18:52:18 UTC
// ----------------------------------------------------------------------------
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include <pcl/IntegerResample.h>
#include <pcl/Selection.h>

// ----------------------------------------------------------------------------

namespace pcl
{

// ----------------------------------------------------------------------------

void IntegerResample::GetNewSizes( int& width, int& height ) const
{
   int z = pcl::Abs( m_zoomFactor );
   if ( z > 1 )
      if ( m_zoomFactor > 0 )
         width *= z, height *= z;
      else
         width /= z, height /= z;
}

// ----------------------------------------------------------------------------

class PCL_IntegerResampleEngine
{
public:

   template <class P> inline
   static void Apply( GenericImage<P>& image, const IntegerResample& Z )
   {
      int width = image.Width();
      int w0 = width;
      int height = image.Height();
      int h0 = height;

      Z.GetNewSizes( width, height );

      if ( width == w0 && height == h0 )
         return;

      if ( width == 0 || height == 0 )
      {
         image.FreeData();
         return;
      }

      image.EnsureUnique();

      typename P::sample* f = 0;
      typename P::sample** f0 = 0;

      int n = image.NumberOfChannels();
      size_type N = image.NumberOfPixels();
      typename GenericImage<P>::color_space cs0 = image.ColorSpace();

      StatusMonitor status = image.Status();

      int z = pcl::Abs( Z.ZoomFactor() );
      int z2 = z*z;
      int n2 = z2 >> 1;

      try
      {
         if ( status.IsInitializationEnabled() )
         {
            String info = (Z.ZoomFactor() > 0) ? "Upsampling" : "Downsampling";

            info.AppendFormat( " %d:%d, %dx%d",
               (Z.ZoomFactor() > 0) ? z : 1, (Z.ZoomFactor() > 0) ? 1 : z, width, height );

            if ( Z.ZoomFactor() < 0 )
            {
               info += ", ";
               switch ( Z.DownsampleMode() )
               {
               default:
               case IntegerDownsampleMode::Average: info += "average"; break;
               case IntegerDownsampleMode::Median:  info += "median"; break;
               case IntegerDownsampleMode::Maximum: info += "maximum"; break;
               case IntegerDownsampleMode::Minimum: info += "minimum"; break;
               }
            }

            status.Initialize( info, n*N );
         }

         GenericVector<typename P::sample> fm;
         if ( Z.ZoomFactor() < 0 && Z.DownsampleMode() == IntegerDownsampleMode::Median )
            fm = GenericVector<typename P::sample>( z2 );

         f0 = image.ReleaseData();

         for ( int c = 0; c < n; ++c, status += N )
         {
            f = image.Allocator().AllocatePixels( width, height );

            if ( Z.ZoomFactor() > 0 )
            {
               const typename P::sample* f0c = f0[c];

               for ( int y = 0; y < h0; ++y )
               {
                  int yz = y*z;

                  for ( int x = 0; x < w0; ++x )
                  {
                     int xz = x*z;
                     typename P::sample v = *f0c++;

                     for ( int i = 0; i < z; ++i )
                     {
                        typename P::sample* fi = f + (size_type( yz + i )*width + xz);

                        for ( int j = 0; j < z; ++j )
                           *fi++ = v;
                     }
                  }
               }
            }
            else
            {
               typename P::sample* fz = f;

               for ( int y = 0; y < height; ++y )
               {
                  const typename P::sample* fy = f0[c] + size_type( y )*z*w0;

                  for ( int x = 0; x < width; ++x )
                  {
                     const typename P::sample* fyx = fy + x*z;

                     switch ( Z.DownsampleMode() )
                     {
                     default:
                     case IntegerDownsampleMode::Average:
                        {
                           double s = 0;
                           for ( int i = 0; i < z; ++i, fyx += w0 )
                              for ( int j = 0; j < z; ++j )
                                 s += fyx[j];
                           *fz++ = typename P::sample( P::IsFloatSample() ? s/z2 : Round( s/z2 ) );
                        }
                        break;

                     case IntegerDownsampleMode::Median:
                        {
                           typename P::sample* fmi = *fm;
                           for ( int i = 0; i < z; ++i, fyx += w0 )
                              for ( int j = 0; j < z; ++j )
                                 *fmi++ = fyx[j];

                           *fz++ = (z & 1) ?
                                 *Select( *fm, fm.At( z2 ), n2 ) :
                                 P::FloatToSample( 0.5*(double( *Select( *fm, fm.At( z2 ), n2   ) ) +
                                                        double( *Select( *fm, fm.At( z2 ), n2-1 ) )) );
                        }
                        break;

                     case IntegerDownsampleMode::Maximum:
                        {
                           *fz = P::MinSampleValue();
                           for ( int i = 0; i < z; ++i, fyx += w0 )
                              for ( int j = 0; j < z; ++j )
                                 if ( fyx[j] > *fz )
                                    *fz = fyx[j];
                           ++fz;
                        }
                        break;

                     case IntegerDownsampleMode::Minimum:
                        {
                           *fz = P::MaxSampleValue();
                           for ( int i = 0; i < z; ++i, fyx += w0 )
                              for ( int j = 0; j < z; ++j )
                                 if ( fyx[j] < *fz )
                                    *fz = fyx[j];
                           ++fz;
                        }
                        break;
                     }
                  }
               }
            }

            image.Allocator().Deallocate( f0[c] );
            f0[c] = f;
            f = 0;
         }

         image.ImportData( f0, width, height, n, cs0 ).Status() = status;
      }
      catch ( ... )
      {
         if ( f != 0 )
            image.Allocator().Deallocate( f );
         if ( f0 != 0 )
         {
            for ( int c = 0; c < n; ++c )
               if ( f0[c] != 0 )
                  image.Allocator().Deallocate( f0[c] );
            image.Allocator().Deallocate( f0 );
         }
         image.FreeData();
         throw;
      }
   }
};

// ----------------------------------------------------------------------------

void IntegerResample::Apply( Image& image ) const
{
   PCL_IntegerResampleEngine::Apply( image, *this );
}

void IntegerResample::Apply( DImage& image ) const
{
   PCL_IntegerResampleEngine::Apply( image, *this );
}

void IntegerResample::Apply( UInt8Image& image ) const
{
   PCL_IntegerResampleEngine::Apply( image, *this );
}

void IntegerResample::Apply( UInt16Image& image ) const
{
   PCL_IntegerResampleEngine::Apply( image, *this );
}

void IntegerResample::Apply( UInt32Image& image ) const
{
   PCL_IntegerResampleEngine::Apply( image, *this );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/IntegerResample.cpp - Released 2015/12/17 18:52:18 UTC
