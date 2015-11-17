//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.02.0096
// ----------------------------------------------------------------------------
// INDIDeviceControllerInterface.cpp - Released 2015/10/13 15:55:45 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2015 Klaus Kretzschmar
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

#include "INDIClient.h"
#include "INDIDeviceControllerInterface.h"
#include "INDIParameters.h"
#include "INDIDeviceControllerProcess.h"
#include "PropertyNode.h"

#include "INDI/basedevice.h"
#include "INDI/indiapi.h"
#include "INDI/indibase.h"

#include <pcl/Console.h>

#include <assert.h>

#define LIST_MINHEIGHT( fnt ) RoundInt( 8.125*fnt.Height() )

namespace pcl
{

// ----------------------------------------------------------------------------

INDIDeviceControllerInterface* TheINDIDeviceControllerInterface = nullptr;

// ----------------------------------------------------------------------------

//#include "PixInsightINDIIcon.xpm"// ### TODO

// ----------------------------------------------------------------------------

INDIDeviceControllerInterface::INDIDeviceControllerInterface() :
   ProcessInterface(),
   instance( TheINDIDeviceControllerProcess ),
   GUI( nullptr ),
   m_serverMessage()
{
   TheINDIDeviceControllerInterface = this;
}

INDIDeviceControllerInterface::~INDIDeviceControllerInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;

   if (m_propertyTreeMap.size()!=0){
	   for (PropertyTreeMapType::iterator iter=m_propertyTreeMap.begin();iter!=m_propertyTreeMap.end(); iter++ ){
		   delete iter->second;
	   }
   }
}

IsoString INDIDeviceControllerInterface::Id() const
{
   return "INDIDeviceController";
}

MetaProcess* INDIDeviceControllerInterface::Process() const
{
   return TheINDIDeviceControllerProcess;
}

const char** INDIDeviceControllerInterface::IconImageXPM() const
{
   return 0; // PixInsightINDIIcon_XPM; // ### TODO
}

InterfaceFeatures INDIDeviceControllerInterface::Features() const
{
	return   InterfaceFeature::DragObject | InterfaceFeature::ApplyGlobalButton | InterfaceFeature::ApplyToViewButton | InterfaceFeature::BrowseDocumentationButton | InterfaceFeature::ResetButton;
}

void INDIDeviceControllerInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void INDIDeviceControllerInterface::ApplyInstanceGlobal() const
{
   instance.LaunchGlobal();
}

void INDIDeviceControllerInterface::ResetInstance()
{
   INDIDeviceControllerInstance defaultInstance( TheINDIDeviceControllerProcess );
   ImportProcess( defaultInstance );
}

bool INDIDeviceControllerInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "INDI Device Controller" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheINDIDeviceControllerProcess;
}

ProcessImplementation* INDIDeviceControllerInterface::NewProcess() const
{
   return new INDIDeviceControllerInstance( instance );
}

bool INDIDeviceControllerInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const INDIDeviceControllerInstance* r = dynamic_cast<const INDIDeviceControllerInstance*>( &p );
   if ( r == 0 )
   {
      whyNot = "Not an INDIDeviceController instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool INDIDeviceControllerInterface::RequiresInstanceValidation() const
{
   return true;
}

bool INDIDeviceControllerInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void INDIDeviceControllerInterface::UpdateControls()
{
   GUI->ParameterPort_SpinBox.SetValue( instance.p_port );
   GUI->ParameterHost_Edit.SetText( instance.p_host );
}

// ----------------------------------------------------------------------------


void INDIDeviceControllerInterface::__IntegerValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->ParameterPort_SpinBox )
      instance.p_port = value;
}


void INDIDeviceControllerInterface::__EditGetFocus( Control& sender )
{
   if ( sender == GUI->ParameterHost_Edit )
   {
      // If you need to do something when sender gets focus, do it here.
   }
}

void INDIDeviceControllerInterface::__EditCompleted( Edit& sender )
{
   if ( sender == GUI->ParameterHost_Edit )
      instance.p_host = sender.Text();
}

