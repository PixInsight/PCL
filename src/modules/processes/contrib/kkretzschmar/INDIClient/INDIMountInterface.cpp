//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.07.0141
// ----------------------------------------------------------------------------
// INDIMountInterface.cpp - Released 2016/04/28 15:13:36 UTC
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

#include <pcl/Console.h>
#include <pcl/Graphics.h>
#include <pcl/Math.h>

#include "INDIDeviceControllerInterface.h"
#include "INDIMountInterface.h"
#include "INDIMountProcess.h"
#include "SkyMap.h"

#include "INDI/basedevice.h"
#include "INDI/indiapi.h"
#include "INDI/indibase.h"

#include <assert.h>

#define DEBUG 0

namespace pcl
{

// ----------------------------------------------------------------------------

INDIMountInterface* TheINDIMountInterface = nullptr;

// ----------------------------------------------------------------------------

//#include "PixInsightINDIIcon.xpm"

// ----------------------------------------------------------------------------

INDIMountInterface::INDIMountInterface() :
   ProcessInterface(),
   instance( TheINDIMountProcess ),
   GUI( nullptr ),
   m_serverMessage(),
   m_Device(),
   m_TargetRA( "0" ),
   m_TargetDEC( "0" ),
   m_downloadedFile( "2" ),
   m_skymap( nullptr ),
   m_geoLat( 0 ),
   m_lst( 0 ),
   m_scopeRA( 0 ),
   m_scopeDEC( 0 ),
   m_alignedRA( 0 ),
   m_alignedDEC( 0 ),
   m_limitStarMag( 4.5 ),
   m_isAllSkyView( true )
{
   TheINDIMountInterface = this;
}

INDIMountInterface::~INDIMountInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
   if ( m_skymap != nullptr )
      delete m_skymap, m_skymap = nullptr;
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
   return nullptr; // ### TODO
}

InterfaceFeatures INDIMountInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

void INDIMountInterface::ResetInstance()
{
   INDIMountInstance defaultInstance( TheINDIMountProcess );
   ImportProcess( defaultInstance );
}

bool INDIMountInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData(*this );
      SetWindowTitle( "INDI Mount Controller" );
      ResetInstance();
      UpdateControls();
   }

   dynamic = false;
   return &P == TheINDIMountProcess;
}

ProcessImplementation* INDIMountInterface::NewProcess() const
{
   INDIMountInstance* instance = new INDIMountInstance(TheINDIMountProcess);
   instance->p_deviceName = m_Device;
   // TODO copy other process parameters
   return instance ;

}

