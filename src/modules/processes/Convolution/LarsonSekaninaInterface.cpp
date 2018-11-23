//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard Convolution Process Module Version 01.01.03.0272
// ----------------------------------------------------------------------------
// LarsonSekaninaInterface.cpp - Released 2018-11-23T18:45:58Z
// ----------------------------------------------------------------------------
// This file is part of the standard Convolution PixInsight module.
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

#include "LarsonSekaninaInterface.h"
#include "LarsonSekaninaProcess.h"
#include "LarsonSekaninaParameters.h"

//#include <pcl/RealTimePreview.h>

namespace pcl
{

// ----------------------------------------------------------------------------

LarsonSekaninaInterface* TheLarsonSekaninaInterface = 0;

// ----------------------------------------------------------------------------

#include "LarsonSekaninaIcon.xpm"

// ----------------------------------------------------------------------------

LarsonSekaninaInterface::LarsonSekaninaInterface() :
ProcessInterface(), instance( TheLarsonSekaninaProcess ), GUI( 0 )
{
   TheLarsonSekaninaInterface = this;
}

LarsonSekaninaInterface::~LarsonSekaninaInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString LarsonSekaninaInterface::Id() const
{
   return "LarsonSekanina";
}

MetaProcess* LarsonSekaninaInterface::Process() const
{
   return TheLarsonSekaninaProcess;
}

const char** LarsonSekaninaInterface::IconImageXPM() const
{
   return LarsonSekaninaIcon_XPM;
}

InterfaceFeatures LarsonSekaninaInterface::Features() const
{
   return InterfaceFeature::Default /*| InterfaceFeature::RealTimeButton*/;
}

void LarsonSekaninaInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void LarsonSekaninaInterface::ResetInstance()
{
   LarsonSekaninaInstance defaultInstance( TheLarsonSekaninaProcess );
   ImportProcess( defaultInstance );
}

bool LarsonSekaninaInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "LarsonSekanina" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheLarsonSekaninaProcess;
}

ProcessImplementation* LarsonSekaninaInterface::NewProcess() const
{
   return new LarsonSekaninaInstance( instance );
}

bool LarsonSekaninaInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const LarsonSekaninaInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a LarsonSekanina instance.";
   return false;
}

bool LarsonSekaninaInterface::RequiresInstanceValidation() const
{
   return true;
}

bool LarsonSekaninaInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void LarsonSekaninaInterface::UpdateControls()
{
   UpdateFilterControls();
   UpdateApplyModeControls();
   UpdateRangeControls();
}

void LarsonSekaninaInterface::UpdateFilterControls()
{
   if ( instance.radiusDiff <= 10 )
   {
      GUI->RadiusCoarse_Slider.SetValue( 10 );
      GUI->RadiusFine_Slider.SetValue( int( 10 * instance.radiusDiff ) );
      GUI->RadiusFine_Slider.Enable();
   }
   else
   {
      GUI->RadiusCoarse_Slider.SetValue( int( instance.radiusDiff = int( instance.radiusDiff ) ) );
      GUI->RadiusFine_Slider.SetValue( 100 );
      GUI->RadiusFine_Slider.Disable();
   }
   GUI->Radius_NumericEdit.SetValue( instance.radiusDiff );

   if ( instance.angleDiff <= 10 )
   {
      GUI->AngleCoarse_Slider.SetValue( 10 );
      GUI->AngleFine_Slider.SetValue( int( 10 * instance.angleDiff ) );
      GUI->AngleFine_Slider.Enable();
   }
   else
   {
      GUI->AngleCoarse_Slider.SetValue( int( instance.angleDiff = int( instance.angleDiff ) ) );
      GUI->AngleFine_Slider.SetValue( 100 );
      GUI->AngleFine_Slider.Disable();
   }
   GUI->Angle_NumericEdit.SetValue( instance.angleDiff );

   GUI->X_NumericEdit.SetValue( instance.center.x );
   GUI->Y_NumericEdit.SetValue( instance.center.y );

   GUI->Interpolation_ComboBox.SetCurrentItem( instance.interpolation );
}

void LarsonSekaninaInterface::UpdateApplyModeControls()
{
   GUI->Amount_NumericControl.SetValue( instance.amount );
   GUI->Threshold_NumericControl.SetValue( instance.threshold );
   GUI->Deringing_NumericControl.SetValue( instance.deringing );
   GUI->UseLuminance_CheckBox.SetChecked( instance.useLuminance );
   GUI->HighPassMode_CheckBox.SetChecked( instance.highPass );
}

