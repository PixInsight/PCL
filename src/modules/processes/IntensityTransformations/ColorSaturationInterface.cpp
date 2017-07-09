//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.07.0861
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0393
// ----------------------------------------------------------------------------
// ColorSaturationInterface.cpp - Released 2017-07-09T18:07:33Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "ColorSaturationInterface.h"
#include "ColorSaturationProcess.h"

#include <pcl/AutoPointer.h>
#include <pcl/GlobalSettings.h>
#include <pcl/Graphics.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/RealTimePreview.h>

#define CURSOR_TOLERANCE   PixInsightSettings::GlobalInteger( "ImageWindow/CursorTolerance" )
#define WHEEL_STEP_ANGLE   PixInsightSettings::GlobalInteger( "ImageWindow/WheelStepAngle" )

namespace pcl
{

// ----------------------------------------------------------------------------

ColorSaturationInterface* TheColorSaturationInterface = 0;

// ----------------------------------------------------------------------------

#include "ColorSaturationIcon.xpm"
#include "show_grid.xpm"
#include "akima_interpolation.xpm"
#include "cubic_spline_interpolation.xpm"
#include "linear_interpolation.xpm"
#include "delete_point_mode.xpm"
#include "select_point_mode.xpm"
#include "edit_point_mode.xpm"

// ----------------------------------------------------------------------------

static const int s_maxZoom = 99;
static const int s_maxScale = 10;

// ----------------------------------------------------------------------------

ColorSaturationInterface::ColorSaturationInterface() :
ProcessInterface(),
instance( TheColorSaturationProcess ), m_realTimeThread( 0 ), GUI( 0 ),
m_viewportBitmap( Bitmap::Null() ),
m_viewportDirty( true )
{
   TheColorSaturationInterface = this;

   m_mode = EditMode;
   m_savedMode = NoMode;

   m_currentPoint = 0;

   m_readoutActive = false;
   for ( int i = 0; i < 4; ++i )
      m_readouts[i] = 0;

   m_zoomX = m_zoomY = 1;
   m_scale = 1;
   m_wheelSteps = 0;

   m_showGrid = true;

   m_panning = 0;
   m_panOrigin = 0;

   m_cursorVisible = false;
   m_dragging = false;
   m_cursorPos = -1;
   m_curvePos = 0;

   m_channelColor = RGBAColor( 0xFF, 0xFF, 0x00 ); // H

   m_gridColor0      = RGBAColor( 0x50, 0x50, 0x50 );
   m_gridColor1      = RGBAColor( 0x37, 0x37, 0x37 );
   m_gridColor2      = RGBAColor( 0x60, 0x60, 0x60 );
   m_backgroundColor = RGBAColor( 0x00, 0x00, 0x00 );

   m_minCurveWidth = 300;
   m_minCurveHeight = 300;
   m_scaleSize = 8;

   m_settingUp = false;
}

ColorSaturationInterface::~ColorSaturationInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

// ----------------------------------------------------------------------------

IsoString ColorSaturationInterface::Id() const
{
   return "ColorSaturation";
}

// ----------------------------------------------------------------------------

MetaProcess* ColorSaturationInterface::Process() const
{
   return TheColorSaturationProcess;
}

// ----------------------------------------------------------------------------

const char** ColorSaturationInterface::IconImageXPM() const
{
   return ColorSaturationIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures ColorSaturationInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::RealTimeButton;
}

// ----------------------------------------------------------------------------

void ColorSaturationInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void ColorSaturationInterface::RealTimePreviewUpdated( bool active )
{
   if ( GUI != 0 )
      if ( active )
         RealTimePreview::SetOwner( *this ); // implicitly updates the r-t preview
      else
         RealTimePreview::SetOwner( ProcessInterface::Null() );
}

// ----------------------------------------------------------------------------

void ColorSaturationInterface::ResetInstance()
{
   ColorSaturationInstance defaultInstance( TheColorSaturationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool ColorSaturationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "ColorSaturation" );
      OnKeyPress( (Control::keyboard_event_handler)&ColorSaturationInterface::__KeyPress, *this );
      OnKeyRelease( (Control::keyboard_event_handler)&ColorSaturationInterface::__KeyRelease, *this );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheColorSaturationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* ColorSaturationInterface::NewProcess() const
{
   return new ColorSaturationInstance( instance );
}

// ----------------------------------------------------------------------------

bool ColorSaturationInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const ColorSaturationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a ColorSaturation instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool ColorSaturationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool ColorSaturationInterface::ImportProcess( const ProcessImplementation& p )
{
   instance.Assign( p );

   m_currentPoint = 0;

   UpdateControls();
   UpdateCurve();
   UpdateRealTimePreview();
   return true;
}

// ----------------------------------------------------------------------------

bool ColorSaturationInterface::RequiresRealTimePreviewUpdate( const UInt16Image& image, const View&, int /*zoomLevel*/ ) const
{
   return image.IsColor();
}

// ----------------------------------------------------------------------------

ColorSaturationInterface::RealTimeThread::RealTimeThread() : Thread(), m_instance( TheColorSaturationProcess )
{
}

void ColorSaturationInterface::RealTimeThread::Reset( const UInt16Image& image, const ColorSaturationInstance& instance )
{
   image.ResetSelections();
   m_image.Assign( image );
   m_instance.Assign( instance );
}

void ColorSaturationInterface::RealTimeThread::Run()
{
   m_instance.Preview( m_image );
}

bool ColorSaturationInterface::GenerateRealTimePreview( UInt16Image& image, const View&, int, String& ) const
{
   if ( !image.IsColor() )
      return true;

   m_realTimeThread = new RealTimeThread;

   for ( ;; )
   {
      m_realTimeThread->Reset( image, instance );
      m_realTimeThread->Start();

      while ( m_realTimeThread->IsActive() )
      {
         ProcessEvents();

         if ( !IsRealTimePreviewActive() )
         {
            m_realTimeThread->Abort();
            m_realTimeThread->Wait();

            delete m_realTimeThread;
            m_realTimeThread = 0;
            return false;
         }
      }

      if ( !m_realTimeThread->IsAborted() )
      {
         image.Assign( m_realTimeThread->m_image );

         delete m_realTimeThread;
         m_realTimeThread = 0;
         return true;
      }
   }
}

// ----------------------------------------------------------------------------

bool ColorSaturationInterface::WantsReadoutNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void ColorSaturationInterface::BeginReadout( const View& v )
{
   if ( GUI != 0 && IsVisible() )
      m_readoutActive = true;
}

// ----------------------------------------------------------------------------

void ColorSaturationInterface::UpdateReadout( const View& v, const DPoint& p, double R, double G, double B, double A )
{
   if ( m_readoutActive )
   {
      m_readouts[0] = R;
      m_readouts[1] = G;
      m_readouts[2] = B;
      m_readouts[3] = A;
      v.Window().GetRGBWS( m_readoutRGBWS );
      UpdateCurve();
   }
}

// ----------------------------------------------------------------------------

void ColorSaturationInterface::EndReadout( const View& v )
{
   if ( m_readoutActive )
   {
      m_readoutActive = false;
      UpdateCurve();
   }
}


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

size_type ColorSaturationInterface::FindPoint( double x, double y, int tolerancePx ) const
{
   double delta = double( tolerancePx )/(GUI->Curve_ScrollBox.Viewport().Width()*m_zoomX - 1);

   const double* xa = instance.C.XVector();
   const double* ya = instance.C.YVector();

   for ( size_type i = 0, n = instance.C.Length(); i < n; ++i )
   {
      double dxi = Abs( x - xa[i] );
      if ( dxi <= delta )
      {
         double dyi = Abs( y - ya[i] );
         if ( dyi <= delta )
         {
            size_type j = i + 1;
            if ( j < n )
            {
               double dxj = Abs( x - xa[j] );
               if ( dxj <= delta )
               {
                  double dyj = Abs( y - ya[j] );
                  if ( dyj <= delta )
                  {
                     if ( dxj*dxj + dyj*dyj < dxi*dxi + dyi*dyi )
                        i = j;
                  }
               }
            }

            return i;
         }
      }
   }

   return ~size_type( 0 );
}

size_type ColorSaturationInterface::CreatePoint( double x, double y, int tolerancePx )
{
   double delta = float( tolerancePx )/(GUI->Curve_ScrollBox.Viewport().Width()*m_zoomX - 1);

   const double* xa = instance.C.XVector();
   const double* ya = instance.C.YVector();

   size_type i;
   for ( i = 0; ; )
   {
      double dxi = Abs( x - xa[i] );
      if ( dxi <= delta )
      {
         size_type j = i + 1;
         if ( j < instance.C.Length() )
         {
            double dxj = Abs( x - xa[j] );
            if ( dxj <= delta )
            {
               double dyi = Abs( y - ya[i] );
               double dyj = Abs( y - ya[j] );
               if ( dxj*dxj + dyj*dyj < dxi*dxi + dyi*dyi )
                  i = j;
            }
         }

         instance.C.Y( i ) = y;
         break;
      }

      if ( x < xa[i] || ++i == instance.C.Length() )
      {
         instance.C.Add( x, y );
         break;
      }
   }

   if ( IsRealTimePreviewActive() )
      UpdateRealTimePreview();

   return i;
}

bool ColorSaturationInterface::DragPoint( size_type i, double x, double y )
{
   bool ok = false;

   if ( i > 0 && i < instance.C.Length()-1 && x > instance.C.X( i-1 ) && x < instance.C.X( i+1 ) )
   {
      ok = true;
      instance.C.Remove( i );
      instance.C.Add( x, y );
   }

   if ( ok == false )
      instance.C.Y( i ) = y;

   if ( i == 0 || i == instance.C.Length()-1)
      instance.C.Y( instance.C.Length()-1 ) = instance.C.Y( 0 ) = y;

   UpdateRealTimePreview();

   return ok;
}

size_type ColorSaturationInterface::RemovePoint( double x, double y, int tolerancePx )
{
   size_type i = FindPoint( x, y, tolerancePx );

   if ( i != ~size_type( 0 ) )
   {
      size_type i1 = instance.C.Length() - 1;
      if ( i == 0 || i == i1)
         instance.C.Y( i1 ) =instance.C.Y( 0 ) = 0;
      else
      {
            instance.C.Remove( i );
      }

      UpdateRealTimePreview();
   }

   return i;
}

// ----------------------------------------------------------------------------

void ColorSaturationInterface::SetZoom( int hz, int vz, const Point* p )
{
   m_settingUp = true;

   m_zoomX = hz;
   m_zoomY = vz;

   bool hsb = m_zoomX > 1;
   bool vsb = m_zoomY > 1;

   GUI->Curve_ScrollBox.ShowScrollBars( hsb, vsb );

   int visibleWidth = GUI->Curve_ScrollBox.Viewport().Width();
   int visibleHeight = GUI->Curve_ScrollBox.Viewport().Height();

   int contentsWidth = visibleWidth * m_zoomX;
   int contentsHeight = visibleHeight * m_zoomY;

   if ( hsb )
   {
      int m = contentsWidth - visibleWidth;
      GUI->Curve_ScrollBox.SetHorizontalScrollRange( 0, m );

      if ( p != 0 )
         GUI->Curve_ScrollBox.SetHorizontalScrollPosition( Range( p->x*m_zoomX - (visibleWidth >> 1), 0, m ) );
   }
   else
      GUI->Curve_ScrollBox.SetHorizontalScrollRange( 0, 0 );

   GUI->Curve_ScrollBox.SetPageWidth( visibleWidth );

   if ( vsb )
   {
      int m = contentsHeight - visibleHeight;
      GUI->Curve_ScrollBox.SetVerticalScrollRange( 0, m );

      if ( p != 0 )
         GUI->Curve_ScrollBox.SetVerticalScrollPosition( Range( p->y*m_zoomY - (visibleHeight >> 1), 0, m ) );
   }
   else
      GUI->Curve_ScrollBox.SetVerticalScrollRange( 0, 0 );

   GUI->Curve_ScrollBox.SetPageHeight( visibleHeight );

   UpdateZoomControls();
   UpdateCurve();

   m_settingUp = false;
}

void ColorSaturationInterface::SetMode( working_mode m )
{
   m_mode = m;

   if ( GUI->Curve_ScrollBox.Viewport().IsUnderMouse() )
      GUI->Curve_ScrollBox.Viewport().Update();

   UpdateModeControls();
   UpdateCurveInfo();
}

void ColorSaturationInterface::SetInterpolation( int type )
{
   instance.C.SetType( type );

   UpdateInterpolationControls();
   UpdateCurve();
   UpdateRealTimePreview();
}

void ColorSaturationInterface::SelectPoint( size_type p, bool pan )
{
   m_currentPoint = p;

   if ( pan && (m_zoomX != 1 || m_zoomY != 1) )
   {
      Rect r = GUI->Curve_ScrollBox.Viewport().ClientRect();
      int w1 = r.Width()*m_zoomX - 1;
      int h1 = r.Height()*m_zoomY - 1;
      GUI->Curve_ScrollBox.SetScrollPosition(
               Range( RoundInt( CurrentInputValue()*w1 ) - r.Width()/2, 0, w1 ),
               Range( RoundInt( (0.5 - CurrentOutputValue()/(2*m_scale))*h1 ) - r.Height()/2, 0, h1 ) );
   }

   UpdateCurveControls();
   UpdateCurve();
}

// ----------------------------------------------------------------------------

void ColorSaturationInterface::UpdateControls()
{
   UpdateModeControls();
   UpdateZoomControls();
   UpdateGraphicsControls();
   UpdateInterpolationControls();
   UpdateCurveControls();

   GUI->HueShift_NumericControl.SetValue( instance.hueShift );
}

void ColorSaturationInterface::UpdateModeControls()
{
   GUI->EditPointMode_ToolButton.SetChecked( m_mode == EditMode );
   GUI->SelectPointMode_ToolButton.SetChecked( m_mode == SelectMode );
   GUI->DeletePointMode_ToolButton.SetChecked( m_mode == DeleteMode );
   GUI->ZoomInMode_ToolButton.SetChecked( m_mode == ZoomInMode );
   GUI->ZoomOutMode_ToolButton.SetChecked( m_mode == ZoomOutMode );
   GUI->PanMode_ToolButton.SetChecked( m_mode == PanMode );
}

void ColorSaturationInterface::UpdateZoomControls()
{
   GUI->Zoom_SpinBox.SetValue( m_zoomX );
   GUI->Scale_SpinBox.SetValue( m_scale );
}

void ColorSaturationInterface::UpdateGraphicsControls()
{
   GUI->ShowGrid_ToolButton.SetChecked( m_showGrid );
}

void ColorSaturationInterface::UpdateInterpolationControls()
{
   GUI->AkimaSubsplines_ToolButton.SetChecked( instance.C.Type() == CurveType::AkimaSubsplines );
   GUI->CubicSpline_ToolButton.SetChecked( instance.C.Type() == CurveType::CubicSpline );
   GUI->Linear_ToolButton.SetChecked( instance.C.Type() == CurveType::Linear );
}

void ColorSaturationInterface::UpdateCurveControls()
{
   GUI->Input_NumericEdit.SetValue( CurrentInputValue() );
   GUI->Output_NumericEdit.SetValue( CurrentOutputValue() );

   bool isFirstPoint = m_currentPoint == 0;
   bool isLastPoint = m_currentPoint == instance.C.Length()-1;

   GUI->Input_NumericEdit.Enable( !(isFirstPoint || isLastPoint) );

   GUI->PrevPoint_ToolButton.Enable( !isFirstPoint );
   GUI->FirstPoint_ToolButton.Enable( !isFirstPoint );

   GUI->NextPoint_ToolButton.Enable( !isLastPoint );
   GUI->LastPoint_ToolButton.Enable( !isLastPoint );

   GUI->CurrentPoint_Label.SetText( String().Format( "%u / %u",
                                    m_currentPoint + 1, instance.C.Length() ) );

   GUI->RestoreCurve_ToolButton.Enable( !m_storedCurve.IsIdentity() );
}

void ColorSaturationInterface::UpdateCurve()
{
   UpdateCurveInfo();

   m_viewportDirty = true;
   GUI->Curve_ScrollBox.Viewport().Update();
}

void ColorSaturationInterface::UpdateCurveInfo()
{
   if ( !m_cursorVisible )
   {
      GUI->Info_Label.Clear();
      return;
   }

   String s;

   switch ( m_mode )
   {
   default:
   case EditMode:    s = (m_dragging ? " Editing Point" : " Add/Edit Point"); break;
   case SelectMode:  s = " Select Point"; break;
   case DeleteMode:  s = " Delete Point"; break;
   case ZoomInMode:  s = " Zoom In"; break;
   case ZoomOutMode: s = " Zoom Out"; break;
   case PanMode:     s = " Pan"; break;
   }

   s.AppendFormat( " | x = %.5f | y = %.5f", m_curvePos.x, m_curvePos.y );

   GUI->Info_Label.SetText( s );
}

void ColorSaturationInterface::UpdateRealTimePreview()
{
   if ( IsRealTimePreviewActive() )
   {
      if ( m_realTimeThread != 0 )
         m_realTimeThread->Abort();
      GUI->UpdateRealTimePreview_Timer.Start();
   }
}

// ----------------------------------------------------------------------------

void ColorSaturationInterface::RegenerateViewport()
{
   Rect r0 = GUI->Curve_ScrollBox.Viewport().BoundsRect();
   int w0 = r0.Width();
   int h0 = r0.Height();

   m_viewportDirty = false;

   if ( m_viewportBitmap.IsNull() )
      m_viewportBitmap = Bitmap( w0, h0, BitmapFormat::RGB32 );
   m_viewportBitmap.Fill( m_backgroundColor );

   Rect r( r0 + GUI->Curve_ScrollBox.ScrollPosition() );
   int w = w0*m_zoomX;
   int h = h0*m_zoomY;

   if ( m_showGrid )
   {
      Graphics g( m_viewportBitmap );
      g.EnableAntialiasing();
      PlotScale( g, r, w, h );
      PlotGrid( g, r, w, h, m_zoomX, m_zoomY );
   }

   Bitmap bmp( w0, h0 );
   {
      bmp.Fill( 0xFF000000 );
      Graphics g( bmp );
      g.EnableAntialiasing();
      g.SetCompositionOperator( CompositionOp::Screen );
      PlotCurve( g, r, w, h, m_zoomX, m_zoomY );
   }

   Graphics g( m_viewportBitmap );
   g.SetCompositionOperator( CompositionOp::Difference );
   g.DrawBitmap( 0, 0, bmp );
}

// ----------------------------------------------------------------------------

void ColorSaturationInterface::PlotGrid( Graphics& g, const Rect& r, int width, int height, int hZoom, int vZoom )
{
   int n = 8 * Min( hZoom, vZoom );

   double dx = double( width - 1 )/n;
   double dy = double( height - 1 )/n;

   int ix0 = int( r.x0/dx );
   int ix1 = int( r.x1/dx );

   int iy0 = int( r.y0/dy );
   int iy1 = int( r.y1/dy );

   int w = r.Width();
   int h = r.Height();

   Pen p0( m_gridColor0, DisplayPixelRatio(), PenStyle::Solid );
   Pen p1( m_gridColor1, DisplayPixelRatio(), PenStyle::Dot );
   Pen p2( m_gridColor2, DisplayPixelRatio(), PenStyle::DashDot );

   for ( int i = ix0; i <= ix1; ++i )
   {
      int x = RoundInt( dx*i ) - r.x0;

      if ( x >= w )
         break;

      g.SetPen( (i & 1) ? p1 : p0 );
      g.DrawLine( x, 0, x, h );
   }

   for ( int i = iy0; i <= iy1; ++i )
   {
      int y = RoundInt( dy*i ) - r.y0;

      if ( y >= h )
         break;

      g.SetPen( (i & 1) ? p1 : p0 );
      g.DrawLine( 0, y, w, y );
   }

   g.SetPen( p2 );
   g.DrawLine( 0, height >> 1, w, height >> 1 );
}

static RGBA ScaleColor_H( float f )
{
   RGBColorSystem::sample r, g, b;
   RGBColorSystem::HSVToRGB( r, g, b, f, 0.9, 0.9 );
   return RGBAColor( float( r ), float( g ), float( b ) );
}

static RGBA ScaleColor_S( float f )
{
   RGBColorSystem::sample r, g, b;
   RGBColorSystem::HSVToRGB( r, g, b, 0.75, f, 0.9 );
   return RGBAColor( float( r ), float( g ), float( b ) );
}

void ColorSaturationInterface::PlotScale( Graphics& g, const Rect& r, int width, int height )
{
   int ui8 = LogicalPixelsToPhysical( 8 );

   int w = r.Width();
   int h = r.Height();

   int x0 = -r.x0;
   int y0 = height - ui8 - r.y0;

   for ( int y = 0; y < h; ++y )
   {
      g.SetPen( ScaleColor_S( 1 - float( y + r.y0 )/(height - 1) ) );
      g.DrawLine( x0, y, x0+ui8, y );
   }

   for ( int x = 0; x < w; ++x )
   {
      int diag = (x + r.x0 < ui8) ? ui8 - (x + r.x0) : 0;
      g.SetPen( ScaleColor_H( instance.ShiftHueValue( float( x + r.x0 )/(width - 1) ) ) );
      g.DrawLine( x, y0+diag, x, y0+ui8 );
   }
}

void ColorSaturationInterface::PlotCurve( Graphics& g, const Rect& r,  int width, int height, int hZoom, int vZoom )
{
   const HSCurve& C = instance.C;

   AutoPointer<HSCurve::interpolator> interpolator( C.InitInterpolator() );
   Array<Point> curve;
   double dx = 1.0/(width - 1);
   for ( int w = r.Width(), x = 0; x < w; ++x )
      curve.Add( Point( x, RoundInt( (height - 1)*(1 - CurveToViewport( (*interpolator)( (x + r.x0)*dx ) )) ) - r.y0 ) );

   g.SetPen( m_channelColor, DisplayPixelRatio() );
   g.DrawPolyline( curve );

   int ui2 = LogicalPixelsToPhysical( 2 );
   g.SetBrush( 0xFF000000 );
   for ( size_type i = 0; i < C.Length(); ++i )
   {
      int x = RoundInt( C.XVector()[i]*(width - 1) ) - r.x0;
      int y = RoundInt( (1 - CurveToViewport( C.YVector()[i] ))*(height - 1) ) - r.y0;

      if ( i == m_currentPoint )
         g.SetBrush( m_channelColor );

      g.DrawRect( x-ui2, y-ui2, x+ui2, y+ui2 );

      if ( i == m_currentPoint )
         g.SetBrush( 0xFF000000 );
   }
}

void ColorSaturationInterface::PlotReadouts( Graphics& g, const Bitmap& bmp, const Rect& r, int width, int height )
{
   int w = bmp.Width();
   int h = bmp.Height();

   float hue = instance.UnshiftHueValue( m_readoutRGBWS.Hue( m_readouts[0], m_readouts[1], m_readouts[2] ) );
   int x = RoundInt( hue*(width - 1) ) - r.x0;

   float sat = m_readoutRGBWS.HSVSaturation( m_readouts[0], m_readouts[1], m_readouts[2] );
   int y = RoundInt( 0.5*height*(1 - sat) ) - r.y0;

   g.SetPen( m_channelColor, DisplayPixelRatio() );
   if ( x >= 0 && x < w )
      g.DrawLine( x, 0, x, h );
   if ( y >= 0 && y < h )
      g.DrawLine( 0, y, w, y );
}

void ColorSaturationInterface::PlotCursor( Graphics& g, const Rect& r )
{
   int w = r.Width();
   int h = r.Height();
   int x = m_cursorPos.x - r.x0;
   int y = m_cursorPos.y - r.y0;

   if ( m_mode == ZoomInMode || m_mode == ZoomOutMode || m_mode == PanMode )
   {
      String resource;
      switch ( m_mode )
      {
      case ZoomInMode:
         resource = ":/cursors/view/zoom_in.png";
         break;
      case ZoomOutMode:
         resource = ":/cursors/view/zoom_out.png";
         break;
      case PanMode:
         resource = m_panning ? ":/cursors/view/bidi_pan.png" : ":/cursors/view/pan.png";
         break;
      default: // ?!
         return;
      }

      Bitmap csr( ScaledResource( resource ) );
      g.DrawBitmap( x - (csr.Width() >> 1), y - (csr.Height() >> 1), csr );
   }
   else
   {
      g.SetPen( 0xffffffff, DisplayPixelRatio() );

      if ( x >= 0 && x < w )
         g.DrawLine( x, 0, x, h );
      if ( y >= 0 && y < h )
         g.DrawLine( 0, y, w, y );

      if ( m_mode == SelectMode )
      {
         int ui8 = LogicalPixelsToPhysical( 8 );
         g.SetBrush( Brush::Null() );
         g.DrawRect( x-ui8, y-ui8, x+ui8, y+ui8 );
      }
      else if ( m_mode == DeleteMode )
      {
         int ui8 = LogicalPixelsToPhysical( 8 );
         int ui5 = LogicalPixelsToPhysical( 5 );
         g.DrawLine( x-ui8, y-ui8, x-ui5, y-ui5 );
         g.DrawLine( x+ui8, y-ui8, x+ui5, y-ui5 );
         g.DrawLine( x-ui8, y+ui8, x-ui5, y+ui5 );
         g.DrawLine( x+ui8, y+ui8, x+ui5, y+ui5 );
      }
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ColorSaturationInterface::__Curve_Paint( Control& sender, const pcl::Rect& updateRect )
{
   if ( m_viewportDirty )
      RegenerateViewport();

   if ( m_readoutActive || m_cursorVisible )
   {
      Bitmap bmp = m_viewportBitmap.Subimage( updateRect );
      {
         Graphics g( bmp );
         g.EnableAntialiasing();
         g.SetCompositionOperator( CompositionOp::Difference );

         if ( m_readoutActive )
         {
            Rect r0 = sender.ClientRect();
            int w = r0.Width()*m_zoomX;
            int h = r0.Height()*m_zoomY;
            Rect r( updateRect + GUI->Curve_ScrollBox.ScrollPosition() );
            PlotReadouts( g, bmp, r, w, h );
         }

         if ( m_cursorVisible )
            PlotCursor( g, updateRect );
      }

      Graphics g( sender );
      g.DrawBitmap( updateRect.LeftTop(), bmp );
   }
   else
   {
      Graphics g( sender );
      g.DrawBitmapRect( updateRect.LeftTop(), m_viewportBitmap, updateRect );
   }
}

void ColorSaturationInterface::__Curve_Resize( Control& sender,
   int /*newWidth*/, int /*newHeight*/, int /*oldWidth*/, int /*oldHeight*/ )
{
   if ( sender == GUI->Curve_ScrollBox.Viewport() )
   {
      m_viewportBitmap = Bitmap::Null();
      m_viewportDirty = true;

      if ( !m_settingUp )
         SetZoom( m_zoomX, m_zoomY );
   }
}

void ColorSaturationInterface::__Curve_ScrollPosUpdated( ScrollBox& sender, int pos )
{
   if ( sender == GUI->Curve_ScrollBox )
      UpdateCurve();
}

void ColorSaturationInterface::__Curve_Enter( Control& sender )
{
   if ( sender == GUI->Curve_ScrollBox.Viewport() )
      m_cursorVisible = true;
   m_cursorPos = -1;
}

void ColorSaturationInterface::__Curve_Leave( Control& sender )
{
   m_cursorVisible = false;
   UpdateCurveInfo();
   sender.Update();
}

void ColorSaturationInterface::__Curve_MousePress(
   Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   m_cursorPos = pos;

   if ( button == MouseButton::Left )
   {
      switch ( m_mode )
      {
      case ZoomInMode:
      case ZoomOutMode:
         if ( sender == GUI->Curve_ScrollBox.Viewport() )
         {
            Point p = pos + GUI->Curve_ScrollBox.ScrollPosition();
            p.x /= m_zoomX;
            p.y /= m_zoomY;

            if ( m_mode == ZoomInMode )
               SetZoom( Min( m_zoomX+1, s_maxZoom ), Min( m_zoomY+1, s_maxZoom ), &p );
            else
               SetZoom( Max( 1, m_zoomX-1 ), Max( 1, m_zoomY-1 ), &p );
         }
         break;

      case PanMode:
         m_panOrigin = pos;
         ++m_panning;
         break;

      default:
         {
            Rect r = sender.ClientRect();
            int w = r.Width() * m_zoomX;
            int h = r.Height() * m_zoomY;
            int t = LogicalPixelsToPhysical( CURSOR_TOLERANCE );

            Point p = pos + GUI->Curve_ScrollBox.ScrollPosition();

            m_curvePos.x = Range( double( p.x )/(w - 1), 0.0, 1.0 );
            m_curvePos.y = ViewportToCurve( Range( (1 - double( p.y)/(h - 1)), -10.0, +10.0 ) );

            size_type i = ~size_type( 0 );

            switch ( m_mode )
            {
            case EditMode:
               if ( !modifiers )
               {
                  m_dragging = true;
                  i = CreatePoint( m_curvePos, int( t*1.5 ) );
               }
               break;

            case SelectMode:
               i = FindPoint( m_curvePos, t );
               break;

            case DeleteMode:
               i = RemovePoint( m_curvePos, t );
               break;

            default:
               break;
            }

            if ( i != ~size_type( 0 ) )
               SelectPoint( i, false );
         }
         break;
      }
   }
   else if ( button == MouseButton::Middle )
   {
      if ( m_savedMode == NoMode )
      {
         m_savedMode = m_mode;
         SetMode( PanMode );
         m_panOrigin = pos;
         ++m_panning;
      }
   }
   else if ( button == MouseButton::Right )
   {
      size_type i = RemovePoint( m_curvePos, LogicalPixelsToPhysical( CURSOR_TOLERANCE ) );
      if ( i != ~size_type( 0 ) )
         SelectPoint( i, false );
   }

   UpdateCurveInfo();
}

void ColorSaturationInterface::__Curve_MouseRelease(
   Control& /*sender*/, const pcl::Point& /*pos*/, int button, unsigned /*buttons*/, unsigned /*modifiers*/ )
{
   if ( button == MouseButton::Middle && m_savedMode != NoMode )
   {
      SetMode( m_savedMode );
      m_savedMode = NoMode;
   }

   m_panning = 0;
   m_dragging = false;
   UpdateCurveInfo();
}

void ColorSaturationInterface::__Curve_MouseMove(
   Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers )
{
   if ( m_cursorVisible )
   {
      Rect r = sender.ClientRect();
      int w = r.Width();
      int h = r.Height();

      for ( int i = 0; i < 2; ++i )
      {
         double f = DisplayPixelRatio();
         int ui1 = RoundInt( f );
         if ( m_mode == ZoomInMode || m_mode == ZoomOutMode || m_mode == PanMode )
         {
            int ui16 = RoundInt( f*16 );
            sender.Update( m_cursorPos.x-ui16-ui1, m_cursorPos.y-ui16-ui1, m_cursorPos.x+ui16+ui1, m_cursorPos.y+ui16+ui1 );
         }
         else
         {
            sender.Update( m_cursorPos.x-ui1-ui1, 0, m_cursorPos.x+ui1+ui1, h );
            sender.Update( 0, m_cursorPos.y-ui1-ui1, w, m_cursorPos.y+ui1+ui1 );
            if ( m_mode == SelectMode || m_mode == DeleteMode )
            {
               int ui10 = RoundInt( f*10 );
               sender.Update( m_cursorPos.x-ui10-ui1, m_cursorPos.y-ui10-ui1, m_cursorPos.x+ui10+ui1, m_cursorPos.y+ui10+ui1 );
            }
         }

         if ( i == 0 )
            m_cursorPos = pos;
      }

      w *= m_zoomX;
      h *= m_zoomY;

      Point p = pos + GUI->Curve_ScrollBox.ScrollPosition();

      m_curvePos.x = Range( double( p.x )/(w - 1), 0.0, 1.0 );
      m_curvePos.y = ViewportToCurve( Range( 1 - double( p.y )/(h - 1), -10.0, +10.0 ) );

      if ( m_dragging )
      {
         DragPoint( m_currentPoint, m_curvePos );
         UpdateCurveControls();
         UpdateCurve();
      }

      UpdateCurveInfo();
   }

   if ( m_panning )
   {
      Point d = m_panOrigin - pos;
      m_panOrigin = pos;
      GUI->Curve_ScrollBox.SetScrollPosition( GUI->Curve_ScrollBox.ScrollPosition() + d );
   }
}

void ColorSaturationInterface::__Curve_MouseWheel(
   Control& sender, const pcl::Point& pos, int delta, unsigned buttons, unsigned modifiers )
{
   if ( sender == GUI->Curve_ScrollBox.Viewport() )
   {
      m_wheelSteps += delta; // delta is rotation angle in 1/8 degree steps
      if ( Abs( m_wheelSteps ) >= WHEEL_STEP_ANGLE*8 )
      {
         Point p = pos + GUI->Curve_ScrollBox.ScrollPosition();
         p.x /= m_zoomX;
         p.y /= m_zoomY;
         int d = (delta > 0) ? -1 : +1;
         SetZoom( Range( m_zoomX+d, 1, s_maxZoom ), Range( m_zoomY+d, 1, s_maxZoom ), &p );
         m_wheelSteps = 0;
      }
   }
}

void ColorSaturationInterface::__Curve_KeyPress( Control& sender, int key, unsigned modifiers, bool& wantsKey )
{
   wantsKey = false;

   switch ( key )
   {
   case KeyCode::Left:
   case KeyCode::Right:
      if (
#if defined( __PCL_MACOSX )
         (modifiers & KeyModifier::Meta) != 0
#else
         (modifiers & KeyModifier::Control) != 0
#endif
         )
      {
         size_type i = CurrentPoint();

         switch ( key )
         {
         case KeyCode::Left:  if ( i > 0 ) --i; break;
         case KeyCode::Right: if ( i < instance.C.Length()-1 ) ++i; break;
         }

         SelectPoint( i );

         wantsKey = true;
         break;
      }
      // fall through
   case KeyCode::Up:
   case KeyCode::Down:
      {
         Rect r = GUI->Curve_ScrollBox.Viewport().ClientRect();
         int w = r.Width() * m_zoomX;
         int h = r.Height() * m_zoomY;
         double x = CurrentInputValue();
         double y = CurrentOutputValue();
         double dx = 1.0/w;
         double dy = double( m_scale )/h;

         switch ( key )
         {
         case KeyCode::Left:  x -= dx; break;
         case KeyCode::Right: x += dx; break;
         case KeyCode::Up:    y += dy; break;
         case KeyCode::Down:  y -= dy; break;
         }

         x = Range( x, 0.0, 1.0 );
         y = Range( y, -10.0, +10.0 );

         if ( x != CurrentInputValue() || y != CurrentOutputValue() )
         {
            DragPoint( CurrentPoint(), x, y );
            UpdateCurveControls();
            UpdateCurve();
         }
      }

      wantsKey = true;
      break;

   case KeyCode::Delete:
      {
         size_type i = RemovePoint( CurrentInputValue(), CurrentOutputValue(), 0 );
         if ( i != ~size_type( 0 ) )
            SelectPoint( i );
      }
      wantsKey = true;
      break;
   }
}

void ColorSaturationInterface::__CurveParameter_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Input_NumericEdit )
   {
      double x = Round( value, TheXHSParameter->Precision() );

      if ( x != CurrentInputValue() )
      {
         double d = 1/Pow10I<double>()( TheXHSParameter->Precision() );
         for ( size_type i = 0; i < instance.C.Length(); ++i )
            if ( i != CurrentPoint() && Abs( x - instance.C.X( i ) ) < d )
            {
               GUI->Input_NumericEdit.SetValue( CurrentInputValue() );
               pcl::MessageBox(
                  "Two curve points cannot have identical abscissae - previous X-value inserted.",
                  "ColorSaturation", StdIcon::Error ).Execute();
               return;
            }

         DPoint p( x, CurrentOutputValue() );

         instance.C.Remove( CurrentPoint() );
         instance.C.Add( p );

         m_currentPoint = instance.C.Search( x );
      }
   }
   else if ( sender == GUI->Output_NumericEdit )
   {
      CurrentOutputValue() = Round( value, TheYHSParameter->Precision() );
   }

   UpdateCurve();
   UpdateRealTimePreview();
}

void ColorSaturationInterface::__Mode_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->EditPointMode_ToolButton )
      SetMode( EditMode );
   else if ( sender == GUI->SelectPointMode_ToolButton )
      SetMode( SelectMode );
   else if ( sender == GUI->DeletePointMode_ToolButton )
      SetMode( DeleteMode );
   else if ( sender == GUI->ZoomInMode_ToolButton )
      SetMode( ZoomInMode );
   else if ( sender == GUI->ZoomOutMode_ToolButton )
      SetMode( ZoomOutMode );
   else if ( sender == GUI->PanMode_ToolButton )
      SetMode( PanMode );
}

