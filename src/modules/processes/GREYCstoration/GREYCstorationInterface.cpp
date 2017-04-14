//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard GREYCstoration Process Module Version 01.00.02.0281
// ----------------------------------------------------------------------------
// GREYCstorationInterface.cpp - Released 2017-04-14T23:07:12Z
// ----------------------------------------------------------------------------
// This file is part of the standard GREYCstoration PixInsight module.
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

/******************************************************************************
 * CImg Library and GREYCstoration Algorithm:
 *   Copyright David Tschumperlé - http://www.greyc.ensicaen.fr/~dtschump/
 *
 * See:
 *   http://cimg.sourceforge.net
 *   http://www.greyc.ensicaen.fr/~dtschump/greycstoration/
 *
 * This software is governed by the CeCILL-C license under French law and
 * abiding by the rules of distribution of free software.  You can  use,
 * modify and or redistribute the software under the terms of the CeCILL-C
 * license as circulated by CEA, CNRS and INRIA at the following URL:
 * "http://www.cecill.info".
 *****************************************************************************/

#include "GREYCstorationInterface.h"
#include "GREYCstorationProcess.h"
#include "GREYCstorationParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------

GREYCstorationInterface* TheGREYCstorationInterface = 0;

// ----------------------------------------------------------------------------

#include "GREYCstorationIcon.xpm"

// ----------------------------------------------------------------------------

GREYCstorationInterface::GREYCstorationInterface() :
ProcessInterface(), instance( TheGREYCstorationProcess ), GUI( 0 )
{
   TheGREYCstorationInterface = this;
}

GREYCstorationInterface::~GREYCstorationInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString GREYCstorationInterface::Id() const
{
   return "GREYCstoration";
}

MetaProcess* GREYCstorationInterface::Process() const
{
   return TheGREYCstorationProcess;
}

const char** GREYCstorationInterface::IconImageXPM() const
{
   return GREYCstorationIcon_XPM;
}

void GREYCstorationInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void GREYCstorationInterface::ResetInstance()
{
   GREYCstorationInstance defaultInstance( TheGREYCstorationProcess );
   ImportProcess( defaultInstance );
}

bool GREYCstorationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "GREYCstoration" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheGREYCstorationProcess;
}

ProcessImplementation* GREYCstorationInterface::NewProcess() const
{
   return new GREYCstorationInstance( instance );
}

bool GREYCstorationInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   if ( dynamic_cast<const GREYCstorationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a GREYCstoration instance.";
   return false;
}

bool GREYCstorationInterface::RequiresInstanceValidation() const
{
   return true;
}

bool GREYCstorationInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void GREYCstorationInterface::UpdateControls()
{
   GUI->Iterations_SpinBox.SetValue( instance.numberOfIterations );
   GUI->Amplitude_NumericControl.SetValue( instance.amplitude );
   GUI->Sharpness_NumericControl.SetValue( instance.sharpness );
   GUI->Anisotropy_NumericControl.SetValue( instance.anisotropy );
   GUI->Alpha_NumericControl.SetValue( instance.alpha );
   GUI->Sigma_NumericControl.SetValue( instance.sigma );
   GUI->SpatialStepSize_NumericControl.SetValue( instance.spatialStepSize );
   GUI->AngularStepSize_NumericControl.SetValue( instance.angularStepSize );
   GUI->Precision_NumericControl.SetValue( instance.precision );
   GUI->Interpolation_ComboBox.SetCurrentItem( instance.interpolation );
   GUI->FastApproximation_CheckBox.SetChecked( instance.fastApproximation );
   GUI->CoupledChannels_CheckBox.SetChecked( instance.coupledChannels );
}

// ----------------------------------------------------------------------------