void LarsonSekaninaInterface::UpdateRangeControls()
{
   GUI->RangeLow_NumericControl.SetValue( instance.rangeLow );
   GUI->RangeLow_NumericControl.Enable( !instance.disableExtension );

   GUI->RangeHigh_NumericControl.SetValue( instance.rangeHigh );
   GUI->RangeHigh_NumericControl.Enable( !instance.disableExtension );

   GUI->Disable_CheckBox.SetChecked( instance.disableExtension );
}

// ----------------------------------------------------------------------------

void LarsonSekaninaInterface::__RadiusEdit_ValueUpdated( NumericEdit& sender, double value )
{
   instance.radiusDiff = value;
   UpdateFilterControls();
}

void LarsonSekaninaInterface::__Radius_ValueUpdated( Slider& sender, int value )
{
   if ( sender == GUI->RadiusCoarse_Slider )
      instance.radiusDiff = value;
   else
      instance.radiusDiff = 0.1 * value;

   UpdateFilterControls();
}

void LarsonSekaninaInterface::__AngleEdit_ValueUpdated( NumericEdit& sender, double value )
{
   instance.angleDiff = value;
   UpdateFilterControls();
}

void LarsonSekaninaInterface::__Angle_ValueUpdated( Slider& sender, int value )
{
   if ( sender == GUI->AngleCoarse_Slider )
      instance.angleDiff = value;
   else
      instance.angleDiff = 0.1 * value;

   UpdateFilterControls();
}

void LarsonSekaninaInterface::__Coords_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->X_NumericEdit )
      instance.center.x = value;
   else if ( sender == GUI->Y_NumericEdit )
      instance.center.y = value;

   UpdateFilterControls();
}

void LarsonSekaninaInterface::__Interpolation_ItemSelected( ComboBox& sender, int itemIndex )
{
    instance.interpolation = itemIndex;
}

void LarsonSekaninaInterface::__Amount_ValueUpdated( NumericEdit& sender, double value )
{
   instance.amount = value;
   UpdateApplyModeControls();
}

void LarsonSekaninaInterface::__Threshold_ValueUpdated( NumericEdit& sender, double value )
{
   instance.threshold = value;
   UpdateApplyModeControls();
}

void LarsonSekaninaInterface::__Deringing_ValueUpdated( NumericEdit& sender, double value )
{
   instance.deringing = value;
   UpdateApplyModeControls();
}

void LarsonSekaninaInterface::__UseLuminance_Click( Button& sender, bool checked )
{
   instance.useLuminance = checked;
   UpdateApplyModeControls();
}

void LarsonSekaninaInterface::__HighPassMode_Click( Button& sender, bool checked )
{
   instance.highPass = checked;
   UpdateApplyModeControls();
}

void LarsonSekaninaInterface::__Range_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->RangeLow_NumericControl )
      instance.rangeLow = value;
   else if ( sender == GUI->RangeHigh_NumericControl )
      instance.rangeHigh = value;

   UpdateRangeControls();
}

void LarsonSekaninaInterface::__Disable_Click( Button& sender, bool checked )
{
   instance.disableExtension = checked;
   UpdateRangeControls();
}

// ----------------------------------------------------------------------------

