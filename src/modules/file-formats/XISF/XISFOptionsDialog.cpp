//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0779
// ----------------------------------------------------------------------------
// Standard XISF File Format Module Version 01.00.05.0101
// ----------------------------------------------------------------------------
// XISFOptionsDialog.cpp - Released 2015/12/18 08:55:16 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard XISF PixInsight module.
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

#include "XISF.h"
#include "XISFOptionsDialog.h"

namespace pcl
{

// ----------------------------------------------------------------------------

XISFOptionsDialogBase::XISFOptionsDialogBase( const XISFOptions& xisfOptions, const char* labelForWidth ) :
   Dialog(), options( xisfOptions ), m_labelWidth( 0 )
{
   m_labelWidth = Font().Width( String( labelForWidth ? labelForWidth : "Compression codec:" ) + "MM" );
   int ui4 = LogicalPixelsToPhysical( 4 );

   //

   const char* compressionCodecToolTip =
      "<p>Algorithm for compression of XISF data blocks.</p>"
      "<p>ZLib is a lossless compression algorithm capable of very high compression ratios, but comparatively slow, "
      "especially for compression or large blocks.</p>"
      "<p>LZ4 is an extremely fast lossless compression algorithm, but usually achieves significantly smaller "
      "compression ratios than zlib.</p>"
      "<p>LZ4-HC is the high-compression variant of LZ4. It achieves somewhat smaller compression ratios than zlib, "
      "but is faster for compression and extremely fast for decompression.</p>"
      "<p>You can use the compression level parameter to tune compression speed versus compression ratio, or leave "
      "the <i>auto</i> option checked to select the best tradeoff setting for each compression codec automatically. "
      "See also the <i>byte shuffling</i> option.</p>";

   CompressionCodec_Label.SetText( "Compression codec:" );
   CompressionCodec_Label.SetToolTip( compressionCodecToolTip );
   CompressionCodec_Label.SetMinWidth( m_labelWidth );
   CompressionCodec_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   CompressionCodec_ComboBox.AddItem( "ZLib (deflate)" );
   CompressionCodec_ComboBox.AddItem( "LZ4" );
   CompressionCodec_ComboBox.AddItem( "LZ4-HC" );
   CompressionCodec_ComboBox.SetToolTip( compressionCodecToolTip );
   CompressionCodec_ComboBox.SetCurrentItem( CompressionMethodToComboBoxItem( options.compressionMethod ) );

   CompressionCodec_Sizer.SetSpacing( 4 );
   CompressionCodec_Sizer.Add( CompressionCodec_Label );
   CompressionCodec_Sizer.Add( CompressionCodec_ComboBox );
   CompressionCodec_Sizer.AddStretch();

   const char* compressionLevelToolTip =
      "<p>Compression level. Lower values provide faster compression, while higher values give better compression ratios "
      "at the expense of more computation time. If this parameter is set to zero, the XISF module will automatically "
      "select a good compromise between speed and compression efficiency for the selected compression codec.</p>";

   CompressionLevel_Label.SetText( "Compression level:" );
   CompressionLevel_Label.SetToolTip( compressionLevelToolTip );
   CompressionLevel_Label.SetMinWidth( m_labelWidth );
   CompressionLevel_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   CompressionLevel_SpinBox.SetRange( 0, XISF_COMPRESSION_LEVEL_MAX );
   CompressionLevel_SpinBox.SetStepSize( XISF_COMPRESSION_LEVEL_MAX/5 );
   CompressionLevel_SpinBox.SetMinimumValueText( "<Auto>" );
   CompressionLevel_SpinBox.SetToolTip( compressionLevelToolTip );
   CompressionLevel_SpinBox.SetValue( options.compressionLevel );

   CompressionLevel_Sizer.SetSpacing( 4 );
   CompressionLevel_Sizer.Add( CompressionLevel_Label );
   CompressionLevel_Sizer.Add( CompressionLevel_SpinBox );
   CompressionLevel_Sizer.AddStretch();

   CompressionShuffle_CheckBox.SetText( "Byte shuffling" );
   CompressionShuffle_CheckBox.SetToolTip( "<p>The byte shuffle algorithm reorganizes the data in a clever way that "
      "can greatly improve compression ratios, especially for data with high locality (or data containing similar values "
      "close together), such as most scientific and observational data. This option can be particularly efficient for "
      "compression of data structured as contiguous sequences of 16-bit, 32-bit and 64-bit numbers, such as most images "
      "processed with PixInsight. For 8-bit data, byte shuffling has obviously no effect on the compression ratio and "
      "hence is ignored.</p>" );
   CompressionShuffle_CheckBox.SetChecked( options.compressionMethod == XISF_COMPRESSION_NONE ||
                                           XISFEngineBase::CompressionUsesByteShuffle( options.compressionMethod ) );

   CompressionShuffle_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   CompressionShuffle_Sizer.Add( CompressionShuffle_CheckBox );
   CompressionShuffle_Sizer.AddStretch();

   DataCompression_Sizer.SetMargin( 8 );
   DataCompression_Sizer.SetSpacing( 4 );
   DataCompression_Sizer.Add( CompressionCodec_Sizer );
   DataCompression_Sizer.Add( CompressionLevel_Sizer );
   DataCompression_Sizer.Add( CompressionShuffle_Sizer );

   DataCompression_GroupBox.SetTitle( "XISF Data Block Compression" );
   DataCompression_GroupBox.EnableTitleCheckBox();
   DataCompression_GroupBox.SetChecked( options.compressionMethod != XISF_COMPRESSION_NONE );
   DataCompression_GroupBox.SetSizer( DataCompression_Sizer );
   DataCompression_GroupBox.AdjustToContents();

   //

   const char* checksumToolTip =
      "<p>Cryptographic hashing algorithm for calculation of block checksums.</p>"
      "<p>This implementation supports the SHA-1, SHA-256 and SHA-512 algorithms. These algorithms compute checksums "
      "of 20, 32 and 64 bytes, respectively.</p>"
      "<p>Checksums are verified when data blocks are accessed, providing data integrity protection.</p>";

   Checksums_Label.SetText( "Block checksums:" );
   Checksums_Label.SetToolTip( checksumToolTip );
   Checksums_Label.SetMinWidth( m_labelWidth );
   Checksums_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Checksums_ComboBox.AddItem( "SHA-1" );
   Checksums_ComboBox.AddItem( "SHA-256" );
   Checksums_ComboBox.AddItem( "SHA-512" );
   Checksums_ComboBox.SetToolTip( checksumToolTip );
   Checksums_ComboBox.SetCurrentItem( ChecksumMethodToComboBoxItem( options.checksumMethod ) );

   Checksums_Sizer.SetSpacing( 4 );
   Checksums_Sizer.Add( Checksums_Label );
   Checksums_Sizer.Add( Checksums_ComboBox );
   Checksums_Sizer.AddStretch();

   Security_Sizer.SetMargin( 8 );
   Security_Sizer.SetSpacing( 4 );
   Security_Sizer.Add( Checksums_Sizer );

   Security_GroupBox.SetTitle( "Security" );
   Security_GroupBox.EnableTitleCheckBox();
   Security_GroupBox.SetChecked( options.checksumMethod != XISF_CHECKSUM_NONE );
   Security_GroupBox.SetSizer( Security_Sizer );
   Security_GroupBox.AdjustToContents();

   //

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&XISFOptionsDialogBase::Base_Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&XISFOptionsDialogBase::Base_Button_Click, *this );

