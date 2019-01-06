//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 01.01.05.0427
// ----------------------------------------------------------------------------
// FITSPreferencesDialog.cpp - Released 2018-12-12T09:25:15Z
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
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

#include "FITSPreferencesDialog.h"

#include <float.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FITSPreferencesDialog::FITSPreferencesDialog( const FITSFormat::OutOfRangePolicyOptions& r,
                                              const FITSFormat::EmbeddingOverrides& o, const FITSImageOptions& f ) :
   Dialog(),
   outOfRange( r ),
   overrides( o ),
   fitsOptions( f )
{
   pcl::Font fnt = Font();
   int labelWidth = fnt.Width( String( "Default integer signedness:" ) + 'M' );
   int editWidth = fnt.Width( String( '0', 30 ) );
   int ui4 = LogicalPixelsToPhysical( 4 );

   //

   LowerRange_NumericEdit.label.SetText( "Lower Range:" );
   LowerRange_NumericEdit.label.SetMinWidth( labelWidth );
   LowerRange_NumericEdit.SetReal( true );
   LowerRange_NumericEdit.SetRange( -DBL_MAX, +DBL_MAX );
   LowerRange_NumericEdit.SetPrecision( 15 );
   LowerRange_NumericEdit.EnableScientificNotation();
   LowerRange_NumericEdit.SetScientificNotationTriggerExponent( 5 );
   LowerRange_NumericEdit.edit.SetFixedWidth( editWidth );
   LowerRange_NumericEdit.sizer.AddStretch();
   LowerRange_NumericEdit.SetToolTip( "<p>Lower range of read FITS floating point pixel samples</p>" );
   LowerRange_NumericEdit.SetValue( outOfRange.lowerRange );

   UpperRange_NumericEdit.label.SetText( "Upper Range:" );
   UpperRange_NumericEdit.label.SetMinWidth( labelWidth );
   UpperRange_NumericEdit.SetReal( true );
   UpperRange_NumericEdit.SetRange( -DBL_MAX, +DBL_MAX );
   UpperRange_NumericEdit.SetPrecision( 15 );
   UpperRange_NumericEdit.EnableScientificNotation();
   UpperRange_NumericEdit.SetScientificNotationTriggerExponent( 5 );
   UpperRange_NumericEdit.edit.SetFixedWidth( editWidth );
   UpperRange_NumericEdit.sizer.AddStretch();
   UpperRange_NumericEdit.SetToolTip( "<p>Upper range of read FITS floating point pixel samples</p>" );
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
      "FITS image exceed the default input range.</p>"
      "<p><b>** Important **</b> If the 'Ignore' option is selected for 'Action' "
      "(see below), this option will be ignored (not applied). Please read the tooltip "
      "information provided for the 'Action' combo box below.</p>";

   ReadRangeMode_Label.SetText( "How to proceed:" );
   ReadRangeMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ReadRangeMode_Label.SetMinWidth( labelWidth );
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
   ReadRescaleMode_Label.SetMinWidth( labelWidth );
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

   const char* coordinateOriginToolTip =
      "<p>This parameter specifies the growing direction of vertical "
      "coordinates when reading and writing FITS images. Data acquired with most "
      "professional instruments (HST, Gemini, Subaru, etc.) and high-end CCD camera "
      "control software packages normally follow the lower left origin convention.</p>"
      "<p>The upper left origin is coherent with the rest of image file formats, and "
      "is used by some amateur CCD camera control and image processing software packages.</p>";

   CoordinateOrigin_Label.SetText( "Coordinate origin:" );
   CoordinateOrigin_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CoordinateOrigin_Label.SetMinWidth( labelWidth );
   CoordinateOrigin_Label.SetToolTip( coordinateOriginToolTip );

   CoordinateOrigin_ComboBox.AddItem( "Upper left corner (up-bottom)" );
   CoordinateOrigin_ComboBox.AddItem( "Lower left corner (bottom-up)" );
   CoordinateOrigin_ComboBox.SetToolTip( coordinateOriginToolTip );
   CoordinateOrigin_ComboBox.SetCurrentItem( fitsOptions.bottomUp ? 1 : 0 );

   CoordinateOrigin_Sizer.SetSpacing( 4 );
   CoordinateOrigin_Sizer.Add( CoordinateOrigin_Label );
   CoordinateOrigin_Sizer.Add( CoordinateOrigin_ComboBox, 100 );

   const char* integerSignednessToolTip =
      "<p>The FITS standard does not support unsigned integers as one of its "
      "<i>native</i> data types. However, a special FITS header keyword, namely BZERO, "
      "can be used to store integer values with arbitrary zero offsets.</p>"
      "<p>If the corresponding option is selected, newly created integer FITS images will "
      "store unsigned integers by default (unless otherwise specified) through the BZERO "
      "convention: For 16-bit unsigned integers, the value BZERO=32768 (2<sup>15</sup>) will "
      "be used; for 32-bit unsigned integers, BZERO=2147483648 (2<sup>31</sup>). In both "
      "cases the keyword BSCALE=1 will also be included in the header of the corresponding "
      "FITS HDU (header/data unit).</p>";

   IntegerSignedness_Label.SetText( "Default integer signedness:" );
   IntegerSignedness_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   IntegerSignedness_Label.SetMinWidth( labelWidth );
   IntegerSignedness_Label.SetToolTip( integerSignednessToolTip );

   IntegerSignedness_ComboBox.AddItem( "Write signed integers (FITS native)" );
   IntegerSignedness_ComboBox.AddItem( "Write unsigned integers (BZERO/BSCALE)" );
   IntegerSignedness_ComboBox.SetToolTip( integerSignednessToolTip );

   IntegerSignedness_ComboBox.SetCurrentItem( fitsOptions.unsignedIntegers ? 1 : 0 );

   IntegerSignedness_Sizer.SetSpacing( 4 );
   IntegerSignedness_Sizer.Add( IntegerSignedness_Label );
   IntegerSignedness_Sizer.Add( IntegerSignedness_ComboBox, 100 );

   WriteScalingKeywordsForSignedData_CheckBox.SetText( "Write scaling keywords to signed integer images" );
   WriteScalingKeywordsForSignedData_CheckBox.SetToolTip(
      "<p>When this option is selected, the BZERO=0 and BSCALE=1 keywords will also be "
      "written to the headers of <i>signed</i> integer FITS images. Although these "
      "keywords are not necessary to define the default range and scaling ratio of signed "
      "integer data, they can improve compatibility with other imaging applications that "
      "seem to need them. This option is enabled by default.</p>" );
   WriteScalingKeywordsForSignedData_CheckBox.SetChecked( fitsOptions.writeScalingKeywordsForSignedData );

   WriteScalingKeywordsForSignedData_Sizer.AddUnscaledSpacing( labelWidth + ui4 );
   WriteScalingKeywordsForSignedData_Sizer.Add( WriteScalingKeywordsForSignedData_CheckBox );
   WriteScalingKeywordsForSignedData_Sizer.AddStretch();

   SignedIntegersArePhysical_CheckBox.SetText( "Signed integer images store physical pixel data" );
   SignedIntegersArePhysical_CheckBox.SetToolTip(
      "<p>Some applications write raw pixel data as signed integer FITS images. In these "
      "images, the negative half of the available numeric range is not used, and hence the "
      "data are stored with one bit less than the nominal bit depth. Enable this option only "
      "if you need compatibility with one of those applications.</p>"
      "<p>Note that if you write signed integer FITS images with PixInsight (which is "
      "discouraged for the sake of compatibility), the whole available numeric range will "
      "always be used. This means that if this option is enabled, signed FITS files written by "
      "PixInsight will not be loaded correctly. This option is disabled by default.</p>" );
   SignedIntegersArePhysical_CheckBox.SetChecked( fitsOptions.signedIntegersArePhysical );

   SignedIntegersArePhysical_Sizer.AddUnscaledSpacing( labelWidth + ui4 );
   SignedIntegersArePhysical_Sizer.Add( SignedIntegersArePhysical_CheckBox );
   SignedIntegersArePhysical_Sizer.AddStretch();

   CleanupHeaders_CheckBox.SetText( "Cleanup header keywords" );
   CleanupHeaders_CheckBox.SetToolTip(
      "<p>The FITS standard allows only printable ASCII characters in header keywords (ISO/IEC 8859-1 code "
      "points from 0x20 to 0x7E). Some applications write illegal characters to FITS keyword comments and "
      "values. For example, the <i>degree sign</i>, whose code is 0xB0 (decimal 176), is written by some "
      "applications to represent temperature values and angles in FITS keyword comments. These nonstandard FITS "
      "files can be loaded in PixInsight, but we'll never allow writing them unless the illegal characters are "
      "either removed or replaced with legal ASCII values.</p>"
      "<p>If this option is enabled, any illegal characters in the header keywords of a FITS file will be "
      "replaced by their best legal counterparts before writing the file to disk. For example, an illegal "
      "character 0xC0 (Latin capital letter A with grave, &Agrave;) will be replaced by 0x41, or a Latin capital "
      "letter A. Similarly, an illegal copyright sign (0xA9, &copy;) will be replaced by the standard "
      "three-character sequence \'(C)\'. This option is disabled by default.</p>" );
   CleanupHeaders_CheckBox.SetChecked( fitsOptions.cleanupHeaders );

   CleanupHeaders_Sizer.AddUnscaledSpacing( labelWidth + ui4 );
   CleanupHeaders_Sizer.Add( CleanupHeaders_CheckBox );
   CleanupHeaders_Sizer.AddStretch();

   MiscellaneousOptions_Sizer.SetMargin( 6 );
   MiscellaneousOptions_Sizer.SetSpacing( 4 );
   MiscellaneousOptions_Sizer.Add( CoordinateOrigin_Sizer );
   MiscellaneousOptions_Sizer.Add( IntegerSignedness_Sizer );
   MiscellaneousOptions_Sizer.Add( WriteScalingKeywordsForSignedData_Sizer );
   MiscellaneousOptions_Sizer.Add( SignedIntegersArePhysical_Sizer );
   MiscellaneousOptions_Sizer.Add( CleanupHeaders_Sizer );

   MiscellaneousOptions_GroupBox.SetTitle( "Miscellaneous Options" );
   MiscellaneousOptions_GroupBox.SetSizer( MiscellaneousOptions_Sizer );
   MiscellaneousOptions_GroupBox.AdjustToContents();

   //

   ICCProfile_CheckBox.SetText( "ICC Profile" );
   ICCProfile_CheckBox.SetTristateMode();
   ICCProfile_CheckBox.SetToolTip( "Override global core application settings for embedded ICC profiles." );
   ICCProfile_CheckBox.SetState( overrides.overrideICCProfileEmbedding ?
      (overrides.embedICCProfiles ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   Properties_CheckBox.SetText( "Properties" );
   Properties_CheckBox.SetTristateMode();
   Properties_CheckBox.SetToolTip( "Override global core application settings for embedded image properties." );
   Properties_CheckBox.SetState( overrides.overridePropertyEmbedding ?
      (overrides.embedProperties ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   EmbeddedDataLeft_Sizer.SetSpacing( 4 );
   EmbeddedDataLeft_Sizer.Add( ICCProfile_CheckBox );
   EmbeddedDataLeft_Sizer.Add( Properties_CheckBox );
   //EmbeddedDataLeft_Sizer.AddStretch();

   Thumbnail_CheckBox.SetText( "Thumbnail Image" );
   Thumbnail_CheckBox.SetTristateMode();
   Thumbnail_CheckBox.SetToolTip( "Override global core application settings for embedded thumbnails." );
   Thumbnail_CheckBox.SetState( overrides.overrideThumbnailEmbedding ?
      (overrides.embedThumbnails ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   FixedPrecision_CheckBox.SetText( "Fixed-precision keywords" );
   FixedPrecision_CheckBox.SetToolTip( "Write fixed-precision floating-point FITS keywords" );
   FixedPrecision_CheckBox.SetChecked( fitsOptions.writeFixedFloatKeywords );

   EmbeddedDataRight_Sizer.SetSpacing( 4 );
   EmbeddedDataRight_Sizer.Add( Thumbnail_CheckBox );
   EmbeddedDataRight_Sizer.Add( FixedPrecision_CheckBox );
   //EmbeddedDataRight_Sizer.AddStretch();

   EmbeddedData_Sizer.SetMargin( 6 );
   EmbeddedData_Sizer.SetSpacing( 12 );
   EmbeddedData_Sizer.Add( EmbeddedDataLeft_Sizer );
   EmbeddedData_Sizer.Add( EmbeddedDataRight_Sizer );

   EmbeddedData_GroupBox.SetTitle( "Override Embedding Settings" );
   EmbeddedData_GroupBox.SetSizer( EmbeddedData_Sizer );
   EmbeddedData_GroupBox.AdjustToContents();

   //

   Reset_PushButton.SetText( "Reset" );
   Reset_PushButton.SetDefault();
   Reset_PushButton.OnClick( (pcl::Button::click_event_handler)&FITSPreferencesDialog::Button_Click, *this );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&FITSPreferencesDialog::Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&FITSPreferencesDialog::Button_Click, *this );

   BottomSection_Sizer.SetSpacing( 8 );
   BottomSection_Sizer.Add( Reset_PushButton );
   BottomSection_Sizer.AddStretch();
   BottomSection_Sizer.Add( OK_PushButton );
   BottomSection_Sizer.Add( Cancel_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.Add( ReadRange_GroupBox );
   Global_Sizer.Add( ReadRangeOptions_GroupBox );
   Global_Sizer.Add( MiscellaneousOptions_GroupBox );
   Global_Sizer.Add( EmbeddedData_GroupBox );
   Global_Sizer.SetSpacing( 12 );
   Global_Sizer.Add( BottomSection_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "FITS Format Preferences" );

   OnReturn( (Dialog::return_event_handler)&FITSPreferencesDialog::Dialog_Return, *this );
}

// ----------------------------------------------------------------------------

void FITSPreferencesDialog::Button_Click( Button& sender, bool checked )
{
   if ( sender == Reset_PushButton )
   {
      LowerRange_NumericEdit.SetValue( 0 );
      UpperRange_NumericEdit.SetValue( 1 );
      ReadRangeMode_ComboBox.SetCurrentItem( FITSFormat::OutOfRangePolicy_Default );
      ReadRescaleMode_ComboBox.SetCurrentItem( FITSFormat::OutOfRangeFix_Default );

      ICCProfile_CheckBox.SetState( CheckState::ThirdState );
      Thumbnail_CheckBox.SetState( CheckState::ThirdState );

      FITSImageOptions o;
      CoordinateOrigin_ComboBox.SetCurrentItem( o.bottomUp ? 1 : 0 ); // bottom->up
      IntegerSignedness_ComboBox.SetCurrentItem( o.unsignedIntegers ? 1 : 0 ); // unsigned integers
      FixedPrecision_CheckBox.SetChecked( o.writeFixedFloatKeywords );
      WriteScalingKeywordsForSignedData_CheckBox.SetChecked( o.writeScalingKeywordsForSignedData );
      SignedIntegersArePhysical_CheckBox.SetChecked( fitsOptions.signedIntegersArePhysical );
   }
   else if ( sender == OK_PushButton )
      Ok();
   else if ( sender == Cancel_PushButton )
      Cancel();
}

// ----------------------------------------------------------------------------

void FITSPreferencesDialog::Dialog_Return( Dialog& sender, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      outOfRange.lowerRange = LowerRange_NumericEdit.Value();
      outOfRange.upperRange = UpperRange_NumericEdit.Value();
      outOfRange.outOfRangePolicy = FITSFormat::out_of_range_policy( ReadRangeMode_ComboBox.CurrentItem() );
      outOfRange.outOfRangeFixMode = FITSFormat::out_of_range_fix_mode( ReadRescaleMode_ComboBox.CurrentItem() );

      overrides.overrideICCProfileEmbedding = ICCProfile_CheckBox.State() != CheckState::ThirdState;
      overrides.embedICCProfiles = ICCProfile_CheckBox.IsChecked();

      overrides.overrideThumbnailEmbedding = Thumbnail_CheckBox.State() != CheckState::ThirdState;
      overrides.embedThumbnails = Thumbnail_CheckBox.IsChecked();

      overrides.overridePropertyEmbedding = Properties_CheckBox.State() != CheckState::ThirdState;
      overrides.embedProperties = Properties_CheckBox.IsChecked();

      fitsOptions.bottomUp = CoordinateOrigin_ComboBox.CurrentItem() == 1;
      fitsOptions.unsignedIntegers = IntegerSignedness_ComboBox.CurrentItem() == 1;
      fitsOptions.writeFixedFloatKeywords = FixedPrecision_CheckBox.IsChecked();
      fitsOptions.writeScalingKeywordsForSignedData = WriteScalingKeywordsForSignedData_CheckBox.IsChecked();
      fitsOptions.signedIntegersArePhysical = SignedIntegersArePhysical_CheckBox.IsChecked();
      fitsOptions.cleanupHeaders = CleanupHeaders_CheckBox.IsChecked();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FITSPreferencesDialog.cpp - Released 2018-12-12T09:25:15Z
