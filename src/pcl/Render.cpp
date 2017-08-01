//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// pcl/Render.cpp - Released 2017-08-01T14:23:38Z
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

/*
 * ### TODO ###
 *
 * Rewrite this code to clean and organize it.
 */

#include <pcl/Bitmap.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ImageVariant.h>
#include <pcl/ImageWindow.h>
#include <pcl/Thread.h>
#include <pcl/Vector.h>

#define MinZoom   -32
#define MaxZoom   100

namespace pcl
{

// ----------------------------------------------------------------------------

typedef RGBColorSystem::sample
   (RGBColorSystem::*single_component_func)( RGBColorSystem::sample,
                                             RGBColorSystem::sample,
                                             RGBColorSystem::sample ) const;

static single_component_func singleComponents[] =
{
   &RGBColorSystem::CIEL,  0,                              0,
   &RGBColorSystem::CIEa,  &RGBColorSystem::CIEb,          &RGBColorSystem::CIEc, &RGBColorSystem::CIEh,
   &RGBColorSystem::CIEX,  &RGBColorSystem::CIEY,          &RGBColorSystem::CIEZ,
   &RGBColorSystem::Hue,   &RGBColorSystem::HSVSaturation, &RGBColorSystem::HSISaturation,
   &RGBColorSystem::Value, &RGBColorSystem::Intensity
};

// ----------------------------------------------------------------------------

struct RenderThreadData
{
   Bitmap*       bitmap;        // target RGBA bitmap
   int           x0, y0;        // target coordinates
   int           z0;            // original zoom level (z0 > 0: zoom in, z0 < 0: zoom out)
   int           z;             // absolute value of zoom level, z > 0
   int           channel;       // source channel
   int           maskMode;      // mask rendering mode
   bool          maskInverted;  // invert mask ?
   bool          colorImage;    // true if source image is RGB color
   bool          colorMask;     // true if source mask is RGB color
   const uint8** LUT;           // source LUT
   bool          noLUT[ 4 ];    // flags for quick LUT selection

   Rect          r;             // source rectangle
   int           w, h;          // dimensions of source rectangle
   int           n;             // number of source nominal channels

   Point         mp;            // source mask origin

   bool          hasAlpha;      // true if the source image has alpha channels

   single_component_func F;     // single component function

   // Data used for z < 0 only
   bool          hq;            // high-quality downsampling?
   int           wz, hz;        // for z < 0: dimensions of target region
   const int*    iz;            // subsample index
   double        _1_Pz2;        // divisor for subsampled totals
   double        _255_Pz2;      // 8-bit conversion factor / divisor for subsampled totals
   double        _64K_Pz2;      // 16-bit conversion factor / divisor for subsampled totals
   double        _1_Mz2;        // divisor for subsampled mask totals

   // Image and mask
   const AbstractImage* image;  // source image
   const AbstractImage* mask;   // source mask

   // RGBWS
   const RGBColorSystem* rgbws;
};

class RenderThreadBase
{
public:

   RenderThreadBase( const RenderThreadData* d, int y0, int y1 ) :
   data( d ), startRow( y0 ), endRow( y1 )
   {
   }

   template <class P, class M>
   static bool InitData( RenderThreadData& data,
                         Bitmap& bitmap, int x0, int y0, int z, int channel,
                         const GenericImage<P>& image,
                         bool showAlpha,
                         const GenericImage<M>* mask, int maskMode, bool maskInverted,
                         const uint8** LUT,
                         bool fast );
protected:

   const RenderThreadData* data;
   int                     startRow, endRow; // from 0 to h or hz

   template <class P> void Render( const GenericImage<P>& );
   template <class M> void Mask( const GenericImage<M>& );

   template <typename T, typename Ts> inline
   double SubsampledTotal( const T** f, Ts& s, int x );

   template <typename T>
   double SubsampledTotal( const T** f, int x )
   {
      uint32 s;
      return SubsampledTotal( f, s, x );
   }

   double SubsampledTotal( const uint32** f, int x )
   {
      uint64 s;
      return SubsampledTotal( f, s, x );
   }

   double SubsampledTotal( const float** f, int x )
   {
      float s;
      return SubsampledTotal( f, s, x );
   }

   double SubsampledTotal( const double** f, int x )
   {
      double s;
      return SubsampledTotal( f, s, x );
   }

   inline void ApplyMask( uint8& pr, uint8& pg, uint8& pb, float mx );
   inline void ApplyMask( uint8& pr, uint8& pg, uint8& pb, float mr, float mg, float mb );

private:

   typedef GenericVector<IVector>   subsample_index_list;
   static subsample_index_list      subsample_index;

   static void InitSubsampleIndex();
};

template <class P, class M>
class RenderThread : public Thread, public RenderThreadBase
{
public:

   RenderThread( const RenderThreadData* d, int y0, int y1 ) :
   Thread(), RenderThreadBase( d, y0, y1 )
   {
   }

