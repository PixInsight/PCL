//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.02.0096
// ----------------------------------------------------------------------------
// CCDFrameInterface.cpp - Released 2015/10/13 15:55:45 UTC
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

#include "CCDFrameInterface.h"
#include "CCDFrameProcess.h"
#include "INDIDeviceControllerInterface.h"

#include "INDI/basedevice.h"
#include "INDI/indiapi.h"
#include "INDI/indibase.h"

#include <pcl/Console.h>

#include <assert.h>

// time out in seconds
#define pcl_timeout 60

namespace pcl
{

// ----------------------------------------------------------------------------

CCDFrameInterface* TheCCDFrameInterface = nullptr;

// ----------------------------------------------------------------------------

//#include "CCDFrameProcess.xpm"

// ----------------------------------------------------------------------------

CCDFrameInterface::CCDFrameInterface() :
   ProcessInterface(),
   instance( TheCCDFrameProcess),
   GUI( nullptr )
{
   TheCCDFrameInterface = this;
   m_NumOfExposures = 1;
   m_Temperature = 0;
   m_FrameFolder = "./";
   m_FramePrefix = "Image";
   m_isWaiting = false;
   m_timeoutCounter = 0;
}

CCDFrameInterface::~CCDFrameInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString CCDFrameInterface::Id() const
{
   return "INDICCDController";
}

MetaProcess* CCDFrameInterface::Process() const
{
   return TheCCDFrameProcess;
}

const char** CCDFrameInterface::IconImageXPM() const
{
   return 0; // CCDFrameProcess_XPM; // ### TODO
}

InterfaceFeatures CCDFrameInterface::Features() const
{
   return  InterfaceFeature::ApplyGlobalButton | InterfaceFeature::ApplyToViewButton | InterfaceFeature::BrowseDocumentationButton | InterfaceFeature::ResetButton;
}

void CCDFrameInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void CCDFrameInterface::ApplyInstanceGlobal() const
{
   instance.LaunchGlobal();
}

void CCDFrameInterface::ResetInstance()
{
   INDIDeviceControllerInstance defaultInstance( TheCCDFrameProcess );
   ImportProcess( defaultInstance );
}

bool CCDFrameInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "INDI CCD Controller" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheCCDFrameProcess;
}

ProcessImplementation* CCDFrameInterface::NewProcess() const
{
   return new CCDFrameInstance( instance );
}

bool CCDFrameInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const CCDFrameInstance* r = dynamic_cast<const CCDFrameInstance*>( &p );
   if ( r == nullptr )
   {
      whyNot = "Not an INDICCDFrame instance.";
      return false;
   }
   whyNot.Clear();
   return true;
}

bool CCDFrameInterface::RequiresInstanceValidation() const
{
   return true;
}

bool CCDFrameInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void CCDFrameInterface::UpdateControls()
{
}

// ----------------------------------------------------------------------------

