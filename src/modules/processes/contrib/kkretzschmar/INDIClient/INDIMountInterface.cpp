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
#include "INDIMountParameters.h"

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
   m_execution( nullptr ),
   m_downloadedFile( "2" )
{
   TheINDIMountInterface = this;
}

INDIMountInterface::~INDIMountInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
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
   INDIMountInstance* instance = new INDIMountInstance( TheINDIMountProcess );
   instance->p_deviceName = m_Device;
   instance->p_slewRate = GUI->MountMoveSpeed_ComboBox.CurrentItem();
   instance->p_lst = CoordUtils::convertFromHMS(GUI->LST_H_Edit.Value(), GUI->LST_M_Edit.Value(),GUI->LST_S_Edit.Value());
   instance->p_currentRA = CoordUtils::convertFromHMS(GUI->RA_H_Edit.Value(), GUI->RA_M_Edit.Value(),GUI->RA_S_Edit.Value());
   instance->p_currentDEC = CoordUtils::convertFromHMS(GUI->DEC_D_Edit.Value(), GUI->DEC_M_Edit.Value(),GUI->DEC_S_Edit.Value());
   instance->p_targetRA = CoordUtils::convertFromHMS(GUI->TargetRA_H_SpinBox.Value(), GUI->TargetRA_M_SpinBox.Value(),GUI->TargetRA_S_NumericEdit.Value());
   instance->p_targetDEC = CoordUtils::convertFromHMS(GUI->TargetDEC_H_SpinBox.Value(), GUI->TargetDEC_M_SpinBox.Value(),GUI->TargetDEC_S_NumericEdit.Value());
   if (GUI->MountTargetDECIsSouth_CheckBox.IsChecked())
	   instance->p_targetDEC *= -1.0;
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
	   CoordUtils::HMS hms;

	   hms = CoordUtils::convertToHMS( instance->p_lst );
	   GUI->LST_H_Edit.SetValue(hms.hour);
	   GUI->LST_M_Edit.SetValue(hms.minute);
	   GUI->LST_S_Edit.SetValue(hms.second);

	   hms = CoordUtils::convertToHMS( instance->p_currentRA );
	   GUI->RA_H_Edit.SetValue(hms.hour);
	   GUI->RA_M_Edit.SetValue(hms.minute);
	   GUI->RA_S_Edit.SetValue(hms.second);

	   hms = CoordUtils::convertToHMS( instance->p_currentDEC );
	   GUI->DEC_D_Edit.SetValue(hms.hour);
	   GUI->DEC_M_Edit.SetValue(hms.minute);
	   GUI->DEC_S_Edit.SetValue(hms.second);

	   hms = CoordUtils::convertToHMS( instance->p_targetRA );
	   GUI->TargetRA_H_SpinBox.SetValue(hms.hour);
	   GUI->TargetRA_M_SpinBox.SetValue(hms.minute);
	   GUI->TargetRA_S_NumericEdit.SetValue(hms.second);

	   hms = CoordUtils::convertToHMS( instance->p_targetDEC );
	   GUI->TargetDEC_H_SpinBox.SetValue(fabs(hms.hour));
	   GUI->TargetDEC_M_SpinBox.SetValue(hms.minute);
	   GUI->TargetDEC_S_NumericEdit.SetValue(hms.second);
	   if (instance->p_targetDEC <0)
		   GUI->MountTargetDECIsSouth_CheckBox.Check();
	   else
		   GUI->MountTargetDECIsSouth_CheckBox.Uncheck();

	   UpdateControls(); // Remove
	   if ( instance->ValidateDevice( false/*throwErrors*/ ) )
	   {
		   m_Device = instance->p_deviceName;
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
		GUI->MountMove_Control.Disable();
		GUI->MountGoto_Control.Disable();
	}
	else
	{
		if ( !GUI->UpdateDeviceProperties_Timer.IsRunning() )
			GUI->UpdateDeviceProperties_Timer.Start();

		GUI->MountProperties_Control.Enable();
		GUI->MountMove_Control.Enable();
		GUI->MountGoto_Control.Enable();
	}
}

