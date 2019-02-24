//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0938
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 01.00.00.0359
// ----------------------------------------------------------------------------
// ICCProfileTransformationInterface.cpp - Released 2019-01-21T12:06:41Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
//
// Copyright (c) 2003-2019 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "ICCProfileTransformationInterface.h"
#include "ICCProfileTransformationProcess.h"
#include "ICCProfileTransformationParameters.h"

#include <pcl/ICCProfileTransformation.h>
#include <pcl/GlobalSettings.h>
#include <pcl/ImageWindow.h>
#include <pcl/ErrorHandler.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ICCProfileTransformationInterface* TheICCProfileTransformationInterface = nullptr;

// ----------------------------------------------------------------------------

#include "ICCProfileTransformationIcon.xpm"

// ----------------------------------------------------------------------------

#define currentView  GUI->AllImages_ViewList.CurrentView()

// ----------------------------------------------------------------------------

ICCProfileTransformationInterface::ICCProfileTransformationInterface() :
   instance( TheICCProfileTransformationProcess )
{
   TheICCProfileTransformationInterface = this;
}

ICCProfileTransformationInterface::~ICCProfileTransformationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString ICCProfileTransformationInterface::Id() const
{
   return "ICCProfileTransformation";
}

MetaProcess* ICCProfileTransformationInterface::Process() const
{
   return TheICCProfileTransformationProcess;
}

const char** ICCProfileTransformationInterface::IconImageXPM() const
{
   return ICCProfileTransformationIcon_XPM;
}

InterfaceFeatures ICCProfileTransformationInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::TrackViewButton;
}

void ICCProfileTransformationInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentWindow();
}

void ICCProfileTransformationInterface::TrackViewUpdated( bool active )
{
   if ( GUI != nullptr )
      if ( active )
      {
         ImageWindow w = ImageWindow::ActiveWindow();
         if ( !w.IsNull() )
            ImageFocused( w.MainView() );
         else
            UpdateControls();
      }
}

void ICCProfileTransformationInterface::ResetInstance()
{
   ICCProfileTransformationInstance defaultInstance( TheICCProfileTransformationProcess );
   ImportProcess( defaultInstance );
}

bool ICCProfileTransformationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "ICCProfileTransformation" );
      GUI->AllImages_ViewList.Regenerate( true/*mainViews*/, false/*previews*/ );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheICCProfileTransformationProcess;
}

ProcessImplementation* ICCProfileTransformationInterface::NewProcess() const
{
   return new ICCProfileTransformationInstance( instance );
}

bool ICCProfileTransformationInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const ICCProfileTransformationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an ICCProfileTransformation instance.";
   return false;
}

bool ICCProfileTransformationInterface::RequiresInstanceValidation() const
{
   return true;
}

bool ICCProfileTransformationInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

bool ICCProfileTransformationInterface::WantsImageNotifications() const
{
   return true;
}

void ICCProfileTransformationInterface::ImageUpdated( const View& v )
{
   if ( GUI != nullptr )
      if ( v == currentView )
         UpdateControls();
}

void ICCProfileTransformationInterface::ImageFocused( const View& v )
{
   if ( GUI != nullptr )
      if ( IsTrackViewActive() )
         if ( !v.IsNull() )
         {
            GUI->AllImages_ViewList.SelectView( v.Window().MainView() ); // normally not necessary, but we can invoke this f() directly
            UpdateControls();
         }
}

void ICCProfileTransformationInterface::ImageCMUpdated( const View& v )
{
   if ( GUI != nullptr )
      if ( v == currentView )
         UpdateControls();
}

bool ICCProfileTransformationInterface::WantsGlobalNotifications() const
{
   return true;
}