CCDFrameInterface::GUIData::GUIData(CCDFrameInterface& w )
{
   int emWidth = w.Font().Width( 'm' );
   int labelWidth1 = w.Font().Width( "Number of frames:" ) + emWidth;

   //

   CCDDevice_SectionBar.SetTitle( "INDI CCD Device Selection" );
   CCDDevice_SectionBar.SetSection( CCDDevice_Control );
   CCDDevice_Control.SetSizer( CCDDevice_Sizer );

   CCDDevice_Label.SetText( "INDI CCD Device:" );
   CCDDevice_Label.SetToolTip( "<p>Select an INDI CCD device.</p>" );
   CCDDevice_Label.SetMinWidth( labelWidth1 );
   CCDDevice_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   CCDDevice_Combo.OnItemSelected( (ComboBox::item_event_handler)&CCDFrameInterface::ComboItemSelected, w );

   CCDDevice_Sizer.SetSpacing( 4 );
   CCDDevice_Sizer.Add( CCDDevice_Label );
   CCDDevice_Sizer.Add( CCDDevice_Combo, 100 );

   //

   CCDParam_SectionBar.SetTitle( "CCD Parameters" );
   CCDParam_SectionBar.SetSection( CCDParam_Control );
   CCDParam_Control.SetSizer( CCDParam_Sizer );

   CCDTemp_NumericEdit.SetReal();
   CCDTemp_NumericEdit.SetPrecision( 2 );
   CCDTemp_NumericEdit.SetRange( -30, 50 );
   CCDTemp_NumericEdit.label.SetText( "Temperature:" );
   CCDTemp_NumericEdit.label.SetFixedWidth( labelWidth1 );
   CCDTemp_NumericEdit.edit.SetFixedWidth( 5*emWidth );
   CCDTemp_NumericEdit.sizer.AddStretch();
   CCDTemp_NumericEdit.SetToolTip( "<p>Current chip temperature in degrees Celsius.</p>" );
   CCDTemp_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CCDFrameInterface::EditValueUpdated, w );
   CCDTemp_NumericEdit.Disable();

   CCDTargetTemp_NumericEdit.SetReal();
   CCDTargetTemp_NumericEdit.SetPrecision( 2 );
   CCDTargetTemp_NumericEdit.SetRange(-30 , 25 );
   CCDTargetTemp_NumericEdit.edit.SetFixedWidth( 5*emWidth );
   CCDTargetTemp_NumericEdit.slider.SetScaledMinWidth(200);
   CCDTargetTemp_NumericEdit.slider.SetRange(0,30);
   CCDTargetTemp_NumericEdit.SetToolTip( "<p>Target chip temperature in degrees Celsius.</p>" );
   CCDTargetTemp_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CCDFrameInterface::EditValueUpdated, w );
   CCDTargetTemp_NumericEdit.Disable();

   CCDTemp_PushButton.SetText("Set");
   CCDTemp_PushButton.SetFixedWidth( 10*emWidth );
   CCDTemp_PushButton.OnClick( (Button::click_event_handler)&CCDFrameInterface::SetCCDPropertyButton_Click, w );
   CCDTemp_PushButton.Disable();

   CCDTemp_HSizer.SetSpacing(6);
   CCDTemp_HSizer.Add(CCDTemp_NumericEdit);
   CCDTemp_HSizer.Add(CCDTargetTemp_NumericEdit);
   CCDTemp_HSizer.AddStretch();
   CCDTemp_HSizer.Add(CCDTemp_PushButton);

   CCDBinX_Label.SetText( "Binning X:" );
   CCDBinX_Label.SetFixedWidth( labelWidth1 );
   CCDBinX_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CCDBinX_Label.SetToolTip("<p>Horizontal pixel binning factor.</p>");
   CCDBinX_Label.Disable();

   CCDBinX_Combo.AddItem("1");
   CCDBinX_Combo.AddItem("2");
   CCDBinX_Combo.AddItem("3");
   CCDBinX_Combo.AddItem("4");
   CCDBinX_Combo.SetToolTip("<p>Set horizontal pixel binning factor.</p>");
   CCDBinX_Combo.OnItemSelected( (ComboBox::item_event_handler)&CCDFrameInterface::ComboItemSelected, w );
   CCDBinX_Combo.Disable();

   CCDBinX_HSizer.SetSpacing(6);
   CCDBinX_HSizer.Add(CCDBinX_Label);
   CCDBinX_HSizer.Add(CCDBinX_Combo);
   CCDBinX_HSizer.AddStretch();

   CCDBinY_Label.SetText( "Binning Y:" );
   CCDBinY_Label.SetFixedWidth( labelWidth1 );
   CCDBinY_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CCDBinY_Label.SetToolTip("<p>Vertical pixel binning factor.</p>");
   CCDBinY_Label.Disable();

   CCDBinY_Combo.AddItem("1");
   CCDBinY_Combo.AddItem("2");
   CCDBinY_Combo.AddItem("3");
   CCDBinY_Combo.AddItem("4");
   CCDBinY_Combo.SetToolTip("<p>Set vertical pixel binning factor.</p>");
   CCDBinY_Combo.OnItemSelected( (ComboBox::item_event_handler)&CCDFrameInterface::ComboItemSelected, w );
   CCDBinY_Combo.Disable();

   CCDBinY_HSizer.SetSpacing(6);
   CCDBinY_HSizer.Add(CCDBinY_Label);
   CCDBinY_HSizer.Add(CCDBinY_Combo);
   CCDBinY_HSizer.AddStretch();

   CCDFrameType_Label.SetText( "Frame Type:" );
   CCDFrameType_Label.SetFixedWidth( labelWidth1 );
   CCDFrameType_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CCDFrameType_Label.SetToolTip("<p>Type of frame: Possible values are 1) Light, 2) Bias, 3) Dark, 4) Flat.</p>");
   CCDFrameType_Label.Disable();

   CCDFrameType_Combo.InsertItem(Light, "Light frame");
   CCDFrameType_Combo.InsertItem(Bias,  "Bias frame");
   CCDFrameType_Combo.InsertItem(Dark,  "Dark frame");
   CCDFrameType_Combo.InsertItem(Flat,  "Flat frame");
   CCDFrameType_Combo.SetToolTip("<p>The frame type will be stored as a FITS keyword in the FITS header.</p>");
   CCDFrameType_Combo.OnItemSelected( (ComboBox::item_event_handler)&CCDFrameInterface::ComboItemSelected, w );
   CCDFrameType_Combo.Disable();

   CCDFrameType_HSizer.SetSpacing(6);
   CCDFrameType_HSizer.Add(CCDFrameType_Label);
   CCDFrameType_HSizer.Add(CCDFrameType_Combo);
   CCDFrameType_HSizer.AddStretch();


   UploadMode_Label.SetText("Upload mode:");
   UploadMode_Label.SetToolTip("<p> Frame upload mode: Possible values are 1) upload to client only, 2) upload to server only, 3) upload both to client and server.<p>");

   UploadMode_Label.SetMinWidth( labelWidth1 );
   UploadMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   UploadMode_Label.Disable();

   UploadMode_Combo.InsertItem(UploadClient,"Upload to client only ");
   UploadMode_Combo.InsertItem(UploadServer,"Upload to INDI server only ");
   UploadMode_Combo.InsertItem(UploadBoth,"Upload both: client and server");
   UploadMode_Combo.AdjustToContents();
   UploadMode_Combo.OnItemSelected( (ComboBox::item_event_handler)&CCDFrameInterface::ComboItemSelected, w );
   UploadMode_Combo.Disable();
   UploadMode_Combo.SetToolTip("<p> Upload to client: The frame will be uploaded to the client and displayed in a separate image window. The frame will be not stored to the file system. <p>"
           	   	   	   	   	   "<p> Upload to server: The frame will be stored on the file system of the INDI server. The directory and filename can be set separately. The frame will not be uploaded to the client. <p>"
           	   	   	   	   	   "<p> Upload both: The frame will be uploaded to the client and displayed and stored on the file system of the server.<p>");

   UploadMode_HSizer.SetSpacing(6);
   UploadMode_HSizer.Add(UploadMode_Label);
   UploadMode_HSizer.Add(UploadMode_Combo);
   UploadMode_HSizer.AddStretch();

   UploadDir_Label.SetText("Upload directory:");
   UploadDir_Label.SetToolTip("<p>Directory where the acquired CCD frames on the INDI server are stored.<p>");
   UploadDir_Label.SetMinWidth(labelWidth1);
   UploadDir_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   UploadDir_Label.Disable();

   UploadDir_Edit.SetFixedWidth(25 * emWidth);
   UploadDir_Edit.SetReadOnly();

   m_updateNewDirStr=true;

   UploadDir_HSizer.SetSpacing( 4 );
   UploadDir_HSizer.Add( UploadDir_Label );
   UploadDir_HSizer.Add( UploadDir_Edit );
   UploadDir_HSizer.AddStretch();

   UploadNewDir_Label.SetText("Set upload directory:");
   UploadNewDir_Label.SetToolTip("<p>Set directory where the acquired CCD frames on the INDI server are stored.<p>");
   UploadNewDir_Label.SetMinWidth(labelWidth1);
   UploadNewDir_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   UploadNewDir_Label.Disable();

   UploadNewDir_Edit.SetFixedWidth(25 * emWidth);
   UploadNewDir_Edit.OnEnter((Control::event_handler)& CCDFrameInterface::EditEntered,w );
   UploadNewDir_Edit.Disable();

   UploadNewDir_PushButton.SetText("Set");
   UploadNewDir_PushButton.SetFixedWidth( 10*emWidth );
   UploadNewDir_PushButton.OnClick( (Button::click_event_handler)&CCDFrameInterface::SetCCDPropertyButton_Click, w );
   UploadNewDir_PushButton.Disable();

   UploadNewDir_HSizer.SetSpacing( 4 );
   UploadNewDir_HSizer.Add( UploadNewDir_Label );
   UploadNewDir_HSizer.Add( UploadNewDir_Edit );
   UploadNewDir_HSizer.AddStretch();
   UploadNewDir_HSizer.Add( UploadNewDir_PushButton );

   UploadPrefix_Label.SetText("Frame image prefix:");
   UploadPrefix_Label.SetToolTip("<p> Frame file prefix of the acquired CCD frames.<p>"
		                         "<p> 'Image_XX' is replaced by 'Image_01','Image_02',...<p>");
   UploadPrefix_Label.SetMinWidth(labelWidth1);
   UploadPrefix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   UploadPrefix_Label.Disable();

   UploadPrefix_Edit.SetFixedWidth(25 * emWidth);
   UploadPrefix_Edit.SetReadOnly();

   UploadPrefix_HSizer.SetSpacing( 4 );
   UploadPrefix_HSizer.Add( UploadPrefix_Label );
   UploadPrefix_HSizer.Add( UploadPrefix_Edit );
   UploadPrefix_HSizer.AddStretch();

   UploadNewPrefix_Label.SetText("Set image prefix:");
   UploadNewPrefix_Label.SetToolTip("<p> Set a new frame file prefix of the acquired CCD frames.<p>"
           	   	   	   	   	   	   	"<p> 'Image_XX' is replaced by 'Image_01','Image_02' <p>");
   UploadNewPrefix_Label.SetMinWidth(labelWidth1);
   UploadNewPrefix_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   UploadNewPrefix_Label.Disable();

   UploadNewPrefix_Edit.SetFixedWidth(25 * emWidth);
   UploadNewPrefix_Edit.OnEnter((Control::event_handler)& CCDFrameInterface::EditEntered,w );
   UploadNewPrefix_Edit.OnTextUpdated((Edit::text_event_handler)&CCDFrameInterface::EditTextUpdated, w);
   UploadNewPrefix_Edit.SetText("OBJECT_B0x0_E0.000_XX");
   UploadNewPrefix_Edit.Disable();

   UploadNewPrefix_PushButton.SetText("Set");
   UploadNewPrefix_PushButton.SetFixedWidth( 10*emWidth );
   UploadNewPrefix_PushButton.OnClick( (Button::click_event_handler)&CCDFrameInterface::SetCCDPropertyButton_Click, w );
   UploadNewPrefix_PushButton.Disable();

   UploadNewPrefix_HSizer.SetSpacing( 4 );
   UploadNewPrefix_HSizer.Add( UploadNewPrefix_Label );
   UploadNewPrefix_HSizer.Add( UploadNewPrefix_Edit );
   UploadNewPrefix_HSizer.AddStretch();
   UploadNewPrefix_HSizer.Add( UploadNewPrefix_PushButton );

   m_updateNewPrefixStr=true;

   CCDParam_Sizer.SetSpacing( 4 );
   CCDParam_Sizer.Add( CCDTemp_HSizer );
   CCDParam_Sizer.Add( CCDBinX_HSizer );
   CCDParam_Sizer.Add( CCDBinY_HSizer );
   CCDParam_Sizer.Add( CCDFrameType_HSizer );
   CCDParam_Sizer.Add( UploadMode_HSizer );
   CCDParam_Sizer.Add( UploadDir_HSizer );
   CCDParam_Sizer.Add( UploadNewDir_HSizer );
   CCDParam_Sizer.Add( UploadPrefix_HSizer );
   CCDParam_Sizer.Add( UploadNewPrefix_HSizer );

   //

   FrameExposure_SectionBar.SetTitle( "Frame Acquisition" );
   FrameExposure_SectionBar.SetSection( FrameExposure_Control );
   FrameExposure_Control.SetSizer( FrameExposure_Sizer );

   ExpTime_NumericEdit.SetReal();
   ExpTime_NumericEdit.SetPrecision( 3 );
   ExpTime_NumericEdit.SetRange( 0.001, 60000 ); // 1000 minutes = 16.666... hours
   ExpTime_NumericEdit.label.SetText( "Exposure Time:" );
   ExpTime_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ExpTime_NumericEdit.edit.SetFixedWidth( 10*emWidth );
   ExpTime_NumericEdit.sizer.AddStretch();
   ExpTime_NumericEdit.SetToolTip( "<p>Exposure time in seconds.</p>" );
   ExpTime_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CCDFrameInterface::EditValueUpdated, w );

   ExpDelayTime_NumericEdit.SetReal();
   ExpDelayTime_NumericEdit.SetPrecision( 3 );
   ExpDelayTime_NumericEdit.SetRange( 0.001, 600 ); // 10 minutes
   ExpDelayTime_NumericEdit.label.SetText( "Delay:" );
   ExpDelayTime_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ExpDelayTime_NumericEdit.edit.SetFixedWidth( 10*emWidth );
   ExpDelayTime_NumericEdit.sizer.AddStretch();
   ExpDelayTime_NumericEdit.SetToolTip( "<p>Waiting time between exposures in seconds.</p>" );
   ExpDelayTime_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CCDFrameInterface::EditValueUpdated, w );

   ExpNum_Label.SetText( "Number of frames:" );
   ExpNum_Label.SetToolTip( "<p>Number of frames to be taken.</p>" );
   ExpNum_Label.SetMinWidth( labelWidth1 );
   ExpNum_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ExpNum_SpinBox.SetRange( 1, 1000 );
   ExpNum_SpinBox.SetFixedWidth( 10*emWidth );
   ExpNum_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&CCDFrameInterface::SpinValueUpdated, w );

   ExpNum_Sizer.SetSpacing( 4 );
   ExpNum_Sizer.Add( ExpNum_Label );
   ExpNum_Sizer.Add( ExpNum_SpinBox );
   ExpNum_Sizer.AddStretch();

   FrameExposureTopLeft_Sizer.SetSpacing( 4 );
   FrameExposureTopLeft_Sizer.Add( ExpTime_NumericEdit );
   FrameExposureTopLeft_Sizer.Add( ExpDelayTime_NumericEdit );
   FrameExposureTopLeft_Sizer.Add( ExpNum_Sizer );

   StartExposure_PushButton.SetText( "Start Exposure" );
   StartExposure_PushButton.SetIcon( w.ScaledResource( ":/icons/play.png" ) );
   StartExposure_PushButton.OnClick( (Button::click_event_handler)&CCDFrameInterface::StartExposureButton_Click, w );

   CancelExposure_PushButton.SetText( "Cancel" );
   CancelExposure_PushButton.SetIcon( w.ScaledResource( ":/icons/stop.png" ) );
   CancelExposure_PushButton.OnClick( (Button::click_event_handler)&CCDFrameInterface::CancelButton_Click, w );

   FrameExposureTopRight_Sizer.AddStretch();
   FrameExposureTopRight_Sizer.Add( StartExposure_PushButton );
   FrameExposureTopRight_Sizer.AddSpacing( 8 );
   FrameExposureTopRight_Sizer.Add( CancelExposure_PushButton );
   FrameExposureTopRight_Sizer.AddStretch();

   FrameExposureTop_Sizer.SetSpacing( 8 );
   FrameExposureTop_Sizer.Add( FrameExposureTopLeft_Sizer );
   FrameExposureTop_Sizer.Add( FrameExposureTopRight_Sizer, 100 );

   ExpDur_Label.SetText( "Timer:" );
   ExpDur_Label.SetToolTip( "<p>Exposure time in seconds.</p>" );
   ExpDur_Label.SetMinWidth( labelWidth1 );
   ExpDur_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ExpDur_Edit.SetFixedWidth( 10*emWidth );
   ExpDur_Edit.SetText( "0" );
   ExpDur_Edit.SetReadOnly();

   ExpDelay_Label.SetText( "Delay:" );
   ExpDelay_Label.SetToolTip( "<p>Waiting time before next exposure in seconds.</p>" );
   ExpDelay_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

   ExpDelay_Edit.SetFixedWidth( 10*emWidth );
   ExpDelay_Edit.SetText( "0" );
   ExpDelay_Edit.SetReadOnly();

   ExpFrame_Label.SetText( "Frame:" );
   ExpFrame_Label.SetToolTip( "<p>Frame number.</p>" );
   ExpFrame_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ExpFrame_Edit.SetFixedWidth( 10*emWidth );
   ExpFrame_Edit.SetText( "0" );
   ExpFrame_Edit.SetReadOnly();

   ExpDur_Sizer.Add( ExpDur_Label );
   ExpDur_Sizer.AddSpacing( 4 );
   ExpDur_Sizer.Add( ExpDur_Edit );
   ExpDur_Sizer.AddSpacing( 8 );
   ExpDur_Sizer.Add( ExpDelay_Label );
   ExpDur_Sizer.AddSpacing( 4 );
   ExpDur_Sizer.Add( ExpDelay_Edit );
   ExpDur_Sizer.AddSpacing( 8 );
   ExpDur_Sizer.Add( ExpFrame_Label );
   ExpDur_Sizer.AddSpacing( 4 );
   ExpDur_Sizer.Add( ExpFrame_Edit );
   ExpDur_Sizer.AddStretch();

   FrameExposure_Sizer.Add( FrameExposureTop_Sizer );
   FrameExposure_Sizer.AddSpacing( 8 );
   FrameExposure_Sizer.Add( ExpDur_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( CCDDevice_SectionBar );
   Global_Sizer.Add( CCDDevice_Control );
   Global_Sizer.Add( CCDParam_SectionBar );
   Global_Sizer.Add( CCDParam_Control );
   Global_Sizer.Add( FrameExposure_SectionBar );
   Global_Sizer.Add( FrameExposure_Control );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedHeight();

   //

   UpdateDeviceList_Timer.SetInterval( 1 );
   UpdateDeviceList_Timer.SetPeriodic( true );
   UpdateDeviceList_Timer.OnTimer( (Timer::timer_event_handler)&CCDFrameInterface::UpdateDeviceList_Timer, w );

   UpdateDeviceList_Timer.Start();

   ExposureDuration_Timer.SetInterval( 1 );
   ExposureDuration_Timer.SetPeriodic( true );
   ExposureDuration_Timer.OnTimer( (Timer::timer_event_handler)&CCDFrameInterface::ExposureDuration_Timer, w );

   ExposureDelay_Timer.SetInterval( 1 );
   ExposureDelay_Timer.SetPeriodic( true );
   ExposureDelay_Timer.OnTimer( (Timer::timer_event_handler) &CCDFrameInterface::ExposureDelay_Timer, w );

   Temperature_Timer.SetInterval( 1 );
   Temperature_Timer.SetPeriodic( true );
   Temperature_Timer.OnTimer( (Timer::timer_event_handler)&CCDFrameInterface::INDI_ServerMonitor_Timer, w );
}

