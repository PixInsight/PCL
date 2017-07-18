//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard ColorCalibration Process Module Version 01.03.02.0301
// ----------------------------------------------------------------------------
// PhotometricColorCalibrationInterface.cpp - Released 2017-07-18T16:14:18Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorCalibration PixInsight module.
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

#include "PhotometricColorCalibrationGraphInterface.h"
#include "PhotometricColorCalibrationInterface.h"
#include "PhotometricColorCalibrationProcess.h"
#include "PhotometricColorCalibrationParameters.h"

#include <pcl/Console.h>
#include <pcl/ErrorHandler.h>
#include <pcl/MessageBox.h>
#include <pcl/MetaModule.h>
#include <pcl/NetworkTransfer.h>
#include <pcl/PreviewSelectionDialog.h>
#include <pcl/TextBox.h>
#include <pcl/ViewSelectionDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

struct ServerData
{
   String name;
   String url;
};

struct CatalogData
{
   String name;
   String description;
};

static Array<ServerData>  s_servers;
static Array<CatalogData> s_solverCatalogs;
static Array<CatalogData> s_photometryCatalogs;
static bool               s_dataInitialized = false;

static void InitializeData()
{
   if ( !s_dataInitialized )
   {
      s_servers.Clear();
      s_servers << ServerData{ "CDS Strasbourg, France", "http://cdsarc.u-strasbg.fr/" }
                << ServerData{ "ADAC Tokyo, Japan", "http://vizier.nao.ac.jp/" }
                << ServerData{ "CADC Victoria, Canada ", "http://vizier.hia.nrc.ca/" }
                << ServerData{ "CASU Cambridge, UK", "http://vizier.ast.cam.ac.uk/" }
                << ServerData{ "IUCAA Pune, India", "http://vizier.iucaa.ernet.in/" }
                << ServerData{ "NAOC Beijing, China", "http://VizieR.china-vo.org/" }
                << ServerData{ "INASAN Moscow, Russia", "http://vizier.inasan.ru/" }
                << ServerData{ "CFA Harvard, Cambridge, USA", "http://vizier.cfa.harvard.edu/" }
                << ServerData{ "JAC Hilo, Hawaii, USA", "http://www.ukirt.hawaii.edu/" }
                << ServerData{ "SAAO, South Africa", "http://viziersaao.chpc.ac.za/" };

      s_solverCatalogs.Clear();
      s_solverCatalogs << CatalogData{ "UCAC3", "UCAC3 catalog (100,765,502 objects)" }
                       << CatalogData{ "PPMXL", "PPMXL catalog (910,469,430 objects)" }
                       << CatalogData{ "TYCHO-2", "Tycho-2 catalog (2,539,913 stars)" }
                       << CatalogData{ "Bright Stars", "Bright Star Catalog, 5th ed. (Hoffleit+, 9110 stars)" }
                       << CatalogData{ "Gaia", "Gaia Data Release 1 (Gaia collaboration 2016, 1,142,679,769 sources)" };

      s_photometryCatalogs.Clear();
      s_photometryCatalogs << CatalogData{ "APASS", "AAVSO Photometric All Sky Survey DR9 (Henden+, 2016, 62 million stars)" };

      s_dataInitialized = true;
   }
}

// ----------------------------------------------------------------------------

PhotometricColorCalibrationInterface* ThePhotometricColorCalibrationInterface = nullptr;

// ----------------------------------------------------------------------------

#include "PhotometricColorCalibrationIcon.xpm"

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

// km/s -> AU/day
static const double KMS2AUY = 365.25*86400/149597870e3;

class CoordinateSearchDialog : public Dialog
{
public:

   CoordinateSearchDialog();

   const String& ObjectName() const
   {
      return m_objectName;
   }

   const String& ObjectType() const
   {
      return m_objectType;
   }

   const String& SpectralType() const
   {
      return m_spectralType;
   }

   double VMagnitude() const
   {
      return m_vmag;
   }

   // degrees
   double RA() const
   {
      return m_RA;
   }

   // degrees
   double Dec() const
   {
      return m_Dec;
   }

   // mas/year
   double MuRA() const
   {
      return m_muRA;
   }

   // mas/year
   double MuDec() const
   {
      return m_muDec;
   }

   // mas
   double Parallax() const
   {
      return m_parallax;
   }

   // AU/year
   double RadialVelocity() const
   {
      return m_radVel;
   }

   bool HasValidCoordinates() const
   {
      return m_valid;
   }

private:

   VerticalSizer       Global_Sizer;
      HorizontalSizer      Search_Sizer;
         Label                ObjectName_Label;
         Edit                 ObjectName_Edit;
         PushButton           Search_Button;
      TextBox              SearchInfo_TextBox;
      HorizontalSizer      Buttons_Sizer;
         PushButton           Get_Button;
         PushButton           Cancel_Button;

   String    m_objectName;
   String    m_objectType;
   String    m_spectralType;
   double    m_vmag;     // flux in the V filter
   double    m_RA;       // in degrees
   double    m_Dec;      // in degrees
   double    m_muRA;     // in mas/year
   double    m_muDec;    // in mas/year
   double    m_parallax; // in mas
   double    m_radVel;   // in AU/year
   bool      m_valid;
   bool      m_downloading;
   bool      m_abort;
   bool      m_firstTimeShown;
   IsoString m_downloadData;

   void e_Show( Control& sender );
   void e_GetFocus( Control& sender );
   void e_LoseFocus( Control& sender );
   bool e_Download( NetworkTransfer& sender, const void* buffer, fsize_type size );
   bool e_Progress( NetworkTransfer& sender,
                    fsize_type downloadTotal, fsize_type downloadCurrent,
                    fsize_type uploadTotal, fsize_type uploadCurrent );
   void e_Click( Button& sender, bool checked );
};

// ----------------------------------------------------------------------------

