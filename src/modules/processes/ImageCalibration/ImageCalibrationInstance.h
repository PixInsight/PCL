//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard ImageCalibration Process Module Version 01.04.01.0345
// ----------------------------------------------------------------------------
// ImageCalibrationInstance.h - Released 2018-11-01T11:07:21Z
// ----------------------------------------------------------------------------
// This file is part of the standard ImageCalibration PixInsight module.
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

#ifndef __ImageCalibrationInstance_h
#define __ImageCalibrationInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/Vector.h>
#include <pcl/Matrix.h>

#include "ImageCalibrationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class FileFormatInstance;

class CalibrationThread;
struct CalibrationThreadData;

typedef IndirectArray<CalibrationThread>  thread_list;

class ImageCalibrationInstance : public ProcessImplementation
{
public:

   ImageCalibrationInstance( const MetaProcess* );
   ImageCalibrationInstance( const ImageCalibrationInstance& );

   virtual void Assign( const ProcessImplementation& );
   virtual bool CanExecuteOn( const View&, String& whyNot ) const;
   virtual bool IsHistoryUpdater( const View& v ) const;
   virtual bool CanExecuteGlobal( String& whyNot ) const;
   virtual bool ExecuteGlobal();
   virtual void* LockParameter( const MetaParameter*, size_type /*tableRow*/ );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

   struct OverscanRegions
   {
      pcl_bool enabled;    // whether to apply this overscan correction
      Rect     sourceRect; // source overscan region
      Rect     targetRect; // image region to be corrected

      OverscanRegions( const Rect& rs = Rect( 0 ), const Rect& rt = Rect( 0 ), bool enable = false ) :
      enabled( enable ), sourceRect( rs ), targetRect( rt )
      {
      }

      OverscanRegions( const OverscanRegions& x ) :
      enabled( x.enabled ), sourceRect( x.sourceRect ), targetRect( x.targetRect )
      {
      }

      bool IsValid() const
      {
         if ( !enabled )
            return true;
         if ( !sourceRect.IsNormal() || !targetRect.IsNormal() )
            return false;
         if ( sourceRect.x0 < 0 || sourceRect.y0 < 0 ||
              targetRect.x0 < 0 || targetRect.y0 < 0 )
            return false;
         return true;
      }
   };

   struct Overscan
   {
      pcl_bool        enabled;       // whether overscan correction is globally enabled
      OverscanRegions overscan[ 4 ]; // four overscan source and target regions
      Rect            imageRect;     // image region (i.e. the cropping rectangle)

      Overscan() : enabled( false ), imageRect( 0 )
      {
      }

      Overscan( const Overscan& x ) : enabled( x.enabled ), imageRect( x.imageRect )
      {
         for ( int i = 0; i < 4; ++i )
            overscan[i] = x.overscan[i];
      }

      bool IsValid() const
      {
         if ( !enabled )
            return true;
         for ( int i = 0; i < 4; ++i )
            if ( !overscan[i].IsValid() )
               return false;
         if ( !imageRect.IsNormal() )
            return false;
         if ( imageRect.x0 < 0 || imageRect.y0 < 0 )
            return false;
         return true;
      }

      bool HasOverscanRegions() const
      {
         for ( int i = 0; i < 4; ++i )
            if ( overscan[i].enabled )
               return true;
         return false;
      }
   };

   typedef Array<OverscanRegions> overscan_list;
   typedef Array<overscan_list>   overscan_table;

private:

   struct ImageItem
   {
      pcl_bool enabled; // if disabled, skip (ignore) this image
      String   path;    // absolute file path

      ImageItem( const String& p = String() ) : enabled( true ), path( p )
      {
      }

      ImageItem( const ImageItem& x ) : enabled( x.enabled ), path( x.path )
      {
      }

      bool IsValid() const
      {
         return !enabled || !path.IsEmpty();
      }
   };

   typedef Array<ImageItem>  image_list;

   // The set of target frames to be calibrated
   image_list      targetFrames;

   // Format hints
   String          inputHints;      // e.g.: "raw cfa"
   String          outputHints;     // e.g.: "bottom-up"

   // Subtractive pedestal
   int32           pedestal;        // in 16-bit DN
   pcl_enum        pedestalMode;    // literal | default keyword | custom keyword
   String          pedestalKeyword;

   // Overscan source and target regions
   Overscan        overscan;

   // Master bias, dark and flat frames
   ImageItem       masterBias;
   ImageItem       masterDark;
   ImageItem       masterFlat;

   // Calibration options
   pcl_bool        calibrateBias;   // apply master bias frame calibration (overscan)
   pcl_bool        calibrateDark;   // apply master dark frame calibration (overscan+bias)
   pcl_bool        calibrateFlat;   // apply master flat frame calibration (overscan+bias+dark)

   // Dark frame optimization
   pcl_bool        optimizeDarks;   // perform dark frame optimizations
   float           darkOptimizationThreshold;   // ### DEPRECATED
   float           darkOptimizationLow;    // lower bound for dark frame optimization in sigma units
   int32           darkOptimizationWindow; // size in px of the central region for dark optimization
   pcl_enum        darkCFADetectionMode;   // detect/force/ignore CFA in the master dark image

   // Noise estimates
   pcl_bool        evaluateNoise;   // perform MRS noise evaluation
   pcl_enum        noiseEvaluationAlgorithm;

   // Output files
   String          outputDirectory;
   String          outputExtension;
   String          outputPrefix;
   String          outputPostfix;
   pcl_enum        outputSampleFormat;
   int32           outputPedestal;  // additive in 16-bit DN to ensure positivity
   pcl_bool        overwriteExistingFiles;
   pcl_enum        onError;
   pcl_bool        noGUIMessages;

   // Read-only output properties
   struct OutputData
   {
      String     outputFilePath;
      FVector    darkScalingFactors = FVector( 0.0F, 3 );
      FVector    noiseEstimates = FVector( 0.0F, 3 );
      FVector    noiseFractions = FVector( 0.0F, 3 );
      StringList noiseAlgorithms = StringList( size_type( 3 ) );
   };
   Array<OutputData> output;

   // -------------------------------------------------------------------------

   // Working geometry validation
   Rect geometry,             // uncalibrated, including overscan regions
        calibratedGeometry;   // calibrated, with overscan regions trimmed
   void ValidateImageGeometry( const Image*, bool uncalibrated = true );

   // Group source overscan regions with identical target regions
   overscan_table BuildOverscanTable() const;

   // Subtract input pedestal if appropriate, depending on instance parameters
   // and file keywords.
   void SubtractPedestal( Image*, FileFormatInstance& );

   // Read a master calibration frame
   Image* LoadCalibrationFrame( const String& filePath, bool willCalibrate, bool* hasCFA = 0 );

   // Read a target frame file
   thread_list LoadTargetFrame( const String& filePath, const CalibrationThreadData& );

   // Write a calibrated image file
   void WriteCalibratedImage( const CalibrationThread* );

   friend class CalibrationThread;
   friend class ImageCalibrationInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ImageCalibrationInstance_h

// ----------------------------------------------------------------------------
// EOF ImageCalibrationInstance.h - Released 2018-11-01T11:07:21Z
