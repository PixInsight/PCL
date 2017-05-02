//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard Image Process Module Version 01.02.09.0371
// ----------------------------------------------------------------------------
// CreateAlphaChannelsInterface.cpp - Released 2017-05-02T09:43:00Z
// ----------------------------------------------------------------------------
// This file is part of the standard Image PixInsight module.
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

#include "CreateAlphaChannelsInterface.h"
#include "CreateAlphaChannelsParameters.h"
#include "CreateAlphaChannelsProcess.h"

#include <pcl/ErrorHandler.h>
#include <pcl/Graphics.h>
#include <pcl/View.h>
#include <pcl/ViewSelectionDialog.h>

#define AUTO_ID   "<Auto>"

namespace pcl
{

// ----------------------------------------------------------------------------

CreateAlphaChannelsInterface* TheCreateAlphaChannelsInterface = nullptr;

// ----------------------------------------------------------------------------

#include "CreateAlphaChannelsIcon.xpm"

// ----------------------------------------------------------------------------

CreateAlphaChannelsInterface::CreateAlphaChannelsInterface() :
   instance( TheCreateAlphaChannelsProcess )
{
   TheCreateAlphaChannelsInterface = this;
}

CreateAlphaChannelsInterface::~CreateAlphaChannelsInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString CreateAlphaChannelsInterface::Id() const
{
   return "CreateAlphaChannels";
}

MetaProcess* CreateAlphaChannelsInterface::Process() const
{
   return TheCreateAlphaChannelsProcess;
}

const char** CreateAlphaChannelsInterface::IconImageXPM() const
{
   return CreateAlphaChannelsIcon_XPM;
}

void CreateAlphaChannelsInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void CreateAlphaChannelsInterface::ResetInstance()
{
   CreateAlphaChannelsInstance defaultInstance( TheCreateAlphaChannelsProcess );
   ImportProcess( defaultInstance );
}

bool CreateAlphaChannelsInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "CreateAlphaChannels" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheCreateAlphaChannelsProcess;
}

ProcessImplementation* CreateAlphaChannelsInterface::NewProcess() const
{
   return new CreateAlphaChannelsInstance( instance );
}

bool CreateAlphaChannelsInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const CreateAlphaChannelsInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a CreateAlphaChannels instance.";
   return false;
}

bool CreateAlphaChannelsInterface::RequiresInstanceValidation() const
{
   return true;
}

bool CreateAlphaChannelsInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void CreateAlphaChannelsInterface::UpdateControls()
{
   GUI->FromImage_GroupBox.SetChecked( instance.fromImage );
   GUI->ImageId_Edit.SetText( instance.imageId.IsEmpty() ? AUTO_ID : instance.imageId );
   GUI->InvertSource_CheckBox.SetChecked( instance.invertSourceImage );
   GUI->CloseSource_CheckBox.SetChecked( instance.closeSourceImage );

   GUI->FromTransparency_GroupBox.SetChecked( !instance.fromImage );
   GUI->Transparency_NumericControl.SetValue( instance.transparency );
   GUI->ColorSample_Control.Update();

   GUI->Replace_CheckBox.SetChecked( instance.replace );
}

// ----------------------------------------------------------------------------

void CreateAlphaChannelsInterface::__SourceMode_Check( GroupBox& sender, bool checked )
{
   if ( sender == GUI->FromImage_GroupBox )
      instance.fromImage = checked;
   else if ( sender == GUI->FromTransparency_GroupBox )
      instance.fromImage = !checked;
   UpdateControls();
}

void CreateAlphaChannelsInterface::__ImageId_GetFocus( Control& sender )
{
   Edit* e = dynamic_cast<Edit*>( &sender );
   if ( e != nullptr )
      if ( e->Text() == AUTO_ID )
         e->Clear();
}

void CreateAlphaChannelsInterface::__ImageId_EditCompleted( Edit& sender )
{
   try
   {
      String id = sender.Text();
      id.Trim();
      if ( !id.IsEmpty() && id != AUTO_ID && !id.IsValidIdentifier() )
         throw Error( "Invalid identifier: " + id );
      instance.imageId = (id != AUTO_ID) ? id : String();
      sender.SetText( instance.imageId.IsEmpty() ? AUTO_ID : instance.imageId );
   }
   ERROR_CLEANUP(
      sender.SetText( instance.imageId );
      sender.SelectAll();
      sender.Focus()
   )
}

void CreateAlphaChannelsInterface::__SelectSource_Click( Button& /*sender*/, bool /*checked*/ )
{
   ViewSelectionDialog d( instance.imageId, false/*allowPreviews*/ );
   d.SetWindowTitle( "Select Alpha Channel Source" );
   if ( d.Execute() == StdDialogCode::Ok )
   {
      instance.imageId = d.Id();
      UpdateControls();
   }
}

void CreateAlphaChannelsInterface::__SourceOption_Click( Button& sender, bool checked )
{
   if ( sender == GUI->InvertSource_CheckBox )
      instance.invertSourceImage = checked;
   else if ( sender == GUI->CloseSource_CheckBox )
      instance.closeSourceImage = checked;
   UpdateControls();
}

void CreateAlphaChannelsInterface::__Transparency_ValueUpdated( NumericEdit& /*sender*/, double value )
{
   GUI->Transparency_NumericControl.SetValue( instance.transparency = value );
   GUI->ColorSample_Control.Update();
}

