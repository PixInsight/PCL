//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.07.0141
// ----------------------------------------------------------------------------
// INDIDeviceControllerInterface.cpp - Released 2016/04/28 15:13:36 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2016 Klaus Kretzschmar
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
#include "INDIDeviceControllerParameters.h"
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
   GUI( nullptr )
{
   TheINDIDeviceControllerInterface = this;
}

INDIDeviceControllerInterface::~INDIDeviceControllerInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;

   for ( PropertyTreeMapType::iterator iter = m_propertyTreeMap.begin(); iter != m_propertyTreeMap.end(); iter++ )
      delete iter->second;
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
   return nullptr; // PixInsightINDIIcon_XPM; // ### TODO
}

InterfaceFeatures INDIDeviceControllerInterface::Features() const
{
   return InterfaceFeature::DragObject
        | InterfaceFeature::BrowseDocumentationButton;
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
   if ( r == nullptr )
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
   GUI->ParameterPort_SpinBox.SetValue( instance.p_serverPort );
   GUI->ParameterHost_Edit.SetText( instance.p_serverHostName );
}

// ----------------------------------------------------------------------------

void INDIDeviceControllerInterface::__IntegerValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->ParameterPort_SpinBox )
      instance.p_serverPort = value;
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
      instance.p_serverHostName = sender.Text();
}

void INDIDeviceControllerInterface::__CameraListButtons_Click( Button& sender, bool checked )
{
   if ( sender == GUI->ConnectServer_PushButton )
   {
      try
      {
         if ( !INDIClient::HasClient() )
            INDIClient::NewClient( &instance );

         if ( !INDIClient::TheClient()->serverIsConnected() )
         {
            IsoString hostName8 = instance.p_serverHostName.ToUTF8();
            INDIClient::TheClient()->setServer( hostName8.c_str(), instance.p_serverPort );
            if ( INDIClient::TheClient()->connectServer() )
            {
               GUI->UpdateDeviceList_Timer.Start();
               GUI->UpdateServerMessage_Timer.Start();
               GUI->UpdatePropertyList_Timer.Start();
            }
            else
               GUI->ServerMessage_Label.SetText( "Connection to INDI server failed. Please check server host and port." );
         }
      }
      ERROR_HANDLER
   }
   else if ( sender == GUI->DisconnectServer_PushButton )
   {
      if ( INDIClient::HasClient() )
      {
         try
         {
            if ( INDIClient::TheClient()->serverIsConnected() )
               INDIClient::TheClient()->disconnectServer();

            GUI->UpdateDeviceList_Timer.Stop();
            GUI->UpdatePropertyList_Timer.Stop();
            GUI->UpdateServerMessage_Timer.Stop();

            m_rootNodeMap.clear();
            m_deviceNodeMap.clear();
            m_deviceRootNodeMap.clear();
            for ( auto item : m_propertyTreeMap )
               delete item.second;
            m_propertyTreeMap.clear();

            // flag property list items as Insert to create new property tree
            for ( auto item : instance.o_properties )
               item.PropertyFlag = Insert;

            GUI->DeviceList_TreeBox.Clear();
            GUI->PropertyList_TreeBox.Clear();

            // clear instance variables
            instance.o_devices.Clear();
            instance.o_properties.Clear();

            if ( !INDIClient::TheClient()->serverIsConnected() )
            {
               INDIClient::DestroyClient();
               GUI->ServerMessage_Label.SetText( "Successfully disconnected from server" );
            }
         }
         ERROR_HANDLER
      }
   }
   else if ( sender == GUI->ConnectDevice_PushButton )
   {
      if ( INDIClient::HasClient() )
      {
         try
         {
            pcl::IndirectArray<pcl::TreeBox::Node> selectedNodes = GUI->DeviceList_TreeBox.SelectedNodes();
            for ( auto nodePtr : selectedNodes )
            {
               IsoString deviceName( nodePtr->Text( TextColumn ).To7BitASCII() );
               INDI::BaseDevice* device = INDIClient::TheClient()->getDevice( deviceName.c_str() );
               if ( device != nullptr )
                  if ( !device->isConnected() )
                     INDIClient::TheClient()->connectDevice( deviceName.c_str() );
            }
         }
         ERROR_HANDLER
      }
   }
   else if ( sender == GUI->DisconnectDevice_PushButton )
   {
      if ( INDIClient::HasClient() )
      {
         try
         {
            pcl::IndirectArray<pcl::TreeBox::Node> selectedNodes = GUI->DeviceList_TreeBox.SelectedNodes();
            for ( auto nodePtr : selectedNodes )
            {
               IsoString deviceName( nodePtr->Text( TextColumn ).To7BitASCII() );
               INDI::BaseDevice* device = INDIClient::TheClient()->getDevice( deviceName.c_str() );
               if ( device != nullptr )
                  if ( device->isConnected() )
                     INDIClient::TheClient()->disconnectDevice( deviceName.c_str() );
               nodePtr->Enable();
            }
         }
         ERROR_HANDLER
      }
   }
}

