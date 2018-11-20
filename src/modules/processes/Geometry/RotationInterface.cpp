//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.10.0915
// ----------------------------------------------------------------------------
// Standard Geometry Process Module Version 01.02.02.0387
// ----------------------------------------------------------------------------
// RotationInterface.cpp - Released 2018-11-01T11:07:20Z
// ----------------------------------------------------------------------------
// This file is part of the standard Geometry PixInsight module.
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

#include "RotationInterface.h"
#include "RotationParameters.h"
#include "RotationProcess.h"

#include <pcl/Graphics.h>

namespace pcl
{

// ----------------------------------------------------------------------------

RotationInterface* TheRotationInterface = nullptr;

// ----------------------------------------------------------------------------

#include "RotationIcon.xpm"

// ----------------------------------------------------------------------------

RotationInterface::RotationInterface() :
   instance( TheRotationProcess )
{
   TheRotationInterface = this;
}

RotationInterface::~RotationInterface()
{
   if ( GUI != nullptr )
      delete GUI, GUI = nullptr;
}

IsoString RotationInterface::Id() const
{
   return "Rotation";
}

MetaProcess* RotationInterface::Process() const
{
   return TheRotationProcess;
}

const char** RotationInterface::IconImageXPM() const
{
   return RotationIcon_XPM;
}

void RotationInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentWindow();
}

void RotationInterface::ResetInstance()
{
   RotationInstance defaultInstance( TheRotationProcess );
   ImportProcess( defaultInstance );
}

bool RotationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == nullptr )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "Rotation" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheRotationProcess;
}

ProcessImplementation* RotationInterface::NewProcess() const
{
   return new RotationInstance( instance );
}

bool RotationInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const RotationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a Rotation instance.";
   return false;
}

bool RotationInterface::RequiresInstanceValidation() const
{
   return true;
}

bool RotationInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

bool RotationInterface::WantsReadoutNotifications() const
{
   return true;
}

void RotationInterface::UpdateReadout( const View&, const pcl::DPoint&, double R, double G, double B, double /*A*/ )
{
   if ( GUI != nullptr )
      if ( IsVisible() )
         if ( GUI->FillColor_SectionBar.Section().IsVisible() )
         {
            instance.p_fillColor[0] = R;
            instance.p_fillColor[1] = G;
            instance.p_fillColor[2] = B;
            UpdateFillColorControls();
         }
}

// ----------------------------------------------------------------------------

void RotationInterface::UpdateControls()
{
   UpdateNumericControls();
   UpdateFillColorControls();
}

void RotationInterface::UpdateNumericControls()
{
   GUI->Angle_NumericEdit.SetValue( Abs( Deg( instance.p_angle ) ) );

   GUI->Clockwise_CheckBox.SetChecked( instance.p_angle < 0 );

   GUI->Dial_Control.Update();

   GUI->OptimizeFast_CheckBox.SetChecked( instance.p_optimizeFast );

   GUI->Algorithm_ComboBox.SetCurrentItem( instance.p_interpolation );

   GUI->ClampingThreshold_NumericEdit.SetValue( instance.p_clampingThreshold );
   GUI->ClampingThreshold_NumericEdit.Enable( InterpolationAlgorithm::IsClampedInterpolation( instance.p_interpolation ) );

   GUI->Smoothness_NumericEdit.SetValue( instance.p_smoothness );
   GUI->Smoothness_NumericEdit.Enable( InterpolationAlgorithm::IsCubicFilterInterpolation( instance.p_interpolation ) );
}

void RotationInterface::UpdateFillColorControls()
{
   GUI->Red_NumericControl.SetValue( instance.p_fillColor[0] );
   GUI->Green_NumericControl.SetValue( instance.p_fillColor[1] );
   GUI->Blue_NumericControl.SetValue( instance.p_fillColor[2] );
   GUI->Alpha_NumericControl.SetValue( instance.p_fillColor[3] );
   GUI->ColorSample_Control.Update();
}

// ----------------------------------------------------------------------------

void RotationInterface::__Angle_ValueUpdated( NumericEdit& sender, double value )
{
   double a = Rad( value );
   if ( GUI->Clockwise_CheckBox.IsChecked() )
      a = -a;
   instance.p_angle = ArcTan( Sin( a ), Cos( a ) );

   UpdateNumericControls();
}

void RotationInterface::__Clockwise_Click( Button& sender, bool checked )
{
   if ( Round( Abs( Deg( instance.p_angle ) ), 3 ) < 180 )
   {
      instance.p_angle = -instance.p_angle;
      UpdateNumericControls();
   }
   else
      GUI->Clockwise_CheckBox.Uncheck();
}

