//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard ColorSpaces Process Module Version 01.01.00.0317
// ----------------------------------------------------------------------------
// ChannelCombinationInterface.cpp - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard ColorSpaces PixInsight module.
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

#include "ChannelCombinationInterface.h"
#include "ChannelCombinationParameters.h"
#include "ChannelCombinationProcess.h"
#include "ChannelSourceSelectionDialog.h"

#include <pcl/ErrorHandler.h>
#include <pcl/View.h>

#define AUTO_ID   "<Auto>"

namespace pcl
{

// ----------------------------------------------------------------------------

ChannelCombinationInterface* TheChannelCombinationInterface = nullptr;

// ----------------------------------------------------------------------------

#include "ChannelCombinationIcon.xpm"

// ----------------------------------------------------------------------------

ChannelCombinationInterface::ChannelCombinationInterface() :
   instance( TheChannelCombinationProcess )
{
   TheChannelCombinationInterface = this;
}

ChannelCombinationInterface::~ChannelCombinationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString ChannelCombinationInterface::Id() const
{
   return "ChannelCombination";
}

MetaProcess* ChannelCombinationInterface::Process() const
{
   return TheChannelCombinationProcess;
}

const char** ChannelCombinationInterface::IconImageXPM() const
{
   return ChannelCombinationIcon_XPM;
}

InterfaceFeatures ChannelCombinationInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::ApplyGlobalButton;
}

void ChannelCombinationInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void ChannelCombinationInterface::ResetInstance()
{
   ChannelCombinationInstance defaultInstance( TheChannelCombinationProcess );
   ImportProcess( defaultInstance );
}

bool ChannelCombinationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "ChannelCombination" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheChannelCombinationProcess;
}

ProcessImplementation* ChannelCombinationInterface::NewProcess() const
{
   return new ChannelCombinationInstance( instance );
}

bool ChannelCombinationInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const ChannelCombinationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a ChannelCombination instance.";
   return false;
}

bool ChannelCombinationInterface::RequiresInstanceValidation() const
{
   return true;
}

bool ChannelCombinationInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void ChannelCombinationInterface::UpdateControls()
{
   GUI->RGB_RadioButton.SetChecked( instance.ColorSpace() == ColorSpaceId::RGB );
   GUI->HSV_RadioButton.SetChecked( instance.ColorSpace() == ColorSpaceId::HSV );
   GUI->HSI_RadioButton.SetChecked( instance.ColorSpace() == ColorSpaceId::HSI );
   GUI->CIEXYZ_RadioButton.SetChecked( instance.ColorSpace() == ColorSpaceId::CIEXYZ );
   GUI->CIELab_RadioButton.SetChecked( instance.ColorSpace() == ColorSpaceId::CIELab );
   GUI->CIELch_RadioButton.SetChecked( instance.ColorSpace() == ColorSpaceId::CIELch );

   //

   GUI->C0_Label.SetText( ColorSpaceId::ChannelId( instance.ColorSpace(), 0 ) );

   GUI->C0_CheckBox.SetChecked( instance.IsChannelEnabled( 0 ) );

   GUI->C0_Edit.SetText( instance.ChannelId( 0 ).IsEmpty() ? AUTO_ID : instance.ChannelId( 0 ) );
   GUI->C0_Edit.Enable( instance.IsChannelEnabled( 0 ) );

   GUI->C0_ToolButton.Enable( instance.IsChannelEnabled( 0 ) );

   //

   GUI->C1_Label.SetText( ColorSpaceId::ChannelId( instance.ColorSpace(), 1 ) );

   GUI->C1_CheckBox.SetChecked( instance.IsChannelEnabled( 1 ) );

   GUI->C1_Edit.SetText( instance.ChannelId( 1 ).IsEmpty() ? AUTO_ID : instance.ChannelId( 1 ) );
   GUI->C1_Edit.Enable( instance.IsChannelEnabled( 1 ) );

   GUI->C1_ToolButton.Enable( instance.IsChannelEnabled( 1 ) );

   //

   GUI->C2_Label.SetText( ColorSpaceId::ChannelId( instance.ColorSpace(), 2 ) );

   GUI->C2_CheckBox.SetChecked( instance.IsChannelEnabled( 2 ) );

   GUI->C2_Edit.SetText( instance.ChannelId( 2 ).IsEmpty() ? AUTO_ID : instance.ChannelId( 2 ) );
   GUI->C2_Edit.Enable( instance.IsChannelEnabled( 2 ) );

   GUI->C2_ToolButton.Enable( instance.IsChannelEnabled( 2 ) );
}