bool INDIMountInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const INDIMountInstance* r = dynamic_cast<const INDIMountInstance*>( &p );
   if ( r == nullptr )
   {
      whyNot = "Not a INDIMount instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool INDIMountInterface::RequiresInstanceValidation() const
{
   return true;
}

bool INDIMountInterface::ImportProcess( const ProcessImplementation& p )
{
   const INDIMountInstance* instance = dynamic_cast<const INDIMountInstance*>( &p );
   if (instance != nullptr)
   {
	   // set GUI parameters from imported process instance
	   UpdateControls(); // Remove
	   if ( instance->ValidateDevice( false/*throwErrors*/ ) )
	   {
		   m_Device = instance->p_deviceName;
		   // instance->SendDeviceProperties();
	   }
	   else
		   m_Device.Clear();

	   return true;
   }
   return false;
}

// ----------------------------------------------------------------------------

void INDIMountInterface::UpdateControls()
{
	if ( m_Device.IsEmpty() )
	{
		GUI->UpdateDeviceProperties_Timer.Stop();
		GUI->MountProperties_Control.Disable();
		GUI->MountGoto_Control.Disable();
	}
	else
	{
		if ( !GUI->UpdateDeviceProperties_Timer.IsRunning() )
			GUI->UpdateDeviceProperties_Timer.Start();

		GUI->MountProperties_Control.Enable();
		GUI->MountGoto_Control.Enable();
	}
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

   CoordUtils::HMS ra_hms = CoordUtils::parse(raCoord);
   CoordUtils::HMS dec_hms = CoordUtils::parse(decCoord);

   SetWindowTitle( "Set equatorial coordinates for target object." );

   RA_Property_Label.SetMinWidth(labelWidth);
   RA_Property_Label.SetText( "RA (hh:mm:ss.ss)" );
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
   DEC_Property_Label.SetText( "Dec (dd:mm:ss.ss)" );
   DEC_Property_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

   DEC_Hour_Edit.SetMaxWidth(editWidth);
   DEC_Hour_Edit.SetText(IsoString(dec_hms.hour));
   DEC_Hour_Edit.OnEditCompleted(
         (Edit::edit_event_handler) &EditEqCoordPropertyDialog::EditCompleted,
         *this);

   DEC_Colon1_Label.SetText( ":" );

   DEC_Minute_Edit.SetMaxWidth(editWidth);
   DEC_Minute_Edit.SetText(IsoString(dec_hms.minute));
   DEC_Minute_Edit.OnEditCompleted(
         (Edit::edit_event_handler) &EditEqCoordPropertyDialog::EditCompleted,
         *this);

   DEC_Colon2_Label.SetText( ":" );

   DEC_Second_Edit.SetMaxWidth(editWidth);
   DEC_Second_Edit.SetText(IsoString(dec_hms.second));
   DEC_Second_Edit.OnEditCompleted(
         (Edit::edit_event_handler) &EditEqCoordPropertyDialog::EditCompleted,
         *this);

   OK_PushButton.SetText( "OK" );
   OK_PushButton.OnClick(
         (Button::click_event_handler) &EditEqCoordPropertyDialog::Ok_Button_Click,
         *this);
   Cancel_PushButton.SetText( "Cancel" );
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

   int emWidth = fnt.Width( 'm' );
   int editWidth = 8*emWidth;
   int searchEditWidth = fnt.Width(String('0', 15));
   SetWindowTitle( "Online Search" );

   Search_Sizer.SetSpacing(10);
   Search_Sizer.SetMargin(10);

   Search_Label.SetText( "Object: " );
   Search_Label.SetToolTip( "<p>Search for object </p>" );
   Search_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

   Search_Edit.SetMinWidth(searchEditWidth);
   Search_Edit.SetMaxWidth(70);
   Search_Edit.OnEditCompleted((Edit::edit_event_handler) &CoordSearchDialog::EditCompleted,*this);

   Search_PushButton.SetText( "Search" );
   Search_PushButton.OnClick((Button::click_event_handler) &CoordSearchDialog::Button_Click,*this);

   Search_Sizer.Add(Search_Label);
   Search_Sizer.Add(Search_Edit);
   Search_Sizer.Add(Search_PushButton);

   Global_Sizer.Add(Search_Sizer);

   SearchResult_Sizer.SetSpacing(10);
   SearchResult_Sizer.SetMargin(10);

   TRA_Label.SetText( "Target RA:" );
   TRA_Label.SetToolTip( "<p>Target right ascension position </p>" );
   TRA_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

   TRA_Edit.SetMinWidth(editWidth);
   TRA_Edit.SetMaxWidth(70);
   TRA_Edit.SetText( "00:00:00" );
   TRA_Edit.Disable();

   TDEC_Label.SetText( "Target Dec:" );
   TDEC_Label.SetToolTip( "<p>Target declination position </p>" );
   TDEC_Label.SetTextAlignment(TextAlign::Left | TextAlign::VertCenter);

   TDEC_Edit.SetMinWidth(editWidth);
   TDEC_Edit.SetMaxWidth(70);
   TDEC_Edit.SetText( "00:00:00" );
   TDEC_Edit.Disable();

   SearchResult_Sizer.Add(TRA_Label);
   SearchResult_Sizer.Add(TRA_Edit);
   SearchResult_Sizer.Add(TDEC_Label);
   SearchResult_Sizer.Add(TDEC_Edit);

   Global_Sizer.Add(SearchResult_Sizer);

   SearchControl_Sizer.SetSpacing(10);
   SearchControl_Sizer.SetMargin(10);

   SearchOK_PushButton.SetText( "OK" );
   SearchOK_PushButton.OnClick((Button::click_event_handler) &CoordSearchDialog::Button_Click,*this);
   SearchCancel_PushButton.SetText( "Cancel" );
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

   if ( sender == Search_PushButton )
   {
      NetworkTransfer transfer;
      String url( "http://simbad.u-strasbg.fr/simbad/sim-tap/sync?request=doQuery&lang=adql&format=text&query=" );
      String select_stmt = "SELECT basic.OID, RA, DEC, main_id AS \"Main identifier\" FROM basic JOIN ident ON oidref = oid WHERE id = '" + String( m_targetObj ) + "';";
      //String url( "http://vizier.cfa.harvard.edu/viz-bin/nph-sesame/-oI/A?" );
      //url.Append(m_targetObj);
      url.Append( select_stmt );
      transfer.SetURL( url );
      transfer.OnDownloadDataAvailable(
            (NetworkTransfer::download_event_handler) &CoordSearchDialog::DownloadObjectCoordinates, *this );
      if ( !transfer.Download() )
      {
         Console().WriteLn( "Download failed with error '" + String( transfer.ErrorInformation() ) + "'" );
      }
      else
      {
         Console().WriteLn( String().Format( "%d bytes downloaded @ %.3g KiB/s",
                                             transfer.BytesTransferred(), transfer.TotalSpeed() ) );
         StringList lines;
         m_downloadedFile.Break( lines, '\n', true/*trim*/ );
         if ( lines.Length() >= 3 )
         {
            StringList tokens;
            lines[2].Break( tokens, '|', true/*trim*/ );
            if ( tokens.Length() == 4 )
            {
               double ra_in_hours = tokens[1].ToDouble()/360*24;
               m_RA_TargetCoord = ra_in_hours;
               m_DEC_TargetCoord = tokens[2].ToDouble();
               CoordUtils::HMS coord_RA_HMS = CoordUtils::convertToHMS( m_RA_TargetCoord );
               CoordUtils::HMS coord_DEC_HMS = CoordUtils::convertToHMS( m_DEC_TargetCoord );
               TRA_Edit.SetText( String().Format( "%02d:%02d:%2.2f",
                                             int( coord_RA_HMS.hour ),
                                             int( coord_RA_HMS.minute ),
                                             coord_RA_HMS.second ) );
               TDEC_Edit.SetText( String().Format( "%02d:%02d:%2.2f",
                                             int( coord_DEC_HMS.hour ),
                                             int( coord_DEC_HMS.minute ),
                                             coord_DEC_HMS.second ) );
            }
         }
      }
      m_downloadedFile.Clear();

   }
   else if ( sender == SearchOK_PushButton )
   {
      Ok();
   }
   else if ( sender == SearchCancel_PushButton )
   {
      m_RA_TargetCoord = m_DEC_TargetCoord = 0;
      Cancel();
   }
}

INDIMountInterface::GUIData::GUIData(INDIMountInterface& w )
{
   int emWidth = w.Font().Width( 'm' );
   int labelWidth1 = w.Font().Width( "Server file name template:" ) + emWidth;
   int editWidth1 = 6*emWidth;
   int editWidth2 = 8*emWidth;

   // Mount Device Properties Section ==============================================================
   ServerParameters_SectionBar.SetTitle( "Device Properties" );
   ServerParameters_SectionBar.SetSection( ServerParameters_Control );

   MountDevice_Label.SetText( "INDI Mount device:" );
   MountDevice_Label.SetToolTip( "<p>Select an INDI Mount device.</p>" );
   MountDevice_Label.SetMinWidth( labelWidth1 );
   MountDevice_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   MountDevice_Combo.OnItemSelected( (ComboBox::item_event_handler)& INDIMountInterface::e_ItemSelected, w );

   MountDevice_Sizer.SetSpacing( 4);
   MountDevice_Sizer.Add( MountDevice_Label );
   MountDevice_Sizer.Add( MountDevice_Combo, 100 );


   LST_Label.SetText( "Local Siderial Time (h:m:s):" );
   LST_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   LST_Label.SetFixedWidth(labelWidth1);
   LST_Label.SetToolTip( "<p>Local Siderial Time (LST)</p>" );

   LST_H_Edit.SetInteger();
   LST_H_Edit.SetRange(0,23);
   LST_H_Edit.edit.SetFixedWidth(editWidth1);
   LST_H_Edit.SetToolTip( "<p>Local Siderial Time: (hour)</p>" );
   LST_H_Edit.Disable();
   LST_M_Edit.SetInteger();
   LST_M_Edit.SetRange(0,59);
   LST_M_Edit.edit.SetFixedWidth(editWidth1);
   LST_M_Edit.SetToolTip( "<p>Local Siderial Time: (minutes)</p>" );
   LST_M_Edit.Disable();
   LST_S_Edit.SetReal();
   LST_S_Edit.SetPrecision(3);
   LST_S_Edit.SetRange(0,59);
   LST_S_Edit.edit.SetFixedWidth(editWidth1);
   LST_S_Edit.SetToolTip( "<p>Local Siderial Time: (seconds)</p>" );
   LST_S_Edit.Disable();
   MountLST_Sizer.Add(LST_Label);
   MountLST_Sizer.Add(LST_H_Edit);
   MountLST_Sizer.Add(LST_M_Edit);
   MountLST_Sizer.Add(LST_S_Edit);
   MountLST_Sizer.AddStretch();

   RA_Label.SetText( "Right Ascension (h:m:s):" );
   RA_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RA_Label.SetFixedWidth(labelWidth1);
   RA_H_Edit.SetInteger();
   RA_H_Edit.SetRange(0,23);
   RA_H_Edit.SetToolTip( "<p>Telescope position in equatorial coordinates: Right Ascension (hour)</p>" );
   RA_H_Edit.edit.SetFixedWidth(editWidth1);
   RA_H_Edit.Disable();
   RA_M_Edit.SetInteger();
   RA_M_Edit.SetRange(0,59);
   RA_M_Edit.edit.SetFixedWidth(editWidth1);
   RA_M_Edit.SetToolTip( "<p>Telescope position in equatorial coordinates: Right Ascension (minutes)</p>" );
   RA_M_Edit.Disable();
   RA_S_Edit.SetReal();
   RA_S_Edit.SetPrecision(3);
   RA_S_Edit.SetRange(0,59);
   RA_S_Edit.edit.SetFixedWidth(editWidth1);
   RA_S_Edit.SetToolTip( "<p>Telescope position in equatorial coordinates: Right Ascension (seconds)</p>" );
   RA_S_Edit.Disable();
   MountEQRA_Sizer.Add(RA_Label);
   MountEQRA_Sizer.Add(RA_H_Edit);
   MountEQRA_Sizer.Add(RA_M_Edit);
   MountEQRA_Sizer.Add(RA_S_Edit);
   MountEQRA_Sizer.AddStretch();

   DEC_Label.SetText( "Declination (d:m:s):" );
   DEC_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   DEC_Label.SetFixedWidth(labelWidth1);
   DEC_D_Edit.SetInteger();
   DEC_D_Edit.SetRange(0,59);
   DEC_D_Edit.edit.SetFixedWidth(editWidth1);
   DEC_D_Edit.SetToolTip( "<p>Telescope position in equatorial coordinates: Declination (degrees)</p>" );
   DEC_D_Edit.Disable();
   DEC_M_Edit.SetInteger();
   DEC_M_Edit.SetRange(0,59);
   DEC_M_Edit.edit.SetFixedWidth(editWidth1);
   DEC_M_Edit.SetToolTip( "<p>Telescope position in equatorial coordinates: Declination (minutes)</p>" );
   DEC_M_Edit.Disable();
   DEC_S_Edit.SetReal();
   DEC_S_Edit.SetPrecision(3);
   DEC_S_Edit.SetRange(0,59);
   DEC_S_Edit.edit.SetFixedWidth(editWidth1);
   DEC_S_Edit.SetToolTip( "<p>Telescope position in equatorial coordinates: Declination (seconds)</p>" );
   DEC_S_Edit.Disable();
   MountEQDEC_Sizer.Add(DEC_Label);
   MountEQDEC_Sizer.Add(DEC_D_Edit);
   MountEQDEC_Sizer.Add(DEC_M_Edit);
   MountEQDEC_Sizer.Add(DEC_S_Edit);
   MountEQDEC_Sizer.AddStretch();

   AZ_Label.SetText( "Azimuth (d:m:s):" );
   AZ_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   AZ_Label.SetFixedWidth(labelWidth1);
   AZ_D_Edit.SetInteger();
   AZ_D_Edit.SetRange(0,59);
   AZ_D_Edit.edit.SetFixedWidth(editWidth1);
   AZ_D_Edit.SetToolTip( "<p>Telescope position in equatorial coordinates: Azimuth (degrees)</p>" );
   AZ_D_Edit.Disable();
   AZ_M_Edit.SetInteger();
   AZ_M_Edit.SetRange(0,59);
   AZ_M_Edit.edit.SetFixedWidth(editWidth1);
   AZ_M_Edit.SetToolTip( "<p>Telescope position in equatorial coordinates: Azimuth (minutes)</p>" );
   AZ_M_Edit.Disable();
   AZ_S_Edit.SetReal();
   AZ_S_Edit.SetPrecision(3);
   AZ_S_Edit.SetRange(0,59);
   AZ_S_Edit.edit.SetFixedWidth(editWidth1);
   AZ_S_Edit.SetToolTip( "<p>Telescope position in equatorial coordinates: Azimuth (seconds)</p>" );
   AZ_S_Edit.Disable();
   MountHZAZ_Sizer.Add(AZ_Label);
   MountHZAZ_Sizer.Add(AZ_D_Edit);
   MountHZAZ_Sizer.Add(AZ_M_Edit);
   MountHZAZ_Sizer.Add(AZ_S_Edit);
   MountHZAZ_Sizer.AddStretch();

   ALT_Label.SetText( "Altitude (d:m:s):" );
   ALT_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ALT_Label.SetFixedWidth(labelWidth1);
   ALT_D_Edit.SetInteger();
   ALT_D_Edit.SetRange(0,59);
   ALT_D_Edit.edit.SetFixedWidth(editWidth1);
   ALT_D_Edit.SetToolTip( "<p>Telescope position in equatorial coordinates: Altitude (degrees)</p>" );
   ALT_D_Edit.Disable();
   ALT_M_Edit.SetInteger();
   ALT_M_Edit.SetRange(0,59);
   ALT_M_Edit.edit.SetFixedWidth(editWidth1);
   ALT_M_Edit.SetToolTip( "<p>Telescope position in equatorial coordinates: Altitude (minutes)</p>" );
   ALT_M_Edit.Disable();
   ALT_S_Edit.SetReal();
   ALT_S_Edit.SetPrecision(3);
   ALT_S_Edit.SetRange(0,59);
   ALT_S_Edit.edit.SetFixedWidth(editWidth1);
   ALT_S_Edit.SetToolTip( "<p>Telescope position in equatorial coordinates: Altitude (seconds)</p>" );
   ALT_S_Edit.Disable();
   MountHZALT_Sizer.Add(ALT_Label);
   MountHZALT_Sizer.Add(ALT_D_Edit);
   MountHZALT_Sizer.Add(ALT_M_Edit);
   MountHZALT_Sizer.Add(ALT_M_Edit);
   MountHZALT_Sizer.AddStretch();

   MountProperties_Sizer.SetSpacing(4);
   MountProperties_Sizer.Add(MountLST_Sizer);
   MountProperties_Sizer.Add(MountEQRA_Sizer);
   MountProperties_Sizer.Add(MountEQDEC_Sizer);
   MountProperties_Sizer.Add(MountHZAZ_Sizer);
   MountProperties_Sizer.Add(MountHZALT_Sizer);

   MountProperties_Control.SetSizer(MountProperties_Sizer);

   ServerParameters_Sizer.SetSpacing( 4 );
   ServerParameters_Sizer.Add( MountDevice_Sizer );
   ServerParameters_Sizer.Add( MountProperties_Control );

   ServerParameters_Control.SetSizer( ServerParameters_Sizer );

   // Mount Device Goto Section
   MountGoto_SectionBar.SetTitle("Goto Target");
   MountGoto_SectionBar.SetSection(MountGoto_Control);

   TargetRA_Label.SetText( "Right Ascension (h::m::s):" );
   TargetRA_Label.SetToolTip("<p>Target object position in equatorial coordinates: Right Ascension (h::m::s)""");
   TargetRA_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   TargetRA_Label.SetFixedWidth(labelWidth1);
   TargetRA_H_SpinBox.SetRange(0,23);
   TargetRA_H_SpinBox.SetFixedWidth( editWidth2 );
   TargetRA_M_SpinBox.SetRange(0,59);
   TargetRA_M_SpinBox.SetFixedWidth( editWidth2 );

   TargetRA_S_NumericEdit.SetReal();
   TargetRA_S_NumericEdit.SetPrecision( 3 );
   TargetRA_S_NumericEdit.SetRange(0,59.999);
   TargetRA_S_NumericEdit.edit.SetFixedWidth( editWidth2 );
   TargetRA_S_NumericEdit.sizer.AddStretch();

   MountTargetRA_Sizer.SetSpacing(4);
   MountTargetRA_Sizer.Add(TargetRA_Label);
   MountTargetRA_Sizer.Add(TargetRA_H_SpinBox);
   MountTargetRA_Sizer.Add(TargetRA_M_SpinBox);
   MountTargetRA_Sizer.Add(TargetRA_S_NumericEdit);
   MountTargetRA_Sizer.AddStretch();

   TargetDEC_Label.SetText( "Declination (d::m::s):" );
   TargetDEC_Label.SetToolTip("<p>Target object position in equatorial coordinates: Declination (d::m::s)""");
   TargetDEC_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   TargetDEC_Label.SetFixedWidth(labelWidth1);
   TargetDEC_H_SpinBox.SetRange(0,90);
   TargetDEC_H_SpinBox.SetFixedWidth( editWidth2 );
   TargetDEC_M_SpinBox.SetRange(0,59);
   TargetDEC_M_SpinBox.SetFixedWidth( editWidth2 );
   TargetDEC_S_NumericEdit.SetReal();
   TargetDEC_S_NumericEdit.SetPrecision( 3 );
   TargetDEC_S_NumericEdit.SetRange(0,59.999);
   TargetDEC_S_NumericEdit.edit.SetFixedWidth( editWidth2 );
   MountTargetDECIsSouth_CheckBox.SetText("Southern hemisphere");
   MountTargetDECIsSouth_CheckBox.SetToolTip( "<p>When checked coordinates are negative</p>");

   MountTargetDEC_Sizer.SetSpacing(4);
   MountTargetDEC_Sizer.Add(TargetDEC_Label);
   MountTargetDEC_Sizer.Add(TargetDEC_H_SpinBox);
   MountTargetDEC_Sizer.Add(TargetDEC_M_SpinBox);
   MountTargetDEC_Sizer.Add(TargetDEC_S_NumericEdit);
   MountTargetDEC_Sizer.Add(MountTargetDECIsSouth_CheckBox);
   MountTargetDEC_Sizer.AddStretch();

   MountGotoLeft_Sizer.SetSpacing(4);
   MountGotoLeft_Sizer.Add(MountTargetRA_Sizer);
   MountGotoLeft_Sizer.Add(MountTargetDEC_Sizer);

   MountSearch_PushButton.SetText( "Search" );
   MountSearch_PushButton.SetIcon(w.ScaledResource( ":/icons/find.png" ));
   MountSearch_PushButton.SetStyleSheet( "QPushButton { text-align: left; }" );
   MountSearch_PushButton.OnClick( (Button::click_event_handler) &INDIMountInterface::e_Click, w );

   MountGotoRight_Sizer.SetSpacing(4);
   MountGotoRight_Sizer.Add(MountSearch_PushButton);

   MountGotoCoord_HSizer.SetSpacing(4);
   MountGotoCoord_HSizer.Add(MountGotoLeft_Sizer);
   MountGotoCoord_HSizer.Add(MountGotoRight_Sizer);
   MountGotoCoord_HSizer.AddStretch();

   MountGotoCommand_Label.SetText("Goto:");
   MountGotoCommand_Label.SetToolTip("<p>Start or stop goto execution");
   MountGotoCommand_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   MountGotoCommand_Label.SetFixedWidth(labelWidth1);
   MountGotoStart_PushButton.SetText( "Goto" );
   MountGotoStart_PushButton.SetIcon(w.ScaledResource( ":/icons/play.png" ));
   MountGotoStart_PushButton.SetStyleSheet( "QPushButton { text-align: left; }" );
   MountGotoStart_PushButton.OnClick( (Button::click_event_handler) &INDIMountInterface::e_Click, w );
   MountGotoCancel_PushButton.SetText( "Cancel" );
   MountGotoCancel_PushButton.SetIcon(w.ScaledResource( ":/icons/stop.png" ));
   MountGotoCancel_PushButton.SetStyleSheet( "QPushButton { text-align: left; }" );

   MountGotoStart_Sizer.SetSpacing(4);
   MountGotoStart_Sizer.Add(MountGotoCommand_Label);
   MountGotoStart_Sizer.Add(MountGotoStart_PushButton);
   MountGotoStart_Sizer.Add(MountGotoCancel_PushButton);
   MountGotoStart_Sizer.AddStretch();

   MountSynchCommand_Label.SetText("Synch:");
   MountSynchCommand_Label.SetToolTip("<p>Synchronize ...");
   MountSynchCommand_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   MountSynchCommand_Label.SetFixedWidth(labelWidth1);
   MountSynch_PushButton.SetText( "Synch" );
   MountSynch_PushButton.SetIcon(w.ScaledResource( ":/toolbar/image-mode-pan-horz.png" ));
   MountSynch_PushButton.SetStyleSheet( "QPushButton { text-align: left; }" );
   MountSynch_PushButton.OnClick( (Button::click_event_handler) &INDIMountInterface::e_Click, w );

   MountSynch_Sizer.SetSpacing(4);
   MountSynch_Sizer.Add(MountSynchCommand_Label);
   MountSynch_Sizer.Add(MountSynch_PushButton);
   MountSynch_Sizer.AddStretch();

   MountGoto_Sizer.SetSpacing(4);
   MountGoto_Sizer.Add(MountGotoCoord_HSizer);
   MountGoto_Sizer.Add(MountGotoStart_Sizer);
   MountGoto_Sizer.Add(MountSynch_Sizer);

   MountGoto_Control.SetSizer(MountGoto_Sizer);

   // Global Sizer
   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( ServerParameters_SectionBar );
   Global_Sizer.Add( ServerParameters_Control );
   Global_Sizer.Add( MountGoto_SectionBar );
   Global_Sizer.Add( MountGoto_Control );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedHeight();

   UpdateDeviceList_Timer.SetInterval( 0.5 );
   UpdateDeviceList_Timer.SetPeriodic( true );
   UpdateDeviceList_Timer.OnTimer( (Timer::timer_event_handler)&INDIMountInterface::UpdateDeviceList_Timer, w );
   UpdateDeviceList_Timer.Start();

   UpdateDeviceProperties_Timer.SetInterval( 1 );
   UpdateDeviceProperties_Timer.SetPeriodic( true );
   UpdateDeviceProperties_Timer.OnTimer( (Timer::timer_event_handler)&INDIMountInterface::UpdateMount_Timer, w );
}

bool INDIMountInterface::e_DownloadDataAvailable( NetworkTransfer& sender, const void* buffer, fsize_type size )
{
   m_downloadedFile.Append( static_cast<const char*>(buffer), size );
   return true;
}


void INDIMountInterface::UpdateDeviceList_Timer( Timer& sender )
{
   if ( sender == GUI->UpdateDeviceList_Timer )
   {
	   GUI->MountDevice_Combo.Clear();
	   if ( INDIClient::HasClient() )
	   {
		  INDIClient* indi = INDIClient::TheClient();
		  ExclConstDeviceList x = indi->ConstDeviceList();
		  const INDIDeviceListItemArray& devices(x);
		  if (devices.IsEmpty())
			 GUI->MountDevice_Combo.AddItem("<No Device Available>");
		  else {
			 GUI->MountDevice_Combo.AddItem("<No Device Selected>");

			 for (auto device : devices)
			 {
				INDIPropertyListItem item;
				if (indi->HasPropertyItem(device.DeviceName, "EQUATORIAL_EOD_COORD","RA")) // is this a camera device?
				GUI->MountDevice_Combo.AddItem(device.DeviceName);
			 }

			 int i = Max(0, GUI->MountDevice_Combo.FindItem(m_Device));
			 GUI->MountDevice_Combo.SetCurrentItem(i);
			 if (i > 0)
				 goto __device_found;
		  }
	   }
	   else
		   GUI->MountDevice_Combo.AddItem( "<INDI Server Not Connected>" );

	   m_Device.Clear();
   }
__device_found:
   UpdateControls();
}

void INDIMountInterface::UpdateMount_Timer( Timer& sender )
{
   if ( !INDIClient::HasClient() )
	   return;

   if ( sender == GUI->UpdateDeviceProperties_Timer )
   {
      INDIClient* indi = INDIClient::TheClient();
      INDIPropertyListItem MountProp;
      if ( indi->GetPropertyItem( m_Device, "TIME_LST", "LST", MountProp ) )
      {
    	 CoordUtils::HMS hms = CoordUtils::parse(MountProp.PropertyValue);
    	 GUI->LST_H_Edit.SetValue(hms.hour);
    	 GUI->LST_M_Edit.SetValue(hms.minute);
    	 GUI->LST_S_Edit.SetValue(hms.second);
         indi->GetPropertyItem( m_Device, "TIME_LST", "LST", MountProp, false/*formatted*/ );
         m_lst = MountProp.PropertyValue.ToDouble();
      }

      if ( indi->GetPropertyItem( m_Device, "EQUATORIAL_EOD_COORD", "RA", MountProp ) )
      {
    	 CoordUtils::HMS hms = CoordUtils::parse(MountProp.PropertyValue);
         GUI->RA_H_Edit.SetValue(hms.hour);
         GUI->RA_M_Edit.SetValue(hms.minute);
         GUI->RA_S_Edit.SetValue(hms.second);
         indi->GetPropertyItem( m_Device, "EQUATORIAL_EOD_COORD", "RA", MountProp, false/*formatted*/ );
         m_scopeRA = MountProp.PropertyValue.ToDouble();
      }

      if (indi->GetPropertyItem( m_Device, "EQUATORIAL_EOD_COORD", "DEC", MountProp ) )
      {
    	 CoordUtils::HMS hms = CoordUtils::parse(MountProp.PropertyValue);
         GUI->DEC_D_Edit.SetValue(hms.hour);
         GUI->DEC_M_Edit.SetValue(hms.minute);
         GUI->DEC_S_Edit.SetValue(hms.second);
         indi->GetPropertyItem( m_Device, "EQUATORIAL_EOD_COORD", "DEC", MountProp, false/*formatted*/ );
         m_scopeDEC = MountProp.PropertyValue.ToDouble();
      }

      if ( indi->GetPropertyItem( m_Device, "ALIGNTELESCOPECOORDS", "ALIGNTELESCOPE_RA", MountProp, false/*formatted*/ ) )
         m_alignedRA = MountProp.PropertyValue.ToDouble();

      if ( indi->GetPropertyItem( m_Device, "ALIGNTELESCOPECOORDS", "ALIGNTELESCOPE_DE", MountProp, false/*formatted*/ ) )
         m_alignedDEC = MountProp.PropertyValue.ToDouble();

      if ( indi->GetPropertyItem( m_Device, "HORIZONTAL_COORD", "ALT", MountProp ) )
      {
    	 CoordUtils::HMS hms = CoordUtils::parse(MountProp.PropertyValue);
    	 GUI->ALT_D_Edit.SetValue(hms.hour);
    	 GUI->ALT_M_Edit.SetValue(hms.minute);
    	 GUI->ALT_S_Edit.SetValue(hms.second);
      }
      if ( indi->GetPropertyItem( m_Device, "HORIZONTAL_COORD", "AZ", MountProp ) )
      {
    	 CoordUtils::HMS hms = CoordUtils::parse(MountProp.PropertyValue);
    	 GUI->AZ_D_Edit.SetValue(hms.hour);
    	 GUI->AZ_M_Edit.SetValue(hms.minute);
    	 GUI->AZ_S_Edit.SetValue(hms.second);
      }
      if ( indi->GetPropertyItem( m_Device, "GEOGRAPHIC_COORD", "LAT", MountProp, false/*formatted*/ ) )
         m_geoLat = MountProp.PropertyValue.ToDouble();
   }
}

void INDIMountInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( !INDIClient::HasClient() )
	  return;

   if ( sender == GUI->MountDevice_Combo )
   {
      m_Device = (itemIndex > 0) ? sender.ItemText( itemIndex ).Trimmed() : String();
      UpdateControls();
   }
}