// ----------------------------------------------------------------------------

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
   DEC_D_Edit.SetRange(-90,90);
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
   AZ_D_Edit.SetRange(0,359);
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
   ALT_D_Edit.SetRange(-90,90);
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
   MountHZALT_Sizer.Add(ALT_S_Edit);
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

   // Mount Device Move Section
   MountMove_SectionBar.SetTitle("Slew control:");
   MountMove_SectionBar.SetSection(MountMove_Control);


   MountMoveSpeed_Label.SetText("Slew speed:");
   MountMoveSpeed_Label.SetFixedWidth(labelWidth1);
   MountMoveSpeed_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   const char* slewSpeedTooltipText =
		   "<p>Predefined slew rates (ascending order): Guide, Centering, Find, Maximum</p>"
		   "<p> There might be more device specific slew rates, which can be changed with the Device Controller</p>";
   MountMoveSpeed_ComboBox.AddItem("Guide");
   MountMoveSpeed_ComboBox.AddItem("Centering");
   MountMoveSpeed_ComboBox.AddItem("Find");
   MountMoveSpeed_ComboBox.AddItem("Max");
   MountMoveSpeed_ComboBox.SetToolTip(slewSpeedTooltipText);
   MountMoveSpeed_ComboBox.OnItemSelected( (ComboBox::item_event_handler)& INDIMountInterface::e_ItemSelected, w );

   MountMoveSpeed_Sizer.SetSpacing(4);
   MountMoveSpeed_Sizer.Add(MountMoveSpeed_Label);
   MountMoveSpeed_Sizer.Add(MountMoveSpeed_ComboBox,100);


   MountMove_Label.SetFixedWidth(labelWidth1);
   MountMove_Label.SetText("Slew direction:");
   MountMove_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   MountMoveLeft_PushButton.SetText("West");
   MountMoveLeft_PushButton.SetIcon(w.ScaledResource( ":/icons/arrow-left.png" ));
   MountMoveLeft_PushButton.SetStyleSheet( "QPushButton { text-align: left; }" );
   MountMoveLeft_PushButton.OnPress((Button::press_event_handler) &INDIMountInterface::e_Press, w);
   MountMoveLeft_PushButton.OnRelease((Button::press_event_handler) &INDIMountInterface::e_Release, w);
   MountLeft_Sizer.SetSpacing(4);
   MountLeft_Sizer.Add(MountMoveLeft_PushButton);

   MountMoveTop_PushButton.SetText("North");
   MountMoveTop_PushButton.SetIcon(w.ScaledResource( ":/icons/arrow-up.png" ));
   MountMoveTop_PushButton.SetStyleSheet( "QPushButton { text-align: left; }" );
   MountMoveTop_PushButton.OnPress((Button::press_event_handler) &INDIMountInterface::e_Press, w);
   MountMoveTop_PushButton.OnRelease((Button::press_event_handler) &INDIMountInterface::e_Release, w);
   MountMoveStop_PushButton.SetText("Stop");
   MountMoveStop_PushButton.SetIcon(w.ScaledResource( ":/icons/delete.png" ));
   MountMoveStop_PushButton.SetStyleSheet( "QPushButton { text-align: left; }" );
   MountMoveStop_PushButton.OnClick( (Button::click_event_handler) &INDIMountInterface::e_Click, w );
   MountMoveBottom_PushButton.SetText("South");
   MountMoveBottom_PushButton.SetIcon(w.ScaledResource( ":/icons/arrow-down.png" ));
   MountMoveBottom_PushButton.SetStyleSheet( "QPushButton { text-align: left; }" );
   MountMoveBottom_PushButton.OnPress((Button::press_event_handler) &INDIMountInterface::e_Press, w);
   MountMoveBottom_PushButton.OnRelease((Button::press_event_handler) &INDIMountInterface::e_Release, w);
   MountTopBottom_Sizer.SetSpacing(4);
   MountTopBottom_Sizer.Add(MountMoveTop_PushButton);
   MountTopBottom_Sizer.Add(MountMoveStop_PushButton);
   MountTopBottom_Sizer.Add(MountMoveBottom_PushButton);

   MountMoveRight_PushButton.SetText("East");
   MountMoveRight_PushButton.SetIcon(w.ScaledResource( ":/icons/arrow-right.png" ));
   MountMoveRight_PushButton.SetStyleSheet( "QPushButton { text-align: center; }" );
   MountMoveRight_PushButton.OnPress((Button::press_event_handler) &INDIMountInterface::e_Press, w);
   MountMoveRight_PushButton.OnRelease((Button::press_event_handler) &INDIMountInterface::e_Release, w);
   MountRight_Sizer.SetSpacing(4);
   MountRight_Sizer.Add(MountMoveRight_PushButton);

   MountMoveDirection_Sizer.SetSpacing(4);
   MountMoveDirection_Sizer.Add(MountMove_Label);
   MountMoveDirection_Sizer.Add(MountLeft_Sizer);
   MountMoveDirection_Sizer.Add(MountTopBottom_Sizer);
   MountMoveDirection_Sizer.Add(MountRight_Sizer);
   MountMoveDirection_Sizer.AddStretch();

   MountMove_Sizer.SetSpacing(4);
   MountMove_Sizer.Add(MountMoveSpeed_Sizer);
   MountMove_Sizer.Add(MountMoveDirection_Sizer);


   MountMove_Control.SetSizer(MountMove_Sizer);

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
   MountTargetDECIsSouth_CheckBox.SetText("South");
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


   MountParking_Label.SetText("Park telescope:");
   MountParking_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   MountParking_Label.SetFixedWidth(labelWidth1);
   MountParking_CheckBox.SetToolTip("<p>When checked goto execution will park the telescope.</p>");

   MountParking_Sizer.SetSpacing(4);
   MountParking_Sizer.Add(MountParking_Label);
   MountParking_Sizer.Add(MountParking_CheckBox);
   MountParking_Sizer.AddStretch();


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
   MountGotoCancel_PushButton.OnClick( (Button::click_event_handler) &INDIMountInterface::e_Click, w );
   MountGotoCancel_PushButton.Disable();

   MountGotoStart_Sizer.SetSpacing(4);
   MountGotoStart_Sizer.Add(MountGotoCommand_Label);
   MountGotoStart_Sizer.Add(MountGotoStart_PushButton);
   MountGotoStart_Sizer.Add(MountGotoCancel_PushButton);
   MountGotoStart_Sizer.Add(MountGotoInfo_Label,20);
   MountGotoStart_Sizer.AddStretch();

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
   MountGoto_Sizer.Add(MountParking_Sizer);
   MountGoto_Sizer.Add(MountGotoStart_Sizer);
   MountGoto_Sizer.Add(MountSynch_Sizer);

   MountGoto_Control.SetSizer(MountGoto_Sizer);

   // Global Sizer
   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( ServerParameters_SectionBar );
   Global_Sizer.Add( ServerParameters_Control );
   Global_Sizer.Add( MountMove_SectionBar );
   Global_Sizer.Add( MountMove_Control );
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
   if ( sender == GUI->UpdateDeviceList_Timer)
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
      }

      if ( indi->GetPropertyItem( m_Device, "EQUATORIAL_EOD_COORD", "RA", MountProp ) )
      {
    	 CoordUtils::HMS hms = CoordUtils::parse(MountProp.PropertyValue);
         GUI->RA_H_Edit.SetValue(hms.hour);
         GUI->RA_M_Edit.SetValue(hms.minute);
         GUI->RA_S_Edit.SetValue(hms.second);
      }

      if (indi->GetPropertyItem( m_Device, "EQUATORIAL_EOD_COORD", "DEC", MountProp ) )
      {
    	 CoordUtils::HMS hms = CoordUtils::parse(MountProp.PropertyValue);
         GUI->DEC_D_Edit.SetValue(hms.hour);
         GUI->DEC_M_Edit.SetValue(hms.minute);
         GUI->DEC_S_Edit.SetValue(hms.second);
      }


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

      {
    	  static const char* indiSewRates[] = { "SLEW_GUIDE", "SLEW_CENTERING", "SLEW_FIND", "SLEW_MAX" };
    	  for ( size_type i = 0; i < ItemsInArray( indiSewRates ); ++i )
    		  if ( indi->GetPropertyItem( m_Device, "TELESCOPE_SLEW_RATE", indiSewRates[i], MountProp ) )
    			  if ( MountProp.PropertyValue == "ON" )
    			  {
    				  GUI->MountMoveSpeed_Label.Enable();
    				  GUI->MountMoveSpeed_ComboBox.SetCurrentItem( i );
    				  GUI->MountMoveSpeed_ComboBox.Enable();
    				  break;
    			  }
      }

   }
}