void RotationInterface::__OptimizeFast_Click( Button& sender, bool checked )
{
   instance.p_optimizeFast = checked;
}

void RotationInterface::__AngleDial_Paint( Control& sender, const Rect& updateRect )
{
   Rect r( sender.BoundsRect() );

   int w = r.Width();
   int h = r.Height();
   double x0 = w/2.0;
   double y0 = h/2.0;
   double f = sender.DisplayPixelRatio();

   VectorGraphics g( sender );
   if ( f > 1 )
      g.EnableAntialiasing();

   g.FillRect( r, 0xff000000 );

   g.SetBrush( Brush::Null() );
   g.SetPen( 0xff7f7f7f, f );
   g.DrawLine( x0, 0, x0, h );
   g.DrawLine( 0, y0, w, y0 );

   g.EnableAntialiasing();
   g.DrawEllipse( r );

   double sa, ca;
   SinCos( instance.p_angle, sa, ca );
   double x1 = x0 + 0.5*w*ca;
   double y1 = y0 - 0.5*h*sa;

   g.SetPen( 0xffffffff, f );
   g.SetBrush( 0xffffffff );
   g.DrawLine( x0, y0, x1, y1 );
   double d3 = f*3;
   g.DrawRect( x1-d3, y1-d3, x1+d3, y1+d3 );
}

void RotationInterface::__AngleDial_MouseMove( Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers )
{
   if ( dragging )
   {
      double a = Round( Deg( ArcTan( sender.ClientHeight()/2.0 - pos.y,
                                     pos.x - sender.ClientWidth()/2.0 ) ), 3 );
      instance.p_angle = Rad( a );
      UpdateNumericControls();
   }
}

void RotationInterface::__AngleDial_MousePress( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( button != MouseButton::Left )
      return;

   dragging = true;
   __AngleDial_MouseMove( sender, pos, buttons, modifiers );
}

void RotationInterface::__AngleDial_MouseRelease( Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   dragging = false;
}

void RotationInterface::__Algorithm_ItemSelected( ComboBox& sender, int itemIndex )
{
   if ( sender == GUI->Algorithm_ComboBox )
   {
      instance.p_interpolation = itemIndex;
      GUI->ClampingThreshold_NumericEdit.Enable( InterpolationAlgorithm::IsClampedInterpolation( instance.p_interpolation ) );
      GUI->Smoothness_NumericEdit.Enable( InterpolationAlgorithm::IsCubicFilterInterpolation( instance.p_interpolation ) );
   }
}

void RotationInterface::__Algorithm_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->ClampingThreshold_NumericEdit )
      instance.p_clampingThreshold = value;
   else if ( sender == GUI->Smoothness_NumericEdit )
      instance.p_smoothness = value;
}

void RotationInterface::__FilColor_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Red_NumericControl )
      instance.p_fillColor[0] = value;
   else if ( sender == GUI->Green_NumericControl )
      instance.p_fillColor[1] = value;
   else if ( sender == GUI->Blue_NumericControl )
      instance.p_fillColor[2] = value;
   else if ( sender == GUI->Alpha_NumericControl )
      instance.p_fillColor[3] = value;

   GUI->ColorSample_Control.Update();
}

void RotationInterface::__ColorSample_Paint( Control& sender, const Rect& updateRect )
{
   Graphics g( sender );

   RGBA color = RGBAColor( float( instance.p_fillColor[0] ),
                           float( instance.p_fillColor[1] ),
                           float( instance.p_fillColor[2] ),
                           float( instance.p_fillColor[3] ) );

   if ( Alpha( color ) != 0 )
   {
      g.SetBrush( Bitmap( sender.ScaledResource( ":/image-window/transparent-small.png" ) ) );
      g.SetPen( Pen::Null() );
      g.DrawRect( sender.BoundsRect() );
   }

   g.SetBrush( color );
   g.SetPen( 0xff000000, sender.DisplayPixelRatio() );
   g.DrawRect( sender.BoundsRect() );
}

// ----------------------------------------------------------------------------