void CCDFrameInterface::UpdateDeviceList()
{
   if ( TheINDIDeviceControllerInterface != nullptr )
   {
      INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;
      if ( !instance.p_deviceList.IsEmpty() ) {
         for ( INDIDeviceControllerInstance::DeviceListType::iterator iter = instance.p_deviceList.Begin();
               iter != instance.p_deviceList.End();
               ++iter ) {
        	 if (GUI->CCDDevice_Combo.FindItem(iter->DeviceName) == -1){ //item not found
        		 GUI->CCDDevice_Combo.AddItem( iter->DeviceName );
        	 }
         }
         //GUI->UpdateDeviceList_Timer.Stop();
      }
   }
}

void CCDFrameInterface::UpdateDeviceList_Timer( Timer &sender )
{
   if( sender == GUI->UpdateDeviceList_Timer  ){
      UpdateDeviceList();
   }
}

void CCDFrameInterface::ExposureDuration_Timer( Timer &sender )
{
   if ( sender == GUI->ExposureDuration_Timer )
   {
      GUI->ExpDur_Edit.SetText( String( sender.Count() ) );
      // increase timeout counter
      m_timeoutCounter++;
      if ( sender.Count()*sender.Interval() >= GUI->ExpTime_NumericEdit.Value() ){
         GUI->ExposureDuration_Timer.Stop();
         // reset timeout counter
         m_timeoutCounter=0;
      }
   }
}

