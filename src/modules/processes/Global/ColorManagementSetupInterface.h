//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard Global Process Module Version 01.02.07.0397
// ----------------------------------------------------------------------------
// ColorManagementSetupInterface.h - Released 2018-12-12T09:25:24Z
// ----------------------------------------------------------------------------
// This file is part of the standard Global PixInsight module.
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

#ifndef __ColorManagementSetupInterface_h
#define __ColorManagementSetupInterface_h

#include <pcl/CheckBox.h>
#include <pcl/ColorComboBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/GroupBox.h>
#include <pcl/ICCProfile.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/RadioButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/StringList.h>

#include "ColorManagementSetupInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class ColorManagementSetupInterface : public ProcessInterface
{
public:

   ColorManagementSetupInterface();
   virtual ~ColorManagementSetupInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;

   virtual void Initialize();

   virtual void ApplyInstance() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;
   virtual bool ImportProcess( const ProcessImplementation& );

private:

   ColorManagementSetupInstance instance;

   ICCProfile::profile_list rgbProfiles;      // RGB only
   ICCProfile::profile_list grayProfiles;     // RGB and grayscale only
   ICCProfile::profile_list proofingProfiles; // any color space

   struct GUIData
   {
      GUIData( ColorManagementSetupInterface& );

      VerticalSizer     Global_Sizer;

      SectionBar        MonitorProfile_SectionBar;
      Control           MonitorProfile_Control;
      VerticalSizer     MonitorProfile_Sizer;
         Edit              MonitorProfile_Edit;
         Edit              MonitorProfileFullPath_Edit;
         HorizontalSizer   RenderingIntent_Sizer;
            Label             RenderingIntent_Label;
            ComboBox          RenderingIntent_ComboBox;

      SectionBar        SystemSettings_SectionBar;
      Control           SystemSettings_Control;
      VerticalSizer     SystemSettings_Sizer;
         GroupBox          NewMonitorProfile_GroupBox;
         VerticalSizer     NewMonitorProfile_Sizer;
            Edit              NewMonitorProfile_Edit;
            ComboBox          NewMonitorProfile_ComboBox;

      SectionBar        DefaultProfiles_SectionBar;
      Control           DefaultProfiles_Control;
      VerticalSizer     DefaultProfiles_Sizer;
         GroupBox          RGBProfiles_GroupBox;
         VerticalSizer     RGBProfiles_Sizer;
            Edit              RGBProfileId_Edit;
            ComboBox          RGBProfile_ComboBox;
         GroupBox          GrayscaleProfiles_GroupBox;
         VerticalSizer     GrayscaleProfiles_Sizer;
            Edit              GrayscaleProfileId_Edit;
            ComboBox          GrayscaleProfile_ComboBox;

      SectionBar        DefaultPolicies_SectionBar;
      Control           DefaultPolicies_Control;
      HorizontalSizer   DefaultPolicies_Sizer;
         GroupBox          OnProfileMismatch_GroupBox;
         VerticalSizer     OnProfileMismatch_Sizer;
            RadioButton       OnProfileMismatch_Ask_RadioButton;
            RadioButton       OnProfileMismatch_Keep_RadioButton;
            RadioButton       OnProfileMismatch_Convert_RadioButton;
            RadioButton       OnProfileMismatch_Discard_RadioButton;
            RadioButton       OnProfileMismatch_Disable_RadioButton;
         GroupBox          OnMissingProfile_GroupBox;
         VerticalSizer     OnMissingProfile_Sizer;
            RadioButton       OnMissingProfile_Ask_RadioButton;
            RadioButton       OnMissingProfile_Assign_RadioButton;
            RadioButton       OnMissingProfile_Leave_RadioButton;
            RadioButton       OnMissingProfile_Disable_RadioButton;

      SectionBar        Proofing_SectionBar;
      Control           Proofing_Control;
      VerticalSizer     Proofing_Sizer;
         GroupBox          ProofingProfile_GroupBox;
         VerticalSizer     ProofingProfile_Sizer;
            Edit              ProofingProfileId_Edit;
            ComboBox          ProofingProfile_ComboBox;
         HorizontalSizer   ProofingIntent_Sizer;
            Label             ProofingIntent_Label;
            ComboBox          ProofingIntent_ComboBox;
         HorizontalSizer   UseProofingBPC_Sizer;
            CheckBox          UseProofingBPC_CheckBox;
         HorizontalSizer   DefaultProofingEnabled_Sizer;
            CheckBox          DefaultProofingEnabled_CheckBox;
         HorizontalSizer   DefaultGamutCheckEnabled_Sizer;
            CheckBox          DefaultGamutCheckEnabled_CheckBox;
         HorizontalSizer   GamutWarningColor_Sizer;
            Label             GamutWarningColor_Label;
            ColorComboBox     GamutWarningColor_ComboBox;
            Control           GamutWarningColor_Control;

      SectionBar        GlobalOptions_SectionBar;
      Control           GlobalOptions_Control;
      HorizontalSizer   GlobalOptions_Sizer;
         VerticalSizer     MiscOptions_Sizer;
            CheckBox          EnableColorManagement_CheckBox;
            CheckBox          UseLowResolutionCLUTs_CheckBox;
         VerticalSizer     EmbedProfiles_Sizer;
            CheckBox          EmbedProfilesInRGBImages_CheckBox;
            CheckBox          EmbedProfilesInGrayscaleImages_CheckBox;

      HorizontalSizer      GlobalActions_Sizer;
         PushButton           RefreshProfiles_PushButton;
         PushButton           LoadCurrentSettings_PushButton;
   };

   GUIData* GUI = nullptr;

   void UpdateControls();
   void RefreshProfiles();

   void __RenderingIntent_ItemSelected( ComboBox& sender, int itemIndex );
   void __Profile_ItemSelected( ComboBox& sender, int itemIndex );
   void __Profile_EditCompleted( Edit& );
   void __OnProfileMismatch_ButtonClick( Button& sender, bool checked );
   void __OnMissingProfile_ButtonClick( Button& sender, bool checked );
   void __ProofingOptions_ButtonClick( Button& sender, bool checked );
   void __ColorSelected( ColorComboBox& sender, RGBA color );
   void __ColorSample_Paint( Control& sender, const Rect& updateRect );
   void __ColorSample_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers );
   void __GlobalOptions_ButtonClick( Button& sender, bool checked );
   void __RefreshProfiles_ButtonClick( Button& sender, bool checked );
   void __LoadCurrentSettings_ButtonClick( Button& sender, bool checked );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern ColorManagementSetupInterface* TheColorManagementSetupInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __ColorManagementSetupInterface_h

// ----------------------------------------------------------------------------
// EOF ColorManagementSetupInterface.h - Released 2018-12-12T09:25:24Z