void ColorSaturationInterface::__Zoom_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->Zoom11_ToolButton )
      SetZoom( 1, 1 );
}

void ColorSaturationInterface::__Zoom_ValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->Zoom_SpinBox )
      SetZoom( value, value );
}

void ColorSaturationInterface::__Scale_ValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->Scale_SpinBox )
      m_scale = value;
   UpdateCurve();
}

void ColorSaturationInterface::__ShowGrid_ButtonClick( Button& /*sender*/, bool checked )
{
   m_showGrid = checked;
   UpdateCurve();
}

void ColorSaturationInterface::__PointNavigation_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->PrevPoint_ToolButton )
   {
      if ( CurrentPoint() > 0 )
         SelectPoint( CurrentPoint() - 1 );
   }
   else if ( sender == GUI->FirstPoint_ToolButton )
   {
      if ( CurrentPoint() != 0 )
         SelectPoint( 0 );
   }
   else if ( sender == GUI->NextPoint_ToolButton )
   {
      if ( CurrentPoint() < instance.C.Length()-1 )
         SelectPoint( CurrentPoint()+1 );
   }
   else if ( sender == GUI->LastPoint_ToolButton )
   {
      if ( CurrentPoint() != instance.C.Length()-1 )
         SelectPoint( instance.C.Length()-1 );
   }
}

