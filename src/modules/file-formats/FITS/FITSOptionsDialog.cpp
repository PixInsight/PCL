//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 01.01.02.0314
// ----------------------------------------------------------------------------
// FITSOptionsDialog.cpp - Released 2015/10/08 11:24:33 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard FITS PixInsight module.
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

#include "FITSOptionsDialog.h"

namespace pcl
{

// ----------------------------------------------------------------------------

FITSOptionsDialog::FITSOptionsDialog( const pcl::ImageOptions& o, const pcl::FITSImageOptions& f ) :
   Dialog(),
   options( o ),
   fitsOptions( f )
{
   pcl::Font fnt = Font();
   int labelWidth = fnt.Width( String( "64-bit IEEE 754 floating point" ) + String( 'M', 3 ) );

   //

   UInt8_RadioButton.SetText( "8-bit unsigned integer" );
   UInt8_RadioButton.SetMinWidth( labelWidth );

   UInt16_RadioButton.SetText( "16-bit unsigned integer" );
   UInt16_RadioButton.SetMinWidth( labelWidth );

   Int16_RadioButton.SetText( "16-bit signed integer" );
   Int16_RadioButton.SetMinWidth( labelWidth );

   Float_RadioButton.SetText( "32-bit IEEE 754 floating point" );
   Float_RadioButton.SetMinWidth( labelWidth );

   SampleFormatLeft_Sizer.SetSpacing( 4 );
   SampleFormatLeft_Sizer.Add( UInt8_RadioButton );
   SampleFormatLeft_Sizer.Add( UInt16_RadioButton );
   SampleFormatLeft_Sizer.Add( Int16_RadioButton );
   SampleFormatLeft_Sizer.Add( Float_RadioButton );

   UInt32_RadioButton.SetText( "32-bit unsigned integer" );
   UInt32_RadioButton.SetMinWidth( labelWidth );

   Int32_RadioButton.SetText( "32-bit signed integer" );
   Int32_RadioButton.SetMinWidth( labelWidth );

   Int64_RadioButton.SetText( "64-bit signed integer" );
   Int64_RadioButton.SetMinWidth( labelWidth );
   Int64_RadioButton.Disable();
   Int64_RadioButton.SetToolTip( "<* not available *>" );

   Double_RadioButton.SetText( "64-bit IEEE 754 floating point" );
   Double_RadioButton.SetMinWidth( labelWidth );

   SampleFormatRight_Sizer.SetSpacing( 4 );
   SampleFormatRight_Sizer.Add( UInt32_RadioButton );
   SampleFormatRight_Sizer.Add( Int32_RadioButton );
   SampleFormatRight_Sizer.Add( Int64_RadioButton );
   SampleFormatRight_Sizer.Add( Double_RadioButton );

   SampleFormat_Sizer.SetMargin( 6 );
   SampleFormat_Sizer.SetSpacing( 12 );
   SampleFormat_Sizer.Add( SampleFormatLeft_Sizer );
   SampleFormat_Sizer.Add( SampleFormatRight_Sizer );

   SampleFormat_GroupBox.SetTitle( "Sample Format" );
   SampleFormat_GroupBox.SetSizer( SampleFormat_Sizer );
   SampleFormat_GroupBox.AdjustToContents();

   //

   ICCProfile_CheckBox.SetText( "ICC Profile" );
   ICCProfile_CheckBox.SetMinWidth( labelWidth );
   ICCProfile_CheckBox.SetToolTip( "Embed an ICC profile" );

   Properties_CheckBox.SetText( "Properties" );
   Properties_CheckBox.SetMinWidth( labelWidth );
   Properties_CheckBox.SetToolTip( "Embed image properties" );

   EmbeddedDataLeft_Sizer.SetSpacing( 4 );
   EmbeddedDataLeft_Sizer.Add( ICCProfile_CheckBox );
   EmbeddedDataLeft_Sizer.Add( Properties_CheckBox );
   //EmbeddedDataLeft_Sizer.AddStretch();

   Thumbnail_CheckBox.SetText( "Thumbnail Image" );
   Thumbnail_CheckBox.SetMinWidth( labelWidth );
   Thumbnail_CheckBox.SetToolTip( "Embed an 8-bit reduced version of the image for quick reference" );

   FixedPrecision_CheckBox.SetText( "Fixed-precision keywords" );
   FixedPrecision_CheckBox.SetMinWidth( labelWidth );
   FixedPrecision_CheckBox.SetToolTip( "Write fixed-precision floating-point FITS keywords" );

   EmbeddedDataRight_Sizer.SetSpacing( 4 );
   EmbeddedDataRight_Sizer.Add( Thumbnail_CheckBox );
   EmbeddedDataRight_Sizer.Add( FixedPrecision_CheckBox );
   //EmbeddedDataRight_Sizer.AddStretch();

   EmbeddedData_Sizer.SetMargin( 6 );
   EmbeddedData_Sizer.SetSpacing( 12 );
   EmbeddedData_Sizer.Add( EmbeddedDataLeft_Sizer );
   EmbeddedData_Sizer.Add( EmbeddedDataRight_Sizer );

   EmbeddedData_GroupBox.SetTitle( "Embedded Data" );
   EmbeddedData_GroupBox.SetSizer( EmbeddedData_Sizer );
   EmbeddedData_GroupBox.AdjustToContents();

   //

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&FITSOptionsDialog::Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&FITSOptionsDialog::Button_Click, *this );

