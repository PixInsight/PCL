//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard ArcsinhStretch Process Module Version 00.00.01.0104
// ----------------------------------------------------------------------------
// ArcsinhStretchInterface.cpp 
// ----------------------------------------------------------------------------
// This file is part of the standard ArcsinhStretch PixInsight module.
//
// Copyright (c) 2003-2017 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "ArcsinhStretchInterface.h"
#include "ArcsinhStretchParameters.h"
#include "ArcsinhStretchProcess.h"

#include <pcl/MuteStatus.h>
#include <pcl/RealTimePreview.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ArcsinhStretchInterface* TheArcsinhStretchInterface = 0;

// ----------------------------------------------------------------------------

//#include "ArcsinhStretchIcon.xpm"

// ----------------------------------------------------------------------------

ArcsinhStretchInterface::ArcsinhStretchInterface() :
ProcessInterface(), instance(TheArcsinhStretchProcess), m_realTimeThread(0), GUI(0)
{
   TheArcsinhStretchInterface = this;
}

ArcsinhStretchInterface::~ArcsinhStretchInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

//This function enables the real time preview button on the dialog 
InterfaceFeatures ArcsinhStretchInterface::Features() const
{
	return InterfaceFeature::Default | InterfaceFeature::RealTimeButton;
}

IsoString ArcsinhStretchInterface::Id() const
{
   return "ArcsinhStretch";
}

MetaProcess* ArcsinhStretchInterface::Process() const
{
   return TheArcsinhStretchProcess;
}

const char** ArcsinhStretchInterface::IconImageXPM() const
{
   return 0; // ArcsinhStretchIcon_XPM; ---> put a nice icon here
}

void ArcsinhStretchInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}
bool ArcsinhStretchInterface::WantsRealTimePreviewNotifications() const
{
	return true;
}
void ArcsinhStretchInterface::RealTimePreviewOwnerChanged(ProcessInterface& iface)
{
	if (GUI != 0)
	{
		GUI->EstimateBlackPoint_Button.Enable(iface == *this && IsRealTimePreviewActive());
		GUI->ParameterPreviewClipped_CheckBox.Enable(iface == *this && IsRealTimePreviewActive());
	}
}
void ArcsinhStretchInterface::RealTimePreviewUpdated(bool active)
{
	if (GUI != 0)
		if (active)
			RealTimePreview::SetOwner(*this); // implicitly updates the real time preview
		else
			RealTimePreview::SetOwner(ProcessInterface::Null());
}

void ArcsinhStretchInterface::ResetInstance()
{
   ArcsinhStretchInstance defaultInstance( TheArcsinhStretchProcess );
   ImportProcess( defaultInstance );

   UpdateRealTimePreview();
}

bool ArcsinhStretchInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "ArcsinhStretch" );
	  UpdateSliderControls();
	  UpdateControls();
   }

   dynamic = false;
   return &P == TheArcsinhStretchProcess;
}

ProcessImplementation* ArcsinhStretchInterface::NewProcess() const
{
   return new ArcsinhStretchInstance( instance );
}

bool ArcsinhStretchInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const ArcsinhStretchInstance* r = dynamic_cast<const ArcsinhStretchInstance*>( &p );
   if ( r == 0 )
   {
      whyNot = "Not a ArcsinhStretch instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool ArcsinhStretchInterface::RequiresInstanceValidation() const
{
   return true;
}

bool ArcsinhStretchInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateSliderControls();
   UpdateControls();
   return true;
}
bool ArcsinhStretchInterface::RequiresRealTimePreviewUpdate(const UInt16Image&, const View&, int zoomLevel) const
{
	return true;
}
// ----------------------------------------------------------------------------

ArcsinhStretchInterface::RealTimeThread::RealTimeThread() : Thread(), m_instance(TheArcsinhStretchProcess)
{
}

void ArcsinhStretchInterface::RealTimeThread::Reset(const UInt16Image& image, const ArcsinhStretchInstance& instance)
{
	image.ResetSelections();
	m_image.Assign(image);
	m_instance.Assign(instance);
}