void INDIDeviceControllerInterface::__CameraListButtons_Click( Button& sender, bool checked )
{

        if ( sender == GUI->ConnectServer_PushButton )
        {
            try
            {
				if (indiClient.get() == 0)
					indiClient.reset(new INDIClient(&instance));

				IsoString ASCIIHost(instance.p_host);
				indiClient->setServer(ASCIIHost.c_str() , instance.p_port);

				bool connected=false;

				connected = indiClient->connectServer();

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

				if (indiClient->serverIsConnected())
					indiClient->disconnectServer();


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

				for (INDIDeviceControllerInstance::PropertyListType::iterator iter=instance.p_propertyList.Begin() ; iter!=instance.p_propertyList.End(); ++iter){
					iter->PropertyFlag=Insert;
				}

				GUI->DeviceList_TreeBox.Clear();
				// clear property list
				GUI->PropertyList_TreeBox.Clear();


				if (!indiClient->serverIsConnected()) {
					GUI->ServerMessage_Label.SetText("Successfully disconnected from server");
				}

            }
            ERROR_HANDLER
		}
		else if ( sender == GUI->ConnectDevice_PushButton)
		{
			try
            {
				pcl::IndirectArray<pcl::TreeBox::Node> selectedNodes = GUI->DeviceList_TreeBox.SelectedNodes();

				for (pcl::IndirectArray<pcl::TreeBox::Node>::iterator it=selectedNodes.Begin(); it!=selectedNodes.End();++it){
					IsoString deviceName((*it)->Text(TextColumn).To7BitASCII());

					INDI::BaseDevice* device = indiClient->getDevice(deviceName.c_str());

					if (device)
						if (!device->isConnected())
							indiClient->connectDevice(deviceName.c_str());
				}
			}
            ERROR_HANDLER
		}
		else if ( sender == GUI->DisconnectDevice_PushButton)
		{
			try
			{
				pcl::IndirectArray<pcl::TreeBox::Node> selectedNodes =  GUI->DeviceList_TreeBox.SelectedNodes();

				for (pcl::IndirectArray<pcl::TreeBox::Node>::iterator it=selectedNodes.Begin(); it!=selectedNodes.End();++it){
					IsoString deviceName((*it)->Text(TextColumn).To7BitASCII());

					INDI::BaseDevice* device = indiClient->getDevice(deviceName.c_str());

					if (device)
						if (device->isConnected())
							indiClient->disconnectDevice(deviceName.c_str());

					(*it)->Enable();
				}
			}
            ERROR_HANDLER
		}
		else if ( sender == GUI->RefreshDevice_PushButton)
		{
			try
            {
				UpdateDeviceList();
			}
            ERROR_HANDLER
		}
}


void INDIDeviceControllerInterface::UpdateDeviceList(){


	GUI->DeviceList_TreeBox.DisableUpdates();

	if (indiClient.get() == 0)
		return;

	if (instance.p_deviceList.Begin()==instance.p_deviceList.End())
		return;

	for (INDIDeviceControllerInstance::DeviceListType::iterator iter=instance.p_deviceList.Begin() ; iter!=instance.p_deviceList.End(); ++iter){

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

		INDI::BaseDevice* device = indiClient->getDevice(IsoString(iter->DeviceName).c_str());
		if (device && device->isConnected()) {
			deviceNode->getTreeBoxNode()->SetIcon( TextColumn, ScaledResource( ":/bullets/bullet-ball-glass-green.png" ) );
      } else {
			deviceNode->getTreeBoxNode()->SetIcon( TextColumn, ScaledResource( ":/bullets/bullet-ball-glass-grey.png" ) );
		}


	}
	//GUI->UpdateDeviceList_Timer.Stop();
	GUI->DeviceList_TreeBox.EnableUpdates();

}
// ----------------------------------------------------------------------------

