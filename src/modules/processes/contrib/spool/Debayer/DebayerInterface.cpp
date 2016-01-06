//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard Debayer Process Module Version 01.04.03.0209
// ----------------------------------------------------------------------------
// DebayerInterface.cpp - Released 2015/12/18 08:55:08 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard Debayer PixInsight module.
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

#include <pcl/Settings.h>

#include "DebayerInterface.h"
#include "DebayerProcess.h"
#include "DebayerParameters.h"

// Settings keys
#define KEY_BAYERPATTERN   "BayerPattern"
#define KEY_DEBAYERMETHOD  "DebayerMethod"
#define KEY_EVALUATENOISE  "DebayerEvaluateNoise"
#define KEY_NOISEEVALALG   "DebayerNoiseEvaluationAlgorithm"

namespace pcl
{

// ----------------------------------------------------------------------------

DebayerInterface* TheDebayerInterface = 0;

// ----------------------------------------------------------------------------

#include "DebayerIcon.xpm"

// ----------------------------------------------------------------------------

DebayerInterface::DebayerInterface() :
ProcessInterface(), instance( TheDebayerProcess ), GUI( 0 )
{
   TheDebayerInterface = this;
}

DebayerInterface::~DebayerInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString DebayerInterface::Id() const
{
   return "Debayer";
}

MetaProcess* DebayerInterface::Process() const
{
   return TheDebayerProcess;
}

const char** DebayerInterface::IconImageXPM() const
{
   return Pixmap;
}

void DebayerInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void DebayerInterface::ResetInstance()
{
   DebayerInstance defaultInstance( TheDebayerProcess );
   ImportProcess( defaultInstance );
}

bool DebayerInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Debayer" );
      LoadSettings();
      UpdateControls();
   }

   dynamic = false;
   return &P == TheDebayerProcess;
}

ProcessImplementation* DebayerInterface::NewProcess() const
{
   return new DebayerInstance( instance );
}

bool DebayerInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const DebayerInstance* r = dynamic_cast<const DebayerInstance*>( &p );
   if ( r == 0 )
   {
      whyNot = "Not a Debayer instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool DebayerInterface::RequiresInstanceValidation() const
{
   return true;
}

bool DebayerInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DebayerInterface::UpdateControls()
{
   GUI->BayerPatternCombo.SetCurrentItem( instance.p_bayerPattern );

   GUI->DebayerMethodCombo.SetCurrentItem( instance.p_debayerMethod );

   GUI->EvaluateNoiseCheckBox.SetChecked( instance.p_evaluateNoise );

   GUI->NoiseEvaluation_Label.Enable( instance.p_evaluateNoise );

   GUI->NoiseEvaluation_ComboBox.SetCurrentItem( instance.p_noiseEvaluationAlgorithm );
   GUI->NoiseEvaluation_ComboBox.Enable( instance.p_evaluateNoise );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DebayerInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->BayerPatternCombo )
      instance.p_bayerPattern = itemIndex;
   else if ( sender == GUI->NoiseEvaluation_ComboBox )
      instance.p_noiseEvaluationAlgorithm = itemIndex;
   else if ( sender == GUI->DebayerMethodCombo )
      instance.p_debayerMethod = itemIndex;
}

