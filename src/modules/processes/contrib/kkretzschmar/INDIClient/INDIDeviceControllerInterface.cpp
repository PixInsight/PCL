//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.01.00.0238
// ----------------------------------------------------------------------------
// INDIDeviceControllerInterface.cpp - Released 2019-01-21T12:06:42Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2018 Klaus Kretzschmar
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

#include <pcl/Dialog.h>
#include <pcl/MessageBox.h>
#include <pcl/RadioButton.h>

#define DEVICE_COLUMN   0
#define PROPERTY_COLUMN 0
#define ELEMENT_COLUMN  0
#define VALUE_COLUMN    1
#define LABEL_COLUMN    2

#define DEVICE_TREE_MINHEIGHT( fnt )   RoundInt( 8.125*fnt.Height() )

namespace pcl
{

// ----------------------------------------------------------------------------

INDIDeviceControllerInterface* TheINDIDeviceControllerInterface = nullptr;

// ----------------------------------------------------------------------------

//#include "PixInsightINDIIcon.xpm"// ### TODO

// ----------------------------------------------------------------------------

/*
 * Device and property tree nodes.
 */

class DeviceNode : public TreeBox::Node
{
public:

   DeviceNode( TreeBox& parent, const INDIDeviceListItem& item )
   {
      parent.Add( this );
      Update( item );
   }

   void Update( const INDIDeviceListItem& item )
   {
      m_item = item;
      Update();
   }

   void Update()
   {
      const char* iconResource;
      if (m_item.DeviceName.IsEmpty()) {
         iconResource = ":/icons/error.png";
      } else {
         iconResource =  INDIClient::TheClient()->IsDeviceConnected(IsoString(m_item.DeviceName)) ? ":/bullets/bullet-ball-glass-green.png" : ":/bullets/bullet-ball-glass-grey.png";
      }

      SetIcon( DEVICE_COLUMN, ParentTree().ScaledResource( iconResource ) );

      SetText( DEVICE_COLUMN, m_item.DeviceName );
   }

   const INDIDeviceListItem& Item() const
   {
      return m_item;
   }

   const String& DeviceName() const
   {
      return m_item.DeviceName;
   }

private:

   INDIDeviceListItem m_item;
};

class PropertyNode : public TreeBox::Node
{
public:

   PropertyNode( DeviceNode* parent, const INDIPropertyListItem& item )
   {
      parent->Add( this );
      Update( item );
   }

   void Update( const INDIPropertyListItem& item )
   {
      m_item = item;

      const char* iconResource;
      switch ( PropertyState() )
      {
      case INDIGO_OK_STATE:
         iconResource = ":/bullets/bullet-ball-glass-green.png";
         break;
      case INDIGO_ALERT_STATE:
         iconResource = ":/bullets/bullet-ball-glass-red.png";
         break;
      case INDIGO_BUSY_STATE:
         iconResource = ":/bullets/bullet-ball-glass-yellow.png";
         break;
      default: // ?!
      case INDIGO_IDLE_STATE:
         iconResource = ":/bullets/bullet-ball-glass-grey.png";
         break;
      }
      SetIcon( PROPERTY_COLUMN, ParentTree().ScaledResource( iconResource ) );

      SetText( PROPERTY_COLUMN, m_item.Property );
   }

   const INDIPropertyListItem& Item() const
   {
      return m_item;
   }

   const String& DeviceName() const
   {
      return m_item.Device;
   }

   const String& PropertyName() const
   {
      return m_item.Property;
   }

   unsigned PropertyState() const
   {
      return m_item.PropertyState;
   }

private:

   INDIPropertyListItem m_item;
};

class PropertyElementNode : public TreeBox::Node
{
public:

   PropertyElementNode( PropertyNode* parent, const INDIPropertyListItem& item )
   {
      parent->Add( this );
      Update( item );
   }

   void Update( const INDIPropertyListItem& item )
   {
      m_item = item;

      SetText( ELEMENT_COLUMN, m_item.Element );

      if ( m_item.PropertyTypeStr == "INDI_NUMBER" )
         SetText( VALUE_COLUMN, PropertyUtils::FormattedNumber( m_item.PropertyValue, IsoString( m_item.PropertyNumberFormat ) ) );
      else
         SetText( VALUE_COLUMN, m_item.PropertyValue );

      SetText( LABEL_COLUMN, m_item.ElementLabel );

      SetToolTip( 0, m_item.PropertyKey );
   }

   const INDIPropertyListItem& Item() const
   {
      return m_item;
   }

private:

