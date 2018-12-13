//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.04.01.0016
// ----------------------------------------------------------------------------
// SubframeSelectorInstance.h - Released 2018-12-12T09:25:25Z
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

#ifndef __SubframeSelectorInstance_h
#define __SubframeSelectorInstance_h

#include <pcl/MetaParameter.h>
#include <pcl/ProcessImplementation.h>

#include "SubframeSelectorMeasureData.h"
#include "SubframeSelectorParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

struct MeasureThreadInputData;
class SubframeSelectorMeasureThread;

typedef IndirectArray<SubframeSelectorMeasureThread> thread_list;

// ----------------------------------------------------------------------------

class SubframeSelectorInstance : public ProcessImplementation
{
public:

   SubframeSelectorInstance( const MetaProcess* );
   SubframeSelectorInstance( const SubframeSelectorInstance& );

   void Assign( const ProcessImplementation& ) override;

   bool CanExecuteOn( const View&, String& whyNot ) const override;
   bool IsHistoryUpdater( const View& v ) const override;

   bool CanExecuteGlobal( String& whyNot ) const override;
   bool ExecuteGlobal() override;

   void* LockParameter( const MetaParameter*, size_type tableRow ) override;
   bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow ) override;
   size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const override;

   void ApproveMeasurements();
   void WeightMeasurements();

private:

   struct SubframeItem
   {
      pcl_bool enabled; // if disabled, skip (ignore) this image
      String   path;    // absolute file path

      SubframeItem( const String& p = String() ) :
         enabled( TheSSSubframeEnabledParameter->DefaultValue() ),
         path( p )
      {
      }

      SubframeItem( const SubframeItem& ) = default;
   };

   typedef Array<SubframeItem>  subframe_list;

   // Types of run methods.
   pcl_enum           p_routine;

   // The set of subframes to measure.
   pcl_bool           p_fileCache;
   subframe_list      p_subframes;

   // The settings for measurements and reporting.
   float              p_subframeScale;
   float              p_cameraGain;
   pcl_enum           p_cameraResolution;
   int32              p_siteLocalMidnight;
   pcl_enum           p_scaleUnit;
   pcl_enum           p_dataUnit;

   // The settings for star detection.
   int32              p_structureLayers;
   int32              p_noiseLayers;
   int32              p_hotPixelFilterRadius;
   pcl_bool           p_hotPixelFilter;
   int32              p_noiseReductionFilterRadius;
   float              p_sensitivity;
   float              p_peakResponse;
   float              p_maxDistortion;
   float              p_upperLimit;
   int32              p_backgroundExpansion;
   float              p_xyStretch;
   pcl_enum           p_psfFit;
   pcl_bool           p_psfFitCircular;
   uint16             p_pedestal;
   Rect               p_roi;

   // Format hints
   String             p_inputHints;
   String             p_outputHints;

   // Output files.
   String             p_outputDirectory;
   String             p_outputExtension;
   String             p_outputPrefix;
   String             p_outputPostfix;
   String             p_outputKeyword;
   pcl_bool           p_overwriteExistingFiles;
   pcl_enum           p_onError;

   // The expressions.
   String             p_approvalExpression;
   String             p_weightingExpression;

   // How to view the measured subframes.
   pcl_enum           p_sortingProperty;
   pcl_enum           p_graphProperty;

   // The set of measured subframes.
   Array<MeasureItem> p_measures;

   bool CanTestStarDetector( String& whyNot ) const;
   bool TestStarDetector();

   bool CanMeasure( String& whyNot ) const;
   bool Measure();

   bool CanOutput( String& whyNot ) const;
   bool Output();

   // Read a subframe file.
   ImageVariant* LoadSubframe( const String& filePath );

   // Write output file.
   void WriteMeasuredImage( MeasureItem* measureItem );

   friend class SubframeSelectorProcess;
   friend class SubframeSelectorInterface;
   friend class SubframeSelectorExpressionsInterface;
   friend class SubframeSelectorMeasurementsInterface;
   friend class SubframeSelectorMeasureThread;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __SubframeSelectorInstance_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInstance.h - Released 2018-12-12T09:25:25Z
