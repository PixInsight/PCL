//     ____   ______ __
//    / __ \ / ____// /
//   / /_/ // /    / /
//  / ____// /___ / /___   PixInsight Class Library
// /_/     \____//_____/   PCL 02.01.11.0937
// ----------------------------------------------------------------------------
// Standard IntensityTransformations Process Module Version 01.07.01.0424
// ----------------------------------------------------------------------------
// CurvesTransformationInterface.cpp - Released 2018-12-12T09:25:25Z
// ----------------------------------------------------------------------------
// This file is part of the standard IntensityTransformations PixInsight module.
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

#include "CurvesTransformationInterface.h"
#include "CurvesTransformationProcess.h"
#include "HistogramTransformationInterface.h"

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

CurvesTransformationInterface* TheCurvesTransformationInterface = 0;

// ----------------------------------------------------------------------------

#include "CurvesTransformationIcon.xpm"
#include "show_all_curves.xpm"
#include "show_grid.xpm"
#include "akima_interpolation.xpm"
#include "cubic_spline_interpolation.xpm"
#include "linear_interpolation.xpm"
#include "delete_point_mode.xpm"
#include "select_point_mode.xpm"
#include "edit_point_mode.xpm"

// ----------------------------------------------------------------------------

static const int s_maxZoom = 99;

// ----------------------------------------------------------------------------

CurvesTransformationInterface::CurvesTransformationInterface() :
   ProcessInterface(),
   m_instance( TheCurvesTransformationProcess ),
   m_realTimeThread( 0 ),
   GUI( 0 ),
   m_mode( EditMode ),
   m_savedMode( NoMode ),
   m_channel( CurveIndex::RGBK ),
   m_currentPoint( 0, CurveIndex::NumberOfCurves ),
   m_histograms(),
   m_histogramView( View::Null() ),
   m_histogramColor( false ),
   m_readoutActive( false ),
   m_readouts( 0.0, 4 ),
   m_readoutRGBWS(),
   m_zoomX( 1 ),
   m_zoomY( 1 ),
   m_wheelSteps( 0 ),
   m_showAllCurves( true ),
   m_showGrid( true ),
   m_panning( 0 ),
   m_panOrigin( 0 ),
   m_cursorVisible( false ),
   m_dragging( false ),
   m_cursorPos( -1 ),
   m_curvePos( 0 ),
   m_viewportBitmap( Bitmap::Null() ),
   m_viewportDirty( true ),
   m_storedCurve(),
   m_channelColor( CurveIndex::NumberOfCurves ),
   m_settingUp( false )
{
   TheCurvesTransformationInterface = this;

   m_channelColor[CurveIndex::R   ] = RGBAColor( 0xFF, 0x20, 0x20 );
   m_channelColor[CurveIndex::G   ] = RGBAColor( 0x00, 0xFF, 0x00 );
   m_channelColor[CurveIndex::B   ] = RGBAColor( 0x00, 0xA0, 0xFF );
   m_channelColor[CurveIndex::RGBK] = RGBAColor( 0xE0, 0xE0, 0xE0 );
   m_channelColor[CurveIndex::A   ] = RGBAColor( 0xFF, 0xFF, 0xFF );
   m_channelColor[CurveIndex::L   ] = RGBAColor( 0xFF, 0xFF, 0xFF );
   m_channelColor[CurveIndex::a   ] = RGBAColor( 0xA0, 0xA0, 0xFF );
   m_channelColor[CurveIndex::b   ] = RGBAColor( 0xFF, 0x40, 0x80 );
   m_channelColor[CurveIndex::c   ] = RGBAColor( 0xFF, 0x80, 0x00 );
   m_channelColor[CurveIndex::H   ] = RGBAColor( 0xFF, 0xFF, 0x00 );
   m_channelColor[CurveIndex::S   ] = RGBAColor( 0xFF, 0xC0, 0xFF );
   m_gridColor0                     = RGBAColor( 0x50, 0x50, 0x50 );
   m_gridColor1                     = RGBAColor( 0x37, 0x37, 0x37 );
   m_backgroundColor                = RGBAColor( 0x00, 0x00, 0x00 );
}

CurvesTransformationInterface::~CurvesTransformationInterface()
{
   if ( GUI != 0 )
      delete GUI, GUI = 0;
}

// ----------------------------------------------------------------------------

IsoString CurvesTransformationInterface::Id() const
{
   return "CurvesTransformation";
}

// ----------------------------------------------------------------------------

MetaProcess* CurvesTransformationInterface::Process() const
{
   return TheCurvesTransformationProcess;
}

// ----------------------------------------------------------------------------

const char** CurvesTransformationInterface::IconImageXPM() const
{
   return CurvesTransformationIcon_XPM;
}

// ----------------------------------------------------------------------------

InterfaceFeatures CurvesTransformationInterface::Features() const
{
   return InterfaceFeature::Default | InterfaceFeature::RealTimeButton | InterfaceFeature::TrackViewButton;
}

// ----------------------------------------------------------------------------

void CurvesTransformationInterface::ApplyInstance() const
{
   m_instance.LaunchOnCurrentView();
}

// ----------------------------------------------------------------------------

void CurvesTransformationInterface::RealTimePreviewUpdated( bool active )
{
   if ( GUI != 0 )
      if ( active )
         RealTimePreview::SetOwner( *this ); // implicitly updates the r-t preview
      else
         RealTimePreview::SetOwner( ProcessInterface::Null() );
}

// ----------------------------------------------------------------------------

void CurvesTransformationInterface::TrackViewUpdated( bool active )
{
   if ( GUI != 0 )
      if ( active )
      {
         ImageWindow w = ImageWindow::ActiveWindow();
         if ( !w.IsNull() )
            ImageFocused( w.CurrentView() );
      }
      else
      {
         if ( !m_histograms.IsEmpty() )
         {
            m_histograms.Clear();
            m_histogramView = View::Null();
            m_histogramColor = false;
            UpdateCurve();
         }
      }
}

// ----------------------------------------------------------------------------

void CurvesTransformationInterface::ResetInstance()
{
   CurvesTransformationInstance defaultInstance( TheCurvesTransformationProcess );
   ImportProcess( defaultInstance );
}

// ----------------------------------------------------------------------------

bool CurvesTransformationInterface::Launch( const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/ )
{
   if ( GUI == 0 )
   {
      GUI = new GUIData( *this );
      SetWindowTitle( "CurvesTransformation" );
      OnKeyPress( (Control::keyboard_event_handler)&CurvesTransformationInterface::__KeyPress, *this );
      OnKeyRelease( (Control::keyboard_event_handler)&CurvesTransformationInterface::__KeyRelease, *this );
      UpdateControls();
   }

   dynamic = false;
   return &P == TheCurvesTransformationProcess;
}

// ----------------------------------------------------------------------------

ProcessImplementation* CurvesTransformationInterface::NewProcess() const
{
   return new CurvesTransformationInstance( m_instance );
}

// ----------------------------------------------------------------------------

bool CurvesTransformationInterface::ValidateProcess( const ProcessImplementation& p, pcl::String& whyNot ) const
{
   if ( dynamic_cast<const CurvesTransformationInstance*>( &p ) != nullptr )
      return true;
   whyNot = "Not a CurvesTransformation instance.";
   return false;
}

// ----------------------------------------------------------------------------

