//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0294
// ----------------------------------------------------------------------------
// ChannelCombinationInterface.h - Released 2015/12/18 08:55:08 UTC
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

#ifndef __ChannelCombinationInterface_h
#define __ChannelCombinationInterface_h

#include <pcl/ProcessInterface.h>

#include <pcl/Sizer.h>
#include <pcl/GroupBox.h>
#include <pcl/RadioButton.h>
#include <pcl/CheckBox.h>
#include <pcl/Edit.h>
#include <pcl/ToolButton.h>
#include <pcl/NumericControl.h>
#include <pcl/ViewList.h>

#include "ChannelCombinationInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ChannelCombinationInterface : public ProcessInterface
{
public:

   ChannelCombinationInterface();
   virtual ~ChannelCombinationInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   InterfaceFeatures Features() const;

   virtual void Initialize();

   virtual void ApplyInstance() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;
   virtual bool ImportProcess( const ProcessImplementation& );

private:

   ChannelCombinationInstance instance;

   struct GUIData
   {
      GUIData( ChannelCombinationInterface& );

      HorizontalSizer      Global_Sizer;
         GroupBox          ColorSpace_GroupBox;
         HorizontalSizer   ColorSpace_Sizer;
            VerticalSizer     ColorSpaceLeft_Sizer;
               RadioButton       RGB_RadioButton;
               RadioButton       HSV_RadioButton;
               RadioButton       HSI_RadioButton;
            VerticalSizer     ColorSpaceRight_Sizer;
               RadioButton       CIEXYZ_RadioButton;
               RadioButton       CIELab_RadioButton;
               RadioButton       CIELch_RadioButton;
         GroupBox          Channels_GroupBox;
         VerticalSizer     Channels_Sizer;
            HorizontalSizer   C0_Sizer;
               CheckBox          C0_CheckBox;
               Edit              C0_Edit;
               ToolButton        C0_ToolButton;
            HorizontalSizer   C1_Sizer;
               CheckBox          C1_CheckBox;
               Edit              C1_Edit;
               ToolButton        C1_ToolButton;
            HorizontalSizer   C2_Sizer;
               CheckBox          C2_CheckBox;
               Edit              C2_Edit;
               ToolButton        C2_ToolButton;
            HorizontalSizer   TargetImage_Sizer;
               Label             TargetImage_Label;
               ViewList          TargetImage_ViewList;
   };

   GUIData* GUI;

   void UpdateControls();

   void __ColorSpace_Click( Button& sender, bool checked );
   void __Channel_Click( Button& sender, bool checked );
   void __ChannelId_GetFocus( Control& sender );
   void __ChannelId_EditCompleted( Edit& sender );
   void __Channel_SelectSource_Click( Button& sender, bool checked );
   void __TargetImage_ViewSelected( ViewList& sender, View& view );

   friend struct GUIData;
   friend class  ChannelSourceSelectionDialog;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ChannelCombinationInterface* TheChannelCombinationInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ChannelCombinationInterface_h

// ----------------------------------------------------------------------------
// EOF ChannelCombinationInterface.h - Released 2015/12/18 08:55:08 UTC