CoordinateSearchDialog::CoordinateSearchDialog() :
   m_RA( 0 ),
   m_Dec( 0 ),
   m_muRA( 0 ),
   m_muDec( 0 ),
   m_parallax( 0 ),
   m_valid( false ),
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
   Get_Button.SetToolTip( "<p>Acquire object coordinates.</p>" );
   Get_Button.OnClick( (Button::click_event_handler)&CoordinateSearchDialog::e_Click, *this );
   Get_Button.Disable();

   Cancel_Button.SetText( "Cancel" );
   Cancel_Button.SetIcon( ScaledResource( ":/icons/cancel.png" ) );
   Cancel_Button.OnClick( (Button::click_event_handler)&CoordinateSearchDialog::e_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( Get_Button );
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

// ----------------------------------------------------------------------------

void CoordinateSearchDialog::e_Show( Control& )
{
   if ( m_firstTimeShown )
   {
      m_firstTimeShown = false;
      AdjustToContents();
      SetMinSize();
   }
}

// ----------------------------------------------------------------------------

void CoordinateSearchDialog::e_GetFocus( Control& sender )
{
   if ( sender == ObjectName_Edit )
      Search_Button.SetDefault();
}

// ----------------------------------------------------------------------------

void CoordinateSearchDialog::e_LoseFocus( Control& sender )
{
   if ( sender == ObjectName_Edit )
      Get_Button.SetDefault();
}

// ----------------------------------------------------------------------------

bool CoordinateSearchDialog::e_Download( NetworkTransfer& sender, const void* buffer, fsize_type size )
{
   if ( m_abort )
      return false;

   m_downloadData.Append( static_cast<const char*>( buffer ), size );
   return true;
}

// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------

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
                        + String().Format( "%+8.2f mas/year", m_muRA );
                  if ( m_muDec != 0 )
                     info
                        +=           "<br><b>Proper motion Dec :</b> "
                        + String().Format( "%+8.2f mas/year", m_muDec );
                  if ( m_parallax != 0 )
                     info
                        +=           "<br><b>Parallax          :</b> "
                        + String().Format( "%8.2f mas", m_parallax );
                  if ( m_radVel != 0 )
                     info
                        +=           "<br><b>Radial velocity   :</b> "
                        + String().Format( "%+.4g AU/year", m_radVel );
                  if ( !m_spectralType.IsEmpty() )
                     info
                        +=           "<br><b>Spectral type     :</b> "
                        + m_spectralType;
                  if ( m_vmag < 100 )
                     info
                        +=           "<br><b>V Magnitude       :</b> "
                        + String().Format( "%.4g", m_vmag );
                  info += "<br>";

                  SearchInfo_TextBox.Insert( info );
                  m_valid = true;
                  Get_Button.Enable();
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
// ----------------------------------------------------------------------------

PhotometricColorCalibrationInterface::PhotometricColorCalibrationInterface() :
   m_instance( ThePhotometricColorCalibrationProcess )
{
   ThePhotometricColorCalibrationInterface = this;
}

// ----------------------------------------------------------------------------

PhotometricColorCalibrationInterface::~PhotometricColorCalibrationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString PhotometricColorCalibrationInterface::Id() const
{
   return "PhotometricColorCalibration";
}

// ----------------------------------------------------------------------------

MetaProcess* PhotometricColorCalibrationInterface::Process() const
{
   return ThePhotometricColorCalibrationProcess;
}

// ----------------------------------------------------------------------------

const char** PhotometricColorCalibrationInterface::IconImageXPM() const
{
   return PhotometricColorCalibrationIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures PhotometricColorCalibrationInterface::Features() const
{
   return InterfaceFeature::Default;
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::ResetInstance()
{
   PhotometricColorCalibrationInstance defaultInstance( ThePhotometricColorCalibrationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool PhotometricColorCalibrationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   PhotometricColorCalibrationProcess::InitializeWhiteReferences();

   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "PhotometricColorCalibration" );
      UpdateControls();
   }

   dynamic = false;
   return &P == ThePhotometricColorCalibrationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* PhotometricColorCalibrationInterface::NewProcess() const
{
   return new PhotometricColorCalibrationInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool PhotometricColorCalibrationInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const PhotometricColorCalibrationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a PhotometricColorCalibration instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool PhotometricColorCalibrationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool PhotometricColorCalibrationInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

#define TARGET_IMAGE             String( "<target image>" )
#define REFERENCE_ID( x )        (x.IsEmpty() ? TARGET_IMAGE : x)
#define BACKGROUND_REFERENCE_ID  REFERENCE_ID( m_instance.p_backgroundReferenceViewId )

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::UpdateControls()
{
   if ( PhotometricColorCalibrationProcess::HasValidWhiteReferences() )
   {
      int itemIndex = PhotometricColorCalibrationProcess::FindWhiteReferenceById( m_instance.p_whiteReferenceId );
      if ( itemIndex < 0 )
      {
         itemIndex = int( PhotometricColorCalibrationProcess::WhiteReferences().Length() );
         if ( GUI->WhiteReference_ComboBox.NumberOfItems() < itemIndex )
            GUI->WhiteReference_ComboBox.AddItem( "<custom reference>" );
      }
      GUI->WhiteReference_ComboBox.SetCurrentItem( itemIndex );
   }

   int itemIndex = -1;
   for ( size_type i = 0; i < s_servers.Length(); ++i )
      if ( m_instance.p_serverURL == s_servers[i].url )
      {
         itemIndex = int( i );
         break;
      }
   if ( itemIndex < 0 )
   {
      itemIndex = int( s_servers.Length() );
      if ( GUI->Server_ComboBox.NumberOfItems() < itemIndex+1 )
         GUI->Server_ComboBox.AddItem( "<custom server>" );
   }
   GUI->Server_ComboBox.SetCurrentItem( itemIndex );
   GUI->Server_ComboBox.SetToolTip( m_instance.p_serverURL );

   GUI->ApplyColorCalibration_CheckBox.SetChecked( m_instance.p_applyCalibration );

   GUI->RA_Edit.SetText( RAToString( m_instance.p_centerRA ) );

   GUI->Dec_Edit.SetText( DecToString( m_instance.p_centerDec ) );

   GUI->ShowComplexAngles_CheckBox.SetChecked( m_showComplexAngles );

   GUI->RAInTimeUnits_CheckBox.SetChecked( m_raInTimeUnits );

   GUI->Epoch_Edit.SetText( EpochToString( m_instance.p_epochJD ) );

   GUI->FocalLength_NumericEdit.SetValue( m_instance.p_focalLength );
   GUI->PixelSize_NumericEdit.SetValue( m_instance.p_pixelSize );

   GUI->SolverAutoCatalog_CheckBox.SetChecked( m_instance.p_solverAutoCatalog );

   itemIndex = -1;
   for ( size_type i = 0; i < s_solverCatalogs.Length(); ++i )
      if ( m_instance.p_solverCatalogName == s_solverCatalogs[i].name )
      {
         itemIndex = int( i );
         break;
      }
   if ( itemIndex < 0 )
   {
      itemIndex = int( s_solverCatalogs.Length() );
      if ( GUI->SolverCatalog_ComboBox.NumberOfItems() < itemIndex+1 )
         GUI->SolverCatalog_ComboBox.AddItem( "<custom catalog>" );
   }
   else
      GUI->SolverCatalog_ComboBox.SetToolTip( s_solverCatalogs[itemIndex].description );
   GUI->SolverCatalog_ComboBox.SetCurrentItem( itemIndex );

   GUI->SolverCatalog_Label.Enable( !m_instance.p_solverAutoCatalog );
   GUI->SolverCatalog_ComboBox.Enable( !m_instance.p_solverAutoCatalog );

   GUI->SolverAutoLimitMagnitude_CheckBox.SetChecked( m_instance.p_solverAutoLimitMagnitude );

   GUI->SolverLimitMagnitude_SpinBox.SetValue( m_instance.p_solverLimitMagnitude );

   GUI->SolverLimitMagnitude_Label.Enable( !m_instance.p_solverAutoLimitMagnitude );
   GUI->SolverLimitMagnitude_SpinBox.Enable( !m_instance.p_solverAutoLimitMagnitude );

   GUI->DistortionCorrection_CheckBox.SetChecked( m_instance.p_solverDistortionCorrection );

   GUI->ForcePlateSolve_CheckBox.SetChecked( m_instance.p_forcePlateSolve );

   GUI->IgnorePositionAndScale_CheckBox.Enable( m_instance.p_forcePlateSolve );
   GUI->IgnorePositionAndScale_CheckBox.SetChecked( m_instance.p_ignoreImagePositionAndScale );

   GUI->ProjectionSystem_ComboBox.SetCurrentItem( m_instance.p_solverProjection );

   GUI->StarSensitivity_NumericControl.SetValue( m_instance.p_solverStarSensitivity );

   GUI->NoiseReduction_SpinBox.SetValue( m_instance.p_solverNoiseLayers );

   GUI->AlignmentDevice_ComboBox.SetCurrentItem( m_instance.p_solverAlignmentDevice );

   GUI->SplineSmoothing_NumericControl.SetValue( m_instance.p_solverSplineSmoothing );

   itemIndex = -1;
   for ( size_type i = 0; i < s_photometryCatalogs.Length(); ++i )
      if ( m_instance.p_photCatalogName == s_photometryCatalogs[i].name )
      {
         itemIndex = int( i );
         break;
      }
   if ( itemIndex < 0 )
   {
      itemIndex = int( s_photometryCatalogs.Length() );
      if ( GUI->PhotometryCatalog_ComboBox.NumberOfItems() < itemIndex+1 )
         GUI->PhotometryCatalog_ComboBox.AddItem( "<custom catalog>" );
   }
   else
      GUI->PhotometryCatalog_ComboBox.SetToolTip( s_photometryCatalogs[itemIndex].description );
   GUI->PhotometryCatalog_ComboBox.SetCurrentItem( itemIndex );

   GUI->PhotometryAutoLimitMagnitude_CheckBox.SetChecked( m_instance.p_photAutoLimitMagnitude );

   GUI->PhotometryLimitMagnitude_SpinBox.SetValue( m_instance.p_photLimitMagnitude );

   GUI->PhotometryLimitMagnitude_Label.Enable( !m_instance.p_photAutoLimitMagnitude );
   GUI->PhotometryLimitMagnitude_SpinBox.Enable( !m_instance.p_photAutoLimitMagnitude );

   GUI->AutoAperture_CheckBox.SetChecked( m_instance.p_photAutoAperture );

   GUI->Aperture_Label.Enable( !m_instance.p_photAutoAperture );

   GUI->Aperture_SpinBox.SetValue( m_instance.p_photAperture );
   GUI->Aperture_SpinBox.Enable( !m_instance.p_photAutoAperture );

   GUI->SaturationThreshold_NumericControl.SetValue( m_instance.p_photSaturationThreshold );

   GUI->UsePSFPhotometry_CheckBox.SetChecked( m_instance.p_photUsePSF );

   GUI->ShowDetectedStars_CheckBox.SetChecked( m_instance.p_photShowDetectedStars );

   GUI->ShowBackgroundModels_CheckBox.SetChecked( m_instance.p_photShowBackgroundModels );

   GUI->GenerateGraphs_CheckBox.SetChecked( m_instance.p_photGenerateGraphs );

   GUI->BackgroundReference_SectionBar.SetChecked( m_instance.p_neutralizeBackground );

   GUI->BackgroundReference_Control.Enable( m_instance.p_applyCalibration );

   GUI->BackgroundReferenceView_Edit.SetText( BACKGROUND_REFERENCE_ID );

   GUI->BackgroundLow_NumericControl.SetValue( m_instance.p_backgroundLow );

   GUI->BackgroundHigh_NumericControl.SetValue( m_instance.p_backgroundHigh );

   GUI->BackgroundROI_GroupBox.SetChecked( m_instance.p_backgroundUseROI );

   GUI->BackgroundROIX0_SpinBox.SetValue( m_instance.p_backgroundROI.x0 );

   GUI->BackgroundROIY0_SpinBox.SetValue( m_instance.p_backgroundROI.y0 );

   GUI->BackgroundROIWidth_SpinBox.SetValue( m_instance.p_backgroundROI.Width() );

   GUI->BackgroundROIHeight_SpinBox.SetValue( m_instance.p_backgroundROI.Height() );
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::AcquireMetadata( const ImageWindow& window )
{
   if ( window.IsNull() )
      return;

   FITSKeywordArray keywords = window.Keywords();
   IsoStringList foundKeywords;
   for ( const FITSHeaderKeyword& keyword : keywords )
   {
      String value = keyword.StripValueDelimiters();

      if ( keyword.name.CompareIC( "FOCALLEN" ) == 0 )
      {
         if ( !value.TryToFloat( m_instance.p_focalLength ) )
            continue;
      }
      else if ( keyword.name.CompareIC( "XPIXSZ" ) == 0 )
      {
         if ( !value.TryToFloat( m_instance.p_pixelSize ) )
            continue;
      }
      else if ( keyword.name.CompareIC( "OBJCTRA" ) == 0 )
      {
         if ( !value.TrySexagesimalToDouble( m_instance.p_centerRA, ' '/*separator*/ ) )
            continue;
         m_instance.p_centerRA *= 15;
      }
      else if ( keyword.name.CompareIC( "OBJCTDEC" ) == 0 )
      {
         if ( !value.TrySexagesimalToDouble( m_instance.p_centerDec, ' '/*separator*/ ) )
            continue;
      }
      else if ( keyword.name.CompareIC( "DATE-OBS" ) == 0 )
      {
         int year, month, day;
         double dayf, tz;
         if ( !value.TryParseISO8601DateTime( year, month, day, dayf, tz ) )
            continue;
         m_instance.p_epochJD = ComplexTimeToJD( year, month, day, dayf - tz/24 );
      }
      else
         continue;

      foundKeywords << keyword.name;
   }

   if ( foundKeywords.IsEmpty() )
   {
      MessageBox( "<p>The active image has no position or acquisition metadata.</p>",
                  "PhotometricColorCalibration", StdIcon::Error, StdButton::Ok ).Execute();
      return;
   }

   UpdateControls();

   MessageBox( "<p>The following metadata items have been retrieved from the active image:</p>"
               "<p>" + String().ToCommaSeparated( foundKeywords ) + "</p>",
               "PhotometricColorCalibration", StdIcon::Information, StdButton::Ok ).Execute();
}

// ----------------------------------------------------------------------------

String PhotometricColorCalibrationInterface::RAToString( double ra ) const
{
   if ( m_raInTimeUnits )
      ra /= 15;

   if ( m_showComplexAngles )
      return String::ToSexagesimal( ra, SexagesimalConversionOptions( 3/*items*/,
                                                                      3/*precision*/,
                                                                      false/*sign*/,
                                                                      0/*width*/,
                                                                      ' '/*separator*/ ) );
   return String().Format( "%.8g", ra );
}

String PhotometricColorCalibrationInterface::DecToString( double deg ) const
{
   if ( m_showComplexAngles )
      return String::ToSexagesimal( deg, SexagesimalConversionOptions( 3/*items*/,
                                                                       2/*precision*/,
                                                                       true/*sign*/,
                                                                       0/*width*/,
                                                                       ' '/*separator*/ ) );
   return String().Format( "%+.7g", deg );
}

String PhotometricColorCalibrationInterface::EpochToString( double jd ) const
{
   return TimePoint( jd ).ToString( ISO8601ConversionOptions( 3/*timeItems*/,
                                                              0/*precision*/ ) );

}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::e_GetFocus( Control& sender )
{
   if ( sender == GUI->BackgroundReferenceView_Edit )
   {
      Edit* e = dynamic_cast<Edit*>( &sender );
      if ( e != nullptr ) // ?!
         if ( e->Text() == TARGET_IMAGE )
            e->Clear();
   }
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::e_EditCompleted( Edit& sender )
{
   String text = sender.Text().Trimmed();

   if ( sender == GUI->RA_Edit )
   {
      if ( !text.IsEmpty() )
      {
         try
         {
            double deg = text.SexagesimalToDouble( ' '/*separator*/ );
            if ( m_raInTimeUnits )
               deg *= 15;
            while ( deg < 0 )
               deg += 360;
            while ( deg > 360 )
               deg -= 360;
            m_instance.p_centerRA = deg;
         }
         ERROR_HANDLER
      }
      sender.SetText( RAToString( m_instance.p_centerRA ) );
   }
   else if ( sender == GUI->Dec_Edit )
   {
      if ( !text.IsEmpty() )
      {
         try
         {
            m_instance.p_centerDec = Range( text.SexagesimalToDouble( ' '/*separator*/ ), -90.0, +90.0 );
         }
         ERROR_HANDLER
      }
      sender.SetText( DecToString( m_instance.p_centerDec ) );
   }
   else if ( sender == GUI->Epoch_Edit )
   {
      if ( !text.IsEmpty() )
      {
         int year, month = 1, day = 1;
         double dayf = 0, tz = 0;
         if ( text.TryParseISO8601DateTime( year, month, day, dayf, tz ) )
            m_instance.p_epochJD = ComplexTimeToJD( year, month, day, dayf - tz/24 );
         else
         {
            try
            {
               StringList tokens;
               text.Break( tokens, ' ', true/*trim*/ );
               tokens.Remove( String() );
               switch ( tokens.Length() )
               {
               case 3:
                  dayf = tokens[2].ToDouble();
                  dayf -= (day = TruncInt( dayf ));
               case 2:
                  month = tokens[1].ToInt();
               case 1:
                  year = tokens[0].ToInt();
                  break;
               default:
                  throw Error( "Too many date items." );
               }
               m_instance.p_epochJD = ComplexTimeToJD( year, month, day, dayf );
            }
            ERROR_HANDLER
         }
      }
      sender.SetText( EpochToString( m_instance.p_epochJD ) );
   }
   else if ( sender == GUI->BackgroundReferenceView_Edit )
   {
      try
      {
         if ( text == TARGET_IMAGE )
            text.Clear();
         if ( !text.IsEmpty() )
            if ( !View::IsValidViewId( text ) )
               throw Error( "Invalid view identifier: " + text );
         m_instance.p_backgroundReferenceViewId = text;
         sender.SetText( BACKGROUND_REFERENCE_ID );
      }
      catch ( ... )
      {
         sender.SetText( BACKGROUND_REFERENCE_ID );
         try
         {
            throw;
         }
         ERROR_HANDLER
         sender.SelectAll();
         sender.Focus();
      }
   }
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->WhiteReference_ComboBox )
   {
      if ( itemIndex >= 0 )
         if ( size_type( itemIndex ) < PhotometricColorCalibrationProcess::WhiteReferences().Length() )
         {
            const WhiteReference& W = PhotometricColorCalibrationProcess::WhiteReferences()[itemIndex];
            m_instance.p_whiteReferenceId = W.id;
            m_instance.p_whiteReferenceName = W.name;
            m_instance.p_whiteReferenceJB_JV = W.JB_JV;
            m_instance.p_whiteReferenceSr_JV = W.Sr_JV;
            m_instance.p_zeroPointJB_JV = PhotometricColorCalibrationProcess::ZeroPoint().JB_JV;
            m_instance.p_zeroPointSr_JV = PhotometricColorCalibrationProcess::ZeroPoint().Sr_JV;
         }
   }
   else if ( sender == GUI->Server_ComboBox )
   {
      if ( itemIndex >= 0 )
         if ( size_type( itemIndex ) < s_servers.Length() )
            m_instance.p_serverURL = s_servers[itemIndex].url;
      GUI->Server_ComboBox.SetToolTip( m_instance.p_serverURL );
   }
   else if ( sender == GUI->SolverCatalog_ComboBox )
   {
      if ( itemIndex >= 0 )
         if ( size_type( itemIndex ) < s_solverCatalogs.Length() )
         {
            m_instance.p_solverCatalogName = s_solverCatalogs[itemIndex].name;
            GUI->SolverCatalog_ComboBox.SetToolTip( s_solverCatalogs[itemIndex].description );
         }
   }
   else if ( sender == GUI->ProjectionSystem_ComboBox )
   {
      m_instance.p_solverProjection = itemIndex;
   }
   else if ( sender == GUI->AlignmentDevice_ComboBox )
   {
      m_instance.p_solverAlignmentDevice = itemIndex;
   }
   else if ( sender == GUI->PhotometryCatalog_ComboBox )
   {
      if ( itemIndex >= 0 )
         if ( size_type( itemIndex ) < s_photometryCatalogs.Length() )
         {
            m_instance.p_photCatalogName = s_photometryCatalogs[itemIndex].name;
            GUI->PhotometryCatalog_ComboBox.SetToolTip( s_photometryCatalogs[itemIndex].description );
         }
   }
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::e_Click( Button& sender, bool checked )
{
   if ( sender == GUI->ApplyColorCalibration_CheckBox )
   {
      m_instance.p_applyCalibration = checked;
      UpdateControls();
   }
   else if ( sender == GUI->ShowComplexAngles_CheckBox )
   {
      m_showComplexAngles = checked;
      UpdateControls();
   }
   else if ( sender == GUI->RAInTimeUnits_CheckBox )
   {
      m_raInTimeUnits = checked;
      UpdateControls();
   }
   else if ( sender == GUI->SearchCoordinates_Button )
   {
      if ( m_searchDialog == nullptr )
         m_searchDialog = new CoordinateSearchDialog;
      if ( m_searchDialog->Execute() )
         if ( m_searchDialog->HasValidCoordinates() )
         {
            m_instance.p_centerRA = m_searchDialog->RA();
            m_instance.p_centerDec = m_searchDialog->Dec();
            UpdateControls();
         }
   }
   else if ( sender == GUI->AcquireFromImage_Button )
   {
      ImageWindow window = ImageWindow::ActiveWindow();
      if ( window.IsNull() )
      {
         MessageBox( "<p>There is no active image window.</p>",
                     "PhotometricColorCalibration", StdIcon::Error, StdButton::Ok ).Execute();
         return;
      }
      AcquireMetadata( window );
   }
   else if ( sender == GUI->SolverAutoCatalog_CheckBox )
   {
      m_instance.p_solverAutoCatalog = checked;
      UpdateControls();
   }
   else if ( sender == GUI->SolverAutoLimitMagnitude_CheckBox )
   {
      m_instance.p_solverAutoLimitMagnitude = checked;
      UpdateControls();
   }
   else if ( sender == GUI->DistortionCorrection_CheckBox )
   {
      m_instance.p_solverDistortionCorrection = checked;
      UpdateControls();
   }
   else if ( sender == GUI->ForcePlateSolve_CheckBox )
   {
      m_instance.p_forcePlateSolve = checked;
      UpdateControls();
   }
   else if ( sender == GUI->IgnorePositionAndScale_CheckBox )
   {
      m_instance.p_ignoreImagePositionAndScale = checked;
   }
   else if ( sender == GUI->ResetSolverConfiguration_ToolButton )
   {
      if ( MessageBox( "<p>This will reset the entire configuration of the ImageSolver script to factory-default settings.</p>"
                       "<p><b>Are you sure?</b></p>",
                       "PhotometricColorCalibration",
                       StdIcon::Warning,
                       StdButton::No, StdButton::Yes ).Execute() == StdButton::Yes )
      {
         String scriptPath = PixInsightSettings::GlobalString( "Application/SrcDirectory" ) + "/scripts/AdP/ImageSolver.js";
         Console().Show();
         Console().ExecuteScript( scriptPath, StringKeyValueList() << StringKeyValue( "resetSettingsAndExit", "true" ) );
      }
   }
   else if ( sender == GUI->PhotometryAutoLimitMagnitude_CheckBox )
   {
      m_instance.p_photAutoLimitMagnitude = checked;
      UpdateControls();
   }
   else if ( sender == GUI->AutoAperture_CheckBox )
   {
      m_instance.p_photAutoAperture = checked;
      UpdateControls();
   }
   else if ( sender == GUI->UsePSFPhotometry_CheckBox )
   {
      m_instance.p_photUsePSF = checked;
      UpdateControls();
   }
   else if ( sender == GUI->ShowDetectedStars_CheckBox )
   {
      m_instance.p_photShowDetectedStars = checked;
   }
   else if ( sender == GUI->ShowBackgroundModels_CheckBox )
   {
      m_instance.p_photShowBackgroundModels = checked;
   }
   else if ( sender == GUI->GenerateGraphs_CheckBox )
   {
      m_instance.p_photGenerateGraphs = checked;
      if ( checked )
      {
         if ( !ThePhotometricColorCalibrationGraphInterface->IsVisible() )
            ThePhotometricColorCalibrationGraphInterface->Launch();
      }
   }
   else if ( sender == GUI->ResetPhotometryConfiguration_ToolButton )
   {
      if ( MessageBox( "<p>This will reset the entire configuration of the Photometry script to factory-default settings.</p>"
                       "<p><b>Are you sure?</b></p>",
                       "PhotometricColorCalibration",
                       StdIcon::Warning,
                       StdButton::No, StdButton::Yes ).Execute() == StdButton::Yes )
      {
         String scriptPath = PixInsightSettings::GlobalString( "Application/SrcDirectory" ) + "/scripts/AdP/AperturePhotometry.js";
         Console().Show();
         Console().ExecuteScript( scriptPath, StringKeyValueList() << StringKeyValue( "resetSettingsAndExit", "true" ) );
      }
   }
   else if ( sender == GUI->BackgroundReferenceView_ToolButton )
   {
      ViewSelectionDialog d( m_instance.p_backgroundReferenceViewId );
      if ( d.Execute() == StdDialogCode::Ok )
      {
         m_instance.p_backgroundReferenceViewId = d.Id();
         GUI->BackgroundReferenceView_Edit.SetText( BACKGROUND_REFERENCE_ID );
      }
   }
   else if ( sender == GUI->BackgroundROISelectPreview_Button )
   {
      PreviewSelectionDialog d;
      if ( d.Execute() == StdDialogCode::Ok )
         if ( !d.Id().IsEmpty() )
         {
            View view = View::ViewById( d.Id() );
            if ( !view.IsNull() )
            {
               m_instance.p_backgroundROI = view.Window().PreviewRect( view.Id() );
               UpdateControls();
            }
         }
   }
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::e_SectionCheck( SectionBar& sender, bool checked )
{
   if ( sender == GUI->BackgroundReference_SectionBar )
      m_instance.p_neutralizeBackground = checked;
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::e_GroupCheck( GroupBox& sender, bool checked )
{
   if ( sender == GUI->BackgroundROI_GroupBox )
      m_instance.p_backgroundUseROI = checked;
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::e_EditValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->FocalLength_NumericEdit )
   {
      m_instance.p_focalLength = value;
   }
   else if ( sender == GUI->PixelSize_NumericEdit )
   {
      m_instance.p_pixelSize = value;
   }
   else if ( sender == GUI->SplineSmoothing_NumericControl )
   {
      m_instance.p_solverSplineSmoothing = value;
   }
   else if ( sender == GUI->StarSensitivity_NumericControl )
   {
      m_instance.p_solverStarSensitivity = value;
   }
   else if ( sender == GUI->SaturationThreshold_NumericControl )
   {
      m_instance.p_photSaturationThreshold = value;
   }
   else if ( sender == GUI->BackgroundLow_NumericControl )
   {
      m_instance.p_backgroundLow = value;
      if ( m_instance.p_backgroundHigh < m_instance.p_backgroundLow )
         GUI->BackgroundHigh_NumericControl.SetValue( m_instance.p_backgroundHigh = m_instance.p_backgroundLow );
   }
   else if ( sender == GUI->BackgroundHigh_NumericControl )
   {
      m_instance.p_backgroundHigh = value;
      if ( m_instance.p_backgroundHigh < m_instance.p_backgroundLow )
         GUI->BackgroundHigh_NumericControl.SetValue( m_instance.p_backgroundLow = m_instance.p_backgroundHigh );
   }
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::e_SpinValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->SolverLimitMagnitude_SpinBox )
   {
      m_instance.p_solverLimitMagnitude = value;
   }
   else if ( sender == GUI->NoiseReduction_SpinBox )
   {
      m_instance.p_solverNoiseLayers = value;
   }
   else if ( sender == GUI->PhotometryLimitMagnitude_SpinBox )
   {
      m_instance.p_photLimitMagnitude = value;
   }
   else if ( sender == GUI->Aperture_SpinBox )
   {
      m_instance.p_photAperture = value;
   }
   else if ( sender == GUI->BackgroundROIX0_SpinBox )
   {
      m_instance.p_backgroundROI.x0 = value;
   }
   else if ( sender == GUI->BackgroundROIY0_SpinBox )
   {
      m_instance.p_backgroundROI.y0 = value;
   }
   else if ( sender == GUI->BackgroundROIWidth_SpinBox )
   {
      m_instance.p_backgroundROI.x1 = m_instance.p_backgroundROI.x0 + value;
   }
   else if ( sender == GUI->BackgroundROIHeight_SpinBox )
   {
      m_instance.p_backgroundROI.y1 = m_instance.p_backgroundROI.y0 + value;
   }
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::e_ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->ImageParameters_Control || sender == GUI->AcquireFromImage_Button || sender == GUI->BackgroundReferenceView_Edit )
      wantsView = true;
   else if ( sender == GUI->BackgroundROI_GroupBox || sender == GUI->BackgroundROISelectPreview_Button )
      wantsView = view.IsPreview();
}

// ----------------------------------------------------------------------------

void PhotometricColorCalibrationInterface::e_ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->ImageParameters_Control || sender == GUI->AcquireFromImage_Button )
   {
      AcquireMetadata( view.Window() );
   }
   else if ( sender == GUI->BackgroundReferenceView_Edit )
   {
      m_instance.p_backgroundReferenceViewId = view.FullId();
      GUI->BackgroundReferenceView_Edit.SetText( BACKGROUND_REFERENCE_ID );
   }
   else if ( sender == GUI->BackgroundROI_GroupBox || sender == GUI->BackgroundROISelectPreview_Button )
   {
      if ( view.IsPreview() )
      {
         m_instance.p_backgroundUseROI = true;
         m_instance.p_backgroundROI = view.Window().PreviewRect( view.Id() );
         UpdateControls();
      }
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

PhotometricColorCalibrationInterface::GUIData::GUIData( PhotometricColorCalibrationInterface& w )
{
   PhotometricColorCalibrationProcess::InitializeWhiteReferences();
   InitializeData();

   //

   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( "Saturation threshold:m" );
   int labelWidth2 = fnt.Width( "Height:m" );
   int editWidth1 = fnt.Width( "2000-01-01T12:00:00Zmm" );
   int editWidth2 = fnt.Width( "9999999999m" );
   int editWidth3 = fnt.Width( "9999999m" );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   const char* whiteReferenceToolTip = "<p>Reference object to calculate white balancing functions.</p>"
      "<p>Hot (blue) references, such as B and A stars, tend to induce a red color cast. "
      "Cold (red) references, such as K and M stars, tend to cause a blue color cast. The default average spiral galaxy "
      "reference is, in our opinion, truly representative of the deep space, and hence the most plausible white "
      "reference for most applications.</p>";

   WhiteReference_Label.SetText( "White reference:" );
   WhiteReference_Label.SetFixedWidth( labelWidth1 );
   WhiteReference_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   WhiteReference_Label.SetToolTip( whiteReferenceToolTip );

   if ( PhotometricColorCalibrationProcess::HasValidWhiteReferences() )
   {
      for ( const WhiteReference& ref : PhotometricColorCalibrationProcess::WhiteReferences() )
         WhiteReference_ComboBox.AddItem( ref.name );
      WhiteReference_ComboBox.SetToolTip( whiteReferenceToolTip );
      WhiteReference_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&PhotometricColorCalibrationInterface::e_ItemSelected, w );

      WhiteReference_Sizer.SetSpacing( 4 );
      WhiteReference_Sizer.Add( WhiteReference_Label );
      WhiteReference_Sizer.Add( WhiteReference_ComboBox );
      WhiteReference_Sizer.AddStretch();
   }
   else
   {
      WhiteReference_Label.Hide();
      WhiteReference_ComboBox.Hide();
   }

   //

   const char* serverToolTip = "<p>PhotometricColorCalibration, as well as the scripts that it uses internally, "
      "depend on the VizieR service to access online astronomical catalogs and databases. This option allows you "
      "to select the best VizieR mirror server, typically the closest one to your location.</p>";

   Server_Label.SetText( "Database server:" );
   Server_Label.SetFixedWidth( labelWidth1 );
   Server_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Server_Label.SetToolTip( serverToolTip );

   for ( const ServerData& server : s_servers )
      Server_ComboBox.AddItem( server.name );
   Server_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&PhotometricColorCalibrationInterface::e_ItemSelected, w );

   Server_Sizer.SetSpacing( 4 );
   Server_Sizer.Add( Server_Label );
   Server_Sizer.Add( Server_ComboBox );
   Server_Sizer.AddStretch();

   //

   ApplyColorCalibration_CheckBox.SetText( "Apply color calibration" );
   ApplyColorCalibration_CheckBox.SetToolTip( "<p>Disable this option to compute color calibration functions "
      "and scaling factors, but without applying them to the target image. This option should normally be enabled.</p>" );
   ApplyColorCalibration_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   ApplyColorCalibration_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ApplyColorCalibration_Sizer.Add( ApplyColorCalibration_CheckBox );
   ApplyColorCalibration_Sizer.AddStretch();

   //

   ImageParameters_SectionBar.SetTitle( "Image Parameters" );
   ImageParameters_SectionBar.SetSection( ImageParameters_Control );

   //

   const char* raToolTip = "<p>Approximate right ascension coordinate of the center of the image.</p>"
      "<p>Does not need to be accurate, but should not be off by more than a 50% with respect to the area covered by "
      "the image. If the target image already has its center position stored in metadata, it will be used instead of "
      "this value, unless the <i>ignore existing metadata</i> option is enabled.</p>"
      "<p>You can write an explicit right ascension coordinate as a space-separated list of hours (or degrees, if <i>R.A. "
      "in time units</i> is unchecked), minutes and seconds. The minutes and seconds items are optional, and you can "
      "specify a fractional part for the last item. Alternatively, you can search for object coordinates online by "
      "clicking the <i>Search Coordinates</i> button.</p>";

   RA_Label.SetText( "Right ascension:" );
   RA_Label.SetFixedWidth( labelWidth1 );
   RA_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   RA_Label.SetToolTip( raToolTip );

   RA_Edit.SetToolTip( raToolTip );
   RA_Edit.SetMinWidth( editWidth1 );
   RA_Edit.OnEditCompleted( (Edit::edit_event_handler)&PhotometricColorCalibrationInterface::e_EditCompleted, w );

   RA_Sizer.SetSpacing( 4 );
   RA_Sizer.Add( RA_Label );
   RA_Sizer.Add( RA_Edit, 100 );
   RA_Sizer.AddStretch();

   //

   const char* decToolTip = "<p>Approximate declination coordinate of the center of the image.</p>"
      "<p>Does not need to be accurate, but should not be off by more than a 50% with respect to the area covered by "
      "the image. If the target image already has its center position stored in metadata, it will be used instead of "
      "this value, unless the <i>ignore existing metadata</i> option is enabled.</p>"
      "<p>You can write an explicit declination coordinate as a space-separated list of degrees, minutes and seconds. "
      "The minutes and seconds items are optional, and you can specify a fractional part for the last item. A plus or minus "
      "sign character is optional before the first degrees item, and obviously necessary for Southern declinations. "
      "Alternatively, you can search for object coordinates online by clicking the <i>Search Coordinates</i> button.</p>";

   Dec_Label.SetText( "Declination:" );
   Dec_Label.SetFixedWidth( labelWidth1 );
   Dec_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Dec_Label.SetToolTip( decToolTip );

   Dec_Edit.SetToolTip( decToolTip );
   Dec_Edit.SetMinWidth( editWidth1 );
   Dec_Edit.OnEditCompleted( (Edit::edit_event_handler)&PhotometricColorCalibrationInterface::e_EditCompleted, w );

   Dec_Sizer.SetSpacing( 4 );
   Dec_Sizer.Add( Dec_Label );
   Dec_Sizer.Add( Dec_Edit, 100 );
   Dec_Sizer.AddStretch();

   //

   ShowComplexAngles_CheckBox.SetText( "Show complex angles" );
   ShowComplexAngles_CheckBox.SetToolTip( "<p>If enabled, coordinates will be shown as complex angular quantities "
      "(degrees/hours, minutes, and seconds). If disabled, coordinates will be shown as scalars (degrees or hours with decimals).</p>" );
   ShowComplexAngles_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   ShowComplexAngles_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ShowComplexAngles_Sizer.Add( ShowComplexAngles_CheckBox );
   ShowComplexAngles_Sizer.AddStretch();

   //

   RAInTimeUnits_CheckBox.SetText( "R.A. in time units" );
   RAInTimeUnits_CheckBox.SetToolTip( "<p>When enabled, the right ascension coordinate will be shown expressed in hours, "
      "minutes, and seconds. When disabled, the right ascension will be expressed in degrees, arcminutes and arcseconds.</p>" );
   RAInTimeUnits_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   RAInTimeUnits_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   RAInTimeUnits_Sizer.Add( RAInTimeUnits_CheckBox );
   RAInTimeUnits_Sizer.AddStretch();

   //

   const char* epochToolTip = "<p>The date the image was acquired. This parameter is used to compute proper motions of stars, "
      "in order to compare the actual positions recorded in the image with the corresponding positions stored in a catalog. "
      "This date is also used to compute apparent star positions, when appropriate.</p>";

   Epoch_Label.SetText( "Observation date:" );
   Epoch_Label.SetFixedWidth( labelWidth1 );
   Epoch_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Epoch_Label.SetToolTip( epochToolTip );

   Epoch_Edit.SetToolTip( epochToolTip );
   Epoch_Edit.SetMinWidth( editWidth1 );
   Epoch_Edit.OnEditCompleted( (Edit::edit_event_handler)&PhotometricColorCalibrationInterface::e_EditCompleted, w );

   Epoch_Sizer.SetSpacing( 4 );
   Epoch_Sizer.Add( Epoch_Label );
   Epoch_Sizer.Add( Epoch_Edit, 100 );
   Epoch_Sizer.AddStretch();

   //

   FocalLength_NumericEdit.SetReal();
   FocalLength_NumericEdit.SetPrecision( 2 );
   FocalLength_NumericEdit.EnableFixedPrecision();
   FocalLength_NumericEdit.SetRange( ThePCCFocalLengthParameter->MinimumValue(), ThePCCFocalLengthParameter->MaximumValue() );
   FocalLength_NumericEdit.label.SetText( "Focal length:" );
   FocalLength_NumericEdit.label.SetFixedWidth( labelWidth1 );
   FocalLength_NumericEdit.edit.SetFixedWidth( editWidth2 );
   FocalLength_NumericEdit.sizer.AddStretch();
   FocalLength_NumericEdit.SetToolTip( "<p>Approximate effective focal length in millimeters. Does not need to be accurate, "
      "but should not be wrong by more than a 50%.</p>" );
   FocalLength_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&PhotometricColorCalibrationInterface::e_EditValueUpdated, w );

   //

   PixelSize_NumericEdit.SetReal();
   PixelSize_NumericEdit.SetPrecision( 2 );
   PixelSize_NumericEdit.EnableFixedPrecision();
   PixelSize_NumericEdit.SetRange( ThePCCPixelSizeParameter->MinimumValue(), ThePCCPixelSizeParameter->MaximumValue() );
   PixelSize_NumericEdit.label.SetText( "Pixel size:" );
   PixelSize_NumericEdit.label.SetFixedWidth( labelWidth1 );
   PixelSize_NumericEdit.edit.SetFixedWidth( editWidth2 );
   PixelSize_NumericEdit.sizer.AddStretch();
   PixelSize_NumericEdit.SetToolTip( "<p>Approximate pixel size in micrometers, including pixel binning, if applicable. "
      "Does not need to be accurate, but should not be wrong by more than a 50%.</p>" );
   PixelSize_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&PhotometricColorCalibrationInterface::e_EditValueUpdated, w );

   //

   ImageParametersLeft_Sizer.SetSpacing( 4 );
   ImageParametersLeft_Sizer.Add( RA_Sizer );
   ImageParametersLeft_Sizer.Add( Dec_Sizer );
   ImageParametersLeft_Sizer.Add( ShowComplexAngles_Sizer );
   ImageParametersLeft_Sizer.Add( RAInTimeUnits_Sizer );
   ImageParametersLeft_Sizer.Add( Epoch_Sizer );
   ImageParametersLeft_Sizer.Add( FocalLength_NumericEdit );
   ImageParametersLeft_Sizer.Add( PixelSize_NumericEdit );

   //

   SearchCoordinates_Button.SetText( "Search Coordinates" );
   SearchCoordinates_Button.SetIcon( w.ScaledResource( ":/icons/find.png" ) );
   SearchCoordinates_Button.SetStyleSheet( "QPushButton { text-align: left; }" );
   SearchCoordinates_Button.SetToolTip( "<p>Open the Online Coordinate Search dialog.</p>" );
   SearchCoordinates_Button.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );
   SearchCoordinates_Button.OnViewDrag( (Control::view_drag_event_handler)&PhotometricColorCalibrationInterface::e_ViewDrag, w );
   SearchCoordinates_Button.OnViewDrop( (Control::view_drop_event_handler)&PhotometricColorCalibrationInterface::e_ViewDrop, w );

   //

   AcquireFromImage_Button.SetText( "Acquire from Image" );
   AcquireFromImage_Button.SetIcon( w.ScaledResource( ":/icons/window-export.png" ) );
   AcquireFromImage_Button.SetStyleSheet( "QPushButton { text-align: left; }" );
   AcquireFromImage_Button.SetToolTip( "<p>Acquire coordinates, focal length, pixel dimensions and observation date "
      "from the active image, if the corresponding metadata are available.</p>"
      "<p>You can also drag a view selector directly to this button.</p>" );
   AcquireFromImage_Button.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );
   AcquireFromImage_Button.OnViewDrag( (Control::view_drag_event_handler)&PhotometricColorCalibrationInterface::e_ViewDrag, w );
   AcquireFromImage_Button.OnViewDrop( (Control::view_drop_event_handler)&PhotometricColorCalibrationInterface::e_ViewDrop, w );

   //

   ImageParametersRight_Sizer.SetSpacing( 16 );
   ImageParametersRight_Sizer.AddStretch();
   ImageParametersRight_Sizer.Add( SearchCoordinates_Button );
   ImageParametersRight_Sizer.Add( AcquireFromImage_Button );
   ImageParametersRight_Sizer.AddStretch();

   //

   ImageParameters_Sizer.SetSpacing( 32 );
   ImageParameters_Sizer.Add( ImageParametersLeft_Sizer );
   ImageParameters_Sizer.Add( ImageParametersRight_Sizer );
   ImageParameters_Sizer.AddStretch();

   ImageParameters_Control.SetSizer( ImageParameters_Sizer );
   ImageParameters_Control.AdjustToContents();

   //

   PlateSolverParameters_SectionBar.SetTitle( "Plate Solving Parameters" );
   PlateSolverParameters_SectionBar.SetSection( PlateSolverParameters_Control );

   //

   SolverAutoCatalog_CheckBox.SetText( "Automatic catalog" );
   SolverAutoCatalog_CheckBox.SetToolTip( "<p>When this option is enabled, PhotometricColorCalibration will select an optimal "
      "astrometric catalog for the field of view of the target image. Enabling this option is recommended under normal working conditions.</p>" );
   SolverAutoCatalog_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   SolverAutoCatalog_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SolverAutoCatalog_Sizer.Add( SolverAutoCatalog_CheckBox );
   SolverAutoCatalog_Sizer.AddStretch();

   //

   const char* solverCatalogToolTip = "<p>This is the catalog used to acquire positions and proper motions of stars for "
      "plate solving the target image.</p>"
      "<p>You should select the appropriate catalog for your image. For example, for very wide field images (focal lengths "
      "smaller than 25 mm), you should use the catalog Bright Stars because it provides enough stars and the queries are fast "
      "for large fields. PPMXL and UCAC-3 are useful for very deep images because they include very dim stars. However, the "
      "queries in these two catalogs can be very slow for relatively large fields.</p>";

   SolverCatalog_Label.SetText( "Astrometry catalog:" );
   SolverCatalog_Label.SetFixedWidth( labelWidth1 );
   SolverCatalog_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   SolverCatalog_Label.SetToolTip( solverCatalogToolTip );

   for ( const CatalogData& catalog : s_solverCatalogs )
      SolverCatalog_ComboBox.AddItem( catalog.name );
   SolverCatalog_ComboBox.SetToolTip( solverCatalogToolTip );
   SolverCatalog_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&PhotometricColorCalibrationInterface::e_ItemSelected, w );

   SolverCatalog_Sizer.SetSpacing( 4 );
   SolverCatalog_Sizer.Add( SolverCatalog_Label );
   SolverCatalog_Sizer.Add( SolverCatalog_ComboBox );
   SolverCatalog_Sizer.AddStretch();

   //

   SolverAutoLimitMagnitude_CheckBox.SetText( "Automatic limit magnitude" );
   SolverAutoLimitMagnitude_CheckBox.SetToolTip( "<p>When this option is enabled, PhotometricColorCalibration will select an optimal "
      "limit magnitude for plate solving based on the field of view of the target image. Enabling this option is recommended under "
      "normal working conditions.</p>" );
   SolverAutoLimitMagnitude_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   SolverAutoLimitMagnitude_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   SolverAutoLimitMagnitude_Sizer.Add( SolverAutoLimitMagnitude_CheckBox );
   SolverAutoLimitMagnitude_Sizer.AddStretch();

   //

   const char* solverLimitMagnitudeToolTip = "<p>This parameter limits the number of stars extracted from the catalog to those "
      "with magnitude smaller than this value. This limit magnitude should be similar to the magnitude of the dimmest stars in the image. "
      "If this value is too low, the plate solving algorithm may not have enough stars to compare, but if the value is too high, it will "
      "try to match catalog stars that are not visible on the image. This will slow down the process, and could prevent the algorithm "
      "from finding a valid solution.</p>";

   SolverLimitMagnitude_Label.SetText( "Limit magnitude:" );
   SolverLimitMagnitude_Label.SetFixedWidth( labelWidth1 );
   SolverLimitMagnitude_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   SolverLimitMagnitude_Label.SetToolTip( solverLimitMagnitudeToolTip );

   SolverLimitMagnitude_SpinBox.SetRange( int( ThePCCSolverLimitMagnitudeParameter->MinimumValue() ),
                                          int( ThePCCSolverLimitMagnitudeParameter->MaximumValue() ) );
   SolverLimitMagnitude_SpinBox.SetToolTip( solverLimitMagnitudeToolTip );
