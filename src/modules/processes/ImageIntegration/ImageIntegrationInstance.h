//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.14.00.0390
// ----------------------------------------------------------------------------
// ImageIntegrationInstance.h - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageIntegration PixInsight module.
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
   pcl_bool    p_generateDrizzleData;     // append rejection and weight data to existing .drz files
   pcl_bool    p_closePreviousImages;     // close existing integration and map images before running

   int32       p_bufferSizeMB;  // size of a row buffer in megabytes
   int32       p_stackSizeMB;   // size of the pixel integration stack in megabytes

   pcl_bool    p_useROI;        // use a region of interest; entire image otherwise
   Rect        p_roi;           // region of interest

   pcl_bool    p_useCache;      // use the dynamic file cache

   pcl_bool    p_evaluateNoise; // perform a MRS Gaussian noise estimation for the resulting image
   float       p_mrsMinDataFraction; // minimum fraction of data for a valid MRS noise evaluation

   pcl_bool    p_noGUIMessages; // only show errors on the console

   // High-level parallelism
   pcl_bool    p_useFileThreads;
   float       p_fileThreadOverload;

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
      int32      numberOfChannels          = 0; // number of nominal channels (1 or 3)
      uint64     numberOfPixels            = 0; // area of the integrated image in pixels
      uint64     totalPixels               = 0; // total integrated pixels (area*numberOfFiles)

      // Per-channel data for the final integrated image

      UI64Vector totalRejectedLow          = UI64Vector( 0, 3 ); // low rejected pixels
      UI64Vector totalRejectedHigh         = UI64Vector( 0, 3 ); // high rejected pixels
      DVector    finalNoiseEstimates       = DVector( 0, 3 );    // noise estimates for the integrated image
      DVector    finalScaleEstimates       = DVector( 0, 3 );    // scale estimates for the integrated image
      DVector    finalLocationEstimates    = DVector( 0, 3 );    // location estimates for the integrated image
      FVector    referenceNoiseReductions  = FVector( 0, 3 );    // noise reduction w.r.t. reference image
      FVector    medianNoiseReductions     = FVector( 0, 3 );    // median noise reduction
      FVector    referenceSNRIncrements    = FVector( 0, 3 );    // ### DEPRECATED - SNR increment w.r.t. the reference image
      FVector    averageSNRIncrements      = FVector( 0, 3 );    // ### DEPRECATED - average SNR increment

      // Per-channel data for each integrated image

      struct ImageData
      {
         FVector    weights      = FVector( 0, 3 );    // image weight (only if p_generateIntegratedImage)
         UI64Vector rejectedLow  = UI64Vector( 0, 3 ); // number of low rejected pixels
         UI64Vector rejectedHigh = UI64Vector( 0, 3 ); // number of high rejected pixels
      };

      Array<ImageData> imageData;
   };

   OutputData o_output;

   //

   struct RejectionDataItem
   {
      float value;                // scaled value
      float raw;                  // raw value
      int   index           : 28; // file index
      bool  rejectLow       :  1; // statistically rejected low pixel?
      bool  rejectHigh      :  1; // statistically rejected high pixel?
      bool  rejectRangeLow  :  1; // range rejected low pixel?
      bool  rejectRangeHigh :  1; // range rejected high pixel?

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
// EOF ImageIntegrationInstance.h - Released 2017-05-02T09:43:00Z