void CreateAlphaChannelsInterface::__ColorSample_Paint( Control& sender, const Rect& updateRect )
{
   Graphics g( sender );
   g.SetBrush( RGBAColor( float( instance.transparency ), float( instance.transparency ), float( instance.transparency ) ) );
   g.SetPen( 0xff000000, sender.DisplayPixelRatio() );
   g.DrawRect( sender.BoundsRect() );
}

void CreateAlphaChannelsInterface::__Replace_Click( Button& /*sender*/, bool checked )
{
   instance.replace = checked;
}

void CreateAlphaChannelsInterface::__ViewDrag( Control& sender, const Point& pos, const View& view, unsigned modifiers, bool& wantsView )
{
   if ( sender == GUI->ImageId_Edit )
      wantsView = view.IsMainView();
}

void CreateAlphaChannelsInterface::__ViewDrop( Control& sender, const Point& pos, const View& view, unsigned modifiers )
{
   if ( sender == GUI->ImageId_Edit )
      if ( view.IsMainView() )
      {
         instance.imageId = view.Id();
         UpdateControls();
      }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

CreateAlphaChannelsInterface::GUIData::GUIData( CreateAlphaChannelsInterface& w )
{
   pcl::Font fnt = w.Font();
   int editWidth = fnt.Width( String( 'M', 40 ) );

   //

   ImageId_Edit.SetMinWidth( editWidth );
   ImageId_Edit.OnGetFocus( (Control::event_handler)&CreateAlphaChannelsInterface::__ImageId_GetFocus, w );
   ImageId_Edit.OnEditCompleted( (Edit::edit_event_handler)&CreateAlphaChannelsInterface::__ImageId_EditCompleted, w );
   ImageId_Edit.OnViewDrag( (Control::view_drag_event_handler)&CreateAlphaChannelsInterface::__ViewDrag, w );
   ImageId_Edit.OnViewDrop( (Control::view_drop_event_handler)&CreateAlphaChannelsInterface::__ViewDrop, w );

   SelectSource_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   SelectSource_ToolButton.SetScaledFixedSize( 20, 20 );
   SelectSource_ToolButton.SetToolTip( "Select alpha channel source image" );
   SelectSource_ToolButton.OnClick( (ToolButton::click_event_handler)&CreateAlphaChannelsInterface::__SelectSource_Click, w );

   ImageId_Sizer.SetSpacing( 4 );
   ImageId_Sizer.Add( ImageId_Edit, 100 );
   ImageId_Sizer.Add( SelectSource_ToolButton );

   InvertSource_CheckBox.SetText( "Invert" );
   InvertSource_CheckBox.SetToolTip( "Invert source alpha pixels" );
   InvertSource_CheckBox.OnClick( (ToolButton::click_event_handler)&CreateAlphaChannelsInterface::__SourceOption_Click, w );

   CloseSource_CheckBox.SetText( "Close source" );
   CloseSource_CheckBox.SetToolTip( "Close alpha channel source image" );
   CloseSource_CheckBox.OnClick( (ToolButton::click_event_handler)&CreateAlphaChannelsInterface::__SourceOption_Click, w );

   FromImage_Sizer.SetMargin( 6 );
   FromImage_Sizer.SetSpacing( 4 );
   FromImage_Sizer.Add( ImageId_Sizer );
   FromImage_Sizer.Add( InvertSource_CheckBox );
   FromImage_Sizer.Add( CloseSource_CheckBox );

   FromImage_GroupBox.SetTitle( "From Image" );
   FromImage_GroupBox.EnableTitleCheckBox();
   FromImage_GroupBox.SetSizer( FromImage_Sizer );
   FromImage_GroupBox.OnCheck( (GroupBox::check_event_handler)&CreateAlphaChannelsInterface::__SourceMode_Check, w );

   //

   Transparency_NumericControl.label.Hide();
   Transparency_NumericControl.slider.SetRange( 0, 100 );
   Transparency_NumericControl.slider.SetScaledMinWidth( 150 );
   Transparency_NumericControl.SetReal();
   Transparency_NumericControl.SetRange( TheCATransparencyParameter->MinimumValue(), TheCATransparencyParameter->MaximumValue() );
   Transparency_NumericControl.SetPrecision( TheCATransparencyParameter->Precision() );
   Transparency_NumericControl.SetToolTip( "Transparency" );
   Transparency_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&CreateAlphaChannelsInterface::__Transparency_ValueUpdated, w );

   ColorSample_Control.SetScaledFixedWidth( 60 );
   ColorSample_Control.OnPaint( (Control::paint_event_handler)&CreateAlphaChannelsInterface::__ColorSample_Paint, w );

   FromTransparency_Sizer.SetMargin( 6 );
   FromTransparency_Sizer.SetSpacing( 4 );
   FromTransparency_Sizer.Add( Transparency_NumericControl, 100 );
   FromTransparency_Sizer.Add( ColorSample_Control, 100 );

   FromTransparency_GroupBox.SetTitle( "From Transparency" );
   FromTransparency_GroupBox.EnableTitleCheckBox();
   FromTransparency_GroupBox.SetSizer( FromTransparency_Sizer );
   FromTransparency_GroupBox.OnCheck( (GroupBox::check_event_handler)&CreateAlphaChannelsInterface::__SourceMode_Check, w );

   //

   Replace_CheckBox.SetText( "Replace existing alpha channels" );
   Replace_CheckBox.OnClick( (ToolButton::click_event_handler)&CreateAlphaChannelsInterface::__Replace_Click, w );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( FromImage_GroupBox );
   Global_Sizer.Add( FromTransparency_GroupBox );
   Global_Sizer.Add( Replace_CheckBox );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CreateAlphaChannelsInterface.cpp - Released 2017-05-02T09:43:00Z
