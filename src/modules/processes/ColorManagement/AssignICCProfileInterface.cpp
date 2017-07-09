//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard ColorManagement Process Module Version 01.00.00.0322
// ----------------------------------------------------------------------------
// AssignICCProfileInterface.cpp - Released 2017-07-09T18:07:32Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorManagement PixInsight module.
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

#include "AssignICCProfileInterface.h"
#include "AssignICCProfileProcess.h"
#include "AssignICCProfileParameters.h"

#include <pcl/GlobalSettings.h>
#include <pcl/ImageWindow.h>

namespace pcl
{

// ----------------------------------------------------------------------------

AssignICCProfileInterface* TheAssignICCProfileInterface = nullptr;

// ----------------------------------------------------------------------------

#include "AssignICCProfileIcon.xpm"

// ----------------------------------------------------------------------------

#define currentView  GUI->AllImages_ViewList.CurrentView()

// ----------------------------------------------------------------------------

AssignICCProfileInterface::AssignICCProfileInterface() :
   instance( TheAssignICCProfileProcess )
{
   TheAssignICCProfileInterface = this;
}

AssignICCProfileInterface::~AssignICCProfileInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString AssignICCProfileInterface::Id() const
{
   return "AssignICCProfile";
}

MetaProcess* AssignICCProfileInterface::Process() const
{
   return TheAssignICCProfileProcess;
}

const char** AssignICCProfileInterface::IconImageXPM() const
{
   return AssignICCProfileIcon_XPM;
}

InterfaceFeatures AssignICCProfileInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::TrackViewButton;
}

void AssignICCProfileInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentWindow();
}

void AssignICCProfileInterface::TrackViewUpdated( bool active )
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

void AssignICCProfileInterface::ResetInstance()
{
   AssignICCProfileInstance defaultInstance( TheAssignICCProfileProcess );
   ImportProcess( defaultInstance );
}

bool AssignICCProfileInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "AssignICCProfile" );
      GUI->AllImages_ViewList.Regenerate( true/*mainViews*/, false/*previews*/ );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheAssignICCProfileProcess;
}

ProcessImplementation* AssignICCProfileInterface::NewProcess() const
{
   return new AssignICCProfileInstance( instance );
}

bool AssignICCProfileInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const AssignICCProfileInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not an AssignICCProfile instance.";
   return false;
}

bool AssignICCProfileInterface::RequiresInstanceValidation() const
{
   return true;
}

bool AssignICCProfileInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

bool AssignICCProfileInterface::WantsImageNotifications() const
{
   return true;
}

void AssignICCProfileInterface::ImageUpdated( const View& v )
{
   if ( GUI != nullptr )
      if ( v == currentView )
         UpdateControls();
}

void AssignICCProfileInterface::ImageFocused( const View& v )
{
   if ( GUI != nullptr )
      if ( IsTrackViewActive() )
         if ( !v.IsNull() )
         {
            GUI->AllImages_ViewList.SelectView( v.Window().MainView() ); // normally not necessary, but we can invoke this f() directly
            UpdateControls();
         }
}

void AssignICCProfileInterface::ImageCMUpdated( const View& v )
{
   if ( GUI != nullptr )
      if ( v == currentView )
         UpdateControls();
}

bool AssignICCProfileInterface::WantsGlobalNotifications() const
{
   return true;
}

void AssignICCProfileInterface::GlobalCMUpdated()
{
   if ( GUI != nullptr )
      if ( !currentView.IsNull() )
         UpdateControls(); // in case the selected view has no assigned profile
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void AssignICCProfileInterface::UpdateControls()
{
   if ( currentView.IsNull() )
      GUI->CurrentProfile_Edit.Clear();
   else
   {
      ICCProfile icc;
      currentView.Window().GetICCProfile( icc );
      if ( icc.IsProfile() )
         GUI->CurrentProfile_Edit.SetText( icc.Description() );
      else
      {
         String defaultProfilePath = PixInsightSettings::GlobalString( currentView.IsColor() ?
            "ColorManagement/DefaultRGBProfilePath" : "ColorManagement/DefaultGrayscaleProfilePath" );

         icc.Load( defaultProfilePath );

         String text = "<* None *>";

         if ( icc.IsProfile() )
            text += " (" + icc.Description() + ')';

         GUI->CurrentProfile_Edit.SetText( text );
      }
   }

   GUI->AssignDefault_RadioButton.SetChecked( instance.mode == AssignMode::AssignDefaultProfile );
   GUI->LeaveUntagged_RadioButton.SetChecked( instance.mode == AssignMode::LeaveUntagged );
   GUI->AssignProfile_RadioButton.SetChecked( instance.mode == AssignMode::AssignNewProfile );

   GUI->TargetProfile_Edit.SetText( instance.targetProfile );
   GUI->TargetProfile_Edit.Enable( instance.mode == AssignMode::AssignNewProfile );

   if ( profiles.IsEmpty() )
      RefreshProfiles();

   GUI->AllProfiles_ComboBox.SetCurrentItem( 0 );
   GUI->AllProfiles_ComboBox.Enable( instance.mode == AssignMode::AssignNewProfile );

   GUI->RefreshProfiles_ToolButton.Enable( instance.mode == AssignMode::AssignNewProfile );
}

void AssignICCProfileInterface::RefreshProfiles()
{
   profiles = ICCProfile::FindProfilesByColorSpace( ICCColorSpace::RGB|ICCColorSpace::Gray );

   GUI->AllProfiles_ComboBox.Clear();
   GUI->AllProfiles_ComboBox.AddItem( " --> Available ICC Profiles <-- " );
   for ( ICCProfile::profile_list::const_iterator i = profiles.Begin(); i != profiles.End(); ++i )
      GUI->AllProfiles_ComboBox.AddItem( i->description );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void AssignICCProfileInterface::__ViewList_ViewSelected( ViewList& /*sender*/, View& /*view*/ )
{
   UpdateControls();
}

void AssignICCProfileInterface::__AssignMode_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->AssignDefault_RadioButton )
      instance.mode = AssignMode::AssignDefaultProfile;
   else if ( sender == GUI->LeaveUntagged_RadioButton )
      instance.mode = AssignMode::LeaveUntagged;
   else if ( sender == GUI->AssignProfile_RadioButton )
      instance.mode = AssignMode::AssignNewProfile;

   UpdateControls();
}

