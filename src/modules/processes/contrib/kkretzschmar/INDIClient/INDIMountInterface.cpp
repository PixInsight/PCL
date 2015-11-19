//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.02.0096
// ----------------------------------------------------------------------------
// INDIMountInterface.cpp - Released 2015/10/13 15:55:45 UTC
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

#include "INDIMountInterface.h"

#include "INDI/basedevice.h"
#include "INDI/indibase.h"
#include "INDI/indiapi.h"
#include <pcl/Console.h>
#include <pcl/Math.h>
#include <pcl/Graphics.h>
#include <assert.h>
#include <fstream>

#include "INDIMountProcess.h"
#include "INDIDeviceControllerInterface.h"
#include "SkyMap.h"

// time out in seconds
#define pcl_timeout 60
#define DEBUG 0
namespace pcl
{

// ----------------------------------------------------------------------------

INDIMountInterface* TheINDIMountInterface = 0;

// ----------------------------------------------------------------------------

//#include "PixInsightINDIIcon.xpm"

// ----------------------------------------------------------------------------

INDIMountInterface::INDIMountInterface() :
ProcessInterface(), instance( TheINDIMountProcess),GUI( 0 ),m_serverMessage(""), m_Device(""),m_TargetRA("0"),m_TargetDEC("0"),m_downloadedFile("2"),m_skymap(NULL), m_geoLat(0), m_lst(0),m_scopeRA(0),m_scopeDEC(0),m_alignedRA(0),m_alignedDEC(0),m_limitStarMag(4.5),m_isAllSkyView(true)
{
   TheINDIMountInterface = this;
}

INDIMountInterface::~INDIMountInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
   if (m_skymap!=NULL)
	   delete m_skymap;
}

IsoString INDIMountInterface::Id() const
{
   return "INDIMount";
}

MetaProcess* INDIMountInterface::Process() const
{
   return TheINDIMountProcess;
}

const char** INDIMountInterface::IconImageXPM() const
{
   return 0; // PixInsightINDIIcon_XPM; ---> put a nice icon here
}

InterfaceFeatures INDIMountInterface::Features() const
{
	return  InterfaceFeature::BrowseDocumentationButton ;
}

void INDIMountInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void INDIMountInterface::ApplyInstanceGlobal() const
{
   instance.LaunchGlobal();
}

void INDIMountInterface::ResetInstance()
{
   INDIDeviceControllerInstance defaultInstance( TheINDIMountProcess );
   ImportProcess( defaultInstance );
}

bool INDIMountInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData(*this );
      SetWindowTitle( "INDI Mount Controller" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheINDIMountProcess;
}

ProcessImplementation* INDIMountInterface::NewProcess() const
{
   return new INDIMountInstance( instance );
}