//    SolverLimitMagnitude_SpinBox.SetFixedWidth( editWidth1 );
   SolverLimitMagnitude_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   SolverLimitMagnitude_Sizer.SetSpacing( 4 );
   SolverLimitMagnitude_Sizer.Add( SolverLimitMagnitude_Label );
   SolverLimitMagnitude_Sizer.Add( SolverLimitMagnitude_SpinBox );
   SolverLimitMagnitude_Sizer.AddStretch();

   //

   DistortionCorrection_CheckBox.SetText( "Distortion correction" );
   DistortionCorrection_CheckBox.SetToolTip( "<p>This option enables a distortion correction algorithm based on 2-D surface splines, "
      "also known as <i>thin plates</i>. This option should be selected when the target image deviates from an ideal projected image. "
      "This usually happens in images taken with focal lengths shorter than 400 mm.</p>" );
   DistortionCorrection_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   DistortionCorrection_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   DistortionCorrection_Sizer.Add( DistortionCorrection_CheckBox );
   DistortionCorrection_Sizer.AddStretch();

   //

   ForcePlateSolve_CheckBox.SetText( "Force plate solving" );
   ForcePlateSolve_CheckBox.SetToolTip( "<p>If enabled, the target image will be solved even if it contains a valid astrometric "
      "solution, which will be replaced with a newly computed one.</p>" );
   ForcePlateSolve_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   ForcePlateSolve_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ForcePlateSolve_Sizer.Add( ForcePlateSolve_CheckBox );
   ForcePlateSolve_Sizer.AddStretch();

   //

   IgnorePositionAndScale_CheckBox.SetText( "Ignore existing metadata" );
   IgnorePositionAndScale_CheckBox.SetToolTip( "<p>If enabled, existing metadata will be ignored and the values set by the "
      "running instance, that is, on this interface, will be used instead. This includes the right ascension and declination "
      "coordinate of the center of the image, the focal length, pixel size and observation date. If disabled, existing metadata "
      "will take precedence over the values defined by this instance.</p>" );
   IgnorePositionAndScale_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   //

   const char* resetSolverToolTip = "<p>Reset configuration settings for the ImageSolver script.</p>"
      "<p>This is normally not necessary, since scripts manage their own configuration settings internally. However, "
      "sometimes there are configuration parameters that may become wrong or corrupted for a variety of reasons. Use this "
      "option to initialize all working parameters of the ImageSolver script to factory-default values.</p>";

   ResetSolverConfiguration_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/reload.png" ) ) );
   ResetSolverConfiguration_ToolButton.SetScaledFixedSize( 22, 22 );
   ResetSolverConfiguration_ToolButton.SetToolTip( resetSolverToolTip );
   ResetSolverConfiguration_ToolButton.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   //

   IgnorePositionAndScale_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   IgnorePositionAndScale_Sizer.Add( IgnorePositionAndScale_CheckBox );
   IgnorePositionAndScale_Sizer.AddStretch();
   IgnorePositionAndScale_Sizer.Add( ResetSolverConfiguration_ToolButton );

   //

   PlateSolverParameters_Sizer.SetSpacing( 4 );
   PlateSolverParameters_Sizer.Add( SolverAutoCatalog_Sizer );
   PlateSolverParameters_Sizer.Add( SolverCatalog_Sizer );
   PlateSolverParameters_Sizer.Add( SolverAutoLimitMagnitude_Sizer );
   PlateSolverParameters_Sizer.Add( SolverLimitMagnitude_Sizer );
   PlateSolverParameters_Sizer.Add( DistortionCorrection_Sizer );
   PlateSolverParameters_Sizer.Add( ForcePlateSolve_Sizer );
   PlateSolverParameters_Sizer.Add( IgnorePositionAndScale_Sizer );

   PlateSolverParameters_Control.SetSizer( PlateSolverParameters_Sizer );
   PlateSolverParameters_Control.AdjustToContents();

   //

   AdvancedPlateSolverParameters_SectionBar.SetTitle( "Advanced Plate Solving Parameters" );
   AdvancedPlateSolverParameters_SectionBar.SetSection( AdvancedPlateSolverParameters_Control );

   //

   const char* projectionSystemToolTip = "<p>Most images can be solved using the default Gnomonic projection, since it can "
      "approximate how most optical systems project an image on the focal plane. However, some images, especially very "
      "wide-field images (focal lengths less than 10-15 mm), usually require different projections to find a consistent solution. "
      "Several projections are provided besides Gnomonic, which can be more suitable for wide angle images.</p>";

   ProjectionSystem_Label.SetText( "Projection system:" );
   ProjectionSystem_Label.SetFixedWidth( labelWidth1 );
   ProjectionSystem_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ProjectionSystem_Label.SetToolTip( projectionSystemToolTip );

   // N.B.: Items must be coherent with the ImageSolver script.
   ProjectionSystem_ComboBox.AddItem( "Gnomonic" );
   ProjectionSystem_ComboBox.AddItem( "Stereographic" );
   ProjectionSystem_ComboBox.AddItem( "Plate-carrée" );
   ProjectionSystem_ComboBox.AddItem( "Mercator" );
   ProjectionSystem_ComboBox.AddItem( "Hammer-Aitoff" );
   ProjectionSystem_ComboBox.AddItem( "Zenithal equal area" );
   ProjectionSystem_ComboBox.AddItem( "Orthographic" );
   ProjectionSystem_ComboBox.SetToolTip( projectionSystemToolTip );
   ProjectionSystem_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&PhotometricColorCalibrationInterface::e_ItemSelected, w );

   ProjectionSystem_Sizer.SetSpacing( 4 );
   ProjectionSystem_Sizer.Add( ProjectionSystem_Label );
   ProjectionSystem_Sizer.Add( ProjectionSystem_ComboBox );
   ProjectionSystem_Sizer.AddStretch();

   //

   StarSensitivity_NumericControl.label.SetText( "Log(sensitivity):" );
   StarSensitivity_NumericControl.label.SetFixedWidth( labelWidth1 );
   StarSensitivity_NumericControl.slider.SetRange( 0, 50 );
   StarSensitivity_NumericControl.slider.SetScaledMinWidth( 250 );
   StarSensitivity_NumericControl.SetReal();
   StarSensitivity_NumericControl.SetRange( ThePCCSolverStarSensitivityParameter->MinimumValue(), ThePCCSolverStarSensitivityParameter->MaximumValue() );
   StarSensitivity_NumericControl.SetPrecision( ThePCCSolverStarSensitivityParameter->Precision() );
   StarSensitivity_NumericControl.SetToolTip( "<p>Logarithm of the star detection sensitivity. Increase this parameter to "
      "detect less stars. The default value of -1 is normally appropriate for most linear images.</p>" );
   StarSensitivity_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&PhotometricColorCalibrationInterface::e_EditValueUpdated, w );

   //

   const char* noiseReductionToolTip = "<p>Some images have so much noise that the star detection algorithm can mistake "
      "noise for stars. This parameter removes noise from the image in order to improve star detection. This value is the "
      "number of wavelet layers that will be removed for noise reduction. Use 0 for no noise reduction.</p>";

   NoiseReduction_Label.SetText( "Noise reduction:" );
   NoiseReduction_Label.SetFixedWidth( labelWidth1 );
   NoiseReduction_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   NoiseReduction_Label.SetToolTip( noiseReductionToolTip );

   NoiseReduction_SpinBox.SetRange( int( ThePCCSolverNoiseLayersParameter->MinimumValue() ),
                                    int( ThePCCSolverNoiseLayersParameter->MaximumValue() ) );
   NoiseReduction_SpinBox.SetToolTip( noiseReductionToolTip );