void INDIDeviceControllerInterface::UpdateDeviceList()
{
   if ( INDIClient::HasClient() )
      for ( auto item : instance.o_devices )
      {
         PropertyNode* rootNode;
         PropertyNodeMapType::iterator rootNodeIter = m_deviceRootNodeMap.find( item.DeviceName );
         if ( rootNodeIter == m_deviceRootNodeMap.end() )
            m_deviceRootNodeMap[item.DeviceName] = rootNode = new PropertyNode( GUI->DeviceList_TreeBox );
         else
            rootNode = rootNodeIter->second;
         assert( rootNode != nullptr );

         PropertyNode* deviceNode;
         PropertyNodeMapType::iterator deviceNodeIter = m_deviceNodeMap.find( item.DeviceName );
         if ( deviceNodeIter == m_deviceNodeMap.end() )
            m_deviceNodeMap[item.DeviceName] = deviceNode = new PropertyNode( rootNode, IsoString( item.DeviceName ) );
         else
            deviceNode = deviceNodeIter->second;
         assert( deviceNode != NULL );

         deviceNode->getTreeBoxNode()->SetText( TextColumn, item.DeviceName );
         deviceNode->getTreeBoxNode()->SetAlignment( TextColumn, TextAlign::Left );

         INDI::BaseDevice* device;
         {
            IsoString s( item.DeviceName );
            device = INDIClient::TheClient()->getDevice( s.c_str() );
         }
         deviceNode->getTreeBoxNode()->SetIcon( TextColumn, ScaledResource(
            (device != nullptr && device->isConnected()) ? ":/bullets/bullet-ball-glass-green.png" : ":/bullets/bullet-ball-glass-grey.png" ) );
      }
}

// ----------------------------------------------------------------------------

