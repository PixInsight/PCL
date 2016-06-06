//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard INDIClient Process Module Version 01.00.12.0183
// ----------------------------------------------------------------------------
// INDIMountInterface.cpp - Released 2016/06/04 15:14:47 UTC
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
#include <pcl/MetaModule.h>

#include "INDIDeviceControllerInterface.h"
#include "INDIMountInterface.h"
#include "INDIMountParameters.h"
#include "INDIMountProcess.h"

#include "INDI/basedevice.h"
#include "INDI/indiapi.h"
#include "INDI/indibase.h"

namespace pcl
{

// ----------------------------------------------------------------------------

INDIMountInterface* TheINDIMountInterface = nullptr;

// ----------------------------------------------------------------------------

//#include "PixInsightINDIIcon.xpm"

// ----------------------------------------------------------------------------

CoordinateSearchDialog::CoordinateSearchDialog() :
   Dialog(),
   m_targetRA( 0 ),
   m_targetDec( 0 ),
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

   SearchInfo_TextBox.SetScaledMinSize( 500, 300 );
   SearchInfo_TextBox.SetReadOnly();
   SearchInfo_TextBox.SetStyleSheet( ScaledStyleSheet(
         "QTextEdit {"
            "font-family: DejaVu Sans Mono, Monospace;"
            "font-size: 8pt;"
            "background: #141414;" // borrowed from /rsc/qss/core-standard.qss
            "color: #E8E8E8;"
         "}"
      ) );

   Get_Button.SetText( "Get" );
   Get_Button.SetIcon( ScaledResource( ":/icons/window-import.png" ) );
   Get_Button.OnClick( (Button::click_event_handler)&CoordinateSearchDialog::e_Click, *this );
   Get_Button.Disable();

   GoTo_Button.SetText( "GoTo" );
   GoTo_Button.SetIcon( ScaledResource( ":/icons/play.png" ) );
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
      String url( "http://simbad.u-strasbg.fr/simbad/sim-tap/sync?request=doQuery&lang=adql&format=text&query=" );
      String select_stmt = "SELECT basic.OID, RA, DEC, main_id "
                           "AS \"Main identifier\" "
                           "FROM basic JOIN ident "
                           "ON oidref = oid "
                           "WHERE id = '" + objectName + "';";
      url << select_stmt;

      NetworkTransfer transfer;
      transfer.SetURL( url );
      transfer.OnDownloadDataAvailable( (NetworkTransfer::download_event_handler)&CoordinateSearchDialog::e_Download, *this );
      transfer.OnTransferProgress( (NetworkTransfer::progress_event_handler)&CoordinateSearchDialog::e_Progress, *this );

      SearchInfo_TextBox.SetText( "Performing online search:<br><wrap><raw>" + url + "</raw><br><br><flush>" );
      Module->ProcessEvents();

      m_downloadData.Clear();
      m_downloading = true;
      m_abort = false;
      bool ok = transfer.Download();
      m_downloading = false;