bool CurvesTransformationInterface::RequiresInstanceValidation() const
{
   return true;
}

// ----------------------------------------------------------------------------

bool CurvesTransformationInterface::ImportProcess( const ProcessImplementation& p )
{
   m_instance.Assign( p );

   for ( int i = 0; i < CurveIndex::NumberOfCurves; ++i )
      m_currentPoint[i] = 0;

   UpdateControls();
   UpdateCurve();
   UpdateRealTimePreview();
   return true;
}

// ----------------------------------------------------------------------------

bool CurvesTransformationInterface::RequiresRealTimePreviewUpdate( const UInt16Image&, const View&, int/*zoomLevel*/ ) const
{
   return true;
}

// ----------------------------------------------------------------------------

CurvesTransformationInterface::RealTimeThread::RealTimeThread() :
Thread(), m_instance( TheCurvesTransformationProcess )
{
}

void CurvesTransformationInterface::RealTimeThread::Reset( const UInt16Image& image,
                                                           const CurvesTransformationInstance& instance )
{
   image.ResetSelections();
   m_image.Assign( image );
   m_instance.Assign( instance );
}

void CurvesTransformationInterface::RealTimeThread::Run()
{
   m_instance.Preview( m_image );
}

bool CurvesTransformationInterface::GenerateRealTimePreview( UInt16Image& image, const View&, int, String& ) const
{
   m_realTimeThread = new RealTimeThread;

   for ( ;; )
   {
      m_realTimeThread->Reset( image, m_instance );
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

bool CurvesTransformationInterface::WantsImageNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void CurvesTransformationInterface::ImageUpdated( const View& view )
{
   if ( GUI != 0 )
      if ( IsTrackViewActive() )
         if ( view == m_histogramView )
         {
            if ( !m_histogramView.HasProperty( "Histogram16" ) )
               m_histogramView.ComputeProperty( "Histogram16", false/*notify*/ );
            m_histogramColor = view.IsColor();

            UpdateHistograms();
         }
}

// ----------------------------------------------------------------------------

void CurvesTransformationInterface::ImageFocused( const View& view )
{
   if ( GUI != 0 )
      if ( IsTrackViewActive() )
      {
         m_histogramView = view;

         if ( !m_histogramView.IsNull() )
         {
            if ( !m_histogramView.HasProperty( "Histogram16" ) )
               m_histogramView.ComputeProperty( "Histogram16", false/*notify*/ );
            m_histogramColor = view.IsColor();
         }

         UpdateHistograms();
      }
}

// ----------------------------------------------------------------------------

void CurvesTransformationInterface::ImageDeleted( const View& view )
{
   if ( GUI != 0 )
      if ( view == m_histogramView )
      {
         m_histograms.Clear();
         m_histogramView = View::Null();
         m_histogramColor = false;
         UpdateCurve();
      }
}

// ----------------------------------------------------------------------------

bool CurvesTransformationInterface::WantsReadoutNotifications() const
{
   return true;
}

// ----------------------------------------------------------------------------

void CurvesTransformationInterface::BeginReadout( const View& v )
{
   if ( GUI != 0 && IsVisible() )
      m_readoutActive = true;
}

// ----------------------------------------------------------------------------

void CurvesTransformationInterface::UpdateReadout( const View& v, const DPoint& p, double R, double G, double B, double A )
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

void CurvesTransformationInterface::EndReadout( const View& v )
{
   if ( m_readoutActive )
   {
      m_readoutActive = false;
      UpdateCurve();
   }
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

size_type CurvesTransformationInterface::FindPoint( int c, double x, double y, int tolerancePx ) const
{
   double delta = double( tolerancePx )/(GUI->Curve_ScrollBox.Viewport().Width()*m_zoomX - 1);

   const double* xa = m_instance[c].XVector();
   const double* ya = m_instance[c].YVector();

   for ( size_type i = 0, n = m_instance[c].Length(); i < n; ++i )
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

size_type CurvesTransformationInterface::CreatePoint( int c, double x, double y, int tolerancePx )
{
   double delta = double( tolerancePx )/(GUI->Curve_ScrollBox.Viewport().Width()*m_zoomX - 1);

   const double* xa = m_instance[c].XVector();
   const double* ya = m_instance[c].YVector();

   size_type i;
   for ( i = 0; ; )
   {
      double dxi = Abs( x - xa[i] );
      if ( dxi <= delta )
      {
         size_type j = i + 1;
         if ( j < m_instance[c].Length() )
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

         m_instance[c].Y( i ) = y;
         break;
      }

      if ( x < xa[i] || ++i == m_instance[c].Length() )
      {
         m_instance[c].Add( x, y );
         break;
      }
   }

   UpdateRealTimePreview();

   return i;
}

bool CurvesTransformationInterface::DragPoint( int c, size_type i, double x, double y )
{
   bool ok = false;

   if ( i > 0 && i < m_instance[c].Length()-1 && x > m_instance[c].X( i-1 ) && x < m_instance[c].X( i+1 ) )
   {
      ok = true;
      m_instance[c].Remove( i );
      m_instance[c].Add( x, y );
   }

   if ( !ok )
      m_instance[c].Y( i ) = y;

   UpdateRealTimePreview();

   return ok;
}

size_type CurvesTransformationInterface::RemovePoint( int c, double x, double y, int tolerancePx )
{
   size_type i = FindPoint( c, x, y, tolerancePx );

   if ( i != ~size_type( 0 ) )
   {
      if ( i == 0 )
         m_instance[c].Y( 0 ) = 0;
      else
      {
         size_type i1 = m_instance[c].Length() - 1;

         if ( i == i1 )
            m_instance[c].Y( i1 ) = 1;
         else
            m_instance[c].Remove( i );
      }

      UpdateRealTimePreview();
   }

   return i;
}

// ----------------------------------------------------------------------------

void CurvesTransformationInterface::GetHistograms()
{
   m_histograms.Clear();
   if ( m_histogramView.IsNull() )
      return;
   if ( !m_histogramView.HasProperty( "Histogram16" ) )
      return;
   UI64Matrix M = m_histogramView.PropertyValue( "Histogram16" ).ToUI64Matrix();
   for ( int i = 0; i < M.Rows(); ++i )
   {
      Histogram h( TheHistogramTransformationInterface->PlotResolution() );
      Histogram g( M.RowVector( i ) );
      g.Resample( h );
      m_histograms.Add( h );
   }
}

// ----------------------------------------------------------------------------

void CurvesTransformationInterface::SetChannel( int c )
{
   m_channel = c;
   UpdateChannelControls();
   UpdateCurveControls();
   UpdateInterpolationControls();
   UpdateCurve();
}

void CurvesTransformationInterface::SetZoom( int hz, int vz, const Point* p )
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
         GUI->Curve_ScrollBox.SetHorizontalScrollPosition(
                  Range( p->x*m_zoomX - (visibleWidth >> 1), 0, m ) );
   }
   else
      GUI->Curve_ScrollBox.SetHorizontalScrollRange( 0, 0 );

   GUI->Curve_ScrollBox.SetPageWidth( visibleWidth );

   if ( vsb )
   {
      int m = contentsHeight - visibleHeight;
      GUI->Curve_ScrollBox.SetVerticalScrollRange( 0, m );

      if ( p != 0 )
         GUI->Curve_ScrollBox.SetVerticalScrollPosition(
                  Range( p->y*m_zoomY - (visibleHeight >> 1), 0, m ) );
   }
   else
      GUI->Curve_ScrollBox.SetVerticalScrollRange( 0, 0 );

   GUI->Curve_ScrollBox.SetPageHeight( visibleHeight );

   UpdateZoomControls();
   UpdateCurve();

   m_settingUp = false;
}

void CurvesTransformationInterface::SetMode( working_mode m )
{
   m_mode = m;

   if ( GUI->Curve_ScrollBox.Viewport().IsUnderMouse() )
      GUI->Curve_ScrollBox.Viewport().Update();

   UpdateModeControls();
   UpdateCurveInfo();
}

void CurvesTransformationInterface::SetInterpolation( int type )
{
   m_instance[m_channel].SetType( type );

   UpdateInterpolationControls();
   UpdateCurve();
   UpdateRealTimePreview();
}

void CurvesTransformationInterface::SelectPoint( size_type p, bool pan )
{
   m_currentPoint[m_channel] = p;

   if ( pan && (m_zoomX != 1 || m_zoomY != 1) )
   {
      Rect r = GUI->Curve_ScrollBox.Viewport().ClientRect();
      int w1 = r.Width()*m_zoomX - 1;
      int h1 = r.Height()*m_zoomY - 1;
      GUI->Curve_ScrollBox.SetScrollPosition(
               Range( RoundInt( CurrentInputValue()*w1 ) - r.Width()/2, 0, w1 ),
               Range( RoundInt( (1 - CurrentOutputValue())*h1 ) - r.Height()/2, 0, h1 ) );
   }

   UpdateCurveControls();
   UpdateCurve();
}

// ----------------------------------------------------------------------------

void CurvesTransformationInterface::UpdateControls()
{
   UpdateModeControls();
   UpdateZoomControls();
   UpdateGraphicsControls();
   UpdateChannelControls();
   UpdateInterpolationControls();
   UpdateCurveControls();
}

void CurvesTransformationInterface::UpdateModeControls()
{
   GUI->EditPointMode_ToolButton.SetChecked( m_mode == EditMode );
   GUI->SelectPointMode_ToolButton.SetChecked( m_mode == SelectMode );
   GUI->DeletePointMode_ToolButton.SetChecked( m_mode == DeleteMode );
   GUI->ZoomInMode_ToolButton.SetChecked( m_mode == ZoomInMode );
   GUI->ZoomOutMode_ToolButton.SetChecked( m_mode == ZoomOutMode );
   GUI->PanMode_ToolButton.SetChecked( m_mode == PanMode );
}

void CurvesTransformationInterface::UpdateZoomControls()
{
   GUI->Zoom_SpinBox.SetValue( m_zoomX );
}

void CurvesTransformationInterface::UpdateGraphicsControls()
{
   GUI->ShowAll_ToolButton.SetChecked( m_showAllCurves );
   GUI->ShowGrid_ToolButton.SetChecked( m_showGrid );
}

void CurvesTransformationInterface::UpdateChannelControls()
{
   GUI->R_ToolButton.SetChecked( m_channel == CurveIndex::R );
   GUI->G_ToolButton.SetChecked( m_channel == CurveIndex::G );
   GUI->B_ToolButton.SetChecked( m_channel == CurveIndex::B );
   GUI->RGBK_ToolButton.SetChecked( m_channel == CurveIndex::RGBK );
   GUI->A_ToolButton.SetChecked( m_channel == CurveIndex::A );
   GUI->L_ToolButton.SetChecked( m_channel == CurveIndex::L );
   GUI->a_ToolButton.SetChecked( m_channel == CurveIndex::a );
   GUI->b_ToolButton.SetChecked( m_channel == CurveIndex::b );
   GUI->c_ToolButton.SetChecked( m_channel == CurveIndex::c );
   GUI->H_ToolButton.SetChecked( m_channel == CurveIndex::H );
   GUI->S_ToolButton.SetChecked( m_channel == CurveIndex::S );
}

void CurvesTransformationInterface::UpdateInterpolationControls()
{
   GUI->AkimaSubsplines_ToolButton.SetChecked( m_instance[m_channel].Type() == CurveType::AkimaSubsplines );
   GUI->CubicSpline_ToolButton.SetChecked( m_instance[m_channel].Type() == CurveType::CubicSpline );
   GUI->Linear_ToolButton.SetChecked( m_instance[m_channel].Type() == CurveType::Linear );
}

void CurvesTransformationInterface::UpdateCurveControls()
{
   GUI->Input_NumericEdit.SetValue( CurrentInputValue() );
   GUI->Output_NumericEdit.SetValue( CurrentOutputValue() );

   bool isFirstPoint = m_currentPoint[m_channel] == 0;
   bool isLastPoint = m_currentPoint[m_channel] == m_instance[m_channel].Length()-1;

   GUI->Input_NumericEdit.Enable( !(isFirstPoint || isLastPoint) );

   GUI->PrevPoint_ToolButton.Enable( !isFirstPoint );
   GUI->FirstPoint_ToolButton.Enable( !isFirstPoint );

   GUI->NextPoint_ToolButton.Enable( !isLastPoint );
   GUI->LastPoint_ToolButton.Enable( !isLastPoint );

   GUI->CurrentPoint_Label.SetText( String().Format( "%u / %u",
                                    m_currentPoint[m_channel] + 1, m_instance[m_channel].Length() ) );

   GUI->RestoreCurve_ToolButton.Enable( !m_storedCurve.IsIdentity() );
}

void CurvesTransformationInterface::UpdateCurve()
{
   UpdateCurveInfo();

   m_viewportDirty = true;
   GUI->Curve_ScrollBox.Viewport().Update();
}

void CurvesTransformationInterface::UpdateCurveInfo()
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

void CurvesTransformationInterface::UpdateHistograms()
{
   if ( !m_histogramView.IsNull() )
   {
      GetHistograms();
      UpdateCurve();
   }
}

void CurvesTransformationInterface::UpdateRealTimePreview()
{
   if ( IsRealTimePreviewActive() )
   {
      if ( m_realTimeThread != 0 )
         m_realTimeThread->Abort();
      GUI->UpdateRealTimePreview_Timer.Start();
   }
}

// ----------------------------------------------------------------------------

void CurvesTransformationInterface::RegenerateViewport()
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

   Bitmap bmp( w0, h0, BitmapFormat::RGB32 );
   {
      bmp.Fill( 0xFF000000 );
      Graphics g( bmp );
      g.EnableAntialiasing();
      g.SetCompositionOperator( CompositionOp::Screen );

      for ( int i = 0; i < CurveIndex::NumberOfCurves; ++i )
         if ( i == m_channel || m_showAllCurves && !m_instance[i].IsIdentity() )
         {
            if ( !m_histograms.IsEmpty() )
            {
               bool rejectSaturated = TheHistogramTransformationInterface->RejectingSaturated();

               if ( m_channel == CurveIndex::RGBK )
               {
                  size_type n = m_histogramColor ? 3 : 1;
                  count_type peak = 0;
                  if ( rejectSaturated )
                     for ( size_type i = 0; i < n; ++i )
                        peak = Max( peak, m_histograms[i].PeakCount( 1, m_histograms[i].Resolution()-2 ) );
                  if ( !rejectSaturated || peak == 0 )
                     for ( size_type i = 0; i < n; ++i )
                        peak = Max( peak, m_histograms[i].PeakCount() );

                  g.SetOpacity( 0.5 );
                  for ( size_type i = 0; i < n; ++i )
                  {
                     g.SetPen( m_channelColor[(n > 1) ? i : CurveIndex::RGBK], DisplayPixelRatio() );
                     TheHistogramTransformationInterface->PlotHistogram( g, r, m_histograms[i], peak, w, h, m_zoomX, m_zoomY );
                  }
                  g.SetOpacity( 1.0 );
               }
               else if ( m_channel < CurveIndex::RGBK )
               {
                  if ( m_histogramColor )
                  {
                     count_type peak = 0;
                     if ( rejectSaturated )
                        peak = m_histograms[m_channel].PeakCount( 1, m_histograms[m_channel].Resolution()-2 );
                     if ( !rejectSaturated || peak == 0 )
                        peak = m_histograms[m_channel].PeakCount();

                     g.SetPen( m_channelColor[m_channel], DisplayPixelRatio() );
                     g.SetOpacity( 0.5 );
                     TheHistogramTransformationInterface->PlotHistogram( g, r, m_histograms[m_channel], peak, w, h, m_zoomX, m_zoomY );
                     g.SetOpacity( 1.0 );
                  }
               }
               else if ( m_channel == CurveIndex::A )
               {
                  int c = m_histogramColor ? 3 : 1;
                  if ( m_histograms.Length() > size_type( c ) )
                  {
                     count_type peak = 0;
                     if ( rejectSaturated )
                        peak = m_histograms[c].PeakCount( 1, m_histograms[c].Resolution()-2 );
                     if ( !rejectSaturated || peak == 0 )
                        peak = m_histograms[c].PeakCount();

                     g.SetPen( m_channelColor[4], DisplayPixelRatio() );
                     g.SetOpacity( 0.5 );
                     TheHistogramTransformationInterface->PlotHistogram( g, r, m_histograms[c], peak, w, h, m_zoomX, m_zoomY );
                     g.SetOpacity( 1.0 );
                  }
               }
            }

            PlotCurve( g, r, i, w, h, m_zoomX, m_zoomY );
         }
   }

   Graphics g( m_viewportBitmap );
   g.SetCompositionOperator( CompositionOp::Difference );
   g.DrawBitmap( 0, 0, bmp );
}

// ----------------------------------------------------------------------------

void CurvesTransformationInterface::PlotGrid( Graphics& g, const Rect& r, int width, int height, int hZoom, int vZoom )
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
}

