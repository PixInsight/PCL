// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard Geometry Process Module Version 01.01.00.0245
// ****************************************************************************
// DynamicCropPreferencesDialog.cpp - Released 2014/10/29 07:34:55 UTC
// ****************************************************************************
// This file is part of the standard Geometry PixInsight module.
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

#include "DynamicCropPreferencesDialog.h"
#include "DynamicCropInterface.h"

namespace pcl
{

#define fillColor    TheDynamicCropInterface->fillColor

// ----------------------------------------------------------------------------

DynamicCropPreferencesDialog::DynamicCropPreferencesDialog() : Dialog()
{
   savedColor = fillColor;

   pcl::Font fnt = Font();
   int buttonWidth = fnt.Width( String( "Cancel" ) + String( 'M', 4 ) );

   Black_RadioButton.SetText( "Black" );
   Black_RadioButton.OnClick( (pcl::Button::click_event_handler)&DynamicCropPreferencesDialog::Button_Click, *this );

   White_RadioButton.SetText( "White" );
   White_RadioButton.OnClick( (pcl::Button::click_event_handler)&DynamicCropPreferencesDialog::Button_Click, *this );

   Alpha_Label.SetText( "Alpha Blend:" );
   Alpha_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Alpha_Slider.SetRange( 0, 255 );
   Alpha_Slider.SetStepSize( 10 );
   Alpha_Slider.SetTickFrequency( 10 );
   Alpha_Slider.SetTickStyle( TickStyle::NoTicks );
   Alpha_Slider.SetMinWidth( 265 );
   Alpha_Slider.OnValueUpdated( (Slider::value_event_handler)&DynamicCropPreferencesDialog::Slider_ValueUpdated, *this );

   FillColor_Sizer.SetMargin( 8 );
   FillColor_Sizer.SetSpacing( 6 );
   FillColor_Sizer.Add( Black_RadioButton );
   FillColor_Sizer.Add( White_RadioButton );
   FillColor_Sizer.AddSpacing( 10 );
   FillColor_Sizer.Add( Alpha_Label );
   FillColor_Sizer.Add( Alpha_Slider );

   FillColor_GroupBox.SetTitle( "Fill Color" );
   FillColor_GroupBox.SetSizer( FillColor_Sizer );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetMinWidth( buttonWidth );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&DynamicCropPreferencesDialog::Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetMinWidth( buttonWidth );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&DynamicCropPreferencesDialog::Button_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( FillColor_GroupBox );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "DynamicCrop Interface Preferences" );

   OnReturn( (pcl::Dialog::return_event_handler)&DynamicCropPreferencesDialog::Dialog_Return, *this );

   Black_RadioButton.SetChecked( Red( fillColor ) == 0 );
   White_RadioButton.SetChecked( Red( fillColor ) != 0 );

   Alpha_Slider.SetValue( Alpha( fillColor ) );

   Update();
}

void DynamicCropPreferencesDialog::Update()
{
   int alpha = Alpha_Slider.Value();

   Alpha_Slider.SetToolTip( (alpha == 0) ? "Transparent" :
                  ((alpha == 255) ? "Opaque" : String( Alpha_Slider.Value() )) );

   uint8 base = White_RadioButton.IsChecked() ? 0xFF : 0x00;

   fillColor = RGBAColor( base, base, base, alpha );

   if ( TheDynamicCropInterface->view != 0 )
      TheDynamicCropInterface->UpdateView();
}

void DynamicCropPreferencesDialog::Slider_ValueUpdated( Slider& /*sender*/, int /*value*/ )
{
   Update();
}

void DynamicCropPreferencesDialog::Button_Click( Button& sender, bool /*checked*/ )
{
   if ( sender == Black_RadioButton || sender == White_RadioButton )
      Update();
   else if ( sender == OK_PushButton )
      Ok();
   else
      Cancel();
}

void DynamicCropPreferencesDialog::Dialog_Return( Dialog& sender, int retVal )
{
   if ( retVal != StdDialogCode::Ok )
   {
      fillColor = savedColor;

      if ( TheDynamicCropInterface->view != 0 )
         TheDynamicCropInterface->UpdateView();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF DynamicCropPreferencesDialog.cpp - Released 2014/10/29 07:34:55 UTC
