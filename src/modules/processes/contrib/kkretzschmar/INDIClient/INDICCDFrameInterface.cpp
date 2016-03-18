//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.03.0102
// ----------------------------------------------------------------------------
// INDICCDFrameInterface.cpp - Released 2016/03/18 13:15:37 UTC
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

#include "INDICCDFrameInstance.h"
#include "INDICCDFrameInterface.h"
#include "INDICCDFrameParameters.h"
#include "INDICCDFrameProcess.h"

#include "INDI/basedevice.h"
#include "INDI/indiapi.h"
#include "INDI/indibase.h"

#include <pcl/Console.h>

#include <assert.h>

// time out in seconds
#define PCL_TIMEOUT 60

namespace pcl
{

// ----------------------------------------------------------------------------

INDICCDFrameInterface* TheINDICCDFrameInterface = nullptr;

// ----------------------------------------------------------------------------

//#include "INDICCDFrameProcess.xpm" // ### TODO

// ----------------------------------------------------------------------------

// ### Make this a public PCL API?
class SimpleGetStringDialog : public Dialog
{
public:

   SimpleGetStringDialog( const String& label, const String& text = String(), const String& title = "INDICCDFrame" ) :
      Dialog()
   {
      Text_Label.SetText( label );
      Text_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

      Text_Edit.SetText( text.Trimmed() );
      Text_Edit.SetMinWidth( 32*Font().Width( 'm' ) );

      Text_Sizer.SetSpacing( 6 );
      Text_Sizer.Add( Text_Label );
      Text_Sizer.Add( Text_Edit, 100 );

      OK_PushButton.SetText( "OK" );
      OK_PushButton.SetDefault();
      OK_PushButton.OnClick( (Button::click_event_handler)&SimpleGetStringDialog::e_Click, *this );

      Cancel_PushButton.SetText( "Cancel" );
      Cancel_PushButton.OnClick( (Button::click_event_handler)&SimpleGetStringDialog::e_Click, *this );

      Buttons_Sizer.SetSpacing( 8 );
      Buttons_Sizer.AddStretch();
      Buttons_Sizer.Add( OK_PushButton );
      Buttons_Sizer.Add( Cancel_PushButton );

      Global_Sizer.SetMargin( 8 );
      Global_Sizer.SetSpacing( 6 );
      Global_Sizer.Add( Text_Sizer );
      Global_Sizer.Add( Buttons_Sizer );

      SetSizer( Global_Sizer );

      SetWindowTitle( title );

      OnShow( (Control::event_handler)&SimpleGetStringDialog::e_Show, *this );
   }

   String Text() const
   {
      return Text_Edit.Text().Trimmed();
   }

private:

   VerticalSizer     Global_Sizer;
      HorizontalSizer   Text_Sizer;
         Label             Text_Label;
         Edit              Text_Edit;
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
};

// ----------------------------------------------------------------------------

INDICCDFrameInterface::INDICCDFrameInterface() :
   ProcessInterface(),
   m_device(),
   GUI( nullptr )
{
   TheINDICCDFrameInterface = this;
}

INDICCDFrameInterface::~INDICCDFrameInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString INDICCDFrameInterface::Id() const
{
   return "INDICCDFrame";
}

MetaProcess* INDICCDFrameInterface::Process() const
{
   return TheINDICCDFrameProcess;
}

const char** INDICCDFrameInterface::IconImageXPM() const
{
   return 0; // INDICCDFrameProcess_XPM; // ### TODO
}

InterfaceFeatures INDICCDFrameInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

void INDICCDFrameInterface::ResetInstance()
{
   INDIDeviceControllerInstance defaultInstance( TheINDICCDFrameProcess );
   ImportProcess( defaultInstance );
}

bool INDICCDFrameInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "INDI CCD Controller" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheINDICCDFrameProcess;
}