   INDIPropertyListItem m_item;
};

// ----------------------------------------------------------------------------

/*
 * Property editing dialogs.
 */

class PropertyEditDialog : public Dialog
{
public:

   PropertyEditDialog( const INDIPropertyListItem& item ) :
      m_item( item )
   {
      Key_Label.SetText( item.PropertyKey );

      Text_Label.SetText( item.ElementLabel );
      Text_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

      Text_Sizer.SetSpacing( 6 );
      Text_Sizer.Add( Text_Label );

      OK_PushButton.SetText( "OK" );
      OK_PushButton.SetDefault();
      OK_PushButton.OnClick( (Button::click_event_handler)&PropertyEditDialog::e_Click, *this );

      Cancel_PushButton.SetText( "Cancel" );
      Cancel_PushButton.OnClick( (Button::click_event_handler)&PropertyEditDialog::e_Click, *this );

      Buttons_Sizer.SetSpacing( 8 );
      Buttons_Sizer.AddStretch();
      Buttons_Sizer.Add( OK_PushButton );
      Buttons_Sizer.Add( Cancel_PushButton );

      Global_Sizer.SetMargin( 8 );
      Global_Sizer.SetSpacing( 6 );
      Global_Sizer.Add( Key_Label );
      Global_Sizer.Add( Text_Sizer );
      Global_Sizer.Add( Buttons_Sizer );

      SetSizer( Global_Sizer );

      const char* typeTitleChunk;
      switch( m_item.PropertyType )
      {
      case INDIGO_SWITCH_VECTOR: typeTitleChunk = "Switch "; break;
      case INDIGO_NUMBER_VECTOR: typeTitleChunk = "Number "; break;
      case INDIGO_LIGHT_VECTOR:  typeTitleChunk = "Light ";  break;
      case INDIGO_TEXT_VECTOR:   typeTitleChunk = "Text ";   break;
      default:          typeTitleChunk = "";        break;
      }
      SetWindowTitle( "INDI " + String( typeTitleChunk ) + "Property" );

      SetScaledMinWidth( 500 );

      OnShow( (Control::event_handler)&PropertyEditDialog::e_Show, *this );
   }

   INDINewPropertyItem NewItem() const
   {
      const char* typeName;
      switch( m_item.PropertyType )
      {
      case INDIGO_SWITCH_VECTOR: typeName = "INDI_SWITCH";  break;
      case INDIGO_NUMBER_VECTOR: typeName = "INDI_NUMBER";  break;
      case INDIGO_LIGHT_VECTOR:  typeName = "INDI_LIGHT";   break;
      case INDIGO_TEXT_VECTOR:   typeName = "INDI_TEXT";    break;
      default:          typeName = "INDI_UNKNOWN"; break;
      }
      INDINewPropertyItem newItem( m_item.Device, m_item.Property, typeName, m_item.Element, NewItemValue() );
      newItem.PropertyKey = m_item.PropertyKey;
      return newItem;
   }

   static bool EditProperty( INDINewPropertyItem& result, const INDIPropertyListItem& );

protected:

   VerticalSizer     Global_Sizer;
      Label             Key_Label;
      HorizontalSizer   Text_Sizer;
         Label             Text_Label;
      HorizontalSizer   Buttons_Sizer;
         PushButton        OK_PushButton;
         PushButton        Cancel_PushButton;

   void e_Show( Control& )
   {
      AdjustToContents();
      SetFixedHeight();
      SetMinWidth();
   }

   void e_Click( Button& sender, bool checked )
   {
      if ( sender == OK_PushButton )
         Ok();
      else if ( sender == Cancel_PushButton )
         Cancel();
   }

   virtual String NewItemValue() const = 0;

private:

   INDIPropertyListItem m_item;
};

class TextPropertyEditDialog : public PropertyEditDialog
{
public:

   TextPropertyEditDialog( const INDIPropertyListItem& item ) :
      PropertyEditDialog( item )
   {
      Text_Edit.SetMinWidth( 32*Font().Width( 'm' ) );
      Text_Sizer.Add( Text_Edit, 100 );

      Text_Edit.SetText( item.PropertyValue );
   }

protected:

   Edit Text_Edit;

   virtual String NewItemValue() const
   {
      return Text_Edit.Text().Trimmed();
   }
};

class NumberPropertyEditDialog : public TextPropertyEditDialog
{
public:

   NumberPropertyEditDialog( const INDIPropertyListItem& item ) :
      TextPropertyEditDialog( item )
   {
   }

private:

   virtual String NewItemValue() const
   {
      return String( Text_Edit.Text().ToDouble() );
   }
};

class SwitchPropertyEditDialog : public PropertyEditDialog
{
public:

