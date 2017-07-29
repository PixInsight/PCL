//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.03.0823
// ----------------------------------------------------------------------------
// Standard RestorationFilters Process Module Version 01.00.05.0290
// ----------------------------------------------------------------------------
// RestorationFilterInterface.cpp - Released 2017-05-02T09:43:01Z
// ----------------------------------------------------------------------------
// This file is part of the standard RestorationFilters PixInsight module.
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

#include "RestorationFilterInterface.h"
#include "RestorationFilterProcess.h"

#include <pcl/ErrorHandler.h>
#include <pcl/Graphics.h>
#include <pcl/ViewSelectionDialog.h>

namespace pcl
{

// ----------------------------------------------------------------------------

RestorationFilterInterface* TheRestorationFilterInterface = 0;

// ----------------------------------------------------------------------------

#include "RestorationFilterIcon.xpm"

// ----------------------------------------------------------------------------

// Target ComboBox items
enum { ToCIEL, ToCIEY, ToRGB };

// ----------------------------------------------------------------------------

RestorationFilterInterface::RestorationFilterInterface() :
ProcessInterface(), instance( TheRestorationFilterProcess ), GUI( 0 )
{
   TheRestorationFilterInterface = this;
}

RestorationFilterInterface::~RestorationFilterInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString RestorationFilterInterface::Id() const
{
   return "RestorationFilter";
}

MetaProcess* RestorationFilterInterface::Process() const
{
   return TheRestorationFilterProcess;
}

const char** RestorationFilterInterface::IconImageXPM() const
{
   return RestorationFilterIcon_XPM;
}

void RestorationFilterInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void RestorationFilterInterface::ResetInstance()
{
   RestorationFilterInstance defaultInstance( TheRestorationFilterProcess );
   ImportProcess( defaultInstance );
}

bool RestorationFilterInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "RestorationFilter" );
      ImportProcess( instance );
   }

   dynamic = false;
   return &P == TheRestorationFilterProcess;
}

ProcessImplementation* RestorationFilterInterface::NewProcess() const
{
   return new RestorationFilterInstance( instance );
}

bool RestorationFilterInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const RestorationFilterInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a RestorationFilter instance.";
   return false;
}

bool RestorationFilterInterface::RequiresInstanceValidation() const
{
   return true;
}

bool RestorationFilterInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void RestorationFilterInterface::UpdateControls()
{
   UpdatePSFControls();
   UpdateNoiseControls();
   UpdateFilterControls();
   UpdateDeringingControls();
   UpdateRangeControls();
}

void RestorationFilterInterface::UpdatePSFControls()
{
   GUI->PSFMode_TabBox.SelectPage( int( instance.psfMode ) );

   GUI->ParametricSigma_NumericControl.SetValue( instance.psfSigma );
   GUI->ParametricShape_NumericControl.SetValue( instance.psfShape );
   GUI->ParametricRatio_NumericControl.SetValue( instance.psfAspectRatio );
   GUI->ParametricAngle_NumericControl.SetValue( instance.psfRotationAngle );
   GUI->ParametricAngle_NumericControl.Enable( instance.psfAspectRatio < 1 );

   GUI->MotionLength_NumericControl.SetValue( instance.psfMotionLength );
   GUI->MotionAngle_NumericControl.SetValue( instance.psfMotionRotationAngle );

   GUI->PSFImage_Edit.SetText( instance.psfViewId );

   GUI->PSFDraw_Control.Update();
}

void RestorationFilterInterface::UpdateNoiseControls()
{
   double noise;
   switch ( instance.algorithm )
   {
   case RFAlgorithm::Wiener:
      noise = instance.K;
      GUI->Noise_NumericControl.label.SetText( "K:" );
      break;
   default:
   case RFAlgorithm::ConstrainedLeastSquares:
      noise = instance.gamma;
      GUI->Noise_NumericControl.label.SetText( L"\x3b3:" );
      break;
   }

   GUI->Noise_NumericControl.SetValue( noise );

   int d = int( Floor( Log( noise ) ) );
   GUI->NoiseCoarse_SpinBox.SetValue( -d );
   GUI->NoiseFine_SpinBox.SetValue( RoundI( noise * Pow10I<double>()( -d ) ) );
}

