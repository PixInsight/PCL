//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0819
// ----------------------------------------------------------------------------
// Standard XISF File Format Module Version 01.00.09.0125
// ----------------------------------------------------------------------------
// XISFPreferencesDialog.cpp - Released 2017-04-14T23:07:03Z
// ----------------------------------------------------------------------------
// This file is part of the standard XISF PixInsight module.
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

#include "XISFPreferencesDialog.h"

namespace pcl
{

// ----------------------------------------------------------------------------

XISFPreferencesDialog::XISFPreferencesDialog( const XISFFormat::EmbeddingOverrides& e, const XISFOptions& xisfOptions ) :
   XISFOptionsDialogBase( xisfOptions, "Maximum inline block size:" ), overrides( e )
{
   int ui4 = LogicalPixelsToPhysical( 4 );

   StoreFITSKeywords_CheckBox.SetText( "Store FITS header keywords" );
   StoreFITSKeywords_CheckBox.SetToolTip( "<p>Store existing FITS keywords in XISF files.</p>"
      "<p>The XISF format allows serialization of FITS header keywords for compatibility with data stored "
      "as FITS files and tools that depend on FITS metadata. You normally should leave this option enabled.</p>" );
   StoreFITSKeywords_CheckBox.SetChecked( options.storeFITSKeywords );

   StoreFITSKeywords_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   StoreFITSKeywords_Sizer.Add( StoreFITSKeywords_CheckBox );
   StoreFITSKeywords_Sizer.AddStretch();

   IgnoreFITSKeywords_CheckBox.SetText( "Ignore FITS header keywords" );
   IgnoreFITSKeywords_CheckBox.SetToolTip( "<p>Do not load FITS keywords from XISF files.</p>"
      "<p>For normal operation, this option should always be disabled. Beware that enabling this option may cause "
      "problems if some tools in your workflow depend on the presence of specific FITS header keywords in your images.</p>" );
   IgnoreFITSKeywords_CheckBox.SetChecked( options.ignoreFITSKeywords );

   IgnoreFITSKeywords_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   IgnoreFITSKeywords_Sizer.Add( IgnoreFITSKeywords_CheckBox );
   IgnoreFITSKeywords_Sizer.AddStretch();

   ImportFITSKeywords_CheckBox.SetText( "Import FITS header keywords as properties" );
   ImportFITSKeywords_CheckBox.SetToolTip( "<p>Import FITS header keywords from XISF files as view properties.</p>"
      "<p>FITS header keywords are imported as Boolean, IsoString or Float64 properties with the 'FITS:' prefix.</p>" );
   ImportFITSKeywords_CheckBox.SetChecked( options.importFITSKeywords );

   ImportFITSKeywords_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   ImportFITSKeywords_Sizer.Add( ImportFITSKeywords_CheckBox );
   ImportFITSKeywords_Sizer.AddStretch();

   FITSCompatibility_Sizer.SetMargin( 8 );
   FITSCompatibility_Sizer.SetSpacing( 4 );
   FITSCompatibility_Sizer.Add( StoreFITSKeywords_Sizer );
   FITSCompatibility_Sizer.Add( IgnoreFITSKeywords_Sizer );
   FITSCompatibility_Sizer.Add( ImportFITSKeywords_Sizer );

   FITSCompatibility_GroupBox.SetTitle( "Legacy FITS Format Compatibility" );
   FITSCompatibility_GroupBox.SetSizer( FITSCompatibility_Sizer );
   FITSCompatibility_GroupBox.AdjustToContents();

   //

   const char* alignmentSizeToolTip =
      "<p>Size in bytes of the minimum space allocated to store a binary data block in an XISF file. "
      "All file blocks (including the XML file header) are stored using integer multiples of the "
      "block alignment size.</p>"
      "<p>If properly configured, block alignment can improve read performance at the expense of "
      "some wasted disk space. The default value is 4096 bytes. Set a block size of one byte to "
      "disable the XISF block alignment feature.</p>"
      "<p>Block alignment is automatically disabled for compressed files.</p>";

   AlignmentSize_Label.SetText( "Block alignment size:" );
   AlignmentSize_Label.SetToolTip( alignmentSizeToolTip );
   AlignmentSize_Label.SetMinWidth( m_labelWidth );
   AlignmentSize_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   AlignmentSize_SpinBox.SetRange( 1, 0xffff );
   AlignmentSize_SpinBox.SetMinimumValueText( "<disabled>" );
   AlignmentSize_SpinBox.SetToolTip( alignmentSizeToolTip );
   AlignmentSize_SpinBox.SetValue( Max( uint16( 1 ), options.blockAlignmentSize ) );

   AlignmentSize_Sizer.SetSpacing( 4 );
   AlignmentSize_Sizer.Add( AlignmentSize_Label );
   AlignmentSize_Sizer.Add( AlignmentSize_SpinBox );
   AlignmentSize_Sizer.AddStretch();

   const char* maxInlineSizeToolTip =
      "<p>Maximum size in bytes of a data block stored as inline XML contents. Inline data blocks are "
      "encoded as base64.</p>"
      "<p>The default value is 3072 bytes. This is coherent with the default block alignment size of "
      "4096 bytes, since base64 encoding requires 4/3 of the original data size (4096 = 3072*4/3).</p>";

   MaxInlineSize_Label.SetText( "Maximum inline block size:" );
   MaxInlineSize_Label.SetToolTip( maxInlineSizeToolTip );
   MaxInlineSize_Label.SetMinWidth( m_labelWidth );
   MaxInlineSize_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   MaxInlineSize_SpinBox.SetRange( 0, 0xffff );
   MaxInlineSize_SpinBox.SetMinimumValueText( "<disabled>" );
   MaxInlineSize_SpinBox.SetToolTip( maxInlineSizeToolTip );
   MaxInlineSize_SpinBox.SetValue( options.maxInlineBlockSize );

   MaxInlineSize_Sizer.SetSpacing( 4 );
   MaxInlineSize_Sizer.Add( MaxInlineSize_Label );
   MaxInlineSize_Sizer.Add( MaxInlineSize_SpinBox );
   MaxInlineSize_Sizer.AddStretch();

   DataAlignment_Sizer.SetMargin( 8 );
   DataAlignment_Sizer.SetSpacing( 4 );
   DataAlignment_Sizer.Add( AlignmentSize_Sizer );
   DataAlignment_Sizer.Add( MaxInlineSize_Sizer );

   DataAlignment_GroupBox.SetTitle( "XISF Data Block Alignment" );
   DataAlignment_GroupBox.SetSizer( DataAlignment_Sizer );
   DataAlignment_GroupBox.AdjustToContents();

   //

   Properties_CheckBox.SetText( "Image properties" );
   Properties_CheckBox.SetTristateMode();
   Properties_CheckBox.SetToolTip( "<p>Override global core application settings for embedded image properties.</p>" );
   Properties_CheckBox.SetState( overrides.overridePropertyEmbedding ?
      (overrides.embedProperties ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   Properties_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   Properties_Sizer.Add( Properties_CheckBox );
   Properties_Sizer.AddStretch();

   ICCProfile_CheckBox.SetText( "ICC profile" );
   ICCProfile_CheckBox.SetTristateMode();
   ICCProfile_CheckBox.SetToolTip( "<p>Override global core application settings for embedded ICC profiles.</p>" );
   ICCProfile_CheckBox.SetState( overrides.overrideICCProfileEmbedding ?
      (overrides.embedICCProfiles ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   ICCProfile_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   ICCProfile_Sizer.Add( ICCProfile_CheckBox );
   ICCProfile_Sizer.AddStretch();

   DisplayFunction_CheckBox.SetText( "Display function" );
   DisplayFunction_CheckBox.SetTristateMode();
   DisplayFunction_CheckBox.SetToolTip( "<p>Override global core application settings for embedded display functions.</p>" );
   DisplayFunction_CheckBox.SetState( overrides.overrideDisplayFunctionEmbedding ?
      (overrides.embedDisplayFunctions ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   DisplayFunction_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   DisplayFunction_Sizer.Add( DisplayFunction_CheckBox );
   DisplayFunction_Sizer.AddStretch();

   RGBWorkingSpace_CheckBox.SetText( "RGB working space" );
   RGBWorkingSpace_CheckBox.SetTristateMode();
   RGBWorkingSpace_CheckBox.SetToolTip( "<p>Override global core application settings for embedded RGB working spaces.</p>" );
   RGBWorkingSpace_CheckBox.SetState( overrides.overrideRGBWorkingSpaceEmbedding ?
      (overrides.embedRGBWorkingSpaces ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   RGBWorkingSpace_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   RGBWorkingSpace_Sizer.Add( RGBWorkingSpace_CheckBox );
   RGBWorkingSpace_Sizer.AddStretch();

   Thumbnail_CheckBox.SetText( "Thumbnail image" );
   Thumbnail_CheckBox.SetTristateMode();
   Thumbnail_CheckBox.SetToolTip( "<p>Override global core application settings for embedded thumbnails.</p>" );
   Thumbnail_CheckBox.SetState( overrides.overrideThumbnailEmbedding ?
      (overrides.embedThumbnails ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   Thumbnail_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   Thumbnail_Sizer.Add( Thumbnail_CheckBox );
   Thumbnail_Sizer.AddStretch();

   PreviewRects_CheckBox.SetText( "Preview rectangles" );
   PreviewRects_CheckBox.SetTristateMode();
   PreviewRects_CheckBox.SetToolTip( "<p>Override global core application settings for embedded preview rectangles.</p>" );
   PreviewRects_CheckBox.SetState( overrides.overridePreviewRectsEmbedding ?
      (overrides.embedPreviewRects ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   PreviewRects_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   PreviewRects_Sizer.Add( PreviewRects_CheckBox );
   PreviewRects_Sizer.AddStretch();

   EmbeddedData_Sizer.SetMargin( 8 );
   EmbeddedData_Sizer.SetSpacing( 4 );
   EmbeddedData_Sizer.Add( Properties_Sizer );
   EmbeddedData_Sizer.Add( ICCProfile_Sizer );
   EmbeddedData_Sizer.Add( DisplayFunction_Sizer );
   EmbeddedData_Sizer.Add( RGBWorkingSpace_Sizer );
   EmbeddedData_Sizer.Add( Thumbnail_Sizer );
   EmbeddedData_Sizer.Add( PreviewRects_Sizer );

   EmbeddedData_GroupBox.SetTitle( "Override Embedding Settings" );
   EmbeddedData_GroupBox.SetSizer( EmbeddedData_Sizer );
   EmbeddedData_GroupBox.AdjustToContents();

   //

   Reset_PushButton.SetText( "Reset" );
   Reset_PushButton.SetDefault();
   Reset_PushButton.OnClick( (pcl::Button::click_event_handler)&XISFPreferencesDialog::Button_Click, *this );

   BottomSection_Sizer.Insert( 0, Reset_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.Add( FITSCompatibility_GroupBox );
   Global_Sizer.Add( DataCompression_GroupBox );
   Global_Sizer.Add( DataAlignment_GroupBox );
   Global_Sizer.Add( Security_GroupBox );
   Global_Sizer.Add( EmbeddedData_GroupBox );
   Global_Sizer.AddSpacing( 8 );
   Global_Sizer.Add( BottomSection_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "XISF Format Preferences" );

   OnReturn( (Dialog::return_event_handler)&XISFPreferencesDialog::Dialog_Return, *this );
}

// ----------------------------------------------------------------------------

void XISFPreferencesDialog::Button_Click( Button& sender, bool checked )
{
   if ( sender == Reset_PushButton )
   {
      ICCProfile_CheckBox.SetState( CheckState::ThirdState );
      Properties_CheckBox.SetState( CheckState::ThirdState );
      Thumbnail_CheckBox.SetState( CheckState::ThirdState );

      XISFOptions defaultOptions;

      StoreFITSKeywords_CheckBox.SetChecked( defaultOptions.storeFITSKeywords );
      IgnoreFITSKeywords_CheckBox.SetChecked( defaultOptions.ignoreFITSKeywords );
      ImportFITSKeywords_CheckBox.SetChecked( defaultOptions.importFITSKeywords );

      CompressionCodec_ComboBox.SetCurrentItem( CompressionCodecToComboBoxItem( defaultOptions.compressionCodec ) );
      CompressionLevel_SpinBox.SetValue( defaultOptions.compressionLevel );
      CompressionShuffle_CheckBox.SetChecked( options.compressionCodec == XISFCompression::None ||
                                              XISF::CompressionUsesByteShuffle( options.compressionCodec ) );
      DataCompression_GroupBox.SetChecked( options.compressionCodec != XISFCompression::None );

      Checksums_ComboBox.SetCurrentItem( ChecksumAlgorithmToComboBoxItem( defaultOptions.checksumAlgorithm ) );
      Security_GroupBox.SetChecked( options.checksumAlgorithm != XISFChecksum::None );

      AlignmentSize_SpinBox.SetValue( defaultOptions.blockAlignmentSize );
      MaxInlineSize_SpinBox.SetValue( defaultOptions.maxInlineBlockSize );
   }
   else
      Base_Button_Click( sender, checked );
}

// ----------------------------------------------------------------------------

void XISFPreferencesDialog::Dialog_Return( Dialog& sender, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      overrides.overridePropertyEmbedding = Properties_CheckBox.State() != CheckState::ThirdState;
      overrides.embedProperties = Properties_CheckBox.IsChecked();

      overrides.overrideICCProfileEmbedding = ICCProfile_CheckBox.State() != CheckState::ThirdState;
      overrides.embedICCProfiles = ICCProfile_CheckBox.IsChecked();

      overrides.overrideDisplayFunctionEmbedding = DisplayFunction_CheckBox.State() != CheckState::ThirdState;
      overrides.embedDisplayFunctions = DisplayFunction_CheckBox.IsChecked();

      overrides.overrideRGBWorkingSpaceEmbedding = RGBWorkingSpace_CheckBox.State() != CheckState::ThirdState;
      overrides.embedRGBWorkingSpaces = RGBWorkingSpace_CheckBox.IsChecked();

      overrides.overrideThumbnailEmbedding = Thumbnail_CheckBox.State() != CheckState::ThirdState;
      overrides.embedThumbnails = Thumbnail_CheckBox.IsChecked();

      overrides.overridePreviewRectsEmbedding = PreviewRects_CheckBox.State() != CheckState::ThirdState;
      overrides.embedPreviewRects = PreviewRects_CheckBox.IsChecked();

      options.storeFITSKeywords = StoreFITSKeywords_CheckBox.IsChecked();
      options.ignoreFITSKeywords = IgnoreFITSKeywords_CheckBox.IsChecked();
      options.importFITSKeywords = ImportFITSKeywords_CheckBox.IsChecked();

      GetBaseParameters();

      options.blockAlignmentSize = AlignmentSize_SpinBox.Value();
      options.maxInlineBlockSize = MaxInlineSize_SpinBox.Value();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF XISFPreferencesDialog.cpp - Released 2017-04-14T23:07:03Z
