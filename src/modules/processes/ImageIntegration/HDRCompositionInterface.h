//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard ImageIntegration Process Module Version 01.09.04.0322
// ----------------------------------------------------------------------------
// HDRCompositionInterface.h - Released 2016/02/21 20:22:43 UTC
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

#ifndef __HDRCompositionInterface_h
#define __HDRCompositionInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/ToolButton.h>
#include <pcl/PushButton.h>
#include <pcl/NumericControl.h>
#include <pcl/TreeBox.h>
#include <pcl/SpinBox.h>
#include <pcl/ComboBox.h>
#include <pcl/CheckBox.h>

#include "HDRCompositionInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class HDRCompositionInterface : public ProcessInterface
{
public:

   HDRCompositionInterface();
   virtual ~HDRCompositionInterface();

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

   // -------------------------------------------------------------------------

private:

   HDRCompositionInstance instance;

   // -------------------------------------------------------------------------

   struct GUIData
   {
      GUIData( HDRCompositionInterface& );

      VerticalSizer  Global_Sizer;

      SectionBar        InputImages_SectionBar;
      Control           InputImages_Control;
      HorizontalSizer   InputImages_Sizer;
         TreeBox           InputImages_TreeBox;
         VerticalSizer     InputButtons_Sizer;
            PushButton        AddFiles_PushButton;
            PushButton        MoveUp_PushButton;
            PushButton        MoveDown_PushButton;
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

      SectionBar        HDRComposition_SectionBar;
      Control           HDRComposition_Control;
      VerticalSizer     HDRComposition_Sizer;
         NumericControl    MaskBinarizingThreshold_NumericControl;
         HorizontalSizer   MaskSmoothness_Sizer;
            Label             MaskSmoothness_Label;
            SpinBox           MaskSmoothness_SpinBox;
         HorizontalSizer   MaskGrowth_Sizer;
            Label             MaskGrowth_Label;
            SpinBox           MaskGrowth_SpinBox;
         HorizontalSizer   AutoExposures_Sizer;
            CheckBox          AutoExposures_CheckBox;
         HorizontalSizer   RejectBlack_Sizer;
            CheckBox          RejectBlack_CheckBox;
         HorizontalSizer   Generate64BitResult_Sizer;
            CheckBox          Generate64BitResult_CheckBox;
         HorizontalSizer   OutputMasks_Sizer;
            CheckBox          OutputMasks_CheckBox;
         HorizontalSizer   ClosePreviousImages_Sizer;
            CheckBox          ClosePreviousImages_CheckBox;

      SectionBar        FittingRegion_SectionBar;
      Control           FittingRegion_Control;
      VerticalSizer     FittingRegion_Sizer;
         HorizontalSizer   FittingRectX0_Sizer;
            Label             FittingRectX0_Label;
            SpinBox           FittingRectX0_SpinBox;
         HorizontalSizer   FittingRectY0_Sizer;
            Label             FittingRectY0_Label;
            SpinBox           FittingRectY0_SpinBox;
         HorizontalSizer   FittingRectWidth_Sizer;
            Label             FittingRectWidth_Label;
            SpinBox           FittingRectWidth_SpinBox;
         HorizontalSizer   FittingRectHeight_Sizer;
            Label             FittingRectHeight_Label;
            SpinBox           FittingRectHeight_SpinBox;
            PushButton        SelectPreview_Button;
   };

   GUIData* GUI;

   // Interface Updates

   void UpdateControls();
   void UpdateInputImagesItem( size_type );
   void UpdateInputImagesList();
   void UpdateImageSelectionButtons();
   void UpdateFormatHintsControls();
   void UpdateHDRCompositionControls();
   void UpdateFittingRegionControls();

   // Event Handlers

   void __InputImages_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void __InputImages_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col );
   void __InputImages_NodeSelectionUpdated( TreeBox& sender );
   void __InputImages_Click( Button& sender, bool checked );

   void __FormatHints_EditCompleted( Edit& sender );

   void __HDRComposition_EditValueUpdated( NumericEdit& sender, double value );
   void __HDRComposition_SpinValueUpdated( SpinBox& sender, int value );
   void __HDRComposition_Click( Button& sender, bool checked );

   void __FittingRegion_Check( SectionBar& sender, bool checked );
   void __FittingRegion_SpinValueUpdated( SpinBox& sender, int value );
   void __FittingRegion_Click( Button& sender, bool checked );

   void __ToggleSection( SectionBar& sender, Control& section, bool start );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern HDRCompositionInterface* TheHDRCompositionInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __HDRCompositionInterface_h

// ----------------------------------------------------------------------------
// EOF HDRCompositionInterface.h - Released 2016/02/21 20:22:43 UTC
