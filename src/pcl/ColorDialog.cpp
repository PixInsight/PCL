// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/ColorDialog.cpp - Released 2014/10/29 07:34:20 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
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

#include <pcl/ColorDialog.h>
#include <pcl/Graphics.h>

namespace pcl
{

// ----------------------------------------------------------------------------

SimpleColorDialog::SimpleColorDialog( RGBA* p, unsigned f ) :
Dialog(), color( p ), workingColor( p ? *p : 0u ), flags( f )
{
   if ( IsGrayscale() )
   {
      SetGreen( workingColor, Red( workingColor ) );
      SetBlue( workingColor, Red( workingColor ) );
   }

   if ( !IsAlphaEnabled() )
      SetAlpha( workingColor, 255 );

   //

   pcl::Font fnt = Font();
   int labelWidth = fnt.Width( String( "Alpha:" ) + 'T' );
   int buttonWidth = fnt.Width( String( "Cancel" ) + String( 'M', 4 ) );

   //

   V0_NumericControl.label.SetText( IsGrayscale() ? "Gray:" : "Red:" );
   V0_NumericControl.label.SetFixedWidth( labelWidth );
   V0_NumericControl.slider.SetRange( 0, 255 );
   V0_NumericControl.slider.SetMinWidth( 300 );
   V0_NumericControl.SetInteger();
   V0_NumericControl.SetRange( 0, 255 );
   V0_NumericControl.SetValue( Red( workingColor ) );
   V0_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SimpleColorDialog::__Color_ValueUpdated, *this );

   //

   V1_NumericControl.label.SetText( "Green:" );
   V1_NumericControl.label.SetFixedWidth( labelWidth );
   V1_NumericControl.slider.SetRange( 0, 255 );
   V1_NumericControl.slider.SetMinWidth( 300 );
   V1_NumericControl.SetInteger();
   V1_NumericControl.SetRange( 0, 255 );
   V1_NumericControl.SetValue( Green( workingColor ) );
   V1_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SimpleColorDialog::__Color_ValueUpdated, *this );

   //

   V2_NumericControl.label.SetText( "Blue:" );
   V2_NumericControl.label.SetFixedWidth( labelWidth );
   V2_NumericControl.slider.SetRange( 0, 255 );
   V2_NumericControl.slider.SetMinWidth( 300 );
   V2_NumericControl.SetInteger();
   V2_NumericControl.SetRange( 0, 255 );
   V2_NumericControl.SetValue( Blue( workingColor ) );
   V2_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SimpleColorDialog::__Color_ValueUpdated, *this );

   //

   V3_NumericControl.label.SetText( "Alpha:" );
   V3_NumericControl.label.SetFixedWidth( labelWidth );
   V3_NumericControl.slider.SetRange( 0, 255 );
   V3_NumericControl.slider.SetMinWidth( 300 );
   V3_NumericControl.SetInteger();
   V3_NumericControl.SetRange( 0, 255 );
   V3_NumericControl.SetValue( Alpha( workingColor ) );
   V3_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&SimpleColorDialog::__Color_ValueUpdated, *this );

   //

   Sliders_Sizer.SetSpacing( 4 );
   Sliders_Sizer.Add( V0_NumericControl );
   Sliders_Sizer.Add( V1_NumericControl );
   Sliders_Sizer.Add( V2_NumericControl );
   Sliders_Sizer.Add( V3_NumericControl );

   //

   ColorSample_Control.SetFixedWidth( 60 );
   ColorSample_Control.OnPaint( (Control::paint_event_handler)&SimpleColorDialog::__ColorSample_Paint, *this );

   //

   Color_Sizer.SetSpacing( 4 );
   Color_Sizer.Add( Sliders_Sizer, 100 );
   Color_Sizer.Add( ColorSample_Control );

   //

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetFixedWidth( buttonWidth );
   OK_PushButton.SetCursor( pcl::Cursor( StdCursor::Checkmark ) );
   OK_PushButton.OnClick( (pcl::Button::click_event_handler)&SimpleColorDialog::__Done_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetFixedWidth( buttonWidth );
   Cancel_PushButton.SetCursor( pcl::Cursor( StdCursor::Crossmark ) );
   Cancel_PushButton.OnClick( (pcl::Button::click_event_handler)&SimpleColorDialog::__Done_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   //

   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.SetMargin( 6 );
   Global_Sizer.Add( Color_Sizer );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );

   if ( IsGrayscale() )
   {
      V1_NumericControl.Hide();
      V2_NumericControl.Hide();
   }

   if ( !IsAlphaEnabled() )
      V3_NumericControl.Hide();

   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "Define Custom Color" );

   OnReturn( (Dialog::return_event_handler)&SimpleColorDialog::__Dialog_Return, *this );
}

SimpleColorDialog::~SimpleColorDialog()
{
}

void SimpleColorDialog::SetColor( RGBA rgba )
{
   workingColor = rgba;

   if ( IsGrayscale() )
   {
      V0_NumericControl.SetValue( Red( workingColor ) );
      V1_NumericControl.SetValue( Red( workingColor ) );
      V2_NumericControl.SetValue( Red( workingColor ) );
   }
   else
   {
      V0_NumericControl.SetValue( Red( workingColor ) );
      V1_NumericControl.SetValue( Green( workingColor ) );
      V2_NumericControl.SetValue( Blue( workingColor ) );
   }

   if ( IsAlphaEnabled() )
      V3_NumericControl.SetValue( Alpha( workingColor ) );
}

void SimpleColorDialog::__Color_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == V0_NumericControl )
      SetRed( workingColor, int( value ) );

   if ( IsGrayscale() )
   {
      SetGreen( workingColor, int( value ) );
      SetBlue( workingColor, int( value ) );
   }
   else
   {
      if ( sender == V1_NumericControl )
         SetGreen( workingColor, int( value ) );
      else if ( sender == V2_NumericControl )
         SetBlue( workingColor, int( value ) );
   }

   if ( IsAlphaEnabled() )
      if ( sender == V3_NumericControl )
         SetAlpha( workingColor, int( value ) );

   ColorSample_Control.Update();
}

void SimpleColorDialog::__ColorSample_Paint( Control& sender, const Rect& updateRect )
{
   Graphics g( sender );
   Rect r = sender.BoundsRect();
   if ( Alpha( workingColor ) != 0xff )
      g.DrawTiledBitmap( r, Bitmap( String( ":/images/transparent_small.png" ) ) );
   g.SetBrush( workingColor );
   g.SetPen( RGBAColor( 0, 0, 0 ) );
   g.DrawRect( r );
}

void SimpleColorDialog::__Done_Click( Button& sender, bool /*checked*/ )
{
   if ( sender == OK_PushButton )
      Ok();
   else
      Cancel();
}

void SimpleColorDialog::__Dialog_Return( Dialog& sender, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
      if ( color != 0 )
         *color = workingColor;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/ColorDialog.cpp - Released 2014/10/29 07:34:20 UTC