   SwitchPropertyEditDialog( const INDIPropertyListItem& item ) :
      PropertyEditDialog( item )
   {
      On_RadioButton.SetText( "ON" );
      Off_RadioButton.SetText( "OFF" );
      Text_Sizer.AddSpacing( 4 );
      Text_Sizer.Add( On_RadioButton );
      Text_Sizer.AddSpacing( 4 );
      Text_Sizer.Add( Off_RadioButton );
      Text_Sizer.AddStretch();

      if ( item.PropertyValue == "ON" )
         On_RadioButton.SetChecked();
      else
         Off_RadioButton.SetChecked();
   }

private:

   RadioButton On_RadioButton;
   RadioButton Off_RadioButton;

   virtual String NewItemValue() const
   {
      return On_RadioButton.IsChecked() ? "ON" : "OFF";
   }
};

class CoordinatesPropertyEditDialog : public PropertyEditDialog
{
public:

   CoordinatesPropertyEditDialog( const INDIPropertyListItem& item ) :
      PropertyEditDialog( item )
   {
      Hours_Edit.SetFixedWidth( 8*Font().Width( '0' ) );
      Colon1_Label.SetText( ':' );
      Colon1_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
      Minutes_Edit.SetFixedWidth( 8*Font().Width( '0' ) );
      Colon2_Label.SetText( ':' );
      Colon2_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );
      Seconds_Edit.SetFixedWidth( 9*Font().Width( '0' ) );
      Negative_CheckBox.SetText( "Negative" );

      Text_Sizer.AddSpacing( 4 );
      Text_Sizer.Add( Hours_Edit );
      Text_Sizer.Add( Colon1_Label );
      Text_Sizer.Add( Minutes_Edit );
      Text_Sizer.Add( Colon2_Label );
      Text_Sizer.Add( Seconds_Edit );
      Text_Sizer.AddSpacing( 4 );
      Text_Sizer.Add( Negative_CheckBox );
      Text_Sizer.AddStretch();

      int sign, s1, s2; double s3;
      DecimalToSexagesimal( sign, s1, s2, s3, item.PropertyValue.ToDouble() );
      Hours_Edit.SetText( String( s1 ) );
      Minutes_Edit.SetText( String( s2 ) );
      Seconds_Edit.SetText( String().Format( "%.3lf", s3 ) );
      Negative_CheckBox.SetChecked( sign < 0 );
   }

private:

   Edit     Hours_Edit;
   Label    Colon1_Label;
   Edit     Minutes_Edit;
   Label    Colon2_Label;
   Edit     Seconds_Edit;
   CheckBox Negative_CheckBox;

   virtual String NewItemValue() const
   {
      int sign = Negative_CheckBox.IsChecked() ? -1 : +1;
      double s1 = Hours_Edit.Text().ToDouble();
      double s2 = Minutes_Edit.Text().ToDouble();
      double s3 = Seconds_Edit.Text().ToDouble();
      double c = sign*(s1 + (s2 + s3/60)/60);
      return String( c );
   }
};

bool PropertyEditDialog::EditProperty( INDINewPropertyItem& result, const INDIPropertyListItem& item )
{
   AutoPointer<PropertyEditDialog> dialog;
   switch ( item.PropertyType )
   {
   case INDIGO_NUMBER_VECTOR:
      if ( item.PropertyNumberFormat.Find( 'm' ) != String::notFound )
         dialog = new CoordinatesPropertyEditDialog( item );
      else
         dialog = new NumberPropertyEditDialog( item );
      break;
   case INDIGO_SWITCH_VECTOR:
      dialog = new SwitchPropertyEditDialog( item );
      break;
   default:
   case INDIGO_TEXT_VECTOR:
      dialog = new TextPropertyEditDialog( item );
      break;
   }

   if ( dialog->Execute() )
   {
      result = dialog->NewItem();
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------

INDIDeviceControllerInterface::INDIDeviceControllerInterface()
{
   TheINDIDeviceControllerInterface = this;
}

INDIDeviceControllerInterface::~INDIDeviceControllerInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
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

bool INDIDeviceControllerInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "INDI Device Controller" );
      GUI->HostName_Edit.SetText( TheIDCServerHostNameParameter->DefaultValue() );
      GUI->Port_SpinBox.SetValue( int( TheIDCServerPortParameter->DefaultValue() ) );
      UpdateDeviceLists();
      UpdateNodeActionButtons( nullptr );
   }

   dynamic = false;
   return &P == TheINDIDeviceControllerProcess;
}

