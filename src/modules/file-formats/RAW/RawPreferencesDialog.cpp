//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard RAW File Format Module Version 01.05.00.0405
// ----------------------------------------------------------------------------
// RawPreferencesDialog.cpp - Released 2018-11-01T11:07:09Z
// ----------------------------------------------------------------------------
// This file is part of the standard RAW PixInsight module.
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

#include "RawPreferencesDialog.h"

namespace pcl
{

// ----------------------------------------------------------------------------

RawPreferencesDialog::RawPreferencesDialog( RawPreferences& prf ) :
   preferences( prf ),
   savedPreferences( prf )
{
   pcl::Font fnt = Font();
   int labelWidth1 = fnt.Width( String( "FBDD noise reduction:" ) + 'M' );
   int ui4 = LogicalPixelsToPhysical( 4 );

   //

   CreateSuperPixels_CheckBox.SetText( "Create super-pixels" );
   CreateSuperPixels_CheckBox.SetToolTip(
      "<p>This option uses the Bayer matrix to create one <i>super-pixel</i> from each group of four pixels "
      "(for example, RGGB) without interpolation. The dimensions of the resulting image are thus divided by two.</p>"
      "<p>This option is ignored for frames acquired with X-Trans sensors.</p>" );
   CreateSuperPixels_CheckBox.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   CreateSuperPixels_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   CreateSuperPixels_Sizer.Add( CreateSuperPixels_CheckBox );
   CreateSuperPixels_Sizer.AddStretch();

   OutputRawRGB_CheckBox.SetText( "Create raw RGB image" );
   OutputRawRGB_CheckBox.SetToolTip(
      "<p>This option creates an RGB color image from the CFA matrix <i>as is</i>. No interpolation is done and each pixel "
      "is either red, green or blue, as defined by the CFA pattern.</p>" );
   OutputRawRGB_CheckBox.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   OutputRawRGB_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OutputRawRGB_Sizer.Add( OutputRawRGB_CheckBox );
   OutputRawRGB_Sizer.AddStretch();

   OutputCFA_CheckBox.SetText( "Create raw CFA image" );
   OutputCFA_CheckBox.SetToolTip(
      "<p>This option creates a grayscale image from the CFA matrix <i>as is</i>. No interpolation is done and the output "
      "is a monochrome image reproducing the CFA pattern, as stored in the device.</p>" );
   OutputCFA_CheckBox.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   OutputCFA_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   OutputCFA_Sizer.Add( OutputCFA_CheckBox );
   OutputCFA_Sizer.AddStretch();

   NoAutoFlip_CheckBox.SetText( "No image flip" );
   NoAutoFlip_CheckBox.SetToolTip(
      "<p>Do not flip the output image. If you disable this option, the flip operation specified by the camera "
      "(0, 90, 180 or 270 degrees) will be applied to the output image.</p>" );
   NoAutoFlip_CheckBox.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   NoAutoFlip_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   NoAutoFlip_Sizer.Add( NoAutoFlip_CheckBox );
   NoAutoFlip_Sizer.AddStretch();

   NoAutoCrop_CheckBox.SetText( "No image crop" );
   NoAutoCrop_CheckBox.SetToolTip(
      "<p>Do not crop unused areas of raw frames. If you enable this option, the dimensions of the output image will be the "
      "width and height of the whole raw frame stored in the device, including unused margins. This option is always disabled "
      "by default, and can only be applied when no interpolation is selected. When this option is enabled, no white balancing "
      "can be applied.</p>" );
   NoAutoCrop_CheckBox.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   NoAutoCrop_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   NoAutoCrop_Sizer.Add( NoAutoCrop_CheckBox );
   NoAutoCrop_Sizer.AddStretch();

   NoBlackPointCorrection_CheckBox.SetText( "No black point correction" );
   NoBlackPointCorrection_CheckBox.SetToolTip(
      "<p>When this option is enabled, no black point correction is applied to raw images to remove "
      "<i>darkness level pedestals</i>. This option must be enabled to load all raw images referred to a fixed "
      "zero black point value.</p>" );
   NoBlackPointCorrection_CheckBox.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   NoBlackPointCorrection_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   NoBlackPointCorrection_Sizer.Add( NoBlackPointCorrection_CheckBox );
   NoBlackPointCorrection_Sizer.AddStretch();

   NoClipHighlights_CheckBox.SetText( "No highlights clipping" );
   NoClipHighlights_CheckBox.SetToolTip(
      "<p>When this option is enabled, highlights will be left unclipped resulting in various shades of pink. "
      "With this option disabled, highlights will be clipped automatically to solid white.</p>" );
   NoClipHighlights_CheckBox.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   NoClipHighlights_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   NoClipHighlights_Sizer.Add( NoClipHighlights_CheckBox );
   NoClipHighlights_Sizer.AddStretch();

   NoiseThreshold_NumericControl.label.SetText( "Noise threshold:" );
   NoiseThreshold_NumericControl.label.SetMinWidth( labelWidth1 );
   NoiseThreshold_NumericControl.slider.SetRange( 0, 200 );
   NoiseThreshold_NumericControl.slider.SetScaledMinWidth( 250 );
   NoiseThreshold_NumericControl.SetInteger();
   NoiseThreshold_NumericControl.SetRange( 0, 2000 );
   NoiseThreshold_NumericControl.SetToolTip(
      "<p>Apply a wavelet-based noise reduction. The optimal noise threshold depends on the camera and the "
      "ISO speed used, but should normally be somewhere between 100 and 1000.</p>"
      "<p>Set this parameter to zero to disable noise reduction.</p>" );
   NoiseThreshold_NumericControl.OnValueUpdated( (NumericControl::value_event_handler)&RawPreferencesDialog::NumericControl_ValueUpdated, *this );

   OutputOptions_Sizer.SetMargin( 6 );
   OutputOptions_Sizer.SetSpacing( 6 );
   OutputOptions_Sizer.Add( CreateSuperPixels_Sizer );
   OutputOptions_Sizer.Add( OutputRawRGB_Sizer );
   OutputOptions_Sizer.Add( OutputCFA_Sizer );
   OutputOptions_Sizer.Add( NoAutoFlip_Sizer );
   OutputOptions_Sizer.Add( NoAutoCrop_Sizer );
   OutputOptions_Sizer.Add( NoBlackPointCorrection_Sizer );
   OutputOptions_Sizer.Add( NoClipHighlights_Sizer );
   OutputOptions_Sizer.Add( NoiseThreshold_NumericControl );

   OutputOptions_GroupBox.SetTitle( "Output Options" );
   OutputOptions_GroupBox.SetSizer( OutputOptions_Sizer );

   //

   Bilinear_RadioButton.SetText( "Bilinear" );
   Bilinear_RadioButton.SetToolTip( "<p>Bilinear interpolation. Very fast, "
      "but generates aliasing artifacts that degrade small-scale image structures.</p>" );
   Bilinear_RadioButton.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   Bilinear_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   Bilinear_Sizer.Add( Bilinear_RadioButton );
   Bilinear_Sizer.AddStretch();

   VNG_RadioButton.SetText( "VNG" );
   VNG_RadioButton.SetToolTip( "<p>Variable Number of Gradients interpolation algorithm.</p>" );
   VNG_RadioButton.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   VNG_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   VNG_Sizer.Add( VNG_RadioButton );
   VNG_Sizer.AddStretch();

   PPG_RadioButton.SetText( "PPG" );
   PPG_RadioButton.SetToolTip( "<p>Patterned Pixel Grouping interpolation algorithm.</p>" );
   PPG_RadioButton.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   PPG_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   PPG_Sizer.Add( PPG_RadioButton );
   PPG_Sizer.AddStretch();

   AHD_RadioButton.SetText( "AHD" );
   AHD_RadioButton.SetToolTip( "<p>Adaptive Homogeneity-Directed demosaicing algorithm.</p>" );
   AHD_RadioButton.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   AHD_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   AHD_Sizer.Add( AHD_RadioButton );
   AHD_Sizer.AddStretch();

   DCB_RadioButton.SetText( "DCB" );
   DCB_RadioButton.SetToolTip( "<p>DCB demosaicing algorithm by Jacek Gozdz.</p>" );
   DCB_RadioButton.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   DCB_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   DCB_Sizer.Add( DCB_RadioButton );
   DCB_Sizer.AddStretch();

   DCBIterations_Label.SetText( "DCB iterations:" );
   DCBIterations_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   DCBIterations_Label.SetMinWidth( labelWidth1 );
   DCBIterations_Label.SetToolTip( "<p>Number of iterations for the DCB interpolation algorithm.</p>" );

   DCBIterations_SpinBox.SetRange( 1, 5 );
   DCBIterations_SpinBox.SetToolTip( "<p>Number of iterations for the DCB interpolation algorithm.</p>" );
   DCBIterations_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&RawPreferencesDialog::SpinBox_ValueUpdated, *this );