class INDIMountInterfaceExecution : public AbstractINDIMountExecution
{
public:

	INDIMountInterfaceExecution( INDIMountInterface* iface ) :
		AbstractINDIMountExecution( *dynamic_cast<INDIMountInstance*>( iface->NewProcess() ) ),
      m_iface( iface ),
      m_instanceAuto( &m_instance ),
      m_abortRequested( false ),
	  m_commandType(IMCCommandType::Default)
   {
   }

   virtual void Abort()
   {
      m_abortRequested = true;
   }

private:

   INDIMountInterface*            m_iface;
   AutoPointer<INDIMountInstance> m_instanceAuto;
   bool                           m_abortRequested;

   pcl_enum                       m_commandType;

virtual void StartMountEvent(double targetRA, double currentRA, double targetDEC, double currentDEC, pcl_enum commandType)
{
	m_iface->m_execution = this;
	m_commandType = commandType;
	m_iface->GUI->TargetRA_H_SpinBox.Disable();
	m_iface->GUI->TargetRA_M_SpinBox.Disable();
	m_iface->GUI->TargetDEC_H_SpinBox.Disable();
	m_iface->GUI->TargetDEC_M_SpinBox.Disable();
	m_iface->GUI->TargetRA_S_NumericEdit.Disable();
	m_iface->GUI->TargetDEC_S_NumericEdit.Disable();
	m_iface->GUI->MountTargetDECIsSouth_CheckBox.Disable();
	m_iface->GUI->MountSynch_PushButton.Disable();
	m_iface->GUI->MountSearch_PushButton.Disable();
	m_iface->GUI->MountParking_CheckBox.Disable();
	m_iface->GUI->MountGotoStart_PushButton.Disable();
	m_iface->GUI->MountMoveLeft_PushButton.Disable();
	m_iface->GUI->MountMoveRight_PushButton.Disable();
	m_iface->GUI->MountMoveTop_PushButton.Disable();
	m_iface->GUI->MountMoveBottom_PushButton.Disable();
	m_iface->GUI->MountMoveStop_PushButton.Disable();
	m_iface->GUI->MountGotoCancel_PushButton.Enable();
	m_iface->GUI->MountGotoInfo_Label.Clear();
	m_iface->ProcessEvents();

}

virtual void MountEvent(double targetRA, double currentRA, double targetDEC, double currentDEC)
{
	if ( m_abortRequested )
		AbstractINDIMountExecution::Abort();

	switch (m_commandType){
	case IMCCommandType::Goto:
	{
		double deltaRA = fabs(targetRA - currentRA) * 60;
		double deltaDEC = fabs(targetDEC - currentDEC) * 60;
		m_iface->GUI->MountGotoInfo_Label.SetText(String().Format("Slewing: Delta (ra,dec) = (%.3lf,  %.3lf) (arc min)",deltaRA,deltaDEC));
	}
	break;
	case IMCCommandType::Park:
	{
		m_iface->GUI->MountGotoInfo_Label.SetText(String().Format("Parking telescope"));
	}
	break;
	case IMCCommandType::Synch:
	break;
	default:
	break;
	};

	m_iface->ProcessEvents();
}

virtual void EndMountEvent()
{
	m_iface->m_execution = nullptr;
	m_iface->GUI->TargetRA_H_SpinBox.Enable();
	m_iface->GUI->TargetRA_M_SpinBox.Enable();
	m_iface->GUI->TargetDEC_H_SpinBox.Enable();
	m_iface->GUI->TargetDEC_M_SpinBox.Enable();
	m_iface->GUI->TargetRA_S_NumericEdit.Enable();
	m_iface->GUI->TargetDEC_S_NumericEdit.Enable();
	m_iface->GUI->MountTargetDECIsSouth_CheckBox.Enable();
	m_iface->GUI->MountSynch_PushButton.Enable();
	m_iface->GUI->MountSearch_PushButton.Enable();
	m_iface->GUI->MountParking_CheckBox.Enable();
	m_iface->GUI->MountGotoStart_PushButton.Enable();
	m_iface->GUI->MountMoveLeft_PushButton.Enable();
	m_iface->GUI->MountMoveRight_PushButton.Enable();
	m_iface->GUI->MountMoveTop_PushButton.Enable();
	m_iface->GUI->MountMoveBottom_PushButton.Enable();
	m_iface->GUI->MountMoveStop_PushButton.Enable();
	m_iface->GUI->MountGotoCancel_PushButton.Disable();
	m_iface->GUI->MountGotoInfo_Label.Clear();
	m_iface->ProcessEvents();
}

virtual void AbortEvent()
{
	EndMountEvent();
}

};