ProcessImplementation* INDIDeviceControllerInterface::NewProcess() const
{
   INDIDeviceControllerInstance* newInstance = new INDIDeviceControllerInstance( TheINDIDeviceControllerProcess );
   newInstance->p_serverHostName = GUI->HostName_Edit.Text().Trimmed();
   newInstance->p_serverPort = GUI->Port_SpinBox.Value();
   return newInstance;
}

bool INDIDeviceControllerInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const INDIDeviceControllerInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an INDIDeviceController instance.";
   return false;
}

bool INDIDeviceControllerInterface::RequiresInstanceValidation() const
{
   return true;
}

bool INDIDeviceControllerInterface::ImportProcess( const ProcessImplementation& p )
{
   const INDIDeviceControllerInstance* r = dynamic_cast<const INDIDeviceControllerInstance*>( &p );
   if ( r != nullptr )
   {
      INDIDeviceControllerInstance instance( *r );
      if ( instance.ExecuteGlobal() )
      {
         GUI->HostName_Edit.SetText( instance.p_serverHostName.Trimmed() );
         GUI->Port_SpinBox.SetValue( instance.p_serverPort );
      }
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------

INDIDeviceControllerInterface::GUIData::GUIData( INDIDeviceControllerInterface& w )
{
   pcl::Font fnt = w.Font();
   int editWidth1 = fnt.Width( String( '0', 14 ) );
   int buttonWidth1 = fnt.Width( "Disconnect" ) + w.LogicalPixelsToPhysical( 30 );

   String buttonStyle = "QPushButton { text-align: left; min-width: " + String( buttonWidth1 ) + "px; }";

   Server_SectionBar.SetTitle( "INDI Server Connection" );
   Server_SectionBar.SetSection( Server_Control );
   Server_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&INDIDeviceControllerInterface::e_ToggleSection, w );

   HostName_Label.SetText( "Host:" );
   HostName_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   HostName_Edit.SetMinWidth( editWidth1 );
   HostName_Edit.OnEditCompleted( (Edit::edit_event_handler)&INDIDeviceControllerInterface::e_EditCompleted, w );

   Port_Label.SetText( "Port:" );
   Port_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Port_SpinBox.SetRange( int( TheIDCServerPortParameter->MinimumValue() ), int( TheIDCServerPortParameter->MaximumValue() ) );
   Port_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&INDIDeviceControllerInterface::e_SpinValueUpdated, w );

   Connect_PushButton.SetText( "Connect" );
   Connect_PushButton.SetIcon( w.ScaledResource( ":/icons/power.png" ) );
   Connect_PushButton.SetStyleSheet( buttonStyle );
   Connect_PushButton.OnClick( (Button::click_event_handler)&INDIDeviceControllerInterface::e_Click, w );

   Disconnect_PushButton.SetText( "Disconnect" );
   Disconnect_PushButton.SetIcon( w.ScaledResource( ":/icons/stop.png" ) );
   Disconnect_PushButton.SetStyleSheet( buttonStyle );
   Disconnect_PushButton.OnClick( (Button::click_event_handler)&INDIDeviceControllerInterface::e_Click, w );

   ServerAction_Sizer.SetSpacing( 6 );
   ServerAction_Sizer.Add( Connect_PushButton );
   ServerAction_Sizer.Add( Disconnect_PushButton );

   Server_Sizer.SetSpacing( 4 );
   Server_Sizer.Add( HostName_Label );
   Server_Sizer.Add( HostName_Edit, 100 );
   Server_Sizer.AddSpacing( 6 );
   Server_Sizer.Add( Port_Label );
   Server_Sizer.Add( Port_SpinBox );
   Server_Sizer.AddSpacing( 8 );
   Server_Sizer.Add( ServerAction_Sizer );

   Server_Control.SetSizer( Server_Sizer );

   //

   Devices_SectionBar.SetTitle( "INDI Devices" );
   Devices_SectionBar.SetSection( Devices_Control );
   Devices_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&INDIDeviceControllerInterface::e_ToggleSection, w );

   Devices_TreeBox.SetMinHeight( DEVICE_TREE_MINHEIGHT( fnt ) );
   Devices_TreeBox.SetScaledMinWidth( 500 );
   Devices_TreeBox.SetNumberOfColumns( LABEL_COLUMN+1 );
   //Devices_TreeBox.SetHeaderText( DEVICE_COLUMN,   String() );
   //Devices_TreeBox.SetHeaderText( PROPERTY_COLUMN, "Property" );
   Devices_TreeBox.SetHeaderText( ELEMENT_COLUMN,  "Device/Property" ); // merge device/property/element items
   Devices_TreeBox.SetHeaderText( VALUE_COLUMN,    "Value" );
   Devices_TreeBox.SetHeaderText( LABEL_COLUMN,    "Label" );
   Devices_TreeBox.EnableRootDecoration();
   Devices_TreeBox.EnableAlternateRowColor();
   Devices_TreeBox.DisableMultipleSelections();
   Devices_TreeBox.SetStyleSheet( w.ScaledStyleSheet(
         "QTreeView {"
            "font-family: DejaVu Sans Mono, Monospace;"
            "font-size: 9pt;"
         "}"
      ) );
   Devices_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&INDIDeviceControllerInterface::e_CurrentNodeUpdated, w );
   Devices_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&INDIDeviceControllerInterface::e_NodeActivated, w );
   Devices_TreeBox.OnNodeDoubleClicked( (TreeBox::node_event_handler)&INDIDeviceControllerInterface::e_NodeDoubleClicked, w );
   Devices_TreeBox.OnNodeExpanded( (TreeBox::node_expand_event_handler)&INDIDeviceControllerInterface::e_NodeExpanded, w );
   Devices_TreeBox.OnNodeCollapsed( (TreeBox::node_expand_event_handler)&INDIDeviceControllerInterface::e_NodeCollapsed, w );
   Devices_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&INDIDeviceControllerInterface::e_NodeSelectionUpdated, w );

   NodeAction_PushButton.SetText( String() ); // updated dynamically
   NodeAction_PushButton.SetStyleSheet( buttonStyle );
   NodeAction_PushButton.OnClick( (Button::click_event_handler)&INDIDeviceControllerInterface::e_Click, w );

   NodeAction_Sizer.SetSpacing( 6 );
   NodeAction_Sizer.Add( NodeAction_PushButton );
   NodeAction_Sizer.AddStretch();

   Devices_Sizer.Add( Devices_TreeBox, 100 );
   Devices_Sizer.AddSpacing( 8 );
   Devices_Sizer.Add( NodeAction_Sizer );

   Devices_Control.SetSizer( Devices_Sizer );

   //

   ServerMessage_Label.SetTextAlignment( TextAlign::Left|TextAlign::Bottom );
   ServerMessage_Label.SetFixedHeight( 3*fnt.Height() );
   ServerMessage_Label.EnableWordWrapping();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Server_SectionBar );
   Global_Sizer.Add( Server_Control );
   Global_Sizer.Add( Devices_SectionBar );
   Global_Sizer.Add( Devices_Control );
   Global_Sizer.Add( ServerMessage_Label );

   SynchronizeWithServer_Timer.SetInterval( 0.5 );
   SynchronizeWithServer_Timer.SetPeriodic( true );
   SynchronizeWithServer_Timer.OnTimer( (Timer::timer_event_handler)&INDIDeviceControllerInterface::e_Timer, w );

   w.OnShow( (Control::event_handler)&INDIDeviceControllerInterface::e_Show, w );
   w.OnHide( (Control::event_handler)&INDIDeviceControllerInterface::e_Hide, w );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
}

