//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.15.0203
// ----------------------------------------------------------------------------
// INDIMountInterface.cpp - Released 2017-05-02T09:43:01Z
// ----------------------------------------------------------------------------
// This file is part of the standard INDIClient PixInsight module.
//
// Copyright (c) 2014-2017 Klaus Kretzschmar
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
#include <pcl/MetaModule.h>
#include <pcl/FileDialog.h>
#include <pcl/ExternalProcess.h>


#include "ApparentPosition.h"
#include "Alignment.h"
#include "INDIDeviceControllerInterface.h"
#include "INDIMountInterface.h"
#include "INDIMountParameters.h"
#include "INDIMountProcess.h"

#include "INDI/basedevice.h"
#include "INDI/indiapi.h"
#include "INDI/indibase.h"

#include <time.h>

namespace pcl
{

// ----------------------------------------------------------------------------

// km/s -> AU/day
static const double KMS2AUY = 365.25*86400/149597870e3;

// ----------------------------------------------------------------------------

INDIMountInterface* TheINDIMountInterface = nullptr;

// ----------------------------------------------------------------------------

//#include "PixInsightINDIIcon.xpm"

// ----------------------------------------------------------------------------

CoordinateSearchDialog::CoordinateSearchDialog() :
   m_RA( 0 ),
   m_Dec( 0 ),
   m_muRA( 0 ),
   m_muDec( 0 ),
   m_parallax( 0 ),
   m_valid( false ),
   m_goto( false ),
   m_downloading( false ),
   m_abort( false ),
   m_firstTimeShown( true )
{
   const char* objectNameToolTip =
      "<p>Name or identifier of the object to search for. Examples: M31, Pleiades, NGC 253, Orion Nebula, Antares.</p>";

   ObjectName_Label.SetText( "Object: " );
   ObjectName_Label.SetToolTip( objectNameToolTip );
   ObjectName_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   ObjectName_Edit.SetToolTip( objectNameToolTip );
   ObjectName_Edit.OnGetFocus( (Control::event_handler)&CoordinateSearchDialog::e_GetFocus, *this );
   ObjectName_Edit.OnLoseFocus( (Control::event_handler)&CoordinateSearchDialog::e_LoseFocus, *this );

   Search_Button.SetText( "Search" );
   Search_Button.SetIcon( ScaledResource( ":/icons/find.png" ) );
   Search_Button.SetToolTip( "<p>Perform online coordinate search.</p>" );
   Search_Button.OnClick( (Button::click_event_handler)&CoordinateSearchDialog::e_Click, *this );

   Search_Sizer.SetSpacing( 4 );
   Search_Sizer.Add( ObjectName_Label );
   Search_Sizer.Add( ObjectName_Edit, 100 );
   Search_Sizer.Add( Search_Button );

   SearchInfo_TextBox.SetReadOnly();
   SearchInfo_TextBox.SetStyleSheet( ScaledStyleSheet(
         "QTextEdit {"
            "font-family: DejaVu Sans Mono, Monospace;"
            "font-size: 8pt;"
            "background: #141414;" // borrowed from /rsc/qss/core-standard.qss
            "color: #E8E8E8;"
         "}"
      ) );
   SearchInfo_TextBox.Restyle();
   SearchInfo_TextBox.SetMinSize( SearchInfo_TextBox.Font().Width( 'm' )*81, SearchInfo_TextBox.Font().Height()*22 );

   Get_Button.SetText( "Get" );
   Get_Button.SetIcon( ScaledResource( ":/icons/window-import.png" ) );
   Get_Button.SetToolTip( "<p>Acquire apparent coordinates.</p>" );
   Get_Button.OnClick( (Button::click_event_handler)&CoordinateSearchDialog::e_Click, *this );
   Get_Button.Disable();

   GoTo_Button.SetText( "GoTo" );
   GoTo_Button.SetIcon( ScaledResource( ":/icons/play.png" ) );
   GoTo_Button.SetToolTip( "<p>Slew to apparent coordinates.</p>" );
   GoTo_Button.OnClick( (Button::click_event_handler)&CoordinateSearchDialog::e_Click, *this );
   GoTo_Button.Disable();

   Cancel_Button.SetText( "Cancel" );
   Cancel_Button.SetIcon( ScaledResource( ":/icons/cancel.png" ) );
   Cancel_Button.OnClick( (Button::click_event_handler)&CoordinateSearchDialog::e_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( Get_Button );
   Buttons_Sizer.Add( GoTo_Button );
   Buttons_Sizer.Add( Cancel_Button );

   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.SetMargin( 8 );
   Global_Sizer.Add( Search_Sizer );
   Global_Sizer.Add( SearchInfo_TextBox, 100 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   SetWindowTitle( "Online Coordinate Search" );

   OnShow( (Control::event_handler)&CoordinateSearchDialog::e_Show, *this );
}

void CoordinateSearchDialog::e_Show( Control& )
{
   if ( m_firstTimeShown )
   {
      m_firstTimeShown = false;
      AdjustToContents();
      SetMinSize();
   }
}

void CoordinateSearchDialog::e_GetFocus( Control& sender )
{
   if ( sender == ObjectName_Edit )
      Search_Button.SetDefault();
}

void CoordinateSearchDialog::e_LoseFocus( Control& sender )
{
   if ( sender == ObjectName_Edit )
      Get_Button.SetDefault();
}

bool CoordinateSearchDialog::e_Download( NetworkTransfer& sender, const void* buffer, fsize_type size )
{
   if ( m_abort )
      return false;

   m_downloadData.Append( static_cast<const char*>( buffer ), size );
   return true;
}

bool CoordinateSearchDialog::e_Progress( NetworkTransfer& sender,
                                         fsize_type downloadTotal, fsize_type downloadCurrent,
                                         fsize_type uploadTotal, fsize_type uploadCurrent )
{
   if ( m_abort )
      return false;

   if ( downloadTotal > 0 )
      SearchInfo_TextBox.Insert( String().Format( "<end><clrbol>%u of %u bytes transferred (%d%%)<flush>",
                                                  downloadCurrent, downloadTotal,
                                                  RoundInt( 100.0*downloadCurrent/downloadTotal ) ) );
   else
      SearchInfo_TextBox.Insert( String().Format( "<end><clrbol>%u bytes transferred (unknown size)<flush>",
                                                  downloadCurrent ) );
   SearchInfo_TextBox.Focus();
   Module->ProcessEvents();
   return true;
}

void CoordinateSearchDialog::e_Click( Button& sender, bool checked )
{
   if ( sender == Search_Button )
   {
      String objectName = ObjectName_Edit.Text().Trimmed();
      ObjectName_Edit.SetText( objectName );
      if ( objectName.IsEmpty() )
      {
         SearchInfo_TextBox.SetText( "\x1b[31m*** Error: No object has been specified.\x1b[39m<br>" );
         ObjectName_Edit.Focus();
         return;
      }

      m_valid = false;
      Get_Button.Disable();
      GoTo_Button.Disable();

      //String url( "http://vizier.cfa.harvard.edu/viz-bin/nph-sesame/-oI/A?" );
      //url << objectName;
      String url( "http://simbad.u-strasbg.fr/simbad/sim-tap/sync?request=doQuery&lang=adql&format=TSV&query=" );
      String select_stmt = "SELECT oid, ra, dec, pmra, pmdec, plx_value, rvz_radvel, main_id, otype_txt, sp_type, flux "
                           "FROM basic "
                           "JOIN ident ON ident.oidref = oid "
                           "LEFT OUTER JOIN flux ON flux.oidref = oid AND flux.filter = 'V' "
                           "WHERE id = '" + objectName + "';";
      url << select_stmt;

      NetworkTransfer transfer;
      transfer.SetURL( url );
      transfer.OnDownloadDataAvailable( (NetworkTransfer::download_event_handler)&CoordinateSearchDialog::e_Download, *this );
      transfer.OnTransferProgress( (NetworkTransfer::progress_event_handler)&CoordinateSearchDialog::e_Progress, *this );

      SearchInfo_TextBox.SetText( "<wrap><raw>" + url + "</raw><br><br><flush>" );
      Module->ProcessEvents();

      m_downloadData.Clear();
      m_downloading = true;
      m_abort = false;
      bool ok = transfer.Download();
      m_downloading = false;

      if ( ok )
      {
         SearchInfo_TextBox.Insert( String().Format( "<end><clrbol>%d bytes downloaded @ %.3g KiB/s<br>",
                                                     transfer.BytesTransferred(), transfer.TotalSpeed() ) );
         //SearchInfo_TextBox.Insert( "<end><cbr><br><raw>" + m_downloadData + "</raw>" );

         StringList lines;
         m_downloadData.Break( lines, '\n' );
         if ( lines.Length() >= 2 )
         {
            // The first line has column titles. The second line has values.
            StringList tokens;
            lines[1].Break( tokens, '\t', true/*trim*/ );
            if ( tokens.Length() == 11 )
            {
               m_RA = tokens[1].ToDouble();                                         // degrees
               m_Dec = tokens[2].ToDouble();                                        // degrees
               m_muRA = tokens[3].IsEmpty() ? 0.0 : tokens[3].ToDouble();           // mas/yr
               m_muDec = tokens[4].IsEmpty() ? 0.0 : tokens[4].ToDouble();          // mas/yr
               m_parallax = tokens[5].IsEmpty() ? 0.0 : tokens[5].ToDouble();       // mas
               m_radVel = tokens[6].IsEmpty() ? 0.0 : tokens[6].ToDouble()*KMS2AUY; // km/s -> AU/year
               m_objectName = tokens[7].Unquoted().Trimmed();
               m_objectType = tokens[8].Unquoted().Trimmed();
               m_spectralType = tokens[9].Unquoted().Trimmed();
               m_vmag = tokens[10].IsEmpty() ? 101.0 : tokens[10].ToDouble();

               try
               {
                  String info =
                           "<end><cbr><br><b>Object            :</b> "
                        + m_objectName
                        +            "<br><b>Object type       :</b> "
                        + m_objectType
                        +            "<br><b>Right Ascension   :</b> "
                        + String::ToSexagesimal( m_RA/15,
                                       SexagesimalConversionOptions( 3/*items*/, 3/*precision*/, false/*sign*/, 3/*width*/ ) )
                        +            "<br><b>Declination       :</b> "
                        + String::ToSexagesimal( m_Dec,
                                       SexagesimalConversionOptions( 3/*items*/, 2/*precision*/, true/*sign*/, 3/*width*/ ) );
                  if ( m_muRA != 0 )
                     info
                        +=           "<br><b>Proper motion RA  :</b> "
                        + String().Format( "%+8.2lf mas/year", m_muRA );
                  if ( m_muDec != 0 )
                     info
                        +=           "<br><b>Proper motion Dec :</b> "
                        + String().Format( "%+8.2lf mas/year", m_muDec );
                  if ( m_parallax != 0 )
                     info
                        +=           "<br><b>Parallax          :</b> "
                        + String().Format( "%8.2lf mas", m_parallax );
                  if ( m_radVel != 0 )
                     info
                        +=           "<br><b>Radial velocity   :</b> "
                        + String().Format( "%+.4lg AU/year", m_radVel );
                  if ( !m_spectralType.IsEmpty() )
                     info
                        +=           "<br><b>Spectral type     :</b> "
                        + m_spectralType;
                  if ( m_vmag < 100 )
                     info
                        +=           "<br><b>V Magnitude       :</b> "
                        + String().Format( "%.4lg", m_vmag );
                  info += "<br>";

                  SearchInfo_TextBox.Insert( info );
                  m_valid = true;
                  Get_Button.Enable();
                  GoTo_Button.Enable();
               }
               catch ( ... )
               {
               }
            }
         }

         if ( !m_valid )
            SearchInfo_TextBox.Insert( "<end><cbr><br>\x1b[31m*** Error: Unable to acquire valid coordinate information.\x1b[39m<br>" );
      }
      else
      {
         if ( m_abort )
            SearchInfo_TextBox.Insert( "<end><cbr><br>\x1b[31m<raw><* abort *></raw>\x1b[39m<br>" );
         else
            SearchInfo_TextBox.Insert( "<end><cbr><br>\x1b[31m*** Error: Download failed: <raw>" + transfer.ErrorInformation() + "</raw>\x1b[39m<br>" );
      }

      // ### FIXME: Workaround to force visibility of TextBox focus.
      SearchInfo_TextBox.Unfocus();
      Module->ProcessEvents();
      SearchInfo_TextBox.Focus();

      m_downloadData.Clear();
   }
   else if ( sender == Get_Button )
   {
      m_goto = false;
      Ok();
   }
   else if ( sender == GoTo_Button )
   {
      m_goto = true;
      Ok();
   }
   else if ( sender == Cancel_Button )
   {
      if ( m_downloading )
         m_abort = true;
      else
         Cancel();
   }
}

SyncDataListDialog::SyncDataListDialog(Array<SyncDataPoint>& syncDataArray) :
   Dialog(), m_syncDataList(syncDataArray), m_firstTimeShown(true)
{

	SnycData_TreeBox.SetMinHeight( Font().Width( 'm' )*60 );
	SnycData_TreeBox.SetScaledMinWidth(  Font().Height()  * 50 );
	SnycData_TreeBox.SetNumberOfColumns( 8 );
	SnycData_TreeBox.SetHeaderText( 2, "HA" );
	SnycData_TreeBox.SetHeaderText( 3, "Dec" );
	SnycData_TreeBox.SetHeaderText( 4, "Delta HA" );
	SnycData_TreeBox.SetHeaderText( 5, "Delta Dec" );
	SnycData_TreeBox.SetHeaderText( 6, "Pier side");
	SnycData_TreeBox.SetHeaderText( 7, "Created on");
	SnycData_TreeBox.EnableAlternateRowColor();
	SnycData_TreeBox.EnableMultipleSelections(true);


	for (size_t index = 0;  index < syncDataArray.Length(); ++index) {
		TreeBox::Node* node =  new TreeBox::Node();
		auto syncDataPoint = syncDataArray[index];

		node->SetText(0,String().Format("%i",index));
		node->SetIcon(1, syncDataPoint.enabled ? ScaledResource(":/browser/enabled.png") : ScaledResource(":/browser/disabled.png") );
		node->SetText(2,String().Format("%2.3f",AlignmentModel::rangeShiftHourAngle(syncDataPoint.localSiderialTime-syncDataPoint.celestialRA)));
		node->SetText(3,String().Format("%2.3f",syncDataPoint.celestialDEC));
		node->SetText(4,String().Format("%2.5f",syncDataPoint.telecopeRA-syncDataPoint.celestialRA));
		node->SetText(5,String().Format("%2.5f",syncDataPoint.celestialDEC-syncDataPoint.telecopeDEC));
		node->SetText(6,String(syncDataPoint.pierSide == IMCPierSide::West ? "West" :  syncDataPoint.pierSide == IMCPierSide::East ? "East" : "" ));
		node->SetText(7,syncDataPoint.creationTime.ToString());
		SnycData_TreeBox.Add(node);
	}

	for (int i = 0 ; i < SnycData_TreeBox.NumberOfColumns(); ++i){
		SnycData_TreeBox.AdjustColumnWidthToContents( i );
	}

	SyncDataList_Sizer.SetSpacing( 8 );
	SyncDataList_Sizer.Add(SnycData_TreeBox, 100);

	Delete_Button.SetText( "Delete" );
	//Delete_Button.SetIcon( ScaledResource( ":/icons/window-import.png" ) );
	Delete_Button.OnClick( (Button::click_event_handler)&SyncDataListDialog::e_Click, *this );

	Enable_Button.SetText( "Enable" );
	//Delete_Button.SetIcon( ScaledResource( ":/icons/window-import.png" ) );
	Enable_Button.OnClick( (Button::click_event_handler)&SyncDataListDialog::e_Click, *this );

	Disable_Button.SetText( "Disable" );
	//Delete_Button.SetIcon( ScaledResource( ":/icons/window-import.png" ) );
	Disable_Button.OnClick( (Button::click_event_handler)&SyncDataListDialog::e_Click, *this );

	Ok_Button.SetText( "Ok" );
	Ok_Button.SetIcon( ScaledResource( ":/icons/ok.png" ) );
	Ok_Button.OnClick( (Button::click_event_handler)&SyncDataListDialog::e_Click, *this );

	Cancel_Button.SetText( "Cancel" );
	Cancel_Button.SetIcon( ScaledResource( ":/icons/cancel.png" ) );
	Cancel_Button.OnClick( (Button::click_event_handler)&SyncDataListDialog::e_Click, *this );

	SyncDataListButton_Sizer.SetSpacing(8);
	SyncDataListButton_Sizer.Add(Enable_Button);
	SyncDataListButton_Sizer.Add(Disable_Button);
	SyncDataListButton_Sizer.Add(Delete_Button);
	SyncDataListButton_Sizer.AddStretch();
	SyncDataListButton_Sizer.Add(Ok_Button);
	SyncDataListButton_Sizer.Add(Cancel_Button);

	Global_Sizer.SetSpacing( 8 );
	Global_Sizer.SetMargin( 8 );
	Global_Sizer.Add(SyncDataList_Sizer);
	Global_Sizer.Add(SyncDataListButton_Sizer);
	SetSizer( Global_Sizer );

	SetWindowTitle( "List of Sync Data Points" );

	OnShow( (Control::event_handler)&SyncDataListDialog::e_Show, *this );
	OnClose( (Control::close_event_handler)&SyncDataListDialog::e_Close, *this );

}

void SyncDataListDialog::e_Show( Control& )
{
   if ( m_firstTimeShown )
   {
      m_firstTimeShown = false;
      AdjustToContents();
      SetMinSize();
   }
}

void SyncDataListDialog::e_Close( Control& sender , bool& allowClose){

}

void SyncDataListDialog::e_Click( Button& sender, bool checked ){

	if (sender == Disable_Button){
		for ( auto node: SnycData_TreeBox.SelectedNodes()){
			int index = SnycData_TreeBox.ChildIndex(node);
			m_syncDataList[index].enabled = false;
			node->SetIcon(1, ScaledResource(":/browser/disabled.png") );
		}
	}
	if (sender == Enable_Button){
		for ( auto node: SnycData_TreeBox.SelectedNodes()){
			int index = SnycData_TreeBox.ChildIndex(node);
			m_syncDataList[index].enabled = true;
			node->SetIcon(1, ScaledResource(":/browser/enabled.png") );
		}
	}
	if (sender == Delete_Button){
		for ( auto node: SnycData_TreeBox.SelectedNodes()){
			int index = SnycData_TreeBox.ChildIndex(node);
			m_syncDataList.Remove(m_syncDataList[index]);
			SnycData_TreeBox.Remove(index);
		}
	}
	if (sender == Ok_Button){
		Ok();
	}
	if (sender == Cancel_Button){
		Cancel();
	}

}

#define ADD_LABLED_CHECKBOXES_TO_SIZER(NAME) \
   NAME##_Sizer.Add(NAME##_Label);         \
   NAME##_Sizer.Add(NAME##_CheckBox);      \
   NAME##_Sizer.AddStretch();              \
   MountAlignmentConfig_Sizer.Add(NAME##_Sizer)


 AlignmentConfigDialog::AlignmentConfigDialog(INDIMountInterface& w):Dialog(), m_interface(w) {

	int labelWidth1 = w.Font().Width("Polar axis displacment :");

	MountAlignmentConfig_Sizer.SetSpacing( 8 );
	MountAlignmentConfig_Sizer.SetMargin( 8 );

	const char* pierSideToolTipText =
				"<p>Create a model for each side of the pier. This is useful for mounts"
			    "which perform a meridian flip when objects pass the meridian.</p>";
				"<p>Creating models for each pier side can dramatically improve the accuracy of pointing model,"
				"since some pointing error sources depend on the position and balance of the mounted devices.</p>"
				"<p>Note that two pointing model need twice as many sync points.</p>";


	ModelPierSide_Label.SetText("Model each pier side :");
	ModelPierSide_Label.SetToolTip(pierSideToolTipText);
	ModelPierSide_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
	ModelPierSide_Label.SetFixedWidth(labelWidth1);

	ModelPierSide_CheckBox.SetToolTip(pierSideToolTipText);
	ModelPierSide_CheckBox.Check();

	ADD_LABLED_CHECKBOXES_TO_SIZER(ModelPierSide);

	const char* offsetToolTipText =
			"<p>Zero-point offset in rightascension (RA) and declination (DEC) axes.</p>";

	Offset_Label.SetText("Offset:");
	Offset_Label.SetToolTip(offsetToolTipText);
	Offset_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
	Offset_Label.SetFixedWidth(labelWidth1);

	Offset_CheckBox.SetToolTip(offsetToolTipText);
	Offset_CheckBox.Check();

	ADD_LABLED_CHECKBOXES_TO_SIZER(Offset);

	const char* collimationToolTipText =
			"<p>Misalignment between optical and mechanical axes</p>";

	Collimation_Label.SetText("Collimation:");
	Collimation_Label.SetToolTip(collimationToolTipText);
	Collimation_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
	Collimation_Label.SetFixedWidth(labelWidth1);

	Collimation_CheckBox.SetToolTip(collimationToolTipText);
	Collimation_CheckBox.Check();

	ADD_LABLED_CHECKBOXES_TO_SIZER(Collimation);

	const char* nonPerpendicularToolTipText =
				"<p>Polar/declination axis non-orthogonality error.</p>";

	NonPerpendicular_Label.SetText("Perpendicularity:");
	NonPerpendicular_Label.SetToolTip(nonPerpendicularToolTipText);
	NonPerpendicular_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
	NonPerpendicular_Label.SetFixedWidth(labelWidth1);

	NonPerpendicular_CheckBox.SetToolTip(nonPerpendicularToolTipText);
	NonPerpendicular_CheckBox.Check();

	ADD_LABLED_CHECKBOXES_TO_SIZER(NonPerpendicular);

	const char* polarAxisDisplacementToolTipText =
					"<p>Polar axis displacement between instrumental and true poles.</p>";

	PolarAxisDisplacement_Label.SetText("Polar axis displacment:");
	PolarAxisDisplacement_Label.SetToolTip(polarAxisDisplacementToolTipText);
	PolarAxisDisplacement_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
	PolarAxisDisplacement_Label.SetFixedWidth(labelWidth1);

	PolarAxisDisplacement_CheckBox.SetToolTip(polarAxisDisplacementToolTipText);
	PolarAxisDisplacement_CheckBox.Check();

	ADD_LABLED_CHECKBOXES_TO_SIZER(PolarAxisDisplacement);

	const char* tubeFlexureToolTipText =
					"<p>Tube flexure away from zenith.</p>";

	TubeFlexure_Label.SetText("Tube flexure:");
	TubeFlexure_Label.SetToolTip(tubeFlexureToolTipText);
	TubeFlexure_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
	TubeFlexure_Label.SetFixedWidth(labelWidth1);

	TubeFlexure_CheckBox.SetToolTip(tubeFlexureToolTipText);
	TubeFlexure_CheckBox.Check();

	ADD_LABLED_CHECKBOXES_TO_SIZER(TubeFlexure);

	const char* forkFlexureToolTipText =
					"<p>Fork flexure away from zenith (only for non German Equatorial Mounts).</p>";

	ForkFlexure_Label.SetText("Fork flexure:");
	ForkFlexure_Label.SetToolTip(forkFlexureToolTipText);
	ForkFlexure_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
	ForkFlexure_Label.SetFixedWidth(labelWidth1);

	ForkFlexure_CheckBox.SetToolTip(forkFlexureToolTipText);

	ADD_LABLED_CHECKBOXES_TO_SIZER(ForkFlexure);

	const char* deltaAxisToolTipText =
					"<p>Bending of declination axle (only for German Equatorial Mounts).</p>";

	DeltaAxisFlexure_Label.SetText("Dec axis bending:");
	DeltaAxisFlexure_Label.SetToolTip(deltaAxisToolTipText);
	DeltaAxisFlexure_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
	DeltaAxisFlexure_Label.SetFixedWidth(labelWidth1);

	DeltaAxisFlexure_CheckBox.SetToolTip(deltaAxisToolTipText);
	DeltaAxisFlexure_CheckBox.Check();

	ADD_LABLED_CHECKBOXES_TO_SIZER(DeltaAxisFlexure);

	const char* linearTermToolTipText =
					"<p>Linear scale error in hour angle.</p>";

	Linear_Label.SetText("Linear:");
	Linear_Label.SetToolTip(linearTermToolTipText);
	Linear_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
	Linear_Label.SetFixedWidth(labelWidth1);

	Linear_CheckBox.SetToolTip(linearTermToolTipText);
	Linear_CheckBox.Check();

	ADD_LABLED_CHECKBOXES_TO_SIZER(Linear);

	const char* quadraticTermToolTipText =
						"<p>Quadratic scale error in hour angle.</p>";

	Quadratic_Label.SetText("Quadratic:");
	Quadratic_Label.SetToolTip(quadraticTermToolTipText);
	Quadratic_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
	Quadratic_Label.SetFixedWidth(labelWidth1);

	Quadratic_CheckBox.SetToolTip(quadraticTermToolTipText);
	Quadratic_CheckBox.Check();

	ADD_LABLED_CHECKBOXES_TO_SIZER(Quadratic);

	AnalyticalAlignment_ConfigControl.SetSizer(MountAlignmentConfig_Sizer);

	AlignmentConfig_TabBox.AddPage( AnalyticalAlignment_ConfigControl, "Analytical Pointing Model" );
	AlignmentConfig_TabBox.OnPageSelected((TabBox::page_event_handler)&AlignmentConfigDialog::e_PageSelected, *this );
	//AlignmentConfig_TabBox.AddPage( SurfaceSplineAlignment_ConfigControl, "Surface Splines Pointing Model" );

	Ok_Button.SetText( "Ok" );
	Ok_Button.SetIcon( ScaledResource( ":/icons/ok.png" ) );
	Ok_Button.OnClick( (Button::click_event_handler)&AlignmentConfigDialog::e_Click, *this );

	Cancel_Button.SetText( "Cancel" );
	Cancel_Button.SetIcon( ScaledResource( ":/icons/cancel.png" ) );
	Cancel_Button.OnClick( (Button::click_event_handler)&AlignmentConfigDialog::e_Click, *this );

	AlignmentConfigButton_Sizer.SetSpacing(8);
	AlignmentConfigButton_Sizer.Add(Ok_Button);
	AlignmentConfigButton_Sizer.Add(Cancel_Button);
	AlignmentConfigButton_Sizer.AddStretch();

	Global_Sizer.SetSpacing( 8 );
	Global_Sizer.SetMargin( 8 );
	Global_Sizer.Add(AlignmentConfig_TabBox);
	Global_Sizer.Add(AlignmentConfigButton_Sizer);

	SetSizer( Global_Sizer );

	SetWindowTitle( "Telescope Pointing Models" );

	OnShow( (Control::event_handler)&AlignmentConfigDialog::e_Show, *this );
}

 void AlignmentConfigDialog::e_Show( Control& )
 {
    if ( m_firstTimeShown )
    {
       m_firstTimeShown = false;
       AdjustToContents();
       SetMinSize();
    }
 }

 void AlignmentConfigDialog::e_Click( Button& sender, bool checked ){

 	if (sender == Ok_Button){
 		m_interface.GUI->m_modelBothPierSides = ModelPierSide_CheckBox.IsChecked();
 		m_interface.GUI->m_alignmentConfigOffset = Offset_CheckBox.IsChecked();
 		m_interface.GUI->m_alignmentConfigCollimation = Collimation_CheckBox.IsChecked();
 		m_interface.GUI->m_alignmentConfigNonPerpendicular = NonPerpendicular_CheckBox.IsChecked();
 		m_interface.GUI->m_alignmentConfigPolarAxisDisp = PolarAxisDisplacement_CheckBox.IsChecked();
 		m_interface.GUI->m_alignmentConfigDecAxisFlexure = DeltaAxisFlexure_CheckBox.IsChecked();
 		m_interface.GUI->m_alignmentConfigTubeFlexure = TubeFlexure_CheckBox.IsChecked();
 		m_interface.GUI->m_alignmentConfigForkFlexure = ForkFlexure_CheckBox.IsChecked();
 		m_interface.GUI->m_alignmentLinear = Linear_CheckBox.IsChecked();
 		m_interface.GUI->m_alignmentQuadratic = Quadratic_CheckBox.IsChecked();
 		m_interface.GUI->m_aignmentModelIndex = AlignmentConfig_TabBox.CurrentPageIndex() + 1;
 		Ok();
 	}
 	if (sender == Cancel_Button){
 		Cancel();
 	}
 }

 void AlignmentConfigDialog::e_PageSelected(TabBox& sender, int tabIndex) {

	 if (sender == AlignmentConfig_TabBox) {

	 }

 }

 MountConfigDialog::MountConfigDialog(const String& deviceName, double geoLat, double geoLong, double telescopeAperture, double telescopeFocalLength):ConfigDialogBase(deviceName),m_device(deviceName){

	 int emWidth = Font().Width( 'm' );
	 int editWidth1 = RoundInt( 4.25*emWidth );
	 int editWidth2 = RoundInt( 5.25*emWidth );

	 int sign,s1,s2;
	 double s3;

	 int labelWidth1 = Font().Width("Geographic Longitude:");

	 Latitude_Label.SetText( "Geographic Latitude:" );
	 Latitude_Label.SetToolTip( "<p>Position of observatory: Geographic latitude (g:m:s)</p>" );
	 Latitude_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
	 Latitude_Label.SetFixedWidth( labelWidth1 );

	 DecimalToSexagesimal(sign,s1,s2,s3,geoLat);
	 Latitude_H_SpinBox.SetRange( 0, 90 );
	 Latitude_H_SpinBox.SetFixedWidth( editWidth1 );
	 Latitude_H_SpinBox.SetValue(Abs(s1));

	 Latitude_M_SpinBox.SetRange( 0, 59 );
	 Latitude_M_SpinBox.SetFixedWidth( editWidth1 );
	 Latitude_M_SpinBox.SetValue(s2);

	 Latitude_S_NumericEdit.SetReal();
	 Latitude_S_NumericEdit.SetPrecision( 3 );
	 Latitude_S_NumericEdit.EnableFixedPrecision();
	 Latitude_S_NumericEdit.SetRange( 0, 59.999 );
	 Latitude_S_NumericEdit.label.Hide();
	 Latitude_S_NumericEdit.edit.SetFixedWidth( editWidth2 );
	 Latitude_S_NumericEdit.SetValue(s3);

	 LatitudeIsSouth_CheckBox.SetText( "South" );
	 LatitudeIsSouth_CheckBox.SetToolTip( "<p>When checked, latitude is negative (Southern hemisphere).</p>" );
	 LatitudeIsSouth_CheckBox.SetChecked(sign<0);


	 Latitude_Sizer.SetSpacing( 4 );
	 Latitude_Sizer.Add( Latitude_Label );
	 Latitude_Sizer.Add( Latitude_H_SpinBox );
	 Latitude_Sizer.Add( Latitude_M_SpinBox );
	 Latitude_Sizer.Add( Latitude_S_NumericEdit );
	 Latitude_Sizer.Add( LatitudeIsSouth_CheckBox );
	 Latitude_Sizer.AddStretch();

	 DecimalToSexagesimal(sign,s1,s2,s3,geoLong);
	 Longitude_Label.SetText( "Geographic Longitude:" );
	 Longitude_Label.SetToolTip( "<p>Position of observatory: Geographic longitude (g:m:s)</p></p>" );
	 Longitude_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
	 Longitude_Label.SetFixedWidth( labelWidth1 );

	 Longitude_H_SpinBox.SetRange( 0, 180 );
	 Longitude_H_SpinBox.SetFixedWidth( editWidth1 );
	 Longitude_H_SpinBox.SetValue(Abs(s1));

	 Longitude_M_SpinBox.SetRange( 0, 59 );
	 Longitude_M_SpinBox.SetFixedWidth( editWidth1 );
	 Longitude_M_SpinBox.SetValue(s2);

	 Longitude_S_NumericEdit.SetReal();
	 Longitude_S_NumericEdit.SetPrecision( 2 );
	 Longitude_S_NumericEdit.EnableFixedPrecision();
	 Longitude_S_NumericEdit.SetRange( 0, 59.99 );
	 Longitude_S_NumericEdit.label.Hide();
	 Longitude_S_NumericEdit.edit.SetFixedWidth( editWidth2 );
	 Longitude_S_NumericEdit.SetValue(s3);

	 LongitudeIsWest_CheckBox.SetText( "West" );
	 LongitudeIsWest_CheckBox.SetToolTip( "<p>When checked, numeric value refers to westward longitude (negative longitude).</p>" );
	 LongitudeIsWest_CheckBox.SetChecked(sign<0);

	 Longitude_Sizer.SetSpacing( 4 );
	 Longitude_Sizer.Add( Longitude_Label );
	 Longitude_Sizer.Add( Longitude_H_SpinBox );
	 Longitude_Sizer.Add( Longitude_M_SpinBox );
	 Longitude_Sizer.Add( Longitude_S_NumericEdit );
	 Longitude_Sizer.Add( LongitudeIsWest_CheckBox );
	 Longitude_Sizer.AddStretch();

	 TelescopeAperture_NumericEdit.label.SetText("Telescope aperture");
	 TelescopeAperture_NumericEdit.label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
	 TelescopeAperture_NumericEdit.label.SetToolTip("<p>Set telescope aperture (mm)</p>");
	 TelescopeAperture_NumericEdit.label.SetFixedWidth( labelWidth1 );
	 TelescopeAperture_NumericEdit.SetInteger();
	 TelescopeAperture_NumericEdit.SetRange(10,10000);
	 TelescopeAperture_NumericEdit.edit.SetFixedWidth( editWidth2 );
	 TelescopeAperture_NumericEdit.SetValue(telescopeAperture);

	 TelescopeAperture_Sizer.SetSpacing( 8 );
	 TelescopeAperture_Sizer.SetMargin( 8 );
	 TelescopeAperture_Sizer.Add(TelescopeAperture_NumericEdit);
	 TelescopeAperture_Sizer.AddStretch();

	 TelescopeFocalLength_NumericEdit.label.SetText("Telescope focal length");
	 TelescopeFocalLength_NumericEdit.label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
	 TelescopeFocalLength_NumericEdit.label.SetToolTip("<p>Set telescope focal length (mm)</p>");
	 TelescopeFocalLength_NumericEdit.label.SetFixedWidth( labelWidth1 );
	 TelescopeFocalLength_NumericEdit.SetInteger();
	 TelescopeFocalLength_NumericEdit.SetRange(100,10000);
	 TelescopeFocalLength_NumericEdit.edit.SetFixedWidth( editWidth2 );
	 TelescopeFocalLength_NumericEdit.SetValue(telescopeFocalLength);

	 TelescopeFocalLength_Sizer.SetSpacing( 8 );
	 TelescopeFocalLength_Sizer.SetMargin( 8 );
	 TelescopeFocalLength_Sizer.Add(TelescopeFocalLength_NumericEdit);
	 TelescopeFocalLength_Sizer.AddStretch();


	 Global_Sizer.SetSpacing( 8 );
	 Global_Sizer.SetMargin( 8 );
	 Global_Sizer.Add(Latitude_Sizer);
	 Global_Sizer.Add(Longitude_Sizer);
	 Global_Sizer.Add( TelescopeAperture_Sizer );
	 Global_Sizer.Add( TelescopeFocalLength_Sizer );

	 addBaseControls();
 }

 void MountConfigDialog::sendUpdatedProperties() {
	 double lat = SexagesimalToDecimal( LatitudeIsSouth_CheckBox.IsChecked() ? -1 : +1,
			 Latitude_H_SpinBox.Value(), Latitude_M_SpinBox.Value(), Latitude_S_NumericEdit.Value() );

	 INDIClient::TheClient()->SendNewPropertyItem( m_device, "GEOGRAPHIC_COORD", "INDI_NUMBER", "LAT", lat);

	 double longitude = SexagesimalToDecimal( LongitudeIsWest_CheckBox.IsChecked() ? -1 : +1,
			 Longitude_H_SpinBox.Value(), Longitude_M_SpinBox.Value(), Longitude_S_NumericEdit.Value() );

	 INDIClient::TheClient()->SendNewPropertyItem( m_device, "GEOGRAPHIC_COORD", "INDI_NUMBER", "LONG", longitude);

	 // sending telescope info in bulk request
	 INDINewPropertyItem newPropertyItem(m_device, "TELESCOPE_INFO", "INDI_NUMBER");
	 newPropertyItem.ElementValues << ElementValue("TELESCOPE_APERTURE",TelescopeAperture_NumericEdit.Value());
	 newPropertyItem.ElementValues << ElementValue("TELESCOPE_FOCAL_LENGTH",TelescopeFocalLength_NumericEdit.Value());
	 newPropertyItem.ElementValues << ElementValue("GUIDER_APERTURE",10);
	 newPropertyItem.ElementValues << ElementValue("GUIDER_FOCAL_LENGTH",100);

	 INDIClient::TheClient()->SendNewPropertyItem(newPropertyItem );

 }




// ----------------------------------------------------------------------------

INDIMountInterface::INDIMountInterface() :
   ProcessInterface(),
   m_device()
{
   TheINDIMountInterface = this;
}

INDIMountInterface::~INDIMountInterface()
{
   if ( m_searchDialog != nullptr )
      delete m_searchDialog, m_searchDialog = nullptr;
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

void INDIMountInterface::GUIData::getAlignmentConfigParamter(int32& configParam){

	if (m_alignmentConfigOffset) {
		configParam |= 1 << 1; // offset
	}
	if (m_alignmentConfigCollimation) {
		configParam |= 1 << 2; // collimation error
	}
	if (m_alignmentConfigNonPerpendicular) {
		configParam |= 1 << 3; // non-perpendicular dec-ra axis error
	}
	if (m_alignmentConfigPolarAxisDisp) {
		configParam |= 1 << 4; // polar-axis horizontal displacement
		configParam |= 1 << 5; // polar-axis vertical displacement
	}
	if (m_alignmentConfigTubeFlexure) {
		configParam |= 1 << 6; // tube flexure
	}
	if (m_alignmentConfigForkFlexure) {
		configParam |= 1 << 7; // fork flexure
	}
	if (m_alignmentConfigDecAxisFlexure){
		configParam |= 1 << 8; // delta-axis flexure
	}
	if (m_alignmentLinear){
		configParam |= 1 << 9; // linear terms
	}
	if (m_alignmentQuadratic){
		configParam |= 1 << 10; // quadratic terms
	}
	if (m_modelBothPierSides) {
		configParam |= 1 << 31; // set if modeling for each pier side is requested
	}

}

ProcessImplementation* INDIMountInterface::NewProcess() const
{
   INDIMountInstance* instance = new INDIMountInstance( TheINDIMountProcess );
   instance->p_deviceName = m_device;
   instance->p_slewRate = GUI->SlewSpeed_ComboBox.CurrentItem();

   instance->p_targetRA = SexagesimalToDecimal( 0,
                              GUI->TargetRA_H_SpinBox.Value(), GUI->TargetRA_M_SpinBox.Value(), GUI->TargetRA_S_NumericEdit.Value() );

   instance->p_targetDec = SexagesimalToDecimal( GUI->MountTargetDECIsSouth_CheckBox.IsChecked() ? -1 : +1,
                              GUI->TargetDec_H_SpinBox.Value(), GUI->TargetDec_M_SpinBox.Value(), GUI->TargetDec_S_NumericEdit.Value() );

   instance->p_computeApparentPosition = GUI->MountComputeApparentPosition_CheckBox.IsChecked();

   instance->p_alignmentMethod = GUI->m_aignmentModelIndex;

   instance->p_enableAlignmentCorrection =  GUI->MountAlignmentCorrection_CheckBox.IsChecked();



   switch(instance->p_alignmentMethod){
   	case IMCAlignmentMethod::AnalyticalModel:
   	{
   		GUI->getAlignmentConfigParamter(instance->p_alignmentConfig);
   	}
   	}

   instance->p_alignmentFile = GUI->AlignmentFile_Edit.Text();


   instance->GetCurrentCoordinates();

   instance->p_pierSide = instance->o_currentLST <= instance->o_currentRA ? IMCPierSide::West : IMCPierSide::East;

   instance->o_geographicLatitude = m_geoLatitude;

   return instance;
}

bool INDIMountInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const INDIMountInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an INDIMount instance.";
   return false;
}

bool INDIMountInterface::RequiresInstanceValidation() const
{
   return true;
}

bool INDIMountInterface::ImportProcess( const ProcessImplementation& p )
{
   const INDIMountInstance* instance = dynamic_cast<const INDIMountInstance*>( &p );
   if ( instance != nullptr )
   {
      double targetRA, targetDec;
      instance->GetTargetCoordinates( targetRA, targetDec );

      int sign, s1, s2; double s3;
      DecimalToSexagesimal( sign, s1, s2, s3, targetRA );
      GUI->TargetRA_H_SpinBox.SetValue( s1 );
      GUI->TargetRA_M_SpinBox.SetValue( s2 );
      GUI->TargetRA_S_NumericEdit.SetValue( s3 );

      DecimalToSexagesimal( sign, s1, s2, s3, targetDec );
      GUI->TargetDec_H_SpinBox.SetValue( s1 );
      GUI->TargetDec_M_SpinBox.SetValue( s2 );
      GUI->TargetDec_S_NumericEdit.SetValue( s3 );
      GUI->MountTargetDECIsSouth_CheckBox.SetChecked( sign < 0 );

      if ( instance->ValidateDevice( false/*throwErrors*/ ) )
         m_device = instance->p_deviceName;
      else
         m_device.Clear();

      UpdateControls();

      return true;
   }

   return false;
}

// ----------------------------------------------------------------------------

void INDIMountInterface::UpdateControls()
{
   if ( m_device.IsEmpty() )
   {
      GUI->UpdateDeviceProperties_Timer.Stop();
      GUI->MountProperties_Control.Disable();
      GUI->MountAlignment_Control.Disable();
      GUI->Slew_Control.Disable();
      GUI->MountGoTo_Control.Disable();
   }
   else
   {
      if ( !GUI->UpdateDeviceProperties_Timer.IsRunning() )
         GUI->UpdateDeviceProperties_Timer.Start();

      GUI->MountProperties_Control.Enable();
      GUI->MountAlignment_Control.Enable();
      GUI->Slew_Control.Enable();
      GUI->MountGoTo_Control.Enable();
   }
}

// ----------------------------------------------------------------------------

INDIMountInterface::GUIData::GUIData( INDIMountInterface& w )
{
   String infoLabelStyleSheet =
      "QLabel {"
         "font-family: DejaVu Sans Mono, Monospace;"
      "}";

   String moveButtonStyleSheet = w.ScaledStyleSheet(
      "QToolButton, QToolButton:hover, QToolButton:pressed {"
         "border-image: none;"
         "border: 1px solid gray;"
         "border-radius: 4px;"
      "}" );

   int emWidth = w.Font().Width( 'm' );
   int labelWidth1 = w.Font().Width( "Local Sidereal Time:" ) + emWidth;
   int editWidth1 = RoundInt( 4.25*emWidth );
   int editWidth2 = RoundInt( 5.25*emWidth );

   ServerParameters_SectionBar.SetTitle( "Device Properties" );
   ServerParameters_SectionBar.SetSection( ServerParameters_Control );

   MountDevice_Label.SetText( "INDI Mount device:" );
   MountDevice_Label.SetToolTip( "<p>Select an INDI Mount device.</p>" );
   MountDevice_Label.SetMinWidth( labelWidth1 );
   MountDevice_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   MountDeviceConfig_ToolButton.SetIcon(w.ScaledResource(":/icons/wrench.png"));
   MountDeviceConfig_ToolButton.SetScaledFixedSize(22, 22);
   MountDeviceConfig_ToolButton.SetToolTip("<p>Configure INDI mount device</p>");
   MountDeviceConfig_ToolButton.OnClick((Button::click_event_handler) &INDIMountInterface::e_Click, w);

   MountDevice_Combo.OnItemSelected( (ComboBox::item_event_handler)& INDIMountInterface::e_ItemSelected, w );


   MountDevice_Sizer.SetSpacing( 4 );
   MountDevice_Sizer.Add( MountDevice_Label );
   MountDevice_Sizer.Add( MountDevice_Combo, 100 );
   MountDevice_Sizer.Add( MountDeviceConfig_ToolButton);

   LST_Label.SetText( "Local Sidereal Time:" );
   LST_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   LST_Label.SetFixedWidth( labelWidth1 );

   LST_Value_Label.SetStyleSheet( infoLabelStyleSheet );
   LST_Value_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   MountLST_Sizer.SetSpacing( 4 );
   MountLST_Sizer.Add( LST_Label );
   MountLST_Sizer.Add( LST_Value_Label, 100 );

   RA_Label.SetText( "Right Ascension:" );
   RA_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RA_Label.SetFixedWidth( labelWidth1 );

   RA_Value_Label.SetStyleSheet( infoLabelStyleSheet );
   RA_Value_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   MountEQRA_Sizer.SetSpacing( 4 );
   MountEQRA_Sizer.Add( RA_Label );
   MountEQRA_Sizer.Add( RA_Value_Label, 100 );

   Dec_Label.SetText( "Declination:" );
   Dec_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Dec_Label.SetFixedWidth( labelWidth1 );

   Dec_Value_Label.SetStyleSheet( infoLabelStyleSheet );
   Dec_Value_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   MountEQDec_Sizer.SetSpacing( 4 );
   MountEQDec_Sizer.Add( Dec_Label );
   MountEQDec_Sizer.Add( Dec_Value_Label, 100 );

   AZ_Label.SetText( "Azimuth:" );
   AZ_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   AZ_Label.SetFixedWidth( labelWidth1 );

   AZ_Value_Label.SetStyleSheet( infoLabelStyleSheet );
   AZ_Value_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   MountHZAZ_Sizer.SetSpacing( 4 );
   MountHZAZ_Sizer.Add( AZ_Label );
   MountHZAZ_Sizer.Add( AZ_Value_Label, 100 );

   ALT_Label.SetText( "Altitude:" );
   ALT_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ALT_Label.SetFixedWidth( labelWidth1 );

   ALT_Value_Label.SetStyleSheet( infoLabelStyleSheet );
   ALT_Value_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   MountHZALT_Sizer.SetSpacing( 4 );
   MountHZALT_Sizer.Add( ALT_Label );
   MountHZALT_Sizer.Add( ALT_Value_Label, 100 );

   MountProperties_Sizer.SetSpacing( 4 );
   MountProperties_Sizer.Add( MountLST_Sizer );
   MountProperties_Sizer.Add( MountEQRA_Sizer );
   MountProperties_Sizer.Add( MountEQDec_Sizer );
   MountProperties_Sizer.Add( MountHZAZ_Sizer );
   MountProperties_Sizer.Add( MountHZALT_Sizer );

   MountProperties_Control.SetSizer( MountProperties_Sizer );

   ServerParameters_Sizer.SetSpacing( 4 );
   ServerParameters_Sizer.Add( MountDevice_Sizer );
   ServerParameters_Sizer.Add( MountProperties_Control );

   ServerParameters_Control.SetSizer( ServerParameters_Sizer );

   //
   MountAlignment_SectionBar.SetTitle( "Pointing Models" );
   MountAlignment_SectionBar.SetSection(MountAlignment_Control);

   const char* alignmentfileToolTipText =
      		   "<p>File with metadata of a telescope pointing model. </p>";

   AlignmentFile_Label.SetText("Pointing model:");
   AlignmentFile_Label.SetToolTip(alignmentfileToolTipText);
   AlignmentFile_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
   AlignmentFile_Label.SetFixedWidth(labelWidth1);

   AlignmentFile_Edit.SetToolTip(alignmentfileToolTipText);
   AlignmentFile_Edit.SetReadOnly();

   AlignmentFile_ToolButton.SetIcon(w.ScaledResource(":/icons/select-file.png"));
   AlignmentFile_ToolButton.SetScaledFixedSize(22, 22);
   AlignmentFile_ToolButton.SetToolTip("<p>Select the ponting model file:</p>");
   AlignmentFile_ToolButton.OnClick((Button::click_event_handler) &INDIMountInterface::e_Click, w);

   MountAlignmentFile_Sizer.SetSpacing(8);
   MountAlignmentFile_Sizer.Add(AlignmentFile_Label);
   MountAlignmentFile_Sizer.Add(AlignmentFile_Edit, 100);
   MountAlignmentFile_Sizer.AddSpacing(4);
   MountAlignmentFile_Sizer.Add(AlignmentFile_ToolButton);

   const char* syncdatafileToolTipText =
         		   "<p>File which store the sync data. </p>";


   SyncDataList_Button.SetText("Syncdata List");
   SyncDataList_Button.SetIcon( w.ScaledResource( ":/icons/list.png" ) );
   SyncDataList_Button.SetStyleSheet("QPushButton { text-align: left; }");
   SyncDataList_Button.OnClick((Button::click_event_handler) &INDIMountInterface::e_Click, w);

   const char* alignmentConfigToolTipText = "<p>Configure and create a new pointing model.</p>";

   MountAligmentModelConfig_Button.SetText("Configure");
   MountAligmentModelConfig_Button.SetIcon(w.ScaledResource(":/icons/wrench.png"));
   MountAligmentModelConfig_Button.SetStyleSheet("QPushButton { text-align: left; }");
   MountAligmentModelConfig_Button.OnClick((Button::click_event_handler) &INDIMountInterface::e_Click, w);

   MountAligmentModelFit_Button.SetText("Create Model");
   MountAligmentModelFit_Button.SetIcon( w.ScaledResource( ":/icons/gear.png" ) );
   MountAligmentModelFit_Button.SetStyleSheet("QPushButton { text-align: left; }");
   MountAligmentModelFit_Button.OnClick((Button::click_event_handler) &INDIMountInterface::e_Click, w);

   MountAlignmentPlotResiduals_CheckBox.SetText("plot residuals");
   MountAlignmentPlotResiduals_CheckBox.SetToolTip("<p>plot residuals</p>");

   MountAlignmentCorrection_CheckBox.SetText( "Apply Pointing correction" );
   MountAlignmentCorrection_CheckBox.SetToolTip( "<p>Compute and apply telescope pointing correction to target coordinates.</p>" );
   MountAlignmentCorrection_Sizer.SetSpacing(8);
   MountAlignmentCorrection_Sizer.AddSpacing( labelWidth1 + 4 );
   MountAlignmentCorrection_Sizer.Add( MountAlignmentCorrection_CheckBox );
   MountAlignmentCorrection_Sizer.Add( MountAlignmentPlotResiduals_CheckBox );
   MountAlignmentCorrection_Sizer.AddStretch();


   MountAlignmentConfig_Sizer.SetSpacing(8);
   MountAlignmentConfig_Sizer.AddSpacing( labelWidth1 + 4 );
   MountAlignmentConfig_Sizer.Add(MountAligmentModelConfig_Button);
   MountAlignmentConfig_Sizer.Add(MountAligmentModelFit_Button);
   MountAlignmentConfig_Sizer.Add(SyncDataList_Button);
   MountAlignmentConfig_Sizer.AddStretch();

   MountAlignment_Sizer.SetSpacing( 8 );
   MountAlignment_Sizer.Add( MountAlignmentFile_Sizer );
   MountAlignment_Sizer.Add( MountAlignmentConfig_Sizer );
   MountAlignment_Sizer.Add( MountAlignmentCorrection_Sizer );

   MountAlignment_Control.SetSizer(MountAlignment_Sizer);

   //

   MountGoTo_SectionBar.SetTitle( "GoTo" );
   MountGoTo_SectionBar.SetSection( MountGoTo_Control );

   TargetRA_Label.SetText( "Right Ascension:" );
   TargetRA_Label.SetToolTip( "<p>Target object position in equatorial coordinates: Right Ascension (h:m:s)</p>" );
   TargetRA_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   TargetRA_Label.SetFixedWidth( labelWidth1 );

   TargetRA_H_SpinBox.SetRange( 0, 23 );
   TargetRA_H_SpinBox.SetFixedWidth( editWidth1 );

   TargetRA_M_SpinBox.SetRange( 0, 59 );
   TargetRA_M_SpinBox.SetFixedWidth( editWidth1 );

   TargetRA_S_NumericEdit.SetReal();
   TargetRA_S_NumericEdit.SetPrecision( 3 );
   TargetRA_S_NumericEdit.EnableFixedPrecision();
   TargetRA_S_NumericEdit.SetRange( 0, 59.999 );
   TargetRA_S_NumericEdit.label.Hide();
   TargetRA_S_NumericEdit.edit.SetFixedWidth( editWidth2 );

   MountTargetRA_Sizer.SetSpacing( 4 );
   MountTargetRA_Sizer.Add( TargetRA_Label );
   MountTargetRA_Sizer.Add( TargetRA_H_SpinBox );
   MountTargetRA_Sizer.Add( TargetRA_M_SpinBox );
   MountTargetRA_Sizer.Add( TargetRA_S_NumericEdit );
   MountTargetRA_Sizer.AddStretch();

   TargetDec_Label.SetText( "Declination:" );
   TargetDec_Label.SetToolTip( "<p>Target object position in equatorial coordinates: Declination (d:m:s)</p>" );
   TargetDec_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   TargetDec_Label.SetFixedWidth( labelWidth1 );

   TargetDec_H_SpinBox.SetRange( 0, 90 );
   TargetDec_H_SpinBox.SetFixedWidth( editWidth1 );

   TargetDec_M_SpinBox.SetRange( 0, 59 );
   TargetDec_M_SpinBox.SetFixedWidth( editWidth1 );

   TargetDec_S_NumericEdit.SetReal();
   TargetDec_S_NumericEdit.SetPrecision( 2 );
   TargetDec_S_NumericEdit.EnableFixedPrecision();
   TargetDec_S_NumericEdit.SetRange( 0, 59.99 );
   TargetDec_S_NumericEdit.label.Hide();
   TargetDec_S_NumericEdit.edit.SetFixedWidth( editWidth2 );

   MountTargetDECIsSouth_CheckBox.SetText( "South" );
   MountTargetDECIsSouth_CheckBox.SetToolTip( "<p>When checked, declination is negative (Southern hemisphere).</p>" );

   MountTargetDec_Sizer.SetSpacing( 4 );
   MountTargetDec_Sizer.Add( TargetDec_Label );
   MountTargetDec_Sizer.Add( TargetDec_H_SpinBox );
   MountTargetDec_Sizer.Add( TargetDec_M_SpinBox );
   MountTargetDec_Sizer.Add( TargetDec_S_NumericEdit );
   MountTargetDec_Sizer.Add( MountTargetDECIsSouth_CheckBox );
   MountTargetDec_Sizer.AddStretch();

   MountSearch_Button.SetText( "Search" );
   MountSearch_Button.SetIcon( w.ScaledResource( ":/icons/find.png" ) );
   MountSearch_Button.SetStyleSheet( "QPushButton { text-align: left; }" );
   MountSearch_Button.SetToolTip( "<p>Open the Online Coordinate Search dialog.</p>" );
   MountSearch_Button.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );

   MountComputeApparentPosition_CheckBox.SetText( "Equinox J2000.0" );
   MountComputeApparentPosition_CheckBox.SetToolTip( "<p>Check if manually entered coordinates refer to J2000.0 equinox.</p>" );
   MountComputeApparentPosition_Sizer.AddSpacing( labelWidth1 + 4 );
   MountComputeApparentPosition_Sizer.Add( MountComputeApparentPosition_CheckBox );
   MountComputeApparentPosition_Sizer.AddStretch();

   MountSearch_Sizer.SetSpacing( 8 );
   MountSearch_Sizer.AddSpacing( labelWidth1 + 4 );
   MountSearch_Sizer.Add( MountSearch_Button );
   MountSearch_Sizer.AddStretch();



   MountGoToStart_Button.SetText( "GoTo" );
   MountGoToStart_Button.SetIcon( w.ScaledResource( ":/icons/play.png" ) );
   MountGoToStart_Button.SetStyleSheet( "QPushButton { text-align: left; }" );
   MountGoToStart_Button.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );

   MountSync_Button.SetText( "Sync" );
   MountSync_Button.SetIcon( w.ScaledResource( ":/icons/track.png" ) );
   MountSync_Button.SetStyleSheet( "QPushButton { text-align: left; }" );
   MountSync_Button.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );

   MountPark_Button.SetText( "Park" );
   MountPark_Button.SetIcon( w.ScaledResource( ":/icons/move-center.png" ) );
   MountPark_Button.SetStyleSheet( "QPushButton { text-align: left; }" );
   MountPark_Button.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );

   MountGoToStart_Sizer.SetSpacing( 8 );
   MountGoToStart_Sizer.AddSpacing( labelWidth1 + 4 );
   MountGoToStart_Sizer.Add( MountGoToStart_Button );
   MountGoToStart_Sizer.Add( MountSync_Button );
   MountGoToStart_Sizer.Add( MountPark_Button );
   MountGoToStart_Sizer.AddStretch();

   MountGoToCancel_Button.SetText( "Cancel" );
   MountGoToCancel_Button.SetIcon( w.ScaledResource( ":/icons/cancel.png" ) );
   MountGoToCancel_Button.SetStyleSheet( "QPushButton { text-align: left; }" );
   MountGoToCancel_Button.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );
   MountGoToCancel_Button.Disable();

   MountGoToCancel_Sizer.AddSpacing( labelWidth1 + 4 );
   MountGoToCancel_Sizer.Add( MountGoToCancel_Button );
   MountGoToCancel_Sizer.AddStretch();

   MountGoToInfo_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   MountGoTo_Sizer.SetSpacing( 8 );
   MountGoTo_Sizer.Add( MountTargetRA_Sizer );
   MountGoTo_Sizer.Add( MountTargetDec_Sizer );
   MountGoTo_Sizer.Add( MountComputeApparentPosition_Sizer );
   MountGoTo_Sizer.Add( MountSearch_Sizer );
   MountGoTo_Sizer.Add( MountGoToStart_Sizer );
   MountGoTo_Sizer.Add( MountGoToCancel_Sizer );
   MountGoTo_Sizer.Add( MountGoToInfo_Label );

   MountGoTo_Control.SetSizer( MountGoTo_Sizer );
	//

   Slew_SectionBar.SetTitle( "Slew" );
   Slew_SectionBar.SetSection( Slew_Control );

   SlewTopLeft_Button.SetIcon( w.ScaledResource( ":/icons/move-left-up.png" ) );
   SlewTopLeft_Button.SetScaledFixedSize( 32, 32 );
   SlewTopLeft_Button.SetToolTip( "Northwest" );
   SlewTopLeft_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewTopLeft_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewTopLeft_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewTop_Button.SetIcon( w.ScaledResource( ":/icons/up.png" ) );
   SlewTop_Button.SetScaledFixedSize( 32, 32 );
   SlewTop_Button.SetToolTip( "North" );
   SlewTop_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewTop_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewTop_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewTopRight_Button.SetIcon( w.ScaledResource( ":/icons/move-right-up.png" ) );
   SlewTopRight_Button.SetScaledFixedSize( 32, 32 );
   SlewTopRight_Button.SetToolTip( "Northeast" );
   SlewTopRight_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewTopRight_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewTopRight_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewTopRow_Sizer.SetSpacing( 8 );
   SlewTopRow_Sizer.AddSpacing( labelWidth1 + 4 );
   SlewTopRow_Sizer.Add( SlewTopLeft_Button );
   SlewTopRow_Sizer.Add( SlewTop_Button );
   SlewTopRow_Sizer.Add( SlewTopRight_Button );

   SlewLeft_Button.SetIcon( w.ScaledResource( ":/icons/left.png" ) );
   SlewLeft_Button.SetScaledFixedSize( 32, 32 );
   SlewLeft_Button.SetToolTip( "West" );
   SlewLeft_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewLeft_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewLeft_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewStop_Button.SetIcon( w.ScaledResource( ":/icons/stop.png" ) );
   SlewStop_Button.SetScaledFixedSize( 32, 32 );
   SlewStop_Button.SetToolTip( "Stop" );
   SlewStop_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewStop_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewStop_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewRight_Button.SetIcon( w.ScaledResource( ":/icons/right.png" ) );
   SlewRight_Button.SetScaledFixedSize( 32, 32 );
   SlewRight_Button.SetToolTip( "East" );
   SlewRight_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewRight_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewRight_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewMiddleRow_Sizer.SetSpacing( 8 );
   SlewMiddleRow_Sizer.AddSpacing( labelWidth1 + 4 );
   SlewMiddleRow_Sizer.Add( SlewLeft_Button );
   SlewMiddleRow_Sizer.Add( SlewStop_Button );
   SlewMiddleRow_Sizer.Add( SlewRight_Button );

   SlewBottomLeft_Button.SetIcon( w.ScaledResource( ":/icons/move-left-down.png" ) );
   SlewBottomLeft_Button.SetScaledFixedSize( 32, 32 );
   SlewBottomLeft_Button.SetToolTip( "Southwest" );
   SlewBottomLeft_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewBottomLeft_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewBottomLeft_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewBottom_Button.SetIcon( w.ScaledResource( ":/icons/down.png" ) );
   SlewBottom_Button.SetScaledFixedSize( 32, 32 );
   SlewBottom_Button.SetToolTip( "South" );
   SlewBottom_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewBottom_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewBottom_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewBottomRight_Button.SetIcon( w.ScaledResource( ":/icons/move-right-down.png" ) );
   SlewBottomRight_Button.SetScaledFixedSize( 32, 32 );
   SlewBottomRight_Button.SetToolTip( "Southeast" );
   SlewBottomRight_Button.SetStyleSheet( moveButtonStyleSheet );
   SlewBottomRight_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   SlewBottomRight_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   SlewBottomRow_Sizer.SetSpacing( 8 );
   SlewBottomRow_Sizer.AddSpacing( labelWidth1 + 4 );
   SlewBottomRow_Sizer.Add( SlewBottomLeft_Button );
   SlewBottomRow_Sizer.Add( SlewBottom_Button );
   SlewBottomRow_Sizer.Add( SlewBottomRight_Button );

   SlewLeft_Sizer.SetSpacing( 8 );
   SlewLeft_Sizer.Add( SlewTopRow_Sizer );
   SlewLeft_Sizer.Add( SlewMiddleRow_Sizer );
   SlewLeft_Sizer.Add( SlewBottomRow_Sizer );

   SlewLeft_Control.SetSizer( SlewLeft_Sizer );

   const char* slewSpeedTooltipText =
      "<p>Predefined slew rates, in ascending speed order: Guide, Centering, Find, Maximum.</p>"
      "<p>There might be more device-specific slew rates, which can be selected with INDI Device Controller.</p>";

   SlewSpeed_Label.SetText( "Slew speed:" );
   SlewSpeed_Label.SetToolTip( slewSpeedTooltipText );
   SlewSpeed_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   SlewSpeed_ComboBox.AddItem( "Guide" );
   SlewSpeed_ComboBox.AddItem( "Centering" );
   SlewSpeed_ComboBox.AddItem( "Find" );
   SlewSpeed_ComboBox.AddItem( "Maximum" );
   SlewSpeed_ComboBox.SetToolTip( slewSpeedTooltipText );
   SlewSpeed_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&INDIMountInterface::e_ItemSelected, w );

   SlewRight_Sizer.AddStretch();
   SlewRight_Sizer.Add( SlewSpeed_Label );
   SlewRight_Sizer.AddSpacing( 2 );
   SlewRight_Sizer.Add( SlewSpeed_ComboBox );
   SlewRight_Sizer.AddStretch();

   Slew_Sizer.SetSpacing( 32 );
   Slew_Sizer.Add( SlewLeft_Control );
   Slew_Sizer.Add( SlewRight_Sizer );
   Slew_Sizer.AddStretch();

   Slew_Control.SetSizer( Slew_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( ServerParameters_SectionBar );
   Global_Sizer.Add( ServerParameters_Control );
   Global_Sizer.Add( MountAlignment_SectionBar );
   Global_Sizer.Add( MountAlignment_Control );
   Global_Sizer.Add( MountGoTo_SectionBar );
   Global_Sizer.Add( MountGoTo_Control );
   Global_Sizer.Add( Slew_SectionBar );
   Global_Sizer.Add( Slew_Control );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedHeight();

   UpdateDeviceList_Timer.SetInterval( 0.5 );
   UpdateDeviceList_Timer.SetPeriodic( true );
   UpdateDeviceList_Timer.OnTimer( (Timer::timer_event_handler)&INDIMountInterface::e_Timer, w );
   UpdateDeviceList_Timer.Start();

   UpdateDeviceProperties_Timer.SetInterval( 1 );
   UpdateDeviceProperties_Timer.SetPeriodic( true );
   UpdateDeviceProperties_Timer.OnTimer( (Timer::timer_event_handler)&INDIMountInterface::e_Timer, w );
}

