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
   m_newPropertyListItem.Property=String("CCD_EXPOSURE");
   m_newPropertyListItem.Element=String("CCD_EXPOSURE_VALUE");
   m_newPropertyListItem.PropertyType=String("INDI_NUMBER");
   m_NumOfExposures=1;
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

	FrameExposure_SectionBar.SetTitle("Frame Shooting");
	FrameExposure_SectionBar.SetSection(FrameExposure_Control);
	FrameExposure_Control.SetSizer(FrameExposure_Sizer);

	ExpTime_Label.SetText( "Exposure Time:" );
	ExpTime_Label.SetToolTip( "<p>Specify exposure time in seconds.</p>" );
	ExpTime_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

	ExpTime_Edit.SetMinWidth( 30);
	ExpTime_Edit.OnEditCompleted( (Edit::edit_event_handler)&CCDFrameInterface::EditCompleted, w );
   
	StartExposure_PushButton.SetText( "Start Exposure" );
	StartExposure_PushButton.OnClick( (Button::click_event_handler) &CCDFrameInterface::StartExposureButton_Click, w );

	ExpTime_Sizer.SetSpacing(10);
	ExpTime_Sizer.SetMargin(10);
	ExpTime_Sizer.Add(ExpTime_Label);
	ExpTime_Sizer.Add(ExpTime_Edit);

	ExpNum_Label.SetText( "Number of frames:" );
	ExpNum_Label.SetToolTip( "<p>Number of frames to be taken.</p>" );
	ExpNum_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

	ExpNum_Edit.SetMinWidth( 30);
	ExpNum_Edit.OnEditCompleted( (Edit::edit_event_handler)&CCDFrameInterface::EditCompleted, w );

	ExpNum_Sizer.SetSpacing(10);
	ExpNum_Sizer.SetMargin(10);
	ExpNum_Sizer.Add(ExpNum_Label);
	ExpNum_Sizer.Add(ExpNum_Edit);
	ExpNum_Sizer.Add(StartExposure_PushButton);

	FrameExposure_Sizer.Add(ExpTime_Sizer);
	FrameExposure_Sizer.Add(ExpNum_Sizer);

	Global_Sizer.SetMargin( 8 );
	Global_Sizer.SetSpacing( 6 );
	Global_Sizer.Add(CCDDevice_SectionBar);
	Global_Sizer.Add(CCDDevice_Control);
	Global_Sizer.Add(FrameExposure_SectionBar);
	Global_Sizer.Add(FrameExposure_Control);

	w.SetSizer(Global_Sizer);
	
	UpdateDeviceList_Timer.SetInterval( 0.5 );
    UpdateDeviceList_Timer.SetPeriodic( true );
    UpdateDeviceList_Timer.OnTimer( (Timer::timer_event_handler)&CCDFrameInterface::UpdateDeviceList_Timer, w );

	UpdateDeviceList_Timer.Start();

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

void CCDFrameInterface::ComboItemSelected(ComboBox& sender, int itemIndex) {
	if (sender == GUI->CCDDevice_Combo){
		m_newPropertyListItem.Device = sender.ItemText(itemIndex);
	}
}

 void CCDFrameInterface::StartExposureButton_Click(Button& sender, bool checked){
	 if (ThePixInsightINDIInterface!=0){
		PixInsightINDIInstance* pInstance=&ThePixInsightINDIInterface->instance;

		for (int num=0; num<m_NumOfExposures;++num){
			pInstance->sendNewPropertyValue(m_newPropertyListItem);

			Array<ImageWindow> imgArray = ImageWindow::Open(String("C:/Users/klaus/tmp/Image.fits"), IsoString("image"));

			imgArray[0].ZoomToFit( false ); // don't allow zoom > 1
			imgArray[0].Show();
		}

	 }
 }

 void CCDFrameInterface::EditCompleted(Edit& sender){
	 if (sender == GUI->ExpTime_Edit){
		 m_newPropertyListItem.NewPropertyValue = sender.Text();
	 }else if (sender == GUI->ExpTime_Edit){
		 m_NumOfExposures = sender.Text().ToInt();
	 }
 }

} // pcl

// ****************************************************************************
// EOF PixInsightINDIInterface.cpp - Released 2013/03/24 18:42:27 UTC