//    NoiseReduction_SpinBox.SetFixedWidth( editWidth1 );
   NoiseReduction_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   NoiseReduction_Sizer.SetSpacing( 4 );
   NoiseReduction_Sizer.Add( NoiseReduction_Label );
   NoiseReduction_Sizer.Add( NoiseReduction_SpinBox );
   NoiseReduction_Sizer.AddStretch();

   //

   const char* alignmentDeviceToolTip = "<p>The algorithmic device used for star matching during the initial image "
      "registration step. Triangle similarity works for most images and supports specular transformations, that is, images "
      "mirrored horizontally or vertically. Polygon matching is much more robust and tolerant of distortions, but does not "
      "support mirrored images.</p>";

   AlignmentDevice_Label.SetText( "Alignment device:" );
   AlignmentDevice_Label.SetFixedWidth( labelWidth1 );
   AlignmentDevice_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   AlignmentDevice_Label.SetToolTip( alignmentDeviceToolTip );

   // N.B.: Items must be coherent with the ImageSolver script.
   AlignmentDevice_ComboBox.AddItem( "Triangle similarity" );
   AlignmentDevice_ComboBox.AddItem( "Polygon matching" );
   AlignmentDevice_ComboBox.SetToolTip( alignmentDeviceToolTip );
   AlignmentDevice_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&PhotometricColorCalibrationInterface::e_ItemSelected, w );

   AlignmentDevice_Sizer.SetSpacing( 4 );
   AlignmentDevice_Sizer.Add( AlignmentDevice_Label );
   AlignmentDevice_Sizer.Add( AlignmentDevice_ComboBox );
   AlignmentDevice_Sizer.AddStretch();

   //

   SplineSmoothing_NumericControl.label.SetText( "Spline smoothing:" );
   SplineSmoothing_NumericControl.label.SetFixedWidth( labelWidth1 );
   SplineSmoothing_NumericControl.slider.SetRange( 0, 50 );
   SplineSmoothing_NumericControl.slider.SetScaledMinWidth( 250 );
   SplineSmoothing_NumericControl.SetReal();
   SplineSmoothing_NumericControl.SetRange( ThePCCSolverSplineSmoothingParameter->MinimumValue(), ThePCCSolverSplineSmoothingParameter->MaximumValue() );
   SplineSmoothing_NumericControl.SetPrecision( ThePCCSolverSplineSmoothingParameter->Precision() );
   SplineSmoothing_NumericControl.SetToolTip( "<p>When nonzero, this parameter causes the distortion correction algorithm to "
      "use approximating instead of interpolating surface splines (thin plates). Approximating surface splines are robust to "
      "outliers in the distortion correction model, including uncertainty in star detection, moving asteroids and other varying "
      "image features that can be accidentally interpreted and matched as stars.</p>" );
   SplineSmoothing_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&PhotometricColorCalibrationInterface::e_EditValueUpdated, w );

   //

   AdvancedPlateSolverParameters_Sizer.SetSpacing( 4 );
   AdvancedPlateSolverParameters_Sizer.Add( ProjectionSystem_Sizer );
   AdvancedPlateSolverParameters_Sizer.Add( StarSensitivity_NumericControl );
   AdvancedPlateSolverParameters_Sizer.Add( NoiseReduction_Sizer );
   AdvancedPlateSolverParameters_Sizer.Add( AlignmentDevice_Sizer );
   AdvancedPlateSolverParameters_Sizer.Add( SplineSmoothing_NumericControl );

   AdvancedPlateSolverParameters_Control.SetSizer( AdvancedPlateSolverParameters_Sizer );
   AdvancedPlateSolverParameters_Control.AdjustToContents();

   //

   PhotometryParameters_SectionBar.SetTitle( "Photometry Parameters" );
   PhotometryParameters_SectionBar.SetSection( PhotometryParameters_Control );

   //

   const char* photometryCatalogToolTip = "<p>This is the catalog used to acquire photometric data of stars.</p>"
      "<p>As of releasing this version of the PhotometricColorCalibration tool, the only catalog providing enough data for "
      "the implemented algorithms is the AAVSO Photometric All-Sky Survey (APASS). Future versions will support more "
      "catalogs as they become available and we manage to analyze them and adapt our algorithms as necessary.</p>";

   PhotometryCatalog_Label.SetText( "Photometry catalog:" );
   PhotometryCatalog_Label.SetFixedWidth( labelWidth1 );
   PhotometryCatalog_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   PhotometryCatalog_Label.SetToolTip( photometryCatalogToolTip );

   for ( const CatalogData& catalog : s_photometryCatalogs )
      PhotometryCatalog_ComboBox.AddItem( catalog.name );
   PhotometryCatalog_ComboBox.SetToolTip( photometryCatalogToolTip );
   PhotometryCatalog_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&PhotometricColorCalibrationInterface::e_ItemSelected, w );

   PhotometryCatalog_Sizer.SetSpacing( 4 );
   PhotometryCatalog_Sizer.Add( PhotometryCatalog_Label );
   PhotometryCatalog_Sizer.Add( PhotometryCatalog_ComboBox );
   PhotometryCatalog_Sizer.AddStretch();

   //

   PhotometryAutoLimitMagnitude_CheckBox.SetText( "Automatic limit magnitude" );
   PhotometryAutoLimitMagnitude_CheckBox.SetToolTip( "<p>When this option is enabled, PhotometricColorCalibration will select an optimal "
      "limit magnitude for photometry based on the field of view of the target image. Enabling this option is recommended under normal "
      "working conditions.</p>" );
   PhotometryAutoLimitMagnitude_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   PhotometryAutoLimitMagnitude_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   PhotometryAutoLimitMagnitude_Sizer.Add( PhotometryAutoLimitMagnitude_CheckBox );
   PhotometryAutoLimitMagnitude_Sizer.AddStretch();

   //

   const char* photometryLimitMagnitudeToolTip = "<p>This parameter limits the stars extracted from the photometric catalog to those "
      "with magnitudes smaller than this value. As happens with the limit magnitude for plate solving, this magnitude should be compatible "
      "with the stars represented on the image. Bear in mind however, that the APASS survey is only valid from about 7th magnitude to "
      "about 17th magnitude. Specifying a too high limit magnitude will only slow down the photometric analysis without any benefit.</p>";

   PhotometryLimitMagnitude_Label.SetText( "Limit magnitude:" );
   PhotometryLimitMagnitude_Label.SetFixedWidth( labelWidth1 );
   PhotometryLimitMagnitude_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   PhotometryLimitMagnitude_Label.SetToolTip( photometryLimitMagnitudeToolTip );

   PhotometryLimitMagnitude_SpinBox.SetRange( int( ThePCCPhotLimitMagnitudeParameter->MinimumValue() ),
                                              int( ThePCCPhotLimitMagnitudeParameter->MaximumValue() ) );
   PhotometryLimitMagnitude_SpinBox.SetToolTip( photometryLimitMagnitudeToolTip );
