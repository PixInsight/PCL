//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.01.0784
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0355
// ----------------------------------------------------------------------------
// AutoHistogramInterface.cpp - Released 2016/02/21 20:22:43 UTC
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
//
// Copyright (c) 2003-2016 Pleiades Astrophoto S.L. All Rights Reserved.
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

#include "AutoHistogramInterface.h"
#include "AutoHistogramParameters.h"
#include "AutoHistogramProcess.h"

#include <pcl/Color.h>
#include <pcl/Graphics.h>
#include <pcl/HistogramTransformation.h>
#include <pcl/ImageWindow.h>

namespace pcl
{

// ----------------------------------------------------------------------------

AutoHistogramInterface* TheAutoHistogramInterface = 0;

// ----------------------------------------------------------------------------

#include "AutoHistogramIcon.xpm"

// ----------------------------------------------------------------------------

AutoHistogramInterface::AutoHistogramInterface() :
ProcessInterface(), instance( TheAutoHistogramProcess ), GUI( 0 )
{
   TheAutoHistogramInterface = this;
}

AutoHistogramInterface::~AutoHistogramInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

// ----------------------------------------------------------------------------

IsoString AutoHistogramInterface::Id() const
{
   return "AutoHistogram";
}

MetaProcess* AutoHistogramInterface::Process() const
{
   return TheAutoHistogramProcess;
}

const char** AutoHistogramInterface::IconImageXPM() const
{
   return AutoHistogramIcon_XPM;
}

void AutoHistogramInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void AutoHistogramInterface::ResetInstance()
{
   AutoHistogramInstance defaultInstance( TheAutoHistogramProcess );
   ImportProcess( defaultInstance );
}

bool AutoHistogramInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "AutoHistogram" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheAutoHistogramProcess;
}

ProcessImplementation* AutoHistogramInterface::NewProcess() const
{
   return new AutoHistogramInstance( instance );
}

