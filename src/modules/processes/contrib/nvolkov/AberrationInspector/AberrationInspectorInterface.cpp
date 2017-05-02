//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard AberrationInspector Process Module Version 01.01.02.0208
// ----------------------------------------------------------------------------
// AberrationInspectorInterface.cpp - Released 2016/02/29 00:00:00 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard AberrationInspector PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "AberrationInspectorInterface.h"
#include "AberrationInspectorProcess.h"
#include "AberrationInspectorParameters.h"


#include <pcl/Settings.h>
#include <pcl/MessageBox.h>

namespace pcl
{

	// ----------------------------------------------------------------------------

	Interface* TheInterface = 0;

	// ----------------------------------------------------------------------------

#include "AberrationInspectorIcon.xpm"

	// ----------------------------------------------------------------------------

	Interface::Interface() :
		ProcessInterface(),
		instance(TheAberrationInspectorProcess),
		GUI(0)
	{
		TheInterface = this;
	}

	Interface::~Interface()
	{
		if (GUI != 0)
			delete GUI, GUI = 0;
	}

	IsoString Interface::Id() const
	{
		return "AberrationInspector";
	}

	MetaProcess* Interface::Process() const
	{
		return TheAberrationInspectorProcess;
	}

	const char** Interface::IconImageXPM() const
	{
		return AberrationInspectorIcon_XPM; //---> put a nice icon here
	}

	InterfaceFeatures Interface::Features() const
	{
		return InterfaceFeature::DefaultGlobal | InterfaceFeature::PreferencesButton;
	}

	void Interface::ApplyInstance() const
	{
		instance.LaunchOnCurrentView();
	}

	void Interface::ResetInstance()
	{
		AberrationInspectorInstance defaultAberrationInspectorInstance(TheAberrationInspectorProcess);
		ImportProcess(defaultAberrationInspectorInstance);
	}

	bool Interface::Launch(const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/)
	{
		if (GUI == 0)
		{
			GUI = new GUIData(*this);
			SetWindowTitle("AberrationInspector");
		}

		dynamic = false;
		return &P == TheAberrationInspectorProcess;
	}

	ProcessImplementation* Interface::NewProcess() const
	{
		return new AberrationInspectorInstance(instance);
	}

	bool Interface::ValidateProcess(const ProcessImplementation& p, String& whyNot) const
	{
		if ( dynamic_cast<const AberrationInspectorInstance*>( &p ) != nullptr )
         return true;
      whyNot = "Not a AberrationInspector instance.";
      return false;
	}

	bool Interface::RequiresInstanceValidation() const
	{
		return true;
	}

	bool Interface::ImportProcess(const ProcessImplementation& p)
	{
		instance.Assign(p);
		UpdateControls();
		return true;
	}


	void Interface::EditPreferences()
	{
		MessageBox m("Save current setting ?",
			"AberrationInspector Preferences", // caption
			StdIcon::Question, StdButton::No, StdButton::Yes);
		if (m.Execute() != StdButton::Yes) return;

		Settings::WriteI(SettingsKey() + "MosaicSize", instance.p_mosaicSize);
		Settings::WriteI(SettingsKey() + "PanelSize", instance.p_panelSize);
		Settings::WriteI(SettingsKey() + "Separation", instance.p_separation);
		Settings::Write(SettingsKey() + "Brightness", instance.p_brightness);
	}

	void Interface::LoadSettings()
	{
		Settings::ReadI(SettingsKey() + "MosaicSize", instance.p_mosaicSize);
		Settings::ReadI(SettingsKey() + "PanelSize", instance.p_panelSize);
		Settings::ReadI(SettingsKey() + "Separation", instance.p_separation);
		Settings::Read(SettingsKey() + "Brightness", instance.p_brightness);

		UpdateControls();
	}

	// ----------------------------------------------------------------------------

	void Interface::UpdateControls()
	{
		GUI->MosaicSize_Control.SetValue(instance.p_mosaicSize);
		GUI->PanelSize_Control.SetValue(instance.p_panelSize);
		GUI->Separation_Control.SetValue(instance.p_separation);
		GUI->Brightness_Control.SetValue(instance.p_brightness);
	}

	// ----------------------------------------------------------------------------

	void Interface::__RealValueUpdated(NumericEdit& sender, double value)
	{
		if (sender == GUI->Brightness_Control)
			instance.p_brightness = value;
	}


