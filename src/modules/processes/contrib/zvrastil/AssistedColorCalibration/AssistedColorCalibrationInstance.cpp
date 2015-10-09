//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard AssistedColorCalibration Process Module Version 01.00.00.0125
// ----------------------------------------------------------------------------
// AssistedColorCalibrationInstance.cpp - Released 2015/10/08 11:24:40 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard AssistedColorCalibration PixInsight module.
//
// Copyright (c) 2010-2015 Zbynek Vrastil
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L.
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

#include "AssistedColorCalibrationInstance.h"
#include "AssistedColorCalibrationParameters.h"

#include <pcl/AutoViewLock.h>
#include <pcl/Console.h>
#include <pcl/MessageBox.h>
#include <pcl/Mutex.h>
#include <pcl/StdStatus.h>
#include <pcl/Thread.h>
#include <pcl/View.h>

#define REFRESH_COUNT   65536

namespace pcl
{

// ----------------------------------------------------------------------------

AssistedColorCalibrationInstance::AssistedColorCalibrationInstance( const MetaProcess* m ) :
   ProcessImplementation( m ),
   redCorrectionFactor( TheACCRedCorrectionFactor->DefaultValue() ),
   greenCorrectionFactor( TheACCGreenCorrectionFactor->DefaultValue() ),
   blueCorrectionFactor( TheACCBlueCorrectionFactor->DefaultValue() ),
   backgroundReference( TheACCBackgroundReference->DefaultValue() ),
   histogramShadows( TheACCHistogramShadows->DefaultValue() ),
   histogramHighlights( TheACCHistogramHighlights->DefaultValue() ),
   histogramMidtones( TheACCHistogramMidtones->DefaultValue() ),
   saturationBoost( TheACCSaturationBoost->DefaultValue() )
{
}

// ----------------------------------------------------------------------------

AssistedColorCalibrationInstance::AssistedColorCalibrationInstance( const AssistedColorCalibrationInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInstance::Assign( const ProcessImplementation& p )
{
   const AssistedColorCalibrationInstance* x = dynamic_cast<const AssistedColorCalibrationInstance*>( &p );
   if ( x != 0 )
   {
      this->redCorrectionFactor = x->redCorrectionFactor;
      this->greenCorrectionFactor = x->greenCorrectionFactor;
      this->blueCorrectionFactor = x->blueCorrectionFactor;
      this->backgroundReference = x->backgroundReference;
      this->histogramShadows = x->histogramShadows;
      this->histogramHighlights = x->histogramHighlights;
      this->histogramMidtones = x->histogramMidtones;
      this->saturationBoost = x->saturationBoost;
   }
}

// ----------------------------------------------------------------------------

bool AssistedColorCalibrationInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "AssistedColorCalibration cannot be executed on complex images.";
      return false;
   }

   if ( !view.Image()->IsColor() )
   {
      whyNot = "AssistedColorCalibration can only be executed on color images.";
      return false;
   }

   whyNot.Clear();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

template <class P>
class AssistedColorCalibrationThread : public Thread
{
public:

   AssistedColorCalibrationThread( const AssistedColorCalibrationInstance& instance,
                                   const AbstractImage::ThreadData& data,
                                   GenericImage<P>& a_image,
                                   bool prev, double* br, double lCorr, size_type p0, size_type p1 ) :
   Thread(),
   T( instance ), m_data( data ), image( a_image ),
   isPreview(prev), start( p0 ), end( p1 ), backRef(br), luminanceCorrection(lCorr)
   {
   }

   virtual void Run()
   {
      INIT_THREAD_MONITOR()

      // get pointer to image area which we're about to process
      typename P::sample* pR = image[0] + start;
      typename P::sample* pG = image[1] + start;
      typename P::sample* pB = image[2] + start;
      typename P::sample* pN = image[0] + end;

      // get white balance coefficents
      double red, green, blue;
      T.GetWBCorrectionFactors( red, green, blue );

      // get histogram transformation coefficients
      double shadows, highlights, midtones;
      T.GetHistogramTransformation( shadows, highlights, midtones );

      // get saturation enhancement coefficient
      double saturation = T.GetSaturationBoost();

      // get image color working space
      const RGBColorSystem& rgbws = image.RGBWorkingSpace();

      for ( ;; )
      {
         // get pixel color
         RGBColorSystem::sample R, G, B, H, S, V, L;
         P::FromSample( R, *pR );
         P::FromSample( G, *pG );
         P::FromSample( B, *pB );

         // apply white balance
         R = Range( R * red, 0.0, 1.0 );
         G = Range( G * green, 0.0, 1.0 );
         B = Range( B * blue, 0.0, 1.0 );

         // if the image is preview, apply further processing
         if ( isPreview )
         {
            // remove background (background coefficients are already pre-calibrated with same white balance coefficents)
            // use precomputed luminance correction factor to keep effect of Histogram Transformation stable
            R = luminanceCorrection * ( R - backRef[0] );
            G = luminanceCorrection * ( G - backRef[1] );
            B = luminanceCorrection * ( B - backRef[2] );

            // apply histogram transformation
            R = HistogramTransformation::MTF( midtones, Range( (R - shadows) / ( highlights - shadows ), 0.0, 1.0 ) );
            G = HistogramTransformation::MTF( midtones, Range( (G - shadows) / ( highlights - shadows ), 0.0, 1.0 ) );
            B = HistogramTransformation::MTF( midtones, Range( (B - shadows) / ( highlights - shadows ), 0.0, 1.0 ) );

            // apply saturation enhancement
            if (fabs(saturation-1.0) > 1e-3)
            {
               rgbws.RGBToHSVL( H, S, V, L, R, G, B );
               rgbws.HSVLToRGB( R, G, B, H, Range( S*saturation, 0.0, 1.0 ), V, L );
            }
         }

         // set new pixel color
         *pR = P::ToSample( R );
         *pG = P::ToSample( G );
         *pB = P::ToSample( B );

         // go to next pixel
         if ( ++pR == pN )
            break;
         ++pG;
         ++pB;

         UPDATE_THREAD_MONITOR( 65536 )
      }
   }

private:

   const AssistedColorCalibrationInstance& T;
   const AbstractImage::ThreadData& m_data;
   GenericImage<P>& image;
   bool isPreview;
   size_type start, end;
   double* backRef;
   double luminanceCorrection;
};

template <class P>
static void ApplyAssistedColorCalibration( GenericImage<P>& image, const AssistedColorCalibrationInstance& T, bool isPreview, double* originalMedian )
{
   // get background reference
   double backRef[3];
   for (int i = 0; i < 3; i++)
      backRef[i] = 0.0;

   double luminanceCorrection = 1.0;

   if ( isPreview )
   {
      // calculate background reference
      T.CalculateBackgroundReference( backRef[0], backRef[1], backRef[2] );

      // apply current white balance to background reference
      T.CorrectBackgroundReference( backRef[0], backRef[1], backRef[2] );

      // compute luminance correction factor
      // this factor express the shift of the median luminance after white balance and background subtraction
      // it will be used to maintain stable input to histogram transformation and to prevent need to update
      // histogram transformation each time white balance coefficients are changed
      double processedMedian[3];
      double wbFactors[3];
      T.GetWBCorrectionFactors( wbFactors[0], wbFactors[1], wbFactors[2] );

      // compute approximation of processed image median
      for (int i = 0; i < 3; i++)
         processedMedian[i] = originalMedian[i]*wbFactors[i] - backRef[i];

      // compute approximation of luminance for each median using HSV system
      RGBColorSystem::sample H, S;
      RGBColorSystem::sample originalV, processedV;
      RGBColorSystem::RGBToHSV( H, S, originalV, originalMedian[0], originalMedian[1], originalMedian[2] );
      RGBColorSystem::RGBToHSV( H, S, processedV, processedMedian[0], processedMedian[1], processedMedian[2] );

      // compute ratio between median luminance of processed and unprocessed image
      luminanceCorrection = originalV / processedV;
   }

   // prepare parallel processing
   size_type N = image.NumberOfPixels();

   size_type numberOfThreads = Thread::NumberOfThreads( N, 16 );
   size_type pixelsPerThread = N/numberOfThreads;

   image.Status().Initialize( "Assisted color calibration", N );

   AbstractImage::ThreadData data( image, N );

   // create processing threads
   ReferenceArray<AssistedColorCalibrationThread<P> > threads;
   for ( size_type i = 0, p = 0; i < numberOfThreads; ++i, p += pixelsPerThread )
      threads.Add( new AssistedColorCalibrationThread<P>( T, data, image,
                              isPreview, backRef, luminanceCorrection, p, Min( p + pixelsPerThread, N ) ) );

   AbstractImage::RunThreads( threads, data );
   threads.Destroy();

   image.Status() = data.status;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// calculates background reference from given view using median
void AssistedColorCalibrationInstance::CalculateBackgroundReference( double &red, double &green, double &blue ) const
{
   View view = View::ViewById( backgroundReference );
   if ( !view.IsNull() )
      if ( view.Image()->NumberOfNominalChannels() == 3 )
      {
         // get background reference as median for each channel
         DVector median = view.ComputeOrFetchProperty( "Median", false/*notify*/ ).ToDVector();
         red   = median[0];
         green = median[1];
         blue  = median[2];
         return;
      }
   red = green = blue = 0;
}

// ----------------------------------------------------------------------------

// corrects background reference with current white balance coefficients
void AssistedColorCalibrationInstance::CorrectBackgroundReference( double &red, double &green, double &blue ) const
{
   // multiply background reference with coefficients
   red *= redCorrectionFactor;
   green *= greenCorrectionFactor;
   blue *= blueCorrectionFactor;

   // get smallest value
   double backMin = Min( red, Min( green, blue ) );

   // subtract it from reference to minimize clipping
   red -= backMin;
   green -= backMin;
   blue -= backMin;
}

// ----------------------------------------------------------------------------

bool AssistedColorCalibrationInstance::ExecuteOn( View& view )
{
   // check if background reference is color image
   if ( !backgroundReference.IsEmpty() )
   {
      View backRefView = View::ViewById( backgroundReference );

      if ( backRefView.Image().IsComplexSample() )
      {
         MessageBox mb("Background Reference cannot be complex image.",
            "Not executed",
            StdIcon::Information);
         mb.Execute();
         return false;
      }

      if ( !backRefView.Image()->IsColor() )
      {
         MessageBox mb("Background Reference must be a color image.",
            "Not executed",
            StdIcon::Information);
         mb.Execute();
         return false;
      }
   }

   AutoViewLock lock( view );

   // processing is only applied to previews
   bool isPreview = view.IsPreview();

   // compute median of original image. this median will be used in preview
   // processing for normalization of luminance before applying histogram transformation
   // the goal is to prevent need to update histogram transformation parameters every time
   // white balance coefficients are changed
   double originalMedian[3] = { 0.0, 0.0, 0.0 };
   if ( isPreview )
   {
      DVector median = view.ComputeOrFetchProperty( "Median", false/*notify*/ ).ToDVector();
      originalMedian[0] = median[0];
      originalMedian[1] = median[1];
      originalMedian[2] = median[2];
   }

   ImageVariant image = view.Image();

   Console().EnableAbort();

   StandardStatus status;
   image->SetStatusCallback( &status );

   image->ResetSelections();

   if ( !image.IsComplexSample() )
      if ( image.IsFloatSample() )
         switch ( image.BitsPerSample() )
         {
         case 32:
            ApplyAssistedColorCalibration( static_cast<pcl::Image&>( *image ), *this, isPreview, originalMedian );
            break;
         case 64:
            ApplyAssistedColorCalibration( static_cast<pcl::DImage&>( *image ), *this, isPreview, originalMedian );
            break;
         }
      else
         switch ( image.BitsPerSample() )
         {
         case  8:
            ApplyAssistedColorCalibration( static_cast<pcl::UInt8Image&>( *image ), *this, isPreview, originalMedian );
            break;
         case 16:
            ApplyAssistedColorCalibration( static_cast<pcl::UInt16Image&>( *image ), *this, isPreview, originalMedian );
            break;
         case 32:
            ApplyAssistedColorCalibration( static_cast<pcl::UInt32Image&>( *image ), *this, isPreview, originalMedian );
            break;
         }

   return true;
}

// ----------------------------------------------------------------------------

void* AssistedColorCalibrationInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheACCRedCorrectionFactor )
      return &redCorrectionFactor;
   if ( p == TheACCGreenCorrectionFactor )
      return &greenCorrectionFactor;
   if ( p == TheACCBlueCorrectionFactor )
      return &blueCorrectionFactor;
   if ( p == TheACCBackgroundReference )
      return backgroundReference.c_str();
   if ( p == TheACCHistogramShadows )
      return &histogramShadows;
   if ( p == TheACCHistogramHighlights )
      return &histogramHighlights;
   if ( p == TheACCHistogramMidtones )
      return &histogramMidtones;
   if ( p == TheACCSaturationBoost )
      return &saturationBoost;