   BottomSection_Sizer.SetSpacing( 8 );
   BottomSection_Sizer.AddStretch();
   BottomSection_Sizer.Add( OK_PushButton );
   BottomSection_Sizer.Add( Cancel_PushButton );
}

int XISFOptionsDialogBase::CompressionMethodToComboBoxItem( int method )
{
   switch ( method )
   {
   default:
   case XISF_COMPRESSION_ZLIB:
   case XISF_COMPRESSION_ZLIB_SH:
      return 0;
   case XISF_COMPRESSION_LZ4:
   case XISF_COMPRESSION_LZ4_SH:
      return 1;
   case XISF_COMPRESSION_LZ4HC:
   case XISF_COMPRESSION_LZ4HC_SH:
      return 2;
   }
}

int XISFOptionsDialogBase::ComboBoxItemToCompressionMethod( int item )
{
   bool withByteShuffle = CompressionShuffle_CheckBox.IsChecked();
   switch ( item )
   {
   case 0:
      return withByteShuffle ? XISF_COMPRESSION_ZLIB_SH : XISF_COMPRESSION_ZLIB;
   case 1:
      return withByteShuffle ? XISF_COMPRESSION_LZ4_SH : XISF_COMPRESSION_LZ4;
   case 2:
      return withByteShuffle ? XISF_COMPRESSION_LZ4HC_SH : XISF_COMPRESSION_LZ4HC;
   default: // !?
      return XISF_COMPRESSION_UNKNOWN;
   }
}