//    PhotometryLimitMagnitude_SpinBox.SetFixedWidth( editWidth1 );
   PhotometryLimitMagnitude_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   PhotometryLimitMagnitude_Sizer.SetSpacing( 4 );
   PhotometryLimitMagnitude_Sizer.Add( PhotometryLimitMagnitude_Label );
   PhotometryLimitMagnitude_Sizer.Add( PhotometryLimitMagnitude_SpinBox );
   PhotometryLimitMagnitude_Sizer.AddStretch();

   //

   AutoAperture_CheckBox.SetText( "Automatic aperture" );
   AutoAperture_CheckBox.SetToolTip( "<p>When this option is enabled, PhotometricColorCalibration will calculate an optimal "
      "photometric aperture based on the characteristics of the selected catalog and the scale of the target image. Enabling this "
      "option is recommended in most cases.</p>" );
   AutoAperture_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   AutoAperture_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   AutoAperture_Sizer.Add( AutoAperture_CheckBox );
   AutoAperture_Sizer.AddStretch();

   //

   const char* minApertureToolTip = "<p>The photometric aperture in pixels.</p>"
      "<p>PhotometricColorCalibration uses a circular aperture shape, so the value of this parameter is the diameter in pixels of the "
      "circular region used to sample star fluxes on the image. This parameter is disabled when the <i>automatic aperture</i> option "
      "is enabled.</p>";

   Aperture_Label.SetText( "Aperture:" );
   Aperture_Label.SetFixedWidth( labelWidth1 );
   Aperture_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Aperture_Label.SetToolTip( minApertureToolTip );

   Aperture_SpinBox.SetRange( int( ThePCCPhotApertureParameter->MinimumValue() ),
                                 int( ThePCCPhotApertureParameter->MaximumValue() ) );
   Aperture_SpinBox.SetToolTip( minApertureToolTip );