void RestorationFilterInterface::UpdateFilterControls()
{
   GUI->Algorithm_ComboBox.SetCurrentItem( instance.algorithm );

   GUI->Amount_NumericControl.SetValue( instance.amount );

   GUI->Target_ComboBox.SetCurrentItem( instance.toLuminance ? (instance.linear ? ToCIEY : ToCIEL) : ToRGB );
}

void RestorationFilterInterface::UpdateDeringingControls()
{
   GUI->Deringing_SectionBar.SetChecked( instance.deringing );

   GUI->DeringingDark_NumericControl.SetValue( instance.deringingDark );
   GUI->DeringingBright_NumericControl.SetValue( instance.deringingBright );

   GUI->OutputDeringingMaps_CheckBox.SetChecked( instance.outputDeringingMaps );
}

void RestorationFilterInterface::UpdateRangeControls()
{
   GUI->RangeLow_NumericControl.SetValue( instance.rangeLow );
   GUI->RangeHigh_NumericControl.SetValue( instance.rangeHigh );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void RestorationFilterInterface::__PSF_PageSelected( TabBox& sender, int pageIndex )
{
   instance.psfMode = pageIndex;
   UpdateControls();
}

void RestorationFilterInterface::__PSF_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->ParametricSigma_NumericControl )
      instance.psfSigma = Max( TheRFPSFParametricSigmaParameter->MinimumValue(), value );
   else if ( sender == GUI->ParametricShape_NumericControl )
      instance.psfShape = Max( TheRFPSFParametricShapeParameter->MinimumValue(), value );
   else if ( sender == GUI->ParametricRatio_NumericControl )
      instance.psfAspectRatio = value;
   else if ( sender == GUI->ParametricAngle_NumericControl )
      instance.psfRotationAngle = value;
   else if ( sender == GUI->MotionLength_NumericControl )
      instance.psfMotionLength = Max( TheRFPSFMotionLengthParameter->MinimumValue(), value );
   else if ( sender == GUI->MotionAngle_NumericControl )
      instance.psfMotionRotationAngle = value;

   UpdateControls();
}

void RestorationFilterInterface::__PSF_EditCompleted( Edit& sender )
{
   try
   {
      String id = sender.Text();
      id.Trim();
      if ( !id.IsEmpty() && !View::IsValidViewId( id ) )
         throw Error( "Invalid view identifier: " + id );
      instance.psfViewId = id;
      UpdateControls();
   }
   ERROR_CLEANUP(
      sender.SetText( instance.psfViewId );
      sender.SelectAll();
      sender.Focus()
   )
}

void RestorationFilterInterface::__PSF_Click( Button& sender, bool checked )
{
   if ( sender == GUI->PSFImage_Button )
   {
      ViewSelectionDialog d( instance.psfViewId );
      d.SetWindowTitle( "Select PSF Image" );
      if ( d.Execute() )
         instance.psfViewId = d.Id();
      UpdateControls();
   }
}

void RestorationFilterInterface::__PSF_Paint( Control& sender, const Rect& updateRect )
{
   Graphics g( sender );
   Image f;
   if ( instance.CreatePSF( f ) )
   {
      int z = (sender.Width() >= f.Width()) ? sender.Width()/f.Width() : -f.Width()/sender.Width();
      g.DisableSmoothInterpolation();
      g.DrawScaledBitmap( sender.BoundsRect(), Bitmap::Render( ImageVariant( &f ), z ) );
      GUI->PSFDraw_Label.SetText( String().Format( "%d x %d", f.Width(), f.Height() ) );
   }
   else
   {
      g.FillRect( sender.BoundsRect(), 0xff000000 );
      GUI->PSFDraw_Label.SetText( "<* Not found *>" );
   }
}