// ----------------------------------------------------------------------------

void INDIDeviceControllerInterface::UpdateNodeActionButtons( TreeBox::Node* node )
{
   if ( node != nullptr )
      if ( INDIClient::HasClient() )
      {
         DeviceNode* deviceNode = dynamic_cast<DeviceNode*>( node );
         if ( deviceNode != nullptr )
         {
            IsoString deviceName( deviceNode->DeviceName().To7BitASCII() );

            if ( !deviceName.IsEmpty() )
            {
               GUI->NodeAction_PushButton.Enable();
               if ( INDIClient::TheClient()->IsDeviceConnected(deviceName) )
               {
                  GUI->NodeAction_PushButton.SetText( "Disconnect" );
                  GUI->NodeAction_PushButton.SetIcon( ScaledResource( ":/icons/stop.png" ) );
                  GUI->NodeAction_PushButton.SetToolTip( "<p>Disconnect from '" + deviceNode->DeviceName() + "' device.</p>" );
               }
               else
               {
                  GUI->NodeAction_PushButton.SetText( "Connect" );
                  GUI->NodeAction_PushButton.SetIcon( ScaledResource( ":/icons/power.png" ) );
                  GUI->NodeAction_PushButton.SetToolTip( "<p>Connect to '" + deviceNode->DeviceName() + "' device.</p>" );
               }
               return;
            }
         }
         else
         {
            PropertyElementNode* elementNode = dynamic_cast<PropertyElementNode*>( node );
            if ( elementNode != nullptr )
            {
               GUI->NodeAction_PushButton.Enable();
               GUI->NodeAction_PushButton.SetText( "Edit" );
               GUI->NodeAction_PushButton.SetIcon( ScaledResource( ":/icons/item-edit.png" ) );
               GUI->NodeAction_PushButton.SetToolTip( "<p>Edit '" + elementNode->Item().PropertyKey + "' property element.</p>" );
               return;
            }
         }
      }

   GUI->NodeAction_PushButton.SetText( "Unavailable" );
   GUI->NodeAction_PushButton.SetIcon( ScaledResource( ":/icons/delete.png" ) );
   GUI->NodeAction_PushButton.SetToolTip( (GUI->Devices_TreeBox.NumberOfChildren() > 0) ?
      "<p>No action available for the selected tree item.</p>"
      "<p>Please select a device or editable property element.</p>"
      :
      "<p>No devices available.</p>" );
   GUI->NodeAction_PushButton.Disable();
}

