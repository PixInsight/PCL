// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard Convolution Process Module Version 01.01.03.0138
// ****************************************************************************
// LarsonSekaninaInstance.cpp - Released 2014/10/29 07:34:55 UTC
// ****************************************************************************
// This file is part of the standard Convolution PixInsight module.
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

#include "LarsonSekaninaInstance.h"
#include "LarsonSekaninaParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/Image.h>
#include <pcl/PixelInterpolation.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

LarsonSekaninaInstance::LarsonSekaninaInstance( const MetaProcess* m ) :
ProcessImplementation( m ),
interpolation( LSInterpolation::Default ),
radiusDiff( TheLSRadiusDiffParameter->DefaultValue() ),
angleDiff( TheLSAngleDiffParameter->DefaultValue() ),
center( TheLSCenterXParameter->DefaultValue(), TheLSCenterYParameter->DefaultValue() ),
amount( TheLSAmountParameter->DefaultValue() ),
threshold( TheLSThresholdParameter->DefaultValue() ),
deringing( TheLSDeringingParameter->DefaultValue() ),
useLuminance( TheLSUseLuminanceParameter->DefaultValue() ),
highPass( TheLSHighPassParameter->DefaultValue() ),
rangeLow( TheLSRangeLowParameter->DefaultValue() ),
rangeHigh( TheLSRangeHighParameter->DefaultValue() ),
disableExtension( TheLSDisableExtensionParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

LarsonSekaninaInstance::LarsonSekaninaInstance( const LarsonSekaninaInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void LarsonSekaninaInstance::Assign( const ProcessImplementation& p )
{
   const LarsonSekaninaInstance* x = dynamic_cast<const LarsonSekaninaInstance*>( &p );
   if ( x != 0 )
   {
      interpolation = x->interpolation;
      radiusDiff = x->radiusDiff;
      angleDiff = x->angleDiff;
      center = x->center;
      amount = x->amount;
      threshold = x->threshold;
      deringing = x->deringing;
      useLuminance = x->useLuminance;
      highPass = x->highPass;
      rangeLow = x->rangeLow;
      rangeHigh = x->rangeHigh;
      disableExtension = x ->disableExtension;
   }
}

// ----------------------------------------------------------------------------

bool LarsonSekaninaInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "LarsonSekanina cannot be executed on complex images.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------

inline static
void ToPolar( int x, int y, DPoint center, double& r, double& theta )
{
   double dx = x - center.x;
   double dy = y - center.y;
   r = Sqrt( dx*dx + dy*dy );
   theta = ArcTan( dy, dx );
}

inline static
void ToCartesian( double r, double theta, DPoint center, DPoint& p )
{
   p.x = center.x + r*Cos( theta );
   p.y = center.y + r*Sin( theta );
}

template <class P1, class P2> static
void Convolve_2( const GenericImage<P1>& image, GenericImage<P2>& sharp,
                 pcl_enum interpolation, float dR, float angleD, DPoint center, int c )
{
   PixelInterpolation* P = 0;
   PixelInterpolation::Interpolator<P1>* interpolator = 0;

   try
   {
      switch ( interpolation )
      {
      case LSInterpolation::Bilinear:
         P = new BilinearPixelInterpolation();
         break;
      default:
      case LSInterpolation::Bicubic:
         P = new BicubicPixelInterpolation();
         break;
      case LSInterpolation::BicubicSpline:
         P = new BicubicSplinePixelInterpolation();
         break;
      case LSInterpolation::BicubicBSpline:
         P = new BicubicBSplinePixelInterpolation();
         break;
      }

      interpolator = P->NewInterpolator( (P1*)0, image[c], image.Width(), image.Height() );

      int w = image.Width() - 1;
      int h = image.Height() - 1;

      double fimg, fsharp;
      StatusMonitor monitor;
      monitor.Initialize( "<end><cbr>High-pass Larson-Sekanina filter", image.NumberOfPixels() );

      sharp.Zero();

      float dAlpha = Rad( angleD );
      for ( int x = 0; x < image.Width(); ++x )
         for ( int y = 0; y < image.Height(); ++y, ++monitor )
         {
            // Get the central value
            P1::FromSample( fimg, image.Pixel( x, y, c ) );
            fsharp = fimg+fimg;

            double r, theta;
            ToPolar( x, y, center, r, theta);

            DPoint delta;

            // Positive differential
            ToCartesian( r-dR, theta+dAlpha, center, delta );
            if ( delta.x < 0 )
               delta.x = Abs( delta.x );
            else if ( delta.x > w )
               delta.x = 2*w - delta.x;
            if ( delta.y < 0 )
               delta.y = Abs( delta.y );
            else if ( delta.y > h )
               delta.y = 2*h - delta.y;
            P1::FromSample( fimg, (*interpolator)( delta ) );
            fsharp -= fimg;

            //Negative differential
            ToCartesian( r-dR, theta-dAlpha, center, delta );
            if ( delta.x < 0 )
               delta.x = Abs( delta.x );
            else if ( delta.x > w )
               delta.x = 2*w - delta.x;
            if ( delta.y < 0 )
               delta.y = Abs( delta.y );
            else if ( delta.y > h )
               delta.y = 2*h - delta.y;
            P1::FromSample( fimg, (*interpolator)( delta ) );
            fsharp -= fimg;

            sharp.Pixel( x, y ) = P2::ToSample( fsharp );
         }

      delete interpolator;
      delete P;
   }

   catch ( ... )
   {
      if ( interpolator != 0 )
         delete interpolator;
      if ( P != 0 )
         delete P;
      throw;
   }
}

template <class P> static
void Convolve_1( const GenericImage<P>& image, ImageVariant& v2,
                 pcl_enum interpolation, float radiusD, float angleD, DPoint center, int c )
{
   switch ( v2.BitsPerSample() )
   {
   case 32 : Convolve_2( image, static_cast<Image&>( *v2 ), interpolation, radiusD, angleD, center, c ); break;
   case 64 : Convolve_2( image, static_cast<DImage&>( *v2 ), interpolation, radiusD, angleD, center, c ); break;
   }
}

static
void Convolve( const ImageVariant& v1, ImageVariant& v2,
               pcl_enum interpolation, float radiusD, float angleD, DPoint center, int c )
{
   if ( v1.IsFloatSample() )
      switch ( v1.BitsPerSample() )
      {
      case 32 : Convolve_1( static_cast<const Image&>( *v1 ), v2, interpolation, radiusD, angleD, center, c ); break;
      case 64 : Convolve_1( static_cast<const DImage&>( *v1 ), v2, interpolation, radiusD, angleD, center, c ); break;
      }
   else
      switch ( v1.BitsPerSample() )
      {
      case  8 : Convolve_1( static_cast<const UInt8Image&>( *v1 ), v2, interpolation, radiusD, angleD, center, c ); break;
      case 16 : Convolve_1( static_cast<const UInt16Image&>( *v1 ), v2, interpolation, radiusD, angleD, center, c ); break;
      case 32 : Convolve_1( static_cast<const UInt32Image&>( *v1 ), v2, interpolation, radiusD, angleD, center, c ); break;
      }
}

// ----------------------------------------------------------------------------

inline static
void Apply_PixelValues( double& imgPixel, double& sharpPixel, float threshold, float deringing, float amount, pcl_bool highPass)
{
   if ( Abs( sharpPixel ) > threshold)
   {
      if ( sharpPixel < 0 )
         sharpPixel *= 1 - deringing;

      if ( highPass )
         imgPixel = imgPixel*(1 - amount) + amount*sharpPixel;
      else
         imgPixel += 10*amount*sharpPixel;
   }
}

template <class P1, class P2> static
void ApplyFilter_2( GenericImage<P1>& image, const GenericImage<P2>& sharp,
                    float amount, float threshold, float deringing,
                    float rangeLow, float rangeHigh, pcl_bool disableExtension, int c, pcl_bool highPass )
{
   float rangeWidth = 1 + rangeHigh + rangeLow;
   bool isRange = rangeWidth + 1 != 1;

   StandardStatus callback;
   StatusMonitor monitor;
   monitor.SetCallback( &callback );
   monitor.Initialize( "<end><cbr>Larson-Sekanina filter", image.NumberOfPixels() );

   for ( int x = 0; x < image.Width(); ++x )
      for ( int y = 0; y < image.Height(); ++y, ++monitor )
      {
         double f1, f2;
         P1::FromSample( f1, image.Pixel( x, y, c ) );
         P2::FromSample( f2, sharp.Pixel( x, y ) );
         Apply_PixelValues( f1, f2, threshold, deringing, amount, highPass );

         if ( disableExtension )
            image.Pixel( x, y, c ) = P1::ToSample( f1 );
         else
         {
            if ( isRange )
               f1 = (f1 + rangeLow)/rangeWidth;
            image.Pixel( x, y, c ) = P1::ToSample( pcl::Range( f1, 0.0, 1.0 ) );
         }
      }

   if ( disableExtension )
      Console().WarningLn( "<end><cbr>*** Warning: Dynamic range extension has been disabled - check pixel values!" );
}

template <class P> static
void ApplyFilter_1( GenericImage<P>& image, const ImageVariant& v2,
                    float amount, float threshold, float deringing,
                    float rangeLow, float rangeHigh, pcl_bool disableExtension, int c, pcl_bool highPass )
{
   switch ( v2.BitsPerSample() )
   {
   case 32 : ApplyFilter_2( image, static_cast<const Image&>( *v2 ),
                            amount, threshold, deringing, rangeLow, rangeHigh, disableExtension, c, highPass ); break;
   case 64 : ApplyFilter_2( image, static_cast<const DImage&>( *v2 ),
                            amount, threshold, deringing, rangeLow, rangeHigh, disableExtension, c, highPass ); break;
   }
}

static
void ApplyFilter( ImageVariant& v1, const ImageVariant& v2,
                  float amount, float threshold, float deringing,
                  float rangeLow, float rangeHigh, pcl_bool disableExtension, int c, pcl_bool highPass )
{
   if ( v1.IsFloatSample() )
      switch ( v1.BitsPerSample() )
      {
      case 32 : ApplyFilter_1( static_cast<Image&>( *v1 ), v2,
                               amount, threshold, deringing, rangeLow, rangeHigh, disableExtension, c, highPass ); break;
      case 64 : ApplyFilter_1( static_cast<DImage&>( *v1 ), v2,
                               amount, threshold, deringing, rangeLow, rangeHigh, disableExtension, c, highPass ); break;
      }
   else
      switch ( v1.BitsPerSample() )
      {
      case  8 : ApplyFilter_1( static_cast<UInt8Image&>( *v1 ), v2,
                               amount, threshold, deringing, rangeLow, rangeHigh, false, c, highPass ); break;
      case 16 : ApplyFilter_1( static_cast<UInt16Image&>( *v1 ), v2,
                               amount, threshold, deringing, rangeLow, rangeHigh, false, c, highPass ); break;
      case 32 : ApplyFilter_1( static_cast<UInt32Image&>( *v1 ), v2,
                               amount, threshold, deringing, rangeLow, rangeHigh, false, c, highPass ); break;
      }
}

// ----------------------------------------------------------------------------

bool LarsonSekaninaInstance::ExecuteOn( View& view )
{
   AutoViewLock lock( view );

   ImageVariant image = view.Image();

   if ( image.IsComplexSample() )
      return false;

   StandardStatus status;
   image.SetStatusCallback( &status );

   Console().EnableAbort();

   ImageVariant sharpImg;
   sharpImg.CreateFloatImage( (image.BitsPerSample() > 32) ? image.BitsPerSample() : 32 );
   sharpImg.AllocateImage( image.AnyImage()->Width(), image.AnyImage()->Height(), 1, ColorSpace::Gray );

   if ( useLuminance && image.AnyImage()->IsColor() )
   {
      ImageVariant L;
      image.GetLightness( L );
      Convolve( L, sharpImg, interpolation, radiusDiff, angleDiff, center, 0 );
      ApplyFilter( L, sharpImg, amount, threshold, deringing, rangeLow, rangeHigh, false, 0, highPass );
      image.SetLightness( L );
   }
   else
   {
      for ( int c = 0, n = image.AnyImage()->NumberOfNominalChannels(); c < n; ++c )
      {
         image.AnyImage()->SelectChannel( c );
         if ( n > 1 )
            Console().WriteLn( "<end><cbr>Processing channel #" + String( c ) );

         Convolve( image, sharpImg, interpolation, radiusDiff, angleDiff, center, c );
         ApplyFilter( image, sharpImg, amount, threshold, deringing, rangeLow, rangeHigh, disableExtension, c, highPass );
      }
   }

   return true;
}

// ----------------------------------------------------------------------------

void* LarsonSekaninaInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheLSInterpolationParameter )
      return &interpolation;
   if ( p == TheLSRadiusDiffParameter )
      return &radiusDiff;
   if ( p == TheLSAngleDiffParameter )
      return &angleDiff;
   if ( p == TheLSCenterXParameter )
      return &center.x;
   if ( p == TheLSCenterYParameter )
      return &center.y;
   if ( p == TheLSAmountParameter )
      return &amount;
   if ( p == TheLSThresholdParameter )
      return &threshold;
   if ( p == TheLSDeringingParameter )
      return &deringing;
   if ( p == TheLSRangeLowParameter )
      return &rangeLow;
   if ( p == TheLSRangeHighParameter )
      return &rangeHigh;
   if ( p == TheLSUseLuminanceParameter )
      return &useLuminance;
   if ( p == TheLSHighPassParameter )
      return &highPass;
   if ( p == TheLSDisableExtensionParameter )
      return &disableExtension;
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF LarsonSekaninaInstance.cpp - Released 2014/10/29 07:34:55 UTC
