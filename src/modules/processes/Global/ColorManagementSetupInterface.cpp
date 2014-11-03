// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard Global Process Module Version 01.02.05.0258
// ****************************************************************************
// ColorManagementSetupInterface.cpp - Released 2014/10/29 07:34:56 UTC
// ****************************************************************************
// This file is part of the standard Global PixInsight module.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "ColorManagementSetupInterface.h"
#include "ColorManagementSetupProcess.h"
#include "ColorManagementSetupParameters.h"

#include <pcl/ICCProfile.h>
#include <pcl/GlobalSettings.h>
#include <pcl/Graphics.h>
#include <pcl/ColorDialog.h>
#include <pcl/ErrorHandler.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ColorManagementSetupInterface* TheColorManagementSetupInterface = 0;

// ----------------------------------------------------------------------------

#include "ColorManagementSetupIcon.xpm"

// ----------------------------------------------------------------------------

ColorManagementSetupInterface::ColorManagementSetupInterface() :
ProcessInterface(), instance( TheColorManagementSetupProcess ), GUI( 0 )
{
   TheColorManagementSetupInterface = this;
}

ColorManagementSetupInterface::~ColorManagementSetupInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString ColorManagementSetupInterface::Id() const
{
   return "ColorManagementSetup";
}

MetaProcess* ColorManagementSetupInterface::Process() const
{
   return TheColorManagementSetupProcess;
}

const char** ColorManagementSetupInterface::IconImageXPM() const
{
   return ColorManagementSetupIcon_XPM;
}

InterfaceFeatures ColorManagementSetupInterface::Features() const
{
   return InterfaceFeature::DefaultGlobal;
}

void ColorManagementSetupInterface::Initialize()
{
   // ### We need to initialize the instance here because it couldn't be
   //     completely initialized in the class constructor. This is because
   //     ColorManagementSetupInstance needs to invoke member functions of the
   //     PixInsightSettings class to retrieve the current values of global
   //     settings variables.
   //     See also the constructor of ColorManagementSetupInstance.

   instance = ColorManagementSetupInstance( TheColorManagementSetupProcess );
}

void ColorManagementSetupInterface::ApplyInstance() const
{
   instance.LaunchGlobal();
}

void ColorManagementSetupInterface::ResetInstance()
{
   ColorManagementSetupInstance defaultInstance( TheColorManagementSetupProcess );
   ImportProcess( defaultInstance );
}

bool ColorManagementSetupInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   // ### Deferred initialization
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "ColorManagementSetup" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheColorManagementSetupProcess;
}

ProcessImplementation* ColorManagementSetupInterface::NewProcess() const
{
   return new ColorManagementSetupInstance( instance );
}