int XISFOptionsDialogBase::ChecksumMethodToComboBoxItem( int method )
{
   switch ( method )
   {
   default:
   case XISF_CHECKSUM_SHA1:
      return 0;
   case XISF_CHECKSUM_SHA256:
      return 1;
   case XISF_CHECKSUM_SHA512:
      return 2;
   }
}

int XISFOptionsDialogBase::ComboBoxItemToChecksumMethod( int item )
{
   switch ( item )
   {
   case 0:
      return XISF_CHECKSUM_SHA1;
   case 1:
      return XISF_CHECKSUM_SHA256;
   case 2:
      return XISF_CHECKSUM_SHA512;
   default: // !?
      return XISF_CHECKSUM_UNKNOWN;
   }
}

void XISFOptionsDialogBase::GetBaseParameters()
{
   if ( DataCompression_GroupBox.IsChecked() )
      options.compressionMethod = ComboBoxItemToCompressionMethod( CompressionCodec_ComboBox.CurrentItem() );
   else
      options.compressionMethod = XISF_COMPRESSION_NONE;

   options.compressionLevel = uint8( CompressionLevel_SpinBox.Value() );

   if ( Security_GroupBox.IsChecked() )
      options.checksumMethod = ComboBoxItemToChecksumMethod( Checksums_ComboBox.CurrentItem() );
   else
      options.checksumMethod = XISF_CHECKSUM_NONE;
}

void XISFOptionsDialogBase::Base_Button_Click( Button& sender, bool checked )
{
   if ( sender == OK_PushButton )
      Ok();
   else if ( sender == Cancel_PushButton )
      Cancel();
}

// ----------------------------------------------------------------------------

