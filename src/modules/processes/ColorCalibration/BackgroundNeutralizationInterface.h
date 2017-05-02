//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.02.00.0257
// ----------------------------------------------------------------------------
// BackgroundNeutralizationInterface.h - Released 2017-05-02T09:43:00Z
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

#ifndef __BackgroundNeutralizationInterface_h
#define __BackgroundNeutralizationInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/Label.h>
#include <pcl/Edit.h>
#include <pcl/NumericControl.h>
#include <pcl/ToolButton.h>
#include <pcl/PushButton.h>
#include <pcl/GroupBox.h>
#include <pcl/ComboBox.h>
#include <pcl/SpinBox.h>

#include "BackgroundNeutralizationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class BackgroundNeutralizationInterface : public ProcessInterface
{
public:

   BackgroundNeutralizationInterface();
   virtual ~BackgroundNeutralizationInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;
   virtual void ApplyInstance() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

private:

   BackgroundNeutralizationInstance instance;

   struct GUIData
   {
      GUIData( BackgroundNeutralizationInterface& );

      VerticalSizer     Global_Sizer;

      HorizontalSizer   BackgroundReferenceView_Sizer;
         Label             BackgroundReferenceView_Label;
         Edit              BackgroundReferenceView_Edit;
         ToolButton        BackgroundReferenceView_ToolButton;
      NumericControl    BackgroundLow_NumericControl;
      NumericControl    BackgroundHigh_NumericControl;
      HorizontalSizer   Mode_Sizer;
         Label             Mode_Label;
         ComboBox          Mode_ComboBox;
      NumericControl    TargetBackground_NumericControl;
      GroupBox          ROI_GroupBox;
      VerticalSizer     ROI_Sizer;
         HorizontalSizer   ROIRow1_Sizer;
            Label             ROIX0_Label;
            SpinBox           ROIX0_SpinBox;
            Label             ROIY0_Label;
            SpinBox           ROIY0_SpinBox;
         HorizontalSizer   ROIRow2_Sizer;
            Label             ROIWidth_Label;
            SpinBox           ROIWidth_SpinBox;
            Label             ROIHeight_Label;
            SpinBox           ROIHeight_SpinBox;
            PushButton        ROISelectPreview_Button;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();

   void __GetFocus( Control& sender );
   void __EditCompleted( Edit& sender );
   void __EditValueUpdated( NumericEdit& sender, double value );
   void __Click( Button& sender, bool checked );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __ROI_Check( GroupBox& sender, bool checked );
   void __ROI_SpinValueUpdated( SpinBox& sender, int value );
   void __ROI_Click( Button& sender, bool checked );
   void __ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView );
   void __ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern BackgroundNeutralizationInterface* TheBackgroundNeutralizationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __BackgroundNeutralizationInterface_h

// ----------------------------------------------------------------------------
// EOF BackgroundNeutralizationInterface.h - Released 2017-05-02T09:43:00Z