bool ColorManagementSetupInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   const ColorManagementSetupInstance* r = dynamic_cast<const ColorManagementSetupInstance*>( &p );

   if ( r == 0 )
   {
      whyNot = "Not a ColorManagementSetup instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool ColorManagementSetupInterface::RequiresInstanceValidation() const
{
   return true;
}

bool ColorManagementSetupInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ColorManagementSetupInterface::UpdateControls()
{
   if ( rgbProfiles.IsEmpty() || grayProfiles.IsEmpty() )
      RefreshProfiles();

   String monitorProfilePath = PixInsightSettings::GlobalString( "ColorManagement/MonitorProfilePath" );

   GUI->MonitorProfile_Edit.SetText( ICCProfile( monitorProfilePath ).Description() );
   GUI->MonitorProfileFullPath_Edit.SetText( monitorProfilePath );

   GUI->RenderingIntent_ComboBox.SetCurrentItem( instance.defaultRenderingIntent );

#ifdef __PCL_X11
   GUI->NewMonitorProfile_Edit.SetText( instance.updateMonitorProfile );
   GUI->NewMonitorProfile_ComboBox.SetCurrentItem( 0 );
#endif

   GUI->RGBProfileId_Edit.SetText( instance.defaultRGBProfile );
   GUI->RGBProfile_ComboBox.SetCurrentItem( 0 );

   GUI->GrayscaleProfileId_Edit.SetText( instance.defaultGrayProfile );
   GUI->GrayscaleProfile_ComboBox.SetCurrentItem( 0 );

   GUI->OnProfileMismatch_Ask_RadioButton.SetChecked( instance.onProfileMismatch == CMSOnProfileMismatch::AskUser );
   GUI->OnProfileMismatch_Keep_RadioButton.SetChecked( instance.onProfileMismatch == CMSOnProfileMismatch::KeepEmbedded );
   GUI->OnProfileMismatch_Convert_RadioButton.SetChecked( instance.onProfileMismatch == CMSOnProfileMismatch::ConvertToDefault );
   GUI->OnProfileMismatch_Discard_RadioButton.SetChecked( instance.onProfileMismatch == CMSOnProfileMismatch::DiscardEmbedded );
   GUI->OnProfileMismatch_Disable_RadioButton.SetChecked( instance.onProfileMismatch == CMSOnProfileMismatch::DisableCM );

   GUI->OnMissingProfile_Ask_RadioButton.SetChecked( instance.onMissingProfile == CMSOnMissingProfile::AskUser );
   GUI->OnMissingProfile_Assign_RadioButton.SetChecked( instance.onMissingProfile == CMSOnMissingProfile::AssignDefault );
   GUI->OnMissingProfile_Leave_RadioButton.SetChecked( instance.onMissingProfile == CMSOnMissingProfile::LeaveUntagged );
   GUI->OnMissingProfile_Disable_RadioButton.SetChecked( instance.onMissingProfile == CMSOnMissingProfile::DisableCM );

   GUI->ProofingProfileId_Edit.SetText( instance.proofingProfile );
   GUI->ProofingProfile_ComboBox.SetCurrentItem( 0 );

   GUI->ProofingIntent_ComboBox.SetCurrentItem( instance.proofingIntent );

   GUI->UseProofingBPC_CheckBox.SetChecked( instance.useProofingBPC );

   GUI->DefaultProofingEnabled_CheckBox.SetChecked( instance.defaultProofingEnabled );
   GUI->DefaultGamutCheckEnabled_CheckBox.SetChecked( instance.defaultGamutCheckEnabled );

   GUI->GamutWarningColor_ComboBox.SetCurrentColor( instance.gamutWarningColor );
   GUI->GamutWarningColor_Control.Update();

   GUI->EnableColorManagement_CheckBox.SetChecked( instance.enabled );
   GUI->EmbedProfilesInRGBImages_CheckBox.SetChecked( instance.defaultEmbedProfilesInRGBImages );
   GUI->EmbedProfilesInGrayscaleImages_CheckBox.SetChecked( instance.defaultEmbedProfilesInGrayscaleImages );
}

void ColorManagementSetupInterface::RefreshProfiles()
{
   rgbProfiles = ICCProfile::FindProfilesByColorSpace( ICCColorSpace::RGB );

   GUI->RGBProfile_ComboBox.Clear();
   GUI->RGBProfile_ComboBox.AddItem( String().Format( "--> Available RGB ICC Profiles (%u) <--", rgbProfiles.Length() ) );
#ifdef __PCL_X11
   GUI->NewMonitorProfile_ComboBox.Clear();
   GUI->NewMonitorProfile_ComboBox.AddItem( String().Format( "--> Available RGB ICC Profiles (%u) <--", rgbProfiles.Length() ) );
#endif
   for ( ICCProfile::profile_list::const_iterator i = rgbProfiles.Begin(); i != rgbProfiles.End(); ++i )
   {
      GUI->RGBProfile_ComboBox.AddItem( i->description );
#ifdef __PCL_X11
      GUI->NewMonitorProfile_ComboBox.AddItem( i->description );
#endif
   }

   grayProfiles = ICCProfile::FindProfilesByColorSpace( ICCColorSpace::Gray|ICCColorSpace::RGB );

   GUI->GrayscaleProfile_ComboBox.Clear();
   GUI->GrayscaleProfile_ComboBox.AddItem( String().Format( "--> Available RGB/Gray ICC Profiles (%u) <--", grayProfiles.Length() ) );
   for ( ICCProfile::profile_list::const_iterator i = grayProfiles.Begin(); i != grayProfiles.End(); ++i )
      GUI->GrayscaleProfile_ComboBox.AddItem( i->description );

   proofingProfiles = ICCProfile::FindProfilesByColorSpace( ICCColorSpace::Any );

   GUI->ProofingProfile_ComboBox.Clear();
   GUI->ProofingProfile_ComboBox.AddItem( String().Format( "--> Available ICC Profiles (%u) <--", proofingProfiles.Length() ) );
   for ( ICCProfile::profile_list::const_iterator i = proofingProfiles.Begin(); i != proofingProfiles.End(); ++i )
      GUI->ProofingProfile_ComboBox.AddItem( i->description );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ColorManagementSetupInterface::__RenderingIntent_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->RenderingIntent_ComboBox )
      instance.defaultRenderingIntent = itemIndex;
   else if ( sender == GUI->ProofingIntent_ComboBox )
      instance.proofingIntent = itemIndex;
}

void ColorManagementSetupInterface::__Profile_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->RGBProfile_ComboBox )
   {
      if ( itemIndex > 0 && itemIndex <= int( rgbProfiles.Length() ) ) // first item is title
      {
         instance.defaultRGBProfile = rgbProfiles[itemIndex-1].description; // skip the first title item
         UpdateControls();
      }
   }
   else if ( sender == GUI->GrayscaleProfile_ComboBox )
   {
      if ( itemIndex > 0 && itemIndex <= int( grayProfiles.Length() ) ) // first item is title
      {
         instance.defaultGrayProfile = grayProfiles[itemIndex-1].description; // skip the first title item
         UpdateControls();
      }
   }
   else if ( sender == GUI->ProofingProfile_ComboBox )
   {
      if ( itemIndex > 0 && itemIndex <= int( proofingProfiles.Length() ) ) // first item is title
      {
         instance.proofingProfile = proofingProfiles[itemIndex-1].description; // skip the first title item
         UpdateControls();
      }
   }