void INDIMountInterface::e_Timer( Timer& sender )
{
   if ( sender == GUI->UpdateDeviceList_Timer )
   {
      GUI->MountDevice_Combo.Clear();
      if ( INDIClient::HasClient() )
      {
         INDIClient* indi = INDIClient::TheClient();
         ExclConstDeviceList x = indi->ConstDeviceList();
         const INDIDeviceListItemArray& devices( x );
         if ( devices.IsEmpty() )
            GUI->MountDevice_Combo.AddItem( "<No Device Available>" );
         else
         {
            GUI->MountDevice_Combo.AddItem( String() );

            for ( auto device : devices )
               if ( indi->HasPropertyItem( device.DeviceName, "EQUATORIAL_EOD_COORD", "RA" ) ) // is this a mount device?
                  GUI->MountDevice_Combo.AddItem( device.DeviceName );

            GUI->MountDevice_Combo.SetItemText( 0,
                  (GUI->MountDevice_Combo.NumberOfItems() > 1) ? "<No Device Selected>" : "<No Mount Device Available>" );

            int i = Max( 0, GUI->MountDevice_Combo.FindItem( m_device ) );
            GUI->MountDevice_Combo.SetCurrentItem( i );
            if ( i > 0 )
               goto __device_found;
         }
      }
      else
         GUI->MountDevice_Combo.AddItem( "<INDI Server Not Connected>" );

      m_device.Clear();

__device_found:

      UpdateControls();
   }
   else if ( sender == GUI->UpdateDeviceProperties_Timer )
   {
      if ( !INDIClient::HasClient() )
         return;

      INDIClient* indi = INDIClient::TheClient();
      INDIPropertyListItem mountProp;

      if ( indi->GetPropertyItem( m_device, "TIME_LST", "LST", mountProp, false/*formatted*/ ) )
         GUI->LST_Value_Label.SetText( String::ToSexagesimal( mountProp.PropertyValue.ToDouble(),
                                 SexagesimalConversionOptions( 3/*items*/, 3/*precision*/, false/*sign*/, 3/*width*/ ) ) );
      else
         GUI->LST_Value_Label.Clear();

      if ( indi->GetPropertyItem( m_device, "EQUATORIAL_EOD_COORD", "RA", mountProp, false/*formatted*/ ) )
         GUI->RA_Value_Label.SetText( String::ToSexagesimal( mountProp.PropertyValue.ToDouble(),
                                 SexagesimalConversionOptions( 3/*items*/, 3/*precision*/, false/*sign*/, 3/*width*/ ) ) );
      else
         GUI->RA_Value_Label.Clear();

      if ( indi->GetPropertyItem( m_device, "EQUATORIAL_EOD_COORD", "DEC", mountProp, false/*formatted*/ ) )
         GUI->Dec_Value_Label.SetText( String::ToSexagesimal( mountProp.PropertyValue.ToDouble(),
                                 SexagesimalConversionOptions( 3/*items*/, 2/*precision*/, true/*sign*/, 3/*width*/ ) ) );
      else
         GUI->Dec_Value_Label.Clear();

      if ( indi->GetPropertyItem( m_device, "HORIZONTAL_COORD", "ALT", mountProp, false/*formatted*/ ) )
         GUI->ALT_Value_Label.SetText( String::ToSexagesimal( mountProp.PropertyValue.ToDouble(),
                                 SexagesimalConversionOptions( 3/*items*/, 2/*precision*/, true/*sign*/, 3/*width*/ ) ) );
      else
         GUI->ALT_Value_Label.Clear();

      if ( indi->GetPropertyItem( m_device, "HORIZONTAL_COORD", "AZ", mountProp, false/*formatted*/ ) )
         GUI->AZ_Value_Label.SetText( String::ToSexagesimal( mountProp.PropertyValue.ToDouble(),
                                 SexagesimalConversionOptions( 3/*items*/, 2/*precision*/, true/*sign*/, 3/*width*/ ) ) );
      else
         GUI->AZ_Value_Label.Clear();

      bool foundSlewRate = false;
      static const char* indiSlewRates[] = { "SLEW_GUIDE", "SLEW_CENTERING", "SLEW_FIND", "SLEW_MAX" };
      for ( size_type i = 0; i < ItemsInArray( indiSlewRates ); ++i )
         if ( indi->GetPropertyItem( m_device, "TELESCOPE_SLEW_RATE", indiSlewRates[i], mountProp ) )
            if ( mountProp.PropertyValue == "ON" )
            {
               foundSlewRate = true;
               GUI->SlewSpeed_ComboBox.SetCurrentItem( i );
               break;
            }
      GUI->SlewSpeed_Label.Enable( foundSlewRate );
      GUI->SlewSpeed_ComboBox.Enable( foundSlewRate );

      if ( indi->GetPropertyItem( m_device, "GEOGRAPHIC_COORD", "LAT", mountProp, false/*formatted*/ ) )
    	  m_geoLatitude =  mountProp.PropertyValue.ToDouble();
      else
    	  m_geoLatitude = 0;

      if ( indi->GetPropertyItem( m_device, "GEOGRAPHIC_COORD", "LONG", mountProp, false/*formatted*/ ) )
    	  m_geoLongitude = mountProp.PropertyValue.ToDouble();
      else
    	  m_geoLongitude = 0;

      if ( indi->GetPropertyItem( m_device, "TELESCOPE_INFO", "TELESCOPE_APERTURE", mountProp, false/*formatted*/ ) )
    	  m_telescopeAperture =  mountProp.PropertyValue.ToDouble();
      else
    	  m_telescopeAperture = 0;

      if ( indi->GetPropertyItem( m_device, "TELESCOPE_INFO", "TELESCOPE_FOCAL_LENGTH", mountProp, false/*formatted*/ ) )
    	  m_telescopeFocalLength = mountProp.PropertyValue.ToDouble();
      else
    	  m_telescopeFocalLength = 0;


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
      m_command( IMCCommand::Default )
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
   pcl_enum                       m_command;

   virtual void StartMountEvent( double targetRA, double currentRA, double targetDec, double currentDec, pcl_enum command )
   {
      m_iface->m_execution = this;
      m_command = command;
      m_iface->GUI->TargetRA_H_SpinBox.Disable();
      m_iface->GUI->TargetRA_M_SpinBox.Disable();
      m_iface->GUI->TargetDec_H_SpinBox.Disable();
      m_iface->GUI->TargetDec_M_SpinBox.Disable();
      m_iface->GUI->TargetRA_S_NumericEdit.Disable();
      m_iface->GUI->TargetDec_S_NumericEdit.Disable();
      m_iface->GUI->MountTargetDECIsSouth_CheckBox.Disable();
      m_iface->GUI->MountSearch_Button.Disable();
      m_iface->GUI->SlewLeft_Control.Disable();
      m_iface->GUI->MountGoToStart_Button.Disable();
      m_iface->GUI->MountSync_Button.Disable();
      m_iface->GUI->MountPark_Button.Disable();
      m_iface->GUI->MountGoToCancel_Button.Enable();
      m_iface->GUI->MountGoToInfo_Label.Clear();
      m_iface->GUI->AlignmentFile_Label.Disable();
      m_iface->GUI->AlignmentFile_Edit.Disable();
      m_iface->GUI->AlignmentFile_ToolButton.Disable();
      m_iface->GUI->MountAligmentModelConfig_Button.Disable();
      m_iface->GUI->MountAligmentModelFit_Button.Disable();
      m_iface->GUI->MountAligmentModelConfig_Button.Disable();
      m_iface->GUI->MountAlignmentCorrection_CheckBox.Disable();
      Module->ProcessEvents();
   }

   virtual void MountEvent( double targetRA, double currentRA, double targetDec, double currentDec )
   {
      if ( m_abortRequested )
         AbstractINDIMountExecution::Abort();

      switch ( m_command )
      {
      case IMCCommand::GoTo:
         m_iface->GUI->MountGoToInfo_Label.SetText(
              "Slewing: dRA = "
            + String::ToSexagesimal( targetRA - currentRA,
                           SexagesimalConversionOptions( 3/*items*/, 3/*precision*/, true/*sign*/ ) )
            + ", dDec = "
            + String::ToSexagesimal( targetDec - currentDec,
                           SexagesimalConversionOptions( 3/*items*/, 2/*precision*/, true/*sign*/ ) ) );
         break;

      case IMCCommand::Park:
      case IMCCommand::ParkDefault:
         m_iface->GUI->MountGoToInfo_Label.SetText( "Parking telescope..." );
         break;

      case IMCCommand::Sync:
         break;

      default:
         break;
      };

      Module->ProcessEvents();
   }

   virtual void EndMountEvent()
   {
      m_iface->m_execution = nullptr;
      m_iface->GUI->TargetRA_H_SpinBox.Enable();
      m_iface->GUI->TargetRA_M_SpinBox.Enable();
      m_iface->GUI->TargetDec_H_SpinBox.Enable();
      m_iface->GUI->TargetDec_M_SpinBox.Enable();
      m_iface->GUI->TargetRA_S_NumericEdit.Enable();
      m_iface->GUI->TargetDec_S_NumericEdit.Enable();
      m_iface->GUI->MountTargetDECIsSouth_CheckBox.Enable();
      m_iface->GUI->MountSearch_Button.Enable();
      m_iface->GUI->SlewLeft_Control.Enable();
      m_iface->GUI->MountGoToStart_Button.Enable();
      m_iface->GUI->MountSync_Button.Enable();
      m_iface->GUI->MountPark_Button.Enable();
      m_iface->GUI->MountGoToCancel_Button.Disable();
      m_iface->GUI->MountGoToInfo_Label.Clear();
      m_iface->GUI->AlignmentFile_Label.Enable();
      m_iface->GUI->AlignmentFile_Edit.Enable();
      m_iface->GUI->AlignmentFile_ToolButton.Enable();
      m_iface->GUI->MountAligmentModelConfig_Button.Enable();
      m_iface->GUI->MountAligmentModelFit_Button.Enable();
      m_iface->GUI->MountAligmentModelConfig_Button.Enable();
      m_iface->GUI->MountAlignmentCorrection_CheckBox.Enable();

      Module->ProcessEvents();
   }

   virtual void WaitEvent()
   {
      Module->ProcessEvents();
   }

   virtual void AbortEvent()
   {
      EndMountEvent();
   }
};

void INDIMountInterface::e_Click( Button& sender, bool checked )
{
   if ( !INDIClient::HasClient() )
      return;

   if ( sender == GUI->MountGoToStart_Button )
   {
      INDIMountInterfaceExecution( this ).Perform( IMCCommand::GoTo );
   }
   else if ( sender == GUI->MountSync_Button )
   {
      INDIMountInterfaceExecution( this ).Perform( IMCCommand::Sync );
   }
   else if ( sender == GUI->MountPark_Button )
   {
      INDIMountInterfaceExecution( this ).Perform( IMCCommand::ParkDefault );
   }
   else if ( sender == GUI->MountGoToCancel_Button )
   {
      if ( m_execution != nullptr )
         m_execution->Abort();
   }
   else if ( sender == GUI->MountSearch_Button )
   {
	   // FIXME - memory leak?
      if ( m_searchDialog == nullptr )
         m_searchDialog = new CoordinateSearchDialog;
      if ( m_searchDialog->Execute() )
         if ( m_searchDialog->HasValidCoordinates() )
         {
            time_t t0 = ::time( 0 );
            const tm* t = ::gmtime( &t0 );
            double jd = ComplexTimeToJD( t->tm_year+1900, t->tm_mon+1, t->tm_mday, (t->tm_hour + (t->tm_min + t->tm_sec/60.0)/60.0)/24.0 );
            double ra = Rad( m_searchDialog->RA() );
            double dec = Rad( m_searchDialog->Dec() );
            ApparentPosition( jd ).Apply( ra, dec, m_searchDialog->MuRA(), m_searchDialog->MuDec() );

            int sign, s1, s2; double s3;
            DecimalToSexagesimal( sign, s1, s2, s3, Deg( ra )/15 );
            GUI->TargetRA_H_SpinBox.SetValue( s1 );
            GUI->TargetRA_M_SpinBox.SetValue( s2 );
            GUI->TargetRA_S_NumericEdit.SetValue( s3 );

            DecimalToSexagesimal( sign, s1, s2, s3, Deg( dec ) );
            GUI->TargetDec_H_SpinBox.SetValue( s1 );
            GUI->TargetDec_M_SpinBox.SetValue( s2 );
            GUI->TargetDec_S_NumericEdit.SetValue( s3 );
            GUI->MountTargetDECIsSouth_CheckBox.SetChecked( sign < 0 );

            if ( m_searchDialog->GoToTarget() )
               INDIMountInterfaceExecution( this ).Perform( IMCCommand::GoTo );

            GUI->MountComputeApparentPosition_CheckBox.SetChecked( false );
         }
   }
   else if ( sender == GUI->SlewStop_Button )
   {
      INDIClient::TheClient()->MaybeSendNewPropertyItem( m_device, "TELESCOPE_ABORT_MOTION", "INDI_SWITCH", "ABORT", "ON", true/*async*/ );
   }
   else if ( sender == GUI->AlignmentFile_ToolButton)
   {
	   SaveFileDialog f;
	   f.SetCaption( "INDIMount: Select Alignment File" );
	   if ( f.Execute() )
		   GUI->AlignmentFile_Edit.SetText( f.FileName() );

   } else if ( sender == GUI->MountAligmentModelFit_Button)
   {
	   INDIMountInterfaceExecution( this ).Perform( IMCCommand::FitPointingModel );

	   if (GUI->MountAlignmentPlotResiduals_CheckBox.IsChecked()){
		   AutoPointer<AlignmentModel> aModel = nullptr;
		   int32 alignmentConfig = 0;
		   GUI->getAlignmentConfigParamter(alignmentConfig);
		   switch (GUI->m_aignmentModelIndex){
		   case IMCAlignmentMethod::AnalyticalModel:
			   aModel = GeneralAnalyticalPointingModel::create(m_geoLatitude, alignmentConfig, GUI->m_modelBothPierSides);
			   break;
		   default:
			   throw Error( "Internal error: AbstractINDIMountInterface: Unknown Pointing Model.");
		   }

		   aModel->readObject(GUI->AlignmentFile_Edit.Text());
		   plotAlignemtResiduals(aModel.Ptr());
	   }
   }
   else if ( sender == GUI->SyncDataList_Button)
   {
	   AutoPointer<AlignmentModel> aModel = nullptr;
	   int32 alignmentConfig = 0;
	   GUI->getAlignmentConfigParamter(alignmentConfig);
	   switch (GUI->m_aignmentModelIndex){
	   case IMCAlignmentMethod::AnalyticalModel:
		   aModel = GeneralAnalyticalPointingModel::create(m_geoLatitude, alignmentConfig, GUI->m_modelBothPierSides);
		   break;
	   default:
		   throw Error( "Internal error: AbstractINDIMountInterface: Unknown Pointing Model.");
	   }
	   aModel->readObject(GUI->AlignmentFile_Edit.Text());
	   Array<SyncDataPoint>& syncDataList = aModel->getSyncDataPoints();
	   AutoPointer<SyncDataListDialog> syncDataListDialog = new SyncDataListDialog(syncDataList);

	   if ( syncDataListDialog->Execute() ){
		   aModel->writeObject(GUI->AlignmentFile_Edit.Text());
	   }

   }
   else if ( sender == GUI->MountAligmentModelConfig_Button){
	   // FIXME - memory leak?
	   if ( m_alignmentConfigDialog == nullptr )
		   m_alignmentConfigDialog = new AlignmentConfigDialog(*this);

	   m_alignmentConfigDialog->Execute();
   }
   else if (sender == GUI->MountDeviceConfig_ToolButton) {

	   MountConfigDialog mountConfig(m_device, getGeographicLatitude(), getGeographicLongitude(), getTelescopeAperture(), getTelescopeFocalLength());
	   if (mountConfig.Execute() && INDIClient::HasClient()) {

	   }
   }
}

void INDIMountInterface::e_Press( Button& sender )
{
   if ( !INDIClient::HasClient() )
      return;

   if ( sender == GUI->SlewTopLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_NORTH", "ON", true/*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_WEST", "ON", true/*async*/ );
   }
   else if ( sender == GUI->SlewTop_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_NORTH", "ON", true/*async*/ );
   }
   if ( sender == GUI->SlewTopRight_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_NORTH", "ON", true/*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_EAST", "ON", true/*async*/ );
   }
   else if ( sender == GUI->SlewLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_WEST", "ON", true/*async*/ );
   }
   else if ( sender == GUI->SlewRight_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_EAST", "ON", true/*async*/ );
   }
   else if ( sender == GUI->SlewBottomLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_SOUTH", "ON", true/*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_WEST", "ON", true/*async*/ );
   }
   else if ( sender == GUI->SlewBottom_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_SOUTH", "ON", true/*async*/ );
   }
   else if ( sender == GUI->SlewBottomRight_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_SOUTH", "ON", true/*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_EAST", "ON", true/*async*/ );
   }
}