void ArcsinhStretchInterface::RealTimeThread::Run()
{
	MuteStatus status;
	m_image.SetStatusCallback(&status);
	m_instance.Preview(m_image);
	m_image.ResetSelections();
}
bool ArcsinhStretchInterface::GenerateRealTimePreview(UInt16Image& image, const View&, int zoomLevel, String&) const
{
	m_preview_blackpoint_level = image.MinimumPixelValue() / 65536.0;  //m_preview_blackpoint_level is mutable, which allows it to be changed within this const function
	m_realTimeThread = new RealTimeThread;

	//Generate a histogram of values
	int histogram[65535];
	for (int i = 0; i < 65536; i++)
		histogram[i] = 0;

	size_type numChannels = image.NumberOfNominalChannels();
	int xDim = image.Width();
	int yDim = image.Height();
	for (int ix = 0; ix < xDim; ix++)
	{
		for (int iy = 0; iy < yDim; iy++)
		{
			Point point = Point(ix, iy);
			double intensity = 0;
			for (int ich = 0; ich < numChannels; ich++)
			{
				intensity = image.Pixel(point, ich);
				histogram[(int)intensity]++;
			}
		}
	}
	//Set the black point so that 2% of pixel values in the preview window become clipped to zero 
	int sum = histogram[0];
	int ihist = 0;
	while (sum < xDim*yDim*numChannels / 50)
	{
		ihist++;
		sum += histogram[ihist];
	}
	if (ihist == 0)
		m_preview_blackpoint_level = 0.0;
	else
		m_preview_blackpoint_level = (ihist - 1) / 65536.0;


	for (;;)
	{
		m_realTimeThread->Reset(image, instance);
		m_realTimeThread->Start();

		while (m_realTimeThread->IsActive())
		{
			ProcessEvents();

			if (!IsRealTimePreviewActive())
			{
				m_realTimeThread->Abort();
				m_realTimeThread->Wait();

				delete m_realTimeThread;
				m_realTimeThread = 0;
				return false;
			}
		}

		if (!m_realTimeThread->IsAborted())
		{
			image.Assign(m_realTimeThread->m_image);

			delete m_realTimeThread;
			m_realTimeThread = 0;
			return true;
		}
	}
}

// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::UpdateControls()
{
	
	GUI->Stretch_NumericEdit.SetValue(instance.p_stretch);
	GUI->BlackPoint_NumericEdit.SetValue(instance.p_blackpoint);
    GUI->ParameterProtectHighlights_CheckBox.SetChecked( instance.p_protectHighlights );
    GUI->ParameterUseRgbws_CheckBox.SetChecked(instance.p_useRgbws);
    GUI->ParameterPreviewClipped_CheckBox.SetChecked(instance.p_previewClipped);
}
void ArcsinhStretchInterface::UpdateSliderControls()
{
	GUI->FineAdjust_Slider.SetValue(500);  // Centre of the range 0-1000 
	GUI->BlackPoint_Slider.SetValue(instance.p_blackpoint * 10000);		 //In the range 0-2000 since max(p_blackpoint) = 0.2
	GUI->Stretch_Slider.SetValue(log10(instance.p_stretch)*1000.0/3.0);  //In the range 0-1000


}

void ArcsinhStretchInterface::UpdateRealTimePreview()
{
	if (IsRealTimePreviewActive())
	{
		if (m_realTimeThread != 0)
			m_realTimeThread->Abort();
		GUI->UpdateRealTimePreview_Timer.Start();
	}
}
// ----------------------------------------------------------------------------

void ArcsinhStretchInterface::__RealValueUpdated( NumericEdit& sender, double value )
{
	if (sender == GUI->Stretch_NumericEdit)
	{
		instance.p_stretch = value;
	}
	if (sender == GUI->BlackPoint_NumericEdit)
	{
		instance.p_blackpoint = value;
	}

	UpdateSliderControls();
	UpdateRealTimePreview();

}