INDIDeviceControllerInterface::GUIData::GUIData( INDIDeviceControllerInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Three:" ) ); // the longest label text
   int editWidth1 = fnt.Width( String( '0',14 ) );

   INDIServer_SectionBar.SetTitle("INDI Server Connection");
   INDIServer_SectionBar.SetSection(INDIServerConnection_Control);
   INDIServer_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&INDIDeviceControllerInterface::__ToggleSection, w );
   INDIServerConnection_Control.SetSizer(ParameterHost_Sizer);
   ParameterPort_Label.SetText( "Port:" );
   ParameterPort_Label.SetMinWidth( labelWidth1 );
   ParameterPort_Label.SetToolTip( "<p>INDI server port.</p>" );
   ParameterPort_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ParameterPort_SpinBox.SetRange( int( TheINDIServerPort->MinimumValue() ), int( TheINDIServerPort->MaximumValue() ) );
   ParameterPort_SpinBox.SetFixedWidth( editWidth1 );
   ParameterPort_SpinBox.SetToolTip( "<p>INDI server port.</p>" );
   ParameterPort_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&INDIDeviceControllerInterface::__IntegerValueUpdated, w );

   ParameterHost_Label.SetText( "Host:" );
   ParameterHost_Label.SetMinWidth( labelWidth1 );
   ParameterHost_Label.SetToolTip( "<p>INDI server host.</p>" );
   ParameterHost_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ParameterHost_Edit.SetMinWidth( editWidth1 );
   ParameterHost_Edit.OnGetFocus( (Control::event_handler)&INDIDeviceControllerInterface::__EditGetFocus, w );
   ParameterHost_Edit.OnEditCompleted( (Edit::edit_event_handler)&INDIDeviceControllerInterface::__EditCompleted, w );

   ParameterPort_Label.SetText( "Port:" );
   ParameterPort_Label.SetMinWidth( labelWidth1 );
   ParameterPort_Label.SetToolTip( "<p>INDI server port.</p>" );
   ParameterPort_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ParameterPort_SpinBox.SetRange( int( TheINDIServerPort->MinimumValue() ), int( TheINDIServerPort->MaximumValue() ) );
   ParameterPort_SpinBox.SetFixedWidth( editWidth1 );
   ParameterPort_SpinBox.SetToolTip( "<p>INDI server port.</p>" );
   ParameterPort_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&INDIDeviceControllerInterface::__IntegerValueUpdated, w );

   //pushbuttons
   ConnectServer_PushButton.SetText( "Connect" );
   ConnectServer_PushButton.OnClick( (Button::click_event_handler) &INDIDeviceControllerInterface::__CameraListButtons_Click, w );
   DisconnectServer_PushButton.SetText( "Disconnect" );
   DisconnectServer_PushButton.OnClick( (Button::click_event_handler) &INDIDeviceControllerInterface::__CameraListButtons_Click, w );

   ConnectionServer_Sizer.SetSpacing( 6 );
   ConnectionServer_Sizer.Add(ConnectServer_PushButton);
   ConnectionServer_Sizer.Add(DisconnectServer_PushButton);

   ParameterHost_Sizer.SetSpacing( 4 );
   ParameterHost_Sizer.Add( ParameterHost_Label );
   ParameterHost_Sizer.Add( ParameterHost_Edit, 100 );
   ParameterHost_Sizer.Add( ParameterPort_Label );
   ParameterHost_Sizer.Add( ParameterPort_SpinBox );
   ParameterHost_Sizer.AddSpacing( 4 );
   ParameterHost_Sizer.Add( ConnectionServer_Sizer );

   // INDI devices
   INDIDevices_SectionBar.SetTitle("INDI Devices");
   INDIDevices_SectionBar.SetSection(INDIDevices_Control);
   INDIDevices_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&INDIDeviceControllerInterface::__ToggleSection, w );
   INDIDevices_Control.SetSizer(INDIDevice_Sizer);

   DeviceList_TreeBox.EnableAlternateRowColor();
   DeviceList_TreeBox.EnableMultipleSelections();
   DeviceList_TreeBox.SetNumberOfColumns(1);
   DeviceList_TreeBox.SetHeaderText(0,"Device");
   DeviceList_TreeBox.SetMinHeight( LIST_MINHEIGHT( fnt ) );

   ConnectDevice_PushButton.SetText("Connect");
   ConnectDevice_PushButton.OnClick((Button::click_event_handler) &INDIDeviceControllerInterface::__CameraListButtons_Click, w );
   DisconnectDevice_PushButton.SetText("Disconnect");
   DisconnectDevice_PushButton.OnClick((Button::click_event_handler) &INDIDeviceControllerInterface::__CameraListButtons_Click, w );
   RefreshDevice_PushButton.SetText("Refresh");
   RefreshDevice_PushButton.OnClick((Button::click_event_handler) &INDIDeviceControllerInterface::__CameraListButtons_Click, w );

   DeviceAction_Sizer.SetSpacing( 6 );
   DeviceAction_Sizer.Add(ConnectDevice_PushButton);
   DeviceAction_Sizer.Add(DisconnectDevice_PushButton);
   DeviceAction_Sizer.Add(RefreshDevice_PushButton);
   DeviceAction_Sizer.AddStretch();

   INDIDevice_Sizer.Add( DeviceList_TreeBox, 100 );
   INDIDevice_Sizer.AddSpacing( 8 );
   INDIDevice_Sizer.Add( DeviceAction_Sizer );

   INDIProperties_SectionBar.SetTitle( "INDI Device Properties" );
   INDIProperties_SectionBar.SetSection(INDIProperties_Control);
   INDIProperties_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&INDIDeviceControllerInterface::__ToggleSection, w );
   INDIProperties_Control.SetSizer(INDIDeviceProperty_Sizer);

   PropertyList_TreeBox.EnableAlternateRowColor();
   PropertyList_TreeBox.SetNumberOfColumns(5);
   PropertyList_TreeBox.HideColumn(TypeColumn);
   PropertyList_TreeBox.HideColumn(NumberFormatColumn);
   PropertyList_TreeBox.SetColumnWidth( 0, w.LogicalPixelsToPhysical( 300 ) );
   PropertyList_TreeBox.SetHeaderText( TextColumn, "Property" );
   PropertyList_TreeBox.SetHeaderText( ValueColumn, "Value" );
   PropertyList_TreeBox.SetHeaderText( LabelColumn, "Label" );
   PropertyList_TreeBox.SetMinHeight( LIST_MINHEIGHT( fnt ) );

   PropertyList_TreeBox.OnClose((Control::close_event_handler)&INDIDeviceControllerInterface::__Close,w);

   ServerMessageLabel_Label.SetText( "Last server message:" );
   ServerMessageLabel_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   ServerMessage_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   ServerMessage_Label.SetFixedHeight( 2*fnt.Height() );
   ServerMessage_Label.EnableWordWrapping();

   ServerMessage_Sizer.Add(ServerMessageLabel_Label);
   ServerMessage_Sizer.Add(ServerMessage_Label);

   RefreshProperty_PushButton.SetText("Refresh");
   RefreshProperty_PushButton.OnClick((Button::click_event_handler) &INDIDeviceControllerInterface::PropertyButton_Click, w );
   EditProperty_PushButton.SetText("Edit");
   EditProperty_PushButton.OnClick((Button::click_event_handler) &INDIDeviceControllerInterface::PropertyButton_Click, w );

   Buttons_Sizer.SetSpacing( 6 );
   Buttons_Sizer.Add(RefreshProperty_PushButton);
   Buttons_Sizer.Add(EditProperty_PushButton);
   Buttons_Sizer.AddStretch();

   INDIDeviceProperty_Sizer.Add( PropertyList_TreeBox, 100 );
   INDIDeviceProperty_Sizer.AddSpacing( 8 );
   INDIDeviceProperty_Sizer.Add( Buttons_Sizer );

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
   UpdateDeviceList_Timer.OnTimer( (Timer::timer_event_handler)&INDIDeviceControllerInterface::__UpdateDeviceList_Timer, w );

   UpdatePropertyList_Timer.SetInterval( 0.5 );
   UpdatePropertyList_Timer.SetPeriodic( true );
   UpdatePropertyList_Timer.OnTimer( (Timer::timer_event_handler)&INDIDeviceControllerInterface::__UpdatePropertyList_Timer, w );

   UpdateServerMessage_Timer.SetInterval( 0.5 );
   UpdateServerMessage_Timer.SetPeriodic( true );
   UpdateServerMessage_Timer.OnTimer( (Timer::timer_event_handler)&INDIDeviceControllerInterface::__UpdateServerMessage_Timer, w );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
}