void ColorSaturationInterface::__Interpolation_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->AkimaSubsplines_ToolButton )
      SetInterpolation( CurveType::AkimaSubsplines );
   else if ( sender == GUI->CubicSpline_ToolButton )
      SetInterpolation( CurveType::CubicSpline );
   else if ( sender == GUI->Linear_ToolButton )
      SetInterpolation( CurveType::Linear );
}

void ColorSaturationInterface::__StoreCurve_ButtonClick( Button& /*sender*/, bool /*checked*/ )
{
   m_storedCurve = instance.C;
   UpdateCurveControls();
}

void ColorSaturationInterface::__RestoreCurve_ButtonClick( Button& /*sender*/, bool /*checked*/ )
{
   instance.C = m_storedCurve;
   m_currentPoint = 0;

   UpdateControls();
   UpdateCurve();
   UpdateRealTimePreview();
}

void ColorSaturationInterface::__ReverseCurve_ButtonClick( Button& /*sender*/, bool /*checked*/ )
{
   instance.C.Reverse();

   UpdateCurveControls();
   UpdateCurve();
   UpdateRealTimePreview();
}

void ColorSaturationInterface::__ResetCurve_ButtonClick( Button& /*sender*/, bool /*checked*/ )
{
   instance.C.Reset();

   m_currentPoint = 0;

   UpdateCurveControls();
   UpdateCurve();
   UpdateRealTimePreview();
}