RGBA CurvesTransformationInterface::ScaleColor( float f ) const
{
   RGBColorSystem::sample r, g, b;

   switch ( m_channel )
   {
   case CurveIndex::R:
      r = f;
      g = 0;
      b = 0;
      break;
   case CurveIndex::G:
      r = 0;
      g = f;
      b = 0;
      break;
   case CurveIndex::B:
      r = 0;
      g = 0;
      b = f;
      break;
   default: // ?!
   case CurveIndex::RGBK:
      r = f;
      g = f;
      b = f;
      break;
   case CurveIndex::A:
      r = f;
      g = f;
      b = f;
      break;
   case CurveIndex::L:
      r = f;
      g = f;
      b = f;
      break;
   case CurveIndex::a:
      r = f;
      g = 1 - f;
      b = 0;
      break;
   case CurveIndex::b:
      r = f;
      g = f;
      b = 1 - f;
      break;
   case CurveIndex::c:
      RGBColorSystem::HSVToRGB( r, g, b, 0.075, f, 0.9 );
      break;
   case CurveIndex::H:
      RGBColorSystem::HSVToRGB( r, g, b, f, 0.9, 0.9 );
      break;
   case CurveIndex::S:
      RGBColorSystem::HSVToRGB( r, g, b, 0.75, f, 0.9 );
      break;
   }

   return RGBAColor( float( r ), float( g ), float( b ) );
}

