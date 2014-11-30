// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard XISF File Format Module Version 01.00.00.0023
// ****************************************************************************
// XISFPreferencesDialog.cpp - Released 2014/11/30 10:38:10 UTC
// ****************************************************************************
// This file is part of the standard XISF PixInsight module.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
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
// ****************************************************************************

#include "XISFPreferencesDialog.h"

#include <zlib/zlib.h> // for Z_BEST_SPEED/Z_BEST_COMPRESSION

namespace pcl
{

// ----------------------------------------------------------------------------

XISFPreferencesDialog::XISFPreferencesDialog( const XISFFormat::EmbeddingOverrides& e, const XISFOptions& o ) :
Dialog(), overrides( e ), options( o )
{
   pcl::Font fnt = Font();
   int labelWidth = fnt.Width( String( "Maximum inline block size:" ) + "M" );

   //

   StoreFITSKeywords_CheckBox.SetText( "Store FITS header keywords" );
   StoreFITSKeywords_CheckBox.SetToolTip( "<p>Store existing FITS keywords in XISF files.</p>"
      "<p>The XISF format allows serialization of FITS header keywords for compatibility with data stored "
      "as FITS files and tools that depend on FITS metadata. You normally should leave this option enabled.</p>" );
   StoreFITSKeywords_CheckBox.SetChecked( options.storeFITSKeywords );

   StoreFITSKeywords_Sizer.AddSpacing( labelWidth + 4 );
   StoreFITSKeywords_Sizer.Add( StoreFITSKeywords_CheckBox );
   StoreFITSKeywords_Sizer.AddStretch();

   IgnoreFITSKeywords_CheckBox.SetText( "Ignore FITS header keywords" );
   IgnoreFITSKeywords_CheckBox.SetToolTip( "<p>Do not load FITS keywords from XISF files.</p>"
      "<p>For normal operation, this option should always be disabled. Beware that enabling this option may cause "
      "problems if some tools in your workflow depend on the presence of specific FITS header keywords in your images.</p>" );
   IgnoreFITSKeywords_CheckBox.SetChecked( options.ignoreFITSKeywords );

   IgnoreFITSKeywords_Sizer.AddSpacing( labelWidth + 4 );
   IgnoreFITSKeywords_Sizer.Add( IgnoreFITSKeywords_CheckBox );
   IgnoreFITSKeywords_Sizer.AddStretch();

   ImportFITSKeywords_CheckBox.SetText( "Import FITS header keywords as properties" );
   ImportFITSKeywords_CheckBox.SetToolTip( "<p>Import FITS header keywords from XISF files as view properties.</p>"
      "<p>FITS header keywords are imported as Boolean, IsoString or Float64 properties with the 'FITS:' prefix.</p>" );
   ImportFITSKeywords_CheckBox.SetChecked( options.importFITSKeywords );

   ImportFITSKeywords_Sizer.AddSpacing( labelWidth + 4 );
   ImportFITSKeywords_Sizer.Add( ImportFITSKeywords_CheckBox );
   ImportFITSKeywords_Sizer.AddStretch();

   FITSCompatibility_Sizer.SetMargin( 8 );
   FITSCompatibility_Sizer.SetSpacing( 4 );
   FITSCompatibility_Sizer.Add( StoreFITSKeywords_Sizer );
   FITSCompatibility_Sizer.Add( IgnoreFITSKeywords_Sizer );
   FITSCompatibility_Sizer.Add( ImportFITSKeywords_Sizer );

   FITSCompatibility_GroupBox.SetTitle( "FITS Compatibility" );
   FITSCompatibility_GroupBox.SetSizer( FITSCompatibility_Sizer );
   FITSCompatibility_GroupBox.AdjustToContents();

   //

   Compression_CheckBox.SetText( "Compressed data" );
   Compression_CheckBox.SetToolTip( "<p>Compress data blocks with the zlib/deflate algorithm.</p>" );
   Compression_CheckBox.SetChecked( options.compressData );
   Compression_CheckBox.OnClick( (pcl::Button::click_event_handler)&XISFPreferencesDialog::__Button_Click, *this );

   Compression_Sizer.AddSpacing( labelWidth + 4 );
   Compression_Sizer.Add( Compression_CheckBox );
   Compression_Sizer.AddStretch();

   const char* compressionLevelToolTip =
      "<p>Zlib compression level between 1 and 9. A value of one gives the fastest compression, "
      "while 9 gives the best compression ratio at the expense of more computation time. The default "
      "compression level is 6, which is a compromise between speed and compression efficiency.</p>";

   CompressionLevel_Label.SetText( "Compression level:" );
   CompressionLevel_Label.SetToolTip( compressionLevelToolTip );
   CompressionLevel_Label.SetMinWidth( labelWidth );
   CompressionLevel_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   CompressionLevel_Label.Enable( options.compressData );

   CompressionLevel_SpinBox.SetRange( int( Z_BEST_SPEED ), int( Z_BEST_COMPRESSION ) );
   CompressionLevel_SpinBox.SetToolTip( compressionLevelToolTip );
   CompressionLevel_SpinBox.SetValue( Range( options.compressionLevel, uint8( Z_BEST_SPEED ), uint8( Z_BEST_COMPRESSION ) ) );
   CompressionLevel_SpinBox.Enable( options.compressData );

   CompressionLevel_Sizer.SetSpacing( 4 );
   CompressionLevel_Sizer.Add( CompressionLevel_Label );
   CompressionLevel_Sizer.Add( CompressionLevel_SpinBox );
   CompressionLevel_Sizer.AddStretch();

   DataCompression_Sizer.SetMargin( 8 );
   DataCompression_Sizer.SetSpacing( 4 );
   DataCompression_Sizer.Add( Compression_Sizer );
   DataCompression_Sizer.Add( CompressionLevel_Sizer );

   DataCompression_GroupBox.SetTitle( "Data Compression" );
   DataCompression_GroupBox.SetSizer( DataCompression_Sizer );
   DataCompression_GroupBox.AdjustToContents();

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
   AlignmentSize_Label.SetMinWidth( labelWidth );
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
   MaxInlineSize_Label.SetMinWidth( labelWidth );
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

   DataAlignment_GroupBox.SetTitle( "Data Alignment" );
   DataAlignment_GroupBox.SetSizer( DataAlignment_Sizer );
   DataAlignment_GroupBox.AdjustToContents();

   //

   Properties_CheckBox.SetText( "Properties" );
   Properties_CheckBox.SetTristateMode();
   Properties_CheckBox.SetToolTip( "<p>Override global core application settings for embedded image properties.</p>" );
   Properties_CheckBox.SetState( overrides.overridePropertyEmbedding ?
      (overrides.embedProperties ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   Properties_Sizer.AddSpacing( labelWidth + 4 );
   Properties_Sizer.Add( Properties_CheckBox );
   Properties_Sizer.AddStretch();

   ICCProfile_CheckBox.SetText( "ICC Profile" );
   ICCProfile_CheckBox.SetTristateMode();
   ICCProfile_CheckBox.SetToolTip( "<p>Override global core application settings for embedded ICC profiles.</p>" );
   ICCProfile_CheckBox.SetState( overrides.overrideICCProfileEmbedding ?
      (overrides.embedICCProfiles ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   ICCProfile_Sizer.AddSpacing( labelWidth + 4 );
   ICCProfile_Sizer.Add( ICCProfile_CheckBox );
   ICCProfile_Sizer.AddStretch();

   Metadata_CheckBox.SetText( "Metadata" );
   Metadata_CheckBox.SetTristateMode();
   Metadata_CheckBox.SetToolTip( "<p>Override global core application settings for embedded XML metadata.</p>" );
   Metadata_CheckBox.SetState( overrides.overrideMetadataEmbedding ?
      (overrides.embedMetadata ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   Metadata_Sizer.AddSpacing( labelWidth + 4 );
   Metadata_Sizer.Add( Metadata_CheckBox );
   Metadata_Sizer.AddStretch();

   Thumbnail_CheckBox.SetText( "Thumbnail Image" );
   Thumbnail_CheckBox.SetTristateMode();
   Thumbnail_CheckBox.SetToolTip( "<p>Override global core application settings for embedded thumbnails.</p>" );
   Thumbnail_CheckBox.SetState( overrides.overrideThumbnailEmbedding ?
      (overrides.embedThumbnails ? CheckState::Checked : CheckState::Unchecked) : CheckState::ThirdState );

   Thumbnail_Sizer.AddSpacing( labelWidth + 4 );
   Thumbnail_Sizer.Add( Thumbnail_CheckBox );
   Thumbnail_Sizer.AddStretch();

   EmbeddedData_Sizer.SetMargin( 8 );
   EmbeddedData_Sizer.SetSpacing( 4 );
   EmbeddedData_Sizer.Add( Properties_Sizer );
   EmbeddedData_Sizer.Add( ICCProfile_Sizer );
   EmbeddedData_Sizer.Add( Metadata_Sizer );
   EmbeddedData_Sizer.Add( Thumbnail_Sizer );

   EmbeddedData_GroupBox.SetTitle( "Override Embedding Settings" );
   EmbeddedData_GroupBox.SetSizer( EmbeddedData_Sizer );
   EmbeddedData_GroupBox.AdjustToContents();

   //

   Reset_PushButton.SetText( "Reset" );
   Reset_PushButton.SetDefault();
   Reset_PushButton.OnClick( (pcl::Button::click_event_handler)&XISFPreferencesDialog::__Button_Click, *this );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&XISFPreferencesDialog::__Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&XISFPreferencesDialog::__Button_Click, *this );

   BottomSection_Sizer.SetSpacing( 8 );
   BottomSection_Sizer.Add( Reset_PushButton );
   BottomSection_Sizer.AddStretch();
   BottomSection_Sizer.Add( OK_PushButton );
   BottomSection_Sizer.Add( Cancel_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 12 );
   Global_Sizer.Add( FITSCompatibility_GroupBox );
   Global_Sizer.Add( DataCompression_GroupBox );
   Global_Sizer.Add( DataAlignment_GroupBox );
   Global_Sizer.Add( EmbeddedData_GroupBox );
   Global_Sizer.AddSpacing( 8 );
   Global_Sizer.Add( BottomSection_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "XISF Format Preferences" );

   OnReturn( (Dialog::return_event_handler)&XISFPreferencesDialog::__Dialog_Return, *this );
}

// ----------------------------------------------------------------------------

void XISFPreferencesDialog::__Button_Click( Button& sender, bool checked )
{
   if ( sender == Compression_CheckBox )
   {
      CompressionLevel_Label.Enable( Compression_CheckBox.IsChecked() );
      CompressionLevel_SpinBox.Enable( Compression_CheckBox.IsChecked() );
   }
   else if ( sender == Reset_PushButton )
   {
      ICCProfile_CheckBox.SetState( CheckState::ThirdState );
      Metadata_CheckBox.SetState( CheckState::ThirdState );
      Properties_CheckBox.SetState( CheckState::ThirdState );
      Thumbnail_CheckBox.SetState( CheckState::ThirdState );

      XISFOptions defaultOptions;
      StoreFITSKeywords_CheckBox.SetChecked( defaultOptions.storeFITSKeywords );
      IgnoreFITSKeywords_CheckBox.SetChecked( defaultOptions.ignoreFITSKeywords );
      ImportFITSKeywords_CheckBox.SetChecked( defaultOptions.importFITSKeywords );
      Compression_CheckBox.SetChecked( defaultOptions.compressData );
      CompressionLevel_SpinBox.SetValue( Range( defaultOptions.compressionLevel, uint8( 1 ), uint8( 9 ) ) );
      AlignmentSize_SpinBox.SetValue( defaultOptions.blockAlignmentSize );
      MaxInlineSize_SpinBox.SetValue( defaultOptions.maxInlineBlockSize );
   }
   else if ( sender == OK_PushButton )
      Ok();
   else if ( sender == Cancel_PushButton )
      Cancel();
}

// ----------------------------------------------------------------------------

void XISFPreferencesDialog::__Dialog_Return( Dialog& sender, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      overrides.overrideICCProfileEmbedding = ICCProfile_CheckBox.State() != CheckState::ThirdState;
      overrides.embedICCProfiles = ICCProfile_CheckBox.IsChecked();

      overrides.overrideMetadataEmbedding = Metadata_CheckBox.State() != CheckState::ThirdState;
      overrides.embedMetadata = Metadata_CheckBox.IsChecked();

      overrides.overridePropertyEmbedding = Properties_CheckBox.State() != CheckState::ThirdState;
      overrides.embedProperties = Properties_CheckBox.IsChecked();

      overrides.overrideThumbnailEmbedding = Thumbnail_CheckBox.State() != CheckState::ThirdState;
      overrides.embedThumbnails = Thumbnail_CheckBox.IsChecked();

      options.storeFITSKeywords = StoreFITSKeywords_CheckBox.IsChecked();
      options.ignoreFITSKeywords = IgnoreFITSKeywords_CheckBox.IsChecked();
      options.importFITSKeywords = ImportFITSKeywords_CheckBox.IsChecked();

      options.compressData = Compression_CheckBox.IsChecked();
      options.compressionLevel = uint8( CompressionLevel_SpinBox.Value() );

      options.blockAlignmentSize = AlignmentSize_SpinBox.Value();
      options.maxInlineBlockSize = MaxInlineSize_SpinBox.Value();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF XISFPreferencesDialog.cpp - Released 2014/11/30 10:38:10 UTC