void ICCProfileTransformationInterface::GlobalCMUpdated()
{
   if ( GUI != nullptr )
   {
      RefreshDefaultProfiles();
      UpdateControls();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ICCProfileTransformationInterface::UpdateControls()
{
   if ( currentView.IsNull() )
      GUI->SourceProfile_Edit.Clear();
   else
   {
      ICCProfile icc;
      currentView.Window().GetICCProfile( icc );

      if ( icc.IsProfile() )
         GUI->SourceProfile_Edit.SetText( icc.Description() );
      else
      {
         String defaultProfilePath = PixInsightSettings::GlobalString( currentView.IsColor() ?
            "ColorManagement/DefaultRGBProfilePath" : "ColorManagement/DefaultGrayscaleProfilePath" );
         try
         {
            icc.Load( defaultProfilePath );
         }
         ERROR_HANDLER

         String text = "<* None *>";
         if ( icc.IsProfile() )
            text += " (" + icc.Description() + ')';
         GUI->SourceProfile_Edit.SetText( text );
      }
   }

   GUI->ToProfile_RadioButton.SetChecked( !instance.toDefaultProfile );

   GUI->TargetProfile_Edit.SetText( instance.targetProfile );

   if ( profiles.IsEmpty() )
      RefreshProfiles();

   GUI->AllProfiles_ComboBox.SetCurrentItem( 0 );

   GUI->ToDefaultProfile_RadioButton.SetChecked( instance.toDefaultProfile );

   if ( defaultRGBProfile.IsEmpty() || defaultGrayscaleProfile.IsEmpty() )
      RefreshDefaultProfiles();

   GUI->DefaultRGBProfile_Edit.SetText( defaultRGBProfile );
   GUI->DefaultGrayscaleProfile_Edit.SetText( defaultGrayscaleProfile );

   GUI->TargetProfile_Edit.Disable( instance.toDefaultProfile );
   GUI->AllProfiles_ComboBox.Disable( instance.toDefaultProfile );
   GUI->RefreshProfiles_ToolButton.Disable( instance.toDefaultProfile );

   GUI->DefaultRGBProfile_Label.Enable( instance.toDefaultProfile );
   GUI->DefaultRGBProfile_Edit.Enable( instance.toDefaultProfile );
   GUI->DefaultGrayscaleProfile_Label.Enable( instance.toDefaultProfile );
   GUI->DefaultGrayscaleProfile_Edit.Enable( instance.toDefaultProfile );

   GUI->RenderingIntent_ComboBox.SetCurrentItem( instance.renderingIntent );

   GUI->UseBlackPointCompensation_CheckBox.SetChecked( instance.useBlackPointCompensation );
   GUI->UseFloatingPointTransformation_CheckBox.SetChecked( instance.useFloatingPointTransformation );
}

void ICCProfileTransformationInterface::RefreshProfiles()
{
   profiles = ICCProfile::FindProfilesByColorSpace( ICCColorSpace::RGB|ICCColorSpace::Gray );

   GUI->AllProfiles_ComboBox.Clear();
   GUI->AllProfiles_ComboBox.AddItem( " --> Available ICC Profiles <-- " );
   for ( ICCProfile::profile_list::const_iterator i = profiles.Begin(); i != profiles.End(); ++i )
      GUI->AllProfiles_ComboBox.AddItem( i->description );
}

void ICCProfileTransformationInterface::RefreshDefaultProfiles()
{
   try
   {
      defaultRGBProfile = ICCProfile( PixInsightSettings::GlobalString(
                                       "ColorManagement/DefaultRGBProfilePath" ) ).Description();
      defaultGrayscaleProfile = ICCProfile( PixInsightSettings::GlobalString(
                                       "ColorManagement/DefaultGrayscaleProfilePath" ) ).Description();
   }
   ERROR_HANDLER
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ICCProfileTransformationInterface::__ViewList_ViewSelected( ViewList& /*sender*/, View& /*view*/ )
{
   UpdateControls();
}

void ICCProfileTransformationInterface::__TargetProfile_EditCompleted( Edit& sender )
{
   String txt = sender.Text().Trimmed();
   instance.targetProfile = txt;
   instance.toDefaultProfile = false;
   UpdateControls();
}

void ICCProfileTransformationInterface::__Profile_ItemSelected( ComboBox& /*sender*/, int itemIndex )
{
   if ( itemIndex > 0 )
      if ( itemIndex <= int( profiles.Length() ) ) // first item is title
      {
         instance.targetProfile = profiles[itemIndex-1].description; // skip the first title item
         instance.toDefaultProfile = false;
         UpdateControls();
      }
}

void ICCProfileTransformationInterface::__RefreshProfiles_ButtonClick( Button& /*sender*/, bool /*checked*/ )
{
   RefreshProfiles();
   UpdateControls();
}

void ICCProfileTransformationInterface::__ToProfile_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->ToProfile_RadioButton )
      instance.toDefaultProfile = !checked;
   else if ( sender == GUI->ToDefaultProfile_RadioButton )
      instance.toDefaultProfile = checked;
   UpdateControls();
}

void ICCProfileTransformationInterface::__RenderingIntent_ItemSelected( ComboBox& /*sender*/, int itemIndex )
{
   instance.renderingIntent = itemIndex;
   UpdateControls();
}

void ICCProfileTransformationInterface::__Option_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->UseBlackPointCompensation_CheckBox )
      instance.useBlackPointCompensation = checked;
   else if ( sender == GUI->UseFloatingPointTransformation_CheckBox )
      instance.useFloatingPointTransformation = checked;
}

void ICCProfileTransformationInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->AllImages_ViewList )
      wantsView = view.IsMainView();
}

void ICCProfileTransformationInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->AllImages_ViewList )
      if ( view.IsMainView() )
      {
         DeactivateTrackView();
         GUI->AllImages_ViewList.SelectView( view );
         UpdateControls();
      }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ICCProfileTransformationInterface::GUIData::GUIData( ICCProfileTransformationInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Grayscale:" ) );
   int editWidth1 = fnt.Width( String( 'M', 50 ) );

   //

   AllImages_ViewList.OnViewSelected( (ViewList::view_event_handler)&ICCProfileTransformationInterface::__ViewList_ViewSelected, w );
   AllImages_ViewList.OnViewDrag( (Control::view_drag_event_handler)&ICCProfileTransformationInterface::__ViewDrag, w );
   AllImages_ViewList.OnViewDrop( (Control::view_drop_event_handler)&ICCProfileTransformationInterface::__ViewDrop, w );

   SourceProfile_Edit.SetMinWidth( editWidth1 );
   SourceProfile_Edit.SetReadOnly();

   SourceProfile_Sizer.SetMargin( 6 );
   SourceProfile_Sizer.SetSpacing( 4 );
   SourceProfile_Sizer.Add( AllImages_ViewList );
   SourceProfile_Sizer.Add( SourceProfile_Edit );

   SourceProfile_GroupBox.SetTitle( "Source Profile" );
   SourceProfile_GroupBox.SetSizer( SourceProfile_Sizer );

   //

   ToProfile_RadioButton.SetText( "Convert to the specified profile:" );
   ToProfile_RadioButton.OnClick( (Button::click_event_handler)&ICCProfileTransformationInterface::__ToProfile_ButtonClick, w );

   TargetProfile_Edit.SetMinWidth( editWidth1 );
   TargetProfile_Edit.OnEditCompleted( (Edit::edit_event_handler)&ICCProfileTransformationInterface::__TargetProfile_EditCompleted, w );

   AllProfiles_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ICCProfileTransformationInterface::__Profile_ItemSelected, w );

   RefreshProfiles_ToolButton.SetIcon( w.ScaledResource( ":/icons/refresh.png" ) );
   RefreshProfiles_ToolButton.SetScaledFixedSize( 20, 20 );
   RefreshProfiles_ToolButton.SetToolTip( "Refresh profile list" );
   RefreshProfiles_ToolButton.OnClick( (Button::click_event_handler)&ICCProfileTransformationInterface::__RefreshProfiles_ButtonClick, w );

   AllProfiles_Sizer.SetSpacing( 4 );
   AllProfiles_Sizer.Add( AllProfiles_ComboBox, 100 );
   AllProfiles_Sizer.Add( RefreshProfiles_ToolButton );

   ToDefaultProfile_RadioButton.SetText( "Convert to the default profile:" );
   ToDefaultProfile_RadioButton.OnClick( (Button::click_event_handler)&ICCProfileTransformationInterface::__ToProfile_ButtonClick, w );

   DefaultRGBProfile_Label.SetText( "RGB:" );
   DefaultRGBProfile_Label.SetToolTip( "<p>Default ICC profile that will be used for RGB color images.</p>" );
   DefaultRGBProfile_Label.SetFixedWidth( labelWidth1 );
   DefaultRGBProfile_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   DefaultRGBProfile_Edit.SetMinWidth( editWidth1 );
   DefaultRGBProfile_Edit.SetReadOnly();

   DefaultRGBProfile_Sizer.SetSpacing( 4 );
   DefaultRGBProfile_Sizer.Add( DefaultRGBProfile_Label );
   DefaultRGBProfile_Sizer.Add( DefaultRGBProfile_Edit, 100 );

   DefaultGrayscaleProfile_Label.SetText( "Grayscale:" );
   DefaultGrayscaleProfile_Label.SetToolTip( "<p>Default ICC profile that will be used for grayscale color images.</p>" );
   DefaultGrayscaleProfile_Label.SetFixedWidth( labelWidth1 );
   DefaultGrayscaleProfile_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   DefaultGrayscaleProfile_Edit.SetMinWidth( editWidth1 );
   DefaultGrayscaleProfile_Edit.SetReadOnly();

   DefaultGrayscaleProfile_Sizer.SetSpacing( 4 );
   DefaultGrayscaleProfile_Sizer.Add( DefaultGrayscaleProfile_Label );
   DefaultGrayscaleProfile_Sizer.Add( DefaultGrayscaleProfile_Edit, 100 );

   TargetProfile_Sizer.SetMargin( 6 );
   TargetProfile_Sizer.SetSpacing( 4 );
   TargetProfile_Sizer.Add( ToProfile_RadioButton );
   TargetProfile_Sizer.Add( TargetProfile_Edit );
   TargetProfile_Sizer.Add( AllProfiles_Sizer );
   TargetProfile_Sizer.Add( ToDefaultProfile_RadioButton );
   TargetProfile_Sizer.Add( DefaultRGBProfile_Sizer );
   TargetProfile_Sizer.Add( DefaultGrayscaleProfile_Sizer );

   TargetProfile_GroupBox.SetTitle( "Target Profile" );
   TargetProfile_GroupBox.SetSizer( TargetProfile_Sizer );

   //

   RenderingIntent_ComboBox.AddItem( "Perceptual (photographic images)" );
   RenderingIntent_ComboBox.AddItem( "Saturation (graphics)" );
   RenderingIntent_ComboBox.AddItem( "Relative Colorimetric (match white points)" );
   RenderingIntent_ComboBox.AddItem( "Absolute Colorimetric (proofing)" );
   RenderingIntent_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&ICCProfileTransformationInterface::__RenderingIntent_ItemSelected, w );

   RenderingIntent_Sizer.SetMargin( 6 );
   RenderingIntent_Sizer.Add( RenderingIntent_ComboBox );

   RenderingIntent_GroupBox.SetTitle( "Rendering Intent" );
   RenderingIntent_GroupBox.SetSizer( RenderingIntent_Sizer );
   RenderingIntent_GroupBox.SetToolTip(
      "<p>A <i>rendering intent</i> is a method to map existing out-of-gamut colors in the image color "
      "space to the color space of the target ICC profile.</p>"

      "<p>The <i>relative colorimetric</i> rendering intent matches the white point in the image color "
      "space to the white point in the target color space. For example, if the target ICC profile "
      "characterizes a printing device, this means that pure white will not be printed at all (no ink), "
      "but will be represented by the <i>white of the paper</i>. Out-of-gamut colors are replaced by the "
      "nearest in-gamut colors. Relative colorimetric is in general the best balanced option for the sake "
      "of color accuracy in both the <i>numerical</i> and <i>perceptual</i> senses.</p>"

      "<p>The <i>absolute colorimetric</i> intent also replaces out-of-gamut colors by their nearest "
      "matches in the output color space, but it doesn't try to match the white points. If the white "
      "points in the image and target color spaces don't match, you'll get a color cast in the highlights. "
      "However, since this intent doesn't alter any in-gamut colors, it is the best option in terms of "
      "<i>numeric</i> color accuracy.</p>"

      "<p>The <i>perceptual</i> intent maps all colors in the image space to the target space. With the "
      "perceptual intent, no out-of-gamut color is clipped, as happens with the colorimetric intents, but "
      "all colors can be altered, and the changes can be significant if the image space is much wider than "
      "the output space. In general, relative colorimetric is preferred, but perceptual can yield more "
      "vibrant results that may be desirable in some cases.</p>"

      "<p>Finally, the <i>saturation</i> intent tries to maximize color saturation in the output, at the "
      "cost of sacrifying color accuracy, when necessary. This intent can be a good option for synthetic "
      "graphics and illustrations, but not for reproduction of photographic images.</p>" );

   //

   UseBlackPointCompensation_CheckBox.SetText( "Black point compensation" );
   UseBlackPointCompensation_CheckBox.SetToolTip( "<p>Apply a black point compensation (BPC) "
      "algorithm to generate proofing renditions. BPC may help in better preserving shadow detail.</p>" );
   UseBlackPointCompensation_CheckBox.OnClick( (Button::click_event_handler)&ICCProfileTransformationInterface::__Option_ButtonClick, w );

   UseFloatingPointTransformation_CheckBox.SetText( "Floating point transformation" );
   UseFloatingPointTransformation_CheckBox.SetToolTip(
      "<p>Force the use of floating point operations for computation of transformed pixel samples of "
      "all numerical data types.</p>"
      "<p>When this option is disabled, 16-bit integer arithmetics will be used for 8-bit and 16-bit "
      "integer images. Floating point will always be used for the rest of images, irrespective of the "
      "state of this option, in order to preserve numerical accuracy. This option is enabled by default</p>" );
   UseFloatingPointTransformation_CheckBox.OnClick( (Button::click_event_handler)&ICCProfileTransformationInterface::__Option_ButtonClick, w );

   Options_Sizer.SetMargin( 6 );
   Options_Sizer.SetSpacing( 4 );
   Options_Sizer.Add( UseBlackPointCompensation_CheckBox, 50 );
   Options_Sizer.Add( UseFloatingPointTransformation_CheckBox, 50 );

   Options_GroupBox.SetTitle( "Options" );
   Options_GroupBox.SetSizer( Options_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( SourceProfile_GroupBox );
   Global_Sizer.Add( TargetProfile_GroupBox );
   Global_Sizer.Add( RenderingIntent_GroupBox );
   Global_Sizer.Add( Options_GroupBox );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ICCProfileTransformationInterface.cpp - Released 2019-01-21T12:06:41Z