#ifdef __PCL_X11
   else if ( sender == GUI->NewMonitorProfile_ComboBox )
   {
      if ( itemIndex > 0 && itemIndex <= int( rgbProfiles.Length() ) ) // first item is title
      {
         instance.updateMonitorProfile = rgbProfiles[itemIndex-1].description; // skip the first title item
         UpdateControls();
      }
   }
#endif
}

void ColorManagementSetupInterface::__Profile_EditCompleted( Edit& sender )
{
   String txt = sender.Text();
   txt.Trim();

   if ( sender == GUI->RGBProfileId_Edit )
      instance.defaultRGBProfile = txt;
   else if ( sender == GUI->GrayscaleProfileId_Edit )
      instance.defaultGrayProfile = txt;
   else if ( sender == GUI->ProofingProfileId_Edit )
      instance.proofingProfile = txt;
#ifdef __PCL_X11
   else if ( sender == GUI->NewMonitorProfile_Edit )
      instance.updateMonitorProfile = txt;
#endif

   UpdateControls();
}

void ColorManagementSetupInterface::__OnProfileMismatch_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->OnProfileMismatch_Ask_RadioButton )
      instance.onProfileMismatch = CMSOnProfileMismatch::AskUser;
   else if ( sender == GUI->OnProfileMismatch_Keep_RadioButton )
      instance.onProfileMismatch = CMSOnProfileMismatch::KeepEmbedded;
   else if ( sender == GUI->OnProfileMismatch_Convert_RadioButton )
      instance.onProfileMismatch = CMSOnProfileMismatch::ConvertToDefault;
   else if ( sender == GUI->OnProfileMismatch_Discard_RadioButton )
      instance.onProfileMismatch = CMSOnProfileMismatch::DiscardEmbedded;
   else if ( sender == GUI->OnProfileMismatch_Disable_RadioButton )
      instance.onProfileMismatch = CMSOnProfileMismatch::DisableCM;
}

void ColorManagementSetupInterface::__OnMissingProfile_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->OnMissingProfile_Ask_RadioButton )
      instance.onMissingProfile = CMSOnMissingProfile::AskUser;
   else if ( sender == GUI->OnMissingProfile_Assign_RadioButton )
      instance.onMissingProfile = CMSOnMissingProfile::AssignDefault;
   else if ( sender == GUI->OnMissingProfile_Leave_RadioButton )
      instance.onMissingProfile = CMSOnMissingProfile::LeaveUntagged;
   else if ( sender == GUI->OnMissingProfile_Disable_RadioButton )
      instance.onMissingProfile = CMSOnMissingProfile::DisableCM;
}

void ColorManagementSetupInterface::__ProofingOptions_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->UseProofingBPC_CheckBox )
      instance.useProofingBPC = checked;
   else if ( sender == GUI->DefaultProofingEnabled_CheckBox )
      instance.defaultProofingEnabled = checked;
   else if ( sender == GUI->DefaultGamutCheckEnabled_CheckBox )
      instance.defaultGamutCheckEnabled = checked;
}

void ColorManagementSetupInterface::__ColorSelected( ColorComboBox& /*sender*/, RGBA color )
{
   instance.gamutWarningColor = uint32( color );
   GUI->GamutWarningColor_Control.Update();
}

void ColorManagementSetupInterface::__ColorSample_Paint( Control& sender, const Rect& /*updateRect*/ )
{
   Graphics g( sender );
   g.SetBrush( instance.gamutWarningColor );
   g.SetPen( RGBAColor( 0, 0, 0 ) );
   g.DrawRect( sender.BoundsRect() );
}

void ColorManagementSetupInterface::__ColorSample_MouseRelease(
   Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( sender.IsUnderMouse() )
   {
      SimpleColorDialog dlg;
      dlg.SetColor( instance.gamutWarningColor );
      if ( dlg.Execute() == StdDialogCode::Ok )
      {
         instance.gamutWarningColor = dlg.Color();
         GUI->GamutWarningColor_ComboBox.SetCurrentColor( instance.gamutWarningColor );
         GUI->GamutWarningColor_Control.Update();
      }
   }
}

void ColorManagementSetupInterface::__GlobalOptions_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->EnableColorManagement_CheckBox )
      instance.enabled = checked;
   else if ( sender == GUI->EmbedProfilesInRGBImages_CheckBox )
      instance.defaultEmbedProfilesInRGBImages = checked;
   else if ( sender == GUI->EmbedProfilesInGrayscaleImages_CheckBox )
      instance.defaultEmbedProfilesInGrayscaleImages = checked;
}