void INDIMountInterface::e_Click(Button& sender, bool checked){

   if ( !INDIClient::HasClient() )
	  return;

   INDIClient* indi = INDIClient::TheClient();

   if ( sender == GUI->MountGotoStart_PushButton )
   {
	   double tg_ra=   (double) GUI->TargetRA_H_SpinBox.Value()
	   				    + (double) GUI->TargetRA_M_SpinBox.Value() / 60
	   			    	+ GUI->TargetRA_S_NumericEdit.Value() / 3600;
	   m_TargetRA = String(tg_ra);

	   double tg_dec  =   (double) GUI->TargetDEC_H_SpinBox.Value()
	   	   			    + (double) GUI->TargetDEC_M_SpinBox.Value() / 60
	   	   			   	+ GUI->TargetDEC_S_NumericEdit.Value() / 3600;

	   if (GUI->MountTargetDECIsSouth_CheckBox.IsChecked())
		   tg_dec *=-1;

	   m_TargetDEC = String(tg_dec);

      INDINewPropertyItem newPropertyListItem;
      newPropertyListItem.Device = m_Device;
      newPropertyListItem.Property = "EQUATORIAL_EOD_COORD";
      newPropertyListItem.PropertyType = "INDI_NUMBER";

      newPropertyListItem.ElementValue.Add(ElementValuePair("RA",m_TargetRA));
      newPropertyListItem.ElementValue.Add(ElementValuePair("DEC",m_TargetDEC));

      // send (RA,DEC) coordinates in a bulk request
      indi->SendNewPropertyItem( newPropertyListItem, true/*async*/ );
   }
   else if ( sender == GUI->MountSearch_PushButton )
   {
      CoordSearchDialog coordSearchDialog;
      if ( coordSearchDialog.Execute() )
      {
         double target_ra = coordSearchDialog.getTargetRaCoord();
         double target_dec = coordSearchDialog.getTargetDECCoord();
         CoordUtils::HMS coord_RA_HMS = CoordUtils::convertToHMS( target_ra );
         CoordUtils::HMS coord_DEC_HMS = CoordUtils::convertToHMS( target_dec );
         GUI->TargetRA_H_SpinBox.SetValue(coord_RA_HMS.hour);
         GUI->TargetRA_M_SpinBox.SetValue(coord_RA_HMS.minute);
         GUI->TargetRA_S_NumericEdit.SetValue(coord_RA_HMS.second);
         GUI->TargetDEC_H_SpinBox.SetValue(coord_DEC_HMS.hour);
         GUI->TargetDEC_M_SpinBox.SetValue(coord_DEC_HMS.minute);
         GUI->TargetDEC_S_NumericEdit.SetValue(coord_DEC_HMS.second);
         if (target_dec <0)
        	 GUI->MountTargetDECIsSouth_CheckBox.Check();

         m_TargetRA = String( target_ra );
         m_TargetDEC = String( target_dec );
      }
   }
   else if ( sender == GUI->MountSynch_PushButton )
   {


      INDINewPropertyItem newPropertyListItem;
      newPropertyListItem.Device = m_Device;
      newPropertyListItem.Property = "ON_COORD_SET";
      newPropertyListItem.PropertyType = "INDI_SWITCH";
      newPropertyListItem.ElementValue.Add(ElementValuePair("SYNC","ON"));

      indi->SendNewPropertyItem( newPropertyListItem, true/*async*/ );
      newPropertyListItem.ElementValue.Clear();

      newPropertyListItem.Property = "EQUATORIAL_EOD_COORD";
      newPropertyListItem.PropertyType = "INDI_NUMBER";
      newPropertyListItem.ElementValue.Add(ElementValuePair("RA",m_TargetRA));
      newPropertyListItem.ElementValue.Add(ElementValuePair("DEC",m_TargetDEC));


      // send (RA,DEC) coordinates in propertyVector
      indi->SendNewPropertyItem( newPropertyListItem, true/*async*/ );
      newPropertyListItem.ElementValue.Clear();

      newPropertyListItem.Property = "ON_COORD_SET";
      newPropertyListItem.PropertyType = "INDI_SWITCH";
      newPropertyListItem.ElementValue.Add(ElementValuePair("TRACK","ON"));
      indi->SendNewPropertyItem( newPropertyListItem, true/*async*/ );
   }
}


// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIMountInterface.cpp - Released 2016/04/28 15:13:36 UTC
