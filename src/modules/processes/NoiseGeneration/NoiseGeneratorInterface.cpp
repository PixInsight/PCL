//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard NoiseGeneration Process Module Version 01.00.02.0344
// ----------------------------------------------------------------------------
// NoiseGeneratorInterface.cpp - Released 2018-12-12T09:25:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard NoiseGeneration PixInsight module.
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

#include "NoiseGeneratorInterface.h"
#include "NoiseGeneratorProcess.h"

namespace pcl
{

// ----------------------------------------------------------------------------

NoiseGeneratorInterface* TheNoiseGeneratorInterface = nullptr;

// ----------------------------------------------------------------------------

#include "NoiseGeneratorIcon.xpm"

// ----------------------------------------------------------------------------

NoiseGeneratorInterface::NoiseGeneratorInterface() :
   instance( TheNoiseGeneratorProcess )
{
   TheNoiseGeneratorInterface = this;
}

// ----------------------------------------------------------------------------

NoiseGeneratorInterface::~NoiseGeneratorInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

// ----------------------------------------------------------------------------

IsoString NoiseGeneratorInterface::Id() const
{
   return "NoiseGenerator";
}

// ----------------------------------------------------------------------------

MetaProcess* NoiseGeneratorInterface::Process() const
{
   return TheNoiseGeneratorProcess;
}

// ----------------------------------------------------------------------------

const char** NoiseGeneratorInterface::IconImageXPM() const
{
   return NoiseGeneratorIcon_XPM;
}

// ----------------------------------------------------------------------------

void NoiseGeneratorInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void NoiseGeneratorInterface::ResetInstance()
{
   NoiseGeneratorInstance defaultInstance( TheNoiseGeneratorProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool NoiseGeneratorInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "NoiseGenerator" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheNoiseGeneratorProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* NoiseGeneratorInterface::NewProcess() const
{
   return new NoiseGeneratorInstance( instance );
}

// ----------------------------------------------------------------------------

bool NoiseGeneratorInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const NoiseGeneratorInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a NoiseGenerator instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool NoiseGeneratorInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool NoiseGeneratorInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void NoiseGeneratorInterface::UpdateControls()
{
   GUI->Amount_NumericControl.SetValue( instance.p_amount );
   GUI->Uniform_RadioButton.SetChecked( instance.p_distribution == NGNoiseDistribution::Uniform );
   GUI->Normal_RadioButton.SetChecked( instance.p_distribution == NGNoiseDistribution::Normal );
   GUI->Poisson_RadioButton.SetChecked( instance.p_distribution == NGNoiseDistribution::Poisson );
   GUI->Impulsional_RadioButton.SetChecked( instance.p_distribution == NGNoiseDistribution::Impulsional );
   GUI->ImpulsionalProb_NumericControl.SetValue( instance.p_impulsionalNoiseProbability );
   GUI->ImpulsionalProb_NumericControl.Enable( instance.p_distribution == NGNoiseDistribution::Impulsional );
}

// ----------------------------------------------------------------------------

void NoiseGeneratorInterface::__ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Amount_NumericControl )
      instance.p_amount = value;
   else if ( sender == GUI->ImpulsionalProb_NumericControl )
      instance.p_impulsionalNoiseProbability = value;
}

// ----------------------------------------------------------------------------

void NoiseGeneratorInterface::__Click( Button& sender, bool checked )
{
   if ( sender == GUI->Uniform_RadioButton )
      instance.p_distribution = NGNoiseDistribution::Uniform;
   else if ( sender == GUI->Normal_RadioButton )
      instance.p_distribution = NGNoiseDistribution::Normal;
   else if ( sender == GUI->Poisson_RadioButton )
      instance.p_distribution = NGNoiseDistribution::Poisson;
   else if ( sender == GUI->Impulsional_RadioButton )
      instance.p_distribution = NGNoiseDistribution::Impulsional;

   GUI->ImpulsionalProb_NumericControl.Enable( instance.p_distribution == NGNoiseDistribution::Impulsional );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

NoiseGeneratorInterface::GUIData::GUIData( NoiseGeneratorInterface& w )
{
   Amount_NumericControl.label.SetText( "Amplitude:" );
   Amount_NumericControl.slider.SetRange( 0, 250 );
   Amount_NumericControl.slider.SetScaledMinWidth( 250 );
   Amount_NumericControl.SetReal();
   Amount_NumericControl.SetPrecision( TheNGNoiseAmountParameter->Precision() );
   Amount_NumericControl.SetRange( 0, 1 );
   Amount_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&NoiseGeneratorInterface::__ValueUpdated, w );

   Uniform_RadioButton.SetText( "Uniform" );
   Uniform_RadioButton.OnClick( (Button::click_event_handler)&NoiseGeneratorInterface::__Click, w );

   Normal_RadioButton.SetText( "Normal (aka Gaussian)" );
   Normal_RadioButton.OnClick( (Button::click_event_handler)&NoiseGeneratorInterface::__Click, w );

   Poisson_RadioButton.SetText( "Poisson" );
   Poisson_RadioButton.OnClick( (Button::click_event_handler)&NoiseGeneratorInterface::__Click, w );

   Impulsional_RadioButton.SetText( "Impulsional (Salt && Pepper)" );
   Impulsional_RadioButton.OnClick( (Button::click_event_handler)&NoiseGeneratorInterface::__Click, w );

   ImpulsionalProb_NumericControl.sizer.InsertSpacing( 0, 20 );
   ImpulsionalProb_NumericControl.label.SetText( "Probability:" );
   ImpulsionalProb_NumericControl.slider.SetRange( 0, 100 );
   ImpulsionalProb_NumericControl.slider.SetScaledMinWidth( 150 );
   ImpulsionalProb_NumericControl.SetReal();
   ImpulsionalProb_NumericControl.SetPrecision( TheNGImpulsionalNoiseProbabilityParameter->Precision() );
   ImpulsionalProb_NumericControl.SetRange( 0, 1 );
   ImpulsionalProb_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&NoiseGeneratorInterface::__ValueUpdated, w );

   Distribution_Sizer.SetMargin( 6 );
   Distribution_Sizer.SetSpacing( 4 );
   Distribution_Sizer.Add( Uniform_RadioButton );
   Distribution_Sizer.Add( Normal_RadioButton );
   Distribution_Sizer.Add( Poisson_RadioButton );
   Distribution_Sizer.Add( Impulsional_RadioButton );
   Distribution_Sizer.Add( ImpulsionalProb_NumericControl );

   Distribution_GroupBox.SetTitle( "Distribution" );
   Distribution_GroupBox.SetSizer( Distribution_Sizer );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Amount_NumericControl );
   Global_Sizer.Add( Distribution_GroupBox );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF NoiseGeneratorInterface.cpp - Released 2018-12-12T09:25:25Z
