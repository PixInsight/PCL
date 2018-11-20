//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard Sandbox Process Module Version 01.00.02.0269
// ----------------------------------------------------------------------------
// SandboxInterface.cpp - Released 2018-11-01T11:07:21Z
// ----------------------------------------------------------------------------
// This file is part of the standard Sandbox PixInsight module.
//
// Copyright (c) 2003-2018 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "SandboxInterface.h"
#include "SandboxParameters.h"
#include "SandboxProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SandboxInterface* TheSandboxInterface = nullptr;

// ----------------------------------------------------------------------------

//#include "SandboxIcon.xpm"

// ----------------------------------------------------------------------------

SandboxInterface::SandboxInterface() :
   instance( TheSandboxProcess )
{
   TheSandboxInterface = this;
}

// ----------------------------------------------------------------------------

SandboxInterface::~SandboxInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString SandboxInterface::Id() const
{
   return "Sandbox";
}

// ----------------------------------------------------------------------------

MetaProcess* SandboxInterface::Process() const
{
   return TheSandboxProcess;
}

// ----------------------------------------------------------------------------

const char** SandboxInterface::IconImageXPM() const
{
   return nullptr; // SandboxIcon_XPM; ---> put a nice icon here
}

// ----------------------------------------------------------------------------

void SandboxInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void SandboxInterface::ResetInstance()
{
   SandboxInstance defaultInstance( TheSandboxProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool SandboxInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Sandbox" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheSandboxProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* SandboxInterface::NewProcess() const
{
   return new SandboxInstance( instance );
}

// ----------------------------------------------------------------------------

bool SandboxInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const SandboxInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a Sandbox instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool SandboxInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool SandboxInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SandboxInterface::UpdateControls()
{
   GUI->ParameterOne_NumericControl.SetValue( instance.p_one );
   GUI->ParameterTwo_SpinBox.SetValue( instance.p_two );
   GUI->ParameterThree_CheckBox.SetChecked( instance.p_three );
   GUI->ParameterFour_ComboBox.SetCurrentItem( instance.p_four );
   GUI->ParameterFive_Edit.SetText( instance.p_five );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void SandboxInterface::e_RealValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->ParameterOne_NumericControl )
      instance.p_one = value;
}

// ----------------------------------------------------------------------------

void SandboxInterface::e_IntegerValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->ParameterTwo_SpinBox )
      instance.p_two = value;
}

// ----------------------------------------------------------------------------

void SandboxInterface::e_ItemClicked( Button& sender, bool checked )
{
   if ( sender == GUI->ParameterThree_CheckBox )
      instance.p_three = checked;
}

// ----------------------------------------------------------------------------

void SandboxInterface::e_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->ParameterFour_ComboBox )
      instance.p_four = itemIndex;
}

// ----------------------------------------------------------------------------

void SandboxInterface::e_EditGetFocus( Control& sender )
{
   if ( sender == GUI->ParameterFive_Edit )
   {
      // If you need to do something when sender gets focus, do it here.
   }
}

// ----------------------------------------------------------------------------

