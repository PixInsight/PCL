// ****************************************************************************
// PixInsight Class Library - PCL 02.00.02.0584
// Standard PixInsightINDI Process Module Version 01.00.02.0092
// ****************************************************************************
// PixInsightINDIInterface.cpp - Released 2013/03/24 18:42:27 UTC
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

#include "PixInsightINDIInterface.h"
#include "PixInsightINDIParameters.h"
#include "PixInsightINDIProcess.h"
#include "PixInsightINDIclient.h"
#include "basedevice.h"
#include "indibase.h"
#include "indiapi.h"
#include <pcl/Console.h>
#include <assert.h>

#include "PropertyNode.h"

namespace pcl
{

// ----------------------------------------------------------------------------

PixInsightINDIInterface* ThePixInsightINDIInterface = 0;

// ----------------------------------------------------------------------------

//#include "PixInsightINDIIcon.xpm"

// ----------------------------------------------------------------------------

PixInsightINDIInterface::PixInsightINDIInterface() :
ProcessInterface(), instance( ThePixInsightINDIProcess ), GUI( 0 ), m_serverMessage("")
{
   ThePixInsightINDIInterface = this;
}

PixInsightINDIInterface::~PixInsightINDIInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;

   if (m_propertyTreeMap.size()!=0){
	   for (PropertyTreeMapType::iterator iter=m_propertyTreeMap.begin();iter!=m_propertyTreeMap.end(); iter++ ){
		   delete iter->second;
	   }
   }
}

IsoString PixInsightINDIInterface::Id() const
{
   return "PixInsightINDI";
}

MetaProcess* PixInsightINDIInterface::Process() const
{
   return ThePixInsightINDIProcess;
}

const char** PixInsightINDIInterface::IconImageXPM() const
{
   return 0; // PixInsightINDIIcon_XPM; ---> put a nice icon here
}

InterfaceFeatures PixInsightINDIInterface::Features() const
{
	return   InterfaceFeature::DragObject |   InterfaceFeature::BrowseDocumentationButton;
}

void PixInsightINDIInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void PixInsightINDIInterface::ApplyInstanceGlobal() const
{
   instance.LaunchGlobal();
}

void PixInsightINDIInterface::ResetInstance()
{
   PixInsightINDIInstance defaultInstance( ThePixInsightINDIProcess );
   ImportProcess( defaultInstance );
}

bool PixInsightINDIInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "INDI Device Controller" );
      UpdateControls();
   }

   dynamic = false;
   return &P == ThePixInsightINDIProcess;
}

ProcessImplementation* PixInsightINDIInterface::NewProcess() const
{
   return new PixInsightINDIInstance( instance );
}

bool PixInsightINDIInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const PixInsightINDIInstance* r = dynamic_cast<const PixInsightINDIInstance*>( &p );
   if ( r == 0 )
   {
      whyNot = "Not a PixInsightINDI instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool PixInsightINDIInterface::RequiresInstanceValidation() const
{
   return true;
}

bool PixInsightINDIInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void PixInsightINDIInterface::UpdateControls()
{
   GUI->ParameterPort_SpinBox.SetValue( instance.p_port );
   GUI->ParameterHost_Edit.SetText( instance.p_host );
}

// ----------------------------------------------------------------------------


void PixInsightINDIInterface::__IntegerValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->ParameterPort_SpinBox )
      instance.p_port = value;
}


void PixInsightINDIInterface::__EditGetFocus( Control& sender )
{
   if ( sender == GUI->ParameterHost_Edit )
   {
      // If you need to do something when sender gets focus, do it here.
   }
}

void PixInsightINDIInterface::__EditCompleted( Edit& sender )
{
   if ( sender == GUI->ParameterHost_Edit )
      instance.p_host = sender.Text();
}

