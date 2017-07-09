//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard JPEG2000 File Format Module Version 01.00.02.0317
// ----------------------------------------------------------------------------
// JPEG2000OptionsDialog.cpp - Released 2017-07-09T18:07:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard JPEG2000 PixInsight module.
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

#include "JPEG2000OptionsDialog.h"

namespace pcl
{

// ----------------------------------------------------------------------------

JPEG2000OptionsDialog::JPEG2000OptionsDialog( const ImageOptions& o, const JPEG2000ImageOptions& t, bool isJPC ) :
   Dialog(),
   options(), jp2Options()
{
   int labelWidth1 = Font().Width( String( "Decompression Layers:" ) + 'M' );

   Lossless_RadioButton.SetText( "Lossless" );
   Lossless_RadioButton.SetToolTip( "Nondestructive compression" );
   Lossless_RadioButton.OnClick( (pcl::Button::click_event_handler)&JPEG2000OptionsDialog::CompressionMode_Click, *this );

   Lossy_RadioButton.SetText( "Lossy" );
   Lossy_RadioButton.SetToolTip( "Destructive compression" );
   Lossy_RadioButton.OnClick( (pcl::Button::click_event_handler)&JPEG2000OptionsDialog::CompressionMode_Click, *this );

   CompressionRate_NumericControl.label.SetText( "Compression Rate:" );
   CompressionRate_NumericControl.label.SetMinWidth( labelWidth1 );
   CompressionRate_NumericControl.slider.SetScaledMinWidth( 125 );
   CompressionRate_NumericControl.slider.SetRange( 1, 999 );
   CompressionRate_NumericControl.SetReal( true );
   CompressionRate_NumericControl.SetRange( 0.01, 0.99 );
   CompressionRate_NumericControl.SetPrecision( 2 );
   CompressionRate_NumericControl.SetToolTip( "Compression factor: compressed/original size" );

   UnsignedSample_CheckBox.SetText( "Unsigned Samples" );
   UnsignedSample_CheckBox.SetToolTip( "Write unsigned sample values" );

   Compression_Sizer.SetMargin( 6 );
   Compression_Sizer.Add( Lossless_RadioButton );
   Compression_Sizer.AddSpacing( 6 );
   Compression_Sizer.Add( Lossy_RadioButton );
   Compression_Sizer.Add( CompressionRate_NumericControl );
   Compression_Sizer.AddSpacing( 8 );
   Compression_Sizer.Add( UnsignedSample_CheckBox );

   Compression_GroupBox.SetTitle( "Compression" );
   Compression_GroupBox.SetSizer( Compression_Sizer );
   Compression_GroupBox.AdjustToContents();
   Compression_GroupBox.SetMinSize();

   //

   DecompressionLayers_Label.SetText( "Decompression Layers:" );
   DecompressionLayers_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   DecompressionLayers_Label.SetMinWidth( labelWidth1 );

   DecompressionLayers_SpinBox.SetRange( 2, 10 );
   DecompressionLayers_SpinBox.SetMinWidth( Font().Width( "000000" ) );

   DecompressionLayers_Sizer.SetSpacing( 6 );
   DecompressionLayers_Sizer.Add( DecompressionLayers_Label );
   DecompressionLayers_Sizer.Add( DecompressionLayers_SpinBox );
   DecompressionLayers_Sizer.AddStretch();

   ProgressionOrder_Label.SetText( "Progression Order:" );
   ProgressionOrder_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ProgressionOrder_Label.SetMinWidth( labelWidth1 );

   ProgressionOrder_ComboBox.AddItem( "LRCP: Layer-Resolution-Component-Position" );
   ProgressionOrder_ComboBox.AddItem( "RLCP: Resolution-Layer-Component-Position" );
   ProgressionOrder_ComboBox.AddItem( "RPCL: Resolution-Position-Component-Layer" );
   ProgressionOrder_ComboBox.AddItem( "PCRL: Position-Component-Resolution-Layer" );
   ProgressionOrder_ComboBox.AddItem( "CPRL: Component-Position-Resolution-Layer" );

   ProgressionOrder_Sizer.SetSpacing( 6 );
   ProgressionOrder_Sizer.Add( ProgressionOrder_Label );
   ProgressionOrder_Sizer.Add( ProgressionOrder_ComboBox, 100 );

   Progressive_Sizer.SetMargin( 6 );
   Progressive_Sizer.SetSpacing( 6 );
   Progressive_Sizer.Add( DecompressionLayers_Sizer );
   Progressive_Sizer.Add( ProgressionOrder_Sizer );

   Progressive_GroupBox.SetTitle( "Progressive Decoding" );
   Progressive_GroupBox.EnableTitleCheckBox();
   Progressive_GroupBox.SetSizer( Progressive_Sizer );
   Progressive_GroupBox.AdjustToContents();
   Progressive_GroupBox.SetMinSize();

   //

   TileWidth_NumericEdit.label.SetText( "Tile Width:" );
   TileWidth_NumericEdit.label.SetMinWidth( labelWidth1 );
   TileWidth_NumericEdit.SetInteger();
   TileWidth_NumericEdit.SetRange( 16, 8192 );

   TileHeight_NumericEdit.label.SetText( "Tile Height:" );
   TileHeight_NumericEdit.SetInteger();
   TileHeight_NumericEdit.SetRange( 16, 8192 );

   Tiled_Sizer.SetMargin( 6 );
   Tiled_Sizer.Add( TileWidth_NumericEdit );
   Tiled_Sizer.AddStretch( 25 );
   Tiled_Sizer.Add( TileHeight_NumericEdit );
   Tiled_Sizer.AddStretch( 75 );

   Tiled_GroupBox.SetTitle( "Tiled Image" );
   Tiled_GroupBox.EnableTitleCheckBox();
   Tiled_GroupBox.SetSizer( Tiled_Sizer );
   Tiled_GroupBox.AdjustToContents();
   Tiled_GroupBox.SetMinSize();

   //

   ICCProfile_CheckBox.SetText( "ICC Profile" );
   ICCProfile_CheckBox.SetToolTip( "Embed an ICC profile" );

   Resolution_CheckBox.SetText( "Resolution" );
   Resolution_CheckBox.SetToolTip( "Embed image resolution information" );

   EmbeddedData_Sizer.SetMargin( 6 );
   EmbeddedData_Sizer.Add( ICCProfile_CheckBox );
   EmbeddedData_Sizer.Add( Resolution_CheckBox );

   EmbeddedData_GroupBox.SetTitle( "Embedded Data" );
   EmbeddedData_GroupBox.SetSizer( EmbeddedData_Sizer );
   EmbeddedData_GroupBox.AdjustToContents();
   EmbeddedData_GroupBox.SetMinSize();

   if ( isJPC )
      EmbeddedData_GroupBox.Hide();

   //

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&JPEG2000OptionsDialog::Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&JPEG2000OptionsDialog::Button_Click, *this );