void ColorSaturationInterface::__KeyPress( Control& sender, int key, unsigned modifiers, bool& wantsKey )
{
   bool modify =
#if defined( __PCL_MACOSX )
      (modifiers & KeyModifier::Shift) != 0;
#else
      (modifiers & KeyModifier::Alt) != 0;
#endif

   bool spaceBar = (modifiers & KeyModifier::SpaceBar) != 0;

   wantsKey = false;

   switch ( key )
   {
#if defined( __PCL_MACOSX )
   case KeyCode::Shift:
      if ( m_savedMode == NoMode )
      {
         m_savedMode = m_mode;
         SetMode( SelectMode );
         wantsKey = true;
      }
      else
#else
   case KeyCode::Alt:
#endif
      switch ( m_mode )
      {
      case ZoomInMode:
         if ( m_savedMode == NoMode || spaceBar )
         {
            SetMode( ZoomOutMode );
            if ( m_savedMode == NoMode )
               m_savedMode = m_mode;
            wantsKey = true;
         }
         break;

      case ZoomOutMode:
         if ( m_savedMode == NoMode )
         {
            m_savedMode = m_mode;
            SetMode( ZoomInMode );
            wantsKey = true;
         }
         break;

      default:
         break;
      }
      break;

#if !defined( __PCL_MACOSX )
   case KeyCode::Shift:
      if ( m_savedMode == NoMode )
      {
         m_savedMode = m_mode;
         SetMode( SelectMode );
         wantsKey = true;
      }
      break;
#endif

#if defined( __PCL_MACOSX )
   case KeyCode::Alt:
#else
   case KeyCode::Control:
#endif
      if ( spaceBar )
      {
         SetMode( modify ? ZoomOutMode : ZoomInMode );
         wantsKey = true;
      }
#if !defined( __PCL_MACOSX )
      else if ( m_savedMode == NoMode )
      {
         m_savedMode = m_mode;
         SetMode( DeleteMode );
         wantsKey = true;
      }
#endif
      break;

#if defined( __PCL_MACOSX )
   case KeyCode::Meta:
      if ( m_savedMode == NoMode )
      {
         m_savedMode = m_mode;
         SetMode( DeleteMode );
         wantsKey = true;
      }
      break;
#endif

   case KeyCode::Space:
      {
         bool change =
#if defined( __PCL_MACOSX )
         (modifiers & KeyModifier::Alt) != 0;
#else
         (modifiers & KeyModifier::Control) != 0;
#endif
         if ( m_savedMode == NoMode || change )
         {
            m_savedMode = change ? EditMode : m_mode;
            SetMode( change ? (modify ? ZoomOutMode : ZoomInMode) : PanMode );
         }
      }
      wantsKey = true;
      break;
   }
}