   DCBIterations_Sizer.SetSpacing( 4 );
   DCBIterations_Sizer.Add( DCBIterations_Label );
   DCBIterations_Sizer.Add( DCBIterations_SpinBox );
   DCBIterations_Sizer.AddStretch();

   DCBRefinement_CheckBox.SetText( "DCB refinement" );
   DCBRefinement_CheckBox.SetToolTip( "<p>Optional DCB refinement step.</p>" );
   DCBRefinement_CheckBox.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   DCBRefinement_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   DCBRefinement_Sizer.Add( DCBRefinement_CheckBox );
   DCBRefinement_Sizer.AddStretch();

   DHT_RadioButton.SetText( "DHT" );
   DHT_RadioButton.SetToolTip( "<p>DHT demosaicing algorithm by Anton Petrusevich.</p>" );
   DHT_RadioButton.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   DHT_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   DHT_Sizer.Add( DHT_RadioButton );
   DHT_Sizer.AddStretch();

   AAHD_RadioButton.SetText( "AAHD" );
   AAHD_RadioButton.SetToolTip( "<p>Modified AHD demosaicing algorithm by Anton Petrusevich.</p>" );
   AAHD_RadioButton.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   AAHD_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   AAHD_Sizer.Add( AAHD_RadioButton );
   AAHD_Sizer.AddStretch();