bool INDIMountInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const INDIMountInstance* r = dynamic_cast<const INDIMountInstance*>( &p );
   if ( r == 0 )
   {
      whyNot = "Not a INDIMount instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}// ----------------------------------------------------------------------------




bool INDIMountInterface::RequiresInstanceValidation() const
{
   return true;
}

bool INDIMountInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void INDIMountInterface::UpdateControls()
{
   
}



// ----------------------------------------------------------------------------

void EditEqCoordPropertyDialog::setRACoords(String value){
	StringList tokens;
	value.Break(tokens,':',true);
	assert(tokens.Length()==3);
	tokens[0].TrimLeft();
	RA_Hour_Edit.SetText(tokens[0]);
	RA_Minute_Edit.SetText(tokens[1]);
	RA_Second_Edit.SetText(tokens[2]);
	m_ra_hour=tokens[0].ToDouble();
	m_ra_minute=tokens[1].ToDouble();
	m_ra_second=tokens[2].ToDouble();
	double coord_ra=m_ra_hour + m_ra_minute / 60 + m_ra_second / 3600;
	m_RA_TargetCoord = String(coord_ra);
}

void EditEqCoordPropertyDialog::setDECCoords(String value){
	StringList tokens;
	value.Break(tokens,':',true);
	assert(tokens.Length()==3);
	tokens[0].TrimLeft();
	DEC_Hour_Edit.SetText(tokens[0]);
	DEC_Minute_Edit.SetText(tokens[1]);
	DEC_Second_Edit.SetText(tokens[2]);
	m_dec_deg=tokens[0].ToDouble();
	m_dec_arcminute=tokens[1].ToDouble();
	m_dec_arcsecond=tokens[2].ToDouble();
	int sign=m_dec_deg >= 0 ? 1 : -1;
	double coord_dec=sign * (fabs(m_dec_deg) + m_dec_arcminute / 60 + m_dec_arcsecond / 3600);
	m_DEC_TargetCoord = String(coord_dec);
}

void EditEqCoordPropertyDialog::EditCompleted( Edit& sender )
{
	if (sender==RA_Hour_Edit){
		m_ra_hour=sender.Text().ToDouble();
		m_ra_minute=RA_Minute_Edit.Text().ToDouble();
		m_ra_second=RA_Second_Edit.Text().ToDouble();
	}
	if (sender==RA_Minute_Edit){
		m_ra_hour=RA_Hour_Edit.Text().ToDouble();
		m_ra_minute=sender.Text().ToDouble();
		m_ra_second=RA_Second_Edit.Text().ToDouble();
	}
	if (sender==RA_Second_Edit){
		m_ra_hour=RA_Hour_Edit.Text().ToDouble();
		m_ra_minute=RA_Minute_Edit.Text().ToDouble();
		m_ra_second=sender.Text().ToDouble();
	}
	double coord_ra=m_ra_hour + m_ra_minute / 60 + m_ra_second / 3600;
	m_RA_TargetCoord = String(coord_ra);

	if (sender == DEC_Hour_Edit) {
		m_dec_deg = sender.Text().ToDouble();
		m_dec_arcminute = DEC_Minute_Edit.Text().ToDouble();
		m_dec_arcsecond = DEC_Second_Edit.Text().ToDouble();
	}
	if (sender == DEC_Minute_Edit) {
		m_dec_deg = DEC_Hour_Edit.Text().ToDouble();
		m_dec_arcminute = sender.Text().ToDouble();
		m_dec_arcsecond = DEC_Second_Edit.Text().ToDouble();
	}
	if (sender == DEC_Second_Edit) {
		m_dec_deg = DEC_Hour_Edit.Text().ToDouble();
		m_dec_arcminute = DEC_Minute_Edit.Text().ToDouble();
		m_dec_arcsecond = sender.Text().ToDouble();
	}
	double sign=m_dec_deg >= 0 ? 1.0 : -1.0;
	double coord_dec = sign * (fabs(m_dec_deg) + m_dec_arcminute / 60 + m_dec_arcsecond / 3600);
	m_DEC_TargetCoord = String(coord_dec);

}

void EditEqCoordPropertyDialog::Ok_Button_Click( Button& sender, bool checked ){
	Ok();
}
void EditEqCoordPropertyDialog::Cancel_Button_Click( Button& sender, bool checked ){
	Cancel();
}

EditEqCoordPropertyDialog::EditEqCoordPropertyDialog(String raCoord, String decCoord):m_ra_hour(0),m_ra_minute(0),m_ra_second(0),m_dec_deg(0),m_dec_arcsecond(0),m_dec_arcminute(0),m_DEC_TargetCoord(String("")){
	pcl::Font fnt = Font();
	int labelWidth = fnt.Width(String('0', 20));
	int editWidth = fnt.Width(String('0', 4));

	CoordUtils::coord_HMS ra_hms = CoordUtils::parse(raCoord);
	CoordUtils::coord_HMS dec_hms = CoordUtils::parse(decCoord);

	SetWindowTitle(String("Set equatorial coordinates for target object."));

	RA_Property_Label.SetMinWidth(labelWidth);
	RA_Property_Label.SetText("Ra (hh:mm:ss.ss)");
	RA_Property_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

	RA_Hour_Edit.SetMaxWidth(editWidth);
	RA_Hour_Edit.SetText(IsoString(ra_hms.hour));
	RA_Hour_Edit.OnEditCompleted(
			(Edit::edit_event_handler) &EditEqCoordPropertyDialog::EditCompleted,
			*this);

	RA_Colon1_Label.SetText(":");

	RA_Minute_Edit.SetMaxWidth(editWidth);
	RA_Minute_Edit.SetText(IsoString(ra_hms.minute));
	RA_Minute_Edit.OnEditCompleted(
			(Edit::edit_event_handler) &EditEqCoordPropertyDialog::EditCompleted,
			*this);

	RA_Colon2_Label.SetText(":");

	RA_Second_Edit.SetMaxWidth(editWidth);
	RA_Second_Edit.SetText(IsoString(ra_hms.second));
	RA_Second_Edit.OnEditCompleted(
			(Edit::edit_event_handler) &EditEqCoordPropertyDialog::EditCompleted,
			*this);

	DEC_Property_Label.SetMinWidth(labelWidth);
	DEC_Property_Label.SetText("Dec (dd:mm:ss.ss)");
	DEC_Property_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

	DEC_Hour_Edit.SetMaxWidth(editWidth);
	DEC_Hour_Edit.SetText(IsoString(dec_hms.hour));
	DEC_Hour_Edit.OnEditCompleted(
			(Edit::edit_event_handler) &EditEqCoordPropertyDialog::EditCompleted,
			*this);

	DEC_Colon1_Label.SetText(":");

	DEC_Minute_Edit.SetMaxWidth(editWidth);
	DEC_Minute_Edit.SetText(IsoString(dec_hms.minute));
	DEC_Minute_Edit.OnEditCompleted(
			(Edit::edit_event_handler) &EditEqCoordPropertyDialog::EditCompleted,
			*this);

	DEC_Colon2_Label.SetText(":");

	DEC_Second_Edit.SetMaxWidth(editWidth);
	DEC_Second_Edit.SetText(IsoString(dec_hms.second));
	DEC_Second_Edit.OnEditCompleted(
			(Edit::edit_event_handler) &EditEqCoordPropertyDialog::EditCompleted,
			*this);

	OK_PushButton.SetText("OK");
	OK_PushButton.OnClick(
			(Button::click_event_handler) &EditEqCoordPropertyDialog::Ok_Button_Click,
			*this);
	Cancel_PushButton.SetText("Cancel");
	Cancel_PushButton.OnClick(
			(Button::click_event_handler) &EditEqCoordPropertyDialog::Cancel_Button_Click,
			*this);

	RA_Property_Sizer.SetMargin(10);
	RA_Property_Sizer.SetSpacing(4);
	RA_Property_Sizer.Add(RA_Property_Label);
	RA_Property_Sizer.Add(RA_Hour_Edit);
	RA_Property_Sizer.Add(RA_Colon1_Label);
	RA_Property_Sizer.Add(RA_Minute_Edit);
	RA_Property_Sizer.Add(RA_Colon2_Label);
	RA_Property_Sizer.Add(RA_Second_Edit);
	RA_Property_Sizer.AddStretch();

	DEC_Property_Sizer.SetMargin(10);
	DEC_Property_Sizer.SetSpacing(4);
	DEC_Property_Sizer.Add(DEC_Property_Label);
	DEC_Property_Sizer.Add(DEC_Hour_Edit);
	DEC_Property_Sizer.Add(DEC_Colon1_Label);
	DEC_Property_Sizer.Add(DEC_Minute_Edit);
	DEC_Property_Sizer.Add(DEC_Colon2_Label);
	DEC_Property_Sizer.Add(DEC_Second_Edit);
	DEC_Property_Sizer.AddStretch();

	Buttons_Sizer.SetSpacing(4);
	Buttons_Sizer.AddSpacing(10);
	Buttons_Sizer.AddStretch();
	Buttons_Sizer.Add(OK_PushButton);
	Buttons_Sizer.AddStretch();
	Buttons_Sizer.Add(Cancel_PushButton);
	Buttons_Sizer.AddStretch();

	Global_Sizer.Add(RA_Property_Sizer);
	Global_Sizer.Add(DEC_Property_Sizer);
	Global_Sizer.Add(Buttons_Sizer);

	SetSizer(Global_Sizer);
}

CoordSearchDialog::CoordSearchDialog():m_targetObj(""),m_RA_TargetCoord(0),m_DEC_TargetCoord(0),m_downloadedFile(""){
	pcl::Font fnt = Font();
	int editWidth = fnt.Width(String('0', 8));
	int searchEditWidth = fnt.Width(String('0', 15));
	SetWindowTitle(String("Online Search."));

	Search_Sizer.SetSpacing(10);
	Search_Sizer.SetMargin(10);

	Search_Label.SetText("Object: ");
	Search_Label.SetToolTip("<p>Search for object </p>");
	Search_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

	Search_Edit.SetMinWidth(searchEditWidth);
	Search_Edit.SetMaxWidth(70);
	Search_Edit.OnEditCompleted((Edit::edit_event_handler) &CoordSearchDialog::EditCompleted,*this);

	Search_PushButton.SetText("Search");
	Search_PushButton.OnClick((Button::click_event_handler) &CoordSearchDialog::Button_Click,*this);

	Search_Sizer.Add(Search_Label);
	Search_Sizer.Add(Search_Edit);
	Search_Sizer.Add(Search_PushButton);

	Global_Sizer.Add(Search_Sizer);

	SearchResult_Sizer.SetSpacing(10);
	SearchResult_Sizer.SetMargin(10);

	TRA_Label.SetText("Target RA: ");
	TRA_Label.SetToolTip("<p>Target right ascension position </p>");
	TRA_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

	TRA_Edit.SetMinWidth(editWidth);
	TRA_Edit.SetMaxWidth(70);
	TRA_Edit.SetText("00:00:00");
	TRA_Edit.Disable();

	TDEC_Label.SetText("Target DEC: ");
	TDEC_Label.SetToolTip("<p>Target declination position </p>");
	TDEC_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

	TDEC_Edit.SetMinWidth(editWidth);
	TDEC_Edit.SetMaxWidth(70);
	TDEC_Edit.SetText("00:00:00");
	TDEC_Edit.Disable();

	SearchResult_Sizer.Add(TRA_Label);
	SearchResult_Sizer.Add(TRA_Edit);
	SearchResult_Sizer.Add(TDEC_Label);
	SearchResult_Sizer.Add(TDEC_Edit);

	Global_Sizer.Add(SearchResult_Sizer);

	SearchControl_Sizer.SetSpacing(10);
	SearchControl_Sizer.SetMargin(10);

	SearchOK_PushButton.SetText("OK");
	SearchOK_PushButton.OnClick((Button::click_event_handler) &CoordSearchDialog::Button_Click,*this);
	SearchCancel_PushButton.SetText("Cancel");
	SearchCancel_PushButton.OnClick((Button::click_event_handler) &CoordSearchDialog::Button_Click,*this);

	SearchControl_Sizer.Add(SearchOK_PushButton);
	SearchControl_Sizer.Add(SearchCancel_PushButton);

	Global_Sizer.Add(SearchControl_Sizer);

	SetSizer(Global_Sizer);
}

void CoordSearchDialog::EditCompleted( Edit& sender ){
	m_targetObj=sender.Text();
}

bool CoordSearchDialog::DownloadObjectCoordinates(NetworkTransfer &sender, const void *buffer, fsize_type size){
	 m_downloadedFile.Append(static_cast<const char*>(buffer),size);
	 return true;
 }

void CoordSearchDialog::Button_Click(Button& sender, bool checked){

	if (sender == Search_PushButton) {
		NetworkTransfer transfer;
		String url=String("http://simbad.u-strasbg.fr/simbad/sim-tap/sync?request=doQuery&lang=adql&format=text&query=");
		String select_stmt=String().Format("SELECT basic.OID, RA, DEC, main_id AS \"Main identifier\" FROM basic JOIN ident ON oidref = oid WHERE id = '%s';",IsoString(m_targetObj).c_str());
		//String url = String("http://vizier.cfa.harvard.edu/viz-bin/nph-sesame/-oI/A?");
		//url.Append(m_targetObj);
		url.Append(select_stmt);
		transfer.SetURL(url);
		transfer.OnDownloadDataAvailable(
				(NetworkTransfer::download_event_handler) &CoordSearchDialog::DownloadObjectCoordinates,
				*this);
		if (!transfer.Download()) {
			Console().WriteLn(String().Format("Download failed with error '%s'",IsoString(transfer.ErrorInformation()).c_str()));
		} else {
			Console().WriteLn(
					String().Format(
							"%d bytes downloaded with speed %f KiB/seconds.",
							transfer.BytesTransferred(),
							transfer.TotalSpeed()));


			StringList lines;
			m_downloadedFile.Break(lines, '\n', true);
			if(lines.Length() >= 3){
				StringList tokens;

				lines[2].Break(tokens, '|', true);
				if (tokens.Length() == 4){

					double ra_in_hours = tokens[1].ToDouble() / 360.0 * 24.0;

					m_RA_TargetCoord = ra_in_hours;
					m_DEC_TargetCoord = tokens[2].ToDouble();

					CoordUtils::coord_HMS coord_RA_HMS = CoordUtils::convertToHMS(m_RA_TargetCoord);
					CoordUtils::coord_HMS coord_DEC_HMS = CoordUtils::convertToHMS(m_DEC_TargetCoord);

					TRA_Edit.SetText(String().Format("%02d:%02d:%2.2f",(int) coord_RA_HMS.hour,(int) coord_RA_HMS.minute, coord_RA_HMS.second));
					TDEC_Edit.SetText(String().Format("%02d:%02d:%2.2f",(int) coord_DEC_HMS.hour,(int) coord_DEC_HMS.minute, coord_DEC_HMS.second));
				}
			}
		}
		m_downloadedFile.Clear();

	} else if (sender == SearchOK_PushButton) {
		Ok();
	} else if (sender == SearchCancel_PushButton) {
		m_RA_TargetCoord  = 0;
		m_DEC_TargetCoord = 0;
		Cancel();
	}

}

INDIMountInterface::GUIData::GUIData(INDIMountInterface& w ){
	pcl::Font fnt = pcl::Font();
	int editWidth = fnt.Width(String('0', 8));

	// Mount Device Selection Section ==============================================================
	MountDevice_SectionBar.SetTitle("INDI Mount Device Selection");
	MountDevice_SectionBar.SetSection(MountDevice_Control);
	MountDevice_Control.SetSizer(MountDevice_Sizer);

	MountDevice_Label.SetText( "INDI Mount Device:" );
	MountDevice_Label.SetToolTip( "<p>Select an INDI Mount device.</p>" );
	MountDevice_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

	MountDevice_Combo.OnItemSelected((ComboBox::item_event_handler)& INDIMountInterface::ComboItemSelected,w);

	MountDevice_Sizer.SetSpacing(10);
	MountDevice_Sizer.SetMargin(10);
	MountDevice_Sizer.Add(MountDevice_Label);
	MountDevice_Sizer.AddSpacing(10);
	MountDevice_Sizer.Add(MountDevice_Combo);

	// SkyChart Section ================================================================================
	SkyChart_SectionBar.SetTitle("Sky Chart");
	SkyChart_SectionBar.SetSection(SkyChart_Control);
	SkyChart_Control.SetSizer(SkyChart_HSizer);

	SkyChart_AllSkyGraphicsControl.SetBackgroundColor( StringToRGBAColor( "black" ) );
	SkyChart_AllSkyGraphicsControl.SetFixedSize(SkyChart_Control.Width(),SkyChart_Control.Width());
	SkyChart_AllSkyGraphicsControl.OnPaint( (Control::paint_event_handler)&INDIMountInterface::SkyChart_Paint, w );
	SkyChart_TabBox.AddPage(SkyChart_AllSkyGraphicsControl,String("Sky"));

	SkyChart_FoVGraphicsControl.SetBackgroundColor( StringToRGBAColor( "black" ) );
	SkyChart_FoVGraphicsControl.SetFixedSize(SkyChart_Control.Width(),SkyChart_Control.Width());
	SkyChart_FoVGraphicsControl.OnPaint( (Control::paint_event_handler)&INDIMountInterface::SkyChart_Paint, w );
	SkyChart_TabBox.AddPage(SkyChart_FoVGraphicsControl,String("Field of View"));

	SkyChart_TabBox.OnPageSelected((TabBox::page_event_handler)&INDIMountInterface::TabPageSelected,w);
	SkyChart_HSizer.Add(SkyChart_TabBox);

	// Mount Coordinates and time Section ==============================================================
	//--> Labels
	MountCoordAndTime_SectionBar.SetTitle("Time & Coordinates ");
	MountCoordAndTime_SectionBar.SetSection(MountCoordAndTime_Control);
	MountCoordAndTime_Control.SetSizer(MountCoordAndTime_HSizer);
	MountCoordAndTime_HSizer.SetSpacing(10);
	MountCoordAndTime_HSizer.SetMargin(10);
	MountCoordAndTime_HSizer.Add(MountCAT_Label_VSizer);
	MountCoordAndTime_HSizer.Add(MountCAT_First_VSizer);
	MountCoordAndTime_HSizer.Add(MountCAT_Second_VSizer);
	MountCoordAndTime_HSizer.AddStretch();

	MountTime_Label.SetText( "Current Date/Time:" );
	MountTime_Label.SetToolTip( "<p>Current Julian Date (JD) and Local Sideral Time (LST).</p>" );
	MountTime_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
	MountCAT_Label_VSizer.Add(MountTime_Label);

	MountEQC_Label.SetText( "Equatorial Coordinates:" );
	MountEQC_Label.SetToolTip( "<p>Current equatorial coordinates the telescope is pointing to.</p>" );
	MountEQC_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
	MountCAT_Label_VSizer.Add(MountEQC_Label);

	MountHZC_Label.SetText( "Horizonzal Coordinates:" );
	MountHZC_Label.SetToolTip( "<p>Current horizonzal coordinates the telescope is pointing to.</p>" );
	MountHZC_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
	MountCAT_Label_VSizer.Add(MountHZC_Label);


	//--> Labeled Controls
	MountCAT_First_VSizer.Add(MountTime_Sizer);
	MountCAT_First_VSizer.Add(MountEQC_Sizer);
	MountCAT_First_VSizer.Add(MountHZC_Sizer);

	MountTime_Sizer.SetSpacing(10);
	MountTime_Sizer.SetMargin(10);

	UTC_Label.SetText("JD: ");
	UTC_Label.SetToolTip("<p>Current Julian Date (UT)</p>");
	UTC_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

	UTC_Edit.SetMinWidth(50);
	UTC_Edit.SetMaxWidth(70);
	UTC_Edit.Disable();

	MountTime_Sizer.Add(UTC_Label);
	MountTime_Sizer.Add(UTC_Edit);

	MountEQC_Sizer.SetSpacing(10);
	MountEQC_Sizer.SetMargin(10);

	RA_Label.SetText("RA: ");
	RA_Label.SetToolTip("<p>Current right ascension position </p>");
	RA_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

	RA_Edit.SetMinWidth(50);
	RA_Edit.SetMaxWidth(70);
	RA_Edit.Disable();

	MountEQC_Sizer.Add(RA_Label);
	MountEQC_Sizer.Add(RA_Edit);

	MountHZC_Sizer.SetSpacing(10);
	MountHZC_Sizer.SetMargin(10);

	AZ_Label.SetText("AZ: ");
	AZ_Label.SetToolTip("<p>Current azimutal position </p>");
	AZ_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

	AZ_Edit.SetMinWidth(50);
	AZ_Edit.SetMaxWidth(70);
	AZ_Edit.Disable();

	MountHZC_Sizer.Add(AZ_Label);
	MountHZC_Sizer.Add(AZ_Edit);


	MountCAT_Second_VSizer.Add(MountTime_2_Sizer);
	MountCAT_Second_VSizer.Add(MountEQC_2_Sizer);
	MountCAT_Second_VSizer.Add(MountHZC_2_Sizer);

	MountTime_2_Sizer.SetSpacing(10);
	MountTime_2_Sizer.SetMargin(10);

	LST_Label.SetText("LST: ");
	LST_Label.SetToolTip("<p>Current Local Siderial Time (LST)</p>");
	LST_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

	LST_Edit.SetMinWidth(50);
	LST_Edit.SetMaxWidth(70);
	LST_Edit.Disable();

	MountTime_2_Sizer.Add(LST_Label);
	MountTime_2_Sizer.Add(LST_Edit);

	MountEQC_2_Sizer.SetSpacing(10);
	MountEQC_2_Sizer.SetMargin(10);

	DEC_Label.SetText("DEC: ");
	DEC_Label.SetToolTip("<p>Current declination position </p>");
	DEC_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

	DEC_Edit.SetMinWidth(50);
	DEC_Edit.SetMaxWidth(70);
	DEC_Edit.Disable();

	MountEQC_2_Sizer.Add(DEC_Label);
	MountEQC_2_Sizer.Add(DEC_Edit);

	MountHZC_2_Sizer.SetSpacing(10);
	MountHZC_2_Sizer.SetMargin(10);

	ALT_Label.SetText("ALT: ");
	ALT_Label.SetToolTip("<p>Current altitude position </p>");
	ALT_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

	ALT_Edit.SetMinWidth(50);
	ALT_Edit.SetMaxWidth(70);
	ALT_Edit.Disable();

	MountHZC_2_Sizer.Add(ALT_Label);
	MountHZC_2_Sizer.Add(ALT_Edit);


	// Mount Goto Target Section ==============================================================
	MountGoto_SectionBar.SetTitle("Goto Target ");
	MountGoto_SectionBar.SetSection(MountGoto_Control);
	MountGoto_Control.SetSizer(MountGoto_VSizer);

	// MountGoto_HSizer
	MountGoto_HSizer.SetSpacing(10);
	MountGoto_HSizer.SetMargin(10);

	TRA_Label.SetText("Target RA: ");
	TRA_Label.SetToolTip("<p>Target right ascension position </p>");
	TRA_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

	TRA_Edit.SetMinWidth(editWidth);
	TRA_Edit.SetMaxWidth(70);
	TRA_Edit.SetText("00:00:00");
	TRA_Edit.Disable();

	TDEC_Label.SetText("Target DEC: ");
	TDEC_Label.SetToolTip("<p>Target declination position </p>");
	TDEC_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

	TDEC_Edit.SetMinWidth(editWidth);
	TDEC_Edit.SetMaxWidth(70);
	TDEC_Edit.SetText("00:00:00");
	TDEC_Edit.Disable();

	MountSet_PushButton.SetText( "Set" );
	MountSet_PushButton.OnClick( (Button::click_event_handler) &INDIMountInterface::Button_Click, w );

	MountSearch_PushButton.SetText( "Search" );
	MountSearch_PushButton.OnClick( (Button::click_event_handler) &INDIMountInterface::Button_Click, w );


	MountGoto_HSizer.Add(TRA_Label);
	MountGoto_HSizer.Add(TRA_Edit);
	MountGoto_HSizer.Add(TDEC_Label);
	MountGoto_HSizer.Add(TDEC_Edit);
	MountGoto_HSizer.Add(MountSet_PushButton);
	MountGoto_HSizer.Add(MountSearch_PushButton);

	MountGoto_VSizer.Add(MountGoto_HSizer);

	//MountGoto_Second_HSizer
	MountGoto_Second_HSizer.SetSpacing(10);
	MountGoto_Second_HSizer.SetMargin(10);

	MountGoto_PushButton.SetText( "Goto" );
	MountGoto_PushButton.OnClick( (Button::click_event_handler) &INDIMountInterface::Button_Click, w );

	MountSynch_PushButton.SetText( "Synch" );
	MountSynch_PushButton.OnClick( (Button::click_event_handler) &INDIMountInterface::Button_Click, w );


	MountGoto_Second_HSizer.Add(MountGoto_PushButton);
	MountGoto_Second_HSizer.Add(MountSynch_PushButton);
	MountGoto_VSizer.Add(MountGoto_Second_HSizer);

	// Global Sizer
	Global_Sizer.SetMargin( 8 );
	Global_Sizer.SetSpacing( 6 );
	Global_Sizer.Add(MountDevice_SectionBar);
	Global_Sizer.Add(MountDevice_Control);
	Global_Sizer.Add(SkyChart_SectionBar);
	Global_Sizer.Add(SkyChart_Control);
	Global_Sizer.Add(MountCoordAndTime_SectionBar);
	Global_Sizer.Add(MountCoordAndTime_Control);
	Global_Sizer.Add(MountGoto_SectionBar);
	Global_Sizer.Add(MountGoto_Control);

	w.SetSizer(Global_Sizer);
	
	UpdateDeviceList_Timer.SetInterval( 0.5 );
    UpdateDeviceList_Timer.SetPeriodic( true );
    UpdateDeviceList_Timer.OnTimer( (Timer::timer_event_handler)&INDIMountInterface::UpdateDeviceList_Timer, w );

	UpdateDeviceList_Timer.Start();

	UpdateMount_Timer.SetInterval( 1 );
	UpdateMount_Timer.SetPeriodic( true );
	UpdateMount_Timer.OnTimer( (Timer::timer_event_handler)&INDIMountInterface::UpdateMount_Timer, w );



}

void INDIMountInterface::TabPageSelected( TabBox& sender, int pageIndex ){
	if (sender == GUI->SkyChart_TabBox){
		if (pageIndex==0){ // AllSky tab page
			m_isAllSkyView = true;
		} else if (pageIndex==1){ // FoV tab page
			double CCD_chipHeight=2200;
			double CCD_chipWidth=2750;
			double CCD_pixelSize=4.54 /1000;
			double TEL_focalLength=700;
			double FoV_width=CCD_chipWidth*CCD_pixelSize  / TEL_focalLength * 3438 / 60;
			double FoV_height=CCD_chipHeight*CCD_pixelSize / TEL_focalLength * 3438 / 60;
			double limitStarMag = 13;

			double ra_center = m_scopeRA *360.0 / 24.0;
			double dec_center = m_scopeDEC;

			// download stars
			NetworkTransfer transfer;
			IsoString url =	IsoString("http://simbad.u-strasbg.fr/simbad/sim-tap/sync?request=doQuery&lang=adql&format=text&query=");
			IsoString select_stmt = m_skymap->getASDLFoVQueryString(ra_center,dec_center,FoV_width,FoV_height,limitStarMag);
			Console().WriteLn(IsoString().Format("QueryStr = %s",	select_stmt.c_str()));
			url.Append(select_stmt);
			transfer.SetURL(url);
			transfer.OnDownloadDataAvailable(
					(NetworkTransfer::download_event_handler) &INDIMountInterface::DownloadObjectCoordinates,
					*this);
			if (!transfer.Download()) {
				Console().WriteLn(String().Format("Download failed with error '%s'",IsoString(transfer.ErrorInformation()).c_str()));
			} else {
				Console().WriteLn(String().Format("%d bytes downloaded with speed %f KiB/seconds.",
										transfer.BytesTransferred(),
										transfer.TotalSpeed()));

				StringList lines;
				m_downloadedFile.Break(lines, '\n', true);
				m_skymap->clearFoVObjectList();
				if (lines.Length() > 0) {
					for (size_t i = 0; i < lines.Length(); i++) {
						if (i <= 1)
							continue;

						StringList tokens;
						lines[i].Break(tokens, '|', true);
						if (tokens.Length()<4)
							continue;
						SkyMap::object star;
						star.mainId = tokens[0];
						star.ra = tokens[1].ToDouble();
						star.dec = tokens[2].ToDouble();
						star.v = tokens[3].ToDouble();
						if (tokens.Length()==6){
							star.spType = tokens[5];
						}
#if DEBUG
						Console().WriteLn(IsoString().Format("star=%s, ra=%f, dec=%f",star.mainId.c_str(),star.ra,star.dec));
#endif
						m_skymap->addObjectToFoV(star);
					}
				}
			}
			m_downloadedFile.Clear();
			m_isAllSkyView = false;
		}
	}
}


bool INDIMountInterface::DownloadObjectCoordinates(NetworkTransfer &sender, const void *buffer, fsize_type size){
	 m_downloadedFile.Append(static_cast<const char*>(buffer),size);
	 return true;
 }

void INDIMountInterface::UpdateDeviceList(){

	if (TheINDIDeviceControllerInterface!=0){
		INDIDeviceControllerInstance* pInstance=&TheINDIDeviceControllerInterface->instance;

		if (pInstance->p_deviceList.Begin()==pInstance->p_deviceList.End())
			return;

		for (INDIDeviceControllerInstance::DeviceListType::iterator iter=pInstance->p_deviceList.Begin() ; iter!=pInstance->p_deviceList.End(); ++iter){
			GUI->MountDevice_Combo.AddItem(iter->DeviceName);
		}
		GUI->UpdateDeviceList_Timer.Stop();

	}

}

void INDIMountInterface::UpdateDeviceList_Timer( Timer &sender )
{
	if( sender == GUI->UpdateDeviceList_Timer  ){
		UpdateDeviceList();
	}
}

void INDIMountInterface::UpdateMount_Timer( Timer &sender )
{
	if (TheINDIDeviceControllerInterface != 0) {
		if (sender == GUI->UpdateMount_Timer) {
			INDINewPropertyListItem newPropertyListItem;
			// get temperature value
			INDIDeviceControllerInstance* pInstance =
					&TheINDIDeviceControllerInterface->instance;

			if (pInstance==NULL)
				return;

			INDIPropertyListItem MountProp;
			// get JULIANDATE value
			if (pInstance->getINDIPropertyItem(m_Device,
					"JULIAN", "JULIANDATE", MountProp)) {
				GUI->UTC_Edit.SetText(MountProp.PropertyValue);
			}
			// get LST value
			if (pInstance->getINDIPropertyItem(m_Device,
					"TIME_LST", "LST", MountProp)) {
					GUI->LST_Edit.SetText(MountProp.PropertyValue);
					pInstance->getINDIPropertyItem(m_Device,
										"TIME_LST", "LST", MountProp,false);
					m_lst = MountProp.PropertyValue.ToDouble();
			}
			// get RA value
			if (pInstance->getINDIPropertyItem(m_Device,
								"EQUATORIAL_EOD_COORD", "RA", MountProp)) {
								GUI->RA_Edit.SetText(MountProp.PropertyValue);
				pInstance->getINDIPropertyItem(m_Device,"EQUATORIAL_EOD_COORD", "RA", MountProp,false);
				m_scopeRA = MountProp.PropertyValue.ToDouble();
			}
			// get DEC value
			if (pInstance->getINDIPropertyItem(m_Device,
								"EQUATORIAL_EOD_COORD", "DEC", MountProp)) {
								GUI->DEC_Edit.SetText(MountProp.PropertyValue);
				pInstance->getINDIPropertyItem(m_Device,"EQUATORIAL_EOD_COORD", "DEC", MountProp,false);
				m_scopeDEC = MountProp.PropertyValue.ToDouble();
			}
			// get ALIGNEDCOORD RA value
			if (pInstance->getINDIPropertyItem(m_Device, "ALIGNTELESCOPECOORDS",
					"ALIGNTELESCOPE_RA", MountProp,false)) {
				m_alignedRA = MountProp.PropertyValue.ToDouble();
			}
			// get ALIGNEDCOORD DEC value
			if (pInstance->getINDIPropertyItem(m_Device, "ALIGNTELESCOPECOORDS",
					"ALIGNTELESCOPE_DE", MountProp,false)) {
				m_alignedDEC = MountProp.PropertyValue.ToDouble();
			}
			// get ALT value
			if (pInstance->getINDIPropertyItem(m_Device,
								"HORIZONTAL_COORD", "ALT", MountProp)) {
								GUI->ALT_Edit.SetText(MountProp.PropertyValue);
						}
			// get AZ value
			if (pInstance->getINDIPropertyItem(m_Device,
								"HORIZONTAL_COORD", "AZ", MountProp)) {
								GUI->AZ_Edit.SetText(MountProp.PropertyValue);
			}
			// get LAT value
			if (pInstance->getINDIPropertyItem(m_Device,
								"GEOGRAPHIC_COORD", "LAT", MountProp,false)) {
								m_geoLat=MountProp.PropertyValue.ToDouble();
			}
		}
		if (m_isAllSkyView){
			GUI->SkyChart_AllSkyGraphicsControl.Repaint();
		} else {
			GUI->SkyChart_FoVGraphicsControl.Repaint();
		}
	}
}

void INDIMountInterface::ComboItemSelected(ComboBox& sender, int itemIndex) {
	if (sender == GUI->MountDevice_Combo){
		m_Device = sender.ItemText(itemIndex);
		if (TheINDIDeviceControllerInterface != 0) {

			INDIDeviceControllerInstance* pInstance = &TheINDIDeviceControllerInterface->instance;

			if (pInstance==NULL)
				return;

			// Start update time
			GUI->UpdateMount_Timer.Start();

		}
		Sleep(2);
		// Download stars from simbad database
		NetworkTransfer transfer;
		IsoString url =	IsoString("http://simbad.u-strasbg.fr/simbad/sim-tap/sync?request=doQuery&lang=adql&format=text&query=");
		//IsoString url =	IsoString("http://simbad.cfa.harvard.edu/simbad/sim-tap/sync?request=doQuery&lang=adql&format=text&query=");

		SkyMap::geoCoord geoCoord;
		geoCoord.geo_lat = m_geoLat;
		SkyMap::filter filter;
		filter.dec_lowerLimit =	m_geoLat < 0 ? 90.0 - m_geoLat : m_geoLat - 90.0;
		filter.dec_upperLimit = m_geoLat < 0 ? -90.0 : 90.0;
		filter.v_upperLimit = m_limitStarMag;

		m_skymap = new SkyMap(filter, geoCoord);
		IsoString select_stmt = m_skymap->getASDLQueryString();
		Console().WriteLn(
				IsoString().Format("QueryStr = %s",
						m_skymap->getASDLQueryString().c_str()));
		url.Append(select_stmt);
		transfer.SetURL(url);
		transfer.OnDownloadDataAvailable(
				(NetworkTransfer::download_event_handler) &INDIMountInterface::DownloadObjectCoordinates,
				*this);
		if (!transfer.Download()) {
			Console().WriteLn(
					String().Format("Download failed with error '%s'",
							IsoString(transfer.ErrorInformation()).c_str()));
			if (transfer.WasAborted()){
				Console().WriteLn("Download was aborted");
			}
		} else {
			Console().WriteLn(
					String().Format(
							"%d bytes downloaded with speed %f KiB/seconds.",
							transfer.BytesTransferred(),
							transfer.TotalSpeed()));

			StringList lines;
			m_downloadedFile.Break(lines, '\n', true);
			Console().WriteLn(IsoString().Format("%s",m_downloadedFile.c_str()));
			if (lines.Length() > 0) {
				for (size_t i = 0; i < lines.Length(); i++) {
					if (i <= 1)
						continue;

					StringList tokens;
					lines[i].Break(tokens, '|', true);
					if (tokens.Length() != 5)
						continue;
					SkyMap::object star;
					star.mainId = tokens[0];
					star.ra = tokens[1].ToDouble();
					star.dec = tokens[2].ToDouble();
					star.v = tokens[3].ToDouble();
					star.spType = tokens[4];
#if DEBUG
					Console().WriteLn(IsoString().Format("star=%s, ra=%f, dec=%f, vmag=%f, sp=%s",star.mainId.c_str(),star.ra,star.dec,star.v,star.spType.c_str()));
#endif
					m_skymap->addObject(star);
				}
			}
		}
		m_downloadedFile.Clear();
	}
}


 void INDIMountInterface::EditCompleted(Edit& sender){
	 /*if (sender == GUI->MountGoto_PushButton){

	 }*/
 }


 void INDIMountInterface::Button_Click(Button& sender, bool checked){

	if (sender == GUI->MountSet_PushButton) {
		EditEqCoordPropertyDialog eqCoordDialog(GUI->RA_Edit.Text(),GUI->DEC_Edit.Text());
		if (eqCoordDialog.Execute()) {

			GUI->TRA_Edit.SetText(String().Format("%02d:%02d:%2.2f",(int) eqCoordDialog.getRaHour(),(int) eqCoordDialog.getRaMinute(), eqCoordDialog.getRaSecond()));
			GUI->TDEC_Edit.SetText(String().Format("%02d:%02d:%2.2f",(int) eqCoordDialog.getDecDeg(),(int) eqCoordDialog.getDecArcMinute(), eqCoordDialog.getDecArcSecond()));

			m_TargetRA= eqCoordDialog.getTargetRaCoord();
			m_TargetDEC= eqCoordDialog.getTargetDECCoord();


		}
	 } else if (sender == GUI->MountGoto_PushButton) {
		INDIDeviceControllerInstance* pInstance =	&TheINDIDeviceControllerInterface->instance;

		if (pInstance == NULL)
			return;

		INDIDeviceControllerInstance::NewPropertyListType propertyVector;
		INDINewPropertyListItem newPropertyListItem;
		newPropertyListItem.Device = m_Device;
		newPropertyListItem.Property = String("EQUATORIAL_EOD_COORD");
		newPropertyListItem.Element = String("RA");
		newPropertyListItem.PropertyType = String("INDI_NUMBER");
		newPropertyListItem.NewPropertyValue = m_TargetRA;
		propertyVector.Add(newPropertyListItem);

		newPropertyListItem.Element = String("DEC");
		newPropertyListItem.NewPropertyValue = m_TargetDEC;
		propertyVector.Add(newPropertyListItem);

		// send (RA,DEC) coordinates in propertyVector
		pInstance->sendNewPropertyVector(propertyVector, true);
	} else if (sender == GUI->MountSearch_PushButton) {

		CoordSearchDialog coordSearchDialog;
		if (coordSearchDialog.Execute()){

			double target_ra = coordSearchDialog.getTargetRaCoord();
			double target_dec = coordSearchDialog.getTargetDECCoord();
			CoordUtils::coord_HMS coord_RA_HMS = CoordUtils::convertToHMS(target_ra);
			CoordUtils::coord_HMS coord_DEC_HMS = CoordUtils::convertToHMS(target_dec);

			GUI->TRA_Edit.SetText(String().Format("%02d:%02d:%2.2f",(int) coord_RA_HMS.hour,(int) coord_RA_HMS.minute, coord_RA_HMS.second));
			GUI->TDEC_Edit.SetText(String().Format("%02d:%02d:%2.2f",(int) coord_DEC_HMS.hour,(int) coord_DEC_HMS.minute, coord_DEC_HMS.second));

			m_TargetRA= String(target_ra);
			m_TargetDEC= String(target_dec);
		}

	} else if (sender == GUI->MountSynch_PushButton) {

		INDIDeviceControllerInstance* pInstance =	&TheINDIDeviceControllerInterface->instance;

		if (pInstance == NULL)
			return;

		INDINewPropertyListItem newPropertyListItem;
		newPropertyListItem.Device = m_Device;
		newPropertyListItem.Property = String("ON_COORD_SET");
		newPropertyListItem.Element = String("SYNC");
		newPropertyListItem.PropertyType = String("INDI_SWITCH");
		newPropertyListItem.NewPropertyValue = IsoString("ON");
		pInstance->sendNewPropertyValue(newPropertyListItem, true);

		INDIDeviceControllerInstance::NewPropertyListType propertyVector;
		newPropertyListItem.Property = String("EQUATORIAL_EOD_COORD");
		newPropertyListItem.Element = String("RA");
		newPropertyListItem.PropertyType = String("INDI_NUMBER");
		newPropertyListItem.NewPropertyValue = m_TargetRA;
		propertyVector.Add(newPropertyListItem);

		newPropertyListItem.Element = String("DEC");
		newPropertyListItem.NewPropertyValue = m_TargetDEC;
		propertyVector.Add(newPropertyListItem);

		// send (RA,DEC) coordinates in propertyVector
		pInstance->sendNewPropertyVector(propertyVector, true);

		newPropertyListItem.Device = m_Device;
		newPropertyListItem.Property = String("ON_COORD_SET");
		newPropertyListItem.Element = String("TRACK");
		newPropertyListItem.PropertyType = String("INDI_SWITCH");
		newPropertyListItem.NewPropertyValue = IsoString("ON");
		pInstance->sendNewPropertyValue(newPropertyListItem, true);

	}
 }


 void INDIMountInterface::SkyChart_Paint( Control& sender, const Rect& updateRect ){
	Graphics g(sender);

	RGBA darkRed = RGBAColor(153, 0, 0);
	RGBA darkYellow = RGBAColor(153, 153, 0);
	RGBA darkGreen = RGBAColor(0, 153, 0);

	Rect r(sender.BoundsRect());

	int w = r.Width();
	int h = r.Height();
	int x0 = w >> 1;
	int y0 = h >> 1;

	g.FillRect(r, RGBAColor(0, 0, 0));
	g.SetBrush(Brush::Null());

	g.SetPen(darkRed);
	double margin=10;
	g.DrawLine(x0, 0+margin, x0, h-margin);
	g.DrawLine(0+margin, y0, w-margin, y0);

	g.EnableAntialiasing();

	if (this->m_isAllSkyView){
		double chartRadius=x0-margin;
		g.DrawCircle(x0,y0,chartRadius);

		// draw telescope position
		StereoProjection::sphericalCoord spherical;

		double hourAngle = (m_TargetRA.ToDouble() - m_lst) * 360.0 / 24.0;
		double currentAlt=SkyMap::getObjectAltitude(m_TargetDEC.ToDouble(),hourAngle,m_geoLat);
		double currentAz=SkyMap::getObjectAzimut(m_TargetDEC.ToDouble(),hourAngle,m_geoLat);
		spherical.phi = Rad(currentAz);
		spherical.theta = Rad(90.0 + currentAlt);
		StereoProjection::polarCoord polarCoord = StereoProjection::projectScaled(spherical,chartRadius);
		double x = StereoProjection::polarToEuklid(polarCoord).x;
		double y = StereoProjection::polarToEuklid(polarCoord).y;
#if 0
		Console().WriteLn(String().Format("x=%f, y=%f, r=%f",x,y,chartRadius));
		Console().WriteLn(String().Format("x0=%d, y0=%d",x0,y0));
		Console().WriteLn(String().Format("w=%d, h=%d",w,h));
		Console().WriteLn(String().Format("phi=%f, theta=%f",spherical.phi,spherical.theta));
		Console().WriteLn(String().Format("r=%f, pphi=%f",polarCoord.r,polarCoord.phi));
#endif
		g.DrawCircle(x0+x,y0+y,5);

		g.SetPen(darkGreen);
		hourAngle = (m_scopeRA - m_lst) * 360.0 / 24.0;
		currentAlt=SkyMap::getObjectAltitude(m_scopeDEC,hourAngle,m_geoLat);
		currentAz=SkyMap::getObjectAzimut(m_scopeDEC,hourAngle,m_geoLat);
		spherical.phi = Rad(currentAz);
		spherical.theta = Rad(90.0 + currentAlt);
		polarCoord = StereoProjection::projectScaled(spherical,chartRadius);
		x = StereoProjection::polarToEuklid(polarCoord).x;
		y = StereoProjection::polarToEuklid(polarCoord).y;
		g.DrawCircle(x0+x,y0+y,5);
		g.SetPen(darkYellow);

		if (m_skymap != NULL) {
			m_skymap->plotStars(m_lst, m_geoLat, x0, y0, chartRadius, g, m_limitStarMag);
		}
	} else {
		if (m_skymap != NULL) {
			double CCD_chipHeight=2200;
			double CCD_chipWidth=2750;
			double CCD_pixelSize=4.54 /1000;
			double TEL_focalLength=700;
			double FoV_width=CCD_chipWidth*CCD_pixelSize  / TEL_focalLength * 3438 / 60;
			double FoV_height=CCD_chipHeight*CCD_pixelSize / TEL_focalLength * 3438 / 60;

			double scale = 180.0 / FoV_width;

			// draw scope position
			double currentAlt = SkyMap::getObjectAltitude(m_scopeDEC, m_scopeRA * 360.0 / 24.0,m_geoLat);
			double currentAz = SkyMap::getObjectAzimut(m_scopeDEC, m_scopeRA * 360.0 / 24.0,m_geoLat);

			StereoProjection::sphericalCoord spherical;
			spherical.phi = Rad(currentAz);
			spherical.theta = Rad(90.0 + currentAlt);
			StereoProjection::polarCoord polarCoord = StereoProjection::projectScaled(spherical,scale * x0);
			double x = StereoProjection::polarToEuklid(polarCoord).x;
			double y = StereoProjection::polarToEuklid(polarCoord).y;

			//Console().WriteLn(String().Format("xx=%f, yy=%f, r=%f",x,y,scale * x0));
			g.DrawCircle(x0 -x + x, y0 - y + y, 5);

			// draw alignment deviation
			double currentAlignAlt = SkyMap::getObjectAltitude(m_alignedDEC, m_alignedRA * 360.0 / 24.0,m_geoLat);
			double currentAlignAz = SkyMap::getObjectAzimut(m_alignedDEC,m_alignedRA * 360.0 / 24.0, m_geoLat);

			StereoProjection::sphericalCoord alignSpherical;
			alignSpherical.phi = Rad(currentAlignAz);
			alignSpherical.theta = Rad(90.0 + currentAlignAlt);
			StereoProjection::polarCoord alignPolarCoord =	StereoProjection::projectScaled(alignSpherical, scale * x0);
			double alignX = StereoProjection::polarToEuklid(alignPolarCoord).x;
			double alignY = StereoProjection::polarToEuklid(alignPolarCoord).y;

			//Console().WriteLn(String().Format("xx=%f, yy=%f, r=%f",x,y,scale * x0));
			g.SetPen(darkGreen);
			g.DrawLine(x0 - x + x, y0 - y + y, x0 - x + alignX, y0 - y +alignY);


			m_skymap->plotFoVStars(m_lst, m_geoLat, x0-x, y0-y, scale * x0, g, 13);
		}
	}

}

} // pcl

// ****************************************************************************
// EOF PixInsightINDIInterface.cpp - Released 2013/03/24 18:42:27 UTC
