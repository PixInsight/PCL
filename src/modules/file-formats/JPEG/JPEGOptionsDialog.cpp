//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0927
// ----------------------------------------------------------------------------
// Standard JPEG File Format Module Version 01.00.04.0358
// ----------------------------------------------------------------------------
// JPEGOptionsDialog.cpp - Released 2018-11-23T16:14:51Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG PixInsight module.
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

#include "JPEGOptionsDialog.h"

namespace pcl
{

// ----------------------------------------------------------------------------

JPEGOptionsDialog::JPEGOptionsDialog( const ImageOptions& o, const JPEGImageOptions& t ) :
   options( o ), jpegOptions( t )
{
   pcl::Font fnt = Font();
   int labelWidth = fnt.Width( String( "Quality:" ) + String( 'M', 3 ) );

   Quality_Label.SetText( "Quality:" );
   Quality_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Quality_Label.SetMinWidth( labelWidth );

   Quality_SpinBox.SetRange( 1, 100 );
   Quality_SpinBox.SetStepSize( 5 );
   Quality_SpinBox.EnableWrapping();
   Quality_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&JPEGOptionsDialog::SpinBox_ValueUpdated, *this );

   Quality_Slider.SetRange( 1, 100 );
   Quality_Slider.SetStepSize( 5 );
   Quality_Slider.SetTickFrequency( 5 );
   Quality_Slider.SetTickStyle( TickStyle::Bottom );
   Quality_Slider.SetScaledMinWidth( 210 );
   Quality_Slider.OnValueUpdated( (Slider::value_event_handler)&JPEGOptionsDialog::Slider_ValueUpdated, *this );

   CompressionRow1_Sizer.SetSpacing( 6 );
   CompressionRow1_Sizer.Add( Quality_Label );
   CompressionRow1_Sizer.Add( Quality_SpinBox );
   CompressionRow1_Sizer.Add( Quality_Slider, 100 );

   OptimizedCoding_CheckBox.SetText( "Optimized coding" );
   OptimizedCoding_CheckBox.SetToolTip( "<p>Optimize entropy encoding. "
                           "This option forces the use of Huffman coding.</p>" );
   OptimizedCoding_CheckBox.OnClick( (Button::click_event_handler)&JPEGOptionsDialog::Button_Click, *this );

   CompressionRow2_Sizer.AddUnscaledSpacing( labelWidth );
   CompressionRow2_Sizer.AddSpacing( 6 );
   CompressionRow2_Sizer.Add( OptimizedCoding_CheckBox, 100 );

   ArithmeticCoding_CheckBox.SetText( "Arithmetic coding" );
   ArithmeticCoding_CheckBox.SetToolTip( "<p>Use arithmetic coding instead of Huffman coding. "
                           "This option is incompatible with entropy optimization.</p>" );

   CompressionRow3_Sizer.AddUnscaledSpacing( labelWidth );
   CompressionRow3_Sizer.AddSpacing( 6 );
   CompressionRow3_Sizer.Add( ArithmeticCoding_CheckBox, 100 );

   Progressive_CheckBox.SetText( "Progressive" );
   Progressive_CheckBox.SetToolTip( "<p>Create a JPEG image that can be loaded by successive layers.</p>" );

   CompressionRow4_Sizer.AddUnscaledSpacing( labelWidth );
   CompressionRow4_Sizer.AddSpacing( 6 );
   CompressionRow4_Sizer.Add( Progressive_CheckBox, 100 );

   Compression_Sizer.SetMargin( 6 );
   Compression_Sizer.SetSpacing( 4 );
   Compression_Sizer.Add( CompressionRow1_Sizer );
   Compression_Sizer.Add( CompressionRow2_Sizer );
   Compression_Sizer.Add( CompressionRow3_Sizer );
   Compression_Sizer.Add( CompressionRow4_Sizer );

   JPEGCompression_GroupBox.SetTitle( "JPEG Compression" );
   JPEGCompression_GroupBox.SetSizer( Compression_Sizer );
   JPEGCompression_GroupBox.AdjustToContents();

   ICCProfile_CheckBox.SetText( "ICC Profile" );
   ICCProfile_CheckBox.SetToolTip( "Embed an ICC profile" );

   ICCProfile_Sizer.AddUnscaledSpacing( labelWidth );
   ICCProfile_Sizer.AddSpacing( 6 );
   ICCProfile_Sizer.Add( ICCProfile_CheckBox, 100 );

   EmbeddedData_Sizer.SetMargin( 6 );
   EmbeddedData_Sizer.Add( ICCProfile_Sizer );

   EmbeddedData_GroupBox.SetTitle( "Embedded Data" );
   EmbeddedData_GroupBox.SetSizer( EmbeddedData_Sizer );
   EmbeddedData_GroupBox.AdjustToContents();

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (Button::click_event_handler)&JPEGOptionsDialog::Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&JPEGOptionsDialog::Button_Click, *this );

   BottomSection_Sizer.SetSpacing( 8 );
   BottomSection_Sizer.AddStretch();
   BottomSection_Sizer.Add( OK_PushButton );
   BottomSection_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.Add( JPEGCompression_GroupBox );
   Global_Sizer.Add( EmbeddedData_GroupBox );
   Global_Sizer.AddSpacing( 8 );
   Global_Sizer.Add( BottomSection_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "JPEG Options" );

   OnReturn( (Dialog::return_event_handler)&JPEGOptionsDialog::Dialog_Return, *this );

   if ( jpegOptions.quality < 1 || jpegOptions.quality > 100 )
      jpegOptions.quality = JPEGQuality::Default;

   Quality_SpinBox.SetValue( jpegOptions.quality );
   Quality_Slider.SetValue( jpegOptions.quality );
   OptimizedCoding_CheckBox.SetChecked( jpegOptions.optimizedCoding );
   ArithmeticCoding_CheckBox.SetChecked( jpegOptions.arithmeticCoding );
   ArithmeticCoding_CheckBox.Enable( !jpegOptions.optimizedCoding );
   Progressive_CheckBox.SetChecked( jpegOptions.progressive );

   ICCProfile_CheckBox.SetChecked( options.embedICCProfile );
}

// ----------------------------------------------------------------------------

void JPEGOptionsDialog::SpinBox_ValueUpdated( SpinBox& /*sender*/, int value )
{
   Quality_Slider.SetValue( value );
}

// ----------------------------------------------------------------------------

void JPEGOptionsDialog::Slider_ValueUpdated( Slider& /*sender*/, int value )
{
   Quality_SpinBox.SetValue( value );
}

// ----------------------------------------------------------------------------

void JPEGOptionsDialog::Button_Click( Button& sender, bool checked )
{
   if ( sender == OptimizedCoding_CheckBox )
      ArithmeticCoding_CheckBox.Enable( !checked );
   else if ( sender == OK_PushButton )
      Ok();
   else if ( sender == Cancel_PushButton )
      Cancel();
}

// ----------------------------------------------------------------------------

void JPEGOptionsDialog::Dialog_Return( Dialog& /*sender*/, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      jpegOptions.quality = Quality_SpinBox.Value();
      jpegOptions.optimizedCoding = OptimizedCoding_CheckBox.IsChecked();
      jpegOptions.arithmeticCoding = ArithmeticCoding_CheckBox.IsChecked();
      jpegOptions.progressive = Progressive_CheckBox.IsChecked();

      options.embedICCProfile = ICCProfile_CheckBox.IsChecked();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF JPEGOptionsDialog.cpp - Released 2018-11-23T16:14:51Z