void GREYCstorationInterface::__ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Amplitude_NumericControl )
      instance.amplitude = value;
   else if ( sender == GUI->Sharpness_NumericControl )
      instance.sharpness = value;
   else if ( sender == GUI->Anisotropy_NumericControl )
      instance.anisotropy = value;
   else if ( sender == GUI->Alpha_NumericControl )
      instance.alpha = value;
   else if ( sender == GUI->Sigma_NumericControl )
      instance.sigma = value;
   else if ( sender == GUI->SpatialStepSize_NumericControl )
      instance.spatialStepSize = value;
   else if ( sender == GUI->AngularStepSize_NumericControl )
      instance.angularStepSize = value;
   else if ( sender == GUI->Precision_NumericControl )
      instance.precision = value;
}

void GREYCstorationInterface::__IntValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->Iterations_SpinBox )
      instance.numberOfIterations = value;
}

void GREYCstorationInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Interpolation_ComboBox )
      instance.interpolation = itemIndex;
}

void GREYCstorationInterface::__Click( Button& sender, bool checked )
{
   if ( sender == GUI->FastApproximation_CheckBox )
      instance.fastApproximation = checked;
   else if ( sender == GUI->CoupledChannels_CheckBox )
      instance.coupledChannels = checked;
}

// ----------------------------------------------------------------------------