void CCDFrameInterface::ExposureDelay_Timer( Timer &sender )
{
   if ( sender == GUI->ExposureDelay_Timer )
   {
      if ( sender.Count()*sender.Interval() >= GUI->ExpDelayTime_NumericEdit.Value() )
      {
         GUI->ExposureDelay_Timer.Stop();
         m_isWaiting = false;
         GUI->ExpDelay_Edit.SetText( String(0) );
      }
      GUI->ExpDelay_Edit.SetText( String( sender.Count()) );
   }
}

void CCDFrameInterface::EditEntered(Control& sender){
	if (sender == GUI->UploadNewDir_Edit){
		GUI->m_updateNewDirStr=false;
	}
	if (sender == GUI->UploadNewPrefix_Edit) {
		GUI->m_updateNewPrefixStr=false;
	}
}

void CCDFrameInterface::EditTextUpdated( Edit& sender, const String& text ){
	if (sender == GUI->UploadNewPrefix_Edit){
		size_t splitPos = text.Find('_');
		GUI->m_frameTypePrefix[0] = text.Substring(0, splitPos);
	}
}

void CCDFrameInterface::EnableUploadSettingControls() {
	GUI->UploadDir_Label.Enable();
	GUI->UploadNewDir_Label.Enable();
	GUI->UploadNewDir_Edit.Enable();
	GUI->UploadNewDir_PushButton.Enable();
	GUI->UploadPrefix_Label.Enable();
	GUI->UploadNewPrefix_Label.Enable();
	GUI->UploadNewPrefix_Edit.Enable();
	GUI->UploadNewPrefix_PushButton.Enable();
}

