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

#include "CCDFrameInterface.h"

#include "basedevice.h"
#include "indibase.h"
#include "indiapi.h"
#include <pcl/Console.h>
#include <assert.h>
#include <fstream>

#include "CCDFrameProcess.h"
#include "PixInsightINDIInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

CCDFrameInterface* TheCCDFrameInterface = 0;

// ----------------------------------------------------------------------------

//#include "PixInsightINDIIcon.xpm"

// ----------------------------------------------------------------------------

CCDFrameInterface::CCDFrameInterface() :
ProcessInterface(), instance( TheCCDFrameProcess),GUI( 0 )
{
   TheCCDFrameInterface = this;
   m_NumOfExposures=1;
   m_Temperature=0;
   m_saveFrame=false;
   m_FrameFolder=String("./");
   m_FramePrefix=String("Image");
   m_isWaiting=false;
}

CCDFrameInterface::~CCDFrameInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString CCDFrameInterface::Id() const
{
   return "CCDFrame";
}

MetaProcess* CCDFrameInterface::Process() const
{
   return TheCCDFrameProcess;
}

const char** CCDFrameInterface::IconImageXPM() const
{
   return 0; // PixInsightINDIIcon_XPM; ---> put a nice icon here
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
   PixInsightINDIInstance defaultInstance( TheCCDFrameProcess );
   ImportProcess( defaultInstance );
}

