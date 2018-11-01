//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard TIFF File Format Module Version 01.00.07.0353
// ----------------------------------------------------------------------------
// TIFFOptionsDialog.cpp - Released 2018-11-01T11:07:09Z
// ----------------------------------------------------------------------------
// This file is part of the standard TIFF PixInsight module.
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

#include "TIFFOptionsDialog.h"

namespace pcl
{

// ----------------------------------------------------------------------------

TIFFOptionsDialog::TIFFOptionsDialog( const pcl::ImageOptions& o, const pcl::TIFFImageOptions& t ) :
Dialog(), options( o ), tiffOptions( t )
{
   int labelWidth = Font().Width( String( "64-bit IEEE 754 floating point" ) + 'M' );

   UInt8_RadioButton.SetText( "8-bit unsigned integer" );
   UInt8_RadioButton.SetMinWidth( labelWidth );

   UInt16_RadioButton.SetText( "16-bit unsigned integer" );
   UInt16_RadioButton.SetMinWidth( labelWidth );

   UInt32_RadioButton.SetText( "32-bit unsigned integer" );
   UInt32_RadioButton.SetMinWidth( labelWidth );

   Float_RadioButton.SetText( "32-bit IEEE 754 floating point" );
   Float_RadioButton.SetMinWidth( labelWidth );

   Double_RadioButton.SetText( "64-bit IEEE 754 floating point" );
   Double_RadioButton.SetMinWidth( labelWidth );

   SampleFormat_Sizer.SetMargin( 6 );
   SampleFormat_Sizer.Add( UInt8_RadioButton );
   SampleFormat_Sizer.Add( UInt16_RadioButton );
   SampleFormat_Sizer.Add( UInt32_RadioButton );
   SampleFormat_Sizer.Add( Float_RadioButton );
   SampleFormat_Sizer.Add( Double_RadioButton );

   SampleFormat_GroupBox.SetTitle( "Sample Format" );
   SampleFormat_GroupBox.SetSizer( SampleFormat_Sizer );
   SampleFormat_GroupBox.AdjustToContents();

   NoCompression_RadioButton.SetText( "None" );
   NoCompression_RadioButton.SetMinWidth( labelWidth );

   ZIP_RadioButton.SetText( "ZIP" );
   ZIP_RadioButton.SetMinWidth( labelWidth );

   LZW_RadioButton.SetText( "LZW - deprecated" );
   LZW_RadioButton.SetMinWidth( labelWidth );
   LZW_RadioButton.SetToolTip( "LZW compression (discouraged)" );

   Compression_Sizer.SetMargin( 6 );
   Compression_Sizer.Add( NoCompression_RadioButton );
   Compression_Sizer.Add( ZIP_RadioButton );
   Compression_Sizer.Add( LZW_RadioButton );

   Compression_GroupBox.SetTitle( "Compression" );
   Compression_GroupBox.SetSizer( Compression_Sizer );
   Compression_GroupBox.AdjustToContents();

   ICCProfile_CheckBox.SetText( "ICC Profile" );
   ICCProfile_CheckBox.SetMinWidth( labelWidth );
   ICCProfile_CheckBox.SetToolTip( "Embed an ICC profile" );

   EmbeddedData_Sizer.SetMargin( 6 );
   EmbeddedData_Sizer.Add( ICCProfile_CheckBox );

   EmbeddedData_GroupBox.SetTitle( "Embedded Data" );
   EmbeddedData_GroupBox.SetSizer( EmbeddedData_Sizer );
   EmbeddedData_GroupBox.AdjustToContents();

   Planar_CheckBox.SetText( "Planar organization" );
   Planar_CheckBox.AdjustToContents();
   Planar_CheckBox.SetMinSize();
   Planar_CheckBox.SetToolTip( "Write each color channel in a separate data block, or <i>plane</i>." );

   AssociatedAlpha_CheckBox.SetText( "Associated alpha channel" );
   AssociatedAlpha_CheckBox.AdjustToContents();
   AssociatedAlpha_CheckBox.SetMinSize();
   AssociatedAlpha_CheckBox.SetToolTip( "Select the active alpha channel as the associated image transparency." );

   PremultipliedAlpha_CheckBox.SetText( "Premultiplied alpha channel" );
   PremultipliedAlpha_CheckBox.AdjustToContents();
   PremultipliedAlpha_CheckBox.SetMinSize();
   PremultipliedAlpha_CheckBox.SetToolTip( "Write RGB/K components premultiplied by the active alpha channel." );

   Miscellaneous_Sizer.SetMargin( 6 );
   Miscellaneous_Sizer.Add( Planar_CheckBox );
   Miscellaneous_Sizer.Add( AssociatedAlpha_CheckBox );
   Miscellaneous_Sizer.Add( PremultipliedAlpha_CheckBox );

   Miscellaneous_GroupBox.SetTitle( "Miscellaneous" );
   Miscellaneous_GroupBox.SetSizer( Miscellaneous_Sizer );
   Miscellaneous_GroupBox.AdjustToContents();

   LeftPanel_Sizer.SetSpacing( 4 );
   LeftPanel_Sizer.Add( SampleFormat_GroupBox );
   LeftPanel_Sizer.Add( Compression_GroupBox );
   LeftPanel_Sizer.Add( EmbeddedData_GroupBox );
   LeftPanel_Sizer.Add( Miscellaneous_GroupBox );

   ImageDescription_Sizer.SetMargin( 6 );
   ImageDescription_Sizer.Add( ImageDescription_TextBox );

   ImageDescription_GroupBox.SetTitle( "Image Description" );
   ImageDescription_GroupBox.SetSizer( ImageDescription_Sizer );

   ImageCopyright_Sizer.SetMargin( 6 );
   ImageCopyright_Sizer.Add( ImageCopyright_TextBox );

   ImageCopyright_GroupBox.SetTitle( "Image Copyright" );
   ImageCopyright_GroupBox.SetSizer( ImageCopyright_Sizer );

   SoftwareDescription_Sizer.SetMargin( 6 );
   SoftwareDescription_Sizer.Add( SoftwareDescription_TextBox );

   SoftwareDescription_GroupBox.SetTitle( "Software Description" );
   SoftwareDescription_GroupBox.SetSizer( SoftwareDescription_Sizer );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&TIFFOptionsDialog::Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&TIFFOptionsDialog::Button_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   RightPanel_Sizer.SetSpacing( 4 );
   RightPanel_Sizer.Add( ImageDescription_GroupBox, 50 );
   RightPanel_Sizer.Add( ImageCopyright_GroupBox, 25 );
   RightPanel_Sizer.Add( SoftwareDescription_GroupBox, 25 );
   RightPanel_Sizer.AddSpacing( 8 );
   RightPanel_Sizer.Add( Buttons_Sizer );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.Add( LeftPanel_Sizer );
   Global_Sizer.Add( RightPanel_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "TIFF Options" );

   OnReturn( (pcl::Dialog::return_event_handler)&TIFFOptionsDialog::Dialog_Return, *this );

   UInt8_RadioButton.SetChecked( options.bitsPerSample == 8 );
   UInt16_RadioButton.SetChecked( options.bitsPerSample == 16 );
   UInt32_RadioButton.SetChecked( options.bitsPerSample == 32 );
   Float_RadioButton.SetChecked( options.bitsPerSample == 32 && options.ieeefpSampleFormat );
   Double_RadioButton.SetChecked( options.bitsPerSample == 64 && options.ieeefpSampleFormat );

   NoCompression_RadioButton.SetChecked( tiffOptions.compression == pcl::TIFFCompression::None );
   ZIP_RadioButton.SetChecked( tiffOptions.compression == pcl::TIFFCompression::ZIP );
   LZW_RadioButton.SetChecked( tiffOptions.compression == pcl::TIFFCompression::LZW );

   ICCProfile_CheckBox.SetChecked( options.embedICCProfile );

   ImageDescription_TextBox.SetText( tiffOptions.imageDescription );
   ImageCopyright_TextBox.SetText( tiffOptions.copyright );
   SoftwareDescription_TextBox.SetText( tiffOptions.software );

   Planar_CheckBox.SetChecked( tiffOptions.planar );
   AssociatedAlpha_CheckBox.SetChecked( tiffOptions.associatedAlpha );
   PremultipliedAlpha_CheckBox.SetChecked( tiffOptions.premultipliedAlpha );
}

// ----------------------------------------------------------------------------

void TIFFOptionsDialog::Button_Click( Button& sender, bool /*checked*/ )
{
   if ( sender == OK_PushButton )
      Ok();
   else
      Cancel();
}

// ----------------------------------------------------------------------------

void TIFFOptionsDialog::Dialog_Return( Dialog& /*sender*/, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      if ( UInt8_RadioButton.IsChecked() )
      {
         options.bitsPerSample = 8;
         options.ieeefpSampleFormat = false;
      }
      else if ( UInt16_RadioButton.IsChecked() )
      {
         options.bitsPerSample = 16;
         options.ieeefpSampleFormat = false;
      }
      else if ( UInt32_RadioButton.IsChecked() )
      {
         options.bitsPerSample = 32;
         options.ieeefpSampleFormat = false;
      }
      else if ( Float_RadioButton.IsChecked() )
      {
         options.bitsPerSample = 32;
         options.ieeefpSampleFormat = true;
      }
      else if ( Double_RadioButton.IsChecked() )
      {
         options.bitsPerSample = 64;
         options.ieeefpSampleFormat = true;
      }

      if ( NoCompression_RadioButton.IsChecked() )
         tiffOptions.compression = pcl::TIFFCompression::None;
      else if ( ZIP_RadioButton.IsChecked() )
         tiffOptions.compression = pcl::TIFFCompression::ZIP;
      else if ( LZW_RadioButton.IsChecked() ) // ### PCL 1.x: Consider suppressing
         tiffOptions.compression = pcl::TIFFCompression::LZW;

      options.embedICCProfile = ICCProfile_CheckBox.IsChecked();

      tiffOptions.planar = Planar_CheckBox.IsChecked();

      tiffOptions.associatedAlpha = AssociatedAlpha_CheckBox.IsChecked();
      tiffOptions.premultipliedAlpha = PremultipliedAlpha_CheckBox.IsChecked();

      tiffOptions.imageDescription = ImageDescription_TextBox.Text();
      tiffOptions.copyright = ImageCopyright_TextBox.Text();
      tiffOptions.software = SoftwareDescription_TextBox.Text();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF TIFFOptionsDialog.cpp - Released 2018-11-01T11:07:09Z