//    Aperture_SpinBox.SetFixedWidth( editWidth1 );
   Aperture_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   Aperture_Sizer.SetSpacing( 4 );
   Aperture_Sizer.Add( Aperture_Label );
   Aperture_Sizer.Add( Aperture_SpinBox );
   Aperture_Sizer.AddStretch();

   //

   SaturationThreshold_NumericControl.label.SetText( "Saturation threshold:" );
   SaturationThreshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   SaturationThreshold_NumericControl.slider.SetRange( 0, 200 );
   SaturationThreshold_NumericControl.slider.SetScaledMinWidth( 250 );
   SaturationThreshold_NumericControl.SetReal();
   SaturationThreshold_NumericControl.SetRange( ThePCCPhotSaturationThresholdParameter->MinimumValue(), ThePCCPhotSaturationThresholdParameter->MaximumValue() );
   SaturationThreshold_NumericControl.SetPrecision( ThePCCPhotSaturationThresholdParameter->Precision() );
   SaturationThreshold_NumericControl.SetToolTip( "<p>Stars with one or more pixels with a value above this threshold will be "
                           "measured, but a control flag will be set to mark them as saturated.</p>" );
   SaturationThreshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&PhotometricColorCalibrationInterface::e_EditValueUpdated, w );

   //

   UsePSFPhotometry_CheckBox.SetText( "PSF photometry" );
   UsePSFPhotometry_CheckBox.SetToolTip( "<p>If enabled, PhotometricColorCalibration will perform PSF photometry instead of "
      "aperture photometry to analyze star fluxes.</p>" );
   UsePSFPhotometry_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   UsePSFPhotometry_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   UsePSFPhotometry_Sizer.Add( UsePSFPhotometry_CheckBox );
   UsePSFPhotometry_Sizer.AddStretch();

   //

   ShowDetectedStars_CheckBox.SetText( "Show detected stars" );
   ShowDetectedStars_CheckBox.SetToolTip( "<p>Generate a control image where the measured star apertures are represented as "
      "circles. Green circles represent valid star measurements. Red circles denote errors.</p>" );
   ShowDetectedStars_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   ShowDetectedStars_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ShowDetectedStars_Sizer.Add( ShowDetectedStars_CheckBox );
   ShowDetectedStars_Sizer.AddStretch();

   //

   ShowBackgroundModels_CheckBox.SetText( "Show background models" );
   ShowBackgroundModels_CheckBox.SetToolTip( "<p>Generate a control image with a representation of the estimated local "
      "background used for photometric analysis.</p>" );
   ShowBackgroundModels_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   ShowBackgroundModels_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   ShowBackgroundModels_Sizer.Add( ShowBackgroundModels_CheckBox );
   ShowBackgroundModels_Sizer.AddStretch();

   //

   GenerateGraphs_CheckBox.SetText( "Generate graphs" );
   GenerateGraphs_CheckBox.SetToolTip( "<p>Generate interactive graphs to analyze the linear fits between catalog "
      "and image color indices for measured stars.</p>" );
   GenerateGraphs_CheckBox.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   //

   const char* resetPhotometryToolTip = "<p>Reset configuration settings for the Photometry script.</p>"
      "<p>This is normally not necessary, since scripts manage their own configuration settings internally. However, "
      "sometimes there are configuration parameters that may become wrong or corrupted for a variety of reasons. Use this "
      "option to initialize all working parameters of the Photometry script to factory-default values.</p>";

   ResetPhotometryConfiguration_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/reload.png" ) ) );
   ResetPhotometryConfiguration_ToolButton.SetScaledFixedSize( 22, 22 );
   ResetPhotometryConfiguration_ToolButton.SetToolTip( resetPhotometryToolTip );
   ResetPhotometryConfiguration_ToolButton.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   //

   GenerateGraphs_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   GenerateGraphs_Sizer.Add( GenerateGraphs_CheckBox );
   GenerateGraphs_Sizer.AddStretch();
   GenerateGraphs_Sizer.Add( ResetPhotometryConfiguration_ToolButton );

   //

   PhotometryParameters_Sizer.SetSpacing( 4 );
   PhotometryParameters_Sizer.Add( PhotometryCatalog_Sizer );
   PhotometryParameters_Sizer.Add( PhotometryAutoLimitMagnitude_Sizer );
   PhotometryParameters_Sizer.Add( PhotometryLimitMagnitude_Sizer );
   PhotometryParameters_Sizer.Add( AutoAperture_Sizer );
   PhotometryParameters_Sizer.Add( Aperture_Sizer );
   PhotometryParameters_Sizer.Add( SaturationThreshold_NumericControl );
   PhotometryParameters_Sizer.Add( UsePSFPhotometry_Sizer );
   PhotometryParameters_Sizer.Add( ShowDetectedStars_Sizer );
   PhotometryParameters_Sizer.Add( ShowBackgroundModels_Sizer );
   PhotometryParameters_Sizer.Add( GenerateGraphs_Sizer );

   PhotometryParameters_Control.SetSizer( PhotometryParameters_Sizer );
   PhotometryParameters_Control.AdjustToContents();

   //

   BackgroundReference_SectionBar.SetTitle( "Background Neutralization" );
   BackgroundReference_SectionBar.EnableTitleCheckBox();
   BackgroundReference_SectionBar.OnCheck( (SectionBar::check_event_handler)&PhotometricColorCalibrationInterface::e_SectionCheck, w );
   BackgroundReference_SectionBar.SetSection( BackgroundReference_Control );

   //

   BackgroundReferenceView_Label.SetText( "Reference image:" );
   BackgroundReferenceView_Label.SetFixedWidth( labelWidth1 );
   BackgroundReferenceView_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   BackgroundReferenceView_Edit.SetToolTip( "<p>Background reference image.</p>"
      "<p>PhotometricColorCalibration will use pixels read from this image to compute a mean background level for "
      "each color channel. If you leave this field blank (or with its default &lt;target image&gt; value), the "
      "target image will be also the background reference image during the color calibration process.</p>"
      "<p>You should specify a view that represents the <i>true background</i> of the image. In most cases this "
      "means that you must select a view whose pixels are strongly dominated by the sky background, as it is "
      "being represented on the target image. A typical example involves defining a small preview over a free "
      "sky area of the target image, and selecting it here as the background reference image.</p>"
      "<p>You can also drag a view selector to this control, and the corresponding view will be selected.</p>" );
   BackgroundReferenceView_Edit.OnGetFocus( (Control::event_handler)&PhotometricColorCalibrationInterface::e_GetFocus, w );
   BackgroundReferenceView_Edit.OnEditCompleted( (Edit::edit_event_handler)&PhotometricColorCalibrationInterface::e_EditCompleted, w );
   BackgroundReferenceView_Edit.OnViewDrag( (Control::view_drag_event_handler)&PhotometricColorCalibrationInterface::e_ViewDrag, w );
   BackgroundReferenceView_Edit.OnViewDrop( (Control::view_drop_event_handler)&PhotometricColorCalibrationInterface::e_ViewDrop, w );

   BackgroundReferenceView_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   BackgroundReferenceView_ToolButton.SetScaledFixedSize( 20, 20 );
   BackgroundReferenceView_ToolButton.SetToolTip( "<p>Select the background reference image.</p>" );
   BackgroundReferenceView_ToolButton.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );

   BackgroundReferenceView_Sizer.SetSpacing( 4 );
   BackgroundReferenceView_Sizer.Add( BackgroundReferenceView_Label );
   BackgroundReferenceView_Sizer.Add( BackgroundReferenceView_Edit );
   BackgroundReferenceView_Sizer.Add( BackgroundReferenceView_ToolButton );

   //

   BackgroundLow_NumericControl.label.SetText( "Lower limit:" );
   BackgroundLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   BackgroundLow_NumericControl.slider.SetRange( 0, 100 );
   BackgroundLow_NumericControl.slider.SetScaledMinWidth( 200 );
   BackgroundLow_NumericControl.SetReal();
   BackgroundLow_NumericControl.SetRange( ThePCCBackgroundLowParameter->MinimumValue(), ThePCCBackgroundLowParameter->MaximumValue() );
   BackgroundLow_NumericControl.SetPrecision( ThePCCBackgroundLowParameter->Precision() );
   BackgroundLow_NumericControl.SetToolTip( "<p>Lower bound of the set of background pixels. Background reference "
      "pixels equal to or smaller than this value will be rejected for calculation of mean background levels.</p>" );
   BackgroundLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&PhotometricColorCalibrationInterface::e_EditValueUpdated, w );

   //

   BackgroundHigh_NumericControl.label.SetText( "Upper limit:" );
   BackgroundHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   BackgroundHigh_NumericControl.slider.SetRange( 0, 100 );
   BackgroundHigh_NumericControl.slider.SetScaledMinWidth( 200 );
   BackgroundHigh_NumericControl.SetReal();
   BackgroundHigh_NumericControl.SetRange( ThePCCBackgroundHighParameter->MinimumValue(), ThePCCBackgroundHighParameter->MaximumValue() );
   BackgroundHigh_NumericControl.SetPrecision( ThePCCBackgroundHighParameter->Precision() );
