//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.11.00.0343
// ----------------------------------------------------------------------------
// ImageIntegrationInstance.h - Released 2016/11/12 12:09:51 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __ImageIntegrationInstance_h
#define __ImageIntegrationInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Vector.h>
#include <pcl/Matrix.h>

#include "ImageIntegrationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ImageIntegrationInstance : public ProcessImplementation
{
public:

   ImageIntegrationInstance( const MetaProcess* );
   ImageIntegrationInstance( const ImageIntegrationInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View&, String& whyNot ) const;
   virtual bool IsHistoryUpdater( const View& v ) const;

   virtual bool CanExecuteGlobal( String& whyNot ) const;
   virtual bool ExecuteGlobal();

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

private:

   struct ImageItem
   {
      pcl_bool enabled;
      String   path;    // image file
      String   drzPath; // drizzle data file

      ImageItem( const String& p = String() ) : enabled( true ), path( p ), drzPath()
      {
      }

      ImageItem( const ImageItem& x ) : enabled( x.enabled ), path( x.path ), drzPath( x.drzPath )
      {
      }

      bool IsDefined() const
      {
         return !path.IsEmpty();
      }
   };

   typedef Array<ImageItem>  image_list;

   image_list  p_images;

   String      p_inputHints;    // input format hints

   pcl_enum    p_combination;   // combination operation: average, median, minimum, maximum

   pcl_enum    p_normalization; // none | additive | multiplicative | additive+scaling | mult.+scaling

   pcl_enum    p_weightMode;    // don't care | exposure time | noise | signal | median | mean | keyword
   String      p_weightKeyword;
   pcl_enum    p_weightScale;   // scale estimator used for image weighting

   pcl_bool    p_ignoreNoiseKeywords; // always evaluate noise of input images; ignore NOISExx keywords

   pcl_enum    p_rejection;     // pixel rejection: None|MinMax|PClip|SigmaClip|AvgSigmaClip|CCDClip
   pcl_enum    p_rejectionNormalization;

   int32       p_minMaxLow;     // low rejected pixels for min/max
   int32       p_minMaxHigh;    // high rejected pixels for min/max

   float       p_pcClipLow;     // low percentile clipping point
   float       p_pcClipHigh;    // high percentile clipping point

   float       p_sigmaLow;      // low sigma clipping, in sigma units
   float       p_sigmaHigh;     // high sigma clipping, in sigma units

   float       p_linearFitLow;  // low linear fit tolerance, in sigma units
   float       p_linearFitHigh; // high linear fit tolerance, in sigma units

   float       p_ccdGain;       // CCD gain in e-/ADU
   float       p_ccdReadNoise;  // CCD read noise in e-
   float       p_ccdScaleNoise; // CCD scale noise (or sensitivity noise), dimensionless

   pcl_bool    p_clipLow;       // clip low pixels
   pcl_bool    p_clipHigh;      // clip high pixels

   pcl_bool    p_rangeClipLow;  // perform low range clipping?
   float       p_rangeLow;      // low range clipping point
   pcl_bool    p_rangeClipHigh; // perform high range clipping?
   float       p_rangeHigh;     // high range clipping point
   pcl_bool    p_reportRangeRejection; // count range rejected pixels in rejection summaries
   pcl_bool    p_mapRangeRejection;    // include range rejected pixels in rejection maps

   pcl_bool    p_generate64BitResult;     // generate a 64-bit floating point result instead of 32-bit
   pcl_bool    p_generateRejectionMaps;   // generate pixel rejection map images
   pcl_bool    p_generateIntegratedImage; // generate integrated image, or only rejection maps
   pcl_bool    p_generateDrizzleData;   // append rejection and weight data to existing .drz files
   pcl_bool    p_closePreviousImages;     // close existing integration and map images before running

   int32       p_bufferSizeMB;  // size of a row buffer in megabytes
   int32       p_stackSizeMB;   // size of the pixel integration stack in megabytes

   pcl_bool    p_useROI;        // use a region of interest; entire image otherwise
   Rect        p_roi;           // region of interest

   pcl_bool    p_useCache;      // use the dynamic file cache

   pcl_bool    p_evaluateNoise; // perform a MRS Gaussian noise estimation for the resulting image
   float       p_mrsMinDataFraction; // minimum fraction of data for a valid MRS noise evaluation

   pcl_bool    p_noGUIMessages; // only show errors on the console

   /*
    * Read-only output properties
    */

   struct OutputData
   {
      // General integration data

      String     integrationImageId;       // identifier of the output integrated image
      String     lowRejectionMapImageId;   // identifier of the output low rejection map image
      String     highRejectionMapImageId;  // identifier of the output high rejection map image
      String     slopeMapImageId;          // identifier of the output slope map image
      int32      numberOfChannels;         // number of nominal channels (1 or 3)
      uint64     numberOfPixels;           // area of the integrated image in pixels
      uint64     totalPixels;              // total integrated pixels (area*numberOfFiles)

      // Per-channel data for the final integrated image

      UI64Vector totalRejectedLow;         // low rejected pixels
      UI64Vector totalRejectedHigh;        // high rejected pixels
      DVector    finalNoiseEstimates;      // noise estimates for the integrated image
      DVector    finalScaleEstimates;      // scale estimates for the integrated image
      DVector    finalLocationEstimates;   // location estimates for the integrated image
      FVector    referenceNoiseReductions; // noise reduction w.r.t. reference image
      FVector    medianNoiseReductions;    // median noise reduction
      FVector    referenceSNRIncrements;   // ### DEPRECATED - SNR increment w.r.t. the reference image
      FVector    averageSNRIncrements;     // ### DEPRECATED - average SNR increment

      // Per-channel data for each integrated image

      struct ImageData
      {
         FVector    weights;               // image weight (only if p_generateIntegratedImage)
         UI64Vector rejectedLow;           // number of low rejected pixels
         UI64Vector rejectedHigh;          // number of high rejected pixels

         ImageData() : weights( 0, 3 ), rejectedLow( 0, 3 ), rejectedHigh( 0, 3 )
         {
         }

         ImageData( const ImageData& x )
         {
            (void)operator =( x );
         }

         ImageData& operator =( const ImageData& x )
         {
            weights = x.weights;
            rejectedLow = x.rejectedLow;
            rejectedHigh = x.rejectedHigh;
            return *this;
         }
      };

      Array<ImageData> imageData;

      OutputData() :
         integrationImageId(),
         lowRejectionMapImageId(),
         highRejectionMapImageId(),
         slopeMapImageId(),
         numberOfChannels( 0 ),
         numberOfPixels( 0 ),
         totalPixels( 0 ),
         totalRejectedLow( 0, 3 ),
         totalRejectedHigh( 0, 3 ),
         finalNoiseEstimates( 0, 3 ),
         finalScaleEstimates( 0, 3 ),
         finalLocationEstimates( 0, 3 ),
         referenceNoiseReductions( 0, 3 ),
         medianNoiseReductions( 0, 3 ),
         referenceSNRIncrements( 0, 3 ),
         averageSNRIncrements( 0, 3 ),
         imageData()
      {
      }

      OutputData( const OutputData& x )
      {
         (void)operator =( x );
      }

      OutputData& operator =( const OutputData& x )
      {
         integrationImageId       = x.integrationImageId;
         lowRejectionMapImageId   = x.lowRejectionMapImageId;
         highRejectionMapImageId  = x.highRejectionMapImageId;
         slopeMapImageId          = x.slopeMapImageId;
         numberOfChannels         = x.numberOfChannels;
         numberOfPixels           = x.numberOfPixels;
         totalPixels              = x.totalPixels;
         totalRejectedLow         = x.totalRejectedLow;
         totalRejectedHigh        = x.totalRejectedHigh;
         finalNoiseEstimates      = x.finalNoiseEstimates;
         finalScaleEstimates      = x.finalScaleEstimates;
         finalLocationEstimates   = x.finalLocationEstimates;
         referenceNoiseReductions = x.referenceNoiseReductions;
         medianNoiseReductions    = x.medianNoiseReductions;
         referenceSNRIncrements   = x.referenceSNRIncrements;
         averageSNRIncrements     = x.averageSNRIncrements;
         imageData                = x.imageData;
         return *this;
      }
   };

   OutputData o_output;

   //

   struct RejectionDataItem
   {
      float value;            // scaled value
      float raw;              // raw value
      int   index           : 28;  // file index
      bool  rejectLow       :  1;  // statistically rejected low pixel?
      bool  rejectHigh      :  1;  // statistically rejected high pixel?
      bool  rejectRangeLow  :  1;  // range rejected low pixel?
      bool  rejectRangeHigh :  1;  // range rejected high pixel?

      RejectionDataItem()
      {
      }

      RejectionDataItem( const RejectionDataItem& x ) :
         value( x.value ), raw( x.raw ), index( x.index ),
         rejectLow( x.rejectLow ), rejectHigh( x.rejectHigh ),
         rejectRangeLow( x.rejectRangeLow ), rejectRangeHigh( x.rejectRangeHigh )
      {
      }

      void Set( float v, int i )
      {
         value = raw = IsFinite( v ) ? v : .0F;
         index = i;
         rejectLow = rejectHigh = rejectRangeLow = rejectRangeHigh = false;
      }

      bool IsStatisticallyRejected() const
      {
         return rejectLow || rejectHigh;
      }

      bool IsRangeRejected() const
      {
         return rejectRangeLow || rejectRangeHigh;
      }

      bool IsRejected() const
      {
         return IsStatisticallyRejected() || IsRangeRejected();
      }

      bool operator ==( const RejectionDataItem& x ) const
      {
         return index == x.index;
      }

      bool operator <( const RejectionDataItem& x ) const
      {
         return (IsRejected() == x.IsRejected()) ? value < x.value : !IsRejected();
      }

      static bool CompareRaw( const RejectionDataItem& a, const RejectionDataItem& b )
      {
         return a.raw < b.raw;
      }
   };

   typedef GenericMatrix<RejectionDataItem>  RejectionMatrix;
   typedef GenericVector<RejectionMatrix>    RejectionStacks;
   typedef GenericVector<IVector>            RejectionCounts;
   typedef GenericVector<FVector>            RejectionSlopes;

   ImageWindow CreateImageWindow( const IsoString& id, int bitsPerSample );

   struct IntegrationDescriptionItems
   {
      String pixelCombination;
      String outputNormalization;
      String weightMode;
      String scaleEstimator;
      String rangeRejection;
      String pixelRejection;
      String rejectionNormalization;
      String rejectionClippings;
      String rejectionParameters;
      String regionOfInterest;

      IntegrationDescriptionItems( const ImageIntegrationInstance& );
   };

   void GetIntegrationDescriptionItems( IntegrationDescriptionItems& ) const;

   String IntegrationDescription() const;

   friend class FileItem;
   friend class IntegrationFile;
   friend class DataLoaderEngine;
   friend class RejectionEngine;
   friend class IntegrationEngine;
   friend class ImageIntegrationInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ImageIntegrationInstance_h

// ----------------------------------------------------------------------------
// EOF ImageIntegrationInstance.h - Released 2016/11/12 12:09:51 UTC