void INDIDeviceControllerInterface::UpdateDeviceLists()
{
   if ( !INDIClient::HasClient() || !INDIClient::TheClient()->IsServerConnected() )
   {
      if ( GUI->Devices_TreeBox.NumberOfChildren() > 0 )
      {
         GUI->Devices_TreeBox.Clear();
         UpdateNodeActionButtons( nullptr );
      }
      return;
   }

   INDIDeviceListItemArray createdDevices, removedDevices;
   bool haveChanges = INDIClient::TheClient()->ReportChangedDeviceLists( createdDevices, removedDevices );

   INDIPropertyListItemArray createdProperties, removedProperties, updatedProperties;
   haveChanges |= INDIClient::TheClient()->ReportChangedPropertyLists( createdProperties, removedProperties, updatedProperties );

   GUI->Devices_TreeBox.DisableUpdates();

   if ( !createdDevices.IsEmpty() )
      for ( auto item : createdDevices )
         new DeviceNode( GUI->Devices_TreeBox, item );

   if ( !removedDevices.IsEmpty() )
      for ( int n = GUI->Devices_TreeBox.NumberOfChildren(), i = n; --i >= 0; )
      {
         DeviceNode* deviceNode = static_cast<DeviceNode*>( GUI->Devices_TreeBox[i] );
         if ( removedDevices.Contains( deviceNode->Item() ) )
            GUI->Devices_TreeBox.Remove( i );
      }

   if ( !createdProperties.IsEmpty() )
      for ( auto item : createdProperties )
         for ( int n = GUI->Devices_TreeBox.NumberOfChildren(), i = 0; i < n; ++i )
         {
            DeviceNode* deviceNode = static_cast<DeviceNode*>( GUI->Devices_TreeBox[i] );
            if ( deviceNode->DeviceName() == item.Device )
            {
               bool inserted = false;
               for ( int n = deviceNode->NumberOfChildren(), i = 0; i < n; ++i )
               {
                  PropertyNode* propertyNode = static_cast<PropertyNode*>( (*deviceNode)[i] );
                  if ( propertyNode->PropertyName() == item.Property )
                  {
                     new PropertyElementNode( propertyNode, item );
                     inserted = true;
                     break;
                  }
               }
               if ( !inserted )
                  new PropertyElementNode( new PropertyNode( deviceNode, item ), item );
               break;
            }
         }

   if ( !removedProperties.IsEmpty() )
      for ( auto item : removedProperties )
         for ( int n = GUI->Devices_TreeBox.NumberOfChildren(), i = 0; i < n; ++i )
         {
            DeviceNode* deviceNode = static_cast<DeviceNode*>( GUI->Devices_TreeBox[i] );
            if ( deviceNode->DeviceName() == item.Device )
            {
               for ( int n = deviceNode->NumberOfChildren(), i = 0; i < n; ++i )
               {
                  PropertyNode* propertyNode = static_cast<PropertyNode*>( (*deviceNode)[i] );
                  if ( propertyNode->PropertyName() == item.Property )
                  {
                     deviceNode->Remove( i );
                     break;
                  }
               }
               break;
            }
         }

   if ( !updatedProperties.IsEmpty() )
      for ( auto item : updatedProperties )
         for ( int n = GUI->Devices_TreeBox.NumberOfChildren(), i = 0; i < n; ++i )
         {
            DeviceNode* deviceNode = static_cast<DeviceNode*>( GUI->Devices_TreeBox[i] );
            if ( deviceNode->DeviceName() == item.Device )
            {
               for ( int n = deviceNode->NumberOfChildren(), i = 0; i < n; ++i )
               {
                  PropertyNode* propertyNode = static_cast<PropertyNode*>( (*deviceNode)[i] );
                  if ( propertyNode->PropertyName() == item.Property )
                  {
                     propertyNode->Update( item );
                     for ( int n = propertyNode->NumberOfChildren(), i = 0; i < n; ++i )
                     {
                        PropertyElementNode* elementNode = static_cast<PropertyElementNode*>( (*propertyNode)[i] );
                        if ( elementNode->Item() == item )
                        {
                           elementNode->Update( item );
                           break;
                        }
                     }
                     break;
                  }
               }
               break;
            }
         }

   // Update to reflect connected/disconnected state (since the server does not
   // notify us when this happens...)
   for ( int n = GUI->Devices_TreeBox.NumberOfChildren(), i = 0; i < n; ++i )
      static_cast<DeviceNode*>( GUI->Devices_TreeBox[i] )->Update();

   if ( haveChanges )
      AdjustTreeColumns();

   GUI->Devices_TreeBox.EnableUpdates();

   UpdateNodeActionButtons( GUI->Devices_TreeBox.CurrentNode() );
}

