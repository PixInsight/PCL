//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard StarGenerator Process Module Version 01.01.00.0247
// ----------------------------------------------------------------------------
// StarGeneratorInterface.cpp - Released 2016/02/21 20:22:43 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard StarGenerator PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "StarGeneratorInterface.h"
#include "StarGeneratorProcess.h"
#include "StarGeneratorParameters.h"

#include <pcl/Math.h>
#include <pcl/FileDialog.h>
#include <pcl/Settings.h>

namespace pcl
{

// ----------------------------------------------------------------------------

StarGeneratorInterface* TheStarGeneratorInterface = 0;

// ----------------------------------------------------------------------------

#include "StarGeneratorIcon.xpm"

// ----------------------------------------------------------------------------

StarGeneratorInterface::StarGeneratorInterface() :
ProcessInterface(), instance( TheStarGeneratorProcess ), GUI( 0 )
{
   TheStarGeneratorInterface = this;
}

StarGeneratorInterface::~StarGeneratorInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString StarGeneratorInterface::Id() const
{
   return "StarGenerator";
}

MetaProcess* StarGeneratorInterface::Process() const
{
   return TheStarGeneratorProcess;
}

const char** StarGeneratorInterface::IconImageXPM() const
{
   return StarGeneratorIcon_XPM;
}

InterfaceFeatures StarGeneratorInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

void StarGeneratorInterface::ApplyInstance() const
{
   instance.LaunchGlobal();
}

void StarGeneratorInterface::ResetInstance()
{
   StarGeneratorInstance defaultInstance( TheStarGeneratorProcess );
   ImportProcess( defaultInstance );
}

bool StarGeneratorInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   // ### Deferred initialization
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "StarGenerator" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheStarGeneratorProcess;
}

ProcessImplementation* StarGeneratorInterface::NewProcess() const
{
   return new StarGeneratorInstance( instance );
}