void PixInsightINDIInterface::Buttons_Click( Button& sender, bool checked )
{
        
        if ( sender == GUI->ConnectServer_PushButton )
        {
            try
            {
				if (indiClient.get() == 0)
					indiClient.reset(new INDIClient(&instance));

				if (indiClient.get()->serverIsConnected())
					return;

				IsoString ASCIIHost(instance.p_host);
				indiClient.get()->setServer(ASCIIHost.c_str() , instance.p_port);

				bool connected=false;

				connected = indiClient.get()->connectServer();

				if (connected){
					GUI->UpdateDeviceList_Timer.Start();
					GUI->UpdateServerMessage_Timer.Start();
					GUI->UpdatePropertyList_Timer.Start();
				}
				else{
					GUI->ServerMessage_Label.SetText("Connection to INDI server failed. Please check server host and port.");
				}
            }
            ERROR_HANDLER
        }
		else if ( sender == GUI->DisconnectServer_PushButton )
		{
			try
            {
                if (indiClient.get() == 0)
					indiClient.reset(new INDIClient(&instance));


				if (indiClient.get()->serverIsConnected())
					indiClient.get()->disconnectServer();
				else
					return;


				GUI->UpdateDeviceList_Timer.Stop();
				GUI->UpdatePropertyList_Timer.Stop();
				GUI->UpdateServerMessage_Timer.Stop();

				// clear node maps
				m_rootNodeMap.clear();
				m_deviceNodeMap.clear();
				m_deviceRootNodeMap.clear();

				for (PropertyTreeMapType::iterator iter=m_propertyTreeMap.begin();iter!=m_propertyTreeMap.end(); iter++ ){
					delete iter->second;
				}

				m_propertyTreeMap.clear();
				// flag property list items as Insert to create new property tree


				for (PixInsightINDIInstance::PropertyListType::iterator iter=instance.p_propertyList.Begin() ; iter!=instance.p_propertyList.End(); ++iter){
					iter->PropertyFlag=Insert;
				}

				GUI->DeviceList_TreeBox.Clear();
				// clear property list
				GUI->PropertyList_TreeBox.Clear();

				if (!indiClient.get()->serverIsConnected()) {
					GUI->ServerMessage_Label.SetText("Successfully disconnected from server");
				}

            }
            ERROR_HANDLER
		}
		else if ( sender == GUI->ConnectDevice_PushButton)
		{
			try
            {
				pcl::IndirectArray<pcl::TreeBox::Node> selectedNodes;
				GUI->DeviceList_TreeBox.GetSelectedNodes(selectedNodes);

				for (pcl::IndirectArray<pcl::TreeBox::Node>::iterator it=selectedNodes.Begin(); it!=selectedNodes.End();++it){
					IsoString deviceName((*it)->Text(TextColumn).To7BitASCII());
					
					INDI::BaseDevice* device = indiClient.get()->getDevice(deviceName.c_str());

					if (device)
						if (!device->isConnected())
							indiClient.get()->connectDevice(deviceName.c_str());
				}
			}
            ERROR_HANDLER
		}
		else if ( sender == GUI->DisconnectDevice_PushButton)
		{
			try
			{
				pcl::IndirectArray<pcl::TreeBox::Node> selectedNodes;
				GUI->DeviceList_TreeBox.GetSelectedNodes(selectedNodes);

				for (pcl::IndirectArray<pcl::TreeBox::Node>::iterator it=selectedNodes.Begin(); it!=selectedNodes.End();++it){
					IsoString deviceName((*it)->Text(TextColumn).To7BitASCII());

					INDI::BaseDevice* device = indiClient.get()->getDevice(deviceName.c_str());

					if (device)
						if (device->isConnected())
							indiClient.get()->disconnectDevice(deviceName.c_str());

					(*it)->Enable();
				}
			}
            ERROR_HANDLER
		}
		else if ( sender == GUI->WatchDevice_PushButton)
		{
			try
			{
				pcl::IndirectArray<pcl::TreeBox::Node> selectedNodes;
				GUI->DeviceList_TreeBox.GetSelectedNodes(selectedNodes);

				for (pcl::IndirectArray<pcl::TreeBox::Node>::iterator it=selectedNodes.Begin(); it!=selectedNodes.End();++it) {
					IsoString deviceName((*it)->Text(TextColumn).To7BitASCII());

					indiClient.get()->watchDevice(deviceName.c_str());
				}
				MessageBox mBox(String("Please reconnect to the INDI server to activate watching devices. Press first the \"Disconnect\" and then the \"Connect\" button in the server connection section."),
								String("Reconnect to INDI server"),StdIcon::Information);
				mBox.Execute();
			}
			ERROR_HANDLER
		}
}


