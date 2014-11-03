// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard AssistedColorCalibration Process Module Version 01.00.00.0096
// ****************************************************************************
// AssistedColorCalibrationInterface.cpp - Released 2014/10/29 07:35:26 UTC
// ****************************************************************************
// This file is part of the standard AssistedColorCalibration PixInsight module.
//
// Copyright (c) 2010-2014 Zbynek Vrastil
// Copyright (c) 2003-2014 Pleiades Astrophoto S.L.
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

#include "AssistedColorCalibrationInterface.h"
#include "AssistedColorCalibrationProcess.h"
#include "AssistedColorCalibrationParameters.h"

#include "pcl/Graphics.h"
#include "pcl/Vector.h"

namespace pcl
{

// ----------------------------------------------------------------------------

AssistedColorCalibrationInterface* TheAssistedColorCalibrationInterface = 0;

// ----------------------------------------------------------------------------

#include "AssistedColorCalibrationIcon.xpm"

// ----------------------------------------------------------------------------

AssistedColorCalibrationInterface::AssistedColorCalibrationInterface() :
ProcessInterface(),
instance( TheAssistedColorCalibrationProcess ),
GUI( 0 ),
inputScrBmp( 0 ),
slidersScrBmp( 0 )
{
   TheAssistedColorCalibrationInterface = this;
   sliderBeingDragged = NoSlider;
}

AssistedColorCalibrationInterface::~AssistedColorCalibrationInterface()
{
   if ( inputScrBmp != 0 )
      delete inputScrBmp, inputScrBmp = 0;
   if ( slidersScrBmp != 0 )
      delete slidersScrBmp, slidersScrBmp = 0;

   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

IsoString AssistedColorCalibrationInterface::Id() const
{
   return "AssistedColorCalibration";
}

MetaProcess* AssistedColorCalibrationInterface::Process() const
{
   return TheAssistedColorCalibrationProcess;
}

const char** AssistedColorCalibrationInterface::IconImageXPM() const
{
   return AssistedColorCalibrationIcon_XPM;
}

void AssistedColorCalibrationInterface::ApplyInstance() const
{

   instance.LaunchOnCurrentView();
}

void AssistedColorCalibrationInterface::ResetInstance()
{
   AssistedColorCalibrationInstance defaultInstance( TheAssistedColorCalibrationProcess );
   ImportProcess( defaultInstance );
}

bool AssistedColorCalibrationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   // ### Deferred initialization
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "AssistedColorCalibration" );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheAssistedColorCalibrationProcess;
}

ProcessImplementation* AssistedColorCalibrationInterface::NewProcess() const
{
   return new AssistedColorCalibrationInstance( instance );
}

