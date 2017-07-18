//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0869
// ----------------------------------------------------------------------------
// Standard TIFF File Format Module Version 01.00.07.0339
// ----------------------------------------------------------------------------
// TIFFPreferencesDialog.cpp - Released 2017-07-18T16:14:10Z
// ----------------------------------------------------------------------------
// This file is part of the standard TIFF PixInsight module.
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

#include "TIFFPreferencesDialog.h"

#include <float.h>

namespace pcl
{

// ----------------------------------------------------------------------------

TIFFPreferencesDialog::TIFFPreferencesDialog(
   const TIFFFormat::OutOfRangePolicyOptions& r, const TIFFFormat::EmbeddingOverrides& o, const TIFFImageOptions& f ) :
Dialog(), outOfRange( r ), overrides( o ), tiffOptions( f )
{
   pcl::Font fnt = Font();
   int labelWidth = fnt.Width( String( "64-bit IEEE 754 floating point" ) + 'M' );
   int labelWidth1 = fnt.Width( String( "How to proceed:" ) + 'M' );
   int editWidth = fnt.Width( String( '0', 30 ) );

   //

   LowerRange_NumericEdit.label.SetText( "Lower Range:" );
   LowerRange_NumericEdit.label.SetMinWidth( labelWidth1 );
   LowerRange_NumericEdit.SetReal( true );
   LowerRange_NumericEdit.SetRange( -DBL_MAX, +DBL_MAX );
   LowerRange_NumericEdit.SetPrecision( 15 );
   LowerRange_NumericEdit.EnableScientificNotation();
   LowerRange_NumericEdit.SetScientificNotationTriggerExponent( 5 );
   LowerRange_NumericEdit.edit.SetFixedWidth( editWidth );
   LowerRange_NumericEdit.sizer.AddStretch();
   LowerRange_NumericEdit.SetToolTip( "<p>Lower range of read TIFF floating point pixel samples</p>" );
   LowerRange_NumericEdit.SetValue( outOfRange.lowerRange );

   UpperRange_NumericEdit.label.SetText( "Upper Range:" );
   UpperRange_NumericEdit.label.SetMinWidth( labelWidth1 );
   UpperRange_NumericEdit.SetReal( true );
   UpperRange_NumericEdit.SetRange( -DBL_MAX, +DBL_MAX );
   UpperRange_NumericEdit.SetPrecision( 15 );
   UpperRange_NumericEdit.EnableScientificNotation();
   UpperRange_NumericEdit.SetScientificNotationTriggerExponent( 5 );
   UpperRange_NumericEdit.edit.SetFixedWidth( editWidth );
   UpperRange_NumericEdit.sizer.AddStretch();
   UpperRange_NumericEdit.SetToolTip( "<p>Upper range of read TIFF floating point pixel samples</p>" );
   UpperRange_NumericEdit.SetValue( outOfRange.upperRange );

   ReadRange_Sizer.SetMargin( 6 );
   ReadRange_Sizer.SetSpacing( 4 );
   ReadRange_Sizer.Add( LowerRange_NumericEdit );
   ReadRange_Sizer.Add( UpperRange_NumericEdit );

   ReadRange_GroupBox.SetTitle( "Default Floating Point Input Range" );
   ReadRange_GroupBox.SetSizer( ReadRange_Sizer );
   ReadRange_GroupBox.AdjustToContents();

   //

   const char* readRangeModeToolTip =
   "<p>This parameter specifies how to proceed when pixel values in a read "
   "TIFF image exceed the default input range.</p>"
   "<p><b>** Important **</b> If the 'Ignore' option is selected for 'Action' "
   "(see below), this option will be ignored (not applied). Please read the tooltip "
   "information provided for the 'Action' combo box below.</p>";

   ReadRangeMode_Label.SetText( "How to proceed:" );
   ReadRangeMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ReadRangeMode_Label.SetMinWidth( labelWidth1 );
   ReadRangeMode_Label.SetToolTip( readRangeModeToolTip );

   ReadRangeMode_ComboBox.AddItem( "Normalize to the default input range" );
   ReadRangeMode_ComboBox.AddItem( "Ask on out-of-range read values" );
   ReadRangeMode_ComboBox.AddItem( "Always ask" );
   ReadRangeMode_ComboBox.SetToolTip( readRangeModeToolTip );
   ReadRangeMode_ComboBox.SetCurrentItem( outOfRange.outOfRangePolicy );

   ReadRangeMode_Sizer.SetSpacing( 4 );
   ReadRangeMode_Sizer.Add( ReadRangeMode_Label );
   ReadRangeMode_Sizer.Add( ReadRangeMode_ComboBox, 100 );

   const char* readRescaleModeToolTip =
   "<p>This parameter specifies how to fix input pixel samples that "
   "exceed the default input range in a read FITS image.</p>"
   "<p><b>** Warning **</b> If you select the 'Ignore' option, out-of-range pixel "
   "values will not be fixed. Images with out-of-range pixels will not be "
   "rendered correctly on the screen and may cause problems with some tools."
   "If you select this option, platform stability is not guaranteed. Use this "
   "option <b><u>at your own risk.</u></b></p>";

   ReadRescaleMode_Label.SetText( "Action:" );
   ReadRescaleMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ReadRescaleMode_Label.SetMinWidth( labelWidth1 );
   ReadRescaleMode_Label.SetToolTip( readRescaleModeToolTip );

   ReadRescaleMode_ComboBox.AddItem( "Rescale image on out-of-range values" );
   ReadRescaleMode_ComboBox.AddItem( "Truncate all out-of-range values" );
   ReadRescaleMode_ComboBox.AddItem( "Ignore out-of-range values" );
   ReadRescaleMode_ComboBox.SetToolTip( readRescaleModeToolTip );
   ReadRescaleMode_ComboBox.SetCurrentItem( outOfRange.outOfRangeFixMode );

   ReadRescaleMode_Sizer.SetSpacing( 4 );
   ReadRescaleMode_Sizer.Add( ReadRescaleMode_Label );
   ReadRescaleMode_Sizer.Add( ReadRescaleMode_ComboBox, 100 );

   ReadRangeOptions_Sizer.SetMargin( 6 );
   ReadRangeOptions_Sizer.SetSpacing( 4 );
   ReadRangeOptions_Sizer.Add( ReadRangeMode_Sizer );
   ReadRangeOptions_Sizer.Add( ReadRescaleMode_Sizer );

   ReadRangeOptions_GroupBox.SetTitle( "Floating Point Out Of Range Policy" );
   ReadRangeOptions_GroupBox.SetSizer( ReadRangeOptions_Sizer );
   ReadRangeOptions_GroupBox.AdjustToContents();

   //

   NoCompression_RadioButton.SetText( "None" );
   NoCompression_RadioButton.SetMinWidth( labelWidth );
   NoCompression_RadioButton.SetChecked( tiffOptions.compression == pcl::TIFFCompression::None );

   ZIP_RadioButton.SetText( "ZIP" );
   ZIP_RadioButton.SetMinWidth( labelWidth );
   ZIP_RadioButton.SetChecked( tiffOptions.compression == pcl::TIFFCompression::ZIP );

   LZW_RadioButton.SetText( "LZW" );
   LZW_RadioButton.SetMinWidth( labelWidth );
   LZW_RadioButton.SetChecked( tiffOptions.compression == pcl::TIFFCompression::LZW );

   Compression_Sizer.SetMargin( 6 );
   Compression_Sizer.Add( NoCompression_RadioButton );
   Compression_Sizer.Add( ZIP_RadioButton );
   Compression_Sizer.Add( LZW_RadioButton );

   Compression_GroupBox.SetTitle( "Compression" );
   Compression_GroupBox.SetSizer( Compression_Sizer );
   Compression_GroupBox.AdjustToContents();

   //

   ICCProfile_CheckBox.SetText( "ICC Profile" );
   ICCProfile_CheckBox.SetMinWidth( labelWidth );
   ICCProfile_CheckBox.SetTristateMode();
   ICCProfile_CheckBox.SetToolTip( "Override global core application settings for embedded ICC profiles." );
   ICCProfile_CheckBox.SetState( overrides.overrideICCProfileEmbedding ?
      (overrides.embedICCProfiles ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   EmbeddedData_Sizer.SetMargin( 6 );
   EmbeddedData_Sizer.Add( ICCProfile_CheckBox );

   EmbeddedData_GroupBox.SetTitle( "Override Embedding Settings" );
   EmbeddedData_GroupBox.SetSizer( EmbeddedData_Sizer );
   EmbeddedData_GroupBox.AdjustToContents();

   //

   Planar_CheckBox.SetText( "Planar organization" );
   Planar_CheckBox.AdjustToContents();
   Planar_CheckBox.SetMinSize();
   Planar_CheckBox.SetToolTip( "Write each color channel in a separate data block, or <i>plane</i>." );
   Planar_CheckBox.SetChecked( tiffOptions.planar );

   AssociatedAlpha_CheckBox.SetText( "Associated alpha channel" );
   AssociatedAlpha_CheckBox.AdjustToContents();
   AssociatedAlpha_CheckBox.SetMinSize();
   AssociatedAlpha_CheckBox.SetToolTip( "Select the active alpha channel as the associated image transparency." );
   AssociatedAlpha_CheckBox.SetChecked( tiffOptions.associatedAlpha );

   PremultipliedAlpha_CheckBox.SetText( "Premultiplied alpha channel" );
   PremultipliedAlpha_CheckBox.AdjustToContents();
   PremultipliedAlpha_CheckBox.SetMinSize();
   PremultipliedAlpha_CheckBox.SetToolTip( "Write RGB/K components premultiplied by the active alpha channel." );
   PremultipliedAlpha_CheckBox.SetChecked( tiffOptions.premultipliedAlpha );

   Miscellaneous_Sizer.SetMargin( 6 );
   Miscellaneous_Sizer.Add( Planar_CheckBox );
   Miscellaneous_Sizer.Add( AssociatedAlpha_CheckBox );
   Miscellaneous_Sizer.Add( PremultipliedAlpha_CheckBox );

   Miscellaneous_GroupBox.SetTitle( "Miscellaneous" );
   Miscellaneous_GroupBox.SetSizer( Miscellaneous_Sizer );
   Miscellaneous_GroupBox.AdjustToContents();

   //

   LeftPanel_Sizer.SetSpacing( 6 );
   LeftPanel_Sizer.Add( Compression_GroupBox );
   LeftPanel_Sizer.Add( EmbeddedData_GroupBox );
   LeftPanel_Sizer.Add( Miscellaneous_GroupBox );

   //

   ImageCopyright_TextBox.SetText( tiffOptions.copyright );

   ImageCopyright_Sizer.SetMargin( 6 );
   ImageCopyright_Sizer.Add( ImageCopyright_TextBox );

   ImageCopyright_GroupBox.SetTitle( "Image Copyright" );
   ImageCopyright_GroupBox.SetSizer( ImageCopyright_Sizer );

   SoftwareDescription_TextBox.SetText( tiffOptions.software );

   SoftwareDescription_Sizer.SetMargin( 6 );
   SoftwareDescription_Sizer.Add( SoftwareDescription_TextBox );

   SoftwareDescription_GroupBox.SetTitle( "Software Description" );
   SoftwareDescription_GroupBox.SetSizer( SoftwareDescription_Sizer );

   //

   Reset_PushButton.SetText( "Reset" );
   Reset_PushButton.SetDefault();
   Reset_PushButton.OnClick( (pcl::Button::click_event_handler)&TIFFPreferencesDialog::Button_Click, *this );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&TIFFPreferencesDialog::Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&TIFFPreferencesDialog::Button_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.Add( Reset_PushButton );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   //

   RightPanel_Sizer.SetSpacing( 6 );
   RightPanel_Sizer.Add( ImageCopyright_GroupBox, 75 );
   RightPanel_Sizer.Add( SoftwareDescription_GroupBox, 25 );
   RightPanel_Sizer.AddSpacing( 8 );
   RightPanel_Sizer.Add( Buttons_Sizer );

   //

   BottomSection_Sizer.SetSpacing( 16 );
   BottomSection_Sizer.Add( LeftPanel_Sizer );
   BottomSection_Sizer.Add( RightPanel_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.Add( ReadRange_GroupBox );
   Global_Sizer.Add( ReadRangeOptions_GroupBox );
   Global_Sizer.AddSpacing( 8 );
   Global_Sizer.Add( BottomSection_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "TIFF Format Preferences" );

   OnReturn( (pcl::Dialog::return_event_handler)&TIFFPreferencesDialog::Dialog_Return, *this );
}

// ----------------------------------------------------------------------------

void TIFFPreferencesDialog::Button_Click( Button& sender, bool /*checked*/ )
{
   if ( sender == Reset_PushButton )
   {
      LowerRange_NumericEdit.SetValue( 0 );
      UpperRange_NumericEdit.SetValue( 1 );
      ReadRangeMode_ComboBox.SetCurrentItem( TIFFFormat::OutOfRangePolicy_Default );
      ReadRescaleMode_ComboBox.SetCurrentItem( TIFFFormat::OutOfRangeFix_Default );

      ICCProfile_CheckBox.SetState( CheckState::ThirdState );

      TIFFImageOptions o;

      NoCompression_RadioButton.SetChecked( o.compression == TIFFCompression::None );
      ZIP_RadioButton.SetChecked( o.compression == TIFFCompression::ZIP );
      LZW_RadioButton.SetChecked( o.compression == TIFFCompression::LZW ); // ### PCL 1.1: Consider suppressing

      Planar_CheckBox.SetChecked( o.planar );

      AssociatedAlpha_CheckBox.SetChecked( o.associatedAlpha );
      PremultipliedAlpha_CheckBox.SetChecked( o.premultipliedAlpha );

      ImageCopyright_TextBox.SetText( o.copyright );
      SoftwareDescription_TextBox.SetText( o.software );
   }
   else if ( sender == OK_PushButton )
      Ok();
   else
      Cancel();
}

// ----------------------------------------------------------------------------

void TIFFPreferencesDialog::Dialog_Return( Dialog& sender, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      outOfRange.lowerRange = LowerRange_NumericEdit.Value();
      outOfRange.upperRange = UpperRange_NumericEdit.Value();
      outOfRange.outOfRangePolicy = TIFFFormat::out_of_range_policy( ReadRangeMode_ComboBox.CurrentItem() );
      outOfRange.outOfRangeFixMode = TIFFFormat::out_of_range_fix_mode( ReadRescaleMode_ComboBox.CurrentItem() );

      overrides.overrideICCProfileEmbedding = ICCProfile_CheckBox.State() != CheckState::ThirdState;
      overrides.embedICCProfiles = ICCProfile_CheckBox.IsChecked();

      if ( NoCompression_RadioButton.IsChecked() )
         tiffOptions.compression = TIFFCompression::None;
      else if ( ZIP_RadioButton.IsChecked() )
         tiffOptions.compression = TIFFCompression::ZIP;
      else if ( LZW_RadioButton.IsChecked() ) // ### PCL 1.1: Consider suppressing
         tiffOptions.compression = TIFFCompression::LZW;

      tiffOptions.planar = Planar_CheckBox.IsChecked();

      tiffOptions.associatedAlpha = AssociatedAlpha_CheckBox.IsChecked();
      tiffOptions.premultipliedAlpha = PremultipliedAlpha_CheckBox.IsChecked();

      tiffOptions.copyright = ImageCopyright_TextBox.Text();
      tiffOptions.copyright.Trim();

      tiffOptions.software = SoftwareDescription_TextBox.Text();
      tiffOptions.software.Trim();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF TIFFPreferencesDialog.cpp - Released 2017-07-18T16:14:10Z