void ColorManagementSetupInterface::__RefreshProfiles_ButtonClick( Button& sender, bool checked )
{
   RefreshProfiles();
}

void ColorManagementSetupInterface::__LoadCurrentSettings_ButtonClick( Button& sender, bool checked )
{
   instance.LoadCurrentSettings();
   UpdateControls();
}

// ----------------------------------------------------------------------------

ColorManagementSetupInterface::GUIData::GUIData( ColorManagementSetupInterface& w )
{
   pcl::Font fnt = w.Font();
   int editWidth1 = fnt.Width( String( 'M', 50 ) );
   int labelWidth1 = fnt.Width( "Rendering intent:" );

   // -------------------------------------------------------------------------

   MonitorProfile_SectionBar.SetTitle( "Monitor Profile" );
   MonitorProfile_SectionBar.SetSection( MonitorProfile_Control );

   //

   MonitorProfile_Edit.SetReadOnly();
   MonitorProfile_Edit.SetToolTip( "<p>This is the ICC profile currently associated to the primary monitor.</p>" );

   MonitorProfileFullPath_Edit.SetReadOnly();
   MonitorProfileFullPath_Edit.SetMinWidth( editWidth1 );

   RenderingIntent_Label.SetText( "Rendering intent:" );
   RenderingIntent_Label.SetMinWidth( labelWidth1 );
   RenderingIntent_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   RenderingIntent_ComboBox.AddItem( "Perceptual (photographic images)" );
   RenderingIntent_ComboBox.AddItem( "Saturation (graphics)" );
   RenderingIntent_ComboBox.AddItem( "Relative Colorimetric (match white points)" );
   RenderingIntent_ComboBox.AddItem( "Absolute Colorimetric (proofing)" );
   RenderingIntent_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ColorManagementSetupInterface::__RenderingIntent_ItemSelected, w );

   RenderingIntent_Sizer.SetSpacing( 4 );
   RenderingIntent_Sizer.Add( RenderingIntent_Label );
   RenderingIntent_Sizer.Add( RenderingIntent_ComboBox, 100 );

   //

   MonitorProfile_Sizer.SetSpacing( 4 );
   MonitorProfile_Sizer.Add( MonitorProfile_Edit );
   MonitorProfile_Sizer.Add( MonitorProfileFullPath_Edit );
   MonitorProfile_Sizer.Add( RenderingIntent_Sizer );

   MonitorProfile_Control.SetSizer( MonitorProfile_Sizer );

   // -------------------------------------------------------------------------

#ifdef __PCL_X11

   SystemSettings_SectionBar.SetTitle( "System Settings (X11 Only)" );
   SystemSettings_SectionBar.SetSection( SystemSettings_Control );

   //

   const char* newMonitorProfileTip =
      "<p>This section allows you to change the current monitor ICC profile (Linux/UNIX only). If "
      "you change this item, a new monitor profile will be scheduled for installation upon "
      "application restart.</p>"
      "<p><b>Warning: Setting an incorrect monitor profile will invalidate the results of the "
      "whole color management subsystem. Change this setting only if you know what you are doing.</b></p>";

   NewMonitorProfile_Edit.SetMinWidth( editWidth1 );
   NewMonitorProfile_Edit.SetToolTip( newMonitorProfileTip );
   NewMonitorProfile_Edit.OnEditCompleted( (Edit::edit_event_handler)&ColorManagementSetupInterface::__Profile_EditCompleted, w );

   NewMonitorProfile_ComboBox.SetToolTip( newMonitorProfileTip );
   NewMonitorProfile_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ColorManagementSetupInterface::__Profile_ItemSelected, w );

   NewMonitorProfile_Sizer.SetMargin( 6 );
   NewMonitorProfile_Sizer.SetSpacing( 4 );
   NewMonitorProfile_Sizer.Add( NewMonitorProfile_Edit );
   NewMonitorProfile_Sizer.Add( NewMonitorProfile_ComboBox );

   NewMonitorProfile_GroupBox.SetTitle( "New Monitor Profile" );
   NewMonitorProfile_GroupBox.SetToolTip( newMonitorProfileTip );
   NewMonitorProfile_GroupBox.SetSizer( NewMonitorProfile_Sizer );

   SystemSettings_Sizer.SetSpacing( 4 );
   SystemSettings_Sizer.Add( NewMonitorProfile_GroupBox );

   SystemSettings_Control.SetSizer( SystemSettings_Sizer );