void INDIDeviceControllerInterface::__UpdateDeviceList_Timer( Timer &sender )
{
   if ( sender == GUI->UpdateDeviceList_Timer )
      UpdateDeviceList();
}

void INDIDeviceControllerInterface::__UpdatePropertyList_Timer( Timer &sender )
{
   if ( sender == GUI->UpdatePropertyList_Timer )
      UpdatePropertyList();
}

void INDIDeviceControllerInterface::__UpdateServerMessage_Timer( Timer &sender )
{
   if ( sender == GUI->UpdateServerMessage_Timer )
      GUI->ServerMessage_Label.SetText( instance.p_currentMessage );
}

void INDIDeviceControllerInterface::__ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start )
   {
      GUI->DeviceList_TreeBox.SetFixedHeight();
      GUI->PropertyList_TreeBox.SetFixedHeight();
   }
   else
   {
      GUI->DeviceList_TreeBox.SetMinHeight( LIST_MINHEIGHT( Font() ) );
      GUI->DeviceList_TreeBox.SetMaxHeight( int_max );
      GUI->PropertyList_TreeBox.SetMinHeight( LIST_MINHEIGHT( Font() ) );
      GUI->PropertyList_TreeBox.SetMaxHeight( int_max );
      if ( GUI->INDIDevices_Control.IsVisible() || GUI->INDIProperties_Control.IsVisible() )
         SetVariableHeight();
      else
         SetFixedHeight();
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

SetPropertyDialog* SetPropertyDialog::createPropertyDialog( String indiType, String numberFormat, INDIDeviceControllerInstance* indiInstance )
{
   if ( indiType == "INDI_NUMBER" )
   {
      size_type im = numberFormat.Find( 'm' );
      if ( im != String::notFound )
         return new EditNumberCoordPropertyDialog( indiInstance );
   }

   if ( indiType == "INDI_SWITCH" )
      return new EditSwitchPropertyDialog( indiInstance );

   return new EditPropertyDialog( indiInstance );
}

void EditPropertyDialog::EditCompleted( Edit& sender )
{
	m_newPropertyListItem.NewPropertyValue=sender.Text();
}

EditPropertyDialog::EditPropertyDialog(INDIDeviceControllerInstance* indiInstance):SetPropertyDialog(indiInstance){
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
	double sign=m_hour >=0 ? 1.0 : -1.0;
	double coord=sign * (fabs(m_hour) + m_minute / 60 + m_second / 3600);
	m_newPropertyListItem.NewPropertyValue=String(coord) ;
}

EditNumberCoordPropertyDialog::EditNumberCoordPropertyDialog(INDIDeviceControllerInstance* indiInstance):SetPropertyDialog(indiInstance),m_hour(0),m_minute(0),m_second(0){
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

EditSwitchPropertyDialog::EditSwitchPropertyDialog(INDIDeviceControllerInstance* indiInstance):SetPropertyDialog(indiInstance){
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



void INDIDeviceControllerInterface::UpdatePropertyList(){
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
	if (indiClient.get()!=NULL){
		indiClient.get()->m_mutex.Lock();
	}
	PropertyNodeFactory factory;
	for (INDIDeviceControllerInstance::PropertyListType::iterator iter=instance.p_propertyList.Begin() ; iter!=instance.p_propertyList.End(); ++iter){

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
			rootNode->accept(findDeviceNodeVisitor,PropertyUtils::getKey(iter->Device),IsoString());
			FindNodeVisitor* findPropNodeVisitor = new FindNodeVisitor();
			rootNode->accept(findPropNodeVisitor,PropertyUtils::getKey(iter->Device,iter->Property),IsoString());
			if (findDeviceNodeVisitor->foundNode() && findPropNodeVisitor->foundNode()){
				findDeviceNodeVisitor->getNode()->RemoveChild(findPropNodeVisitor->getNode());
				delete findPropNodeVisitor->getNode();
			}
			itemsToBeRemoved.push_back(*iter);
		} else  {
			PropertyNode* elemNode = propTree->addElementNode(iter->Device,iter->Property,iter->Element,iter->PropertyState,iter->PropertyLabel);
			elemNode->setNodeINDIType(iter->PropertyTypeStr);
			elemNode->setNodeINDINumberFormat(iter->PropertyNumberFormat);
			if (iter->PropertyTypeStr==String("INDI_NUMBER")){
				elemNode->setNodeINDIValue( PropertyUtils::getFormattedNumber( iter->PropertyValue, IsoString( iter->PropertyNumberFormat ) ) );
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
		INDIDeviceControllerInstance::PropertyListType::iterator iter = instance.p_propertyList.Search(itemsToBeRemoved[i]);
		if (iter==instance.p_propertyList.End()){
			IsoString elemStr=IsoString(iter->Element);
			throw Error(String().Format("%s",elemStr.c_str()));
		}
		instance.p_propertyList.Remove(iter);

	}

	// set newly created elements to Idle
	for (size_t i=0; i<itemsCreated.size(); i++){
		INDIDeviceControllerInstance::PropertyListType::iterator iter =instance.p_propertyList.Search(itemsCreated[i]);
		if (iter!=instance.p_propertyList.End()){
			iter->PropertyFlag=Idle;
		}
	}
	if (indiClient.get()!=NULL){
		indiClient.get()->m_mutex.Unlock();
	}
}


void INDIDeviceControllerInterface::__Close( Control& sender, bool& allowClose){

}

void INDIDeviceControllerInterface::PropertyButton_Click( Button& sender, bool checked ){

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
	else if (sender==GUI->RefreshProperty_PushButton){
		UpdatePropertyList();
	}
}

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerInterface.cpp - Released 2015/10/13 15:55:45 UTC
