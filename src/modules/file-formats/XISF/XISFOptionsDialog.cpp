// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard XISF File Format Module Version 01.00.00.0023
// ****************************************************************************
// XISFOptionsDialog.cpp - Released 2014/11/30 10:38:10 UTC
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

#include "XISFOptionsDialog.h"

#include <zlib/zlib.h> // for Z_BEST_SPEED/Z_BEST_COMPRESSION

namespace pcl
{

// ----------------------------------------------------------------------------

XISFOptionsDialog::XISFOptionsDialog( const ImageOptions& o, const XISFOptions& x, const IsoString& h ) :
Dialog(), imageOptions( o ), xisfOptions( x ), outputHints( h )
{
   pcl::Font fnt = Font();
   int labelWidth = fnt.Width( String( "Compression level:" ) + "MMM" );

   //

   UInt8_RadioButton.SetText( "8-bit unsigned integer" );
   UInt8_RadioButton.SetChecked( imageOptions.bitsPerSample == 8 && !imageOptions.ieeefpSampleFormat );

   UInt8_Sizer.AddSpacing( labelWidth + 4 );
   UInt8_Sizer.Add( UInt8_RadioButton );
   UInt8_Sizer.AddStretch();

   UInt16_RadioButton.SetText( "16-bit unsigned integer" );
   UInt16_RadioButton.SetChecked( imageOptions.bitsPerSample == 16 && !imageOptions.ieeefpSampleFormat );

   UInt16_Sizer.AddSpacing( labelWidth + 4 );
   UInt16_Sizer.Add( UInt16_RadioButton );
   UInt16_Sizer.AddStretch();

   UInt32_RadioButton.SetText( "32-bit unsigned integer" );
   UInt32_RadioButton.SetChecked( imageOptions.bitsPerSample == 32 && !imageOptions.ieeefpSampleFormat );

   UInt32_Sizer.AddSpacing( labelWidth + 4 );
   UInt32_Sizer.Add( UInt32_RadioButton );
   UInt32_Sizer.AddStretch();

   Float_RadioButton.SetText( "32-bit IEEE 754 floating point" );
   Float_RadioButton.SetChecked( imageOptions.bitsPerSample == 32 && imageOptions.ieeefpSampleFormat );

   Float_Sizer.AddSpacing( labelWidth + 4 );
   Float_Sizer.Add( Float_RadioButton );
   Float_Sizer.AddStretch();

   Double_RadioButton.SetText( "64-bit IEEE 754 floating point" );
   Double_RadioButton.SetChecked( imageOptions.bitsPerSample == 64 && imageOptions.ieeefpSampleFormat );

   Double_Sizer.AddSpacing( labelWidth + 4 );
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

   Compression_CheckBox.SetText( "Compressed data" );
   Compression_CheckBox.SetToolTip( "<p>Compress data blocks with the zlib/deflate algorithm.</p>" );
   Compression_CheckBox.SetChecked( xisfOptions.compressData );
   Compression_CheckBox.OnClick( (pcl::Button::click_event_handler)&XISFOptionsDialog::__Button_Click, *this );

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
   CompressionLevel_Label.Enable( xisfOptions.compressData );

   CompressionLevel_SpinBox.SetRange( int( Z_BEST_SPEED ), int( Z_BEST_COMPRESSION ) );
   CompressionLevel_SpinBox.SetToolTip( compressionLevelToolTip );
   CompressionLevel_SpinBox.SetValue( Range( xisfOptions.compressionLevel, uint8( Z_BEST_SPEED ), uint8( Z_BEST_COMPRESSION ) ) );
   CompressionLevel_SpinBox.Enable( xisfOptions.compressData );

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

   Properties_CheckBox.SetText( "Properties" );
   Properties_CheckBox.SetToolTip( "<p>Embed view properties as XISF properties.</p>" );
   Properties_CheckBox.SetChecked( imageOptions.embedProperties );

   Properties_Sizer.AddSpacing( labelWidth + 4 );
   Properties_Sizer.Add( Properties_CheckBox );
   Properties_Sizer.AddStretch();

   FITSKeywords_CheckBox.SetText( "FITS header keywords" );
   FITSKeywords_CheckBox.SetToolTip( "<p>Embed FITS header keywords - "
      "<b>Recommended for compatibility with existing data and tool sets.</b></p>" );
   FITSKeywords_CheckBox.SetChecked( xisfOptions.storeFITSKeywords );

   FITSKeywords_Sizer.AddSpacing( labelWidth + 4 );
   FITSKeywords_Sizer.Add( FITSKeywords_CheckBox );
   FITSKeywords_Sizer.AddStretch();

   ICCProfile_CheckBox.SetText( "ICC Profile" );
   ICCProfile_CheckBox.SetToolTip( "<p>Embed an ICC color profile.</p>" );
   ICCProfile_CheckBox.SetChecked( imageOptions.embedICCProfile );

   ICCProfile_Sizer.AddSpacing( labelWidth + 4 );
   ICCProfile_Sizer.Add( ICCProfile_CheckBox );
   ICCProfile_Sizer.AddStretch();

   Metadata_CheckBox.SetText( "Metadata" );
   Metadata_CheckBox.SetToolTip( "Embed XML metadata <* not available *>." );
   Metadata_CheckBox.SetChecked( imageOptions.embedMetadata );

   Metadata_Sizer.AddSpacing( labelWidth + 4 );
   Metadata_Sizer.Add( Metadata_CheckBox );
   Metadata_Sizer.AddStretch();

   Thumbnail_CheckBox.SetText( "Thumbnail Image" );
   Thumbnail_CheckBox.SetToolTip( "<p>Embed an 8-bit reduced version of the image for quick reference.</p>" );
   Thumbnail_CheckBox.SetChecked( imageOptions.embedThumbnail );

   Thumbnail_Sizer.AddSpacing( labelWidth + 4 );
   Thumbnail_Sizer.Add( Thumbnail_CheckBox );
   Thumbnail_Sizer.AddStretch();

   EmbeddedData_Sizer.SetMargin( 8 );
   EmbeddedData_Sizer.SetSpacing( 4 );
   EmbeddedData_Sizer.Add( Properties_Sizer );
   EmbeddedData_Sizer.Add( FITSKeywords_Sizer );
   EmbeddedData_Sizer.Add( ICCProfile_Sizer );
   EmbeddedData_Sizer.Add( Metadata_Sizer );
   EmbeddedData_Sizer.Add( Thumbnail_Sizer );

   EmbeddedData_GroupBox.SetTitle( "Embedded Data" );
   EmbeddedData_GroupBox.SetSizer( EmbeddedData_Sizer );
   EmbeddedData_GroupBox.AdjustToContents();

   //

   /*
    * ### TODO: Core 1.8.3.x: Support hints in format queries.
    *
   const char* outputHintsToolTip =
      "<p></p>";

   OutputHints_Label.SetText( "Output hints:" );
   OutputHints_Label.SetToolTip( outputHintsToolTip );
   OutputHints_Label.SetMinWidth( labelWidth );
   OutputHints_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   OutputHints_Edit.SetStyleSheet(
         "QWidget {"
         "   font-family: DejaVu Sans Mono, Monospace;"
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

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&XISFOptionsDialog::__Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&XISFOptionsDialog::__Button_Click, *this );

   BottomSection_Sizer.SetSpacing( 8 );
   BottomSection_Sizer.AddStretch();
   BottomSection_Sizer.Add( OK_PushButton );
   BottomSection_Sizer.Add( Cancel_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( SampleFormat_GroupBox );
   Global_Sizer.Add( DataCompression_GroupBox );
   Global_Sizer.Add( EmbeddedData_GroupBox );
   //Global_Sizer.Add( OutputHints_GroupBox ); // ### TODO
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( BottomSection_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "XISF Options" );

   OnReturn( (pcl::Dialog::return_event_handler)&XISFOptionsDialog::__Dialog_Return, *this );
}

// ----------------------------------------------------------------------------

void XISFOptionsDialog::__Button_Click( Button& sender, bool /*checked*/ )
{
   if ( sender == Compression_CheckBox )
   {
      CompressionLevel_Label.Enable( Compression_CheckBox.IsChecked() );
      CompressionLevel_SpinBox.Enable( Compression_CheckBox.IsChecked() );
   }
   else if ( sender == OK_PushButton )
      Ok();
   else if ( sender == Cancel_PushButton )
      Cancel();
}

// ----------------------------------------------------------------------------

void XISFOptionsDialog::__Dialog_Return( Dialog& /*sender*/, int retVal )
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

      xisfOptions.compressData = Compression_CheckBox.IsChecked();
      xisfOptions.compressionLevel = uint8( CompressionLevel_SpinBox.Value() );

      imageOptions.embedProperties = Properties_CheckBox.IsChecked();
      xisfOptions.storeFITSKeywords = FITSKeywords_CheckBox.IsChecked();
      imageOptions.embedICCProfile = ICCProfile_CheckBox.IsChecked();
      imageOptions.embedMetadata = Metadata_CheckBox.IsChecked();
      imageOptions.embedThumbnail = Thumbnail_CheckBox.IsChecked();

      // outputHints = OutputHints_Edit.Text().Trimmed();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF XISFOptionsDialog.cpp - Released 2014/11/30 10:38:10 UTC