bool AutoHistogramInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const AutoHistogramInstance* r = dynamic_cast<const AutoHistogramInstance*>( &p );
   if ( r == 0 )
   {
      whyNot = "Not an AutoHistogram instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool AutoHistogramInterface::RequiresInstanceValidation() const
{
   return true;
}

bool AutoHistogramInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

bool AutoHistogramInterface::WantsReadoutNotifications() const
{
   return true;
}

void AutoHistogramInterface::UpdateReadout( const View& v, const DPoint&, double R, double G, double B, double /*A*/ )
{
   if ( GUI != 0 && IsVisible() && GUI->CaptureReadouts_CheckBox.IsChecked() )
   {
      if ( instance.p_stretchTogether )
      {
         RGBColorSystem rgbws;
         v.Window().GetRGBWS( rgbws );
         instance.p_targetMedian[0] = instance.p_targetMedian[1] = instance.p_targetMedian[2] = rgbws.Lightness( R, G, B );
      }
      else
      {
         instance.p_targetMedian[0] = R;
         instance.p_targetMedian[1] = G;
         instance.p_targetMedian[2] = B;
      }

      UpdateControls();
   }
}

// ----------------------------------------------------------------------------

void AutoHistogramInterface::UpdateControls()
{
   GUI->ClipParameters_SectionBar.SetChecked( instance.p_clip );

   GUI->ClipTogether_RadioButton.SetChecked( instance.p_clipTogether );
   GUI->ClipSeparate_RadioButton.SetChecked( !instance.p_clipTogether );

   GUI->ClipLowRK_NumericControl.label.SetText( instance.p_clipTogether ? "RGB/K:" : "R/K:" );
   GUI->ClipLowRK_NumericControl.Enable( instance.p_clip );
   GUI->ClipLowRK_NumericControl.SetValue( instance.p_clipLow[0] );

   GUI->ClipLowG_NumericControl.Enable( !instance.p_clipTogether && instance.p_clip );
   GUI->ClipLowG_NumericControl.SetValue( instance.p_clipLow[1] );

   GUI->ClipLowB_NumericControl.Enable( !instance.p_clipTogether && instance.p_clip );
   GUI->ClipLowB_NumericControl.SetValue( instance.p_clipLow[2] );

   GUI->ClipHighRK_NumericControl.label.SetText( instance.p_clipTogether ? "RGB/K:" : "R/K:" );
   GUI->ClipHighRK_NumericControl.Enable( instance.p_clip );
   GUI->ClipHighRK_NumericControl.SetValue( instance.p_clipHigh[0] );

   GUI->ClipHighG_NumericControl.Enable( !instance.p_clipTogether && instance.p_clip );
   GUI->ClipHighG_NumericControl.SetValue( instance.p_clipHigh[1] );

   GUI->ClipHighB_NumericControl.Enable( !instance.p_clipTogether && instance.p_clip );
   GUI->ClipHighB_NumericControl.SetValue( instance.p_clipHigh[2] );

   GUI->StretchParameters_SectionBar.SetChecked( instance.p_stretch );

   GUI->StretchMethod_ComboBox.SetCurrentItem( instance.p_stretchMethod );
   GUI->StretchMethod_ComboBox.Enable( instance.p_stretch );

   GUI->StretchTogether_RadioButton.SetChecked( instance.p_stretchTogether );
   GUI->StretchSeparate_RadioButton.SetChecked( !instance.p_stretchTogether );

   GUI->TargetMedianRK_NumericControl.label.SetText( instance.p_stretchTogether ? "RGB/K:" : "R/K:" );
   GUI->TargetMedianRK_NumericControl.Enable( instance.p_stretch );
   GUI->TargetMedianRK_NumericControl.SetValue( instance.p_targetMedian[0] );

   GUI->TargetMedianG_NumericControl.Enable( !instance.p_stretchTogether && instance.p_stretch );
   GUI->TargetMedianG_NumericControl.SetValue( instance.p_targetMedian[1] );

   GUI->TargetMedianB_NumericControl.Enable( !instance.p_stretchTogether && instance.p_stretch );
   GUI->TargetMedianB_NumericControl.SetValue( instance.p_targetMedian[2] );

   GUI->ColorSample_Control.Update();
}

// ----------------------------------------------------------------------------

void AutoHistogramInterface::__Click( Button& sender, bool checked )
{
   if ( sender == GUI->StretchSeparate_RadioButton )
      instance.p_stretchTogether = false;
   else if ( sender == GUI->StretchTogether_RadioButton )
   {
      instance.p_stretchTogether = true;
      instance.p_targetMedian[1] = instance.p_targetMedian[2] = instance.p_targetMedian[0];
   }
   else if ( sender == GUI->ClipSeparate_RadioButton )
      instance.p_clipTogether = false;
   else if ( sender == GUI->ClipTogether_RadioButton )
   {
      instance.p_clipTogether = true;
      instance.p_clipLow[1] = instance.p_clipLow[2] = instance.p_clipLow[0];
      instance.p_clipHigh[1] = instance.p_clipHigh[2] = instance.p_clipHigh[0];
   }
   else if ( sender == GUI->SetToActiveImage_Button )
   {
      ImageWindow w = ImageWindow::ActiveWindow();
      if ( !w.IsNull() )
      {
         View view = w.CurrentView();

         instance.p_stretchTogether = !view.IsColor();

         DVector median = view.ComputeOrFetchProperty( "Median" ).ToDVector();
         for ( int c = 0; c < median.Length(); ++c )
            instance.p_targetMedian[c] = median[c];
      }
   }

   UpdateControls();
}

void AutoHistogramInterface::__CheckSection( SectionBar& sender, bool checked )
{
   if ( sender == GUI->ClipParameters_SectionBar )
      instance.p_clip = checked;
   else if ( sender == GUI->StretchParameters_SectionBar )
      instance.p_stretch = checked;

   UpdateControls();
}

void AutoHistogramInterface::__ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->StretchMethod_ComboBox )
      instance.p_stretchMethod = itemIndex;
}