bool StarGeneratorInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const StarGeneratorInstance* r = dynamic_cast<const StarGeneratorInstance*>( &p );
   if ( r == 0 )
   {
      whyNot = "Not a StarGenerator instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool StarGeneratorInterface::RequiresInstanceValidation() const
{
   return true;
}

bool StarGeneratorInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

void StarGeneratorInterface::SaveSettings() const
{
   Settings::Write( SettingsKey() + "StarDatabasePath", instance.starDatabasePath );
}

void StarGeneratorInterface::LoadSettings()
{
   Settings::Read( SettingsKey() + "StarDatabasePath", instance.starDatabasePath );
   GUI->StarDatabase_Edit.SetText( instance.starDatabasePath );
}

// ----------------------------------------------------------------------------

void StarGeneratorInterface::UpdateControls()
{
   GUI->StarDatabase_Edit.SetText( instance.starDatabasePath );

   UpdateRAControls();
   UpdateDecControls();
   UpdateEpochControls();

   GUI->ProjectionSystem_ComboBox.SetCurrentItem( instance.projectionSystem );

   GUI->FocalLength_NumericEdit.SetValue( instance.focalLength );

   GUI->PixelSize_NumericEdit.SetValue( instance.pixelSize );

   GUI->SensorWidth_NumericEdit.SetValue( instance.sensorWidth );

   GUI->SensorHeight_NumericEdit.SetValue( instance.sensorHeight );

   GUI->LimitMagnitude_NumericEdit.SetValue( instance.limitMagnitude );

   GUI->OutputMode_ComboBox.SetCurrentItem( instance.outputMode );

   GUI->OutputFile_Edit.Enable( instance.outputMode == SGOutputMode::CSVFile );
   GUI->OutputFile_Edit.SetText( instance.outputFilePath );

   GUI->OutputFile_ToolButton.Enable( instance.outputMode == SGOutputMode::CSVFile );

   GUI->StarFWHM_NumericEdit.Enable( instance.outputMode == SGOutputMode::Image );
   GUI->StarFWHM_NumericEdit.SetValue( instance.starFWHM );

   GUI->Nonlinear_CheckBox.Enable( instance.outputMode == SGOutputMode::Image );
   GUI->Nonlinear_CheckBox.SetChecked( instance.nonlinear );

   GUI->TargetMinimumValue_NumericEdit.Enable( instance.outputMode == SGOutputMode::Image && instance.nonlinear );
   GUI->TargetMinimumValue_NumericEdit.SetValue( instance.targetMinimumValue );
}

void StarGeneratorInterface::UpdateRAControls()
{
   double hh = instance.centerRA/15;
   double hm = 60*Frac( hh );
   double hs = Round( 60*Frac( hm ), 2 );
   if ( hs == 60 )
   {
      hs = 0;
      hm += 1;
   }
   if ( hm == 60 )
   {
      hm = 0;
      hh += 1;
   }
   if ( hh >= 24 )
      hh = hm = hs = 0;

   GUI->RAHours_SpinBox.SetValue( int( hh ) );
   GUI->RAMins_SpinBox.SetValue( int( hm ) );
   GUI->RASecs_NumericEdit.SetValue( hs );
}

void StarGeneratorInterface::UpdateDecControls()
{
   double dd = Abs( instance.centerDec );
   double dm = 60*Frac( dd );
   double ds = Round( 60*Frac( dm ), 2 );
   if ( ds == 60 )
   {
      ds = 0;
      dm += 1;
   }
   if ( dm == 60 )
   {
      dm = 0;
      dd += 1;
   }
   if ( dd >= 90 )
      dm = ds = 0;

   GUI->DecDegs_SpinBox.SetValue( int( Sign( instance.centerDec )*dd ) );
   GUI->DecMins_SpinBox.SetValue( int( dm ) );
   GUI->DecSecs_NumericEdit.SetValue( ds );
   GUI->DecSouth_CheckBox.SetChecked( instance.centerDec < 0 );
}

void StarGeneratorInterface::UpdateEpochControls()
{
   int year, month, day;
   double dummy;
   JDToComplexTime( year, month, day, dummy, instance.epoch );

   GUI->EpochYear_SpinBox.SetValue( year );
   GUI->EpochMonth_SpinBox.SetValue( month );
   GUI->EpochDay_SpinBox.SetValue( day );
}

// ----------------------------------------------------------------------------

void StarGeneratorInterface::GetRA()
{
   instance.centerRA = Range( (GUI->RAHours_SpinBox.Value()
                             + GUI->RAMins_SpinBox.Value()/60.0
                             + GUI->RASecs_NumericEdit.Value()/3600.0), 0.0, 24.0 );
   if ( instance.centerRA == 24 )
      instance.centerRA = 0;
   else
      instance.centerRA *= 15;

   UpdateRAControls();
}

void StarGeneratorInterface::GetDec()
{
   instance.centerDec = Range( Abs( GUI->DecDegs_SpinBox.Value() )
                                  + GUI->DecMins_SpinBox.Value()/60.0
                                  + GUI->DecSecs_NumericEdit.Value()/3600.0, 0.0, 90.0 );
   if ( GUI->DecDegs_SpinBox.Value() == 0 )
   {
      if ( GUI->DecSouth_CheckBox.IsChecked() )
         instance.centerDec = -instance.centerDec;
   }
   else
      instance.centerDec *= Sign( GUI->DecDegs_SpinBox.Value() );

   UpdateDecControls();
}

void StarGeneratorInterface::GetEpoch()
{
   instance.epoch = ComplexTimeToJD( GUI->EpochYear_SpinBox.Value(),
                                     GUI->EpochMonth_SpinBox.Value(),
                                     GUI->EpochDay_SpinBox.Value(),
                                     0.5 /*always at noon*/ );
}

// ----------------------------------------------------------------------------

void StarGeneratorInterface::__EditCompleted( Edit& sender )
{
   if ( sender == GUI->StarDatabase_Edit )
   {
      String path = sender.Text().Trimmed();
      instance.starDatabasePath = path;
      sender.SetText( path );
   }
   else if ( sender == GUI->OutputFile_Edit )
   {
      String path = sender.Text().Trimmed();
      instance.outputFilePath = path;
      sender.SetText( path );
   }
}

void StarGeneratorInterface::__RealValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->RASecs_NumericEdit )
      GetRA();
   else if ( sender == GUI->DecSecs_NumericEdit )
      GetDec();
   else if ( sender == GUI->FocalLength_NumericEdit )
      instance.focalLength = value;
   else if ( sender == GUI->PixelSize_NumericEdit )
      instance.pixelSize = value;
   else if ( sender == GUI->SensorWidth_NumericEdit )
      instance.sensorWidth = int32( value );
   else if ( sender == GUI->SensorHeight_NumericEdit )
      instance.sensorHeight = int32( value );
   else if ( sender == GUI->LimitMagnitude_NumericEdit )
      instance.limitMagnitude = value;
   else if ( sender == GUI->StarFWHM_NumericEdit )
      instance.starFWHM = value;
   else if ( sender == GUI->TargetMinimumValue_NumericEdit )
      instance.targetMinimumValue = value;
}

