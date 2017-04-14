//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard NoiseGeneration Process Module Version 01.00.02.0284
// ----------------------------------------------------------------------------
// SimplexNoiseInterface.cpp - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoiseGeneration PixInsight module.
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

#include "SimplexNoiseInterface.h"
#include "SimplexNoiseProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

SimplexNoiseInterface* TheSimplexNoiseInterface = 0;

// ----------------------------------------------------------------------------

#include "SimplexNoiseIcon.xpm"

// ----------------------------------------------------------------------------

SimplexNoiseInterface::SimplexNoiseInterface() :
ProcessInterface(), instance( TheSimplexNoiseProcess ), GUI( 0 )
{
   TheSimplexNoiseInterface = this;
}

SimplexNoiseInterface::~SimplexNoiseInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString SimplexNoiseInterface::Id() const
{
   return "SimplexNoise";
}

MetaProcess* SimplexNoiseInterface::Process() const
{
   return TheSimplexNoiseProcess;
}

const char** SimplexNoiseInterface::IconImageXPM() const
{
   return SimplexNoiseIcon_XPM;
}

void SimplexNoiseInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void SimplexNoiseInterface::ResetInstance()
{
   SimplexNoiseInstance defaultInstance( TheSimplexNoiseProcess );
   ImportProcess( defaultInstance );
}

bool SimplexNoiseInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );

      GUI->Scale10_RadioButton.SetChecked();
      GUI->Scale_SpinBox.SetStepSize( 10 );

      GUI->OffsetX10_RadioButton.SetChecked();
      GUI->OffsetX_SpinBox.SetStepSize( 10 );

      GUI->OffsetY10_RadioButton.SetChecked();
      GUI->OffsetY_SpinBox.SetStepSize( 10 );

      SetWindowTitle( "SimplexNoise" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheSimplexNoiseProcess;
}

ProcessImplementation* SimplexNoiseInterface::NewProcess() const
{
   return new SimplexNoiseInstance( instance );
}

bool SimplexNoiseInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const SimplexNoiseInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a SimplexNoise instance.";
   return false;
}

bool SimplexNoiseInterface::RequiresInstanceValidation() const
{
   return true;
}

bool SimplexNoiseInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void SimplexNoiseInterface::UpdateControls()
{
   GUI->Amount_NumericControl.SetValue( instance.p_amount );
   GUI->Scale_SpinBox.SetValue( instance.p_scale );
   GUI->OffsetX_SpinBox.SetValue( instance.p_offsetX );
   GUI->OffsetY_SpinBox.SetValue( instance.p_offsetY );
   GUI->Operator_ComboBox.SetCurrentItem( instance.p_operator );
}

// ----------------------------------------------------------------------------

void SimplexNoiseInterface::__ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Amount_NumericControl )
      instance.p_amount = value;
}

void SimplexNoiseInterface::__IntValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->Scale_SpinBox )
      instance.p_scale = value;
   else if ( sender == GUI->OffsetX_SpinBox )
      instance.p_offsetX = value;
   else if ( sender == GUI->OffsetY_SpinBox )
      instance.p_offsetY = value;
}

void SimplexNoiseInterface::__Click( Button& sender, bool checked )
{
   if ( sender == GUI->Scale1_RadioButton )
      GUI->Scale_SpinBox.SetStepSize( 1 );
   else if ( sender == GUI->Scale10_RadioButton )
      GUI->Scale_SpinBox.SetStepSize( 10 );
   else if ( sender == GUI->Scale100_RadioButton )
      GUI->Scale_SpinBox.SetStepSize( 100 );

   else if ( sender == GUI->OffsetX1_RadioButton )
      GUI->OffsetX_SpinBox.SetStepSize( 1 );
   else if ( sender == GUI->OffsetX10_RadioButton )
      GUI->OffsetX_SpinBox.SetStepSize( 10 );
   else if ( sender == GUI->OffsetX100_RadioButton )
      GUI->OffsetX_SpinBox.SetStepSize( 100 );

   else if ( sender == GUI->OffsetY1_RadioButton )
      GUI->OffsetY_SpinBox.SetStepSize( 1 );
   else if ( sender == GUI->OffsetY10_RadioButton )
      GUI->OffsetY_SpinBox.SetStepSize( 10 );
   else if ( sender == GUI->OffsetY100_RadioButton )
      GUI->OffsetY_SpinBox.SetStepSize( 100 );
}

void SimplexNoiseInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Operator_ComboBox )
      instance.p_operator = itemIndex;
}

// ----------------------------------------------------------------------------

SimplexNoiseInterface::GUIData::GUIData( SimplexNoiseInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Operator:" ) ) + 4;
   int editWidth1 = fnt.Width( String( '0', 12 ) );

   //

   Amount_NumericControl.label.SetText( "Amount:" );
   Amount_NumericControl.label.SetMinWidth( labelWidth1 );
   Amount_NumericControl.slider.SetRange( 0, 100 );
   Amount_NumericControl.slider.SetScaledMinWidth( 250 );
   Amount_NumericControl.SetReal();
   Amount_NumericControl.SetPrecision( TheSNAmountParameter->Precision() );
   Amount_NumericControl.SetRange( TheSNAmountParameter->MinimumValue(), TheSNAmountParameter->MaximumValue() );
   Amount_NumericControl.edit.SetFixedWidth( editWidth1 );
   Amount_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SimplexNoiseInterface::__ValueUpdated, w );

   Scale_Label.SetText( "Scale:" );
   Scale_Label.SetMinWidth( labelWidth1 );
   Scale_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Scale_SpinBox.SetRange( int( TheSNScaleParameter->MinimumValue() ), int( TheSNScaleParameter->MaximumValue() ) );
   Scale_SpinBox.SetFixedWidth( editWidth1 );
   Scale_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SimplexNoiseInterface::__IntValueUpdated, w );

   Scale1_RadioButton.SetText( "x1" );
   Scale1_RadioButton.OnClick( (pcl::Button::click_event_handler)&SimplexNoiseInterface::__Click, w );
   Scale10_RadioButton.SetText( "x10" );
   Scale10_RadioButton.OnClick( (pcl::Button::click_event_handler)&SimplexNoiseInterface::__Click, w );
   Scale100_RadioButton.SetText( "x100" );
   Scale100_RadioButton.OnClick( (pcl::Button::click_event_handler)&SimplexNoiseInterface::__Click, w );

   Scale_Sizer.SetSpacing( 4 );
   Scale_Sizer.Add( Scale_Label );
   Scale_Sizer.Add( Scale_SpinBox );
   Scale_Sizer.AddSpacing( 8 );
   Scale_Sizer.Add( Scale1_RadioButton );
   Scale_Sizer.Add( Scale10_RadioButton );
   Scale_Sizer.Add( Scale100_RadioButton );
   Scale_Sizer.AddStretch();

   Scale_Control.SetSizer( Scale_Sizer );

   OffsetX_Label.SetText( "X-Offset:" );
   OffsetX_Label.SetMinWidth( labelWidth1 );
   OffsetX_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OffsetX_SpinBox.SetRange( int( TheSNOffsetXParameter->MinimumValue() ), int( TheSNOffsetXParameter->MaximumValue() ) );
   OffsetX_SpinBox.SetFixedWidth( editWidth1 );
   OffsetX_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SimplexNoiseInterface::__IntValueUpdated, w );

   OffsetX1_RadioButton.SetText( "x1" );
   OffsetX1_RadioButton.OnClick( (pcl::Button::click_event_handler)&SimplexNoiseInterface::__Click, w );
   OffsetX10_RadioButton.SetText( "x10" );
   OffsetX10_RadioButton.OnClick( (pcl::Button::click_event_handler)&SimplexNoiseInterface::__Click, w );
   OffsetX100_RadioButton.SetText( "x100" );
   OffsetX100_RadioButton.OnClick( (pcl::Button::click_event_handler)&SimplexNoiseInterface::__Click, w );

   OffsetX_Sizer.SetSpacing( 4 );
   OffsetX_Sizer.Add( OffsetX_Label );
   OffsetX_Sizer.Add( OffsetX_SpinBox );
   OffsetX_Sizer.AddSpacing( 8 );
   OffsetX_Sizer.Add( OffsetX1_RadioButton );
   OffsetX_Sizer.Add( OffsetX10_RadioButton );
   OffsetX_Sizer.Add( OffsetX100_RadioButton );
   OffsetX_Sizer.AddStretch();

   OffsetX_Control.SetSizer( OffsetX_Sizer );

   OffsetY_Label.SetText( "Y-Offset:" );
   OffsetY_Label.SetMinWidth( labelWidth1 );
   OffsetY_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OffsetY_SpinBox.SetRange( int( TheSNOffsetYParameter->MinimumValue() ), int( TheSNOffsetYParameter->MaximumValue() ) );
   OffsetY_SpinBox.SetFixedWidth( editWidth1 );
   OffsetY_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&SimplexNoiseInterface::__IntValueUpdated, w );

   OffsetY1_RadioButton.SetText( "x1" );
   OffsetY1_RadioButton.OnClick( (pcl::Button::click_event_handler)&SimplexNoiseInterface::__Click, w );
   OffsetY10_RadioButton.SetText( "x10" );
   OffsetY10_RadioButton.OnClick( (pcl::Button::click_event_handler)&SimplexNoiseInterface::__Click, w );
   OffsetY100_RadioButton.SetText( "x100" );
   OffsetY100_RadioButton.OnClick( (pcl::Button::click_event_handler)&SimplexNoiseInterface::__Click, w );

   OffsetY_Sizer.SetSpacing( 4 );
   OffsetY_Sizer.Add( OffsetY_Label );
   OffsetY_Sizer.Add( OffsetY_SpinBox );
   OffsetY_Sizer.AddSpacing( 8 );
   OffsetY_Sizer.Add( OffsetY1_RadioButton );
   OffsetY_Sizer.Add( OffsetY10_RadioButton );
   OffsetY_Sizer.Add( OffsetY100_RadioButton );
   OffsetY_Sizer.AddStretch();

   OffsetY_Control.SetSizer( OffsetY_Sizer );

   Operator_Label.SetText( "Operator:" );
   Operator_Label.SetMinWidth( labelWidth1 );
   Operator_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Operator_ComboBox.AddItem( "Copy" );
   Operator_ComboBox.AddItem( "Add" );
   Operator_ComboBox.AddItem( "Subtract" );
   Operator_ComboBox.AddItem( "Multiply" );
   Operator_ComboBox.AddItem( "Divide" );
   Operator_ComboBox.AddItem( "Power" );
   Operator_ComboBox.AddItem( "Absolute Difference" );
   Operator_ComboBox.AddItem( "Screen" );
   Operator_ComboBox.AddItem( "Bitwise OR" );
   Operator_ComboBox.AddItem( "Bitwise AND" );
   Operator_ComboBox.AddItem( "Bitwise XOR" );
   Operator_ComboBox.AddItem( "Bitwise NOR" );
   Operator_ComboBox.AddItem( "Bitwise NAND" );
   Operator_ComboBox.AddItem( "Bitwise XNOR" );
   Operator_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&SimplexNoiseInterface::__ItemSelected, w );

   Operator_Sizer.SetSpacing( 4 );
   Operator_Sizer.Add( Operator_Label );
   Operator_Sizer.Add( Operator_ComboBox );
   Operator_Sizer.AddStretch();

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Amount_NumericControl );
   Global_Sizer.Add( Scale_Control );
   Global_Sizer.Add( OffsetX_Control );
   Global_Sizer.Add( OffsetY_Control );
   Global_Sizer.Add( Operator_Sizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF SimplexNoiseInterface.cpp - Released 2017-04-14T23:07:12Z