void PixInsightINDIInterface::UpdateDeviceList(){


	GUI->DeviceList_TreeBox.DisableUpdates();

	if (indiClient.get() == 0)
		return;

	if (instance.p_deviceList.Begin()==instance.p_deviceList.End())
		return;

	for (PixInsightINDIInstance::DeviceListType::iterator iter=instance.p_deviceList.Begin() ; iter!=instance.p_deviceList.End(); ++iter){

		PropertyNode* rootNode=NULL;
		PropertyNodeMapType::iterator rootNodeIter = m_deviceRootNodeMap.find(iter->DeviceName);
		if (rootNodeIter== m_deviceRootNodeMap.end()){
			rootNode=new PropertyNode(GUI->DeviceList_TreeBox);
			m_deviceRootNodeMap[iter->DeviceName]=rootNode;
		} else {
			rootNode = rootNodeIter->second;
		}
		assert(rootNode!=NULL);

		PropertyNode* deviceNode = NULL;
		PropertyNodeMapType::iterator deviceNodeIter = m_deviceNodeMap.find(iter->DeviceName);
		if (deviceNodeIter== m_deviceNodeMap.end()){
			deviceNode=new PropertyNode(rootNode,IsoString(iter->DeviceName));
			m_deviceNodeMap[iter->DeviceName]=deviceNode;
		} else {
			deviceNode = deviceNodeIter->second;
		}
		assert(deviceNode!=NULL);


		deviceNode->getTreeBoxNode()->SetText( TextColumn, iter->DeviceName );
		deviceNode->getTreeBoxNode()->SetAlignment( TextColumn, TextAlign::Left );

		INDI::BaseDevice* device = indiClient.get()->getDevice(IsoString(iter->DeviceName).c_str());
		if (device){
			if (device->isConnected()){
				Bitmap icon(String(":/bullets/bullet-ball-glass-green.png"));
				deviceNode->getTreeBoxNode()->SetIcon(TextColumn,icon);
			} else {
				Bitmap icon(":/bullets/bullet-ball-glass-grey.png");
				deviceNode->getTreeBoxNode()->SetIcon(TextColumn,icon);
			}

			if (indiClient.get()->isWatched(IsoString(iter->DeviceName).c_str())){
				pcl::Font currentFont = deviceNode->getTreeBoxNode()->Font(TextColumn);
				currentFont.SetBold();
				deviceNode->getTreeBoxNode()->SetFont(TextColumn,currentFont);
			}
		}



	}
	GUI->DeviceList_TreeBox.EnableUpdates();

}
// ----------------------------------------------------------------------------