void INDIMountInterface::e_Click(Button& sender, bool checked){

   if ( !INDIClient::HasClient() )
	  return;

   if ( sender == GUI->MountGotoStart_PushButton )
   {
	  INDIMountInterfaceExecution mountExecution(this);
	  if (GUI->MountParking_CheckBox.IsChecked())
		  mountExecution.setCommandType(IMCCommandType::Park);
	  else
		  mountExecution.setCommandType(IMCCommandType::Goto);
	  mountExecution.Perform();

   }
   else if (sender == GUI->MountGotoCancel_PushButton ){
	   if ( m_execution != nullptr )
		   m_execution->Abort();
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
         GUI->TargetDEC_H_SpinBox.SetValue(fabs(coord_DEC_HMS.hour));
         GUI->TargetDEC_M_SpinBox.SetValue(coord_DEC_HMS.minute);
         GUI->TargetDEC_S_NumericEdit.SetValue(coord_DEC_HMS.second);
         if (target_dec <0)
        	 GUI->MountTargetDECIsSouth_CheckBox.Check();
         else
        	 GUI->MountTargetDECIsSouth_CheckBox.Uncheck();

      }
   }
   else if ( sender == GUI->MountSynch_PushButton )
   {
	   INDIMountInterfaceExecution mountExecution(this);
	   mountExecution.setCommandType(IMCCommandType::Synch);
	   mountExecution.Perform();
   }
   else if ( sender == GUI->MountMoveStop_PushButton )
   {
	   INDIClient* indi = INDIClient::TheClient();
	   INDINewPropertyItem newItem;
	   newItem.Device = m_Device;
	   newItem.Property = "TELESCOPE_ABORT_MOTION";
	   newItem.PropertyType = "INDI_SWITCH";
	   newItem.ElementValue.Add(ElementValuePair("ABORT","ON"));

	   indi->SendNewPropertyItem( newItem, true /*async*/  );
   }
}