void AutoHistogramInterface::__ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->ClipLowRK_NumericControl )
   {
      instance.p_clipLow[0] = value;
      if ( instance.p_clipTogether )
      {
         instance.p_clipLow[1] = instance.p_clipLow[2] = value;
         GUI->ClipLowG_NumericControl.SetValue( value );
         GUI->ClipLowB_NumericControl.SetValue( value );
      }
   }
   else if ( sender == GUI->ClipLowG_NumericControl )
      instance.p_clipLow[1] = value;
   else if ( sender == GUI->ClipLowB_NumericControl )
      instance.p_clipLow[2] = value;
   else if ( sender == GUI->ClipHighRK_NumericControl )
   {
      instance.p_clipHigh[0] = value;
      if ( instance.p_clipTogether )
      {
         instance.p_clipHigh[1] = instance.p_clipHigh[2] = value;
         GUI->ClipHighG_NumericControl.SetValue( value );
         GUI->ClipHighB_NumericControl.SetValue( value );
      }
   }
   else if ( sender == GUI->ClipHighG_NumericControl )
      instance.p_clipHigh[1] = value;
   else if ( sender == GUI->ClipHighB_NumericControl )
      instance.p_clipHigh[2] = value;
   else if ( sender == GUI->TargetMedianRK_NumericControl )
   {
      instance.p_targetMedian[0] = value;
      if ( instance.p_stretchTogether )
      {
         instance.p_targetMedian[1] = instance.p_targetMedian[2] = value;
         GUI->TargetMedianG_NumericControl.SetValue( value );
         GUI->TargetMedianB_NumericControl.SetValue( value );
      }
      GUI->ColorSample_Control.Update();
   }
   else if ( sender == GUI->TargetMedianG_NumericControl )
   {
      instance.p_targetMedian[1] = value;
      GUI->ColorSample_Control.Update();
   }
   else if ( sender == GUI->TargetMedianB_NumericControl )
   {
      instance.p_targetMedian[2] = value;
      GUI->ColorSample_Control.Update();
   }
}

void AutoHistogramInterface::__Paint( Control& sender, const Rect& /*updateRect*/ )
{
   if ( sender == GUI->ColorSample_Control )
   {
      Graphics g( sender );
      g.SetBrush( RGBAColor( float( instance.p_targetMedian[0] ),
                             float( instance.p_targetMedian[1] ),
                             float( instance.p_targetMedian[2] ) ) );
      g.SetPen( 0xff000000, sender.DisplayPixelRatio() );
      g.DrawRect( sender.BoundsRect() );
   }
}

// ----------------------------------------------------------------------------