PixInsightINDIInterface::GUIData::GUIData( PixInsightINDIInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Three:" ) ); // the longest label text
   int editWidth1 = fnt.Width( String( '0',14 ) );

   INDIServer_SectionBar.SetTitle("INDI Server Connection");
   INDIServer_SectionBar.SetSection(INDIServerConnection_Control);
   INDIServerConnection_Control.SetSizer(ParameterHost_Sizer);
   ParameterPort_Label.SetText( "Port:" );
   ParameterPort_Label.SetMinWidth( labelWidth1 );
   ParameterPort_Label.SetToolTip( "<p>INDI server port.</p>" );
   ParameterPort_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ParameterPort_SpinBox.SetRange( int( TheINDIServerPort->MinimumValue() ), int( TheINDIServerPort->MaximumValue() ) );
   ParameterPort_SpinBox.SetFixedWidth( editWidth1 );
   ParameterPort_SpinBox.SetToolTip( "<p>INDI server port.</p>" );
   ParameterPort_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PixInsightINDIInterface::__IntegerValueUpdated, w );
   
   ParameterHost_Label.SetText( "Host:" );
   ParameterHost_Label.SetMinWidth( labelWidth1 );
   ParameterHost_Label.SetToolTip( "<p>INDI server host.</p>" );
   ParameterHost_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ParameterHost_Edit.SetMinWidth( editWidth1 );
   ParameterHost_Edit.OnGetFocus( (Control::event_handler)&PixInsightINDIInterface::__EditGetFocus, w );
   ParameterHost_Edit.OnEditCompleted( (Edit::edit_event_handler)&PixInsightINDIInterface::__EditCompleted, w );
   
   ParameterPort_Label.SetText( "Port:" );
   ParameterPort_Label.SetMinWidth( labelWidth1 );
   ParameterPort_Label.SetToolTip( "<p>INDI server port.</p>" );
   ParameterPort_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ParameterPort_SpinBox.SetRange( int( TheINDIServerPort->MinimumValue() ), int( TheINDIServerPort->MaximumValue() ) );
   ParameterPort_SpinBox.SetFixedWidth( editWidth1 );
   ParameterPort_SpinBox.SetToolTip( "<p>INDI server port.</p>" );
   ParameterPort_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PixInsightINDIInterface::__IntegerValueUpdated, w );

   ParameterHost_Sizer.SetSpacing(4);
   ParameterHost_Sizer.Add(ServerData_VSizer );
   ParameterHost_Sizer.Add(ConnectionServer_Sizer );
   ServerData_Sizer.SetSpacing( 4 );
   ServerData_Sizer.Add( ParameterHost_Label );
   ServerData_Sizer.Add( ParameterHost_Edit, 100);
   ServerData_Sizer.Add( ParameterPort_Label );
   ServerData_Sizer.Add( ParameterPort_SpinBox );
   ServerData_VSizer.SetSpacing( 4 );
   ServerData_VSizer.Add(ServerData_Sizer);
   ServerData_VSizer.AddStretch();
   
   //pushbuttons
   ConnectionServer_Sizer.SetSpacing(4);
   ConnectServer_PushButton.SetText( "Connect" );
   ConnectServer_PushButton.OnClick( (Button::click_event_handler) &PixInsightINDIInterface::Buttons_Click, w );
   DisconnectServer_PushButton.SetText( "Disconnect" );
   DisconnectServer_PushButton.OnClick( (Button::click_event_handler) &PixInsightINDIInterface::Buttons_Click, w );

  
   ConnectionServer_Sizer.Add(ConnectServer_PushButton);
   ConnectionServer_Sizer.Add(DisconnectServer_PushButton);
   ConnectionServer_Sizer.AddStretch();

   INDIServerConnection_Control.SetFixedHeight(4*fnt.Height() +2);



   // INDI devices
   INDIDevices_SectionBar.SetTitle("INDI Devices");
   INDIDevices_SectionBar.SetSection(INDIDevices_Control);
   INDIDevices_Control.SetSizer(INDIDevice_Sizer);
   INDIDevices_Control.SetFixedHeight( 8*fnt.Height() +2 );
   DeviceList_TreeBox.EnableAlternateRowColor();
   DeviceList_TreeBox.EnableMultipleSelections();
   DeviceList_TreeBox.SetNumberOfColumns(1);
   DeviceList_TreeBox.SetHeaderText(TextColumn,"Device");


   ConnectDevice_PushButton.SetText("Connect");
   ConnectDevice_PushButton.OnClick((Button::click_event_handler) &PixInsightINDIInterface::Buttons_Click, w );
   DisconnectDevice_PushButton.SetText("Disconnect");
   DisconnectDevice_PushButton.OnClick((Button::click_event_handler) &PixInsightINDIInterface::Buttons_Click, w );
   WatchDevice_PushButton.SetText("Watch");
   WatchDevice_PushButton.OnClick((Button::click_event_handler) &PixInsightINDIInterface::Buttons_Click, w );

   DeviceAction_Sizer.SetSpacing(4);
   DeviceAction_Sizer.Add(ConnectDevice_PushButton);
   DeviceAction_Sizer.Add(DisconnectDevice_PushButton);
   DeviceAction_Sizer.Add(WatchDevice_PushButton);
   DeviceAction_Sizer.AddStretch();

   INDIDevice_Sizer.Add(DeviceList_TreeBox);
   INDIDevice_Sizer.Add(DeviceAction_Sizer);

   INDIProperties_SectionBar.SetTitle("INDI Device Properties");
   INDIProperties_SectionBar.SetSection(INDIProperties_Control);
   INDIProperties_Control.SetSizer(INDIDeviceProperty_Sizer);

   PropertyList_TreeBox.EnableAlternateRowColor();
   PropertyList_TreeBox.SetNumberOfColumns(5);
   PropertyList_TreeBox.HideColumn(TypeColumn);
   PropertyList_TreeBox.HideColumn(NumberFormatColumn);
   PropertyList_TreeBox.SetColumnWidth(0,300);
   PropertyList_TreeBox.SetHeaderText(TextColumn,String("Property"));
   PropertyList_TreeBox.SetHeaderText(ValueColumn,String("Value"));
   PropertyList_TreeBox.SetHeaderText(LabelColumn,String("Label"));
   PropertyList_TreeBox.OnClose((Control::close_event_handler) &PixInsightINDIInterface::__Close,w);

   ServerMessage_Label.SetVariableWidth();
   ServerMessage_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   ServerMessageLabel_Label.SetText("Last server message:");
   ServerMessageLabel_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   ServerMessage_Label.SetTextAlignment(TextAlign::Left|TextAlign::VertCenter);
   ServerMessage_Label.SetMinWidth(fnt.Width(String("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx")));
   ServerMessage_Label.EnableWordWrapping();

   ServerMessage_Sizer.Add(ServerMessageLabel_Label);
   ServerMessage_Sizer.Add(ServerMessage_Label);
   ServerMessage_Sizer.AddStretch();

   EditProperty_PushButton.SetText("Edit");
   EditProperty_PushButton.OnClick((Button::click_event_handler) &PixInsightINDIInterface::PropertyButton_Click, w );

   INDIDeviceProperty_Sizer.Add(PropertyList_TreeBox);
   
   Buttons_Sizer.SetSpacing(4);
   Buttons_Sizer.Add(EditProperty_PushButton);
   Buttons_Sizer.AddStretch();

   INDIDeviceProperty_Sizer.Add(Buttons_Sizer);

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add(INDIServer_SectionBar);	
   Global_Sizer.Add(INDIServerConnection_Control);
   Global_Sizer.Add(INDIDevices_SectionBar);
   Global_Sizer.Add(INDIDevices_Control);
   Global_Sizer.Add(INDIProperties_SectionBar);
   Global_Sizer.Add(INDIProperties_Control);
   Global_Sizer.Add(ServerMessage_Sizer);

   UpdateDeviceList_Timer.SetInterval( 0.5 );
   UpdateDeviceList_Timer.SetPeriodic( true );
   UpdateDeviceList_Timer.OnTimer( (Timer::timer_event_handler)&PixInsightINDIInterface::__UpdateDeviceList_Timer, w );

   UpdatePropertyList_Timer.SetInterval( 0.5 );
   UpdatePropertyList_Timer.SetPeriodic( true );
   UpdatePropertyList_Timer.OnTimer( (Timer::timer_event_handler)&PixInsightINDIInterface::__UpdatePropertyList_Timer, w );


   UpdateServerMessage_Timer.SetInterval( 0.5 );
   UpdateServerMessage_Timer.SetPeriodic( true );
   UpdateServerMessage_Timer.OnTimer( (Timer::timer_event_handler)&PixInsightINDIInterface::__UpdateServerMessage_Timer, w );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();

}