GREYCstorationInterface::GUIData::GUIData( GREYCstorationInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Angular step size:" ) );
   int editWidth1 = fnt.Width( String( '0', 7 ) );
   int comboWidth1 = fnt.Width( String( " 2nd-order Runge-Kutta " ) + String( 'M', 4 ) );

   //

   Iterations_Label.SetText( "Iterations:" );
   Iterations_Label.SetMinWidth( labelWidth1 );
   Iterations_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Iterations_SpinBox.SetRange( int( TheGREYCsIterationsParameter->MinimumValue() ),
                                int( TheGREYCsIterationsParameter->MaximumValue() ) );
   Iterations_SpinBox.SetFixedWidth( editWidth1 );
   Iterations_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&GREYCstorationInterface::__IntValueUpdated, w );

   Iterations_Sizer.SetSpacing( 4 );
   Iterations_Sizer.Add( Iterations_Label );
   Iterations_Sizer.Add( Iterations_SpinBox );
   Iterations_Sizer.AddStretch();

   Amplitude_NumericControl.label.SetText( "Amplitude:" );
   Amplitude_NumericControl.label.SetFixedWidth( labelWidth1 );
   Amplitude_NumericControl.slider.SetScaledMinWidth( 220 );
   Amplitude_NumericControl.slider.SetRange( 0, 100 );
   Amplitude_NumericControl.SetReal();
   Amplitude_NumericControl.SetRange( TheGREYCsAmplitudeParameter->MinimumValue(), TheGREYCsAmplitudeParameter->MaximumValue() );
   Amplitude_NumericControl.SetPrecision( TheGREYCsAmplitudeParameter->Precision() );
   Amplitude_NumericControl.SetToolTip( "Regularization strength per iteration" );
   Amplitude_NumericControl.edit.SetFixedWidth( editWidth1 );
   Amplitude_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&GREYCstorationInterface::__ValueUpdated, w );

   Sharpness_NumericControl.label.SetText( "Sharpness:" );
   Sharpness_NumericControl.label.SetFixedWidth( labelWidth1 );
   Sharpness_NumericControl.slider.SetScaledMinWidth( 220 );
   Sharpness_NumericControl.slider.SetRange( 0, 125 );
   Sharpness_NumericControl.SetReal();
   Sharpness_NumericControl.SetRange( TheGREYCsSharpnessParameter->MinimumValue(), TheGREYCsSharpnessParameter->MaximumValue() );
   Sharpness_NumericControl.SetPrecision( TheGREYCsSharpnessParameter->Precision() );
   Sharpness_NumericControl.SetToolTip( "Contour preservation" );
   Sharpness_NumericControl.edit.SetFixedWidth( editWidth1 );
   Sharpness_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&GREYCstorationInterface::__ValueUpdated, w );

   Anisotropy_NumericControl.label.SetText( "Anisotropy:" );
   Anisotropy_NumericControl.label.SetFixedWidth( labelWidth1 );
   Anisotropy_NumericControl.slider.SetScaledMinWidth( 220 );
   Anisotropy_NumericControl.slider.SetRange( 0, 100 );
   Anisotropy_NumericControl.SetReal();
   Anisotropy_NumericControl.SetRange( TheGREYCsAnisotropyParameter->MinimumValue(), TheGREYCsAnisotropyParameter->MaximumValue() );
   Anisotropy_NumericControl.SetPrecision( TheGREYCsAnisotropyParameter->Precision() );
   Anisotropy_NumericControl.SetToolTip( "Smoothing anisotropy" );
   Anisotropy_NumericControl.edit.SetFixedWidth( editWidth1 );
   Anisotropy_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&GREYCstorationInterface::__ValueUpdated, w );

   Alpha_NumericControl.label.SetText( "Noise scale:" );
   Alpha_NumericControl.label.SetFixedWidth( labelWidth1 );
   Alpha_NumericControl.slider.SetScaledMinWidth( 220 );
   Alpha_NumericControl.slider.SetRange( 0, 150 );
   Alpha_NumericControl.SetReal();
   Alpha_NumericControl.SetRange( TheGREYCsAlphaParameter->MinimumValue(), TheGREYCsAlphaParameter->MaximumValue() );
   Alpha_NumericControl.SetPrecision( TheGREYCsAlphaParameter->Precision() );
   Alpha_NumericControl.SetToolTip( "Noise scale" );
   Alpha_NumericControl.edit.SetFixedWidth( editWidth1 );
   Alpha_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&GREYCstorationInterface::__ValueUpdated, w );

   Sigma_NumericControl.label.SetText( "Regularity:" );
   Sigma_NumericControl.label.SetFixedWidth( labelWidth1 );
   Sigma_NumericControl.slider.SetScaledMinWidth( 220 );
   Sigma_NumericControl.slider.SetRange( 0, 150 );
   Sigma_NumericControl.SetReal();
   Sigma_NumericControl.SetRange( TheGREYCsSigmaParameter->MinimumValue(), TheGREYCsSigmaParameter->MaximumValue() );
   Sigma_NumericControl.SetPrecision( TheGREYCsSigmaParameter->Precision() );
   Sigma_NumericControl.SetToolTip( "Geometry regularity" );
   Sigma_NumericControl.edit.SetFixedWidth( editWidth1 );
   Sigma_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&GREYCstorationInterface::__ValueUpdated, w );

   SpatialStepSize_NumericControl.label.SetText( "Spatial step size:" );
   SpatialStepSize_NumericControl.label.SetFixedWidth( labelWidth1 );
   SpatialStepSize_NumericControl.slider.SetScaledMinWidth( 220 );
   SpatialStepSize_NumericControl.slider.SetRange( 1, 200 );
   SpatialStepSize_NumericControl.SetReal();
   SpatialStepSize_NumericControl.SetRange( TheGREYCsSpatialStepSizeParameter->MinimumValue(), TheGREYCsSpatialStepSizeParameter->MaximumValue() );
   SpatialStepSize_NumericControl.SetPrecision( TheGREYCsSpatialStepSizeParameter->Precision() );
   SpatialStepSize_NumericControl.SetToolTip( "Spatial integration step" );
   SpatialStepSize_NumericControl.edit.SetFixedWidth( editWidth1 );
   SpatialStepSize_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&GREYCstorationInterface::__ValueUpdated, w );

   AngularStepSize_NumericControl.label.SetText( "Angular step size:" );
   AngularStepSize_NumericControl.label.SetFixedWidth( labelWidth1 );
   AngularStepSize_NumericControl.slider.SetScaledMinWidth( 220 );
   AngularStepSize_NumericControl.slider.SetRange( 1, 100 );
   AngularStepSize_NumericControl.SetReal();
   AngularStepSize_NumericControl.SetRange( TheGREYCsAngularStepSizeParameter->MinimumValue(), TheGREYCsAngularStepSizeParameter->MaximumValue() );
   AngularStepSize_NumericControl.SetPrecision( TheGREYCsAngularStepSizeParameter->Precision() );
   AngularStepSize_NumericControl.SetToolTip( "Angular integration step (degrees)" );
   AngularStepSize_NumericControl.edit.SetFixedWidth( editWidth1 );
   AngularStepSize_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&GREYCstorationInterface::__ValueUpdated, w );

   Precision_NumericControl.label.SetText( "Precision:" );
   Precision_NumericControl.label.SetFixedWidth( labelWidth1 );
   Precision_NumericControl.slider.SetScaledMinWidth( 220 );
   Precision_NumericControl.slider.SetRange( 1, 100 );
   Precision_NumericControl.SetReal();
   Precision_NumericControl.SetRange( TheGREYCsPrecisionParameter->MinimumValue(), TheGREYCsPrecisionParameter->MaximumValue() );
   Precision_NumericControl.SetPrecision( TheGREYCsPrecisionParameter->Precision() );
   Precision_NumericControl.SetToolTip( "Computation precision" );
   Precision_NumericControl.edit.SetFixedWidth( editWidth1 );
   Precision_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&GREYCstorationInterface::__ValueUpdated, w );

   Interpolation_Label.SetText( "Interpolation:" );
   Interpolation_Label.SetMinWidth( labelWidth1 );
   Interpolation_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Interpolation_ComboBox.AddItem( " Nearest neighbor " );
   Interpolation_ComboBox.AddItem( " Bilinear " );
   Interpolation_ComboBox.AddItem( " 2nd-order Runge-Kutta " );
   Interpolation_ComboBox.SetFixedWidth( comboWidth1 );
   Interpolation_ComboBox.SetToolTip( "Interpolation type" );
   Interpolation_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&GREYCstorationInterface::__ItemSelected, w );

   Interpolation_Sizer.SetSpacing( 4 );
   Interpolation_Sizer.Add( Interpolation_Label );
   Interpolation_Sizer.Add( Interpolation_ComboBox );
   Interpolation_Sizer.AddStretch();

   FastApproximation_CheckBox.SetText( "Fast approximation" );
   FastApproximation_CheckBox.SetToolTip( "Enable fast approximation" );
   FastApproximation_CheckBox.OnClick( (pcl::Button::click_event_handler)&GREYCstorationInterface::__Click, w );

   CoupledChannels_CheckBox.SetText( "Coupled channels" );
   CoupledChannels_CheckBox.SetToolTip( "Enable multichannel GREYCstoration processing for color images" );
   CoupledChannels_CheckBox.OnClick( (pcl::Button::click_event_handler)&GREYCstorationInterface::__Click, w );

   FastApproximation_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   FastApproximation_Sizer.Add( FastApproximation_CheckBox );
   FastApproximation_Sizer.AddSpacing( 12 );
   FastApproximation_Sizer.Add( CoupledChannels_CheckBox );
   FastApproximation_Sizer.AddStretch();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Iterations_Sizer );
   Global_Sizer.Add( Amplitude_NumericControl );
   Global_Sizer.Add( Sharpness_NumericControl );
   Global_Sizer.Add( Anisotropy_NumericControl );
   Global_Sizer.Add( Alpha_NumericControl );
   Global_Sizer.Add( Sigma_NumericControl );
   Global_Sizer.Add( SpatialStepSize_NumericControl );
   Global_Sizer.Add( AngularStepSize_NumericControl );
   Global_Sizer.Add( Precision_NumericControl );
   Global_Sizer.Add( Interpolation_Sizer );
   Global_Sizer.Add( FastApproximation_Sizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF GREYCstorationInterface.cpp - Released 2017-04-14T23:07:12Z