INDIDeviceControllerInterface::GUIData::GUIData( INDIDeviceControllerInterface& w )
{
   pcl::Font fnt = w.Font();
   int editWidth1 = fnt.Width( String( '0', 14 ) );

   INDIServer_SectionBar.SetTitle( "INDI Server Connection" );
   INDIServer_SectionBar.SetSection( INDIServer_Control );
   INDIServer_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&INDIDeviceControllerInterface::__ToggleSection, w );

   ParameterHost_Label.SetText( "Host:" );
   ParameterHost_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ParameterHost_Edit.SetMinWidth( editWidth1 );
   ParameterHost_Edit.OnGetFocus( (Control::event_handler)&INDIDeviceControllerInterface::__EditGetFocus, w );
   ParameterHost_Edit.OnEditCompleted( (Edit::edit_event_handler)&INDIDeviceControllerInterface::__EditCompleted, w );

   ParameterPort_Label.SetText( "Port:" );
   ParameterPort_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ParameterPort_SpinBox.SetRange( int( TheIDCServerPortParameter->MinimumValue() ), int( TheIDCServerPortParameter->MaximumValue() ) );
   ParameterPort_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&INDIDeviceControllerInterface::__IntegerValueUpdated, w );

   ConnectServer_PushButton.SetText( "Connect" );
   ConnectServer_PushButton.OnClick( (Button::click_event_handler)&INDIDeviceControllerInterface::__CameraListButtons_Click, w );

   DisconnectServer_PushButton.SetText( "Disconnect" );
   DisconnectServer_PushButton.OnClick( (Button::click_event_handler)&INDIDeviceControllerInterface::__CameraListButtons_Click, w );

   ConnectionServer_Sizer.SetSpacing( 6 );
   ConnectionServer_Sizer.Add( ConnectServer_PushButton );
   ConnectionServer_Sizer.Add( DisconnectServer_PushButton );

   INDIServer_Sizer.SetSpacing( 4 );
   INDIServer_Sizer.Add( ParameterHost_Label );
   INDIServer_Sizer.Add( ParameterHost_Edit, 100 );
   INDIServer_Sizer.AddSpacing( 6 );
   INDIServer_Sizer.Add( ParameterPort_Label );
   INDIServer_Sizer.Add( ParameterPort_SpinBox );
   INDIServer_Sizer.AddSpacing( 8 );
   INDIServer_Sizer.Add( ConnectionServer_Sizer );

   INDIServer_Control.SetSizer( INDIServer_Sizer );

   //

   INDIDevices_SectionBar.SetTitle( "INDI Devices" );
   INDIDevices_SectionBar.SetSection( INDIDevices_Control );
   INDIDevices_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&INDIDeviceControllerInterface::__ToggleSection, w );

   DeviceList_TreeBox.EnableAlternateRowColor();
   DeviceList_TreeBox.EnableMultipleSelections();
   DeviceList_TreeBox.SetNumberOfColumns( 1 );
   DeviceList_TreeBox.SetHeaderText( 0, "Device" );
   DeviceList_TreeBox.SetMinHeight( LIST_MINHEIGHT( fnt ) );
   DeviceList_TreeBox.SetScaledMinWidth( 400 );

   ConnectDevice_PushButton.SetText( "Connect" );
   ConnectDevice_PushButton.OnClick( (Button::click_event_handler)&INDIDeviceControllerInterface::__CameraListButtons_Click, w );

   DisconnectDevice_PushButton.SetText( "Disconnect" );
   DisconnectDevice_PushButton.OnClick( (Button::click_event_handler)&INDIDeviceControllerInterface::__CameraListButtons_Click, w );

   DeviceAction_Sizer.SetSpacing( 6 );
   DeviceAction_Sizer.Add( ConnectDevice_PushButton );
   DeviceAction_Sizer.Add( DisconnectDevice_PushButton );
   DeviceAction_Sizer.AddStretch();

   INDIDevices_Sizer.Add( DeviceList_TreeBox, 100 );
   INDIDevices_Sizer.AddSpacing( 8 );
   INDIDevices_Sizer.Add( DeviceAction_Sizer );

   INDIDevices_Control.SetSizer( INDIDevices_Sizer );

   //

   INDIProperties_SectionBar.SetTitle( "INDI Device Properties" );
   INDIProperties_SectionBar.SetSection( INDIProperties_Control );
   INDIProperties_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&INDIDeviceControllerInterface::__ToggleSection, w );

   PropertyList_TreeBox.EnableAlternateRowColor();
   PropertyList_TreeBox.SetNumberOfColumns(5);
   PropertyList_TreeBox.HideColumn(TypeColumn);
   PropertyList_TreeBox.HideColumn(NumberFormatColumn);
   PropertyList_TreeBox.SetColumnWidth( 0, w.LogicalPixelsToPhysical( 300 ) );
   PropertyList_TreeBox.SetHeaderText( TextColumn, "Property" );
   PropertyList_TreeBox.SetHeaderText( ValueColumn, "Value" );
   PropertyList_TreeBox.SetHeaderText( LabelColumn, "Label" );
   PropertyList_TreeBox.SetMinHeight( LIST_MINHEIGHT( fnt ) );
   PropertyList_TreeBox.SetScaledMinWidth( 400 );

   PropertyList_TreeBox.OnClose( (Control::close_event_handler)&INDIDeviceControllerInterface::__Close, w );

   ServerMessageLabel_Label.SetText( "Last server message:" );
   ServerMessageLabel_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   ServerMessage_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   ServerMessage_Label.SetFixedHeight( 2*fnt.Height() );
   ServerMessage_Label.EnableWordWrapping();

   ServerMessage_Sizer.Add( ServerMessageLabel_Label );
   ServerMessage_Sizer.Add( ServerMessage_Label );

   EditProperty_PushButton.SetText( "Edit" );
   EditProperty_PushButton.OnClick( (Button::click_event_handler) &INDIDeviceControllerInterface::PropertyButton_Click, w );

   Buttons_Sizer.SetSpacing( 6 );
   Buttons_Sizer.Add( EditProperty_PushButton );
   Buttons_Sizer.AddStretch();

   INDIProperties_Sizer.Add( PropertyList_TreeBox, 100 );
   INDIProperties_Sizer.AddSpacing( 8 );
   INDIProperties_Sizer.Add( Buttons_Sizer );

   INDIProperties_Control.SetSizer( INDIProperties_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( INDIServer_SectionBar );
   Global_Sizer.Add( INDIServer_Control );
   Global_Sizer.Add( INDIDevices_SectionBar );
   Global_Sizer.Add( INDIDevices_Control );
   Global_Sizer.Add( INDIProperties_SectionBar );
   Global_Sizer.Add( INDIProperties_Control );
   Global_Sizer.Add( ServerMessage_Sizer );

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
      GUI->ServerMessage_Label.SetText( instance.m_currentMessage );
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

void SetPropertyDialog::Ok_Button_Click( Button& sender, bool checked )
{
   if ( INDIClient::HasClient() )
   {
      assert( m_instance != nullptr );

      INDINewPropertyListItem newPropertyListItem = getNewPropertyListItem();
      bool send_ok = m_instance->sendNewPropertyValue( newPropertyListItem, true/*isAsynch*/ );
      if ( !send_ok )
         m_instance->clearNewPropertyList();
      Ok();
   }
}

void SetPropertyDialog::Cancel_Button_Click( Button& sender, bool checked )
{
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
   m_newPropertyListItem.NewPropertyValue = sender.Text();
}

EditPropertyDialog::EditPropertyDialog( INDIDeviceControllerInstance* indiInstance ) :
   SetPropertyDialog( indiInstance )
{
   pcl::Font fnt = Font();
   int labelWidth = fnt.Width( String( '0', 30 ) );
   int editWidth = fnt.Width( String( '0', 16 ) );

   SetWindowTitle( "INDI Number Property Value" );

   Property_Label.SetMinWidth( labelWidth );
   Property_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Property_Edit.SetMinWidth( editWidth );
   Property_Edit.OnEditCompleted( (Edit::edit_event_handler)&EditPropertyDialog::EditCompleted, *this );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.OnClick( (Button::click_event_handler)&SetPropertyDialog::Ok_Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&SetPropertyDialog::Cancel_Button_Click, *this );

   Property_Sizer.SetSpacing( 4 );
   Property_Sizer.Add( Property_Label );
   Property_Sizer.Add( Property_Edit );
   Property_Sizer.AddStretch();

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.Add( Property_Sizer );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );
}