void PixInsightINDIInterface::__UpdateDeviceList_Timer( Timer &sender )
  {
	  
	  if( sender == GUI->UpdateDeviceList_Timer  ){
		  UpdateDeviceList();
	  }
	  
  }

void PixInsightINDIInterface::__UpdatePropertyList_Timer( Timer &sender )
  {

	  if( sender == GUI->UpdatePropertyList_Timer  ){
		  UpdatePropertyList();
	  }

  }

void PixInsightINDIInterface::__UpdateServerMessage_Timer( Timer &sender )
  {

	  if( sender == GUI->UpdateServerMessage_Timer  ){
		  GUI->ServerMessage_Label.SetText(instance.p_currentMessage);
	  }


  }
// ----------------------------------------------------------------------------



void SetPropertyDialog::Ok_Button_Click( Button& sender, bool checked ){

	if (indiClient.get() == 0)
		return;

	assert(m_instance!=NULL);

	INDINewPropertyListItem newPropertyListItem=getNewPropertyListItem();
	bool send_ok = m_instance->sendNewPropertyValue(newPropertyListItem,true /*isAsynch*/);
	if (!send_ok) {
		m_instance->clearNewPropertyList();
	}
	Ok();
}

void SetPropertyDialog::Cancel_Button_Click( Button& sender, bool checked ){
		Cancel();
}

SetPropertyDialog* SetPropertyDialog::createPropertyDialog(IsoString indiType, IsoString numberFormat,PixInsightINDIInstance* indiInstance){
	if (indiType==IsoString("INDI_NUMBER") ){
		size_t im = numberFormat.Find('m');
		if (im!=IsoString::notFound){
			return new EditNumberCoordPropertyDialog(indiInstance);
		}
	}
	if (indiType==IsoString("INDI_SWITCH") ){
		return new EditSwitchPropertyDialog(indiInstance);
	}

	return new EditPropertyDialog(indiInstance);
}

void EditPropertyDialog::EditCompleted( Edit& sender )
{
	m_newPropertyListItem.NewPropertyValue=sender.Text();
}

EditPropertyDialog::EditPropertyDialog(PixInsightINDIInstance* indiInstance):SetPropertyDialog(indiInstance){
	pcl::Font fnt = Font();
	int labelWidth = fnt.Width( String( '0',30 ) );
	int editWidth = fnt.Width( String( '0',10 ) );

	SetWindowTitle(String("INDI number property value"));

	Property_Label.SetMinWidth(labelWidth);
	Property_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

	Property_Edit.SetMinWidth(editWidth);
	Property_Edit.OnEditCompleted( (Edit::edit_event_handler)&EditPropertyDialog::EditCompleted, *this );
   
	OK_PushButton.SetText("OK");
	OK_PushButton.OnClick((Button::click_event_handler) &SetPropertyDialog::Ok_Button_Click, *this );
	Cancel_PushButton.SetText("Cancel");
	Cancel_PushButton.OnClick((Button::click_event_handler) &SetPropertyDialog::Cancel_Button_Click, *this );

	Property_Sizer.SetMargin(10);
	Property_Sizer.SetSpacing(4);
	Property_Sizer.Add(Property_Label);
	Property_Sizer.Add(Property_Edit);
	Property_Sizer.AddStretch();
	
	
	Buttons_Sizer.SetSpacing(4);
	Buttons_Sizer.AddSpacing(10);
	Buttons_Sizer.AddStretch();
	Buttons_Sizer.Add(OK_PushButton);
	Buttons_Sizer.AddStretch();
	Buttons_Sizer.Add(Cancel_PushButton);
	Buttons_Sizer.AddStretch();

	Global_Sizer.Add(Property_Sizer);
	Global_Sizer.Add(Buttons_Sizer);

	SetSizer(Global_Sizer);

}


