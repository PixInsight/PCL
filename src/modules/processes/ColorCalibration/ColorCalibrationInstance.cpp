//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.03.03.0336
// ----------------------------------------------------------------------------
// ColorCalibrationInstance.cpp - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "ColorCalibrationInstance.h"
#include "ColorCalibrationParameters.h"

#include <pcl/ATrousWaveletTransform.h>
#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/StdStatus.h>
#include <pcl/View.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ColorCalibrationInstance::ColorCalibrationInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   whiteLow( TheCCWhiteLowParameter->DefaultValue() ),
   whiteHigh( TheCCWhiteHighParameter->DefaultValue() ),
   whiteUseROI( TheCCWhiteUseROIParameter->DefaultValue() ),
   whiteROI( 0 ),
   structureDetection( TheCCStructureDetectionParameter->DefaultValue() ),
   structureLayers( TheCCStructureLayersParameter->DefaultValue() ),
   noiseLayers( TheCCNoiseLayersParameter->DefaultValue() ),
   manualWhiteBalance( TheCCManualWhiteBalanceParameter->DefaultValue() ),
   manualRedFactor( TheCCManualRedFactorParameter->DefaultValue() ),
   manualGreenFactor( TheCCManualGreenFactorParameter->DefaultValue() ),
   manualBlueFactor( TheCCManualBlueFactorParameter->DefaultValue() ),
   backgroundReferenceViewId(),
   backgroundLow( TheCCBackgroundLowParameter->DefaultValue() ),
   backgroundHigh( TheCCBackgroundHighParameter->DefaultValue() ),
   backgroundUseROI( TheCCBackgroundUseROIParameter->DefaultValue() ),
   backgroundROI( 0 ),
   outputWhiteReferenceMask( TheCCOutputWhiteReferenceMaskParameter->DefaultValue() ),
   outputBackgroundReferenceMask( TheCCOutputBackgroundReferenceMaskParameter->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

ColorCalibrationInstance::ColorCalibrationInstance( const ColorCalibrationInstance& x ) :
   ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void ColorCalibrationInstance::Assign( const ProcessImplementation& p )
{
   const ColorCalibrationInstance* x = dynamic_cast<const ColorCalibrationInstance*>( &p );
   if ( x != nullptr )
   {
      whiteReferenceViewId = x->whiteReferenceViewId;
      whiteLow = x->whiteLow;
      whiteHigh = x->whiteHigh;
      whiteUseROI = x->whiteUseROI;
      whiteROI = x->whiteROI;
      structureDetection = x->structureDetection;
      structureLayers = x->structureLayers;
      noiseLayers = x->noiseLayers;
      manualWhiteBalance = x->manualWhiteBalance;
      manualRedFactor = x->manualRedFactor;
      manualGreenFactor = x->manualGreenFactor;
      manualBlueFactor = x->manualBlueFactor;
      backgroundReferenceViewId = x->backgroundReferenceViewId;
      backgroundLow = x->backgroundLow;
      backgroundHigh = x->backgroundHigh;
      backgroundUseROI = x->backgroundUseROI;
      backgroundROI = x->backgroundROI;
      outputWhiteReferenceMask = x->outputWhiteReferenceMask;
      outputBackgroundReferenceMask = x->outputBackgroundReferenceMask;
   }
}

// ----------------------------------------------------------------------------

UndoFlags ColorCalibrationInstance::UndoMode( const View& ) const
{
   return UndoFlag::PixelData;
}

// ----------------------------------------------------------------------------

bool ColorCalibrationInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "ColorCalibration cannot be executed on complex images.";
      return false;
   }

   if ( !view.IsColor() )
   {
      whyNot = "ColorCalibration can only be executed on color images.";
      return false;
   }

   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#define SD_ITERATIONS   4
#define SD_COST         (SD_ITERATIONS*(4 + 2*structureLayers) + 1)

static const float __3x3Linear_hv[] = { 0.50F, 1.00F, 0.50F };

