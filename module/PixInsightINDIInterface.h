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
#include <pcl/RadioButton.h>
#include <pcl/TreeBox.h>
#include <pcl/ErrorHandler.h>
#include <pcl/Timer.h>
#include <pcl/Mutex.h>
#include <map>
#include <vector>


#include "PixInsightINDIInstance.h"

#if defined(__PCL_LINUX)
#include <memory>
#endif

namespace pcl
{

class PropertyNode;
class PropertyTree;
class PixInsightINDIInterface;

class SetPropertyDialog : public Dialog 
{
public:
	SetPropertyDialog(PixInsightINDIInstance* indiInstance ):Dialog(),m_instance(indiInstance) {}
	virtual ~SetPropertyDialog() {}
	
	virtual void setPropertyLabelString(String label){}
	virtual void setPropertyValueString(String value){}
	void setNewPropertyListItem(INDINewPropertyListItem& newPropItem) {m_newPropertyListItem=newPropItem;}
	INDINewPropertyListItem getNewPropertyListItem(){return m_newPropertyListItem;}

	static SetPropertyDialog* createPropertyDialog(IsoString IndiType,IsoString NumberFormatType,PixInsightINDIInstance* indiInstance);

	void Ok_Button_Click( Button& sender, bool checked );
	void Cancel_Button_Click( Button& sender, bool checked );


protected:
	INDINewPropertyListItem m_newPropertyListItem;

private:
	
	PixInsightINDIInstance*  m_instance;

	friend class INDIClient;

};

class EditPropertyDialog : public SetPropertyDialog {
private:
	VerticalSizer       Global_Sizer;
		HorizontalSizer		Property_Sizer;
			Label               Property_Label;
			Edit				Property_Edit;
		HorizontalSizer	Buttons_Sizer;
			PushButton			OK_PushButton;
			PushButton			Cancel_PushButton;

public:
	EditPropertyDialog(PixInsightINDIInstance* indiInstance );
	virtual ~EditPropertyDialog(){}

	virtual void setPropertyLabelString(String label){Property_Label.SetText(label);}
	virtual void setPropertyValueString(String value){Property_Edit.SetText(value);}
	void EditCompleted( Edit& sender );
};

class EditSwitchPropertyDialog : public SetPropertyDialog {
private:
	VerticalSizer       Global_Sizer;
		HorizontalSizer		Property_Sizer;
			Label               Property_Label;
			Label               ON_Label;
			RadioButton			ON_RadioButton;
			Label               OFF_Label;
			RadioButton			OFF_RadioButton;
		HorizontalSizer	Buttons_Sizer;
			PushButton			OK_PushButton;
			PushButton			Cancel_PushButton;

public:
	EditSwitchPropertyDialog(PixInsightINDIInstance* indiInstance );
	virtual ~EditSwitchPropertyDialog(){}

	virtual void setPropertyLabelString(String label){Property_Label.SetText(label);}
	virtual void setPropertyValueString(String value);
	void ButtonChecked( RadioButton& sender );
};


class EditNumberCoordPropertyDialog : public SetPropertyDialog {
private:
	VerticalSizer       Global_Sizer;
		HorizontalSizer		Property_Sizer;
			Label               Property_Label;
			Edit				Hour_Edit;
			Label               Colon1_Label;
			Edit				Minute_Edit;
			Label               Colon2_Label;
			Edit				Second_Edit;
		HorizontalSizer	Buttons_Sizer;
			PushButton			OK_PushButton;
			PushButton			Cancel_PushButton;

	double m_hour;
	double m_minute;
	double m_second;



public:
	EditNumberCoordPropertyDialog(PixInsightINDIInstance* indiInstance );
	virtual ~EditNumberCoordPropertyDialog(){}

	virtual void setPropertyLabelString(String label){Property_Label.SetText(label);}
	virtual void setPropertyValueString(String value);
	void EditCompleted( Edit& sender);
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
   virtual void ApplyInstanceGlobal() const;
   virtual void ResetInstance();

   virtual bool Launch( const MetaProcess&, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ );

   virtual ProcessImplementation* NewProcess() const;

