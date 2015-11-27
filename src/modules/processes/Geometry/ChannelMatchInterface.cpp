//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0775
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.01.00.0304
// ----------------------------------------------------------------------------
// ChannelMatchInterface.cpp - Released 2015/11/26 16:00:12 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
//
// Copyright (c) 2003-2015 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "ChannelMatchInterface.h"
#include "ChannelMatchProcess.h"
#include "ChannelMatchParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ChannelMatchInterface* TheChannelMatchInterface = 0;

// ----------------------------------------------------------------------------

#include "ChannelMatchIcon.xpm"

// ----------------------------------------------------------------------------

ChannelMatchInterface::ChannelMatchInterface() :
ProcessInterface(), instance( TheChannelMatchProcess ), GUI( 0 )
{
   TheChannelMatchInterface = this;
}

ChannelMatchInterface::~ChannelMatchInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString ChannelMatchInterface::Id() const
{
   return "ChannelMatch";
}

MetaProcess* ChannelMatchInterface::Process() const
{
   return TheChannelMatchProcess;
}

const char** ChannelMatchInterface::IconImageXPM() const
{
   return ChannelMatchIcon_XPM;
}

void ChannelMatchInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void ChannelMatchInterface::ResetInstance()
{
   ChannelMatchInstance defaultInstance( TheChannelMatchProcess );
   ImportProcess( defaultInstance );
}

bool ChannelMatchInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   // ### Deferred initialization
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "ChannelMatch" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheChannelMatchProcess;
}

ProcessImplementation* ChannelMatchInterface::NewProcess() const
{
   return new ChannelMatchInstance( instance );
}

