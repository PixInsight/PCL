//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.15.00.0398
// ----------------------------------------------------------------------------
// ImageIntegrationInterface.h - Released 2017-05-05T08:37:32Z
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

#ifndef __ImageIntegrationInterface_h
#define __ImageIntegrationInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>

#include "ImageIntegrationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ImageIntegrationInterface : public ProcessInterface
{
public:

   ImageIntegrationInterface();
   virtual ~ImageIntegrationInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   InterfaceFeatures Features() const;

   virtual void EditPreferences();
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual void SaveSettings() const;

private:

   ImageIntegrationInstance instance;

   struct GUIData
   {
      GUIData( ImageIntegrationInterface& );

      VerticalSizer  Global_Sizer;

      SectionBar        InputImages_SectionBar;
      Control           InputImages_Control;
      HorizontalSizer   InputImages_Sizer;
         TreeBox           InputImages_TreeBox;
         VerticalSizer     InputButtons_Sizer;
            PushButton        AddFiles_PushButton;
            PushButton        AddDrizzleFiles_PushButton;
            PushButton        ClearDrizzleFiles_PushButton;
            PushButton        SetReference_PushButton;
            PushButton        SelectAll_PushButton;
            PushButton        InvertSelection_PushButton;
            PushButton        ToggleSelected_PushButton;
            PushButton        RemoveSelected_PushButton;
            PushButton        Clear_PushButton;
            CheckBox          StaticDrizzleTargets_CheckBox;
            CheckBox          FullPaths_CheckBox;

      SectionBar        FormatHints_SectionBar;
      Control           FormatHints_Control;
      VerticalSizer     FormatHints_Sizer;
         HorizontalSizer   InputHints_Sizer;
            Label             InputHints_Label;
            Edit              InputHints_Edit;

      SectionBar        Integration_SectionBar;
      Control           Integration_Control;
      VerticalSizer     Integration_Sizer;
         HorizontalSizer   Combination_Sizer;
            Label             Combination_Label;
            ComboBox          Combination_ComboBox;
         HorizontalSizer   Normalization_Sizer;
            Label             Normalization_Label;
            ComboBox          Normalization_ComboBox;
         HorizontalSizer   WeightMode_Sizer;
            Label             WeightMode_Label;
            ComboBox          WeightMode_ComboBox;
         HorizontalSizer   WeightKeyword_Sizer;
            Label             WeightKeyword_Label;
            Edit              WeightKeyword_Edit;
         HorizontalSizer   WeightScale_Sizer;
            Label             WeightScale_Label;
            ComboBox          WeightScale_ComboBox;
         HorizontalSizer   IgnoreNoiseKeywords_Sizer;
            CheckBox          IgnoreNoiseKeywords_CheckBox;
         HorizontalSizer   GenerateIntegratedImage_Sizer;
            CheckBox          GenerateIntegratedImage_CheckBox;
         HorizontalSizer   Generate64BitResult_Sizer;
            CheckBox          Generate64BitResult_CheckBox;
         HorizontalSizer   GenerateDrizzleData_Sizer;
            CheckBox          GenerateDrizzleData_CheckBox;
         HorizontalSizer   EvaluateNoise_Sizer;
            CheckBox          EvaluateNoise_CheckBox;
         HorizontalSizer   ClosePreviousImages_Sizer;
            CheckBox          ClosePreviousImages_CheckBox;
         HorizontalSizer   BufferSize_Sizer;
            Label             BufferSize_Label;
            SpinBox           BufferSize_SpinBox;
         HorizontalSizer   StackSize_Sizer;
            Label             StackSize_Label;
            SpinBox           StackSize_SpinBox;
         HorizontalSizer   Cache_Sizer;
            CheckBox          UseCache_CheckBox;

      SectionBar        Rejection1_SectionBar;
      Control           Rejection1_Control;
      VerticalSizer     Rejection1_Sizer;
         HorizontalSizer   RejectionAlgorithm_Sizer;
            Label             RejectionAlgorithm_Label;
            ComboBox          RejectionAlgorithm_ComboBox;
         HorizontalSizer   RejectionNormalization_Sizer;
            Label             RejectionNormalization_Label;
            ComboBox          RejectionNormalization_ComboBox;
         HorizontalSizer   GenerateRejectionMaps_Sizer;
            CheckBox          GenerateRejectionMaps_CheckBox;
         HorizontalSizer   ClipLow_Sizer;
            CheckBox          ClipLow_CheckBox;
         HorizontalSizer   ClipHigh_Sizer;
            CheckBox          ClipHigh_CheckBox;
         HorizontalSizer   ClipLowRange_Sizer;
            CheckBox          ClipLowRange_CheckBox;
         HorizontalSizer   ClipHighRange_Sizer;
            CheckBox          ClipHighRange_CheckBox;
         HorizontalSizer   ReportRangeRejection_Sizer;
            CheckBox          ReportRangeRejection_CheckBox;
         HorizontalSizer   MapRangeRejection_Sizer;
            CheckBox          MapRangeRejection_CheckBox;

      SectionBar        Rejection2_SectionBar;
      Control           Rejection2_Control;
      VerticalSizer     Rejection2_Sizer;
         HorizontalSizer   MinMaxLow_Sizer;
            Label             MinMaxLow_Label;
            SpinBox           MinMaxLow_SpinBox;
         HorizontalSizer   MinMaxHigh_Sizer;
            Label             MinMaxHigh_Label;
            SpinBox           MinMaxHigh_SpinBox;
         NumericControl    PercentileLow_NumericControl;
         NumericControl    PercentileHigh_NumericControl;
         NumericControl    SigmaLow_NumericControl;
         NumericControl    SigmaHigh_NumericControl;
         NumericControl    LinearFitLow_NumericControl;
         NumericControl    LinearFitHigh_NumericControl;
         NumericControl    RangeLow_NumericControl;
         NumericControl    RangeHigh_NumericControl;

      SectionBar        Rejection3_SectionBar;
      Control           Rejection3_Control;
      VerticalSizer     Rejection3_Sizer;
         NumericControl    CCDGain_NumericControl;
         NumericControl    CCDReadNoise_NumericControl;
         NumericControl    CCDScaleNoise_NumericControl;

      SectionBar        LargeScaleRejection_SectionBar;
      Control           LargeScaleRejection_Control;
      VerticalSizer     LargeScaleRejection_Sizer;
         HorizontalSizer   RejectLargeScaleLow_Sizer;
            CheckBox          RejectLargeScaleLow_CheckBox;
         HorizontalSizer   SmallScaleLayersLow_Sizer;
            Label             SmallScaleLayersLow_Label;
            SpinBox           SmallScaleLayersLow_SpinBox;
         HorizontalSizer   GrowthLow_Sizer;
            Label             GrowthLow_Label;
            SpinBox           GrowthLow_SpinBox;
         HorizontalSizer   RejectLargeScaleHigh_Sizer;
            CheckBox          RejectLargeScaleHigh_CheckBox;
         HorizontalSizer   SmallScaleLayersHigh_Sizer;
            Label             SmallScaleLayersHigh_Label;
            SpinBox           SmallScaleLayersHigh_SpinBox;
         HorizontalSizer   GrowthHigh_Sizer;
            Label             GrowthHigh_Label;
            SpinBox           GrowthHigh_SpinBox;

      SectionBar        ROI_SectionBar;
      Control           ROI_Control;
      VerticalSizer     ROI_Sizer;
         HorizontalSizer   ROIX0_Sizer;
            Label             ROIX0_Label;
            SpinBox           ROIX0_SpinBox;
         HorizontalSizer   ROIY0_Sizer;
            Label             ROIY0_Label;
            SpinBox           ROIY0_SpinBox;
         HorizontalSizer   ROIWidth_Sizer;
            Label             ROIWidth_Label;
            SpinBox           ROIWidth_SpinBox;
         HorizontalSizer   ROIHeight_Sizer;
            Label             ROIHeight_Label;
            SpinBox           ROIHeight_SpinBox;
            PushButton        SelectPreview_Button;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateInputImagesItem( size_type );
   void UpdateInputImagesList();
   void UpdateImageSelectionButtons();
   void UpdateFormatHintsControls();
   void UpdateIntegrationControls();
   void UpdateRejectionControls();
   void UpdateROIControls();

   void __InputImages_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void __InputImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void __InputImages_NodeSelectionUpdated( TreeBox& sender );
   void __InputImages_Click( Button& sender, bool checked );
   void __FormatHints_EditCompleted( Edit& sender );
   void __Integration_ItemSelected( ComboBox& sender, int itemIndex );
   void __Integration_EditCompleted( Edit& sender );
   void __Integration_SpinValueUpdated( SpinBox& sender, int value );
   void __Integration_Click( Button& sender, bool checked );
   void __Rejection_ItemSelected( ComboBox& sender, int itemIndex );
   void __Rejection_SpinValueUpdated( SpinBox& sender, int value );
   void __Rejection_EditValueUpdated( NumericEdit& sender, double value );
   void __Rejection_Click( Button& sender, bool checked );
   void __ROI_Check( SectionBar& sender, bool checked );
   void __ROI_SpinValueUpdated( SpinBox& sender, int value );
   void __ROI_Click( Button& sender, bool checked );
   void __ToggleSection( SectionBar& sender, Control& section, bool start );
   void __FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void __FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   String DrizzleTargetName( const String& filePath );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ImageIntegrationInterface* TheImageIntegrationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ImageIntegrationInterface_h

// ----------------------------------------------------------------------------
// EOF ImageIntegrationInterface.h - Released 2017-05-05T08:37:32Z
