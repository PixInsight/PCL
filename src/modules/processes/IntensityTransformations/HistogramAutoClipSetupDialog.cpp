//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.00.0763
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.00.0314
// ----------------------------------------------------------------------------
// HistogramAutoClipSetupDialog.cpp - Released 2015/10/08 11:24:40 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "HistogramAutoClipSetupDialog.h"
#include "HistogramTransformationInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

HistogramAutoClipSetupDialog::HistogramAutoClipSetupDialog() : Dialog()
{
   pcl::Font fnt = Font();
   int labelWidth = fnt.Width( String( '0', 10 ) );

   m_shadowsAutoClipping = TheHistogramTransformationInterface->m_shadowsAutoClipping;
   m_highlightsAutoClipping = TheHistogramTransformationInterface->m_highlightsAutoClipping;

   ShadowsAmount_NumericControl.label.SetText( "Shadows:" );
   ShadowsAmount_NumericControl.label.SetFixedWidth( labelWidth );
   ShadowsAmount_NumericControl.slider.SetScaledFixedWidth( 250 );
   ShadowsAmount_NumericControl.slider.SetRange( 0, 200 );
   ShadowsAmount_NumericControl.SetReal();
   ShadowsAmount_NumericControl.SetRange( 0.0, 100.0 );
   ShadowsAmount_NumericControl.SetPrecision( 3 );
   ShadowsAmount_NumericControl.SetToolTip( "Shadows auto clipping amount" );
   ShadowsAmount_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&HistogramAutoClipSetupDialog::__ClippingAmount_ValueUpdated, *this );

   HighlightsAmount_NumericControl.label.SetText( "Highlights:" );
   HighlightsAmount_NumericControl.label.SetFixedWidth( labelWidth );
   HighlightsAmount_NumericControl.slider.SetScaledFixedWidth( 250 );
   HighlightsAmount_NumericControl.slider.SetRange( 0, 200 );
   HighlightsAmount_NumericControl.SetReal();
   HighlightsAmount_NumericControl.SetRange( 0.0, 100.0 );
   HighlightsAmount_NumericControl.SetPrecision( 3 );
   HighlightsAmount_NumericControl.SetToolTip( "Highlights auto clipping amount" );
   HighlightsAmount_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&HistogramAutoClipSetupDialog::__ClippingAmount_ValueUpdated, *this );

   ClippingParameters_Sizer.SetMargin( 8 );
   ClippingParameters_Sizer.SetSpacing( 6 );
   ClippingParameters_Sizer.Add( ShadowsAmount_NumericControl );
   ClippingParameters_Sizer.Add( HighlightsAmount_NumericControl );

   ClippingParameters_GroupBox.SetTitle( "Clipping Amounts (Percentage of total pixels)" );
   ClippingParameters_GroupBox.SetSizer( ClippingParameters_Sizer );

   OK_PushButton.SetText( "OK" );
   OK_PushButton.SetDefault();
   OK_PushButton.SetCursor( StdCursor::Checkmark );
   OK_PushButton.OnClick( (Button::click_event_handler)&HistogramAutoClipSetupDialog::__Button_Click, *this );

   Cancel_PushButton.SetText( "Cancel" );
   Cancel_PushButton.SetCursor( StdCursor::Crossmark );
   Cancel_PushButton.OnClick( (Button::click_event_handler)&HistogramAutoClipSetupDialog::__Button_Click, *this );

   Buttons_Sizer.SetSpacing( 8 );
   Buttons_Sizer.AddStretch();
   Buttons_Sizer.Add( OK_PushButton );
   Buttons_Sizer.Add( Cancel_PushButton );

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( ClippingParameters_GroupBox );
   Global_Sizer.AddSpacing( 4 );
   Global_Sizer.Add( Buttons_Sizer );

   SetSizer( Global_Sizer );
   AdjustToContents();
   SetFixedSize();

   SetWindowTitle( "Histogram Auto Clipping Setup" );

   OnReturn( (Dialog::return_event_handler)&HistogramAutoClipSetupDialog::__Dialog_Return, *this );

   ShadowsAmount_NumericControl.SetValue( 100*m_shadowsAutoClipping );
   HighlightsAmount_NumericControl.SetValue( 100*m_highlightsAutoClipping );

   Update();
}

// ----------------------------------------------------------------------------

void HistogramAutoClipSetupDialog::__ClippingAmount_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == ShadowsAmount_NumericControl )
      m_shadowsAutoClipping = value/100;
   else if ( sender == HighlightsAmount_NumericControl )
      m_highlightsAutoClipping = value/100;
}

// ----------------------------------------------------------------------------

void HistogramAutoClipSetupDialog::__Button_Click( Button& sender, bool checked )
{
   if ( sender == OK_PushButton )
      Ok();
   else
      Cancel();
}

// ----------------------------------------------------------------------------

void HistogramAutoClipSetupDialog::__Dialog_Return( Dialog& sender, int retVal )
{
   if ( retVal == StdDialogCode::Ok )
   {
      TheHistogramTransformationInterface->m_shadowsAutoClipping = m_shadowsAutoClipping;
      TheHistogramTransformationInterface->m_highlightsAutoClipping = m_highlightsAutoClipping;
      TheHistogramTransformationInterface->UpdateAutoClippingToolTips();
   }
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF HistogramAutoClipSetupDialog.cpp - Released 2015/10/08 11:24:40 UTC
