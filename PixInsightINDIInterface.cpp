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
#include "basedevice.h"
#include "indibase.h"
#include "indiapi.h"
#include <pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

PixInsightINDIInterface* ThePixInsightINDIInterface = 0;

// ----------------------------------------------------------------------------

//#include "PixInsightINDIIcon.xpm"

// ----------------------------------------------------------------------------

PixInsightINDIInterface::PixInsightINDIInterface() :
ProcessInterface(), instance( ThePixInsightINDIProcess ), GUI( 0 )
{
   ThePixInsightINDIInterface = this;
}

PixInsightINDIInterface::~PixInsightINDIInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
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

void PixInsightINDIInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
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
      SetWindowTitle( "PixInsightINDI" );
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

void PixInsightINDIInterface::__CameraListButtons_Click( Button& sender, bool checked )
{
        
        if ( sender == GUI->ConnectServer_PushButton )
        {
            try
            {
				if (indiClient.get() == 0)
					indiClient.reset(new INDIClient());

				IsoString ASCIIHost(instance.p_host);
				indiClient->setServer(ASCIIHost.c_str() , instance.p_port);

				bool connected=false;

				connected = indiClient->connectServer();

				if (connected)
					Console() <<"Successfully connected to server "<<ASCIIHost.c_str()<<":"<<instance.p_port<<"\n";

				Sleep(2);

				UpdateDeviceList();

            }
            ERROR_HANDLER
        }
		else if ( sender == GUI->DisconnectServer_PushButton )
		{
			try
            {
                if (indiClient.get() == 0)
					indiClient.reset(new INDIClient());

				if (indiClient->serverIsConnected())
					indiClient->disconnectServer();

				if (!indiClient->serverIsConnected())
					Console() <<"Successfully disconnected from server \n";
				GUI->DeviceList_TreeBox.Clear();
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
					IsoString deviceName((*it)->Text(1).To7BitASCII());
					
					INDI::BaseDevice* device = indiClient->getDevice(deviceName.c_str());

					if (device)
						if (!device->isConnected())
							indiClient->connectDevice(deviceName.c_str());	

					if (device->isConnected())
						(*it)->Check();
				}
				
				GUI->DrvPropDlg.Execute();

			}
            ERROR_HANDLER
		}
		else if ( sender == GUI->DisconnectDevice_PushButton)
		{
			try
            {

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


// ----------------------------------------------------------------------------

PixInsightINDIInterface::GUIData::GUIData( PixInsightINDIInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Three:" ) ); // the longest label text
   int editWidth1 = fnt.Width( String( '0',14 ) );


   //
   INDIServer_SectionBar.SetTitle("INDI server connection");
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
   ParameterHost_Edit.SetToolTip( "<p>INDI server host.</p>" );
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

   
   ParameterHost_Sizer.SetSpacing( 4 );
   ParameterHost_Sizer.Add( ParameterHost_Label );
   ParameterHost_Sizer.Add( ParameterHost_Edit, 100);
   ParameterHost_Sizer.Add( ParameterPort_Label );
   ParameterHost_Sizer.Add( ParameterPort_SpinBox );
   ParameterHost_Sizer.Add( ConnectionServer_Sizer );

   //pushbuttons
   ConnectionServer_Sizer.SetSpacing(4);
   ConnectServer_PushButton.SetText( "Connect" );
   ConnectServer_PushButton.OnClick( (Button::click_event_handler) &PixInsightINDIInterface::__CameraListButtons_Click, w );
   DisconnectServer_PushButton.SetText( "Disconnect" );
   DisconnectServer_PushButton.OnClick( (Button::click_event_handler) &PixInsightINDIInterface::__CameraListButtons_Click, w );

  
   ConnectionServer_Sizer.Add(ConnectServer_PushButton);
   ConnectionServer_Sizer.Add(DisconnectServer_PushButton);
   ConnectionServer_Sizer.AddStretch();

   // INDI devices
   INDIDevices_SectionBar.SetTitle("INDI devices");
   INDIServer_SectionBar.SetSection(DeviceList_TreeBox);
   DeviceList_TreeBox.SetMinHeight( 8*fnt.Height() +2 );
   DeviceList_TreeBox.EnableAlternateRowColor();
   DeviceList_TreeBox.EnableMultipleSelections();
   DeviceList_TreeBox.SetNumberOfColumns(2);
   DeviceList_TreeBox.SetHeaderText(0,"Status");
   DeviceList_TreeBox.SetHeaderText(1,"Device");

   DeviceAction_Sizer.SetSpacing(4);
   ConnectDevice_PushButton.SetText("Connect");
   ConnectDevice_PushButton.OnClick((Button::click_event_handler) &PixInsightINDIInterface::__CameraListButtons_Click, w );
   DisconnectDevice_PushButton.SetText("Disconnect");
   DisconnectDevice_PushButton.OnClick((Button::click_event_handler) &PixInsightINDIInterface::__CameraListButtons_Click, w );
   RefreshDevice_PushButton.SetText("Refresh");
   RefreshDevice_PushButton.OnClick((Button::click_event_handler) &PixInsightINDIInterface::__CameraListButtons_Click, w );

   DeviceAction_Sizer.Add(ConnectDevice_PushButton);
   DeviceAction_Sizer.Add(DisconnectDevice_PushButton);
   DeviceAction_Sizer.Add(RefreshDevice_PushButton);
   DeviceAction_Sizer.AddStretch();

   INDIDevice_Sizer.Add(DeviceList_TreeBox);
   INDIDevice_Sizer.Add(DeviceAction_Sizer);
   
   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add(INDIServer_SectionBar);	
   Global_Sizer.Add( INDIServerConnection_Control );
   Global_Sizer.Add(INDIDevices_SectionBar);
   Global_Sizer.Add(INDIDevice_Sizer);

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

void PixInsightINDIInterface::UpdateDeviceList(){
	
	GUI->DeviceList_TreeBox.DisableUpdates();
	GUI->DeviceList_TreeBox.Clear();

	if (indiClient.get() == 0)
		return;

   vector<INDI::BaseDevice *> pDevices = indiClient.get()->getDevices();
   for (std::vector<INDI::BaseDevice *>::iterator it = pDevices.begin(); it!=pDevices.end(); ++it  )
   {
	   TreeBox::Node* node = new TreeBox::Node( GUI->DeviceList_TreeBox );
       if ( node == 0 )
			return;

	   node->SetText( 1, (*it)->getDeviceName() );
       node->SetAlignment( 1, TextAlign::Left );
	   Console()<<"Detected device "<<(*it)->getDeviceName() <<"\n";
   }

   GUI->DeviceList_TreeBox.EnableUpdates();
}


// ----------------------------------------------------------------------------
DevicePropertiesDialog::DevicePropertiesDialog():Dialog(){
	pcl::Font fnt = Font();
	int width = fnt.Width( String( '0',70 ) );

	PropertyList_TreeBox.SetMinHeight( 16*fnt.Height() +2 );
	PropertyList_TreeBox.SetMinWidth(width);
	PropertyList_TreeBox.EnableAlternateRowColor();
	PropertyList_TreeBox.SetNumberOfColumns(4);
	PropertyList_TreeBox.SetHeaderText(0,String("Device"));
	PropertyList_TreeBox.SetHeaderText(1,String("Property"));
	PropertyList_TreeBox.SetHeaderText(2,String("Name"));
	PropertyList_TreeBox.SetHeaderText(3,String("Value"));

	RefreshProperty_PushButton.SetText("Refresh");
	RefreshProperty_PushButton.OnClick((Button::click_event_handler) &DevicePropertiesDialog::Button_Click, *this );
	EditProperty_PushButton.SetText("Edit");
	EditProperty_PushButton.OnClick((Button::click_event_handler) &DevicePropertiesDialog::Button_Click, *this );

	INDIDeviceProperty_Sizer.Add(PropertyList_TreeBox);
	INDIDeviceProperty_Sizer.Add(Buttons_Sizer);
	
	Buttons_Sizer.SetSpacing(4);
	Buttons_Sizer.AddSpacing(10);
	Buttons_Sizer.Add(RefreshProperty_PushButton);
	Buttons_Sizer.Add(EditProperty_PushButton);
	Buttons_Sizer.AddStretch();

	Global_Sizer.Add(INDIDeviceProperty_Sizer);

	SetSizer(Global_Sizer);

}

void DevicePropertiesDialog::UpdatePropertyList(){
	
	PropertyList_TreeBox.DisableUpdates();
	PropertyList_TreeBox.Clear();

	if (indiClient.get() == 0)
		return;

   vector<INDI::BaseDevice *> pDevices = indiClient.get()->getDevices();
   for (std::vector<INDI::BaseDevice *>::iterator it = pDevices.begin(); it!=pDevices.end(); ++it  )
   {
	   TreeBox::Node* pnode = new TreeBox::Node( PropertyList_TreeBox );
		   if ( pnode == 0 )
			   return;
	   
       
	   pnode->SetText( 0, (*it)->getDeviceName() );
	   pnode->SetAlignment( 0, TextAlign::Left );
		   
	   vector<INDI::Property *>* pProperties = (*it)->getProperties();
	   for (std::vector<INDI::Property*>::iterator propIt = pProperties->begin(); propIt!=pProperties->end(); ++propIt){

		   TreeBox::Node* node = new TreeBox::Node();
		   if ( node == 0 )
			   return;
		   pnode->Add(node);
		   
		   switch((*propIt)->getType()){
		   case INDI_TYPE::INDI_TEXT:
		   {
			   for (int i=0; i<(*propIt)->getText()->ntp;i++) {
				   TreeBox::Node* cnode = new TreeBox::Node();
				   node->Add(cnode);
				   cnode->SetText( 3, (*propIt)->getText()->tp[i].text );
				   cnode->SetText( 2, (*propIt)->getText()->tp[i].label );
			   }
				break;
		   }
		   case INDI_TYPE::INDI_SWITCH:
		   {
			   for (int i=0; i<(*propIt)->getSwitch()->nsp;i++) {
				   TreeBox::Node* cnode = new TreeBox::Node();
				   node->Add(cnode);
				   cnode->SetText( 3, (*propIt)->getSwitch()->sp[i].s == ISState::ISS_ON  ? "ON" : "OFF"  );
				   cnode->SetText( 2, (*propIt)->getSwitch()->sp[i].label  );
			   }
			   break;
		   }
		   case INDI_TYPE::INDI_NUMBER:
		   {
			   for (int i=0; i<(*propIt)->getNumber()->nnp;i++) {
				   TreeBox::Node* cnode = new TreeBox::Node();
				   node->Add(cnode);
				   IsoString number((*propIt)->getNumber()->np[i].value);
				   cnode->SetText(3, number.c_str());
				   cnode->SetText( 2, (*propIt)->getNumber()->np[i].label  );
			   }
			   break;
		   }
		   default:
			   node->SetText(2, "no value");
		   }

		   
		   node->SetText( 1, (*propIt)->getLabel() );
		   node->SetAlignment( 1, TextAlign::Left );
		   Console()<<"Detected property "<<(*propIt)->getName() <<"\n";
	   }
   }

   PropertyList_TreeBox.EnableUpdates();
}

void DevicePropertiesDialog::Button_Click( Button& sender, bool checked ){
	
	if ( sender == RefreshProperty_PushButton )
	{
		UpdatePropertyList();
	}
}

} // pcl

// ****************************************************************************
// EOF PixInsightINDIInterface.cpp - Released 2013/03/24 18:42:27 UTC