AutoHistogramInterface::GUIData::GUIData( AutoHistogramInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth = fnt.Width( String( "RGB/K:" ) + 'T' );
   int labelWidth1 = fnt.Width( String( "Stretch method:" ) + 'T' );

   //

   ClipParameters_SectionBar.SetTitle( "Histogram Clipping" );
   ClipParameters_SectionBar.SetSection( ClipParameters_Control );
   ClipParameters_SectionBar.EnableTitleCheckBox();
   ClipParameters_SectionBar.OnCheck( (SectionBar::check_event_handler)&AutoHistogramInterface::__CheckSection, w );

   ClipSeparate_RadioButton.SetText( "Individual RGB/K channels" );
   ClipSeparate_RadioButton.SetToolTip( "<p>Apply separate histogram clippings to each RGB/gray channel.</p>" );
   ClipSeparate_RadioButton.OnClick( (Button::click_event_handler)&AutoHistogramInterface::__Click, w );

   ClipTogether_RadioButton.SetText( "Joint RGB/K channels" );
   ClipTogether_RadioButton.SetToolTip( "<p>Apply a unique histogram clipping to all RGB/gray channels.</p>" );
   ClipTogether_RadioButton.OnClick( (Button::click_event_handler)&AutoHistogramInterface::__Click, w );

   ClipMode_Sizer.SetSpacing( 4 );
   ClipMode_Sizer.Add( ClipTogether_RadioButton );
   ClipMode_Sizer.Add( ClipSeparate_RadioButton );
   ClipMode_Sizer.AddStretch();

   ClipLowRK_NumericControl.label.SetText( "RGB/K:" );
   ClipLowRK_NumericControl.label.SetFixedWidth( labelWidth );
   ClipLowRK_NumericControl.slider.SetScaledMinWidth( 200 );
   ClipLowRK_NumericControl.slider.SetRange( 0, 100 );
   ClipLowRK_NumericControl.SetReal();
   ClipLowRK_NumericControl.SetRange( 0, 1 );
   ClipLowRK_NumericControl.SetPrecision( TheAHClipLowRParameter->Precision() );
   ClipLowRK_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&AutoHistogramInterface::__ValueUpdated, w );

   ClipLowG_NumericControl.label.SetText( "G:" );
   ClipLowG_NumericControl.label.SetFixedWidth( labelWidth );
   ClipLowG_NumericControl.slider.SetScaledMinWidth( 200 );
   ClipLowG_NumericControl.slider.SetRange( 0, 100 );
   ClipLowG_NumericControl.SetReal();
   ClipLowG_NumericControl.SetRange( 0, 1 );
   ClipLowG_NumericControl.SetPrecision( TheAHClipLowGParameter->Precision() );
   ClipLowG_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&AutoHistogramInterface::__ValueUpdated, w );

   ClipLowB_NumericControl.label.SetText( "B:" );
   ClipLowB_NumericControl.label.SetFixedWidth( labelWidth );
   ClipLowB_NumericControl.slider.SetScaledMinWidth( 200 );
   ClipLowB_NumericControl.slider.SetRange( 0, 100 );
   ClipLowB_NumericControl.SetReal();
   ClipLowB_NumericControl.SetRange( 0, 1 );
   ClipLowB_NumericControl.SetPrecision( TheAHClipLowBParameter->Precision() );
   ClipLowB_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&AutoHistogramInterface::__ValueUpdated, w );

   ClipLow_Sizer.SetMargin( 4 );
   ClipLow_Sizer.SetSpacing( 4 );
   ClipLow_Sizer.Add( ClipLowRK_NumericControl );
   ClipLow_Sizer.Add( ClipLowG_NumericControl );
   ClipLow_Sizer.Add( ClipLowB_NumericControl );

   ClipLow_GroupBox.SetTitle( "Shadows Clipping" );
   ClipLow_GroupBox.SetSizer( ClipLow_Sizer );

   ClipHighRK_NumericControl.label.SetText( "RGB/K:" );
   ClipHighRK_NumericControl.label.SetFixedWidth( labelWidth );
   ClipHighRK_NumericControl.slider.SetScaledMinWidth( 200 );
   ClipHighRK_NumericControl.slider.SetRange( 0, 100 );
   ClipHighRK_NumericControl.SetReal();
   ClipHighRK_NumericControl.SetRange( 0, 1 );
   ClipHighRK_NumericControl.SetPrecision( TheAHClipHighRParameter->Precision() );
   ClipHighRK_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&AutoHistogramInterface::__ValueUpdated, w );

   ClipHighG_NumericControl.label.SetText( "G:" );
   ClipHighG_NumericControl.label.SetFixedWidth( labelWidth );
   ClipHighG_NumericControl.slider.SetScaledMinWidth( 200 );
   ClipHighG_NumericControl.slider.SetRange( 0, 100 );
   ClipHighG_NumericControl.SetReal();
   ClipHighG_NumericControl.SetRange( 0, 1 );
   ClipHighG_NumericControl.SetPrecision( TheAHClipHighGParameter->Precision() );
   ClipHighG_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&AutoHistogramInterface::__ValueUpdated, w );

   ClipHighB_NumericControl.label.SetText( "B:" );
   ClipHighB_NumericControl.label.SetFixedWidth( labelWidth );
   ClipHighB_NumericControl.slider.SetScaledMinWidth( 200 );
   ClipHighB_NumericControl.slider.SetRange( 0, 100 );
   ClipHighB_NumericControl.SetReal();
   ClipHighB_NumericControl.SetRange( 0, 1 );
   ClipHighB_NumericControl.SetPrecision( TheAHClipHighBParameter->Precision() );
   ClipHighB_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&AutoHistogramInterface::__ValueUpdated, w );

   ClipHigh_Sizer.SetMargin( 4 );
   ClipHigh_Sizer.SetSpacing( 4 );
   ClipHigh_Sizer.Add( ClipHighRK_NumericControl );
   ClipHigh_Sizer.Add( ClipHighG_NumericControl );
   ClipHigh_Sizer.Add( ClipHighB_NumericControl );

   ClipHigh_GroupBox.SetTitle( "Highlights Clipping" );
   ClipHigh_GroupBox.SetSizer( ClipHigh_Sizer );

   ClipValues_Sizer.SetSpacing( 8 );
   ClipValues_Sizer.Add( ClipLow_GroupBox );
   ClipValues_Sizer.Add( ClipHigh_GroupBox );

   ClipParameters_Sizer.SetMargin( 4 );
   ClipParameters_Sizer.SetSpacing( 4 );
   ClipParameters_Sizer.Add( ClipMode_Sizer );
   ClipParameters_Sizer.Add( ClipValues_Sizer );

   ClipParameters_Control.SetSizer( ClipParameters_Sizer );

   //

   StretchParameters_SectionBar.SetTitle( "Target Median Values" );
   StretchParameters_SectionBar.SetSection( StretchParameters_Control );
   StretchParameters_SectionBar.EnableTitleCheckBox();
   StretchParameters_SectionBar.OnCheck( (SectionBar::check_event_handler)&AutoHistogramInterface::__CheckSection, w );

   const char* stretchMethodToolTip = "<p>This is the algorithm used to apply a nonlinear stretch to the image. "
                                      "The selected algorithm affects the contrast of the result.</p>";

   StretchMethod_Label.SetText( "Stretch method:" );
   StretchMethod_Label.SetMinWidth( labelWidth1 );
   StretchMethod_Label.SetToolTip( stretchMethodToolTip );
   StretchMethod_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   StretchMethod_ComboBox.AddItem( "Gamma Exponent" );
   StretchMethod_ComboBox.AddItem( "Logarithmic Transformation" );
   StretchMethod_ComboBox.AddItem( "Rational Interpolation (MTF)" );
   StretchMethod_ComboBox.SetMaxVisibleItemCount( 4 );
   StretchMethod_ComboBox.SetToolTip( stretchMethodToolTip );
   StretchMethod_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&AutoHistogramInterface::__ItemSelected, w );

   StretchMethod_Sizer.SetSpacing( 4 );
   StretchMethod_Sizer.Add( StretchMethod_Label );
   StretchMethod_Sizer.Add( StretchMethod_ComboBox );
   StretchMethod_Sizer.AddStretch();

   StretchSeparate_RadioButton.SetText( "Individual RGB/K channels" );
   StretchSeparate_RadioButton.SetToolTip( "<p>Apply a separate nonlinear stretching function to each RGB/gray channel.</p>" );
   StretchSeparate_RadioButton.OnClick( (Button::click_event_handler)&AutoHistogramInterface::__Click, w );

   StretchTogether_RadioButton.SetText( "Joint RGB/K channels" );
   StretchTogether_RadioButton.SetToolTip( "<p<Apply a unique nonlinear stretching function to all RGB/gray channels.</p>" );
   StretchTogether_RadioButton.OnClick( (Button::click_event_handler)&AutoHistogramInterface::__Click, w );

   StretchMode_Sizer.SetSpacing( 4 );
   StretchMode_Sizer.Add( StretchTogether_RadioButton );
   StretchMode_Sizer.Add( StretchSeparate_RadioButton );
   StretchMode_Sizer.AddStretch();

   TargetMedianRK_NumericControl.label.SetText( "RGB/K:" );
   TargetMedianRK_NumericControl.label.SetFixedWidth( labelWidth );
   TargetMedianRK_NumericControl.slider.SetScaledMinWidth( 250 );
   TargetMedianRK_NumericControl.slider.SetRange( 0, 250 );
   TargetMedianRK_NumericControl.SetReal();
   TargetMedianRK_NumericControl.SetRange( TheAHTargetMedianRParameter->MinimumValue(), TheAHTargetMedianRParameter->MaximumValue() );
   TargetMedianRK_NumericControl.SetPrecision( TheAHTargetMedianRParameter->Precision() );
   TargetMedianRK_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&AutoHistogramInterface::__ValueUpdated, w );

   TargetMedianG_NumericControl.label.SetText( "G:" );
   TargetMedianG_NumericControl.label.SetFixedWidth( labelWidth );
   TargetMedianG_NumericControl.slider.SetScaledMinWidth( 250 );
   TargetMedianG_NumericControl.slider.SetRange( 0, 250 );
   TargetMedianG_NumericControl.SetReal();
   TargetMedianG_NumericControl.SetRange( TheAHTargetMedianGParameter->MinimumValue(), TheAHTargetMedianGParameter->MaximumValue() );
   TargetMedianG_NumericControl.SetPrecision( TheAHTargetMedianGParameter->Precision() );
   TargetMedianG_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&AutoHistogramInterface::__ValueUpdated, w );

   TargetMedianB_NumericControl.label.SetText( "B:" );
   TargetMedianB_NumericControl.label.SetFixedWidth( labelWidth );
   TargetMedianB_NumericControl.slider.SetScaledMinWidth( 250 );
   TargetMedianB_NumericControl.slider.SetRange( 0, 250 );
   TargetMedianB_NumericControl.SetReal();
   TargetMedianB_NumericControl.SetRange( TheAHTargetMedianBParameter->MinimumValue(), TheAHTargetMedianBParameter->MaximumValue() );
   TargetMedianB_NumericControl.SetPrecision( TheAHTargetMedianBParameter->Precision() );
   TargetMedianB_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&AutoHistogramInterface::__ValueUpdated, w );

   TargetMedianValues_Sizer.SetSpacing( 4 );
   TargetMedianValues_Sizer.Add( TargetMedianRK_NumericControl );
   TargetMedianValues_Sizer.Add( TargetMedianG_NumericControl );
   TargetMedianValues_Sizer.Add( TargetMedianB_NumericControl );

   ColorSample_Control.SetScaledFixedWidth( 60 );
   ColorSample_Control.OnPaint( (Control::paint_event_handler)&AutoHistogramInterface::__Paint, w );

   TargetMedian_Sizer.SetSpacing( 4 );
   TargetMedian_Sizer.Add( TargetMedianValues_Sizer );
   TargetMedian_Sizer.Add( ColorSample_Control );

   CaptureReadouts_CheckBox.SetText( "Capture readouts" );
   CaptureReadouts_CheckBox.SetToolTip( "<p>Read target median values from image readouts when this tool is visible</p>" );

   CaptureReadouts_Sizer.AddUnscaledSpacing( labelWidth + w.LogicalPixelsToPhysical( 4 ) );
   CaptureReadouts_Sizer.Add( CaptureReadouts_CheckBox );

   SetToActiveImage_Button.SetText( "Set As Active Image" );
   SetToActiveImage_Button.SetToolTip( "<p>Read target median values from the current active view.</p>" );
   SetToActiveImage_Button.OnClick( (Button::click_event_handler)&AutoHistogramInterface::__Click, w );

   StretchParameters_Sizer.SetSpacing( 4 );
   StretchParameters_Sizer.SetMargin( 4 );
   StretchParameters_Sizer.Add( StretchMethod_Sizer );
   StretchParameters_Sizer.Add( StretchMode_Sizer );
   StretchParameters_Sizer.Add( TargetMedian_Sizer );
   StretchParameters_Sizer.Add( CaptureReadouts_Sizer );
   StretchParameters_Sizer.Add( SetToActiveImage_Button );

   StretchParameters_Control.SetSizer( StretchParameters_Sizer );

   //

   Global_Sizer.SetMargin( 6 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( ClipParameters_SectionBar );
   Global_Sizer.Add( ClipParameters_Control );
   Global_Sizer.Add( StretchParameters_SectionBar );
   Global_Sizer.Add( StretchParameters_Control );

   w.SetSizer( Global_Sizer );

   ClipParameters_Control.Hide();

   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF AutoHistogramInterface.cpp - Released 2016/02/21 20:22:43 UTC