void CurvesTransformationInterface::PlotScale( Graphics& g, const Rect& r, int width, int height )
{
   int ui8 = LogicalPixelsToPhysical( 8 );

   int w = r.Width();
   int h = r.Height();

   int x0 = -r.x0;
   int y0 = height - ui8 - r.y0;

   if ( m_channel == CurveIndex::H )
   {
      for ( int x = 0; x < w; ++x )
      {
         RGBA color = ScaleColor( float( x + r.x0 )/(width - 1) );
         g.SetPen( color );
         g.DrawLine( x, y0, x, y0+ui8 );
      }

      for ( int y = 0; y < h; ++y )
      {
         RGBA color = ScaleColor( 1 - float( y + r.y0 )/(height - 1) );
         g.SetPen( color );
         g.DrawLine( x0, y, x0+ui8, y );
      }
   }
   else
   {
      GradientBrush::stop_list stops;
      stops.Add( GradientBrush::Stop( 0.0, ScaleColor( float( r.x0 )/(width - 1) ) ) );
      stops.Add( GradientBrush::Stop( 1.0, ScaleColor( float( w + r.x0 - 1 )/(width - 1) ) ) );
      g.FillRect( 0, y0, w, y0+ui8, LinearGradientBrush( 0, 0, w, 0, stops ) );

      stops.Clear();
      stops.Add( GradientBrush::Stop( 0.0, ScaleColor( 1 - float( r.y0 )/(height - 1) ) ) );
      stops.Add( GradientBrush::Stop( 1.0, ScaleColor( 1 - float( h + r.y0 - 1 )/(height - 1) ) ) );
      g.FillRect( x0, 0, x0+ui8, h, LinearGradientBrush( 0, 0, 0, h, stops ) );
   }
}

void CurvesTransformationInterface::PlotCurve( Graphics& g, const Rect& r, int c, int width, int height, int hZoom, int vZoom )
{
   const Curve& curve = m_instance[c];

   AutoPointer<Curve::interpolator> interpolator( curve.InitInterpolator() );
   Array<Point> points;
   double dx = 1.0/(width - 1);
   for ( int w = r.Width(), x = 0; x < w; ++x )
      points.Add( Point( x, RoundInt( (height - 1)*(1 - (*interpolator)( (x + r.x0)*dx )) ) - r.y0 ) );

   g.SetPen( m_channelColor[c], DisplayPixelRatio() );
   g.DrawPolyline( points );

   if ( c == m_channel )
   {
      int ui2 = LogicalPixelsToPhysical( 2 );
      g.SetBrush( 0xFF000000 );
      for ( size_type i = 0; i < curve.Length(); ++i )
      {
         int x = RoundInt( curve.XVector()[i]*(width - 1) ) - r.x0;
         int y = RoundInt( (1 - curve.YVector()[i])*(height - 1) ) - r.y0;

         if ( i == m_currentPoint[c] )
            g.SetBrush( m_channelColor[c] );

         g.DrawRect( x-ui2, y-ui2, x+ui2, y+ui2 );

         if ( i == m_currentPoint[c] )
            g.SetBrush( 0xFF000000 );
      }
   }
}