ProcessImplementation* INDICCDFrameInterface::NewProcess() const
{
   INDICCDFrameInstance* instance = new INDICCDFrameInstance( TheINDICCDFrameProcess );
   instance->p_deviceName = m_device;
   instance->p_uploadMode = GUI->UploadMode_Combo.CurrentItem();
   instance->p_serverUploadDirectory = GUI->UploadDir_Edit.Text().Trimmed();
   //instance->p_serverFileNameTemplate = ; // ### TODO
   instance->p_frameType = GUI->CCDFrameType_Combo.CurrentItem();
   instance->p_binningX = GUI->CCDBinX_Combo.CurrentItem() + 1;
   instance->p_binningY = GUI->CCDBinY_Combo.CurrentItem() + 1;
   instance->p_exposureTime = GUI->ExposureTime_NumericEdit.Value();
   instance->p_exposureDelay = GUI->ExposureDelay_NumericEdit.Value();
   instance->p_exposureCount = GUI->ExposureCount_SpinBox.Value();
   //instance->p_newImageIdTemplate = ; // ### TODO
   return instance;
}

bool INDICCDFrameInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const INDICCDFrameInstance* r = dynamic_cast<const INDICCDFrameInstance*>( &p );
   if ( r == nullptr )
   {
      whyNot = "Not an INDICCDFrame instance.";
      return false;
   }
   whyNot.Clear();
   return true;
}

bool INDICCDFrameInterface::RequiresInstanceValidation() const
{
   return true;
}

bool INDICCDFrameInterface::ImportProcess( const ProcessImplementation& p )
{
   const INDICCDFrameInstance* r = dynamic_cast<const INDICCDFrameInstance*>( &p );
   if ( r != nullptr )
   {
      if ( r->ValidateDevice( false/*throwErrors*/ ) )
      {
         m_device = r->p_deviceName;
         r->SendDeviceProperties();
         GUI->ExposureTime_NumericEdit.SetValue( r->p_exposureTime );
         GUI->ExposureDelay_NumericEdit.SetValue( r->p_exposureDelay );
         GUI->ExposureCount_SpinBox.SetValue( r->p_exposureCount );
         return true;
      }
      else
         m_device.Clear();
      return true;
   }
   return false;
}

// ----------------------------------------------------------------------------