void CCDFrameInterface::DisableUploadSettingControls() {
	GUI->UploadDir_Label.Disable();
	GUI->UploadNewDir_Label.Disable();
	GUI->UploadNewDir_Edit.Disable();
	GUI->UploadNewDir_PushButton.Disable();
	GUI->UploadNewPrefix_Label.Disable();
	GUI->UploadNewPrefix_Edit.Disable();
	GUI->UploadNewPrefix_PushButton.Disable();
}

void CCDFrameInterface::updateNewPrefixControl() {
	if (GUI->m_updateNewPrefixStr) {
		int binningX  =GUI->CCDBinX_Combo.CurrentItem();
		int binningY  =GUI->CCDBinY_Combo.CurrentItem();
		String currentText=GUI->UploadNewPrefix_Edit.Text();

		IsoString frameTypePrefix = IsoString(GUI->m_frameTypePrefix[GUI->CCDFrameType_Combo.CurrentItem()]);
		currentText = String().Format("%s_B%dx%d_E%4.3f_XX",frameTypePrefix.c_str(),++binningX, ++binningY,GUI->ExpTime_NumericEdit.Value());
		GUI->UploadNewPrefix_Edit.SetText(currentText);
	}
}

void CCDFrameInterface::INDI_ServerMonitor_Timer( Timer &sender )
{
   if ( TheINDIDeviceControllerInterface != nullptr)
      if ( sender == GUI->Temperature_Timer )
      {

         INDINewPropertyListItem newPropertyListItem;
         INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;
         INDIPropertyListItem CCDProp;
         // get temperature value
         if ( instance.getINDIPropertyItem( m_Device, "CCD_TEMPERATURE", "CCD_TEMPERATURE_VALUE", CCDProp ) ){
        	GUI->CCDTargetTemp_NumericEdit.Enable();
        	GUI->CCDTargetTemp_NumericEdit.label.Enable();
        	GUI->CCDTemp_PushButton.Enable();
            GUI->CCDTemp_NumericEdit.SetValue( CCDProp.PropertyValue.ToDouble() );
         }
         // get X binning value
         if ( instance.getINDIPropertyItem( m_Device, "CCD_BINNING", "HOR_BIN", CCDProp ) ){
        	GUI->CCDBinX_Combo.Enable();
        	GUI->CCDBinX_Label.Enable();
        	GUI->CCDBinX_Combo.SetCurrentItem(CCDProp.PropertyValue.ToInt()-1);
        	updateNewPrefixControl();
         }
         // get Y binning value
         if ( instance.getINDIPropertyItem( m_Device, "CCD_BINNING", "VER_BIN", CCDProp ) ){
        	GUI->CCDBinY_Combo.Enable();
        	GUI->CCDBinY_Label.Enable();
        	GUI->CCDBinY_Combo.SetCurrentItem(CCDProp.PropertyValue.ToInt()-1);
        	updateNewPrefixControl();
         }
         // get upload mode
         int UploadModeIndex=-1;
         if ( instance.getINDIPropertyItem( m_Device, "UPLOAD_MODE", "UPLOAD_CLIENT", CCDProp ) ){
             if (CCDProp.PropertyValue == "OFF"){
            	 if ( instance.getINDIPropertyItem( m_Device, "UPLOAD_MODE", "UPLOAD_LOCAL", CCDProp ) ){
            		 if (CCDProp.PropertyValue == "OFF"){
            			 UploadModeIndex=GUIData::UploadBoth;
            		 } else {
            			 UploadModeIndex=GUIData::UploadServer;
            		 }
					 EnableUploadSettingControls();
            	 }
             } else {
        		 UploadModeIndex=GUIData::UploadClient;
				 DisableUploadSettingControls();
        	 }
         }
         if (UploadModeIndex>=0){
             GUI->UploadMode_Combo.Enable();
             GUI->UploadMode_Label.Enable();
             GUI->UploadMode_Combo.SetCurrentItem(UploadModeIndex);
         } else {
        	 GUI->UploadMode_Combo.Disable();
         }
         // get frame type
         {
        	 size_t idx=0;
        	 for (auto frameType : GUI->m_frameTypes){
        		 if ( instance.getINDIPropertyItem( m_Device, "CCD_FRAME_TYPE", frameType, CCDProp ) ){
        			 if (CCDProp.PropertyValue == "ON"){
        				 GUI->CCDFrameType_Label.Enable();
           				 GUI->CCDFrameType_Combo.SetCurrentItem(idx);
           				 GUI->CCDFrameType_Combo.Enable();
        				 break;
        			 }
        		 }
        		 idx++;
        	 }
         }
         // get upload directory
         if ( instance.getINDIPropertyItem( m_Device, "UPLOAD_SETTINGS", "UPLOAD_DIR", CCDProp ) ){
        	 GUI->UploadDir_Edit.SetText(CCDProp.PropertyValue);
        	 if (GUI->m_updateNewDirStr){
        		 GUI->UploadNewDir_Edit.SetText(CCDProp.PropertyValue);
        	 }
         }
         // get upload settings
         if ( instance.getINDIPropertyItem( m_Device, "UPLOAD_SETTINGS", "UPLOAD_PREFIX", CCDProp ) ){
				GUI->UploadPrefix_Edit.SetText(CCDProp.PropertyValue);
				updateNewPrefixControl();
         }
      }
}