void INDIMountInterface::e_Release( Button& sender )
{
   if ( !INDIClient::HasClient() )
      return;

   if ( sender == GUI->SlewTopLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_NORTH", "OFF", true/*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_WEST", "OFF", true/*async*/ );
   }
   else if ( sender == GUI->SlewTop_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_NORTH", "OFF", true/*async*/ );
   }
   if ( sender == GUI->SlewTopRight_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_NORTH", "OFF", true/*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_EAST", "OFF", true/*async*/ );
   }
   else if ( sender == GUI->SlewLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_WEST", "OFF", true/*async*/ );
   }
   else if ( sender == GUI->SlewRight_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_EAST", "OFF", true/*async*/ );
   }
   else if ( sender == GUI->SlewBottomLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_SOUTH", "OFF", true/*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_WEST", "OFF", true/*async*/ );
   }
   else if ( sender == GUI->SlewBottom_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_SOUTH", "OFF", true/*async*/ );
   }
   else if ( sender == GUI->SlewBottomRight_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_SOUTH", "OFF", true/*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_EAST", "OFF", true/*async*/ );
   }
}

void INDIMountInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( !INDIClient::HasClient() )
      return;

   if ( sender == GUI->MountDevice_Combo )
   {
      m_device = (itemIndex > 0) ? sender.ItemText( itemIndex ).Trimmed() : String();
      UpdateControls();

      // get initial properties and unpark mount
      if ( !m_device.IsEmpty() )
      {
         INDIPropertyListItem item;

         // load configuration on server
         INDIClient::TheClient()->SendNewPropertyItem( m_device, "CONFIG_PROCESS", "INDI_SWITCH", "CONFIG_LOAD", "ON");

         if ( INDIClient::TheClient()->GetPropertyItem( m_device, "TARGET_EOD_COORD", "RA", item, false/*formatted*/ ) )
         {
            int dum, h, m; double s;
            DecimalToSexagesimal( dum, h, m, s, item.PropertyValue.ToDouble() );
            GUI->TargetRA_H_SpinBox.SetValue( h );
            GUI->TargetRA_M_SpinBox.SetValue( m );
            GUI->TargetRA_S_NumericEdit.SetValue( s );
         }

         if ( INDIClient::TheClient()->GetPropertyItem( m_device, "TARGET_EOD_COORD", "DEC", item, false/*formatted*/ ) )
         {
            int sign, d, m; double s;
            DecimalToSexagesimal( sign, d, m, s, item.PropertyValue.ToDouble() );
            GUI->TargetDec_H_SpinBox.SetValue( d );
            GUI->TargetDec_M_SpinBox.SetValue( m );
            GUI->TargetDec_S_NumericEdit.SetValue( s );
            GUI->MountTargetDECIsSouth_CheckBox.SetChecked( sign < 0 );
         }

         // unpark mount
         INDIMountInterfaceExecution( this ).Perform( IMCCommand::Unpark );
      }
   }
   else if ( sender == GUI->SlewSpeed_ComboBox )
   {
      INDIClient::TheClient()->MaybeSendNewPropertyItem( m_device, "TELESCOPE_SLEW_RATE", "INDI_SWITCH",
                     INDIMountInstance::MountSlewRatePropertyString( itemIndex ), "ON", true/*async*/ );
   }

}


