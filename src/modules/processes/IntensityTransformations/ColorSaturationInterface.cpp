// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard IntensityTransformations Process Module Version 01.07.00.0285
// ****************************************************************************
// ColorSaturationInterface.cpp - Released 2014/10/29 07:35:24 UTC
// ****************************************************************************
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "ColorSaturationInterface.h"
#include "ColorSaturationProcess.h"

#include <pcl/GlobalSettings.h>
#include <pcl/Graphics.h>
#include <pcl/ImageWindow.h>
#include <pcl/MessageBox.h>
#include <pcl/RealTimePreview.h>

namespace pcl
{

// ----------------------------------------------------------------------------

ColorSaturationInterface* TheColorSaturationInterface = 0;

// ----------------------------------------------------------------------------

#include "ColorSaturationIcon.xpm"
#include "zoom_in_cursor.xpm"
#include "zoom_out_cursor.xpm"
#include "pan_cursor.xpm"
#include "show_grid.xpm"
#include "store_curve.xpm"
#include "restore_curve.xpm"
#include "reverse.xpm"
#include "akima_interpolation.xpm"
#include "cubic_spline_interpolation.xpm"
#include "linear_interpolation.xpm"
#include "delete_point_mode.xpm"
#include "select_point_mode.xpm"
#include "edit_point_mode.xpm"

// ----------------------------------------------------------------------------

static const int maxZoom = 99;
static const int maxScale = 10;

// ----------------------------------------------------------------------------

#define CURSOR_TOLERANCE   PixInsightSettings::GlobalInteger( "ImageWindow/CursorTolerance" )

// ----------------------------------------------------------------------------

ColorSaturationInterface::ColorSaturationInterface() :
ProcessInterface(),
instance( TheColorSaturationProcess ), m_realTimeThread( 0 ), GUI( 0 ),
viewportBitmap( Bitmap::Null() ),
viewportDirty( true )
{
   TheColorSaturationInterface = this;

   mode = EditMode;
   savedMode = NoMode;

   currentPoint = 0;

   readoutActive = false;
   for ( int i = 0; i < 4; ++i )
      readouts[i] = 0;

   zoomX = zoomY = 1;
   scale = 1;

   showGrid = true;

   panning = 0;
   panOrigin = 0;

   cursorVisible = false;
   dragging = false;
   cursorPos = -1;
   curvePos = 0;

   channelColor = RGBAColor( 0xFF, 0xFF, 0x00 ); // H

   gridColor0      = RGBAColor( 0x50, 0x50, 0x50 );
   gridColor1      = RGBAColor( 0x37, 0x37, 0x37 );
   gridColor2      = RGBAColor( 0x60, 0x60, 0x60 );
   backgroundColor = RGBAColor( 0x00, 0x00, 0x00 );

   minCurveWidth = 300;
   minCurveHeight = 300;
   scaleSize = 8;

   settingUp = false;
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
   const ColorSaturationInstance* r = dynamic_cast<const ColorSaturationInstance*>( &p );

   if ( r == 0 )
   {
      whyNot = "Not a ColorSaturation instance.";
      return false;
   }

   whyNot.Clear();
   return true;
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

   currentPoint = 0;

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
      readoutActive = true;
}

// ----------------------------------------------------------------------------

void ColorSaturationInterface::UpdateReadout( const View& v, const DPoint& p, double R, double G, double B, double A )
{
   if ( readoutActive )
   {
      readouts[0] = R;
      readouts[1] = G;
      readouts[2] = B;
      readouts[3] = A;
      v.Window().GetRGBWS( readoutRGBWS );
      UpdateCurve();
   }
}

// ----------------------------------------------------------------------------

void ColorSaturationInterface::EndReadout( const View& v )
{
   if ( readoutActive )
   {
      readoutActive = false;
      UpdateCurve();
   }
}


// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

size_type ColorSaturationInterface::FindPoint( double x, double y, int tolerancePx ) const
{
   double delta = double( tolerancePx )/(GUI->Curve_ScrollBox.Viewport().Width()*zoomX - 1);

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
   double delta = float( tolerancePx )/(GUI->Curve_ScrollBox.Viewport().Width()*zoomX - 1);

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
   settingUp = true;

   zoomX = hz;
   zoomY = vz;

   bool hsb = zoomX > 1;
   bool vsb = zoomY > 1;

   GUI->Curve_ScrollBox.ShowScrollBars( hsb, vsb );

   int visibleWidth = GUI->Curve_ScrollBox.Viewport().Width();
   int visibleHeight = GUI->Curve_ScrollBox.Viewport().Height();

   int contentsWidth = visibleWidth * zoomX;
   int contentsHeight = visibleHeight * zoomY;

   if ( hsb )
   {
      int m = contentsWidth - visibleWidth;
      GUI->Curve_ScrollBox.SetHorizontalScrollRange( 0, m );

      if ( p != 0 )
         GUI->Curve_ScrollBox.SetHorizontalScrollPosition( Range( p->x*zoomX - (visibleWidth >> 1), 0, m ) );
   }
   else
      GUI->Curve_ScrollBox.SetHorizontalScrollRange( 0, 0 );

   GUI->Curve_ScrollBox.SetPageWidth( visibleWidth );

   if ( vsb )
   {
      int m = contentsHeight - visibleHeight;
      GUI->Curve_ScrollBox.SetVerticalScrollRange( 0, m );

      if ( p != 0 )
         GUI->Curve_ScrollBox.SetVerticalScrollPosition( Range( p->y*zoomY - (visibleHeight >> 1), 0, m ) );
   }
   else
      GUI->Curve_ScrollBox.SetVerticalScrollRange( 0, 0 );

   GUI->Curve_ScrollBox.SetPageHeight( visibleHeight );

   UpdateZoomControls();
   UpdateCurve();

   settingUp = false;
}

void ColorSaturationInterface::SetMode( working_mode m )
{
   mode = m;

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
   currentPoint = p;

   if ( pan && (zoomX != 1 || zoomY != 1) )
   {
      Rect r = GUI->Curve_ScrollBox.Viewport().ClientRect();
      int w1 = r.Width()*zoomX - 1;
      int h1 = r.Height()*zoomY - 1;
      GUI->Curve_ScrollBox.SetScrollPosition(
               Range( RoundI( CurrentInputValue()*w1 ) - r.Width()/2, 0, w1 ),
               Range( RoundI( (0.5 - CurrentOutputValue()/(2*scale))*h1 ) - r.Height()/2, 0, h1 ) );
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
   GUI->EditPointMode_ToolButton.SetChecked( mode == EditMode );
   GUI->SelectPointMode_ToolButton.SetChecked( mode == SelectMode );
   GUI->DeletePointMode_ToolButton.SetChecked( mode == DeleteMode );
   GUI->ZoomInMode_ToolButton.SetChecked( mode == ZoomInMode );
   GUI->ZoomOutMode_ToolButton.SetChecked( mode == ZoomOutMode );
   GUI->PanMode_ToolButton.SetChecked( mode == PanMode );
}

void ColorSaturationInterface::UpdateZoomControls()
{
   GUI->Zoom_SpinBox.SetValue( zoomX );
   GUI->Scale_SpinBox.SetValue( scale );
}

void ColorSaturationInterface::UpdateGraphicsControls()
{
   GUI->ShowGrid_ToolButton.SetChecked( showGrid );
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

   bool isFirstPoint = currentPoint == 0;
   bool isLastPoint = currentPoint == instance.C.Length()-1;

   GUI->Input_NumericEdit.Enable( !(isFirstPoint || isLastPoint) );

   GUI->PrevPoint_ToolButton.Enable( !isFirstPoint );
   GUI->FirstPoint_ToolButton.Enable( !isFirstPoint );

   GUI->NextPoint_ToolButton.Enable( !isLastPoint );
   GUI->LastPoint_ToolButton.Enable( !isLastPoint );

   GUI->CurrentPoint_Label.SetText( String().Format( "%u / %u",
                                    currentPoint + 1, instance.C.Length() ) );

   GUI->RestoreCurve_ToolButton.Enable( !storedCurve.IsIdentity() );
}

void ColorSaturationInterface::UpdateCurve()
{
   UpdateCurveInfo();

   viewportDirty = true;
   GUI->Curve_ScrollBox.Viewport().Update();
}

void ColorSaturationInterface::UpdateCurveInfo()
{
   if ( !cursorVisible )
   {
      GUI->Info_Label.Clear();
      return;
   }

   String s;

   switch ( mode )
   {
   default:
   case EditMode:    s = (dragging ? " Editing Point" : " Add/Edit Point"); break;
   case SelectMode:  s = " Select Point"; break;
   case DeleteMode:  s = " Delete Point"; break;
   case ZoomInMode:  s = " Zoom In"; break;
   case ZoomOutMode: s = " Zoom Out"; break;
   case PanMode:     s = " Pan"; break;
   }

   s.AppendFormat( " | x = %.5f | y = %.5f", curvePos.x, curvePos.y );

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

   if ( viewportBitmap.IsNull() )
      viewportBitmap = Bitmap( w0, h0, BitmapFormat::RGB32 );

   viewportDirty = false;

   viewportBitmap.Fill( backgroundColor );

   Rect r( r0 + GUI->Curve_ScrollBox.ScrollPosition() );
   int w = w0*zoomX;
   int h = h0*zoomY;

   if ( showGrid )
   {
      Graphics g( viewportBitmap );
      PlotScale( g, r, w, h );
      PlotGrid( g, r, w, h, zoomX, zoomY );
   }

   Bitmap bmp1( w0, h0 );
   bmp1.Fill( 0xFF000000 );

   Bitmap bmp( w0, h0 );
   bmp.Fill( 0xFF000000 );
   Graphics g( bmp );
   PlotCurve( g, r, w, h, zoomX, zoomY );
   bmp1.Or( bmp );

   viewportBitmap.Xor( bmp1 );
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

   Pen p0( gridColor0, 0, PenStyle::Solid );
   Pen p1( gridColor1, 0, PenStyle::Dot );
   Pen p2( gridColor2, 0, PenStyle::DashDot );

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

   g.SetPen( p2 );
   g.DrawLine( 0, height >> 1, w, height >> 1 );
}

static RGBA __ScaleColor_H( float f )
{
   RGBColorSystem::sample r, g, b;
   RGBColorSystem::HSVToRGB( r, g, b, f, 0.9, 0.9 );
   return RGBAColor( float( r ), float( g ), float( b ) );
}

static RGBA __ScaleColor_S( float f )
{
   RGBColorSystem::sample r, g, b;
   RGBColorSystem::HSVToRGB( r, g, b, 0.75, f, 0.9 );
   return RGBAColor( float( r ), float( g ), float( b ) );
}

void ColorSaturationInterface::PlotScale( Graphics& g, const Rect& r, int width, int height )
{
   int w = r.Width();
   int h = r.Height();

   int x0 = -r.x0;
   int y0 = height - 8 - r.y0;

   for ( int y = 0; y < h; ++y )
   {
      g.SetPen( __ScaleColor_S( 1 - float( y + r.y0 )/(height - 1) ) );
      g.DrawLine( x0, y, x0+7, y );
   }

   for ( int x = 0; x < w; ++x )
   {
      int diag = (x + r.x0 < 7) ? 7 - (x + r.x0) : 0;
      g.SetPen( __ScaleColor_H( instance.ShiftHueValue( float( x + r.x0 )/(width - 1) ) ) );
      g.DrawLine( x, y0+diag, x, y0+7 );
   }
}

void ColorSaturationInterface::PlotCurve( Graphics& g, const Rect& r,  int width, int height, int hZoom, int vZoom )
{
   const HSCurve& C = instance.C;

   g.SetPen( channelColor );

   double dx = 1.0/(width - 1);

   UnidimensionalInterpolation<double>* interpolator = C.InitInterpolator();

   if ( interpolator != 0 )
   {
      Array<Point> curve;

      for ( int w = r.Width(), x = 0; x < w; ++x )
         curve.Add( Point( x, RoundI( (height - 1)*(1 - CurveToViewport( (*interpolator)( (x + r.x0)*dx ) )) ) - r.y0 ) );

      delete interpolator, interpolator = 0;

      g.DrawPolyline( curve );
   }

   g.SetBrush( 0xFF000000 );

   for ( size_type i = 0; i < C.Length(); ++i )
   {
      float fx = C.XVector()[i];
      float fy = CurveToViewport( C.YVector()[i] );

      int x = RoundI( fx*(width - 1) ) - r.x0;
      int y = RoundI( (1 - fy)*(height - 1) ) - r.y0;

      if ( i == currentPoint )
         g.SetBrush( channelColor );

      g.DrawRect( x-2, (y-2), x+3, (y+3) );

      if ( i == currentPoint )
         g.SetBrush( 0xFF000000 );
   }
}

void ColorSaturationInterface::PlotReadouts( Bitmap& bmp, const Rect& r, int width, int height )
{
   int w = bmp.Width();
   int h = bmp.Height();

   float hue = instance.UnshiftHueValue( readoutRGBWS.Hue( readouts[0], readouts[1], readouts[2] ) );
   int x = RoundI( hue*(width - 1) ) - r.x0;

   float sat = readoutRGBWS.HSVSaturation( readouts[0], readouts[1], readouts[2] );
   int y = RoundI( 0.5*height*(1 - sat) ) - r.y0;

   if ( x >= 0 && x < w )
      bmp.Xor( Rect( x, 0, x+1, h ), channelColor );

   if ( y >= 0 && y < h )
      bmp.Xor( Rect( 0, y, w, y+1 ), channelColor );
}

void ColorSaturationInterface::PlotCursor( Bitmap& bmp, const Rect& r )
{
   int w = r.Width();
   int h = r.Height();
   int x = cursorPos.x - r.x0;
   int y = cursorPos.y - r.y0;

   if ( mode == ZoomInMode || mode == ZoomOutMode || mode == PanMode )
   {
      const char** xpm = 0; // calm compiler

      switch ( mode )
      {
      case ZoomInMode:  xpm = zoom_in_cursor_XPM; break;
      case ZoomOutMode: xpm = zoom_out_cursor_XPM; break;
      case PanMode:     xpm = pan_cursor_XPM; break;
      default: break;
      }

      Bitmap csr( xpm );
      bmp.Xor( Point( x - (csr.Width() >> 1), y - (csr.Height() >> 1) ), csr );
   }
   else
   {
      if ( x >= 0 && x < w )
         bmp.Invert( Rect( x, 0, x+1, h ) );
      if ( y >= 0 && y < h )
         bmp.Invert( Rect( 0, y, w, y+1 ) );

      if ( mode == SelectMode || mode == DeleteMode ) // != EditMode
      {
         Bitmap csr( 8+1+8, 8+1+8 );
         csr.Fill( 0xFF000000 );

         Graphics g( csr );
         g.SetPen( 0xFFFFFFFF );

         if ( mode == SelectMode )
         {
            g.SetBrush( Brush::Null() );
            g.DrawRect( 0, 0, 16+1, 16+1 );
         }
         else // DeleteMode
         {
            g.DrawLine(  0,  0,  3,  3 );
            g.DrawLine( 16,  0, 13,  3 );
            g.DrawLine( 16, 16, 13, 13 );
            g.DrawLine(  0, 16,  3, 13 );
         }

         bmp.Xor( Point( x-8, y-8 ), csr );
      }
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

void ColorSaturationInterface::__Curve_Paint( Control& sender, const pcl::Rect& updateRect )
{
   if ( viewportDirty )
      RegenerateViewport();

   if ( readoutActive || cursorVisible )
   {
      Bitmap bmp1 = viewportBitmap.Subimage( updateRect );

      Bitmap bmp( updateRect.Width(), updateRect.Height(), BitmapFormat::RGB32 );
      bmp.Fill( 0xFF000000 );

      if ( readoutActive )
      {
         Rect r0 = sender.ClientRect();
         int w = r0.Width()*zoomX;
         int h = r0.Height()*zoomY;

         Rect r( updateRect + GUI->Curve_ScrollBox.ScrollPosition() );

         PlotReadouts( bmp, r, w, h );
      }

      if ( cursorVisible )
         PlotCursor( bmp, updateRect );

      bmp1.Xor( bmp );

      Graphics g( sender );
      g.DrawBitmap( updateRect.LeftTop(), bmp1 );
   }
   else
   {
      Graphics g( sender );
      g.DrawBitmapRect( updateRect.LeftTop(), viewportBitmap, updateRect );
   }
}

void ColorSaturationInterface::__Curve_Resize( Control& sender,
   int /*newWidth*/, int /*newHeight*/, int /*oldWidth*/, int /*oldHeight*/ )
{
   if ( sender == GUI->Curve_ScrollBox.Viewport() )
   {
      viewportBitmap = Bitmap::Null();
      viewportDirty = true;

      if ( !settingUp )
         SetZoom( zoomX, zoomY );
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
      cursorVisible = true;
   cursorPos = -1;
}

void ColorSaturationInterface::__Curve_Leave( Control& sender )
{
   cursorVisible = false;
   UpdateCurveInfo();
   sender.Update();
}

void ColorSaturationInterface::__Curve_MousePress(
   Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
{
   cursorPos = pos;

   if ( button == MouseButton::Left )
   {
      switch ( mode )
      {
      case ZoomInMode:
      case ZoomOutMode:
         if ( sender == GUI->Curve_ScrollBox.Viewport() )
         {
            Point p = pos + GUI->Curve_ScrollBox.ScrollPosition();
            p.x /= zoomX;
            p.y /= zoomY;

            if ( mode == ZoomInMode )
               SetZoom( Min( zoomX+1, maxZoom ), Min( zoomY+1, maxZoom ), &p );
            else
               SetZoom( Max( 1, zoomX-1 ), Max( 1, zoomY-1 ), &p );
         }
         break;

      case PanMode:
         panOrigin = pos;
         ++panning;
         break;

      default:
         {
            Rect r = sender.ClientRect();
            int w = r.Width() * zoomX;
            int h = r.Height() * zoomY;

            Point p = pos + GUI->Curve_ScrollBox.ScrollPosition();

            curvePos.x = Range( double( p.x )/(w - 1), 0.0, 1.0 );
            curvePos.y = ViewportToCurve( Range( (1 - double( p.y)/(h - 1)), -10.0, +10.0 ) );

            size_type i = ~size_type( 0 );

            switch ( mode )
            {
            case EditMode:
               if ( !modifiers )
               {
                  dragging = true;
                  i = CreatePoint( curvePos, int( CURSOR_TOLERANCE*1.5 ) );
               }
               break;

            case SelectMode:
               i = FindPoint( curvePos, CURSOR_TOLERANCE );
               break;

            case DeleteMode:
               i = RemovePoint( curvePos, CURSOR_TOLERANCE );
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
      if ( savedMode == NoMode )
      {
         savedMode = mode;
         SetMode( PanMode );
         panOrigin = pos;
         ++panning;
      }
   }
   else if ( button == MouseButton::Right )
   {
      size_type i = RemovePoint( curvePos, CURSOR_TOLERANCE );
      if ( i != ~size_type( 0 ) )
         SelectPoint( i, false );
   }

   UpdateCurveInfo();
}

void ColorSaturationInterface::__Curve_MouseRelease(
   Control& /*sender*/, const pcl::Point& /*pos*/, int button, unsigned /*buttons*/, unsigned /*modifiers*/ )
{
   if ( button == MouseButton::Middle && savedMode != NoMode )
   {
      SetMode( savedMode );
      savedMode = NoMode;
   }

   panning = 0;
   dragging = false;
   UpdateCurveInfo();
}

void ColorSaturationInterface::__Curve_MouseMove(
   Control& sender, const pcl::Point& pos, unsigned buttons, unsigned modifiers )
{
   if ( cursorVisible )
   {
      Rect r = sender.ClientRect();
      int w = r.Width();
      int h = r.Height();

      for ( int i = 0; i < 2; ++i )
      {
         if ( mode == ZoomInMode || mode == ZoomOutMode || mode == PanMode )
            sender.Update( cursorPos.x-16, cursorPos.y-16, cursorPos.x+16+1, cursorPos.y+16+1 );
         else
         {
            sender.Update( cursorPos.x, 0, cursorPos.x+1, h );
            sender.Update( 0, cursorPos.y, w, cursorPos.y+1 );

            if ( mode == SelectMode || mode == DeleteMode )
               sender.Update( cursorPos.x-8, cursorPos.y-8, cursorPos.x+8+1, cursorPos.y+8+1 );
         }

         if ( i == 0 )
            cursorPos = pos;
      }

      w *= zoomX;
      h *= zoomY;

      Point p = pos + GUI->Curve_ScrollBox.ScrollPosition();

      curvePos.x = Range( double( p.x )/(w - 1), 0.0, 1.0 );
      curvePos.y = ViewportToCurve( Range( 1 - double( p.y )/(h - 1), -10.0, +10.0 ) );

      if ( dragging )
      {
         DragPoint( currentPoint, curvePos );
         UpdateCurveControls();
         UpdateCurve();
      }

      UpdateCurveInfo();
   }

   if ( panning )
   {
      Point d = panOrigin - pos;
      panOrigin = pos;
      GUI->Curve_ScrollBox.SetScrollPosition( GUI->Curve_ScrollBox.ScrollPosition() + d );
   }
}

void ColorSaturationInterface::__Curve_MouseWheel(
   Control& sender, const pcl::Point& pos, int delta, unsigned buttons, unsigned modifiers )
{
   int d = (delta > 0) ? -1 : +1;

   if ( sender == GUI->Curve_ScrollBox.Viewport() )
   {
      Point p = pos + GUI->Curve_ScrollBox.ScrollPosition();
      p.x /= zoomX;
      p.y /= zoomY;
      SetZoom( Range( zoomX+d, 1, maxZoom ), Range( zoomY+d, 1, maxZoom ), &p );
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
         int w = r.Width() * zoomX;
         int h = r.Height() * zoomY;
         double x = CurrentInputValue();
         double y = CurrentOutputValue();
         double dx = 1.0/w;
         double dy = double( scale )/h;

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

         currentPoint = instance.C.Search( x );
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
      scale = value;
   UpdateCurve();
}

void ColorSaturationInterface::__ShowGrid_ButtonClick( Button& /*sender*/, bool checked )
{
   showGrid = checked;
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
   storedCurve = instance.C;
   UpdateCurveControls();
}

void ColorSaturationInterface::__RestoreCurve_ButtonClick( Button& /*sender*/, bool /*checked*/ )
{
   instance.C = storedCurve;
   currentPoint = 0;

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

   currentPoint = 0;

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
      if ( savedMode == NoMode )
      {
         savedMode = mode;
         SetMode( SelectMode );
         wantsKey = true;
      }
      else
#else
   case KeyCode::Alt:
#endif
      switch ( mode )
      {
      case ZoomInMode:
         if ( savedMode == NoMode || spaceBar )
         {
            SetMode( ZoomOutMode );
            if ( savedMode == NoMode )
               savedMode = mode;
            wantsKey = true;
         }
         break;

      case ZoomOutMode:
         if ( savedMode == NoMode )
         {
            savedMode = mode;
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
      if ( savedMode == NoMode )
      {
         savedMode = mode;
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
      else if ( savedMode == NoMode )
      {
         savedMode = mode;
         SetMode( DeleteMode );
         wantsKey = true;
      }
#endif
      break;

#if defined( __PCL_MACOSX )
   case KeyCode::Meta:
      if ( savedMode == NoMode )
      {
         savedMode = mode;
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
         if ( savedMode == NoMode || change )
         {
            savedMode = change ? EditMode : mode;
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
      if ( savedMode != NoMode )
      {
         SetMode( savedMode );
         savedMode = NoMode;
         wantsKey = true;
      }
      else
#else
   case KeyCode::Alt:
#endif
      switch ( mode )
      {
      case ZoomInMode:
         if ( savedMode == ZoomOutMode )
         {
            SetMode( ZoomOutMode );
            wantsKey = true;
         }
         break;

      case ZoomOutMode:
         if ( savedMode == ZoomInMode || spaceBar )
         {
            SetMode( ZoomInMode );
            wantsKey = true;
         }
         break;

      default:
         break;
      }

      if ( !spaceBar )
         savedMode = NoMode;

      break;

#if !defined( __PCL_MACOSX )
   case KeyCode::Shift:
      if ( savedMode != NoMode )
      {
         SetMode( savedMode );
         savedMode = NoMode;
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
      else if ( savedMode != NoMode )
      {
         SetMode( savedMode );
         savedMode = NoMode;
         wantsKey = true;
      }
      break;

#if defined( __PCL_MACOSX )
   case KeyCode::Meta:
      if ( savedMode != NoMode )
      {
         SetMode( savedMode );
         savedMode = NoMode;
         wantsKey = true;
      }
      break;
#endif

   case KeyCode::Space:
      if ( !spaceBar && savedMode != NoMode ) // avoid auto-repeat kbd. events
      {
         SetMode( savedMode );
         savedMode = NoMode;
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
   int labelWidth = w.Font().Width( String( "Saturation:" ) + 'M' );
   int editWidth1 = w.Font().Width( String( '0', 10 ) );

   // Get exactly the same curve minimum width as we calculate for CurvesTransformationInterface
   int channelLabelWidth = 19 + w.Font().Width( 'M' ) +
#if defined( __PCL_WINDOWS ) || defined( __PCL_MACOSX )
      + 5;
#else
      + 4;
#endif
   int rgbkLabelWidth = 19 + w.Font().Width( "RGB/K" )
#if defined( __PCL_WINDOWS ) || defined( __PCL_MACOSX )
      + 10;
#else
      + 5;
#endif

   int curveMinWidth = 10*channelLabelWidth + rgbkLabelWidth;

   //

   Curve_ScrollBox.DisableAutoScroll();
   Curve_ScrollBox.SetMinSize( curveMinWidth, 225 );
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

   EditPointMode_ToolButton.SetIcon( Bitmap( edit_point_mode_XPM ) );
   EditPointMode_ToolButton.SetFixedSize( 20, 20 );
   EditPointMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   EditPointMode_ToolButton.SetToolTip( "Edit Point mode" );
   EditPointMode_ToolButton.SetCheckable();
   EditPointMode_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Mode_ButtonClick, w );

   SelectPointMode_ToolButton.SetIcon( Bitmap( select_point_mode_XPM ) );
   SelectPointMode_ToolButton.SetFixedSize( 20, 20 );
   SelectPointMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   SelectPointMode_ToolButton.SetToolTip( "Select Point mode [Shift]" );
   SelectPointMode_ToolButton.SetCheckable();
   SelectPointMode_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Mode_ButtonClick, w );

   DeletePointMode_ToolButton.SetIcon( Bitmap( delete_point_mode_XPM ) );
   DeletePointMode_ToolButton.SetFixedSize( 20, 20 );
   DeletePointMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   DeletePointMode_ToolButton.SetToolTip( "Delete Point mode [Ctrl]" );
   DeletePointMode_ToolButton.SetCheckable();
   DeletePointMode_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Mode_ButtonClick, w );

   ZoomInMode_ToolButton.SetIcon( Bitmap( ":/toolbar/image-mode-zoom-in.png" ) );
   ZoomInMode_ToolButton.SetFixedSize( 20, 20 );
   ZoomInMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ZoomInMode_ToolButton.SetToolTip( "Zoom In mode "
#ifdef __PCL_MACOSX
      "[Alt+SpaceBar]" );
#else
      "[Ctrl+SpaceBar]" );
#endif
   ZoomInMode_ToolButton.SetCheckable();
   ZoomInMode_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Mode_ButtonClick, w );

   ZoomOutMode_ToolButton.SetIcon( Bitmap( ":/toolbar/image-mode-zoom-out.png" ) );
   ZoomOutMode_ToolButton.SetFixedSize( 20, 20 );
   ZoomOutMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ZoomOutMode_ToolButton.SetToolTip( "Zoom Out mode "
#ifdef __PCL_MACOSX
      "[Shift+Alt+SpaceBar]" );
#else
      "[Shift+Ctrl+SpaceBar]" );
#endif
   ZoomOutMode_ToolButton.SetCheckable();
   ZoomOutMode_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Mode_ButtonClick, w );

   PanMode_ToolButton.SetIcon( Bitmap( ":/toolbar/image-mode-pan.png" ) );
   PanMode_ToolButton.SetFixedSize( 20, 20 );
   PanMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   PanMode_ToolButton.SetToolTip( "Pan mode [Spacebar]" );
   PanMode_ToolButton.SetCheckable();
   PanMode_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Mode_ButtonClick, w );

   Zoom_SpinBox.SetRange( 1, maxZoom );
   Zoom_SpinBox.SetToolTip( "Zoom" );
   Zoom_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ColorSaturationInterface::__Zoom_ValueUpdated, w );

   Zoom11_ToolButton.SetIcon( Bitmap( ":/toolbar/view-zoom-1-1.png" ) );
   Zoom11_ToolButton.SetFixedSize( 20, 20 );
   Zoom11_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   Zoom11_ToolButton.SetToolTip( "Zoom 1:1" );
   Zoom11_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Zoom_ButtonClick, w );

   Scale_Label.SetText( "Range:" );
   Scale_Label.SetToolTip( "Y-axis (saturation) range." );
   Scale_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   Scale_SpinBox.SetRange( 1, maxScale );
   Scale_SpinBox.SetToolTip( "Y-axis (saturation) range." );
   Scale_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&ColorSaturationInterface::__Scale_ValueUpdated, w );

   ShowGrid_ToolButton.SetIcon( Bitmap( show_grid_XPM ) );
   ShowGrid_ToolButton.SetFixedSize( 20, 20 );
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

   PrevPoint_ToolButton.SetIcon( Bitmap( ":/icons/move-left.png" ) );
   PrevPoint_ToolButton.SetFixedSize( 20, 20 );
   PrevPoint_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   PrevPoint_ToolButton.SetToolTip( "Previous point" );
   PrevPoint_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__PointNavigation_ButtonClick, w );

   FirstPoint_ToolButton.SetIcon( Bitmap( ":/icons/move-left-limit.png" ) );
   FirstPoint_ToolButton.SetFixedSize( 20, 20 );
   FirstPoint_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   FirstPoint_ToolButton.SetToolTip( "First point" );
   FirstPoint_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__PointNavigation_ButtonClick, w );

   StoreCurve_ToolButton.SetIcon( Bitmap( store_curve_XPM ) );
   StoreCurve_ToolButton.SetFixedSize( 20, 20 );
   StoreCurve_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   StoreCurve_ToolButton.SetToolTip( "Store curve" );
   StoreCurve_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__StoreCurve_ButtonClick, w );

   RestoreCurve_ToolButton.SetIcon( Bitmap( restore_curve_XPM ) );
   RestoreCurve_ToolButton.SetFixedSize( 20, 20 );
   RestoreCurve_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   RestoreCurve_ToolButton.SetToolTip( "Restore curve" );
   RestoreCurve_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__RestoreCurve_ButtonClick, w );

   ReverseCurve_ToolButton.SetIcon( Bitmap( reverse_XPM ) );
   ReverseCurve_ToolButton.SetFixedSize( 20, 20 );
   ReverseCurve_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ReverseCurve_ToolButton.SetToolTip( "Reverse curve" );
   ReverseCurve_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__ReverseCurve_ButtonClick, w );

   ResetCurve_ToolButton.SetIcon( Bitmap( String( ":/icons/delete.png" ) ) );
   ResetCurve_ToolButton.SetFixedSize( 20, 20 );
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

   NextPoint_ToolButton.SetIcon( Bitmap( ":/icons/move-right.png" ) );
   NextPoint_ToolButton.SetFixedSize( 20, 20 );
   NextPoint_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   NextPoint_ToolButton.SetToolTip( "Next point" );
   NextPoint_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__PointNavigation_ButtonClick, w );