void DebayerInterface::__ButtonClicked( Button& sender, bool checked )
{
   if ( sender == GUI->EvaluateNoiseCheckBox )
   {
      instance.p_evaluateNoise = checked;
      UpdateControls();
   }
   else if ( sender == GUI->Save_PushButton )
   {
      SaveSettings();
   }
   else if ( sender == GUI->Restore_PushButton )
   {
      LoadSettings();
      UpdateControls();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

DebayerInterface::GUIData::GUIData( DebayerInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Bayer/mosaic pattern:" ) ) + 16; // the longest label text

   // color parameters

   PatternLabel.SetText( "Bayer/mosaic pattern:" );
   PatternLabel.SetFixedWidth( labelWidth1 );
   PatternLabel.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   BayerPatternCombo.AddItem( "RGGB" );
   BayerPatternCombo.AddItem( "BGGR" );
   BayerPatternCombo.AddItem( "GBRG" );
   BayerPatternCombo.AddItem( "GRBG" );
   BayerPatternCombo.SetToolTip( "<p>Select the Bayer pattern of your color (OSC) camera.</p>" );
   BayerPatternCombo.OnItemSelected( (ComboBox::item_event_handler)&DebayerInterface::__ItemSelected, w );

   PatternSizer.SetSpacing( 4 );
   PatternSizer.Add( PatternLabel );
   PatternSizer.Add( BayerPatternCombo, 100 );

   DebayerMethodLabel.SetText( "Debayer method:" );
   DebayerMethodLabel.SetFixedWidth( labelWidth1 );
   DebayerMethodLabel.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   DebayerMethodCombo.AddItem( "SuperPixel" );
   DebayerMethodCombo.AddItem( "Bilinear" );
   DebayerMethodCombo.AddItem( "VNG" );
   DebayerMethodCombo.SetToolTip( "<p>Select the Debayering algorithm. SuperPixel creates a half sized "
      "image, very fast, good quality. The remaining methods create a full sized image, increasing in "
      "quality and computational overhead.</p>" );
   DebayerMethodCombo.OnItemSelected( (ComboBox::item_event_handler)&DebayerInterface::__ItemSelected, w );
   //DebayerMethodCombo.AddItem("AHD (not yet implemented)");

   DebayerMethodSizer.SetSpacing( 4 );
   DebayerMethodSizer.Add( DebayerMethodLabel );
   DebayerMethodSizer.Add( DebayerMethodCombo, 100 );

   EvaluateNoiseCheckBox.SetText( "Evaluate noise" );
   EvaluateNoiseCheckBox.SetToolTip( "<p>If this option is selected, Debayer will compute per-channel noise "
      "estimates for each target image using a wavelet-based algorithm (MRS noise evaluation by default). Noise "
      "estimates will be computed from debayered data and will be stored as NOISExxx FITS header keywords in the "
      "output files. These estimates can be used later by several processes and scripts, most notably by the "
      "ImageIntegration tool, which uses them by default for robust image weighting based on relative SNR values.</p>" );
   EvaluateNoiseCheckBox.OnClick( (Button::click_event_handler)&DebayerInterface::__ButtonClicked, w );

   EvaluateNoiseSizer.SetSpacing( 4 );
   EvaluateNoiseSizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 4 ) );
   EvaluateNoiseSizer.Add( EvaluateNoiseCheckBox );
   EvaluateNoiseSizer.AddStretch();

   const char* noiseEvaluationToolTip = "<p>Noise evaluation algorithm. This option selects an algorithm for automatic "
      "estimation of the standard deviation of the noise in the debayered images. In all cases noise estimates assume "
      "a Gaussian distribution of the noise.</p>"
      "<p>The iterative k-sigma clipping algorithm can be used as a last-resort option in cases where the MRS algorithm "
      "(see below) does not converge systematically. This can happen for images without detectable small-scale noise; "
      "for example, images that have been smoothed as a result of bilinear de-Bayer interpolation.</p>"
      "<p>The multiresolution support (MRS) noise estimation routine implements the iterative algorithm described by "
      "Jean-Luc Starck and Fionn Murtagh in their paper <em>Automatic Noise Estimation from the Multiresolution Support</em> "
      "(Publications of the Royal Astronomical Society of the Pacific, vol. 110, pp. 193-199). In our implementation, the "
      "standard deviation of the noise is evaluated on the first four wavelet layers. This is the most accurate algorithm "
      "available, and hence the default option.</p>";

   NoiseEvaluation_Label.SetText( "Noise evaluation:" );
   NoiseEvaluation_Label.SetFixedWidth( labelWidth1 );
   NoiseEvaluation_Label.SetToolTip( noiseEvaluationToolTip );
   NoiseEvaluation_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   NoiseEvaluation_ComboBox.AddItem( "Iterative K-Sigma Clipping" );
   NoiseEvaluation_ComboBox.AddItem( "Multiresolution Support" );
   NoiseEvaluation_ComboBox.SetToolTip( noiseEvaluationToolTip );
   NoiseEvaluation_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&DebayerInterface::__ItemSelected, w );

   NoiseEvaluation_Sizer.SetSpacing( 4 );
   NoiseEvaluation_Sizer.Add( NoiseEvaluation_Label );
   NoiseEvaluation_Sizer.Add( NoiseEvaluation_ComboBox );
   NoiseEvaluation_Sizer.AddStretch();

   Save_PushButton.SetText( "Save as Default" );
   Save_PushButton.SetMinWidth( labelWidth1 );
   Save_PushButton.SetCursor( StdCursor::Checkmark );
   Save_PushButton.OnClick( (ToolButton::click_event_handler)&DebayerInterface::__ButtonClicked, w );

   Restore_PushButton.SetText( "Restore from Default" );
   Restore_PushButton.SetCursor( StdCursor::Checkmark );
   Restore_PushButton.OnClick( (ToolButton::click_event_handler)&DebayerInterface::__ButtonClicked, w );

   ButtonSizer.SetSpacing( 6 );
   ButtonSizer.Add( Save_PushButton );
   ButtonSizer.AddStretch();
   ButtonSizer.Add( Restore_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( PatternSizer );
   Global_Sizer.Add( DebayerMethodSizer );
   Global_Sizer.Add( EvaluateNoiseSizer );
   Global_Sizer.Add( NoiseEvaluation_Sizer );
   Global_Sizer.Add( ButtonSizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void DebayerInterface::SaveSettings()
{
   Settings::Write ( KEY_BAYERPATTERN,  instance.p_bayerPattern );
   Settings::Write ( KEY_DEBAYERMETHOD, instance.p_debayerMethod );
   Settings::WriteI( KEY_EVALUATENOISE, instance.p_evaluateNoise );
   Settings::Write ( KEY_NOISEEVALALG,  instance.p_noiseEvaluationAlgorithm );
}

void DebayerInterface::LoadSettings()
{
   Settings::Read ( KEY_BAYERPATTERN,   instance.p_bayerPattern );
   Settings::Read ( KEY_DEBAYERMETHOD,  instance.p_debayerMethod );
   Settings::ReadI( KEY_EVALUATENOISE,  instance.p_evaluateNoise );
   Settings::Read ( KEY_NOISEEVALALG,   instance.p_noiseEvaluationAlgorithm );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF DebayerInterface.cpp - Released 2015/12/18 08:55:08 UTC