void EditNumberCoordPropertyDialog::setPropertyValueString(String value){
	StringList tokens;
	value.Break(tokens,':',true);
	assert(tokens.Length()==3);
	tokens[0].TrimLeft();
	Hour_Edit.SetText(tokens[0]);
	Minute_Edit.SetText(tokens[1]);
	Second_Edit.SetText(tokens[2]);
}

void EditNumberCoordPropertyDialog::EditCompleted( Edit& sender )
{
	if (sender==Hour_Edit){
		m_hour=sender.Text().ToDouble();
		m_minute=Minute_Edit.Text().ToDouble();
		m_second=Second_Edit.Text().ToDouble();
	}
	if (sender==Minute_Edit){
		m_hour=Hour_Edit.Text().ToDouble();
		m_minute=sender.Text().ToDouble();
		m_second=Second_Edit.Text().ToDouble();
	}
	if (sender==Second_Edit){
		m_hour=Hour_Edit.Text().ToDouble();
		m_minute=Minute_Edit.Text().ToDouble();
		m_second=sender.Text().ToDouble();
	}
	double coord=m_hour + m_minute / 60 + m_second / 3600;
	m_newPropertyListItem.NewPropertyValue=String(coord) ;
}

EditNumberCoordPropertyDialog::EditNumberCoordPropertyDialog(PixInsightINDIInstance* indiInstance):SetPropertyDialog(indiInstance),m_hour(0),m_minute(0),m_second(0){
	pcl::Font fnt = Font();
	int labelWidth = fnt.Width( String( '0',20 ) );
	int editWidth = fnt.Width( String( '0',4 ) );


	SetWindowTitle(String("INDI coordinate property value"));

	Property_Label.SetMinWidth(labelWidth);
	Property_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

	Hour_Edit.SetMaxWidth(editWidth);
	Hour_Edit.OnEditCompleted( (Edit::edit_event_handler)&EditNumberCoordPropertyDialog::EditCompleted, *this );

	Colon1_Label.SetText(":");

	Minute_Edit.SetMaxWidth(editWidth);
	Minute_Edit.OnEditCompleted( (Edit::edit_event_handler)&EditNumberCoordPropertyDialog::EditCompleted, *this );

	Colon2_Label.SetText(":");

	Second_Edit.SetMaxWidth(editWidth);
	Second_Edit.OnEditCompleted( (Edit::edit_event_handler)&EditNumberCoordPropertyDialog::EditCompleted, *this );


	OK_PushButton.SetText("OK");
	OK_PushButton.OnClick((Button::click_event_handler) &SetPropertyDialog::Ok_Button_Click, *this );
	Cancel_PushButton.SetText("Cancel");
	Cancel_PushButton.OnClick((Button::click_event_handler) &SetPropertyDialog::Cancel_Button_Click, *this );

	Property_Sizer.SetMargin(10);
	Property_Sizer.SetSpacing(4);
	Property_Sizer.Add(Property_Label);
	Property_Sizer.Add(Hour_Edit);
	Property_Sizer.Add(Colon1_Label);
	Property_Sizer.Add(Minute_Edit);
	Property_Sizer.Add(Colon2_Label);
	Property_Sizer.Add(Second_Edit);
	Property_Sizer.AddStretch();


	Buttons_Sizer.SetSpacing(4);
	Buttons_Sizer.AddSpacing(10);
	Buttons_Sizer.AddStretch();
	Buttons_Sizer.Add(OK_PushButton);
	Buttons_Sizer.AddStretch();
	Buttons_Sizer.Add(Cancel_PushButton);
	Buttons_Sizer.AddStretch();

	Global_Sizer.Add(Property_Sizer);
	Global_Sizer.Add(Buttons_Sizer);

	SetSizer(Global_Sizer);

}