      if ( ok )
      {
         SearchInfo_TextBox.Insert( String().Format( "<end><cbr>%d bytes downloaded @ %.3g KiB/s<br>",
                                                     transfer.BytesTransferred(), transfer.TotalSpeed() ) );
         SearchInfo_TextBox.Insert( "<end><cbr><br><raw>" + m_downloadData + "</raw>" );

         StringList lines;
         m_downloadData.Break( lines, '\n', true/*trim*/ );
         if ( lines.Length() >= 3 )
         {
            StringList tokens;
            lines[2].Break( tokens, '|', true/*trim*/ );
            if ( tokens.Length() == 4 )
            {
               m_targetRA = tokens[1].ToDouble()/360*24;
               m_targetDec = tokens[2].ToDouble();
               try
               {
                  SearchInfo_TextBox.Insert(
                        "<end><cbr><br><b>Right Ascension : "
                        + String::ToSexagesimal( m_targetRA,
                                       SexagesimalConversionOptions( 3/*items*/, 3/*precision*/, false/*sign*/, 3/*width*/ ) )
                        +            "<br>Declination     : "
                        + String::ToSexagesimal( m_targetDec,
                                       SexagesimalConversionOptions( 3/*items*/, 2/*precision*/, true/*sign*/, 3/*width*/ ) )
                        +            "</b><br>" );
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

// ----------------------------------------------------------------------------

INDIMountInterface::INDIMountInterface() :
   ProcessInterface(),
   m_device(),
   m_execution( nullptr ),
   m_searchDialog( nullptr ),
   GUI( nullptr )
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

ProcessImplementation* INDIMountInterface::NewProcess() const
{
   INDIMountInstance* instance = new INDIMountInstance( TheINDIMountProcess );
   instance->p_deviceName = m_device;
   instance->p_slewRate = GUI->MountMoveSpeed_ComboBox.CurrentItem();

   instance->p_targetRA = SexagesimalToDecimal( 0,
                              GUI->TargetRA_H_SpinBox.Value(), GUI->TargetRA_M_SpinBox.Value(), GUI->TargetRA_S_NumericEdit.Value() );

   instance->p_targetDec = SexagesimalToDecimal( GUI->MountTargetDECIsSouth_CheckBox.IsChecked() ? -1 : +1,
                              GUI->TargetDec_H_SpinBox.Value(), GUI->TargetDec_M_SpinBox.Value(), GUI->TargetDec_S_NumericEdit.Value() );

   instance->GetCurrentCoordinates();

   return instance;
}

bool INDIMountInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const INDIMountInstance* r = dynamic_cast<const INDIMountInstance*>( &p );
   if ( r == nullptr )
   {
      whyNot = "Not an INDIMount instance.";
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
   if ( instance != nullptr )
   {
      int sign, s1, s2; double s3;
      DecimalToSexagesimal( sign, s1, s2, s3, instance->p_targetRA );
      GUI->TargetRA_H_SpinBox.SetValue( s1 );
      GUI->TargetRA_M_SpinBox.SetValue( s2 );
      GUI->TargetRA_S_NumericEdit.SetValue( s3 );

      DecimalToSexagesimal( sign, s1, s2, s3, instance->p_targetDec );
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
      GUI->MountMove_Control.Disable();
      GUI->MountGoTo_Control.Disable();
   }
   else
   {
      if ( !GUI->UpdateDeviceProperties_Timer.IsRunning() )
         GUI->UpdateDeviceProperties_Timer.Start();

      GUI->MountProperties_Control.Enable();
      GUI->MountMove_Control.Enable();
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

   MountDevice_Combo.OnItemSelected( (ComboBox::item_event_handler)& INDIMountInterface::e_ItemSelected, w );

   MountDevice_Sizer.SetSpacing( 4 );
   MountDevice_Sizer.Add( MountDevice_Label );
   MountDevice_Sizer.Add( MountDevice_Combo, 100 );

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

   MountMove_SectionBar.SetTitle( "Slew" );
   MountMove_SectionBar.SetSection( MountMove_Control );

   MountMoveTopLeft_Button.SetIcon( w.ScaledResource( ":/icons/move-left-up.png" ) );
   MountMoveTopLeft_Button.SetScaledFixedSize( 32, 32 );
   MountMoveTopLeft_Button.SetToolTip( "Northwest" );
   MountMoveTopLeft_Button.SetStyleSheet( moveButtonStyleSheet );
   MountMoveTopLeft_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   MountMoveTopLeft_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   MountMoveTop_Button.SetIcon( w.ScaledResource( ":/icons/up.png" ) );
   MountMoveTop_Button.SetScaledFixedSize( 32, 32 );
   MountMoveTop_Button.SetToolTip( "North" );
   MountMoveTop_Button.SetStyleSheet( moveButtonStyleSheet );
   MountMoveTop_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   MountMoveTop_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   MountMoveTopRight_Button.SetIcon( w.ScaledResource( ":/icons/move-right-up.png" ) );
   MountMoveTopRight_Button.SetScaledFixedSize( 32, 32 );
   MountMoveTopRight_Button.SetToolTip( "Northeast" );
   MountMoveTopRight_Button.SetStyleSheet( moveButtonStyleSheet );
   MountMoveTopRight_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   MountMoveTopRight_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   MountMoveTopRow_Sizer.SetSpacing( 8 );
   MountMoveTopRow_Sizer.AddSpacing( labelWidth1 + 4 );
   MountMoveTopRow_Sizer.Add( MountMoveTopLeft_Button );
   MountMoveTopRow_Sizer.Add( MountMoveTop_Button );
   MountMoveTopRow_Sizer.Add( MountMoveTopRight_Button );

   MountMoveLeft_Button.SetIcon( w.ScaledResource( ":/icons/left.png" ) );
   MountMoveLeft_Button.SetScaledFixedSize( 32, 32 );
   MountMoveLeft_Button.SetToolTip( "West" );
   MountMoveLeft_Button.SetStyleSheet( moveButtonStyleSheet );
   MountMoveLeft_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   MountMoveLeft_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   MountMoveStop_Button.SetIcon( w.ScaledResource( ":/icons/stop.png" ) );
   MountMoveStop_Button.SetScaledFixedSize( 32, 32 );
   MountMoveStop_Button.SetToolTip( "Stop" );
   MountMoveStop_Button.SetStyleSheet( moveButtonStyleSheet );
   MountMoveStop_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   MountMoveStop_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   MountMoveRight_Button.SetIcon( w.ScaledResource( ":/icons/right.png" ) );
   MountMoveRight_Button.SetScaledFixedSize( 32, 32 );
   MountMoveRight_Button.SetToolTip( "East" );
   MountMoveRight_Button.SetStyleSheet( moveButtonStyleSheet );
   MountMoveRight_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   MountMoveRight_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   MountMoveMiddleRow_Sizer.SetSpacing( 8 );
   MountMoveMiddleRow_Sizer.AddSpacing( labelWidth1 + 4 );
   MountMoveMiddleRow_Sizer.Add( MountMoveLeft_Button );
   MountMoveMiddleRow_Sizer.Add( MountMoveStop_Button );
   MountMoveMiddleRow_Sizer.Add( MountMoveRight_Button );

   MountMoveBottomLeft_Button.SetIcon( w.ScaledResource( ":/icons/move-left-down.png" ) );
   MountMoveBottomLeft_Button.SetScaledFixedSize( 32, 32 );
   MountMoveBottomLeft_Button.SetToolTip( "Southwest" );
   MountMoveBottomLeft_Button.SetStyleSheet( moveButtonStyleSheet );
   MountMoveBottomLeft_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   MountMoveBottomLeft_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   MountMoveBottom_Button.SetIcon( w.ScaledResource( ":/icons/down.png" ) );
   MountMoveBottom_Button.SetScaledFixedSize( 32, 32 );
   MountMoveBottom_Button.SetToolTip( "South" );
   MountMoveBottom_Button.SetStyleSheet( moveButtonStyleSheet );
   MountMoveBottom_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   MountMoveBottom_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   MountMoveBottomRight_Button.SetIcon( w.ScaledResource( ":/icons/move-right-down.png" ) );
   MountMoveBottomRight_Button.SetScaledFixedSize( 32, 32 );
   MountMoveBottomRight_Button.SetToolTip( "Southeast" );
   MountMoveBottomRight_Button.SetStyleSheet( moveButtonStyleSheet );
   MountMoveBottomRight_Button.OnPress( (Button::press_event_handler)&INDIMountInterface::e_Press, w );
   MountMoveBottomRight_Button.OnRelease( (Button::press_event_handler)&INDIMountInterface::e_Release, w );

   MountMoveBottomRow_Sizer.SetSpacing( 8 );
   MountMoveBottomRow_Sizer.AddSpacing( labelWidth1 + 4 );
   MountMoveBottomRow_Sizer.Add( MountMoveBottomLeft_Button );
   MountMoveBottomRow_Sizer.Add( MountMoveBottom_Button );
   MountMoveBottomRow_Sizer.Add( MountMoveBottomRight_Button );

   SlewLeft_Sizer.SetSpacing( 8 );
   SlewLeft_Sizer.Add( MountMoveTopRow_Sizer );
   SlewLeft_Sizer.Add( MountMoveMiddleRow_Sizer );
   SlewLeft_Sizer.Add( MountMoveBottomRow_Sizer );

   SlewLeft_Control.SetSizer( SlewLeft_Sizer );

   const char* slewSpeedTooltipText =
      "<p>Predefined slew rates, in ascending speed order: Guide, Centering, Find, Maximum.</p>"
      "<p>There might be more device-specific slew rates, which can be selected with INDI Device Controller.</p>";

   MountMoveSpeed_Label.SetText( "Slew speed:" );
   MountMoveSpeed_Label.SetToolTip( slewSpeedTooltipText );
   MountMoveSpeed_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   MountMoveSpeed_ComboBox.AddItem( "Guide" );
   MountMoveSpeed_ComboBox.AddItem( "Centering" );
   MountMoveSpeed_ComboBox.AddItem( "Find" );
   MountMoveSpeed_ComboBox.AddItem( "Maximum" );
   MountMoveSpeed_ComboBox.SetToolTip( slewSpeedTooltipText );
   MountMoveSpeed_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&INDIMountInterface::e_ItemSelected, w );

   SlewRight_Sizer.AddStretch();
   SlewRight_Sizer.Add( MountMoveSpeed_Label );
   SlewRight_Sizer.AddSpacing( 2 );
   SlewRight_Sizer.Add( MountMoveSpeed_ComboBox );
   SlewRight_Sizer.AddStretch();

   MountMove_Sizer.SetSpacing( 32 );
   MountMove_Sizer.Add( SlewLeft_Control );
   MountMove_Sizer.Add( SlewRight_Sizer );
   MountMove_Sizer.AddStretch();

   MountMove_Control.SetSizer( MountMove_Sizer );

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
   TargetRA_S_NumericEdit.SetRange( 0, 59.9999999999999 );
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
   TargetDec_S_NumericEdit.SetRange( 0, 59.9999999999999 );
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
   MountSearch_Button.OnClick( (Button::click_event_handler)&INDIMountInterface::e_Click, w );

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
   MountGoTo_Sizer.Add( MountSearch_Sizer );
   MountGoTo_Sizer.Add( MountGoToStart_Sizer );
   MountGoTo_Sizer.Add( MountGoToCancel_Sizer );
   MountGoTo_Sizer.Add( MountGoToInfo_Label );

   MountGoTo_Control.SetSizer( MountGoTo_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( ServerParameters_SectionBar );
   Global_Sizer.Add( ServerParameters_Control );
   Global_Sizer.Add( MountMove_SectionBar );
   Global_Sizer.Add( MountMove_Control );
   Global_Sizer.Add( MountGoTo_SectionBar );
   Global_Sizer.Add( MountGoTo_Control );

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
               GUI->MountMoveSpeed_ComboBox.SetCurrentItem( i );
               break;
            }
      GUI->MountMoveSpeed_Label.Enable( foundSlewRate );
      GUI->MountMoveSpeed_ComboBox.Enable( foundSlewRate );
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
      Module->ProcessEvents();
   }

   virtual void MountEvent( double targetRA, double currentRA, double targetDec, double currentDec )
   {
      if ( m_abortRequested )
         AbstractINDIMountExecution::Abort();

      switch ( m_command )
      {
      case IMCCommand::Goto:
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
      INDIMountInterfaceExecution( this ).Perform( IMCCommand::Goto );
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
      if ( m_searchDialog == nullptr )
         m_searchDialog = new CoordinateSearchDialog;
      if ( m_searchDialog->Execute() )
         if ( m_searchDialog->HasValidTargetCoordinates() )
         {
            int sign, s1, s2; double s3;
            DecimalToSexagesimal( sign, s1, s2, s3, m_searchDialog->TargetRA() );
            GUI->TargetRA_H_SpinBox.SetValue( s1 );
            GUI->TargetRA_M_SpinBox.SetValue( s2 );
            GUI->TargetRA_S_NumericEdit.SetValue( s3 );

            DecimalToSexagesimal( sign, s1, s2, s3, m_searchDialog->TargetDec() );
            GUI->TargetDec_H_SpinBox.SetValue( s1 );
            GUI->TargetDec_M_SpinBox.SetValue( s2 );
            GUI->TargetDec_S_NumericEdit.SetValue( s3 );
            GUI->MountTargetDECIsSouth_CheckBox.SetChecked( sign < 0 );

            if ( m_searchDialog->GoToTarget() )
               INDIMountInterfaceExecution( this ).Perform( IMCCommand::Goto );
         }
   }
   else if ( sender == GUI->MountMoveStop_Button )
   {
      INDIClient::TheClient()->MaybeSendNewPropertyItem( m_device, "TELESCOPE_ABORT_MOTION", "INDI_SWITCH", "ABORT", "ON", true/*async*/ );
   }
}

void INDIMountInterface::e_Press( Button& sender )
{
   if ( !INDIClient::HasClient() )
      return;

   if ( sender == GUI->MountMoveTopLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_NORTH", "ON", true/*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_WEST", "ON", true/*async*/ );
   }
   else if ( sender == GUI->MountMoveTop_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_NORTH", "ON", true/*async*/ );
   }
   if ( sender == GUI->MountMoveTopRight_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_NORTH", "ON", true/*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_EAST", "ON", true/*async*/ );
   }
   else if ( sender == GUI->MountMoveLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_WEST", "ON", true/*async*/ );
   }
   else if ( sender == GUI->MountMoveRight_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_EAST", "ON", true/*async*/ );
   }
   else if ( sender == GUI->MountMoveBottomLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_SOUTH", "ON", true/*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_WEST", "ON", true/*async*/ );
   }
   else if ( sender == GUI->MountMoveBottom_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_SOUTH", "ON", true/*async*/ );
   }
   else if ( sender == GUI->MountMoveBottomRight_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_SOUTH", "ON", true/*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_EAST", "ON", true/*async*/ );
   }
}

void INDIMountInterface::e_Release( Button& sender )
{
   if ( !INDIClient::HasClient() )
      return;

   if ( sender == GUI->MountMoveTopLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_NORTH", "OFF", true/*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_WEST", "OFF", true/*async*/ );
   }
   else if ( sender == GUI->MountMoveTop_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_NORTH", "OFF", true/*async*/ );
   }
   if ( sender == GUI->MountMoveTopRight_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_NORTH", "OFF", true/*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_EAST", "OFF", true/*async*/ );
   }
   else if ( sender == GUI->MountMoveLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_WEST", "OFF", true/*async*/ );
   }
   else if ( sender == GUI->MountMoveRight_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_EAST", "OFF", true/*async*/ );
   }
   else if ( sender == GUI->MountMoveBottomLeft_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_SOUTH", "OFF", true/*async*/ );
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_WE", "INDI_SWITCH", "MOTION_WEST", "OFF", true/*async*/ );
   }
   else if ( sender == GUI->MountMoveBottom_Button )
   {
      INDIClient::TheClient()->SendNewPropertyItem( m_device, "TELESCOPE_MOTION_NS", "INDI_SWITCH", "MOTION_SOUTH", "OFF", true/*async*/ );
   }
   else if ( sender == GUI->MountMoveBottomRight_Button )
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
   else if ( sender == GUI->MountMoveSpeed_ComboBox )
   {
      INDIClient::TheClient()->MaybeSendNewPropertyItem( m_device, "TELESCOPE_SLEW_RATE", "INDI_SWITCH",
                     INDIMountInstance::MountSlewRatePropertyString( itemIndex ), "ON", true/*async*/ );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF INDIMountInterface.cpp - Released 2016/06/04 15:14:47 UTC