   virtual void Run()
   {
      Render( *reinterpret_cast<const GenericImage<P>*>( data->image ) );
      if ( data->mask != 0 )
         Mask( *reinterpret_cast<const GenericImage<M>*>( data->mask ) );
   }
};

// ----------------------------------------------------------------------------

template <class P, class M>
bool RenderThreadBase::InitData( RenderThreadData& data,
                                 Bitmap& bitmap, int x0, int y0, int z, int channel,
                                 const GenericImage<P>& image,
                                 bool showAlpha,
                                 const GenericImage<M>* mask, int maskMode, bool maskInverted,
                                 const uint8** LUT,
                                 bool fast )
{
   if ( subsample_index.IsEmpty() )
      InitSubsampleIndex();

   if ( image.IsEmptySelection() || bitmap.IsEmpty() )
      return false;

   int wq = bitmap.Width();
   int hq = bitmap.Height();

   if ( x0 >= wq || y0 >= hq )
      return false;

   data.bitmap = &bitmap;
   data.x0 = x0;
   data.y0 = y0;
   data.z0 = z;
   data.z = Abs( z );
   data.channel = channel;
   data.colorImage = image.IsColor();
   data.LUT = LUT;

   if ( mask != 0 )
   {
      data.maskMode = maskMode;
      data.maskInverted = maskInverted;
      data.colorMask = mask != 0 && mask->IsColor();
      data.mp = mask->SelectedPoint();
   }

   if ( LUT != 0 )
   {
      data.noLUT[0] = LUT[0] == 0;

      if ( data.colorImage )
      {
         data.noLUT[1] = LUT[1] == 0;
         data.noLUT[2] = LUT[2] == 0;
         data.noLUT[3] = LUT[3] == 0;
      }
      else
         data.noLUT[1] = data.noLUT[2] = data.noLUT[3] = true;
   }
   else
      data.noLUT[0] = data.noLUT[1] = data.noLUT[2] = data.noLUT[3] = true;

   data.r = image.SelectedRectangle();
   data.w = data.r.Width();
   data.h = data.r.Height();
   data.n = image.NumberOfNominalChannels();

   data.hasAlpha = showAlpha && image.HasAlphaChannels();

   if ( channel >= DisplayChannel::Lightness && channel <= DisplayChannel::Intensity )
      data.F = singleComponents[channel - DisplayChannel::Lightness];
   else
      data.F = 0;

   if ( z < 0 )
   {
      data.hq = z == -2 || !fast;

      data.wz = Range( data.w/data.z, 1, wq-x0 );
      data.hz = Range( data.h/data.z, 1, hq-y0 );

      data.iz = subsample_index[data.z].Begin();

      int z2 = data.hq ? z*z : data.z;
      data._1_Pz2 = 1.0/P::MaxSampleValue()/z2;
      data._255_Pz2 = 255 * data._1_Pz2;
      data._64K_Pz2 = 65535 * data._1_Pz2;
      if ( mask != 0 )
         data._1_Mz2 = 1.0/M::MaxSampleValue()/z2;
   }

   data.image = &image;
   data.mask = mask;
   data.rgbws = &image.RGBWorkingSpace();

   return true;
}

RenderThreadBase::subsample_index_list RenderThreadBase::subsample_index;

void RenderThreadBase::InitSubsampleIndex()
{
   subsample_index = subsample_index_list( 1 - MinZoom );
   for ( int z = 3; z <= -MinZoom; ++z )
   {
      subsample_index[z] = IVector( z );
      if ( z & 1 )
      {
         int x1 = 0;
         for ( int i = 0; i < z; i += 2, x1 += 2 )
            subsample_index[z][i] = x1;
         x1 -= 3;
         for ( int i = 1; i < z; i += 2, x1 -= 2 )
            subsample_index[z][i] = x1;
      }
      else
      {
         for ( int i = 0, j = z-1, x1 = 0, x2 = j; i < j; i += 2, j -= 2, x1 += 2, x2 -= 2 )
         {
            subsample_index[z][i] = x1;
            subsample_index[z][j] = x2;
         }
         for ( int i = 1, j = z-2, x1 = j, x2 = 1; i < j; i += 2, j -= 2, x1 -= 2, x2 += 2 )
         {
            subsample_index[z][i] = x1;
            subsample_index[z][j] = x2;
         }
      }
   }
}

#define bitmap       data->bitmap
#define X0           data->x0
#define Y0           data->y0
#define Z0           data->z0
#define z            data->z
#define channel      data->channel
#define maskMode     data->maskMode
#define maskInverted data->maskInverted
#define colorImage   data->colorImage
#define colorMask    data->colorMask
#define LUT          data->LUT
#define noLUT        data->noLUT
#define r            data->r
#define w            data->w
#define h            data->h
#define n            data->n
#define mp           data->mp
#define hasAlpha     data->hasAlpha
#define F            data->F
#define hq           data->hq
#define wz           data->wz
#define hz           data->hz
#define iz           data->iz
#define _1_Pz2       data->_1_Pz2
#define _255_Pz2     data->_255_Pz2
#define _64K_Pz2     data->_64K_Pz2
#define _1_Mz2       data->_1_Mz2
#define rgbws        data->rgbws

template <typename T, typename Ts>
double RenderThreadBase::SubsampledTotal( const T** f, Ts& s, int x )
{
   s = 0;
   if ( hq )
   {
      for ( int i = 0, xz = x*z; i < z; ++i )
         for ( int j = 0, xj = xz; j < z; ++j, ++xj )
            s += f[i][xj];
   }
   else
   {
      x *= z;
      for ( int i = 0; i < z; ++i )
         s += f[i][x + iz[i]];
      /*
      if ( z & 1 )
      {
         int x1 = x*z;
         for ( int i = 0; i < z; i += 2, x1 += 2 )
            s += f[i][x1];
         x1 -= 3;
         for ( int i = 1; i < z; i += 2, x1 -= 2 )
            s += f[i][x1];
      }
      else
      {
         int x0 = x*z;
         for ( int i = 0, j = z-1, x1 = x0, x2 = x0+j; i < j; i += 2, j -= 2, x1 += 2, x2 -= 2 )
         {
            s += f[i][x1];
            s += f[j][x2];
         }
         for ( int i = 1, j = z-2, x1 = x0+j, x2 = x0+1; i < j; i += 2, j -= 2, x1 -= 2, x2 += 2 )
         {
            s += f[i][x1];
            s += f[j][x2];
         }
      }
      */

      /*
      int x1 = x*z;
      for ( int i = 0; i < z; ++i )
         s += f[i][x1++];
      for ( int i = 0; i < z; ++i )
         s += f[i][--x1];
      */
      /*
      for ( int i = 0, x1 = x*z, x2 = x1+z; i < z; ++i )
      {
         s += f[i][x1++];
         s += f[i][--x2];
      }
      */
   }

   return s;
}

void RenderThreadBase::ApplyMask( uint8& pr, uint8& pg, uint8& pb, float mx )
{
   if ( maskMode == MaskMode::Replace )
   {
      pr = pg = pb = UInt8PixelTraits::ToSample( mx );
   }
   else
   {
      UInt8PixelTraits::Mul( pr, mx );
      UInt8PixelTraits::Mul( pg, mx );
      UInt8PixelTraits::Mul( pb, mx );

      if ( maskMode != MaskMode::Multiply )
      {
         mx = 1 - mx;
         switch ( maskMode )
         {
         case MaskMode::OverlayRed:
            UInt8PixelTraits::Add( pr, mx );
            break;
         case MaskMode::OverlayGreen:
            UInt8PixelTraits::Add( pg, mx );
            break;
         case MaskMode::OverlayBlue:
            UInt8PixelTraits::Add( pb, mx );
            break;
         case MaskMode::OverlayYellow:
            UInt8PixelTraits::Add( pr, mx );
            UInt8PixelTraits::Add( pg, mx );
            break;
         case MaskMode::OverlayMagenta:
            UInt8PixelTraits::Add( pr, mx );
            UInt8PixelTraits::Add( pb, mx );
            break;
         case MaskMode::OverlayCyan:
            UInt8PixelTraits::Add( pg, mx );
            UInt8PixelTraits::Add( pb, mx );
            break;
         case MaskMode::OverlayOrange:
            UInt8PixelTraits::Add( pr, mx );
            UInt8PixelTraits::Add( pg, mx/2 );
            break;
         case MaskMode::OverlayViolet:
            UInt8PixelTraits::Add( pr, mx/2 );
            UInt8PixelTraits::Add( pb, mx );
            break;
         default:
            break;
         }
      }
   }
}

void RenderThreadBase::ApplyMask( uint8& pr, uint8& pg, uint8& pb, float mr, float mg, float mb )
{
   if ( maskMode == MaskMode::Replace )
   {
      UInt8PixelTraits::Mov( pr, mr );
      UInt8PixelTraits::Mov( pg, mg );
      UInt8PixelTraits::Mov( pb, mb );
   }
   else
   {
      switch ( channel )
      {
      // Individual RGB channels show their corresponding mask channels just like
      // grayscale masks.
      case DisplayChannel::Red:
         ApplyMask( pr, pg, pb, mr );
         break;
      case DisplayChannel::Green:
         ApplyMask( pr, pg, pb, mg );
         break;
      case DisplayChannel::Blue:
         ApplyMask( pr, pg, pb, mb );
         break;

      // Other display modes except alpha channels.
      case DisplayChannel::RGBK:
      case DisplayChannel::Lightness:
      case DisplayChannel::ChrominanceRG:
      case DisplayChannel::ChrominanceLFixed:
      case DisplayChannel::CIE_a:
      case DisplayChannel::CIE_b:
      case DisplayChannel::CIE_c:
      case DisplayChannel::CIE_h:
      case DisplayChannel::CIE_X:
      case DisplayChannel::CIE_Y:
      case DisplayChannel::CIE_Z:
      case DisplayChannel::Hue:
      case DisplayChannel::SaturationHSV:
      case DisplayChannel::SaturationHSI:
      case DisplayChannel::Value:
      case DisplayChannel::Intensity:
         if ( maskMode == MaskMode::Multiply )
         {
            UInt8PixelTraits::Mul( pr, mr );
            UInt8PixelTraits::Mul( pg, mg );
            UInt8PixelTraits::Mul( pb, mb );
         }
         else switch ( maskMode )
         {
         case MaskMode::OverlayRed:
            UInt8PixelTraits::Mul( pr, mr );
            UInt8PixelTraits::Add( pr, 1-mr );
            break;
         case MaskMode::OverlayGreen:
            UInt8PixelTraits::Mul( pg, mg );
            UInt8PixelTraits::Add( pg, 1-mg );
            break;
         case MaskMode::OverlayBlue:
            UInt8PixelTraits::Mul( pb, mb );
            UInt8PixelTraits::Add( pb, 1-mb );
            break;
         case MaskMode::OverlayYellow:
            UInt8PixelTraits::Mul( pr, mr );
            UInt8PixelTraits::Add( pr, 1-mr );
            UInt8PixelTraits::Mul( pg, mg );
            UInt8PixelTraits::Add( pg, 1-mg );
            break;
         case MaskMode::OverlayMagenta:
            UInt8PixelTraits::Mul( pr, mr );
            UInt8PixelTraits::Add( pr, 1-mr );
            UInt8PixelTraits::Mul( pb, mb );
            UInt8PixelTraits::Add( pb, 1-mb );
            break;
         case MaskMode::OverlayCyan:
            UInt8PixelTraits::Mul( pg, mg );
            UInt8PixelTraits::Add( pg, 1-mg );
            UInt8PixelTraits::Mul( pb, mb );
            UInt8PixelTraits::Add( pb, 1-mb );
            break;
         case MaskMode::OverlayOrange:
            UInt8PixelTraits::Mul( pr, mr );
            UInt8PixelTraits::Add( pr, 1-mr );
            UInt8PixelTraits::Mul( pg, mg );
            UInt8PixelTraits::Add( pg, 1-mg/2 );
            break;
         case MaskMode::OverlayViolet:
            UInt8PixelTraits::Mul( pr, mr );
            UInt8PixelTraits::Add( pr, 1-mr/2 );
            UInt8PixelTraits::Mul( pb, mb );
            UInt8PixelTraits::Add( pb, 1-mb );
            break;
         default:
            break;
         }
         break;

      // alpha channels don't show color masks
      default:
         break;
      }
   }
}

template <class P>
void RenderThreadBase::Render( const GenericImage<P>& image )
{
   uint32 argb = 0xFF000000U;
   uint8* pb = (uint8*)&argb;
   uint8* pg = pb + 1;
   uint8* pr = pg + 1;
   uint8* pa = pr + 1;

   if ( Z0 >= 0 )
   {
      const typename P::sample* f0, * f1, * f2, * fa, * fw;
      uint32* b[ MaxZoom ];

      for ( int y = startRow; y < endRow; ++y )
      {
         for ( int dy = 0; dy < z; ++dy )
            b[dy] = ((uint32*)bitmap->ScanLine( Y0 + y*z + dy )) + X0;

         switch ( channel )
         {
         case DisplayChannel::RGBK:
            {
               f0 = image.PixelAddress( r.x0, r.y0+y, 0 );
               fw = f0 + w;

               if ( colorImage )
               {
                  f1 = image.PixelAddress( r.x0, r.y0+y, 1 );
                  f2 = image.PixelAddress( r.x0, r.y0+y, 2 );
                  fa = hasAlpha ? image.PixelAddress( r.x0, r.y0+y, 3 ) : 0;

                  do
                  {
                     *pr = noLUT[0] ? UInt8PixelTraits::ToSample( *f0++ ) : LUT[0][UInt16PixelTraits::ToSample( *f0++ )];
                     *pg = noLUT[1] ? UInt8PixelTraits::ToSample( *f1++ ) : LUT[1][UInt16PixelTraits::ToSample( *f1++ )];
                     *pb = noLUT[2] ? UInt8PixelTraits::ToSample( *f2++ ) : LUT[2][UInt16PixelTraits::ToSample( *f2++ )];
                     if ( hasAlpha )
                        *pa = UInt8PixelTraits::ToSample( *fa++ );

                     for ( int dy = 0; dy < z; ++dy )
                        for ( int dx = 0; dx < z; ++dx )
                           *(b[dy])++ = argb;
                  }
                  while ( f0 != fw );
               }
               else
               {
                  fa = hasAlpha ? image.PixelAddress( r.x0, r.y0+y, 1 ) : 0;

                  do
                  {
                     *pb = *pg = *pr =
                        noLUT[0] ? UInt8PixelTraits::ToSample( *f0++ ) : LUT[0][UInt16PixelTraits::ToSample( *f0++ )];
                     if ( hasAlpha )
                        *pa = UInt8PixelTraits::ToSample( *fa++ );

                     for ( int dy = 0; dy < z; ++dy )
                        for ( int dx = 0; dx < z; ++dx )
                           *(b[dy])++ = argb;
                  }
                  while ( f0 != fw );
               }
            }
            break;

         case DisplayChannel::Red:
         case DisplayChannel::Green:
         case DisplayChannel::Blue:
            {
               int ch = channel - DisplayChannel::Red;

               f0 = image.PixelAddress( r.x0, r.y0+y, ch );
               fw = f0 + w;
               fa = hasAlpha ? image.PixelAddress( r.x0, r.y0+y, 3 ) : 0;

               do
               {
                  *pb = *pg = *pr =
                     noLUT[ch] ? UInt8PixelTraits::ToSample( *f0++ ) : LUT[ch][UInt16PixelTraits::ToSample( *f0++ )];
                  if ( hasAlpha )
                     *pa = UInt8PixelTraits::ToSample( *fa++ );

                  for ( int dy = 0; dy < z; ++dy )
                     for ( int dx = 0; dx < z; ++dx )
                        *(b[dy])++ = argb;
               }
               while ( f0 != fw );
            }
            break;

         default: // alpha channels
            {
               int ch = n + channel - DisplayChannel::Alpha;

               f0 = image.PixelAddress( r.x0, r.y0+y, ch );
               fw = f0 + w;

               do
               {
                  *pb = *pg = *pr = UInt8PixelTraits::ToSample( *f0++ );
                  for ( int dy = 0; dy < z; ++dy )
                     for ( int dx = 0; dx < z; ++dx )
                        *(b[dy])++ = argb;
               }
               while ( f0 != fw );
            }
            break;

         case DisplayChannel::Lightness:
         case DisplayChannel::CIE_X:
         case DisplayChannel::CIE_Y:
         case DisplayChannel::CIE_Z:
            {
               f0 = image.PixelAddress( r.x0, r.y0+y, 0 );
               f1 = image.PixelAddress( r.x0, r.y0+y, 1 );
               f2 = image.PixelAddress( r.x0, r.y0+y, 2 );
               fw = f0 + w;
               fa = hasAlpha ? image.PixelAddress( r.x0, r.y0+y, 3 ) : 0;

               do
               {
                  RGBColorSystem::sample r0, r1, r2, L;
                  P::FromSample( r0, *f0++ );
                  P::FromSample( r1, *f1++ );
                  P::FromSample( r2, *f2++ );
                  L = (rgbws->*F)( r0, r1, r2 );

                  *pb = *pg = *pr = noLUT[3] ?
                                 UInt8PixelTraits::ToSample( L ) :
                                 LUT[3][UInt16PixelTraits::ToSample( L )];
                  if ( hasAlpha )
                     *pa = UInt8PixelTraits::ToSample( *fa++ );

                  for ( int dy = 0; dy < z; ++dy )
                     for ( int dx = 0; dx < z; ++dx )
                        *(b[dy])++ = argb;
               }
               while ( f0 != fw );
            }
            break;

         case DisplayChannel::ChrominanceRG:
            {
               f0 = image.PixelAddress( r.x0, r.y0+y, 0 );
               f1 = image.PixelAddress( r.x0, r.y0+y, 1 );
               f2 = image.PixelAddress( r.x0, r.y0+y, 2 );
               fw = f0 + w;
               fa = hasAlpha ? image.PixelAddress( r.x0, r.y0+y, 3 ) : 0;

               do
               {
                  RGBColorSystem::sample r0, r1, r2;
                  P::FromSample( r0, *f0++ );
                  P::FromSample( r1, *f1++ );
                  P::FromSample( r2, *f2++ );

                  rgbws->RGBToCIEab( r0, r1, r0, r1, r2 );

                  *pr = UInt8PixelTraits::ToSample( r0 );
                  *pg = UInt8PixelTraits::ToSample( r1 );
                  if ( hasAlpha )
                     *pa = UInt8PixelTraits::ToSample( *fa++ );

                  for ( int dy = 0; dy < z; ++dy )
                     for ( int dx = 0; dx < z; ++dx )
                        *(b[dy])++ = argb;
               }
               while ( f0 != fw );
            }
            break;

         case DisplayChannel::ChrominanceLFixed:
            {
               f0 = image.PixelAddress( r.x0, r.y0+y, 0 );
               f1 = image.PixelAddress( r.x0, r.y0+y, 1 );
               f2 = image.PixelAddress( r.x0, r.y0+y, 2 );
               fw = f0 + w;
               fa = hasAlpha ? image.PixelAddress( r.x0, r.y0+y, 3 ) : 0;

               do
               {
                  RGBColorSystem::sample r0, r1, r2;
                  P::FromSample( r0, *f0++ );
                  P::FromSample( r1, *f1++ );
                  P::FromSample( r2, *f2++ );

                  rgbws->RGBToCIEab( r0, r1, r0, r1, r2 );
                  rgbws->CIELabToRGB( r0, r1, r2, 0.5, r0, r1 );

                  *pr = UInt8PixelTraits::ToSample( r0 );
                  *pg = UInt8PixelTraits::ToSample( r1 );
                  *pb = UInt8PixelTraits::ToSample( r2 );
                  if ( hasAlpha )
                     *pa = UInt8PixelTraits::ToSample( *fa++ );

                  for ( int dy = 0; dy < z; ++dy )
                     for ( int dx = 0; dx < z; ++dx )
                        *(b[dy])++ = argb;
               }
               while ( f0 != fw );
            }
            break;

         case DisplayChannel::CIE_a:
         case DisplayChannel::CIE_b:
         case DisplayChannel::CIE_c:
         case DisplayChannel::CIE_h:
         case DisplayChannel::Hue:
         case DisplayChannel::SaturationHSV:
         case DisplayChannel::SaturationHSI:
         case DisplayChannel::Value:
         case DisplayChannel::Intensity:
            {
               f0 = image.PixelAddress( r.x0, r.y0+y, 0 );
               f1 = image.PixelAddress( r.x0, r.y0+y, 1 );
               f2 = image.PixelAddress( r.x0, r.y0+y, 2 );
               fw = f0 + w;
               fa = hasAlpha ? image.PixelAddress( r.x0, r.y0+y, 3 ) : 0;

               do
               {
                  RGBColorSystem::sample r0, r1, r2;
                  P::FromSample( r0, *f0++ );
                  P::FromSample( r1, *f1++ );
                  P::FromSample( r2, *f2++ );

                  *pb = *pg = *pr = UInt8PixelTraits::ToSample( (rgbws->*F)( r0, r1, r2 ) );
                  if ( hasAlpha )
                     *pa = UInt8PixelTraits::ToSample( *fa++ );

                  for ( int dy = 0; dy < z; ++dy )
                     for ( int dx = 0; dx < z; ++dx )
                        *(b[dy])++ = argb;
               }
               while ( f0 != fw );
            }
            break;
         }
      }
   }
   else // Z0 < 0
   {
      const typename P::sample* f[ 3 ][ -MinZoom ]; // R,G,B
      const typename P::sample* fa[ -MinZoom ]; // alpha

// ###
/*
      int SN = 4*((z*z)/4 + (((z*z)%4 != 0) ? 1 : 0));
      float* S = (float*)_mm_malloc( SN*sizeof( float ), 16 );
      memset( S, 0, SN*sizeof( float ) );
*/
// ###

      for ( int y = startRow, qy = Y0 + startRow; y < endRow; ++y, ++qy )
      {
         uint32* b = ((uint32*)bitmap->ScanLine( qy )) + X0;

         switch ( channel )
         {
         case DisplayChannel::RGBK:

            for ( int c = 0; c < n; ++c )
               for ( int i = 0; i < z; ++i )
                  f[c][i] = image.PixelAddress( r.x0, r.y0 + y*z + i, c );

            if ( hasAlpha )
               for ( int i = 0, c = colorImage ? 3 : 1; i < z; ++i )
                  fa[i] = image.PixelAddress( r.x0, r.y0 + y*z + i, c );

            for ( int x = 0; x < wz; ++x )
            {
// ###
/*
               for ( int c = 0; c < n; ++c )
               {
                  float* p = S;
                  for ( int i = 0, xz = x*z; i < z; ++i )
                     for ( int j = 0, xj = xz; j < z; ++j, ++xj )
                        *p++ = float( f[c][i][xj] );

                  __m128* m = reinterpret_cast<__m128*>( S );
                  union { __m128 m; float f[ 4 ]; }  __attribute__ ((aligned (16))) s;
                  s.f[0] = s.f[1] = s.f[2] = s.f[3] = 0;
                  for ( int i = 0; i < SN; i += 4 )
                     s.m = _mm_add_ps( s.m, *m++ );

                  float ss = s.f[0] + s.f[1] + s.f[2] + s.f[3];

                  bx[c] = noLUT[c] ? UInt8PixelTraits::FloatToSample( ss*_255_Pz2 ) :
                                     LUT[c][UInt16PixelTraits::FloatToSample( ss*_64K_Pz2 )];
               }
*/
// ###
               double s = SubsampledTotal( f[0], x );
               *pr = noLUT[0] ? UInt8PixelTraits::FloatToSample( s*_255_Pz2 ) :
                                 LUT[0][UInt16PixelTraits::FloatToSample( s*_64K_Pz2 )];
               if ( colorImage )
               {
                  s = SubsampledTotal( f[1], x );
                  *pg = noLUT[1] ? UInt8PixelTraits::FloatToSample( s*_255_Pz2 ) :
                                   LUT[1][UInt16PixelTraits::FloatToSample( s*_64K_Pz2 )];
                  s = SubsampledTotal( f[2], x );
                  *pb = noLUT[2] ? UInt8PixelTraits::FloatToSample( s*_255_Pz2 ) :
                                   LUT[2][UInt16PixelTraits::FloatToSample( s*_64K_Pz2 )];
               }
               else
                  *pg = *pb = *pr;

               if ( hasAlpha )
                  *pa = UInt8PixelTraits::FloatToSample( SubsampledTotal( fa, x )*_255_Pz2 );

               *b++ = argb;
            }

            break;

         case DisplayChannel::Red:
         case DisplayChannel::Green:
         case DisplayChannel::Blue:
            {
               int ch = channel - DisplayChannel::Red;
               const typename P::sample** f0 = f[0];

               for ( int i = 0; i < z; ++i )
                  f0[i] = image.PixelAddress( r.x0, r.y0 + y*z + i, ch );

               if ( hasAlpha )
                  for ( int i = 0, c = colorImage ? 3 : 1; i < z; ++i )
                     fa[i] = image.PixelAddress( r.x0, r.y0 + y*z + i, c );

               for ( int x = 0; x < wz; ++x )
               {
                  double s = SubsampledTotal( f0, x );
                  *pr = *pg = *pb = noLUT[ch] ?
                     UInt8PixelTraits::FloatToSample( s*_255_Pz2 ) :
                     LUT[ch][UInt16PixelTraits::FloatToSample( s*_64K_Pz2 )];

                  if ( hasAlpha )
                     *pa = UInt8PixelTraits::FloatToSample( SubsampledTotal( fa, x )*_255_Pz2 );

                  *b++ = argb;
               }
            }
            break;

         default: // alpha channels
            {
               int ch = n + channel - DisplayChannel::Alpha;
               const typename P::sample** f0 = f[0];

               for ( int i = 0; i < z; ++i )
                  f0[i] = image.PixelAddress( r.x0, r.y0 + y*z + i, ch );

               for ( int x = 0; x < wz; ++x )
               {
                  *pr = *pg = *pb = UInt8PixelTraits::FloatToSample( SubsampledTotal( f0, x )*_255_Pz2 );
                  *b++ = argb;
               }
            }
            break;

         case DisplayChannel::Lightness:
         case DisplayChannel::CIE_X:
         case DisplayChannel::CIE_Y:
         case DisplayChannel::CIE_Z:

            for ( int c = 0; c < 3; ++c )
               for ( int i = 0; i < z; ++i )
                  f[c][i] = image.PixelAddress( r.x0, r.y0 + y*z + i, c );

            if ( hasAlpha )
               for ( int i = 0, c = colorImage ? 3 : 1; i < z; ++i )
                  fa[i] = image.PixelAddress( r.x0, r.y0 + y*z + i, c );

            for ( int x = 0; x < wz; ++x )
            {
               double L = (rgbws->*F)( SubsampledTotal( f[0], x ) * _1_Pz2,
                                       SubsampledTotal( f[1], x ) * _1_Pz2,
                                       SubsampledTotal( f[2], x ) * _1_Pz2 );
               *pr = *pg = *pb = noLUT[3] ?
                        UInt8PixelTraits::ToSample( L ) :
                        LUT[3][UInt16PixelTraits::ToSample( L )];

               if ( hasAlpha )
                  *pa = UInt8PixelTraits::FloatToSample( SubsampledTotal( fa, x )*_255_Pz2 );

               *b++ = argb;
            }

            break;

         case DisplayChannel::ChrominanceRG:

            for ( int c = 0; c < 3; ++c )
               for ( int i = 0; i < z; ++i )
                  f[c][i] = image.PixelAddress( r.x0, r.y0 + y*z + i, c );

            if ( hasAlpha )
               for ( int i = 0, c = colorImage ? 3 : 1; i < z; ++i )
                  fa[i] = image.PixelAddress( r.x0, r.y0 + y*z + i, c );

            for ( int x = 0; x < wz; ++x )
            {
               RGBColorSystem::sample r0, r1;
               rgbws->RGBToCIEab( r0, r1, SubsampledTotal( f[0], x ) * _1_Pz2,
                                          SubsampledTotal( f[1], x ) * _1_Pz2,
                                          SubsampledTotal( f[2], x ) * _1_Pz2 );
               *pr = UInt8PixelTraits::ToSample( r0 );
               *pg = UInt8PixelTraits::ToSample( r1 );

               if ( hasAlpha )
                  *pa = UInt8PixelTraits::FloatToSample( SubsampledTotal( fa, x )*_255_Pz2 );

               *b++ = argb;
            }

            break;

         case DisplayChannel::ChrominanceLFixed:

            for ( int c = 0; c < 3; ++c )
               for ( int i = 0; i < z; ++i )
                  f[c][i] = image.PixelAddress( r.x0, r.y0 + y*z + i, c );

            if ( hasAlpha )
               for ( int i = 0, c = colorImage ? 3 : 1; i < z; ++i )
                  fa[i] = image.PixelAddress( r.x0, r.y0 + y*z + i, c );

            for ( int x = 0; x < wz; ++x )
            {
               RGBColorSystem::sample r0, r1, r2;
               rgbws->RGBToCIEab( r0, r1, SubsampledTotal( f[0], x ) * _1_Pz2,
                                          SubsampledTotal( f[1], x ) * _1_Pz2,
                                          SubsampledTotal( f[2], x ) * _1_Pz2 );
               rgbws->CIELabToRGB( r0, r1, r2, 0.5, r0, r1 );
               *pr = UInt8PixelTraits::ToSample( r0 );
               *pg = UInt8PixelTraits::ToSample( r1 );
               *pb = UInt8PixelTraits::ToSample( r2 );

               if ( hasAlpha )
                  *pa = UInt8PixelTraits::FloatToSample( SubsampledTotal( fa, x )*_255_Pz2 );

               *b++ = argb;
            }

            break;

         case DisplayChannel::CIE_a:
         case DisplayChannel::CIE_b:
         case DisplayChannel::CIE_c:
         case DisplayChannel::CIE_h:
         case DisplayChannel::Hue:
         case DisplayChannel::SaturationHSV:
         case DisplayChannel::SaturationHSI:
         case DisplayChannel::Value:
         case DisplayChannel::Intensity:

            for ( int c = 0; c < 3; ++c )
               for ( int i = 0; i < z; ++i )
                  f[c][i] = image.PixelAddress( r.x0, r.y0 + y*z + i, c );

            if ( hasAlpha )
               for ( int i = 0, c = colorImage ? 3 : 1; i < z; ++i )
                  fa[i] = image.PixelAddress( r.x0, r.y0 + y*z + i, c );

            for ( int x = 0; x < wz; ++x )
            {
               *pr = *pg = *pb = UInt8PixelTraits::ToSample(
                                       (rgbws->*F)( SubsampledTotal( f[0], x ) * _1_Pz2,
                                                    SubsampledTotal( f[1], x ) * _1_Pz2,
                                                    SubsampledTotal( f[2], x ) * _1_Pz2 ) );

               if ( hasAlpha )
                  *pa = UInt8PixelTraits::FloatToSample( SubsampledTotal( fa, x )*_255_Pz2 );

               *b++ = argb;
            }

            break;
         }
      }
// ###
/*
      _mm_free( S );
*/
// ###
   }
}

template <class M>
void RenderThreadBase::Mask( const GenericImage<M>& mask )
{
   uint32 argb = 0xFF000000U;
   uint8* pb = (uint8*)&argb;
   uint8* pg = pb + 1;
   uint8* pr = pg + 1;

   if ( Z0 >= 0 )
   {
      uint32* b[ MaxZoom ];

      for ( int y = startRow; y < endRow; ++y )
      {
         for ( int dy = 0; dy < z; ++dy )
            b[dy] = ((uint32*)bitmap->ScanLine( Y0 + y*z + dy )) + X0;

         const typename M::sample* m0 = mask.PixelAddress( mp.x, mp.y+y, 0 );
         const typename M::sample* mw = m0 + w;

         if ( colorMask )
         {
            const typename M::sample* m1 = mask.PixelAddress( mp.x, mp.y+y, 1 );
            const typename M::sample* m2 = mask.PixelAddress( mp.x, mp.y+y, 2 );

            for ( ;; )
            {
               argb = **b;

               if ( maskInverted )
                  ApplyMask( *pr, *pg, *pb,
                             FloatPixelTraits::ToSample( (typename M::sample)(M::MaxSampleValue() - *m0) ),
                             FloatPixelTraits::ToSample( (typename M::sample)(M::MaxSampleValue() - *m1) ),
                             FloatPixelTraits::ToSample( (typename M::sample)(M::MaxSampleValue() - *m2) ) );
               else
                  ApplyMask( *pr, *pg, *pb,
                             FloatPixelTraits::ToSample( *m0 ),
                             FloatPixelTraits::ToSample( *m1 ),
                             FloatPixelTraits::ToSample( *m2 ) );

               for ( int dy = 0; dy < z; ++dy )
                  for ( int dx = 0; dx < z; ++dx )
                     *(b[dy])++ = argb;

               if ( ++m0 == mw )
                  break;

               ++m1;
               ++m2;
            }
         }
         else // !colorMask
         {
            do
            {
               argb = **b;

               ApplyMask( *pr, *pg, *pb, FloatPixelTraits::ToSample(
                              maskInverted ? (typename M::sample)(M::MaxSampleValue() - *m0) : *m0 ) );

               for ( int dy = 0; dy < z; ++dy )
                  for ( int dx = 0; dx < z; ++dx )
                     *(b[dy])++ = argb;
            }
            while ( ++m0 != mw );
         }
      }
   }
   else // Z0 < 0
   {
      for ( int y = startRow, qy = Y0 + startRow; y < endRow; ++y, ++qy )
      {
         uint32* b = ((uint32*)bitmap->ScanLine( qy )) + X0;

         if ( colorMask )
         {
            const typename M::sample* m[ 3 ][ -MinZoom ];
            for ( int c = 0; c < 3; ++c )
               for ( int i = 0; i < z; ++i )
                  m[c][i] = mask.PixelAddress( mp.x, mp.y + y*z + i, c );

            for ( int x = 0; x < wz; ++x, ++b )
            {
               float s0 = SubsampledTotal( m[0], x ) * _1_Mz2;
               float s1 = SubsampledTotal( m[1], x ) * _1_Mz2;
               float s2 = SubsampledTotal( m[2], x ) * _1_Mz2;
               if ( maskInverted )
                  ApplyMask( ((uint8*)b)[2], ((uint8*)b)[1], ((uint8*)b)[0], 1-s0, 1-s1, 1-s2 );
               else
                  ApplyMask( ((uint8*)b)[2], ((uint8*)b)[1], ((uint8*)b)[0], s0, s1, s2 );
            }
         }
         else // !colorMask
         {
            const typename M::sample* m[ -MinZoom ];
            for ( int i = 0; i < z; ++i )
               m[i] = mask.PixelAddress( mp.x, mp.y + y*z + i, 0 );

            for ( int x = 0; x < wz; ++x, ++b )
            {
               float s = SubsampledTotal( m, x ) * _1_Mz2;
               ApplyMask( ((uint8*)b)[2], ((uint8*)b)[1], ((uint8*)b)[0], maskInverted ? 1-s : s );
            }
         }
      }
   }
}

#undef bitmap
#undef X0
#undef Y0
#undef Z0
#undef z
#undef channel
#undef maskMode
#undef maskInverted
#undef colorImage
#undef colorMask
#undef LUT
#undef noLUT
#undef r
#undef w
#undef h
#undef n
#undef mp
#undef hasAlpha
#undef F
#undef hq
#undef wz
#undef hz
#undef iz
#undef _1_Pz2
#undef _255_Pz2
#undef _64K_Pz2
#undef _1_Mz2
#undef rgbws

// ----------------------------------------------------------------------------

template <class P, class M> inline
bool __Render( Bitmap& bitmap, int x0, int y0, int z, int channel,
               const GenericImage<P>& image,
               bool showAlpha,
               const GenericImage<M>* mask, int maskMode, bool maskInverted,
               const uint8** LUT,
               bool fast,
               bool (*callback)() )
{
   if ( z == 0 || z == -1 )
      z = 1;

   RenderThreadData data;
   if ( !RenderThreadBase::InitData( data,
                                     bitmap, x0, y0, z, channel, image, showAlpha,
                                     mask, maskMode, maskInverted, LUT, fast ) )
   {
      return false;
   }

   int h = (z < 0) ? data.hz : data.h;

   if ( callback == 0 )
   {
      int numberOfThreads = Thread::NumberOfThreads( h, 1 );
      if ( numberOfThreads > 1 )
      {
         int rowsPerThread = h/numberOfThreads;
         IndirectArray<RenderThread<P,M> > threads;
         for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
            threads.Add( new RenderThread<P,M>( &data,
                                                i*rowsPerThread,
                                                (j < numberOfThreads) ? j*rowsPerThread : h ) );
         for ( int i = 0; i < numberOfThreads; ++i )
            threads[i]->Start( ThreadPriority::DefaultMax, i );
         for ( int i = 0; i < numberOfThreads; ++i )
            threads[i]->Wait();

         threads.Destroy();
      }
      else
      {
         RenderThread<P,M> thread( &data, 0, h );
         thread.Run();
      }
   }
   else
   {
      static int numberOfProcessors = 0;
      if ( numberOfProcessors <= 0 )
         numberOfProcessors = Max( 1, PixInsightSettings::GlobalInteger( "System/NumberOfProcessors" ) );

      int rowsPerStep = Max( 1, 65536/data.w );
      if ( z < 0 )
         rowsPerStep = Max( 1, rowsPerStep/data.z );
      rowsPerStep *= numberOfProcessors;

      int numberOfSteps = Max( 1, h/rowsPerStep );
      rowsPerStep = h/numberOfSteps;

      for ( int step = 0; step < numberOfSteps; ++step )
      {
         int firstRow = step*rowsPerStep;
         int numberOfRows = (step < numberOfSteps-1) ? rowsPerStep : h - firstRow;
         int numberOfThreads = Thread::NumberOfThreads( numberOfRows, 1 );

         if ( numberOfThreads > 1 )
         {
            int rowsPerThread = numberOfRows/numberOfThreads;
            IndirectArray<RenderThread<P,M> > threads;
            for ( int i = 0, j = 1; i < numberOfThreads; ++i, ++j )
               threads.Add( new RenderThread<P,M>( &data,
                                                   firstRow + i*rowsPerThread,
                                                   firstRow + ((j < numberOfThreads) ? j*rowsPerThread : numberOfRows ) ) );
            for ( int i = 0; i < numberOfThreads; ++i )
               threads[i]->Start( ThreadPriority::DefaultMax, i );
            for ( int i = 0; i < numberOfThreads; ++i )
               threads[i]->Wait();

            threads.Destroy();
         }
         else
         {
            RenderThread<P,M> thread( &data, firstRow, firstRow + numberOfRows );
            thread.Run();
         }

         if ( !(*callback)() )
            return false;
      }
   }

   return true;
}

// ----------------------------------------------------------------------------

template <class P> inline
bool __Render( Bitmap& bitmap, int x, int y, int zoom, int channel,
               const GenericImage<P>& image,
               bool showAlpha,
               const ImageVariant* mask, int maskMode, bool maskInverted,
               const uint8** LUT,
               bool fast,
               bool (*callback)() )
{
   if ( mask == 0 || !*mask )
      return __Render( bitmap, x, y, zoom, channel, image, showAlpha,
                       static_cast<const GenericImage<P>*>( 0 ),
                       0, false, LUT, fast, callback );
   else
   {
      if ( mask->IsFloatSample() )
         switch ( mask->BitsPerSample() )
         {
         case 32: return __Render( bitmap, x, y, zoom, channel, image, showAlpha,
                                   &static_cast<const Image&>( **mask ),
                                   maskMode, maskInverted, LUT, fast, callback );

         case 64: return __Render( bitmap, x, y, zoom, channel, image, showAlpha,
                                   &static_cast<const DImage&>( **mask ),
                                   maskMode, maskInverted, LUT, fast, callback );
         }
      else
         switch ( mask->BitsPerSample() )
         {
         case  8: return __Render( bitmap, x, y, zoom, channel, image, showAlpha,
                                   &static_cast<const UInt8Image&>( **mask ),
                                   maskMode, maskInverted, LUT, fast, callback );

         case 16: return __Render( bitmap, x, y, zoom, channel, image, showAlpha,
                                   &static_cast<const UInt16Image&>( **mask ),
                                   maskMode, maskInverted, LUT, fast, callback );

         case 32: return __Render( bitmap, x, y, zoom, channel, image, showAlpha,
                                   &static_cast<const UInt32Image&>( **mask ),
                                   maskMode, maskInverted, LUT, fast, callback );
         }
   }

   return false;
}

// ----------------------------------------------------------------------------

bool Render( Bitmap& bitmap, int x, int y, int zoom, int channel,
             const ImageVariant& image,
             bool showAlpha,
             const ImageVariant* mask, int maskMode, bool maskInverted,
             const uint8** LUT,
             bool fast,
             bool (*callback)() )
{
   if ( image )
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32: return __Render( bitmap, x, y, zoom, channel,
                                   static_cast<const Image&>( *image ),
                                   showAlpha, mask, maskMode, maskInverted, LUT, fast, callback );

         case 64: return __Render( bitmap, x, y, zoom, channel,
                                   static_cast<const DImage&>( *image ),
                                   showAlpha, mask, maskMode, maskInverted, LUT, fast, callback );
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8: return __Render( bitmap, x, y, zoom, channel,
                                   static_cast<const UInt8Image&>( *image ),
                                   showAlpha, mask, maskMode, maskInverted, LUT, fast, callback );

         case 16: return __Render( bitmap, x, y, zoom, channel,
                                   static_cast<const UInt16Image&>( *image ),
                                   showAlpha, mask, maskMode, maskInverted, LUT, fast, callback );

         case 32: return __Render( bitmap, x, y, zoom, channel,
                                   static_cast<const UInt32Image&>( *image ),
                                   showAlpha, mask, maskMode, maskInverted, LUT, fast, callback );
         }

   return false;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF pcl/Render.cpp - Released 2017-08-01T14:23:38Z