void RestorationFilterInterface::__Noise_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Noise_NumericControl )
   {
      switch ( instance.algorithm )
      {
      case RFAlgorithm::Wiener:
         instance.K = value;
         break;
      default:
      case RFAlgorithm::ConstrainedLeastSquares:
         instance.gamma = value;
         break;
      }

      UpdateNoiseControls();
   }
}

void RestorationFilterInterface::__Noise_SpinValueUpdated( SpinBox& sender, int value )
{
   double* x;
   switch ( instance.algorithm )
   {
   case RFAlgorithm::Wiener:
      x = &instance.K;
      break;
   default:
   case RFAlgorithm::ConstrainedLeastSquares:
      x = &instance.gamma;
      break;
   }

   int d = int( Floor( Log( *x ) ) );

   if ( sender == GUI->NoiseCoarse_SpinBox )
   {
      int u = RoundI( *x * Pow10I<double>()( -d ) );
      *x = u * Pow10I<double>()( -value );
   }
   else if ( sender == GUI->NoiseFine_SpinBox )
      *x = value * Pow10I<double>()( d );

   UpdateNoiseControls();
}

void RestorationFilterInterface::__Filter_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Algorithm_ComboBox )
   {
      instance.algorithm = pcl_enum( itemIndex );
      UpdateNoiseControls();
   }
}

void RestorationFilterInterface::__Filter_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Amount_NumericControl )
      instance.amount = value;
}

void RestorationFilterInterface::__Target_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Target_ComboBox )
   {
      switch ( itemIndex )
      {
      case ToCIEL:
         instance.toLuminance = true;
         instance.linear = false;
         break;
      case ToCIEY:
         instance.toLuminance = true;
         instance.linear = true;
         break;
      case ToRGB:
         instance.toLuminance = false;
         instance.linear = false;
         break;
      }
   }

   UpdateFilterControls();
}

void RestorationFilterInterface::__Deringing_Check( SectionBar& sender, bool checked )
{
   if ( sender == GUI->Deringing_SectionBar )
      instance.deringing = checked;
   UpdateDeringingControls();
}

void RestorationFilterInterface::__Deringing_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->DeringingDark_NumericControl )
      instance.deringingDark = value;
   else if ( sender == GUI->DeringingBright_NumericControl )
      instance.deringingBright = value;
   UpdateDeringingControls();
}

void RestorationFilterInterface::__Deringing_Click( Button& sender, bool checked )
{
   if ( sender == GUI->OutputDeringingMaps_CheckBox )
      instance.outputDeringingMaps = checked;
   UpdateDeringingControls();
}

void RestorationFilterInterface::__Range_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->RangeLow_NumericControl )
      instance.rangeLow = value;
   else if ( sender == GUI->RangeHigh_NumericControl )
      instance.rangeHigh = value;
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

