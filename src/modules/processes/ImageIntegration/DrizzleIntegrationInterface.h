//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.12.00.0351
// ----------------------------------------------------------------------------
// DrizzleIntegrationInterface.h - Released 2016/12/20 17:42:14 UTC
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

#ifndef __DrizzleIntegrationInterface_h
#define __DrizzleIntegrationInterface_h

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

#include "DrizzleIntegrationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class DrizzleIntegrationInterface : public ProcessInterface
{
public:

   DrizzleIntegrationInterface();
   virtual ~DrizzleIntegrationInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   InterfaceFeatures Features() const;

   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual void SaveSettings() const;

private:

   DrizzleIntegrationInstance m_instance;

   struct GUIData
   {
      GUIData( DrizzleIntegrationInterface& );

      VerticalSizer  Global_Sizer;

      SectionBar        InputData_SectionBar;
      Control           InputData_Control;
      HorizontalSizer   InputData_Sizer;
         TreeBox           InputData_TreeBox;
         VerticalSizer     InputButtons_Sizer;
            PushButton        AddFiles_PushButton;
            PushButton        SelectAll_PushButton;
            PushButton        InvertSelection_PushButton;
            PushButton        ToggleSelected_PushButton;
            PushButton        RemoveSelected_PushButton;
            PushButton        Clear_PushButton;
            CheckBox          FullPaths_CheckBox;

      SectionBar        FormatHints_SectionBar;
      Control           FormatHints_Control;
      VerticalSizer     FormatHints_Sizer;
         HorizontalSizer   InputHints_Sizer;
            Label             InputHints_Label;
            Edit              InputHints_Edit;
         HorizontalSizer   InputDirectory_Sizer;
            Label             InputDirectory_Label;
            Edit              InputDirectory_Edit;
            ToolButton        InputDirectory_ToolButton;

      SectionBar        Integration_SectionBar;
      Control           Integration_Control;
      VerticalSizer     Integration_Sizer;
         HorizontalSizer   Scale_Sizer;
            Label             Scale_Label;
            SpinBox           Scale_SpinBox;
         NumericControl    DropShrink_NumericControl;
         HorizontalSizer   KernelFunction_Sizer;
            Label             KernelFunction_Label;
            ComboBox          KernelFunction_ComboBox;
         HorizontalSizer   GridSize_Sizer;
            Label             GridSize_Label;
            SpinBox           GridSize_SpinBox;
         HorizontalSizer   EnableRejection_Sizer;
            CheckBox          EnableRejection_CheckBox;
         HorizontalSizer   EnableImageWeighting_Sizer;
            CheckBox          EnableImageWeighting_CheckBox;
         HorizontalSizer   EnableSurfaceSplines_Sizer;
            CheckBox          EnableSurfaceSplines_CheckBox;
         HorizontalSizer   ClosePreviousImages_Sizer;
            CheckBox          ClosePreviousImages_CheckBox;

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

   GUIData* GUI;

   // Interface Updates

   void UpdateControls();
   void UpdateInputDataItem( size_type );
   void UpdateInputDataList();
   void UpdateDataSelectionButtons();
   void UpdateFormatHintsControls();
   void UpdateIntegrationControls();
   void UpdateROIControls();

   // Event Handlers

   void __ValueUpdated( NumericEdit& sender, double value );
   void __CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void __NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void __NodeSelectionUpdated( TreeBox& sender );
   void __Click( Button& sender, bool checked );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __EditCompleted( Edit& sender );
   void __SpinValueUpdated( SpinBox& sender, int value );
   void __ToggleSection( SectionBar& sender, Control& section, bool start );
   void __CheckSection( SectionBar& sender, bool checked );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern DrizzleIntegrationInterface* TheDrizzleIntegrationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __DrizzleIntegrationInterface_h

// ----------------------------------------------------------------------------
// EOF DrizzleIntegrationInterface.h - Released 2016/12/20 17:42:14 UTC