   HalfSize_RadioButton.SetText( "Half-size color image" );
   HalfSize_RadioButton.SetToolTip( "<p>This option generates a demosaiced RGB color image with half "
      "the dimensions of the original raw frame. It is the fastest option available (twice as fast as bilinear) "
      "and is useful to load quick image previews.</p>" );
   HalfSize_RadioButton.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   HalfSize_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   HalfSize_Sizer.Add( HalfSize_RadioButton );
   HalfSize_Sizer.AddStretch();

   InterpolateAs4Colors_CheckBox.SetText( "Interpolate RGB as four colors" );
   InterpolateAs4Colors_CheckBox.SetToolTip( "<p>Use this option if the output shows false 2x2 meshes with VNG, or mazes with AHD.</p>" );
   InterpolateAs4Colors_CheckBox.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   InterpolateAs4Colors_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   InterpolateAs4Colors_Sizer.Add( InterpolateAs4Colors_CheckBox );
   InterpolateAs4Colors_Sizer.AddStretch();

   const char* fbddToolTip = "<p>FBDD noise reduction iterations.</p>"
      "<p>Jacek Gozdz's FBDD noise reduction algorithm is very efficient at removing small-scale bright artifacts, such as "
      "hot pixels in underexposed raw data. Quoted from the author's website (http://www.linuxphoto.org/html/fbdd.html):</p>"
      "<p><em>It works before demosaicing and can be followed by any demosaicing method. FBDD eliminates the most "
      "visible impulse noise leaving 99% of the details intact and significantly reduces chroma noise - this helps "
      "getting good results using different denoising techniques applied after demosaicing. It doesn't affect "
      "contrast or saturation of the image therefore no post processing is requied. The resulting image looks "
      "grainy and natural.</em></p>"
      "<p>A parameter value of zero will disable FBDD noise reduction. A value of one will apply the algorithm to the raw image. "
      "A value of two will apply an additional noise reduction step to the chroma components.</p>";