   virtual bool ValidateProcess( const ProcessImplementation&, String& whyNot ) const;
   virtual bool RequiresInstanceValidation() const;

   virtual bool ImportProcess( const ProcessImplementation& );

   virtual InterfaceFeatures Features() const;

   void UpdateControls();
   
   // PixInsight client GUI
   struct GUIData
   {
      GUIData( PixInsightINDIInterface& );
      ~GUIData(){delete SetPropDlg;}
	  Timer				 UpdateDeviceList_Timer;
	  Timer              UpdatePropertyList_Timer;
	  Timer              UpdateServerMessage_Timer;

      VerticalSizer      Global_Sizer;
	   SectionBar         INDIServer_SectionBar;
	   Control			  INDIServerConnection_Control;
		HorizontalSizer		ParameterHost_Sizer;
		  VerticalSizer         ServerData_VSizer;
		   HorizontalSizer		  ServerData_Sizer;
              Label				  ParameterHost_Label;
              Edit				  ParameterHost_Edit;
              Label				  ParameterPort_Label;
              SpinBox			  ParameterPort_SpinBox;
		  VerticalSizer		     ConnectionServer_Sizer;
				PushButton			ConnectServer_PushButton;
				PushButton			DisconnectServer_PushButton;
		SectionBar         INDIDevices_SectionBar;
		Control			   INDIDevices_Control;
		 HorizontalSizer	INDIDevice_Sizer;
			TreeBox				DeviceList_TreeBox;
			VerticalSizer		DeviceAction_Sizer;
				PushButton			ConnectDevice_PushButton;
				PushButton			DisconnectDevice_PushButton;
		SectionBar         INDIProperties_SectionBar;
		Control			   INDIProperties_Control;
		 HorizontalSizer    INDIDeviceProperty_Sizer;
		 VerticalSizer		INDIDevicePropertyTreeBox_Sizer;
			TreeBox				PropertyList_TreeBox;
		 VerticalSizer		  Buttons_Sizer;
			PushButton			EditProperty_PushButton;
		 HorizontalSizer      ServerMessage_Sizer;
		    Label               ServerMessageLabel_Label;
		    Label               ServerMessage_Label;
		SetPropertyDialog* SetPropDlg;
   };

   private:

   typedef std::map<IsoString,PropertyNode*> PropertyNodeMapType;
   typedef std::map<IsoString,PropertyTree*> PropertyTreeMapType;
   typedef std::vector<PropertyNode*>        PropertyNodeVectorType;
   PixInsightINDIInstance instance;

   GUIData* GUI;

   IsoString m_serverMessage;

   PropertyNodeMapType m_rootNodeMap;
   PropertyNodeMapType m_deviceRootNodeMap;
   PropertyNodeMapType m_deviceNodeMap;
   PropertyTreeMapType m_propertyTreeMap;


   void UpdatePropertyList();

   void __UpdateDeviceList_Timer( Timer& sender );
   void __UpdatePropertyList_Timer( Timer& sender );
   void __UpdateServerMessage_Timer( Timer& sender );

   void UpdateDeviceList();

   // Event Handlers
   void __CameraListButtons_Click( Button& sender, bool checked );
   void PropertyButton_Click( Button& sender, bool checked );

   void __RealValueUpdated( NumericEdit& sender, double value );
   void __IntegerValueUpdated( SpinBox& sender, int value );
   void __ItemClicked( Button& sender, bool checked );
   void __ItemSelected( ComboBox& sender, int itemIndex );
   void __EditGetFocus( Control& sender );
   void __EditCompleted( Edit& sender );
   void __Close(Control& sender, bool& allowClose);

   friend struct GUIData;
   friend class  DevicePropertiesDialog;
   friend class  INDIClient;
   friend class  CCDFrameInterface;
};

// ----------------------------------------------------------------------------

PCL_BEGIN_LOCAL
extern PixInsightINDIInterface* ThePixInsightINDIInterface; 
PCL_END_LOCAL

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PixInsightINDIInterface_h

// ****************************************************************************
// EOF PixInsightINDIInterface.h - Released 2013/03/24 18:42:27 UTC
