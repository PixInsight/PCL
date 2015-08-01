//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0749
// ----------------------------------------------------------------------------
// Standard FITS File Format Module Version 01.01.02.0306
// ----------------------------------------------------------------------------
// FITSRangeOptionsDialog.cpp - Released 2015/07/31 11:49:40 UTC
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

#include "FITSRangeOptionsDialog.h"

#include <float.h>

namespace pcl
{

// ----------------------------------------------------------------------------

FITSRangeOptionsDialog::FITSRangeOptionsDialog(
   const FITSFormat::OutOfRangePolicyOptions& r, int bps, double min, double max ) :
Dialog(), outOfRange( r )
{
   if ( max < min )
      Swap( min,max );

   FITSFormat::OutOfRangePolicyOptions o = FITSFormat::DefaultOutOfRangePolicyOptions();

   //

   pcl::Font fnt = Font();
   int labelWidth = fnt.Width( String( "If out of range:" ) + 'M' );
   int editWidth = fnt.Width( String( '0', 30 ) );

   //

   Info_Label.EnableWordWrapping();
   Info_Label.EnableRichText();
   Info_Label.SetStyle( FrameStyle::Styled );
   Info_Label.SetMargin( 6 );
   Info_Label.SetText( "<p>This FITS file stores a " + String( bps ) + "-bit floating point image.<br>"
      "Its extreme pixel sample values are:</p>"
      "<p style=\"white-space:pre;\"><pre>" + String().Format( "Minimum: %+.15e", min ) + "<br>"
                                            + String().Format( "Maximum: %+.15e", max ) + "</pre></p>"
      "<p>Please specify a range of values to correctly interpret existing pixel values in this image. "
      "The lower and upper ranges below correspond to the limits of the available dynamic range in the "
      "context of this FITS image. PixInsight will use these limits to scale pixel values to the normalized "
      "[0,1] range, where zero represents black and one represents white.</p>" );

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
   LowerRange_NumericEdit.SetToolTip( "Lower range of input floating point pixel samples" );
   LowerRange_NumericEdit.SetValue( o.lowerRange );

   UpperRange_NumericEdit.label.SetText( "Upper Range:" );
   UpperRange_NumericEdit.label.SetMinWidth( labelWidth );
   UpperRange_NumericEdit.SetReal( true );
   UpperRange_NumericEdit.SetRange( -DBL_MAX, +DBL_MAX );
   UpperRange_NumericEdit.SetPrecision( 15 );
   UpperRange_NumericEdit.EnableScientificNotation();
   UpperRange_NumericEdit.SetScientificNotationTriggerExponent( 5 );
   UpperRange_NumericEdit.edit.SetFixedWidth( editWidth );
   UpperRange_NumericEdit.sizer.AddStretch();
   UpperRange_NumericEdit.SetToolTip( "Upper range of input floating point pixel samples" );
   UpperRange_NumericEdit.SetValue( o.upperRange );

   ReadRescaleMode_Label.SetText( "If out of range:" );
   ReadRescaleMode_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   ReadRescaleMode_Label.SetMinWidth( labelWidth );

   ReadRescaleMode_ComboBox.AddItem( "Rescale image to the specified range" );
   ReadRescaleMode_ComboBox.AddItem( "Truncate all out-of-range values" );
   ReadRescaleMode_ComboBox.SetToolTip( "<p>This parameter tells how to fix input pixel samples whose values "
                                        "exceed the specified input range.</p>" );
   ReadRescaleMode_ComboBox.SetCurrentItem( outOfRange.outOfRangeFixMode );

   ReadRescaleMode_Sizer.SetSpacing( 4 );
   ReadRescaleMode_Sizer.Add( ReadRescaleMode_Label );
   ReadRescaleMode_Sizer.Add( ReadRescaleMode_ComboBox, 100 );

   ReadRange_Sizer.SetMargin( 6 );
   ReadRange_Sizer.SetSpacing( 4 );
   ReadRange_Sizer.Add( LowerRange_NumericEdit );
   ReadRange_Sizer.Add( UpperRange_NumericEdit );
   ReadRange_Sizer.Add( ReadRescaleMode_Sizer );

   ReadRange_GroupBox.SetTitle( "Floating Point Input Range" );
   ReadRange_GroupBox.SetSizer( ReadRange_Sizer );
   ReadRange_GroupBox.AdjustToContents();
   ReadRange_GroupBox.SetMinSize();

   //

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&FITSRangeOptionsDialog::Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&FITSRangeOptionsDialog::Button_Click, *this );

   BottomSection_Sizer.SetSpacing( 8 );
   BottomSection_Sizer.AddStretch();
   BottomSection_Sizer.Add( OK_PushButton );
   BottomSection_Sizer.Add( Cancel_PushButton );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 8 );
   Global_Sizer.Add( Info_Label );
   Global_Sizer.Add( ReadRange_GroupBox );
   Global_Sizer.AddSpacing( 8 );
   Global_Sizer.Add( BottomSection_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "FITS Floating Point Range Options" );

   OnReturn( (Dialog::return_event_handler)&FITSRangeOptionsDialog::Dialog_Return, *this );
}

// ----------------------------------------------------------------------------

void FITSRangeOptionsDialog::Button_Click( Button& sender, bool checked )
{
   if ( sender == OK_PushButton )
      Ok();
   else if ( sender == Cancel_PushButton )
      Cancel();
}

// ----------------------------------------------------------------------------

void FITSRangeOptionsDialog::Dialog_Return( Dialog& sender, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      outOfRange.lowerRange = LowerRange_NumericEdit.Value();
      outOfRange.upperRange = UpperRange_NumericEdit.Value();
      outOfRange.outOfRangeFixMode = FITSFormat::out_of_range_fix_mode( ReadRescaleMode_ComboBox.CurrentItem() );
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF FITSRangeOptionsDialog.cpp - Released 2015/07/31 11:49:40 UTC