void EditNumberCoordPropertyDialog::setPropertyValueString( String value )
{
   StringList tokens;
   value.Break( tokens, ':', true/*trim*/ );
   assert( tokens.Length() == 3 );
   tokens[0].TrimLeft();
   Hour_Edit.SetText( tokens[0] );
   Minute_Edit.SetText( tokens[1] );
   Second_Edit.SetText( tokens[2] );
}

void EditNumberCoordPropertyDialog::EditCompleted( Edit& sender )
{
   if ( sender == Hour_Edit )
   {
      m_hour = sender.Text().ToDouble();
      m_minute = Minute_Edit.Text().ToDouble();
      m_second = Second_Edit.Text().ToDouble();
   }
   else if ( sender == Minute_Edit )
   {
      m_hour = Hour_Edit.Text().ToDouble();
      m_minute = sender.Text().ToDouble();
      m_second = Second_Edit.Text().ToDouble();
   }
   else if ( sender == Second_Edit )
   {
      m_hour = Hour_Edit.Text().ToDouble();
      m_minute = Minute_Edit.Text().ToDouble();
      m_second = sender.Text().ToDouble();
   }
   double coord = Abs( m_hour ) + m_minute/60 + m_second/3600;
   if ( m_hour < 0 )
      coord = -coord;
   m_newPropertyListItem.NewPropertyValue = String( coord );
}

EditNumberCoordPropertyDialog::EditNumberCoordPropertyDialog( INDIDeviceControllerInstance* indiInstance ) :
   SetPropertyDialog( indiInstance ),
   m_hour( 0 ),
   m_minute( 0 ),
   m_second( 0 )
{
   pcl::Font fnt = Font();
   int labelWidth = fnt.Width( String( '0', 20 ) );
   int editWidth = fnt.Width( String( '0', 4 ) );

   SetWindowTitle( "INDI Coordinate Property Value" );

   Property_Label.SetMinWidth( labelWidth );
   Property_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Hour_Edit.SetMaxWidth( editWidth );
   Hour_Edit.OnEditCompleted( (Edit::edit_event_handler)&EditNumberCoordPropertyDialog::EditCompleted, *this );

   Colon1_Label.SetText( ":" );

   Minute_Edit.SetMaxWidth( editWidth );
   Minute_Edit.OnEditCompleted( (Edit::edit_event_handler)&EditNumberCoordPropertyDialog::EditCompleted, *this );

   Colon2_Label.SetText( ":" );

   Second_Edit.SetMaxWidth( editWidth );
   Second_Edit.OnEditCompleted( (Edit::edit_event_handler)&EditNumberCoordPropertyDialog::EditCompleted, *this );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.OnClick( (Button::click_event_handler)&SetPropertyDialog::Ok_Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&SetPropertyDialog::Cancel_Button_Click, *this );

   Property_Sizer.SetSpacing( 4 );
   Property_Sizer.Add( Property_Label );
   Property_Sizer.Add( Hour_Edit );
   Property_Sizer.Add( Colon1_Label );
   Property_Sizer.Add( Minute_Edit );
   Property_Sizer.Add( Colon2_Label );
   Property_Sizer.Add( Second_Edit );
   Property_Sizer.AddStretch();

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.Add( Property_Sizer );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );
}