INDICCDFrameInterface::GUIData::GUIData( INDICCDFrameInterface& w )
{
   int emWidth = w.Font().Width( 'm' );
   int labelWidth1 = w.Font().Width( "Server upload directory:" ) + emWidth;
   int editWidth1 = 5*emWidth;
   int editWidth2 = 8*emWidth;

   //

   CCDParam_SectionBar.SetTitle( "CCD Device" );
   CCDParam_SectionBar.SetSection( CCDParam_Control );
   CCDParam_Control.SetSizer( CCDParam_Sizer );

   CCDDevice_Label.SetText( "INDI CCD device:" );
   CCDDevice_Label.SetToolTip( "<p>Select an INDI CCD device.</p>" );
   CCDDevice_Label.SetMinWidth( labelWidth1 );
   CCDDevice_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   CCDDevice_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );

   CCDDevice_Sizer.SetSpacing( 4 );
   CCDDevice_Sizer.Add( CCDDevice_Label );
   CCDDevice_Sizer.Add( CCDDevice_Combo, 100 );

   CCDTemp_NumericEdit.SetReal();
   CCDTemp_NumericEdit.SetPrecision( 2 );
   CCDTemp_NumericEdit.SetRange( -50, +50 );
   CCDTemp_NumericEdit.label.SetText( "Temperature:" );
   CCDTemp_NumericEdit.label.SetFixedWidth( labelWidth1 );
   CCDTemp_NumericEdit.edit.SetFixedWidth( editWidth1 );
   CCDTemp_NumericEdit.sizer.AddStretch();
   CCDTemp_NumericEdit.SetToolTip( "<p>Current chip temperature in degrees Celsius.</p>" );
   CCDTemp_NumericEdit.Disable();

   CCDTargetTemp_NumericEdit.SetReal();
   CCDTargetTemp_NumericEdit.SetPrecision( 2 );
   CCDTargetTemp_NumericEdit.SetRange( -30, +25 );
   CCDTargetTemp_NumericEdit.edit.SetFixedWidth( editWidth1 );
   CCDTargetTemp_NumericEdit.slider.SetScaledMinWidth( 200 );
   CCDTargetTemp_NumericEdit.slider.SetRange( 0, 110 );
   CCDTargetTemp_NumericEdit.SetToolTip( "<p>Target chip temperature in degrees Celsius.</p>" );
   CCDTargetTemp_NumericEdit.Disable();

   CCDTargetTemp_PushButton.SetText( "Set" );
   CCDTargetTemp_PushButton.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );
   CCDTargetTemp_PushButton.Disable();

   CCDTemp_HSizer.SetSpacing( 4 );
   CCDTemp_HSizer.Add( CCDTemp_NumericEdit );
   CCDTemp_HSizer.Add( CCDTargetTemp_NumericEdit, 100 );
   CCDTemp_HSizer.AddSpacing( 4 );
   CCDTemp_HSizer.Add( CCDTargetTemp_PushButton );

   const char* ccdBinXToolTipText =
      "<p>Horizontal (X-axis) pixel binning factor.</p>";

   CCDBinX_Label.SetText( "Binning X:" );
   CCDBinX_Label.SetFixedWidth( labelWidth1 );
   CCDBinX_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CCDBinX_Label.SetToolTip( ccdBinXToolTipText );
   CCDBinX_Label.Disable();

   CCDBinX_Combo.AddItem( "1" );
   CCDBinX_Combo.AddItem( "2" );
   CCDBinX_Combo.AddItem( "3" );
   CCDBinX_Combo.AddItem( "4" );
   CCDBinX_Combo.AddItem( "5" );
   CCDBinX_Combo.AddItem( "6" );
   CCDBinX_Combo.AddItem( "7" );
   CCDBinX_Combo.AddItem( "8" );
   CCDBinX_Combo.SetFixedWidth( editWidth1 );
   CCDBinX_Combo.SetToolTip( ccdBinXToolTipText );
   CCDBinX_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );
   CCDBinX_Combo.Disable();

   CCDBinX_HSizer.SetSpacing( 4 );
   CCDBinX_HSizer.Add( CCDBinX_Label );
   CCDBinX_HSizer.Add( CCDBinX_Combo );
   CCDBinX_HSizer.AddStretch();

   const char* ccdBinYToolTipText =
      "<p>Vertical (Y-axis) pixel binning factor.</p>";

   CCDBinY_Label.SetText( "Binning Y:" );
   CCDBinY_Label.SetFixedWidth( labelWidth1 );
   CCDBinY_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CCDBinY_Label.SetToolTip( ccdBinYToolTipText );
   CCDBinY_Label.Disable();

   CCDBinY_Combo.AddItem( "1" );
   CCDBinY_Combo.AddItem( "2" );
   CCDBinY_Combo.AddItem( "3" );
   CCDBinY_Combo.AddItem( "4" );
   CCDBinY_Combo.AddItem( "5" );
   CCDBinY_Combo.AddItem( "6" );
   CCDBinY_Combo.AddItem( "7" );
   CCDBinY_Combo.AddItem( "8" );
   CCDBinY_Combo.SetFixedWidth( editWidth1 );
   CCDBinY_Combo.SetToolTip( ccdBinYToolTipText );
   CCDBinY_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );
   CCDBinY_Combo.Disable();

   CCDBinY_HSizer.SetSpacing( 4 );
   CCDBinY_HSizer.Add( CCDBinY_Label );
   CCDBinY_HSizer.Add( CCDBinY_Combo );
   CCDBinY_HSizer.AddStretch();

   const char* ccdFrameTypeToolTipText =
      "<p>The frame type will be stored as a standard FITS header keyword in each acquired frame.</p>";

   CCDFrameType_Label.SetText( "Frame type:" );
   CCDFrameType_Label.SetFixedWidth( labelWidth1 );
   CCDFrameType_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CCDFrameType_Label.SetToolTip( ccdFrameTypeToolTipText );
   CCDFrameType_Label.Disable();

   CCDFrameType_Combo.AddItem( "Light frame" );
   CCDFrameType_Combo.AddItem( "Bias frame" );
   CCDFrameType_Combo.AddItem( "Dark frame" );
   CCDFrameType_Combo.AddItem( "Flat frame" );
   CCDFrameType_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );
   CCDFrameType_Combo.SetToolTip( ccdFrameTypeToolTipText );
   CCDFrameType_Combo.Disable();

   CCDFrameType_HSizer.SetSpacing( 4 );
   CCDFrameType_HSizer.Add( CCDFrameType_Label );
   CCDFrameType_HSizer.Add( CCDFrameType_Combo );
   CCDFrameType_HSizer.AddStretch();

   const char* uploadModeToolTipText =
      "<p>Upload to client: The frame will be uploaded to the client and displayed on a new image window. "
      "The frame will not be stored on the client's file system.</p>"
      "<p>Upload to server: The frame will be stored on the file system of the INDI server. "
      "The upload directory can be specified. "
      "The frame will not be uploaded to the client.</p>"
      "<p>Upload both: The frame will be stored on the server's file system, then uploaded to the client and displayed.</p>";

   UploadMode_Label.SetText( "Upload mode:" );
   UploadMode_Label.SetToolTip( uploadModeToolTipText );

   UploadMode_Label.SetMinWidth( labelWidth1 );
   UploadMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   UploadMode_Label.Disable();

   UploadMode_Combo.AddItem( "Upload to client only" );
   UploadMode_Combo.AddItem( "Upload to INDI server only" );
   UploadMode_Combo.AddItem( "Upload both: Client and server" );
   UploadMode_Combo.AdjustToContents();
   UploadMode_Combo.OnItemSelected( (ComboBox::item_event_handler)&INDICCDFrameInterface::e_ItemSelected, w );
   UploadMode_Combo.SetToolTip( uploadModeToolTipText );
   UploadMode_Combo.Disable();

   UploadMode_HSizer.SetSpacing( 4 );
   UploadMode_HSizer.Add( UploadMode_Label );
   UploadMode_HSizer.Add( UploadMode_Combo );
   UploadMode_HSizer.AddStretch();

   UploadDir_Label.SetText( "Server upload directory:" );
   UploadDir_Label.SetToolTip( "<p>The directory where acquired CCD frames will be stored on the INDI server.</p>" );
   UploadDir_Label.SetMinWidth( labelWidth1 );
   UploadDir_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   UploadDir_Label.Disable();

   UploadDir_Edit.SetReadOnly();

   UploadDir_PushButton.SetText( "Set" );
   UploadDir_PushButton.OnClick( (Button::click_event_handler)&INDICCDFrameInterface::e_Click, w );
   UploadDir_PushButton.Disable();

   UploadDir_HSizer.SetSpacing( 4 );
   UploadDir_HSizer.Add( UploadDir_Label );
   UploadDir_HSizer.Add( UploadDir_Edit, 100 );
   UploadDir_HSizer.AddSpacing( 4 );
   UploadDir_HSizer.Add( UploadDir_PushButton );

   CCDProperties_Sizer.SetSpacing( 4 );
   CCDProperties_Sizer.Add( CCDTemp_HSizer );
   CCDProperties_Sizer.Add( CCDBinX_HSizer );
   CCDProperties_Sizer.Add( CCDBinY_HSizer );
   CCDProperties_Sizer.Add( CCDFrameType_HSizer );
   CCDProperties_Sizer.Add( UploadMode_HSizer );
   CCDProperties_Sizer.Add( UploadDir_HSizer );

   CCDProperties_Control.SetSizer( CCDProperties_Sizer );

   CCDParam_Sizer.SetSpacing( 4 );
   CCDParam_Sizer.Add( CCDDevice_Sizer );
   CCDParam_Sizer.Add( CCDProperties_Control );

   //

   FrameAcquisition_SectionBar.SetTitle( "Frame Acquisition" );
   FrameAcquisition_SectionBar.SetSection( FrameAcquisition_Control );

   ExposureTime_NumericEdit.SetReal();
   ExposureTime_NumericEdit.SetPrecision( 3 );
   ExposureTime_NumericEdit.SetRange( 0.001, 60000 ); // 1000 minutes = 16.666... hours
   ExposureTime_NumericEdit.label.SetText( "Exposure time:" );
   ExposureTime_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ExposureTime_NumericEdit.edit.SetFixedWidth( editWidth2 );
   ExposureTime_NumericEdit.sizer.AddStretch();
   ExposureTime_NumericEdit.SetValue( 1 );
   ExposureTime_NumericEdit.SetToolTip( "<p>Exposure time in seconds.</p>" );

   ExposureDelay_NumericEdit.SetReal();
   ExposureDelay_NumericEdit.SetPrecision( 3 );
   ExposureDelay_NumericEdit.SetRange( 0, 600 ); // 10 minutes
   ExposureDelay_NumericEdit.label.SetText( "Exposure delay:" );
   ExposureDelay_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ExposureDelay_NumericEdit.edit.SetFixedWidth( editWidth2 );
   ExposureDelay_NumericEdit.sizer.AddStretch();
   ExposureDelay_NumericEdit.SetValue( 0 );
   ExposureDelay_NumericEdit.SetToolTip( "<p>Waiting time between exposures in seconds.</p>" );

   ExposureCount_Label.SetText( "Number of frames:" );
   ExposureCount_Label.SetToolTip( "<p>Number of frames to be acquired.</p>" );
   ExposureCount_Label.SetMinWidth( labelWidth1 );
   ExposureCount_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ExposureCount_SpinBox.SetRange( 1, 1000 );
   ExposureCount_SpinBox.SetFixedWidth( editWidth2 );
   ExposureCount_SpinBox.SetValue( 1 );

   ExposureCount_Sizer.SetSpacing( 4 );
   ExposureCount_Sizer.Add( ExposureCount_Label );
   ExposureCount_Sizer.Add( ExposureCount_SpinBox );
   ExposureCount_Sizer.AddStretch();

   FrameAcquisition_Sizer.SetSpacing( 4 );
   FrameAcquisition_Sizer.Add( ExposureTime_NumericEdit );
   FrameAcquisition_Sizer.Add( ExposureDelay_NumericEdit );
   FrameAcquisition_Sizer.Add( ExposureCount_Sizer );

   FrameAcquisition_Control.SetSizer( FrameAcquisition_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( CCDParam_SectionBar );
   Global_Sizer.Add( CCDParam_Control );
   Global_Sizer.Add( FrameAcquisition_SectionBar );
   Global_Sizer.Add( FrameAcquisition_Control );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedHeight();

   w.OnShow( (Control::event_handler)&INDICCDFrameInterface::e_Show, w );
   w.OnHide( (Control::event_handler)&INDICCDFrameInterface::e_Hide, w );

   //

   UpdateDeviceList_Timer.SetInterval( 1 );
   UpdateDeviceList_Timer.SetPeriodic();
   UpdateDeviceList_Timer.OnTimer( (Timer::timer_event_handler)&INDICCDFrameInterface::e_Timer, w );

   UpdateDeviceProperties_Timer.SetInterval( 1 );
   UpdateDeviceProperties_Timer.SetPeriodic();
   UpdateDeviceProperties_Timer.OnTimer( (Timer::timer_event_handler)&INDICCDFrameInterface::e_Timer, w );
}

