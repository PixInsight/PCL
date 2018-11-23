//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0364
// ----------------------------------------------------------------------------
// LRGBCombinationInstance.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "LRGBCombinationInstance.h"
#include "LRGBCombinationParameters.h"
#include "LRGBCombinationProcess.h"

#include <pcl/ATrousWaveletTransform.h>
#include <pcl/AutoViewLock.h>
#include <pcl/HistogramTransformation.h>
#include <pcl/ImageWindow.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

LRGBCombinationInstance::LRGBCombinationInstance( const MetaProcess* P ) :
   ProcessImplementation( P )
{
   for ( int i = 0; i < 4; ++i )
   {
      channelEnabled[i] = true;
      //channelId[i] = String();
      channelWeight[i] = 1;
   }

   luminanceMTF = TheLRGBLuminanceMTFParameter->DefaultValue();
   saturationMTF = TheLRGBSaturationMTFParameter->DefaultValue();
   clipHighlights = TheLRGBClipHighlightsParameter->DefaultValue();
   noiseReduction = TheLRGBNoiseReductionParameter->DefaultValue();
   numberOfRemovedWaveletLayers = (uint32)TheLRGBLayersRemovedParameter->DefaultValue();
   numberOfProtectedWaveletLayers = (uint32)TheLRGBLayersProtectedParameter->DefaultValue();
}

LRGBCombinationInstance::LRGBCombinationInstance( const LRGBCombinationInstance& x ) :
   ProcessImplementation( x )
{
   for ( int i = 0; i < 4; ++i )
   {
      channelEnabled[i] = x.channelEnabled[i];
      channelId[i] = x.channelId[i];
      channelWeight[i] = x.channelWeight[i];
   }

   luminanceMTF = x.luminanceMTF;
   saturationMTF = x.saturationMTF;
   clipHighlights = x.clipHighlights;
   noiseReduction = x.noiseReduction;
   numberOfRemovedWaveletLayers = x.numberOfRemovedWaveletLayers;
   numberOfProtectedWaveletLayers = x.numberOfProtectedWaveletLayers;
}

// ----------------------------------------------------------------------------