void ColorSaturationInterface::__KeyRelease( Control& sender, int key, unsigned modifiers, bool& wantsKey )
{
   bool spaceBar = (modifiers & KeyModifier::SpaceBar) != 0;

   wantsKey = false;

   switch ( key )
   {
#if defined( __PCL_MACOSX )
   case KeyCode::Shift:
      if ( m_savedMode != NoMode )
      {
         SetMode( m_savedMode );
         m_savedMode = NoMode;
         wantsKey = true;
      }
      else
#else
   case KeyCode::Alt:
#endif
      switch ( m_mode )
      {
      case ZoomInMode:
         if ( m_savedMode == ZoomOutMode )
         {
            SetMode( ZoomOutMode );
            wantsKey = true;
         }
         break;

      case ZoomOutMode:
         if ( m_savedMode == ZoomInMode || spaceBar )
         {
            SetMode( ZoomInMode );
            wantsKey = true;
         }
         break;

      default:
         break;
      }

      if ( !spaceBar )
         m_savedMode = NoMode;

      break;

#if !defined( __PCL_MACOSX )
   case KeyCode::Shift:
      if ( m_savedMode != NoMode )
      {
         SetMode( m_savedMode );
         m_savedMode = NoMode;
         wantsKey = true;
      }
      break;
#endif

#if defined( __PCL_MACOSX )
   case KeyCode::Alt:
#else
   case KeyCode::Control:
#endif
      if ( spaceBar )
      {
         SetMode( PanMode );
         wantsKey = true;
      }
      else if ( m_savedMode != NoMode )
      {
         SetMode( m_savedMode );
         m_savedMode = NoMode;
         wantsKey = true;
      }
      break;

#if defined( __PCL_MACOSX )
   case KeyCode::Meta:
      if ( m_savedMode != NoMode )
      {
         SetMode( m_savedMode );
         m_savedMode = NoMode;
         wantsKey = true;
      }
      break;
#endif

   case KeyCode::Space:
      if ( !spaceBar && m_savedMode != NoMode ) // avoid auto-repeat kbd. events
      {
         SetMode( m_savedMode );
         m_savedMode = NoMode;
         wantsKey = true;
      }
      break;
   }
}