String CCDFrameInterface::getUploadModePropertyString(int UploadModeIdx) {
	String PropertyElement;
	switch (UploadModeIdx) {
	case GUIData::UploadClient:
		PropertyElement = "UPLOAD_CLIENT";
		break;
	case GUIData::UploadServer:
		PropertyElement = "UPLOAD_LOCAL";
		break;
	case GUIData::UploadBoth:
		PropertyElement = "UPLOAD_BOTH";
		break;
	}
	return PropertyElement;
}

String CCDFrameInterface::getCCDFrameTypePropertyString(int FrameTypeIdx) {
	String PropertyElement;
	switch (FrameTypeIdx) {
	case GUIData::Light:
		PropertyElement = "FRAME_LIGHT";
		break;
	case GUIData::Bias:
		PropertyElement = "FRAME_BIAS";
		break;
	case GUIData::Dark:
		PropertyElement = "FRAME_DARK";
		break;
	case GUIData::Flat:
		PropertyElement = "FRAME_FLAT";
		break;
	}
	return PropertyElement;
}

void CCDFrameInterface::ComboItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->CCDDevice_Combo )
   {
      m_Device = sender.ItemText( itemIndex );
      if ( TheINDIDeviceControllerInterface != nullptr )
      {
         INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;
         INDIPropertyListItem CCDProp;
         // check for cooler connection (e.g. Atik cameras)
         GUI->Temperature_Timer.Start();
         if ( instance.getINDIPropertyItem( m_Device, "COOLER_CONNECTION", "CONNECT_COOLER", CCDProp ) )
            if( CCDProp.PropertyValue == "OFF" )
            {
               INDINewPropertyListItem newPropertyListItem;
               newPropertyListItem.Device = m_Device;
               newPropertyListItem.Property = "COOLER_CONNECTION";
               newPropertyListItem.Element = "CONNECT_COOLER";
               newPropertyListItem.PropertyType = "INDI_SWITCH";
               newPropertyListItem.NewPropertyValue = "ON";
               instance.sendNewPropertyValue( newPropertyListItem, true );
            }
      }
   } else if (sender == GUI->CCDBinY_Combo){
	   if ( TheINDIDeviceControllerInterface != nullptr ){
			INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;
			INDIPropertyListItem CCDProp;
		if ( instance.getINDIPropertyItem( m_Device, "CCD_BINNING", "VER_BIN", CCDProp ) ){
			INDINewPropertyListItem newPropertyListItem;
			newPropertyListItem.Device = m_Device;
			newPropertyListItem.Property = "CCD_BINNING";
			newPropertyListItem.Element = "VER_BIN";
			newPropertyListItem.PropertyType = "INDI_NUMBER";
			newPropertyListItem.NewPropertyValue = GUI->CCDBinY_Combo.ItemText(itemIndex);
			instance.sendNewPropertyValue(newPropertyListItem, true);
			// Enable updates
			GUI->m_updateNewPrefixStr=true;
		}
	   }
   } else if (sender == GUI->CCDBinX_Combo){
	   if ( TheINDIDeviceControllerInterface != nullptr ){
			INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;
			INDIPropertyListItem CCDProp;
		if ( instance.getINDIPropertyItem( m_Device, "CCD_BINNING", "HOR_BIN", CCDProp ) ){
			INDINewPropertyListItem newPropertyListItem;
			newPropertyListItem.Device = m_Device;
			newPropertyListItem.Property = "CCD_BINNING";
			newPropertyListItem.Element = "HOR_BIN";
			newPropertyListItem.PropertyType = "INDI_NUMBER";
			newPropertyListItem.NewPropertyValue = GUI->CCDBinX_Combo.ItemText(itemIndex);
			instance.sendNewPropertyValue(newPropertyListItem, true);
			// Enable updates
			GUI->m_updateNewPrefixStr=true;
		}
	   }
   } else if (sender == GUI->UploadMode_Combo){
	   if ( TheINDIDeviceControllerInterface != nullptr ){
			INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;
			INDIPropertyListItem CCDProp;
			String PropertyElement = getUploadModePropertyString(itemIndex);
		if ( instance.getINDIPropertyItem( m_Device, "UPLOAD_MODE", PropertyElement, CCDProp ) ){
			INDINewPropertyListItem newPropertyListItem;
			newPropertyListItem.Device = m_Device;
			newPropertyListItem.Property = "UPLOAD_MODE";
			newPropertyListItem.Element = PropertyElement;;
			newPropertyListItem.PropertyType = "INDI_SWITCH";
			newPropertyListItem.NewPropertyValue = "ON";
			instance.sendNewPropertyValue(newPropertyListItem, true);
		}
	   }
   } else if (sender == GUI->CCDFrameType_Combo){
	   if ( TheINDIDeviceControllerInterface != nullptr ){
			INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;
			INDIPropertyListItem CCDProp;
			String PropertyElement = getCCDFrameTypePropertyString(itemIndex);
		if ( instance.getINDIPropertyItem( m_Device, "CCD_FRAME_TYPE", PropertyElement, CCDProp ) ){
			INDINewPropertyListItem newPropertyListItem;
			newPropertyListItem.Device = m_Device;
			newPropertyListItem.Property = "CCD_FRAME_TYPE";
			newPropertyListItem.Element = PropertyElement;;
			newPropertyListItem.PropertyType = "INDI_SWITCH";
			newPropertyListItem.NewPropertyValue = "ON";
			instance.sendNewPropertyValue(newPropertyListItem, true);
			//Enable updates
			GUI->m_updateNewPrefixStr=true;
		}
	   }
   }

}