bool ChannelMatchInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const ChannelMatchInstance* r = dynamic_cast<const ChannelMatchInstance*>( &p );

   if ( r == 0 )
   {
      whyNot = "Not a ChannelMatch instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool ChannelMatchInterface::RequiresInstanceValidation() const
{
   return true;
}

bool ChannelMatchInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ChannelMatchInterface::UpdateControls()
{
   bool isR = instance.p_channelEnabled[0];
   bool isG = instance.p_channelEnabled[1];
   bool isB = instance.p_channelEnabled[2];

   //

   GUI->R_CheckBox.SetChecked( isR );

   GUI->R_XOffset_NumericEdit.SetValue( instance.p_channelOffset[0].x );
   GUI->R_XOffset_NumericEdit.Enable( isR );

   GUI->R_Left_ToolButton.Enable( isR );
   GUI->R_Right_ToolButton.Enable( isR );

   GUI->R_YOffset_NumericEdit.SetValue( instance.p_channelOffset[0].y );
   GUI->R_YOffset_NumericEdit.Enable( isR );

   GUI->R_Up_ToolButton.Enable( isR );
   GUI->R_Down_ToolButton.Enable( isR );

   GUI->R_Factor_NumericControl.SetValue( instance.p_channelFactor[0] );
   GUI->R_Factor_NumericControl.Enable( isR );

   //

   GUI->G_CheckBox.SetChecked( isG );

   GUI->G_XOffset_NumericEdit.SetValue( instance.p_channelOffset[1].x );
   GUI->G_XOffset_NumericEdit.Enable( isG );

   GUI->G_Left_ToolButton.Enable( isG );
   GUI->G_Right_ToolButton.Enable( isG );

   GUI->G_YOffset_NumericEdit.SetValue( instance.p_channelOffset[1].y );
   GUI->G_YOffset_NumericEdit.Enable( isG );

   GUI->G_Up_ToolButton.Enable( isG );
   GUI->G_Down_ToolButton.Enable( isG );

   GUI->G_Factor_NumericControl.SetValue( instance.p_channelFactor[1] );
   GUI->G_Factor_NumericControl.Enable( isG );

   //

   GUI->B_CheckBox.SetChecked( isB );

   GUI->B_XOffset_NumericEdit.SetValue( instance.p_channelOffset[2].x );
   GUI->B_XOffset_NumericEdit.Enable( isB );

   GUI->B_Left_ToolButton.Enable( isB );
   GUI->B_Right_ToolButton.Enable( isB );

   GUI->B_YOffset_NumericEdit.SetValue( instance.p_channelOffset[2].y );
   GUI->B_YOffset_NumericEdit.Enable( isB );

   GUI->B_Up_ToolButton.Enable( isB );
   GUI->B_Down_ToolButton.Enable( isB );

   GUI->B_Factor_NumericControl.SetValue( instance.p_channelFactor[2] );
   GUI->B_Factor_NumericControl.Enable( isB );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ChannelMatchInterface::__Channel_Click( Button& sender, bool checked )
{
   int i = -1;
   if ( sender == GUI->R_CheckBox )
      i = 0;
   else if ( sender == GUI->G_CheckBox )
      i = 1;
   else if ( sender == GUI->B_CheckBox )
      i = 2;

   if ( i >= 0 )
   {
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
}

void ChannelMatchInterface::__Channel_Offset_Click( Button& sender, bool /*checked*/ )
{
   int sign;
   float* item;
   NumericEdit* edit;

   if ( sender == GUI->R_Left_ToolButton )
   {
      sign = -1;
      item = &(instance.p_channelOffset[0].x);
      edit = &GUI->R_XOffset_NumericEdit;
   }
   else if ( sender == GUI->R_Right_ToolButton )
   {
      sign = +1;
      item = &(instance.p_channelOffset[0].x);
      edit = &GUI->R_XOffset_NumericEdit;
   }
   else if ( sender == GUI->R_Up_ToolButton )
   {
      sign = -1;
      item = &(instance.p_channelOffset[0].y);
      edit = &GUI->R_YOffset_NumericEdit;
   }
   else if ( sender == GUI->R_Down_ToolButton )
   {
      sign = +1;
      item = &(instance.p_channelOffset[0].y);
      edit = &GUI->R_YOffset_NumericEdit;
   }

   else if ( sender == GUI->G_Left_ToolButton )
   {
      sign = -1;
      item = &(instance.p_channelOffset[1].x);
      edit = &GUI->G_XOffset_NumericEdit;
   }
   else if ( sender == GUI->G_Right_ToolButton )
   {
      sign = +1;
      item = &(instance.p_channelOffset[1].x);
      edit = &GUI->G_XOffset_NumericEdit;
   }
   else if ( sender == GUI->G_Up_ToolButton )
   {
      sign = -1;
      item = &(instance.p_channelOffset[1].y);
      edit = &GUI->G_YOffset_NumericEdit;
   }
   else if ( sender == GUI->G_Down_ToolButton )
   {
      sign = +1;
      item = &(instance.p_channelOffset[1].y);
      edit = &GUI->G_YOffset_NumericEdit;
   }

   else if ( sender == GUI->B_Left_ToolButton )
   {
      sign = -1;
      item = &(instance.p_channelOffset[2].x);
      edit = &GUI->B_XOffset_NumericEdit;
   }
   else if ( sender == GUI->B_Right_ToolButton )
   {
      sign = +1;
      item = &(instance.p_channelOffset[2].x);
      edit = &GUI->B_XOffset_NumericEdit;
   }
   else if ( sender == GUI->B_Up_ToolButton )
   {
      sign = -1;
      item = &(instance.p_channelOffset[2].y);
      edit = &GUI->B_YOffset_NumericEdit;
   }
   else if ( sender == GUI->B_Down_ToolButton )
   {
      sign = +1;
      item = &(instance.p_channelOffset[2].y);
      edit = &GUI->B_YOffset_NumericEdit;
   }
   else
      return;

   *item = Range( Round( *item + sign*0.1F, 2 ), -100.0F, +100.0F );

   edit->SetValue( *item );
}

void ChannelMatchInterface::__Channel_Offset_ValueUpdated( NumericEdit& sender, double value )
{
        if ( sender == GUI->R_XOffset_NumericEdit )
      instance.p_channelOffset[0].x = value;
   else if ( sender == GUI->R_YOffset_NumericEdit )
      instance.p_channelOffset[0].y = value;

   else if ( sender == GUI->G_XOffset_NumericEdit )
      instance.p_channelOffset[1].x = value;
   else if ( sender == GUI->G_YOffset_NumericEdit )
      instance.p_channelOffset[1].y = value;

   else if ( sender == GUI->B_XOffset_NumericEdit )
      instance.p_channelOffset[2].x = value;
   else if ( sender == GUI->B_YOffset_NumericEdit )
      instance.p_channelOffset[2].y = value;
}

void ChannelMatchInterface::__Channel_Factor_ValueUpdated( NumericEdit& sender, double value )
{
        if ( sender == GUI->R_Factor_NumericControl )
      instance.p_channelFactor[0] = value;
   else if ( sender == GUI->G_Factor_NumericControl )
      instance.p_channelFactor[1] = value;
   else if ( sender == GUI->B_Factor_NumericControl )
      instance.p_channelFactor[2] = value;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ChannelMatchInterface::GUIData::GUIData( ChannelMatchInterface& w )
{
   R_CheckBox.SetText( "R" );
   R_CheckBox.OnClick( (Button::click_event_handler)&ChannelMatchInterface::__Channel_Click, w );

   G_CheckBox.SetText( "G" );
   G_CheckBox.OnClick( (Button::click_event_handler)&ChannelMatchInterface::__Channel_Click, w );

   B_CheckBox.SetText( "B" );
   B_CheckBox.OnClick( (Button::click_event_handler)&ChannelMatchInterface::__Channel_Click, w );

   Channels_Sizer.SetMargin( 6 );
   Channels_Sizer.Add( R_CheckBox );
   Channels_Sizer.Add( G_CheckBox );
   Channels_Sizer.Add( B_CheckBox );

   Channels_GroupBox.SetTitle( "RGB" );
   Channels_GroupBox.SetSizer( Channels_Sizer );
   Channels_GroupBox.AdjustToContents();
   Channels_GroupBox.SetFixedWidth();

   R_XOffset_NumericEdit.label.Hide();
   R_XOffset_NumericEdit.SetReal();
   R_XOffset_NumericEdit.SetRange( TheChannelXOffsetParameter->MinimumValue(), TheChannelXOffsetParameter->MaximumValue() );
   R_XOffset_NumericEdit.SetPrecision( TheChannelXOffsetParameter->Precision() );
   R_XOffset_NumericEdit.SetToolTip( "X-offset, red channel" );
   R_XOffset_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ChannelMatchInterface::__Channel_Offset_ValueUpdated, w );

   R_Left_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/move-left.png" ) ) );
   R_Left_ToolButton.SetScaledFixedSize( 20, 20 );
   R_Left_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   R_Left_ToolButton.OnClick( (ToolButton::click_event_handler)&ChannelMatchInterface::__Channel_Offset_Click, w );

   R_Right_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/move-right.png" ) ) );
   R_Right_ToolButton.SetScaledFixedSize( 20, 20 );
   R_Right_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   R_Right_ToolButton.OnClick( (ToolButton::click_event_handler)&ChannelMatchInterface::__Channel_Offset_Click, w );

   R_XOffset_Sizer.Add( R_XOffset_NumericEdit );
   R_XOffset_Sizer.AddSpacing( 4 );
   R_XOffset_Sizer.Add( R_Left_ToolButton );
   R_XOffset_Sizer.Add( R_Right_ToolButton );

   G_XOffset_NumericEdit.label.Hide();
   G_XOffset_NumericEdit.SetReal();
   G_XOffset_NumericEdit.SetRange( TheChannelXOffsetParameter->MinimumValue(), TheChannelXOffsetParameter->MaximumValue() );
   G_XOffset_NumericEdit.SetPrecision( TheChannelXOffsetParameter->Precision() );
   G_XOffset_NumericEdit.SetToolTip( "X-offset, green channel" );
   G_XOffset_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ChannelMatchInterface::__Channel_Offset_ValueUpdated, w );

   G_Left_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/move-left.png" ) ) );
   G_Left_ToolButton.SetScaledFixedSize( 20, 20 );
   G_Left_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   G_Left_ToolButton.OnClick( (ToolButton::click_event_handler)&ChannelMatchInterface::__Channel_Offset_Click, w );

   G_Right_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/move-right.png" ) ) );
   G_Right_ToolButton.SetScaledFixedSize( 20, 20 );
   G_Right_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   G_Right_ToolButton.OnClick( (ToolButton::click_event_handler)&ChannelMatchInterface::__Channel_Offset_Click, w );

   G_XOffset_Sizer.Add( G_XOffset_NumericEdit );
   G_XOffset_Sizer.AddSpacing( 4 );
   G_XOffset_Sizer.Add( G_Left_ToolButton );
   G_XOffset_Sizer.Add( G_Right_ToolButton );

   B_XOffset_NumericEdit.label.Hide();
   B_XOffset_NumericEdit.SetReal();
   B_XOffset_NumericEdit.SetRange( TheChannelXOffsetParameter->MinimumValue(), TheChannelXOffsetParameter->MaximumValue() );
   B_XOffset_NumericEdit.SetPrecision( TheChannelXOffsetParameter->Precision() );
   B_XOffset_NumericEdit.SetToolTip( "X-offset, blue channel" );
   B_XOffset_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ChannelMatchInterface::__Channel_Offset_ValueUpdated, w );

   B_Left_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/move-left.png" ) ) );
   B_Left_ToolButton.SetScaledFixedSize( 20, 20 );
   B_Left_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   B_Left_ToolButton.OnClick( (ToolButton::click_event_handler)&ChannelMatchInterface::__Channel_Offset_Click, w );

   B_Right_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/move-right.png" ) ) );
   B_Right_ToolButton.SetScaledFixedSize( 20, 20 );
   B_Right_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   B_Right_ToolButton.OnClick( (ToolButton::click_event_handler)&ChannelMatchInterface::__Channel_Offset_Click, w );

   B_XOffset_Sizer.Add( B_XOffset_NumericEdit );
   B_XOffset_Sizer.AddSpacing( 4 );
   B_XOffset_Sizer.Add( B_Left_ToolButton );
   B_XOffset_Sizer.Add( B_Right_ToolButton );

   XOffset_Sizer.SetMargin( 6 );
   XOffset_Sizer.SetSpacing( 4 );
   XOffset_Sizer.Add( R_XOffset_Sizer );
   XOffset_Sizer.Add( G_XOffset_Sizer );
   XOffset_Sizer.Add( B_XOffset_Sizer );

   XOffset_GroupBox.SetTitle( "X-Offset" );
   XOffset_GroupBox.SetSizer( XOffset_Sizer );

   R_YOffset_NumericEdit.label.Hide();
   R_YOffset_NumericEdit.SetReal();
   R_YOffset_NumericEdit.SetRange( TheChannelXOffsetParameter->MinimumValue(), TheChannelXOffsetParameter->MaximumValue() );
   R_YOffset_NumericEdit.SetPrecision( TheChannelXOffsetParameter->Precision() );
   R_YOffset_NumericEdit.SetToolTip( "Y-offset, red channel" );
   R_YOffset_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ChannelMatchInterface::__Channel_Offset_ValueUpdated, w );

   R_Up_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/move-up.png" ) ) );
   R_Up_ToolButton.SetScaledFixedSize( 20, 20 );
   R_Up_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   R_Up_ToolButton.OnClick( (ToolButton::click_event_handler)&ChannelMatchInterface::__Channel_Offset_Click, w );

   R_Down_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/move-down.png" ) ) );
   R_Down_ToolButton.SetScaledFixedSize( 20, 20 );
   R_Down_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   R_Down_ToolButton.OnClick( (ToolButton::click_event_handler)&ChannelMatchInterface::__Channel_Offset_Click, w );

   R_YOffset_Sizer.Add( R_YOffset_NumericEdit );
   R_YOffset_Sizer.AddSpacing( 4 );
   R_YOffset_Sizer.Add( R_Up_ToolButton );
   R_YOffset_Sizer.Add( R_Down_ToolButton );

   G_YOffset_NumericEdit.label.Hide();
   G_YOffset_NumericEdit.SetReal();
   G_YOffset_NumericEdit.SetRange( TheChannelXOffsetParameter->MinimumValue(), TheChannelXOffsetParameter->MaximumValue() );
   G_YOffset_NumericEdit.SetPrecision( TheChannelXOffsetParameter->Precision() );
   G_YOffset_NumericEdit.SetToolTip( "Y-offset, green channel" );
   G_YOffset_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ChannelMatchInterface::__Channel_Offset_ValueUpdated, w );

   G_Up_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/move-up.png" ) ) );
   G_Up_ToolButton.SetScaledFixedSize( 20, 20 );
   G_Up_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   G_Up_ToolButton.OnClick( (ToolButton::click_event_handler)&ChannelMatchInterface::__Channel_Offset_Click, w );

   G_Down_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/move-down.png" ) ) );
   G_Down_ToolButton.SetScaledFixedSize( 20, 20 );
   G_Down_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   G_Down_ToolButton.OnClick( (ToolButton::click_event_handler)&ChannelMatchInterface::__Channel_Offset_Click, w );

   G_YOffset_Sizer.Add( G_YOffset_NumericEdit );
   G_YOffset_Sizer.AddSpacing( 4 );
   G_YOffset_Sizer.Add( G_Up_ToolButton );
   G_YOffset_Sizer.Add( G_Down_ToolButton );

   B_YOffset_NumericEdit.label.Hide();
   B_YOffset_NumericEdit.SetReal();
   B_YOffset_NumericEdit.SetRange( TheChannelXOffsetParameter->MinimumValue(), TheChannelXOffsetParameter->MaximumValue() );
   B_YOffset_NumericEdit.SetPrecision( TheChannelXOffsetParameter->Precision() );
   B_YOffset_NumericEdit.SetToolTip( "Y-offset, blue channel" );
   B_YOffset_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ChannelMatchInterface::__Channel_Offset_ValueUpdated, w );

   B_Up_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/move-up.png" ) ) );
   B_Up_ToolButton.SetScaledFixedSize( 20, 20 );
   B_Up_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   B_Up_ToolButton.OnClick( (ToolButton::click_event_handler)&ChannelMatchInterface::__Channel_Offset_Click, w );

   B_Down_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/move-down.png" ) ) );
   B_Down_ToolButton.SetScaledFixedSize( 20, 20 );
   B_Down_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   B_Down_ToolButton.OnClick( (ToolButton::click_event_handler)&ChannelMatchInterface::__Channel_Offset_Click, w );

   B_YOffset_Sizer.Add( B_YOffset_NumericEdit );
   B_YOffset_Sizer.AddSpacing( 4 );
   B_YOffset_Sizer.Add( B_Up_ToolButton );
   B_YOffset_Sizer.Add( B_Down_ToolButton );

   YOffset_Sizer.SetMargin( 6 );
   YOffset_Sizer.SetSpacing( 4 );
   YOffset_Sizer.Add( R_YOffset_Sizer );
   YOffset_Sizer.Add( G_YOffset_Sizer );
   YOffset_Sizer.Add( B_YOffset_Sizer );

   YOffset_GroupBox.SetTitle( "Y-Offset" );
   YOffset_GroupBox.SetSizer( YOffset_Sizer );

   // -------------------------------------------------------------------------

   R_Factor_NumericControl.label.Hide();
   R_Factor_NumericControl.slider.SetScaledMinWidth( 200 );
   R_Factor_NumericControl.slider.SetRange( 0, 100 );
   R_Factor_NumericControl.SetReal();
   R_Factor_NumericControl.SetRange( TheChannelFactorParameter->MinimumValue(), TheChannelFactorParameter->MaximumValue() );
   R_Factor_NumericControl.SetPrecision( TheChannelFactorParameter->Precision() );
   R_Factor_NumericControl.SetToolTip( "Linear correction factor, red channel" );
   R_Factor_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ChannelMatchInterface::__Channel_Factor_ValueUpdated, w );

   G_Factor_NumericControl.label.Hide();
   G_Factor_NumericControl.slider.SetScaledMinWidth( 200 );
   G_Factor_NumericControl.slider.SetRange( 0, 100 );
   G_Factor_NumericControl.SetReal();
   G_Factor_NumericControl.SetRange( TheChannelFactorParameter->MinimumValue(), TheChannelFactorParameter->MaximumValue() );
   G_Factor_NumericControl.SetPrecision( TheChannelFactorParameter->Precision() );
   G_Factor_NumericControl.SetToolTip( "Linear correction factor, green channel" );
   G_Factor_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ChannelMatchInterface::__Channel_Factor_ValueUpdated, w );

   B_Factor_NumericControl.label.Hide();
   B_Factor_NumericControl.slider.SetScaledMinWidth( 200 );
   B_Factor_NumericControl.slider.SetRange( 0, 100 );
   B_Factor_NumericControl.SetReal();
   B_Factor_NumericControl.SetRange( TheChannelFactorParameter->MinimumValue(), TheChannelFactorParameter->MaximumValue() );
   B_Factor_NumericControl.SetPrecision( TheChannelFactorParameter->Precision() );
   B_Factor_NumericControl.SetToolTip( "Linear correction factor, blue channel" );
   B_Factor_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ChannelMatchInterface::__Channel_Factor_ValueUpdated, w );

   Factors_Sizer.SetMargin( 6 );
   Factors_Sizer.SetSpacing( 4 );
   Factors_Sizer.Add( R_Factor_NumericControl );
   Factors_Sizer.Add( G_Factor_NumericControl );
   Factors_Sizer.Add( B_Factor_NumericControl );

   Factors_GroupBox.SetTitle( "Linear Correction Factors" );
   Factors_GroupBox.SetSizer( Factors_Sizer );

   // -------------------------------------------------------------------------

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Channels_GroupBox );
   Global_Sizer.Add( XOffset_GroupBox );
   Global_Sizer.Add( YOffset_GroupBox );
   Global_Sizer.Add( Factors_GroupBox, 100 );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ChannelMatchInterface.cpp - Released 2015/11/26 16:00:12 UTC