void INDIDeviceControllerInterface::AdjustTreeColumns()
{
   for ( int i = 0, n = GUI->Devices_TreeBox.NumberOfColumns(); i < n; ++i )
      GUI->Devices_TreeBox.AdjustColumnWidthToContents( i );
}

// ----------------------------------------------------------------------------

void INDIDeviceControllerInterface::e_Show( Control& )
{
   GUI->SynchronizeWithServer_Timer.Stop();
   if ( INDIClient::HasClient() )
   {
      GUI->Devices_TreeBox.Clear();
      UpdateNodeActionButtons( nullptr );
      INDIClient::TheClient()->RestartChangeReports();
   }
   GUI->SynchronizeWithServer_Timer.Start();
}

void INDIDeviceControllerInterface::e_Hide( Control& )
{
   GUI->SynchronizeWithServer_Timer.Stop();
}

void INDIDeviceControllerInterface::e_ToggleSection( SectionBar& sender, Control& section, bool start )
{
   if ( start )
      GUI->Devices_TreeBox.SetFixedHeight();
   else
   {
      GUI->Devices_TreeBox.SetMinHeight( DEVICE_TREE_MINHEIGHT( Font() ) );
      GUI->Devices_TreeBox.SetMaxHeight( int_max );
      if ( GUI->Devices_Control.IsVisible() )
         SetVariableHeight();
      else
         SetFixedHeight();
   }
}