   return 0;
}

// ----------------------------------------------------------------------------

bool AssistedColorCalibrationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheACCBackgroundReference )
   {
      backgroundReference.Clear();
      if ( sizeOrLength > 0 )
         backgroundReference.SetLength( sizeOrLength );
   }
   else
      return false;

   return true;
}

// ----------------------------------------------------------------------------

size_type AssistedColorCalibrationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheACCBackgroundReference )
      return backgroundReference.Length();
   return 0;
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInstance::GetWBCorrectionFactors( double &red, double &green, double &blue ) const
{
   red = this->redCorrectionFactor;
   green = this->greenCorrectionFactor;
   blue = this->blueCorrectionFactor;
}

// ----------------------------------------------------------------------------

const String& AssistedColorCalibrationInstance::GetBackgroundReference() const
{
   return this->backgroundReference;
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInstance::GetHistogramTransformation( double &shadows, double &highlights, double &midtones ) const
{
   shadows = this->histogramShadows;
   highlights = this->histogramHighlights;
   midtones = this->histogramMidtones;
}

// ----------------------------------------------------------------------------

double AssistedColorCalibrationInstance::GetSaturationBoost() const
{
   return this->saturationBoost;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AssistedColorCalibrationInstance.cpp - Released 2015/10/08 11:24:40 UTC