void ArcsinhStretchInterface::__SliderValueUpdated(Slider& sender, int value)
{
	if (sender == GUI->BlackPoint_Slider)
	{
		instance.p_coarse = value * 100;  //value is in the range 0-2000
		instance.p_blackpoint = (instance.p_coarse) / 1000000.0;  // In the range 0-0.2
	}
	else if (sender == GUI->Stretch_Slider)
		instance.p_stretch = Pow10(3.0*value / 1000.0); //value is in the range 0-1000
	else if (sender == GUI->FineAdjust_Slider)
	{
		instance.p_blackpoint = instance.p_blackpointBeforeAdjustment + (value - 500.0) / 1000000.0;  //value is in the range 0-1000
		if (instance.p_blackpoint < 0.0) instance.p_blackpointBeforeAdjustment -= instance.p_blackpoint;
		GUI->BlackPoint_Slider.SetValue(instance.p_blackpoint*10000);  // In the range 0-0.2
	}

	UpdateControls();
	UpdateRealTimePreview();
}
void ArcsinhStretchInterface::__FineAdjustSliderEnter(Control& sender)
{
	__FineAdjustSliderGetFocus(sender);
}
void ArcsinhStretchInterface::__FineAdjustSliderLeave(Control& sender)
{
	__FineAdjustSliderLoseFocus(sender);
}
void ArcsinhStretchInterface::__FineAdjustSliderMousePress(Control& sender, const pcl::Point &pos, int button, unsigned buttons, unsigned modifiers)
{
	__FineAdjustSliderGetFocus(sender);
}
void ArcsinhStretchInterface::__FineAdjustSliderMouseRelease(Control& sender, const pcl::Point &pos, int button, unsigned buttons, unsigned modifiers)
{
	__FineAdjustSliderLoseFocus(sender);
}
void ArcsinhStretchInterface::__FineAdjustSliderLoseFocus(Control& sender)
{
	GUI->FineAdjust_Slider.SetValue(500);  //Set it to the halfway point
}

void ArcsinhStretchInterface::__FineAdjustSliderGetFocus(Control& sender)
{
	instance.p_blackpointBeforeAdjustment = instance.p_blackpoint;
}


void ArcsinhStretchInterface::__ItemClicked( Button& sender, bool checked )
{
   if ( sender == GUI->ParameterProtectHighlights_CheckBox )
      instance.p_protectHighlights = checked;
   if (sender == GUI->ParameterUseRgbws_CheckBox)
	   instance.p_useRgbws = checked;
   if (sender == GUI->ParameterPreviewClipped_CheckBox)
	   instance.p_previewClipped = checked;
   if (sender == GUI->EstimateBlackPoint_Button)
   {
	   instance.p_blackpoint = m_preview_blackpoint_level;
	   GUI->BlackPoint_NumericEdit.SetValue(instance.p_blackpoint);
	   UpdateSliderControls();
   }

   UpdateRealTimePreview();

}



void ArcsinhStretchInterface::__UpdateRealTimePreview_Timer(Timer& sender)
{
	if (m_realTimeThread != 0)
		if (m_realTimeThread->IsActive())
			return;

	if (IsRealTimePreviewActive())
		RealTimePreview::Update();
}

// ----------------------------------------------------------------------------