static void GetStructureMap( Image& map, int structureLayers, int noiseLayers )
{
   ATrousWaveletTransform::WaveletScalingFunction H;
   H.Set( SeparableFilter( __3x3Linear_hv, __3x3Linear_hv, 3 ) );

   ATrousWaveletTransform W( H );
   W.SetNumberOfLayers( structureLayers );
   for ( int j = noiseLayers; j < structureLayers; ++j )
      W.DisableLayer( j );

   for ( int i = 0; i < SD_ITERATIONS; ++i )
   {
      W << map;                  // structureLayers*N
      Image tmp;
      tmp.Status() = map.Status();
      W >> tmp;                  // structureLayers*N
      tmp.Truncate();            // N
      map.Status() = tmp.Status();
      map -= tmp;                // N
      map.Truncate();            // N
      map.Rescale();             // N
   }

   map.Binarize( 0.0005 );       // N
}

static Image GetIntensity( const Image& image )
{
   StatusMonitor status = image.Status();
   image.Status() = StatusMonitor();
   Image I;
   image.GetIntensity( I );
   image.Status() = I.Status() = status;
   return I;
}

static void GetStructures( Image& image, int structureLayers, int noiseLayers )
{
   Image map = GetIntensity( image );

   bool initializeStatus = image.Status().IsInitializationEnabled();
   if ( initializeStatus )
   {
      map.Status() = image.Status();
      map.Status().Initialize( "Extracting structures", (map.NumberOfPixels()*SD_COST + image.NumberOfNominalChannels()) );
      map.Status().DisableInitialization();
   }

   GetStructureMap( map, structureLayers, noiseLayers );

   image.Status() = map.Status();
   for ( int c = 0; c < image.NumberOfNominalChannels(); ++c )
   {
      image.SelectChannel( c );
      image.Mul( map );             // N
   }
   image.ResetSelections();

   if ( initializeStatus )
      image.Status().EnableInitialization();
}

// ----------------------------------------------------------------------------

template <class P>
static void Calibrate( GenericImage<P>& image, const DVector& kw, const DVector& bg )
{
   for ( int c = 0; c < 3; ++c )
   {
      double bgc = bg[c];
      double kwc = kw[c];
      for ( typename GenericImage<P>::sample_iterator i( image, c ); i; ++i )
         *i = (*i - bgc)*kwc + bgc;
      image.Status() += image.NumberOfPixels();
   }

   image.Normalize();
}

static void Calibrate( ImageVariant& image, const DVector& kw, const DVector& bg )
{
   // The calibration image is always a floating point image
   switch ( image.BitsPerSample() )
   {
   case 32 : Calibrate( static_cast<Image&>( *image ), kw, bg ); break;
   case 64 : Calibrate( static_cast<DImage&>( *image ), kw, bg ); break;
   }
}

// ----------------------------------------------------------------------------

template <class P>
static DVector WhiteReferenceVector( const GenericImage<P>& image, UInt8Image* mask, float low, float high )
{
   DVector w( 0.0, 3 );

   UInt8Image::roi_sample_iterator m;
   if ( mask != 0 )
   {
      mask->Zero();
      m = UInt8Image::roi_sample_iterator( *mask, image.SelectedRectangle() );
   }

   size_type n = 0;
   for ( typename GenericImage<P>::const_roi_pixel_iterator i( image ); i; ++i, ++image.Status() )
   {
      float fR; P::FromSample( fR, i[0] );
      if ( fR > low && fR < high )
      {
         float fG; P::FromSample( fG, i[1] );
         if ( fG > low && fG < high )
         {
            float fB; P::FromSample( fB, i[2] );
            if ( fB > low && fB < high )
            {
               w[0] += fR;
               w[1] += fG;
               w[2] += fB;
               if ( m )
                  *m = 0xff;
               ++n;
            }
         }
      }

      if ( m )
         ++m;
   }

   if ( n > 0 )
      w /= n;

   return w;
}

static DVector WhiteReferenceVector( const ImageVariant& image, ImageVariant& mask, float low, float high )
{
   UInt8Image* maskImage = 0;
   if ( mask )
      maskImage = &static_cast<UInt8Image&>( *mask );

   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32 : return WhiteReferenceVector( static_cast<const Image&>( *image ), maskImage, low, high );
      case 64 : return WhiteReferenceVector( static_cast<const DImage&>( *image ), maskImage, low, high );
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8 : return WhiteReferenceVector( static_cast<const UInt8Image&>( *image ), maskImage, low, high );
      case 16 : return WhiteReferenceVector( static_cast<const UInt16Image&>( *image ), maskImage, low, high );
      case 32 : return WhiteReferenceVector( static_cast<const UInt32Image&>( *image ), maskImage, low, high );
      }

   return 0;
}

// ----------------------------------------------------------------------------