bool AssistedColorCalibrationInterface::ValidateProcess( const ProcessImplementation& p, String& whyNot ) const
{
   const AssistedColorCalibrationInstance* r = dynamic_cast<const AssistedColorCalibrationInstance*>( &p );
   if ( r == 0 )
   {
      whyNot = "Not a AssistedColorCalibration instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool AssistedColorCalibrationInterface::RequiresInstanceValidation() const
{
   return true;
}

bool AssistedColorCalibrationInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );
   UpdateControls();
   return true;
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::UpdateControls()
{
   GUI->RedCorrectionFactor_NumericControl.SetValue( instance.redCorrectionFactor );
   GUI->GreenCorrectionFactor_NumericControl.SetValue( instance.greenCorrectionFactor );
   GUI->BlueCorrectionFactor_NumericControl.SetValue( instance.blueCorrectionFactor );
   GUI->HistogramShadows_NumericEdit.SetValue( instance.histogramShadows );
   GUI->HistogramHighlights_NumericEdit.SetValue( instance.histogramHighlights );
   GUI->HistogramMidtones_NumericEdit.SetValue( instance.histogramMidtones );
   GUI->SaturationBoost_NumericControl.SetValue( instance.saturationBoost );
   GUI->BackgroundRef_ViewList.Regenerate();
   GUI->BackgroundRef_ViewList.SelectView( instance.backgroundReference.IsEmpty() ? View::Null() : View::ViewById( instance.backgroundReference ) );
   GUI->HistogramPlot_Control.Update();
   GUI->HistogramSliders_Control.Update();
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::PlotMidtonesTransferCurve( Graphics& g, const Rect& r, int width, int height )
{
   int xc0 = RoundI( instance.histogramShadows*(width - 1) );
   if ( xc0 >= r.x1 )
      return;

   int xc1 = RoundI( instance.histogramHighlights*(width - 1) );
   if ( xc1 < r.x0 )
      return;

   g.SetPen( RGBAColor( 255, 255, 255 ) );

   if ( xc1 - xc0 < 2 )
   {
      g.DrawLine( xc0-r.x0, 0, xc1-r.x0, r.Height()-1 );
      return;
   }

   double dx = 1.0/(xc1 - xc0);
   double m = instance.histogramMidtones;

   for ( int px0 = Max( r.x0-1, xc0 ), px1 = Min( r.x1, xc1 ), xi = px0, x0, y0; xi <= px1; ++xi )
   {
      int x = xi - r.x0;
      int y = RoundI( (height - 1)*(1 - HistogramTransformation::MTF( m, (xi - xc0)*dx )) ) - r.y0;

      if ( xi != px0 )
      {
         if ( xi != px1 && x == x0 && y == y0 )
            continue;

         g.DrawLine( x0, y0, x, y );
      }

      x0 = x;
      y0 = y;
   }
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::PlotHandler( Graphics& g, double v, int x0, int width )
{
   int h = 12;
   int h2 = (h >> 1) + 1;

   int x = RoundI( v*(width - 1) ) - x0;

   GenericVector<Point> notch( 4 );
   notch[0] = Point( x,      h-h2 );
   notch[1] = Point( x-h2+1, h-1  );
   notch[2] = Point( x+h2-1, h-1  );
   notch[3] = Point( x,      h-h2 );

   g.SetPen( HandlerColor( v ) );
   g.DrawLine( x, 0, x, h-h2-1 );
   g.DrawPolyline( notch );
}

// ----------------------------------------------------------------------------

RGBA AssistedColorCalibrationInterface::HandlerColor( double v ) const
{
   if ( v > 0.5 )
      return 0xFF000000;
   return 0xFFFFFFFF;
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::PlotGrid(
   Graphics& g, const Rect& r, int width, int height )
{
   const RGBA gridColor0 = RGBAColor( 0x50, 0x50, 0x50 );
   const RGBA gridColor1 = RGBAColor( 0x37, 0x37, 0x37 );

   double dx = double( width - 1 )/8;
   double dy = double( height - 1 )/8;

   int ix0 = int( r.x0/dx );
   int ix1 = int( r.x1/dx );

   int iy0 = int( r.y0/dy );
   int iy1 = int( r.y1/dy );

   int w = r.Width();
   int h = r.Height();

   Pen p0( gridColor0, 0, PenStyle::Solid );
   Pen p1( gridColor1, 0, PenStyle::Dot );

   for ( int i = ix0; i <= ix1; ++i )
   {
      int x = RoundI( dx*i ) - r.x0;

      if ( x >= w )
         break;

      g.SetPen( (i & 1) ? p1 : p0 );
      g.DrawLine( x, 0, x, h );
   }

   for ( int i = iy0; i <= iy1; ++i )
   {
      int y = RoundI( dy*i ) - r.y0;

      if ( y >= h )
         break;

      g.SetPen( (i & 1) ? p1 : p0 );
      g.DrawLine( 0, y, w, y );
   }
}


// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::GenerateInputScreenBitmap()
{
   Rect r0 = GUI->HistogramPlot_Control.BoundsRect();
   int w0 = r0.Width();
   int h0 = r0.Height();

   if ( inputScrBmp == 0 )
      inputScrBmp = new Bitmap( w0, h0, BitmapFormat::RGB32 );

   inputScrBmp->Fill( RGBAColor( 0, 0, 0 ) );

   Graphics g( *inputScrBmp );
   PlotGrid( g, r0, w0, h0 );
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::PlotScale( Graphics& g, const Rect& r, int width )
{
   int w = r.Width();
   int h = r.Height();

   float v0 = float( r.x0 )/(width - 1);
   float v1 = float( r.x1 - 1 )/(width - 1);

   GradientBrush::stop_list stops;
   stops.Add( GradientBrush::Stop( 0.0, RGBAColor( v0, v0, v0 ) ) );
   stops.Add( GradientBrush::Stop( 1.0, RGBAColor( v1, v1, v1 ) ) );

   g.FillRect( 0, 0, w, h, LinearGradientBrush( 0, 0, w, 0, stops ) );
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::GenerateSlidersScreenBitmap()
{
   Rect r0 = GUI->HistogramSliders_Control.BoundsRect();
   int w0 = r0.Width();
   int h0 = r0.Height();

   if ( slidersScrBmp == 0 )
      slidersScrBmp = new Bitmap( w0, h0, BitmapFormat::RGB32 );

   Graphics g( *slidersScrBmp );

   PlotScale( g, r0, w0 );
}

// ----------------------------------------------------------------------------

AssistedColorCalibrationInterface::SliderId AssistedColorCalibrationInterface::FindHandler( double v ) const
{
   double c0 = instance.histogramShadows;
   double c1 = instance.histogramHighlights;
   double cm = c0 + instance.histogramMidtones*(c1 - c0);

   double dcm = Abs( v - cm );
   double dc0 = Abs( v - c0 );
   double dc1 = Abs( v - c1 );

   if ( dcm < dc0 )
   {
      if ( dcm < dc1 )
         return MSlider;
   }

   if ( dc0 < dcm )
   {
      if ( dc0 < dc1 )
         return C0Slider;
   }

   return C1Slider;
}

// ----------------------------------------------------------------------------

double AssistedColorCalibrationInterface::SliderToHistogram( int x ) const
{
   return double( x ) / (GUI->HistogramSliders_Control.Width() - 1);
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::SetClippingParameters( double c0, double c1 )
{
   instance.histogramShadows = c0;
   instance.histogramHighlights = c1;

   GUI->HistogramShadows_NumericEdit.SetValue( instance.histogramShadows );
   GUI->HistogramHighlights_NumericEdit.SetValue( instance.histogramHighlights );

   GUI->HistogramSliders_Control.Update();
   GUI->HistogramPlot_Control.Update();
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::SetShadowsClipping( double x )
{
   x = Min( x, double( instance.histogramHighlights ) );
   instance.histogramShadows = x;

   GUI->HistogramShadows_NumericEdit.SetValue( x );

   GUI->HistogramSliders_Control.Update();
   GUI->HistogramPlot_Control.Update();
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::SetHighlightsClipping( double x )
{
   x = Max( x, double( instance.histogramShadows ) );
   instance.histogramHighlights = x;

   GUI->HistogramHighlights_NumericEdit.SetValue( x );

   GUI->HistogramSliders_Control.Update();
   GUI->HistogramPlot_Control.Update();
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::SetMidtonesBalance( double x )
{
   instance.histogramMidtones = x;

   GUI->HistogramMidtones_NumericEdit.SetValue( x );

   GUI->HistogramSliders_Control.Update();
   GUI->HistogramPlot_Control.Update();
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::ResetWhiteBalance()
{
   instance.redCorrectionFactor = TheACCRedCorrectionFactor->DefaultValue();
   instance.greenCorrectionFactor = TheACCGreenCorrectionFactor->DefaultValue();
   instance.blueCorrectionFactor = TheACCBlueCorrectionFactor->DefaultValue();

   GUI->RedCorrectionFactor_NumericControl.SetValue( instance.redCorrectionFactor );
   GUI->GreenCorrectionFactor_NumericControl.SetValue( instance.greenCorrectionFactor );
   GUI->BlueCorrectionFactor_NumericControl.SetValue( instance.blueCorrectionFactor );
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::__CorrectionFactorUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->RedCorrectionFactor_NumericControl )
      instance.redCorrectionFactor = value;
   else if ( sender == GUI->GreenCorrectionFactor_NumericControl )
      instance.greenCorrectionFactor = value;
   else if ( sender == GUI->BlueCorrectionFactor_NumericControl )
      instance.blueCorrectionFactor = value;
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::__HistogramParameterUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->HistogramShadows_NumericEdit )
      SetShadowsClipping( value );
   else if ( sender == GUI->HistogramHighlights_NumericEdit )
      SetHighlightsClipping( value );
   else if ( sender == GUI->HistogramMidtones_NumericEdit )
      SetMidtonesBalance( value );
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::__SaturationBoostUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->SaturationBoost_NumericControl )
      instance.saturationBoost = value;
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::__BackgroundRefViewSelected( ViewList& sender, View& view )
{
   if ( sender == GUI->BackgroundRef_ViewList )
      instance.backgroundReference = view.FullId();
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::__Histogram_Paint( Control& sender, const pcl::Rect& updateRect )
{
   if ( sender == GUI->HistogramPlot_Control )
   {
      if ( inputScrBmp == 0 )
         GenerateInputScreenBitmap();

      if ( inputScrBmp != 0 )
      {
         Bitmap bmp1 = inputScrBmp->Subimage( updateRect );

         Bitmap bmp( updateRect.Width(), updateRect.Height() );
         bmp.Fill( 0xFF000000 );

         Rect r0 = sender.ClientRect();

         {
            Graphics g( bmp );
            PlotMidtonesTransferCurve( g, updateRect, r0.Width(), r0.Height() );
         }

         bmp1.Xor( bmp );
         bmp1.SetPixelFormat( BitmapFormat::RGB32 );

         Graphics g( sender );
         g.DrawBitmap( updateRect.LeftTop(), bmp1 );
      }
   }
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::__Sliders_Paint( Control& sender, const pcl::Rect& updateRect )
{
   if ( slidersScrBmp == 0 )
      GenerateSlidersScreenBitmap();

   if ( slidersScrBmp != 0 )
   {
      Graphics g( sender );
      g.DrawBitmapRect( updateRect.LeftTop(), *slidersScrBmp, updateRect );

      double c0, m, c1;
      instance.GetHistogramTransformation( c0, c1, m );

      int w = sender.Width();
      PlotHandler( g, c0, 0, w );
      PlotHandler( g, c0 + m*(c1 - c0), 0, w );
      PlotHandler( g, c1, 0, w );
   }
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::__Sliders_MousePress(
   Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   if ( button != MouseButton::Left )
      return;

   sliderBeingDragged = FindHandler( SliderToHistogram( pos.x ) );

   __Sliders_MouseMove( sender, pos, buttons, modifiers );
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::__Sliders_MouseRelease(
   Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   sliderBeingDragged = NoSlider;
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::__Sliders_MouseMove(
   Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers )
{
   if ( sliderBeingDragged != NoSlider )
   {
      double v = SliderToHistogram( pos.x );

      double c0 = instance.histogramShadows;
      double c1 = instance.histogramHighlights;

      if ( sliderBeingDragged == MSlider )
      {
         SetMidtonesBalance( Round( (c0 != c1) ? (Range( v, c0, c1 ) - c0)/(c1 - c0) : c0,
                                 TheACCHistogramMidtones->Precision() ) );
      }
      else if ( sliderBeingDragged == C0Slider )
      {
         v = Range( Round( v, TheACCHistogramShadows->Precision() ), TheACCHistogramShadows->MinimumValue(), TheACCHistogramShadows->MaximumValue() );
         SetClippingParameters( v, Max( v, c1 ) );
      }
      else if ( sliderBeingDragged == C1Slider )
      {
         v =  Range( Round( v, TheACCHistogramHighlights->Precision() ), TheACCHistogramHighlights->MinimumValue(), TheACCHistogramHighlights->MaximumValue() );
         SetClippingParameters( Min( c0, v ), v );
      }
   }
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::__Reset_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->HistogramReset_ToolButton )
   {
      SetClippingParameters( TheACCHistogramShadows->DefaultValue(), TheACCHistogramHighlights->DefaultValue() );
      SetMidtonesBalance( TheACCHistogramMidtones->DefaultValue() );
   }
   else if ( sender == GUI->WhiteBalanceReset_ToolButton )
   {
      ResetWhiteBalance();
   }
}

// ----------------------------------------------------------------------------

AssistedColorCalibrationInterface::GUIData::GUIData( AssistedColorCalibrationInterface& w )
{
   // white balance section

   SetUpCorrectionFactorNumericControl( w, RedCorrectionFactor_NumericControl, TheACCRedCorrectionFactor, "Red" );
   SetUpCorrectionFactorNumericControl( w, GreenCorrectionFactor_NumericControl, TheACCGreenCorrectionFactor, "Green" );
   SetUpCorrectionFactorNumericControl( w, BlueCorrectionFactor_NumericControl, TheACCBlueCorrectionFactor, "Blue" );

   WhiteBalance_Sizer.SetSpacing( 6 );
   WhiteBalance_Sizer.Add( RedCorrectionFactor_NumericControl );
   WhiteBalance_Sizer.Add( GreenCorrectionFactor_NumericControl );
   WhiteBalance_Sizer.Add( BlueCorrectionFactor_NumericControl );

   WhiteBalanceReset_ToolButton.SetIcon( Bitmap( ":/icons/delete.png" ) );
   WhiteBalanceReset_ToolButton.SetFixedSize( 20, 20 );
   WhiteBalanceReset_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   WhiteBalanceReset_ToolButton.SetToolTip( "Reset white balance factors" );
   WhiteBalanceReset_ToolButton.OnClick( (ToolButton::click_event_handler)&AssistedColorCalibrationInterface::__Reset_ButtonClick, w );

   WhiteBalanceReset_Sizer.AddStretch(100);
   WhiteBalanceReset_Sizer.Add( WhiteBalanceReset_ToolButton );

   WhiteBalance_SizerH.SetMargin( 8 );
   WhiteBalance_SizerH.SetSpacing( 6 );
   WhiteBalance_SizerH.Add( WhiteBalance_Sizer, 100 );
   WhiteBalance_SizerH.Add( WhiteBalanceReset_Sizer );

   WhiteBalance_Control.SetSizer( WhiteBalance_SizerH );

   WhiteBalance_Section.SetTitle( "White Balance" );
   WhiteBalance_Section.SetSection( WhiteBalance_Control );

   // preview section

   //   background group

   pcl::Font fnt = w.Font();
   BackgroundRef_Label.SetText( "Background Reference:" );
   BackgroundRef_Label.SetFixedWidth( fnt.Width( "Background Reference:" ) );
   BackgroundRef_Label.SetTextAlignment( TextAlign::Right|TextAlign::VertCenter );
   BackgroundRef_Label.SetToolTip( "<p>The view or preview, which will be used to get image background. The background value "
      "will be subtracted from the preview before applying histogram and saturation trasformations.</p>" );

   BackgroundRef_ViewList.OnViewSelected( (ViewList::view_event_handler)&AssistedColorCalibrationInterface::__BackgroundRefViewSelected, w );

   BackgroundRef_Sizer.SetSpacing( 4 );
   BackgroundRef_Sizer.Add( BackgroundRef_Label );
   BackgroundRef_Sizer.Add( BackgroundRef_ViewList, 100 );

   Background_Sizer.SetMargin( 8 );
   Background_Sizer.SetSpacing( 6 );
   Background_Sizer.Add( BackgroundRef_Sizer );

   Background_Group.SetTitle( "Background Correction" );
   Background_Group.SetSizer( Background_Sizer );

   //   histogram group

   HistogramPlot_Control.SetFixedHeight( 150 );
   HistogramPlot_Control.SetFixedWidth( 200 );
   HistogramPlot_Control.SetToolTip("<p> The histogram transformation applied to the preview image <i>after</i> applying white balance coefficients and "
      "subtracting background reference. Use it to set up the preview so the image is reasonably close to what you expect as final result.</p>");
   HistogramPlot_Control.OnPaint( (Control::paint_event_handler)&AssistedColorCalibrationInterface::__Histogram_Paint, w );

   HistogramSliders_Control.EnableMouseTracking();
   HistogramSliders_Control.SetCursor( StdCursor::UpArrow );
   HistogramSliders_Control.SetFixedHeight( 12 );
   HistogramSliders_Control.OnPaint( (Control::paint_event_handler)&AssistedColorCalibrationInterface::__Sliders_Paint, w );
   HistogramSliders_Control.OnMouseMove( (Control::mouse_event_handler)&AssistedColorCalibrationInterface::__Sliders_MouseMove, w );
   HistogramSliders_Control.OnMousePress( (Control::mouse_button_event_handler)&AssistedColorCalibrationInterface::__Sliders_MousePress, w );
   HistogramSliders_Control.OnMouseRelease( (Control::mouse_button_event_handler)&AssistedColorCalibrationInterface::__Sliders_MouseRelease, w );

   HistogramPlot_Sizer.SetMargin( 8 );
   HistogramPlot_Sizer.SetSpacing( 0 );
   HistogramPlot_Sizer.Add( HistogramPlot_Control );
   HistogramPlot_Sizer.Add( HistogramSliders_Control );

   SetUpHistogramNumericEdit( w, HistogramShadows_NumericEdit, TheACCHistogramShadows, "Shadows", "<p>Shadows clipping</p>" );
   SetUpHistogramNumericEdit( w, HistogramHighlights_NumericEdit, TheACCHistogramHighlights, "Highlights", "<p>Highlights clipping</p>" );
   SetUpHistogramNumericEdit( w, HistogramMidtones_NumericEdit, TheACCHistogramMidtones, "Midtones", "<p>Midtones balance</p>" );

   HistogramReset_ToolButton.SetIcon( Bitmap( ":/icons/delete.png" ) );
   HistogramReset_ToolButton.SetFixedSize( 20, 20 );
   HistogramReset_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   HistogramReset_ToolButton.SetToolTip( "Reset histogram settings" );
   HistogramReset_ToolButton.OnClick( (ToolButton::click_event_handler)&AssistedColorCalibrationInterface::__Reset_ButtonClick, w );

   HistogramReset_Sizer.AddStretch( 100 );
   HistogramReset_Sizer.Add( HistogramReset_ToolButton );

   HistogramValues_Sizer.SetMargin( 8 );
   HistogramValues_Sizer.SetSpacing( 6 );
   HistogramValues_Sizer.Add( HistogramShadows_NumericEdit );
   HistogramValues_Sizer.Add( HistogramHighlights_NumericEdit );
   HistogramValues_Sizer.Add( HistogramMidtones_NumericEdit );
   HistogramValues_Sizer.AddStretch();
   HistogramValues_Sizer.Add( HistogramReset_Sizer );

   Histogram_Sizer.Add( HistogramPlot_Sizer );
   Histogram_Sizer.Add( HistogramValues_Sizer );

   Histogram_Group.SetTitle( "Histogram Transformation" );
   Histogram_Group.SetSizer( Histogram_Sizer );

   //   saturation group

   SetUpSaturationNumericControl( w, SaturationBoost_NumericControl, TheACCSaturationBoost, "Saturation", "<p>Saturation boost</p>" );

   Saturation_Sizer.SetMargin( 8 );
   Saturation_Sizer.SetSpacing( 6 );
   Saturation_Sizer.Add( SaturationBoost_NumericControl, 100 );

   Saturation_Group.SetTitle( "Saturation Enhancement" );
   Saturation_Group.SetSizer( Saturation_Sizer );

   // preview section global

   PreviewParams_Sizer.SetMargin( 8 );
   PreviewParams_Sizer.SetSpacing( 6 );
   PreviewParams_Sizer.Add( Background_Group );
   PreviewParams_Sizer.Add( Histogram_Group );
   PreviewParams_Sizer.Add( Saturation_Group );

   PreviewParams_Control.SetSizer( PreviewParams_Sizer );

   PreviewParams_Section.SetTitle( "Preview Parameters" );
   PreviewParams_Section.SetSection( PreviewParams_Control );
   PreviewParams_Section.SetToolTip( "<p>Parameters in these section are only applied to Previews and not to regular images. They should be used to "
      "get image reasonably close to final result, so you can evaluate applied white balance coefficients. These settins will NOT be applied to your image.</p>" );

   // global sizer

   Global_Sizer.SetMargin( 8 );
   Global_Sizer.SetSpacing( 6 );
   Global_Sizer.Add( WhiteBalance_Section );
   Global_Sizer.Add( WhiteBalance_Control );
   Global_Sizer.Add( PreviewParams_Section );
   Global_Sizer.Add( PreviewParams_Control );


   w.SetSizer( Global_Sizer );
   w.AdjustToContents();
   w.SetFixedSize();
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::GUIData::SetUpCorrectionFactorNumericControl( AssistedColorCalibrationInterface& w,
        NumericControl &control, MetaFloat *parameter, const String &label)
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Green:" ) ); // the longest label text
   int editWidth1 = fnt.Width( String( '0', 7 ) );

   control.label.SetText( label + ":" );
   control.label.SetFixedWidth( labelWidth1 );
   control.slider.SetRange( 0, 2000 );
   control.SetReal();
   control.SetRange( parameter->MinimumValue(), parameter->MaximumValue() );
   control.SetPrecision( parameter->Precision() );
   control.SetToolTip( String("<p>") + label + " channel correction factor.</p>" );
   control.edit.SetFixedWidth( editWidth1 );
   control.OnValueUpdated( (NumericEdit::value_event_handler)&AssistedColorCalibrationInterface::__CorrectionFactorUpdated, w );
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::GUIData::SetUpHistogramNumericEdit( AssistedColorCalibrationInterface& w, NumericEdit &control,
         MetaFloat *parameter, const String &label, const String& tooltip )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Highlights:" ) ); // the longest label text
   int editWidth1 = fnt.Width( String( '0', 9 ) );

   control.label.SetText( label + ":" );
   control.label.SetFixedWidth( labelWidth1 );
   control.SetReal();
   control.SetRange( parameter->MinimumValue(), parameter->MaximumValue() );
   control.SetPrecision( parameter->Precision() );
   control.SetToolTip( tooltip );
   control.edit.SetFixedWidth( editWidth1 );
   control.OnValueUpdated( (NumericEdit::value_event_handler)&AssistedColorCalibrationInterface::__HistogramParameterUpdated, w );
}

// ----------------------------------------------------------------------------

void AssistedColorCalibrationInterface::GUIData::SetUpSaturationNumericControl( AssistedColorCalibrationInterface& w, NumericControl &control,
         MetaFloat *parameter, const String &label, const String& tooltip )
{
   pcl::Font fnt = w.Font();
   int labelWidth1 = fnt.Width( String( "Saturation:" ) ); // the longest label text
   int editWidth1 = fnt.Width( String( '0', 7 ) );

   control.label.SetText( label + ":" );
   control.label.SetFixedWidth( labelWidth1 );
   control.slider.SetRange( 0, 2000 );
   control.SetReal();
   control.SetRange( parameter->MinimumValue(), parameter->MaximumValue() );
   control.SetPrecision( parameter->Precision() );
   control.SetToolTip( tooltip );
   control.edit.SetFixedWidth( editWidth1 );
   control.OnValueUpdated( (NumericEdit::value_event_handler)&AssistedColorCalibrationInterface::__SaturationBoostUpdated, w );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF AssistedColorCalibrationInterface.cpp - Released 2014/10/29 07:35:26 UTC
