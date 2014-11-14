// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/FastRotation.cpp - Released 2014/11/14 17:17:00 UTC
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

#include <pcl/FastRotation.h>

// ----------------------------------------------------------------------------

namespace pcl
{

// ----------------------------------------------------------------------------

class PCL_FastRotationEngine
{
public:

   template <class P> static
   void Rotate180( GenericImage<P>& image )
   {
      size_type N = image.NumberOfPixels();
      int n = image.NumberOfChannels();

      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "Rotate 180 degrees", n*N );

      for ( int c = 0; c < n; ++c, image.Status() += N )
         for ( int y0 = 0, y1 = image.Height()-1; y0 <= y1; ++y0, --y1 )
         {
            typename P::sample* f0 = image.ScanLine( y0, c );
            typename P::sample* f1 = image.ScanLine( y1, c );

            if ( y0 != y1 )
            {
               int x0 = 0, x1 = image.Width()-1;

               while ( x0 < x1 )
               {
                  pcl::Swap( f0[x0], f1[x1] );
                  pcl::Swap( f0[x1], f1[x0] );
                  ++x0;
                  --x1;
               }

               if ( x0 == x1 )
                  pcl::Swap( f0[x0], f1[x0] );
            }
            else
               for ( typename P::sample* f = f0, * g = f0+image.Width()-1; f < g; )
                  pcl::Swap( *f++, *g-- );
         }
   }

   template <class P> static
   void Rotate90CW( GenericImage<P>& image )
   {
      image.SetUnique();

      int w = image.Width();
      int h = image.Height();
      int h1 = h - 1;
      int n = image.NumberOfChannels();
      size_type N = image.NumberOfPixels();
      typename GenericImage<P>::color_space cs0 = image.ColorSpace();
      StatusMonitor status = image.Status();

      typename P::sample** f0 = 0;

      try
      {
         if ( image.Status().IsInitializationEnabled() )
            status.Initialize( "Rotate 90 degrees, clockwise", n*N );

         f0 = image.ReleaseData();

         typename GenericImage<P>::sample_array tmp( N );

         for ( int c = 0; c < n; ++c, status += N )
         {
            typename P::sample* f = f0[c];
            typename P::sample* t = tmp.Begin();
            ::memcpy( t, f, N*P::BytesPerSample() );
            for ( int y = 0; y < h; ++y )
               for ( int x = 0, h1y = h1-y; x < w; ++x, ++t )
                  f[x*h + h1y] = *t;
         }

         image.ImportData( f0, h, w, n, cs0 ).Status() = status;
      }
      catch ( ... )
      {
         if ( f0 != 0 )
         {
            for ( int c = 0; c < n; ++c )
               if ( f0[c] != 0 )
                  image.Allocator().Deallocate( f0[c] );
            image.Allocator().Deallocate( f0 );
            image.FreeData();
         }
         throw;
      }
   }

   template <class P> static
   void Rotate90CCW( GenericImage<P>& image )
   {
      image.SetUnique();

      int w = image.Width();
      int h = image.Height();
      int w1 = w - 1;
      int n = image.NumberOfChannels();
      size_type N = image.NumberOfPixels();
      typename GenericImage<P>::color_space cs0 = image.ColorSpace();
      StatusMonitor status = image.Status();

      typename P::sample** f0 = 0;

      try
      {
         if ( image.Status().IsInitializationEnabled() )
            status.Initialize( "Rotate 90 degrees, counter-clockwise", n*N );

         typename GenericImage<P>::sample_array tmp( N );

         f0 = image.ReleaseData();

         for ( int c = 0; c < n; ++c, status += N )
         {
            typename P::sample* f = f0[c];
            typename P::sample* t = tmp.Begin();
            ::memcpy( t, f, N*P::BytesPerSample() );
            for ( int y = 0; y < h; ++y )
               for ( int x = 0; x < w; ++x, ++t )
                  f[(w1-x)*h + y] = *t;
         }

         image.ImportData( f0, h, w, n, cs0 ).Status() = status;
      }
      catch ( ... )
      {
         if ( f0 != 0 )
         {
            for ( int c = 0; c < n; ++c )
               if ( f0[c] != 0 )
                  image.Allocator().Deallocate( f0[c] );
            image.Allocator().Deallocate( f0 );
            image.FreeData();
         }
         throw;
      }
   }

   template <class P> static
   void HorizontalMirror( GenericImage<P>& image )
   {
      size_type N = image.NumberOfPixels();
      int n = image.NumberOfChannels();

      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "Horizontal mirror", n*N );

      for ( int c = 0; c < n; ++c, image.Status() += N )
         for ( int y = 0; y < image.Height(); ++y )
            for ( typename P::sample* f = image.ScanLine( y, c ),
                                    * g = f + image.Width()-1; f < g; )
            {
               pcl::Swap( *f++, *g-- );
            }
   }

   template <class P> static
   void VerticalMirror( GenericImage<P>& image )
   {
      size_type N = image.NumberOfPixels();
      int n = image.NumberOfChannels();

      if ( image.Status().IsInitializationEnabled() )
         image.Status().Initialize( "Vertical mirror", n*N );

      for ( int c = 0; c < n; ++c, image.Status() += N )
         for ( int y0 = 0, y1 = image.Height()-1; y0 < y1; ++y0, --y1 )
            for ( typename P::sample* f0 = image.ScanLine( y0, c ),
                                    * f1 = image.ScanLine( y1, c ),
                                    * fw = f0 + image.Width();
                  f0 < fw; )
            {
               pcl::Swap( *f0++, *f1++ );
            }
   }
};