bool LRGBCombinationInstance::Validate( pcl::String& info )
{
   int n = 0;
   for ( int i = 0; i < 4; ++i )
   {
      if ( !channelId[i].IsEmpty() && !channelId[i].IsValidIdentifier() )
      {
         info = "Invalid image identifier: " + channelId[i];
         return false;
      }

      if ( channelEnabled[i] )
         ++n;
   }

   if ( n == 0 )
   {
      info = "No channel selected for combination";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

void LRGBCombinationInstance::Assign( const ProcessImplementation& p )
{
   const LRGBCombinationInstance* x = dynamic_cast<const LRGBCombinationInstance*>( &p );
   if ( x != nullptr )
   {
      for ( int i = 0; i < 4; ++i )
      {
         channelEnabled[i] = x->channelEnabled[i];
         channelId[i] = x->channelId[i];
         channelWeight[i] = x->channelWeight[i];
      }

      luminanceMTF = x->luminanceMTF;
      saturationMTF = x->saturationMTF;
      clipHighlights = x->clipHighlights;
      noiseReduction = x->noiseReduction;
      numberOfRemovedWaveletLayers = x->numberOfRemovedWaveletLayers;
      numberOfProtectedWaveletLayers = x->numberOfProtectedWaveletLayers;
   }
}

// ----------------------------------------------------------------------------

UndoFlags LRGBCombinationInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool LRGBCombinationInstance::CanExecuteOn( const View& v, String& whyNot ) const
{
   if ( v.Image().IsComplexSample() )
   {
      whyNot = "LRGBCombination cannot be executed on complex images.";
      return false;
   }

   if ( v.Image()->ColorSpace() == ColorSpace::Gray )
   {
      whyNot = "LRGBCombination cannot be executed on grayscale images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------

#ifdef _MSC_VER
#pragma optimize( "", off )
#endif

#ifdef __GNUC__
__attribute__((noinline))
#endif
static RGBColorSystem::sample CIEL(
                           const RGBColorSystem& rgbws,
                           RGBColorSystem::sample R, RGBColorSystem::sample G, RGBColorSystem::sample B )
{
   return rgbws.CIEL( R, G, B );
}

#ifdef __GNUC__
__attribute__((noinline))
#endif
static void RGBToCIEab(  const RGBColorSystem& rgbws,
                           RGBColorSystem::sample& a, RGBColorSystem::sample& b,
                           RGBColorSystem::sample R, RGBColorSystem::sample G, RGBColorSystem::sample B )
{
   rgbws.RGBToCIEab( a, b, R, G, B );
}

#ifdef __GNUC__
__attribute__((noinline))
#endif
static void RGBToCIELab(  const RGBColorSystem& rgbws,
                           RGBColorSystem::sample& L, RGBColorSystem::sample& a, RGBColorSystem::sample& b,
                           RGBColorSystem::sample R, RGBColorSystem::sample G, RGBColorSystem::sample B )
{
   rgbws.RGBToCIELab( L, a, b, R, G, B );
}

#ifdef __GNUC__
__attribute__((noinline))
#endif
static void CIELabToRGB( const RGBColorSystem& rgbws,
                           RGBColorSystem::sample& R, RGBColorSystem::sample& G, RGBColorSystem::sample& B,
                           RGBColorSystem::sample L, RGBColorSystem::sample a, RGBColorSystem::sample b )
{
   rgbws.CIELabToRGB( R, G, B, L, a, b );
}

#ifdef __GNUC__
__attribute__((noinline))
#endif
static void RGBToCIEch(  const RGBColorSystem& rgbws,
                           RGBColorSystem::sample& c, RGBColorSystem::sample& h,
                           RGBColorSystem::sample R, RGBColorSystem::sample G, RGBColorSystem::sample B )
{
   RGBColorSystem::sample L;
   rgbws.RGBToCIELch( L, c, h, R, G, B );
}

#ifdef __GNUC__
__attribute__((noinline))
#endif
static void RGBToCIELch(  const RGBColorSystem& rgbws,
                           RGBColorSystem::sample& L, RGBColorSystem::sample& c, RGBColorSystem::sample& h,
                           RGBColorSystem::sample R, RGBColorSystem::sample G, RGBColorSystem::sample B )
{
   rgbws.RGBToCIELch( L, c, h, R, G, B );
}

#ifdef __GNUC__
__attribute__((noinline))
#endif
static void CIELchToRGB( const RGBColorSystem& rgbws,
                           RGBColorSystem::sample& R, RGBColorSystem::sample& G, RGBColorSystem::sample& B,
                           RGBColorSystem::sample L, RGBColorSystem::sample c, RGBColorSystem::sample h )
{
   rgbws.CIELchToRGB( R, G, B, L, c, h );
}

static const float s_3x3Linear[] = { 0.50F, 1.00F, 0.50F };

#ifdef __GNUC__
__attribute__((noinline))
#endif
static void WvltNR2( Image& a, Image& b, int n )
{
   ATrousWaveletTransform::WaveletScalingFunction H;
   H.Set( SeparableFilter( s_3x3Linear, s_3x3Linear, 3 ) );

   ATrousWaveletTransform W( H );
   W.SetDyadicScalingSequence();
   W.SetNumberOfLayers( n );

   for ( int i = 0; i < n; ++i )
      W.DisableLayer( i );

   W << a;
   W >> a;

   b.Status() = a.Status();

   W << b;
   W >> b;

   a.Status() = b.Status();
   b.Status().Clear();
}

// 64*N
template <class P>
#ifdef __GNUC__
__attribute__((noinline))
#endif
static void WvltNRMask( UInt8Image& mask, const GenericImage<P>& img )
{
   Image wrkc( img.Width(), img.Height() );

   wrkc.Status() = img.Status();

   const RGBColorSystem& rgbws = img.RGBWorkingSpace();

   const typename P::sample* R = img.PixelData( 0 );
   const typename P::sample* G = img.PixelData( 1 );
   const typename P::sample* B = img.PixelData( 2 );

   for ( Image::sample* fc = wrkc.PixelData(),
                      * fN = fc + wrkc.NumberOfPixels(); fc != fN; ++wrkc.Status() )   // N
   {
      RGBColorSystem::sample r, g, b;
      P::FromSample( r, *R++ );
      P::FromSample( g, *G++ );
      P::FromSample( b, *B++ );

      *fc++ = rgbws.CIEc( r, g, b );
   }

   ATrousWaveletTransform W( SeparableFilter( s_3x3Linear, s_3x3Linear, 3 ) );
   W.SetDyadicScalingSequence();
   W.SetNumberOfLayers( 8 );
   W.DisableLayer( 3 );
   W.DisableLayer( 4 );
   W.DisableLayer( 5 );
   W.DisableLayer( 6 );
   W.DisableLayer( 7 );

   Image tmp;

   for ( int i = 0; i < 3; ++i )
   {
      W << wrkc;        // N*8
      tmp.Status() = wrkc.Status();
      W >> tmp;         // N*8;
      tmp.Truncate();   // N
      wrkc.Status() = tmp.Status();
      wrkc -= tmp;      // N
      wrkc.Truncate();  // N
      wrkc.Rescale();   // N
   }

   wrkc.Invert();       // N

   double median = wrkc.Median();
   double stdDev = wrkc.StdDev();
   img.Status() += wrkc.NumberOfPixels(); // N

   HistogramTransformation H;
   H.SetHighlightsClipping( median - stdDev );
   H.SetShadowsClipping( median - 2*stdDev );
   H >> wrkc;           // N

   img.Status() = wrkc.Status();

   mask = wrkc;
}

//
// Total count:
//
//    (2 + 4*r)*N + ((p != 0) ? (65 + 4*(r - p))*N : 0)
//
// where: r = Number of removed wavelet layers
//        p = Number of protected wavelet layers
//        N = Number of pixels
//
template <class P>
#ifdef __GNUC__
__attribute__((noinline))
#endif
static void ApplyChromaNR( GenericImage<P>& img, int removedLayers, int protectedLayers )
{
   UInt8Image mask;

   if ( protectedLayers > 0 )
      WvltNRMask( mask, img ); // 64*N

   Image wrka( img.Width(), img.Height() );
   Image wrkb( img.Width(), img.Height() );

   wrka.Status() = img.Status();

   const RGBColorSystem& rgbws = img.RGBWorkingSpace();

   typename P::sample* R = img.PixelData( 0 );
   typename P::sample* G = img.PixelData( 1 );
   typename P::sample* B = img.PixelData( 2 );

   for ( Image::sample* fa = wrka.PixelData(),
                      * fb = wrkb.PixelData(),
                      * fN = fa + wrka.NumberOfPixels(); fa != fN; ++wrka.Status() )   // N
   {
      RGBColorSystem::sample r, g, b, a_, b_;

      P::FromSample( r, *R++ );
      P::FromSample( g, *G++ );
      P::FromSample( b, *B++ );

      rgbws.RGBToCIEab( a_, b_, r, g, b );

      *fa++ = a_;
      *fb++ = b_;
   }

   WvltNR2( wrka, wrkb, removedLayers );   // 4*removedLayers*N

   if ( protectedLayers > 0 )
   {
      R = img.PixelData( 0 );
      G = img.PixelData( 1 );
      B = img.PixelData( 2 );

      const UInt8Image::sample* M = mask.PixelData();

      for ( Image::sample* fa = wrka.PixelData(),
                         * fb = wrkb.PixelData(),
                         * fN = fa + wrka.NumberOfPixels(); fa != fN; ++wrka.Status() )   // N
      {
         RGBColorSystem::sample r, g, b, a_, b_;

         P::FromSample( r, *R++ );
         P::FromSample( g, *G++ );
         P::FromSample( b, *B++ );

         rgbws.RGBToCIEab( a_, b_, r, g, b );

         RGBColorSystem::sample m; UInt8PixelTraits::FromSample( m, *M++ );
         RGBColorSystem::sample m1 = 1 - m;
         *fa = m*(*fa) + m1*a_; ++fa;
         *fb = m*(*fb) + m1*b_; ++fb;
      }

      mask.FreeData();

      WvltNR2( wrka, wrkb, Max( 1, removedLayers - protectedLayers ) );  // 4*(removedLayers - protectedLayers)*N
   }

   R = img.PixelData( 0 );
   G = img.PixelData( 1 );
   B = img.PixelData( 2 );

   for ( const Image::sample* fa = wrka.PixelData(),
                            * fb = wrkb.PixelData(),
                            * fN = fa + wrka.NumberOfPixels(); fa != fN; ++wrka.Status() )   // N
   {
      RGBColorSystem::sample r, g, b, L, a_, b_;

      P::FromSample( r, *R );
      P::FromSample( g, *G );
      P::FromSample( b, *B );

      rgbws.RGBToCIELab( L, a_, b_, r, g, b );
      rgbws.CIELabToRGB( r, g, b, L, *fa++, *fb++ );

      *R++ = P::ToSample( r );
      *G++ = P::ToSample( g );
      *B++ = P::ToSample( b );
   }

   img.Status() = wrka.Status();
}

template <class P, class P0, class P1, class P2>
#ifdef __GNUC__
__attribute__((noinline))
#endif
static void CombineLRGB( GenericImage<P>& img, const Rect& r,
                         const double k[], double mL, double mc, bool clip,
                         const GenericImage<P0>* srcR,
                         const GenericImage<P1>* srcG,
                         const GenericImage<P2>* srcB, ImageVariant& srcL )
{
   AbstractImage* srcImgL = srcL.ImagePtr();

   typename P::sample* dstR = img.PixelData( 0 );
   typename P::sample* dstG = img.PixelData( 1 );
   typename P::sample* dstB = img.PixelData( 2 );

   const RGBColorSystem& rgbws = img.RGBWorkingSpace();

   double k0 = k[0];
   double k1 = k[1];
   double k2 = k[2];

   double kMax = *MaxItem( k, k+3 );
   if ( 1 + kMax != 1 )
   {
      k0 /= kMax;
      k1 /= kMax;
      k2 /= kMax;
   }

   bool isKR = k0 != 1;
   bool isKG = k1 != 1;
   bool isKB = k2 != 1;

   bool isKL = k[3] != 1;
   double k31 = 1 - k[3];

   bool isLuminanceMTF = mL != 1;
   bool isSaturationMTF = mc != 1;

   bool isCIE = srcImgL != 0 || isLuminanceMTF || isSaturationMTF || isKL;

   double clipValue = 1;
   if ( clip )
      if ( isKR || isKG || isKB )
         clipValue = Min( Min( (k0 != 0) ? k0 : 1.0,
                               (k1 != 0) ? k1 : 1.0 ), (k2 != 0) ? k2 : 1.0 );
      else
         clip = false;

   for ( int y = r.y0; y < r.y1; ++y )
   {
      const typename P0::sample* dataR = (srcR != 0) ? srcR->PixelAddress( r.x0, y ) : 0;
      const typename P1::sample* dataG = (srcG != 0) ? srcG->PixelAddress( r.x0, y ) : 0;
      const typename P2::sample* dataB = (srcB != 0) ? srcB->PixelAddress( r.x0, y ) : 0;

      const uint8* dataL = 0;
      if ( srcImgL != 0 )
      {
         if ( srcL.IsFloatSample() )
            switch ( srcL.BitsPerSample() )
            {
            case 32:
               dataL = (uint8*)static_cast<Image*>( srcImgL )->PixelAddress( r.x0, y );
               break;
            case 64:
               dataL = (uint8*)static_cast<DImage*>( srcImgL )->PixelAddress( r.x0, y );
               break;
            }
         else
            switch ( srcL.BitsPerSample() )
            {
            case  8:
               dataL = (uint8*)static_cast<UInt8Image*>( srcImgL )->PixelAddress( r.x0, y );
               break;
            case 16:
               dataL = (uint8*)static_cast<UInt16Image*>( srcImgL )->PixelAddress( r.x0, y );
               break;
            case 32:
               dataL = (uint8*)static_cast<UInt32Image*>( srcImgL )->PixelAddress( r.x0, y );
               break;
            }
      }

      for ( int x = r.x0; x < r.x1; ++x, ++img.Status() )
      {
         RGBColorSystem::sample R, G, B;

         if ( dataR != 0 )
            P0::FromSample( R, *dataR++ );
         else
            P::FromSample( R, *dstR );

         if ( dataG != 0 )
            P1::FromSample( G, *dataG++ );
         else
            P::FromSample( G, *dstG );

         if ( dataB != 0 )
            P2::FromSample( B, *dataB++ );
         else
            P::FromSample( B, *dstB );

         if ( isKR )
            R *= k0;

         if ( clip && R != 0 )
            if ( R >= clipValue )
               R = 1;
            else
               R /= clipValue;

         if ( isKG )
            G *= k1;

         if ( clip && G != 0 )
            if ( G >= clipValue )
               G = 1;
            else
               G /= clipValue;

         if ( isKB )
            B *= k2;

         if ( clip && B != 0 )
            if ( B >= clipValue )
               B = 1;
            else
               B /= clipValue;

         if ( isCIE )
         {
            RGBColorSystem::sample L;

            if ( dataL != 0 )
            {
               if ( srcL.IsFloatSample() )
                  switch ( srcL.BitsPerSample() )
                  {
                  case 32:
                     FloatPixelTraits::FromSample( L, *(float*)dataL );
                     break;
                  case 64:
                     DoublePixelTraits::FromSample( L, *(double*)dataL );
                     break;
                  default:
                     L = 0;
                     break;
                  }
               else
                  switch ( srcL.BitsPerSample() )
                  {
                  case  8:
                     UInt8PixelTraits::FromSample( L, *(uint8*)dataL );
                     break;
                  case 16:
                     UInt16PixelTraits::FromSample( L, *(uint16*)dataL );
                     break;
                  case 32:
                     UInt32PixelTraits::FromSample( L, *(uint32*)dataL );
                     break;
                  default:
                     L = 0;
                     break;
                  }

               dataL += srcL.BytesPerSample();
            }
            else
               L = CIEL( rgbws, R, G, B );

            if ( isLuminanceMTF )
               L = HistogramTransformation::MTF( mL, L );

            if ( isKL )
               L *= k[3];

            if ( isSaturationMTF )
            {
               RGBColorSystem::sample c, h;

               if ( isKL )
               {
                  RGBColorSystem::sample L0;
                  RGBToCIELch( rgbws, L0, c, h, R, G, B );
                  L += k31*L0;
               }
               else
                  RGBToCIEch( rgbws, c, h, R, G, B );

               c = HistogramTransformation::MTF( mc, c )*L + c*(1 - L);

               CIELchToRGB( rgbws, R, G, B, L, c, h );
            }
            else
            {
               RGBColorSystem::sample a, b;

               if ( isKL )
               {
                  RGBColorSystem::sample L0;
                  RGBToCIELab( rgbws, L0, a, b, R, G, B );
                  L += k31*L0;
               }
               else
                  RGBToCIEab( rgbws, a, b, R, G, B );

               CIELabToRGB( rgbws, R, G, B, L, a, b );
            }
         }

         *dstR++ = P::ToSample( R );
         *dstG++ = P::ToSample( G );
         *dstB++ = P::ToSample( B );
      }
   }
}

template <class P, class P0, class P1>
#ifdef __GNUC__
__attribute__((noinline))
#endif
static void CombineLRGB( GenericImage<P>& img, const Rect& r, const double k[], double mL, double mc, bool clip,
                     const GenericImage<P0>* srcR,
                     const GenericImage<P1>* srcG, ImageVariant& srcB, ImageVariant& srcL )
{
   if ( srcB )
   {
      if ( srcB.IsFloatSample() )
         switch ( srcB.BitsPerSample() )
         {
         case 32:
            CombineLRGB( img, r, k, mL, mc, clip, srcR, srcG, static_cast<Image*>( srcB.ImagePtr() ), srcL );
            break;
         case 64:
            CombineLRGB( img, r, k, mL, mc, clip, srcR, srcG, static_cast<DImage*>( srcB.ImagePtr() ), srcL );
            break;
         }
      else
         switch ( srcB.BitsPerSample() )
         {
         case  8:
            CombineLRGB( img, r, k, mL, mc, clip, srcR, srcG, static_cast<UInt8Image*>( srcB.ImagePtr() ), srcL );
            break;
         case 16:
            CombineLRGB( img, r, k, mL, mc, clip, srcR, srcG, static_cast<UInt16Image*>( srcB.ImagePtr() ), srcL );
            break;
         case 32:
            CombineLRGB( img, r, k, mL, mc, clip, srcR, srcG, static_cast<UInt32Image*>( srcB.ImagePtr() ), srcL );
            break;
         }
   }
   else
      CombineLRGB( img, r, k, mL, mc, clip, srcR, srcG, static_cast<UInt8Image*>( 0 ), srcL );
}

template <class P, class P0>
#ifdef __GNUC__
__attribute__((noinline))
#endif
static void CombineLRGB( GenericImage<P>& img, const Rect& r, const double k[], double mL, double mc, bool clip,
                     const GenericImage<P0>* srcR, ImageVariant& srcG, ImageVariant& srcB, ImageVariant& srcL )
{
   if ( srcG )
   {
      if ( srcG.IsFloatSample() )
         switch ( srcG.BitsPerSample() )
         {
         case 32:
            CombineLRGB( img, r, k, mL, mc, clip, srcR, static_cast<Image*>( srcG.ImagePtr() ), srcB, srcL );
            break;
         case 64:
            CombineLRGB( img, r, k, mL, mc, clip, srcR, static_cast<DImage*>( srcG.ImagePtr() ), srcB, srcL );
            break;
         }
      else
         switch ( srcG.BitsPerSample() )
         {
         case  8:
            CombineLRGB( img, r, k, mL, mc, clip, srcR, static_cast<UInt8Image*>( srcG.ImagePtr() ), srcB, srcL );
            break;
         case 16:
            CombineLRGB( img, r, k, mL, mc, clip, srcR, static_cast<UInt16Image*>( srcG.ImagePtr() ), srcB, srcL );
            break;
         case 32:
            CombineLRGB( img, r, k, mL, mc, clip, srcR, static_cast<UInt32Image*>( srcG.ImagePtr() ), srcB, srcL );
            break;
         }
   }
   else
      CombineLRGB( img, r, k, mL, mc, clip, srcR, static_cast<UInt8Image*>( 0 ), srcB, srcL );
}

template <class P>
#ifdef __GNUC__
__attribute__((noinline))
#endif
static void CombineLRGB( GenericImage<P>& img, const Rect& r, const double k[], double mL, double mc, bool clip,
                     ImageVariant& srcR, ImageVariant& srcG, ImageVariant& srcB, ImageVariant& srcL )
{
   if ( srcR )
   {
      if ( srcR.IsFloatSample() )
         switch ( srcR.BitsPerSample() )
         {
         case 32:
            CombineLRGB( img, r, k, mL, mc, clip, static_cast<Image*>( srcR.ImagePtr() ), srcG, srcB, srcL );
            break;
         case 64:
            CombineLRGB( img, r, k, mL, mc, clip, static_cast<DImage*>( srcR.ImagePtr() ), srcG, srcB, srcL );
            break;
         }
      else
         switch ( srcR.BitsPerSample() )
         {
         case  8:
            CombineLRGB( img, r, k, mL, mc, clip, static_cast<UInt8Image*>( srcR.ImagePtr() ), srcG, srcB, srcL );
            break;
         case 16:
            CombineLRGB( img, r, k, mL, mc, clip, static_cast<UInt16Image*>( srcR.ImagePtr() ), srcG, srcB, srcL );
            break;
         case 32:
            CombineLRGB( img, r, k, mL, mc, clip, static_cast<UInt32Image*>( srcR.ImagePtr() ), srcG, srcB, srcL );
            break;
         }
   }
   else
      CombineLRGB( img, r, k, mL, mc, clip, static_cast<UInt8Image*>( nullptr ), srcG, srcB, srcL );
}

#ifdef _MSC_VER
#pragma optimize( "", on )
#endif

bool LRGBCombinationInstance::ExecuteOn( View& view )
{
   ImageWindow sourceWindow[ 4 ];
   ImageVariant sourceImage[ 4 ];

   AutoViewLock lock( view );

   ImageVariant image = view.Image();
   if ( image.IsComplexSample() )
      throw Error( "LRGBCombination cannot be executed on complex images." );
   if ( image->ColorSpace() == ColorSpace::Gray )
      throw Error( "LRGBCombination cannot be executed on grayscale images." );

   Console().EnableAbort();

   StandardStatus status;
   image->SetStatusCallback( &status );

   // Base id, preview rectangle or entire image boundaries
   String baseId;
   Rect r;
   int w0, h0;
   if ( view.IsPreview() )
   {
      ImageWindow w = view.Window();
      View mainView = w.MainView();
      baseId = mainView.Id();
      r = w.PreviewRect( view.Id() );
      mainView.GetSize( w0, h0 );
   }
   else
   {
      baseId = view.Id();
      r = image->Bounds();
      w0 = r.Width();
      h0 = r.Height();
   }

   int numberOfSources = 0;

   for ( int i = 0; i < 4; ++i )
      if ( channelEnabled[i] )
      {
         String id = channelId[i];
         if ( id.IsEmpty() )
            id = baseId + '_' + ColorSpaceId::ChannelId( (i < 3) ? ColorSpaceId::RGB : ColorSpaceId::CIELab,
                                                         (i < 3) ? i : 0 );

         sourceWindow[i] = ImageWindow::WindowById( id );
         if ( sourceWindow[i].IsNull() )
            throw Error( "LRGBCombination: Source image not found: " + id );

         sourceImage[i] = sourceWindow[i].MainView().Image();
         if ( !sourceImage[i] )
            throw Error( "LRGBCombination: Invalid source image: " + id );

         if ( sourceImage[i]->IsColor() )
            throw Error( "LRGBCombination: Invalid source color space: " + id );

         if ( sourceImage[i]->Width() != w0 || sourceImage[i]->Height() != h0 )
            throw Error( "LRGBCombination: Incompatible source image geometry: " + id );

         ++numberOfSources;
      }

   if ( numberOfSources == 0 )
      throw Error( "LRGBCombination: No source images." );

   image->Status().Initialize( sourceImage[3] ?
      "Combining normalized LRGB channels" : "Combining RGB channels", image->NumberOfPixels() );

   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32:
         CombineLRGB( static_cast<Image&>( *image ), r,
                      channelWeight, luminanceMTF, saturationMTF, clipHighlights,
                      sourceImage[0], sourceImage[1], sourceImage[2], sourceImage[3] );
         break;
      case 64:
         CombineLRGB( static_cast<DImage&>( *image ), r,
                      channelWeight, luminanceMTF, saturationMTF, clipHighlights,
                      sourceImage[0], sourceImage[1], sourceImage[2], sourceImage[3] );
         break;
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8:
         CombineLRGB( static_cast<UInt8Image&>( *image ), r,
                      channelWeight, luminanceMTF, saturationMTF, clipHighlights,
                      sourceImage[0], sourceImage[1], sourceImage[2], sourceImage[3] );
         break;
      case 16:
         CombineLRGB( static_cast<UInt16Image&>( *image ), r,
                      channelWeight, luminanceMTF, saturationMTF, clipHighlights,
                      sourceImage[0], sourceImage[1], sourceImage[2], sourceImage[3] );
         break;
      case 32:
         CombineLRGB( static_cast<UInt32Image&>( *image ), r,
                      channelWeight, luminanceMTF, saturationMTF, clipHighlights,
                      sourceImage[0], sourceImage[1], sourceImage[2], sourceImage[3] );
         break;
      }

   if ( noiseReduction )
   {
      size_type N = image->NumberOfPixels();
      size_type count = (2 + 4*numberOfRemovedWaveletLayers)*N;

      if ( numberOfProtectedWaveletLayers > 0 )
         count += (65 + 4*Max( uint32( 1 ), numberOfRemovedWaveletLayers - numberOfProtectedWaveletLayers ))*N;

      image->Status().Initialize( "LRGB chroma noise reduction", count );
      image->Status().DisableInitialization();

      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32:
            ApplyChromaNR( static_cast<Image&>( *image ),
                           numberOfRemovedWaveletLayers, numberOfProtectedWaveletLayers );
            break;
         case 64:
            ApplyChromaNR( static_cast<DImage&>( *image ),
                           numberOfRemovedWaveletLayers, numberOfProtectedWaveletLayers );
            break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8:
            ApplyChromaNR( static_cast<UInt8Image&>( *image ),
                           numberOfRemovedWaveletLayers, numberOfProtectedWaveletLayers );
            break;
         case 16:
            ApplyChromaNR( static_cast<UInt16Image&>( *image ),
                           numberOfRemovedWaveletLayers, numberOfProtectedWaveletLayers );
            break;
         case 32:
            ApplyChromaNR( static_cast<UInt32Image&>( *image ),
                           numberOfRemovedWaveletLayers, numberOfProtectedWaveletLayers );
            break;
         }
   }

   return true;
}

// ----------------------------------------------------------------------------

bool LRGBCombinationInstance::CanExecuteGlobal( String& whyNot ) const
{
   return true;
}

bool LRGBCombinationInstance::ExecuteGlobal()
{
   ImageWindow sourceWindow[ 4 ];
   ImageVariant sourceImage[ 4 ];

   int width = 0, height = 0;
   bool floatSample = false;
   int bitsPerSample = 0;

   RGBColorSystem rgbws;
   double xRes, yRes;
   bool metric;

   int numberOfSources = 0;

   for ( int i = 0; i < 4; ++i )
      if ( channelEnabled[i] && !channelId[i].IsEmpty() )
      {
         sourceWindow[i] = ImageWindow::WindowById( channelId[i] );
         if ( sourceWindow[i].IsNull() )
            throw Error( "LRGBCombination: Source image not found: " + channelId[i] );

         sourceImage[i] = sourceWindow[i].MainView().Image();
         if ( !sourceImage[i] )
            throw Error( "LRGBCombination: Invalid source image: " + channelId[i] );

         if ( sourceImage[i]->IsColor() )
            throw Error( "LRGBCombination: Invalid source color space: " + channelId[i] );

         if ( sourceImage[i].IsFloatSample() )
            floatSample = true;

         if ( sourceImage[i].BitsPerSample() > bitsPerSample )
            bitsPerSample = sourceImage[i].BitsPerSample();

         if ( width == 0 )
         {
            width = sourceImage[i]->Width();
            height = sourceImage[i]->Height();
            sourceWindow[i].GetRGBWS( rgbws );
            sourceWindow[i].GetResolution( xRes, yRes, metric );
         }
         else
         {
            if ( sourceImage[i]->Width() != width || sourceImage[i]->Height() != height )
               throw Error( "LRGBCombination: Incompatible source image geometry: " + channelId[i] );
         }

         ++numberOfSources;
      }

   if ( numberOfSources == 0 )
      throw Error( "LRGBCombination: No source images." );

   ImageWindow w( width, height, 3, bitsPerSample, floatSample, true, true );
   if ( w.IsNull() )
      throw Error( "LRGBCombination: Unable to create target image." );

   w.SetRGBWS( rgbws );
   w.SetResolution( xRes, yRes, metric );

   View mainView = w.MainView();

   AutoViewLock lock( mainView );

   try
   {
      ImageVariant image = mainView.Image();

      Console().EnableAbort();

      StandardStatus status;
      image->SetStatusCallback( &status );

      image->Status().Initialize( "Combining normalized LRGB channels", image->NumberOfPixels() );

      Rect r = image->Bounds();

      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32:
            CombineLRGB( static_cast<Image&>( *image ), r,
                         channelWeight, luminanceMTF, saturationMTF, clipHighlights,
                         sourceImage[0], sourceImage[1], sourceImage[2], sourceImage[3] );
            break;
         case 64:
            CombineLRGB( static_cast<DImage&>( *image ), r,
                         channelWeight, luminanceMTF, saturationMTF, clipHighlights,
                         sourceImage[0], sourceImage[1], sourceImage[2], sourceImage[3] );
            break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8:
            CombineLRGB( static_cast<UInt8Image&>( *image ), r,
                         channelWeight, luminanceMTF, saturationMTF, clipHighlights,
                         sourceImage[0], sourceImage[1], sourceImage[2], sourceImage[3] );
            break;
         case 16:
            CombineLRGB( static_cast<UInt16Image&>( *image ), r,
                         channelWeight, luminanceMTF, saturationMTF, clipHighlights,
                         sourceImage[0], sourceImage[1], sourceImage[2], sourceImage[3] );
            break;
         case 32:
            CombineLRGB( static_cast<UInt32Image&>( *image ), r,
                         channelWeight, luminanceMTF, saturationMTF, clipHighlights,
                         sourceImage[0], sourceImage[1], sourceImage[2], sourceImage[3] );
            break;
         }

      if ( noiseReduction )
      {
         size_type N = image->NumberOfPixels();
         size_type count = (2 + 4*numberOfRemovedWaveletLayers)*N;

         if ( numberOfProtectedWaveletLayers > 0 )
            count += (65 + 4*Max( uint32( 1 ), numberOfRemovedWaveletLayers - numberOfProtectedWaveletLayers ))*N;

         image->Status().Initialize( "LRGB chrominance noise reduction", count );
         image->Status().DisableInitialization();

         if ( image.IsFloatSample() )
            switch ( image.BitsPerSample() )
            {
            case 32:
               ApplyChromaNR( static_cast<Image&>( *image ),
                              numberOfRemovedWaveletLayers, numberOfProtectedWaveletLayers );
               break;
            case 64:
               ApplyChromaNR( static_cast<DImage&>( *image ),
                              numberOfRemovedWaveletLayers, numberOfProtectedWaveletLayers );
               break;
            }
         else
            switch ( image.BitsPerSample() )
            {
            case  8:
               ApplyChromaNR( static_cast<UInt8Image&>( *image ),
                              numberOfRemovedWaveletLayers, numberOfProtectedWaveletLayers );
               break;
            case 16:
               ApplyChromaNR( static_cast<UInt16Image&>( *image ),
                              numberOfRemovedWaveletLayers, numberOfProtectedWaveletLayers );
               break;
            case 32:
               ApplyChromaNR( static_cast<UInt32Image&>( *image ),
                              numberOfRemovedWaveletLayers, numberOfProtectedWaveletLayers );
               break;
            }
      }

      w.Show();

      return true;
   }
   catch ( ... )
   {
      w.Close();
      throw;
   }
}

// ----------------------------------------------------------------------------

void* LRGBCombinationInstance::LockParameter( const MetaParameter* p, size_type tableRow )
{
   if ( p == TheLRGBChannelEnabledParameter )
      return channelEnabled + tableRow;
   if ( p == TheLRGBChannelIdParameter )
      return channelId[tableRow].Begin();
   if ( p == TheLRGBChannelWeightParameter )
      return channelWeight + tableRow;
   if ( p == TheLRGBLuminanceMTFParameter )
      return &luminanceMTF;
   if ( p == TheLRGBSaturationMTFParameter )
      return &saturationMTF;
   if ( p == TheLRGBClipHighlightsParameter )
      return &clipHighlights;
   if ( p == TheLRGBNoiseReductionParameter )
      return &noiseReduction;
   if ( p == TheLRGBLayersRemovedParameter )
      return &numberOfRemovedWaveletLayers;
   if ( p == TheLRGBLayersProtectedParameter )
      return &numberOfProtectedWaveletLayers;
   return 0;
}

bool LRGBCombinationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheLRGBChannelIdParameter )
   {
      channelId[tableRow].Clear();
      if ( sizeOrLength > 0 )
         channelId[tableRow].SetLength( sizeOrLength );
      return true;
   }

   if ( p == TheLRGBChannelTableParameter )
      return true;

   return false;
}

size_type LRGBCombinationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheLRGBChannelIdParameter )
      return channelId[tableRow].Length();
   if ( p == TheLRGBChannelTableParameter )
      return 4;
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LRGBCombinationInstance.cpp - Released 2018-11-23T18:45:58Z