template <class P>
static DVector BackgroundReferenceVector( const GenericImage<P>& image, UInt8Image* mask, float low, float high )
{
   UInt8Image::roi_sample_iterator m;
   if ( mask != 0 )
   {
      mask->Zero();
      m = UInt8Image::roi_sample_iterator( *mask, image.SelectedRectangle() );
   }

   Array<float> bR, bG, bB;
   for ( typename GenericImage<P>::const_roi_pixel_iterator i( image ); i; ++i, ++image.Status() )
   {
      float fR; P::FromSample( fR, i[0] );
      if ( fR > low && fR < high )
      {
         float fG; P::FromSample( fG, i[1] );
         if ( fG > low && fG < high )
         {
            float fB; P::FromSample( fB, i[2] );
            if ( fB > low && fB < high )
            {
               bR.Append( fR );
               bG.Append( fG );
               bB.Append( fB );
               if ( m )
                  *m = 0xff;
            }
         }
      }

      if ( m )
         ++m;
   }

   DVector b( 0.0, 3 );
   if ( !bR.IsEmpty() )
   {
      b[0] = pcl::Median( bR.Begin(), bR.End() );
      b[1] = pcl::Median( bG.Begin(), bG.End() );
      b[2] = pcl::Median( bB.Begin(), bB.End() );
   }

   return b;
}

static DVector BackgroundReferenceVector( const ImageVariant& image, ImageVariant& mask, float low, float high )
{
   UInt8Image* maskImage = 0;
   if ( mask )
      maskImage = &static_cast<UInt8Image&>( *mask );

   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
      case 32 : return BackgroundReferenceVector( static_cast<const Image&>( *image ), maskImage, low, high );
      case 64 : return BackgroundReferenceVector( static_cast<const DImage&>( *image ), maskImage, low, high );
      }
   else
      switch ( image.BitsPerSample() )
      {
      case  8 : return BackgroundReferenceVector( static_cast<const UInt8Image&>( *image ), maskImage, low, high );
      case 16 : return BackgroundReferenceVector( static_cast<const UInt16Image&>( *image ), maskImage, low, high );
      case 32 : return BackgroundReferenceVector( static_cast<const UInt32Image&>( *image ), maskImage, low, high );
      }

   return 0;
}

// ----------------------------------------------------------------------------

static IsoString ValidFullId( const IsoString& id )
{
   IsoString validId( id );
   validId.ReplaceString( "->", "_" );
   return validId;
}

// ----------------------------------------------------------------------------