void EditSwitchPropertyDialog::setPropertyValueString(String value){
	if (value==String("ON")){
		ON_RadioButton.Check();
		OFF_RadioButton.Uncheck();
	}
	else if (value==String("OFF")) {
		ON_RadioButton.Uncheck();
		OFF_RadioButton.Check();
	} else {
		throw Error("Invalid Switch value.");
	}
}

void EditSwitchPropertyDialog::ButtonChecked( RadioButton& sender )
{
	if (sender==ON_RadioButton){
		m_newPropertyListItem.NewPropertyValue=String("ON") ;
	}

	if (sender==OFF_RadioButton){
		m_newPropertyListItem.NewPropertyValue=String("OFF") ;
	}

}

EditSwitchPropertyDialog::EditSwitchPropertyDialog(PixInsightINDIInstance* indiInstance):SetPropertyDialog(indiInstance){
	pcl::Font fnt = Font();
	int labelWidth = fnt.Width( String( '0',20 ) );

	SetWindowTitle(String("INDI switch property value"));

	Property_Label.SetMinWidth(labelWidth);
	Property_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

	ON_Label.SetText("ON");
	OFF_Label.SetText("OFF");

	//ON_RadioButton.Uncheck();
	ON_RadioButton.OnCheck((RadioButton::check_event_handler) &EditSwitchPropertyDialog::ButtonChecked,*this);


	//OFF_RadioButton.Check();
	//OFF_RadioButton.Disable(true);
	OFF_RadioButton.OnCheck((RadioButton::check_event_handler) &EditSwitchPropertyDialog::ButtonChecked,*this);

	OK_PushButton.SetText("OK");
	OK_PushButton.OnClick((Button::click_event_handler) &SetPropertyDialog::Ok_Button_Click, *this );
	Cancel_PushButton.SetText("Cancel");
	Cancel_PushButton.OnClick((Button::click_event_handler) &SetPropertyDialog::Cancel_Button_Click, *this );

	Property_Sizer.SetMargin(10);
	Property_Sizer.SetSpacing(4);
	Property_Sizer.Add(Property_Label);
	Property_Sizer.Add(ON_Label);
	Property_Sizer.Add(ON_RadioButton);
	Property_Sizer.Add(OFF_Label);
	Property_Sizer.Add(OFF_RadioButton);
	Property_Sizer.AddStretch();


	Buttons_Sizer.SetSpacing(4);
	Buttons_Sizer.AddSpacing(10);
	Buttons_Sizer.AddStretch();
	Buttons_Sizer.Add(OK_PushButton);
	Buttons_Sizer.AddStretch();
	Buttons_Sizer.Add(Cancel_PushButton);
	Buttons_Sizer.AddStretch();

	Global_Sizer.Add(Property_Sizer);
	Global_Sizer.Add(Buttons_Sizer);

	SetSizer(Global_Sizer);

}