// ----------------------------------------------------------------------------

void Rotate180::Apply( pcl::Image& image ) const
{
   PCL_FastRotationEngine::Rotate180( image );
}

void Rotate180::Apply( pcl::DImage& image ) const
{
   PCL_FastRotationEngine::Rotate180( image );
}

void Rotate180::Apply( pcl::ComplexImage& image ) const
{
   PCL_FastRotationEngine::Rotate180( image );
}

void Rotate180::Apply( pcl::DComplexImage& image ) const
{
   PCL_FastRotationEngine::Rotate180( image );
}

void Rotate180::Apply( pcl::UInt8Image& image ) const
{
   PCL_FastRotationEngine::Rotate180( image );
}

void Rotate180::Apply( pcl::UInt16Image& image ) const
{
   PCL_FastRotationEngine::Rotate180( image );
}

void Rotate180::Apply( pcl::UInt32Image& image ) const
{
   PCL_FastRotationEngine::Rotate180( image );
}

// ----------------------------------------------------------------------------

void Rotate90CW::Apply( pcl::Image& image ) const
{
   PCL_FastRotationEngine::Rotate90CW( image );
}

void Rotate90CW::Apply( pcl::DImage& image ) const
{
   PCL_FastRotationEngine::Rotate90CW( image );
}

void Rotate90CW::Apply( pcl::ComplexImage& image ) const
{
   PCL_FastRotationEngine::Rotate90CW( image );
}

void Rotate90CW::Apply( pcl::DComplexImage& image ) const
{
   PCL_FastRotationEngine::Rotate90CW( image );
}

void Rotate90CW::Apply( pcl::UInt8Image& image ) const
{
   PCL_FastRotationEngine::Rotate90CW( image );
}

void Rotate90CW::Apply( pcl::UInt16Image& image ) const
{
   PCL_FastRotationEngine::Rotate90CW( image );
}

void Rotate90CW::Apply( pcl::UInt32Image& image ) const
{
   PCL_FastRotationEngine::Rotate90CW( image );
}

// ----------------------------------------------------------------------------

void Rotate90CCW::Apply( pcl::Image& image ) const
{
   PCL_FastRotationEngine::Rotate90CCW( image );
}

void Rotate90CCW::Apply( pcl::DImage& image ) const
{
   PCL_FastRotationEngine::Rotate90CCW( image );
}

void Rotate90CCW::Apply( pcl::ComplexImage& image ) const
{
   PCL_FastRotationEngine::Rotate90CCW( image );
}

void Rotate90CCW::Apply( pcl::DComplexImage& image ) const
{
   PCL_FastRotationEngine::Rotate90CCW( image );
}

void Rotate90CCW::Apply( pcl::UInt8Image& image ) const
{
   PCL_FastRotationEngine::Rotate90CCW( image );
}

void Rotate90CCW::Apply( pcl::UInt16Image& image ) const
{
   PCL_FastRotationEngine::Rotate90CCW( image );
}

void Rotate90CCW::Apply( pcl::UInt32Image& image ) const
{
   PCL_FastRotationEngine::Rotate90CCW( image );
}

// ----------------------------------------------------------------------------

void HorizontalMirror::Apply( pcl::Image& image ) const
{
   PCL_FastRotationEngine::HorizontalMirror( image );
}

void HorizontalMirror::Apply( pcl::DImage& image ) const
{
   PCL_FastRotationEngine::HorizontalMirror( image );
}

void HorizontalMirror::Apply( pcl::ComplexImage& image ) const
{
   PCL_FastRotationEngine::HorizontalMirror( image );
}

void HorizontalMirror::Apply( pcl::DComplexImage& image ) const
{
   PCL_FastRotationEngine::HorizontalMirror( image );
}

void HorizontalMirror::Apply( pcl::UInt8Image& image ) const
{
   PCL_FastRotationEngine::HorizontalMirror( image );
}

void HorizontalMirror::Apply( pcl::UInt16Image& image ) const
{
   PCL_FastRotationEngine::HorizontalMirror( image );
}

void HorizontalMirror::Apply( pcl::UInt32Image& image ) const
{
   PCL_FastRotationEngine::HorizontalMirror( image );
}

// ----------------------------------------------------------------------------

void VerticalMirror::Apply( pcl::Image& image ) const
{
   PCL_FastRotationEngine::VerticalMirror( image );
}

void VerticalMirror::Apply( pcl::DImage& image ) const
{
   PCL_FastRotationEngine::VerticalMirror( image );
}

void VerticalMirror::Apply( pcl::ComplexImage& image ) const
{
   PCL_FastRotationEngine::VerticalMirror( image );
}

void VerticalMirror::Apply( pcl::DComplexImage& image ) const
{
   PCL_FastRotationEngine::VerticalMirror( image );
}

void VerticalMirror::Apply( pcl::UInt8Image& image ) const
{
   PCL_FastRotationEngine::VerticalMirror( image );
}

void VerticalMirror::Apply( pcl::UInt16Image& image ) const
{
   PCL_FastRotationEngine::VerticalMirror( image );
}

void VerticalMirror::Apply( pcl::UInt32Image& image ) const
{
   PCL_FastRotationEngine::VerticalMirror( image );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/FastRotation.cpp - Released 2014/11/14 17:17:00 UTC