   LastPoint_ToolButton.SetIcon( Bitmap( ":/icons/move-right-limit.png" ) );
   LastPoint_ToolButton.SetFixedSize( 20, 20 );
   LastPoint_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   LastPoint_ToolButton.SetToolTip( "Last point" );
   LastPoint_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__PointNavigation_ButtonClick, w );

   CurrentPoint_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   AkimaSubsplines_ToolButton.SetIcon( Bitmap( akima_interpolation_XPM ) );
   AkimaSubsplines_ToolButton.SetFixedSize( 20, 20 );
   AkimaSubsplines_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   AkimaSubsplines_ToolButton.SetToolTip( "Akima subspline interpolation" );
   AkimaSubsplines_ToolButton.SetCheckable();
   AkimaSubsplines_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Interpolation_ButtonClick, w );

   CubicSpline_ToolButton.SetIcon( Bitmap( cubic_spline_interpolation_XPM ) );
   CubicSpline_ToolButton.SetFixedSize( 20, 20 );
   CubicSpline_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   CubicSpline_ToolButton.SetToolTip( "Cubic spline interpolation" );
   CubicSpline_ToolButton.SetCheckable();
   CubicSpline_ToolButton.OnClick( (ToolButton::click_event_handler)&ColorSaturationInterface::__Interpolation_ButtonClick, w );

   Linear_ToolButton.SetIcon( Bitmap( linear_interpolation_XPM ) );
   Linear_ToolButton.SetFixedSize( 20, 20 );
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
   HueShift_NumericControl.slider.SetMinWidth( 200 );
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

// ****************************************************************************
// EOF ColorSaturationInterface.cpp - Released 2014/10/29 07:35:24 UTC