// ----------------------------------------------------------------------------

void ChannelCombinationInterface::__ColorSpace_Click( Button& sender, bool checked )
{
   if ( sender == GUI->RGB_RadioButton )
      instance.p_colorSpace = ColorSpaceId::RGB;
   else if ( sender == GUI->HSV_RadioButton )
      instance.p_colorSpace = ColorSpaceId::HSV;
   else if ( sender == GUI->HSI_RadioButton )
      instance.p_colorSpace = ColorSpaceId::HSI;
   else if ( sender == GUI->CIEXYZ_RadioButton )
      instance.p_colorSpace = ColorSpaceId::CIEXYZ;
   else if ( sender == GUI->CIELab_RadioButton )
      instance.p_colorSpace = ColorSpaceId::CIELab;
   else if ( sender == GUI->CIELch_RadioButton )
      instance.p_colorSpace = ColorSpaceId::CIELch;

   instance.p_channelEnabled[0] = instance.p_channelEnabled[1] = instance.p_channelEnabled[2] = true;

   UpdateControls();
}

void ChannelCombinationInterface::__Channel_Click( Button& sender, bool checked )
{
   int i;
   if ( sender == GUI->C0_CheckBox )
      i = 0;
   else if ( sender == GUI->C1_CheckBox )
      i = 1;
   else if ( sender == GUI->C2_CheckBox )
      i = 2;
   else
      return;

   int n = 0;
   for ( int j = 0; j < 3; ++j )
   {
      if ( j == i )
         instance.p_channelEnabled[i] = checked;
      if ( instance.p_channelEnabled[j] )
         ++n;
   }

   if ( n == 0 )
      for ( int j = 0; j < 3; ++j )
         instance.p_channelEnabled[j] = true;

   UpdateControls();
}

void ChannelCombinationInterface::__ChannelId_GetFocus( Control& sender )
{
   Edit* e = dynamic_cast<Edit*>( &sender );
   if ( e != nullptr )
      if ( e->Text() == AUTO_ID )
         e->Clear();
}

void ChannelCombinationInterface::__ChannelId_EditCompleted( Edit& sender )
{
   int i;
   if ( sender == GUI->C0_Edit )
      i = 0;
   else if ( sender == GUI->C1_Edit )
      i = 1;
   else if ( sender == GUI->C2_Edit )
      i = 2;
   else
      return;

   try
   {
      pcl::String id = sender.Text().Trimmed();
      if ( !id.IsEmpty() )
         if ( id != AUTO_ID )
            if ( !id.IsValidIdentifier() )
               throw Error( "Invalid identifier: " + id );

      instance.p_channelId[i] = (id != AUTO_ID) ? id : String();
      sender.SetText( instance.p_channelId[i].IsEmpty() ? AUTO_ID : instance.p_channelId[i] );
      return;
   }
   ERROR_CLEANUP(
      sender.SetText( instance.p_channelId[i] );
      sender.SelectAll();
      sender.Focus()
   )
}

void ChannelCombinationInterface::__Channel_SelectSource_Click( Button& sender, bool /*checked*/ )
{
   int i;
   if ( sender == GUI->C0_ToolButton )
      i = 0;
   else if ( sender == GUI->C1_ToolButton )
      i = 1;
   else if ( sender == GUI->C2_ToolButton )
      i = 2;
   else
      return;

   String suffix = String( '_' ) + ColorSpaceId::ChannelId( instance.p_colorSpace, i );
   String description = ColorSpaceId::SpaceId( instance.p_colorSpace ) + String().Format( " Channel #%d", i );

   ChannelSourceSelectionDialog dlg( suffix, description );
   if ( dlg.Execute() == StdDialogCode::Ok )
   {
      instance.p_channelId[i] = dlg.SourceId();
      UpdateControls();
   }
}

void ChannelCombinationInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->C0_Edit || sender == GUI->C1_Edit || sender == GUI->C2_Edit )
      wantsView = view.IsMainView() && !view.IsColor();
}

void ChannelCombinationInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( view.IsMainView() )
      if ( !view.IsColor() )
      {
         int i;
         if ( sender == GUI->C0_Edit )
            i = 0;
         else if ( sender == GUI->C1_Edit )
            i = 1;
         else if ( sender == GUI->C2_Edit )
            i = 2;
         else
            return;

         instance.p_channelId[i] = view.Id();
         UpdateControls();
      }
}

// ----------------------------------------------------------------------------

ChannelCombinationInterface::GUIData::GUIData( ChannelCombinationInterface& w )
{
   pcl::Font fnt = w.Font();
   int chEditWidth = fnt.Width( String( 'M', 35 ) );
   int chLabelWidth = fnt.Width( "Sv" ) + 4;

   //

   RGB_RadioButton.SetText( "RGB" );
   RGB_RadioButton.OnClick( (pcl::Button::click_event_handler)&ChannelCombinationInterface::__ColorSpace_Click, w );

   HSV_RadioButton.SetText( "HSV" );
   HSV_RadioButton.OnClick( (pcl::Button::click_event_handler)&ChannelCombinationInterface::__ColorSpace_Click, w );

   HSI_RadioButton.SetText( "HSI" );
   HSI_RadioButton.OnClick( (pcl::Button::click_event_handler)&ChannelCombinationInterface::__ColorSpace_Click, w );

   ColorSpaceLeft_Sizer.Add( RGB_RadioButton );
   ColorSpaceLeft_Sizer.Add( HSV_RadioButton );
   ColorSpaceLeft_Sizer.Add( HSI_RadioButton );

   //

   CIEXYZ_RadioButton.SetText( "CIE XYZ" );
   CIEXYZ_RadioButton.OnClick( (pcl::Button::click_event_handler)&ChannelCombinationInterface::__ColorSpace_Click, w );

   CIELab_RadioButton.SetText( "CIE L*a*b*" );
   CIELab_RadioButton.OnClick( (pcl::Button::click_event_handler)&ChannelCombinationInterface::__ColorSpace_Click, w );

   CIELch_RadioButton.SetText( "CIE L*c*h*" );
   CIELch_RadioButton.OnClick( (pcl::Button::click_event_handler)&ChannelCombinationInterface::__ColorSpace_Click, w );

   ColorSpaceRight_Sizer.Add( CIEXYZ_RadioButton );
   ColorSpaceRight_Sizer.Add( CIELab_RadioButton );
   ColorSpaceRight_Sizer.Add( CIELch_RadioButton );

   //

   ColorSpace_Sizer.SetMargin( 6 );
   ColorSpace_Sizer.SetSpacing( 10 );
   ColorSpace_Sizer.Add( ColorSpaceLeft_Sizer );
   ColorSpace_Sizer.Add( ColorSpaceRight_Sizer );

   ColorSpace_GroupBox.SetTitle( "Color Space" );
   ColorSpace_GroupBox.SetSizer( ColorSpace_Sizer );
   ColorSpace_GroupBox.AdjustToContents();

   //

   C0_CheckBox.OnClick( (pcl::Button::click_event_handler)&ChannelCombinationInterface::__Channel_Click, w );

   C0_Label.SetFixedWidth( chLabelWidth );
   C0_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );

   C0_Edit.SetMinWidth( chEditWidth );
   C0_Edit.OnGetFocus( (Control::event_handler)&ChannelCombinationInterface::__ChannelId_GetFocus, w );
   C0_Edit.OnEditCompleted( (Edit::edit_event_handler)&ChannelCombinationInterface::__ChannelId_EditCompleted, w );
   C0_Edit.OnViewDrag( (Control::view_drag_event_handler)&ChannelCombinationInterface::__ViewDrag, w );
   C0_Edit.OnViewDrop( (Control::view_drop_event_handler)&ChannelCombinationInterface::__ViewDrop, w );

   C0_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   C0_ToolButton.SetScaledFixedSize( 20, 20 );
   C0_ToolButton.SetToolTip( "Select channel #0 source image" );
   C0_ToolButton.OnClick( (ToolButton::click_event_handler)&ChannelCombinationInterface::__Channel_SelectSource_Click, w );

   C0_Sizer.Add( C0_CheckBox );
   C0_Sizer.Add( C0_Label );
   C0_Sizer.AddSpacing( 4 );
   C0_Sizer.Add( C0_Edit, 100 );
   C0_Sizer.AddSpacing( 4 );
   C0_Sizer.Add( C0_ToolButton );

   //

   C1_CheckBox.OnClick( (pcl::Button::click_event_handler)&ChannelCombinationInterface::__Channel_Click, w );

   C1_Label.SetFixedWidth( chLabelWidth );
   C1_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );

   C1_Edit.SetMinWidth( chEditWidth );
   C1_Edit.OnGetFocus( (Control::event_handler)&ChannelCombinationInterface::__ChannelId_GetFocus, w );
   C1_Edit.OnEditCompleted( (Edit::edit_event_handler)&ChannelCombinationInterface::__ChannelId_EditCompleted, w );
   C1_Edit.OnViewDrag( (Control::view_drag_event_handler)&ChannelCombinationInterface::__ViewDrag, w );
   C1_Edit.OnViewDrop( (Control::view_drop_event_handler)&ChannelCombinationInterface::__ViewDrop, w );

   C1_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   C1_ToolButton.SetScaledFixedSize( 20, 20 );
   C1_ToolButton.SetToolTip( "Select channel #1 source image" );
   C1_ToolButton.OnClick( (ToolButton::click_event_handler)&ChannelCombinationInterface::__Channel_SelectSource_Click, w );

   C1_Sizer.Add( C1_CheckBox );
   C1_Sizer.Add( C1_Label );
   C1_Sizer.AddSpacing( 4 );
   C1_Sizer.Add( C1_Edit, 100 );
   C1_Sizer.AddSpacing( 4 );
   C1_Sizer.Add( C1_ToolButton );

   //

   C2_CheckBox.OnClick( (pcl::Button::click_event_handler)&ChannelCombinationInterface::__Channel_Click, w );

   C2_Label.SetFixedWidth( chLabelWidth );
   C2_Label.SetTextAlignment( TextAlign::Center|TextAlign::VertCenter );

   C2_Edit.SetMinWidth( chEditWidth );
   C2_Edit.OnGetFocus( (Control::event_handler)&ChannelCombinationInterface::__ChannelId_GetFocus, w );
   C2_Edit.OnEditCompleted( (Edit::edit_event_handler)&ChannelCombinationInterface::__ChannelId_EditCompleted, w );
   C2_Edit.OnViewDrag( (Control::view_drag_event_handler)&ChannelCombinationInterface::__ViewDrag, w );
   C2_Edit.OnViewDrop( (Control::view_drop_event_handler)&ChannelCombinationInterface::__ViewDrop, w );

   C2_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   C2_ToolButton.SetScaledFixedSize( 20, 20 );
   C2_ToolButton.SetToolTip( "Select channel #2 source image" );
   C2_ToolButton.OnClick( (ToolButton::click_event_handler)&ChannelCombinationInterface::__Channel_SelectSource_Click, w );

   C2_Sizer.Add( C2_CheckBox );
   C2_Sizer.Add( C2_Label );
   C2_Sizer.AddSpacing( 4 );
   C2_Sizer.Add( C2_Edit, 100 );
   C2_Sizer.AddSpacing( 4 );
   C2_Sizer.Add( C2_ToolButton );

   //

   Channels_Sizer.SetMargin( 6 );
   Channels_Sizer.SetSpacing( 4 );
   Channels_Sizer.Add( C0_Sizer );
   Channels_Sizer.Add( C1_Sizer );
   Channels_Sizer.Add( C2_Sizer );
   Channels_Sizer.AddStretch();

   Channels_GroupBox.SetTitle( "Channels / Source Images" );
   Channels_GroupBox.SetSizer( Channels_Sizer );
   Channels_GroupBox.AdjustToContents();

   //

   C0_CheckBox.SetFixedWidth();
   C1_CheckBox.SetFixedWidth();
   C2_CheckBox.SetFixedWidth();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( ColorSpace_GroupBox );
   Global_Sizer.Add( Channels_GroupBox );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelCombinationInterface.cpp - Released 2017-05-02T09:43:00Z
