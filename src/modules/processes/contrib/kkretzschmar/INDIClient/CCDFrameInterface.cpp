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
   m_saveFrame = false;
   m_FrameFolder = "./";
   m_FramePrefix = "Image";
   m_isWaiting = false;
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
   CCDTemp_NumericEdit.SetRange( -30, 0 );
   CCDTemp_NumericEdit.label.SetText( "Temperature:" );
   CCDTemp_NumericEdit.label.SetFixedWidth( labelWidth1 );
   CCDTemp_NumericEdit.edit.SetFixedWidth( 10*emWidth );
   CCDTemp_NumericEdit.sizer.AddStretch();
   CCDTemp_NumericEdit.SetToolTip( "<p>Current chip temperature in degrees Celsius.</p>" );
   CCDTemp_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CCDFrameInterface::EditValueUpdated, w );
   CCDTemp_NumericEdit.Disable();

   CCDBinX_NumericEdit.SetInteger();
   CCDBinX_NumericEdit.SetRange( 1, 4 );
   CCDBinX_NumericEdit.label.SetText( "Binning X:" );
   CCDBinX_NumericEdit.label.SetFixedWidth( labelWidth1 );
   CCDBinX_NumericEdit.edit.SetFixedWidth( 10*emWidth );
   CCDBinX_NumericEdit.sizer.AddStretch();
   CCDBinX_NumericEdit.SetToolTip( "<p>Horizontal pixel binning factor.</p>" );
   CCDBinX_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CCDFrameInterface::EditValueUpdated, w );
   CCDBinX_NumericEdit.Disable();

   CCDBinY_NumericEdit.SetInteger();
   CCDBinY_NumericEdit.SetRange( 1, 4 );
   CCDBinY_NumericEdit.label.SetText( "Binning Y:" );
   CCDBinY_NumericEdit.label.SetFixedWidth( labelWidth1 );
   CCDBinY_NumericEdit.edit.SetFixedWidth( 10*emWidth );
   CCDBinY_NumericEdit.sizer.AddStretch();
   CCDBinY_NumericEdit.SetToolTip( "<p>Vertical pixel binning factor.</p>" );
   CCDBinY_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CCDFrameInterface::EditValueUpdated, w );
   CCDBinY_NumericEdit.Disable();

   CCDParam_Sizer.SetSpacing( 4 );
   CCDParam_Sizer.Add( CCDTemp_NumericEdit );
   CCDParam_Sizer.Add( CCDBinX_NumericEdit );
   CCDParam_Sizer.Add( CCDBinY_NumericEdit );

   //

   FrameExposure_SectionBar.SetTitle( "Frame Shooting" );
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
   //ExpNum_SpinBox.Disable();

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

   UpdateDeviceList_Timer.SetInterval( 0.5 );
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
   Temperature_Timer.OnTimer( (Timer::timer_event_handler)&CCDFrameInterface::Temperature_Timer, w );
}

void CCDFrameInterface::UpdateDeviceList()
{
   if ( TheINDIDeviceControllerInterface != nullptr )
   {
      INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;
      if ( !instance.p_deviceList.IsEmpty() )
         for ( INDIDeviceControllerInstance::DeviceListType::iterator iter = instance.p_deviceList.Begin();
               iter != instance.p_deviceList.End();
               ++iter )
            GUI->CCDDevice_Combo.AddItem( iter->DeviceName );
      GUI->UpdateDeviceList_Timer.Stop();
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
      if ( sender.Count()*sender.Interval() >= GUI->ExpTime_NumericEdit.Value() )
         GUI->ExposureDuration_Timer.Stop();
   }
}

void CCDFrameInterface::ExposureDelay_Timer( Timer &sender )
{
   if ( sender == GUI->ExposureDelay_Timer )
   {
      GUI->ExpDelay_Edit.SetText( String( sender.Count() + GUI->ExpDur_Edit.Text().ToDouble() ) );
      if ( sender.Count()*sender.Interval() >= GUI->ExpDelayTime_NumericEdit.Value() )
      {
         GUI->ExposureDelay_Timer.Stop();
         m_isWaiting = false;
      }
   }
}

void CCDFrameInterface::Temperature_Timer( Timer &sender )
{
   if ( TheINDIDeviceControllerInterface != nullptr)
      if ( sender == GUI->Temperature_Timer )
      {
         INDINewPropertyListItem newPropertyListItem;
         INDIDeviceControllerInstance& instance = TheINDIDeviceControllerInterface->instance;
         INDIPropertyListItem CCDProp;
         // get temperature value
         if ( instance.getINDIPropertyItem( m_Device, "CCD_TEMPERATURE", "CCD_TEMPERATURE_VALUE", CCDProp ) )
            GUI->CCDTemp_NumericEdit.SetValue( CCDProp.PropertyValue.ToDouble() );
         // get X binning value
         if ( instance.getINDIPropertyItem( m_Device, "CCD_BINNING", "HOR_BIN", CCDProp ) )
            GUI->CCDBinX_NumericEdit.SetValue( CCDProp.PropertyValue.ToInt() );
         // get X binning value
         if ( instance.getINDIPropertyItem( m_Device, "CCD_BINNING", "VER_BIN", CCDProp ) )
            GUI->CCDBinY_NumericEdit.SetValue( CCDProp.PropertyValue.ToInt() );
      }
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
         bool send_ok = instance.sendNewPropertyValue( newPropertyListItem, true/*isAsynchCall*/ );
         if ( !send_ok )
            break;

         // TODO enable abort
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
            // timimg problem: wait until server sends BUSY
            do
            {
               instance.getINDIPropertyItem( m_Device, "CCD_EXPOSURE", "CCD_EXPOSURE_VALUE", ccdExposure );
               ProcessEvents();
               serverExposureIsBusy = ccdExposure.PropertyState == IPS_BUSY ;
            }
            while ( !serverExposureIsBusy && (GUI->ExpDur_Edit.Text().ToFloat() < pcl_timeout) && !instance.getInternalAbortFlag() );

            do
            {
               instance.getINDIPropertyItem( m_Device, "CCD_EXPOSURE", "CCD_EXPOSURE_VALUE", ccdExposure );
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

            if ( m_saveFrame )
            {
               String source = tmpDir + "/Image.fits";
               String dest   = m_FrameFolder + this->m_FramePrefix + '_' + String( num ) + ".fits";
               File::CopyFile( dest, source );
            }
         }

         // wait until next exposure
         if ( GUI->ExpDelayTime_NumericEdit.Value() > 0 )
         {
            GUI->ExposureDelay_Timer.Start();
            m_isWaiting=true;
         }

         while ( m_isWaiting )
            ProcessEvents();
      }

      instance.setInternalAbortFlag( false );

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
   }
   else if ( sender == GUI->ExpDelayTime_NumericEdit )
   {
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CCDFrameInterface.cpp - Released 2015/10/13 15:55:45 UTC