void StarGeneratorInterface::__IntegerValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->RAHours_SpinBox || sender == GUI->RAMins_SpinBox )
      GetRA();
   else if ( sender == GUI->DecDegs_SpinBox || sender == GUI->DecMins_SpinBox )
      GetDec();
   else if ( sender == GUI->EpochYear_SpinBox || sender == GUI->EpochMonth_SpinBox || sender == GUI->EpochDay_SpinBox )
      GetEpoch();
}

void StarGeneratorInterface::__Button_Clicked( Button& sender, bool checked )
{
   if ( sender == GUI->StarDatabase_ToolButton )
   {
      OpenFileDialog d;
      d.DisableMultipleSelections();
      d.SetCaption( "StarGenerator: Select Star Database File" );
      if ( d.Execute() )
      {
         instance.starDatabasePath = d.FileName();
         GUI->StarDatabase_Edit.SetText( instance.starDatabasePath );
      }
   }
   else if ( sender == GUI->OutputFile_ToolButton )
   {
      SaveFileDialog d;
      d.SetCaption( "StarGenerator: Select Output CSV File" );
      d.EnableOverwritePrompt();

      FileFilter csvFilter;
      csvFilter.SetDescription( "CSV Text Files" );
      csvFilter.AddExtension( ".csv" );
      d.Filters().Add( csvFilter );
      FileFilter allFilter;
      allFilter.SetDescription( "Any Files" );
      allFilter.AddExtension( "*" );
      d.Filters().Add( allFilter );

      if ( d.Execute() )
      {
         instance.outputFilePath = d.FileName();
         GUI->OutputFile_Edit.SetText( instance.outputFilePath );
      }
   }
   else if ( sender == GUI->DecSouth_CheckBox )
   {
      if ( checked != (instance.centerDec < 0) )
      {
         instance.centerDec = -instance.centerDec;
         UpdateDecControls();
      }
   }
   else if ( sender == GUI->Nonlinear_CheckBox )
   {
      instance.nonlinear = checked;
      UpdateControls();
   }
}

void StarGeneratorInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->ProjectionSystem_ComboBox )
      instance.projectionSystem = itemIndex;
   else if ( sender == GUI->OutputMode_ComboBox )
   {
      instance.outputMode = itemIndex;
      UpdateControls();
   }
}

// ----------------------------------------------------------------------------