//    BackgroundHigh_NumericControl.edit.SetFixedWidth( editWidth1 );
   BackgroundHigh_NumericControl.SetToolTip( "<p>Upper bound of the set of background pixels. Background reference "
      "pixels equal to or larger than this value will be rejected for calculation of mean background levels.</p>" );
   BackgroundHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&PhotometricColorCalibrationInterface::e_EditValueUpdated, w );

   //

   BackgroundROI_GroupBox.SetTitle( "Region of Interest" );
   BackgroundROI_GroupBox.EnableTitleCheckBox();
   BackgroundROI_GroupBox.SetToolTip( "<p>Besides (or instead of) selecting a reference view for background sampling, "
      "you can define a rectangular region of interest (ROI) to restrict background evaluation to pixels within that "
      "region. By defining a ROI on the target image (leaving the reference image field blank), you can define instances "
      "that don't depend on the existence of specific views or previews. Such process instances are more portable and "
      "integrate much better with projects and icons where you are defining process templates that can be applied to "
      "multiple images.</p>" );
   BackgroundROI_GroupBox.OnCheck( (GroupBox::check_event_handler)&PhotometricColorCalibrationInterface::e_GroupCheck, w );
   BackgroundROI_GroupBox.OnViewDrag( (Control::view_drag_event_handler)&PhotometricColorCalibrationInterface::e_ViewDrag, w );
   BackgroundROI_GroupBox.OnViewDrop( (Control::view_drop_event_handler)&PhotometricColorCalibrationInterface::e_ViewDrop, w );

   //