RestorationFilterInterface::GUIData::GUIData( RestorationFilterInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Aspect ratio:" ) );
   int editWidth1 = fnt.Width( String( '0', 7 ) );
   int editWidth2 = fnt.Width( String( '0', 11 ) );
   int editWidth3 = fnt.Width( String( '0', 19 ) );

   //

   PSF_SectionBar.SetTitle( "PSF" );
   PSF_SectionBar.SetSection( PSF_Control );

   ParametricSigma_NumericControl.label.SetText( "StdDev:" );
   ParametricSigma_NumericControl.label.SetFixedWidth( labelWidth1 );
   ParametricSigma_NumericControl.slider.SetScaledMinWidth( 150 );
   ParametricSigma_NumericControl.slider.SetRange( 0, 100 );
   ParametricSigma_NumericControl.SetReal();
   // ### We use a trick to achieve the correct slider stepping (see also the event handler)
   ParametricSigma_NumericControl.SetRange( 0 /*TheRFPSFParametricSigmaParameter->MinimumValue()*/, TheRFPSFParametricSigmaParameter->MaximumValue() );
   ParametricSigma_NumericControl.SetPrecision( TheRFPSFParametricSigmaParameter->Precision() );
   ParametricSigma_NumericControl.edit.SetFixedWidth( editWidth1 );
   ParametricSigma_NumericControl.SetToolTip( "<p>Standard deviation of the parametric PSF function.</p>"
      "By increasing this parameter the PSF will be larger, and hence the deconvolution process will act at "
      "larger dimensional scales.</p>" );
   ParametricSigma_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RestorationFilterInterface::__PSF_ValueUpdated, w );

   ParametricShape_NumericControl.label.SetText( "Shape:" );
   ParametricShape_NumericControl.label.SetFixedWidth( labelWidth1 );
   ParametricShape_NumericControl.slider.SetScaledMinWidth( 150 );
   ParametricShape_NumericControl.slider.SetRange( 0, 120 );
   ParametricShape_NumericControl.SetReal();
   // ### We use a trick to achieve the correct slider stepping (see also the event handler)
   ParametricShape_NumericControl.SetRange( 0 /*TheRFPSFParametricShapeParameter->MinimumValue()*/, TheRFPSFParametricShapeParameter->MaximumValue() );
   ParametricShape_NumericControl.SetPrecision( TheRFPSFParametricShapeParameter->Precision() );
   ParametricShape_NumericControl.edit.SetFixedWidth( editWidth1 );
   ParametricShape_NumericControl.SetToolTip( "<p>Overall shape of the PSF function.</p>"
      "<p>This parameter controls the <i>kurtosis</i> of the PSF function distribution. When shape = 2 the function "
      "distribution is Gaussian (also known as <i>normal distribution</i>). When shape &lt; 2 the distribution is "
      "<i>leptokurtic</i>, leading to a peaked PSF that can be used, for example, to approximate the shape of a "
      "diffraction pattern. When shape &gt; 2 the distribution is <i>platykurtic</i> and the PSF has a comparatively "
      "flat profile.</p>" );
   ParametricShape_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RestorationFilterInterface::__PSF_ValueUpdated, w );

   ParametricRatio_NumericControl.label.SetText( "Aspect ratio:" );
   ParametricRatio_NumericControl.label.SetFixedWidth( labelWidth1 );
   ParametricRatio_NumericControl.slider.SetScaledMinWidth( 150 );
   ParametricRatio_NumericControl.slider.SetRange( 0, 100 );
   ParametricRatio_NumericControl.SetReal();
   ParametricRatio_NumericControl.SetRange( TheRFPSFParametricAspectRatioParameter->MinimumValue(), TheRFPSFParametricAspectRatioParameter->MaximumValue() );
   ParametricRatio_NumericControl.SetPrecision( TheRFPSFParametricAspectRatioParameter->Precision() );
   ParametricRatio_NumericControl.edit.SetFixedWidth( editWidth1 );
   ParametricRatio_NumericControl.SetToolTip( "<p>Aspect ratio of the PSF function (vertical/horizontal axis ratio).</p>" );
   ParametricRatio_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RestorationFilterInterface::__PSF_ValueUpdated, w );

   ParametricAngle_NumericControl.label.SetText( "Rotation:" );
   ParametricAngle_NumericControl.label.SetFixedWidth( labelWidth1 );
   ParametricAngle_NumericControl.slider.SetScaledMinWidth( 150 );
   ParametricAngle_NumericControl.slider.SetRange( 0, 180 );
   ParametricAngle_NumericControl.SetReal();
   ParametricAngle_NumericControl.SetRange( TheRFPSFParametricRotationAngleParameter->MinimumValue(), TheRFPSFParametricRotationAngleParameter->MaximumValue() );
   ParametricAngle_NumericControl.SetPrecision( TheRFPSFParametricRotationAngleParameter->Precision() );
   ParametricAngle_NumericControl.edit.SetFixedWidth( editWidth1 );
   ParametricAngle_NumericControl.SetToolTip( "<p>Rotation angle of the distorted PSF function in degrees.</p>" );
   ParametricAngle_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RestorationFilterInterface::__PSF_ValueUpdated, w );

   Parametric_Sizer.SetSpacing( 4 );
   Parametric_Sizer.SetMargin( 6 );
   Parametric_Sizer.Add( ParametricSigma_NumericControl );
   Parametric_Sizer.Add( ParametricShape_NumericControl );
   Parametric_Sizer.Add( ParametricRatio_NumericControl );
   Parametric_Sizer.Add( ParametricAngle_NumericControl );

   Parametric_Control.SetSizer( Parametric_Sizer );

   MotionLength_NumericControl.label.SetText( "Length:" );
   MotionLength_NumericControl.label.SetFixedWidth( labelWidth1 );
   MotionLength_NumericControl.slider.SetScaledMinWidth( 150 );
   MotionLength_NumericControl.slider.SetRange( 0, 100 );
   MotionLength_NumericControl.SetReal();
   // ### We use a trick to achieve the correct slider stepping (see also the event handler)
   MotionLength_NumericControl.SetRange( 0 /*TheRFPSFMotionLengthParameter->MinimumValue()*/, TheRFPSFMotionLengthParameter->MaximumValue() );
   MotionLength_NumericControl.SetPrecision( TheRFPSFMotionLengthParameter->Precision() );
   MotionLength_NumericControl.edit.SetFixedWidth( editWidth1 );
   MotionLength_NumericControl.SetToolTip( "<p>Length of the motion length PSF in pixels.</p>" );
   MotionLength_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RestorationFilterInterface::__PSF_ValueUpdated, w );

   MotionAngle_NumericControl.label.SetText( "Angle:" );
   MotionAngle_NumericControl.label.SetFixedWidth( labelWidth1 );
   MotionAngle_NumericControl.slider.SetScaledMinWidth( 150 );
   MotionAngle_NumericControl.slider.SetRange( 0, 180 );
   MotionAngle_NumericControl.SetReal();
   MotionAngle_NumericControl.SetRange( TheRFPSFMotionRotationAngleParameter->MinimumValue(), TheRFPSFMotionRotationAngleParameter->MaximumValue() );
   MotionAngle_NumericControl.SetPrecision( TheRFPSFMotionRotationAngleParameter->Precision() );
   MotionAngle_NumericControl.edit.SetFixedWidth( editWidth1 );
   MotionAngle_NumericControl.SetToolTip( "<p>Rotation angle of motion length PSF in degrees.</p>" );
   MotionAngle_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RestorationFilterInterface::__PSF_ValueUpdated, w );

   Motion_Sizer.SetSpacing( 4 );
   Motion_Sizer.SetMargin( 6 );
   Motion_Sizer.AddStretch();
   Motion_Sizer.Add( MotionLength_NumericControl );
   Motion_Sizer.Add( MotionAngle_NumericControl );
   Motion_Sizer.AddStretch();

   Motion_Control.SetSizer( Motion_Sizer );

   PSFImage_Label.SetText( "View Identifier" );

   PSFImage_Edit.SetToolTip( "<p>Identifier of the external PSF view.</p>" );
   PSFImage_Edit.OnEditCompleted( (Edit::edit_event_handler)&RestorationFilterInterface::__PSF_EditCompleted, w );

   PSFImage_Button.SetIcon( Bitmap( w.ScaledResource( ":/icons/select-view.png" ) ) );
   PSFImage_Button.SetScaledFixedSize( 20, 20 );
   PSFImage_Button.SetToolTip( "<p>Select view.</p>" );
   PSFImage_Button.OnClick( (Button::click_event_handler)&RestorationFilterInterface::__PSF_Click, w );

   PSFImageEdit_Sizer.SetSpacing( 4 );
   PSFImageEdit_Sizer.Add( PSFImage_Edit, 100 );
   PSFImageEdit_Sizer.Add( PSFImage_Button );

   PSFImage_Sizer.SetSpacing( 4 );
   PSFImage_Sizer.SetMargin( 6 );
   PSFImage_Sizer.AddStretch();
   PSFImage_Sizer.Add( PSFImage_Label );
   PSFImage_Sizer.Add( PSFImageEdit_Sizer );
   PSFImage_Sizer.AddStretch();

   PSFImage_Control.SetSizer( PSFImage_Sizer );

   PSFMode_TabBox.AddPage( Parametric_Control, "Parametric PSF" );
   PSFMode_TabBox.AddPage( Motion_Control, "Motion Blur PSF" );
   PSFMode_TabBox.AddPage( PSFImage_Control, "External PSF" );
   PSFMode_TabBox.SetScaledMinWidth( 350 );
   PSFMode_TabBox.OnPageSelected( (TabBox::page_event_handler)&RestorationFilterInterface::__PSF_PageSelected, w );

   PSFDraw_Control.SetScaledFixedSize( 100, 100 );
   PSFDraw_Control.OnPaint( (Control::paint_event_handler)&RestorationFilterInterface::__PSF_Paint, w );

   PSFDraw_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   PSFDraw_Sizer.SetSpacing( 4 );
   PSFDraw_Sizer.AddStretch();
   PSFDraw_Sizer.Add( PSFDraw_Control );
   PSFDraw_Sizer.Add( PSFDraw_Label );

   PSF_Sizer.SetSpacing( 4 );
   PSF_Sizer.Add( PSFMode_TabBox );
   PSF_Sizer.Add( PSFDraw_Sizer );

   PSF_Control.SetSizer( PSF_Sizer );

   //

   Noise_SectionBar.SetTitle( "Noise Estimation" );
   Noise_SectionBar.SetSection( Noise_Control );

   Noise_NumericControl.label.SetText( "K:" );
   Noise_NumericControl.label.SetFixedWidth( labelWidth1 + w.LogicalPixelsToPhysical( 6+1 ) );
   Noise_NumericControl.SetReal();
   Noise_NumericControl.SetRange( TheRFWienerKParameter->MinimumValue(), TheRFWienerKParameter->MaximumValue() );
   Noise_NumericControl.SetPrecision( TheRFWienerKParameter->Precision() );
   Noise_NumericControl.edit.SetFixedWidth( editWidth3 );
   Noise_NumericControl.SetToolTip( "<p>Noise estimate: decrease it to increase the filtering strength.</p>" );
   Noise_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RestorationFilterInterface::__Noise_ValueUpdated, w );
   Noise_NumericControl.sizer.AddStretch();

   NoiseFine_SpinBox.SetRange( 1, 9 );
   NoiseFine_SpinBox.SetToolTip( "<p>Noise estimate, fine adjustment</p>" );
   NoiseFine_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&RestorationFilterInterface::__Noise_SpinValueUpdated, w );

   NoiseCoarse_Label.SetText( L" \xd7 10^\x2212" );
   NoiseCoarse_Label.SetToolTip( "<p>Noise estimate, coarse adjustment</p>" );
   NoiseCoarse_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   NoiseCoarse_SpinBox.SetRange( 0, 15 );
   NoiseCoarse_SpinBox.SetToolTip( "<p>Noise estimate, coarse adjustment</p>" );
   NoiseCoarse_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&RestorationFilterInterface::__Noise_SpinValueUpdated, w );

   Noise_Sizer.SetSpacing( 4 );
   Noise_Sizer.Add( Noise_NumericControl );
   Noise_Sizer.AddSpacing( 24 );
   Noise_Sizer.Add( NoiseFine_SpinBox );
   Noise_Sizer.Add( NoiseCoarse_Label );
   Noise_Sizer.Add( NoiseCoarse_SpinBox );
   Noise_Sizer.AddStretch();

   Noise_Control.SetSizer( Noise_Sizer );

   //

   Filter_SectionBar.SetTitle( "Filter Parameters" );
   Filter_SectionBar.SetSection( Filter_Control );

   Algorithm_Label.SetText( "Algorithm:" );
   Algorithm_Label.SetFixedWidth( labelWidth1 + w.LogicalPixelsToPhysical( 6+1 ) );
   Algorithm_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Algorithm_ComboBox.AddItem( "Wiener Filtering" );
   Algorithm_ComboBox.AddItem( "Constrained Least Squares Filtering" );
   Algorithm_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&RestorationFilterInterface::__Filter_ItemSelected, w );

   Algorithm_Sizer.SetSpacing( 4 );
   Algorithm_Sizer.Add( Algorithm_Label );
   Algorithm_Sizer.Add( Algorithm_ComboBox, 100 );

   Amount_NumericControl.label.SetText( "Amount:" );
   Amount_NumericControl.label.SetFixedWidth( labelWidth1 + w.LogicalPixelsToPhysical( 6+1 ) );
   Amount_NumericControl.slider.SetScaledMinWidth( 200 );
   Amount_NumericControl.slider.SetRange( 0, 100 );
   Amount_NumericControl.SetReal();
   Amount_NumericControl.SetRange( TheRFAmountParameter->MinimumValue(), TheRFAmountParameter->MaximumValue() );
   Amount_NumericControl.SetPrecision( TheRFAmountParameter->Precision() );
   Amount_NumericControl.edit.SetFixedWidth( editWidth2 );
   Amount_NumericControl.SetToolTip( "<p>Filtering amount.</p>" );
   Amount_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RestorationFilterInterface::__Filter_ValueUpdated, w );

   Target_Label.SetText( "Target:" );
   Target_Label.SetFixedWidth( labelWidth1 + w.LogicalPixelsToPhysical( 6+1 ) );
   Target_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );

   Target_ComboBox.AddItem( "Lightness (CIE L*)" );
   Target_ComboBox.AddItem( "Luminance (CIE Y)" );
   Target_ComboBox.AddItem( "RGB/K components" );
   Target_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&RestorationFilterInterface::__Target_ItemSelected, w );

   Target_Sizer.SetSpacing( 4 );
   Target_Sizer.Add( Target_Label );
   Target_Sizer.Add( Target_ComboBox );
   Target_Sizer.AddStretch();

   Filter_Sizer.SetSpacing( 4 );
   Filter_Sizer.Add( Algorithm_Sizer );
   Filter_Sizer.Add( Amount_NumericControl );
   Filter_Sizer.Add( Target_Sizer );

   Filter_Control.SetSizer( Filter_Sizer );

   //

   Deringing_SectionBar.SetTitle( "Deringing" );
   Deringing_SectionBar.EnableTitleCheckBox();
   Deringing_SectionBar.OnCheck( (SectionBar::check_event_handler)&RestorationFilterInterface::__Deringing_Check, w );
   Deringing_SectionBar.SetSection( Deringing_Control );

   //

   DeringingDark_NumericControl.label.SetText( "Dark:" );
   DeringingDark_NumericControl.label.SetMinWidth( labelWidth1 + w.LogicalPixelsToPhysical( 6+1 ) );
   DeringingDark_NumericControl.slider.SetRange( 0, 100 );
   DeringingDark_NumericControl.SetReal();
   DeringingDark_NumericControl.SetRange( TheRFDeringingDarkParameter->MinimumValue(), TheRFDeringingDarkParameter->MaximumValue() );
   DeringingDark_NumericControl.SetPrecision( TheRFDeringingDarkParameter->Precision() );
   DeringingDark_NumericControl.SetToolTip( "<p>Deringing regularization strength, dark ringing artifacts.</p>" );
   DeringingDark_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RestorationFilterInterface::__Deringing_ValueUpdated, w );

   DeringingBright_NumericControl.label.SetText( "Bright:" );
   DeringingBright_NumericControl.label.SetMinWidth( labelWidth1 + w.LogicalPixelsToPhysical( 6+1 ) );
   DeringingBright_NumericControl.slider.SetRange( 0, 100 );
   DeringingBright_NumericControl.SetReal();
   DeringingBright_NumericControl.SetRange( TheRFDeringingBrightParameter->MinimumValue(), TheRFDeringingBrightParameter->MaximumValue() );
   DeringingBright_NumericControl.SetPrecision( TheRFDeringingBrightParameter->Precision() );
   DeringingBright_NumericControl.SetToolTip( "<p>Deringing regularization strength, bright ringing artifacts.</p>" );
   DeringingBright_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RestorationFilterInterface::__Deringing_ValueUpdated, w );

   OutputDeringingMaps_CheckBox.SetText( "Output deringing maps" );
   OutputDeringingMaps_CheckBox.SetToolTip( "<p>Generate an image window for each deringing map image. "
      "New image windows will be created for the dark and bright deringing maps, if the corresponding amount "
      "parameters are nonzero.</p>" );
   OutputDeringingMaps_CheckBox.OnClick( (Button::click_event_handler)&RestorationFilterInterface::__Deringing_Click, w );

   OutputDeringingMaps_Sizer.AddUnscaledSpacing( labelWidth1 + w.LogicalPixelsToPhysical( 6+1 + 4 ) );
   OutputDeringingMaps_Sizer.Add( OutputDeringingMaps_CheckBox );
   OutputDeringingMaps_Sizer.AddStretch();

   Deringing_Sizer.SetSpacing( 4 );
   Deringing_Sizer.Add( DeringingDark_NumericControl );
   Deringing_Sizer.Add( DeringingBright_NumericControl );
   Deringing_Sizer.Add( OutputDeringingMaps_Sizer );

   Deringing_Control.SetSizer( Deringing_Sizer );

   //

   Range_SectionBar.SetTitle( "Dynamic Range Extension" );
   Range_SectionBar.SetSection( Range_Control );

   RangeLow_NumericControl.label.SetText( "Low range:" );
   RangeLow_NumericControl.label.SetFixedWidth( labelWidth1 + w.LogicalPixelsToPhysical( 6+1 ) );
   RangeLow_NumericControl.slider.SetScaledMinWidth( 200 );
   RangeLow_NumericControl.slider.SetRange( 0, 100 );
   RangeLow_NumericControl.SetReal();
   RangeLow_NumericControl.SetRange( TheRFRangeLowParameter->MinimumValue(), TheRFRangeLowParameter->MaximumValue() );
   RangeLow_NumericControl.SetPrecision( TheRFRangeLowParameter->Precision() );
   RangeLow_NumericControl.SetToolTip( "<p>Shadows dynamic range extension.</p>" );
   RangeLow_NumericControl.edit.SetFixedWidth( editWidth2 );
   RangeLow_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RestorationFilterInterface::__Range_ValueUpdated, w );

   RangeHigh_NumericControl.label.SetText( "High range:" );
   RangeHigh_NumericControl.label.SetFixedWidth( labelWidth1 + w.LogicalPixelsToPhysical( 6+1 ) );
   RangeHigh_NumericControl.slider.SetScaledMinWidth( 200 );
   RangeHigh_NumericControl.slider.SetRange( 0, 100 );
   RangeHigh_NumericControl.SetReal();
   RangeHigh_NumericControl.SetRange( TheRFRangeHighParameter->MinimumValue(), TheRFRangeHighParameter->MaximumValue() );
   RangeHigh_NumericControl.SetPrecision( TheRFRangeHighParameter->Precision() );
   RangeHigh_NumericControl.SetToolTip( "<p>Highlights dynamic range extension.</p>" );
   RangeHigh_NumericControl.edit.SetFixedWidth( editWidth2 );
   RangeHigh_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RestorationFilterInterface::__Range_ValueUpdated, w );

   Range_Sizer.SetSpacing( 8 );
   Range_Sizer.Add( RangeLow_NumericControl );
   Range_Sizer.Add( RangeHigh_NumericControl );

   Range_Control.SetSizer( Range_Sizer );

   //

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( PSF_SectionBar );
   Global_Sizer.Add( PSF_Control );
   Global_Sizer.Add( Noise_SectionBar );
   Global_Sizer.Add( Noise_Control );
   Global_Sizer.Add( Filter_SectionBar );
   Global_Sizer.Add( Filter_Control );
   Global_Sizer.Add( Deringing_SectionBar );
   Global_Sizer.Add( Deringing_Control );
   Global_Sizer.Add( Range_SectionBar );
   Global_Sizer.Add( Range_Control );

   w.SetSizer( Global_Sizer );

   w.AdjustToContents();
   w.SetFixedWidth();

   Deringing_Control.Hide();
   Range_Control.Hide();

   w.AdjustToContents();
   w.SetFixedHeight();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RestorationFilterInterface.cpp - Released 2017-05-02T09:43:01Z
