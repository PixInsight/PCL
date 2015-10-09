//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 01.00.00.0244
// ----------------------------------------------------------------------------
// AssignICCProfileInterface.h - Released 2015/10/08 11:24:39 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
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

#ifndef __AssignICCProfileInterface_h
#define __AssignICCProfileInterface_h

#include <pcl/ProcessInterface.h>
#include <pcl/ICCProfile.h>

#include <pcl/Sizer.h>
#include <pcl/ViewList.h>
#include <pcl/GroupBox.h>
#include <pcl/Label.h>
#include <pcl/ComboBox.h>
#include <pcl/ToolButton.h>
#include <pcl/Edit.h>
#include <pcl/RadioButton.h>
#include <pcl/PushButton.h>

#include "AssignICCProfileInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------
// AssignICCProfileInterface
// ----------------------------------------------------------------------------

class AssignICCProfileInterface : public ProcessInterface
{
public:

   AssignICCProfileInterface();
   virtual ~AssignICCProfileInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual InterfaceFeatures Features() const;

   virtual void Initialize();

   virtual void ApplyInstance() const;
   virtual void TrackViewUpdated( bool active );
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, pcl::String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual bool WantsImageNotifications() const;
   virtual void ImageUpdated( const View& );
   virtual void ImageFocused( const View& );
   virtual void ImageCMUpdated( const View& );

   virtual bool WantsGlobalNotifications() const;
   virtual void GlobalCMUpdated();

   // -------------------------------------------------------------------------

private:

   AssignICCProfileInstance   instance;
   ICCProfile::profile_list   profiles;

   // -------------------------------------------------------------------------

   struct GUIData
   {
      GUIData( AssignICCProfileInterface& );

      VerticalSizer     Global_Sizer;

      GroupBox          CurrentProfile_GroupBox;
      VerticalSizer     CurrentProfile_Sizer;
         ViewList          AllImages_ViewList;
         Edit              CurrentProfile_Edit;

      GroupBox          NewProfile_GroupBox;
      VerticalSizer     NewProfile_Sizer;
         RadioButton       AssignDefault_RadioButton;
         RadioButton       LeaveUntagged_RadioButton;
         RadioButton       AssignProfile_RadioButton;
         Edit              TargetProfile_Edit;
         HorizontalSizer   AllProfiles_Sizer;
            ComboBox          AllProfiles_ComboBox;
            ToolButton        RefreshProfiles_ToolButton;
   };

   GUIData* GUI;

   void UpdateControls();
   void RefreshProfiles();

   void __ViewList_ViewSelected( ViewList& sender, View& view );

   void __AssignMode_ButtonClick( Button& sender, bool checked );
   void __TargetProfile_EditCompleted( Edit& );
   void __Profile_ItemSelected( ComboBox& sender, int itemIndex );
   void __RefreshProfiles_ButtonClick( Button& sender, bool checked );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern AssignICCProfileInterface* TheAssignICCProfileInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __AssignICCProfileInterface_h

// ----------------------------------------------------------------------------
// EOF AssignICCProfileInterface.h - Released 2015/10/08 11:24:39 UTC