void SandboxInterface::e_EditCompleted( Edit& sender )
{
   if ( sender == GUI->ParameterFive_Edit )
      instance.p_five = sender.Text();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

SandboxInterface::GUIData::GUIData( SandboxInterface& w )
{
   pcl::Font font = w.Font();
   int labelWidth1 = font.Width( String( "Three:" ) ); // the longest label text
   int editWidth1 = font.Width( String( '0', 7 ) );

   //

   ParameterOne_NumericControl.label.SetText( "One:" );
   ParameterOne_NumericControl.label.SetFixedWidth( labelWidth1 );
   ParameterOne_NumericControl.slider.SetScaledMinWidth( 250 );
   ParameterOne_NumericControl.slider.SetRange( 0, 100 );
   ParameterOne_NumericControl.SetReal();
   ParameterOne_NumericControl.SetRange( TheSandboxParameterOneParameter->MinimumValue(), TheSandboxParameterOneParameter->MaximumValue() );
   ParameterOne_NumericControl.SetPrecision( TheSandboxParameterOneParameter->Precision() );
   ParameterOne_NumericControl.SetToolTip( "<p>This is the first parameter.</p>" );
   ParameterOne_NumericControl.edit.SetFixedWidth( editWidth1 );
   ParameterOne_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SandboxInterface::e_RealValueUpdated, w );

   //

   ParameterTwo_Label.SetText( "Two:" );
   ParameterTwo_Label.SetMinWidth( labelWidth1 );
   ParameterTwo_Label.SetToolTip( "<p>This is the second parameter.</p>" );
   ParameterTwo_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ParameterTwo_SpinBox.SetRange( int( TheSandboxParameterTwoParameter->MinimumValue() ), int( TheSandboxParameterTwoParameter->MaximumValue() ) );
   ParameterTwo_SpinBox.SetFixedWidth( editWidth1 );
   ParameterTwo_SpinBox.SetToolTip( "<p>This is the second parameter.</p>" );
   ParameterTwo_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SandboxInterface::e_IntegerValueUpdated, w );

   ParameterTwo_Sizer.SetSpacing( 4 );
   ParameterTwo_Sizer.Add( ParameterTwo_Label );
   ParameterTwo_Sizer.Add( ParameterTwo_SpinBox );
   ParameterTwo_Sizer.AddStretch();

   //

   ParameterThree_CheckBox.SetText( "Three" );
   ParameterThree_CheckBox.SetToolTip( "<p>This is the third parameter.</p>" );
   ParameterThree_CheckBox.OnClick( (pcl::Button::click_event_handler)&SandboxInterface::e_ItemClicked, w );

   ParameterThree_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   ParameterThree_Sizer.Add( ParameterThree_CheckBox );
   ParameterThree_Sizer.AddStretch();

   //

   ParameterFour_Label.SetText( "Four:" );
   ParameterFour_Label.SetMinWidth( labelWidth1 );
   ParameterFour_Label.SetToolTip( "<p>This is the fourth parameter.</p>" );
   ParameterFour_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ParameterFour_ComboBox.AddItem( "First Item" );
   ParameterFour_ComboBox.AddItem( "Second Item" );
   ParameterFour_ComboBox.AddItem( "Third Item" );
   ParameterFour_ComboBox.SetToolTip( "<p>This is the fourth parameter.</p>" );
   ParameterFour_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SandboxInterface::e_ItemSelected, w );

   ParameterFour_Sizer.SetSpacing( 4 );
   ParameterFour_Sizer.Add( ParameterFour_Label );
   ParameterFour_Sizer.Add( ParameterFour_ComboBox );
   ParameterFour_Sizer.AddStretch();

   //

   ParameterFive_Label.SetText( "Five:" );
   ParameterFive_Label.SetMinWidth( labelWidth1 );
   ParameterFive_Label.SetToolTip( "<p>This is the fifth parameter.</p>" );
   ParameterFive_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   ParameterFive_Edit.SetMinWidth( editWidth1 );
   ParameterFive_Edit.SetToolTip( "<p>This is the fifth parameter.</p>" );
   ParameterFive_Edit.OnGetFocus( (Control::event_handler)&SandboxInterface::e_EditGetFocus, w );
   ParameterFive_Edit.OnEditCompleted( (Edit::edit_event_handler)&SandboxInterface::e_EditCompleted, w );

   ParameterFive_Sizer.SetSpacing( 4 );
   ParameterFive_Sizer.Add( ParameterFive_Label );
   ParameterFive_Sizer.Add( ParameterFive_Edit, 100 );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( ParameterOne_NumericControl );
   Global_Sizer.Add( ParameterTwo_Sizer );
   Global_Sizer.Add( ParameterThree_Sizer );
   Global_Sizer.Add( ParameterFour_Sizer );
   Global_Sizer.Add( ParameterFive_Sizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SandboxInterface.cpp - Released 2018-11-01T11:07:21Z