bool ColorCalibrationInstance::ExecuteOn( View& view )
{
   ImageWindow whiteMaskWindow, backgroundMaskWindow;

   try
   {
      AutoViewLock lock( view );

      if ( !view.IsColor() )
         throw Error( "ColorCalibration can only work for color images: " + view.FullId() );

      if ( whiteUseROI )
      {
         whiteROI.Order();
         if ( !whiteROI.IsRect() )
            throw Error( "Empty white reference ROI defined" );
      }

      if ( backgroundUseROI )
      {
         backgroundROI.Order();
         if ( !backgroundROI.IsRect() )
            throw Error( "Empty background reference ROI defined" );
      }

      StandardStatus status;
      Console console;

      console.EnableAbort();

      /*
       * Background Evaluation
       */
      DVector bg;
      {
         View bkgView;

         if ( backgroundReferenceViewId.IsEmpty() )
            bkgView = view;
         else
         {
            bkgView = View::ViewById( backgroundReferenceViewId );
            if ( bkgView.IsNull() )
               throw Error( "No such view (background reference): " + backgroundReferenceViewId );
            if ( !bkgView.IsColor() )
               throw Error( "The background reference view must be a color image: " + backgroundReferenceViewId );
         }

         AutoViewLock bkgLock( bkgView, false/*lock*/ );
         if ( bkgView.CanWrite() )
            bkgLock.LockForWrite();

         ImageVariant m;
         if ( outputBackgroundReferenceMask )
         {
            IsoString id = ValidFullId( bkgView.FullId() ) + "_background";
            backgroundMaskWindow = ImageWindow( bkgView.Width(), bkgView.Height(), 1/*numberOfChannels*/,
                                                8/*bitsPerSample*/, false/*floatSample*/,
                                                false/*color*/,
                                                true/*initialProcessing*/,
                                                id );
            if ( backgroundMaskWindow.IsNull() )
               throw Error( "Unable to create image window: " + id );
            m = backgroundMaskWindow.MainView().Image();
         }

         ImageVariant v = bkgView.Image();
         if ( backgroundUseROI )
         {
            v.SelectRectangle( backgroundROI );
            Rect r = v.SelectedRectangle();
            if ( !r.IsRect() )
               v.ResetSelection();
            console.WriteLn( String().Format( "<end><cbr>Background reference ROI : left=%d, top=%d, width=%d, height=%d",
                                              r.x0, r.y0, r.Width(), r.Height() ) );
         }

         v.SetStatusCallback( &status );
         v.Status().Initialize( "Evaluating background", v.NumberOfSelectedPixels() );

         bg = BackgroundReferenceVector( v, m, backgroundLow, backgroundHigh );

         console.NoteLn( String().Format( "<end><cbr>* Background reference:\n"
                                          "B_R : %.5e\n"
                                          "B_G : %.5e\n"
                                          "B_B : %.5e", bg[0], bg[1], bg[2] ) );
      }

      /*
       * White Balancing
       */
      DVector kw( 3 );
      if ( manualWhiteBalance )
      {
         kw[0] = manualRedFactor;
         kw[1] = manualGreenFactor;
         kw[2] = manualBlueFactor;
      }
      else
      {
         View whiteView;

         if ( whiteReferenceViewId.IsEmpty() )
            whiteView = view;
         else
         {
            whiteView = View::ViewById( whiteReferenceViewId );
            if ( whiteView.IsNull() )
               throw Error( "No such view (white reference): " + whiteReferenceViewId );
            if ( !whiteView.IsColor() )
               throw Error( "The white reference view must be a color image: " + whiteReferenceViewId );
         }

         AutoViewLock whiteLock( whiteView, false/*lock*/ );
         if ( whiteView.CanWrite() )
            whiteLock.LockForWrite();

         ImageVariant m;
         if ( outputWhiteReferenceMask )
         {
            IsoString id = ValidFullId( whiteView.FullId() ) + "_white";
            whiteMaskWindow = ImageWindow( whiteView.Width(), whiteView.Height(), 1/*numberOfChannels*/,
                                           8/*bitsPerSample*/, false/*floatSample*/,
                                           false/*color*/,
                                           true/*initialProcessing*/,
                                           id );
            if ( whiteMaskWindow.IsNull() )
               throw Error( "Unable to create image window: " + id );
            m = whiteMaskWindow.MainView().Image();
         }

         ImageVariant v;
         if ( structureDetection )
         {
            v.CreateFloatImage();
            v.CopyImage( whiteView.Image() );
            v.SetStatusCallback( &status );
            GetStructures( static_cast<Image&>( *v ), structureLayers, noiseLayers );
         }
         else
            v = whiteView.Image();

         if ( whiteUseROI )
         {
            v.SelectRectangle( whiteROI );
            Rect r = v.SelectedRectangle();
            if ( !r.IsRect() )
               v.ResetSelection();
            console.WriteLn( String().Format( "<end><cbr>White reference ROI : left=%d, top=%d, width=%d, height=%d",
                                              r.x0, r.y0, r.Width(), r.Height() ) );
         }

         v.SetStatusCallback( &status );
         v.Status().Initialize( "Calculating color correction", v.NumberOfSelectedPixels() );

         DVector white = WhiteReferenceVector( v, m, whiteLow, whiteHigh );
         for ( int c = 0; c < 3; ++c )
         {
            if ( 1 + white[c] == 1 )
               throw Error( String().Format( "No significant white reference pixels for channel #%d: ", c ) + whiteView.FullId() );
            white[c] = Abs( white[c] - bg[c] );
         }

         console.NoteLn( String().Format( "<end><cbr>* White reference:\n"
                                          "W0_R : %.5e\n"
                                          "W0_G : %.5e\n"
                                          "W0_B : %.5e", white[0], white[1], white[2] ) );
         int rc = (white[0] > white[1]) ? ((white[0] > white[2]) ? 0 : 2) : ((white[1] > white[2]) ? 1 : 2);
         for ( int c = 0; c < 3; ++c )
         {
            if ( c == rc )
               kw[c] = 1;
            else
               kw[c] = white[rc]/white[c];
         }

         console.NoteLn( String().Format( "<end><cbr>* White balance factors:\n"
                                          "W_R : %.4e\n"
                                          "W_G : %.4e\n"
                                          "W_B : %.4e", kw[0], kw[1], kw[2] ) );
      }

      /*
       * Color Calibration
       */
      ImageVariant v = view.Image();

      v.SetStatusCallback( &status );
      v.Status().Initialize( "Applying color calibration", 3*2*v.NumberOfPixels() );
      v.Status().DisableInitialization();

      if ( v.IsFloatSample() )
      {
         Calibrate( v, kw, bg );
      }
      else
      {
         ImageVariant v1;
         v1.CreateFloatImage( (v.BitsPerSample() > 16) ? 64 : 32 );
         v1.CopyImage( v );
         Calibrate( v1, kw, bg );
         v.CopyImage( v1 );
      }

      if ( !whiteMaskWindow.IsNull() )
         whiteMaskWindow.Show();

      if ( !backgroundMaskWindow.IsNull() )
         backgroundMaskWindow.Show();

      return true;
   }

   catch ( ... )
   {
      if ( !whiteMaskWindow.IsNull() )
         whiteMaskWindow.Close();
      if ( !backgroundMaskWindow.IsNull() )
         backgroundMaskWindow.Close();
      throw;
   }
}

