// ****************************************************************************
// PixInsight Class Library - PCL 02.00.02.0584
// Standard PixInsightINDI Process Module Version 01.00.02.0092
// ****************************************************************************
// PixInsightINDIInterface.h - Released 2013/03/24 18:42:27 UTC
// ****************************************************************************
// This file is part of the standard PixInsightINDI PixInsight module.
//
// Copyright (c) 2003-2013, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#ifndef __PixInsightINDIInterface_h
#define __PixInsightINDIInterface_h

#include <pcl/Dialog.h>
#include <pcl/CheckBox.h>
#include <pcl/ComboBox.h>
#include <pcl/Edit.h>
#include <pcl/Label.h>
#include <pcl/NumericControl.h>
#include <pcl/ProcessInterface.h>
#include <pcl/Sizer.h>
#include <pcl/SectionBar.h>
#include <pcl/SpinBox.h>
#include <pcl/PushButton.h>
#include <pcl/TreeBox.h>
#include <pcl/ErrorHandler.h>
#include <pcl/Timer.h>

#include "PixInsightINDIInstance.h"

#if defined(__PCL_LINUX)
#include <memory>
#endif

namespace pcl
{

class DevicePropertiesDialog : public Dialog 
{
public:
	DevicePropertiesDialog();
	void UpdatePropertyList();
private:
	
	IsoString m_serverMessage;
	HorizontalSizer       Global_Sizer;
		VerticalSizer		INDIDeviceProperty_Sizer;
			TreeBox				PropertyList_TreeBox;
			Label               DeviceMessage_Label;
		 VerticalSizer			Buttons_Sizer;
			PushButton			RefreshProperty_PushButton;
			PushButton			EditProperty_PushButton;
	
	void Button_Click( Button& sender, bool checked );
	
	friend class PixInsightINDIMediator;
	
};	


// ----------------------------------------------------------------------------

class PixInsightINDIInterface : public ProcessInterface
{
public:

   PixInsightINDIInterface();
   virtual ~PixInsightINDIInterface();

   virtual IsoString Id() const;
   virtual MetaProcess* Process() const;
   virtual const char** IconImageXPM() const;

   virtual void ApplyInstance() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );


   
   struct GUIData
   {
      GUIData( PixInsightINDIInterface& );
	  Timer				 UpdateDeviceList_Timer;

      VerticalSizer      Global_Sizer;
	   SectionBar         INDIServer_SectionBar;
	   Control			  INDIServerConnection_Control;
		HorizontalSizer		ParameterHost_Sizer;
            Label				ParameterHost_Label;
            Edit				ParameterHost_Edit;
            Label				ParameterPort_Label;
            SpinBox				ParameterPort_SpinBox;
			VerticalSizer		ConnectionServer_Sizer;
				PushButton			ConnectServer_PushButton;
				PushButton			DisconnectServer_PushButton;
		SectionBar         INDIDevices_SectionBar;
		 HorizontalSizer	INDIDevice_Sizer;
			TreeBox				DeviceList_TreeBox;
			Label				DeviceMessage_Label;
			VerticalSizer		DeviceAction_Sizer;
				PushButton			ConnectDevice_PushButton;
				PushButton			DisconnectDevice_PushButton;
				PushButton			RefreshDevice_PushButton;

		DevicePropertiesDialog DrvPropDlg;
   };

   private:


   PixInsightINDIInstance instance;

   GUIData* GUI;
   
   void __UpdateDeviceList_Timer( Timer& sender );
   
   void UpdateControls();
   void UpdateDeviceList();

   int  m_numOfDevices;
   bool m_PropertyListNeedsUpdate;
   bool numOfDevicesChanged();

   // Event Handlers
   void __CameraListButtons_Click( Button& sender, bool checked );

   void __RealValueUpdated( NumericEdit& sender, double value );
   void __IntegerValueUpdated( SpinBox& sender, int value );
   void __ItemClicked( Button& sender, bool checked );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __EditGetFocus( Control& sender );
   void __EditCompleted( Edit& sender );

   friend struct GUIData;
   friend class PixInsightINDIMediator;
   
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern PixInsightINDIInterface* ThePixInsightINDIInterface;
extern auto_ptr<INDIClient> indiClient;
extern auto_ptr<PixInsightINDIMediator> mediator;
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PixInsightINDIInterface_h

// ****************************************************************************
// EOF PixInsightINDIInterface.h - Released 2013/03/24 18:42:27 UTC