	void Interface::__IntegerValueUpdated(NumericEdit& sender, double value)
	{
		if (sender == GUI->MosaicSize_Control)
			instance.p_mosaicSize = value;
		if (sender == GUI->PanelSize_Control)
			instance.p_panelSize = value;
		if (sender == GUI->Separation_Control)
			instance.p_separation = value;
	}


	// ----------------------------------------------------------------------------

	Interface::GUIData::GUIData(Interface& w)
	{
		pcl::Font fnt = w.Font();
		int labelWidth1 = fnt.Width(String("Mosaic size:")); // the longest label text
		int editWidth1 = fnt.Width(String('0', 7));
		const int sliderWidth = 250;

		//

		MosaicSize_Control.label.SetText("Mosaic size:");
		MosaicSize_Control.label.SetFixedWidth(labelWidth1);
		MosaicSize_Control.slider.SetMinWidth(sliderWidth);
		MosaicSize_Control.slider.SetRange(TheMosaicSize->MinimumValue(), TheMosaicSize->MaximumValue());
		MosaicSize_Control.SetRange(TheMosaicSize->MinimumValue(), TheMosaicSize->MaximumValue());
		MosaicSize_Control.SetPrecision(TheMosaicSize->Precision());
		MosaicSize_Control.SetToolTip("<p>Number of rows and columns in the mosaic.</p>");
		MosaicSize_Control.edit.SetFixedWidth(editWidth1);
		MosaicSize_Control.OnValueUpdated((NumericEdit::value_event_handler)&Interface::__IntegerValueUpdated, w);

		//

		PanelSize_Control.label.SetText("Panel size:");
		PanelSize_Control.label.SetFixedWidth(labelWidth1);
		PanelSize_Control.slider.SetMinWidth(sliderWidth);
		PanelSize_Control.slider.SetRange(ThePanelSize->MinimumValue(), ThePanelSize->MaximumValue());
		PanelSize_Control.SetRange(ThePanelSize->MinimumValue(), ThePanelSize->MaximumValue());
		PanelSize_Control.SetPrecision(ThePanelSize->Precision());
		PanelSize_Control.SetToolTip("<p>Width and height in pixels of each panel in the mosaic.</p>");
		PanelSize_Control.edit.SetFixedWidth(editWidth1);
		PanelSize_Control.OnValueUpdated((NumericEdit::value_event_handler)&Interface::__IntegerValueUpdated, w);

		//

		Separation_Control.label.SetText("Separation:");
		Separation_Control.label.SetFixedWidth(labelWidth1);
		Separation_Control.slider.SetMinWidth(sliderWidth);
		Separation_Control.slider.SetRange(TheSeparation->MinimumValue(), TheSeparation->MaximumValue());
		Separation_Control.SetRange(TheSeparation->MinimumValue(), TheSeparation->MaximumValue());
		Separation_Control.SetPrecision(TheSeparation->Precision());
		Separation_Control.SetToolTip("<p>Separation in pixels between each panel in the mosaic.</p>");
		Separation_Control.edit.SetFixedWidth(editWidth1);
		Separation_Control.OnValueUpdated((NumericEdit::value_event_handler)&Interface::__IntegerValueUpdated, w);

		//

		Brightness_Control.label.SetText("Brightness:");
		Brightness_Control.label.SetFixedWidth(labelWidth1);
		Brightness_Control.slider.SetMinWidth(sliderWidth);
		Brightness_Control.slider.SetRange(TheBrightness->MinimumValue(), TheBrightness->MaximumValue()*sliderWidth);
		Brightness_Control.SetReal();
		Brightness_Control.SetRange(TheBrightness->MinimumValue(), TheBrightness->MaximumValue());
		Brightness_Control.SetPrecision(TheBrightness->Precision());
		Brightness_Control.SetToolTip("<p>Brightness of separation between each panel in the mosaic.</p><p>0 is black and 1 is white.</p>");
		Brightness_Control.edit.SetFixedWidth(editWidth1);
		Brightness_Control.OnValueUpdated((NumericEdit::value_event_handler)&Interface::__RealValueUpdated, w);

		//

		Global_Sizer.SetMargin(8);
		Global_Sizer.SetSpacing(6);
		Global_Sizer.Add(MosaicSize_Control);
		Global_Sizer.Add(PanelSize_Control);
		Global_Sizer.Add(Separation_Control);
		Global_Sizer.Add(Brightness_Control);

		w.SetSizer(Global_Sizer);
		w.AdjustToContents();
		w.SetFixedSize();
	}

	// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF AberrationInspectorInterface.cpp - Released 2016/02/29 00:00:00 UTC