void PixInsightINDIInterface::UpdatePropertyList(){
	GUI->PropertyList_TreeBox.DisableUpdates();
	std::vector<INDIPropertyListItem> itemsToBeRemoved;
	std::vector<INDIPropertyListItem> itemsCreated;

	if (indiClient.get() == 0){
		GUI->PropertyList_TreeBox.EnableUpdates();
		return;
	}

	if (instance.p_propertyList.Begin()==instance.p_propertyList.End()){
		GUI->PropertyList_TreeBox.EnableUpdates();
		return;
	}
	PropertyNodeFactory factory;
	for (PixInsightINDIInstance::PropertyListType::iterator iter=instance.p_propertyList.Begin() ; iter!=instance.p_propertyList.End(); ++iter){

		if (iter->PropertyFlag==Idle){
			continue;
		}

		PropertyNode* rootNode=NULL;
		PropertyNodeMapType::iterator rootNodeIter = m_rootNodeMap.find(iter->Device);
		if (rootNodeIter== m_rootNodeMap.end()){
			rootNode=new PropertyNode(GUI->PropertyList_TreeBox);
			m_rootNodeMap[iter->Device]=rootNode;
		} else {
			rootNode = rootNodeIter->second;
		}
		assert(rootNode!=NULL);

		PropertyTree*  propTree = NULL;
		PropertyTreeMapType::iterator propTreeIter = m_propertyTreeMap.find(iter->Device);
		if (propTreeIter==m_propertyTreeMap.end()){
			propTree = new PropertyTree(rootNode,&factory);
		} else {
			propTree = propTreeIter->second;
		}
		assert(propTree!=NULL);

		if (iter->PropertyFlag==Remove){
			FindNodeVisitor* findDeviceNodeVisitor = new FindNodeVisitor();
			rootNode->accept(findDeviceNodeVisitor,PropertyUtils::getKey(iter->Device),IsoString(""));
			FindNodeVisitor* findPropNodeVisitor = new FindNodeVisitor();
			rootNode->accept(findPropNodeVisitor,PropertyUtils::getKey(iter->Device,iter->Property),IsoString(""));
			if (findDeviceNodeVisitor->foundNode()&&findPropNodeVisitor->foundNode()){
				findDeviceNodeVisitor->getNode()->RemoveChild(findPropNodeVisitor->getNode());
				delete findPropNodeVisitor->getNode();
			}
			itemsToBeRemoved.push_back(*iter);
		} else  {
			PropertyNode* elemNode = propTree->addElementNode(iter->Device,iter->Property,iter->Element,iter->PropertyState,iter->PropertyLabel);
			elemNode->setNodeINDIType(iter->PropertyTypeStr);
			elemNode->setNodeINDINumberFormat(iter->PropertyNumberFormat);
			if (iter->PropertyTypeStr==String("INDI_NUMBER")){
				elemNode->setNodeINDIValue(PropertyUtils::getFormattedNumber(iter->PropertyValue,iter->PropertyNumberFormat));
			} else {
				elemNode->setNodeINDIValue(iter->PropertyValue);
			}
			elemNode->setNodeINDILabel(iter->ElementLabel);
			elemNode->getTreeBoxNode()->SetAlignment(TextColumn, TextAlign::Left);
			elemNode->getTreeBoxNode()->SetAlignment(ValueColumn, TextAlign::Left);
			elemNode->getTreeBoxNode()->SetAlignment(LabelColumn, TextAlign::Left);
			if (iter->PropertyFlag==Insert){
				itemsCreated.push_back(*iter);
			}
		}

	}
	GUI->PropertyList_TreeBox.EnableUpdates();

	// remove properties from property list
	for (size_t i=0; i<itemsToBeRemoved.size(); i++){
		PixInsightINDIInstance::PropertyListType::iterator iter = instance.p_propertyList.Search(itemsToBeRemoved[i]);
		if (iter==instance.p_propertyList.End()){
			IsoString elemStr=IsoString(iter->Element);
			throw Error(String().Format("%s",elemStr.c_str()));
		}
		instance.p_propertyList.Remove(iter);

	}

	// set newly created elements to Idle
	for (size_t i=0; i<itemsCreated.size(); i++){
		PixInsightINDIInstance::PropertyListType::iterator iter =instance.p_propertyList.Search(itemsCreated[i]);
		if (iter!=instance.p_propertyList.End()){
			iter->PropertyFlag=Idle;
		}
	}
}


void PixInsightINDIInterface::__Close( Control& sender, bool& allowClose){

}

void PixInsightINDIInterface::PropertyButton_Click( Button& sender, bool checked ){
	
	if (sender==GUI->EditProperty_PushButton){
		pcl::IndirectArray<pcl::TreeBox::Node> selectedNodes;
		selectedNodes = GUI->PropertyList_TreeBox.SelectedNodes();

		if (selectedNodes.Begin()!=selectedNodes.End())
		{

			pcl::IndirectArray<pcl::TreeBox::Node>::iterator it=selectedNodes.Begin();

			// check that selected node is an element node, i.e. has two parents
			if ((*it)->Parent()==NULL || (*it)->Parent()->Parent()==NULL){
				throw Error("Please select an property element node (leaf of the tree)");
			}

			// Create dialog window according to the property type
			GUI->SetPropDlg = SetPropertyDialog::createPropertyDialog((*it)->Text(TypeColumn),(*it)->Text(NumberFormatColumn), &instance);
			CHECK_POINTER(GUI->SetPropDlg);

			GUI->SetPropDlg->setPropertyLabelString((*it)->Text(LabelColumn));
			GUI->SetPropDlg->setPropertyValueString((*it)->Text(ValueColumn));
			INDINewPropertyListItem propItem;
			propItem.Device=(*it)->Parent()->Parent()->Text(TextColumn);
			propItem.Property=(*it)->Parent()->Text(TextColumn);
			propItem.Element=(*it)->Text(TextColumn);
			propItem.NewPropertyValue=(*it)->Text(ValueColumn);
			propItem.PropertyType=(*it)->Text(TypeColumn);
			propItem.PropertyKey=PropertyUtils::getKey(propItem.Device,propItem.Property,propItem.Element);
			GUI->SetPropDlg->setNewPropertyListItem(propItem);

		}

		GUI->SetPropDlg->Execute();
	}

}

} // pcl

// ****************************************************************************
// EOF PixInsightINDIInterface.cpp - Released 2013/03/24 18:42:27 UTC