void EditSwitchPropertyDialog::setPropertyValueString( String value )
{
   if ( value == "ON" )
   {
      ON_RadioButton.Check();
      OFF_RadioButton.Uncheck();
   }
   else if ( value == "OFF" )
   {
      ON_RadioButton.Uncheck();
      OFF_RadioButton.Check();
   }
   else
      throw Error( "Invalid switch value." );
}

void EditSwitchPropertyDialog::ButtonChecked( RadioButton& sender )
{
   if ( sender == ON_RadioButton )
      m_newPropertyListItem.NewPropertyValue = "ON";
   else if ( sender == OFF_RadioButton )
      m_newPropertyListItem.NewPropertyValue = "OFF";
}

EditSwitchPropertyDialog::EditSwitchPropertyDialog( INDIDeviceControllerInstance* indiInstance ) :
   SetPropertyDialog( indiInstance )
{
   pcl::Font fnt = Font();
   int labelWidth = fnt.Width( String( '0', 20 ) );

   SetWindowTitle( "INDI switch property value" );

   Property_Label.SetMinWidth(labelWidth);
   Property_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ON_Label.SetText( "ON" );
   OFF_Label.SetText( "OFF" );

   //ON_RadioButton.Uncheck();
   ON_RadioButton.OnCheck( (RadioButton::check_event_handler)&EditSwitchPropertyDialog::ButtonChecked, *this );

   //OFF_RadioButton.Check();
   //OFF_RadioButton.Disable( true );
   OFF_RadioButton.OnCheck( (RadioButton::check_event_handler)&EditSwitchPropertyDialog::ButtonChecked,*this );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.OnClick( (Button::click_event_handler)&SetPropertyDialog::Ok_Button_Click, *this );
   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&SetPropertyDialog::Cancel_Button_Click, *this );

   Property_Sizer.SetSpacing( 4 );
   Property_Sizer.Add( Property_Label );
   Property_Sizer.Add( ON_Label );
   Property_Sizer.Add( ON_RadioButton );
   Property_Sizer.Add( OFF_Label );
   Property_Sizer.Add( OFF_RadioButton );
   Property_Sizer.AddStretch();

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.Add( Property_Sizer );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );
}