const char* RESIDUAL_GNUPLOT_TEMPLATE = R"(

set terminal svg enhanced size 1600,1200 enhanced background rgb 'white' font 'helvetica,12'
set ticslevel 0
set xzeroaxis
set output '%s'
set multiplot layout 2,2 title "Hourangle residuals in [arcmin]"
set ylabel "residual hourangle [arcmin]"
set xlabel "Hourangle [deg]"
plot '%s' index 0 using 1:3 title "west" with points pointtype 5, '%s' index 1 using 1:3 title "east" with points pointtype 7
set xlabel "Declination [deg]"
plot '%s' index 0 using 2:3 title "west" with points pointtype 5, '%s' index 1 using 2:3 title "east" with points pointtype 7
set xlabel "Hourangle [deg]"
set ylabel "Declination [deg]"
set zlabel "residual hourangle [arcmin]"
splot '%s' index 0 using 1:2:3 title "west" with points pointtype 5, '%s' index 1 using 1:2:3 title "east" with points pointtype 7,0 notitle
unset multiplot
set output '%s'
set multiplot layout 2,2 title "Declination residuals in [arcmin]"
set ylabel "residual declination [arcmin]"
set xlabel "Hourangle [deg]"
plot '%s' index 0 using 1:4 title "west" with points pointtype 5, '%s' index 1 using 1:4 title "east" with points pointtype 7
set xlabel "Declination [arcmin]"
plot '%s' index 0 using 2:4 title "west" with points pointtype 5, '%s' index 1 using 2:4 title "east" with points pointtype 7
set xlabel "Hourangle [deg]"
set ylabel "Declination [deg]"
set zlabel "residual hourangle [arcmin]"
splot '%s' index 0 using 1:2:4 title "west" with points pointtype 5, '%s' index 1 using 1:2:4 title "east" with points pointtype 7,0 notitle
unset multiplot
set terminal svg size 800,600 enhanced background rgb 'white' font 'helvetica,12'
set xrange[ %f:%f]
set yrange[ %f:%f]
unset border
set xlabel "residual hourangle [arcmin]"
set ylabel "residual declination [arcmin]"
set xtics axis nomirror
set ytics axis nomirror
set output '%s'
set yzeroaxis
set object 10 circle size 1 fs transparent border 1
set object 11 circle size 0.5 fs transparent border 2
set object 12 circle size 0.1 fs transparent border 3
plot '%s' index 0 using 3:4 title "west" with points pointtype 5, '%s' index 1 using 3:4 title "east" with points pointtype 7
)";