void CCDFrameInterface::SetCCDPropertyButton_Click(Button& sender, bool checked){

	if (sender == GUI->CCDTemp_PushButton){
		INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;

		INDINewPropertyListItem newPropertyListItem;
	    newPropertyListItem.Device = m_Device;
	    newPropertyListItem.Property = "CCD_TEMPERATURE";
	    newPropertyListItem.Element = "CCD_TEMPERATURE_VALUE";
	    newPropertyListItem.PropertyType = "INDI_NUMBER";
	    newPropertyListItem.NewPropertyValue = String( GUI->CCDTargetTemp_NumericEdit.Value() );
	    if (!instance.sendNewPropertyValue( newPropertyListItem, true/*isAsynchCall*/ )){
	    	Console().WriteLn(ERR_MSG("Error while sending new property values to INDI server."));
	    }
	    // check send_ok
	} else if (sender == GUI->UploadNewDir_PushButton){
		INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;

		INDINewPropertyListItem newPropertyListItem;
	    newPropertyListItem.Device = m_Device;
	    newPropertyListItem.Property = "UPLOAD_SETTINGS";
	    newPropertyListItem.Element = "UPLOAD_DIR";
	    newPropertyListItem.PropertyType = "INDI_TEXT";
	    newPropertyListItem.NewPropertyValue = GUI->UploadNewDir_Edit.Text();
	    if(!instance.sendNewPropertyValue( newPropertyListItem, true/*isAsynchCall*/ )){
	    	Console().WriteLn(ERR_MSG("Error while sending new property values to INDI server."));
	    }
	    GUI->m_updateNewDirStr=true;
	    // check send_ok
	} else if (sender == GUI->UploadNewPrefix_PushButton){
		INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;

		INDINewPropertyListItem newPropertyListItem;
	    newPropertyListItem.Device = m_Device;
	    newPropertyListItem.Property = "UPLOAD_SETTINGS";
	    newPropertyListItem.Element = "UPLOAD_PREFIX";
	    newPropertyListItem.PropertyType = "INDI_TEXT";
	    newPropertyListItem.NewPropertyValue = GUI->UploadNewPrefix_Edit.Text();
	    if(!instance.sendNewPropertyValue( newPropertyListItem, true/*isAsynchCall*/ )){
	    	Console().WriteLn(ERR_MSG("Error while sending new property values to INDI server."));
	    }
	    GUI->m_updateNewPrefixStr=true;
	    // check send_ok
	}

}