void INDIMountInterface::e_Press(Button& sender)
{
	if ( !INDIClient::HasClient() )
		return;

	if ( sender == GUI->MountMoveTop_PushButton )
	{
		INDIClient* indi = INDIClient::TheClient();
		INDINewPropertyItem newItem;
		newItem.Device = m_Device;
		newItem.Property = "TELESCOPE_MOTION_NS";
		newItem.PropertyType = "INDI_SWITCH";
		newItem.ElementValue.Add(ElementValuePair("MOTION_NORTH","ON"));

		indi->SendNewPropertyItem( newItem, true /*async*/  );
	}

	if ( sender == GUI->MountMoveBottom_PushButton )
	{
		INDIClient* indi = INDIClient::TheClient();
		INDINewPropertyItem newItem;
		newItem.Device = m_Device;
		newItem.Property = "TELESCOPE_MOTION_NS";
		newItem.PropertyType = "INDI_SWITCH";
		newItem.ElementValue.Add(ElementValuePair("MOTION_SOUTH","ON"));

		indi->SendNewPropertyItem( newItem, true /*async*/  );
	}

	if ( sender == GUI->MountMoveLeft_PushButton )
	{
		INDIClient* indi = INDIClient::TheClient();
		INDINewPropertyItem newItem;
		newItem.Device = m_Device;
		newItem.Property = "TELESCOPE_MOTION_WE";
		newItem.PropertyType = "INDI_SWITCH";
		newItem.ElementValue.Add(ElementValuePair("MOTION_WEST","ON"));

		indi->SendNewPropertyItem( newItem, true /*async*/  );
	}

	if ( sender == GUI->MountMoveRight_PushButton )
	{
		INDIClient* indi = INDIClient::TheClient();
		INDINewPropertyItem newItem;
		newItem.Device = m_Device;
		newItem.Property = "TELESCOPE_MOTION_WE";
		newItem.PropertyType = "INDI_SWITCH";
		newItem.ElementValue.Add(ElementValuePair("MOTION_EAST","ON"));

		indi->SendNewPropertyItem( newItem, true /*async*/  );
	}

}