void INDIMountInterface::plotAlignemtResiduals(AlignmentModel* model){

	Array<SyncDataPoint>& syncDatapointList = model->getSyncDataPoints();
	Array<double> delHAs;
	Array<double> delDecs;
	// create residual data file
	IsoString fileContent;
	String modelResidualDataPath = File::SystemTempDirectory();
	if (!modelResidualDataPath.EndsWith('/'))
		modelResidualDataPath += '/';
	modelResidualDataPath += "ModelResidual.dat";
	// pier side is west or None
	double ra_factor = 15 * 60;
	double dec_factor = 60;
	for (auto syncPoint : syncDatapointList) {
		if (!syncPoint.enabled || syncPoint.pierSide == IMCPierSide::East)
			continue;
		double cel_ha = AlignmentModel::rangeShiftHourAngle(syncPoint.localSiderialTime - syncPoint.celestialRA);
		double tel_ha = AlignmentModel::rangeShiftHourAngle(syncPoint.localSiderialTime - syncPoint.telecopeRA);
		double cel_dec = syncPoint.celestialDEC;
		double del_ha = cel_ha - tel_ha;
		double del_dec = syncPoint.celestialDEC - syncPoint.telecopeDEC;

		// compute alignment correction
		double haCor = 0;
		double decCor = 0;
		model->Apply(haCor, decCor, cel_ha, cel_dec);
		double del_haCor = cel_ha - haCor;
		double del_decCor = cel_dec - decCor;
		double haResidual = (del_ha - del_haCor) * ra_factor;
		double decResidual = (del_dec - del_decCor) * dec_factor;
		fileContent.Append(	IsoString().Format("%f %f %f %f\n", cel_ha , cel_dec, haResidual , decResidual));
		delHAs.Append(haResidual);
		delDecs.Append(decResidual);
	}
	fileContent.Append("\n");
	fileContent.Append("\n");
	// pier side is east
	for (auto syncPoint : syncDatapointList) {
		if (!syncPoint.enabled || syncPoint.pierSide != IMCPierSide::East)
			continue;
		double cel_ha = AlignmentModel::rangeShiftHourAngle(syncPoint.localSiderialTime - syncPoint.celestialRA);
		double tel_ha = AlignmentModel::rangeShiftHourAngle(syncPoint.localSiderialTime - syncPoint.telecopeRA);
		double cel_dec = syncPoint.celestialDEC;
		double del_ha = cel_ha - tel_ha;
		double del_dec = syncPoint.celestialDEC - syncPoint.telecopeDEC;

		// compute alignment correction
		double haCor = 0;
		double decCor = 0;
		model->Apply(haCor, decCor, cel_ha, cel_dec);
		double del_haCor = cel_ha - haCor;
		double del_decCor = cel_dec - decCor;
		double haResidual = (del_ha - del_haCor) * ra_factor;
		double decResidual = (del_dec - del_decCor) * dec_factor;
		fileContent.Append(IsoString().Format("%f %f %f %f\n", cel_ha  , cel_dec, haResidual , decResidual));
		delHAs.Append(haResidual);
		delDecs.Append(decResidual);
	}
	if (File::Exists(modelResidualDataPath)) {
		File::Remove(modelResidualDataPath);
	}
	File::WriteTextFile(modelResidualDataPath, fileContent);

	// now create gnuplot file
	IsoString gnufileContent;
	String tmpFilePath = File::SystemTempDirectory();
	if (!tmpFilePath.EndsWith('/'))
		tmpFilePath += '/';
	String gnuFilePath = tmpFilePath + "AlignmentResiduals.gnu";

	// output file names
	Array<String> outputFiles;
	outputFiles.Add(tmpFilePath +  "residuals_ha_3d.svg");
	outputFiles.Add(tmpFilePath +  "residuals_dec_3d.svg");
	outputFiles.Add(tmpFilePath +  "residuals_delha_deldec.svg");
	double maxDev=std::max(*delHAs.MaxItem(),*delDecs.MaxItem());

	gnufileContent = IsoString().Format(RESIDUAL_GNUPLOT_TEMPLATE, outputFiles[0].ToIsoString().c_str(), modelResidualDataPath.ToIsoString().c_str(), modelResidualDataPath.ToIsoString().c_str(),
			 modelResidualDataPath.ToIsoString().c_str(), modelResidualDataPath.ToIsoString().c_str(),
			 modelResidualDataPath.ToIsoString().c_str(), modelResidualDataPath.ToIsoString().c_str(),
			 outputFiles[1].ToIsoString().c_str(), modelResidualDataPath.ToIsoString().c_str(), modelResidualDataPath.ToIsoString().c_str(),
			 modelResidualDataPath.ToIsoString().c_str(), modelResidualDataPath.ToIsoString().c_str(),
			 modelResidualDataPath.ToIsoString().c_str(), modelResidualDataPath.ToIsoString().c_str(),
			-maxDev,maxDev,-maxDev,maxDev,
			outputFiles[2].ToIsoString().c_str(), modelResidualDataPath.ToIsoString().c_str(), modelResidualDataPath.ToIsoString().c_str());

	if (File::Exists(gnuFilePath)) {
		File::Remove(gnuFilePath);
	}
	File::WriteTextFile(gnuFilePath, gnufileContent);

	StringList options;
	options.Add(gnuFilePath);
	ExternalProcess::ExecuteProgram("/opt/PixInsight/bin/gnuplot",options);

	for (auto imagePath :outputFiles ){
		Array<ImageWindow> windowArray = ImageWindow::Open(imagePath);
		windowArray[0].SetZoomFactor(1);
		windowArray[0].FitWindow();
		windowArray[0].Show();
	}
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIMountInterface.cpp - Released 2017-05-02T09:43:01Z
