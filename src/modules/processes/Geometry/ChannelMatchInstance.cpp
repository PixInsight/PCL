// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard Geometry Process Module Version 01.01.00.0247
// ****************************************************************************
// ChannelMatchInstance.cpp - Released 2014/11/14 17:18:46 UTC
// ****************************************************************************
// This file is part of the standard Geometry PixInsight module.
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

#include "ChannelMatchInstance.h"
#include "ChannelMatchParameters.h"

#include <pcl/AutoPointer.h>
#include <pcl/AutoViewLock.h>
#include <pcl/ImageWindow.h>
#include <pcl/PixelInterpolation.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ChannelMatchInstance::ChannelMatchInstance( const MetaProcess* P ) :
ProcessImplementation( P )
{
   for ( int i = 0; i < 3; ++i )
   {
      p_channelEnabled[i] = true;
      p_channelOffset[i] = 0.0F;
      p_channelFactor[i] = 1.0;
   }
}

ChannelMatchInstance::ChannelMatchInstance( const ChannelMatchInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

bool ChannelMatchInstance::Validate( String& info )
{
   int n = 0;
   for ( int i = 0; i < 3; ++i )
      if ( p_channelEnabled[i] )
         ++n;

   if ( n == 0 )
   {
      info = "No channel selected for matching";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

void ChannelMatchInstance::Assign( const ProcessImplementation& p )
{
   const ChannelMatchInstance* x = dynamic_cast<const ChannelMatchInstance*>( &p );
   if ( x != 0 )
      for ( int i = 0; i < 3; ++i )
      {
         p_channelEnabled[i] = x->p_channelEnabled[i];
         p_channelOffset[i] = x->p_channelOffset[i];
         p_channelFactor[i] = x->p_channelFactor[i];
      }
}

// ----------------------------------------------------------------------------

bool ChannelMatchInstance::CanExecuteOn( const View& v, String& whyNot ) const
{
   if ( v.Image().IsComplexSample() )
   {
      whyNot = "ChannelMatch cannot be executed on complex images.";
      return false;
   }

   if ( v.Image().ColorSpace() == ColorSpace::Gray )
   {
      whyNot = "ChannelMatch cannot be executed on grayscale images.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

class ChannelMatchEngine
{
public:

   template <class P>
   static void Apply( GenericImage<P>& img, const ChannelMatchInstance& C )
   {
      typename P::sample* f = 0;
      typename P::sample** f0 = 0;

      int w = img.Width();
      int h = img.Height();

      int n = img.NumberOfChannels();

      typename GenericImage<P>::color_space cs0 = img.ColorSpace();

      size_type N = img.NumberOfPixels();

      StatusMonitor status = img.Status();

      try
      {
         f0 = img.ReleaseData();

         for ( int c = 0; c < 3; ++c )
            if ( C.p_channelEnabled[c] )
            {
               const FPoint& d = C.p_channelOffset[c];
               double k = C.p_channelFactor[c];

               if ( d != 0.0F )
               {
                  status.Initialize( String().Format( "Applying translation, channel #%d, dx=%+.2f, dy=%+.2f", c, d.x, d.y ), N );

                  f = img.Allocator().AllocatePixels( N );
                  typename P::sample* fij = f;

                  const typename P::sample* f0c = f0[c];

                  BicubicSplinePixelInterpolation interpolation;
                  AutoPointer<PixelInterpolation::Interpolator<P> > interpolator;
                  bool interpolate = Frac( d.x ) != 0 || Frac( d.y ) != 0;
                  if ( interpolate )
                     interpolator = interpolation.NewInterpolator( (P*)0, f0c, w, h );

                  for ( int i = 0; i < h; ++i, status += w )
                     for ( int j = 0; j < w; ++j, ++fij )
                     {
                        DPoint p( j - d.x, i - d.y );

                        if ( p.x >= 0 && p.y >= 0 && p.x < w && p.y < h )
                           *fij = interpolate ? (*interpolator)( p ) : f0c[TruncI( p.y )*w + TruncI( p.x )];
                        else
                           *fij = P::MinSampleValue();
                     }

                  img.Allocator().Deallocate( f0[c] );
                  f0[c] = f;
                  f = 0;
               }

               if ( k != 1 )
               {
                  status.Initialize( String().Format( "Applying linear color correction, channel #%d, k=%.8lf", c, k ), N );

                  for ( typename P::sample* f = f0[c], * fN = f + N; f != fN; ++f, ++status )
                  {
                     double v; P::FromSample( v, *f );
                     *f = P::ToSample( v * k );
                  }
               }
            }

         img.ImportData( f0, w, h, n, cs0 );
         img.Status() = status;
      }
      catch ( ... )
      {
         if ( f != 0 )
            img.Allocator().Deallocate( f );
         if ( f0 != 0 )
         {
            for ( int c = 0; c < n; ++c )
               if ( f0[c] != 0 )
                  img.Allocator().Deallocate( f0[c] );
            img.Allocator().Deallocate( f0 );
         }

         img.FreeData();
         throw;
      }
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

bool ChannelMatchInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   Console().EnableAbort();

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      return false;

   // Only color images are supported
   if ( image.ColorSpace() == ColorSpace::Gray )
      return false;

   StandardStatus status;
   image.SetStatusCallback( &status );

   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32: ChannelMatchEngine::Apply( static_cast<Image&>( *image ), *this ); break;
      case 64: ChannelMatchEngine::Apply( static_cast<DImage&>( *image ), *this ); break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8: ChannelMatchEngine::Apply( static_cast<UInt8Image&>( *image ), *this ); break;
      case 16: ChannelMatchEngine::Apply( static_cast<UInt16Image&>( *image ), *this ); break;
      case 32: ChannelMatchEngine::Apply( static_cast<UInt32Image&>( *image ), *this ); break;
      }

   return true;
}

// ----------------------------------------------------------------------------

void* ChannelMatchInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheChannelEnabledParameter )
      return p_channelEnabled+tableRow;
   if ( p == TheChannelXOffsetParameter )
      return &(p_channelOffset[tableRow].x);
   if ( p == TheChannelYOffsetParameter )
      return &(p_channelOffset[tableRow].y);
   if ( p == TheChannelFactorParameter )
      return p_channelFactor+tableRow;
   return 0;
}

bool ChannelMatchInstance::AllocateParameter( size_type length, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheChannelTableParameter )
      return true;
   return false;
}

size_type ChannelMatchInstance::ParameterLength( const MetaParameter* p, size_type /*tableRow*/ ) const
{
   if ( p == TheChannelTableParameter )
      return 3;
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF ChannelMatchInstance.cpp - Released 2014/11/14 17:18:46 UTC
