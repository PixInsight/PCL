//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.15.0209
// ----------------------------------------------------------------------------
// INDIDeviceControllerInterface.h - Released 2017-07-18T16:14:19Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2017 Klaus Kretzschmar
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

#ifndef __INDIDeviceControllerInterface_h
#define __INDIDeviceControllerInterface_h

#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/ProcessInterface.h>
#include <pcl/PushButton.h>
#include <pcl/SectionBar.h>
#include <pcl/Sizer.h>
#include <pcl/SpinBox.h>
#include <pcl/Timer.h>
#include <pcl/TreeBox.h>

#include "INDIDeviceControllerInstance.h"

namespace pcl
{

// ----------------------------------------------------------------------------

class INDIDeviceControllerInterface : public ProcessInterface
{
public:

   INDIDeviceControllerInterface();
   virtual ~INDIDeviceControllerInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual InterfaceFeatures Features() const;

   void UpdateControls();

   struct GUIData
   {
      GUIData( INDIDeviceControllerInterface& );

      VerticalSizer     Global_Sizer;
         SectionBar        Server_SectionBar;
         Control           Server_Control;
         HorizontalSizer   Server_Sizer;
            Label             HostName_Label;
            Edit              HostName_Edit;
            Label             Port_Label;
            SpinBox           Port_SpinBox;
            VerticalSizer     ServerAction_Sizer;
               PushButton        Connect_PushButton;
               PushButton        Disconnect_PushButton;
         SectionBar        Devices_SectionBar;
         Control           Devices_Control;
         HorizontalSizer   Devices_Sizer;
            TreeBox           Devices_TreeBox;
            VerticalSizer     NodeAction_Sizer;
               PushButton        NodeAction_PushButton;
         Label             ServerMessage_Label;

      Timer SynchronizeWithServer_Timer;
   };

private:

   GUIData* GUI = nullptr;

   void UpdateDeviceLists();
   void UpdateNodeActionButtons( TreeBox::Node* );
   void AdjustTreeColumns();

   void e_Show( Control& );
   void e_Hide( Control& );
   void e_ToggleSection( SectionBar& sender, Control& section, bool start );
   void e_Click( Button& sender, bool checked );
   void e_EditCompleted( Edit& sender );
   void e_SpinValueUpdated( SpinBox& sender, int value );
   void e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent );
   void e_NodeActivated( TreeBox& sender, TreeBox::Node&, int col );
   void e_NodeDoubleClicked( TreeBox& sender, TreeBox::Node&, int col );
   void e_NodeExpanded( TreeBox& sender, TreeBox::Node& );
   void e_NodeCollapsed( TreeBox& sender, TreeBox::Node& );
   void e_NodeSelectionUpdated( TreeBox& sender );
   void e_Timer( Timer& sender );

   friend struct GUIData;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern INDIDeviceControllerInterface* TheINDIDeviceControllerInterface;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __INDIDeviceControllerInterface_h

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerInterface.h - Released 2017-07-18T16:14:19Z