#define DELTA_FRAME  1 // width of a GroupBox frame line

   const char* backgroundROIX0ToolTip = "<p>X pixel coordinate of the upper-left corner of the ROI.</p>";

   BackgroundROIX0_Label.SetText( "Left:" );
   BackgroundROIX0_Label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 + DELTA_FRAME ) );
   BackgroundROIX0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIX0_Label.SetToolTip( backgroundROIX0ToolTip );

   BackgroundROIX0_SpinBox.SetRange( 0, int_max );
   BackgroundROIX0_SpinBox.SetToolTip( backgroundROIX0ToolTip );
   BackgroundROIX0_SpinBox.SetMinWidth( editWidth3 );
   BackgroundROIX0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   //

   const char* backgroundROIY0ToolTip = "<p>Y pixel coordinate of the upper-left corner of the ROI.</p>";

   BackgroundROIY0_Label.SetText( "Top:" );
   BackgroundROIY0_Label.SetFixedWidth( labelWidth2 );
   BackgroundROIY0_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIY0_Label.SetToolTip( backgroundROIY0ToolTip );

   BackgroundROIY0_SpinBox.SetRange( 0, int_max );
   BackgroundROIY0_SpinBox.SetToolTip( backgroundROIY0ToolTip );
   BackgroundROIY0_SpinBox.SetMinWidth( editWidth3 );
   BackgroundROIY0_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   //

   BackgroundROIRow1_Sizer.SetSpacing( 4 );
   BackgroundROIRow1_Sizer.Add( BackgroundROIX0_Label );
   BackgroundROIRow1_Sizer.Add( BackgroundROIX0_SpinBox );
   BackgroundROIRow1_Sizer.Add( BackgroundROIY0_Label );
   BackgroundROIRow1_Sizer.Add( BackgroundROIY0_SpinBox );
   BackgroundROIRow1_Sizer.AddStretch();

   //

   const char* backgroundROIWidthToolTip = "<p>Width of the ROI in pixels.</p>";

   BackgroundROIWidth_Label.SetText( "Width:" );
   BackgroundROIWidth_Label.SetFixedWidth( labelWidth1 - w.LogicalPixelsToPhysical( 6 + DELTA_FRAME ) );
   BackgroundROIWidth_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIWidth_Label.SetToolTip( backgroundROIWidthToolTip );

   BackgroundROIWidth_SpinBox.SetRange( 0, int_max );
   BackgroundROIWidth_SpinBox.SetToolTip( backgroundROIWidthToolTip );
   BackgroundROIWidth_SpinBox.SetMinWidth( editWidth3 );
   BackgroundROIWidth_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   //

   const char* backgroundROIHeightToolTip = "<p>Height of the ROI in pixels.</p>";

   BackgroundROIHeight_Label.SetText( "Height:" );
   BackgroundROIHeight_Label.SetFixedWidth( labelWidth2 );
   BackgroundROIHeight_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundROIHeight_Label.SetToolTip( backgroundROIHeightToolTip );

   BackgroundROIHeight_SpinBox.SetRange( 0, int_max );
   BackgroundROIHeight_SpinBox.SetToolTip( backgroundROIHeightToolTip );
   BackgroundROIHeight_SpinBox.SetMinWidth( editWidth3 );
   BackgroundROIHeight_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&PhotometricColorCalibrationInterface::e_SpinValueUpdated, w );

   BackgroundROISelectPreview_Button.SetText( " From Preview " );
   BackgroundROISelectPreview_Button.SetToolTip( "<p>Import ROI coordinates from an existing preview.</p>"
      "<p>Or drag a preview selector to this button and the corresponding coordinates will be selected automatically.</p>" );
   BackgroundROISelectPreview_Button.OnClick( (Button::click_event_handler)&PhotometricColorCalibrationInterface::e_Click, w );
   BackgroundROISelectPreview_Button.OnViewDrag( (Control::view_drag_event_handler)&PhotometricColorCalibrationInterface::e_ViewDrag, w );
   BackgroundROISelectPreview_Button.OnViewDrop( (Control::view_drop_event_handler)&PhotometricColorCalibrationInterface::e_ViewDrop, w );

   //

   BackgroundROIRow2_Sizer.SetSpacing( 4 );
   BackgroundROIRow2_Sizer.Add( BackgroundROIWidth_Label );
   BackgroundROIRow2_Sizer.Add( BackgroundROIWidth_SpinBox );
   BackgroundROIRow2_Sizer.Add( BackgroundROIHeight_Label );
   BackgroundROIRow2_Sizer.Add( BackgroundROIHeight_SpinBox );
   BackgroundROIRow2_Sizer.AddSpacing( 12 );
   BackgroundROIRow2_Sizer.Add( BackgroundROISelectPreview_Button );
   BackgroundROIRow2_Sizer.AddStretch();

   //

   BackgroundROI_Sizer.SetMargin( 6 );
   BackgroundROI_Sizer.SetSpacing( 4 );
   BackgroundROI_Sizer.Add( BackgroundROIRow1_Sizer );
   BackgroundROI_Sizer.Add( BackgroundROIRow2_Sizer );

   BackgroundROI_GroupBox.SetSizer( BackgroundROI_Sizer );

   //

   BackgroundReference_Sizer.SetSpacing( 4 );
   BackgroundReference_Sizer.Add( BackgroundReferenceView_Sizer );
   BackgroundReference_Sizer.Add( BackgroundLow_NumericControl );
   BackgroundReference_Sizer.Add( BackgroundHigh_NumericControl );
   BackgroundReference_Sizer.Add( BackgroundROI_GroupBox );

   BackgroundReference_Control.SetSizer( BackgroundReference_Sizer );
   BackgroundReference_Control.AdjustToContents();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 4 );

   Global_Sizer.Add( WhiteReference_Sizer );
   Global_Sizer.Add( Server_Sizer );
   Global_Sizer.Add( ApplyColorCalibration_Sizer );
   Global_Sizer.Add( ImageParameters_SectionBar );
   Global_Sizer.Add( ImageParameters_Control );
   Global_Sizer.Add( PlateSolverParameters_SectionBar );
   Global_Sizer.Add( PlateSolverParameters_Control );
   Global_Sizer.Add( AdvancedPlateSolverParameters_SectionBar );
   Global_Sizer.Add( AdvancedPlateSolverParameters_Control );
   Global_Sizer.Add( PhotometryParameters_SectionBar );
   Global_Sizer.Add( PhotometryParameters_Control );
   Global_Sizer.Add( BackgroundReference_SectionBar );
   Global_Sizer.Add( BackgroundReference_Control );

   //

   /*
    * N.B.: Using a separate view as background reference with PCC is an
    * "advanced" (that is, usually wrong) option. So we'll hide these controls
    * for sanity (of our own minds, mainly).
    */
   BackgroundReferenceView_Label.Hide();
   BackgroundReferenceView_Edit.Hide();
   BackgroundReferenceView_ToolButton.Hide();

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   AdvancedPlateSolverParameters_Control.Hide();
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF PhotometricColorCalibrationInterface.cpp - Released 2017-07-18T16:14:18Z