void ColorSaturationInterface::__HueShift_ValueUpdated( NumericEdit& /*sender*/, double value )
{
   instance.hueShift = value;

   UpdateControls();
   UpdateCurve();
   UpdateRealTimePreview();
}

void ColorSaturationInterface::__UpdateRealTimePreview_Timer( Timer& sender )
{
   if ( m_realTimeThread != 0 )
      if ( m_realTimeThread->IsActive() )
         return;

   if ( IsRealTimePreviewActive() )
      RealTimePreview::Update();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

ColorSaturationInterface::GUIData::GUIData( ColorSaturationInterface& w )
{
   int ui16 = w.LogicalPixelsToPhysical( 16 );
   int ui24 = w.LogicalPixelsToPhysical( 24 );
   int labelWidth = w.Font().Width( String( "Saturation:" ) + 'M' );
   int editWidth1 = w.Font().Width( String( '0', 10 ) );
   int channelLabelWidth = ui24 + w.Font().Width( 'M' );
   int rgbkLabelWidth = ui24 + w.Font().Width( "RGB/K" );
   int curveMinWidth = 10*channelLabelWidth + rgbkLabelWidth;

   //

   Curve_ScrollBox.DisableAutoScroll();
   Curve_ScrollBox.SetMinSize( curveMinWidth, w.LogicalPixelsToPhysical( 225 ) );
   Curve_ScrollBox.OnHorizontalScrollPosUpdated( (ScrollBox::pos_event_handler)&ColorSaturationInterface::__Curve_ScrollPosUpdated, w );
   Curve_ScrollBox.OnVerticalScrollPosUpdated( (ScrollBox::pos_event_handler)&ColorSaturationInterface::__Curve_ScrollPosUpdated, w );
   Curve_ScrollBox.OnKeyPress( (Control::keyboard_event_handler)&ColorSaturationInterface::__Curve_KeyPress, w );

   Curve_ScrollBox.Viewport().SetCursor( StdCursor::NoCursor );
   Curve_ScrollBox.Viewport().OnPaint( (Control::paint_event_handler)&ColorSaturationInterface::__Curve_Paint, w );
   Curve_ScrollBox.Viewport().OnResize( (Control::resize_event_handler)&ColorSaturationInterface::__Curve_Resize, w );
   Curve_ScrollBox.Viewport().OnEnter( (Control::event_handler)&ColorSaturationInterface::__Curve_Enter, w );
   Curve_ScrollBox.Viewport().OnLeave( (Control::event_handler)&ColorSaturationInterface::__Curve_Leave, w );
   Curve_ScrollBox.Viewport().OnMouseMove( (Control::mouse_event_handler)&ColorSaturationInterface::__Curve_MouseMove, w );
   Curve_ScrollBox.Viewport().OnMousePress( (Control::mouse_button_event_handler)&ColorSaturationInterface::__Curve_MousePress, w );
   Curve_ScrollBox.Viewport().OnMouseRelease( (Control::mouse_button_event_handler)&ColorSaturationInterface::__Curve_MouseRelease, w );
   Curve_ScrollBox.Viewport().OnMouseWheel( (Control::mouse_wheel_event_handler)&ColorSaturationInterface::__Curve_MouseWheel, w );

   //

   EditPointMode_ToolButton.SetIcon( Bitmap( edit_point_mode_XPM ).ScaledToSize( ui16, ui16 ) );
   EditPointMode_ToolButton.SetScaledFixedSize( 20, 20 );
   EditPointMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   EditPointMode_ToolButton.SetToolTip( "Edit Point mode" );
   EditPointMode_ToolButton.SetCheckable();
   EditPointMode_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Mode_ButtonClick, w );

   SelectPointMode_ToolButton.SetIcon( Bitmap( select_point_mode_XPM ).ScaledToSize( ui16, ui16 ) );
   SelectPointMode_ToolButton.SetScaledFixedSize( 20, 20 );
   SelectPointMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   SelectPointMode_ToolButton.SetToolTip( "Select Point mode [Shift]" );
   SelectPointMode_ToolButton.SetCheckable();
   SelectPointMode_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Mode_ButtonClick, w );

   DeletePointMode_ToolButton.SetIcon( Bitmap( delete_point_mode_XPM ).ScaledToSize( ui16, ui16 ) );
   DeletePointMode_ToolButton.SetScaledFixedSize( 20, 20 );
   DeletePointMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   DeletePointMode_ToolButton.SetToolTip( "Delete Point mode [Ctrl]" );
   DeletePointMode_ToolButton.SetCheckable();
   DeletePointMode_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Mode_ButtonClick, w );

   ZoomInMode_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/toolbar/image-mode-zoom-in.png" ) ) );
   ZoomInMode_ToolButton.SetScaledFixedSize( 20, 20 );
   ZoomInMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ZoomInMode_ToolButton.SetToolTip( "Zoom In mode "
#ifdef __PCL_MACOSX
      "[Alt+SpaceBar]" );