bool CCDFrameInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData(*this );
      SetWindowTitle( "INDI CCD Frames" );
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
   if ( r == 0 )
   {
      whyNot = "Not a CCDFrame instance.";
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

CCDFrameInterface::GUIData::GUIData(CCDFrameInterface& w ){

	CCDDevice_SectionBar.SetTitle("INDI CCD Device Selection");
	CCDDevice_SectionBar.SetSection(CCDDevice_Control);
	CCDDevice_Control.SetSizer(CCDDevice_Sizer);

	CCDDevice_Label.SetText( "INDI CCD Device:" );
	CCDDevice_Label.SetToolTip( "<p>Select an INDI CCD device.</p>" );
	CCDDevice_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

	CCDDevice_Combo.OnItemSelected((ComboBox::item_event_handler)& CCDFrameInterface::ComboItemSelected,w);

	CCDDevice_Sizer.SetSpacing(10);
	CCDDevice_Sizer.SetMargin(10);
	CCDDevice_Sizer.Add(CCDDevice_Label);
	CCDDevice_Sizer.AddSpacing(10);
	CCDDevice_Sizer.Add(CCDDevice_Combo);

	CCDParam_SectionBar.SetTitle("CCD Parameters");
	CCDParam_SectionBar.SetSection(CCDParam_Control);
	CCDParam_Control.SetSizer(CCDParam_Sizer);

	CCDTemp_Label.SetText( "Temperature:" );
	CCDTemp_Label.SetToolTip( "<p>Current chip temperature in Celsius.</p>" );
	CCDTemp_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

	CCDTemp_Edit.SetMinWidth( 10);
	CCDTemp_Edit.Disable();

	CCDTemp_Sizer.SetSpacing(10);
	CCDTemp_Sizer.SetMargin(10);
	CCDTemp_Sizer.Add(CCDTemp_Label);
	CCDTemp_Sizer.AddStretch();
	CCDTemp_Sizer.Add(CCDTemp_Edit);
	CCDParam_Sizer.Add(CCDTemp_Sizer);

	CCDBin_Label.SetText( "Binning: " );
	CCDBin_Label.SetToolTip( "<p>Binning X (horizontal) and Y (vertical)</p>" );
	CCDBin_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );


	CCDBinX_Label.SetText( "X: " );
	CCDBinX_Label.SetToolTip( "<p>Binning X (horizontal)</p>" );
	CCDBinX_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

	CCDBinX_Edit.SetMinWidth( 5);
	CCDBinX_Edit.Disable();

	CCDBinY_Label.SetText( "Y: " );
	CCDBinY_Label.SetToolTip( "<p>Binning Y (vertical)</p>" );
	CCDBinY_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

	CCDBinY_Edit.SetMinWidth( 5);
	CCDBinY_Edit.Disable();

	CCDBinning_Sizer.SetSpacing(10);
	CCDBinning_Sizer.SetMargin(10);
	CCDBinning_Sizer.Add(CCDBin_Label);
	CCDBinning_Sizer.AddStretch();
	CCDBinning_Sizer.Add(CCDBinX_Label);
	CCDBinning_Sizer.Add(CCDBinX_Edit);
	CCDBinning_Sizer.Add(CCDBinY_Label);
	CCDBinning_Sizer.Add(CCDBinY_Edit);

	CCDParam_Sizer.Add(CCDBinning_Sizer);

	FrameExposure_SectionBar.SetTitle("Frame Shooting");
	FrameExposure_SectionBar.SetSection(FrameExposure_Control);
	FrameExposure_Control.SetSizer(FrameExposure_Sizer);

	ExpTime_Label.SetText( "Exposure Time:" );
	ExpTime_Label.SetToolTip( "<p>Specify exposure time in seconds.</p>" );
	ExpTime_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

	ExpTime_Edit.SetText("0");
	ExpTime_Edit.SetMinWidth( 30);
	ExpTime_Edit.OnEditCompleted( (Edit::edit_event_handler)&CCDFrameInterface::EditCompleted, w );

	ExpDelayTime_Label.SetText( "Delay:" );
	ExpDelayTime_Label.SetToolTip( "<p>Specify waiting time between exposures in seconds.</p>" );
	ExpDelayTime_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

	ExpDelayTime_Edit.SetText("0");
	ExpDelayTime_Edit.SetMinWidth( 30);
	ExpDelayTime_Edit.OnEditCompleted( (Edit::edit_event_handler)&CCDFrameInterface::EditCompleted, w );


	ExpTime_Sizer.SetSpacing(10);
	ExpTime_Sizer.SetMargin(10);
	ExpTime_Sizer.Add(ExpTime_Label);
	ExpTime_Sizer.Add(ExpTime_Edit);
	ExpTime_Sizer.Add(ExpDelayTime_Label);
	ExpTime_Sizer.Add(ExpDelayTime_Edit);
	ExpTime_Sizer.Add(ExpNum_Label);
	ExpTime_Sizer.Add(ExpNum_Edit);

	ExpNum_Label.SetText( "Number of frames:" );
	ExpNum_Label.SetToolTip( "<p>Number of frames to be taken.</p>" );
	ExpNum_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

	ExpNum_Edit.SetText("0");
	ExpNum_Edit.SetMinWidth( 30);
	ExpNum_Edit.OnEditCompleted( (Edit::edit_event_handler)&CCDFrameInterface::EditCompleted, w );

	ExpNum_Sizer.SetSpacing(10);
	ExpNum_Sizer.SetMargin(10);
	
	StartExposure_PushButton.SetText( "Start Exposure" );
    StartExposure_PushButton.OnClick( (Button::click_event_handler) &CCDFrameInterface::StartExposureButton_Click, w );
    CancelExposure_PushButton.SetText( "Cancel" );
    CancelExposure_PushButton.OnClick( (Button::click_event_handler) &CCDFrameInterface::CancelButton_Click, w );



	ExpButton_Sizer.SetSpacing(10);
	ExpButton_Sizer.SetMargin(10);
	ExpButton_Sizer.Add(StartExposure_PushButton);
	ExpButton_Sizer.Add(CancelExposure_PushButton);

	ExpDur_Label.SetText( "Timer:" );
	ExpDur_Label.SetToolTip( "<p>Exposure time in seconds.</p>" );
	ExpDur_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

	ExpDur_Edit.SetMinWidth(15);
	ExpDur_Edit.SetText(String("0"));
	ExpDur_Edit.SetReadOnly();

	ExpDelay_Label.SetText("Delay:");
	ExpDelay_Label.SetToolTip("<p>Waiting time before next exposure in seconds.</p>");
	ExpDelay_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

	ExpDelay_Edit.SetMinWidth(15);
	ExpDelay_Edit.SetText(String("0"));
	ExpDelay_Edit.SetReadOnly();


	ExpFrame_Label.SetText( "Frame:" );
	ExpFrame_Label.SetToolTip( "<p>Frame number.</p>" );
	ExpFrame_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

	ExpFrame_Edit.SetMinWidth( 15);
	ExpFrame_Edit.SetText(String("0"));
	ExpFrame_Edit.SetReadOnly();
	
	ExpDur_Sizer.SetSpacing(10);
	ExpDur_Sizer.SetMargin(10);
	ExpDur_Sizer.Add(ExpDur_Label);
	ExpDur_Sizer.Add(ExpDur_Edit);
	ExpDur_Sizer.Add(ExpDelay_Label);
	ExpDur_Sizer.Add(ExpDelay_Edit);
	ExpDur_Sizer.Add(ExpFrame_Label);
	ExpDur_Sizer.Add(ExpFrame_Edit);
	ExpDur_Sizer.AddStretch();

	SaveImage_CheckBoxLabel.SetText("Save Frames:");
	SaveImage_CheckBoxLabel.SetToolTip("<p>Enable saving the frames to disk.</p>");
	SaveImage_CheckBoxLabel.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

	SaveImage_CheckBox.OnCheck((CheckBox::check_event_handler )&CCDFrameInterface::CheckBoxChecked,w);

	ImagePath_Label.SetText("Frame Folder:");
	ImagePath_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
	ImagePath_Label.SetToolTip("<p>Specify the directory where to save the frames.</p>");

	ImagePath_Edit.SetText("./");
	ImagePath_Edit.OnEditCompleted( (Edit::edit_event_handler)&CCDFrameInterface::EditCompleted, w );

	ImagePrefix_Label.SetText("Image Prefix:");
	ImagePrefix_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
	ImagePrefix_Label.SetToolTip("<p>Specify a prefix for the frame file name.</p>");

	ImagePrefix_Edit.SetText("Image");
	ImagePrefix_Edit.OnEditCompleted( (Edit::edit_event_handler)&CCDFrameInterface::EditCompleted, w );

	Image_Sizer.SetSpacing(10);
	Image_Sizer.SetMargin(10);
	Image_Sizer.Add(SaveImage_CheckBoxLabel);
	Image_Sizer.Add(SaveImage_CheckBox);
	Image_Sizer.Add(ImagePath_Label);
	Image_Sizer.Add(ImagePath_Edit);
	Image_Sizer.Add(ImagePrefix_Label);
	Image_Sizer.Add(ImagePrefix_Edit);
	Image_Sizer.AddStretch();


	FrameExposure_Sizer.Add(ExpTime_Sizer);
	FrameExposure_Sizer.Add(ExpNum_Sizer);
	FrameExposure_Sizer.Add(Image_Sizer);
	FrameExposure_Sizer.Add(ExpButton_Sizer);
	FrameExposure_Sizer.Add(ExpDur_Sizer);

	Global_Sizer.SetMargin( 8 );
	Global_Sizer.SetSpacing( 6 );
	Global_Sizer.Add(CCDDevice_SectionBar);
	Global_Sizer.Add(CCDDevice_Control);
	Global_Sizer.Add(CCDParam_SectionBar);
	Global_Sizer.Add(CCDParam_Control);
	Global_Sizer.Add(FrameExposure_SectionBar);
	Global_Sizer.Add(FrameExposure_Control);

	w.SetSizer(Global_Sizer);
	
	UpdateDeviceList_Timer.SetInterval( 0.5 );
    UpdateDeviceList_Timer.SetPeriodic( true );
    UpdateDeviceList_Timer.OnTimer( (Timer::timer_event_handler)&CCDFrameInterface::UpdateDeviceList_Timer, w );

	UpdateDeviceList_Timer.Start();


	ExposureDuration_Timer.SetInterval( 1 );
    ExposureDuration_Timer.SetPeriodic( true );
    ExposureDuration_Timer.OnTimer( (Timer::timer_event_handler)&CCDFrameInterface::ExposureDuration_Timer, w );

	ExposureDelay_Timer.SetInterval(1);
	ExposureDelay_Timer.SetPeriodic(true);
	ExposureDelay_Timer.OnTimer((Timer::timer_event_handler) &CCDFrameInterface::ExposureDelay_Timer, w);


    Temperature_Timer.SetInterval( 1 );
    Temperature_Timer.SetPeriodic( true );
    Temperature_Timer.OnTimer( (Timer::timer_event_handler)&CCDFrameInterface::Temperature_Timer, w );


	

}