StarGeneratorInterface::GUIData::GUIData( StarGeneratorInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Nonlinear target minimum:" ) + "M" );
   int editWidth1 = fnt.Width( String( '0',  8 ) );
   int editWidth2 = fnt.Width( String( '0', 12 ) );

   //

   const char* starDatabaseToolTip = "<p>Star database file in PixInsight binary format.</p>"
      "<p>Databases are not included with standard PixInsight distributions. For available star databases, please see:</p>"
      "<p>http://pixinsight.com/download/</p>";

   StarDatabase_Label.SetText( "Star database:" );

   StarDatabase_Edit.SetToolTip( starDatabaseToolTip );
   StarDatabase_Edit.OnEditCompleted( (Edit::edit_event_handler)&StarGeneratorInterface::__EditCompleted, w );

   StarDatabase_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   StarDatabase_ToolButton.SetScaledFixedSize( 20, 20 );
   StarDatabase_ToolButton.SetToolTip( starDatabaseToolTip );
   StarDatabase_ToolButton.OnClick( (Button::click_event_handler)&StarGeneratorInterface::__Button_Clicked, w );

   StarDatabase_Sizer.SetSpacing( 4 );
   StarDatabase_Sizer.Add( StarDatabase_Edit, 100 );
   StarDatabase_Sizer.Add( StarDatabase_ToolButton );

   //

   RA_Label.SetText( "Right ascension (hms):" );
   RA_Label.SetMinWidth( labelWidth1 );
   RA_Label.SetToolTip( "<p>Projection center, right ascension.</p>" );
   RA_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   RAHours_SpinBox.SetRange( 0, 24 );
   RAHours_SpinBox.SetFixedWidth( editWidth1 );
   RAHours_SpinBox.SetToolTip( "<p>Projection center, right ascension hours [0,23].</p>" );
   RAHours_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&StarGeneratorInterface::__IntegerValueUpdated, w );

   RAMins_SpinBox.SetRange( 0, 60 );
   RAMins_SpinBox.SetFixedWidth( editWidth1 );
   RAMins_SpinBox.SetToolTip( "<p>Projection center, right ascension minutes [0,59].</p>" );
   RAMins_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&StarGeneratorInterface::__IntegerValueUpdated, w );

   RASecs_NumericEdit.label.Hide();
   RASecs_NumericEdit.SetReal();
   RASecs_NumericEdit.SetRange( 0, 60 );
   RASecs_NumericEdit.SetPrecision( 3 );
   RASecs_NumericEdit.edit.SetFixedWidth( editWidth1 );
   RASecs_NumericEdit.SetToolTip( "<p>Projection center, right ascension seconds [0,60[.</p>" );
   RASecs_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&StarGeneratorInterface::__RealValueUpdated, w );

   RA_Sizer.SetSpacing( 4 );
   RA_Sizer.Add( RA_Label );
   RA_Sizer.Add( RAHours_SpinBox );
   RA_Sizer.Add( RAMins_SpinBox );
   RA_Sizer.Add( RASecs_NumericEdit );
   RA_Sizer.AddStretch();

   //

   Dec_Label.SetText( "Declination (dms):" );
   Dec_Label.SetMinWidth( labelWidth1 );
   Dec_Label.SetToolTip( "<p>Projection center, declination.</p>" );
   Dec_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   DecDegs_SpinBox.SetRange( -90, +90 );
   DecDegs_SpinBox.SetFixedWidth( editWidth1 );
   DecDegs_SpinBox.SetToolTip( "<p>Projection center, declination degrees [-90,+90].</p>" );
   DecDegs_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&StarGeneratorInterface::__IntegerValueUpdated, w );

   DecMins_SpinBox.SetRange( 0, 60 );
   DecMins_SpinBox.SetFixedWidth( editWidth1 );
   DecMins_SpinBox.SetToolTip( "<p>Projection center, declination arcminutes [0,59].</p>" );
   DecMins_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&StarGeneratorInterface::__IntegerValueUpdated, w );

   DecSecs_NumericEdit.label.Hide();
   DecSecs_NumericEdit.SetReal();
   DecSecs_NumericEdit.SetRange( 0, 60 );
   DecSecs_NumericEdit.SetPrecision( 2 );
   DecSecs_NumericEdit.edit.SetFixedWidth( editWidth1 );
   DecSecs_NumericEdit.SetToolTip( "<p>Projection center, declination arcseconds [0,60[.</p>" );
   DecSecs_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&StarGeneratorInterface::__RealValueUpdated, w );

   DecSouth_CheckBox.SetText( "S" );
   DecSouth_CheckBox.SetToolTip( "<p>South declination.</p>");
   DecSouth_CheckBox.OnClick( (Button::click_event_handler)&StarGeneratorInterface::__Button_Clicked, w );

   Dec_Sizer.SetSpacing( 4 );
   Dec_Sizer.Add( Dec_Label );
   Dec_Sizer.Add( DecDegs_SpinBox );
   Dec_Sizer.Add( DecMins_SpinBox );
   Dec_Sizer.Add( DecSecs_NumericEdit );
   Dec_Sizer.Add( DecSouth_CheckBox );
   Dec_Sizer.AddStretch();

   //

   Epoch_Label.SetText( "Epoch (ymd):" );
   Epoch_Label.SetMinWidth( labelWidth1 );
   Epoch_Label.SetToolTip( "<p>Epoch of the rendition image. StarGenerator will apply star proper motions "
                           "to translate the positions of all projected stars according to the specified "
                           "epoch date.</p>" );
   Epoch_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   EpochYear_SpinBox.SetRange( 1000, 3000 );
   EpochYear_SpinBox.SetFixedWidth( editWidth1 );
   EpochYear_SpinBox.SetToolTip( "<p>Rendition epoch, year [1000,3000].</p>" );
   EpochYear_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&StarGeneratorInterface::__IntegerValueUpdated, w );

   EpochMonth_SpinBox.SetRange( 1, 12 );
   EpochMonth_SpinBox.SetFixedWidth( editWidth1 );
   EpochMonth_SpinBox.SetToolTip( "<p>Rendition epoch, month [1,12].</p>" );
   EpochMonth_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&StarGeneratorInterface::__IntegerValueUpdated, w );

   EpochDay_SpinBox.SetRange( 0, 31 );
   EpochDay_SpinBox.SetFixedWidth( editWidth1 );
   EpochDay_SpinBox.SetToolTip( "<p>Rendition epoch, day [0,31].</p>" );
   EpochDay_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&StarGeneratorInterface::__IntegerValueUpdated, w );

   Epoch_Sizer.SetSpacing( 4 );
   Epoch_Sizer.Add( Epoch_Label );
   Epoch_Sizer.Add( EpochYear_SpinBox );
   Epoch_Sizer.Add( EpochMonth_SpinBox );
   Epoch_Sizer.Add( EpochDay_SpinBox );
   Epoch_Sizer.AddStretch();

   //

   const char* projectionSystemToolTip = "<p>Projection System</p>"
      "<p><b>Conformal projections</b> are <i>angle preserving</i> projection systems. The existing angular "
      "distance between two objects on the sky is preserved on the projected representation. This allows for "
      "projection of large fields of view with minimal distortion. The conformal projections are selected "
      "automatically as a function of the central declination:</p>"
      "<ul>"
      "<li>Stereographic projection for |&delta;| &ge; 70&deg;</li>"
      "<li>Mercator cylindrical projection for |&delta;| &le; 20&deg;</li>"
      "<li>Lambert conformal conic projection for 20&deg; &lt; |&delta;| &lt; 70&deg;</li>"
      "</ul>"
      "<p><b>Gnomonic projection</b> is a <i>nonconformal</i> projection system with two main advantages: it "
      "is valid for all central declinations, and it represents the projection of the sky through a spherical "
      "lens over a flat focal plane. Thus, gnomonic projection reproduces the image formed on the <i>flat</i> "
      "focal plane of an ideal telescope (neglecting geometrical aberrations, field curvature, and distortions). "
      "This is the default projection system.</p>";

   ProjectionSystem_Label.SetText( "Projection:" );
   ProjectionSystem_Label.SetToolTip( projectionSystemToolTip );
   ProjectionSystem_Label.SetMinWidth( labelWidth1 );
   ProjectionSystem_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ProjectionSystem_ComboBox.AddItem( "Conformal" );
   ProjectionSystem_ComboBox.AddItem( "Gnomonic" );
   ProjectionSystem_ComboBox.SetToolTip( projectionSystemToolTip );
   ProjectionSystem_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&StarGeneratorInterface::__ItemSelected, w );

   ProjectionSystem_Sizer.SetSpacing( 4 );
   ProjectionSystem_Sizer.Add( ProjectionSystem_Label );
   ProjectionSystem_Sizer.Add( ProjectionSystem_ComboBox );
   ProjectionSystem_Sizer.AddStretch();

   //

   FocalLength_NumericEdit.label.SetText( "Focal length (mm):" );
   FocalLength_NumericEdit.label.SetFixedWidth( labelWidth1 );
   FocalLength_NumericEdit.SetReal();
   FocalLength_NumericEdit.SetRange( TheSGFocalLengthParameter->MinimumValue(), TheSGFocalLengthParameter->MaximumValue() );
   FocalLength_NumericEdit.SetPrecision( TheSGFocalLengthParameter->Precision() );
   FocalLength_NumericEdit.edit.SetFixedWidth( editWidth2 );
   FocalLength_NumericEdit.SetToolTip( "<p>Focal length in millimeters.</p>" );
   FocalLength_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&StarGeneratorInterface::__RealValueUpdated, w );
   FocalLength_NumericEdit.sizer.AddStretch();

   PixelSize_NumericEdit.label.SetText( "Pixel size (um):" );
   PixelSize_NumericEdit.label.SetFixedWidth( labelWidth1 );
   PixelSize_NumericEdit.SetReal();
   PixelSize_NumericEdit.SetRange( TheSGPixelSizeParameter->MinimumValue(), TheSGPixelSizeParameter->MaximumValue() );
   PixelSize_NumericEdit.SetPrecision( TheSGPixelSizeParameter->Precision() );
   PixelSize_NumericEdit.edit.SetFixedWidth( editWidth2 );
   PixelSize_NumericEdit.SetToolTip( "<p>Pixel size in micrometers.</p>" );
   PixelSize_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&StarGeneratorInterface::__RealValueUpdated, w );
   PixelSize_NumericEdit.sizer.AddStretch();

   SensorWidth_NumericEdit.label.SetText( "Sensor width (px):" );
   SensorWidth_NumericEdit.label.SetFixedWidth( labelWidth1 );
   SensorWidth_NumericEdit.SetInteger();
   SensorWidth_NumericEdit.SetRange( TheSGSensorWidthParameter->MinimumValue(), TheSGSensorWidthParameter->MaximumValue() );
   SensorWidth_NumericEdit.edit.SetFixedWidth( editWidth2 );
   SensorWidth_NumericEdit.SetToolTip( "<p>Sensor width in pixels.</p>" );
   SensorWidth_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&StarGeneratorInterface::__RealValueUpdated, w );
   SensorWidth_NumericEdit.sizer.AddStretch();

   SensorHeight_NumericEdit.label.SetText( "Sensor height (px):" );
   SensorHeight_NumericEdit.label.SetFixedWidth( labelWidth1 );
   SensorHeight_NumericEdit.SetInteger();
   SensorHeight_NumericEdit.SetRange( TheSGSensorHeightParameter->MinimumValue(), TheSGSensorHeightParameter->MaximumValue() );
   SensorHeight_NumericEdit.edit.SetFixedWidth( editWidth2 );
   SensorHeight_NumericEdit.SetToolTip( "<p>Sensor height in pixels.</p>" );
   SensorHeight_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&StarGeneratorInterface::__RealValueUpdated, w );
   SensorHeight_NumericEdit.sizer.AddStretch();

   LimitMagnitude_NumericEdit.label.SetText( "Limit magnitude:" );
   LimitMagnitude_NumericEdit.label.SetFixedWidth( labelWidth1 );
   LimitMagnitude_NumericEdit.SetReal();
   LimitMagnitude_NumericEdit.SetRange( TheSGLimitMagnitudeParameter->MinimumValue(), TheSGLimitMagnitudeParameter->MaximumValue() );
   LimitMagnitude_NumericEdit.SetPrecision( TheSGLimitMagnitudeParameter->Precision() );
   LimitMagnitude_NumericEdit.edit.SetFixedWidth( editWidth2 );
   LimitMagnitude_NumericEdit.SetToolTip( "<p>Highest magnitude of projected stars.</p>" );
   LimitMagnitude_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&StarGeneratorInterface::__RealValueUpdated, w );
   LimitMagnitude_NumericEdit.sizer.AddStretch();

   const char* outputModeToolTip = "<p>Output Mode</p>"
      "<p>In <b>Render Image</b> mode, all stars found within the output coordinate range will be rendered as "
      "Gaussian profiles on a newly created image. The image rendition will be available as a new image window.</p>"
      "<p>In <b>Generate CSV File</b> mode, a comma-separated value (CSV) file will be generated encoded in 8-bit ASCII format. "
      "The first line in the output CSV file specifies the projection dimensions (width,height), and subsequent lines provide "
      "star data (xPos,yPos,brightness). Lines are separated by newline characters (0x0A).</p>";

   OutputMode_Label.SetText( "Output mode:" );
   OutputMode_Label.SetToolTip( outputModeToolTip );
   OutputMode_Label.SetMinWidth( labelWidth1 );
   OutputMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputMode_ComboBox.AddItem( "Render Image" );
   OutputMode_ComboBox.AddItem( "Generate CSV File" );
   OutputMode_ComboBox.SetToolTip( outputModeToolTip );
   OutputMode_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&StarGeneratorInterface::__ItemSelected, w );

   OutputMode_Sizer.SetSpacing( 4 );
   OutputMode_Sizer.Add( OutputMode_Label );
   OutputMode_Sizer.Add( OutputMode_ComboBox );
   OutputMode_Sizer.AddStretch();

   const char* outputFileToolTip = "<p>Select an output file in CSV format.</p>";

   OutputFile_Label.SetText( "Output file:" );

   OutputFile_Edit.SetToolTip( outputFileToolTip );
   OutputFile_Edit.OnEditCompleted( (Edit::edit_event_handler)&StarGeneratorInterface::__EditCompleted, w );

   OutputFile_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-file.png" ) ) );
   OutputFile_ToolButton.SetScaledFixedSize( 20, 20 );
   OutputFile_ToolButton.SetToolTip( outputFileToolTip );
   OutputFile_ToolButton.OnClick( (Button::click_event_handler)&StarGeneratorInterface::__Button_Clicked, w );

   OutputFile_Sizer.SetSpacing( 4 );
   OutputFile_Sizer.Add( OutputFile_Edit, 100 );
   OutputFile_Sizer.Add( OutputFile_ToolButton );

   StarFWHM_NumericEdit.label.SetText( "Star FWHM (\"):" );
   StarFWHM_NumericEdit.label.SetFixedWidth( labelWidth1 );
   StarFWHM_NumericEdit.SetReal();
   StarFWHM_NumericEdit.SetRange( TheSGStarFWHMParameter->MinimumValue(), TheSGStarFWHMParameter->MaximumValue() );
   StarFWHM_NumericEdit.SetPrecision( TheSGStarFWHMParameter->Precision() );
   StarFWHM_NumericEdit.edit.SetFixedWidth( editWidth2 );
   StarFWHM_NumericEdit.SetToolTip( "<p>Star FWHM in arcseconds.</p>" );
   StarFWHM_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&StarGeneratorInterface::__RealValueUpdated, w );
   StarFWHM_NumericEdit.sizer.AddStretch();

   Nonlinear_CheckBox.SetText( "Nonlinear" );
   Nonlinear_CheckBox.SetToolTip( "<p>If this option is enabled, a nonlinear stretch will be applied to the "
      "output image to match the specified <i>minimum sample value</i> (see next parameter).</p>"
      "<p>If this option is disabled, a linear image will be generated: the output image will be strictly a "
      "linear function of star brightness. Note that linear renditions employ extremely large dynamic ranges "
      "(for example, more than 10<sup>8</sup> discrete pixel values are required to render a star of the 16th "
      "magnitude).</p>" );
   Nonlinear_CheckBox.OnClick( (Button::click_event_handler)&StarGeneratorInterface::__Button_Clicked, w );

   Nonlinear_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   Nonlinear_Sizer.Add( Nonlinear_CheckBox );
   Nonlinear_Sizer.AddStretch();

   TargetMinimumValue_NumericEdit.label.SetText( "Nonlinear target minimum:" );
   TargetMinimumValue_NumericEdit.label.SetFixedWidth( labelWidth1 );
   TargetMinimumValue_NumericEdit.SetReal();
   TargetMinimumValue_NumericEdit.SetRange( TheSGTargetMinimumValueParameter->MinimumValue(), TheSGTargetMinimumValueParameter->MaximumValue() );
   TargetMinimumValue_NumericEdit.SetPrecision( TheSGTargetMinimumValueParameter->Precision() );
   TargetMinimumValue_NumericEdit.edit.SetFixedWidth( editWidth2 );
   TargetMinimumValue_NumericEdit.SetToolTip( "<p>Target minimum sample value when nonlinear output is selected.</p>" );
   TargetMinimumValue_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&StarGeneratorInterface::__RealValueUpdated, w );
   TargetMinimumValue_NumericEdit.sizer.AddStretch();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( StarDatabase_Label );
   Global_Sizer.Add( StarDatabase_Sizer );
   Global_Sizer.Add( RA_Sizer );
   Global_Sizer.Add( Dec_Sizer );
   Global_Sizer.Add( Epoch_Sizer );
   Global_Sizer.Add( ProjectionSystem_Sizer );
   Global_Sizer.Add( FocalLength_NumericEdit );
   Global_Sizer.Add( PixelSize_NumericEdit );
   Global_Sizer.Add( SensorWidth_NumericEdit );
   Global_Sizer.Add( SensorHeight_NumericEdit );
   Global_Sizer.Add( LimitMagnitude_NumericEdit );
   Global_Sizer.Add( OutputMode_Sizer );
   Global_Sizer.Add( OutputFile_Sizer );
   Global_Sizer.Add( StarFWHM_NumericEdit );
   Global_Sizer.Add( Nonlinear_Sizer );
   Global_Sizer.Add( TargetMinimumValue_NumericEdit );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF StarGeneratorInterface.cpp - Released 2016/02/21 20:22:43 UTC