RotationInterface::GUIData::GUIData( RotationInterface& w )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Clamping threshold:" ) + 'M' );
   int labelWidth2 = fnt.Width( String( 'M',  2 ) );

   // -------------------------------------------------------------------------

   Rotation_SectionBar.SetTitle( "Rotation" );
   Rotation_SectionBar.SetSection( Rotation_Control );

   Angle_NumericEdit.SetReal();
   Angle_NumericEdit.SetPrecision( 3 );
   Angle_NumericEdit.SetRange( 0, 180 );
   Angle_NumericEdit.label.SetText( "Angle (\xb0):" );
   Angle_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Angle_NumericEdit.sizer.AddStretch();
   Angle_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&RotationInterface::__Angle_ValueUpdated, w );

   Clockwise_Label.SetText( "Clockwise:" );
   Clockwise_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Clockwise_Label.SetFixedWidth( labelWidth1 );

   Clockwise_CheckBox.OnClick( (Button::click_event_handler)&RotationInterface::__Clockwise_Click, w );

   Clockwise_Sizer.SetSpacing( 4 );
   Clockwise_Sizer.Add( Clockwise_Label );
   Clockwise_Sizer.Add( Clockwise_CheckBox );
   Clockwise_Sizer.AddStretch();

   OptimizeFast_Label.SetText( "Use fast rotations:" );
   OptimizeFast_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   OptimizeFast_Label.SetFixedWidth( labelWidth1 );

   OptimizeFast_CheckBox.OnClick( (Button::click_event_handler)&RotationInterface::__OptimizeFast_Click, w );

   OptimizeFast_Sizer.SetSpacing( 4 );
   OptimizeFast_Sizer.Add( OptimizeFast_Label );
   OptimizeFast_Sizer.Add( OptimizeFast_CheckBox );
   OptimizeFast_Sizer.AddStretch();

   RotationLeft_Sizer.SetSpacing( 4 );
   RotationLeft_Sizer.Add( Angle_NumericEdit );
   RotationLeft_Sizer.Add( Clockwise_Sizer );
   RotationLeft_Sizer.AddStretch();
   RotationLeft_Sizer.Add( OptimizeFast_Sizer );

   Dial_Control.SetBackgroundColor( StringToRGBAColor( "black" ) );
   Dial_Control.SetScaledFixedSize( 80, 80 );
   Dial_Control.OnPaint( (Control::paint_event_handler)&RotationInterface::__AngleDial_Paint, w );
   Dial_Control.OnMousePress( (Control::mouse_button_event_handler)&RotationInterface::__AngleDial_MousePress, w );
   Dial_Control.OnMouseRelease( (Control::mouse_button_event_handler)&RotationInterface::__AngleDial_MouseRelease, w );
   Dial_Control.OnMouseMove( (Control::mouse_event_handler)&RotationInterface::__AngleDial_MouseMove, w );

   Rotation_Sizer.SetSpacing( 6 );
   Rotation_Sizer.Add( RotationLeft_Sizer );
   Rotation_Sizer.Add( Dial_Control );

   Rotation_Control.SetSizer( Rotation_Sizer );
   //Rotation_Control.AdjustToContents();
   //Rotation_Control.SetFixedSize();

   // -------------------------------------------------------------------------

   Interpolation_SectionBar.SetTitle( "Interpolation" );
   Interpolation_SectionBar.SetSection( Interpolation_Control );

   Algorithm_Label.SetText( "Algorithm:" );
   Algorithm_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   Algorithm_Label.SetFixedWidth( labelWidth1 );

   Algorithm_ComboBox.AddItem( "Nearest Neighbor" );
   Algorithm_ComboBox.AddItem( "Bilinear" );
   Algorithm_ComboBox.AddItem( "Bicubic Spline" );
   Algorithm_ComboBox.AddItem( "Bicubic B-Spline" );
   Algorithm_ComboBox.AddItem( "Lanczos-3" );
   Algorithm_ComboBox.AddItem( "Lanczos-4" );
   Algorithm_ComboBox.AddItem( "Mitchell-Netravali" );
   Algorithm_ComboBox.AddItem( "Catmull-Rom Spline" );
   Algorithm_ComboBox.AddItem( "Cubic B-Spline" );
   Algorithm_ComboBox.AddItem( "Auto" );
   Algorithm_ComboBox.SetMaxVisibleItemCount( 16 );
   Algorithm_ComboBox.OnItemSelected( (ComboBox::item_event_handler)&RotationInterface::__Algorithm_ItemSelected, w );

   Algorithm_Sizer.SetSpacing( 4 );
   Algorithm_Sizer.Add( Algorithm_Label );
   Algorithm_Sizer.Add( Algorithm_ComboBox, 100 );

   ClampingThreshold_NumericEdit.SetReal();
   ClampingThreshold_NumericEdit.SetPrecision( TheRTClampingThresholdParameter->Precision() );
   ClampingThreshold_NumericEdit.SetRange( TheRTClampingThresholdParameter->MinimumValue(),
                                    TheRTClampingThresholdParameter->MaximumValue() );
   ClampingThreshold_NumericEdit.label.SetText( "Clamping threshold:" );
   ClampingThreshold_NumericEdit.label.SetFixedWidth( labelWidth1 );
   ClampingThreshold_NumericEdit.SetToolTip( "<p>Deringing clamping threshold for bicubic spline and Lanczos interpolation algorithms.</p>" );
   ClampingThreshold_NumericEdit.sizer.AddStretch();
   ClampingThreshold_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&RotationInterface::__Algorithm_ValueUpdated, w );

   Smoothness_NumericEdit.SetReal();
   Smoothness_NumericEdit.SetPrecision( TheRTSmoothnessParameter->Precision() );
   Smoothness_NumericEdit.SetRange( TheRTSmoothnessParameter->MinimumValue(),
                                    TheRTSmoothnessParameter->MaximumValue() );
   Smoothness_NumericEdit.label.SetText( "Smoothness:" );
   Smoothness_NumericEdit.label.SetFixedWidth( labelWidth1 );
   Smoothness_NumericEdit.SetToolTip( "<p>Smoothness level for cubic filter interpolation algorithms.</p>" );
   Smoothness_NumericEdit.sizer.AddStretch();
   Smoothness_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&RotationInterface::__Algorithm_ValueUpdated, w );

   Interpolation_Sizer.SetSpacing( 4 );
   Interpolation_Sizer.Add( Algorithm_Sizer );
   Interpolation_Sizer.Add( ClampingThreshold_NumericEdit );
   Interpolation_Sizer.Add( Smoothness_NumericEdit );

   Interpolation_Control.SetSizer( Interpolation_Sizer );

   // -------------------------------------------------------------------------

   FillColor_SectionBar.SetTitle( "Fill Color" );
   FillColor_SectionBar.SetSection( FillColor_Control );

   Red_NumericControl.label.SetText( "R:" );
   Red_NumericControl.label.SetFixedWidth( labelWidth2 );
   Red_NumericControl.slider.SetRange( 0, 100 );
   Red_NumericControl.SetReal();
   Red_NumericControl.SetRange( 0, 1 );
   Red_NumericControl.SetPrecision( 6 );
   Red_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RotationInterface::__FilColor_ValueUpdated, w );

   Green_NumericControl.label.SetText( "G:" );
   Green_NumericControl.label.SetFixedWidth( labelWidth2 );
   Green_NumericControl.slider.SetRange( 0, 100 );
   Green_NumericControl.SetReal();
   Green_NumericControl.SetRange( 0, 1 );
   Green_NumericControl.SetPrecision( 6 );
   Green_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RotationInterface::__FilColor_ValueUpdated, w );

   Blue_NumericControl.label.SetText( "B:" );
   Blue_NumericControl.label.SetFixedWidth( labelWidth2 );
   Blue_NumericControl.slider.SetRange( 0, 100 );
   Blue_NumericControl.SetReal();
   Blue_NumericControl.SetRange( 0, 1 );
   Blue_NumericControl.SetPrecision( 6 );
   Blue_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RotationInterface::__FilColor_ValueUpdated, w );

   Alpha_NumericControl.label.SetText( "A:" );
   Alpha_NumericControl.label.SetFixedWidth( labelWidth2 );
   Alpha_NumericControl.slider.SetRange( 0, 100 );
   Alpha_NumericControl.SetReal();
   Alpha_NumericControl.SetRange( 0, 1 );
   Alpha_NumericControl.SetPrecision( 6 );
   Alpha_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&RotationInterface::__FilColor_ValueUpdated, w );

   ColorSample_Control.SetScaledFixedHeight( 20 );
   ColorSample_Control.OnPaint( (Control::paint_event_handler)&RotationInterface::__ColorSample_Paint, w );

   FillColor_Sizer.SetSpacing( 4 );
   FillColor_Sizer.Add( Red_NumericControl );
   FillColor_Sizer.Add( Green_NumericControl );
   FillColor_Sizer.Add( Blue_NumericControl );
   FillColor_Sizer.Add( Alpha_NumericControl );
   FillColor_Sizer.Add( ColorSample_Control );

   FillColor_Control.SetSizer( FillColor_Sizer );

   // -------------------------------------------------------------------------

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( Rotation_SectionBar );
   Global_Sizer.Add( Rotation_Control );
   Global_Sizer.Add( Interpolation_SectionBar );
   Global_Sizer.Add( Interpolation_Control );
   Global_Sizer.Add( FillColor_SectionBar );
   Global_Sizer.Add( FillColor_Control );

   Interpolation_Control.Hide();
   FillColor_Control.Hide();

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF RotationInterface.cpp - Released 2018-11-01T11:07:20Z