// ----------------------------------------------------------------------------

void INDICCDFrameInterface::UpdateControls()
{
   if ( m_device.IsEmpty() )
   {
      GUI->UpdateDeviceProperties_Timer.Stop();
      GUI->CCDProperties_Control.Disable();
      GUI->FrameAcquisition_Control.Disable();
   }
   else
   {
      if ( !GUI->UpdateDeviceProperties_Timer.IsRunning() )
         GUI->UpdateDeviceProperties_Timer.Start();

      GUI->CCDProperties_Control.Enable();
      GUI->FrameAcquisition_Control.Enable();
   }
}

void INDICCDFrameInterface::e_Show( Control& )
{
   if ( GUI != nullptr )
   {
      e_Timer( GUI->UpdateDeviceList_Timer );
      GUI->UpdateDeviceList_Timer.Start();
   }
}

void INDICCDFrameInterface::e_Hide( Control& )
{
   if ( GUI != nullptr )
   {
      GUI->UpdateDeviceList_Timer.Stop();
      GUI->UpdateDeviceProperties_Timer.Stop();
   }
}

void INDICCDFrameInterface::e_Timer( Timer& sender )
{
   if ( sender == GUI->UpdateDeviceList_Timer )
   {
      GUI->CCDDevice_Combo.Clear();

      if ( TheINDIDeviceControllerInterface != nullptr )
      {
         INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;
         if ( instance.o_devices.IsEmpty() )
            GUI->CCDDevice_Combo.AddItem( "<No Device Available>" );
         else
         {
            GUI->CCDDevice_Combo.AddItem( "<No Device Selected>" );

            for ( auto device : instance.o_devices )
            {
               INDIPropertyListItem CCDProp;
               if ( instance.getINDIPropertyItem( device.DeviceName, "CCD_FRAME", "WIDTH", CCDProp ) ) // is this a camera device?
                  GUI->CCDDevice_Combo.AddItem( device.DeviceName );
            }

            int i = Max( 0, GUI->CCDDevice_Combo.FindItem( m_device ) );
            GUI->CCDDevice_Combo.SetCurrentItem( i );
            if ( i > 0 )
               goto __device_found;
         }
      }

      m_device.Clear();

__device_found:

      UpdateControls();
   }
   else if ( sender == GUI->UpdateDeviceProperties_Timer )
   {
      if ( TheINDIDeviceControllerInterface == nullptr )
         return;

      INDINewPropertyListItem newPropertyListItem;
      INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;
      INDIPropertyListItem CCDProp;
      // get temperature value
      if ( instance.getINDIPropertyItem( m_device, "CCD_TEMPERATURE", "CCD_TEMPERATURE_VALUE", CCDProp ) )
      {
         GUI->CCDTargetTemp_NumericEdit.Enable();
         GUI->CCDTargetTemp_NumericEdit.label.Enable();
         GUI->CCDTargetTemp_PushButton.Enable();
         GUI->CCDTemp_NumericEdit.SetValue( CCDProp.PropertyValue.ToDouble() );
      }
      // get X binning value
      if ( instance.getINDIPropertyItem( m_device, "CCD_BINNING", "HOR_BIN", CCDProp ) )
      {
         GUI->CCDBinX_Combo.Enable();
         GUI->CCDBinX_Label.Enable();
         GUI->CCDBinX_Combo.SetCurrentItem( CCDProp.PropertyValue.ToInt() - 1 );
      }
      // get Y binning value
      if ( instance.getINDIPropertyItem( m_device, "CCD_BINNING", "VER_BIN", CCDProp ) )
      {
         GUI->CCDBinY_Combo.Enable();
         GUI->CCDBinY_Label.Enable();
         GUI->CCDBinY_Combo.SetCurrentItem( CCDProp.PropertyValue.ToInt() - 1 );
      }
      // get upload mode
      int uploadModeIndex = -1;
      if ( instance.getINDIPropertyItem( m_device, "UPLOAD_MODE", "UPLOAD_CLIENT", CCDProp ) )
      {
         if ( CCDProp.PropertyValue == "OFF" )
         {
            if ( instance.getINDIPropertyItem( m_device, "UPLOAD_MODE", "UPLOAD_LOCAL", CCDProp ) )
            {
               if ( CCDProp.PropertyValue == "OFF" )
                  uploadModeIndex = ICFUploadMode::UploadServerAndClient;
               else
                  uploadModeIndex = ICFUploadMode::UploadServer;
            }

            GUI->UploadDir_Label.Enable();
            GUI->UploadDir_PushButton.Enable();
         }
         else
         {
            uploadModeIndex = ICFUploadMode::UploadClient;

            GUI->UploadDir_Label.Disable();
            GUI->UploadDir_PushButton.Disable();
         }
      }
      if ( uploadModeIndex >= 0 )
      {
         GUI->UploadMode_Combo.Enable();
         GUI->UploadMode_Label.Enable();
         GUI->UploadMode_Combo.SetCurrentItem( uploadModeIndex );
      }
      else
      {
         GUI->UploadMode_Combo.Disable();
         GUI->UploadMode_Label.Disable();
      }
      // get frame type
      {
         static const char* indiFrameTypes[] = { "FRAME_LIGHT", "FRAME_BIAS", "FRAME_DARK", "FRAME_FLAT" };
         for ( size_type i = 0; i < ItemsInArray( indiFrameTypes ); ++i )
            if ( instance.getINDIPropertyItem( m_device, "CCD_FRAME_TYPE", indiFrameTypes[i], CCDProp ) )
               if ( CCDProp.PropertyValue == "ON" )
               {
                  GUI->CCDFrameType_Label.Enable();
                  GUI->CCDFrameType_Combo.SetCurrentItem( i );
                  GUI->CCDFrameType_Combo.Enable();
                  break;
               }
      }
      // get upload directory
      if ( instance.getINDIPropertyItem( m_device, "UPLOAD_SETTINGS", "UPLOAD_DIR", CCDProp ) )
         GUI->UploadDir_Edit.SetText( CCDProp.PropertyValue );
   }
}

void INDICCDFrameInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( TheINDIDeviceControllerInterface == nullptr )
      return;

   INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;
   INDIPropertyListItem CCDProp;

   if ( sender == GUI->CCDDevice_Combo )
   {
      // The first item in the list has been reserved for a "no device" selection.
      m_device = (itemIndex > 0) ? sender.ItemText( itemIndex ).Trimmed() : String();
      UpdateControls();

      // check for cooler connection (e.g. Atik cameras)
      if ( !m_device.IsEmpty() )
         if ( instance.getINDIPropertyItem( m_device, "COOLER_CONNECTION", "CONNECT_COOLER", CCDProp ) )
            if ( CCDProp.PropertyValue == "OFF" )
            {
               INDINewPropertyListItem newPropertyListItem;
               newPropertyListItem.Device = m_device;
               newPropertyListItem.Property = "COOLER_CONNECTION";
               newPropertyListItem.Element = "CONNECT_COOLER";
               newPropertyListItem.PropertyType = "INDI_SWITCH";
               newPropertyListItem.NewPropertyValue = "ON";
               instance.sendNewPropertyValue( newPropertyListItem, true );
            }
   }
   else if ( sender == GUI->CCDBinX_Combo )
   {
      if ( instance.getINDIPropertyItem( m_device, "CCD_BINNING", "HOR_BIN", CCDProp ) )
      {
         INDINewPropertyListItem newPropertyListItem;
         newPropertyListItem.Device = m_device;
         newPropertyListItem.Property = "CCD_BINNING";
         newPropertyListItem.Element = "HOR_BIN";
         newPropertyListItem.PropertyType = "INDI_NUMBER";
         newPropertyListItem.NewPropertyValue = GUI->CCDBinX_Combo.ItemText( itemIndex ).Trimmed();
         instance.sendNewPropertyValue( newPropertyListItem, true );
      }
   }
   else if ( sender == GUI->CCDBinY_Combo )
   {
      if ( instance.getINDIPropertyItem( m_device, "CCD_BINNING", "VER_BIN", CCDProp ) )
      {
         INDINewPropertyListItem newPropertyListItem;
         newPropertyListItem.Device = m_device;
         newPropertyListItem.Property = "CCD_BINNING";
         newPropertyListItem.Element = "VER_BIN";
         newPropertyListItem.PropertyType = "INDI_NUMBER";
         newPropertyListItem.NewPropertyValue = GUI->CCDBinY_Combo.ItemText( itemIndex ).Trimmed();
         instance.sendNewPropertyValue( newPropertyListItem, true );
      }
   }
   else if ( sender == GUI->UploadMode_Combo )
   {
      String PropertyElement = INDICCDFrameInstance::UploadModePropertyString( itemIndex );
      if ( instance.getINDIPropertyItem( m_device, "UPLOAD_MODE", PropertyElement, CCDProp ) )
      {
         INDINewPropertyListItem newPropertyListItem;
         newPropertyListItem.Device = m_device;
         newPropertyListItem.Property = "UPLOAD_MODE";
         newPropertyListItem.Element = PropertyElement;
         newPropertyListItem.PropertyType = "INDI_SWITCH";
         newPropertyListItem.NewPropertyValue = "ON";
         instance.sendNewPropertyValue( newPropertyListItem, true );
      }
   }
   else if ( sender == GUI->CCDFrameType_Combo )
   {
      String PropertyElement = INDICCDFrameInstance::CCDFrameTypePropertyString( itemIndex );
      if ( instance.getINDIPropertyItem( m_device, "CCD_FRAME_TYPE", PropertyElement, CCDProp ) )
      {
         INDINewPropertyListItem newPropertyListItem;
         newPropertyListItem.Device = m_device;
         newPropertyListItem.Property = "CCD_FRAME_TYPE";
         newPropertyListItem.Element = PropertyElement;
         newPropertyListItem.PropertyType = "INDI_SWITCH";
         newPropertyListItem.NewPropertyValue = "ON";
         instance.sendNewPropertyValue( newPropertyListItem, true );
      }
   }
}