#else
      "[Ctrl+SpaceBar]" );
#endif
   ZoomInMode_ToolButton.SetCheckable();
   ZoomInMode_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Mode_ButtonClick, w );

   ZoomOutMode_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/toolbar/image-mode-zoom-out.png" ) ) );
   ZoomOutMode_ToolButton.SetScaledFixedSize( 20, 20 );
   ZoomOutMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ZoomOutMode_ToolButton.SetToolTip( "Zoom Out mode "
#ifdef __PCL_MACOSX
      "[Shift+Alt+SpaceBar]" );
#else
      "[Shift+Ctrl+SpaceBar]" );
#endif
   ZoomOutMode_ToolButton.SetCheckable();
   ZoomOutMode_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Mode_ButtonClick, w );

   PanMode_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/toolbar/image-mode-pan.png" ) ) );
   PanMode_ToolButton.SetScaledFixedSize( 20, 20 );
   PanMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   PanMode_ToolButton.SetToolTip( "Pan mode [Spacebar]" );
   PanMode_ToolButton.SetCheckable();
   PanMode_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Mode_ButtonClick, w );

   Zoom_SpinBox.SetRange( 1, s_maxZoom );
   Zoom_SpinBox.SetToolTip( "Zoom" );
   Zoom_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ColorSaturationInterface::__Zoom_ValueUpdated, w );

   Zoom11_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/toolbar/view-zoom-1-1.png" ) ) );
   Zoom11_ToolButton.SetScaledFixedSize( 20, 20 );
   Zoom11_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   Zoom11_ToolButton.SetToolTip( "Zoom 1:1" );
   Zoom11_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Zoom_ButtonClick, w );

   Scale_Label.SetText( "Range:" );
   Scale_Label.SetToolTip( "Y-axis (saturation) range." );
   Scale_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   Scale_SpinBox.SetRange( 1, s_maxScale );
   Scale_SpinBox.SetToolTip( "Y-axis (saturation) range." );
   Scale_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ColorSaturationInterface::__Scale_ValueUpdated, w );

   ShowGrid_ToolButton.SetIcon( Bitmap( show_grid_XPM ).ScaledToSize( ui16, ui16 ) );
   ShowGrid_ToolButton.SetScaledFixedSize( 20, 20 );
   ShowGrid_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ShowGrid_ToolButton.SetToolTip( "Show grid" );
   ShowGrid_ToolButton.SetCheckable();
   ShowGrid_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__ShowGrid_ButtonClick, w );

   Mode_Sizer.SetSpacing( 4 );
   Mode_Sizer.Add( EditPointMode_ToolButton );
   Mode_Sizer.Add( SelectPointMode_ToolButton );
   Mode_Sizer.Add( DeletePointMode_ToolButton );
   Mode_Sizer.Add( ZoomInMode_ToolButton );
   Mode_Sizer.Add( ZoomOutMode_ToolButton );
   Mode_Sizer.Add( PanMode_ToolButton );
   Mode_Sizer.Add( Zoom_SpinBox );
   Mode_Sizer.Add( Zoom11_ToolButton );
   Mode_Sizer.AddStretch();
   Mode_Sizer.Add( Scale_Label );
   Mode_Sizer.Add( Scale_SpinBox );
   Mode_Sizer.AddStretch();
   Mode_Sizer.Add( ShowGrid_ToolButton );

   //

   Info_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   Info_Label.SetStyle( FrameStyle::Sunken );
   Info_Label.SetLineWidth( 1 );

   //

   Input_NumericEdit.label.SetText( "Hue:" );
   Input_NumericEdit.label.SetFixedWidth( labelWidth );
   Input_NumericEdit.SetReal();
   Input_NumericEdit.SetRange( 0, 1 );
   Input_NumericEdit.SetPrecision( TheXHSParameter->Precision() );
   Input_NumericEdit.SetToolTip( "Color dependency - Hue value" );
   Input_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ColorSaturationInterface::__CurveParameter_ValueUpdated, w );
   Input_NumericEdit.edit.SetFixedWidth( editWidth1 );

   PrevPoint_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/move-left.png" ) ) );
   PrevPoint_ToolButton.SetScaledFixedSize( 20, 20 );
   PrevPoint_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   PrevPoint_ToolButton.SetToolTip( "Previous point" );
   PrevPoint_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__PointNavigation_ButtonClick, w );

   FirstPoint_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/move-left-limit.png" ) ) );
   FirstPoint_ToolButton.SetScaledFixedSize( 20, 20 );
   FirstPoint_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   FirstPoint_ToolButton.SetToolTip( "First point" );
   FirstPoint_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__PointNavigation_ButtonClick, w );

   StoreCurve_ToolButton.SetIcon( w.ScaledResource( ":/icons/upload.png" ) );
   StoreCurve_ToolButton.SetScaledFixedSize( 20, 20 );
   StoreCurve_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   StoreCurve_ToolButton.SetToolTip( "Store curve" );
   StoreCurve_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__StoreCurve_ButtonClick, w );

   RestoreCurve_ToolButton.SetIcon( w.ScaledResource( ":/icons/download.png" ) );
   RestoreCurve_ToolButton.SetScaledFixedSize( 20, 20 );
   RestoreCurve_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   RestoreCurve_ToolButton.SetToolTip( "Restore curve" );
   RestoreCurve_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__RestoreCurve_ButtonClick, w );

   ReverseCurve_ToolButton.SetIcon( w.ScaledResource( ":/icons/picture-flip-vertical" ) );
   ReverseCurve_ToolButton.SetScaledFixedSize( 20, 20 );
   ReverseCurve_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ReverseCurve_ToolButton.SetToolTip( "Reverse curve" );
   ReverseCurve_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__ReverseCurve_ButtonClick, w );

   ResetCurve_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/delete.png" ) ) );
   ResetCurve_ToolButton.SetScaledFixedSize( 20, 20 );
   ResetCurve_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ResetCurve_ToolButton.SetToolTip( "Reset curve" );
   ResetCurve_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__ResetCurve_ButtonClick, w );

   CurveData_Row1_Sizer.SetSpacing( 4 );
   CurveData_Row1_Sizer.Add( Input_NumericEdit );
   CurveData_Row1_Sizer.Add( PrevPoint_ToolButton );
   CurveData_Row1_Sizer.Add( FirstPoint_ToolButton );
   CurveData_Row1_Sizer.AddStretch();
   CurveData_Row1_Sizer.Add( StoreCurve_ToolButton );
   CurveData_Row1_Sizer.Add( RestoreCurve_ToolButton );
   CurveData_Row1_Sizer.Add( ReverseCurve_ToolButton );
   CurveData_Row1_Sizer.Add( ResetCurve_ToolButton );

   //

   Output_NumericEdit.label.SetText( "Saturation:" );
   Output_NumericEdit.label.SetFixedWidth( labelWidth );
   Output_NumericEdit.SetReal();
   Output_NumericEdit.SetRange( -10, 10 );
   Output_NumericEdit.SetPrecision( TheYHSParameter->Precision() );
   Output_NumericEdit.SetToolTip( "Saturation factor" );
   Output_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&ColorSaturationInterface::__CurveParameter_ValueUpdated, w );
   Output_NumericEdit.edit.SetFixedWidth( editWidth1 );

   NextPoint_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/move-right.png" ) ) );
   NextPoint_ToolButton.SetScaledFixedSize( 20, 20 );
   NextPoint_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   NextPoint_ToolButton.SetToolTip( "Next point" );
   NextPoint_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__PointNavigation_ButtonClick, w );

   LastPoint_ToolButton.SetIcon( Bitmap( w.ScaledResource( ":/icons/move-right-limit.png" ) ) );
   LastPoint_ToolButton.SetScaledFixedSize( 20, 20 );
   LastPoint_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   LastPoint_ToolButton.SetToolTip( "Last point" );
   LastPoint_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__PointNavigation_ButtonClick, w );

   CurrentPoint_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   AkimaSubsplines_ToolButton.SetIcon( Bitmap( akima_interpolation_XPM ).ScaledToSize( ui16, ui16 ) );
   AkimaSubsplines_ToolButton.SetScaledFixedSize( 20, 20 );
   AkimaSubsplines_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   AkimaSubsplines_ToolButton.SetToolTip( "Akima subspline interpolation" );
   AkimaSubsplines_ToolButton.SetCheckable();
   AkimaSubsplines_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Interpolation_ButtonClick, w );

   CubicSpline_ToolButton.SetIcon( Bitmap( cubic_spline_interpolation_XPM ).ScaledToSize( ui16, ui16 ) );
   CubicSpline_ToolButton.SetScaledFixedSize( 20, 20 );
   CubicSpline_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   CubicSpline_ToolButton.SetToolTip( "Cubic spline interpolation" );
   CubicSpline_ToolButton.SetCheckable();
   CubicSpline_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Interpolation_ButtonClick, w );

   Linear_ToolButton.SetIcon( Bitmap( linear_interpolation_XPM ).ScaledToSize( ui16, ui16 ) );
   Linear_ToolButton.SetScaledFixedSize( 20, 20 );
   Linear_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   Linear_ToolButton.SetToolTip( "Linear interpolation" );
   Linear_ToolButton.SetCheckable();
   Linear_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Interpolation_ButtonClick, w );

   CurveData_Row2_Sizer.SetSpacing( 4 );
   CurveData_Row2_Sizer.Add( Output_NumericEdit );
   CurveData_Row2_Sizer.Add( NextPoint_ToolButton );
   CurveData_Row2_Sizer.Add( LastPoint_ToolButton );
   CurveData_Row2_Sizer.AddSpacing( 8 );
   CurveData_Row2_Sizer.Add( CurrentPoint_Label );
   CurveData_Row2_Sizer.AddStretch();
   CurveData_Row2_Sizer.Add( AkimaSubsplines_ToolButton );
   CurveData_Row2_Sizer.Add( CubicSpline_ToolButton );
   CurveData_Row2_Sizer.Add( Linear_ToolButton );

   //

   HueShift_NumericControl.label.SetText( "Hue shift:" );
   HueShift_NumericControl.label.SetFixedWidth( labelWidth );
   HueShift_NumericControl.slider.SetScaledMinWidth( 200 );
   HueShift_NumericControl.slider.SetRange( 0, 100 );
   HueShift_NumericControl.SetReal();
   HueShift_NumericControl.SetRange( TheHueShiftParameter->MinimumValue(), TheHueShiftParameter->MaximumValue() );
   HueShift_NumericControl.SetPrecision( TheHueShiftParameter->Precision() );
   HueShift_NumericControl.SetToolTip( "Move the origin of the hue axis." );
   HueShift_NumericControl.edit.SetFixedWidth( editWidth1 );
   HueShift_NumericControl.OnValueUpdated( (NumericEdit::value_event_handler)&ColorSaturationInterface::__HueShift_ValueUpdated, w );

   Global_Sizer.SetMargin( 4 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( Curve_ScrollBox );
   Global_Sizer.Add( Mode_Sizer );
   Global_Sizer.Add( Info_Label );
   Global_Sizer.Add( CurveData_Row1_Sizer );
   Global_Sizer.Add( CurveData_Row2_Sizer );
   Global_Sizer.Add( HueShift_NumericControl );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();

   //

   UpdateRealTimePreview_Timer.SetSingleShot();
   UpdateRealTimePreview_Timer.SetInterval( 0.025 );
   UpdateRealTimePreview_Timer.OnTimer( (Timer::timer_event_handler)&ColorSaturationInterface::__UpdateRealTimePreview_Timer, w );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF ColorSaturationInterface.cpp - Released 2017-07-09T18:07:33Z