   FBDDNoiseReduction_Label.SetText( "FBDD noise reduction:" );
   FBDDNoiseReduction_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   FBDDNoiseReduction_Label.SetMinWidth( labelWidth1 );
   FBDDNoiseReduction_Label.SetToolTip( fbddToolTip );

   FBDDNoiseReduction_SpinBox.SetRange( 0, 2 );
   FBDDNoiseReduction_SpinBox.SetToolTip( fbddToolTip );
   FBDDNoiseReduction_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&RawPreferencesDialog::SpinBox_ValueUpdated, *this );

   FBDDNoiseReduction_Sizer.SetSpacing( 4 );
   FBDDNoiseReduction_Sizer.Add( FBDDNoiseReduction_Label );
   FBDDNoiseReduction_Sizer.Add( FBDDNoiseReduction_SpinBox );
   FBDDNoiseReduction_Sizer.AddStretch();

   Interpolation_Sizer.SetMargin( 6 );
   Interpolation_Sizer.SetSpacing( 6 );
   Interpolation_Sizer.Add( Bilinear_Sizer );
   Interpolation_Sizer.Add( VNG_Sizer );
   Interpolation_Sizer.Add( PPG_Sizer );
   Interpolation_Sizer.Add( AHD_Sizer );
   Interpolation_Sizer.Add( DCB_Sizer );
   Interpolation_Sizer.Add( DCBIterations_Sizer );
   Interpolation_Sizer.Add( DCBRefinement_Sizer );
   Interpolation_Sizer.Add( DHT_Sizer );
   Interpolation_Sizer.Add( AAHD_Sizer );
   Interpolation_Sizer.Add( HalfSize_Sizer );
   Interpolation_Sizer.Add( InterpolateAs4Colors_Sizer );
   Interpolation_Sizer.Add( FBDDNoiseReduction_Sizer );

   Interpolation_GroupBox.SetTitle( "Interpolation" );
   Interpolation_GroupBox.SetSizer( Interpolation_Sizer );

   //

   UseAutoWhiteBalance_CheckBox.SetText( "Automatic white balance" );
   UseAutoWhiteBalance_CheckBox.SetToolTip(
      "<p>Calculate white balancing factors by averaging the entire image.</p>" );
   UseAutoWhiteBalance_CheckBox.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   UseAutoWhiteBalance_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   UseAutoWhiteBalance_Sizer.Add( UseAutoWhiteBalance_CheckBox );
   UseAutoWhiteBalance_Sizer.AddStretch();

   UseCameraWhiteBalance_CheckBox.SetText( "Camera white balance" );
   UseCameraWhiteBalance_CheckBox.SetToolTip(
      "<p>Use the white balance specified by the camera, when available.</p>" );
   UseCameraWhiteBalance_CheckBox.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   UseCameraWhiteBalance_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   UseCameraWhiteBalance_Sizer.Add( UseCameraWhiteBalance_CheckBox );
   UseCameraWhiteBalance_Sizer.AddStretch();

   NoWhiteBalance_CheckBox.SetText( "No white balance" );
   NoWhiteBalance_CheckBox.SetToolTip(
      "<p>Do not apply any white balance scaling.</p>" );
   NoWhiteBalance_CheckBox.OnClick( (Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   NoWhiteBalance_Sizer.AddUnscaledSpacing( labelWidth1 + ui4 );
   NoWhiteBalance_Sizer.Add( NoWhiteBalance_CheckBox );
   NoWhiteBalance_Sizer.AddStretch();

   WhiteBalance_Sizer.SetMargin( 6 );
   WhiteBalance_Sizer.SetSpacing( 6 );
   WhiteBalance_Sizer.Add( UseAutoWhiteBalance_Sizer );
   WhiteBalance_Sizer.Add( UseCameraWhiteBalance_Sizer );
   WhiteBalance_Sizer.Add( NoWhiteBalance_Sizer );

   WhiteBalance_GroupBox.SetTitle( "White Balance" );
   WhiteBalance_GroupBox.SetSizer( WhiteBalance_Sizer );

   //

   PureRaw_PushButton.SetText( "Pure Raw" );
   PureRaw_PushButton.SetToolTip(
      "<p>Load a predefined set of options to read RAW digital camera files as pure raw data.</p>" );
   PureRaw_PushButton.OnClick( (pcl::Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   PureRGB_PushButton.SetText( "Demosaiced RGB" );
   PureRGB_PushButton.SetToolTip(
      "<p>Load a predefined set of options to read RAW digital camera files as demosaiced RGB images.</p>" );
   PureRGB_PushButton.OnClick( (pcl::Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.SetDefault();
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&RawPreferencesDialog::Button_Click, *this );

   BottomRow_Sizer.SetSpacing( 8 );
   BottomRow_Sizer.Add( PureRaw_PushButton );
   BottomRow_Sizer.Add( PureRGB_PushButton );
   BottomRow_Sizer.AddSpacing( 32 );
   BottomRow_Sizer.AddStretch();
   BottomRow_Sizer.Add( OK_PushButton );
   BottomRow_Sizer.Add( Cancel_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.Add( OutputOptions_GroupBox );
   Global_Sizer.Add( Interpolation_GroupBox );
   Global_Sizer.Add( WhiteBalance_GroupBox );
   Global_Sizer.AddSpacing( 8 );
   Global_Sizer.Add( BottomRow_Sizer );

   SetSizer( Global_Sizer );

   SetWindowTitle( "RAW Format Preferences" );

   OnShow( (Control::event_handler)&RawPreferencesDialog::Dialog_Show, *this );
   OnReturn( (Dialog::return_event_handler)&RawPreferencesDialog::Dialog_Return, *this );

   UpdateControls();
}

// ----------------------------------------------------------------------------

void RawPreferencesDialog::UpdateControls()
{
   CreateSuperPixels_CheckBox.SetChecked( preferences.createSuperPixels );
   OutputRawRGB_CheckBox.SetChecked( preferences.outputRawRGB );
   OutputCFA_CheckBox.SetChecked( preferences.outputCFA );
   NoAutoFlip_CheckBox.SetChecked( preferences.noAutoFlip );
   NoAutoCrop_CheckBox.SetChecked( preferences.noAutoCrop );
   NoBlackPointCorrection_CheckBox.SetChecked( preferences.noBlackPointCorrection );
   NoClipHighlights_CheckBox.SetChecked( preferences.noClipHighlights );
   NoiseThreshold_NumericControl.SetValue( preferences.noiseThreshold );

   Bilinear_RadioButton.SetChecked( preferences.interpolation == RawPreferences::Bilinear );
   VNG_RadioButton.SetChecked( preferences.interpolation == RawPreferences::VNG );
   PPG_RadioButton.SetChecked( preferences.interpolation == RawPreferences::PPG );
   AHD_RadioButton.SetChecked( preferences.interpolation == RawPreferences::AHD );

   bool dcb = preferences.interpolation == RawPreferences::DCB;
   DCB_RadioButton.SetChecked( dcb );
   DCBIterations_Label.Enable( dcb );
   DCBIterations_SpinBox.Enable( dcb );
   DCBIterations_SpinBox.SetValue( preferences.dcbIterations );
   DCBRefinement_CheckBox.Enable( dcb );
   DCBRefinement_CheckBox.SetChecked( preferences.dcbRefinement );

   DHT_RadioButton.SetChecked( preferences.interpolation == RawPreferences::DHT );
   AAHD_RadioButton.SetChecked( preferences.interpolation == RawPreferences::AAHD );
   HalfSize_RadioButton.SetChecked( preferences.interpolation == RawPreferences::HalfSize );
   InterpolateAs4Colors_CheckBox.SetChecked( preferences.interpolateAs4Colors );
   FBDDNoiseReduction_SpinBox.SetValue( Range( preferences.fbddNoiseReduction, 0, 2 ) );

   UseAutoWhiteBalance_CheckBox.SetChecked( preferences.useAutoWhiteBalance );
   UseCameraWhiteBalance_CheckBox.SetChecked( preferences.useCameraWhiteBalance );
   NoWhiteBalance_CheckBox.SetChecked( preferences.noWhiteBalance );

   bool raw = preferences.createSuperPixels ||
              preferences.outputRawRGB ||
              preferences.outputCFA;

   NoAutoCrop_CheckBox.Enable( raw );
   NoBlackPointCorrection_CheckBox.Enable( !raw );
   NoClipHighlights_CheckBox.Enable( !raw );
   NoiseThreshold_NumericControl.Enable( !raw );
   Interpolation_GroupBox.Enable( !raw );
   WhiteBalance_GroupBox.Enable( !raw || !preferences.noAutoCrop ); // no-auto-crop implies no-white-balance

   UseAutoWhiteBalance_CheckBox.Enable( !preferences.noWhiteBalance );
   UseCameraWhiteBalance_CheckBox.Enable( !preferences.noWhiteBalance );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void RawPreferencesDialog::NumericControl_ValueUpdated( NumericControl& sender, double value )
{
   if ( sender == NoiseThreshold_NumericControl )
      preferences.noiseThreshold = value;
}

// ----------------------------------------------------------------------------

void RawPreferencesDialog::SpinBox_ValueUpdated( SpinBox& sender, int value )
{
   if ( sender == DCBIterations_SpinBox )
      preferences.dcbIterations = value;
   else if ( sender == FBDDNoiseReduction_SpinBox )
      preferences.fbddNoiseReduction = value;
}

// ----------------------------------------------------------------------------

void RawPreferencesDialog::Button_Click( Button& sender, bool checked )
{
   if ( sender == UseAutoWhiteBalance_CheckBox )
   {
      preferences.useAutoWhiteBalance = checked;
   }
   else if ( sender == UseCameraWhiteBalance_CheckBox )
   {
      preferences.useCameraWhiteBalance = checked;
   }
   else if ( sender == NoWhiteBalance_CheckBox )
   {
      preferences.noWhiteBalance = checked;
   }
   else if ( sender == CreateSuperPixels_CheckBox )
   {
      preferences.createSuperPixels = checked;
      if ( checked )
      {
         preferences.outputRawRGB = false;
         preferences.outputCFA = false;
      }
   }
   else if ( sender == OutputRawRGB_CheckBox )
   {
      preferences.outputRawRGB = checked;
      if ( checked )
      {
         preferences.outputCFA = false;
         preferences.createSuperPixels = false;
      }
   }
   else if ( sender == OutputCFA_CheckBox )
   {
      preferences.outputCFA = checked;
      if ( checked )
      {
         preferences.outputRawRGB = false;
         preferences.createSuperPixels = false;
      }
   }
   else if ( sender == NoAutoFlip_CheckBox )
   {
      preferences.noAutoFlip = checked;
   }
   else if ( sender == NoAutoCrop_CheckBox )
   {
      preferences.noAutoCrop = checked;
   }
   else if ( sender == NoBlackPointCorrection_CheckBox )
   {
      preferences.noBlackPointCorrection = checked;
   }
   else if ( sender == NoClipHighlights_CheckBox )
   {
      preferences.noClipHighlights = checked;
   }
   else if ( sender == Bilinear_RadioButton )
   {
      preferences.interpolation = RawPreferences::Bilinear;
   }
   else if ( sender == VNG_RadioButton )
   {
      preferences.interpolation = RawPreferences::VNG;
   }
   else if ( sender == PPG_RadioButton )
   {
      preferences.interpolation = RawPreferences::PPG;
   }
   else if ( sender == AHD_RadioButton )
   {
      preferences.interpolation = RawPreferences::AHD;
   }
   else if ( sender == DCB_RadioButton )
   {
      preferences.interpolation = RawPreferences::DCB;
   }
   else if ( sender == DCBRefinement_CheckBox )
   {
      preferences.dcbRefinement = checked;
   }
   else if ( sender == DHT_RadioButton )
   {
      preferences.interpolation = RawPreferences::DHT;
   }
   else if ( sender == AAHD_RadioButton )
   {
      preferences.interpolation = RawPreferences::AAHD;
   }
   else if ( sender == HalfSize_RadioButton )
   {
      preferences.interpolation = RawPreferences::HalfSize;
   }
   else if ( sender == InterpolateAs4Colors_CheckBox )
   {
      preferences.interpolateAs4Colors = checked;
   }
   else if ( sender == PureRaw_PushButton )
   {
      preferences.interpolateAs4Colors = false;
      preferences.useAutoWhiteBalance = false;
      preferences.useCameraWhiteBalance = false;
      preferences.noWhiteBalance = true;
      preferences.createSuperPixels = false;
      preferences.outputRawRGB = false;
      preferences.outputCFA = true;
      preferences.noAutoFlip = true;
      preferences.noAutoCrop = false;
      preferences.noBlackPointCorrection = true;
      preferences.noClipHighlights = true;
   }
   else if ( sender == PureRGB_PushButton )
   {
      preferences.interpolation = RawPreferences::VNG;
      preferences.interpolateAs4Colors = false;
      preferences.useAutoWhiteBalance = false;
      preferences.useCameraWhiteBalance = true;
      preferences.noWhiteBalance = false;
      preferences.createSuperPixels = false;
      preferences.outputRawRGB = false;
      preferences.outputCFA = false;
      preferences.noAutoFlip = false;
      preferences.noAutoCrop = false;
      preferences.noBlackPointCorrection = false;
      preferences.noClipHighlights = false;
//       preferences.noiseThreshold = 0; // this is a very specific setting, don't change it
      preferences.dcbIterations = 3;
      preferences.dcbRefinement = false;
      preferences.fbddNoiseReduction = 2;
   }
   else if ( sender == OK_PushButton )
   {
      Ok();
   }
   else if ( sender == Cancel_PushButton )
   {
      Cancel();
   }

   UpdateControls();
}

// ----------------------------------------------------------------------------

void RawPreferencesDialog::Dialog_Show( Control& sender )
{
   // ### BUGFIX: the PureRGB button does not resize correctly upon creation,
   // so we have to force a minimum width here. So far this only happens on
   // Linux and macOS, not on Windows. Core bug? Qt bug?
   PureRGB_PushButton.SetMinWidth( Font().Width( PureRGB_PushButton.Text() + "MMMM" ) );
   AdjustToContents();
   SetFixedSize();
}

// ----------------------------------------------------------------------------

void RawPreferencesDialog::Dialog_Return( Dialog& sender, int retVal )
{
   if ( retVal != StdDialogCode::Ok )
      preferences = savedPreferences;
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RawPreferencesDialog.cpp - Released 2018-11-01T11:07:09Z
