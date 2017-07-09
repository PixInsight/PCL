//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.03.02.0297
// ----------------------------------------------------------------------------
// ColorCalibrationInterface.h - Released 2017-07-09T18:07:32Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#ifndef __ColorCalibrationInterface_h
#define __ColorCalibrationInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/Label.h>
#include <pcl/Edit.h>
#include <pcl/NumericControl.h>
#include <pcl/SectionBar.h>
#include <pcl/ToolButton.h>
//#include <pcl/Timer.h>
#include <pcl/PushButton.h>
#include <pcl/GroupBox.h>
#include <pcl/ComboBox.h>
#include <pcl/SpinBox.h>
#include <pcl/CheckBox.h>

#include "ColorCalibrationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ColorCalibrationInterface : public ProcessInterface
{
public:

   ColorCalibrationInterface();
   virtual ~ColorCalibrationInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;
   virtual void ApplyInstance() const;
   /*
   virtual void RealTimePreviewUpdated( bool active );
   */
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   /*
   virtual bool RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, int zoomLevel ) const;
   virtual bool GenerateRealTimePreview( UInt16Image&, const View&, int zoomLevel, String& info ) const;
   */

private:

   ColorCalibrationInstance instance;

   struct GUIData
   {
      GUIData( ColorCalibrationInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        WhiteReference_SectionBar;
      Control           WhiteReference_Control;
      VerticalSizer     WhiteReference_Sizer;
         HorizontalSizer   WhiteReferenceView_Sizer;
            Label             WhiteReferenceView_Label;
            Edit              WhiteReferenceView_Edit;
            ToolButton        WhiteReferenceView_ToolButton;
         NumericControl    WhiteLow_NumericControl;
         NumericControl    WhiteHigh_NumericControl;
         GroupBox          WhiteROI_GroupBox;
         VerticalSizer     WhiteROI_Sizer;
            HorizontalSizer   WhiteROIRow1_Sizer;
               Label             WhiteROIX0_Label;
               SpinBox           WhiteROIX0_SpinBox;
               Label             WhiteROIY0_Label;
               SpinBox           WhiteROIY0_SpinBox;
            HorizontalSizer   WhiteROIRow2_Sizer;
               Label             WhiteROIWidth_Label;
               SpinBox           WhiteROIWidth_SpinBox;
               Label             WhiteROIHeight_Label;
               SpinBox           WhiteROIHeight_SpinBox;
               PushButton        WhiteROISelectPreview_Button;
         GroupBox          StructureDetection_GroupBox;
         VerticalSizer     StructureDetection_Sizer;
            HorizontalSizer   StructureLayers_Sizer;
               Label             StructureLayers_Label;
               SpinBox           StructureLayers_SpinBox;
            HorizontalSizer   NoiseLayers_Sizer;
               Label             NoiseLayers_Label;
               SpinBox           NoiseLayers_SpinBox;
         GroupBox          ManualWhiteBalance_GroupBox;
         VerticalSizer     ManualWhiteBalance_Sizer;
            NumericControl    ManualRedFactor_NumericControl;
            NumericControl    ManualGreenFactor_NumericControl;
            NumericControl    ManualBlueFactor_NumericControl;
            HorizontalSizer   OutputWhiteReferenceMask_Sizer;
               CheckBox          OutputWhiteReferenceMask_CheckBox;

      SectionBar        BackgroundReference_SectionBar;
      Control           BackgroundReference_Control;
      VerticalSizer     BackgroundReference_Sizer;
         HorizontalSizer   BackgroundReferenceView_Sizer;
            Label             BackgroundReferenceView_Label;
            Edit              BackgroundReferenceView_Edit;
            ToolButton        BackgroundReferenceView_ToolButton;
         NumericControl    BackgroundLow_NumericControl;
         NumericControl    BackgroundHigh_NumericControl;
         HorizontalSizer   OutputBackgroundReferenceMask_Sizer;
            CheckBox          OutputBackgroundReferenceMask_CheckBox;
         GroupBox          BackgroundROI_GroupBox;
         VerticalSizer     BackgroundROI_Sizer;
            HorizontalSizer   BackgroundROIRow1_Sizer;
               Label             BackgroundROIX0_Label;
               SpinBox           BackgroundROIX0_SpinBox;
               Label             BackgroundROIY0_Label;
               SpinBox           BackgroundROIY0_SpinBox;
            HorizontalSizer   BackgroundROIRow2_Sizer;
               Label             BackgroundROIWidth_Label;
               SpinBox           BackgroundROIWidth_SpinBox;
               Label             BackgroundROIHeight_Label;
               SpinBox           BackgroundROIHeight_SpinBox;
               PushButton        BackgroundROISelectPreview_Button;

      /*Timer UpdateRealTime_Timer;*/
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   /*
   void UpdateRealTimePreview();
   */

   void __GetFocus( Control& sender );
   void __EditCompleted( Edit& sender );
   void __EditValueUpdated( NumericEdit& sender, double value );
   void __SpinValueUpdated( SpinBox& sender, int value );
   void __Click( Button& sender, bool checked );
   void __GroupBoxCheck( GroupBox& sender, bool checked );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );
   /*
   void __UpdateRealTime_Timer( Timer& );
   */

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ColorCalibrationInterface* TheColorCalibrationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ColorCalibrationInterface_h

// ----------------------------------------------------------------------------
// EOF ColorCalibrationInterface.h - Released 2017-07-09T18:07:32Z