void AssignICCProfileInterface::__TargetProfile_EditCompleted( Edit& sender )
{
   String txt = sender.Text().Trimmed();
   instance.targetProfile = txt;
   instance.mode = AssignMode::AssignNewProfile;
   UpdateControls();
}

void AssignICCProfileInterface::__Profile_ItemSelected( ComboBox& /*sender*/, int itemIndex )
{
   if ( itemIndex > 0 )
      if ( itemIndex <= int( profiles.Length() ) ) // first item is title
      {
         instance.targetProfile = profiles[itemIndex-1].description; // skip the first title item
         instance.mode = AssignMode::AssignNewProfile;
         UpdateControls();
      }
}

void AssignICCProfileInterface::__RefreshProfiles_ButtonClick( Button& /*sender*/, bool /*checked*/ )
{
   RefreshProfiles();
   UpdateControls();
}

void AssignICCProfileInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->AllImages_ViewList )
      wantsView = view.IsMainView();
}

void AssignICCProfileInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
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

AssignICCProfileInterface::GUIData::GUIData( AssignICCProfileInterface& w )
{
   pcl::Font fnt = w.Font();
   int editWidth1 = fnt.Width( String( 'M', 50 ) );

   //

   AllImages_ViewList.OnViewSelected( (ViewList::view_event_handler)&AssignICCProfileInterface::__ViewList_ViewSelected, w );
   AllImages_ViewList.OnViewDrag( (Control::view_drag_event_handler)&AssignICCProfileInterface::__ViewDrag, w );
   AllImages_ViewList.OnViewDrop( (Control::view_drop_event_handler)&AssignICCProfileInterface::__ViewDrop, w );

   CurrentProfile_Edit.SetMinWidth( editWidth1 );
   CurrentProfile_Edit.SetReadOnly();

   CurrentProfile_Sizer.SetMargin( 6 );
   CurrentProfile_Sizer.SetSpacing( 4 );
   CurrentProfile_Sizer.Add( AllImages_ViewList );
   CurrentProfile_Sizer.Add( CurrentProfile_Edit );

   CurrentProfile_GroupBox.SetTitle( "Current Profile" );
   CurrentProfile_GroupBox.SetSizer( CurrentProfile_Sizer );

   //

   AssignDefault_RadioButton.SetText( "Assign the default profile" );
   AssignDefault_RadioButton.OnClick( (Button::click_event_handler)&AssignICCProfileInterface::__AssignMode_ButtonClick, w );

   LeaveUntagged_RadioButton.SetText( "Leave the image untagged" );
   LeaveUntagged_RadioButton.OnClick( (Button::click_event_handler)&AssignICCProfileInterface::__AssignMode_ButtonClick, w );

   AssignProfile_RadioButton.SetText( "Assign profile:" );
   AssignProfile_RadioButton.OnClick( (Button::click_event_handler)&AssignICCProfileInterface::__AssignMode_ButtonClick, w );

   TargetProfile_Edit.SetMinWidth( editWidth1 );
   TargetProfile_Edit.OnEditCompleted( (Edit::edit_event_handler)&AssignICCProfileInterface::__TargetProfile_EditCompleted, w );

   AllProfiles_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&AssignICCProfileInterface::__Profile_ItemSelected, w );

   RefreshProfiles_ToolButton.SetIcon( w.ScaledResource( ":/icons/refresh.png" ) );
   RefreshProfiles_ToolButton.SetScaledFixedSize( 20, 20 );
   RefreshProfiles_ToolButton.SetToolTip( "Refresh profile list" );
   RefreshProfiles_ToolButton.OnClick( (Button::click_event_handler)&AssignICCProfileInterface::__RefreshProfiles_ButtonClick, w );

   AllProfiles_Sizer.SetSpacing( 4 );
   AllProfiles_Sizer.Add( AllProfiles_ComboBox, 100 );
   AllProfiles_Sizer.Add( RefreshProfiles_ToolButton );

   NewProfile_Sizer.SetMargin( 6 );
   NewProfile_Sizer.SetSpacing( 4 );
   NewProfile_Sizer.Add( AssignDefault_RadioButton );
   NewProfile_Sizer.Add( LeaveUntagged_RadioButton );
   NewProfile_Sizer.Add( AssignProfile_RadioButton );
   NewProfile_Sizer.Add( TargetProfile_Edit );
   NewProfile_Sizer.Add( AllProfiles_Sizer );

   NewProfile_GroupBox.SetTitle( "New Profile" );
   NewProfile_GroupBox.SetSizer( NewProfile_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( CurrentProfile_GroupBox );
   Global_Sizer.Add( NewProfile_GroupBox );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AssignICCProfileInterface.cpp - Released 2017-07-09T18:07:32Z