ArcsinhStretchInterface::GUIData::GUIData( ArcsinhStretchInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "XXXXXXX XXXXXXX" ) ); // the longest label text
   int editWidth1 = fnt.Width( String( '0', 9) );

   Stretch_NumericEdit.label.SetText("Stretch Factor:");
   Stretch_NumericEdit.label.SetFixedWidth(labelWidth1);
   Stretch_NumericEdit.edit.SetFixedWidth(editWidth1);
   Stretch_NumericEdit.SetReal();
   Stretch_NumericEdit.SetRange(TheArcsinhStretchParameter->MinimumValue(), TheArcsinhStretchParameter->MaximumValue());
   Stretch_NumericEdit.SetPrecision(TheArcsinhStretchParameter->Precision());
   Stretch_NumericEdit.SetToolTip("<p>Multiplier applied to faintest detail above background level.</p><p> On slider use Up/Dn arrows, PgUp/PgDn and mouse wheel for fine adjustment.</p>");
   Stretch_NumericEdit.OnValueUpdated((NumericEdit::value_event_handler)&ArcsinhStretchInterface::__RealValueUpdated, w);

   Stretch_Slider.SetScaledMinWidth(250);  // Physical width of slider in pixels
   Stretch_Slider.SetRange(0, 1000);      // Resolution of slider value  //Do not change without examining the chain of consequences i.e. UpdateSliderControls and __SliderValueUpdated
   Stretch_Slider.SetStepSize(1);
   Stretch_Slider.SetToolTip("<p>Multiplier applied to faintest detail above black point.</p><p> Use Up/Dn arrows, PgUp/PgDn and mouse wheel for fine adjustment.</p>");
   Stretch_Slider.OnValueUpdated((Slider::value_event_handler)&ArcsinhStretchInterface::__SliderValueUpdated, w);

   BlackPoint_NumericEdit.label.SetText("Black Point:");
   BlackPoint_NumericEdit.label.SetFixedWidth(labelWidth1);
   BlackPoint_NumericEdit.SetReal();
   BlackPoint_NumericEdit.SetRange(TheArcsinhStretchBlackPointParameter->MinimumValue(), TheArcsinhStretchBlackPointParameter->MaximumValue());
   BlackPoint_NumericEdit.SetPrecision(TheArcsinhStretchBlackPointParameter->Precision());
   BlackPoint_NumericEdit.SetToolTip("<p>Constant value to subtract from the image. It is assumed that background extraction and/or neutralisation has previously been performed on the image.</p>On sliders use Up/Dn arrows, PgUp/PgDn and mouse wheel for fine adjustment.</p>");
   BlackPoint_NumericEdit.edit.SetFixedWidth(editWidth1);
   BlackPoint_NumericEdit.OnValueUpdated((NumericEdit::value_event_handler)&ArcsinhStretchInterface::__RealValueUpdated, w);

   BlackPoint_Slider.SetScaledMinWidth(250);
   BlackPoint_Slider.SetRange(0, 2000);  //Do not change without examining the chain of consequences i.e. UpdateSliderControls and __SliderValueUpdated
   BlackPoint_Slider.SetStepSize(1);
   BlackPoint_Slider.SetToolTip("<p>Constant value to subtract from the image. It is assumed that background extraction and/or neutralisation has previously been performed on the image.</p> <p> Use Up/Dn arrows, PgUp/PgDn and mouse wheel for fine adjustment.</p>");
   BlackPoint_Slider.OnValueUpdated((Slider::value_event_handler)&ArcsinhStretchInterface::__SliderValueUpdated, w);

   FineAdjust_Slider.SetScaledMinWidth(250);
   FineAdjust_Slider.SetRange(0,1000);  //Do not change without examining the chain of consequences i.e. UpdateSliderControls and __SliderValueUpdated
   FineAdjust_Slider.SetStepSize(1);
   FineAdjust_Slider.SetToolTip("<p>Black point fine adjustment slider with re-centring. The slider re-centres after being used so the next adjustment can be up or down.</p> <p> Up/Dn Arrows, PgUp/PgDn and mouse wheel for fine adjustment</p>");
   FineAdjust_Slider.OnValueUpdated((Slider::value_event_handler)&ArcsinhStretchInterface::__SliderValueUpdated, w);
   FineAdjust_Slider.OnEnter((Control::event_handler)&ArcsinhStretchInterface::__FineAdjustSliderEnter, w);  //This allows us to re-centre the slider 
   FineAdjust_Slider.OnLeave((Control::event_handler)&ArcsinhStretchInterface::__FineAdjustSliderLeave, w);  //This allows us to re-centre the slider 

   ParameterProtectHighlights_CheckBox.SetText( "Protect highlights during stretch" );
   ParameterProtectHighlights_CheckBox.SetToolTip( "<p>Scale image to prevent values exceeding 1.0 during stretch - to prevent highlight clipping.</p>" );
   ParameterProtectHighlights_CheckBox.OnClick( (pcl::Button::click_event_handler)&ArcsinhStretchInterface::__ItemClicked, w );

   ParameterProtectHighlights_Sizer.AddUnscaledSpacing(labelWidth1 + w.LogicalPixelsToPhysical(4));
   ParameterProtectHighlights_Sizer.Add(ParameterProtectHighlights_CheckBox);
   ParameterProtectHighlights_Sizer.AddStretch();

   ParameterUseRgbws_CheckBox.SetText("Use RGB Working Space");
   ParameterUseRgbws_CheckBox.SetToolTip("<p>Use RGB Working Space to calculate luminance from RGB components.  Otherwise R, G and B and equally weighted in the luminance calculation.</p>");
   ParameterUseRgbws_CheckBox.OnClick((pcl::Button::click_event_handler)&ArcsinhStretchInterface::__ItemClicked, w);

   ParameterUseRgbws_Sizer.AddUnscaledSpacing(labelWidth1 + w.LogicalPixelsToPhysical(4));
   ParameterUseRgbws_Sizer.Add(ParameterUseRgbws_CheckBox);
   ParameterUseRgbws_Sizer.AddStretch();

   ParameterPreviewClipped_CheckBox.SetText("Highlight values clipped to zero");
   ParameterPreviewClipped_CheckBox.SetToolTip("<p>Preview window will highlight values that become clipped to zero when black point is set.  Otherwise preview window shows values truncated to the usual [0,1] range.</p>");
   ParameterPreviewClipped_CheckBox.OnClick((pcl::Button::click_event_handler)&ArcsinhStretchInterface::__ItemClicked, w);

   Stretch_Sizer.SetMargin(8);
   Stretch_Sizer.SetSpacing(6);
   Stretch_Sizer.Add(Stretch_NumericEdit,40);
   Stretch_Sizer.Add(Stretch_Slider,60);

   BlackPointSliders_Sizer.Add(BlackPoint_Slider);
   BlackPointSliders_Sizer.Add(FineAdjust_Slider);

   BlackPoint_Sizer.SetMargin(8);
   BlackPoint_Sizer.SetSpacing(6);
   BlackPoint_Sizer.Add(BlackPoint_NumericEdit, 40);
   BlackPoint_Sizer.Add(BlackPointSliders_Sizer, 60);

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add(Stretch_Sizer);

   Global_Sizer.Add(BlackPoint_Sizer);
   Global_Sizer.Add( ParameterProtectHighlights_Sizer );
   Global_Sizer.Add(ParameterUseRgbws_Sizer);

   int labelWidth = fnt.Width(String("Estimate Black Point")); 
   EstimateBlackPoint_Button.SetMinWidth(labelWidth);  
   EstimateBlackPoint_Button.SetText("Estimate Black Point");
   EstimateBlackPoint_Button.SetToolTip("<p>Auto-estimate the black point level. The black point is set to a level that allows 2% of values found in the real-time preview window to become clipped to zero.</p>");
   EstimateBlackPoint_Button.OnClick((pcl::Button::click_event_handler)&ArcsinhStretchInterface::__ItemClicked, w);

   PreviewControls_Sizer.SetMargin(8);
   PreviewControls_Sizer.SetSpacing(20);
   PreviewControls_Sizer.Add(EstimateBlackPoint_Button, 50, Align::Center);
   PreviewControls_Sizer.Add(ParameterPreviewClipped_CheckBox, 50, Align::Center);

   PreviewControlsGroupBox.SetTitle("Preview Window");
   PreviewControlsGroupBox.SetToolTip("<p>Tools that operate only on active real time preview window.</p>");
   PreviewControlsGroupBox.SetSizer(PreviewControls_Sizer);

   // Disable until a preview is activated
   EstimateBlackPoint_Button.Disable();
   ParameterPreviewClipped_CheckBox.Disable();

   Global_Sizer.Add(PreviewControlsGroupBox);
   

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();

   UpdateRealTimePreview_Timer.SetSingleShot();
   UpdateRealTimePreview_Timer.SetInterval(0.025);
   UpdateRealTimePreview_Timer.OnTimer((Timer::timer_event_handler)&ArcsinhStretchInterface::__UpdateRealTimePreview_Timer, w);

}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ArcsinhStretchInterface.cpp 