void INDIMountInterface::e_Release(Button& sender)
{
	if ( !INDIClient::HasClient() )
		return;

	if ( sender == GUI->MountMoveTop_PushButton )
	{
		INDIClient* indi = INDIClient::TheClient();
		INDINewPropertyItem newItem;
		newItem.Device = m_Device;
		newItem.Property = "TELESCOPE_MOTION_NS";
		newItem.PropertyType = "INDI_SWITCH";
		newItem.ElementValue.Add(ElementValuePair("MOTION_NORTH","OFF"));

		indi->SendNewPropertyItem( newItem, true /*async*/  );
	}

	if ( sender == GUI->MountMoveBottom_PushButton )
	{
		INDIClient* indi = INDIClient::TheClient();
		INDINewPropertyItem newItem;
		newItem.Device = m_Device;
		newItem.Property = "TELESCOPE_MOTION_NS";
		newItem.PropertyType = "INDI_SWITCH";
		newItem.ElementValue.Add(ElementValuePair("MOTION_SOUTH","OFF"));

		indi->SendNewPropertyItem( newItem, true /*async*/  );
	}

	if ( sender == GUI->MountMoveLeft_PushButton )
	{
		INDIClient* indi = INDIClient::TheClient();
		INDINewPropertyItem newItem;
		newItem.Device = m_Device;
		newItem.Property = "TELESCOPE_MOTION_WE";
		newItem.PropertyType = "INDI_SWITCH";
		newItem.ElementValue.Add(ElementValuePair("MOTION_WEST","OFF"));

		indi->SendNewPropertyItem( newItem, true /*async*/  );
	}

	if ( sender == GUI->MountMoveRight_PushButton )
	{
		INDIClient* indi = INDIClient::TheClient();
		INDINewPropertyItem newItem;
		newItem.Device = m_Device;
		newItem.Property = "TELESCOPE_MOTION_WE";
		newItem.PropertyType = "INDI_SWITCH";
		newItem.ElementValue.Add(ElementValuePair("MOTION_EAST","OFF"));

		indi->SendNewPropertyItem( newItem, true /*async*/  );
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

      // get initial roperties and unpark mount
      if ( !m_Device.IsEmpty() )
      {
    	  INDIClient* indi = INDIClient::TheClient();
    	  INDIPropertyListItem MountProp;

    	  // get initial target coordinates
    	  if ( indi->GetPropertyItem( m_Device, "TARGET_EOD_COORD", "RA", MountProp ) )
    	  {
    		  CoordUtils::HMS hms = CoordUtils::parse(MountProp.PropertyValue);
    		  GUI->TargetRA_H_SpinBox.SetValue(hms.hour);
    		  GUI->TargetRA_M_SpinBox.SetValue(hms.minute);
    		  GUI->TargetRA_S_NumericEdit.SetValue(hms.second);
    	  }

    	  if (indi->GetPropertyItem( m_Device, "TARGET_EOD_COORD", "DEC", MountProp ) )
    	  {
    		  CoordUtils::HMS hms = CoordUtils::parse(MountProp.PropertyValue);
    		  GUI->TargetDEC_H_SpinBox.SetValue(fabs(hms.hour));
    		  GUI->TargetDEC_M_SpinBox.SetValue(hms.minute);
    		  GUI->TargetDEC_S_NumericEdit.SetValue(hms.second);
    		  if (hms.hour<0)
    			  GUI->MountTargetDECIsSouth_CheckBox.Check();
    		  else
    			  GUI->MountTargetDECIsSouth_CheckBox.Uncheck();
    	  }

    	  // unpark mount
    	  INDIMountInterfaceExecution mountExecution(this);
    	  mountExecution.setCommandType(IMCCommandType::Unpark);
    	  mountExecution.Perform();
      }
   }
   else if (sender == GUI->MountMoveSpeed_ComboBox)
   {
	   INDIClient* indi = INDIClient::TheClient();
	   String PropertyElement = INDIMountInstance::MountSlewRatePropertyString( itemIndex );
	   if ( indi->HasPropertyItem( m_Device, "TELESCOPE_SLEW_RATE", PropertyElement ) )
	   {
		   INDINewPropertyItem newItem;
		   newItem.Device = m_Device;
		   newItem.Property = "TELESCOPE_SLEW_RATE";
		   newItem.PropertyType = "INDI_SWITCH";
		   newItem.ElementValue.Add(ElementValuePair(PropertyElement,"ON"));
		   indi->SendNewPropertyItem( newItem, true/*async*/ );
	   }

   }
}


// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIMountInterface.cpp - Released 2016/04/28 15:13:36 UTC