   BottomSection_Sizer.SetSpacing( 8 );
   BottomSection_Sizer.AddStretch();
   BottomSection_Sizer.Add( OK_PushButton );
   BottomSection_Sizer.Add( Cancel_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.Add( Compression_GroupBox );
   Global_Sizer.Add( Progressive_GroupBox );
   Global_Sizer.Add( Tiled_GroupBox );
   Global_Sizer.Add( EmbeddedData_GroupBox );
   Global_Sizer.AddSpacing( 8 );
   Global_Sizer.Add( BottomSection_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( isJPC ? "JPEG2000 Code Stream Options" : "JPEG2000 JP2 Options" );

   OnReturn( (pcl::Dialog::return_event_handler)&JPEG2000OptionsDialog::Dialog_Return, *this );

   options = o;
   jp2Options = t;

   Lossless_RadioButton.SetChecked( !jp2Options.lossyCompression );

   Lossy_RadioButton.SetChecked( jp2Options.lossyCompression );

   CompressionRate_NumericControl.Enable( jp2Options.lossyCompression );
   CompressionRate_NumericControl.SetValue( jp2Options.compressionRate );

   UnsignedSample_CheckBox.SetChecked( !jp2Options.signedSample );

   Progressive_GroupBox.SetChecked( jp2Options.numberOfLayers > 1 );

   DecompressionLayers_SpinBox.SetValue( jp2Options.numberOfLayers );

   ProgressionOrder_ComboBox.SetCurrentItem( jp2Options.progressionOrder );

   Tiled_GroupBox.SetChecked( jp2Options.tiledImage );

   TileWidth_NumericEdit.SetValue( jp2Options.tileWidth );
   TileHeight_NumericEdit.SetValue( jp2Options.tileHeight );

   ICCProfile_CheckBox.SetChecked( options.embedICCProfile );
   Resolution_CheckBox.SetChecked( jp2Options.resolutionData );
}

// ----------------------------------------------------------------------------

void JPEG2000OptionsDialog::CompressionMode_Click( Button& /*sender*/, bool /*checked*/ )
{
   CompressionRate_NumericControl.Enable( Lossy_RadioButton.IsChecked() );
}

// ----------------------------------------------------------------------------

void JPEG2000OptionsDialog::Button_Click( Button& sender, bool /*checked*/ )
{
   if ( sender == OK_PushButton )
      Ok();
   else
      Cancel();
}

// ----------------------------------------------------------------------------

void JPEG2000OptionsDialog::Dialog_Return( Dialog& /*sender*/, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      jp2Options.lossyCompression = Lossy_RadioButton.IsChecked();
      jp2Options.compressionRate = CompressionRate_NumericControl.Value();
      jp2Options.signedSample = !UnsignedSample_CheckBox.IsChecked();
      jp2Options.resolutionData = Resolution_CheckBox.IsChecked();
      jp2Options.tiledImage = Tiled_GroupBox.IsChecked();
      jp2Options.tileWidth = int( TileWidth_NumericEdit.Value() );
      jp2Options.tileHeight = int( TileHeight_NumericEdit.Value() );
      jp2Options.numberOfLayers = Progressive_GroupBox.IsChecked() ? DecompressionLayers_SpinBox.Value() : 1;
      jp2Options.progressionOrder = ProgressionOrder_ComboBox.CurrentItem();

      options.embedICCProfile = ICCProfile_CheckBox.IsChecked();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF JPEG2000OptionsDialog.cpp - Released 2017-07-09T18:07:25Z