void CCDFrameInterface::UpdateDeviceList(){

	if (ThePixInsightINDIInterface!=0){
		PixInsightINDIInstance* pInstance=&ThePixInsightINDIInterface->instance;

		if (pInstance->p_deviceList.Begin()==pInstance->p_deviceList.End())
			return;

		for (PixInsightINDIInstance::DeviceListType::iterator iter=pInstance->p_deviceList.Begin() ; iter!=pInstance->p_deviceList.End(); ++iter){																																																																																																																																														
			GUI->CCDDevice_Combo.AddItem(iter->DeviceName);
		}
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
	if( sender == GUI->ExposureDuration_Timer  ){
		GUI->ExpDur_Edit.SetText(String(sender.Count()));
		if (sender.Count()>=GUI->ExpTime_Edit.Text().ToDouble()){
			GUI->ExposureDuration_Timer.Stop();
		}
	}
}

void CCDFrameInterface::ExposureDelay_Timer( Timer &sender )
{
	if( sender == GUI->ExposureDelay_Timer  ){
		GUI->ExpDelay_Edit.SetText(String(sender.Count()+GUI->ExpDur_Edit.Text().ToDouble()));
		if (sender.Count()>=GUI->ExpDelayTime_Edit.Text().ToDouble()){
			GUI->ExposureDelay_Timer.Stop();
			m_isWaiting=false;
		}
	}
}

void CCDFrameInterface::Temperature_Timer( Timer &sender )
{
	if (ThePixInsightINDIInterface != 0) {
		if (sender == GUI->Temperature_Timer) {
			INDINewPropertyListItem newPropertyListItem;
			// get temperature value
			PixInsightINDIInstance* pInstance =
					&ThePixInsightINDIInterface->instance;

			if (pInstance==NULL)
				return;

			INDIPropertyListItem CCDProp;
			// get temperature value
			if (pInstance->getINDIPropertyItem(m_Device,
					"CCD_TEMPERATURE", "CCD_TEMPERATURE_VALUE", CCDProp)) {
				GUI->CCDTemp_Edit.SetText(CCDProp.PropertyValue);
			}
			else{
				sender.Stop();
				return;
			}
			// get X binning value
			if (pInstance->getINDIPropertyItem(m_Device,
								"CCD_BINNING", "HOR_BIN", CCDProp)) {
				GUI->CCDBinX_Edit.SetText(CCDProp.PropertyValue);
			}
			// get X binning value
			if (pInstance->getINDIPropertyItem(m_Device,
											"CCD_BINNING", "VER_BIN", CCDProp)) {
				GUI->CCDBinY_Edit.SetText(CCDProp.PropertyValue);
			}
		}
	}
}

void CCDFrameInterface::ComboItemSelected(ComboBox& sender, int itemIndex) {
	if (sender == GUI->CCDDevice_Combo){
		m_Device = sender.ItemText(itemIndex);
		if (ThePixInsightINDIInterface != 0) {

			PixInsightINDIInstance* pInstance = &ThePixInsightINDIInterface->instance;

			if (pInstance==NULL)
				return;

			INDIPropertyListItem CCDProp;
			// check for cooler connection (e.g. Atik cameras)
			if (pInstance->getINDIPropertyItem(m_Device,
					"COOLER_CONNECTION", "CONNECT_COOLER", CCDProp)) {
				if(CCDProp.PropertyValue==String("OFF")){
					INDINewPropertyListItem newPropertyListItem;
					newPropertyListItem.Device=m_Device;
					newPropertyListItem.Property = String("COOLER_CONNECTION");
					newPropertyListItem.Element = String("CONNECT_COOLER");
					newPropertyListItem.PropertyType = String("INDI_SWITCH");

					newPropertyListItem.NewPropertyValue = String("ON");
					pInstance->sendNewPropertyValue(newPropertyListItem);
				}
			}
			GUI->Temperature_Timer.Start();
		}
	}
}

void CCDFrameInterface::CancelButton_Click(Button& sender, bool checked){
	PixInsightINDIInstance* pInstance=&ThePixInsightINDIInterface->instance;

	if (pInstance==NULL)
		return;

	INDINewPropertyListItem newPropertyListItem;
	newPropertyListItem.Device = m_Device;
	newPropertyListItem.Property = String("CCD_ABORT_EXPOSURE");
	newPropertyListItem.Element = String("ABORT");
	newPropertyListItem.PropertyType = String("INDI_SWITCH");

	newPropertyListItem.NewPropertyValue = String("ON");
	pInstance->sendNewPropertyValue(newPropertyListItem);


	// stop timer
	GUI->ExposureDuration_Timer.Stop();

	pInstance->doInternalAbort();

}

 void CCDFrameInterface::StartExposureButton_Click(Button& sender, bool checked){
	 if (ThePixInsightINDIInterface!=0){
#if defined(WIN32) 
	  const char* tmpDir = getenv("TMP");
#else
	  const char* tmpDir = getenv("TMPDIR");
#endif
	   if (tmpDir!=NULL) {
		PixInsightINDIInstance* pInstance=&ThePixInsightINDIInterface->instance;
		if (pInstance==NULL)
			return;
		GUI->StartExposure_PushButton.Disable();
		INDIPropertyListItem imageTransfer;
		bool serverSendsImage=true;
		if (pInstance->getINDIPropertyItem(m_Device,"TRANSFER_FORMAT","NONE",imageTransfer)){
			serverSendsImage=!(imageTransfer.PropertyValue==String("ON"));
		}

		for (int num=0; num<m_NumOfExposures;++num){
			GUI->ExpFrame_Edit.SetText(String(num));

			INDINewPropertyListItem newPropertyListItem;
			newPropertyListItem.Device=m_Device;
			newPropertyListItem.Property=String("CCD_EXPOSURE");
			newPropertyListItem.Element=String("CCD_EXPOSURE_VALUE");
			newPropertyListItem.PropertyType=String("INDI_NUMBER");
			newPropertyListItem.NewPropertyValue=String(m_ExposureDuration);

			GUI->ExposureDuration_Timer.Start();
			bool send_ok = pInstance->sendNewPropertyValue(newPropertyListItem);

			if (!send_ok){
				break;
			}

			if (serverSendsImage) {
				Array<ImageWindow> imgArray = ImageWindow::Open(String(tmpDir)+ String("/Image.fits"), IsoString("image"));

				if (imgArray.Length()!=0){
					imgArray[0].ZoomToFit( false ); // don't allow zoom > 1
					imgArray[0].Show();
				}

				if (m_saveFrame) {
					IsoString source = IsoString(String(tmpDir)+ String("/Image.fits"));
					IsoString dest   = IsoString(m_FrameFolder) + IsoString(this->m_FramePrefix) + IsoString("_") +  IsoString(num) + IsoString(".fits");

					std::ifstream infile(source.c_str(), std::ifstream::binary);
					std::ofstream outfile(dest.c_str(), std::ofstream::binary);

					// get size of file
					infile.seekg(0, infile.end);
					long size = infile.tellg();
					infile.seekg(0);

					// allocate memory for file content
					char* buffer = new char[size];

					// read content of infile
					infile.read(buffer, size);

					// write to outfile
					outfile.write(buffer, size);

					// release dynamically-allocated memory
					delete[] buffer;

					outfile.close();
					infile.close();
				}
			}
			// wait until next exposure
			if (GUI->ExpDelayTime_Edit.Text().ToInt()!=0){
				GUI->ExposureDelay_Timer.Start();
				m_isWaiting=true;
			}
			while (m_isWaiting){ProcessEvents();}
		}
		pInstance->setInternalAbortFlag(false);
		GUI->StartExposure_PushButton.Enable();
	   } 

	 }
 }

 void CCDFrameInterface::EditCompleted(Edit& sender){
	 if (sender == GUI->ExpTime_Edit){
		 m_ExposureDuration = sender.Text().ToDouble();
	 }else if (sender == GUI->ExpNum_Edit){
		 m_NumOfExposures = sender.Text().ToInt();
	 }else if (sender == GUI->ImagePath_Edit){
		 m_FrameFolder = sender.Text();
	 } else if (sender == GUI->ImagePrefix_Edit){
	     m_FramePrefix = sender.Text();
	 }
 }

 void CCDFrameInterface::CheckBoxChecked(Button& sender, Button::check_state state){
	 if (sender == GUI->SaveImage_CheckBox){
		 if (state == CheckState::Checked){
			 m_saveFrame=true;
		 }
		 else {
			 m_saveFrame=false;
		 }
	 }
 }

} // pcl

// ****************************************************************************
// EOF PixInsightINDIInterface.cpp - Released 2013/03/24 18:42:27 UTC