void CurvesTransformationInterface::PlotReadouts( Graphics& g, const Bitmap& bmp, const Rect& r, int width, int height )
{
   int w = bmp.Width();
   int h = bmp.Height();
   float d = DisplayPixelRatio();

   if ( m_channel == CurveIndex::RGBK )
   {
      for ( int c = 0; c < 3; ++c )
      {
         int x = RoundInt( m_readouts[c]*(width - 1) ) - r.x0;
         if ( x >= 0 && x < w )
         {
            g.SetPen( m_channelColor[c], d );
            g.DrawLine( x, 0, x, h );
         }
      }
   }
   else
   {
      double f = 0;
      switch ( m_channel )
      {
      case CurveIndex::R:
      case CurveIndex::G:
      case CurveIndex::B:
         f = m_readouts[m_channel];
         break;
      case CurveIndex::A:
         f = m_readouts[3];
         break;
      case CurveIndex::L:
         f = m_readoutRGBWS.Lightness( m_readouts[0], m_readouts[1], m_readouts[2] );
         break;
      case CurveIndex::a:
         f = m_readoutRGBWS.CIEa( m_readouts[0], m_readouts[1], m_readouts[2] );
         break;
      case CurveIndex::b:
         f = m_readoutRGBWS.CIEb( m_readouts[0], m_readouts[1], m_readouts[2] );
         break;
      case CurveIndex::c:
         f = m_readoutRGBWS.CIEc( m_readouts[0], m_readouts[1], m_readouts[2] );
         break;
      case CurveIndex::H:
         f = m_readoutRGBWS.Hue( m_readouts[0], m_readouts[1], m_readouts[2] );
         break;
      case CurveIndex::S:
         f = m_readoutRGBWS.HSVSaturation( m_readouts[0], m_readouts[1], m_readouts[2] );
         break;
      }

      int x = RoundInt( f*(width - 1) ) - r.x0;
      if ( x >= 0 && x < w )
      {
         g.SetPen( m_channelColor[m_channel], d );
         g.DrawLine( x, 0, x, h );
      }
   }
}

void CurvesTransformationInterface::PlotCursor( Graphics& g, const Rect& r )
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

void CurvesTransformationInterface::__Curve_Paint( Control& sender, const pcl::Rect& updateRect )
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

void CurvesTransformationInterface::__Curve_Resize( Control& sender,
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

void CurvesTransformationInterface::__Curve_ScrollPosUpdated( ScrollBox& sender, int pos )
{
   if ( sender == GUI->Curve_ScrollBox )
      UpdateCurve();
}

void CurvesTransformationInterface::__Curve_Enter( Control& sender )
{
   if ( sender == GUI->Curve_ScrollBox.Viewport() )
      m_cursorVisible = true;
   m_cursorPos = -1;
}

void CurvesTransformationInterface::__Curve_Leave( Control& sender )
{
   m_cursorVisible = false;
   UpdateCurveInfo();
   sender.Update();
}

void CurvesTransformationInterface::__Curve_MousePress(
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
            m_curvePos.y = Range( 1 - double( p.y )/(h - 1), 0.0, 1.0 );

            size_type i = ~size_type( 0 );

            switch ( m_mode )
            {
            case EditMode:
               if ( !modifiers )
               {
                  m_dragging = true;
                  i = CreatePoint( m_channel, m_curvePos, int( t*1.5 ) );
               }
               break;

            case SelectMode:
               i = FindPoint( m_channel, m_curvePos, t );
               break;

            case DeleteMode:
               i = RemovePoint( m_channel, m_curvePos, t );
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
      size_type i = RemovePoint( m_channel, m_curvePos, LogicalPixelsToPhysical( CURSOR_TOLERANCE ) );
      if ( i != ~size_type( 0 ) )
         SelectPoint( i, false );
   }

   UpdateCurveInfo();
}

void CurvesTransformationInterface::__Curve_MouseRelease(
   Control& sender, const pcl::Point& pos, int button, unsigned buttons, unsigned modifiers )
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

void CurvesTransformationInterface::__Curve_MouseMove(
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
      m_curvePos.y = Range( 1 - double( p.y )/(h - 1), 0.0, 1.0 );

      if ( m_dragging )
      {
         DragPoint( m_channel, m_currentPoint[m_channel], m_curvePos );
         UpdateCurveControls();
         UpdateCurve();
      }
      else
         UpdateCurveInfo(); // UpdateCurve() already calls UpdateCurveInfo()
   }

   if ( m_panning )
   {
      Point d = m_panOrigin - pos;
      m_panOrigin = pos;
      GUI->Curve_ScrollBox.SetScrollPosition( GUI->Curve_ScrollBox.ScrollPosition() + d );
   }
}

void CurvesTransformationInterface::__Curve_MouseWheel(
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

void CurvesTransformationInterface::__Curve_KeyPress( Control& sender, int key, unsigned modifiers, bool& wantsKey )
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
         case KeyCode::Right: if ( i < m_instance[m_channel].Length()-1 ) ++i; break;
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
         double dy = 1.0/h;

         switch ( key )
         {
         case KeyCode::Left:  x -= dx; break;
         case KeyCode::Right: x += dx; break;
         case KeyCode::Up:    y += dy; break;
         case KeyCode::Down:  y -= dy; break;
         }

         x = Range( x, 0.0, 1.0 );
         y = Range( y, 0.0, 1.0 );

         if ( x != CurrentInputValue() || y != CurrentOutputValue() )
         {
            DragPoint( m_channel, CurrentPoint(), x, y );
            UpdateCurveControls();
            UpdateCurve();
         }
      }

      wantsKey = true;
      break;

   case KeyCode::Delete:
      {
         size_type i = RemovePoint( m_channel, CurrentInputValue(), CurrentOutputValue(), 0 );
         if ( i != ~size_type( 0 ) )
            SelectPoint( i );
      }
      wantsKey = true;
      break;
   }
}

void CurvesTransformationInterface::__CurveParameter_ValueUpdated( NumericEdit& sender, double value )
{
   if ( sender == GUI->Input_NumericEdit )
   {
      double x = Round( value, TheXRParameter->Precision() );

      if ( x != CurrentInputValue() )
      {
         double d = 1/Pow10I<double>()( TheXRParameter->Precision() );
         for ( size_type i = 0; i < m_instance[m_channel].Length(); ++i )
            if ( i != CurrentPoint() && Abs( x - m_instance[m_channel].X( i ) ) < d )
            {
               GUI->Input_NumericEdit.SetValue( CurrentInputValue() );
               pcl::MessageBox(
                  "Two curve points cannot have identical abscissae - previous X-value inserted.",
                  "CurvesTransformation", StdIcon::Error ).Execute();
               return;
            }

         DPoint p( x, CurrentOutputValue() );

         m_instance[m_channel].Remove( CurrentPoint() );
         m_instance[m_channel].Add( p );

         m_currentPoint[m_channel] = m_instance[m_channel].Search( x );
      }
   }
   else if ( sender == GUI->Output_NumericEdit )
   {
      CurrentOutputValue() = Round( value, TheYRParameter->Precision() );
   }

   UpdateCurve();
   UpdateRealTimePreview();
}

void CurvesTransformationInterface::__Mode_ButtonClick( Button& sender, bool /*checked*/ )
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