void INDIDeviceControllerInterface::e_Click( Button& sender, bool checked )
{
   if ( sender == GUI->Connect_PushButton )
   {
      if ( INDIClient::HasClient() )
         if ( INDIClient::TheClient()->IsServerConnected() )
         {
            if ( MessageBox( "<p>About to disconnect from INDI server:</p>"
                     "<p>" + INDIClient::TheClient()->HostName() + ":" + IsoString( INDIClient::TheClient()->Port() ) + "</p>"
                     "<p><b>Are you sure?</b></p>",
                     WindowTitle(),
                     StdIcon::Warning, StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes )
            {
               return;
            }

            INDIClient::TheClient()->disconnectServer();
         }

      IsoString hostName8 = GUI->HostName_Edit.Text().Trimmed().ToUTF8();
      uint32 port = GUI->Port_SpinBox.Value();

      if ( INDIClient::HasClient() )
         INDIClient::TheClient()->setServer( hostName8.c_str(), port );
      else
         INDIClient::NewClient( hostName8, port );

      std::ostringstream errMesg;
      bool success = INDIClient::TheClient()->connectServer(errMesg);
      if ( ! success )
         MessageBox( "<p>Failure to connect to INDI server:</p>"
                     "<p>" + GUI->HostName_Edit.Text().Trimmed() + ":" + String( port ) + "</p>"
                     "<p><b>Possible reason: </b></p>" + IsoString(errMesg.str().c_str()),
                     WindowTitle(),
                     StdIcon::Error, StdButton::Ok ).Execute();

      UpdateDeviceLists();
   }
   else if ( sender == GUI->Disconnect_PushButton )
   {
      if ( INDIClient::HasClient() )
      {
         if ( INDIClient::TheClient()->IsServerConnected() )
            INDIClient::TheClient()->disconnectServer();

         if ( !INDIClient::TheClient()->IsServerConnected() )
         {
            INDIClient::DestroyClient();
            GUI->ServerMessage_Label.SetText( "Successfully disconnected from server." );
         }
      }

      UpdateDeviceLists();
   }
   else
   {
      if ( INDIClient::HasClient() )
         if ( sender == GUI->NodeAction_PushButton )
         {
            DeviceNode* deviceNode = dynamic_cast<DeviceNode*>( GUI->Devices_TreeBox.CurrentNode() );
            if ( deviceNode != nullptr )
            {
               IsoString deviceName( deviceNode->DeviceName().To7BitASCII() );
               if ( !deviceName.IsEmpty() )
               {
                  if ( INDIClient::TheClient()->IsDeviceConnected(deviceName) )
                     INDIClient::TheClient()->disconnectDevice( deviceName );
                  else
                     INDIClient::TheClient()->connectDevice( deviceName );
               }
               else
               {
                  MessageBox( "<p>Unable to find INDI device '" + deviceName + "'</p>",
                              WindowTitle(),
                              StdIcon::Error, StdButton::Ok ).Execute();
               }
            }
            else
            {
               PropertyElementNode* elementNode = dynamic_cast<PropertyElementNode*>( GUI->Devices_TreeBox.CurrentNode() );
               if ( elementNode != nullptr )
               {
                  INDINewPropertyItem result;
                  if ( PropertyEditDialog::EditProperty( result, elementNode->Item() ) )
                     if ( !INDIClient::TheClient()->SendNewPropertyItem( result , true /*asynch*/) )
                        MessageBox( "<p>Failure to send new property item value:</p>"
                                    "<p>" + elementNode->Item().PropertyKey + "</p>",
                                    WindowTitle(),
                                    StdIcon::Error, StdButton::Ok ).Execute();
               }
            }
         }
   }
}

void INDIDeviceControllerInterface::e_EditCompleted( Edit& sender )
{
   if ( sender == GUI->HostName_Edit )
      sender.SetText( sender.Text().Trimmed() );
}

void INDIDeviceControllerInterface::e_SpinValueUpdated( SpinBox& sender, int value )
{
   // Placeholder
   /*
   if ( sender == GUI->Port_SpinBox )
      ...
   */
}

void INDIDeviceControllerInterface::e_CurrentNodeUpdated( TreeBox& sender, TreeBox::Node& current, TreeBox::Node& oldCurrent )
{
   UpdateNodeActionButtons( &current );
}

void INDIDeviceControllerInterface::e_NodeActivated( TreeBox& sender, TreeBox::Node& node, int col )
{
   if ( INDIClient::HasClient() )
   {
      DeviceNode* deviceNode = dynamic_cast<DeviceNode*>( &node );
      if ( deviceNode != nullptr )
      {
         IsoString deviceName( deviceNode->DeviceName().To7BitASCII() );
         if ( !deviceName.IsEmpty() )
            if ( INDIClient::TheClient()->IsDeviceConnected(deviceName) )
            {
               if ( MessageBox( "<p>About to disconnect from INDI device '" + deviceNode->DeviceName() + "'</p>"
                        "<p>Are you sure?</p>",
                        WindowTitle(),
                        StdIcon::Warning,
                        StdButton::No, StdButton::Yes ).Execute() != StdButton::Yes )
               {
                  return;
               }

               INDIClient::TheClient()->disconnectDevice( deviceName );
            }
            else
               INDIClient::TheClient()->connectDevice( deviceName );

      }
      else
      {
         PropertyElementNode* elementNode = dynamic_cast<PropertyElementNode*>( GUI->Devices_TreeBox.CurrentNode() );
         if ( elementNode != nullptr )
         {
            INDINewPropertyItem result;
            if ( PropertyEditDialog::EditProperty( result, elementNode->Item() ) )
               INDIClient::TheClient()->SendNewPropertyItem( result , true /*asynch*/);
         }
      }
   }
}

void INDIDeviceControllerInterface::e_NodeDoubleClicked( TreeBox& sender, TreeBox::Node& node, int col )
{
   // Placeholder
}

void INDIDeviceControllerInterface::e_NodeExpanded( TreeBox& sender, TreeBox::Node& node )
{
   AdjustTreeColumns();
}

void INDIDeviceControllerInterface::e_NodeCollapsed( TreeBox& sender, TreeBox::Node& node )
{
   // Placeholder
}

void INDIDeviceControllerInterface::e_NodeSelectionUpdated( TreeBox& sender )
{
   // Placeholder
}

void INDIDeviceControllerInterface::e_Timer( Timer& sender )
{
   UpdateDeviceLists();
   if ( INDIClient::HasClient() )
      GUI->ServerMessage_Label.SetText( INDIClient::TheClient()->CurrentServerMessage() );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIDeviceControllerInterface.cpp - Released 2019-01-21T12:06:42Z
