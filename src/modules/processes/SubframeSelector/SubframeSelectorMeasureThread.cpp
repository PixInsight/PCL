//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0873
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.00.02.0261
// ----------------------------------------------------------------------------
// SubframeSelectorMeasureThread.cpp - Released 2017-08-01T14:26:58Z
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

#include "SubframeSelectorMeasureThread.h"
#include "SubframeSelectorInterface.h"
#include "SubframeSelectorParameters.h"
#include "SubframeSelectorStarDetector.h"

#include <pcl/Console.h>
#include <pcl/MetaModule.h>
#include <pcl/ErrorHandler.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SubframeSelectorMeasureThread::SubframeSelectorMeasureThread( ImageWindow& subframe, MeasureData& outputData, const String& subframePath,
                                                              const MeasureThreadInputData& data ) :
   m_subframe( subframe ),
   m_outputData( outputData ),
   m_subframePath( subframePath ),
   m_success( false ),
   m_data( data )
{
}

void SubframeSelectorMeasureThread::Run()
{
   try
   {
      m_success = false;

      Console().NoteLn( "<br><br>Measurements for: " + m_subframePath );

      Console().WriteLn( "Accessing Main View" );
      View subframeView = m_subframe.MainView();
      ImageVariant subframe = subframeView.Image();

      if ( subframe.ColorSpace() != ColorSpace::Gray )
      {
         Console().WriteLn( "Converting Main View to Grayscale" );
         subframe.SetColorSpace( ColorSpace::Gray );
      }

      Console().NoteLn( "Running StarDetector" );
      SubframeSelectorStarDetector starDetector;
      starDetector.showStarDetectionMaps                 = m_data.showStarDetectionMaps;
      starDetector.structureLayers                       = m_data.structureLayers;
      starDetector.noiseLayers                           = m_data.noiseLayers;
      starDetector.hotPixelFilterRadius                  = m_data.hotPixelFilterRadius;
      starDetector.noiseReductionFilterRadius            = m_data.noiseReductionFilterRadius;
      starDetector.applyHotPixelFilterToDetectionImage   = m_data.applyHotPixelFilterToDetectionImage;
      starDetector.sensitivity                           = m_data.sensitivity;
      starDetector.peakResponse                          = m_data.peakResponse;
      starDetector.maxDistortion                         = m_data.maxDistortion;
      starDetector.upperLimit                            = m_data.upperLimit;
      starDetector.backgroundExpansion                   = m_data.backgroundExpansion;
      starDetector.xyStretch                             = m_data.xyStretch;
      Array<Star> stars = starDetector.GetStars( subframe );

      Console().NoteLn( String().Format( "Found stars: %i", stars.Length() ) );

//      IsoString key;
//      IsoString value;
//
//      Console().WriteLn( "Looking for process" );
//      Process starAlignmentProcess( IsoString( "StarAlignment" ) );
//      Console().WriteLn( "Creating instance" );
//      ProcessInstance starAlignment( starAlignmentProcess );
//      if ( starAlignment.IsNull() )
//         throw Error( "Couldn't instantiate the StarAlignment process: null" );

//      Console().WriteLn( "Looking for mode" );
//      ProcessParameter mode( starAlignmentProcess, "mode" );
//      if ( mode.IsEnumeration() )
//      {
//         Console().WriteLn( "Checking mode default" );
//         mode.DefaultValue();
//         Console().WriteLn( "Checking mode options" );
//         ProcessParameter::enumeration_element_list enums = mode.EnumerationElements();
//         Console().WriteLn( "Printing mode options" );
//         for ( int i = 0; i < enums.Length(); ++i ) {
//            ProcessParameter::EnumerationElement modeEnum = enums[i];
//            Console().WriteLn( String().Format( "Mode Id = '%s' Value = '%i'", modeEnum.id, modeEnum.value ) );
//         }
//      }

//      Console().WriteLn( "Setting mode" );
//      starAlignment.SetParameterValue( 5, IsoString( "mode" ), 0 );
//
//      starAlignment.SetParameterValue( subframeView.FullId(), IsoString( "referenceImage" ), 0);
//      starAlignment.SetParameterValue( false, IsoString( "referenceIsFile" ), 0);
//
//      Console().WriteLn( "Checking can execute" );
//      String whyNot;
//      if ( !starAlignment.CanExecuteOn( subframeView, whyNot ) )
//         throw Error( "Cannot execute StarAlignment instance on view <br/>"
//                 "Reason: " + whyNot );
//
//      Console().WriteLn( "Executing" );
//      if ( !starAlignment.ExecuteOn( subframeView ) )
//         throw CaughtException();

      m_subframe.Close();

      m_success = true;
   }
   catch ( ... )
   {
      try
      {
         throw;
      }
      ERROR_HANDLER
   }
}

const MeasureThreadInputData& SubframeSelectorMeasureThread::MeasuringData() const
{
   return m_data;
}

const ImageWindow* SubframeSelectorMeasureThread::SubframeImage() const
{
   return &m_subframe;
}

String SubframeSelectorMeasureThread::SubframePath() const
{
   return m_subframePath;
}

const MeasureData& SubframeSelectorMeasureThread::OutputData() const
{
   return m_outputData;
}

int SubframeSelectorMeasureThread::SubimageIndex() const
{
   return m_subimageIndex;
}

bool SubframeSelectorMeasureThread::Success() const
{
   return m_success;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SubframeSelectorMeasureThread.cpp - Released 2017-08-01T14:26:58Z