void CurvesTransformationInterface::__Channel_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->R_ToolButton )
      SetChannel( CurveIndex::R );
   else if ( sender == GUI->G_ToolButton )
      SetChannel( CurveIndex::G );
   else if ( sender == GUI->B_ToolButton )
      SetChannel( CurveIndex::B );
   else if ( sender == GUI->RGBK_ToolButton )
      SetChannel( CurveIndex::RGBK );
   else if ( sender == GUI->A_ToolButton )
      SetChannel( CurveIndex::A );
   else if ( sender == GUI->L_ToolButton )
      SetChannel( CurveIndex::L );
   else if ( sender == GUI->a_ToolButton )
      SetChannel( CurveIndex::a );
   else if ( sender == GUI->b_ToolButton )
      SetChannel( CurveIndex::b );
   else if ( sender == GUI->c_ToolButton )
      SetChannel( CurveIndex::c );
   else if ( sender == GUI->H_ToolButton )
      SetChannel( CurveIndex::H );
   else if ( sender == GUI->S_ToolButton )
      SetChannel( CurveIndex::S );
}

void CurvesTransformationInterface::__Zoom_ButtonClick( Button& sender, bool /*checked*/ )
{
   if ( sender == GUI->Zoom11_ToolButton )
      SetZoom( 1, 1 );
}

void CurvesTransformationInterface::__Zoom_ValueUpdated( SpinBox& sender, int value )
{
   if ( sender == GUI->Zoom_SpinBox )
      SetZoom( value, value );
}

void CurvesTransformationInterface::__ShowAll_ButtonClick( Button& /*sender*/, bool checked )
{
   m_showAllCurves = checked;
   UpdateCurve();
}

void CurvesTransformationInterface::__ShowGrid_ButtonClick( Button& /*sender*/, bool checked )
{
   m_showGrid = checked;
   UpdateCurve();
}

void CurvesTransformationInterface::__PointNavigation_ButtonClick( Button& sender, bool /*checked*/ )
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
      if ( CurrentPoint() < m_instance[m_channel].Length()-1 )
         SelectPoint( CurrentPoint()+1 );
   }
   else if ( sender == GUI->LastPoint_ToolButton )
   {
      if ( CurrentPoint() != m_instance[m_channel].Length()-1 )
         SelectPoint( m_instance[m_channel].Length()-1 );
   }
}

void CurvesTransformationInterface::__Interpolation_ButtonClick( Button& sender, bool checked )
{
   if ( sender == GUI->AkimaSubsplines_ToolButton )
      SetInterpolation( CurveType::AkimaSubsplines );
   else if ( sender == GUI->CubicSpline_ToolButton )
      SetInterpolation( CurveType::CubicSpline );
   else if ( sender == GUI->Linear_ToolButton )
      SetInterpolation( CurveType::Linear );
}

void CurvesTransformationInterface::__StoreCurve_ButtonClick( Button& /*sender*/, bool /*checked*/ )
{
   m_storedCurve = m_instance[m_channel];
   UpdateCurveControls();
}

void CurvesTransformationInterface::__RestoreCurve_ButtonClick( Button& /*sender*/, bool /*checked*/ )
{
   m_instance[m_channel] = m_storedCurve;
   m_currentPoint[m_channel] = 0;

   UpdateControls();
   UpdateCurve();
   UpdateRealTimePreview();
}

void CurvesTransformationInterface::__ReverseCurve_ButtonClick( Button& /*sender*/, bool /*checked*/ )
{
   m_instance[m_channel].Reverse();

   UpdateCurveControls();
   UpdateCurve();
   UpdateRealTimePreview();
}

void CurvesTransformationInterface::__ResetCurve_ButtonClick( Button& /*sender*/, bool /*checked*/ )
{
   m_instance[m_channel].Reset();

   m_currentPoint[m_channel] = 0;

   UpdateCurveControls();
   UpdateCurve();
   UpdateRealTimePreview();
}

void CurvesTransformationInterface::__KeyPress( Control& sender, int key, unsigned modifiers, bool& wantsKey )
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
         wantsKey = true;
      }
      break;
   }
}

void CurvesTransformationInterface::__KeyRelease( Control& sender, int key, unsigned modifiers, bool& wantsKey )
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

void CurvesTransformationInterface::__UpdateRealTimePreview_Timer( Timer& sender )
{
   if ( m_realTimeThread == 0 )
      if ( IsRealTimePreviewActive() )
         RealTimePreview::Update();
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

CurvesTransformationInterface::GUIData::GUIData( CurvesTransformationInterface& w )
{
   int ui16 = w.LogicalPixelsToPhysical( 16 );
   int ui24 = w.LogicalPixelsToPhysical( 24 );
   int labelWidth = w.Font().Width( String( "Output:" ) + 'M' );
   int channelLabelWidth = ui24 + w.Font().Width( 'M' );
   int rgbkLabelWidth = ui24 + w.Font().Width( "RGB/K" );
   int curveMinSize = 10*channelLabelWidth + rgbkLabelWidth;

   //

   Curve_ScrollBox.DisableAutoScroll();
   Curve_ScrollBox.SetMinSize( curveMinSize, curveMinSize );
   Curve_ScrollBox.OnHorizontalScrollPosUpdated( (ScrollBox::pos_event_handler)&CurvesTransformationInterface::__Curve_ScrollPosUpdated, w );
   Curve_ScrollBox.OnVerticalScrollPosUpdated( (ScrollBox::pos_event_handler)&CurvesTransformationInterface::__Curve_ScrollPosUpdated, w );

   Curve_ScrollBox.Viewport().SetCursor( StdCursor::NoCursor );
   Curve_ScrollBox.Viewport().OnPaint( (Control::paint_event_handler)&CurvesTransformationInterface::__Curve_Paint, w );
   Curve_ScrollBox.Viewport().OnResize( (Control::resize_event_handler)&CurvesTransformationInterface::__Curve_Resize, w );
   Curve_ScrollBox.Viewport().OnEnter( (Control::event_handler)&CurvesTransformationInterface::__Curve_Enter, w );
   Curve_ScrollBox.Viewport().OnLeave( (Control::event_handler)&CurvesTransformationInterface::__Curve_Leave, w );
   Curve_ScrollBox.Viewport().OnMouseMove( (Control::mouse_event_handler)&CurvesTransformationInterface::__Curve_MouseMove, w );
   Curve_ScrollBox.Viewport().OnMousePress( (Control::mouse_button_event_handler)&CurvesTransformationInterface::__Curve_MousePress, w );
   Curve_ScrollBox.Viewport().OnMouseRelease( (Control::mouse_button_event_handler)&CurvesTransformationInterface::__Curve_MouseRelease, w );
   Curve_ScrollBox.Viewport().OnMouseWheel( (Control::mouse_wheel_event_handler)&CurvesTransformationInterface::__Curve_MouseWheel, w );
   Curve_ScrollBox.Viewport().OnKeyPress( (Control::keyboard_event_handler)&CurvesTransformationInterface::__Curve_KeyPress, w );

   //

   EditPointMode_ToolButton.SetIcon( Bitmap( edit_point_mode_XPM ).ScaledToSize( ui16, ui16 ) );
   EditPointMode_ToolButton.SetScaledFixedSize( 20, 20 );
   EditPointMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   EditPointMode_ToolButton.SetToolTip( "Edit Point mode" );
   EditPointMode_ToolButton.SetCheckable();
   EditPointMode_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Mode_ButtonClick, w );

   SelectPointMode_ToolButton.SetIcon( Bitmap( select_point_mode_XPM ).ScaledToSize( ui16, ui16 ) );
   SelectPointMode_ToolButton.SetScaledFixedSize( 20, 20 );
   SelectPointMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   SelectPointMode_ToolButton.SetToolTip( "Select Point mode [Shift]" );
   SelectPointMode_ToolButton.SetCheckable();
   SelectPointMode_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Mode_ButtonClick, w );

   DeletePointMode_ToolButton.SetIcon( Bitmap( delete_point_mode_XPM ).ScaledToSize( ui16, ui16 ) );
   DeletePointMode_ToolButton.SetScaledFixedSize( 20, 20 );
   DeletePointMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   DeletePointMode_ToolButton.SetToolTip( "Delete Point mode [Ctrl]" );
   DeletePointMode_ToolButton.SetCheckable();
   DeletePointMode_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Mode_ButtonClick, w );

   ZoomInMode_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-mode-zoom-in.png" ) );
   ZoomInMode_ToolButton.SetScaledFixedSize( 20, 20 );
   ZoomInMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ZoomInMode_ToolButton.SetToolTip( "Zoom In mode "
#ifdef __PCL_MACOSX
      "[Alt+SpaceBar]" );