void CCDFrameInterface::CancelButton_Click( Button& sender, bool checked )
{
   INDINewPropertyListItem newPropertyListItem;
   newPropertyListItem.Device = m_Device;
   newPropertyListItem.Property = "CCD_ABORT_EXPOSURE";
   newPropertyListItem.Element = "ABORT";
   newPropertyListItem.PropertyType = "INDI_SWITCH";
   newPropertyListItem.NewPropertyValue = "ON";

   INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;
   instance.sendNewPropertyValue( newPropertyListItem, true );
   instance.doInternalAbort();
}

void CCDFrameInterface::StartExposureButton_Click( Button& sender, bool checked )
{
   if ( TheINDIDeviceControllerInterface != nullptr )
   {
      String tmpDir = File::SystemTempDirectory();
      INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;

      GUI->StartExposure_PushButton.Disable();

      INDIPropertyListItem uploadLocal;
      bool serverSendsImage = true;
      if ( instance.getINDIPropertyItem( m_Device, "UPLOAD_MODE", "UPLOAD_LOCAL", uploadLocal ) )
         serverSendsImage = uploadLocal.PropertyValue != "ON";

      for ( int num = 0; num < m_NumOfExposures; ++num )
      {
         GUI->ExpFrame_Edit.SetText( String( num ) );

         INDINewPropertyListItem newPropertyListItem;
         newPropertyListItem.Device = m_Device;
         newPropertyListItem.Property = "CCD_EXPOSURE";
         newPropertyListItem.Element = "CCD_EXPOSURE_VALUE";
         newPropertyListItem.PropertyType = "INDI_NUMBER";
         newPropertyListItem.NewPropertyValue = String( m_ExposureDuration );

         GUI->ExposureDuration_Timer.Start();
         if(!instance.sendNewPropertyValue( newPropertyListItem, true/*isAsynchCall*/ )){
        	 Console().WriteLn(ERR_MSG("Error while sending new property values to INDI server."));
         }

         if ( serverSendsImage )
         {
            while ( !instance.getImageDownloadedFlag() && !instance.getInternalAbortFlag() )
            {
               ProcessEvents();
            }
            instance.setImageDownloadedFlag( false );
         }
         else
         {
            INDIPropertyListItem ccdExposure;
            bool serverExposureIsBusy = false;
            // timing problem: wait until server sends BUSY
            do
            {
               if (instance.getINDIPropertyItem( m_Device, "CCD_EXPOSURE", "CCD_EXPOSURE_VALUE", ccdExposure, false)){
            	   serverExposureIsBusy = ccdExposure.PropertyState == IPS_BUSY ;
               }
               ProcessEvents();

            }
            while ( !serverExposureIsBusy && (m_timeoutCounter < pcl_timeout) && !instance.getInternalAbortFlag() );

            // start exposure
            do
            {
               instance.getINDIPropertyItem( m_Device, "CCD_EXPOSURE", "CCD_EXPOSURE_VALUE", ccdExposure, false);
               ProcessEvents();
            }
            while ( ccdExposure.PropertyState == IPS_BUSY && !instance.getInternalAbortFlag() );
         }

         if ( instance.getInternalAbortFlag() )
         {
            // stop timer
            GUI->ExposureDuration_Timer.Stop();
            GUI->ExposureDelay_Timer.Stop();
            break;
         }

         if ( serverSendsImage )
         {
            Array<ImageWindow> imgArray = ImageWindow::Open( tmpDir + "/Image.fits", "image" );
            if ( !imgArray.IsEmpty() )
            {
               imgArray[0].ZoomToFit( false ); // don't allow zoom > 1
               imgArray[0].Show();
            }

         }

         // wait until next exposure
         if ( GUI->ExpDelayTime_NumericEdit.Value() > GUI->ExpDelayTime_NumericEdit.LowerBound() )
         {
            GUI->ExposureDelay_Timer.Start();
            m_isWaiting=true;
         }

         while ( m_isWaiting ){
            ProcessEvents();
         }
      }

      instance.setInternalAbortFlag( false );
      instance.setImageDownloadedFlag( false );

      GUI->StartExposure_PushButton.Enable();
   }
}

void CCDFrameInterface::SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->ExpNum_SpinBox )
      m_NumOfExposures = value;
}

void CCDFrameInterface::EditValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->CCDTemp_NumericEdit )
   {
   }
   else if ( sender == GUI->ExpTime_NumericEdit )
   {
      m_ExposureDuration = value;
      GUI->m_updateNewPrefixStr=true;
      updateNewPrefixControl();

   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CCDFrameInterface.cpp - Released 2015/10/13 15:55:45 UTC
