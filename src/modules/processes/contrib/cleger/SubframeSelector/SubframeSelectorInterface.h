//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard SubframeSelector Process Module Version 01.04.01.0012
// ----------------------------------------------------------------------------
// SubframeSelectorInterface.h - Released 2018-11-23T18:45:58Z
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

#ifndef e_SubframeSelectorInterface_h
#define e_SubframeSelectorInterface_h

#include <pcl/BitmapBox.h>
#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/TextBox.h>
#include <pcl/ToolButton.h>
#include <pcl/TreeBox.h>

#include "GraphWebView.h"
#include "SubframeSelectorInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class SubframeSelectorInterface : public ProcessInterface
{
public:

   SubframeSelectorInterface();
   virtual ~SubframeSelectorInterface();

   IsoString Id() const override;
   MetaProcess* Process() const override;
   const char** IconImageXPM() const override;

   InterfaceFeatures Features() const override;

   void ResetInstance() override;

   bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned&/*flags*/ ) override;

   ProcessImplementation* NewProcess() const override;

   bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const override;
   bool RequiresInstanceValidation() const override;

   bool ImportProcess( const ProcessImplementation& ) override;

private:

   SubframeSelectorInstance m_instance;
   bool                     m_expressionsWasVisible = true;
   bool                     m_measurementsWasVisible = true;

   struct GUIData
   {
      GUIData( SubframeSelectorInterface& );

      VerticalSizer     Global_Sizer;

      HorizontalSizer   Routine_Sizer;
         Label             Routine_Label;
         ComboBox          Routine_Control;
         ToolButton        GoToExpressionsWindow_ToolButton;
         ToolButton        GoToMeasurementsWindow_ToolButton;

      SectionBar        SubframeImages_SectionBar;
      Control           SubframeImages_Control;
      HorizontalSizer   SubframeImages_Sizer;
         TreeBox           SubframeImages_TreeBox;
         VerticalSizer     SubframeButtons_Sizer;
            PushButton        SubframeImages_AddFiles_PushButton;
            PushButton        SubframeImages_Invert_PushButton;
            PushButton        SubframeImages_Toggle_PushButton;
            PushButton        SubframeImages_Remove_PushButton;
            PushButton        SubframeImages_Clear_PushButton;
            CheckBox          SubframeImages_FileCache_Control;

      SectionBar        SystemParameters_SectionBar;
      Control           SystemParameters_Control;
      VerticalSizer     SystemParameters_Sizer;
         HorizontalSizer   SystemParameters_SubframeScale_Sizer;
            NumericControl    SystemParameters_SubframeScale_Control;
            Label             SystemParameters_SubframeScale_Unit;
         HorizontalSizer   SystemParameters_CameraGain_Sizer;
            NumericControl    SystemParameters_CameraGain_Control;
            Label             SystemParameters_CameraGain_Unit;
         HorizontalSizer   SystemParameters_CameraResolution_Sizer;
            Label             SystemParameters_CameraResolution_Label;
            ComboBox          SystemParameters_CameraResolution_Control;
         HorizontalSizer   SystemParameters_SiteLocalMidnight_Sizer;
            Label             SystemParameters_SiteLocalMidnight_Label;
            SpinBox           SystemParameters_SiteLocalMidnight_Control;
            Label             SystemParameters_SiteLocalMidnight_Unit;
         HorizontalSizer   SystemParameters_ScaleUnit_Sizer;
            Label             SystemParameters_ScaleUnit_Label;
            ComboBox          SystemParameters_ScaleUnit_Control;
         HorizontalSizer   SystemParameters_DataUnit_Sizer;
            Label             SystemParameters_DataUnit_Label;
            ComboBox          SystemParameters_DataUnit_Control;

      SectionBar        StarDetectorParameters_SectionBar;
      Control           StarDetectorParameters_Control;
      VerticalSizer     StarDetectorParameters_Sizer;
         HorizontalSizer   StarDetectorParameters_StructureLayers_Sizer;
            Label             StarDetectorParameters_StructureLayers_Label;
            SpinBox           StarDetectorParameters_StructureLayers_Control;
         HorizontalSizer   StarDetectorParameters_NoiseLayers_Sizer;
            Label             StarDetectorParameters_NoiseLayers_Label;
            SpinBox           StarDetectorParameters_NoiseLayers_Control;
         HorizontalSizer   StarDetectorParameters_HotPixelFilterRadius_Sizer;
            Label             StarDetectorParameters_HotPixelFilterRadius_Label;
            SpinBox           StarDetectorParameters_HotPixelFilterRadius_Control;
         HorizontalSizer   StarDetectorParameters_ApplyHotPixelFilter_Sizer;
            CheckBox          StarDetectorParameters_ApplyHotPixelFilter_CheckBox;
         HorizontalSizer   StarDetectorParameters_NoiseReductionFilterRadius_Sizer;
            Label             StarDetectorParameters_NoiseReductionFilterRadius_Label;
            SpinBox           StarDetectorParameters_NoiseReductionFilterRadius_Control;
         NumericControl    StarDetectorParameters_Sensitivity_Control;
         NumericControl    StarDetectorParameters_PeakResponse_Control;
         NumericControl    StarDetectorParameters_MaxDistortion_Control;
         NumericControl    StarDetectorParameters_UpperLimit_Control;
         HorizontalSizer   StarDetectorParameters_BackgroundExpansion_Sizer;
            Label             StarDetectorParameters_BackgroundExpansion_Label;
            SpinBox           StarDetectorParameters_BackgroundExpansion_Control;
         NumericControl    StarDetectorParameters_XYStretch_Control;
         HorizontalSizer   StarDetectorParameters_PSFFit_Sizer;
            Label             StarDetectorParameters_PSFFit_Label;
            ComboBox          StarDetectorParameters_PSFFit_Control;
         HorizontalSizer   StarDetectorParameters_PSFFitCircular_Sizer;
            CheckBox          StarDetectorParameters_PSFFitCircular_CheckBox;
         HorizontalSizer   StarDetectorParameters_Pedestal_Sizer;
            Label             StarDetectorParameters_Pedestal_Label;
            SpinBox           StarDetectorParameters_Pedestal_Control;
            Label             StarDetectorParameters_Pedestal_Unit;

      SectionBar        StarDetectorParameters_ROI_SectionBar;
      Control           StarDetectorParameters_ROI_Control;
      VerticalSizer     StarDetectorParameters_ROI_Sizer;
         HorizontalSizer   StarDetectorParameters_ROIRow1_Sizer;
            Label             StarDetectorParameters_ROIX0_Label;
            SpinBox           StarDetectorParameters_ROIX0_Control;
            Label             StarDetectorParameters_ROIY0_Label;
            SpinBox           StarDetectorParameters_ROIY0_Control;
         HorizontalSizer   StarDetectorParameters_ROIRow2_Sizer;
            Label             StarDetectorParameters_ROIWidth_Label;
            SpinBox           StarDetectorParameters_ROIWidth_Control;
            Label             StarDetectorParameters_ROIHeight_Label;
            SpinBox           StarDetectorParameters_ROIHeight_Control;
            PushButton        StarDetectorParameters_ROISelectPreview_Button;

      SectionBar        FormatHints_SectionBar;
      Control           FormatHints_Control;
      VerticalSizer     FormatHints_Sizer;
         HorizontalSizer   InputHints_Sizer;
            Label             InputHints_Label;
            Edit              InputHints_Edit;
         HorizontalSizer   OutputHints_Sizer;
            Label             OutputHints_Label;
            Edit              OutputHints_Edit;

      SectionBar        OutputFiles_SectionBar;
      Control           OutputFiles_Control;
      VerticalSizer     OutputFiles_Sizer;
         HorizontalSizer   OutputDirectory_Sizer;
            Label             OutputDirectory_Label;
            Edit              OutputDirectory_Edit;
            ToolButton        OutputDirectory_ToolButton;
         HorizontalSizer   OutputChunks_Sizer;
            Label             OutputPrefix_Label;
            Edit              OutputPrefix_Edit;
            Label             OutputPostfix_Label;
            Edit              OutputPostfix_Edit;
         HorizontalSizer   OutputKeyword_Sizer;
            Label             OutputKeyword_Label;
            Edit              OutputKeyword_Edit;
         HorizontalSizer   OutputMisc_Sizer;
            CheckBox          OverwriteExistingFiles_CheckBox;
            Label             OnError_Label;
            ComboBox          OnError_ComboBox;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void UpdateSubframeImageItem( size_type );
   void UpdateSubframeImagesList();
   void UpdateSubframeImageSelectionButtons();
   void UpdateSystemParameters();
   void UpdateStarDetectorParameters();
   void UpdateOutputFilesControls();

   void ShowExpressionsInterface() const;
   void HideExpressionsInterface() const;
   void ShowMeasurementsInterface() const;
   void HideMeasurementsInterface() const;

   /*
    * Event Handlers
    */
   void e_ToggleSection( SectionBar& sender, Control& section, bool start );
   void e_SubframeImages_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void e_SubframeImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void e_SubframeImages_NodeSelectionUpdated( TreeBox& sender );
   void e_SubframeImages_Click( Button& sender, bool checked );
   void e_RealValueUpdated( NumericEdit& sender, double value );
   void e_IntegerValueUpdated( SpinBox& sender, int value );
   void e_ComboSelected( ComboBox& sender, int itemIndex );
   void e_CheckboxUpdated( Button& sender, Button::check_state state );
   void e_ButtonClick( Button& sender, bool checked );
   void e_EditCompleted( Edit& sender );
   void e_StarDetector_ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void e_StarDetector_ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );
   void e_FileDrag( Control& sender, const Point& pos, const StringList& files, unsigned modifiers, bool& wantsFiles );
   void e_FileDrop( Control& sender, const Point& pos, const StringList& files, unsigned modifiers );
   void e_Hide( Control& );

   friend struct GUIData;
   friend class SubframeSelectorInstance;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern SubframeSelectorInterface* TheSubframeSelectorInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // e_SubframeSelectorInterface_h

// ----------------------------------------------------------------------------
// EOF SubframeSelectorInterface.h - Released 2018-11-23T18:45:58Z