#else
      "[Ctrl+SpaceBar]" );
#endif
   ZoomInMode_ToolButton.SetCheckable();
   ZoomInMode_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Mode_ButtonClick, w );

   ZoomOutMode_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-mode-zoom-out.png" ) );
   ZoomOutMode_ToolButton.SetScaledFixedSize( 20, 20 );
   ZoomOutMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ZoomOutMode_ToolButton.SetToolTip( "Zoom Out mode "
#ifdef __PCL_MACOSX
      "[Shift+Alt+SpaceBar]" );
#else
      "[Shift+Ctrl+SpaceBar]" );
#endif
   ZoomOutMode_ToolButton.SetCheckable();
   ZoomOutMode_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Mode_ButtonClick, w );

   PanMode_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-mode-pan.png" ) );
   PanMode_ToolButton.SetScaledFixedSize( 20, 20 );
   PanMode_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   PanMode_ToolButton.SetToolTip( "Pan mode [SpaceBar]" );
   PanMode_ToolButton.SetCheckable();
   PanMode_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Mode_ButtonClick, w );

   Zoom_SpinBox.SetRange( 1, s_maxZoom );
   Zoom_SpinBox.SetToolTip( "Zoom" );
   Zoom_SpinBox.OnValueUpdated( (SpinBox::value_event_handler)&CurvesTransformationInterface::__Zoom_ValueUpdated, w );

   Zoom11_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/view-zoom-1-1.png" ) );
   Zoom11_ToolButton.SetScaledFixedSize( 20, 20 );
   Zoom11_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   Zoom11_ToolButton.SetToolTip( "Zoom 1:1" );
   Zoom11_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Zoom_ButtonClick, w );

   ShowAll_ToolButton.SetIcon( Bitmap( show_all_curves_XPM ).ScaledToSize( ui16, ui16 ) );
   ShowAll_ToolButton.SetScaledFixedSize( 20, 20 );
   ShowAll_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ShowAll_ToolButton.SetToolTip( "Show all curves" );
   ShowAll_ToolButton.SetCheckable();
   ShowAll_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__ShowAll_ButtonClick, w );

   ShowGrid_ToolButton.SetIcon( Bitmap( show_grid_XPM ).ScaledToSize( ui16, ui16 ) );
   ShowGrid_ToolButton.SetScaledFixedSize( 20, 20 );
   ShowGrid_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ShowGrid_ToolButton.SetToolTip( "Show grid" );
   ShowGrid_ToolButton.SetCheckable();
   ShowGrid_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__ShowGrid_ButtonClick, w );

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
   Mode_Sizer.Add( ShowAll_ToolButton );
   Mode_Sizer.Add( ShowGrid_ToolButton );

   //

   Info_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );
   Info_Label.SetStyle( FrameStyle::Sunken );
   Info_Label.SetLineWidth( 1 );

   //

   R_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-display-red.png" ) );
   R_ToolButton.SetText( "R" );
   R_ToolButton.SetFixedWidth( channelLabelWidth );
   R_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   R_ToolButton.SetToolTip( "Red channel" );
   R_ToolButton.SetCheckable();
   R_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Channel_ButtonClick, w );

   G_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-display-green.png" ) );
   G_ToolButton.SetText( "G" );
   G_ToolButton.SetFixedWidth( channelLabelWidth );
   G_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   G_ToolButton.SetToolTip( "Green channel" );
   G_ToolButton.SetCheckable();
   G_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Channel_ButtonClick, w );

   B_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-display-blue.png" ) );
   B_ToolButton.SetText( "B" );
   B_ToolButton.SetFixedWidth( channelLabelWidth );
   B_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   B_ToolButton.SetToolTip( "Blue channel" );
   B_ToolButton.SetCheckable();
   B_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Channel_ButtonClick, w );

   RGBK_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-display-rgb.png" ) );
   RGBK_ToolButton.SetText( "RGB/K" );
   RGBK_ToolButton.SetFixedWidth( rgbkLabelWidth );
   RGBK_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   RGBK_ToolButton.SetToolTip( "RGB/Gray channel" );
   RGBK_ToolButton.SetCheckable();
   RGBK_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Channel_ButtonClick, w );

   A_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-display-alpha.png" ) );
   A_ToolButton.SetText( "A" );
   A_ToolButton.SetFixedWidth( channelLabelWidth );
   A_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   A_ToolButton.SetToolTip( "Alpha channel" );
   A_ToolButton.SetCheckable();
   A_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Channel_ButtonClick, w );

   L_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-display-value.png" ) );
   L_ToolButton.SetText( "L" );
   L_ToolButton.SetFixedWidth( channelLabelWidth );
   L_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   L_ToolButton.SetToolTip( "CIE L* component (lightness)" );
   L_ToolButton.SetCheckable();
   L_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Channel_ButtonClick, w );

   a_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-display-cie-a.png" ) );
   a_ToolButton.SetText( "a" );
   a_ToolButton.SetFixedWidth( channelLabelWidth );
   a_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   a_ToolButton.SetToolTip( "CIE a* component" );
   a_ToolButton.SetCheckable();
   a_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Channel_ButtonClick, w );

   b_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-display-cie-b.png" ) );
   b_ToolButton.SetText( "b" );
   b_ToolButton.SetFixedWidth( channelLabelWidth );
   b_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   b_ToolButton.SetToolTip( "CIE b* component" );
   b_ToolButton.SetCheckable();
   b_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Channel_ButtonClick, w );

   c_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-display-saturation.png" ) );
   c_ToolButton.SetText( "c" );
   c_ToolButton.SetFixedWidth( channelLabelWidth );
   c_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   c_ToolButton.SetToolTip( "CIE c* component" );
   c_ToolButton.SetCheckable();
   c_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Channel_ButtonClick, w );

   H_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-display-hue.png" ) );
   H_ToolButton.SetText( "H" );
   H_ToolButton.SetFixedWidth( channelLabelWidth );
   H_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   H_ToolButton.SetToolTip( "Hue" );
   H_ToolButton.SetCheckable();
   H_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Channel_ButtonClick, w );

   S_ToolButton.SetIcon( w.ScaledResource( ":/toolbar/image-display-saturation.png" ) );
   S_ToolButton.SetText( "S" );
   S_ToolButton.SetFixedWidth( channelLabelWidth );
   S_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   S_ToolButton.SetToolTip( "Saturation" );
   S_ToolButton.SetCheckable();
   S_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Channel_ButtonClick, w );

   Selectors_Sizer.Add( R_ToolButton );
   Selectors_Sizer.Add( G_ToolButton );
   Selectors_Sizer.Add( B_ToolButton );
   Selectors_Sizer.Add( RGBK_ToolButton );
   Selectors_Sizer.Add( A_ToolButton );
   Selectors_Sizer.Add( L_ToolButton );
   Selectors_Sizer.Add( a_ToolButton );
   Selectors_Sizer.Add( b_ToolButton );
   Selectors_Sizer.Add( c_ToolButton );
   Selectors_Sizer.Add( H_ToolButton );
   Selectors_Sizer.Add( S_ToolButton );

   //

   Input_NumericEdit.label.SetText( "Input:" );
   Input_NumericEdit.label.SetFixedWidth( labelWidth );
   Input_NumericEdit.SetReal();
   Input_NumericEdit.SetRange( 0, 1 );
   Input_NumericEdit.SetPrecision( TheXRParameter->Precision() );
   Input_NumericEdit.SetToolTip( "X point coordinate" );
   Input_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CurvesTransformationInterface::__CurveParameter_ValueUpdated, w );

   PrevPoint_ToolButton.SetIcon( w.ScaledResource( ":/icons/move-left.png" ) );
   PrevPoint_ToolButton.SetScaledFixedSize( 20, 20 );
   PrevPoint_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   PrevPoint_ToolButton.SetToolTip( "Previous point" );
   PrevPoint_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__PointNavigation_ButtonClick, w );

   FirstPoint_ToolButton.SetIcon( w.ScaledResource( ":/icons/move-left-limit.png" ) );
   FirstPoint_ToolButton.SetScaledFixedSize( 20, 20 );
   FirstPoint_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   FirstPoint_ToolButton.SetToolTip( "First point" );
   FirstPoint_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__PointNavigation_ButtonClick, w );

   StoreCurve_ToolButton.SetIcon( w.ScaledResource( ":/icons/upload.png" ) );
   StoreCurve_ToolButton.SetScaledFixedSize( 20, 20 );
   StoreCurve_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   StoreCurve_ToolButton.SetToolTip( "Store curve" );
   StoreCurve_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__StoreCurve_ButtonClick, w );

   RestoreCurve_ToolButton.SetIcon( w.ScaledResource( ":/icons/download.png" ) );
   RestoreCurve_ToolButton.SetScaledFixedSize( 20, 20 );
   RestoreCurve_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   RestoreCurve_ToolButton.SetToolTip( "Restore curve" );
   RestoreCurve_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__RestoreCurve_ButtonClick, w );

   ReverseCurve_ToolButton.SetIcon( w.ScaledResource( ":/icons/picture-flip-vertical" ) );
   ReverseCurve_ToolButton.SetScaledFixedSize( 20, 20 );
   ReverseCurve_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ReverseCurve_ToolButton.SetToolTip( "Reverse curve" );
   ReverseCurve_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__ReverseCurve_ButtonClick, w );

   ResetCurve_ToolButton.SetIcon( w.ScaledResource( ":/icons/delete.png" ) );
   ResetCurve_ToolButton.SetScaledFixedSize( 20, 20 );
   ResetCurve_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   ResetCurve_ToolButton.SetToolTip( "Reset curve" );
   ResetCurve_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__ResetCurve_ButtonClick, w );

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

   Output_NumericEdit.label.SetText( "Output:" );
   Output_NumericEdit.label.SetFixedWidth( labelWidth );
   Output_NumericEdit.SetReal();
   Output_NumericEdit.SetRange( 0, 1 );
   Output_NumericEdit.SetPrecision( TheYRParameter->Precision() );
   Output_NumericEdit.SetToolTip( "Y point coordinate" );
   Output_NumericEdit.OnValueUpdated( (NumericEdit::value_event_handler)&CurvesTransformationInterface::__CurveParameter_ValueUpdated, w );

   NextPoint_ToolButton.SetIcon( w.ScaledResource( ":/icons/move-right.png" ) );
   NextPoint_ToolButton.SetScaledFixedSize( 20, 20 );
   NextPoint_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   NextPoint_ToolButton.SetToolTip( "Next point" );
   NextPoint_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__PointNavigation_ButtonClick, w );

   LastPoint_ToolButton.SetIcon( w.ScaledResource( ":/icons/move-right-limit.png" ) );
   LastPoint_ToolButton.SetScaledFixedSize( 20, 20 );
   LastPoint_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   LastPoint_ToolButton.SetToolTip( "Last point" );
   LastPoint_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__PointNavigation_ButtonClick, w );

   CurrentPoint_Label.SetTextAlignment( TextAlign::Left|TextAlign::VertCenter );

   AkimaSubsplines_ToolButton.SetIcon( Bitmap( akima_interpolation_XPM ).ScaledToSize( ui16, ui16 ) );
   AkimaSubsplines_ToolButton.SetScaledFixedSize( 20, 20 );
   AkimaSubsplines_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   AkimaSubsplines_ToolButton.SetToolTip( "Akima subspline interpolation" );
   AkimaSubsplines_ToolButton.SetCheckable();
   AkimaSubsplines_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Interpolation_ButtonClick, w );

   CubicSpline_ToolButton.SetIcon( Bitmap( cubic_spline_interpolation_XPM ).ScaledToSize( ui16, ui16 ) );
   CubicSpline_ToolButton.SetScaledFixedSize( 20, 20 );
   CubicSpline_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   CubicSpline_ToolButton.SetToolTip( "Cubic spline interpolation" );
   CubicSpline_ToolButton.SetCheckable();
   CubicSpline_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Interpolation_ButtonClick, w );

   Linear_ToolButton.SetIcon( Bitmap( linear_interpolation_XPM ).ScaledToSize( ui16, ui16 ) );
   Linear_ToolButton.SetScaledFixedSize( 20, 20 );
   Linear_ToolButton.SetFocusStyle( FocusStyle::NoFocus );
   Linear_ToolButton.SetToolTip( "Linear interpolation" );
   Linear_ToolButton.SetCheckable();
   Linear_ToolButton.OnClick( (ToolButton::click_event_handler)&CurvesTransformationInterface::__Interpolation_ButtonClick, w );

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

   Global_Sizer.SetMargin( 4 );
   Global_Sizer.SetSpacing( 4 );
   Global_Sizer.Add( Curve_ScrollBox );
   Global_Sizer.Add( Mode_Sizer );
   Global_Sizer.Add( Info_Label );
   Global_Sizer.Add( Selectors_Sizer );
   Global_Sizer.Add( CurveData_Row1_Sizer );
   Global_Sizer.Add( CurveData_Row2_Sizer );

   w.SetSizer( Global_Sizer );
   w.AdjustToContents();

   UpdateRealTimePreview_Timer.SetSingleShot();
   UpdateRealTimePreview_Timer.SetInterval( 0.025 );
   UpdateRealTimePreview_Timer.OnTimer( (Timer::timer_event_handler)&CurvesTransformationInterface::__UpdateRealTimePreview_Timer, w );
}

// ----------------------------------------------------------------------------

} // pcl

// ----------------------------------------------------------------------------
// EOF CurvesTransformationInterface.cpp - Released 2018-12-12T09:25:25Z