#endif   // __PCL_X11

   // -------------------------------------------------------------------------

   DefaultProfiles_SectionBar.SetTitle( "Default Profiles" );
   DefaultProfiles_SectionBar.SetSection( DefaultProfiles_Control );

   //

   const char* rgbProfileTip =
      "<p>This is the default ICC profile that will be used for newly created RGB color images.</p>";

   RGBProfileId_Edit.SetMinWidth( editWidth1 );
   RGBProfileId_Edit.SetToolTip( rgbProfileTip );
   RGBProfileId_Edit.OnEditCompleted( (Edit::edit_event_handler)&ColorManagementSetupInterface::__Profile_EditCompleted, w );

   RGBProfile_ComboBox.SetToolTip( rgbProfileTip );
   RGBProfile_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ColorManagementSetupInterface::__Profile_ItemSelected, w );

   RGBProfiles_Sizer.SetMargin( 6 );
   RGBProfiles_Sizer.SetSpacing( 4 );
   RGBProfiles_Sizer.Add( RGBProfileId_Edit );
   RGBProfiles_Sizer.Add( RGBProfile_ComboBox );

   RGBProfiles_GroupBox.SetTitle( "RGB" );
   RGBProfiles_GroupBox.SetToolTip( rgbProfileTip );
   RGBProfiles_GroupBox.SetSizer( RGBProfiles_Sizer );

   //

   const char* grayProfileTip =
      "<p>This is the default ICC profile that will be used for newly created grayscale images.</p>";

   GrayscaleProfileId_Edit.SetMinWidth( editWidth1 );
   GrayscaleProfileId_Edit.SetToolTip( grayProfileTip );
   GrayscaleProfileId_Edit.OnEditCompleted( (Edit::edit_event_handler)&ColorManagementSetupInterface::__Profile_EditCompleted, w );

   GrayscaleProfile_ComboBox.SetToolTip( grayProfileTip );
   GrayscaleProfile_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ColorManagementSetupInterface::__Profile_ItemSelected, w );

   GrayscaleProfiles_Sizer.SetMargin( 6 );
   GrayscaleProfiles_Sizer.SetSpacing( 4 );
   GrayscaleProfiles_Sizer.Add( GrayscaleProfileId_Edit );
   GrayscaleProfiles_Sizer.Add( GrayscaleProfile_ComboBox );

   GrayscaleProfiles_GroupBox.SetTitle( "Grayscale" );
   GrayscaleProfiles_GroupBox.SetToolTip( grayProfileTip );
   GrayscaleProfiles_GroupBox.SetSizer( GrayscaleProfiles_Sizer );

   //

   DefaultProfiles_Sizer.SetSpacing( 4 );
   DefaultProfiles_Sizer.Add( RGBProfiles_GroupBox );
   DefaultProfiles_Sizer.Add( GrayscaleProfiles_GroupBox );

   DefaultProfiles_Control.SetSizer( DefaultProfiles_Sizer );

   // -------------------------------------------------------------------------

   DefaultPolicies_SectionBar.SetTitle( "Default Policies" );
   DefaultPolicies_SectionBar.SetSection( DefaultPolicies_Control );

   //

   OnProfileMismatch_Ask_RadioButton.SetText( "Ask what to do" );
   OnProfileMismatch_Ask_RadioButton.SetToolTip(
      "<p>Lets you take a decision for each image that embeds a mismatching ICC profile.</p>" );
   OnProfileMismatch_Ask_RadioButton.OnClick( (Button::click_event_handler)&ColorManagementSetupInterface::__OnProfileMismatch_ButtonClick, w );

   OnProfileMismatch_Keep_RadioButton.SetText( "Keep embedded profiles" );
   OnProfileMismatch_Keep_RadioButton.SetToolTip(
      "<p>Keep mismatching embedded profiles and use them to manage image renditions.</p>" );
   OnProfileMismatch_Keep_RadioButton.OnClick( (Button::click_event_handler)&ColorManagementSetupInterface::__OnProfileMismatch_ButtonClick, w );

   OnProfileMismatch_Convert_RadioButton.SetText( "Convert to the default profile" );
   OnProfileMismatch_Convert_RadioButton.SetToolTip(
      "<p>Convert image data to the default ICC profile.</p>"
      "<p><b>WARNING: This option modifies pixel values.</b></p>" );
   OnProfileMismatch_Convert_RadioButton.OnClick( (Button::click_event_handler)&ColorManagementSetupInterface::__OnProfileMismatch_ButtonClick, w );

   OnProfileMismatch_Discard_RadioButton.SetText( "Discard mismatching profiles" );
   OnProfileMismatch_Discard_RadioButton.SetToolTip(
      "<p>Ignore mismatching embedded profiles and use the default profile instead.</p>" );
   OnProfileMismatch_Discard_RadioButton.OnClick( (Button::click_event_handler)&ColorManagementSetupInterface::__OnProfileMismatch_ButtonClick, w );

   OnProfileMismatch_Disable_RadioButton.SetText( "Disable color management" );
   OnProfileMismatch_Disable_RadioButton.SetToolTip(
      "<p>Do not manage color for images that embed a mismatching ICC profile.</p>" );
   OnProfileMismatch_Disable_RadioButton.OnClick( (Button::click_event_handler)&ColorManagementSetupInterface::__OnProfileMismatch_ButtonClick, w );

   OnProfileMismatch_Sizer.SetMargin( 6 );
   OnProfileMismatch_Sizer.SetSpacing( 4 );
   OnProfileMismatch_Sizer.Add( OnProfileMismatch_Ask_RadioButton );
   OnProfileMismatch_Sizer.Add( OnProfileMismatch_Keep_RadioButton );
   OnProfileMismatch_Sizer.Add( OnProfileMismatch_Convert_RadioButton );
   OnProfileMismatch_Sizer.Add( OnProfileMismatch_Discard_RadioButton );
   OnProfileMismatch_Sizer.Add( OnProfileMismatch_Disable_RadioButton );

   OnProfileMismatch_GroupBox.SetTitle( "On Profile Mismatch" );
   OnProfileMismatch_GroupBox.SetSizer( OnProfileMismatch_Sizer );

   //

   OnMissingProfile_Ask_RadioButton.SetText( "Ask what to do" );
   OnMissingProfile_Ask_RadioButton.SetToolTip(
      "<p>Lets you take a decision for each image that does not embed an ICC profile.</p>" );
   OnMissingProfile_Ask_RadioButton.OnClick( (Button::click_event_handler)&ColorManagementSetupInterface::__OnMissingProfile_ButtonClick, w );

   OnMissingProfile_Assign_RadioButton.SetText( "Assign the default profile" );
   OnMissingProfile_Assign_RadioButton.SetToolTip(
      "<p>Assign the default profile to all images without an embedded ICC profile.</p>"
      "<p>This assumes that untagged images have their pixel values expressed in the default profile space.</p>" );
   OnMissingProfile_Assign_RadioButton.OnClick( (Button::click_event_handler)&ColorManagementSetupInterface::__OnMissingProfile_ButtonClick, w );

   OnMissingProfile_Leave_RadioButton.SetText( "Leave the image untagged" );
   OnMissingProfile_Leave_RadioButton.SetToolTip(
      "<p>Assume that all images without an embedded ICC profile have their pixel values expressed in "
      "the default profile space, but don't assign the default profile explicitly.</p>" );
   OnMissingProfile_Leave_RadioButton.OnClick( (Button::click_event_handler)&ColorManagementSetupInterface::__OnMissingProfile_ButtonClick, w );

   OnMissingProfile_Disable_RadioButton.SetText( "Disable color management" );
   OnMissingProfile_Disable_RadioButton.SetToolTip(
      "<p>Do not manage color for images that don't embed an ICC profile.</p>" );
   OnMissingProfile_Disable_RadioButton.OnClick( (Button::click_event_handler)&ColorManagementSetupInterface::__OnMissingProfile_ButtonClick, w );

   OnMissingProfile_Sizer.SetMargin( 6 );
   OnMissingProfile_Sizer.SetSpacing( 4 );
   OnMissingProfile_Sizer.Add( OnMissingProfile_Ask_RadioButton );
   OnMissingProfile_Sizer.AddStretch();
   OnMissingProfile_Sizer.Add( OnMissingProfile_Assign_RadioButton );
   OnMissingProfile_Sizer.Add( OnMissingProfile_Leave_RadioButton );
   OnMissingProfile_Sizer.Add( OnMissingProfile_Disable_RadioButton );

   OnMissingProfile_GroupBox.SetTitle( "On Missing Profile" );
   OnMissingProfile_GroupBox.SetSizer( OnMissingProfile_Sizer );

   //

   DefaultPolicies_Sizer.SetSpacing( 8 );
   DefaultPolicies_Sizer.Add( OnProfileMismatch_GroupBox, 50 );
   DefaultPolicies_Sizer.Add( OnMissingProfile_GroupBox, 50 );

   DefaultPolicies_Control.SetSizer( DefaultPolicies_Sizer );

   // -------------------------------------------------------------------------

   Proofing_SectionBar.SetTitle( "Color Proofing" );
   Proofing_SectionBar.SetSection( Proofing_Control );

   //

   const char* proofingProfileTip =
      "<p>This is the ICC profile that will be used for color proofing.</p>";

   ProofingProfileId_Edit.SetMinWidth( editWidth1 );
   ProofingProfileId_Edit.SetToolTip( proofingProfileTip );
   ProofingProfileId_Edit.OnEditCompleted( (Edit::edit_event_handler)&ColorManagementSetupInterface::__Profile_EditCompleted, w );

   ProofingProfile_ComboBox.SetToolTip( proofingProfileTip );
   ProofingProfile_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ColorManagementSetupInterface::__Profile_ItemSelected, w );

   ProofingProfile_Sizer.SetMargin( 6 );
   ProofingProfile_Sizer.SetSpacing( 4 );
   ProofingProfile_Sizer.Add( ProofingProfileId_Edit );
   ProofingProfile_Sizer.Add( ProofingProfile_ComboBox );

   ProofingProfile_GroupBox.SetTitle( "Proofing Profile" );
   ProofingProfile_GroupBox.SetToolTip( proofingProfileTip );
   ProofingProfile_GroupBox.SetSizer( ProofingProfile_Sizer );

   //

   ProofingIntent_Label.SetText( "Proofing intent:" );
   ProofingIntent_Label.SetMinWidth( labelWidth1 );
   ProofingIntent_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ProofingIntent_ComboBox.AddItem( " Perceptual (photographic images) " );
   ProofingIntent_ComboBox.AddItem( " Saturation (graphics) " );
   ProofingIntent_ComboBox.AddItem( " Relative Colorimetric (match white points) " );
   ProofingIntent_ComboBox.AddItem( " Absolute Colorimetric (proofing) " );
   ProofingIntent_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ColorManagementSetupInterface::__RenderingIntent_ItemSelected, w );

   ProofingIntent_Sizer.SetSpacing( 4 );
   ProofingIntent_Sizer.Add( ProofingIntent_Label );
   ProofingIntent_Sizer.Add( ProofingIntent_ComboBox, 100 );

   //

   UseProofingBPC_CheckBox.SetText( "Use black point compensation" );
   UseProofingBPC_CheckBox.SetToolTip(
      "<p>Apply a black point compensation (BPC) algorithm to generate proofing renditions. "
      "Usually BPC helps in preserving shadow detail.</p>" );
   UseProofingBPC_CheckBox.OnClick( (Button::click_event_handler)&ColorManagementSetupInterface::__ProofingOptions_ButtonClick, w );

   UseProofingBPC_Sizer.AddSpacing( labelWidth1 + 4 );
   UseProofingBPC_Sizer.Add( UseProofingBPC_CheckBox, 100 );

   //

   DefaultProofingEnabled_CheckBox.SetText( "Default proofing enabled" );
   DefaultProofingEnabled_CheckBox.SetToolTip(
      "<p>When this option is selected, newly created and just opened images will have "
      "the <i>color proofing</i> feature enabled by default.</p>" );
   DefaultProofingEnabled_CheckBox.OnClick( (Button::click_event_handler)&ColorManagementSetupInterface::__ProofingOptions_ButtonClick, w );

   DefaultProofingEnabled_Sizer.AddSpacing( labelWidth1 + 4 );
   DefaultProofingEnabled_Sizer.Add( DefaultProofingEnabled_CheckBox, 100 );

   //

   DefaultGamutCheckEnabled_CheckBox.SetText( "Default gamut check enabled" );
   DefaultGamutCheckEnabled_CheckBox.SetToolTip(
      "<p>When this option is selected, newly created and just opened images will have "
      "the <i>gamut check</i> feature enabled by default.</p>" );
   DefaultGamutCheckEnabled_CheckBox.OnClick( (Button::click_event_handler)&ColorManagementSetupInterface::__ProofingOptions_ButtonClick, w );

   DefaultGamutCheckEnabled_Sizer.AddSpacing( labelWidth1 + 4 );
   DefaultGamutCheckEnabled_Sizer.Add( DefaultGamutCheckEnabled_CheckBox, 100 );

   //

   static const char* gamutWarningTip =
      "<p>The <i>gamut warning color</i> is used to represent out-of-gamut pixel values on image "
      "renditions when the color proofing and gamut check options are enabled.</p>";

   GamutWarningColor_Label.SetText( "Gamut Warning:" );
   GamutWarningColor_Label.SetMinWidth( labelWidth1 );
   GamutWarningColor_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   GamutWarningColor_Label.SetToolTip( gamutWarningTip );

   GamutWarningColor_ComboBox.OnColorSelected( (ColorComboBox::color_event_handler)&ColorManagementSetupInterface::__ColorSelected, w );
   GamutWarningColor_ComboBox.SetToolTip( gamutWarningTip );

   GamutWarningColor_Control.SetMinWidth( 30 );
   GamutWarningColor_Control.SetCursor( StdCursor::UpArrow );
   GamutWarningColor_Control.SetToolTip(gamutWarningTip );
   GamutWarningColor_Control.OnPaint( (Control::paint_event_handler)&ColorManagementSetupInterface::__ColorSample_Paint, w );
   GamutWarningColor_Control.OnMouseRelease( (Control::mouse_button_event_handler)&ColorManagementSetupInterface::__ColorSample_MouseRelease, w );

   GamutWarningColor_Sizer.SetSpacing( 4 );
   GamutWarningColor_Sizer.Add( GamutWarningColor_Label );
   GamutWarningColor_Sizer.Add( GamutWarningColor_ComboBox, 100 );
   GamutWarningColor_Sizer.Add( GamutWarningColor_Control );

   //

   Proofing_Sizer.SetSpacing( 4 );
   Proofing_Sizer.Add( ProofingProfile_GroupBox );
   Proofing_Sizer.Add( ProofingIntent_Sizer );
   Proofing_Sizer.Add( UseProofingBPC_Sizer );
   Proofing_Sizer.Add( DefaultProofingEnabled_Sizer );
   Proofing_Sizer.Add( DefaultGamutCheckEnabled_Sizer );
   Proofing_Sizer.Add( GamutWarningColor_Sizer );

   Proofing_Control.SetSizer( Proofing_Sizer );

   // -------------------------------------------------------------------------

   GlobalOptions_SectionBar.SetTitle( "Global Options" );
   GlobalOptions_SectionBar.SetSection( GlobalOptions_Control );

   //

   EnableColorManagement_CheckBox.SetText( "Enable Color Management" );
   EnableColorManagement_CheckBox.SetToolTip(
      "<p>Globally enable or disable color management.</p>"
      "<p>Note that color management can also be enabled/disabled individually "
      "for each image window.</p>" );
   EnableColorManagement_CheckBox.OnClick( (Button::click_event_handler)&ColorManagementSetupInterface::__GlobalOptions_ButtonClick, w );

   static const char* embedTip =
      "<p>Globally enable or disable default embedding of ICC profiles in newly generated RGB color "
      "or grayscale image files.</p>"
      "<p>Note that default profile embedding can be enabled/disabled for individual file formats, "
      "and that a last chance to embed or not an ICC profile is given by every format options dialog.</p>";

   EmbedProfilesInRGBImages_CheckBox.SetText( "Embed ICC profiles in RGB images" );
   EmbedProfilesInRGBImages_CheckBox.SetToolTip( embedTip );
   EmbedProfilesInRGBImages_CheckBox.OnClick( (Button::click_event_handler)&ColorManagementSetupInterface::__GlobalOptions_ButtonClick, w );

   EmbedProfilesInGrayscaleImages_CheckBox.SetText( "Embed ICC profiles in grayscale images" );
   EmbedProfilesInGrayscaleImages_CheckBox.SetToolTip( embedTip );
   EmbedProfilesInGrayscaleImages_CheckBox.OnClick( (Button::click_event_handler)&ColorManagementSetupInterface::__GlobalOptions_ButtonClick, w );

   EmbedProfiles_Sizer.SetSpacing( 4 );
   EmbedProfiles_Sizer.Add( EmbedProfilesInRGBImages_CheckBox );
   EmbedProfiles_Sizer.Add( EmbedProfilesInGrayscaleImages_CheckBox );

   GlobalOptions_Sizer.SetMargin( 6 );
   GlobalOptions_Sizer.SetSpacing( 4 );
   GlobalOptions_Sizer.Add( EnableColorManagement_CheckBox );
   GlobalOptions_Sizer.Add( EmbedProfiles_Sizer );

   GlobalOptions_Control.SetSizer( GlobalOptions_Sizer );

   //

   RefreshProfiles_PushButton.SetText( "Refresh Profiles" );
   RefreshProfiles_PushButton.SetToolTip(
      "<p>Rescan the system color directory to find all installed ICC profiles.</p>" );
   RefreshProfiles_PushButton.OnClick( (Button::click_event_handler)&ColorManagementSetupInterface::__RefreshProfiles_ButtonClick, w );

   LoadCurrentSettings_PushButton.SetText( "Load Current Settings" );
   LoadCurrentSettings_PushButton.SetToolTip(
      "<p>Load this window with the current global color management settings.</p>" );
   LoadCurrentSettings_PushButton.OnClick( (Button::click_event_handler)&ColorManagementSetupInterface::__LoadCurrentSettings_ButtonClick, w );

   GlobalActions_Sizer.Add( RefreshProfiles_PushButton );
   GlobalActions_Sizer.AddStretch();
   GlobalActions_Sizer.Add( LoadCurrentSettings_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( MonitorProfile_SectionBar );
   Global_Sizer.Add( MonitorProfile_Control );
#ifdef __PCL_X11
   Global_Sizer.Add( SystemSettings_SectionBar );
   Global_Sizer.Add( SystemSettings_Control );
#endif
   Global_Sizer.Add( DefaultProfiles_SectionBar );
   Global_Sizer.Add( DefaultProfiles_Control );
   Global_Sizer.Add( DefaultPolicies_SectionBar );
   Global_Sizer.Add( DefaultPolicies_Control );
   Global_Sizer.Add( Proofing_SectionBar );
   Global_Sizer.Add( Proofing_Control );
   Global_Sizer.Add( GlobalOptions_SectionBar );
   Global_Sizer.Add( GlobalOptions_Control );
   Global_Sizer.Add( GlobalActions_Sizer );

#ifdef __PCL_X11
   SystemSettings_Control.Hide();
#endif

   DefaultPolicies_Control.Hide();

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF ColorManagementSetupInterface.cpp - Released 2014/10/29 07:34:56 UTC
