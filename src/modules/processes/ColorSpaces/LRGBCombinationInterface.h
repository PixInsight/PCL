//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0288
// ----------------------------------------------------------------------------
// LRGBCombinationInterface.h - Released 2015/11/26 16:00:12 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#ifndef __LRGBCombinationInterface_h
#define __LRGBCombinationInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/CheckBox.h>
#include <pcl/Edit.h>
#include <pcl/ToolButton.h>
#include <pcl/NumericControl.h>
#include <pcl/SpinBox.h>
#include <pcl/ViewList.h>

#include "LRGBCombinationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------
// LRGBCombinationInterface
// ----------------------------------------------------------------------------

class LRGBCombinationInterface : public ProcessInterface
{
public:

   LRGBCombinationInterface();
   virtual ~LRGBCombinationInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   InterfaceFeatures Features() const;

   virtual void ApplyInstance() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;
   virtual bool ImportProcess( const ProcessImplementation& );

   // -------------------------------------------------------------------------

private:

   LRGBCombinationInstance instance;

   // -------------------------------------------------------------------------

   struct GUIData
   {
      GUIData( LRGBCombinationInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        Channels_SectionBar;
      Control           Channels_Control;
      VerticalSizer     Channels_Sizer;
         HorizontalSizer   L_Sizer;
            CheckBox          L_CheckBox;
            Edit              L_Edit;
            ToolButton        L_ToolButton;
         HorizontalSizer   R_Sizer;
            CheckBox          R_CheckBox;
            Edit              R_Edit;
            ToolButton        R_ToolButton;
         HorizontalSizer   G_Sizer;
            CheckBox          G_CheckBox;
            Edit              G_Edit;
            ToolButton        G_ToolButton;
         HorizontalSizer   B_Sizer;
            CheckBox          B_CheckBox;
            Edit              B_Edit;
            ToolButton        B_ToolButton;
         HorizontalSizer   TargetImage_Sizer;
            Label             TargetImage_Label;
            ViewList          TargetImage_ViewList;

      SectionBar        Weights_SectionBar;
      Control           Weights_Control;
      VerticalSizer     Weights_Sizer;
         NumericControl    L_Weight_NumericControl;
         NumericControl    R_Weight_NumericControl;
         NumericControl    G_Weight_NumericControl;
         NumericControl    B_Weight_NumericControl;
         HorizontalSizer   ClipHighlights_Sizer;
            CheckBox          ClipHighlights_CheckBox;

      SectionBar        MTF_SectionBar;
      Control           MTF_Control;
      VerticalSizer     MTF_Sizer;
         NumericControl    LuminanceMTF_NumericControl;
         NumericControl    SaturationMTF_NumericControl;

      SectionBar        NoiseReduction_SectionBar;
      Control           NoiseReduction_Control;
      VerticalSizer     NoiseReduction_Sizer;
         HorizontalSizer   NoiseReductionRow1_Sizer;
            Label             LayersRemoved_Label;
            SpinBox           LayersRemoved_SpinBox;
         HorizontalSizer   NoiseReductionRow2_Sizer;
            Label             LayersProtected_Label;
            SpinBox           LayersProtected_SpinBox;
   };

   GUIData* GUI;

   void UpdateControls();

   void __Channel_Click( Button& sender, bool checked );
   void __ChannelId_GetFocus( Control& sender );
   void __ChannelId_EditCompleted( Edit& sender );
   void __Channel_SelectSource_Click( Button& sender, bool checked );

   void __TargetImage_ViewSelected( ViewList& sender, View& view );

   void __Channel_Weight_ValueUpdated( NumericEdit& sender, double value );
   void __ClipHighlights_Click( Button& sender, bool checked );

   void __MTF_ValueUpdated( NumericEdit& sender, double value );

   void __NoiseReduction_Check( SectionBar& sender, bool checked );
   void __NoiseReduction_ValueUpdated( SpinBox& sender, int value );

   friend struct GUIData;
   friend class  ChannelSourceSelectionDialog;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern LRGBCombinationInterface* TheLRGBCombinationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __LRGBCombinationInterface_h

// ----------------------------------------------------------------------------
// EOF LRGBCombinationInterface.h - Released 2015/11/26 16:00:12 UTC
