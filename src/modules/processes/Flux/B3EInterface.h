//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard Flux Process Module Version 01.00.01.0204
// ----------------------------------------------------------------------------
// B3EInterface.h - Released 2018-12-12T09:25:24Z
// ----------------------------------------------------------------------------
// This file is part of the standard Flux PixInsight module.
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

#ifndef __B3EInterface_h
#define __B3EInterface_h

#include <pcl/Button.h>
#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/ToolButton.h>

#include "B3EInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class B3EInterface : public ProcessInterface
{
public:

   B3EInterface();
   virtual ~B3EInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;

   virtual void ApplyInstance() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

private:

   B3EInstance instance;

   struct GUIData
   {
      GUIData( B3EInterface& );

      VerticalSizer     Global_Sizer;
         VerticalSizer        Input_Sizer;
            HorizontalSizer      InputImage1_Sizer;
               Label                InputImage1_Label;
               Edit                 InputImage1_Edit;
               ToolButton           InputImage1_ToolButton;
            HorizontalSizer      InputImage2_Sizer;
               Label                InputImage2_Label;
               Edit                 InputImage2_Edit;
               ToolButton           InputImage2_ToolButton;
         VerticalSizer        ProcessParameters_Sizer;
            NumericEdit          CenterInput1_NumericEdit;
            NumericEdit          CenterInput2_NumericEdit;
            NumericEdit          CenterOutput_NumericEdit;
            HorizontalSizer      IntensityUnits_Sizer;
               Label                IntensityUnits_Label;
               ComboBox             IntensityUnits_ComboBox;
         VerticalSizer        OutputData_Sizer;
            HorizontalSizer      OutputImages_Sizer;
               Label                OutputImages_Label;
               ComboBox             OutputImages_ComboBox;
            HorizontalSizer      OutOfRangeMask_Sizer;
               CheckBox             OutOfRangeMask_CheckBox;
      // Background Calibration 1
      SectionBar        BackgroundReference1_SectionBar;
      Control           BackgroundReference1_Control;
      VerticalSizer     BackgroundReference1_Sizer;
         HorizontalSizer   BackgroundReferenceView1_Sizer;
            Label             BackgroundReferenceView1_Label;
            Edit              BackgroundReferenceView1_Edit;
            ToolButton        BackgroundReferenceView1_ToolButton;
         NumericControl    BackgroundLow1_NumericControl;
         NumericControl    BackgroundHigh1_NumericControl;
         HorizontalSizer   OutputBackgroundReferenceMask1_Sizer;
            CheckBox          OutputBackgroundReferenceMask1_CheckBox;
         GroupBox          BackgroundROI1_GroupBox;
         VerticalSizer     BackgroundROI1_Sizer;
            HorizontalSizer   BackgroundROIRow11_Sizer;
               Label             BackgroundROIX01_Label;
               SpinBox           BackgroundROIX01_SpinBox;
               Label             BackgroundROIY01_Label;
               SpinBox           BackgroundROIY01_SpinBox;
            HorizontalSizer   BackgroundROIRow21_Sizer;
               Label             BackgroundROIWidth1_Label;
               SpinBox           BackgroundROIWidth1_SpinBox;
               Label             BackgroundROIHeight1_Label;
               SpinBox           BackgroundROIHeight1_SpinBox;
               PushButton        BackgroundROISelectPreview1_Button;
      // Background Calibration 2
      SectionBar        BackgroundReference2_SectionBar;
      Control           BackgroundReference2_Control;
      VerticalSizer     BackgroundReference2_Sizer;
         HorizontalSizer   BackgroundReferenceView2_Sizer;
            Label             BackgroundReferenceView2_Label;
            Edit              BackgroundReferenceView2_Edit;
            ToolButton        BackgroundReferenceView2_ToolButton;
         NumericControl    BackgroundLow2_NumericControl;
         NumericControl    BackgroundHigh2_NumericControl;
         HorizontalSizer   OutputBackgroundReferenceMask2_Sizer;
            CheckBox          OutputBackgroundReferenceMask2_CheckBox;
         GroupBox          BackgroundROI2_GroupBox;
         VerticalSizer     BackgroundROI2_Sizer;
            HorizontalSizer   BackgroundROIRow12_Sizer;
               Label             BackgroundROIX02_Label;
               SpinBox           BackgroundROIX02_SpinBox;
               Label             BackgroundROIY02_Label;
               SpinBox           BackgroundROIY02_SpinBox;
            HorizontalSizer   BackgroundROIRow22_Sizer;
               Label             BackgroundROIWidth2_Label;
               SpinBox           BackgroundROIWidth2_SpinBox;
               Label             BackgroundROIHeight2_Label;
               SpinBox           BackgroundROIHeight2_SpinBox;
               PushButton        BackgroundROISelectPreview2_Button;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __EditCompleted( Edit& sender );
   void __EditCompleted_bkg( Edit& sender );
   void __Clicked( Button& sender, bool checked );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __ValueUpdated( NumericEdit& sender, double value );
   void __GetFocus( Control& sender );
   void __SpinValueUpdated( SpinBox& sender, int value );
   void __GroupBoxCheck( GroupBox& sender, bool checked );
   void __BackgroundReference_Check( SectionBar& sender, bool checked );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern B3EInterface* TheB3EInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __B3EInterface_h

// ----------------------------------------------------------------------------
// EOF B3EInterface.h - Released 2018-12-12T09:25:24Z