XISFOptionsDialog::XISFOptionsDialog( const ImageOptions& o, const XISFOptions& x, const IsoString& h ) :
   XISFOptionsDialogBase( x ), imageOptions( o ), outputHints( h )
{
   int ui4 = LogicalPixelsToPhysical( 4 );

   UInt8_RadioButton.SetText( "8-bit unsigned integer" );
   UInt8_RadioButton.SetChecked( imageOptions.bitsPerSample == 8 && !imageOptions.ieeefpSampleFormat );

   UInt8_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   UInt8_Sizer.Add( UInt8_RadioButton );
   UInt8_Sizer.AddStretch();

   UInt16_RadioButton.SetText( "16-bit unsigned integer" );
   UInt16_RadioButton.SetChecked( imageOptions.bitsPerSample == 16 && !imageOptions.ieeefpSampleFormat );

   UInt16_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   UInt16_Sizer.Add( UInt16_RadioButton );
   UInt16_Sizer.AddStretch();

   UInt32_RadioButton.SetText( "32-bit unsigned integer" );
   UInt32_RadioButton.SetChecked( imageOptions.bitsPerSample == 32 && !imageOptions.ieeefpSampleFormat );

   UInt32_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   UInt32_Sizer.Add( UInt32_RadioButton );
   UInt32_Sizer.AddStretch();

   Float_RadioButton.SetText( "32-bit IEEE 754 floating point" );
   Float_RadioButton.SetChecked( imageOptions.bitsPerSample == 32 && imageOptions.ieeefpSampleFormat );

   Float_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   Float_Sizer.Add( Float_RadioButton );
   Float_Sizer.AddStretch();

   Double_RadioButton.SetText( "64-bit IEEE 754 floating point" );
   Double_RadioButton.SetChecked( imageOptions.bitsPerSample == 64 && imageOptions.ieeefpSampleFormat );

   Double_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   Double_Sizer.Add( Double_RadioButton );
   Double_Sizer.AddStretch();

   SampleFormat_Sizer.SetMargin( 8 );
   SampleFormat_Sizer.SetSpacing( 4 );
   SampleFormat_Sizer.Add( UInt8_Sizer );
   SampleFormat_Sizer.Add( UInt16_Sizer );
   SampleFormat_Sizer.Add( UInt32_Sizer );
   SampleFormat_Sizer.Add( Float_Sizer );
   SampleFormat_Sizer.Add( Double_Sizer );

   SampleFormat_GroupBox.SetTitle( "Pixel Sample Format" );
   SampleFormat_GroupBox.SetSizer( SampleFormat_Sizer );
   SampleFormat_GroupBox.AdjustToContents();

   //

   /*
    * ### TODO: Core 1.8.x: Support hints in format queries.
    *
   const char* outputHintsToolTip =
      "<p></p>";

   OutputHints_Label.SetText( "Output hints:" );
   OutputHints_Label.SetToolTip( outputHintsToolTip );
   OutputHints_Label.SetMinWidth( m_labelWidth );
   OutputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputHints_Edit.SetStyleSheet(
         "QWidget {"
            "font-family: DejaVu Sans Mono, Monospace;"
         "}"
      );
   OutputHints_Edit.SetToolTip( outputHintsToolTip );
   OutputHints_Edit.SetText( outputHints );

   OutputHints_Sizer.SetSpacing( 4 );
   OutputHints_Sizer.Add( OutputHints_Label );
   OutputHints_Sizer.Add( OutputHints_Edit );
   OutputHints_Sizer.AddStretch();

   OutputHints_GroupBox.SetTitle( "Format Hints" );
   OutputHints_GroupBox.SetSizer( OutputHints_Sizer );
   OutputHints_GroupBox.AdjustToContents();
   */

   //

   Properties_CheckBox.SetText( "Image properties" );
   Properties_CheckBox.SetToolTip( "<p>Embed view properties as XISF properties.</p>" );
   Properties_CheckBox.SetChecked( imageOptions.embedProperties );
   Properties_CheckBox.OnClick( (pcl::Button::click_event_handler)&XISFOptionsDialog::Button_Click, *this );

   Properties_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   Properties_Sizer.Add( Properties_CheckBox );
   Properties_Sizer.AddStretch();

   FITSKeywords_CheckBox.SetText( "FITS header keywords" );
   FITSKeywords_CheckBox.SetToolTip( "<p>Embed FITS header keywords - "
      "<b>Recommended for compatibility with existing legacy data and tool sets.</b></p>" );
   FITSKeywords_CheckBox.SetChecked( options.storeFITSKeywords );

   FITSKeywords_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   FITSKeywords_Sizer.Add( FITSKeywords_CheckBox );
   FITSKeywords_Sizer.AddStretch();

   ICCProfile_CheckBox.SetText( "ICC profile" );
   ICCProfile_CheckBox.SetToolTip( "<p>Embed an ICC color profile.</p>" );
   ICCProfile_CheckBox.SetChecked( imageOptions.embedICCProfile );

   ICCProfile_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   ICCProfile_Sizer.Add( ICCProfile_CheckBox );
   ICCProfile_Sizer.AddStretch();

   DisplayFunction_CheckBox.SetText( "Display function" );
   DisplayFunction_CheckBox.SetToolTip( "<p>Embed display function parameters "
      "(aka <i>screen transfer function</i>, or STF).</p>" );
   DisplayFunction_CheckBox.SetChecked( imageOptions.embedDisplayFunction );

   DisplayFunction_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   DisplayFunction_Sizer.Add( DisplayFunction_CheckBox );
   DisplayFunction_Sizer.AddStretch();

   RGBWorkingSpace_CheckBox.SetText( "RGB working space" );
   RGBWorkingSpace_CheckBox.SetToolTip( "<p>Embed RGB working space parameters.</p>" );
   RGBWorkingSpace_CheckBox.SetChecked( imageOptions.embedRGBWS );

   RGBWorkingSpace_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   RGBWorkingSpace_Sizer.Add( RGBWorkingSpace_CheckBox );
   RGBWorkingSpace_Sizer.AddStretch();

   Thumbnail_CheckBox.SetText( "Thumbnail image" );
   Thumbnail_CheckBox.SetToolTip( "<p>Embed an 8-bit reduced version of the image for quick reference.</p>" );
   Thumbnail_CheckBox.SetChecked( imageOptions.embedThumbnail );

   Thumbnail_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   Thumbnail_Sizer.Add( Thumbnail_CheckBox );
   Thumbnail_Sizer.AddStretch();

   PreviewRects_CheckBox.SetText( "Preview rectangles" );
   PreviewRects_CheckBox.SetToolTip( "<p>Embed preview rectangles and identifiers.</p>" );
   PreviewRects_CheckBox.SetChecked( imageOptions.embedPreviewRects );
   PreviewRects_CheckBox.Enable( imageOptions.embedProperties );

   PreviewRects_Sizer.AddUnscaledSpacing( m_labelWidth + ui4 );
   PreviewRects_Sizer.Add( PreviewRects_CheckBox );
   PreviewRects_Sizer.AddStretch();

   EmbeddedData_Sizer.SetMargin( 8 );
   EmbeddedData_Sizer.SetSpacing( 4 );
   EmbeddedData_Sizer.Add( Properties_Sizer );
   EmbeddedData_Sizer.Add( FITSKeywords_Sizer );
   EmbeddedData_Sizer.Add( ICCProfile_Sizer );
   EmbeddedData_Sizer.Add( DisplayFunction_Sizer );
   EmbeddedData_Sizer.Add( RGBWorkingSpace_Sizer );
   EmbeddedData_Sizer.Add( Thumbnail_Sizer );
   EmbeddedData_Sizer.Add( PreviewRects_Sizer );

   EmbeddedData_GroupBox.SetTitle( "Embedded Data" );
   EmbeddedData_GroupBox.SetSizer( EmbeddedData_Sizer );
   EmbeddedData_GroupBox.AdjustToContents();

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.Add( SampleFormat_GroupBox );
   Global_Sizer.Add( DataCompression_GroupBox );
   Global_Sizer.Add( Security_GroupBox );
   Global_Sizer.Add( EmbeddedData_GroupBox );
   //Global_Sizer.Add( OutputHints_GroupBox ); // ### TODO
   Global_Sizer.AddSpacing( 8 );
   Global_Sizer.Add( BottomSection_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "XISF Options" );

   OnReturn( (pcl::Dialog::return_event_handler)&XISFOptionsDialog::Dialog_Return, *this );
}