   BottomSection_Sizer.SetSpacing( 8 );
   BottomSection_Sizer.AddStretch();
   BottomSection_Sizer.Add( OK_PushButton );
   BottomSection_Sizer.Add( Cancel_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.Add( SampleFormat_GroupBox );
   Global_Sizer.Add( EmbeddedData_GroupBox );
   Global_Sizer.AddSpacing( 8 );
   Global_Sizer.Add( BottomSection_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "FITS Options" );

   OnReturn( (pcl::Dialog::return_event_handler)&FITSOptionsDialog::Dialog_Return, *this );

   UInt8_RadioButton.SetChecked( options.bitsPerSample == 8 );
   UInt16_RadioButton.SetChecked( options.bitsPerSample == 16 && fitsOptions.unsignedIntegers );
   Int16_RadioButton.SetChecked( options.bitsPerSample == 16 && !fitsOptions.unsignedIntegers );
   UInt32_RadioButton.SetChecked( options.bitsPerSample == 32 && fitsOptions.unsignedIntegers );
   Int32_RadioButton.SetChecked( options.bitsPerSample == 32 && !fitsOptions.unsignedIntegers );
   Float_RadioButton.SetChecked( options.bitsPerSample == 32 && options.ieeefpSampleFormat );
   Double_RadioButton.SetChecked( options.bitsPerSample == 64 && options.ieeefpSampleFormat );

   ICCProfile_CheckBox.SetChecked( options.embedICCProfile );
   Properties_CheckBox.SetChecked( options.embedProperties );
   FixedPrecision_CheckBox.SetChecked( fitsOptions.writeFixedFloatKeywords );
   Thumbnail_CheckBox.SetChecked( options.embedThumbnail );
}

// ----------------------------------------------------------------------------

void FITSOptionsDialog::Button_Click( Button& sender, bool /*checked*/ )
{
   if ( sender == OK_PushButton )
      Ok();
   else if ( sender == Cancel_PushButton )
      Cancel();
}

// ----------------------------------------------------------------------------

void FITSOptionsDialog::Dialog_Return( Dialog& /*sender*/, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      if ( UInt8_RadioButton.IsChecked() )
      {
         options.bitsPerSample = 8;
         options.ieeefpSampleFormat = false;
         fitsOptions.unsignedIntegers = true;
      }
      else if ( UInt16_RadioButton.IsChecked() )
      {
         options.bitsPerSample = 16;
         options.ieeefpSampleFormat = false;
         fitsOptions.unsignedIntegers = true;
      }
      else if ( Int16_RadioButton.IsChecked() )
      {
         options.bitsPerSample = 16;
         options.ieeefpSampleFormat = false;
         fitsOptions.unsignedIntegers = false;
      }
      else if ( UInt32_RadioButton.IsChecked() )
      {
         options.bitsPerSample = 32;
         options.ieeefpSampleFormat = false;
         fitsOptions.unsignedIntegers = true;
      }
      else if ( Int32_RadioButton.IsChecked() )
      {
         options.bitsPerSample = 32;
         options.ieeefpSampleFormat = false;
         fitsOptions.unsignedIntegers = false;
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

      options.embedICCProfile = ICCProfile_CheckBox.IsChecked();
      options.embedThumbnail = Thumbnail_CheckBox.IsChecked();
      options.embedProperties = Properties_CheckBox.IsChecked();

      fitsOptions.writeFixedFloatKeywords = FixedPrecision_CheckBox.IsChecked();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FITSOptionsDialog.cpp - Released 2015/10/08 11:24:33 UTC