void INDIDeviceControllerInterface::UpdatePropertyList()
{
   Array<INDIPropertyListItem> itemsToBeRemoved, itemsCreated;

   // get popertyList with exclusive access
   ExclPropertyList propertyList = instance.getExclusivePropertyList();

   if ( !INDIClient::HasClient() )
   {
      GUI->PropertyList_TreeBox.EnableUpdates();
      return;
   }

   if ( propertyList.get()->IsEmpty() )
   {
      GUI->PropertyList_TreeBox.EnableUpdates();
      return;
   }

   PropertyNodeFactory factory;
   for ( auto item : *propertyList.get() )
   {
      if ( item.PropertyFlag == Idle )
         continue;

      PropertyNode* rootNode = nullptr;
      PropertyNodeMapType::iterator rootNodeIter = m_rootNodeMap.find( item.Device );
      if ( rootNodeIter == m_rootNodeMap.end() )
         m_rootNodeMap[item.Device] = rootNode = new PropertyNode( GUI->PropertyList_TreeBox );
      else
         rootNode = rootNodeIter->second;
      assert( rootNode != nullptr );

      PropertyTree* propTree = nullptr;
      PropertyTreeMapType::iterator propTreeIter = m_propertyTreeMap.find( item.Device );
      if ( propTreeIter == m_propertyTreeMap.end() )
         propTree = new PropertyTree( rootNode, &factory );
      else
         propTree = propTreeIter->second;
      assert( propTree != nullptr );

      if ( item.PropertyFlag == Remove )
      {
         FindNodeVisitor* findDeviceNodeVisitor = new FindNodeVisitor;
         rootNode->accept( findDeviceNodeVisitor, PropertyUtils::getKey( item.Device ), IsoString() );
         FindNodeVisitor* findPropNodeVisitor = new FindNodeVisitor;
         rootNode->accept( findPropNodeVisitor, PropertyUtils::getKey( item.Device, item.Property ), IsoString() );
         if ( findDeviceNodeVisitor->foundNode() && findPropNodeVisitor->foundNode() )
         {
            findDeviceNodeVisitor->getNode()->RemoveChild( findPropNodeVisitor->getNode() );
            delete findPropNodeVisitor->getNode();
         }
         itemsToBeRemoved << item;
      }
      else
      {
         PropertyNode* elemNode = propTree->addElementNode( item.Device, item.Property, item.Element, item.PropertyState, item.PropertyLabel );
         elemNode->setNodeINDIType( item.PropertyTypeStr );
         elemNode->setNodeINDINumberFormat( item.PropertyNumberFormat );
         if ( item.PropertyTypeStr == "INDI_NUMBER" )
            elemNode->setNodeINDIValue( PropertyUtils::getFormattedNumber( item.PropertyValue, IsoString( item.PropertyNumberFormat ) ) );
         else
            elemNode->setNodeINDIValue( item.PropertyValue );
         elemNode->setNodeINDILabel( item.ElementLabel );
         elemNode->getTreeBoxNode()->SetAlignment( TextColumn, TextAlign::Left );
         elemNode->getTreeBoxNode()->SetAlignment( ValueColumn, TextAlign::Left );
         elemNode->getTreeBoxNode()->SetAlignment( LabelColumn, TextAlign::Left );
         if ( item.PropertyFlag == Insert )
            itemsCreated << item;
      }
   }

   // remove properties from property list
   for( auto item : itemsToBeRemoved )
   {
      INDIDeviceControllerInstance::PropertyListType::iterator iter = propertyList.get()->Search( item );
      if ( iter == propertyList.get()->End() )
         throw Error( String( iter->Element ) );
      propertyList.get()->Remove( iter );
   }

   // set newly created elements to Idle
   for ( auto item : itemsCreated )
   {
      INDIDeviceControllerInstance::PropertyListType::iterator iter = propertyList.get()->Search( item );
      if ( iter != propertyList.get()->End() )
         iter->PropertyFlag = Idle;
   }
}

void INDIDeviceControllerInterface::__Close( Control& sender, bool& allowClose )
{
}

void INDIDeviceControllerInterface::PropertyButton_Click( Button& sender, bool checked )
{
   if ( sender == GUI->EditProperty_PushButton )
   {
      pcl::IndirectArray<pcl::TreeBox::Node> selectedNodes;
      selectedNodes = GUI->PropertyList_TreeBox.SelectedNodes();
      if ( !selectedNodes.IsEmpty() )
      {
         TreeBox::Node* node = *selectedNodes.Begin();

         // check that selected node is an element node, i.e. has two parents
         if ( node->Parent() == nullptr || node->Parent()->Parent() == nullptr )
            throw Error( "Please select a property element node (leaf of the tree)" );

         // Create dialog window according to the property type
         GUI->SetPropDlg = SetPropertyDialog::createPropertyDialog( node->Text( TypeColumn ), node->Text( NumberFormatColumn ), &instance );

         GUI->SetPropDlg->setPropertyLabelString( node->Text( LabelColumn ) );
         GUI->SetPropDlg->setPropertyValueString( node->Text( ValueColumn ) );
         INDINewPropertyListItem propItem;
         propItem.Device = node->Parent()->Parent()->Text( TextColumn );
         propItem.Property = node->Parent()->Text( TextColumn );
         propItem.Element = node->Text( TextColumn );
         propItem.NewPropertyValue = node->Text( ValueColumn );
         propItem.PropertyType = node->Text( TypeColumn );
         propItem.PropertyKey = PropertyUtils::getKey( propItem.Device, propItem.Property, propItem.Element );
         GUI->SetPropDlg->setNewPropertyListItem( propItem );
      }

      GUI->SetPropDlg->Execute();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerInterface.cpp - Released 2016/04/28 15:13:36 UTC
