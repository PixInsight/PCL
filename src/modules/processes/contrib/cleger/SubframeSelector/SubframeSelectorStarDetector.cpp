//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.04.01.0005
// ----------------------------------------------------------------------------
// SubframeSelectorStarDetector.cpp - Released 2018-11-01T11:07:21Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
//
// Copyright (c) 2017-2018 Cameron Leger
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

#include <pcl/ATrousWaveletTransform.h>
#include <pcl/Console.h>
#include <pcl/GaussianFilter.h>
#include <pcl/ImageWindow.h>
#include <pcl/MetaModule.h>
#include <pcl/MorphologicalTransformation.h>
#include <pcl/SeparableConvolution.h>

#include "SubframeSelectorStarDetector.h"

namespace pcl
{

// ----------------------------------------------------------------------------

/*
 * 5x5 B3-spline wavelet scaling function used by the noise estimation routine.
 *
 * Kernel filter coefficients:
 *
 *   1.0/256, 1.0/64, 3.0/128, 1.0/64, 1.0/256,
 *   1.0/64,  1.0/16, 3.0/32,  1.0/16, 1.0/64,
 *   3.0/128, 3.0/32, 9.0/64,  3.0/32, 3.0/128,
 *   1.0/64,  1.0/16, 3.0/32,  1.0/16, 1.0/64,
 *   1.0/256, 1.0/64, 3.0/128, 1.0/64, 1.0/256
 *
 * Note that we use this scaling function as a separable filter (row and column
 * vectors) for performance reasons.
 */
// Separable filter coefficients
const float __5x5B3Spline_hv[] = { 0.0625F, 0.25F, 0.375F, 0.25F, 0.0625F };
// Gaussian noise scaling factors
const float __5x5B3Spline_kj[] =
        { 0.8907F, 0.2007F, 0.0856F, 0.0413F, 0.0205F, 0.0103F, 0.0052F, 0.0026F, 0.0013F, 0.0007F };

// ----------------------------------------------------------------------------

template <class P>
static void GetSamples( const GenericImage<P>& image, Array<double>& sampleArray )
{
   for ( typename GenericImage<P>::const_roi_sample_iterator i( image ); i; ++i )
   {
      double sample = 0; P::FromSample( sample, *i );
      sampleArray.Append( sample );
   }
}

static void GetSamples( const ImageVariant& image, Array<double>& sampleArray )
{
   if ( image.IsFloatSample() )
      switch ( image.BitsPerSample() )
      {
         case 32: return GetSamples( static_cast<const Image&>( *image ), sampleArray );
         case 64: return GetSamples( static_cast<const DImage&>( *image ), sampleArray );
      }
   else
      switch ( image.BitsPerSample() )
      {
         case  8: return GetSamples( static_cast<const UInt8Image&>( *image ), sampleArray );
         case 16: return GetSamples( static_cast<const UInt16Image&>( *image ), sampleArray );
         case 32: return GetSamples( static_cast<const UInt32Image&>( *image ), sampleArray );
      }
}

// ----------------------------------------------------------------------------

static ImageWindow CreateImageWindow( const ImageVariant& image, const String& name )
{
   ImageWindow imageWindow( image.Width(), image.Height(), 1, 32, true, false, true, name );
   ImageVariant imageFromWindow = imageWindow.MainView().Image();
   image.GetIntensity( imageFromWindow );
   imageWindow.Show();
   return imageWindow;
}

// ----------------------------------------------------------------------------

Array<Star> pcl::SubframeSelectorStarDetector::GetStars( ImageVariant* image )
{
   Array<Star> stars;

   // We work on a duplicate of the source grayscale image, or on its HSI
   // intensity component if it is a color image.
   ImageVariant workingImage;
   image->GetIntensity( workingImage );

   if ( showStarDetectionMaps )
      CreateImageWindow( workingImage, "Original" );

   // Hot pixel removal, if applied to the image where we are going to find
   // stars, not just to the image used to build the structure map.
   // When noise reduction is enabled, always remove hot pixels first, or
   // hot pixels would be promoted to "stars".
   bool alreadyFixedHotPixels = false;
   if ( hotPixelFilter || noiseReductionFilterRadius > 0 )
   {
      HotPixelFilter( workingImage );
      if ( showStarDetectionMaps )
         CreateImageWindow( workingImage, "HotPixelFilter" );
      alreadyFixedHotPixels = true;
   }

   // Optional noise reduction
   if ( noiseReductionFilterRadius > 0 ) {
      GaussianFilter gaussianFilter( (noiseReductionFilterRadius << 1)|1 );
      SeparableConvolution convolve( gaussianFilter.AsSeparableFilter() );
      convolve >> workingImage;
      if ( showStarDetectionMaps )
         CreateImageWindow( workingImage, "NoiseReductionFilter" );
   }

   // Structure map
   ImageVariant structures;
   workingImage.GetIntensity( structures );
   if ( !alreadyFixedHotPixels )
      HotPixelFilter( structures );
   StructureMap( structures );
   if ( showStarDetectionMaps )
      CreateImageWindow( structures, "StructuresMap" );

   // Use matrices instead of images for faster access
   Matrix structuresMatrix( structures );
   structures.Free();

   // Structure scanner
   for ( int y0 = 0, x1 = structuresMatrix.Cols()-1, y1 = structuresMatrix.Rows()-1; y0 < y1; ++y0 )
   {
      for ( int x0 = 0; x0 < x1; ++x0 )
      {
         // Exclude background pixels and already visited pixels
         if ( structuresMatrix[y0][x0] == 0 )
            continue;

         // Star pixel coordinates
         Array<Point> starPoints;

         // Star bounding rectangle
         Rect rect( x0, y0, x0+1, y0+1 );

         // Grow star region downward
         for ( int y = y0, x = x0, xa, xb; ; )
         {
            // Add this pixel to the current star
            starPoints.Append( Point( x, y ) );

            // Explore the left segment of this row
            for ( xa = x; xa > 0; )
            {
               if ( structuresMatrix[y][xa-1] == 0 )
                  break;
               --xa;
               starPoints.Append( Point( xa, y ) );
            }

            // Explore the right segment of this row
            for ( xb = x; xb < x1; )
            {
               if ( structuresMatrix[y][xb+1] == 0 )
                  break;
               ++xb;
               starPoints.Append( Point( xb, y ) );
            }

            // xa and xb are now the left and right boundary limits,
            // respectively, of this row in the current star.

            if ( xa < rect.x0 )  // update left boundary
               rect.x0 = xa;

            if ( xb >= rect.x1 ) // update right boundary
               rect.x1 = xb + 1; // bottom-right corner excluded (PCL-specific)

            // Prepare for next row
            ++y;

            // Decide whether we are done with this star now, or if
            // there is at least one more row that must be explored.
            bool nextRow = false;

            // Explore the next row from left to right. We'll continue
            // gathering pixels if we find at least one nonzero map pixel.
            for ( x = xa; x <= xb; ++x )
            {
               if ( structuresMatrix[y][x] != 0 ) {
                  nextRow = true;
                  break;
               }
            }

            if ( !nextRow )
               break;

            // Update bottom boundary
            rect.y1 = y + 1;  // Rect *excludes* the bottom-right corner

            // Terminate if we reach the last row of the image
            if ( y == y1 )
               break;
         }

         /*
          * If this is a reliable star, compute its barycenter coordinates
          * and add it to the star list.
          *
          * Rejection criteria:
          *
          * * Stars whose peak values are greater than the upperLimit
          *   parameter are rejected.
          *
          * * If this structure is touching a border of the image, reject
          *   it. We cannot compute an accurate position for a clipped star.
          *
          * * Too small structures are rejected. This mainly prevents
          *   inclusion of hot (or cold) pixels. This condition is enforced
          *   by the hot pixel removal and noise reduction steps performed
          *   during the structure detection phase.
          *
          * * Too large structures are rejected. This prevents inclusion of
          *   extended nonstellar objects and saturated bright stars. This
          *   is also part of the structure detection algorithm.
          *
          * * Too elongated stars are rejected. The maxDistortion parameter
          *   determines the maximum distortion allowed. A perfect square
          *   has distortion = 1. The distortion of a perfect circle is
          *   pi/4, or about 0.8.
          *
          * * We don't trust stars whose centroids are too misplaced with
          *   respect to their peak positions. This prevents detection of
          *   multiple stars, where an accurate position cannot be computed.
          *
          * * Too flat structures are rejected. The peakResponse parameter
          *   defines a peakedness threshold necessary for a structure to be
          *   idenfified as a valid star.
          */
         if ( rect.Width() > 1 && rect.Height() > 1 )
         {
            if ( rect.y0 > 0 && rect.y1 <= y1 && rect.x0 > 0 && rect.x1 <= x1 )
            {
               double diameter = pcl::Max( rect.Width(), rect.Height() );
               if ( double( starPoints.Length() )/diameter/diameter > maxDistortion )
               {
                  Star star = GetStarParameters( workingImage, rect, starPoints );
                  if ( star.peak <= upperLimit )
                  {
                     if ( star.background == 0 || (star.normalized - star.background)/star.background > sensitivity )
                     {
                        if ( workingImage( star.position.x, star.position.y, 0 ) > 0.85 * star.peak )
                        {
                           Matrix m( workingImage, rect );
                           if ( m.Median() < peakResponse * star.peak )
                              stars.Append( star );
                        }
                     }
                  }
               }
            }
         }

         // Erase this structure.
         for ( size_type i = 0; i < starPoints.Length(); ++i )
         {
            Point p = starPoints[i];
            structuresMatrix[p.y][p.x] = 0;
         }
      }
   }

   workingImage.Free();
   return stars;
}

// ----------------------------------------------------------------------------

void pcl::SubframeSelectorStarDetector::HotPixelFilter( ImageVariant& image )
{
   if ( hotPixelFilterRadius > 0 )
   {
      MorphologicalTransformation morph;
      morph.SetOperator( MedianFilter() );
      if ( hotPixelFilterRadius > 1 )
         morph.SetStructure( CircularStructure( 2*hotPixelFilterRadius + 1 ) );
      else
         morph.SetStructure( BoxStructure( 3 ) );
      morph >> image;
   }
}

// ----------------------------------------------------------------------------

void pcl::SubframeSelectorStarDetector::StructureMap( ImageVariant& image )
{
   // Noise reduction with a low-pass filter
   if ( noiseLayers > 0 )
   {
      GaussianFilter gaussianFilter( 1 + (1 << noiseLayers) );
      SeparableConvolution( gaussianFilter.AsSeparableFilter() ) >> image;
   }

   // Flatten the image with a high-pass filter
   {
      ImageVariant highPass;
      image.GetIntensity( highPass );
      GaussianFilter gaussianFilter( 1 + (1 << structureLayers) );
      SeparableConvolution( gaussianFilter.AsSeparableFilter() ) >> highPass;
      image.Apply( highPass, ImageOp::Sub );
      highPass.Free();
      image.Truncate();
      image.Rescale();
   }

   // Strengthen the smallest structures with a dilation filter
   {
      MorphologicalTransformation dilate;
      dilate.SetOperator( DilationFilter() );
      dilate.SetStructure( BoxStructure( 3 ) );
      dilate >> image;
   }

   // Adaptive binarization based on noise evaluation
   double median = image.Median();
   if ( 1 + median == 1)
   {
      // Black background - probably a synthetic star field

      bool wasRangeClipping    = image.IsRangeClippingEnabled();
      double origRangeClipLow  = image.RangeClipLow();
      double origRangeClipHigh = image.RangeClipHigh();

      image.EnableRangeClipping();
      image.SetRangeClipping( 0, 1 );

      if ( !wasRangeClipping || origRangeClipLow != 0 || origRangeClipHigh != 0 )
         median = image.Median();

      image.Binarize( median + image.MAD( median ) );

      image.EnableRangeClipping( wasRangeClipping );
      image.SetRangeClipping( origRangeClipLow, origRangeClipHigh );
   }
   else
   {
      // A "natural" image
      SeparableFilter filter( __5x5B3Spline_hv, __5x5B3Spline_hv, 5 );
      ATrousWaveletTransform atw( filter, 3 );
      atw << image;
      double noiseEstimate = atw.NoiseKSigma( 1 )/__5x5B3Spline_kj[1];
      image.Binarize( median + 3*noiseEstimate );
   }
}

// ----------------------------------------------------------------------------

Star pcl::SubframeSelectorStarDetector::GetStarParameters( ImageVariant& image, const Rect& rect, const Array<Point>& stars )
{
   Star star( Point( 0, 0 ) );

   // Calculate the mean local background as the median of background pixels
   Array<double> samples;
   Rect rectExpand = rect.InflatedBy( backgroundExpansion );
   image.SelectRectangle( Rect( rectExpand.x0, rectExpand.y0, rectExpand.x1,       rect.y0 ) );
   GetSamples( image, samples );
   image.ResetSelection();
   image.SelectRectangle( Rect( rectExpand.x0,       rect.y0,       rect.x0,       rect.y1 ) );
   GetSamples( image, samples );
   image.ResetSelection();
   image.SelectRectangle( Rect( rectExpand.x0,       rect.y1, rectExpand.x1, rectExpand.y1 ) );
   GetSamples( image, samples );
   image.ResetSelection();
   image.SelectRectangle( Rect(       rect.x1,       rect.y0, rectExpand.x1,       rect.y1 ) );
   GetSamples( image, samples );
   image.ResetSelection();
   star.background = pcl::Median( samples.Begin(), samples.End() );

   // Compute barycenter coordinates
   Matrix matrix( image, rect );
   matrix.Truncate( Range( Matrix( matrix ).Median() + xyStretch*matrix.StdDev(), 0.0, 1.0 ), 1.0 );
   matrix.Rescale();

   double sx = 0;
   double sy = 0;
   double sz = 0;

   for ( int y = rect.y0, i = 0; i < matrix.Rows(); ++y, ++i )
   {
      for ( int x = rect.x0, j = 0; j < matrix.Cols(); ++x, ++j )
      {
         double z = matrix[i][j];
         if ( z > 0 )
         {
            sx += z*x;
            sy += z*y;
            sz += z;
         }
      }
   }

   star.position.x = pcl::RoundInt( sx/sz + 0.5 );
   star.position.y = pcl::RoundInt( sy/sz + 0.5 );

   // Total flux, peak value and structure size
   for ( size_type i = 0; i < stars.Length(); ++i )
   {
      Point point = stars[i];
      double pointValue = image( point.x, point.y, 0 );
      star.flux += pointValue;
      if ( pointValue > star.peak )
         star.peak = pointValue;
   }
   star.size = stars.Length();

   // Detection level corrected for peak response
   star.normalized = star.peak - (1 - peakResponse)*star.flux/star.size;

   return star;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorStarDetector.cpp - Released 2018-11-01T11:07:21Z