// ----------------------------------------------------------------------------

void* ColorCalibrationInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheCCWhiteReferenceViewIdParameter )
      return whiteReferenceViewId.Begin();
   if ( p == TheCCWhiteLowParameter )
      return &whiteLow;
   if ( p == TheCCWhiteHighParameter )
      return &whiteHigh;
   if ( p == TheCCWhiteUseROIParameter )
      return &whiteUseROI;
   if ( p == TheCCWhiteROIX0Parameter )
      return &whiteROI.x0;
   if ( p == TheCCWhiteROIY0Parameter )
      return &whiteROI.y0;
   if ( p == TheCCWhiteROIX1Parameter )
      return &whiteROI.x1;
   if ( p == TheCCWhiteROIY1Parameter )
      return &whiteROI.y1;
   if ( p == TheCCStructureDetectionParameter )
      return &structureDetection;
   if ( p == TheCCStructureLayersParameter )
      return &structureLayers;
   if ( p == TheCCNoiseLayersParameter )
      return &noiseLayers;
   if ( p == TheCCManualWhiteBalanceParameter )
      return &manualWhiteBalance;
   if ( p == TheCCManualRedFactorParameter )
      return &manualRedFactor;
   if ( p == TheCCManualGreenFactorParameter )
      return &manualGreenFactor;
   if ( p == TheCCManualBlueFactorParameter )
      return &manualBlueFactor;
   if ( p == TheCCBackgroundReferenceViewIdParameter )
      return backgroundReferenceViewId.Begin();
   if ( p == TheCCBackgroundLowParameter )
      return &backgroundLow;
   if ( p == TheCCBackgroundHighParameter )
      return &backgroundHigh;
   if ( p == TheCCBackgroundUseROIParameter )
      return &backgroundUseROI;
   if ( p == TheCCBackgroundROIX0Parameter )
      return &backgroundROI.x0;
   if ( p == TheCCBackgroundROIY0Parameter )
      return &backgroundROI.y0;
   if ( p == TheCCBackgroundROIX1Parameter )
      return &backgroundROI.x1;
   if ( p == TheCCBackgroundROIY1Parameter )
      return &backgroundROI.y1;
   if ( p == TheCCOutputWhiteReferenceMaskParameter )
      return &outputWhiteReferenceMask;
   if ( p == TheCCOutputBackgroundReferenceMaskParameter )
      return &outputBackgroundReferenceMask;
   return 0;
}

bool ColorCalibrationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheCCWhiteReferenceViewIdParameter )
   {
      whiteReferenceViewId.Clear();
      if ( sizeOrLength > 0 )
         whiteReferenceViewId.SetLength( sizeOrLength );
   }
   else if ( p == TheCCBackgroundReferenceViewIdParameter )
   {
      backgroundReferenceViewId.Clear();
      if ( sizeOrLength > 0 )
         backgroundReferenceViewId.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

size_type ColorCalibrationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheCCWhiteReferenceViewIdParameter )
      return whiteReferenceViewId.Length();
   if ( p == TheCCBackgroundReferenceViewIdParameter )
      return backgroundReferenceViewId.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ColorCalibrationInstance.cpp - Released 2019-01-21T12:06:41Z
