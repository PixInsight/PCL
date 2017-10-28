//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.00.02.0261
// ----------------------------------------------------------------------------
// SubframeSelectorInstance.h - Released 2017-08-01T14:26:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard SubframeSelector PixInsight module.
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

#ifndef __SubframeSelectorInstance_h
#define __SubframeSelectorInstance_h

#include "SubframeSelectorMeasureThread.h"

#include <pcl/MetaParameter.h> // pcl_bool, pcl_enum
#include <pcl/ProcessImplementation.h>

namespace pcl
{

typedef IndirectArray<SubframeSelectorMeasureThread> thread_list;

// ----------------------------------------------------------------------------

class SubframeSelectorInstance : public ProcessImplementation
{
public:

   SubframeSelectorInstance( const MetaProcess* );
   SubframeSelectorInstance( const SubframeSelectorInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View&, String& whyNot ) const;
   virtual bool IsHistoryUpdater( const View& v ) const;

   bool CanTestStarDetector( String& whyNot ) const;
   bool TestStarDetector();

   bool CanMeasure( String& whyNot ) const;
   bool Measure();

   virtual bool CanExecuteGlobal( String& whyNot ) const;
   virtual bool ExecuteGlobal();

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

private:

   struct SubframeItem
   {
      pcl_bool enabled; // if disabled, skip (ignore) this image
      String   path;    // absolute file path

      SubframeItem( const String& p = String() ) :
              enabled( true ),
              path( p )
      {
      }

      SubframeItem( const SubframeItem& x ) :
              enabled( x.enabled ),
              path( x.path )
      {
      }

      bool IsValid() const
      {
         return !enabled || !path.IsEmpty();
      }
   };

   typedef Array<SubframeItem>  subframe_list;

   typedef Array<MeasureData>  measured_list;

   // The set of subframes to measure
   subframe_list     subframes;

   // The settings for measurements and reporting
   float          subframeScale;
   float          cameraGain;
   pcl_enum       cameraResolution;
   int32          siteLocalMidnight;
   pcl_enum       scaleUnit;
   pcl_enum       dataUnit;

   // The settings for star detection
   int32          structureLayers;
   int32          noiseLayers;
   int32          hotPixelFilterRadius;
   pcl_bool       applyHotPixelFilterToDetectionImage;
   int32          noiseReductionFilterRadius;
   float          sensitivity;
   float          peakResponse;
   float          maxDistortion;
   float          upperLimit;
   int32          backgroundExpansion;
   float          xyStretch;

   // The set of measured subframes
   measured_list     measures;

   // Read a subframe file into a Thread
   thread_list CreateThreadForSubframe( const String& filePath, const MeasureThreadInputData& );

   friend class SubframeSelectorProcess;
   friend class SubframeSelectorInterface;
   friend class SubframeSelectorMeasureThread;
};

// ----------------------------------------------------------------------------


} // pcl

#endif   // __SubframeSelectorInstance_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInstance.h - Released 2017-08-01T14:26:58Z