void INDICCDFrameInterface::e_Click( Button& sender, bool checked )
{
   if ( TheINDIDeviceControllerInterface == nullptr )
      return;

   INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;
   INDINewPropertyListItem newPropertyListItem;
   newPropertyListItem.Device = m_device;

   if ( sender == GUI->CCDTargetTemp_PushButton )
   {
      newPropertyListItem.Property = "CCD_TEMPERATURE";
      newPropertyListItem.Element = "CCD_TEMPERATURE_VALUE";
      newPropertyListItem.PropertyType = "INDI_NUMBER";
      newPropertyListItem.NewPropertyValue = String( GUI->CCDTargetTemp_NumericEdit.Value() );
      instance.sendNewPropertyValue( newPropertyListItem, true/*isAsynchCall*/ );
   }
   else if ( sender == GUI->UploadDir_PushButton )
   {
      SimpleGetStringDialog dialog( "Server upload directory:", GUI->UploadDir_Edit.Text() );
      if ( dialog.Execute() )
      {
         newPropertyListItem.Property = "UPLOAD_SETTINGS";
         newPropertyListItem.Element = "UPLOAD_DIR";
         newPropertyListItem.PropertyType = "INDI_TEXT";
         newPropertyListItem.NewPropertyValue = dialog.Text();
         instance.sendNewPropertyValue( newPropertyListItem, true/*isAsynchCall*/ );
      }
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDICCDFrameInterface.cpp - Released 2016/03/18 13:15:37 UTC