// ----------------------------------------------------------------------------

void XISFOptionsDialog::Button_Click( Button& sender, bool checked )
{
   if ( sender == Properties_CheckBox )
      PreviewRects_CheckBox.Enable( checked );
   else
      Base_Button_Click( sender, checked );
}

void XISFOptionsDialog::Dialog_Return( Dialog&/*sender*/, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      if ( UInt8_RadioButton.IsChecked() )
      {
         imageOptions.bitsPerSample = 8;
         imageOptions.ieeefpSampleFormat = false;
         imageOptions.complexSample = false;
      }
      else if ( UInt16_RadioButton.IsChecked() )
      {
         imageOptions.bitsPerSample = 16;
         imageOptions.ieeefpSampleFormat = false;
         imageOptions.complexSample = false;
      }
      else if ( UInt32_RadioButton.IsChecked() )
      {
         imageOptions.bitsPerSample = 32;
         imageOptions.ieeefpSampleFormat = false;
         imageOptions.complexSample = false;
      }
      else if ( Float_RadioButton.IsChecked() )
      {
         imageOptions.bitsPerSample = 32;
         imageOptions.ieeefpSampleFormat = true;
         imageOptions.complexSample = false;
      }
      else if ( Double_RadioButton.IsChecked() )
      {
         imageOptions.bitsPerSample = 64;
         imageOptions.ieeefpSampleFormat = true;
         imageOptions.complexSample = false;
      }

      GetBaseParameters();

      options.storeFITSKeywords = FITSKeywords_CheckBox.IsChecked();
      imageOptions.embedProperties = Properties_CheckBox.IsChecked();
      imageOptions.embedICCProfile = ICCProfile_CheckBox.IsChecked();
      imageOptions.embedDisplayFunction = DisplayFunction_CheckBox.IsChecked();
      imageOptions.embedRGBWS = RGBWorkingSpace_CheckBox.IsChecked();
      imageOptions.embedThumbnail = Thumbnail_CheckBox.IsChecked();
      imageOptions.embedPreviewRects = PreviewRects_CheckBox.IsChecked();

      // outputHints = OutputHints_Edit.Text().Trimmed();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF XISFOptionsDialog.cpp - Released 2015/12/18 08:55:16 UTC