LarsonSekaninaInterface::GUIData::GUIData( LarsonSekaninaInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Angular Increment:" ) + 'M' );
   int editWidth1 = fnt.Width( String( '0', 12 ) );
   int editWidth2 = fnt.Width( String( '0', 20 ) );
   int ui4 = w.LogicalPixelsToPhysical( 4 );

   //

   Filter_SectionBar.SetTitle( "Filter Parameters" );
   Filter_SectionBar.SetSection( Filter_Control );

   //

   Radius_NumericEdit.label.SetText( "Radial Increment:" );
   Radius_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Radius_NumericEdit.SetReal();
   Radius_NumericEdit.SetRange( TheLSRadiusDiffParameter->MinimumValue(), TheLSRadiusDiffParameter->MaximumValue() );
   Radius_NumericEdit.SetPrecision( TheLSRadiusDiffParameter->Precision() );
   Radius_NumericEdit.SetToolTip( "Radial increment in pixels to calculate the gradient." );
   Radius_NumericEdit.edit.SetFixedWidth( editWidth1 );
   Radius_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&LarsonSekaninaInterface::__RadiusEdit_ValueUpdated, w );

   RadiusCoarse_Slider.SetScaledMinWidth( 250 );
   RadiusCoarse_Slider.SetRange( 10, 100 );
   RadiusCoarse_Slider.SetFixedHeight( RoundInt( 0.75*Radius_NumericEdit.Height() ) );
   RadiusCoarse_Slider.SetToolTip( "Radius coarse: from 10 to 100 pixels." );
   RadiusCoarse_Slider.OnValueUpdated( (Slider::value_event_handler)&LarsonSekaninaInterface::__Radius_ValueUpdated, w );

   RadiusFine_Slider.SetScaledMinWidth( 250 );
   RadiusFine_Slider.SetRange( 0, 100 );
   RadiusFine_Slider.SetFixedHeight( RoundInt( 0.75*Radius_NumericEdit.Height() ) );
   RadiusFine_Slider.SetToolTip( "Radius fine: from 0 to 10 pixels" );
   RadiusFine_Slider.OnValueUpdated( (Slider::value_event_handler)&LarsonSekaninaInterface::__Radius_ValueUpdated, w );

   RadiusSliders_Sizer.SetSpacing( 2 );
   RadiusSliders_Sizer.Add( RadiusCoarse_Slider );
   RadiusSliders_Sizer.Add( RadiusFine_Slider );

   Radius_Sizer.SetSpacing( 4 );
   Radius_Sizer.Add( Radius_NumericEdit );
   Radius_Sizer.Add( RadiusSliders_Sizer, 100 );

   //

   Angle_NumericEdit.label.SetText( "Angular Increment:" );
   Angle_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Angle_NumericEdit.SetReal();
   Angle_NumericEdit.SetRange( TheLSAngleDiffParameter->MinimumValue(), TheLSAngleDiffParameter->MaximumValue() );
   Angle_NumericEdit.SetPrecision( TheLSAngleDiffParameter->Precision() );
   Angle_NumericEdit.SetToolTip( "Angular increment in degrees to calculate the Laplacian." );
   Angle_NumericEdit.edit.SetFixedWidth( editWidth1 );
   Angle_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&LarsonSekaninaInterface::__AngleEdit_ValueUpdated, w );

   AngleCoarse_Slider.SetScaledMinWidth( 250 );
   AngleCoarse_Slider.SetRange( 10, 90 );
   AngleCoarse_Slider.SetFixedHeight( RoundInt( 0.75*Angle_NumericEdit.Height() ) );
   AngleCoarse_Slider.SetToolTip( "Angle coarse: from 10 to 90 degrees." );
   AngleCoarse_Slider.OnValueUpdated( (Slider::value_event_handler)&LarsonSekaninaInterface::__Angle_ValueUpdated, w );

   AngleFine_Slider.SetScaledMinWidth( 250 );
   AngleFine_Slider.SetRange( 0, 100 );
   AngleFine_Slider.SetFixedHeight( RoundInt( 0.75*Angle_NumericEdit.Height() ) );
   AngleFine_Slider.SetToolTip( "Angle fine: from 0 to 10 degrees." );
   AngleFine_Slider.OnValueUpdated( (Slider::value_event_handler)&LarsonSekaninaInterface::__Angle_ValueUpdated, w );

   AngleSliders_Sizer.SetSpacing( 2 );
   AngleSliders_Sizer.Add( AngleCoarse_Slider );
   AngleSliders_Sizer.Add( AngleFine_Slider );

   Angle_Sizer.SetSpacing( 4 );
   Angle_Sizer.Add( Angle_NumericEdit );
   Angle_Sizer.Add( AngleSliders_Sizer, 100 );

   //

   X_NumericEdit.label.SetText( "X-Center:" );
   X_NumericEdit.label.SetFixedWidth( labelWidth1 );
   X_NumericEdit.SetReal();
   X_NumericEdit.SetRange( TheLSCenterXParameter->MinimumValue(), TheLSCenterXParameter->MaximumValue() );
   X_NumericEdit.SetPrecision( TheLSCenterXParameter->Precision() );
   X_NumericEdit.SetToolTip( "Center of polar coordinates, X-coordinate." );
   X_NumericEdit.edit.SetFixedWidth( editWidth2 );
   X_NumericEdit.sizer.AddStretch();
   X_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&LarsonSekaninaInterface::__Coords_ValueUpdated, w );

   Y_NumericEdit.label.SetText( "Y-Center:" );
   Y_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Y_NumericEdit.SetReal();
   Y_NumericEdit.SetRange( TheLSCenterYParameter->MinimumValue(), TheLSCenterYParameter->MaximumValue() );
   Y_NumericEdit.SetPrecision( TheLSCenterYParameter->Precision() );
   Y_NumericEdit.SetToolTip( "Center of polar coordinates, Y-coordinate." );
   Y_NumericEdit.edit.SetFixedWidth( editWidth2 );
   Y_NumericEdit.sizer.AddStretch();
   Y_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&LarsonSekaninaInterface::__Coords_ValueUpdated, w );

   //

   Interpolation_Label.SetText( "Interpolation:" );
   Interpolation_Label.SetFixedWidth( labelWidth1 );
   Interpolation_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Interpolation_ComboBox.AddItem( " Bilinear " );
   Interpolation_ComboBox.AddItem( " Bicubic " );
   Interpolation_ComboBox.AddItem( " Bicubic Spline " );
   Interpolation_ComboBox.AddItem( " Bicubic B-Spline " );
   Interpolation_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&LarsonSekaninaInterface::__Interpolation_ItemSelected, w );

   Interpolation_Sizer.SetSpacing( 4 );
   Interpolation_Sizer.Add( Interpolation_Label );
   Interpolation_Sizer.Add( Interpolation_ComboBox );

   //

   Filter_Sizer.SetSpacing( 6 );
   Filter_Sizer.Add( Radius_Sizer );
   Filter_Sizer.Add( Angle_Sizer );
   Filter_Sizer.AddSpacing( 8 );
   Filter_Sizer.Add( X_NumericEdit );
   Filter_Sizer.Add( Y_NumericEdit );
   Filter_Sizer.Add( Interpolation_Sizer );

   Filter_Control.SetSizer( Filter_Sizer );
   Filter_Control.AdjustToContents();

   //

   ApplyMode_SectionBar.SetTitle( "Filter Application" );
   ApplyMode_SectionBar.SetSection( ApplyMode_Control );

   Amount_NumericControl.label.SetText( "Amount:" );
   Amount_NumericControl.label.SetFixedWidth( labelWidth1 );
   Amount_NumericControl.slider.SetScaledMinWidth( 250 );
   Amount_NumericControl.slider.SetRange( 0, 110 );
   Amount_NumericControl.SetReal();
   Amount_NumericControl.SetRange( TheLSAmountParameter->MinimumValue(), TheLSAmountParameter->MaximumValue() );
   Amount_NumericControl.SetPrecision( TheLSAmountParameter->Precision() );
   Amount_NumericControl.SetToolTip( "Filtering strength." );
   Amount_NumericControl.edit.SetFixedWidth( editWidth1 );
   Amount_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LarsonSekaninaInterface::__Amount_ValueUpdated, w );

   Threshold_NumericControl.label.SetText( "Threshold:" );
   Threshold_NumericControl.label.SetFixedWidth( labelWidth1 );
   Threshold_NumericControl.slider.SetScaledMinWidth( 250 );
   Threshold_NumericControl.slider.SetRange( 0, 100 );
   Threshold_NumericControl.SetReal();
   Threshold_NumericControl.SetRange( TheLSThresholdParameter->MinimumValue(), TheLSThresholdParameter->MaximumValue() );
   Threshold_NumericControl.SetPrecision( TheLSThresholdParameter->Precision() );
   Threshold_NumericControl.SetToolTip( "Threshold value to protect low-contrast image features." );
   Threshold_NumericControl.edit.SetFixedWidth( editWidth1 );
   Threshold_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LarsonSekaninaInterface::__Threshold_ValueUpdated, w );

   Deringing_NumericControl.label.SetText( "Deringing:" );
   Deringing_NumericControl.label.SetFixedWidth( labelWidth1 );
   Deringing_NumericControl.slider.SetScaledMinWidth( 250 );
   Deringing_NumericControl.slider.SetRange( 0, 100 );
   Deringing_NumericControl.SetReal();
   Deringing_NumericControl.SetRange( TheLSDeringingParameter->MinimumValue(), TheLSDeringingParameter->MaximumValue() );
   Deringing_NumericControl.SetPrecision( TheLSDeringingParameter->Precision() );
   Deringing_NumericControl.SetToolTip( "Threshold value to fix dark rings around bright image features." );
   Deringing_NumericControl.edit.SetFixedWidth( editWidth1 );
   Deringing_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LarsonSekaninaInterface::__Deringing_ValueUpdated, w );

   UseLuminance_CheckBox.SetText( "Use luminance" );
   UseLuminance_CheckBox.SetToolTip( "Apply Larson-Sekanina to the luminance of color images exclusively." );
   UseLuminance_CheckBox.OnClick( (Button::click_event_handler)&LarsonSekaninaInterface::__UseLuminance_Click, w );

   HighPassMode_CheckBox.SetText( "High-Pass Mode" );
   HighPassMode_CheckBox.SetToolTip( "Use Larson-Sekanina as a high-pass filter." );
   HighPassMode_CheckBox.OnClick( (Button::click_event_handler)&LarsonSekaninaInterface::__HighPassMode_Click, w );

   UseLuminance_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   UseLuminance_Sizer.Add( UseLuminance_CheckBox );
   UseLuminance_Sizer.AddSpacing( 16 );
   UseLuminance_Sizer.Add( HighPassMode_CheckBox );
   UseLuminance_Sizer.AddStretch();

   ApplyMode_Sizer.SetSpacing( 6 );
   ApplyMode_Sizer.Add( Amount_NumericControl );
   ApplyMode_Sizer.Add( Threshold_NumericControl );
   ApplyMode_Sizer.Add( Deringing_NumericControl );
   ApplyMode_Sizer.Add( UseLuminance_Sizer );

   ApplyMode_Control.SetSizer( ApplyMode_Sizer );
   ApplyMode_Control.AdjustToContents();

   // -------------------------------------------------------------------------

   Range_SectionBar.SetTitle( "Dynamic Range Extension" );
   Range_SectionBar.SetSection( Range_Control );

   RangeLow_NumericControl.label.SetText( "Low Range:" );
   RangeLow_NumericControl.label.SetFixedWidth( labelWidth1 );
   RangeLow_NumericControl.slider.SetScaledMinWidth( 250 );
   RangeLow_NumericControl.slider.SetRange( 0, 100 );
   RangeLow_NumericControl.SetReal();
   RangeLow_NumericControl.SetRange( TheLSRangeLowParameter->MinimumValue(), TheLSRangeLowParameter->MaximumValue() );
   RangeLow_NumericControl.SetPrecision( TheLSRangeLowParameter->Precision() );
   RangeLow_NumericControl.SetToolTip( "Shadows dynamic range extension" );
   RangeLow_NumericControl.edit.SetFixedWidth( editWidth1 );
   RangeLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LarsonSekaninaInterface::__Range_ValueUpdated, w );

   RangeHigh_NumericControl.label.SetText( "High Range:" );
   RangeHigh_NumericControl.label.SetFixedWidth( labelWidth1 );
   RangeHigh_NumericControl.slider.SetScaledMinWidth( 250 );
   RangeHigh_NumericControl.slider.SetRange( 0, 100 );
   RangeHigh_NumericControl.SetReal();
   RangeHigh_NumericControl.SetRange( TheLSRangeHighParameter->MinimumValue(), TheLSRangeHighParameter->MaximumValue() );
   RangeHigh_NumericControl.SetPrecision( TheLSRangeHighParameter->Precision() );
   RangeHigh_NumericControl.SetToolTip( "Highlights dynamic range extension" );
   RangeHigh_NumericControl.edit.SetFixedWidth( editWidth1 );
   RangeHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&LarsonSekaninaInterface::__Range_ValueUpdated, w );

   Disable_CheckBox.SetText( "Disable" );
   Disable_CheckBox.SetToolTip( "Avalaible for Floating Point images only. *** Warning *** out-of-range values may arise!" );
   Disable_CheckBox.OnClick( (Button::click_event_handler)&LarsonSekaninaInterface::__Disable_Click, w );

   Disable_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Disable_Sizer.Add( Disable_CheckBox );
   Disable_Sizer.AddStretch();

   Range_Sizer.SetSpacing( 6 );
   Range_Sizer.Add( RangeLow_NumericControl );
   Range_Sizer.Add( RangeHigh_NumericControl );
   Range_Sizer.Add( Disable_Sizer );

   Range_Control.SetSizer( Range_Sizer );
   Range_Control.AdjustToContents();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Filter_SectionBar );
   Global_Sizer.Add( Filter_Control );
   Global_Sizer.Add( ApplyMode_SectionBar );
   Global_Sizer.Add( ApplyMode_Control );
   Global_Sizer.Add( Range_SectionBar );
   Global_Sizer.Add( Range_Control );

   //

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetMinWidth();

   Range_Control.Hide();

   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF LarsonSekaninaInterface.cpp - Released 2018-11-23T18:45:58Z
